#include "mbasic.h"

namespace dialog
{
	void message(_wstring_view text, _wstring_view title = L"");
	bool y_n(_wstring_view text, _wstring_view title = L"");
	//_string text(_string text, _string title = "");
	
	_path open_file(_wstring_view filter = L"*.*", _wstring_view def_name = L"");
	_path save_file(_wstring_view filter = L"*.*", _wstring_view def_name = L"");

	void panic(_wstring_view text);
}
