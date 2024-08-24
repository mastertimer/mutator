#include <iostream>
#include <chrono>

#include "compression.h"

std::wstring test_file = L"e:\\programs\\ppm\\data\\t110521.txt";
std::wstring test_file2 = L"data/t110521.txt";

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
	int start_p = 0;
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

void test_arithmetic_coding(std::vector<std::string>& parameters)
{
	std::wcout << L"файл: " << test_file << std::endl;
	std::vector<uchar> data, data2;
	_bit_vector res;
	if (!load_file(test_file, data))
	{
		std::wcout << L"ошибка загрузки!" << std::endl;
		return;
	}
	std::wcout << L"размер:            " << std::to_wstring(data.size()) << std::endl;
	double com1 = 0.0, com2 = 0.0;
	double com = information(data, &com1, &com2);
	std::wcout << L"информация:        " << double_to_wstring(com, 1) << L" (" + double_to_wstring(com1, 1) << L" + " <<
		double_to_wstring(com2, 1) + L")" << std::endl;
	std::wcout << L"идеал:             " << double_to_wstring(size_arithmetic_coding(data), 1) << std::endl;
	std::wcout << L"идеал0:            " + double_to_wstring(size_arithmetic_coding(data, 0.01), 1) << std::endl;

	i64 n = 1;
	if (!parameters.empty()) n = std::stoi(parameters[0]);
	if (n < 1) n = 1;
	if (n > 1) std::wcout << std::to_wstring(n) << L" испытаний" << std::endl;

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
	std::wcout << L"arithmetic_coding: " << double_to_wstring(v, 1) << std::endl;
	std::wcout << L"разница:           " << double_to_wstring(v - com, 1) << std::endl;
	std::wcout << L"среднее время, мксек:      " << std::to_wstring(summdt / n) << std::endl;
	std::wcout << L"минимальное время, мксек:  " << std::to_wstring(mindt) << std::endl;
	std::wcout << L"максимальное время, мксек: " << std::to_wstring(maxdt) << std::endl;

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
		std::wcout << L"расжатие норма" << std::endl;
	else
		std::wcout << L"!!ошибка!! расжатый файл не равен исходному!" << std::endl;
	std::wcout << L"среднее время, мксек:      " << std::to_wstring(summdt / n) << std::endl;
	std::wcout << L"минимальное время, мксек:  " << std::to_wstring(mindt) << std::endl;
	std::wcout << L"максимальное время, мксек: " << std::to_wstring(maxdt) << std::endl;
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

void test_test(std::vector<std::string>& parameters)
{
	auto k = find_k3();
	std::wcout << k.first << std::endl;
	std::wcout << k.second << std::endl;
}

void test_ppm(std::vector<std::string>& parameters)
{
	std::wcout << L"файл: " << test_file << std::endl;
	std::vector<uchar> data, data2, res;
	if (!load_file(test_file, data))
	{
		std::wcout << L"ошибка загрузки!" << std::endl;
		return;
	}
	std::wcout << L"размер: " << std::to_wstring(data.size()) << std::endl;

	i64 n = 0;
	if (parameters.size() >= 1)
	{
		n = std::stoi(parameters[0]);
		if (n < 0) n = 0;
	}
	std::wcout << L"порядок = " << std::to_wstring(n) << std::endl;

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
	double v = res.size();
	if (db > 0)
	{
		v -= 1;
		v += db * 0.1;
	}
	std::wcout << L"время, мксек:  " << std::to_wstring(mindt) << std::endl;
	std::wcout << L"ppm:           " << double_to_wstring(v, 1) << std::endl;
	std::wcout << L"ppm_test:      " << double_to_wstring(db2, 1) << std::endl << std::endl;
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

void razl(std::vector<std::string>& parameters)
{
	i64 n = 1;
	if (parameters.size() >= 1)
	{
		n = std::stoi(parameters[0]);
		if (n < 1) n = 1;
	}
	std::wcout << L"разложение числа: " << n << std::endl;
	std::vector<std::vector<i64>> res;
	eee(res, {}, n, n);
/*	for (auto& i : res)
	{
		for (auto& j : i) std::wcout << j << L" ";
		std::wcout << std::endl;
	}*/
	std::wcout << L"всего разложений: " << res.size() << std::endl;
}

int main(int argc, char* argv[])
{
	{
		std::string s = argv[0];
		if (s[1] != ':') test_file = test_file2;
	}
	setlocale(LC_ALL, "RU");
	std::wcout << test_file << std::endl;
    for (;;)
    {
        std::cout << ">";
        std::string cmd;
		std::getline(std::cin, cmd);
		std::vector<std::string> parameters;
		std::string command_name = command_decomposition(cmd, parameters);
		if (command_name == "a")
			test_arithmetic_coding(parameters);
		else if (command_name == "b")
			test_test(parameters);
		else if (command_name == "ppm")
			test_ppm(parameters);
		else if (command_name == "r")
			razl(parameters);
		else if (command_name == "exit")
			break;
		else
			std::cout << "команда не найдена\n";
	}
}
