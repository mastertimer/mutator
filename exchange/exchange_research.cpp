#include "g_exchange_graph.h"
#include "exchange_data.h"
#include "exchange_research.h"

void exchange_fun1(_g_terminal* t)
{
	start_stock();
	t->add_text(L"количество цен: " + std::to_wstring(exchange_data.size()));
	t->add_text(L"размер сжатой записи: " + double_to_wstring(double(exchange_data.info_compressed_size) / exchange_data.size(), 1)); // 20.2
}

void exchange_fun2(_g_terminal* t)
{
}

