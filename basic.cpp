#include "basic.h"

#include <map>
#include <sstream>
#include <iomanip>

bool save_file(wstr fn, const char* data, i64 n)
{
	std::ofstream f(fn, std::ofstream::binary);
	if (!f) return false;
	f.write((char*)data, n);
	return f.good();
}

bool load_file(wstr fn, char** data, i64* n)
{
	*data = 0;
	*n    = 0;
	std::ifstream f(fn, std::ofstream::binary);
	if (!f) return false;
	f.seekg(0, f.end);
	auto siz = f.tellg();
	*data    = new char[siz];
	f.seekg(0);
	f.read(*data, siz);
	if (!f.good())
	{
		delete[] * data;
		*data = 0;
		return false;
	}
	*n = siz;
	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void _stack::push_int24(int a)
{
	a += 0x800000;
	if (size + 4 > capacity) set_capacity((size + 4) * 2);
	*((int*)(data + size)) = a;
	size += 3;
}

void _stack::pop_int24(int& a)
{
	if (adata + 3 > size) return;
	char* x = (char*)(&a);
	x[0] = data[adata++];
	x[1] = data[adata++];
	x[2] = data[adata++];
	x[3] = 0;
	a -= 0x800000;
}

bool _stack::operator==(const _stack& a) const noexcept
{
	if (size != a.size) return false;
	return (memcmp(data, a.data, size) == 0);
}

_stack::_stack(i64 r) noexcept
{
	data     = (r) ? new char[r] : 0;
	capacity = r;
	size     = 0;
	adata    = 0;
}

_stack::_stack(const _stack& a) noexcept : capacity(a.size), size(a.size), adata(a.adata)
{
	data = new char[capacity];
	memcpy(data, a.data, size);
}

_stack::_stack(_stack&& a) noexcept : data(a.data), capacity(a.capacity), size(a.size), adata(a.adata)
{
	a.data = nullptr;
	a.capacity = a.size = a.adata = 0;
}

_stack::_stack(void* data2, int vdata) noexcept
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

bool _stack::save_to_file(wstr fn)
{ 
	return save_file(fn, data, size);
}

bool _stack::load_from_file(wstr fn)
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

_stack& _stack::operator<<(const _stack& a) noexcept
{
	*this << a.size;
	push_data(a.data, a.size);
	return *this;
}

_stack& _stack::operator<<(const std::wstring& a) noexcept
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

_stack& _stack::operator>>(_stack& a) noexcept
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

_stack& _stack::operator>>(std::wstring& s) noexcept
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

void _multi_string::div2line(i64 y, i64 x)
{
	if ((y < 0) || (y >= (int)line.size())) return;
	int l = (int)line[y].size();
	if (x < 0) x = 0;
	if (x > l) x = l;
	line.insert(line.begin() + y, line[y]);
	line[y].erase(x);
	line[y + 1LL].erase(0, x);
}

void _multi_string::insert_char(i64 y, i64 x, wchar_t c)
{
	if ((y < 0) || (y >= (int)line.size())) return;
	if (x < 0) x = 0;
	int l = (int)line[y].size();
	if (x > l) x = l;
	line[y].insert(x, 1, c);
}

bool _multi_string::delete_char(i64 y, i64 x)
{
	if ((y < 0) || (y >= (int)line.size())) return false;
	if (x < 0)
	{
		if (y == 0) return false;
		line[y - 1LL] += line[y];
		line.erase(line.begin() + y);
		return true;
	}
	if (x >= (int)line[y].size())
	{
		if (y == (i64)line.size() - 1) return false;
		line[y] += line[y + 1LL];
		line.erase(line.begin() + (y + 1ULL));
		return true;
	}
	line[y].erase(x, 1);
	return true;
}

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

std::wstring double_to_string(double a, int z)
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

std::string double_to_astring(double a, int z)
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

_xy _xy::rotation(double b) const noexcept
{
	double c = cos(b);
	double s = sin(b);
	return { c * x - s * y, s * x + c * y };
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void _area::operator+=(const _area& b) noexcept
{
	if (b.empty()) return;
	if (empty()) { *this = b; return; }
	if (b.x.min < this->x.min) this->x.min = b.x.min;
	if (b.x.max > this->x.max) this->x.max = b.x.max;
	if (b.y.min < this->y.min) this->y.min = b.y.min;
	if (b.y.max > this->y.max) this->y.max = b.y.max;
}

bool _area::operator<=(const _area& b) const noexcept
{
	if (empty()) return true;
	if (b.empty()) return false;
	return ((x.min >= b.x.min) && (x.max <= b.x.max) && (y.min >= b.y.min) && (y.max <= b.y.max));
}

bool _area::inside(const _area& b) const noexcept
{
	if (b.empty()) return false;
	if (empty()) return true;
	return ((x.min > b.x.min) && (x.max < b.x.max) && (y.min > b.y.min) && (y.max < b.y.max));
}

bool _area::operator<(const _area& b) const noexcept
{
	if (b.empty()) return false;
	if (empty()) return true;
	if ((x.min == b.x.min) && (x.max == b.x.max) && (y.min == b.y.min) && (y.max == b.y.max)) return false;
	return ((x.min >= b.x.min) && (x.max <= b.x.max) && (y.min >= b.y.min) && (y.max <= b.y.max));
}

bool _area::test(_xy b)
{
	if (empty()) return false;
	return ((b.x >= this->x.min) && (b.x <= this->x.max) && (b.y >= this->y.min) && (b.y <= this->y.max));
}

double _area::radius()
{
	if (empty()) return 0.0;
	double dx = x.max - x.min;
	double dy = y.max - y.min;
	return ((dx < dy) ? dx : dy) / 2;
}

_area _area::expansion(double b) const noexcept
{
	if (empty()) return *this;
	return { {x.min - b, x.max + b}, {y.min - b, y.max + b} };
}

_area _area::scaling(double b) const noexcept
{
	if (empty()) return *this;
	double k1 = (1 + b) / 2;
	double k2 = (1 - b) / 2;
	return { {x.min * k1 + x.max * k2, x.min * k2 + x.max * k1}, {y.min * k1 + y.max * k2, y.min * k2 + y.max * k1} };
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

_trans _trans::operator*(_trans tr) const noexcept
{
	tr.offset.x = offset.x + tr.offset.x * scale;
	tr.offset.y = offset.y + tr.offset.y * scale;
	tr.scale *= scale;
	return tr;
}

_xy _trans::inverse(_xy b) const noexcept
{
	return { (b.x - offset.x) / scale, (b.y - offset.y) / scale };
}

_trans _trans::inverse() const noexcept
{
	double mm = 1.0 / scale;
	return { mm, {-offset.x * mm, -offset.y * mm} };
}

_trans _trans::operator/(_trans tr) const noexcept
{
	tr.scale = scale / tr.scale;
	tr.offset.x = offset.x - tr.offset.x * tr.scale;
	tr.offset.y = offset.y - tr.offset.y * tr.scale;
	return tr;
}

bool _trans::operator!=(const _trans& b) const noexcept
{
	return ((scale != b.scale) || (offset.x != b.offset.x) || (offset.y != b.offset.y));
}

_xy _trans::operator()(const _xy& b) const noexcept
{
	return _xy{ b.x * scale + offset.x, b.y * scale + offset.y };
}

_area _trans::inverse(const _area& b) const noexcept
{
	if (b.empty()) return b;
	_area c;
	double mm = 1.0 / scale;
	c.x.min = (b.x.min - offset.x) * mm;
	c.x.max = (b.x.max - offset.x) * mm;
	c.y.min = (b.y.min - offset.y) * mm;
	c.y.max = (b.y.max - offset.y) * mm;
	return c;
}

void _trans::MasToch(_xy b, double m)
{
	offset.x = b.x + m * (offset.x - b.x);
	offset.y = b.y + m * (offset.y - b.y);
	scale *= m;
}

void _trans::operator/=(_trans tr)
{
	scale /= tr.scale;
	offset.x -= tr.offset.x * scale;
	offset.y -= tr.offset.y * scale;
}

_area _trans::operator()(const _area& b) const noexcept
{
	//	if (b.empty()) return b;
	return { {b.x.min * scale + offset.x, b.x.max * scale + offset.x},
			 {b.y.min * scale + offset.y, b.y.max * scale + offset.y} };
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool _iarea::operator!=(_isize b) const noexcept
{
	if (b.empty() && empty()) return false;
	return (x.min != 0) || (y.min != 0) || (x.max != b.x) || (y.max != b.y);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

uint test_line(_xy p1, _xy p2, _xy b)
{
	if (p2.x < p1.x) std::swap(p1, p2);
	if ((p1.x > b.x) || (p2.x <= b.x)) return 0;
	return ((b.x - p1.x) * (p2.y - p1.y) - (b.y - p1.y) * (p2.x - p1.x) > 0);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

_bitmap temp_bmp(10, 10);

#define px(xx,yy) ((uchar*)&data[(yy) * size.x + (xx)])

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool _bitmap::resize(_isize wh)
{
	wh = wh.correct();
	if (size == wh) return false;
	size = wh;
	area = size;
	BITMAPINFO bmi = { sizeof(BITMAPINFOHEADER), (long)size.x, (long)-size.y, 1, 32, BI_RGB, 0, 0, 0, 0, 0 };
	bitmap2 = CreateDIBSection(0, &bmi, DIB_RGB_COLORS, (void**)(&data), 0, 0);
	HGDIOBJ old = (bitmap2 != 0) ? SelectObject(hdc, bitmap2) : 0;
	if (old != 0) DeleteObject(old);
	return true;
}

_bitmap::_bitmap(int rx3, int ry3) : _picture({ rx3, ry3 })
{
	font.lfHeight = 13;              // высота шрифта или символа
	font.lfWidth = 0;               // средняя ширина символов в шрифте
	font.lfEscapement = 0;               // угол, между вектором наклона и осью X устройства
	font.lfOrientation = 0;               // угол, между основной линией каждого символа и осью X устройства
	font.lfWeight = 100;             // толщина шрифта в диапазоне от 0 до 1000
	font.lfItalic = 0;               // курсивный шрифт
	font.lfUnderline = 0;               // подчеркнутый шрифт
	font.lfStrikeOut = 0;               // зачеркнутый шрифт
	font.lfCharSet = RUSSIAN_CHARSET; // набор символов
	font.lfOutPrecision = 0;               // точность вывода
	font.lfClipPrecision = 0;               // точность отсечения
	font.lfQuality = 0;               // качество вывода
	font.lfPitchAndFamily = 0;               // ширина символов и семейство шрифта
	memcpy(font.lfFaceName, L"Tahoma", 14);  // название шрифта

	BITMAPINFO bmi = { sizeof(BITMAPINFOHEADER), rx3, -ry3, 1, 32, BI_RGB, 0, 0, 0, 0, 0 };
	bitmap2 = CreateDIBSection(0, &bmi, DIB_RGB_COLORS, (void**)(&data), 0, 0);
	hdc = CreateCompatibleDC(0);
	if (hdc == 0) return;
	if (bitmap2 != 0) SelectObject(hdc, bitmap2);
	f_c = GetTextColor(hdc);
	f_cf = GetBkColor(hdc);
}

_bitmap::~_bitmap()
{
	DeleteDC(hdc);
	DeleteObject(bitmap2);
	if (hfont != 0) DeleteObject(hfont);
	data = 0; // чтобы ~Picture не выругался
}

void _bitmap::text(int x, int y, std::wstring_view s, int h, uint c, uint bg)
{
	podg_font(h);
	podg_cc(c, bg);
	TextOutW(hdc, x, y, s.data(), (int)s.size());
}

void _bitmap::text(int x, int y, std::string_view s, int h, uint c, uint bg)
{
	podg_font(h);
	podg_cc(c, bg);
	TextOutA(hdc, x, y, s.data(), (int)s.size());
}

_isize _bitmap::size_text(std::wstring_view s, int h)
{
	podg_font(h);
	SIZE a;
	GetTextExtentPoint32W(hdc, s.data(), (int)s.size(), &a);
	return { a.cx, a.cy };
}

_isize _bitmap::size_text(std::string_view s, int h)
{
	podg_font(h);
	SIZE a;
	GetTextExtentPoint32A(hdc, s.data(), (int)s.size(), &a);
	return { a.cx, a.cy };
}

void _bitmap::podg_font(int r)
{
	if (r != font.lfHeight)
	{
		font.lfHeight = r;
		izm_font = true;
	}
	if (izm_font)
	{
		izm_font = false;
		hfont = CreateFontIndirect(&font);
		HGDIOBJ old = SelectObject(hdc, hfont);
		DeleteObject(old);
	}
}

void _bitmap::podg_cc(uint c, uint cf)
{
	c = ((c & 0xff0000) >> 16) + (c & 0xff00) + ((c & 0xff) << 16);
	cf = ((cf & 0xff0000) >> 16) + (cf & 0xff00ff00) + ((cf & 0xff) << 16);
	if (c != f_c) SetTextColor(hdc, f_c = c);
	if (cf != f_cf)
	{
		if (cf >> 24 == 0xff) {
			SetBkColor(hdc, cf);
			if (f_cf >> 24 != 0xff) SetBkMode(hdc, OPAQUE);
		}
		else SetBkMode(hdc, TRANSPARENT);
		f_cf = cf;
	}
}

void _bitmap::set_font(wstr name, bool bold)
{
	izm_font = true;
	font.lfWeight = (bold) ? 700 : 100;
	wcscpy_s(font.lfFaceName, name);
}

void _bitmap::grab_ecran_oo2(HWND hwnd)
{
	RECT rr;
	GetClientRect(hwnd, &rr);
	resize({ (i64)rr.right - rr.left, (i64)rr.bottom - rr.top });
	HDC X = GetDC(hwnd);
	BitBlt(hdc, 0, 0, rr.right - rr.left, rr.bottom - rr.top, X, rr.left, rr.top, SRCCOPY);
	ReleaseDC(hwnd, X);
	// исправление альфа канала
	unsigned long long* ee = (unsigned long long*)data;
	unsigned long long* eemax = (unsigned long long*) & (data[size.x * size.y - 1]);
	while (ee < eemax)*ee++ |= 0xff000000ff000000;
	if (ee == eemax)*((unsigned int*)ee) |= 0xff000000;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

_picture::_picture(_isize r) noexcept
{
	area = size = r.correct();
	if (!size.empty()) data = new uint[size.square()];
}

_picture::_picture(_picture&& move) noexcept : data(move.data), size(move.size), transparent(move.transparent),
area(move.area)
{
	move.data = nullptr;
	move.area = move.size = { 0,0 };
}

void _picture::operator=(const _picture& move) noexcept
{
	resize(move.size);
	transparent = move.transparent;
	memcpy(data, move.data, size.square() * 4);
	area = size;
}

_picture& _picture::operator=(_picture&& move) noexcept
{
	if (&move == this) return *this;
	delete[] data;
	data = move.data;
	size = move.size;
	area = move.area;
	transparent = move.transparent;
	move.data = nullptr;
	move.area = move.size = { 0,0 };
	return *this;
}

void _picture::set_transparent() noexcept
{
	i64 r = size.square();
	for (i64 i = 0; i < r; i++)
		if ((data[i] & 0xff000000) != 0xff000000)
		{
			transparent = true;
			return;
		}
	transparent = false;
}

void _picture::draw(_ixy r, _picture& bm)
{
	_iarea b = move(bm.size, r) & area;
	if (b.empty()) return;
	if (!bm.transparent)
	{
		for (i64 j = b.y.min; j < b.y.max; j++)
			memcpy(px(b.x.min, j), (bm.data + ((j - r.y) * bm.size.x + (b.x.min - r.x))), (b.x.max - b.x.min) * 4);
		return;
	}
	for (i64 j = b.y.min; j < b.y.max; j++)
	{
		uchar* s1_ = px(b.x.min, j);
		uchar* s2_ = (uchar*)(bm.data + ((j - r.y) * bm.size.x + (b.x.min - r.x)));
		for (i64 i = b.x.min; i < b.x.max; i++)
		{
			uint pp2 = s2_[3];
			uint pp1 = 256 - pp2;
			s1_[0] = (s1_[0] * pp1 + s2_[0] * pp2) >> 8;
			s1_[1] = (s1_[1] * pp1 + s2_[1] * pp2) >> 8;
			s1_[2] = (s1_[2] * pp1 + s2_[2] * pp2) >> 8;
			s1_ += 4;
			s2_ += 4;
		}
	}
}

bool _picture::resize(_isize wh) noexcept
{
	wh = wh.correct();
	if (size == wh) return false;
	size = wh;
	delete[] data;
	data = (size.empty()) ? nullptr : (new uint[size.square()]);
	area = size;
	transparent = false;
	return true;
}

void _picture::clear(uint c) noexcept
{
	if (area != size)
	{
		fill_rectangle(size, c, true);
		return;
	}
	transparent = ((c >> 24) != 0xff);
	// сделать 64 цикл!
	fill_rectangle(size, c, true);
}

void _picture::line(_ixy p1, _ixy p2, uint c, bool rep)
{
	uint kk = 255 - (c >> 24);
	if ((kk == 0xFF) && (!rep)) return; // полностью прозрачная
	if (kk == 0) rep = true;
	if (p1.y == p2.y) // горизонтальная линия
	{
		if (p1.x > p2.x) std::swap(p1.x, p2.x);
		if (p1.x < area.x.min) p1.x = area.x.min;
		if (p2.x >= area.x.max) p2.x = area.x.max - 1;
		if ((p1.x > p2.x) || (p1.y < area.y.min) || (p1.y >= area.y.max)) return; // за пределы
		if (rep)
		{
			u64  cc = (((u64)c) << 32) + c;
			u64* ee = (u64*)&(data[p1.y * size.x + p1.x]);
			u64* eemax = (u64*)&(data[p1.y * size.x + p2.x]);
			while (ee < eemax) *ee++ = cc;
			if (ee == eemax) *((uint*)ee) = c;
			return;
		}
		uint   k2 = 256 - kk;
		uint   d1 = (c & 255) * k2;
		uint   d2 = ((c >> 8) & 255) * k2;
		uint   d3 = ((c >> 16) & 255) * k2;
		uchar* c2 = (uchar*)&(data[p1.y * size.x + p1.x]);
		for (i64 d = p1.x - p2.x; d <= 0; d++)
		{
			c2[0] = (c2[0] * kk + d1) >> 8;
			c2[1] = (c2[1] * kk + d2) >> 8;
			c2[2] = (c2[2] * kk + d3) >> 8;
			c2 += 4;
		}
		return;
	}
	if (p1.x == p2.x) // вертикальная линия
	{
		if (p1.y > p2.y) std::swap(p1.y, p2.y);
		if (p1.y < area.y.min) p1.y = area.y.min;
		if (p2.y >= area.y.max) p2.y = area.y.max - 1;
		if ((p1.y > p2.y) || (p1.x < area.x.min) || (p1.x >= area.x.max)) return; // за пределы
		if (rep)
		{
			uint* c2 = &data[p1.y * size.x + p1.x];
			for (i64 y = p1.y - p2.y; y <= 0; y++)
			{
				*c2 = c;
				c2 += size.x;
			}
			return;
		}
		uint   k2 = 256 - kk;
		uint   d1 = (c & 255) * k2;
		uint   d2 = ((c >> 8) & 255) * k2;
		uint   d3 = ((c >> 16) & 255) * k2;
		i64    dc2 = size.x * 4;
		uchar* c2 = (uchar*)&(data[p1.y * size.x + p1.x]);
		for (i64 y = p1.y - p2.y; y <= 0; y++)
		{
			c2[0] = (c2[0] * kk + d1) >> 8;
			c2[1] = (c2[1] * kk + d2) >> 8;
			c2[2] = (c2[2] * kk + d3) >> 8;
			c2 += dc2;
		}
		return;
	}
	i64 d = (abs(p1.x - p2.x) >= abs(p1.y - p2.y)) ? abs(p1.x - p2.x) : abs(p1.y - p2.y);
	i64 dx = ((p2.x - p1.x) << 32) / d;
	i64 dy = ((p2.y - p1.y) << 32) / d;
	i64 x = ((p1.x * 2 + 1) << 31) - dx; // ?? +1 для отрицательных p1.x, p1.y ??
	i64 y = ((p1.y * 2 + 1) << 31) - dy;
	i64 n = 0;
	i64 k = d;
	if (p1.x < area.x.min)
	{
		if (dx <= 0) return;
		n = (i64)((((area.x.min - 0) << 32) - x) / dx);
	}
	else if (p1.x >= area.x.max)
	{
		if (dx >= 0) return;
		n = (int)((((area.x.max - 0) << 32) - x) / dx);
	}
	if (p2.x < area.x.min)
	{
		if (dx >= 0) return;
		k = (int)((((area.x.min - 0) << 32) - x) / dx - 1);
	}
	else if (p2.x >= area.x.max)
	{
		if (dx <= 0) return;
		k = (int)((((area.x.max - 1) << 32) - x) / dx);
	}
	if (p1.y < area.y.min)
	{
		if (dy <= 0) return;
		int n2 = (int)((((area.y.min - 0) << 32) - y) / dy);
		if (n2 > n) n = n2;
	}
	else if (p1.y >= area.y.max)
	{
		if (dy >= 0) return;
		int n2 = (int)((((area.y.max - 0) << 32) - y) / dy);
		if (n2 > n) n = n2;
	}
	if (p2.y < area.y.min)
	{
		if (dy >= 0) return;
		int k2 = (int)((((area.y.min - 0) << 32) - y) / dy - 1);
		if (k2 < k) k = k2;
	}
	else if (p2.y >= area.y.max)
	{
		if (dy <= 0) return;
		int k2 = (int)((((area.y.max - 1) << 32) - y) / dy);
		if (k2 < k) k = k2;
	}
	x += dx * n;
	y += dy * n;
	if (rep)
	{
		for (i64 i = k - n; i >= 0; i--) data[((y += dy) >> 32) * size.x + ((x += dx) >> 32)] = c;
		return;
	}
	uint k2 = 256 - kk;
	uint d1 = (c & 255) * k2;
	uint d2 = ((c >> 8) & 255) * k2;
	uint d3 = ((c >> 16) & 255) * k2;
	for (i64 i = k - n; i >= 0; i--)
	{
		uchar* c2 = (uchar*)&data[((y += dy) >> 32) * size.x + ((x += dx) >> 32)];
		c2[0] = (c2[0] * kk + d1) >> 8;
		c2[1] = (c2[1] * kk + d2) >> 8;
		c2[2] = (c2[2] * kk + d3) >> 8;
	}
}

void _picture::lines(_xy p1, _xy p2, double l, uint c)
{
	uint kk = 255 - (c >> 24);
	if (kk == 0xFF) return; // полностью прозрачная
	if (p1.y > p2.y) std::swap(p1, p2);
	l *= 0.5; // половина толщины
	i64 xmin = int(std::min(p1.x, p2.x) - l);
	i64 xmax = int(std::max(p1.x, p2.x) + l);
	i64 ymin = int(p1.y - l);
	i64 ymax = int(p2.y + l);
	if (xmin < area.x.min) xmin = area.x.min;
	if (ymin < area.y.min) ymin = area.y.min;
	if (xmax >= area.x.max) xmax = area.x.max - 1;
	if (ymax >= area.y.max) ymax = area.y.max - 1;
	if ((xmax < xmin) || (ymax < ymin)) return; // активная область за экраном
	double dx = p2.x - p1.x;
	double dy = p2.y - p1.y;
	if (abs(dy) < 0.001) dy = 0.001; // костыль против горизонтальной линии
	double ll_1 = 1.0 / sqrt(dx * dx + dy * dy);
	if (ll_1 > 2) return; // слишком короткая линия
	double x2y1_y2x1 = p2.x * p1.y - p2.y * p1.x;
	double rcentrscreen = (dy * size.x * 0.5 - dx * size.y * 0.5 + x2y1_y2x1) * ll_1; // расстояние от линии до центра эрана
	if (rcentrscreen < 0) rcentrscreen = -rcentrscreen;
	if (rcentrscreen - l > ((double)size.x + size.y) * 0.5) return; // линия не задевает экран
	double dxdy = dx / dy;
	double db = l * sqrt(dxdy * dxdy + 1.0);
	double xx1 = p1.x + (ymin - p1.y) * dxdy; // х верхней точки пересечения
	double xx2 = xx1 + dxdy;                  // x нижней точки пересечения
	if (dxdy < 0) std::swap(xx1, xx2);
	xx1 -= db;
	xx2 += db;
	double drr = dy * ll_1;
	double temp1 = dy * 0.5 - dx * 0.5 + x2y1_y2x1;
	uint   k2 = 256 - kk;
	uint   red = (c & 255);
	uint   green = ((c >> 8) & 255);
	uint   blue = ((c >> 16) & 255);
	uint   d1 = red * k2;
	uint   d2 = green * k2;
	uint   d3 = blue * k2;
	for (i64 j = ymin; j <= ymax; j++, xx1 += dxdy, xx2 += dxdy) // цикл по линиям
	{
		i64 xx1i = (int)xx1;
		i64 xx2i = (int)xx2;
		if (xx1i < xmin) xx1i = xmin;
		if (xx2i > xmax) xx2i = xmax;
		if (xx2i < xx1i) continue;
		double rr = (dy * xx1i - dx * j + temp1) * ll_1;
		uchar* c2 = (uchar*)&(data[j * size.x + xx1i]);
		for (i64 i = xx1i; i <= xx2i; i++, rr += drr, c2 += 4)
		{
			double rra = ((rr < 0) ? -rr : rr) - 0.5;
			if (rra >= l) continue; // пиксель за границей точки
			if (rra + 1.0 <= l)     // пиксель полностью внутри линии
			{
				c2[0] = (c2[0] * kk + d1) >> 8;
				c2[1] = (c2[1] * kk + d2) >> 8;
				c2[2] = (c2[2] * kk + d3) >> 8;
				continue;
			}
			uint kk2 = (uint)(k2 * ((rra >= -l) ? (l - rra) : (2.0 * l))); // частично внутри, частично снаружи
			uint kkk = 256 - kk2;
			c2[0] = (c2[0] * kkk + red * kk2) >> 8;
			c2[1] = (c2[1] * kkk + green * kk2) >> 8;
			c2[2] = (c2[2] * kkk + blue * kk2) >> 8;
		}
	}
}

void _picture::stretch_draw_speed(_picture* bm, i64 nXDest, i64 nYDest, double m)
{
	if (bm->size.x * bm->size.y == 0) return;
	i64 nWidth = (int)(bm->size.x * m + 0.5);
	i64 nHeight = (int)(bm->size.y * m + 0.5);
	if ((nWidth == bm->size.x) && (nHeight == bm->size.y))
	{
		draw({ nXDest, nYDest }, *bm);
		return;
	}
	i64 nXSrc = 0;
	i64 nYSrc = 0;
	i64 bmrx = nWidth;
	i64 bmry = nHeight;
	if (nXDest < area.x.min)
	{
		nWidth += nXDest - area.x.min;
		nXSrc -= nXDest - area.x.min;
		nXDest = area.x.min;
	}
	if (nYDest < area.y.min)
	{
		nHeight += nYDest - area.y.min;
		nYSrc -= nYDest - area.y.min;
		nYDest = area.y.min;
	}
	if (nXSrc < 0)
	{
		nWidth += nXSrc;
		nXDest -= nXSrc;
		nXSrc = 0;
	}
	if (nYSrc < 0)
	{
		nHeight += nYSrc;
		nYDest -= nYSrc;
		nYSrc = 0;
	}
	if ((nXDest >= area.x.max) || (nYDest >= area.y.max)) return;
	if ((nXSrc >= bmrx) || (nYSrc >= bmry)) return;
	if (nXDest + nWidth > area.x.max) nWidth = area.x.max - nXDest;
	if (nYDest + nHeight > area.y.max) nHeight = area.y.max - nYDest;
	if (nXSrc + nWidth > bmrx) nWidth = bmrx - nXSrc;
	if (nYSrc + nHeight > bmry) nHeight = bmry - nYSrc;
	if ((nWidth <= 0) || (nHeight <= 0)) return;
	double mm = 1.0 / m;
	for (i64 j = 0; j < nHeight; j++)
	{
		uchar* s1 = (uchar*)(data + ((nYDest + j) * size.x + nXDest));
		uint* ss2 = (bm->data + (i64)(((nYSrc + j) * mm)) * bm->size.x);
		for (i64 i = 0; i < nWidth; i++)
		{
			uchar* s2_ = (uchar*)(ss2 + (i64)((nXSrc + i) * mm));
			uint pp2 = s2_[3];
			uint pp1 = 256 - pp2;
			s1[0] = (s1[0] * pp1 + s2_[0] * pp2) >> 8;
			s1[1] = (s1[1] * pp1 + s2_[1] * pp2) >> 8;
			s1[2] = (s1[2] * pp1 + s2_[2] * pp2) >> 8;
			s1 += 4;
		}
	}
}

void _picture::stretch_draw(_picture* bm, i64 x, i64 y, double m)
{
	if (bm->size.x * bm->size.y == 0) return;
	int rx2 = (int)(bm->size.x * m + 0.5);
	int ry2 = (int)(bm->size.y * m + 0.5);
	if ((rx2 == bm->size.x) && (ry2 == bm->size.y))
	{
		draw({ x, y }, *bm);
		return;
	}
	i64 x1 = (x >= area.x.min) ? x : area.x.min;
	i64 y1 = (y >= area.y.min) ? y : area.y.min;
	i64 x2 = x + rx2 - 1;
	i64 y2 = y + ry2 - 1;
	if (x2 >= area.x.max) x2 = area.x.max - 1;
	if (y2 >= area.y.max) y2 = area.y.max - 1;
	if ((x2 < x1) || (y2 < y1)) return;
	i64 nox1 = (i64)x1 - x;
	i64 noy1 = (i64)y1 - y;
	i64 nox2 = (i64)x2 - x;
	i64 noy2 = (i64)y2 - y;
	i64 f1x = bm->size.x;
	i64 f1y = bm->size.y;
	i64 f1xx = rx2;
	i64 f1yy = ry2;
	for (i64 j = noy1; j <= noy2; j++)
	{
		i64  pyn = j * f1y;
		i64  pyk = (j + 1) * f1y - 1;
		i64  pyn2 = pyn / f1yy;
		i64  pyk2 = pyk / f1yy;
		uchar* p1 = (uchar*)&data[(j + y) * size.x + x1 - 1];
		for (i64 i = nox1; i <= nox2; i++)
		{
			p1 += 4;
			i64 pxn = i * f1x;
			i64 pxk = (i + 1) * f1x - 1;
			i64 pxn2 = pxn / f1xx;
			i64 pxk2 = pxk / f1xx;
			if ((pyn2 == pyk2) && (pxn2 == pxk2))
			{
				uchar* p2 = (uchar*)&bm->data[pyn2 * f1x + pxn2];
				uint pp2 = p2[3];
				uint pp1 = 256 - pp2;
				p1[0] = (p1[0] * pp1 + p2[0] * pp2) >> 8;
				p1[1] = (p1[1] * pp1 + p2[1] * pp2) >> 8;
				p1[2] = (p1[2] * pp1 + p2[2] * pp2) >> 8;
				continue;
			}
			if (pyn2 == pyk2)
			{
				uchar* p2 = (uchar*)&bm->data[pyn2 * f1x + pxn2];
				i64 k1 = (pxn2 + 1) * f1xx - pxn;
				i64 k2 = pxk - pxk2 * f1xx + 1;
				i64 S = f1x * 256;
				i64 pp2 = p2[3] * k1;
				i64 R = p2[0] * pp2;
				i64 G = p2[1] * pp2;
				i64 B = p2[2] * pp2;
				i64 A = (255 - p2[3]) * k1;
				p2 += 4;
				for (i64 x_ = pxn2 + 1; x_ < pxk2; x_++)
				{
					pp2 = p2[3] * f1xx;
					R += p2[0] * pp2;
					G += p2[1] * pp2;
					B += p2[2] * pp2;
					A += (255 - p2[3]) * f1xx;
					p2 += 4;
				}
				pp2 = p2[3] * k2;
				R += p2[0] * pp2;
				G += p2[1] * pp2;
				B += p2[2] * pp2;
				A += (255 - p2[3]) * k2;
				p1[0] = (uchar)((p1[0] * A + R) / S);
				p1[1] = (uchar)((p1[1] * A + G) / S);
				p1[2] = (uchar)((p1[2] * A + B) / S);
				continue;
			}
			if (pxn2 == pxk2)
			{
				uchar* p2 = (uchar*)&bm->data[pyn2 * f1x + pxn2];
				i64 k1 = (pyn2 + 1) * f1yy - pyn;
				i64 k2 = pyk - pyk2 * f1yy + 1;
				i64 S = f1y * 256;
				i64 pp2 = p2[3] * k1;
				i64 R = p2[0] * pp2;
				i64 G = p2[1] * pp2;
				i64 B = p2[2] * pp2;
				i64 A = (255 - p2[3]) * k1;
				p2 += (4 * f1x);
				for (i64 y_ = pyn2 + 1; y_ < pyk2; y_++)
				{
					pp2 = p2[3] * f1yy;
					R += p2[0] * pp2;
					G += p2[1] * pp2;
					B += p2[2] * pp2;
					A += (255 - p2[3]) * f1yy;
					p2 += (4 * f1x);
				}
				pp2 = p2[3] * k2;
				R += p2[0] * pp2;
				G += p2[1] * pp2;
				B += p2[2] * pp2;
				A += (255 - p2[3]) * k2;
				p1[0] = (uchar)((p1[0] * A + R) / S);
				p1[1] = (uchar)((p1[1] * A + G) / S);
				p1[2] = (uchar)((p1[2] * A + B) / S);
				continue;
			}
			i64 kx1 = (pxn2 + 1) * f1xx - pxn;
			i64 kx2 = pxk - pxk2 * f1xx + 1;
			i64 ky1 = (pyn2 + 1) * f1yy - pyn;
			i64 ky2 = pyk - pyk2 * f1yy + 1;
			i64 S = f1x * f1y * 256;
			uchar* p2 = (uchar*)&bm->data[pyn2 * f1x + pxn2];
			i64 pp2 = p2[3] * kx1 * ky1;
			i64 R = p2[0] * pp2;
			i64 G = p2[1] * pp2;
			i64 B = p2[2] * pp2;
			i64 A = (255 - p2[3]) * kx1 * ky1;
			p2 += 4;
			for (i64 x_ = pxn2 + 1; x_ < pxk2; x_++)
			{
				pp2 = p2[3] * f1xx * ky1;
				R += p2[0] * pp2;
				G += p2[1] * pp2;
				B += p2[2] * pp2;
				A += (255 - p2[3]) * f1xx * ky1;
				p2 += 4;
			}
			pp2 = p2[3] * kx2 * ky1;
			R += p2[0] * pp2;
			G += p2[1] * pp2;
			B += p2[2] * pp2;
			A += (255 - p2[3]) * kx2 * ky1;
			p2 = (uchar*)&bm->data[pyk2 * f1x + pxn2];
			pp2 = p2[3] * kx1 * ky2;
			R += p2[0] * pp2;
			G += p2[1] * pp2;
			B += p2[2] * pp2;
			A += (255 - p2[3]) * kx1 * ky2;
			p2 += 4;
			for (i64 x_ = pxn2 + 1; x_ < pxk2; x_++)
			{
				pp2 = p2[3] * f1xx * ky2;
				R += p2[0] * pp2;
				G += p2[1] * pp2;
				B += p2[2] * pp2;
				A += (255 - p2[3]) * f1xx * ky2;
				p2 += 4;
			}
			pp2 = p2[3] * kx2 * ky2;
			R += p2[0] * pp2;
			G += p2[1] * pp2;
			B += p2[2] * pp2;
			A += (255 - p2[3]) * kx2 * ky2;
			p2 = (uchar*)&bm->data[(pyn2 + 1) * f1x + pxn2];
			for (i64 y_ = pyn2 + 1; y_ < pyk2; y_++)
			{
				pp2 = p2[3] * f1yy * kx1;
				R += p2[0] * pp2;
				G += p2[1] * pp2;
				B += p2[2] * pp2;
				A += (255 - p2[3]) * f1yy * kx1;
				p2 += (4 * f1x);
			}
			p2 = (uchar*)&bm->data[(pyn2 + 1) * f1x + pxk2];
			for (i64 y_ = pyn2 + 1; y_ < pyk2; y_++)
			{
				pp2 = p2[3] * f1yy * kx2;
				R += p2[0] * pp2;
				G += p2[1] * pp2;
				B += p2[2] * pp2;
				A += (255 - p2[3]) * f1yy * kx2;
				p2 += (4 * f1x);
			}
			for (i64 y_ = pyn2 + 1; y_ < pyk2; y_++)
			{
				p2 = (uchar*)&bm->data[y_ * f1x + pxn2 + 1];
				for (i64 x_ = pxn2 + 1; x_ < pxk2; x_++)
				{
					pp2 = p2[3] * f1xx * f1yy;
					R += p2[0] * pp2;
					G += p2[1] * pp2;
					B += p2[2] * pp2;
					A += (255 - p2[3]) * f1xx * f1yy;
					p2 += 4;
				}
			}
			p1[0] = (uchar)((p1[0] * A + R) / S);
			p1[1] = (uchar)((p1[1] * A + G) / S);
			p1[2] = (uchar)((p1[2] * A + B) / S);
		}
	}
}

void _picture::line_vert_rep_speed(_ixy p, i64 y2, uint c)
{
	uint* c2 = &data[p.y * size.x + p.x];
	for (i64 y = p.y - y2; y <= 0; y++) { *c2 = c; c2 += size.x; }
}

void _picture::fill_rect_rep_speed(_iarea r, uint c)
{
	if (r.x.size() == 1)
	{
		line_vert_rep_speed({ r.x.min, r.y.min }, r.y.max - 1, c);
		return;
	}
	u64 cc = (((u64)c) << 32) + c;
	for (i64 i = r.y.min; i < r.y.max; i++)
	{
		u64* ee = (u64*)(&(data[i * size.x + r.x.min]));
		u64* eemax = (u64*)(&(data[i * size.x + r.x.max - 1]));
		while (ee < eemax) *ee++ = cc;
		if (ee == eemax) *((uint*)ee) = c;
	}
}

void _picture::fill_rectangle(_iarea r, uint c, bool rep)
{
	r &= area;
	if (r.empty()) return;
	uint kk = (c >> 24);
	if (rep || (kk == 0xFF)) { fill_rect_rep_speed(r, c); return; }
	if (kk == 0) return;  // полностью прозрачная
	if (transparent)
		fill_rect_transparent_speed(r, c);
	else
		fill_rect_speed(r, c);
}

void _picture::fill_rect_transparent_speed(_iarea r, uint c)
{
	i64 dx = r.x.size();
	if (dx == 1) // вертикальная линия
	{
		line({ r.x.min, r.y.min }, { r.x.min, r.y.max - 1 }, c); // !!!!!!!!!!!!!!!! вызвать нужную
		return;
	}
	uint kk = 255 - (c >> 24);
	uint k2 = 256 - kk;
	uint d1 = (c & 255) * k2;
	uint d2 = ((c >> 8) & 255) * k2;
	uint d3 = ((c >> 16) & 255) * k2;
	uint d11 = d1 * 255;
	uint d22 = d2 * 255;
	uint d33 = d3 * 255;
	for (i64 i = r.y.min; i < r.y.max; i++)
	{
		uchar* c2 = (uchar*)&(data[i * size.x + r.x.min]);
		for (i64 d = -dx; d < 0; d++)
		{
			uint kk_ = 255 - c2[3];
			uint k2_ = (256 - kk_) * kk;
			uint znam = 65536 - kk * kk_;
			c2[0] = (c2[0] * k2_ + d11) / znam;
			c2[1] = (c2[1] * k2_ + d22) / znam;
			c2[2] = (c2[2] * k2_ + d33) / znam;
			c2[3] = 255 - ((kk_ * kk) >> 8);
			c2 += 4;
		}
	}
}

void _picture::fill_rect_speed(_iarea r, uint c)
{
	i64 dx = r.x.size();
	if (dx == 1) // вертикальная линия
	{
		line({ r.x.min, r.y.min }, { r.x.min, r.y.max - 1 }, c); // !!!!!!!!!!!!!!!! вызвать нужную
		return;
	}
	uint kk = 255 - (c >> 24);
	uint k2 = 256 - kk;
	uint d1 = (c & 255) * k2;
	uint d2 = ((c >> 8) & 255) * k2;
	uint d3 = ((c >> 16) & 255) * k2;
	for (i64 i = r.y.min; i < r.y.max; i++)
	{
		uchar* c2 = (uchar*)&(data[i * size.x + r.x.min]);
		for (i64 d = -dx; d < 0; d++)
		{
			c2[0] = (c2[0] * kk + d1) >> 8;
			c2[1] = (c2[1] * kk + d2) >> 8;
			c2[2] = (c2[2] * kk + d3) >> 8;
			c2 += 4;
		}
	}
}

constexpr int lx2 = 11;
constexpr ushort font16[256][lx2] = { {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {},
   {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {1532}, {28, 0, 0, 28}, {272, 2044, 272, 272, 2044, 272},
   {560, 1096, 4092, 1160, 784}, {520, 276, 148, 584, 1312, 1296, 520}, {920, 1124, 1176, 768, 1152}, {28},
   {4088, 4100}, {4100, 4088}, {40, 16, 40}, {128, 128, 992, 128, 128}, {2048, 1024}, {128, 128}, {1024},
   {1536, 384, 96, 28}, {1016, 1028, 1028, 1028, 1016}, {8, 8, 2044}, {1544, 1284, 1156, 1092, 1080},
   {520, 1028, 1092, 1092, 952}, {384, 352, 280, 2044, 256}, {636, 1060, 1060, 1060, 964},
   {1016, 1092, 1092, 1092, 904}, {4, 1796, 196, 52, 12}, {952, 1092, 1092, 1092, 952}, {568, 1092, 1092, 1092, 1016},
   {1056}, {2048, 1056}, {128, 320, 544, 1040}, {320, 320, 320, 320, 320}, {1040, 544, 320, 128}, {8, 4, 1412, 68, 56},
   {480, 1560, 1032, 2244, 2340, 2340, 2276, 2312, 280, 480}, {1536, 448, 304, 268, 304, 448, 1536},
   {2044, 1092, 1092, 1092, 952}, {1016, 1028, 1028, 1028, 1028, 520}, {2044, 1028, 1028, 1028, 520, 496},
   {2044, 1092, 1092, 1092, 1028}, {2044, 68, 68, 68, 4}, {1016, 1028, 1028, 1092, 580, 1992},
   {2044, 64, 64, 64, 64, 2044}, {2044}, {768, 1024, 1024, 1020}, {2044, 96, 144, 264, 516, 1024},
   {2044, 1024, 1024, 1024, 1024}, {2044, 48, 192, 768, 192, 48, 2044}, {2044, 24, 96, 128, 768, 2044},
   {1016, 1028, 1028, 1028, 1028, 1016}, {2044, 68, 68, 68, 68, 56}, {1016, 1028, 1028, 1284, 1540, 3064},
   {2044, 68, 68, 68, 68, 1976}, {568, 1092, 1092, 1092, 904}, {4, 4, 2044, 4, 4},
   {1020, 1024, 1024, 1024, 1024, 1020}, {12, 112, 384, 1536, 384, 112, 12},
   {12, 112, 384, 1536, 384, 112, 384, 1536, 384, 112, 12}, {1548, 272, 160, 64, 160, 272, 1548},
   {12, 16, 32, 1984, 32, 16, 12}, {1540, 1284, 1156, 1092, 1060, 1044, 1036}, {8188, 4100}, {28, 96, 384, 1536},
   {4100, 8188}, {8, 4, 2, 4, 8}, {4096, 4096, 4096, 4096, 4096, 4096}, {4, 8}, {768, 1184, 1184, 1184, 1984},
   {2044, 1056, 1056, 1056, 960}, {960, 1056, 1056, 1056, 576}, {960, 1056, 1056, 1056, 2044},
   {960, 1184, 1184, 1184, 704}, {2040, 36}, {5056, 5152, 5152, 5152, 4064}, {2044, 64, 32, 32, 1984}, {2020}, {8164},
   {2044, 128, 320, 544, 1024}, {2044}, {2016, 32, 32, 1984, 32, 32, 1984}, {2016, 64, 32, 32, 1984},
   {960, 1056, 1056, 1056, 960}, {8160, 1056, 1056, 1056, 960}, {960, 1056, 1056, 1056, 8160}, {2016, 32},
   {576, 1184, 1312, 576}, {1016, 1056}, {992, 1024, 1024, 512, 2016}, {96, 384, 1536, 384, 96},
   {480, 1536, 384, 96, 384, 1536, 480}, {1632, 384, 384, 1632}, {4096, 4576, 3584, 512, 480},
   {1568, 1312, 1184, 1120}, {64, 1980, 2050}, {4092}, {2050, 1980, 64}, {16, 8, 8, 16, 16, 8}, {1008, 1008, 992},
   {4, 4, 4, 2044, 68, 1604, 1092, 896}, {2044, 4, 6, 5, 4}, {2048, 1024}, {2016, 32, 40, 36},
   {2048, 1024, 2048, 1024}, {1024, 0, 1024, 0, 1024}, {32, 32, 2040, 32, 32}, {544, 544, 2040, 544, 544},
   {160, 1016, 1188, 1060, 1028, 792}, {520, 276, 148, 584, 1312, 1296, 520, 516, 1280, 1280, 512},
   {1024, 1016, 4, 4, 4, 2044, 1088, 1088, 1088, 896}, {256, 640, 1088},
   {2044, 64, 64, 64, 64, 2044, 1088, 1088, 1088, 896}, {2044, 96, 146, 265, 516, 1024},
   {4, 4, 4, 2044, 132, 68, 68, 1920}, {2044, 1024, 7168, 1024, 2044}, {8, 2044, 72, 4136, 4128, 4032}, {8, 16},
   {16, 8}, {4, 8, 4, 8}, {8, 4, 8, 4}, {64, 224, 224, 64}, {64, 64, 64}, {64, 64, 64, 64, 64, 64}, {},
   {4, 28, 4, 0, 28, 4, 8, 4, 28}, {1024, 960, 32, 32, 2016, 1152, 1152, 1152, 768}, {1088, 640, 256},
   {2016, 128, 128, 128, 2016, 1152, 1152, 1152, 768}, {2016, 128, 328, 548, 1024}, {8, 2044, 72, 40, 40, 1984},
   {2016, 1024, 7168, 1024, 2016}, {}, {636, 1153, 1154, 1154, 1153, 1020}, {4580, 4616, 4616, 4068},
   {768, 1024, 1024, 1020}, {372, 136, 136, 136, 372}, {2044, 4, 4, 4, 6}, {8060}, {1128, 2196, 2340, 2628, 1416},
   {2044, 1093, 1092, 1093, 1028}, {112, 136, 292, 340, 340, 260, 136, 112}, {1016, 1092, 1092, 1092, 1028, 520},
   {256, 640, 1344, 640, 1088}, {128, 128, 128, 128, 384}, {64, 64}, {252, 258, 378, 298, 298, 338, 258, 252},
   {1, 2044, 1}, {8, 20, 8}, {1088, 1088, 1520, 1088, 1088}, {2044}, {2020}, {2016, 32, 32, 48},
   {32, 4064, 1024, 1056, 2016, 512}, {56, 124, 4092, 4, 4092}, {64}, {960, 1192, 1184, 1192, 704},
   {2044, 24, 96, 128, 768, 2044, 0, 144, 168, 144}, {960, 1184, 1184, 1056, 576}, {1088, 640, 1344, 640, 256}, {8164},
   {568, 1092, 1092, 1092, 904}, {576, 1184, 1312, 576}, {4, 2016, 4}, {1536, 448, 304, 268, 304, 448, 1536},
   {2044, 1092, 1092, 1092, 900}, {2044, 1092, 1092, 1092, 952}, {2044, 4, 4, 4, 4},
   {3072, 1536, 1532, 1028, 1028, 1028, 2044, 3072}, {2044, 1092, 1092, 1092, 1028},
   {1024, 516, 264, 144, 96, 2044, 96, 144, 264, 516, 1024}, {512, 1032, 1028, 1092, 1092, 952},
   {2044, 768, 128, 96, 24, 2044}, {2044, 769, 130, 98, 25, 2044}, {2044, 96, 144, 264, 516, 1024},
   {1024, 1016, 4, 4, 4, 2044}, {2044, 48, 192, 768, 192, 48, 2044}, {2044, 64, 64, 64, 64, 2044},
   {1016, 1028, 1028, 1028, 1028, 1016}, {2044, 4, 4, 4, 4, 2044}, {2044, 68, 68, 68, 68, 56},
   {1016, 1028, 1028, 1028, 1028, 520}, {4, 4, 2044, 4, 4}, {636, 1152, 1152, 1152, 1152, 1020},
   {112, 136, 136, 136, 2044, 136, 136, 136, 112}, {1548, 272, 160, 64, 160, 272, 1548},
   {2044, 1024, 1024, 1024, 2044, 3072}, {124, 128, 128, 128, 128, 2044}, {2044, 1024, 1024, 2044, 1024, 1024, 2044},
   {2044, 1024, 1024, 2044, 1024, 1024, 2044, 3072}, {12, 4, 2044, 1088, 1088, 1088, 1088, 896},
   {2044, 1088, 1088, 1088, 1088, 896, 0, 2044}, {2044, 1088, 1088, 1088, 1088, 896},
   {520, 1028, 1092, 1092, 1092, 1016}, {2044, 64, 1016, 1028, 1028, 1028, 1016}, {1592, 452, 68, 68, 68, 2044},
   {768, 1184, 1184, 1184, 1984}, {992, 1072, 1064, 1064, 964}, {2016, 1184, 1184, 1184, 832}, {2016, 32, 32, 32},
   {3072, 1984, 1056, 1056, 2016, 3072}, {960, 1184, 1184, 1184, 704}, {1056, 832, 128, 2016, 128, 832, 1056},
   {576, 1056, 1184, 1184, 832}, {2016, 512, 256, 128, 2016}, {2016, 520, 272, 136, 2016}, {2016, 128, 320, 544, 1024},
   {1024, 960, 32, 32, 2016}, {2016, 64, 128, 768, 128, 64, 2016}, {2016, 128, 128, 128, 2016},
   {960, 1056, 1056, 1056, 960}, {2016, 32, 32, 32, 2016}, {8160, 1056, 1056, 1056, 960}, {960, 1056, 1056, 1056, 576},
   {32, 32, 2016, 32, 32}, {4576, 4608, 4608, 4064}, {448, 544, 544, 4080, 544, 544, 448}, {1632, 384, 384, 1632},
   {2016, 1024, 1024, 2016, 3072}, {224, 256, 256, 2016}, {2016, 1024, 1024, 2016, 1024, 1024, 2016},
   {2016, 1024, 1024, 2016, 1024, 1024, 2016, 2048}, {96, 32, 2016, 1152, 1152, 1152, 768},
   {2016, 1152, 1152, 1152, 768, 0, 2016}, {2016, 1152, 1152, 1152, 768}, {576, 1056, 1184, 1184, 960},
   {2016, 128, 960, 1056, 1056, 1056, 960}, {1216, 800, 288, 288, 2016} };

_isize  _picture::size_text16(std::string_view s, i64 n)
{
	i64 l = 0;
	i64 probel = 0;
	for (auto c : s)
	{
		probel = (c == ' ') ? 4 : 1;
		l += probel;
		const ushort* ss = font16[(uchar)(c)];
		int           lx = lx2;
		for (int j = lx - 1; j >= 0; j--)
		{
			if (ss[j]) break;
			lx--;
		}
		l += lx;
	}
	l -= probel;
	return { l * n, 13 * n };
}

void _picture::text16n(i64 x, i64 y, astr s, i64 n, uint c)
{
	if (n <= 1)
	{
		if (n == 1) text16(x, y, s, c);
		return;
	}
	constexpr int ly = 13;
	if ((y >= area.y.max) || (y + ly * n <= area.y.min)) return;
	uint kk = 255 - (c >> 24);
	if (kk == 0xFF) return; // полностью прозрачная
	uint k2 = 256 - kk;
	uint d1 = (c & 255) * k2;
	uint d2 = ((c >> 8) & 255) * k2;
	uint d3 = ((c >> 16) & 255) * k2;
	uint d11 = d1 * 255;
	uint d22 = d2 * 255;
	uint d33 = d3 * 255;

	i64 j0 = 0;
	i64 j1 = ly;
	if (y < area.y.min) j0 = (area.y.min + n - 1 - y) / n;
	if (y + ly * n > area.y.max) j1 = (area.y.max - y) / n;
	while ((*s) && (*s != '\n') && (x < area.x.max))
	{
		i64         probel = (*s == 32) ? 4 : 1;
		const ushort* ss = font16[(uchar)(*s++)];
		i64         lx = lx2;
		for (i64 j = lx - 1; j >= 0; j--)
		{
			if (ss[j]) break;
			lx--;
		}
		if (x + lx * n <= 0)
		{
			x += (lx + probel) * n;
			continue;
		}
		i64 i0 = 0;
		i64 i1_ = lx;
		if (x < area.x.min) i0 = (area.x.min + n - 1 - x) / n;
		if (x + lx * n > area.x.max) i1_ = (area.x.max - x) / n;
		for (i64 j = j0; j < j1; j++)
		{
			ushort mask = (ushort(1) << j);
			for (i64 jj = 0; jj < n; jj++)
			{
				const ushort* sss = ss;
				if (kk == 0)
				{
					uint* c2 = &data[(y + j * n + jj) * size.x + x + i0 * n];
					for (i64 i = i0; i < i1_; i++)
					{
						if (*sss & mask)
						{
							for (int ii = 0; ii < n; ii++)
							{
								*c2 = c;
								c2++;
							}
						}
						else
							c2 += n;
						sss++;
					}
				}
				else
				{
					uchar* c2 = (uchar*)&data[(y + j * n + jj) * size.x + x + i0 * n];
					if (!transparent)
					{
						for (i64 i = i0; i < i1_; i++)
						{
							if (*sss & mask)
							{
								for (int ii = 0; ii < n; ii++)
								{
									c2[0] = (c2[0] * kk + d1) >> 8;
									c2[1] = (c2[1] * kk + d2) >> 8;
									c2[2] = (c2[2] * kk + d3) >> 8;
									c2 += 4;
								}
							}
							else
								c2 += 4uLL * n;
							sss++;
						}
					}
					else
					{
						for (i64 i = i0; i < i1_; i++)
						{
							if (*sss & mask)
							{
								for (int ii = 0; ii < n; ii++)
								{
									uint kk_ = 255 - c2[3];
									uint k2_ = (256 - kk_) * kk;
									uint znam = 65536 - kk * kk_;
									c2[0] = (c2[0] * k2_ + d11) / znam;
									c2[1] = (c2[1] * k2_ + d22) / znam;
									c2[2] = (c2[2] * k2_ + d33) / znam;
									c2[3] = 255 - ((kk_ * kk) >> 8);
									c2 += 4;
								}
							}
							else
								c2 += 4uLL * n;
							sss++;
						}
					}
				}
			}
		}
		x += (lx + probel) * n;
	}
}

void _picture::text16(i64 x, i64 y, std::string_view st, uint c)
{
	constexpr int ly = 13;
	if ((y >= area.y.max) || (y + ly <= area.y.min)) return;
	uint kk = 255 - (c >> 24);
	if (kk == 0xFF) return; // полностью прозрачная
	uint k2 = 256 - kk;
	uint d1 = (c & 255) * k2;
	uint d2 = ((c >> 8) & 255) * k2;
	uint d3 = ((c >> 16) & 255) * k2;
	uint d11 = d1 * 255;
	uint d22 = d2 * 255;
	uint d33 = d3 * 255;

	i64 j0 = 0;
	i64 j1 = ly;
	if (y < area.y.min) j0 = area.y.min - y;
	if (y + ly > area.y.max) j1 = area.y.max - y;
	for (auto s: st)
	{
		if (x >= area.x.max) break;
		i64 probel = (s == 32) ? 4 : 1;
		const ushort* ss = font16[(uchar)s];
		i64 lx = lx2;
		for (i64 j = lx - 1; j >= 0; j--)
		{
			if (ss[j]) break;
			lx--;
		}
		if (x + lx <= 0)
		{
			x += lx + probel;
			continue;
		}
		i64 i0 = 0;
		i64 i1_ = lx;
		if (x < area.x.min) i0 = area.x.min - x;
		if (x + lx > area.x.max) i1_ = area.x.max - x;
		for (i64 j = j0; j < j1; j++)
		{
			ushort        mask = (ushort(1) << j);
			const ushort* sss = ss;
			if (kk == 0)
			{
				uint* c2 = &data[(y + j) * size.x + x + i0];
				for (i64 i = i0; i < i1_; i++)
				{
					if (*sss++ & mask) *c2 = c;
					c2++;
				}
			}
			else
			{
				uchar* c2 = (uchar*)&data[(y + j) * size.x + x + i0];
				if (!transparent)
				{
					for (i64 i = i0; i < i1_; i++)
					{
						if (*sss++ & mask)
						{
							c2[0] = (c2[0] * kk + d1) >> 8;
							c2[1] = (c2[1] * kk + d2) >> 8;
							c2[2] = (c2[2] * kk + d3) >> 8;
						}
						c2 += 4;
					}
				}
				else
				{
					for (i64 i = i0; i < i1_; i++)
					{
						if (*sss++ & mask)
						{
							uint kk_ = 255 - c2[3];
							uint k2_ = (256 - kk_) * kk;
							uint znam = 65536 - kk * kk_;
							c2[0] = (c2[0] * k2_ + d11) / znam;
							c2[1] = (c2[1] * k2_ + d22) / znam;
							c2[2] = (c2[2] * k2_ + d33) / znam;
							c2[3] = 255 - ((kk_ * kk) >> 8);
						}
						c2 += 4;
					}
				}
			}
		}
		x += lx + probel;
	}
}

//  1 -      1
//  2 -      3
//  3 -     11
//  4 -     43
//  5 -    171
//  6 -    683
//  7 -   2731
//  8 -  10923
//  9 -  43691
// 10 - 174763
// 11 - 699051
void _picture::froglif(_xy p, double r, uchar* f, int rf, uint c, uint c2)
{
	if (r < 1) return; // нечего рисовать
	if (((c | c2) >> 24) == 0x00) return; // полностью прозрачный
	uint kk = 255 - (c >> 24);
	uint k2 = 256 - kk;
	uint c_1 = (c & 255);
	uint c_2 = ((c >> 8) & 255);
	uint c_3 = ((c >> 16) & 255);
	uint d1 = c_1 * k2;
	uint d2 = c_2 * k2;
	uint d3 = c_3 * k2;

	double k_a = 0.75; // коэффициент уменьшения толщины линий х.з какой лучше

	i64 x1 = (i64)p.x;
	i64 x2 = (i64)(p.x + r);
	i64 y1 = (i64)p.y;
	i64 y2 = (i64)(p.y + r);

	if (x1 < area.x.min) x1 = area.x.min;
	if (y1 < area.y.min) y1 = area.y.min;
	if (x2 >= area.x.max) x2 = area.x.max - 1;
	if (y2 >= area.y.max) y2 = area.y.max - 1;
	if ((x2 < x1) || (y2 < y1)) return;
	fill_rectangle({ {x1 - 1, x2 + 2}, {y1 - 1, y2 + 2} }, c2);

	double aa = 1.0625;    // минимальная толщина линии
	double bb = 1.0625;    // минимальное пустое место
	double kk_aa = 1.0 / k_a; // коэффициент увеличения линии
	int    rf0 = 0;
	double sl1 = 2 * aa + aa / (kk_aa - 2);
	double sl2 = bb - aa / (kk_aa - 2);
	double ll = aa;
	while (sl1 * kk_aa + sl2 * 2 <= r)
	{
		rf0++;
		sl1 *= kk_aa;
		sl2 *= 2;
		ll *= kk_aa;
	}
	if (rf0 > rf) rf0 = rf;

	struct xxxyyy
	{
		int    ii;     // разреженные биты
		int    sm1;    // смещение ii до 1 на конце
		double l1, l2; // границы столбика
		uchar* f;      // начало нужного куска
		i64 x1, x2;    // реальные координаты начала-конца не выходящие за рамки
		uint kk_1, kk_2;                         // коэффициенты границ
		uint d1_1, d2_1, d3_1, d1_2, d2_2, d3_2; // коэффициенты границ
	};

	int rr = (1 << rf0);
	int rr_1 = rr - 1;

	const int Nxx = 17;
	if (rr + 1 > Nxx) return;
	static xxxyyy xx[Nxx]; // вектор размера rr+1
	static xxxyyy yy[Nxx]; // вектор размера rr+1

	int drf2 = (rf - rf0) * 2;

	for (int i = 0; i <= rr; i++)
	{
		xx[i].ii = ((i & 0x1) + ((i & 0x2) << 1) + ((i & 0x4) << 2) + ((i & 0x8) << 3) + ((i & 0x10) << 4) +
			((i & 0x20) << 5) + ((i & 0x40) << 6) + ((i & 0x80) << 7) + ((i & 0x100) << 8) +
			((i & 0x200) << 9) + ((i & 0x400) << 10) + ((i & 0x800) << 11) + ((i & 0x1000) << 12) +
			((i & 0x2000) << 13) + ((i & 0x4000) << 14))
			<< drf2;
	}

	int sm1 = rf * 2;

	xx[0].l1 = (double)p.x;
	xx[0].l2 = p.x + ll;
	xx[0].sm1 = sm1;
	xx[0].f = f;
	xx[0].x1 = (int)xx[0].l1;
	xx[0].x2 = (int)xx[0].l2;
	xx[rr].l1 = p.x + r - ll;
	xx[rr].l2 = p.x + r;
	xx[rr].sm1 = sm1;
	xx[rr].f = f;
	xx[rr].x1 = (int)xx[rr].l1;
	xx[rr].x2 = (int)xx[rr].l2;
	xxxyyy* xxx = &(xx[0]);
	if ((xxx->x1 <= x2) && (xxx->x2 >= x1))
	{
		if (xxx->l1 < x1)
		{
			xxx->x1 = x1;
			xxx->kk_1 = kk;
			xxx->d1_1 = d1;
			xxx->d2_1 = d2;
			xxx->d3_1 = d3;
		}
		else
		{
			uint k3 = (uint)(k2 * ((xxx->x1 + 1.0) - xxx->l1));
			xxx->kk_1 = 256 - k3;
			xxx->d1_1 = c_1 * k3;
			xxx->d2_1 = c_2 * k3;
			xxx->d3_1 = c_3 * k3;
		}
		if (xxx->x2 > x2)
		{
			xxx->x2 = x2;
			xxx->kk_2 = kk;
			xxx->d1_2 = d1;
			xxx->d2_2 = d2;
			xxx->d3_2 = d3;
		}
		else
		{
			uint k3 = (uint)(k2 * (xxx->l2 - xxx->x2));
			xxx->kk_2 = 256 - k3;
			xxx->d1_2 = c_1 * k3;
			xxx->d2_2 = c_2 * k3;
			xxx->d3_2 = c_3 * k3;
		}
	}
	xxx = &(xx[rr]);
	if ((xxx->x1 <= x2) && (xxx->x2 >= x1))
	{
		if (xxx->l1 < x1)
		{
			xxx->x1 = x1;
			xxx->kk_1 = kk;
			xxx->d1_1 = d1;
			xxx->d2_1 = d2;
			xxx->d3_1 = d3;
		}
		else
		{
			uint k3 = (uint)(k2 * ((xxx->x1 + 1.0) - xxx->l1));
			xxx->kk_1 = 256 - k3;
			xxx->d1_1 = c_1 * k3;
			xxx->d2_1 = c_2 * k3;
			xxx->d3_1 = c_3 * k3;
		}
		if (xxx->x2 > x2)
		{
			xxx->x2 = x2;
			xxx->kk_2 = kk;
			xxx->d1_2 = d1;
			xxx->d2_2 = d2;
			xxx->d3_2 = d3;
		}
		else
		{
			uint k3 = (uint)(k2 * (xxx->l2 - xxx->x2));
			xxx->kk_2 = 256 - k3;
			xxx->d1_2 = c_1 * k3;
			xxx->d2_2 = c_2 * k3;
			xxx->d3_2 = c_3 * k3;
		}
	}

	int    step = (rr >> 1);
	uchar* ff = f;
	int    dff = 1;
	while (step)
	{
		sm1 -= 2;
		ll *= k_a;
		for (int i = step; i <= rr; i += step * 2)
		{
			double  ce = (xx[i - step].l2 + xx[i + step].l1) * 0.5;
			xxx = &(xx[i]);
			xxx->l1 = ce - ll * 0.5;
			xxx->l2 = ce + ll * 0.5;
			xxx->sm1 = sm1;
			xxx->f = ff;
			xxx->x1 = (int)xxx->l1;
			xxx->x2 = (int)xxx->l2;
			if ((xxx->x1 > x2) || (xxx->x2 < x1)) continue;
			if (xxx->l1 < x1)
			{
				xxx->x1 = x1;
				xxx->kk_1 = kk;
				xxx->d1_1 = d1;
				xxx->d2_1 = d2;
				xxx->d3_1 = d3;
			}
			else
			{
				uint k3 = (uint)(k2 * ((xxx->x1 + 1.0) - xxx->l1));
				xxx->kk_1 = 256 - k3;
				xxx->d1_1 = c_1 * k3;
				xxx->d2_1 = c_2 * k3;
				xxx->d3_1 = c_3 * k3;
			}
			if (xxx->x2 > x2)
			{
				xxx->x2 = x2;
				xxx->kk_2 = kk;
				xxx->d1_2 = d1;
				xxx->d2_2 = d2;
				xxx->d3_2 = d3;
			}
			else
			{
				uint k3 = (uint)(k2 * (xxx->l2 - xxx->x2));
				xxx->kk_2 = 256 - k3;
				xxx->d1_2 = c_1 * k3;
				xxx->d2_2 = c_2 * k3;
				xxx->d3_2 = c_3 * k3;
			}
		}
		ff += dff;
		if (dff == 1)
			dff = 2;
		else
			dff *= 4;
		step >>= 1;
	}
	double dy = (double)(p.y - p.x);
	for (int i = 0; i <= rr; i++)
	{
		yy[i].f = xx[i].f;
		yy[i].ii = (xx[i].ii << 1);
		yy[i].sm1 = xx[i].sm1;
		yy[i].l1 = xx[i].l1 + dy;
		yy[i].l2 = xx[i].l2 + dy;
	}
	yy[0].l1 = (double)p.y;     // из-за маленькой дельточки возникла ошибка!
	yy[rr].l2 = p.y + r; // на всякий случай тоже

	int ay = 0;
	while (ay < rr)
	{
		if (y1 < yy[ay].l2) break;
		ay++;
	}
	int ax1 = 0;
	while (ax1 < rr)
	{
		if (x1 < xx[ax1].l2) break;
		ax1++;
	}

	xxxyyy* yyay_1 = &(yy[ay]);
	xxxyyy* yyay = &(yy[ay]);
	for (i64 y = y1; y <= y2; y++)
	{
		if (y >= yyay->l2)
		{
			ay++;
			if (ay > rr) break;
			yyay_1 = yyay;
			yyay = &(yy[ay]);
		}
		int ax = ax1;
		if (y + 1.0 <= yyay->l1) // рисуются только вертикальные линии
		{
			while (true)
			{
				// поиск рисуемой палочки
				while (ax <= rr)
				{
					uint ii;
					if (ax & (rr_1))
						ii = (xx[ax].ii + yy[ay - 1].ii) >> xx[ax].sm1; // ay не может быть равно 0
					else
						ii = 4 + 2 * (ax >> rf0); // первая и последняя палочка
					if (xx[ax].f[ii >> 3] & (uchar(1) << (ii & 7))) break;
					ax++;
				}
				if (ax > rr) break;
				xxxyyy* xxax = &(xx[ax]);
				i64 x11 = xxax->x1;
				if (x11 > x2) break;
				i64 x22 = xxax->x2;
				uchar* cc = (uchar*)&(data[y * size.x + x11]);
				cc[0] = (cc[0] * xxax->kk_1 + xxax->d1_1) >> 8;
				cc[1] = (cc[1] * xxax->kk_1 + xxax->d2_1) >> 8;
				cc[2] = (cc[2] * xxax->kk_1 + xxax->d3_1) >> 8;
				cc += 4;
				i64 d = x11 - x22 + 2;
				if (d <= 1)
				{
					while (d++ <= 0)
					{
						cc[0] = (cc[0] * kk + d1) >> 8;
						cc[1] = (cc[1] * kk + d2) >> 8;
						cc[2] = (cc[2] * kk + d3) >> 8;
						cc += 4;
					}
					cc[0] = (cc[0] * xxax->kk_2 + xxax->d1_2) >> 8;
					cc[1] = (cc[1] * xxax->kk_2 + xxax->d2_2) >> 8;
					cc[2] = (cc[2] * xxax->kk_2 + xxax->d3_2) >> 8;
				}
				ax++;
			}
			continue;
		}
		int stepy = 1 << ((yyay->sm1 - drf2) >> 1);
		int ayx = ax - 1;
		if (ayx < 0) ayx = 0;
		ayx &= (0xFFFFFFFF << ((yyay->sm1 - drf2) >> 1));
		if ((y == (int)yyay->l1) && (yyay->l1 > 0))
		{
			uint k3 = (uint)(k2 * ((y + 1.0) - yyay->l1));
			uint kkw = 256 - k3;
			uint d1w = c_1 * k3;
			uint d2w = c_2 * k3;
			uint d3w = c_3 * k3;
			while (true)
			{
				// поиск рисуемой палочки
				while (ayx < rr)
				{
					uint ii;
					if (ay & (rr_1))
						ii = (((xx[ayx].ii + yyay->ii) >> yyay->sm1) - 1) & 0xFFFFFFFE;
					else
						ii = 5 + 2 * (ay >> rf0); // первая и последняя палочка
					if (yyay->f[ii >> 3] & (uchar(1) << (ii & 7))) break;
					ayx += stepy;
				}
				int ayx_next = ayx + stepy; // следующая после серии палочек
				while (ayx_next < rr)
				{
					uint ii;
					if (ay & (rr_1))
						ii = (((xx[ayx_next].ii + yyay->ii) >> yyay->sm1) - 1) & 0xFFFFFFFE;
					else
						ii = 5 + 2 * (ay >> rf0); // первая и последняя палочка
					if ((yyay->f[ii >> 3] & (uchar(1) << (ii & 7))) == 0) break;
					ayx_next += stepy;
				}
				bool nac = true;
				bool kon = true;
				i64 x11, x22;
				if (ayx >= rr)
				{
					x11 = x2 + 1;
					x22 = x2; // ?? не нужно ??
				}
				else
				{
					x11 = xx[ayx].x1;
					x22 = xx[ayx_next].x2;
					if (x11 < x1)
					{
						x11 = x1;
						nac = false;
					}
					if (x22 > x2)
					{
						x22 = x2;
						kon = false;
					}
				}
				if (x11 > x1) // вертикальные хвостики
				{
					while (true)
					{
						// поиск рисуемой палочки
						bool vepa = false;
						while (ax <= rr)
						{
							xxxyyy* xxax = &(xx[ax]);
							uint    ii, iii;
							if (ax & rr_1)
							{
								if (ay & rr_1)
								{
									ii = (xxax->ii + yyay_1->ii) >> xxax->sm1;
									iii = (xxax->ii + yyay->ii) >> xxax->sm1;
								}
								else
								{
									if (ay)
										ii = (xxax->ii + yyay_1->ii) >> xxax->sm1;
									else
										ii = (xxax->ii + yyay->ii) >> xxax->sm1;
									iii = ii;
								}
							}
							else
							{
								ii = 4 + 2 * (ax >> rf0); // первая и последняя палочка
								iii = ii;
							}
							if (xxax->f[ii >> 3] & (uchar(1) << (ii & 7)))
							{
								if (ay) vepa = true;
								break;
							}
							if (xxax->f[iii >> 3] & (uchar(1) << (iii & 7))) break;
							ax++;
						}
						if (ax > rr) break;
						xxxyyy* xxax = &(xx[ax]);
						i64 xx11 = xxax->x1;
						if ((xx11 >= x11) || (xx11 > x2)) break;
						i64 xx22 = xxax->x2;
						uchar* cc = (uchar*)&(data[y * size.x + xx11]);
						if (vepa)
						{
							cc[0] = (cc[0] * xxax->kk_1 + xxax->d1_1) >> 8;
							cc[1] = (cc[1] * xxax->kk_1 + xxax->d2_1) >> 8;
							cc[2] = (cc[2] * xxax->kk_1 + xxax->d3_1) >> 8;
							cc += 4;
							i64 d = xx11 - xx22 + 2;
							if (d <= 1)
							{
								while (d++ <= 0)
								{
									cc[0] = (cc[0] * kk + d1) >> 8;
									cc[1] = (cc[1] * kk + d2) >> 8;
									cc[2] = (cc[2] * kk + d3) >> 8;
									cc += 4;
								}
								cc[0] = (cc[0] * xxax->kk_2 + xxax->d1_2) >> 8;
								cc[1] = (cc[1] * xxax->kk_2 + xxax->d2_2) >> 8;
								cc[2] = (cc[2] * xxax->kk_2 + xxax->d3_2) >> 8;
							}
						}
						else
						{
							k3 = ((256 - xxax->kk_1) * (256 - kkw) / k2);
							uint kkw2 = 256 - k3;
							cc[0] = (cc[0] * kkw2 + c_1 * k3) >> 8;
							cc[1] = (cc[1] * kkw2 + c_2 * k3) >> 8;
							cc[2] = (cc[2] * kkw2 + c_3 * k3) >> 8;
							cc += 4;
							i64 d = xx11 - xx22 + 2;
							if (d <= 1)
							{
								while (d++ <= 0)
								{
									cc[0] = (cc[0] * kkw + d1w) >> 8;
									cc[1] = (cc[1] * kkw + d2w) >> 8;
									cc[2] = (cc[2] * kkw + d3w) >> 8;
									cc += 4;
								}
								k3 = ((256 - xxax->kk_2) * (256 - kkw) / k2);
								kkw2 = 256 - k3;
								cc[0] = (cc[0] * kkw2 + c_1 * k3) >> 8;
								cc[1] = (cc[1] * kkw2 + c_2 * k3) >> 8;
								cc[2] = (cc[2] * kkw2 + c_3 * k3) >> 8;
							}
						}
						ax++;
					}
				}
				if (x11 > x2) break;
				uchar* cc = (uchar*)&(data[y * size.x + x11]);
				while (x11 <= x22)
				{
					// поиск рисуемой палочки
					if (ay == 0)
						ax = ayx_next + 1;
					else
						while (ax <= ayx_next)
						{
							xxxyyy* xxax = &(xx[ax]);
							uint    ii;
							if (ax & rr_1)
								ii = (xxax->ii + yyay_1->ii) >> xxax->sm1;
							else
								ii = 4 + 2 * (ax >> rf0); // первая и последняя палочка
							if (xxax->f[ii >> 3] & (uchar(1) << (ii & 7))) break;
							ax++;
						}
					xxxyyy* xxax = 0;
					i64 xx11, xx22 = 0; // для параноии компилятора
					if (ax <= rr)
					{
						xxax = &(xx[ax]);
						xx11 = xxax->x1;
						if (xx11 > x22) xx11 = x22 + 1;
						xx22 = xxax->x2;
					}
					else
					{
						xx11 = x22 + 1;
					}
					if (xx11 > x11)
					{
						i64 d = x11 - xx11 + 1;
						if (nac)
						{
							nac = false;
							d++;
							k3 = ((256 - xx[ayx].kk_1) * (256 - kkw) / k2);
							uint kkw2 = 256 - k3;
							cc[0] = (cc[0] * kkw2 + c_1 * k3) >> 8;
							cc[1] = (cc[1] * kkw2 + c_2 * k3) >> 8;
							cc[2] = (cc[2] * kkw2 + c_3 * k3) >> 8;
							cc += 4;
						}
						if (xx11 <= x22)
						{
							while (d++ <= 0)
							{
								cc[0] = (cc[0] * kkw + d1w) >> 8;
								cc[1] = (cc[1] * kkw + d2w) >> 8;
								cc[2] = (cc[2] * kkw + d3w) >> 8;
								cc += 4;
							}
						}
						else
						{
							if (d <= 0)
							{
								if (kon) d++;
								while (d++ <= 0)
								{
									cc[0] = (cc[0] * kkw + d1w) >> 8;
									cc[1] = (cc[1] * kkw + d2w) >> 8;
									cc[2] = (cc[2] * kkw + d3w) >> 8;
									cc += 4;
								}
								if (kon)
								{
									k3 = ((256 - xx[ayx_next].kk_2) * (256 - kkw) / k2);
									uint kkw2 = 256 - k3;
									cc[0] = (cc[0] * kkw2 + c_1 * k3) >> 8;
									cc[1] = (cc[1] * kkw2 + c_2 * k3) >> 8;
									cc[2] = (cc[2] * kkw2 + c_3 * k3) >> 8;
								}
							}
							break;
						}
					}
					uint sl1_, sl2_, kkw2;
					sl2_ = 256 - kkw;
					if (nac)
					{
						nac = false;
						cc[0] = (cc[0] * xxax->kk_1 + xxax->d1_1) >> 8;
						cc[1] = (cc[1] * xxax->kk_1 + xxax->d2_1) >> 8;
						cc[2] = (cc[2] * xxax->kk_1 + xxax->d3_1) >> 8;
					}
					else
					{
						sl1_ = 256 - xxax->kk_1;
						k3 = sl1_ + sl2_ - sl1_ * sl2_ / k2;
						kkw2 = 256 - k3;
						cc[0] = (cc[0] * kkw2 + c_1 * k3) >> 8;
						cc[1] = (cc[1] * kkw2 + c_2 * k3) >> 8;
						cc[2] = (cc[2] * kkw2 + c_3 * k3) >> 8;
					}
					cc += 4;
					i64 d = xx11 - xx22 + 2;
					if (d <= 1)
					{
						while (d++ <= 0)
						{
							cc[0] = (cc[0] * kk + d1) >> 8;
							cc[1] = (cc[1] * kk + d2) >> 8;
							cc[2] = (cc[2] * kk + d3) >> 8;
							cc += 4;
						}
						if ((xx22 >= x22) && kon)
						{
							cc[0] = (cc[0] * xxax->kk_2 + xxax->d1_2) >> 8;
							cc[1] = (cc[1] * xxax->kk_2 + xxax->d2_2) >> 8;
							cc[2] = (cc[2] * xxax->kk_2 + xxax->d3_2) >> 8;
						}
						else
						{
							sl1_ = 256 - xxax->kk_2;
							k3 = sl1_ + sl2_ - sl1_ * sl2_ / k2;
							kkw2 = 256 - k3;
							cc[0] = (cc[0] * kkw2 + c_1 * k3) >> 8;
							cc[1] = (cc[1] * kkw2 + c_2 * k3) >> 8;
							cc[2] = (cc[2] * kkw2 + c_3 * k3) >> 8;
						}
						cc += 4;
					}
					x11 = xx22 + 1;
					ax++;
				}
				if (x22 == x2) break;
				ayx = ayx_next;
			}
			continue;
		}
		if (y == (int)yyay->l2)
		{
			uint k3 = (uint)(k2 * (yyay->l2 - y));
			uint kkw = 256 - k3;
			uint d1w = c_1 * k3;
			uint d2w = c_2 * k3;
			uint d3w = c_3 * k3;
			while (true)
			{
				// поиск рисуемой палочки
				while (ayx < rr)
				{
					uint ii;
					if (ay & (rr_1))
						ii = (((xx[ayx].ii + yyay->ii) >> yyay->sm1) - 1) & 0xFFFFFFFE;
					else
						ii = 5 + 2 * (ay >> rf0); // первая и последняя палочка
					if (yyay->f[ii >> 3] & (uchar(1) << (ii & 7))) break;
					ayx += stepy;
				}
				int ayx_next = ayx + stepy; // следующая после серии палочек
				while (ayx_next < rr)
				{
					uint ii;
					if (ay & (rr_1))
						ii = (((xx[ayx_next].ii + yyay->ii) >> yyay->sm1) - 1) & 0xFFFFFFFE;
					else
						ii = 5 + 2 * (ay >> rf0); // первая и последняя палочка
					if ((yyay->f[ii >> 3] & (uchar(1) << (ii & 7))) == 0) break;
					ayx_next += stepy;
				}
				bool nac = true;
				bool kon = true;
				i64 x11, x22;
				if (ayx >= rr)
				{
					x11 = x2 + 1;
					x22 = x2; // ?? не нужно ??
				}
				else
				{
					x11 = xx[ayx].x1;
					x22 = xx[ayx_next].x2;
					if (x11 < x1)
					{
						x11 = x1;
						nac = false;
					}
					if (x22 > x2)
					{
						x22 = x2;
						kon = false;
					}
				}
				if (x11 > x1) // вертикальные хвостики
				{
					while (true)
					{
						// поиск рисуемой палочки
						bool nipa = false;
						while (ax <= rr)
						{
							xxxyyy* xxax = &(xx[ax]);
							uint    ii, iii;
							if (ax & rr_1)
							{
								if (ay & rr_1)
								{
									ii = (xxax->ii + yyay_1->ii) >> xxax->sm1;
									iii = (xxax->ii + yyay->ii) >> xxax->sm1;
								}
								else
								{
									if (ay)
										ii = (xxax->ii + yyay_1->ii) >> xxax->sm1;
									else
										ii = (xxax->ii + yyay->ii) >> xxax->sm1;
									iii = ii;
								}
							}
							else
							{
								ii = 4 + 2 * (ax >> rf0); // первая и последняя палочка
								iii = ii;
							}
							if (xxax->f[iii >> 3] & (uchar(1) << (iii & 7)))
							{
								if (ay < rr) nipa = true;
								break;
							}
							if (xxax->f[ii >> 3] & (uchar(1) << (ii & 7))) break;
							ax++;
						}
						if (ax > rr) break;
						xxxyyy* xxax = &(xx[ax]);
						i64 xx11 = xxax->x1;
						if ((xx11 >= x11) || (xx11 > x2)) break;
						i64 xx22 = xxax->x2;
						uchar* cc = (uchar*)&(data[y * size.x + xx11]);
						if (nipa)
						{
							cc[0] = (cc[0] * xxax->kk_1 + xxax->d1_1) >> 8;
							cc[1] = (cc[1] * xxax->kk_1 + xxax->d2_1) >> 8;
							cc[2] = (cc[2] * xxax->kk_1 + xxax->d3_1) >> 8;
							cc += 4;
							i64 d = xx11 - xx22 + 2;
							if (d <= 1)
							{
								while (d++ <= 0)
								{
									cc[0] = (cc[0] * kk + d1) >> 8;
									cc[1] = (cc[1] * kk + d2) >> 8;
									cc[2] = (cc[2] * kk + d3) >> 8;
									cc += 4;
								}
								cc[0] = (cc[0] * xxax->kk_2 + xxax->d1_2) >> 8;
								cc[1] = (cc[1] * xxax->kk_2 + xxax->d2_2) >> 8;
								cc[2] = (cc[2] * xxax->kk_2 + xxax->d3_2) >> 8;
							}
						}
						else
						{
							k3 = ((256 - xxax->kk_1) * (256 - kkw) / k2);
							uint kkw2 = 256 - k3;
							cc[0] = (cc[0] * kkw2 + c_1 * k3) >> 8;
							cc[1] = (cc[1] * kkw2 + c_2 * k3) >> 8;
							cc[2] = (cc[2] * kkw2 + c_3 * k3) >> 8;
							cc += 4;
							i64 d = xx11 - xx22 + 2;
							if (d <= 1)
							{
								while (d++ <= 0)
								{
									cc[0] = (cc[0] * kkw + d1w) >> 8;
									cc[1] = (cc[1] * kkw + d2w) >> 8;
									cc[2] = (cc[2] * kkw + d3w) >> 8;
									cc += 4;
								}
								k3 = ((256 - xxax->kk_2) * (256 - kkw) / k2);
								kkw2 = 256 - k3;
								cc[0] = (cc[0] * kkw2 + c_1 * k3) >> 8;
								cc[1] = (cc[1] * kkw2 + c_2 * k3) >> 8;
								cc[2] = (cc[2] * kkw2 + c_3 * k3) >> 8;
							}
						}
						ax++;
					}
				}
				if (x11 > x2) break;
				uchar* cc = (uchar*)&(data[y * size.x + x11]);
				while (x11 <= x22)
				{
					// поиск рисуемой палочки
					if (ay == rr)
						ax = ayx_next + 1;
					else
						while (ax <= ayx_next)
						{
							xxxyyy* xxax = &(xx[ax]);
							uint    ii;
							if (ax & rr_1)
								ii = (xxax->ii + yyay->ii) >> xxax->sm1;
							else
								ii = 4 + 2 * (ax >> rf0); // первая и последняя палочка
							if (xxax->f[ii >> 3] & (uchar(1) << (ii & 7))) break;
							ax++;
						}
					xxxyyy* xxax = 0;
					i64 xx11, xx22 = 0; // для параноии компилятора
					if (ax <= rr)
					{
						xxax = &(xx[ax]);
						xx11 = xxax->x1;
						if (xx11 > x22) xx11 = x22 + 1;
						xx22 = xxax->x2;
					}
					else
					{
						xx11 = x22 + 1;
					}
					if (xx11 > x11)
					{
						i64 d = x11 - xx11 + 1;
						if (nac)
						{
							nac = false;
							d++;
							k3 = ((256 - xx[ayx].kk_1) * (256 - kkw) / k2);
							uint kkw2 = 256 - k3;
							cc[0] = (cc[0] * kkw2 + c_1 * k3) >> 8;
							cc[1] = (cc[1] * kkw2 + c_2 * k3) >> 8;
							cc[2] = (cc[2] * kkw2 + c_3 * k3) >> 8;
							cc += 4;
						}
						if (xx11 <= x22)
						{
							while (d++ <= 0)
							{
								cc[0] = (cc[0] * kkw + d1w) >> 8;
								cc[1] = (cc[1] * kkw + d2w) >> 8;
								cc[2] = (cc[2] * kkw + d3w) >> 8;
								cc += 4;
							}
						}
						else
						{
							if (d <= 0)
							{
								if (kon) d++;
								while (d++ <= 0)
								{
									cc[0] = (cc[0] * kkw + d1w) >> 8;
									cc[1] = (cc[1] * kkw + d2w) >> 8;
									cc[2] = (cc[2] * kkw + d3w) >> 8;
									cc += 4;
								}
								if (kon)
								{
									k3 = ((256 - xx[ayx_next].kk_2) * (256 - kkw) / k2);
									uint kkw2 = 256 - k3;
									cc[0] = (cc[0] * kkw2 + c_1 * k3) >> 8;
									cc[1] = (cc[1] * kkw2 + c_2 * k3) >> 8;
									cc[2] = (cc[2] * kkw2 + c_3 * k3) >> 8;
								}
							}
							break;
						}
					}
					uint sl1_, sl2_, kkw2;
					sl2_ = 256 - kkw;
					if (xxax == nullptr) break; // для параноии компилятора
					if (nac)
					{
						nac = false;
						cc[0] = (cc[0] * xxax->kk_1 + xxax->d1_1) >> 8;
						cc[1] = (cc[1] * xxax->kk_1 + xxax->d2_1) >> 8;
						cc[2] = (cc[2] * xxax->kk_1 + xxax->d3_1) >> 8;
					}
					else
					{
						sl1_ = 256 - xxax->kk_1;
						k3 = sl1_ + sl2_ - sl1_ * sl2_ / k2;
						kkw2 = 256 - k3;
						cc[0] = (cc[0] * kkw2 + c_1 * k3) >> 8;
						cc[1] = (cc[1] * kkw2 + c_2 * k3) >> 8;
						cc[2] = (cc[2] * kkw2 + c_3 * k3) >> 8;
					}
					cc += 4;
					i64 d = xx11 - xx22 + 2;
					if (d <= 1)
					{
						while (d++ <= 0)
						{
							cc[0] = (cc[0] * kk + d1) >> 8;
							cc[1] = (cc[1] * kk + d2) >> 8;
							cc[2] = (cc[2] * kk + d3) >> 8;
							cc += 4;
						}
						if ((xx22 >= x22) && kon)
						{
							cc[0] = (cc[0] * xxax->kk_2 + xxax->d1_2) >> 8;
							cc[1] = (cc[1] * xxax->kk_2 + xxax->d2_2) >> 8;
							cc[2] = (cc[2] * xxax->kk_2 + xxax->d3_2) >> 8;
						}
						else
						{
							sl1_ = 256 - xxax->kk_2;
							k3 = sl1_ + sl2_ - sl1_ * sl2_ / k2;
							kkw2 = 256 - k3;
							cc[0] = (cc[0] * kkw2 + c_1 * k3) >> 8;
							cc[1] = (cc[1] * kkw2 + c_2 * k3) >> 8;
							cc[2] = (cc[2] * kkw2 + c_3 * k3) >> 8;
						}
						cc += 4;
					}
					x11 = xx22 + 1;
					ax++;
				}
				if (x22 == x2) break;
				ayx = ayx_next;
			}
			continue;
		}
		while (true)
		{
			// поиск рисуемой палочки
			while (ayx < rr)
			{
				uint ii;
				if (ay & (rr_1))
					ii = (((xx[ayx].ii + yyay->ii) >> yyay->sm1) - 1) & 0xFFFFFFFE;
				else
					ii = 5 + 2 * (ay >> rf0); // первая и последняя палочка
				if (yyay->f[ii >> 3] & (uchar(1) << (ii & 7))) break;
				ayx += stepy;
			}
			int ayx_next = ayx + stepy; // следующая после серии палочек
			while (ayx_next < rr)
			{
				uint ii;
				if (ay & (rr_1))
					ii = (((xx[ayx_next].ii + yyay->ii) >> yyay->sm1) - 1) & 0xFFFFFFFE;
				else
					ii = 5 + 2 * (ay >> rf0); // первая и последняя палочка
				if ((yyay->f[ii >> 3] & (uchar(1) << (ii & 7))) == 0) break;
				ayx_next += stepy;
			}
			bool nac = true;
			bool kon = true;
			i64 x11, x22;
			if (ayx >= rr)
			{
				x11 = x2 + 1;
				x22 = x2; // ?? не нужно ??
			}
			else
			{
				x11 = xx[ayx].x1;
				x22 = xx[ayx_next].x2;
				if (x11 < x1)
				{
					x11 = x1;
					nac = false;
				}
				if (x22 > x2)
				{
					x22 = x2;
					kon = false;
				}
			}
			if (x11 > x1) // вертикальные хвостики x222
			{
				while (true)
				{
					// поиск рисуемой палочки
					while (ax <= rr)
					{
						xxxyyy* xxax = &(xx[ax]);
						uint    ii, iii;
						if (ax & rr_1)
						{
							if (ay & rr_1)
							{
								ii = (xxax->ii + yyay_1->ii) >> xxax->sm1;
								iii = (xxax->ii + yyay->ii) >> xxax->sm1;
							}
							else
							{
								if (ay)
									ii = (xxax->ii + yyay_1->ii) >> xxax->sm1;
								else
									ii = (xxax->ii + yyay->ii) >> xxax->sm1;
								iii = ii;
							}
						}
						else
						{
							ii = 4 + 2 * (ax >> rf0); // первая и последняя палочка
							iii = ii;
						}
						if (xxax->f[ii >> 3] & (uchar(1) << (ii & 7))) break;
						if (xxax->f[iii >> 3] & (uchar(1) << (iii & 7))) break;
						ax++;
					}
					if (ax > rr) break;
					xxxyyy* xxax = &(xx[ax]);
					i64 xx11 = xxax->x1;
					if ((xx11 >= x11) || (xx11 > x2)) break;
					i64 xx22 = xxax->x2;
					uchar* cc = (uchar*)&(data[y * size.x + xx11]);
					cc[0] = (cc[0] * xxax->kk_1 + xxax->d1_1) >> 8;
					cc[1] = (cc[1] * xxax->kk_1 + xxax->d2_1) >> 8;
					cc[2] = (cc[2] * xxax->kk_1 + xxax->d3_1) >> 8;
					cc += 4;
					i64 d = xx11 - xx22 + 2;
					if (d <= 1)
					{
						while (d++ <= 0)
						{
							cc[0] = (cc[0] * kk + d1) >> 8;
							cc[1] = (cc[1] * kk + d2) >> 8;
							cc[2] = (cc[2] * kk + d3) >> 8;
							cc += 4;
						}
						cc[0] = (cc[0] * xxax->kk_2 + xxax->d1_2) >> 8;
						cc[1] = (cc[1] * xxax->kk_2 + xxax->d2_2) >> 8;
						cc[2] = (cc[2] * xxax->kk_2 + xxax->d3_2) >> 8;
					}
					ax++;
				}
			}
			if (x11 > x2) break;
			uchar* cc = (uchar*)&(data[y * size.x + x11]);
			i64 d = x11 - x22 + 1;
			xxxyyy* xxax = &(xx[ayx]);
			if (nac)
			{
				d++;
				cc[0] = (cc[0] * xxax->kk_1 + xxax->d1_1) >> 8;
				cc[1] = (cc[1] * xxax->kk_1 + xxax->d2_1) >> 8;
				cc[2] = (cc[2] * xxax->kk_1 + xxax->d3_1) >> 8;
				cc += 4;
			}
			if (d <= 1)
			{
				if (!kon) d--;
				while (d++ <= 0)
				{
					cc[0] = (cc[0] * kk + d1) >> 8;
					cc[1] = (cc[1] * kk + d2) >> 8;
					cc[2] = (cc[2] * kk + d3) >> 8;
					cc += 4;
				}
				if (kon)
				{
					xxax = &(xx[ayx_next]);
					cc[0] = (cc[0] * xxax->kk_2 + xxax->d1_2) >> 8;
					cc[1] = (cc[1] * xxax->kk_2 + xxax->d2_2) >> 8;
					cc[2] = (cc[2] * xxax->kk_2 + xxax->d3_2) >> 8;
				}
			}
			if (x22 == x2) break;
			ayx = ayx_next;
			ax = ayx + 1;
		}
	}
}

void _picture::rectangle(_iarea oo, uint c)
{
	if (oo.empty()) return;
	line({ oo.x.min, oo.y.min }, { oo.x.max - 1, oo.y.min }, c);
	if (oo.y.max - oo.y.min == 1) return;
	line({ oo.x.min, oo.y.max - 1 }, { oo.x.max - 1, oo.y.max - 1 }, c);
	if (oo.y.max - oo.y.min > 2)
	{ // такие заморочки из за углов
		line({ oo.x.min, oo.y.min + 1 }, { oo.x.min, oo.y.max - 2 }, c);
		line({ oo.x.max - 1, oo.y.min + 1 }, { oo.x.max - 1, oo.y.max - 2 }, c);
	}
}

_stack& operator<<(_stack& o, _picture const& p)
{
	o << p.size << p.transparent;
	o.push_data(p.data, 4 * p.size.square());
	return o;
}

_stack& operator>>(_stack& o, _picture& p)
{
	_isize r;
	o >> r >> p.transparent;
	p.resize(r);
	o.pop_data(p.data, 4 * p.size.square());
	return o;
}

void _picture::fill_rect_d(double x1, double y1, double x2, double y2, uint c)
{
	uint kk = 255 - (c >> 24);
	if (kk == 0xFF) return; // полностью прозрачная
	if (x1 > x2) std::swap(x1, x2);
	if (y1 > y2) std::swap(y1, y2);
	if (x1 < area.x.min) x1 = (double)area.x.min;
	if (y1 < area.y.min) y1 = (double)area.y.min;
	if (x2 >= area.x.max) x2 = area.x.max - 0.0001;
	if (y2 >= area.y.max) y2 = area.y.max - 0.0001;
	if ((x2 - x1 < 0.004) || (y2 - y1 < 0.004)) return; // слишком тонкий или за пределами

	i64 xx1 = (i64)x1;
	i64 xx2 = (i64)x2;
	i64 yy1 = (i64)y1;
	i64 yy2 = (i64)y2;
	uint r1 = (c & 255);
	uint r2 = ((c >> 8) & 255);
	uint r3 = ((c >> 16) & 255);

	if (yy1 == yy2) // горизонтальная линия
	{
		uchar* c2 = px(xx1, yy1);
		if (xx1 == xx2) // точка
		{
			uint k2 = (uint)((256 - kk) * (x2 - x1) * (y2 - y1));
			kk = 256 - k2;
			c2[0] = (c2[0] * kk + r1 * k2) >> 8;
			c2[1] = (c2[1] * kk + r2 * k2) >> 8;
			c2[2] = (c2[2] * kk + r3 * k2) >> 8;
			return;
		}
		uint k2 = (uint)((256 - kk) * (y2 - y1));

		uint kk2 = (uint)(k2 * (xx1 + 1.0 - x1));
		uint kkk = 256 - kk2;
		c2[0] = (c2[0] * kkk + r1 * kk2) >> 8;
		c2[1] = (c2[1] * kkk + r2 * kk2) >> 8;
		c2[2] = (c2[2] * kkk + r3 * kk2) >> 8;
		c2 += 4;

		kk = 256 - k2;
		uint d1 = r1 * k2;
		uint d2 = r2 * k2;
		uint d3 = r3 * k2;
		i64 d = xx1 - xx2 + 2;
		while (d <= 0)
		{
			c2[0] = (c2[0] * kk + d1) >> 8;
			c2[1] = (c2[1] * kk + d2) >> 8;
			c2[2] = (c2[2] * kk + d3) >> 8;
			c2 += 4;
			d++;
		}

		kk2 = (uint)(k2 * (x2 - xx2));
		kkk = 256 - kk2;
		c2[0] = (c2[0] * kkk + r1 * kk2) >> 8;
		c2[1] = (c2[1] * kkk + r2 * kk2) >> 8;
		c2[2] = (c2[2] * kkk + r3 * kk2) >> 8;

		return;
	}
	if (xx1 == xx2) // вертикальная линия
	{
		uint k2 = (uint)((256 - kk) * (x2 - x1));

		uchar* c2 = px(xx1, yy1);
		uint   kk2 = (uint)(k2 * (yy1 + 1.0 - y1));
		uint   kkk = 256 - kk2;
		c2[0] = (c2[0] * kkk + r1 * kk2) >> 8;
		c2[1] = (c2[1] * kkk + r2 * kk2) >> 8;
		c2[2] = (c2[2] * kkk + r3 * kk2) >> 8;

		kk = 256 - k2;
		uint d1 = r1 * k2;
		uint d2 = r2 * k2;
		uint d3 = r3 * k2;
		for (i64 y = yy1 + 1; y < yy2; y++)
		{
			c2 = px(xx1, y);
			c2[0] = (c2[0] * kk + d1) >> 8;
			c2[1] = (c2[1] * kk + d2) >> 8;
			c2[2] = (c2[2] * kk + d3) >> 8;
		}

		c2 = px(xx1, yy2);
		kk2 = (uint)(k2 * (y2 - yy2));
		kkk = 256 - kk2;
		c2[0] = (c2[0] * kkk + r1 * kk2) >> 8;
		c2[1] = (c2[1] * kkk + r2 * kk2) >> 8;
		c2[2] = (c2[2] * kkk + r3 * kk2) >> 8;

		return;
	}
	u64 cc = (((u64)c) << 32) + c;
	for (i64 y = yy1; y <= yy2; y++)
	{
		uchar* c2 = px(xx1, y);

		uint k2 = (256 - kk);
		if (y == yy1) k2 = (uint)(k2 * (yy1 + 1.0 - y1));
		if (y == yy2) k2 = (uint)(k2 * (y2 - yy2));

		uint kk2 = (uint)(k2 * (xx1 + 1.0 - x1));
		uint kkk = 256 - kk2;
		c2[0] = (c2[0] * kkk + r1 * kk2) >> 8;
		c2[1] = (c2[1] * kkk + r2 * kk2) >> 8;
		c2[2] = (c2[2] * kkk + r3 * kk2) >> 8;
		c2 += 4;

		if (k2 >= 255)
		{
			u64* ee = (u64*)c2;
			u64* eemax = (u64*)px(xx2 - 1, y);
			while (ee < eemax) *ee++ = cc;
			if (ee == eemax) *((uint*)ee) = c;
		}
		else
		{
			kkk = 256 - k2;
			uint d1 = r1 * k2;
			uint d2 = r2 * k2;
			uint d3 = r3 * k2;
			i64  d = xx1 - xx2 + 2;
			while (d <= 0)
			{
				c2[0] = (c2[0] * kkk + d1) >> 8;
				c2[1] = (c2[1] * kkk + d2) >> 8;
				c2[2] = (c2[2] * kkk + d3) >> 8;
				c2 += 4;
				d++;
			}
		}

		c2 = px(xx2, y);
		kk2 = (uint)(k2 * (x2 - xx2));
		kkk = 256 - kk2;
		c2[0] = (c2[0] * kkk + r1 * kk2) >> 8;
		c2[1] = (c2[1] * kkk + r2 * kk2) >> 8;
		c2[2] = (c2[2] * kkk + r3 * kk2) >> 8;
	}
}

void _picture::fill_ring(_xy p, double r, double d, uint c, uint c2)
{
	if (r < 0.5) return; // слишком маленький
	if (c == c2)
	{
		fill_circle(p, r, c);
		return;
	}
	uint kk_ = 255 - (c2 >> 24);
	if (kk_ == 0xFF) // полностью прозрачная внутренность
	{
		ring(p, r, d, c);
		return;
	}
	double r2 = r - d;
	if (r2 < 0) r2 = 0;
	i64 y1 = (i64)(p.y - r);
	y1 = std::max(area.y.min, y1);
	i64 y2 = (i64)(p.y + r);
	y2 = std::min(area.y.max - 1, y2);
	i64 x1 = (i64)(p.x - r);
	x1 = std::max(area.x.min, x1);
	i64 x2 = (i64)(p.x + r);
	x2 = std::min(area.x.max - 1, x2);
	if ((x2 < x1) || (y2 < y1)) return;
	double rrmin = (r - 0.5) * (r - 0.5);
	double rrmax = (r + 0.5) * (r + 0.5);
	double drr = rrmax - rrmin;
	double ddmin = (r2 - 0.5) * (r2 - 0.5);
	double ddmax = (r2 + 0.5) * (r2 + 0.5);
	double ddd = ddmax - ddmin;
	p.x -= 0.5;
	p.y -= 0.5;
	double dxdx0 = (x1 - p.x) * (x1 - p.x);
	double ab0 = 2 * (x1 - p.x) + 1;
	uint kk = 255 - (c >> 24);
	uint k2 = 256 - kk;
	uint c_0 = (c & 255);
	uint c_1 = ((c >> 8) & 255);
	uint c_2 = ((c >> 16) & 255);
	uint d1 = c_0 * k2;
	uint d2 = c_1 * k2;
	uint d3 = c_2 * k2;
	uint k2_ = 256 - kk_;
	uint c2_0 = (c2 & 255);
	uint c2_1 = ((c2 >> 8) & 255);
	uint c2_2 = ((c2 >> 16) & 255);
	uint dd1 = c2_0 * k2_;
	uint dd2 = c2_1 * k2_;
	uint dd3 = c2_2 * k2_;
	if ((kk == 0) && (kk_ == 0))
	{
		for (i64 i = y1; i <= y2; i++)
		{
			double dd = (i - p.y) * (i - p.y) + dxdx0;
			double ab = ab0;
			uchar* cc = px(x1, i);
			i64    d_ = x1 - x2;
			while (d_++ <= 0)
			{
				if (dd < rrmax)
				{
					if (dd <= ddmin) { *((uint*)cc) = c2; }
					else if (dd > rrmin)
					{
						if (dd >= ddmax)
						{
							uint k22 = (uint)(k2 * (rrmax - dd) / drr);
							uint kk2 = 256 - k22;
							cc[0] = (cc[0] * kk2 + c_0 * k22) >> 8;
							cc[1] = (cc[1] * kk2 + c_1 * k22) >> 8;
							cc[2] = (cc[2] * kk2 + c_2 * k22) >> 8;
						}
						else
						{ // тонкое кольцо
							double m1 = (ddmax - dd) / ddd;
							uint   k22 = (uint)(k2_ * m1);
							uint   k33 = (uint)(k2 * ((rrmax - dd) / drr - m1));
							uint   kk2 = 256 - k22 - k33;
							cc[0] = (cc[0] * kk2 + c2_0 * k22 + c_0 * k33) >> 8;
							cc[1] = (cc[1] * kk2 + c2_1 * k22 + c_1 * k33) >> 8;
							cc[2] = (cc[2] * kk2 + c2_2 * k22 + c_2 * k33) >> 8;
						}
					}
					else if (dd >= ddmax)
					{
						*((uint*)cc) = c;
					}
					else
					{
						double m1 = (ddmax - dd) / ddd;
						uint   k22 = (uint)(k2_ * m1);
						uint   k33 = (uint)(k2 * (1 - m1));
						uint   kk2 = 256 - k22 - k33;
						cc[0] = (cc[0] * kk2 + c2_0 * k22 + c_0 * k33) >> 8;
						cc[1] = (cc[1] * kk2 + c2_1 * k22 + c_1 * k33) >> 8;
						cc[2] = (cc[2] * kk2 + c2_2 * k22 + c_2 * k33) >> 8;
					}
				}
				cc += 4;
				dd += ab;
				ab += 2;
			}
		}
		return;
	}
	for (i64 i = y1; i <= y2; i++)
	{
		double dd = (i - p.y) * (i - p.y) + dxdx0;
		double ab = ab0;
		uchar* cc = px(x1, i);
		i64 d_ = x1 - x2;
		while (d_++ <= 0)
		{
			if (dd < rrmax)
			{
				if (dd <= ddmin)
				{
					cc[0] = (cc[0] * kk_ + dd1) >> 8;
					cc[1] = (cc[1] * kk_ + dd2) >> 8;
					cc[2] = (cc[2] * kk_ + dd3) >> 8;
				}
				else if (dd > rrmin)
				{
					if (dd >= ddmax)
					{
						uint k22 = (uint)(k2 * (rrmax - dd) / drr);
						uint kk2 = 256 - k22;
						cc[0] = (cc[0] * kk2 + c_0 * k22) >> 8;
						cc[1] = (cc[1] * kk2 + c_1 * k22) >> 8;
						cc[2] = (cc[2] * kk2 + c_2 * k22) >> 8;
					}
					else
					{ // тонкое кольцо
						double m1 = (ddmax - dd) / ddd;
						uint   k22 = (uint)(k2_ * m1);
						uint   k33 = (uint)(k2 * ((rrmax - dd) / drr - m1));
						uint   kk2 = 256 - k22 - k33;
						cc[0] = (cc[0] * kk2 + c2_0 * k22 + c_0 * k33) >> 8;
						cc[1] = (cc[1] * kk2 + c2_1 * k22 + c_1 * k33) >> 8;
						cc[2] = (cc[2] * kk2 + c2_2 * k22 + c_2 * k33) >> 8;
					}
				}
				else if (dd >= ddmax)
				{
					cc[0] = (cc[0] * kk + d1) >> 8;
					cc[1] = (cc[1] * kk + d2) >> 8;
					cc[2] = (cc[2] * kk + d3) >> 8;
				}
				else
				{
					double m1 = (ddmax - dd) / ddd;
					uint   k22 = (uint)(k2_ * m1);
					uint   k33 = (uint)(k2 * (1 - m1));
					uint   kk2 = 256 - k22 - k33;
					cc[0] = (cc[0] * kk2 + c2_0 * k22 + c_0 * k33) >> 8;
					cc[1] = (cc[1] * kk2 + c2_1 * k22 + c_1 * k33) >> 8;
					cc[2] = (cc[2] * kk2 + c2_2 * k22 + c_2 * k33) >> 8;
				}
			}
			cc += 4;
			dd += ab;
			ab += 2;
		}
	}
}

void _picture::ring(_xy p, double r, double d, uint c)
{
	if (r < 0.5) return; // слишком маленький
	double r2 = r - d;
	if (r2 < 0) r2 = 0;
	i64 y1 = (int)(p.y - r);
	y1 = std::max(area.y.min, y1);
	i64 y2 = (int)(p.y + r);
	y2 = std::min(area.y.max - 1, y2);
	i64 x1 = (int)(p.x - r);
	x1 = std::max(area.x.min, x1);
	i64 x2 = (int)(p.x + r);
	x2 = std::min(area.x.max - 1, x2);
	if ((x2 < x1) || (y2 < y1)) return;

	double rrmin = (r - 0.5) * (r - 0.5);
	double rrmax = (r + 0.5) * (r + 0.5);
	double drr = rrmax - rrmin;
	double ddmin = (r2 - 0.5) * (r2 - 0.5);
	double ddmax = (r2 + 0.5) * (r2 + 0.5);
	double ddd = ddmax - ddmin;

	double xxx2 = (size.x / 2 - p.x) * (size.x / 2 - p.x) + (size.y / 2 - p.y) * (size.y / 2 - p.y);
	double yyy2 = 0.25 * size.x * size.x + 0.25 * size.y * size.y;
	if (xxx2 + yyy2 + 2 * sqrt(xxx2 * yyy2) < ddmin) return; // экран внутри кольца

	p.x -= 0.5;
	p.y -= 0.5;
	double dxdx0 = (x1 - p.x) * (x1 - p.x);
	double ab0 = 2 * (x1 - p.x) + 1;
	uint kk = 255 - (c >> 24);
	uint k2 = 256 - kk;
	uint c_0 = (c & 255);
	uint c_1 = ((c >> 8) & 255);
	uint c_2 = ((c >> 16) & 255);
	uint d1 = c_0 * k2;
	uint d2 = c_1 * k2;
	uint d3 = c_2 * k2;
	if (kk == 0)
	{
		for (i64 i = y1; i <= y2; i++)
		{
			double dd = (i - p.y) * (i - p.y) + dxdx0;
			double ab = ab0;
			uchar* cc = px(x1, i);
			i64 d_ = x1 - x2;
			while (d_++ <= 0)
			{
				if ((dd < rrmax) && (dd > ddmin))
				{
					if (dd > rrmin)
					{
						if (dd >= ddmax)
						{
							uint k22 = (uint)(k2 * (rrmax - dd) / drr);
							uint kk2 = 256 - k22;
							cc[0] = (cc[0] * kk2 + c_0 * k22) >> 8;
							cc[1] = (cc[1] * kk2 + c_1 * k22) >> 8;
							cc[2] = (cc[2] * kk2 + c_2 * k22) >> 8;
						}
						else
						{ // тонкое кольцо
							uint k22 = (uint)(k2 * ((rrmax - dd) / drr - (ddmax - dd) / ddd));
							uint kk2 = 256 - k22;
							cc[0] = (cc[0] * kk2 + c_0 * k22) >> 8;
							cc[1] = (cc[1] * kk2 + c_1 * k22) >> 8;
							cc[2] = (cc[2] * kk2 + c_2 * k22) >> 8;
						}
					}
					else if (dd >= ddmax)
					{
						*((uint*)cc) = c;
					}
					else
					{
						uint k22 = (uint)(k2 * (dd - ddmin) / ddd);
						uint kk2 = 256 - k22;
						cc[0] = (cc[0] * kk2 + c_0 * k22) >> 8;
						cc[1] = (cc[1] * kk2 + c_1 * k22) >> 8;
						cc[2] = (cc[2] * kk2 + c_2 * k22) >> 8;
					}
				}
				cc += 4;
				dd += ab;
				ab += 2;
			}
		}
		return;
	}
	for (i64 i = y1; i <= y2; i++)
	{
		double dd = (i - p.y) * (i - p.y) + dxdx0;
		double ab = ab0;
		uchar* cc = px(x1, i);
		i64 d_ = x1 - x2;
		while (d_++ <= 0)
		{
			if ((dd < rrmax) && (dd > ddmin))
			{
				if (dd > rrmin)
				{
					if (dd >= ddmax)
					{
						uint k22 = (uint)(k2 * (rrmax - dd) / drr);
						uint kk2 = 256 - k22;
						cc[0] = (cc[0] * kk2 + c_0 * k22) >> 8;
						cc[1] = (cc[1] * kk2 + c_1 * k22) >> 8;
						cc[2] = (cc[2] * kk2 + c_2 * k22) >> 8;
					}
					else
					{ // тонкое кольцо
						uint k22 = (uint)(k2 * ((rrmax - dd) / drr - (ddmax - dd) / ddd));
						uint kk2 = 256 - k22;
						cc[0] = (cc[0] * kk2 + c_0 * k22) >> 8;
						cc[1] = (cc[1] * kk2 + c_1 * k22) >> 8;
						cc[2] = (cc[2] * kk2 + c_2 * k22) >> 8;
					}
				}
				else if (dd >= ddmax)
				{
					cc[0] = (cc[0] * kk + d1) >> 8;
					cc[1] = (cc[1] * kk + d2) >> 8;
					cc[2] = (cc[2] * kk + d3) >> 8;
				}
				else
				{
					uint k22 = (uint)(k2 * (dd - ddmin) / ddd);
					uint kk2 = 256 - k22;
					cc[0] = (cc[0] * kk2 + c_0 * k22) >> 8;
					cc[1] = (cc[1] * kk2 + c_1 * k22) >> 8;
					cc[2] = (cc[2] * kk2 + c_2 * k22) >> 8;
				}
			}
			cc += 4;
			dd += ab;
			ab += 2;
		}
	}
}

void _picture::fill_circle(_xy p, double r, uint c)
{
	if (r < 0.5) return; // слишком маленький
	i64 y1 = (i64)(p.y - r);
	y1 = std::max(area.y.min, y1);
	i64 y2 = (i64)(p.y + r);
	y2 = std::min(area.y.max - 1, y2);
	i64 x1 = (i64)(p.x - r);
	x1 = std::max(area.x.min, x1);
	i64 x2 = (i64)(p.x + r);
	x2 = std::min(area.x.max - 1, x2);
	if ((x2 < x1) || (y2 < y1)) return;

	double rrmin = (r - 0.5) * (r - 0.5);
	double rrmax = (r + 0.5) * (r + 0.5);
	double drr = rrmax - rrmin;
	p.x -= 0.5;
	p.y -= 0.5;
	double dxdx0 = (x1 - p.x) * (x1 - p.x);
	double ab0 = 2 * (x1 - p.x) + 1;
	uint kk = 255 - (c >> 24);
	uint k2 = 256 - kk;
	uint c_0 = (c & 255);
	uint c_1 = ((c >> 8) & 255);
	uint c_2 = ((c >> 16) & 255);
	uint d1 = c_0 * k2;
	uint d2 = c_1 * k2;
	uint d3 = c_2 * k2;
	if (kk == 0)
	{
		for (i64 i = y1; i <= y2; i++)
		{
			double dd = (i - p.y) * (i - p.y) + dxdx0;
			double ab = ab0;
			uchar* cc = px(x1, i);
			i64 d = x1 - x2;
			while (d++ <= 0)
			{
				if (dd < rrmax)
				{
					if (dd > rrmin)
					{
						uint k22 = (uint)(k2 * (rrmax - dd) / drr);
						uint kk2 = 256 - k22;
						cc[0] = (cc[0] * kk2 + c_0 * k22) >> 8;
						cc[1] = (cc[1] * kk2 + c_1 * k22) >> 8;
						cc[2] = (cc[2] * kk2 + c_2 * k22) >> 8;
					}
					else
						*((uint*)cc) = c;
				}
				cc += 4;
				dd += ab;
				ab += 2;
			}
		}
		return;
	}
	for (i64 i = y1; i <= y2; i++)
	{
		double dd = (i - p.y) * (i - p.y) + dxdx0;
		double ab = ab0;
		uchar* cc = px(x1, i);
		i64 d = x1 - x2;
		while (d++ <= 0)
		{
			if (dd < rrmax)
			{
				if (dd > rrmin)
				{
					uint k22 = (uint)(k2 * (rrmax - dd) / drr);
					uint kk2 = 256 - k22;
					cc[0] = (cc[0] * kk2 + c_0 * k22) >> 8;
					cc[1] = (cc[1] * kk2 + c_1 * k22) >> 8;
					cc[2] = (cc[2] * kk2 + c_2 * k22) >> 8;
				}
				else
				{
					cc[0] = (cc[0] * kk + d1) >> 8;
					cc[1] = (cc[1] * kk + d2) >> 8;
					cc[2] = (cc[2] * kk + d3) >> 8;
				}
			}
			cc += 4;
			dd += ab;
			ab += 2;
		}
	}
}

void _hsva::operator=(uint c)
{
	double r = ((c >> 16) & 0xFF) / 255.0;
	double g = ((c >> 8) & 0xFF) / 255.0;
	double b = (c & 0xFF) / 255.0;
	double ma = std::max(r, std::max(g, b));
	double mi = std::min(r, std::min(g, b));
	if (ma == mi)
		h = 180;
	else if (ma == r)
		h = 60.0 * (g - b) / (ma - mi) + 360.0 * (g < b);
	else if (ma == g)
		h = 60.0 * (b - r) / (ma - mi) + 120.0;
	else
		h = 60.0 * (r - g) / (ma - mi) + 240.0;
	if (ma == 0)
		s = 0;
	else
		s = 1.0 - mi / ma;
	v = ma;
	a = (c >> 24) / 255.0;
}

_hsva::operator uint()
{
	uint   hi = ((int)(h / 60.0)) % 6;
	double vmin = (1.0 - s) * v;
	double aa = (v - vmin) * ((((int)h) % 60) / 60.0);
	double vinc = vmin + aa;
	double vdec = v - aa;
	double r, g, b;
	switch (hi)
	{
	case 0:
		r = v;
		g = vinc;
		b = vmin;
		break;
	case 1:
		r = vdec;
		g = v;
		b = vmin;
		break;
	case 2:
		r = vmin;
		g = v;
		b = vinc;
		break;
	case 3:
		r = vmin;
		g = vdec;
		b = v;
		break;
	case 4:
		r = vinc;
		g = vmin;
		b = v;
		break;
	case 5:
		r = v;
		g = vmin;
		b = vdec;
		break;
	default: r = g = b = 1.0;
	}
	uint rr = (uint)(r * 255);
	uint gg = (uint)(g * 255);
	uint bb = (uint)(b * 255);
	uint prr = (uint)(a * 255);
	return (prr << 24) + bb + (gg << 8) + (rr << 16);
}

uint brighten(uint c)
{
	uchar* cc = (uchar*)(&c);
	cc[0] = (cc[0] <= 85) ? (cc[0] * 2) : (128 + cc[0] / 2);
	cc[1] = (cc[1] <= 85) ? (cc[1] * 2) : (128 + cc[1] / 2);
	cc[2] = (cc[2] <= 85) ? (cc[2] * 2) : (128 + cc[2] / 2);
	return c;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

constexpr uint c_pak0 = 0;          // для сжатия картинок
constexpr uint c_pak1 = 0xFF208040; // для сжатия картинок

_wjson& _wjson::str(std::string_view name, bool lin)
{
	add_start(name) << '{';
	if (lin)
		file << ' ';
	else
		file << std::endl;
	start = true;
	arra.push_back(false);
	line.push_back(lin);
	return *this;
}

_wjson& _wjson::arr(std::string_view name, bool lin)
{
	add_start(name) << '[';
	if (lin)
		file << ' ';
	else
		file << std::endl;
	start = true;
	arra.push_back(true);
	line.push_back(lin);
	return *this;
}

std::ofstream& _wjson::add_start(std::string_view name)
{
	if (!start)
	{
		file << ",";
		if (line.back())
			file << ' ';
		else
			file << std::endl;
	}
	start = false;
	if (!line.back()) cout();
	if (name != "")	file << "\"" << name << "\": ";
	return file;
}

_wjson& _wjson::end()
{
	if (!start)
		if (line.back())
			file << ' ';
		else
			file << std::endl;
	bool a = arra.back();
	arra.pop_back();
	if (!line.back()) cout();
	line.pop_back();
	file << ((a) ? ']' : '}');
	start = false;
	return *this;
}

_wjson::_wjson(wstr fn)
{
	file.open(fn);
	file << std::setprecision(18);
	line.push_back(false);
	str();
}

std::ofstream& _wjson::cout()
{
	for (uint i = 0; i < arra.size(); i++) file << "\t";
	return file;
}

std::string wstring_to_string(std::wstring_view b)
{
	static std::map<wchar_t, char> conv = {
		// нельзя " и #
		{L'!', '!'},   {L'$', '$'}, {L'%', '%'}, {L'&', '&'}, {L'(', '('}, {L')', ')'},   {L'*', '*'}, {L'+', '+'},
		{L',', ','},   {L'-', '-'}, {L'.', '.'}, {L'/', '/'}, {L'0', '0'}, {L'1', '1'},   {L'2', '2'}, {L'3', '3'},
		{L'4', '4'},   {L'5', '5'}, {L'6', '6'}, {L'7', '7'}, {L'8', '8'}, {L'9', '9'},   {L':', ':'}, {L';', ';'},
		{L'<', '<'},   {L'=', '='}, {L'>', '>'}, {L'?', '?'}, {L'@', '@'}, {L'A', 'A'},   {L'B', 'B'}, {L'C', 'C'},
		{L'D', 'D'},   {L'E', 'E'}, {L'F', 'F'}, {L'G', 'G'}, {L'H', 'H'}, {L'I', 'I'},   {L'J', 'J'}, {L'K', 'K'},
		{L'L', 'L'},   {L'M', 'M'}, {L'N', 'N'}, {L'O', 'O'}, {L'P', 'P'}, {L'Q', 'Q'},   {L'R', 'R'}, {L'S', 'S'},
		{L'T', 'T'},   {L'U', 'U'}, {L'V', 'V'}, {L'W', 'W'}, {L'X', 'X'}, {L'Y', 'Y'},   {L'Z', 'Z'}, {L'[', '['},
		{L'\\', '\\'}, {L']', ']'}, {L'^', '^'}, {L'_', '_'}, {L'`', '`'}, {L'\'', '\''}, {L'a', 'a'}, {L'b', 'b'},
		{L'c', 'c'},   {L'd', 'd'}, {L'e', 'e'}, {L'f', 'f'}, {L'g', 'g'}, {L'h', 'h'},   {L'i', 'i'}, {L'j', 'j'},
		{L'k', 'k'},   {L'l', 'l'}, {L'm', 'm'}, {L'n', 'n'}, {L'o', 'o'}, {L'p', 'p'},   {L'q', 'q'}, {L'r', 'r'},
		{L's', 's'},   {L't', 't'}, {L'u', 'u'}, {L'v', 'v'}, {L'w', 'w'}, {L'x', 'x'},   {L'y', 'y'}, {L'z', 'z'},
		{L'А', 'А'},   {L'Б', 'Б'}, {L'В', 'В'}, {L'Г', 'Г'}, {L'Д', 'Д'}, {L'Е', 'Е'},   {L'Ё', 'Ё'}, {L'Ж', 'Ж'},
		{L'З', 'З'},   {L'И', 'И'}, {L'Й', 'Й'}, {L'К', 'К'}, {L'Л', 'Л'}, {L'М', 'М'},   {L'Н', 'Н'}, {L'О', 'О'},
		{L'П', 'П'},   {L'Р', 'Р'}, {L'С', 'С'}, {L'Т', 'Т'}, {L'У', 'У'}, {L'Ф', 'Ф'},   {L'Х', 'Х'}, {L'Ц', 'Ц'},
		{L'Ч', 'Ч'},   {L'Ш', 'Ш'}, {L'Щ', 'Щ'}, {L'Ъ', 'Ъ'}, {L'Ы', 'Ы'}, {L'Ь', 'Ь'},   {L'Э', 'Э'}, {L'Ю', 'Ю'},
		{L'Я', 'Я'},   {L'а', 'а'}, {L'б', 'б'}, {L'в', 'в'}, {L'г', 'г'}, {L'д', 'д'},   {L'е', 'е'}, {L'ё', 'ё'},
		{L'ж', 'ж'},   {L'з', 'з'}, {L'и', 'и'}, {L'й', 'й'}, {L'к', 'к'}, {L'л', 'л'},   {L'м', 'м'}, {L'н', 'н'},
		{L'о', 'о'},   {L'п', 'п'}, {L'р', 'р'}, {L'с', 'с'}, {L'т', 'т'}, {L'у', 'у'},   {L'ф', 'ф'}, {L'х', 'х'},
		{L'ц', 'ц'},   {L'ч', 'ч'}, {L'ш', 'ш'}, {L'щ', 'щ'}, {L'ъ', 'ъ'}, {L'ы', 'ы'},   {L'ь', 'ь'}, {L'э', 'э'},
		{L'ю', 'ю'},   {L'я', 'я'}, {L' ', ' '} };
	static const char  zz[] = "0123456789abcdef";
	std::ostringstream res;
	for (auto i : b)
	{
		char r = conv[i];
		if (r)
		{
			res << r;
			continue;
		}
		ushort e = i;
		res << '#' << zz[(e >> 12) & 15] << zz[(e >> 8) & 15] << zz[(e >> 4) & 15] << zz[e & 15];
	}
	return res.str();
}

constexpr uint hex_to_byte[256] = {
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 0, 0, 0, 0, 0, 0,
	0,10,11,12,13,14,15, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0,10,11,12,13,14,15, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

wstr string_to_wstring2(std::string_view s)
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
	L'п', L'р', L'с', L'т', L'у', L'ф', L'х', L'ц', L'ч', L'ш', L'щ', L'ъ', L'ы', L'ь', L'э', L'ю', L'я' };

	static std::vector<wchar_t> temp;
	temp.clear();

	for (u64 i = 0; i < s.size(); i++)
	{
		char c = s[i];
		if (c != '#')
		{
			temp.push_back(conv[(uchar)c]);
			continue;
		}
		ushort aa = (hex_to_byte[(uchar)s[i + 1]] << 12) + (hex_to_byte[(uchar)s[i + 2]] << 8) +
			(hex_to_byte[(uchar)s[i + 3]] << 4) + hex_to_byte[(uchar)s[i + 4]];
		i += 4;
		temp.push_back(aa);
	}
	temp.push_back(0);
	return temp.data();
}

_wjson& _wjson::add_hex(std::string_view name, u64 b)
{
	add_start(name) << "0x" << std::hex << b << std::dec;
	return *this;
}

_wjson& _wjson::add(std::string_view name, std::wstring_view b)
{
	add_start(name) << "\"" << wstring_to_string(b) << "\"";
	return *this;
}

_wjson& _wjson::add(std::string_view name, const _multi_string& b)
{
	arr(name);
	for (auto& i : b.line) add(i);
	end();
	return *this;
}

_wjson& _wjson::add(std::string_view name, _trans b)
{
	str(name, true);
	add("scale", b.scale);
	add("offset", b.offset);
	end();
	return *this;
}

_wjson& _wjson::add(std::string_view name, _xy b)
{
	str(name, true);
	add("x", b.x);
	add("y", b.y);
	end();
	return *this;
}

_wjson& _wjson::add(std::string_view name, _area b)
{
	str(name, true);
	add("x", b.x);
	add("y", b.y);
	end();
	return *this;
}

_wjson& _wjson::add(std::string_view name, _interval b)
{
	str(name, true);
	add("min", b.min);
	add("max", b.max);
	end();
	return *this;
}

_wjson& _wjson::add(std::string_view name, const _picture& b)
{
	arr(name);
	bool pak = false;
	if (b.size == _isize{ 24, 24 })
	{
		pak = true;
		for (i64 i = 0; i < 576; i++)
			if ((b.data[i] != c_pak0) && (b.data[i] != c_pak1))
			{
				pak = false;
				break;
			}
	}
	if (!pak)
	{
		for (int j = 0; j < b.size.y; j++) add_mem(&b.data[j * b.size.x], b.size.x * 4ULL);
	}
	else
	{
		uchar a[72] = {};
		for (i64 i = 0; i < 576; i++)
			if (b.data[i] == c_pak1)
				a[i >> 3] |= (1 << (i & 7));
		add_mem(a, 72);
	}
	end();
	return *this;
}

_wjson& _wjson::add_mem(std::string_view name, void* b, u64 size)
{
	static const char zz[] = "0123456789abcdef";
	uchar* c = (uchar*)b;
	std::string       s(size * 2, ' ');
	for (u64 i = 0; i < size; i++)
	{
		s[i * 2] = zz[(c[i] >> 4) & 15];
		s[i * 2 + 1] = zz[c[i] & 15];
	}
	add(name, s);
	return *this;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

_rjson::_rjson(wstr fn)
{
	file.open(fn);
	if (!file.good()) { error = 55; return; }
	obj();
}

bool _rjson::obj(std::string_view name)
{
	if (!read_start(name)) return false;
	char c = 0;
	file >> c;
	if ((c == ']') || (c == '}'))
	{
		file.unget();
		null = true;
		return false;
	}
	if (c != '{') { error = 1; return false; }
	arra.push_back(false);
	start = true;
	return true;
}

bool _rjson::arr(std::string_view name)
{
	if (!read_start(name)) return false;
	char c = 0;
	file >> c;
	if ((c == ']') || (c == '}'))
	{
		file.unget();
		null = true;
		return false;
	}
	if (c != '[') { error = 2; return false; }
	arra.push_back(true);
	start = true;
	return true;
}

void _rjson::end()
{
	if (error) return;
	bool arr = arra.back();
	arra.pop_back();
	char c = 0;
	file >> c;
	if (c != ((arr) ? ']' : '}')) error = 3;
	null = false;
	start = false;
}

astr _rjson::read_just_string()
{
	if (error) return "";
	char c = 0;
	file >> c;
	if (c != '\"') { error = 4; return ""; }
	static std::vector<char> buff;
	buff.clear();
	while (true)
	{
		c = 0;
		file.get(c);
		if (!file.good()) { error = 5; return ""; }
		if (c == '\"') break;
		buff.push_back(c);
	}
	buff.push_back(0);
	return buff.data();
}

bool _rjson::read_start(std::string_view name)
{
	if (error || null) return false;
	if (!start)
	{
		char c = 0;
		file >> c;
		if ((c == ']') || (c == '}'))
		{
			file.unget();
			null = true;
			return false;
		}
		if (c != ',') { error = 6; return false; }
	}
	start = false;
	if (name == "") return true;
	std::string s = read_just_string();
	if (s != name) { error = 7; return false; }
	char c = 0;
	file >> c;
	if (c == ':') return true;
	error = 8;
	return false;
}

astr _rjson::read_string(std::string_view name)
{
	if (!read_start(name)) return "";
	return read_just_string();
}

void _rjson::read(std::string_view name, char& b)
{
	i64 a;
	read(name, a);
	if (error || null) return;
	b = (char)a;
}

void _rjson::read(std::string_view name, uint& b)
{
	u64 a;
	read(name, a);
	if (error || null) return;
	b = (uint)a;
}

void _rjson::read(std::string_view name, u64& b)
{
	if (!read_start(name)) return;
	char c = 0;
	file >> c;
	if (c == '0')
	{
		char c2 = file.get();
		if ((c2 == 'x') || (c2 == 'X'))
		{
			file >> std::hex >> b >> std::dec;
			if (!file.good()) error = 9;
			return;
		}
		file.unget();
	}
	file.unget();
	file >> b;
	if (!file.good()) error = 10;
}

void _rjson::read(std::string_view name, i64& b)
{
	if (!read_start(name)) return;
	file >> b;
	if (!file.good()) error = 14;
}

void _rjson::read(std::string_view name, double& b)
{
	if (!read_start(name)) return;
	file >> b;
	if (!file.good()) error = 15;
}

_interval _rjson::read_area(std::string_view name)
{
	_interval res;
	if (!obj(name)) return res;
	read("min", res.min);
	read("max", res.max);
	end();
	return res;
}

_area _rjson::read_area2(std::string_view name)
{
	_area res;
	if (!obj(name)) return res;
	res.x = read_area("x");
	res.y = read_area("y");
	end();
	return res;
}

void _rjson::read(std::string_view name, _xy& b)
{
	if (!obj(name)) return;
	read("x", b.x);
	read("y", b.y);
	end();
}

_trans _rjson::read_trans(std::string_view name)
{
	_trans res;
	if (!obj(name)) return res;
	read("scale", res.scale);
	read("offset", res.offset);
	end();
	return res;
}

bool string_to_mem(std::string_view s, void* d, i64 size)
{
	if (s.size() != size * 2ULL) return false;
	unsigned char* dd = (unsigned char*)d;
	for (u64 i = 0; i < s.size(); i += 2)
		*dd++ = (hex_to_byte[(uchar)s[i]] << 4) + hex_to_byte[(uchar)s[i + 1]];
	return true;
}

void _rjson::read(std::string_view name, _multi_string& b)
{
	b.line.clear();
	if (!arr(name)) return;
	while (!error)
	{
		astr s = read_string();
		if (null) break;
		b.line.push_back(string_to_wstring2(s));
	}
	end();
}

void _rjson::read(std::string_view name, _picture& b)
{
	std::vector<std::string> temp;
	if (!arr(name)) return;
	while (!error)
	{
		astr s = read_string();
		if (null) break;
		temp.push_back(s);
	}
	end();
	if (temp.size() == 0) { b.resize({ 0, 0 });	return; }
	if (temp.size() == 1)
		if (temp[0].size() == 144) // сжатие кнопок 24x24 (c_def, 0) в будущем сдалать универсальное сжатие картинок
		{
			b.resize({ 24, 24 });
			b.clear(c_pak0);
			uchar a[72];
			if (!string_to_mem(temp[0], a, 72))
			{
				error = 12;
				return;
			}
			for (i64 i = 0; i < 576; i++)
				if (a[i >> 3] & (1 << (i & 7)))
					b.data[i] = c_pak1;
			return;
		}
	int rx = (int)(temp[0].size() / 8);
	b.resize({ rx, (i64)temp.size() });
	for (int j = 0; j < temp.size(); j++)
		if (!string_to_mem(temp[j], &b.data[j * b.size.x], b.size.x * 4))
		{
			error = 12;
			return;
		}
	b.set_transparent();
}

void _rjson::read_mem(std::string_view name, void* b, u64 size)
{
	if (!read_start(name)) return;
	if (!string_to_mem(read_just_string(), b, (int)size)) error = 13;
}

void _rjson::read(std::string_view name, std::wstring& b)
{
	b = string_to_wstring2(read_string(name));
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void _matrix::operator=(_matrix&& a) noexcept
{
	if (data) delete[] data;
	data = a.data;
	size = a.size;
	a.data = nullptr;
	a.size = {};
}

void _matrix::operator=(const _matrix& a) noexcept
{
	resize(a.size);
	memcpy(data, a.data, size.square() * sizeof(double));
}

_matrix::_matrix(_matrix&& a) noexcept : data(a.data), size(a.size)
{
	a.data = nullptr;
	a.size = {};
}

_matrix::_matrix(const _matrix& a) noexcept : size(a.size)
{
	if (size.empty()) return;
	data = new double[size.square()];
	memcpy(data, a.data, size.square() * sizeof(double));
}

/*matrix matrix::operator<<(const matrix& a) const noexcept
{
	if (a.empty()) return *this;
	if (empty()) return a;
	if (ry_ != a.ry_) return matrix();
	matrix b(ry_, rx_ + a.rx_);
	double* rez = b.data_;
	double* m1 = data_;
	double* m2 = a.data_;
	for (int j = 0; j < ry_; j++)
	{
		for (int i = 0; i < rx_; i++, rez++, m1++) *rez = *m1;
		for (int i = 0; i < a.rx_; i++, rez++, m2++)*rez = *m2;
	}
	return b;
}*/

/*matrix matrix::transpose() const noexcept
{
	matrix b(rx_, ry_);
	double* rez = b.data_;
	for (size_t j = 0; j < rx_; j++)
	{
		double* ist = data_ + j;
		for (size_t i = 0; i < ry_; i++, rez++, ist += rx_)	rez = ist;
	}
	return b;
}*/

/*matrix matrix::this_mul_transpose() const noexcept
{
	matrix b(ry_, ry_);
	for (size_t j = 0; j < ry_; j++)
		for (size_t i = j; i < ry_; i++)
		{
			double s = 0;
			double* a1 = data_ + (j * rx_);
			double* a2 = data_ + (i * rx_);
			for (size_t k = 0; k < rx_; k++, a1++, a2++) s += (*a1) * (*a2);
			b[j][i] = b[i][j] = s;
		}
	return b;
}*/

/*matrix matrix::operator*(const matrix& a) const noexcept
{
	if (rx_ != a.ry_) return matrix();
	matrix b(ry_, a.rx_);
	for (size_t j = 0; j < b.ry_; j++)
		for (size_t i = 0; i < b.rx_; i++)
		{
			double s = 0;
			double* a1 = data_ + (j * rx_);
			double* a2 = a.data_ + i;
			for (size_t k = 0; k < rx_; k++)
			{
				s += (*a1) * (*a2);
				a1++;
				a2 += a.rx_;
			}
			b[j][i] = s;
		}
	return b;
}*/

/*matrix matrix::operator-(const matrix& a) const noexcept
{
	if ((rx_ != a.rx_) || (ry_ != a.ry_)) return matrix();
	matrix b(ry_, rx_);
	size_t n = rx_ * ry_;
	for (size_t i = 0; i < n; i++) b.data_[i] = data_[i] - a.data_[i];
	return b;
}*/

/*void matrix::operator+=(const matrix& a) noexcept
{
	if ((rx_ != a.rx_) || (ry_ != a.ry_)) return;
	size_t n = rx_ * ry_;
	for (size_t i = 0; i < n; i++) data_[i] += a.data_[i];
}*/

/*matrix::matrix(size_t ry, size_t rx, double z) noexcept : ry_(ry), rx_(rx)
{
	size_t n = ry_ * rx_;
	if (n == 0) return;
	data_ = new double[n];
	for (size_t i = 0; i < n; i++) data_[i] = z;
}*/

/*matrix::matrix(size_t ry, size_t rx, const std::function<double(size_t, size_t)>& fun) noexcept : ry_(ry), rx_(rx)
{
	if (ry_ * rx_ == 0) return;
	data_ = new double[ry_ * rx_];
	int n = 0;
	for (size_t j = 0; j < ry_; j++)
		for (size_t i = 0; i < rx_; i++, n++)
			data_[n] = fun(j, i);
}*/

_matrix::_matrix(i64 ry, const std::function<double(i64)>& fun)  noexcept : size{1, ry}
{
	if (ry == 0) return;
	data = new double[ry];
	for (i64 i = 0; i < ry; i++) data[i] = fun(i);
}

/*matrix::matrix(size_t ry) noexcept : ry_(ry), rx_(1)
{
	if (ry_) data_ = new double[ry_];
}*/

/*matrix::matrix(size_t ry, size_t rx)  noexcept : ry_(ry), rx_(rx)
{
	if (ry_ * rx_) data_ = new double[ry_ * rx_];
}*/

void _matrix::resize(_isize r)
{
	if (r.square() > size.square())
	{
		delete[] data;
		data = new double[r.square()];
	}
	size = r;
}

/*void matrix::MinMax(double* mi, double* ma)
{
	size_t vdata = rx_ * ry_;
	if (vdata == 0)
	{
		*mi = 1.0;
		*ma = 0;
		return;
	}
	double mii = data_[0];
	double maa = data_[0];
	for (int i = 1; i < vdata; i++)
	{
		if (data_[i] < mii) mii = data_[i];
		if (data_[i] > maa) maa = data_[i];
	}
	*mi = mii;
	*ma = maa;
}*/

/*void matrix::Push(_stack* mem)
{
	*mem << rx_ << ry_;
	mem->push_data(data_, sizeof(double) * rx_ * ry_);
}*/

/*void matrix::Pop(_stack* mem)
{
	size_t rx, ry;
	*mem >> rx >> ry;
	resize(ry, rx);
	mem->pop_data(data_, sizeof(double) * rx_ * ry_);
}*/

/*void matrix::set_diagonal_matrix(size_t n, double dz) noexcept
{
	resize(n, n);
	memset(data_, 0, n * n * sizeof(double));
	for (size_t i = 0; i < n; i++) data_[i * n + i] = dz;
}*/

/*void matrix::FindAllEigenVectors(matrix& R, matrix& A) const noexcept
{
	// Сборник научных программ на Фортране. Вып. 2. Матричная алгебра и линейная
	// алгебра. Нью-Йорк, 1960-1971, пер. с англ. (США). М., "Статистика", 1974.
	// 224 с.
	// стр - 187, подпрограмма EIGEN
	double ANORM, ANRMX, THR, X, Y, SINX, SINX2, COSX, COSX2, SINCS;
	constexpr double RANGE = 1.0E-13;                                     // было -13
//	constexpr double LIM = 1.0E-11;                                       // -6
	A = *this;
	size_t N = A.rx_;
	R.set_diagonal_matrix(N, 1.0);
	ANORM = 0;
	for (int j = 0; j < N; j++)
		for (int i = j + 1; i < N; i++)
			ANORM += A[j][i] * A[j][i];
	if (ANORM > 0)
	{
		ANORM = sqrt(2 * ANORM);
		ANRMX = ANORM * RANGE / N;
		THR = ANORM;
		do
		{
			THR = THR / N;
			bool ind;
			do
			{
				ind = false;
				for (size_t L = 0; L < N - 1; L++)
					for (size_t M = L + 1; M < N; M++)
						if (abs(A[L][M]) >= THR)
						{
							ind = true;
							X = 0.5 * (A[L][L] - A[M][M]);
							Y = -A[L][M] / sqrt(A[L][M] * A[L][M] + X * X);
							if (X < 0) Y = -Y;
							SINX = Y / sqrt(2 * (1 + sqrt(1 - Y * Y)));
							SINX2 = SINX * SINX;
							COSX = sqrt(1 - SINX2);
							COSX2 = COSX * COSX;
							SINCS = SINX * COSX;
							for (size_t i = 0; i < N; i++)
							{
								if ((i != L) && (i != M))
								{
									X = A[i][L] * COSX - A[i][M] * SINX;
									A[M][i] = A[i][M] = A[i][L] * SINX + A[i][M] * COSX;
									A[L][i] = A[i][L] = X;
								}
								X = R[i][L] * COSX - R[i][M] * SINX;
								R[i][M] = R[i][L] * SINX + R[i][M] * COSX;
								R[i][L] = X;
							}
							X = 2 * A[L][M] * SINCS;
							Y = A[L][L] * COSX2 + A[M][M] * SINX2 - X;
							X = A[L][L] * SINX2 + A[M][M] * COSX2 + X;
							A[M][L] = A[L][M] = (A[L][L] - A[M][M]) * SINCS + A[L][M] * (COSX2 - SINX2);
							A[L][L] = Y;
							A[M][M] = X;
						}
			} while (ind);
		} while (THR > ANRMX);
	}
		//for (int i = 0; i < N; i++)
		//	for (int j = i + 1; j < N; j++)
		//		if (A[i][i] < A[j][j])
		//		{
		//			X = A[i][i];
		//			A[i][i] = A[j][j];
		//			A[j][j] = X;
		//			R.PerestCol(i, j);
		//		}

		//// конец подпрограммы EIGEN

		//for (int j = 0; j < N; j++)
		//	for (int i = 0; i < N; i++)
		//		if (i != j) A[j][i] = 0;
		//double SumD = Abs().shpoor();//   /N
		//double SumL = abs(A[0][0]);
		//for (int i = 1; i < N; i++)
		//{
		//	SumL += abs(A[i][i]);
		//	//    if ((::Abs(A[i][i]/SumD)) < 1.0E-8)
		//	if (abs(1.0 - SumL / SumD) < LIM)
		//		//    if (A[i][i] <= 0)
		//	{
		//		Vl = i - 1;
		//		A.ry = i - 1;
		//		A.rx = i - 1;
		//		R.rx = i - 1;
		//		return;
		//	}
		//}
		//Vl = rx_;
}*/

//вычисление псевдообратной матрицы
/*matrix matrix::pseudoinverse() const noexcept
{
	double LIM = 1.0E-13;                                       // min -10 max -15  было -11 -13
	matrix BQ, BL;
	FindAllEigenVectors(BQ, BL);
	double SumD = 0;
	for (size_t i = 0; i < ry_; i++)
		if (BL[i][i] > SumD) SumD = BL[i][i];
	LIM *= SumD;
	matrix Aplus(ry_, ry_, 0);
	for (size_t j = 0; j < ry_; j++)
	{
		double lam = BL[j][j];
		if (lam < LIM) continue;
		lam = 1.0 / lam;
		double* rez = Aplus.data_;
		double* ist = BQ.data_ + j;
		for (size_t y = 0; y < ry_; y++)
		{
			double mn = lam * ist[y * ry_];
			for (size_t x = 0; x < ry_; x++, rez++)
				*rez += mn * ist[x * ry_];
		}
	}
	return Aplus;
}*/

/*double matrix::linear_prediction(const matrix& k) const noexcept
{
	size_t rk = k.size();
	size_t r = size();
	if (r * rk == 0) return 0.0;
	if (rk > r) return data_[r - 1];
	r -= rk;
	double s = 0.0;
	for (size_t ii = 0; ii < rk; ii++) s += k.data_[ii] * data_[ii + r];
	return s;
}*/

/*matrix matrix::linear_prediction(const matrix& k, size_t start, size_t n, size_t ots) const noexcept
{
	matrix b(n, 1, 0);
	if (((k.rx_ != 1) && (k.ry_ != 1)) || ((rx_ != 1) && (ry_ != 1))) return b; // должен быть вектор
	size_t rk = k.size();
	size_t r = size();
	if ((start < rk) || (start > r)) return b;
	for (size_t i = 0; i < n; i++)
	{
		double s = 0;
		for (size_t ii = 0; ii < rk; ii++)
		{
			__int64 p = (__int64)ii - (__int64)rk + (__int64)i - (__int64)ots;
			s += k.data_[ii] * ((p >= 0) ? b.data_[p] : data_[start + p]);
		}
		b.data_[i] = s;
	}
	return b;
}*/

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/*double MatrixColumn::Delta(size_t y)
{
	if ((x_ < 0) || (x_ >= m_->rx_) || (y < 1) || (y >= m_->ry_)) return 0.0;
	return m_->data_[y * (m_->rx_) + x_] - m_->data_[(y - 1) * (m_->rx_) + x_];
}*/

bool matrix_column::min_max(double* mi, double* ma)
{
	if ((x < 0) || (x >= m->size.x) || (m->size.y < 1)) return false;
	double* da = m->data;
	double mii = da[x];
	double maa = da[x];
	i64 vdata = m->size.square();
	for (i64 i = x + m->size.x; i < vdata; i += m->size.x)
	{
		if (da[i] < mii) mii = da[i];
		if (da[i] > maa) maa = da[i];
	}
	*mi = mii;
	*ma = maa;
	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/*matrix get_noise(size_t n, double k)
{

	matrix b(n);
	for (size_t i = 0; i < n; i++)
		b[0][i] = (((double)rnd() / (double)(unsigned __int64)0xFFFFFFFFFFFFFFFF) - 0.5) * k;
	return b;
}*/

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
