#include "basic.h"

#define NOMINMAX
#include <windows.h>
#include <fstream>

namespace
{

	inline u64 mask1(uchar b) noexcept // b единиц
	{ // *
		return (b >= 64) ? 0xffffffffffffffff : ((1ULL << b) - 1);
	}

}

i64 position1_64(u64 a)
{ // **
	if (a >> 32)
	{
		if (a >> 48)
		{
			if (i64 k = position1_8[a >> 56]) return k + 56;
			return position1_8[(a >> 48) & 255] + 48;
		}
		if (i64 k = position1_8[(a >> 40) & 255]) return k + 40;
		return position1_8[(a >> 32) & 255] + 32;
	}
	if (a & 0xffff0000ULL)
	{
		if (i64 k = position1_8[(a >> 24) & 255]) return k + 24;
		return position1_8[(a >> 16) & 255] + 16;
	}
	if (i64 k = position1_8[(a >> 8) & 255]) return k + 8;
	return position1_8[a & 255];
}

void set_clipboard_text(std::wstring_view text)
{
	if (OpenClipboard(0))//открываем буфер обмена
	{
		HGLOBAL hgBuffer;
		char* chBuffer;
		EmptyClipboard(); //очищаем буфер
		size_t ll = text.size() * 2 + 2;
		hgBuffer = GlobalAlloc(GMEM_DDESHARE, ll);//выделяем память
		if (!hgBuffer) goto end;
		chBuffer = (char*)GlobalLock(hgBuffer); //блокируем память
		if (!chBuffer) goto end;
		memcpy(chBuffer, text.data(), ll);
		GlobalUnlock(hgBuffer);//разблокируем память
		SetClipboardData(CF_UNICODETEXT, hgBuffer);//помещаем текст в буфер обмена
	end:
		CloseClipboard(); //закрываем буфер обмена
	}
}

void set_clipboard_text(astr text)
{
	if (OpenClipboard(0))//открываем буфер обмена
	{
		HGLOBAL hgBuffer;
		char* chBuffer;
		EmptyClipboard(); //очищаем буфер
		size_t ll = strlen(text) + 1;
		hgBuffer = GlobalAlloc(GMEM_DDESHARE, ll);//выделяем память
		if (!hgBuffer) goto end;
		chBuffer = (char*)GlobalLock(hgBuffer); //блокируем память
		if (!chBuffer) goto end;
		strcpy_s(chBuffer, ll, LPCSTR(text));
		GlobalUnlock(hgBuffer);//разблокируем память
		SetClipboardData(CF_TEXT, hgBuffer);//помещаем текст в буфер обмена
	end:
		CloseClipboard(); //закрываем буфер обмена
	}
}

bool save_file(std::wstring_view fn, const char* data, i64 n)
{
	std::ofstream f(fn.data(), std::ofstream::binary);
	if (!f) return false;
	f.write((char*)data, n);
	return f.good();
}

bool load_file(std::wstring_view fn, char** data, i64* n, i64 spare_bytes)
{
	*data = nullptr;
	*n    = 0;
	std::ifstream f(fn.data(), std::ofstream::binary);
	if (!f) return false;
	f.seekg(0, f.end);
	auto size = f.tellg();
	*data = new char[i64(size) + spare_bytes];
	f.seekg(0);
	f.read(*data, size);
	if (!f.good())
	{
		delete[] * data;
		*data = nullptr;
		return false;
	}
	*n = size;
	return true;
}

bool load_file(std::wstring_view fn, std::vector<uchar>& res)
{
	res.clear();
	std::ifstream f(fn.data(), std::ofstream::binary);
	if (!f) return false;
	f.seekg(0, f.end);
	auto siz = f.tellg();
	res.resize(siz);
	f.seekg(0);
	f.read((char*)res.data(), siz);
	if (!f.good())
	{
		res.clear();
		return false;
	}
	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool _stack::operator==(const _stack& a) const
{
	if (size != a.size) return false;
	return (memcmp(data, a.data, size) == 0);
}

_stack::_stack(i64 r)
{
	data     = (r) ? new char[r] : 0;
	capacity = r;
	size     = 0;
	adata    = 0;
}

_stack::_stack(const _stack& a) : capacity(a.size), size(a.size), adata(a.adata)
{
	data = new char[capacity];
	memcpy(data, a.data, size);
}

_stack::_stack(_stack&& a) : data(a.data), capacity(a.capacity), size(a.size), adata(a.adata)
{
	a.data = nullptr;
	a.capacity = a.size = a.adata = 0;
}

_stack::_stack(void* data2, int vdata)
{
	adata    = 0;
	size     = vdata;
	capacity = vdata;
	data     = new char[capacity];
	memcpy(data, data2, size);
}

void _stack::erase(i64 n, i64 k)
{
	if ((n < 0) || (n >= size)) return;
	if ((k <= 0) || (n + k > size)) return;
	size -= k;
	adata = 0;
	if (n == size) return;
	memmove(&data[n], &data[n + k], size - n);
}

bool _stack::save_to_file(std::wstring_view fn)
{ 
	return save_file(fn, data, size);
}

bool _stack::load_from_file(std::wstring_view fn)
{
	if (data) delete[] data;
	data     = 0;
	capacity = 0;
	size     = 0;
	adata    = 0;
	i64 ll   = 0;
	if (!load_file(fn, &data, &ll)) return false;
	size = capacity = ll;
	return true;
}

_stack& _stack::operator<<(const _stack& a)
{
	*this << a.size;
	push_data(a.data, a.size);
	return *this;
}

_stack& _stack::operator<<(const _bit_vector& p)
{ // *
	*this << p.data << p.bit;
	return *this;
}

_stack& _stack::operator<<(const std::wstring& a)
{
	uint l = (uint)a.size();
	*this << l;
	push_data(a.data(), l * 2ULL);
	return *this;
}

void _stack::push_data(const void* data2, i64 vdata)
{
	if (size + vdata > capacity) set_capacity((size + vdata) * 2);
	memcpy(data + size, data2, vdata);
	size += vdata;
}

void _stack::push_fill(int vdata, char c)
{
	if (size + vdata > capacity) set_capacity((size + vdata) * 2);
	memset(data + size, c, vdata);
	size += vdata;
}

_stack& _stack::operator>>(_stack& a)
{
	u64 v = 0;
	*this >> v;
	a.size  = 0;
	a.adata = 0;
	if (v == 0) return *this;
	a.set_capacity(v);
	pop_data(a.data, v);
	a.size = v;
	return *this;
}

_stack& _stack::operator>>(_bit_vector& p)
{ // *
	*this >> p.data >> p.bit;
	p.bit_read = 0;
	return *this;
}

_stack& _stack::operator>>(std::wstring& s)
{
	uint v = 0;
	*this >> v;
	s.resize(v);
	if (v > 0) pop_data(&s[0], v * 2ULL);
	return *this;
}

void _stack::pop_data(void* data2, i64 vdata)
{
	if (adata + vdata > size) return;
	memcpy(data2, data + adata, vdata);
	adata += vdata;
}

void _stack::revert(i64 bytes)
{
	if (adata < bytes)
		adata = 0;
	else
		adata -= bytes;
}
void _stack::skip(i64 bytes)
{
	if (adata + bytes > size)
		adata = size;
	else
		adata += bytes;
}

void _stack::set_capacity(i64 rdata)
{
	if (rdata <= capacity) return;
	capacity    = rdata;
	char* data2 = new char[capacity];
	if (size) memcpy(data2, data, size);
	delete[] data;
	data = data2;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

std::wstring string_to_wstring(std::string_view s)
{
		static const wchar_t conv[256] = {
		L'\x0', L'\x1', L'\x2', L'\x3', L'\x4', L'\x5', L'\x6', L'\x7', L'\x8', L'\x9', L'\xa', L'\xb', L'\xc', L'\xd',
		L'\xe', L'\xf', L'\x10', L'\x11', L'\x12', L'\x13', L'\x14', L'\x15', L'\x16', L'\x17', L'\x18', L'\x19',
		L'\x1a', L'\x1b', L'\x1c', L'\x1d', L'\x1e', L'\x1f', L' ', L'!', L'\"', L'#', L'$', L'%', L'&', L'\'', L'(',
		L')', L'*', L'+', L',', L'-', L'.', L'/', L'0', L'1', L'2', L'3', L'4', L'5', L'6', L'7', L'8', L'9', L':',
		L';', L'<', L'=', L'>', L'?', L'@', L'A', L'B', L'C', L'D', L'E', L'F', L'G', L'H', L'I', L'J', L'K', L'L',
		L'M', L'N', L'O', L'P', L'Q', L'R', L'S', L'T', L'U', L'V', L'W', L'X', L'Y', L'Z', L'[', L'\\', L']', L'^',
		L'_', L'`', L'a', L'b', L'c', L'd', L'e', L'f', L'g', L'h', L'i', L'j', L'k', L'l', L'm', L'n', L'o', L'p',
		L'q', L'r', L's', L't', L'u', L'v', L'w', L'x', L'y', L'z', L'{', L'|', L'}', L'~', L' ', L' ', L' ', L' ',
		L' ', L' ', L' ', L' ', L' ', L' ', L' ', L' ', L' ', L' ', L' ', L' ', L' ', L' ', L' ', L' ', L' ', L' ',
		L' ', L' ', L' ', L' ', L' ', L' ', L' ', L' ', L' ', L' ', L' ', L' ', L' ', L' ', L' ', L' ', L' ', L' ',
		L' ', L'Ё', L' ', L' ', L' ', L' ', L' ', L' ', L' ', L' ', L' ', L' ', L' ', L' ', L' ', L' ', L' ', L'ё',
		L' ', L' ', L' ', L' ', L' ', L' ', L' ', L'А', L'Б', L'В', L'Г', L'Д', L'Е', L'Ж', L'З', L'И', L'Й', L'К',
		L'Л', L'М', L'Н', L'О', L'П', L'Р', L'С', L'Т', L'У', L'Ф', L'Х', L'Ц', L'Ч', L'Ш', L'Щ', L'Ъ', L'Ы', L'Ь',
		L'Э', L'Ю', L'Я', L'а', L'б', L'в', L'г', L'д', L'е', L'ж', L'з', L'и', L'й', L'к', L'л', L'м', L'н', L'о',
		L'п', L'р', L'с', L'т', L'у', L'ф', L'х', L'ц', L'ч', L'ш', L'щ', L'ъ', L'ы', L'ь', L'э', L'ю', L'я'};
		std::wstring res(s.size(), L' ');
	for (int i = 0; i < s.size(); i++) res[i] = conv[(uchar)s[i]];
	return res;
}

std::wstring substr(std::wstring_view s, i64 n, i64 k)
{
	if (n >= (i64)s.size()) return L"";
	if (n < 0)
	{
		k += n;
		n = 0;
	}
	if (k < 0) k = 0;
	if (n + k > (i64)s.size()) k = s.size() - n;
	return std::wstring(s.data() + n, k);
}

std::wstring double_to_wstring(double a, int z)
{
	static const double delta[16] = {5E-1, 5E-2,  5E-3,  5E-4,  5E-5,  5E-6,  5E-7,  5E-8,
	                                 5E-9, 5E-10, 5E-11, 5E-12, 5E-13, 5E-14, 5E-15, 5E-16};
	std::wstring        s;
	if (a < 0)
	{
		a = -a;
		s = L"-";
	}
	if (z < 16) a += delta[z];
	i64 b = static_cast<i64>(a);
	s += std::to_wstring(b);
	if (z < 1) return s;
	int l = (int)s.size();
	s.resize(1LL + l + z);
	s[l] = L'.';
	for (int i = 1; i <= z; i++)
	{
		a -= b;
		a *= 10;
		b               = static_cast<i64>(a);
		s[(i64)l + i] = L'0' + (short)b;
	}
	return s;
}

std::string double_to_string(double a, int z)
{
	static const double delta[16] = { 5E-1, 5E-2, 5E-3, 5E-4, 5E-5, 5E-6, 5E-7, 5E-8, 5E-9, 5E-10, 5E-11, 5E-12, 5E-13,
		5E-14, 5E-15, 5E-16 };
	std::string s;
	if (a < 0)
	{
		a = -a;
		s = "-";
	}
	if (z < 16)	a += delta[z];
	i64 b = static_cast<i64>(a);
	s += std::to_string(b);
	if (z < 1) return s;
	int l = (int)s.size();
	s.resize(1i64 + l + z);
	s[l] = '.';
	for (int i = 1; i <= z; i++)
	{
		a -= b;
		a *= 10;
		b = static_cast<i64>(a);
		s[(i64)l + i] = '0' + (char)b;
	}
	return s;
}

wstr uint64_to_wstr_hex(u64 a)
{
	static const wchar_t zz[] = L"0123456789abcdef";
	static wchar_t       s[]  = L"1234567890123456";
	int                  k0   = 0;
	for (int i = 15; i >= 0; i--)
	{
		int b = a & 15;
		s[i]  = zz[b];
		a >>= 4;
		if (b == 0)
			k0++;
		else
			k0 = 0;
	}
	if (k0 == 16) k0 = 15;
	return (s + k0);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void memset32(uint* destination, uint value, i64 size)
{
	for (i64 i = 0; i < size; i++) destination[i] = value;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void _rnd::init(u64 b)
{
	static constexpr u64 dd[521] = {
		0xBE3B0D3E773B3EAC, 0xE35381E6D8B09960, 0x8BC79403A0FFBC4A, 0x590FF05C9DB44258, 0x8E149C5DF9924DA6,
		0xFE3A12707102CD37, 0xB16E7870697C38E5, 0xC66633332C7BF81C, 0x8FD23C3ABD37E032,	0xEE81791910218697,
		0x9894BA4F38A1ABD7, 0x4AE53B309D9ED971, 0xBD9AA006C3259BCF, 0x4FB3F6FB61B2165B, 0xBA0150484AAF6B30,
		0x320CDAD20D4D68A2, 0x0F91791EBEA94652, 0x7C4B5BB96C88F9F7,	0xDF501EC332B5CC0A, 0x21096D3EDB209BBF,
		0x856C89DF5D33E0AA, 0x4D94C3674BC0EA10, 0xC43379E3DCB0590E, 0x52F4A802B8D2EE6D, 0xE039553ADE0F0395,
		0xB70E86A7FB509989, 0x0AA25A448275880C,	0x2A3AA2D3426CFEDB, 0x8E4B6D86ABAD29F4, 0x6E967040D0491908,
		0x12C9BA00EB6F8308, 0x2813C0C70EA13B7D, 0xA596095BC048E725, 0x92EE88AE7E56ECAD, 0x66B27518925DE06C,
		0x8AA54284FC071B04,	0xB2AD8EC5407F303B, 0xB346EBF8144E59B2, 0xDC888B158465BF6C, 0x468FF67B6FA2AFC7,
		0xD7F3316233E42F2B, 0x1DCD122C458D368B, 0xF66DB27D47E41F4E, 0x0C71537D9B292342, 0xC437B48E6778EDE3,
		0xB964D1F611C035B0, 0xD0258318833D0F80, 0xE2A73E3C1BF126B7, 0xD6108AD9E924AA37, 0x9FE5148A30A4898C,
		0xAA03E7D0314E5900, 0x2C1CF04BD465C8E7, 0x31F3DB4AAF9E70BA, 0x661255A1FD9AA770,	0x27AEC46FD9007F0C,
		0xB87950FAAF616331, 0x4A03BD9F5261D536, 0xEA4A9A7E0E95E3E0, 0xEF7356CD779A0171, 0x2C3B2160BFE25544,
		0x15EAA83784BFB4BF, 0x2D08D2DC1FB0F712, 0x97EF387F6DFEBD68,	0xF957CCA34F282B31, 0xFDF9230EDC5795E3,
		0x4A48CBA6450D88CF, 0x34A400A221B12F31, 0xB7203374B6D31CF6, 0xD48559C70C5480DB, 0x433989EC1ACB2E1A,
		0x985F0B5BB4CE9A22, 0xFA6951BFF8933C65,	0xF468B7F717D753E6, 0xAE87AD0D3E3544E4, 0xC2019ABA992F6F6F,
		0xE8C7257650E84E5A, 0x703F2C4D6543969B, 0x085CD910E5D21B37, 0xA01F118E35AADB6F, 0x2838D40BEBEFA5B3,
		0xD4ED7629F3025E5B,	0x49CCCB418B313500, 0x95ADA9FEC6881A09, 0x4FBF3E47D21C0568, 0xDF2E9F3703C41255,
		0x0CA57D8C33A5063A, 0xF608896A4041B000, 0x57FE5C64514EC26E, 0xD0117F129ED5C0FE, 0xB004F0854B7BF41B,
		0x351A0D7976ED987A, 0xEBD0C32ACD303C7A, 0x008D9190FD4C125C, 0xC6EC2E0F5B7586EC, 0x12559C917AFDE43C,
		0x90740B886CBF48B1, 0x0D2E8AE7D7E8400E, 0xE86FFF28CB209F4A, 0x00A22EDAC32B8925,	0x9575FD6F5DA4642C,
		0xD99CB494411182D8, 0x772113CE6506F081, 0xD13BB160C42EB3C6, 0x0562F15320DFDD93, 0xDD98CEA0B604CC12,
		0xC27E27084478430B, 0x0463B33896EA4682, 0xAE9BD158FE65F5A5,	0x8F55025EFB1E987A, 0x34B3680678EAD7E5,
		0xB97856AD5F08F552, 0x6EF98FDBD52CA6B5, 0xFE89F5F5B7E518FA, 0x6E1A401BA8E61A04, 0x7A412629C3C30CD6,
		0xF42DD5F352334257, 0xD9030CA11327259C,	0x7BC6099C639B2D19, 0x387E9A7494D795FF, 0xF816DF9A6FDCF37B,
		0x10E78998CABBDCF2, 0x7083A2317EBF33A8, 0x3E83A2E77EBFC6A2, 0x9EEBEA37122C4988, 0xF1F0FC25E38FFC1E,
		0x0E928704358110D1,	0x12EE4D75E75817DC, 0x6E396A66495D9315, 0xA47A72A536932AA3, 0xA1786A010B7F3CF6,
		0x8A79D6B54BBCDA21, 0x88F222906CC14C75, 0x158E9FDD50A3B238, 0x9BE445E35E1424A8, 0x5B447A427F80D798,
		0xB8F3F1811F363491, 0xF4E230AFBE8F1F7A, 0x5BE70329D1CD31D0, 0x9628D0DCBB4215B0, 0xBE0EE148397C5024,
		0x20E5432583FE22CB, 0xEF81F9DAE03BFB23, 0x837FFD12B42D14D5, 0x9201E2562EB85125,	0xEA122CDBB1713F03,
		0x5D59CA9EEE6B3238, 0xC1C8240BA0971868, 0x89D31FABE9E7C150, 0x4FCFBBBD4566828A, 0x6FE090E7CB10343C,
		0x3CFA156788B5DBFA, 0x8A73A4EF93883B01, 0x0BB193F43DA75B4B,	0x2E8B2E24586FC3F2, 0x9C396E26E86B7B57,
		0xBB1692046E4FCBBE, 0x593FFD25BF8071F6, 0x4CFCECDE10D0EC3D, 0x816DCC2E95EEBA5D, 0xD2B5E89C9022DF3F,
		0x6BE53097FF14E1CD, 0x7FD9BBF9612980BF,	0xDD83C52A171A835C, 0x46BDE7F4A926529D, 0xC46959E05516C792,
		0xB90A9753F1E2E70A, 0x28C6D013C31FC945, 0xCCE9EB13709BE31A, 0xE5FD9FD65BA9BFEB, 0x2FE442C0877550D5,
		0xCE5A46257230BE82,	0x38DCAE0AEE8E7368, 0xFB6BE4965591980C, 0x6AF69E9DD4BDF591, 0x08B400DE315B5685,
		0x995A69959BA5A29E, 0xEB13755DB7F33901, 0xDA161EFA2622ED1A, 0x0C859BFB9D9E3F05, 0xF09D410DB8188B51,
		0xAF9FF6921FD682E8, 0x3A77C6FEE3029087, 0xD7B4A703BDC3A4F2, 0x154E6186CB6B5FC5, 0x9C67A16BA3D74F52,
		0xD9E6CBAD060798D9, 0xAE31A8F5D3395CA0, 0x461B464B0924AD3C, 0x123D415CB6D06AD5,	0x91A8F5C48F7F76BE,
		0x1797D724ABF19C2A, 0xAC19A7E06BB71AEB, 0xEDE32CE7B75A6F68, 0x23175989EE2E3AF4, 0x928DAD299F6050B5,
		0x14B866AB3477FE27, 0x27178C432F1F6AF0, 0x76D6B780B2C454B6,	0x5518EBC193E2E1A0, 0x0B8EAAF902C98987,
		0x8A16B5CD70B727DB, 0x0DF19F64872BD24F, 0xE45B4924C0F8CA9B, 0x501438344F2C94D0, 0x3BFC3B20DBCB57B3,
		0x6E20F770B48BBCEB, 0xBBDFE1904704EC6F,	0x77BEED2445C22CE5, 0x2CE62C00237A7FAD, 0x46C324DC62FA0649,
		0x0166267F19FB6ED0, 0xF14C318A3DA494F2, 0x5DD2FA7C41D9F933, 0xBB9C431260901B44, 0x92B70D9E82DF8E40,
		0x7FC28E1438B2B2C6,	0x3537814ABB88F85C, 0xF32D6E825089C2A7, 0xE5B4C32B689547F9, 0x43931CF6F4C3E808,
		0x7B48EB5C6B3E0DC6, 0x275B57F7D1D99A31, 0x574F65C42050BC0F, 0x9D04974DB06AEEB5, 0x38E13D2AB045ABFE,
		0x3DB0EC8B74517D56, 0x7FC968C6B3684628, 0xFDDC99A3BA47F8A3, 0x30F0485D05C5769A, 0xB5C8B6DBF116848A,
		0xFB2DFD684BE621E3, 0x2DE5277C6BD52871, 0xE27A5E65DB7EE37C, 0x3B79CCA1240D89A1,	0xCD7E98459EB28B4F,
		0x3B3EDA905B4691C3, 0x1CA885F6A0A828D3, 0x7024CC3BC1BACF81, 0x075A790ADF32EED8, 0x6104B23478210184,
		0xD14AE5EFCBB2F493, 0xE22511CC445AA734, 0x99D499106F3D9FA9,	0x3B3C0B65089D6CB1, 0x7D696C8CAD154235,
		0x0033D4DF6A580FD1, 0x530B6F19907F540D, 0xF0C558B706CCAE01, 0xDE94779D637EAEDC, 0xE47EE96D87103BEC,
		0x170DB8012090FE75, 0x491488630BCE25B2,	0x27C1CB90DF964328, 0xEA003D26B37C3698, 0xEE2F52C4DB8600F9,
		0xA076721148B8B645, 0xE30D4DB2A693E568, 0x9E73EAF2954E55B1, 0xFAB2FD7A926E63B5, 0xE06C9A3B0D2955CF,
		0x51E5B947E33A27B3,	0xA82B5507C100D4AE, 0xAE8337F359764B0F, 0x9E6C2DABD51F27D2, 0x95815F71F4409A4D,
		0x6D192A9F2451080C, 0x133CEEEAAF9B0F33, 0x338FE877B5C38B34, 0xDA8B16169365EF6C, 0xAB0A530F1939C124,
		0xBDB9924E953027E9, 0x277F4F52AC40299C, 0xDB8945DAEED441F6, 0xC32BEBBAE69962AC, 0x69B5802C1930E5EF,
		0x0F7BB0E456BDDEF4, 0x86B824F049CB6F7F, 0x4E514A21C76BC813, 0x16E3C7D57BB146E4,	0x1DA2DA9954819EA8,
		0x8A5A9B4CFF0410A2, 0xAB7818AA86FF4DAA, 0xCAD3642B38C832D0, 0xF092D8EDFB7F307D, 0xA4A9B89846E8E85B,
		0x4CBE1B8D999EEEF8, 0x6FB32018822C8E69, 0x3C0DD878E74ABBB1,	0x93B29BCFDA7DD495, 0xE16A2467DCD33DBD,
		0x1766444606E1BA0D, 0xAEE242564D5608C2, 0xB7E2E172E6B655C9, 0x25757886047CDCB3, 0x3716DED6BB93C8BD,
		0x5F454EDB33A1A03B, 0x86755B00DE79516D,	0xAE36B1E0A156AE2D, 0x3607AD9AD8F5FEE5, 0x6DC501432CED638B,
		0xD2C07C5DEDCE392E, 0xC34235EB2A322BBE, 0x16D19A64AD214DDE, 0xEFA579DFF9EBCF35, 0x653BEE8E613CA6E5,
		0x032804307FD31964,	0xC37A0BE25370DF0F, 0xE4DC043B8EBA531D, 0x4930AE552E0EA7A5, 0xB624259F1B9BF781,
		0xC130116F36746D94, 0x63C0030AAC4A9EE8, 0xD858CE42B0200706, 0xF0E503B455E4A48B, 0x79E62EF9153709FE,
		0x579DA166E9B29BE5, 0x027AC5F1F0428C22, 0x0DDB22399EC55EFE, 0xAF888E9034ECEF97, 0x9C8A546AA20A2A6A,
		0x2ADA6EB71F9C8E6C, 0x633B305AD1A6A796, 0x39A0C6A87AAEC2B4, 0x16558DC3C6757637,	0xCC65331C1636F93E,
		0xF7A13CE0FD59E692, 0x5736A48F6DBC262E, 0x2DFB81B2DA00647E, 0xFAF714E33E279405, 0x48B73AA7C54EFB43,
		0x3083A81752151C96, 0xD30D64A663C3554E, 0x1CF5112ECB18CA50,	0xA9DB5FFC7F85F354, 0xC4698CCA94240B26,
		0xC1F780E2395F1B49, 0x25AAED4398237326, 0xC98CA441EA9BAEC0, 0x9D3C5445BF3A939F, 0x2A14E6CFC915CD7D,
		0xC4FB14667DC15496, 0x082B4F485E962212,	0xC34573A5B0E48C1C, 0xC5B18CA5CF25F137, 0x33F9601190FEFCD6,
		0xAFA050B558008D19, 0x833FDB65F35F12F0, 0x1D3416445188D863, 0x53BD94BBA9F020C4, 0xE9F20C8CBF2A7A6E,
		0x39843A48CA4D38B5,	0x2060EE75E8A5BD2E, 0x2441FE9B6999704C, 0x86CD64AE8D2FD1AF, 0xBEB3F92FA0C9F49E,
		0x656A8DFD945985D8, 0xDACDA7119E6BF49E, 0x76E9CA729F020F6A, 0xFE23E4748F64AFCF, 0x92132EF662086BA7,
		0x9415F47185B0C166, 0x5F21A893ADE6A3A7, 0x534ADDBCBEDFCDA0, 0xC341DAC8632A9F85, 0x20B6C5EDB1DEF68C,
		0x8250CDE052028B7B, 0x08F369D4A8F803DE, 0xBC2142D0C3C743F8, 0x2CFDEA872067080D,	0x0FAD675AE1FD0C8A,
		0x15D65334D6F0A587, 0x89E12CC39697B8D1, 0x921FC295C97E331E, 0x4E1E8148BA569532, 0xE7EF4D369666351A,
		0x39A81EE6C99E3626, 0xA0E0283590980DFD, 0x858EA0132D753AA2,	0xCC61A090D74B7C77, 0x55ACFA2DD78A5F67,
		0x440E2C2ED6FEAF94, 0x4906ECF0879F0D8B, 0x10ECD6BF9EE0BC22, 0x1B378659D4C5FD63, 0x68513EF5CA8D7E5E,
		0x2062E742BA6A18E9, 0xE36EF3654A64A473,	0xDDF50DE0C9B67E1D, 0x5BEB1195083475FD, 0x37D14DDB8778AF62,
		0x9B9B5F4E66476984, 0x254B074DD82732AC, 0x3EDB5AFAC84194D4, 0x5EB001A9313E93BF, 0x12E18EE0432ABCB0,
		0xE347924628348FE1,	0xA71D2FC25664362E, 0x496EAE592477C167, 0x7FD1C7E0B77D797A, 0x90998EE7FABCEAAE,
		0xEF68C016830D2ECE, 0x55B5CE1C499C7B62, 0x9B00BBF4C202499E, 0x226144AB1A756D44, 0xBBDDD5CFC3A6562F,
		0x83EAE3348CD26E58, 0x9A77372165AC398E, 0xCBA5D252F55A29BF, 0x501EF60C8ADE5846, 0xE015197AF5DCE94B,
		0xEE68E105D43A8932, 0x793C94C83DB78F7F, 0xBD679E31600DAA1F, 0x5434537DFFF10661,	0x855FA9A02E6A9B15,
		0x515C82CA78EFADBD, 0xA6BF9335ED079A53, 0x450B4663BD948F18, 0xB2140638E4DEBB81, 0x554F77ACB46732C2,
		0xBA866A277AEB77BD, 0x52600A7782B9B127, 0xB5A5FAAB7B2207C0,	0x7034F7756975B5B1, 0x783DF895505271D5,
		0x1BBA55D1822EE786, 0xAA608CD2DD89A289, 0xACD57FA21E7F14FF, 0x2CEB689D1256F261, 0xE9903CA6894514CF,
		0x0B80C3E480D12297, 0x6A46F987D8F3B9F2,	0x75DA684866A858B3, 0xD8FF70008A0ED94D, 0x60A0375BFBD44E2A,
		0xB924B9A602CC9CFA, 0xF307A62CA719644F, 0x71240B201C37B430, 0x01BA7146C32CE9D6, 0x0E293651CAA23577,
		0x50A37CB33D512C35,	0xFAD0D0A3E7C434AB, 0x1A6EA03EFCCF78F7, 0xB74D595192AC9530, 0x5ABCC4A696245FC5,
		0x52C0BD7FD1A9A1EB, 0xED0DC9C54663E6C3, 0x6317D755E86A3397, 0xCE780B2176C74FBE, 0x261C9D368F0F4BFD,
		0xF365AEB953164C00, 0xCBC0E6CC030C9C78, 0x38B33E53C2484532, 0x8711C041745735E5, 0x5D827AAE44ADC9A4,
		0x2AB23D3F4825A8CB, 0xDF4FEEFAC31DE5CF, 0xD767264AB40E1321, 0xCA578C2BB4DCFAA3,	0x7866D6D0DDB8898E,
		0xFC77F04DE82F8A4B, 0xE0C1E6DF3991409C, 0xF0C8FD055543DE78, 0xB08D2DAE0E7F1F07, 0x6E692DF91C603882,
		0x5FF289AF7673D306, 0x99156217FB9964DD, 0xB2F5350B9A2269F9,	0x5ED1C5D27656C3CE, 0x17E3C7B7A656890C,
		0x158D302A7B61895E, 0xB8AF0FB035FB6F19, 0x2CB8E94BF3543A13, 0x64A234D428500D2A, 0x56A1BA740679301B,
		0x9636F1413FEFA7AF, 0x844AD984A4BD77EB,	0xDF40EB27CDB6A0A8, 0x65AAB5F32B8734B1, 0xD5BB78A17A840C20,
		0x813DF90FCBC91D7D, 0x3A554ADD60101567, 0xAFEB0E5F3498FE3D, 0xD7AB5B43C810CB62, 0x982E164CD171AEEF,
		0xDBC68133004907BA,	0x1DD508A2188042A7, 0x827257DBC2EFCFBA, 0x038B779D18E02D28, 0x5085985DD6F65396,
		0x8C114C9D44C8AF92, 0xD88A08B51A7CD3A5, 0xAF2396605C1D9885, 0x17EC4EE59060D4C1, 0x5CB63BCCEA47733E,
		0x2D78E3D87C4F6C9A, 0x0F2B1C550D553632, 0x2D597F81E8D51379, 0x406777B600F95F53, 0xEFC7EB67A74229FE,
		0xBD42168BCD8D0341, 0xE4AA22E5A7857F3F, 0x5D19626A8E7AD972, 0x45A2E44AF74B77D2,	0xAC2A9D6C8EA5A03A,
		0xE8A22A1B17B759D0, 0x24EB623B4E358600, 0xF92A1E27E43ED25C, 0x3744F54A20A45FD2, 0x6AEEA79AF6CBB921,
		0xFDC007333051DE9E, 0xF69911082E6E3847, 0x0E7D8C8830303BF6,	0x214A8E420EA3FE77, 0x30E9CAF4BD64CCB3,
		0xBD1D91BACDC60661, 0x3D966F71E22BE083, 0xC06AC44A5E02E536, 0xDADBCB29A6F8884F, 0x3F64140220163B5E,
		0x7044CAA7BE05B133 };
	i = 520;
	b *= 489133282872437279;
	for (i64 j = 0; j < 521; j++) d[j] = dd[j] + b;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

i64 _bit_vector::size() const
{ // *
	return data.size() * 64 + bit - 64;
}

bool _bit_vector::empty() const
{ // *
	return size() == 0;
}

void _bit_vector::clear()
{ // *
	data.clear();
	bit = 64;
	bit_read = 0;
}

bool _bit_vector::operator==(const _bit_vector& b) const
{ // *
	return (bit == b.bit) && (data == b.data);
}

void _bit_vector::resize(i64 v)
{ // *
	if (v < 0) v = 0;
	data.resize((v + 63) >> 6);
	bit = v & 63;
	if (bit == 0) bit = 64;
	if (v > 0) data.back() &= mask1(bit);
}

u64 _bit_vector::pop()
{ // *
	i64 r = bit_read >> 6;
	uchar bi = bit_read & 63;
	bit_read++;
	return (data[r] >> bi) & 1;
}

u64 _bit_vector::pop_safely()
{ // *
	i64 r = bit_read >> 6;
	uchar bi = bit_read & 63;
	bit_read++;
	if (r >= (i64)data.size()) return 0;
	return (data[r] >> bi) & 1;
}

u64 _bit_vector::pop(uchar n)
{ // *
	if (n == 0) return 0;
	i64 r = bit_read >> 6;
	uchar bi = bit_read & 63;
	bit_read += n;
	if (bi + n <= 64) return (data[r] >> bi) & mask1(n);
	return ((data[r] >> bi) | (data[r + 1] << (64ui8 - bi))) & mask1(n);
}

void _bit_vector::push(u64 a)
{ // *
	a &= 1;
	if (bit == 64)
	{
		data.push_back(a);
		bit = 1;
		return;
	}
	data.back() |= (a << bit++);
}

void _bit_vector::pushnod(u64 a, u64 n)
{ // *
	u64 aa = (a & 1) ? 0xffffffffffffffff : 0;
	if (n <= 64)
	{
		push(aa, n);
		return;
	}
	if (bit < 64)
	{
		u64 nn = 64 - bit;
		push(aa, nn);
		n -= nn;
	}
	for (; n >= 64; n -= 64) data.push_back(aa);
	push(aa, n);
}

void _bit_vector::push(u64 a, uchar n)
{ // *
	if (n == 0) return;
	a &= mask1(n);
	if (bit == 64)
	{
		data.push_back(a);
		bit = n;
		return;
	}
	data.back() |= (a << bit);
	bit += n;
	if (bit <= 64) return;
	bit -= 64;
	data.push_back(a >> (n - bit));
}

void _bit_vector::pushn1(u64 a)
{ // *
	push(a, position1_64(a >> 1));
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
