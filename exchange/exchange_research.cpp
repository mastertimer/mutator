#include "g_exchange_graph.h"
#include "exchange_data.h"
#include "exchange_research.h"

void exchange_fun1(_g_terminal* t)
{
	start_stock();
	t->add_text(L"количество цен: " + std::to_wstring(exchange_data.size()));
	t->add_text(L"размер сжатой записи: " + double_to_wstring(double(exchange_data.info_compressed_size) / exchange_data.size(), 1)); // 20.2
	const _supply_and_demand* prev = nullptr;
	i64 braktime = 0;
	for (auto& i : *exchange_data)
	{
		if (prev)
			if (i.time - prev->time <= 0)
				braktime++;
		prev = &i;
	}
	t->add_text(L"брака времени: " + std::to_wstring(braktime));
}

void exchange_fun2(_g_terminal* t)
{
}

