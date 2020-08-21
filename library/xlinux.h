#pragma once

#include "mbasic.h"

#if TARGET_SYSTEM_LINUX
   #include "color.h"

	#include <X11/Xatom.h>
	#include <X11/Xlib.h>
	#include <X11/Xutil.h>
	#include <X11/extensions/XShm.h>
	#include <X11/extensions/Xdbe.h>
   #include <X11/cursorfont.h>
	#include <codecvt>
	#include <iostream>
	#include <map>
	#include <sys/shm.h>

namespace x
{
	namespace
	{
		Display*     shared_display  = NULL;
		unsigned int reference_count = 0;

		std::map<std::string, Atom> atoms;
	} // namespace
	inline Display* open_display()
	{
		if (reference_count == 0)
		{
			shared_display = XOpenDisplay(getenv("DISPLAY"));
			if (!XShmQueryExtension(shared_display)) throw std::runtime_error("can't load Shm");

			if (!shared_display)
			{
				std::cerr << "Failed to open X11 display; make sure the DISPLAY environment variable is set correctly"
				          << std::endl;
				std::abort();
			}
		}

		++reference_count;
		return shared_display;
	}
	inline void close_display()
	{
		reference_count--;
		if (reference_count == 0) XCloseDisplay(shared_display);
	}
	inline Atom& atom(const std::string& name, bool onlyIfExists = false)
	{
		auto it = atoms.find(name);

		if (it != atoms.end()) return it->second;

		Display* display = open_display();
		Atom     atom    = XInternAtom(display, name.c_str(), onlyIfExists ? True : False);
		close_display();

		atoms[name] = atom;
		return atoms[name];
	}

	struct _shmimage
	{
		XShmSegmentInfo shminfo;
		XImage*         ximage = 0;
		_color*         data   = 0;

		_shmimage() { shminfo.shmaddr = (char*)-1; }
		~_shmimage() { free(); }
		void free()
		{
			auto d = open_display();
			if (ximage)
			{
				XShmDetach(d, &shminfo);
				XDestroyImage(ximage);
				ximage = nullptr;
			}

			if (shminfo.shmaddr != (char*)-1)
			{
				shmdt(shminfo.shmaddr);
				shminfo.shmaddr = (char*)-1;
			}
			close_display();
		}
		bool create(int width, int height)
		{
			auto d        = open_display();
			shminfo.shmid = shmget(IPC_PRIVATE, width * height * 4, IPC_CREAT | 0600);
			if (shminfo.shmid == -1) return false;

			shminfo.shmaddr = (char*)shmat(shminfo.shmid, 0, 0);
			if (shminfo.shmaddr == (char*)-1) return false;

			data             = (_color*)shminfo.shmaddr;
			shminfo.readOnly = false;

			shmctl(shminfo.shmid, IPC_RMID, 0);

			ximage = XShmCreateImage(d, XDefaultVisual(d, XDefaultScreen(d)), DefaultDepth(d, XDefaultScreen(d)),
			                         ZPixmap, 0, &shminfo, 0, 0);
			if (!ximage)
			{
				free();
				return false;
			}

			ximage->data   = (char*)data;
			ximage->width  = width;
			ximage->height = height;

			XShmAttach(d, &shminfo);
			XSync(d, false);
			close_display();
			return true;
		}
		void target(Drawable dr)
		{
			auto d = x::open_display();
			XShmGetImage(d, dr, ximage, 0, 0, AllPlanes);
			x::close_display();
		}
	};
	struct _shmpixmap
	{
		XShmSegmentInfo shminfo;
		Pixmap          pix  = 0;
		_color*         data = 0;

		_shmpixmap() { shminfo.shmaddr = (char*)-1; }
		~_shmpixmap() { free(); }
		void free()
		{
			if (!pix) return;
			auto d = open_display();

			XShmDetach(d, &shminfo);
			shmdt(shminfo.shmaddr);
			shminfo.shmaddr = 0;
			XFreePixmap(d, pix);
			pix = 0;

			close_display();
		}
		void create(int width, int height)
		{
			free();
			auto d        = open_display();
			shminfo.shmid = shmget(IPC_PRIVATE, width * height * 4, IPC_CREAT | 0777);
			if (shminfo.shmid == -1) throw std::runtime_error("can't create shm pixmap");
			shminfo.readOnly = false;
			shminfo.shmaddr  = (char*)shmat(shminfo.shmid, 0, 0);
			XShmAttach(d, &shminfo);
			XSync(d, false);
			shmctl(shminfo.shmid, IPC_RMID, 0);

			if (!shminfo.shmaddr) throw std::runtime_error("can't create shm pixmap");

			data = (_color*)shminfo.shmaddr;

			pix = XShmCreatePixmap(d, XRootWindow(d, XDefaultScreen(d)), shminfo.shmaddr, &shminfo, width, height,
			                       DefaultDepth(d, XDefaultScreen(d)));
			if (!pix) throw std::runtime_error("can't create shm pixmap");

			close_display();
		}
	};

	inline std::string to_utf8(std::wstring_view src)
	{
		std::wstring_convert<std::codecvt_utf8<wchar_t>> conv;
		return conv.to_bytes(src.data());
	}

	inline std::wstring from_utf8(std::string src)
	{
		std::wstring_convert<std::codecvt_utf8<wchar_t>> conv;
		return conv.from_bytes(src);
	}

	inline bool load_font(int h, XFontSet& fontSet, XFontStruct**& fonts, int32& count, bool free = true)
	{
		auto              d = x::open_display();
		std::stringstream ss;
		if (h < 12)
			ss << "-*-*-*-r-*-*-" << h << "-*-*-*-*-*-*-*";
		else
			ss << "-*-*sans*-*-r-*-*-" << h << "-*-*-*-*-*-*-*";
		char** missingList;
		int    missingCount;
		char*  defString;
		auto   buff = fontSet;
		fontSet     = XCreateFontSet(d, ss.str().c_str(), &missingList, &missingCount, &defString);
		if (fontSet)
		{
			XFreeStringList(missingList);
			if (buff && free) XFreeFontSet(d, buff);
			char** fontNames;
			count = XFontsOfFontSet(fontSet, &fonts, &fontNames);
			x::close_display();
			return true;
		}
		fontSet = buff;
		x::close_display();
		return false;
	}

} // namespace x

#endif
