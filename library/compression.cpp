#include <map>

#include "compression.h"

constexpr u64 h0 = 0; // ������
constexpr u64 h1 = 0x40000000; // ��������
constexpr u64 h2 = 0x80000000; // ��������
constexpr u64 h3 = 0xc0000000; // 3/4
constexpr u64 h4 = 0x100000000; // ������

struct _ppc
{
	u64 frequency = 0;
	std::map<uchar, _ppc> next;
};

constexpr double kkk4 = 19.0; // 19.0

struct _frequency
{
	double frequency[256];

	void start() noexcept { for (int i = 0; i < 256; i++) frequency[i] = kkk4 / 256.0; }
	void norm();
	void norm(u64 rr, u64* f);
};

void _frequency::norm(u64 rr, u64* f)
{
	double s = 0;
	for (int i = 0; i < 256; i++) s += frequency[i];
	s = rr / s;
	f[0] = 0;
	for (int i = 1; i <= 256; i++)
	{
		u64 a = (u64)(frequency[i - 1] * s);
		if (a == 0) a = 1;
		f[i] = f[i - 1] + a;
	}
}

void _frequency::norm()
{
	double s = 0;
	for (int i = 0; i < 256; i++) s += frequency[i];
	s = kkk4 / s;
	for (int i = 0; i < 256; i++) frequency[i] *= s;
}

uchar ppm(const std::vector<uchar>& data, std::vector<uchar>& res, u64 g)
{
	res.clear();
	if (data.empty()) return 0;


	u64 bad_bit = 0; // ���������� ������ ���
	uchar bit_size = 0;
	u64 s2 = data.size();
	while (s2) { bit_size++; s2 >>= 1; }
	uchar r1 = bit_size - 1; // �������� ����
	uchar r1n = 6; // ���������� ��� � �������� �����
	auto otgruz = [&](uchar bit)
	{
		r1 = (r1 << 1ui8) + bit;
		if (++r1n == 8)
		{
			res.push_back(r1);
			r1 = r1n = 0;
		}
	};
	for (uchar ii = 0; ii < bit_size; ii++) otgruz((data.size() >> ii) & 1);
	u64 frequency[257]; // ��������� �����
	for (u64 ii = 0; ii < 257; ii++) frequency[ii] = ii;
	u64 begin = h0; // ������ �������� ���������
	u64 end = h4; // ����� �������� ���������


	_ppc ppc;
	_frequency f;
	for (u64 i = 0; i < data.size(); i++)
	{
		u64 c = data[i]; // �������� ������



		f.start();
		for (auto& jj : ppc.next) f.frequency[jj.first] += jj.second.frequency;
		_ppc* ppc2 = &ppc;
		for (u64 j = 1; j <= g; j++)
		{
			if (j > i) break;
			f.norm();
			ppc2 = &ppc2->next[data[i - j]];
			for (auto& jj : ppc2->next) f.frequency[jj.first] += jj.second.frequency;
		}
		//		f.norm();
		f.norm(1073741000, frequency);

		ppc.frequency++;
		ppc.next[(uchar)c].frequency++;
		ppc2 = &ppc;
		for (u64 j = 1; j <= g; j++)
		{
			if (j > i) break;
			ppc2 = &ppc2->next[data[i - j]];
			ppc2->next[(uchar)c].frequency++;
		}



		u64 ed = end - begin;
		end = begin + ed * frequency[c + 1] / frequency[256];
		begin += ed * frequency[c] / frequency[256];
	start:
		if (end <= h2)
		{
			otgruz(0);
			while (bad_bit) { otgruz(1); bad_bit--; }
			begin <<= 1;
			end <<= 1;
			goto start;
		}
		if (begin >= h2)
		{
			otgruz(1);
			while (bad_bit) { otgruz(0); bad_bit--; }
			begin = (begin - h2) << 1;
			end = (end - h2) << 1;
			goto start;
		}
		if ((begin >= h1) && (end <= h3))
		{
			begin = (begin - h1) << 1;
			end = (end - h1) << 1;
			bad_bit++;
			goto start;
		}
		for (u64 i = c + 1; i < 257; i++) frequency[i]++;
	}
	uchar c = ((begin <= h1) && (end >= h2));
	otgruz(c ^ 1);
	otgruz(c);
	while (bad_bit) { otgruz(c); bad_bit--; }
	if (r1n) res.push_back(r1 << (8 - r1n));
	return r1n;
}

_bit_vector arithmetic_coding(const std::vector<uchar>& data)
{
	_bit_vector res;
	u64 bit_size = bit_for_value(data.size() + 1);
	res.pushn(bit_size, 6);
	res.pushn(data.size(), bit_size);
	u64 frequency[256]; // �������
	for (auto& i: frequency) i = 1;
	u64 summ_frequency = 256;
	u64 begin = h0, end = h4; // ������� ��������
	u64 bad_bit = 0; // ���������� ������ ���
	for (u64 c : data)
	{
		u64 chs = 0;
		for (u64 i = 0; i < c; i++) chs += frequency[i];
		u64 ed = end - begin;
		end = begin + ed * (chs + frequency[c]++) / summ_frequency;
		begin += ed * chs / summ_frequency++;
		for (;;)
		{
			if ((end <= h2) || (begin >= h2))
			{
				u64 bi = (begin >= h2);
				res.push1(bi);
				if (bad_bit) { res.pushnod(bi ^ 1, bad_bit); bad_bit = 0; }
				begin = (begin - h2 * bi) << 1;
				end = (end - h2 * bi) << 1;
				continue;
			}
			if ((begin < h1) || (end > h3)) break;
			begin = (begin - h1) << 1;
			end = (end - h1) << 1;
			bad_bit++;
		}
	}
	u64 c = ((begin <= h1) && (end >= h2));
	res.push1(c ^ 1);
	res.pushnod(c, bad_bit + 1);
	return res;
}

void arithmetic_decoding(_bit_vector& data, std::vector<uchar>& res)
{
	res.clear();
	if (data.empty()) return;
	uchar bit_size = data.popn(6);
	u64 raz = data.popn(bit_size);
	res.reserve(raz);

	u64 frequency[257]; // ��������� �����
	for (u64 i = 0; i < 257; i++) frequency[i] = i;

	u64 range[257]; // ��������� ��������� ����
	bool irange[257] = {}; // ��������� �� �������� ��������� ����?

	u64 begin = h0; // ������ �������� ���������
	u64 end = h4; // ����� �������� ���������

	u64 beging = h0; // ������ �������� ���������
	u64 endg = h4; // ����� �������� ���������

	u64 gl = h4; // �������� �������/������

	u64 c0 = 0; // ����� ������� �������������� �������
	u64 c1 = 256; // ������ ������� �������������� �������

	int iii = 0;
	bool recalc = false;

	for (;;)
	{
		uchar bit = data.pop1();
		gl >>= 1;
		if (bit) begin += gl; else end -= gl;

		while (true)
		{
			if (bit || recalc)
			{
				u64 c0_0 = c0;
				u64 c0_1 = c1 - 1;
				while (c0_1 > c0_0)
				{
					u64 cc = (c0_0 + c0_1 + 1) >> 1;
					if (!irange[cc])
					{
						range[cc] = beging + (endg - beging) * frequency[cc] / frequency[256];
						irange[cc] = true;
					}
					if (begin >= range[cc]) c0_0 = cc; else c0_1 = cc - 1; // ??
				}
				c0 = c0_0;
			}
			if ((!bit) || recalc)
			{
				u64 c1_0 = c0 + 1;
				u64 c1_1 = c1;
				while (c1_1 > c1_0)
				{
					u64 cc = (c1_0 + c1_1) >> 1;
					if (!irange[cc])
					{
						range[cc] = beging + (endg - beging) * frequency[cc] / frequency[256];
						irange[cc] = true;
					}
					if (end <= range[cc]) c1_1 = cc; else c1_0 = cc + 1; // ??
				}
				c1 = c1_0;
			}
			recalc = false;
			if (c1 - c0 > 1) break;
			recalc = true;
			res.push_back((uchar)c0);
			iii++;
			if (iii == 147289)
			{
				size_t rr = res.size();
				iii++;
			}
			if (res.size() == raz) return;
			if (!irange[c0]) range[c0] = beging + (endg - beging) * frequency[c0] / frequency[256];
			if (!irange[c1]) range[c1] = beging + (endg - beging) * frequency[c1] / frequency[256];
			beging = range[c0];
			endg = range[c1];
			for (u64 i = 0; i < 257; i++) irange[i] = false;
			for (u64 i = c1; i < 257; i++) frequency[i]++;
			c0 = 0;
			c1 = 256;
		start:
			if (endg <= h2)
			{
				beging <<= 1;
				endg <<= 1;
				begin <<= 1;
				end <<= 1;
				gl <<= 1;
				goto start;
			}
			if (beging >= h2)
			{
				beging = (beging - h2) << 1;
				endg = (endg - h2) << 1;
				begin = (begin - h2) << 1;
				end = (end - h2) << 1;
				gl <<= 1;
				goto start;
			}
			if ((beging >= h1) && (endg <= h3))
			{
				beging = (beging - h1) << 1;
				endg = (endg - h1) << 1;
				begin = (begin - h1) << 1;
				end = (end - h1) << 1;
				gl <<= 1;
				goto start;
			}
		}
	}
}

void arithmetic_decoding2(_bit_vector& data, std::vector<uchar>& res)
{
	res.clear();
	if (data.empty()) return;
	uchar bit_size = data.popn(6);
	u64 raz = data.popn(bit_size);
	res.reserve(raz);

	u64 frequency[257]; // ��������� �����
	for (u64 i = 0; i < 257; i++) frequency[i] = i;

	u64 range[257]; // ��������� ��������� ����
	bool irange[257] = {}; // ��������� �� �������� ��������� ����?

	u64 begin = h0; // ������ �������� ���������
	u64 end = h4; // ����� �������� ���������

	u64 beging = h0; // ������ �������� ���������
	u64 endg = h4; // ����� �������� ���������

	u64 gl = h4; // �������� �������/������

	u64 c0 = 0; // ����� ������� �������������� �������
	u64 c1 = 256; // ������ ������� �������������� �������

	int iii = 0;
	bool recalc = false;

	for (;;)
	{
		uchar bit = data.pop1();
		gl >>= 1;
		if (bit) begin += gl; else end -= gl;

		while (true)
		{
			if (bit || recalc)
			{
				u64 c0_0 = c0;
				u64 c0_1 = c1 - 1;
				while (c0_1 > c0_0)
				{
					u64 cc = (c0_0 + c0_1 + 1) >> 1;
					if (!irange[cc])
					{
						range[cc] = beging + (endg - beging) * frequency[cc] / frequency[256];
						irange[cc] = true;
					}
					if (begin >= range[cc]) c0_0 = cc; else c0_1 = cc - 1; // ??
				}
				c0 = c0_0;
			}
			if ((!bit) || recalc)
			{
				u64 c1_0 = c0 + 1;
				u64 c1_1 = c1;
				while (c1_1 > c1_0)
				{
					u64 cc = (c1_0 + c1_1) >> 1;
					if (!irange[cc])
					{
						range[cc] = beging + (endg - beging) * frequency[cc] / frequency[256];
						irange[cc] = true;
					}
					if (end <= range[cc]) c1_1 = cc; else c1_0 = cc + 1; // ??
				}
				c1 = c1_0;
			}
			recalc = false;
			if (c1 - c0 > 1) break;
			recalc = true;
			res.push_back((uchar)c0);
			iii++;
			if (iii == 147289)
			{
				size_t rr = res.size();
				iii++;
			}
			if (res.size() == raz) return;
			if (!irange[c0]) range[c0] = beging + (endg - beging) * frequency[c0] / frequency[256];
			if (!irange[c1]) range[c1] = beging + (endg - beging) * frequency[c1] / frequency[256];
			beging = range[c0];
			endg = range[c1];
			for (u64 i = 0; i < 257; i++) irange[i] = false;
			for (u64 i = c1; i < 257; i++) frequency[i]++;
			c0 = 0;
			c1 = 256;
		start:
			if (endg <= h2)
			{
				beging <<= 1;
				endg <<= 1;
				begin <<= 1;
				end <<= 1;
				gl <<= 1;
				goto start;
			}
			if (beging >= h2)
			{
				beging = (beging - h2) << 1;
				endg = (endg - h2) << 1;
				begin = (begin - h2) << 1;
				end = (end - h2) << 1;
				gl <<= 1;
				goto start;
			}
			if ((beging >= h1) && (endg <= h3))
			{
				beging = (beging - h1) << 1;
				endg = (endg - h1) << 1;
				begin = (begin - h1) << 1;
				end = (end - h1) << 1;
				gl <<= 1;
				goto start;
			}
		}
	}
}

void AC_pak64(const std::vector<uchar>& data, std::vector<uchar>& res)
{
	u64 tbit = 0;
	i64 L = data.size();
	res.resize(L + 666);
	u64 dv[257], vdv = 257;
	for (i64 i = 0; i < 257; i++) dv[i] = 1;
	u64 n = 0, dn = 0x80000000;
	i64 BrakBit = 0;
	for (i64 i = 0; i <= L; i++)
	{
		i64 c = (i < L) ? data[i] : 256;
		u64 v = 0;
		for (i64 j = 0; j < c; j++) v += dv[j];
		n = n + (u64)dn * v / vdv;
		dn = (u64)dn * dv[c] / vdv;
		for (;;)
		{
			if (n + dn <= 0x40000000)
			{
				i64 no = (tbit >> 3);
				if ((tbit & 7) == 0) res[no] = 0;
				else res[no] <<= 1;
				tbit++;
				while (BrakBit--)
				{
					no = (tbit >> 3);
					if ((tbit & 7) == 0) res[no] = 1;
					else
					{
						res[no] <<= 1;
						res[no]++;
					}
					tbit++;
				}
				BrakBit = 0;
			}
			else
				if (n >= 0x40000000)
				{
					n -= 0x40000000;
					i64 no = (tbit >> 3);
					if ((tbit & 7) == 0) res[no] = 1;
					else
					{
						res[no] <<= 1;
						res[no]++;
					}
					tbit++;
					while (BrakBit--)
					{
						no = (tbit >> 3);
						if ((tbit & 7) == 0) res[no] = 0;
						else res[no] <<= 1;
						tbit++;
					}
					BrakBit = 0;
				}
				else
					if ((n >= 0x20000000) && (n + dn <= 0x60000000))
					{
						BrakBit++;
						n -= 0x20000000;
					}
					else break;
			n <<= 1;
			dn <<= 1;
		}
		dv[c]++;
		vdv++;
	}
	BrakBit++;
	if (n < 0x20000000)
	{
		i64 no = (tbit >> 3);
		if ((tbit & 7) == 0) res[no] = 0;
		else res[no] <<= 1;
		tbit++;
		while (BrakBit--)
		{
			no = (tbit >> 3);
			if ((tbit & 7) == 0) res[no] = 1;
			else
			{
				res[no] <<= 1;
				res[no]++;
			}
			tbit++;
		}
	}
	else
	{
		i64 no = (tbit >> 3);
		if ((tbit & 7) == 0) res[no] = 1;
		else
		{
			res[no] <<= 1;
			res[no]++;
		}
		tbit++;
		while (BrakBit--)
		{
			no = (tbit >> 3);
			if ((tbit & 7) == 0) res[no] = 0;
			else res[no] <<= 1;
			tbit++;
		}
	}
	if (tbit & 7) res[(tbit >> 3)] <<= 8 - (tbit & 7);
	res.resize(((tbit - 1) >> 3) + 1);
}

std::vector<unsigned char> AC_unpak64(std::vector<unsigned char>& A)
{
	std::vector<unsigned char> Re;
	u64 tbit = 0;
	i64 L = A.size();
	Re.resize(L + 666);
	i64 zp = 0;
	u64 dv[257], vdv = 257;
	for (i64 i = 0; i < 257; i++) dv[i] = 1;
	u64 n = 0, dn = 0x80000000, x = 0;
	for (i64 i = 1; i <= 31; i++)
	{
		x <<= 1;
		i64 no = (tbit >> 3) + 1;
		if (no <= L)
			x += ((unsigned char)A[no - 1] >> (7 - (tbit & 7))) & 1;
		else
			if (no - L > 5) return {};
		tbit++;
	}
	for (;;)
	{
		u64 y;
		y = ((unsigned long long)(x - n + 1) * vdv - 1) / dn;
		i64 c;
		u64 v = 0;
		for (c = 0; y >= (v += dv[c]); c++);
		v -= dv[c];
		n = n + (unsigned long long)dn * v / vdv;
		dn = (unsigned long long)dn * dv[c] / vdv;
		for (;;)
		{
			if (n + dn <= 0x40000000)
			{
			}
			else
				if (n >= 0x40000000)
				{
					n -= 0x40000000;
					x -= 0x40000000;
				}
				else
					if ((n >= 0x20000000) && (n + dn <= 0x60000000))
					{
						n -= 0x20000000;
						x -= 0x20000000;
					}
					else break;
			n <<= 1;
			dn <<= 1;
			x <<= 1;
			i64 no = (tbit >> 3) + 1;
			if (no <= L)
				x += (A[no - 1] >> (7 - (tbit & 7))) & 1;
			else
				if (no - L > 5) return {};
			tbit++;
		}
		if (c == 256) break;
		zp++;
		if (zp > Re.size()) Re.resize(Re.size() * 2);
		Re[zp - 1] = c;
		dv[c]++;
		vdv++;
	}
	Re.resize(zp);
	return Re;
}