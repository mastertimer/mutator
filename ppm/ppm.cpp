#include "ui.h"
#include "compression.h"
#include "e_terminal.h"
#include <iostream>
#include <chrono>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

std::wstring test_file = L"e:/mutator/ppm/data/t110521.txt";

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct _ppm_mode : public _ui
{
private:

	virtual bool start2() override;
	void init_ui_elements();
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace
{

bool start_unit()
{
	main_modes[L"ppm"] = std::make_unique<_ppm_mode>();
	return true;
}
auto ignore = start_unit();

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

std::string command_decomposition(const std::string& cmd, std::vector<std::string>& parameters)
{
    std::string command_name;
	i64 i0 = cmd.size();
	// пропуск первых пробелов
	for (i64 i = 0; i < (i64)cmd.size(); i++)
		if ((cmd[i] != ' ') && (cmd[i] != '\t'))
		{
			i0 = i;
			break;
		}
	// вычленение command_name
	for (i64 i = i0; i < (i64)cmd.size(); i++)
		if ((cmd[i] == ' ') || (cmd[i] == '\t'))
		{
			command_name = cmd.substr(i0, i - i0);
			break;
		}
	if (command_name.empty()) command_name = cmd.substr(i0, cmd.size() - i0);
	// вычленение параметров
	int rez = 0; // 0 - пробелы 1 - набор символов 2 - строка "safasf asf"
	i64 start_p = 0;
	for (i64 i = i0 + command_name.size(); i < (i64)cmd.size(); i++)
	{
		wchar_t c = cmd[i];
		if (rez == 0)
		{
			if ((c == ' ') || (c == '\t')) continue;
			start_p = i;
			rez = (c == '"') ? 2 : 1;
			continue;
		}
		if (rez == 1) if ((c != ' ') && (c != '\t')) continue;
		if (rez == 2) if (c != '"') continue;
		parameters.push_back(cmd.substr(start_p, i - start_p + (rez == 2)));
		rez = 0;
	}
	if (rez == 1) parameters.push_back(cmd.substr(start_p, cmd.size() - start_p));
	return command_name;
}

void test_arithmetic_coding(_e_terminal& trm, const std::vector<std::wstring>& parameters)
{
	trm.print(L"файл: " + test_file);
	std::vector<uchar> data, data2;
	_bit_vector res;
	if (!load_file(test_file, data))
	{
		trm.print(L"ошибка загрузки!");
		return;
	}
	trm.print(L"размер:            " + std::to_wstring(data.size()));
	double com1 = 0.0, com2 = 0.0;
	double com = information(data, &com1, &com2);
	trm.print(L"информация:        " + double_to_wstring(com, 1) + L" (" + double_to_wstring(com1, 1) + L" + " +
		double_to_wstring(com2, 1) + L")");
	trm.print(L"идеал:             " + double_to_wstring(size_arithmetic_coding(data), 1));
	trm.print(L"идеал0:            " + double_to_wstring(size_arithmetic_coding(data, 0.01), 1));

	i64 n = 1;
	if (!parameters.empty()) n = std::stoi(parameters[0]);
	if (n < 1) n = 1;
	if (n > 1) trm.print(std::to_wstring(n) + L" испытаний");

	i64 mindt = 1000000000;
	i64 maxdt = 0;
	i64 summdt = 0;

	for (i64 i = 0; i < n; i++)
	{
		auto tt = std::chrono::high_resolution_clock::now();
		arithmetic_coding(data, res);
		std::chrono::nanoseconds dt = std::chrono::high_resolution_clock::now() - tt;
		i64 dtt = dt.count() / 1000;
		if (dtt < mindt) mindt = dtt;
		if (dtt > maxdt) maxdt = dtt;
		summdt += dtt;
	}


	double v = res.size() / 8.0;
	trm.print(L"arithmetic_coding: " + double_to_wstring(v, 1));
	trm.print(L"разница:           " + double_to_wstring(v - com, 1));
	trm.print(L"среднее время, мксек:      " + std::to_wstring(summdt / n));
	trm.print(L"минимальное время, мксек:  " + std::to_wstring(mindt));
	trm.print(L"максимальное время, мксек: " + std::to_wstring(maxdt));

	mindt = 1000000000;
	maxdt = 0;
	summdt = 0;

	for (i64 i = 0; i < n; i++)
	{
		auto tt = std::chrono::high_resolution_clock::now();
		arithmetic_decoding(res, data2);
		std::chrono::nanoseconds dt = std::chrono::high_resolution_clock::now() - tt;
		i64 dtt = dt.count() / 1000;
		if (dtt < mindt) mindt = dtt;
		if (dtt > maxdt) maxdt = dtt;
		summdt += dtt;
	}
	if (data == data2)
		trm.print(L"расжатие норма");
	else
		trm.print(L"!!ошибка!! расжатый файл не равен исходному!");
	trm.print(L"среднее время, мксек:      " + std::to_wstring(summdt / n));
	trm.print(L"минимальное время, мксек:  " + std::to_wstring(mindt));
	trm.print(L"максимальное время, мксек: " + std::to_wstring(maxdt));
}

double f1(double x)
{
	static int n = 1;
	static std::vector<double> k1 = { 0.2, 0.8 };
	return delta_entropy(n, x, k1);
}

std::pair<double, double> find_k1()
{
	int n = 1;
	std::vector<double> k1 = { 0.5, 0.5 };
	double delta = 0.0001;
	auto ff = [&](double x) { return delta_entropy(n, x, k1); };
	auto x_m = 0.5;
	auto m_m = max_of_function(ff, 0, 1);
	for (;;)
	{
		double x = x_m + delta;
		k1[0] = 1.0 - x;
		k1[1] = x;
		auto m = max_of_function(ff, 0, 1);
		if (m < m_m)
		{
			m_m = m;
			x_m = x;
			continue;
		}
		x = x_m - delta;
		k1[0] = 1.0 - x;
		k1[1] = x;
		m = max_of_function(ff, 0, 1);
		if (m < m_m)
		{
			m_m = m;
			x_m = x;
			continue;
		}
		break;
	}
	return { x_m,m_m };
}

std::pair<double, double> find_k2()
{
	int n = 2;
	std::vector<double> k1 = { 0.5, 0.5, 0.5 };
	double delta = 0.0001;
	auto ff = [&](double x) { return delta_entropy(n, x, k1); };
	auto x_m = 0.5;
	auto m_m = max_of_function(ff, 0, 1);
	for (;;)
	{
		double x = x_m + delta;
		k1[0] = 1.0 - x;
		k1[2] = x;
		auto m = max_of_function(ff, 0, 1);
		if (m < m_m)
		{
			m_m = m;
			x_m = x;
			continue;
		}
		x = x_m - delta;
		k1[0] = 1.0 - x;
		k1[2] = x;
		m = max_of_function(ff, 0, 1);
		if (m < m_m)
		{
			m_m = m;
			x_m = x;
			continue;
		}
		break;
	}
	return { x_m,m_m };
}

std::pair<double, double> find_k3()
{
	int n = 3;
	auto x1_m = 0.5;
	auto x2_m = 0.5;
	std::vector<double> k1 = { 1.0 - x2_m, 1.0 - x1_m, x1_m, x2_m };
	double delta = 0.0001;
	auto ff = [&](double x) { return delta_entropy(n, x, k1); };
	auto m_m = max_of_function(ff, 0, 1);
	for (double x1 = 0.5; x1 < 1.0; x1 += delta)
		for (double x2 = x1; x2 < 1.0; x2 += delta)
		{
			k1[0] = 1.0 - x2;
			k1[1] = 1.0 - x1;
			k1[2] = x1;
			k1[3] = x2;
			auto m = max_of_function(ff, 0, 1);
			if (m < m_m)
			{
				m_m = m;
				x1_m = x1;
				x2_m = x2;
			}
		}
	return { x1_m,x2_m };
}

void test_test(_e_terminal& trm, const std::vector<std::wstring>& parameters)
{
	auto k = find_k3();
	trm.print(std::to_wstring(k.first));
	trm.print(std::to_wstring(k.second));
}

void test_ppm(_e_terminal& trm, const std::vector<std::wstring>& parameters)
{
	trm.print(L"файл: " + test_file);
	std::vector<uchar> data, data2, res;
	if (!load_file(test_file, data))
	{
		trm.print(L"ошибка загрузки!");
		return;
	}
	trm.print(L"размер: " + std::to_wstring(data.size()));

	i64 n = 0;
	if (parameters.size() >= 1)
	{
		n = std::stoi(parameters[0]);
		if (n < 0) n = 0;
	}
	trm.print(L"порядок = " + std::to_wstring(n));

	i64 mindt = 1000000000;
	i64 maxdt = 0;
	i64 summdt = 0;

	uchar db = 0;
	double db2 = 0;
	{
		auto tt = std::chrono::high_resolution_clock::now();
//		db = ppm2(data, res, n);
		db = ppm(data, res, n);// в начале биты используемых символов
		db2 = ppm_test(data, n);
		std::chrono::nanoseconds dt = std::chrono::high_resolution_clock::now() - tt;
		i64 dtt = dt.count() / 1000;
		if (dtt < mindt) mindt = dtt;
		if (dtt > maxdt) maxdt = dtt;
		summdt += dtt;
	}
	double v = double(res.size());
	if (db > 0)
	{
		v -= 1;
		v += db * 0.1;
	}
	trm.print(L"время, мксек:  " + std::to_wstring(mindt));
	trm.print(L"ppm:           " + double_to_wstring(v, 1));
	trm.print(L"ppm_test:      " + double_to_wstring(db2, 1));
}

void eee(std::vector<std::vector<i64>>& res, std::vector<i64> tek, i64 sum, i64 maxx)
{
	if (sum == 0)
	{
		res.push_back(tek);
		return;
	}
	for (i64 i = std::min(sum, maxx); i >= 1; i--)
	{
		tek.push_back(i);
		eee(res, tek, sum - i, i);
		tek.pop_back();
	}
}

void razl(_e_terminal& trm, const std::vector<std::wstring>& parameters)
{
	i64 n = 1;
	if (parameters.size() >= 1)
	{
		n = std::stoi(parameters[0]);
		if (n < 1) n = 1;
	}
	trm.print(L"разложение числа: " + std::to_wstring(n));
	std::vector<std::vector<i64>> res;
	eee(res, {}, n, n);
	for (auto& i : res)
	{
		std::wstring s;
		for (auto& j : i) s += std::to_wstring(j) + L" ";
		trm.print(s);
	}
	trm.print(L"всего разложений: " + std::to_wstring(res.size()));
}

bool _ppm_mode::start2()
{
	static bool first_run = true;
	if (first_run) first_run = false; else return true;

	init_ui_elements();
	return true;
}

void _ppm_mode::init_ui_elements()
{
	auto term = std::make_shared<_e_terminal>(this);
	term->local_area.x = _interval(10, 480);
	term->local_area.y = _interval(10, 740);
	n_ko->add_child(term);
	n_act_key = term;
	term->command.insert({ L"a", { L"тест арифметического кодирования", test_arithmetic_coding } });
	term->command.insert({ L"b", { L"test_test", test_test } });
	term->command.insert({ L"ppm", { L"test ppm", test_ppm } });
	term->command.insert({ L"r", { L"разложение числа", razl } });
}