#include <sstream>
#include <algorithm>
#include <map>
#include <ctime>

#include "sable_stat.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool _offers::operator==(const _offers& p) const
{
	for (i64 i = 0; i < roffer; i++) if (offer[i] != p.offer[i]) return false;
	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool _supply_and_demand::operator==(const _supply_and_demand& p) const
{
	return (demand == p.demand) && (supply == p.supply);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

_prices::operator _supply_and_demand()
{
	_supply_and_demand result;
	result.time = time;
	for (i64 i = 0; i < roffer; i++)
	{
		result.demand.offer[i].price = buy[i].value;
		result.demand.offer[i].number = buy[i].number;
		result.supply.offer[i].price = sale[i].value;
		result.supply.offer[i].number = sale[i].number;
	}
	return result;
}

bool _prices::operator==(const _prices& p) const noexcept
{
	for (i64 i = 0; i < roffer; i++)
		if ((buy[i] != p.buy[i]) || (sale[i] != p.sale[i])) return false;
	return true;
}

bool _prices::operator!=(const _prices& p) const noexcept
{
	for (i64 i = 0; i < roffer; i++)
		if ((buy[i] != p.buy[i]) || (sale[i] != p.sale[i])) return true;
	return false;
}

i64 _prices::time_hour()
{
	tm t3;
	localtime_s(&t3, &time);
	return t3.tm_hour;
}

i64 _prices::time_minute()
{
	tm t3;
	localtime_s(&t3, &time);
	return t3.tm_min;
}

i64 _prices::max_number()
{
	i64 result = 0;
	for (auto i : buy) if (i.number > result) result = i.number;
	for (auto i : sale) if (i.number > result) result = i.number;
	return result;
}

i64 _prices::brak()
{
	i64 result = 0;
	for (auto i : buy) if (i.number > 65535) result++;
	for (auto i : sale) if (i.number > 65535) result++;
	return result;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace
{
	const _cdf f_number({ {1, 0, 44}, {2, 0, 187}, {3, 1, 186}, {5, 0, 207}, {6, 2, 130}, {10, 0, 112}, {11, 2, 291},
		{15, 3, 237}, {21, 2, 695}, {25, 0, 91}, {26, 2, 478}, {30, 0, 65}, {31, 4, 229}, {40, 3, 139}, {48, 1, 401},
		{50, 0, 46}, {51, 0, 92}, {52, 1, 147}, {54, 1, 131}, {56, 0, 221}, {57, 1, 244}, {59, 1, 88}, {61, 2, 64},
		{65, 2, 251}, {69, 1, 311}, {71, 2, 154}, {75, 1, 184}, {77, 3, 239}, {85, 1, 309}, {87, 3, 175}, {95, 3, 343},
		{100, 0, 22}, {101, 1, 307}, {103, 0, 170}, {104, 3, 151}, {112, 3, 195}, {120, 2, 319}, {124, 1, 143},
		{126, 4, 126}, {141, 3, 447}, {149, 1, 487}, {151, 2, 248}, {155, 3, 189}, {163, 4, 223}, {179, 3, 160},
		{187, 4, 227}, {200, 0, 40}, {201, 2, 158}, {205, 0, 273}, {206, 2, 194}, {210, 0, 437}, {211, 1, 287},
		{213, 2, 165}, {217, 2, 247}, {221, 2, 415}, {225, 0, 180}, {226, 1, 471}, {228, 3, 84}, {236, 3, 199},
		{244, 3, 253}, {250, 0, 25}, {251, 2, 135}, {255, 0, 299}, {256, 2, 419}, {260, 0, 100}, {261, 4, 113},
		{276, 3, 218}, {284, 4, 82}, {300, 0, 178}, {301, 3, 173}, {309, 4, 69}, {325, 4, 68}, {341, 3, 359},
		{349, 2, 427}, {353, 3, 234}, {361, 4, 242}, {372, 3, 224}, {380, 5, 77}, {410, 4, 157}, {426, 5, 80},
		{454, 4, 250}, {470, 5, 74}, {502, 5, 167}, {534, 3, 1975}, {542, 5, 243}, {574, 5, 245}, {606, 4, 161},
		{622, 4, 435}, {638, 5, 235}, {670, 6, 98}, {734, 7, 124}, {832, 6, 149}, {896, 7, 255}, {1024, 7, 213},
		{1152, 8, 211}, {1408, 9, 203}, {1920, 10, 225}, {2944, 12, 209}, {7040, 14, 350}, {23424, 24, 1463},
		{10000001, 0, 1} }); // 1...670 000

	const _cdf f_delta({ {1, 0, 3}, {2, 0, 6}, {3, 0, 12}, {4, 0, 16}, {5, 0, 40}, {6, 0, 88}, {7, 0, 440},
		{8, 3, 248}, {16, 10, 312}, {1001, 0, 1} }); // 1...345

	const _cdf nnd({ {1, 0, 27}, {2, 0, 23}, {3, 0, 10}, {4, 0, 9}, {5, 0, 14}, {6, 0, 31}, {7, 1, 13},
		{9, 1, 8}, {11, 1, 19}, {13, 2, 52}, {17, 4, 28}, {33, 10, 36}, {1001, 0, 1} }); // 1...210

	const _cdf3 nnds(-12, { 2484, 4020, 2004, 688, 724, 304, 308, 148, 80, 32, 24, 10, 3, 14, 28, 36, 244, 240,
		340, 692, 944, 1492, 3508, 7092, 5044 }); // -20...20

	const _cdf3 nnse0(1, { 14, 12, 26, 16, 34, 114, 104, 146, 152, 200, 136, 338, 440, 472, 344, 312, 376, 504, 466, 3 });

	const _cdf3 nnse200[21] = { _cdf3() , // исправить
		_cdf3(0, {3, 2}), // 1
		_cdf3(0, {3, 4, 6}), // 2
		_cdf3(0, {3, 12, 8, 6}), // 3
		_cdf3(0, {3, 10, 30, 22, 4}), // 4
		_cdf3(0, {3, 10, 22, 62, 46, 4}), // 5
		_cdf3(0, {3, 10, 8, 20, 60, 44, 14}), // 6
		_cdf3(0, {3, 12, 26, 24, 16, 34, 50, 14}), // 7
		_cdf3(0, {3, 10, 30, 24, 54, 48, 32, 38, 12}), // 8
		_cdf3(0, {3, 10, 30, 28, 38, 52, 118, 36, 86, 8}), // 9
		_cdf3(0, {3, 10, 30, 28, 38, 52, 118, 36, 150, 214, 8}), // 10
		_cdf3(0, {3, 10, 8, 28, 46, 52, 126, 36, 478, 350, 158, 22}), // 11
		_cdf3(0, {3, 14, 8, 28, 42, 52, 122, 36, 474, 410, 282, 346, 18}), // 12
		_cdf3(0, {3, 14, 8, 28, 42, 36, 90, 122, 180, 212, 500, 372, 148, 18}), // 13
		_cdf3(0, {3, 14, 8, 28, 42, 36, 122, 90, 212, 148, 372, 436, 308, 500, 18}), // 14
		_cdf3(0, {3, 14, 8, 20, 60, 44, 90, 122, 202, 490, 362, 298, 394, 266, 426, 18}), // 15
		_cdf3(0, {3, 14, 8, 18, 52, 36, 90, 106, 202, 506, 442, 314, 266, 890, 634, 394, 28}), // 16
		_cdf3(0, {3, 14, 8, 18, 42, 44, 90, 92, 252, 506, 442, 444, 316, 634, 826, 570, 890, 20}), // 17
		_cdf3(0, {2, 11, 9, 29, 47, 39, 95, 87, 247, 511, 447, 439, 895, 639, 575, 823, 567, 831, 21}), // 18
		_cdf3(0, {7, 14, 12, 29, 26, 24, 53, 50, 48, 69, 98, 96, 229, 194, 130, 192, 421, 293, 128, 9}), // 19
		_cdf3(0, {2, 19, 25, 63, 47, 39, 43, 33, 87, 91, 81, 183, 187, 177, 503, 375, 507, 379, 369, 497, 13}) // 20
	};

	const _cdf ttrr1({ {-65, 4, 251}, {-51, 1, 141}, {-49, 4, 447}, {-38, 3, 443}, {-30, 0, 205}, {-29, 2, 476},
		{-25, 0, 43}, {-24, 3, 153}, {-16, 3, 196}, {-10, 0, 228}, {-9, 2, 209}, {-5, 0, 147}, {-4, 1, 129},
		{-2, 0, 137}, {-1, 0, 115}, {0, 0, 2124}, {1, 0, 32}, {2, 0, 143}, {3, 1, 245}, {5, 0, 199}, {6, 2, 180},
		{10, 0, 197}, {11, 2, 361}, {15, 3, 263}, {20, 0, 489}, {21, 2, 348}, {25, 0, 14}, {26, 2, 184}, {30, 0, 113},
		{31, 3, 252}, {38, 2, 295}, {42, 3, 157}, {50, 0, 91}, {51, 0, 244}, {52, 2, 221}, {56, 0, 248}, {57, 2, 401},
		{60, 0, 200}, {61, 1, 291}, {63, 3, 159}, {71, 2, 831}, {75, 0, 133}, {76, 3, 211}, {83, 2, 188}, {87, 2, 193},
		{91, 2, 479}, {95, 0, 647}, {96, 2, 232}, {100, 0, 10}, {101, 1, 408}, {103, 3, 229}, {111, 3, 136},
		{119, 3, 169}, {125, 0, 97}, {126, 4, 80}, {142, 3, 429}, {150, 0, 460}, {151, 2, 575}, {155, 3, 511},
		{163, 4, 227}, {179, 4, 253}, {191, 3, 156}, {199, 0, 844}, {200, 0, 55}, {201, 2, 240}, {205, 3, 84},
		{213, 2, 168}, {217, 3, 239}, {225, 0, 189}, {226, 1, 280}, {228, 3, 237}, {236, 4, 108}, {250, 0, 419},
		{251, 1, 903}, {253, 3, 273}, {260, 0, 85}, {261, 4, 249}, {277, 4, 181}, {293, 1, 1612}, {295, 3, 351},
		{303, 4, 201}, {319, 4, 140}, {335, 5, 217}, {356, 4, 176}, {372, 5, 131}, {402, 4, 301}, {418, 5, 164},
		{445, 4, 315}, {461, 5, 303}, {485, 4, 431}, {501, 6, 231}, {565, 6, 185}, {609, 5, 195}, {641, 7, 207},
		{755, 6, 423}, {819, 6, 165}, {883, 7, 132}, {1011, 8, 383}, {1267, 11, 216}, {3315, 24, 3148},
		{10000001, 0, 1} });

	const _cdf ttrr2({ {-199, 6, 204}, {-150, 1, 276}, {-148, 5, 133}, {-125, 0, 205}, {-124, 3, 186},
		{-116, 4, 234}, {-104, 2, 399}, {-100, 0, 23}, {-99, 3, 171}, {-92, 2, 477}, {-88, 1, 361}, {-86, 2, 188},
		{-82, 3, 141}, {-75, 0, 253}, {-74, 3, 187}, {-66, 3, 146}, {-60, 2, 201}, {-56, 0, 271}, {-55, 2, 218},
		{-51, 0, 1007}, {-50, 0, 98}, {-49, 0, 1547}, {-48, 3, 149}, {-40, 3, 138}, {-34, 2, 473}, {-30, 0, 81},
		{-29, 2, 501}, {-25, 0, 14}, {-24, 2, 341}, {-20, 1, 497}, {-18, 3, 189}, {-10, 0, 395}, {-9, 2, 367},
		{-5, 0, 212}, {-4, 1, 489}, {-2, 0, 345}, {-1, 0, 100}, {0, 0, 2059}, {1, 0, 114}, {2, 0, 373}, {3, 1, 378},
		{5, 0, 319}, {6, 2, 369}, {10, 0, 301}, {11, 3, 156}, {19, 3, 383}, {25, 0, 8}, {26, 2, 469}, {30, 0, 116},
		{31, 2, 404}, {35, 4, 251}, {50, 0, 97}, {51, 0, 508}, {52, 2, 177}, {56, 2, 210}, {60, 1, 140}, {62, 4, 219},
		{75, 0, 223}, {76, 3, 194}, {84, 3, 185}, {92, 2, 429}, {96, 2, 349}, {100, 0, 19}, {101, 0, 892},
		{102, 4, 203}, {117, 3, 170}, {125, 0, 237}, {126, 4, 169}, {140, 3, 357}, {148, 4, 159}, {164, 4, 485},
		{175, 0, 449}, {176, 5, 207}, {200, 0, 108}, {201, 1, 636}, {203, 3, 220}, {211, 4, 157}, {225, 0, 751},
		{226, 2, 779}, {230, 4, 165}, {246, 4, 202}, {260, 0, 175}, {261, 5, 197}, {287, 4, 491}, {303, 5, 137},
		{334, 4, 506}, {350, 5, 154}, {382, 5, 196}, {414, 5, 363}, {442, 5, 511}, {474, 5, 153}, {506, 6, 181},
		{570, 6, 132}, {622, 5, 321}, {654, 7, 129}, {762, 6, 447}, {826, 6, 249}, {890, 8, 155}, {1146, 10, 130},
		{2170, 24, 3083}, {10000001, 0, 1} });

	const _cdf ttrr3({ {-317, 5, 84}, {-287, 3, 421}, {-279, 4, 461}, {-268, 3, 273}, {-260, 0, 105}, {-259, 4, 295},
		{-250, 0, 473}, {-249, 4, 189}, {-235, 3, 375}, {-228, 2, 204}, {-224, 3, 134}, {-216, 2, 417}, {-212, 3, 225},
		{-204, 2, 401}, {-200, 0, 99}, {-199, 4, 237}, {-183, 3, 414}, {-175, 0, 599}, {-174, 4, 201}, {-158, 3, 413},
		{-150, 0, 278}, {-149, 4, 133}, {-133, 3, 196}, {-125, 0, 102}, {-124, 2, 293}, {-120, 0, 855}, {-119, 4, 220},
		{-108, 3, 198}, {-100, 0, 10}, {-99, 1, 286}, {-97, 2, 349}, {-93, 3, 129}, {-85, 4, 193}, {-75, 0, 213},
		{-74, 4, 195}, {-60, 2, 259}, {-56, 1, 214}, {-54, 2, 289}, {-50, 0, 108}, {-49, 4, 132}, {-40, 3, 140},
		{-32, 1, 1613}, {-30, 0, 118}, {-29, 2, 406}, {-25, 0, 8}, {-24, 3, 222}, {-16, 3, 467}, {-10, 0, 369},
		{-9, 2, 373}, {-5, 2, 183}, {-1, 0, 100}, {0, 0, 2125}, {1, 0, 121}, {2, 0, 285}, {3, 1, 430}, {5, 0, 407},
		{6, 2, 510}, {10, 0, 403}, {11, 4, 243}, {25, 0, 31}, {26, 2, 302}, {30, 0, 116}, {31, 2, 531}, {35, 4, 181},
		{50, 0, 78}, {51, 2, 382}, {54, 1, 345}, {56, 2, 455}, {60, 1, 423}, {62, 4, 149}, {75, 0, 141}, {76, 4, 387},
		{85, 3, 279}, {93, 3, 238}, {100, 0, 27}, {101, 4, 177}, {117, 3, 503}, {125, 0, 179}, {126, 5, 124},
		{158, 5, 153}, {184, 4, 501}, {200, 0, 197}, {201, 4, 156}, {217, 5, 253}, {244, 4, 263}, {260, 0, 173},
		{261, 6, 231}, {325, 6, 137}, {389, 4, 845}, {405, 6, 190}, {468, 5, 327}, {500, 0, 787}, {501, 5, 477},
		{533, 7, 229}, {661, 7, 471}, {772, 6, 497}, {836, 6, 391}, {900, 8, 209}, {1156, 12, 339},	{5252, 24, 3149},
		{10000001, 0, 1} });

	const _cdf ttrr4({ {-10000000, 24, 2717}, {-6096, 12, 1181}, {-2214, 10, 153}, {-1192, 7, 297}, {-1064, 7, 238},
		{-945, 6, 475}, {-887, 5, 194}, {-855, 6, 112}, {-791, 5, 959}, {-767, 5, 425}, {-735, 6, 178}, {-671, 6, 230},
		{-625, 5, 134}, {-593, 6, 237}, {-539, 5, 225}, {-507, 3, 210}, {-499, 5, 207}, {-467, 5, 141}, {-438, 3, 958},
		{-430, 5, 155}, {-398, 5, 166}, {-375, 4, 154}, {-359, 5, 255}, {-327, 5, 129}, {-308, 3, 402}, {-300, 4, 221},
		{-284, 5, 159}, {-260, 0, 81}, {-259, 3, 863}, {-253, 2, 319}, {-249, 4, 193}, {-235, 3, 271}, {-227, 2, 347},
		{-223, 4, 253}, {-208, 3, 399}, {-200, 0, 98}, {-199, 5, 217}, {-178, 4, 202}, {-162, 4, 251}, {-146, 3, 394},
		{-138, 4, 198}, {-125, 0, 106}, {-124, 2, 318}, {-120, 4, 161}, {-108, 3, 431}, {-100, 0, 12}, {-99, 3, 185},
		{-91, 2, 456}, {-87, 4, 241}, {-75, 0, 254}, {-74, 4, 201}, {-60, 2, 374}, {-56, 1, 479}, {-54, 2, 328},
		{-50, 0, 239}, {-49, 5, 96}, {-30, 0, 80}, {-29, 2, 607}, {-25, 0, 23}, {-24, 3, 130}, {-16, 3, 206},
		{-9, 2, 346}, {-5, 2, 205}, {-1, 1, 235}, {1, 0, 94}, {2, 2, 88}, {6, 2, 274}, {10, 0, 285}, {11, 4, 249},
		{25, 0, 19}, {26, 2, 703}, {30, 0, 203}, {31, 2, 702}, {35, 4, 242}, {50, 0, 64}, {51, 3, 137}, {57, 2, 474},
		{61, 4, 173}, {75, 0, 174}, {76, 4, 142}, {92, 3, 232}, {100, 0, 21}, {101, 5, 187}, {125, 0, 177},
		{126, 5, 189}, {158, 5, 168}, {184, 4, 266}, {200, 0, 303}, {201, 5, 171}, {233, 5, 150}, {260, 0, 214},
		{261, 6, 233}, {325, 6, 136}, {389, 7, 120}, {500, 0, 925}, {501, 7, 139}, {629, 8, 122}, {885, 8, 182},
		{1141, 10, 502}, {2165, 24, 3741}, {10000001, 0, 1} });

	static const _cdf3 nnsegg[21] = { _cdf3() , // исправить
		_cdf3(0, {2, 3}), // 1
		_cdf3(0, {6, 4, 3}), // 2
		_cdf3(0, {6, 12, 8, 3}), // 3
		_cdf3(0, {4, 10, 30, 22, 3}), // 4
		_cdf3(0, {5, 11, 23, 63, 47, 2}), // 5
		_cdf3(0, {5, 31, 27, 19, 55, 39, 2}), // 6
		_cdf3(0, {5, 31, 27, 55, 39, 51, 35, 2}), // 7
		_cdf3(0, {5, 31, 27, 39, 51, 35, 87, 119, 2}), // 8
		_cdf3(0, {15, 13, 19, 59, 43, 57, 49, 41, 33, 2}), // 9
		_cdf3(0, {15, 13, 19, 59, 57, 41, 33, 49, 107, 75, 2}), // 10
		_cdf3(0, {15, 13, 19, 59, 41, 49, 107, 33, 75, 121, 89, 2}), // 11
		_cdf3(0, {15, 9, 19, 59, 53, 37, 75, 107, 125, 93, 77, 109, 2}), // 12
		_cdf3(0, {15, 13, 19, 43, 41, 123, 121, 91, 89, 81, 65, 113, 97, 2}), // 13
		_cdf3(0, {15, 13, 19, 43, 41, 123, 89, 121, 113, 97, 219, 81, 155, 65, 2}), // 14
		_cdf3(0, {15, 13, 19, 59, 49, 107, 89, 105, 73, 65, 249, 203, 185, 139, 97, 2}), // 15
		_cdf3(0, {15, 13, 19, 59, 49, 107, 105, 73, 97, 139, 217, 249, 153, 185, 203, 65, 2}), // 16
		_cdf3(0, {5, 31, 27, 39, 119, 99, 151, 211, 147, 131, 343, 371, 307, 435, 499, 471, 195, 2}), // 17
		_cdf3(0, {5, 11, 63, 39, 119, 175, 215, 367, 399, 271, 1007, 751, 591, 719, 847, 975, 279, 407, 2}), // 18
		_cdf3(0, {5, 11, 63, 55, 79, 239, 199, 303, 295, 263, 943, 999, 871, 679, 935, 615, 743, 687, 391, 2}), // 19
		_cdf3(0, {5, 4, 10, 62, 54, 78, 174, 134, 486, 454, 326, 750, 614, 806, 550, 678, 934, 870, 1006, 366, 7}) // 20
	};
}

bool _sable_stat::add0(const _prices& c)
{
	ip_last.ok = false;
	data.pushn(c.buy[roffer - 1].value, 16);
	for (i64 i = roffer - 1; i >= 0; i--)
	{
		if (i != roffer - 1)
		{
			if (!f_delta.coding(c.buy[i].value - c.buy[i + 1].value, data)) return false;
		}
		if (!f_number.coding(c.buy[i].number, data)) return false;
	}
	if (!nnd.coding(c.sale[0].value - c.buy[0].value, data)) return false;
	for (i64 i = 0; i < roffer; i++)
	{
		if (i != 0)
		{
			if (!f_delta.coding(c.sale[i].value - c.sale[i - 1].value, data)) return false;
		}
		if (!f_number.coding(c.sale[i].number, data)) return false;
	}
	base_buy.resize(roffer);
	base_sale.resize(roffer);
	for (i64 i = 0; i < roffer; i++)
	{
		base_buy[i] = c.buy[i];
		base_sale[i] = c.sale[i];
	}
	return true;
}

i64 calc_delta_del_add(const _one_stat* c, const std::vector<_one_stat>& v)
{
	if (c[0].value == v[0].value) return 0;
	if (c[1].value > c[0].value)
	{
		if (c[0].value < v[0].value)
		{
			for (i64 i = 1; i < roffer; i++) if (c[i].value >= v[0].value) return i;
			return roffer;
		}
		for (i64 i = 1; i < (i64)v.size(); i++) if (v[i].value >= c[0].value) return -i;
		return -((i64)v.size());
	}
	if (c[0].value > v[0].value)
	{
		for (i64 i = 1; i < roffer; i++) if (c[i].value <= v[0].value) return i;
		return roffer;
	}
	for (i64 i = 1; i < (i64)v.size(); i++) if (v[i].value <= c[0].value) return -i;
	return -((i64)v.size());
}

i64 calc_delta_del_add(const _offer* c, const std::vector<_offer>& v)
{
	if (c[0].price == v[0].price) return 0;
	if (c[1].price > c[0].price)
	{
		if (c[0].price < v[0].price)
		{
			for (i64 i = 1; i < roffer; i++) if (c[i].price >= v[0].price) return i;
			return roffer;
		}
		for (i64 i = 1; i < (i64)v.size(); i++) if (v[i].price >= c[0].price) return -i;
		return -((i64)v.size());
	}
	if (c[0].price > v[0].price)
	{
		for (i64 i = 1; i < roffer; i++) if (c[i].price <= v[0].price) return i;
		return roffer;
	}
	for (i64 i = 1; i < (i64)v.size(); i++) if (v[i].price <= c[0].price) return -i;
	return -((i64)v.size());
}

i64 calc_delta_del_add1(const _one_stat* c, const std::vector<_one_stat>& v, i64 n)
{
	if (c[n].value == v[n].value) return 0;
	if (c[1].value > c[0].value)
	{
		if (c[n].value < v[n].value) return 1;
		return -1;
	}
	if (c[n].value > v[n].value) return 1;
	return -1;
}

i64 calc_delta_del_add1(const _offer* c, const std::vector<_offer>& v, i64 n)
{
	if (c[n].price == v[n].price) return 0;
	if (c[1].price > c[0].price)
	{
		if (c[n].price < v[n].price) return 1;
		return -1;
	}
	if (c[n].price > v[n].price) return 1;
	return -1;
}

i64 calc_series_value(const _one_stat* c, const std::vector<_one_stat>& v, i64 n)
{
	i64 k = std::min(roffer, (i64)v.size());
	for (i64 i = n; i < k; i++) if (c[i].value != v[i].value) return i - n;
	return k - n;
}

i64 calc_series_value(const _offer* c, const std::vector<_offer>& v, i64 n)
{
	i64 k = std::min(roffer, (i64)v.size());
	for (i64 i = n; i < k; i++) if (c[i].price != v[i].price) return i - n;
	return k - n;
}

i64 calc_series_number(const _one_stat* c, const std::vector<_one_stat>& v, i64 n, i64 k)
{
	for (i64 i = n; i < k; i++) if (c[i].number != v[i].number) return i - n;
	return k - n;
}

i64 calc_series_number(const _offer* c, const std::vector<_offer>& v, i64 n, i64 k)
{
	for (i64 i = n; i < k; i++) if (c[i].number != v[i].number) return i - n;
	return k - n;
}

bool _sable_stat::coding_delta_number(i64 a, i64 b)
{
	if (a <= 66) return ttrr1.coding(b - a, data); // 25 %
	if (a <= 200) return ttrr2.coding(b - a, data); // 25 %
	if (a <= 318) return ttrr3.coding(b - a, data); // 25 %
	return ttrr4.coding(b - a, data); // 25 %
}

i64 _sable_stat::decoding_delta_number(i64 a)
{
	if (a <= 66) return a + ttrr1.decoding(data); // 25 %
	if (a <= 200) return a + ttrr2.decoding(data); // 25 %
	if (a <= 318) return a + ttrr3.decoding(data); // 25 %
	return a + ttrr4.decoding(data); // 25 %
}

bool _sable_stat::add12(const _one_stat* v1, std::vector<_one_stat>& v0, i64 izm)
{
	i64 kk = (v1[1].value > v1[0].value) ? -1 : 1;
	if (!nnds.coding(izm, data)) return false;
	i64 n = 0;
	i64 tip = izm;
	if (izm == 0)
	{
		n = calc_series_value(v1, v0, 0);
		if (!nnse0.coding(n, data)) return false;
		for (i64 i = 0; i < n;)
		{
			i64 ser = calc_series_number(v1, v0, i, n);
			if (!nnse200[n - i].coding(ser, data)) return false;
			i += ser;
			if (i >= n) break;
			if (!coding_delta_number(v0[i].number, v1[i].number)) return false;
			v0[i].number = v1[i].number;
			i++;
		}
	}
	if (izm < 0)
	{
		i64 lv = (v0.begin() - (izm + 1))->value;
		v0.erase(v0.begin(), v0.begin() - izm);
		i64 n2 = calc_series_value(v1, v0, 0);
		if (!nnsegg[std::min((i64)v0.size(), roffer)].coding(n2, data)) return false;
		if (n2 == 0)
		{
			if (!f_delta.coding((lv - v1[0].value) * kk, data))	return false;
			if (!f_number.coding(v1[0].number, data)) return false;
			v0.insert(v0.begin(), v1[0]);
			n = 1;
		}
		else
		{
			n = n2;
			for (i64 i = 0; i < n;)
			{
				i64 ser = calc_series_number(v1, v0, i, n);
				if (!nnse200[n - i].coding(ser, data)) return false;
				i += ser;
				if (i >= n) break;
				if (!coding_delta_number(v0[i].number, v1[i].number)) return false;
				v0[i].number = v1[i].number;
				i++;
			}
			tip = 0;
		}
	}
	if (izm > 0)
	{
		v0.insert(v0.begin(), izm, {});
		for (i64 i = izm - 1; i >= 0; i--) // кодируется как c add0
		{
			if (!f_delta.coding((v1[i].value - v0[i + 1].value) * kk, data)) return false;
			if (!f_number.coding(v1[i].number, data)) return false;
			v0[i] = v1[i];
		}
		n = izm;
		i64 n2 = calc_series_value(v1, v0, n);
		if (!nnsegg[std::min((i64)v0.size(), roffer) - n].coding(n2, data)) return false;
		if (n2 == 0)
			v0.erase(v0.begin() + n);
		else
		{
			n2 += n;
			for (i64 i = n; i < n2;)
			{
				i64 ser = calc_series_number(v1, v0, i, n2);
				if (!nnse200[n2 - i].coding(ser, data)) return false;
				i += ser;
				if (i >= n2) break;
				if (!coding_delta_number(v0[i].number, v1[i].number)) return false;
				v0[i].number = v1[i].number;
				i++;
			}
			n = n2;
			tip = 0;
		}
	}
	while (n < roffer)
	{
		if (n >= (i64)v0.size())
		{
			v0.resize(roffer);
			for (; n < roffer; n++)
			{
				v0[n] = v1[n];
				if (!f_delta.coding((v1[n - 1].value - v1[n].value) * kk, data)) return false;
				if (!f_number.coding(v1[n].number, data)) return false;
			}
			break;
		}
		if (tip == 0)
		{
			i64 buy_izm2 = calc_delta_del_add1(v1, v0, n);
			data.push1(buy_izm2 < 0);
			if (buy_izm2 < 0)
				v0.erase(v0.begin() + n);
			else
			{
				v0.insert(v0.begin() + n, v1[n]);
				if (!f_delta.coding((v1[n - 1].value - v1[n].value) * kk, data)) return false;
				if (!f_number.coding(v1[n].number, data)) return false;
				n++;
			}
			tip = 1;
			continue;
		}
		i64 n2 = calc_series_value(v1, v0, n);
		if (!nnsegg[std::min((i64)v0.size(), roffer) - n].coding(n2, data)) return false;
		if (n2 > 0)
		{
			n2 += n;
			for (i64 i = n; i < n2;)
			{
				i64 ser = calc_series_number(v1, v0, i, n2);
				if (!nnse200[n2 - i].coding(ser, data)) return false;
				i += ser;
				if (i >= n2) break;
				if (!coding_delta_number(v0[i].number, v1[i].number)) return false;
				v0[i].number = v1[i].number;
				i++;
			}
			n = n2;
		}
		tip = 0;
	}
	return true;
}

bool _sable_stat::add1(const _prices& c)
{
	ip_last.ok = true;
	i64 buy_izm = calc_delta_del_add(c.buy, base_buy);
	i64 sale_izm = calc_delta_del_add(c.sale, base_sale);
	if (std::max(abs(buy_izm), abs(sale_izm)) > 12)
	{
		data.push1(0);
		return add0(c);
	}
	data.push1(1);

	std::vector<_one_stat> bbuy = base_buy;
	std::vector<_one_stat> bsale = base_sale;

	i64 aa0 = data.size();
	if (!add12(c.buy, bbuy, buy_izm)) return false;
	i64 aa1 = data.size();
	if (!add12(c.sale, bsale, sale_izm)) return false;

	ip_last.r = int(data.size() - aa0);
	ip_last.r_pok = int(aa1 - aa0);
	ip_last.r_pro = int(data.size() - aa1);

	base_buy = std::move(bbuy);
	base_sale = std::move(bsale);
	return true;
}

bool _sable_stat::add(const _prices& c)
{
	if (c == back) return true; // с большой вероятностью данные устарели
	if (c.time < back.time) return true; // цены из прошлого не принимаются!
	auto s_data = data.size();
	if (size % step_pak_cc == 0)
	{
		udata.push_back(data.size());
		data.pushn(c.time, 31);
		if (!add0(c))
		{
			udata.pop_back();
			goto err;
		}
	}
	else
	{
		time_t dt = c.time - back.time;
		if (dt == 1) data.push1(0); else { data.push1(1); data.pushn(dt, 31); }
		if (dt > old_dtime)
		{
			if (!add0(c)) goto err;
		}
		else
		{
			if (!add1(c)) goto err;
		}
	}
	size++;
	back = c;
	return true;
err:
	data.resize(s_data);
	return false; // ошибка кодирования, нужно исправлять!!
}

bool _sable_stat::read0(_prices& c)
{
	ip_n.ok = false;
	c.buy[roffer - 1].value = data.popn(16);
	for (i64 i = roffer - 1; i >= 0; i--)
	{
		if (i != roffer - 1) c.buy[i].value = c.buy[i + 1].value + f_delta.decoding(data);
		c.buy[i].number = f_number.decoding(data);
	}
	c.sale[0].value = c.buy[0].value + nnd.decoding(data);
	for (i64 i = 0; i < roffer; i++)
	{
		if (i != 0) c.sale[i].value = c.sale[i - 1].value + f_delta.decoding(data);
		c.sale[i].number = f_number.decoding(data);
	}
	base_buy_r.resize(roffer);
	base_sale_r.resize(roffer);
	for (i64 i = 0; i < roffer; i++)
	{
		base_buy_r[i] = c.buy[i];
		base_sale_r[i] = c.sale[i];
	}
	return true;
}

bool _sable_stat::read12(_one_stat* v1, std::vector<_one_stat>& v0)
{
	i64 kk = (v0[1].value > v0[0].value) ? -1 : 1;
	i64 izm = nnds.decoding(data);
	i64 n = 0;
	i64 tip = izm;
	if (izm == 0)
	{
		n = nnse0.decoding(data);
		for (i64 i = 0; i < n;)
		{
			i64 ser = nnse200[n - i].decoding(data);
			for (i64 j = i; j < i + ser; j++) v1[j] = v0[j];
			i += ser;
			if (i >= n) break;
			v1[i].value = v0[i].value;
			v1[i].number = decoding_delta_number(v0[i].number);
			v0[i].number = v1[i].number;
			i++;
		}
	}
	if (izm < 0)
	{
		i64 lv = (v0.begin() - (izm + 1))->value;
		v0.erase(v0.begin(), v0.begin() - izm);
		i64 n2 = nnsegg[std::min((i64)v0.size(), roffer)].decoding(data);
		if (n2 == 0)
		{
			v1[0].value = lv - kk * f_delta.decoding(data);
			v1[0].number = f_number.decoding(data);
			v0.insert(v0.begin(), v1[0]);
			n = 1;
		}
		else
		{
			n = n2;
			for (i64 i = 0; i < n;)
			{
				i64 ser = nnse200[n - i].decoding(data);
				for (i64 j = i; j < i + ser; j++) v1[j] = v0[j];
				i += ser;
				if (i >= n) break;
				v1[i].value = v0[i].value;
				v1[i].number = decoding_delta_number(v0[i].number);
				v0[i].number = v1[i].number;
				i++;
			}
			tip = 0;
		}
	}
	if (izm > 0)
	{
		v0.insert(v0.begin(), izm, {});
		for (i64 i = izm - 1; i >= 0; i--) // кодируется как c add0
		{
			v1[i].value = v0[i + 1].value + kk * f_delta.decoding(data);
			v1[i].number = f_number.decoding(data);
			v0[i] = v1[i];
		}
		n = izm;
		i64 n2 = nnsegg[std::min((i64)v0.size(), roffer) - n].decoding(data);
		if (n2 == 0)
			v0.erase(v0.begin() + n);
		else
		{
			n2 += n;
			for (i64 i = n; i < n2;)
			{
				i64 ser = nnse200[n2 - i].decoding(data);
				for (i64 j = i; j < i + ser; j++) v1[j] = v0[j];
				i += ser;
				if (i >= n2) break;
				v1[i].value = v0[i].value;
				v1[i].number = decoding_delta_number(v0[i].number);
				v0[i].number = v1[i].number;
				i++;
			}
			n = n2;
			tip = 0;
		}
	}
	while (n < roffer)
	{
		if (n >= (i64)v0.size())
		{
			v0.resize(roffer);
			for (; n < roffer; n++)
			{
				v1[n].value = v1[n - 1].value - kk * f_delta.decoding(data);
				v1[n].number = f_number.decoding(data);
				v0[n] = v1[n];
			}
			break;
		}
		if (tip == 0)
		{
			if (data.pop1())
				v0.erase(v0.begin() + n);
			else
			{
				v1[n].value = v1[n - 1].value - kk * f_delta.decoding(data);
				v1[n].number = f_number.decoding(data);
				v0.insert(v0.begin() + n, v1[n]);
				n++;
			}
			tip = 1;
			continue;
		}
		i64 n2 = nnsegg[std::min((i64)v0.size(), roffer) - n].decoding(data);
		if (n2 > 0)
		{
			n2 += n;
			for (i64 i = n; i < n2;)
			{
				i64 ser = nnse200[n2 - i].decoding(data);
				for (i64 j = i; j < i + ser; j++) v1[j] = v0[j];
				i += ser;
				if (i >= n2) break;
				v1[i].value = v0[i].value;
				v1[i].number = decoding_delta_number(v0[i].number);
				v0[i].number = v1[i].number;
				i++;
			}
			n = n2;
		}
		tip = 0;
	}
	return true;
}

bool _sable_stat::read1(_prices& c)
{
	if (data.pop1() == 0) return read0(c);
	auto aa0 = data.bit_read;
	if (!read12(c.buy, base_buy_r)) return false;
	auto aa1 = data.bit_read;
	if (!read12(c.sale, base_sale_r)) return false;
	ip_n.r = int(data.bit_read - aa0);
	ip_n.r_pok = int(aa1 - aa0);
	ip_n.r_pro = int(data.bit_read - aa1);
	return true;
}

bool _sable_stat::read(i64 n, _prices& c, _info_pak* inf)
{
	if (inf) inf->ok = false;
	if ((n < 0) || (n >= size)) return false;
	if (n == read_n)
	{
		c = read_cc;
		if (inf) *inf = ip_n;
		return true;
	}
	if (n == size - 1)
	{
		c = back;
		if (inf) *inf = ip_last;
		return true;
	}
	if (read_n + 1 != n)
	{
		i64 k = n / step_pak_cc;
		if ((read_n > n) || (read_n <= k * step_pak_cc - 1))
		{
			data.bit_read = udata[k];
			read_n = k * step_pak_cc - 1;
		}
		bool r = false;
		while (read_n < n) r = read(read_n + 1, c, inf);
		return r;
	}
	ip_n.ok = false;
	if (n % step_pak_cc == 0)
	{
		c.time = data.popn(31);
		if (!read0(c)) return false;
	}
	else
	{
		time_t dt;
		if (data.pop1() == 0)
			dt = 1;
		else
			dt = data.popn(31);
		c.time = read_cc.time + dt;
		if (dt > old_dtime)
		{
			if (!read0(c)) return false;
		}
		else
		{
			if (dt == 1) ip_n.ok = true;
			if (!read1(c)) return false;
		}
	}
	read_cc = c;
	read_n = n;
	if (inf) *inf = ip_n;
	return true;
}

void _sable_stat::save_to_file(wstr fn)
{
	_stack mem;
	data.save(mem);
	mem << size;
	mem.push_data(&back, sizeof(back));
	mem << udata;
	mem << base_buy;
	mem << base_sale;
	mem.save_to_file(fn);
}

void _sable_stat::load_from_file(wstr fn)
{
	_stack mem;
	if (!mem.load_from_file(fn)) return;
	data.load(mem);
	mem >> size;
	mem >> back;
	mem >> udata;
	mem >> base_buy;
	mem >> base_sale;
	read_n = -666;
	ip_last.ok = false;
	ip_n.ok = false;
}

void _sable_stat::clear()
{
	data.clear();
	udata.clear();
	back = {};
	size = 0;
	read_n = -666;
	ip_last.ok = false;
	ip_n.ok = false;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

i64  _compression_stock_statistics::decoding_delta_number(i64 a)
{
	if (a <= 66) return a + ttrr1.decoding(data); // 25 %
	if (a <= 200) return a + ttrr2.decoding(data); // 25 %
	if (a <= 318) return a + ttrr3.decoding(data); // 25 %
	return a + ttrr4.decoding(data); // 25 %
}

bool _compression_stock_statistics::read0(_supply_and_demand& c)
{
	ip_n.ok = false;
	c.demand.offer[roffer - 1].price = data.popn(16);
	for (i64 i = roffer - 1; i >= 0; i--)
	{
		if (i != roffer - 1) c.demand.offer[i].price = c.demand.offer[i + 1].price + f_delta.decoding(data);
		c.demand.offer[i].number = f_number.decoding(data);
	}
	c.supply.offer[0].price = c.demand.offer[0].price + nnd.decoding(data);
	for (i64 i = 0; i < roffer; i++)
	{
		if (i != 0) c.supply.offer[i].price = c.supply.offer[i - 1].price + f_delta.decoding(data);
		c.supply.offer[i].number = f_number.decoding(data);
	}
	base_buy_r.resize(roffer);
	base_sale_r.resize(roffer);
	for (i64 i = 0; i < roffer; i++)
	{
		base_buy_r[i] = c.demand.offer[i];
		base_sale_r[i] = c.supply.offer[i];
	}
	return true;
}

bool _compression_stock_statistics::read1(_supply_and_demand& c)
{
	if (data.pop1() == 0) return read0(c);
	auto aa0 = data.bit_read;
	if (!read12(c.demand.offer, base_buy_r)) return false;
	auto aa1 = data.bit_read;
	if (!read12(c.supply.offer, base_sale_r)) return false;
	ip_n.r = int(data.bit_read - aa0);
	ip_n.r_pok = int(aa1 - aa0);
	ip_n.r_pro = int(data.bit_read - aa1);
	return true;
}

bool _compression_stock_statistics::read12(_offer* v1, std::vector<_offer>& v0)
{
	i64 kk = (v0[1].price > v0[0].price) ? -1 : 1;
	i64 izm = nnds.decoding(data);
	i64 n = 0;
	i64 tip = izm;
	if (izm == 0)
	{
		n = nnse0.decoding(data);
		for (i64 i = 0; i < n;)
		{
			i64 ser = nnse200[n - i].decoding(data);
			for (i64 j = i; j < i + ser; j++) v1[j] = v0[j];
			i += ser;
			if (i >= n) break;
			v1[i].price = v0[i].price;
			v1[i].number = decoding_delta_number(v0[i].number);
			v0[i].number = v1[i].number;
			i++;
		}
	}
	if (izm < 0)
	{
		i64 lv = (v0.begin() - (izm + 1))->price;
		v0.erase(v0.begin(), v0.begin() - izm);
		i64 n2 = nnsegg[std::min((i64)v0.size(), roffer)].decoding(data);
		if (n2 == 0)
		{
			v1[0].price = lv - kk * f_delta.decoding(data);
			v1[0].number = f_number.decoding(data);
			v0.insert(v0.begin(), v1[0]);
			n = 1;
		}
		else
		{
			n = n2;
			for (i64 i = 0; i < n;)
			{
				i64 ser = nnse200[n - i].decoding(data);
				for (i64 j = i; j < i + ser; j++) v1[j] = v0[j];
				i += ser;
				if (i >= n) break;
				v1[i].price = v0[i].price;
				v1[i].number = decoding_delta_number(v0[i].number);
				v0[i].number = v1[i].number;
				i++;
			}
			tip = 0;
		}
	}
	if (izm > 0)
	{
		v0.insert(v0.begin(), izm, {});
		for (i64 i = izm - 1; i >= 0; i--) // кодируется как c add0
		{
			v1[i].price = v0[i + 1].price + kk * f_delta.decoding(data);
			v1[i].number = f_number.decoding(data);
			v0[i] = v1[i];
		}
		n = izm;
		i64 n2 = nnsegg[std::min((i64)v0.size(), roffer) - n].decoding(data);
		if (n2 == 0)
			v0.erase(v0.begin() + n);
		else
		{
			n2 += n;
			for (i64 i = n; i < n2;)
			{
				i64 ser = nnse200[n2 - i].decoding(data);
				for (i64 j = i; j < i + ser; j++) v1[j] = v0[j];
				i += ser;
				if (i >= n2) break;
				v1[i].price = v0[i].price;
				v1[i].number = decoding_delta_number(v0[i].number);
				v0[i].number = v1[i].number;
				i++;
			}
			n = n2;
			tip = 0;
		}
	}
	while (n < roffer)
	{
		if (n >= (i64)v0.size())
		{
			v0.resize(roffer);
			for (; n < roffer; n++)
			{
				v1[n].price = v1[n - 1].price - kk * f_delta.decoding(data);
				v1[n].number = f_number.decoding(data);
				v0[n] = v1[n];
			}
			break;
		}
		if (tip == 0)
		{
			if (data.pop1())
				v0.erase(v0.begin() + n);
			else
			{
				v1[n].price = v1[n - 1].price - kk * f_delta.decoding(data);
				v1[n].number = f_number.decoding(data);
				v0.insert(v0.begin() + n, v1[n]);
				n++;
			}
			tip = 1;
			continue;
		}
		i64 n2 = nnsegg[std::min((i64)v0.size(), roffer) - n].decoding(data);
		if (n2 > 0)
		{
			n2 += n;
			for (i64 i = n; i < n2;)
			{
				i64 ser = nnse200[n2 - i].decoding(data);
				for (i64 j = i; j < i + ser; j++) v1[j] = v0[j];
				i += ser;
				if (i >= n2) break;
				v1[i].price = v0[i].price;
				v1[i].number = decoding_delta_number(v0[i].number);
				v0[i].number = v1[i].number;
				i++;
			}
			n = n2;
		}
		tip = 0;
	}
	return true;
}

bool _compression_stock_statistics::read(i64 n, _supply_and_demand& c, _info_pak* inf)
{
	if (inf) inf->ok = false;
	if ((n < 0) || (n >= size)) return false;
	if (n == read_n)
	{
		c = read_cc;
		if (inf) *inf = ip_n;
		return true;
	}
	if (n == size - 1)
	{
		c = back;
		if (inf) *inf = ip_last;
		return true;
	}
	if (read_n + 1 != n)
	{
		i64 k = n / step_pak_cc;
		if ((read_n > n) || (read_n <= k * step_pak_cc - 1))
		{
			data.bit_read = udata[k];
			read_n = k * step_pak_cc - 1;
		}
		bool r = false;
		while (read_n < n) r = read(read_n + 1, c, inf);
		return r;
	}
	ip_n.ok = false;
	if (n % step_pak_cc == 0)
	{
		c.time = data.popn(31);
		if (!read0(c)) return false;
	}
	else
	{
		time_t dt;
		if (data.pop1() == 0)
			dt = 1;
		else
			dt = data.popn(31);
		c.time = read_cc.time + dt;
		if (dt > old_dtime)
		{
			if (!read0(c)) return false;
		}
		else
		{
			if (dt == 1) ip_n.ok = true;
			if (!read1(c)) return false;
		}
	}
	read_cc = c;
	read_n = n;
	if (inf) *inf = ip_n;
	return true;
}

void _compression_stock_statistics::save_to_file(std::wstring_view fn)
{
	_stack mem;
	data.save(mem);
	mem << size;
	mem.push_data(&back, sizeof(back));
	mem << udata;
	mem << base_buy;
	mem << base_sale;
	mem.save_to_file(fn);
}

void _compression_stock_statistics::load_from_file(std::wstring_view fn)
{
	_stack mem;
	if (!mem.load_from_file(fn)) return;
	data.load(mem);
	mem >> size;
	mem >> back;
	mem >> udata;
	mem >> base_buy;
	mem >> base_sale;
	read_n = -666;
	ip_last.ok = false;
	ip_n.ok = false;
}

bool _compression_stock_statistics::add0(const _supply_and_demand& c)
{
	ip_last.ok = false;
	data.pushn(c.demand.offer[roffer - 1].price, 16);
	for (i64 i = roffer - 1; i >= 0; i--)
	{
		if (i != roffer - 1)
		{
			if (!f_delta.coding(c.demand.offer[i].price - c.demand.offer[i + 1].price, data)) return false;
		}
		if (!f_number.coding(c.demand.offer[i].number, data)) return false;
	}
	if (!nnd.coding(c.supply.offer[0].price - c.demand.offer[0].price, data)) return false;
	for (i64 i = 0; i < roffer; i++)
	{
		if (i != 0)
		{
			if (!f_delta.coding(c.supply.offer[i].price - c.supply.offer[i - 1].price, data)) return false;
		}
		if (!f_number.coding(c.supply.offer[i].number, data)) return false;
	}
	base_buy.resize(roffer);
	base_sale.resize(roffer);
	for (i64 i = 0; i < roffer; i++)
	{
		base_buy[i] = c.demand.offer[i];
		base_sale[i] = c.supply.offer[i];
	}
	return true;
}

bool _compression_stock_statistics::coding_delta_number(i64 a, i64 b)
{
	if (a <= 66) return ttrr1.coding(b - a, data); // 25 %
	if (a <= 200) return ttrr2.coding(b - a, data); // 25 %
	if (a <= 318) return ttrr3.coding(b - a, data); // 25 %
	return ttrr4.coding(b - a, data); // 25 %
}

bool _compression_stock_statistics::add12(const _offer* v1, std::vector<_offer>& v0, i64 izm)
{
	i64 kk = (v1[1].price > v1[0].price) ? -1 : 1;
	if (!nnds.coding(izm, data)) return false;
	i64 n = 0;
	i64 tip = izm;
	if (izm == 0)
	{
		n = calc_series_value(v1, v0, 0);
		if (!nnse0.coding(n, data)) return false;
		for (i64 i = 0; i < n;)
		{
			i64 ser = calc_series_number(v1, v0, i, n);
			if (!nnse200[n - i].coding(ser, data)) return false;
			i += ser;
			if (i >= n) break;
			if (!coding_delta_number(v0[i].number, v1[i].number)) return false;
			v0[i].number = v1[i].number;
			i++;
		}
	}
	if (izm < 0)
	{
		i64 lv = (v0.begin() - (izm + 1))->price;
		v0.erase(v0.begin(), v0.begin() - izm);
		i64 n2 = calc_series_value(v1, v0, 0);
		if (!nnsegg[std::min((i64)v0.size(), roffer)].coding(n2, data)) return false;
		if (n2 == 0)
		{
			if (!f_delta.coding((lv - v1[0].price) * kk, data))	return false;
			if (!f_number.coding(v1[0].number, data)) return false;
			v0.insert(v0.begin(), v1[0]);
			n = 1;
		}
		else
		{
			n = n2;
			for (i64 i = 0; i < n;)
			{
				i64 ser = calc_series_number(v1, v0, i, n);
				if (!nnse200[n - i].coding(ser, data)) return false;
				i += ser;
				if (i >= n) break;
				if (!coding_delta_number(v0[i].number, v1[i].number)) return false;
				v0[i].number = v1[i].number;
				i++;
			}
			tip = 0;
		}
	}
	if (izm > 0)
	{
		v0.insert(v0.begin(), izm, {});
		for (i64 i = izm - 1; i >= 0; i--) // кодируется как c add0
		{
			if (!f_delta.coding((v1[i].price - v0[i + 1].price) * kk, data)) return false;
			if (!f_number.coding(v1[i].number, data)) return false;
			v0[i] = v1[i];
		}
		n = izm;
		i64 n2 = calc_series_value(v1, v0, n);
		if (!nnsegg[std::min((i64)v0.size(), roffer) - n].coding(n2, data)) return false;
		if (n2 == 0)
			v0.erase(v0.begin() + n);
		else
		{
			n2 += n;
			for (i64 i = n; i < n2;)
			{
				i64 ser = calc_series_number(v1, v0, i, n2);
				if (!nnse200[n2 - i].coding(ser, data)) return false;
				i += ser;
				if (i >= n2) break;
				if (!coding_delta_number(v0[i].number, v1[i].number)) return false;
				v0[i].number = v1[i].number;
				i++;
			}
			n = n2;
			tip = 0;
		}
	}
	while (n < roffer)
	{
		if (n >= (i64)v0.size())
		{
			v0.resize(roffer);
			for (; n < roffer; n++)
			{
				v0[n] = v1[n];
				if (!f_delta.coding((v1[n - 1].price - v1[n].price) * kk, data)) return false;
				if (!f_number.coding(v1[n].number, data)) return false;
			}
			break;
		}
		if (tip == 0)
		{
			i64 buy_izm2 = calc_delta_del_add1(v1, v0, n);
			data.push1(buy_izm2 < 0);
			if (buy_izm2 < 0)
				v0.erase(v0.begin() + n);
			else
			{
				v0.insert(v0.begin() + n, v1[n]);
				if (!f_delta.coding((v1[n - 1].price - v1[n].price) * kk, data)) return false;
				if (!f_number.coding(v1[n].number, data)) return false;
				n++;
			}
			tip = 1;
			continue;
		}
		i64 n2 = calc_series_value(v1, v0, n);
		if (!nnsegg[std::min((i64)v0.size(), roffer) - n].coding(n2, data)) return false;
		if (n2 > 0)
		{
			n2 += n;
			for (i64 i = n; i < n2;)
			{
				i64 ser = calc_series_number(v1, v0, i, n2);
				if (!nnse200[n2 - i].coding(ser, data)) return false;
				i += ser;
				if (i >= n2) break;
				if (!coding_delta_number(v0[i].number, v1[i].number)) return false;
				v0[i].number = v1[i].number;
				i++;
			}
			n = n2;
		}
		tip = 0;
	}
	return true;
}

bool _compression_stock_statistics::add1(const _supply_and_demand& c)
{
	ip_last.ok = true;
	i64 buy_izm = calc_delta_del_add(c.demand.offer, base_buy);
	i64 sale_izm = calc_delta_del_add(c.supply.offer, base_sale);
	if (std::max(abs(buy_izm), abs(sale_izm)) > 12)
	{
		data.push1(0);
		return add0(c);
	}
	data.push1(1);

	std::vector<_offer> bbuy = base_buy;
	std::vector<_offer> bsale = base_sale;

	i64 aa0 = data.size();
	if (!add12(c.demand.offer, bbuy, buy_izm)) return false;
	i64 aa1 = data.size();
	if (!add12(c.supply.offer, bsale, sale_izm)) return false;

	ip_last.r = int(data.size() - aa0);
	ip_last.r_pok = int(aa1 - aa0);
	ip_last.r_pro = int(data.size() - aa1);

	base_buy = std::move(bbuy);
	base_sale = std::move(bsale);
	return true;
}

bool _compression_stock_statistics::add(const _supply_and_demand& c)
{
	if (c == back) return true; // с большой вероятностью данные устарели
	if (c.time < back.time) return true; // цены из прошлого не принимаются!
	auto s_data = data.size();
	if (size % step_pak_cc == 0)
	{
		udata.push_back(data.size());
		data.pushn(c.time, 31);
		if (!add0(c))
		{
			udata.pop_back();
			goto err;
		}
	}
	else
	{
		time_t dt = c.time - back.time;
		if (dt == 1) data.push1(0); else { data.push1(1); data.pushn(dt, 31); }
		if (dt > old_dtime)
		{
			if (!add0(c)) goto err;
		}
		else
		{
			if (!add1(c)) goto err;
		}
	}
	size++;
	back = c;
	return true;
err:
	data.resize(s_data);
	return false; // ошибка кодирования, нужно исправлять!!
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void _cdf::to_clipboard()
{
	std::stringstream a;
	for (auto& i : fr)
	{
		a << "{" << i.first << ", " << (int)i.bit << ", " << i.prefix << "}, ";
	}
	::set_clipboard_text(a.str().c_str());
}

/*double _cdf::calc_size1(const _statistics& st)
{
	i64 s = 0;
	for (auto i : st.data)
	{
		auto n = std::upper_bound(fr.begin(), fr.end(), i.value, [](i64 a, _frequency b) { return (a < b.first); });
		if ((n == fr.begin()) || (n == fr.end())) continue; // return -1;
		n--;
		s += ((i64)n->bit0 + n->bit) * i.number;
	}
	return double(s) / st.number();
}*/

bool _cdf::coding(i64 a, _bit_vector& bs) const noexcept
{
	auto n = std::upper_bound(fr.begin(), fr.end(), a, [](i64 a, _frequency b) { return (a < b.first); });
	if ((n == fr.begin()) || (n == fr.end())) return false;
	if (bst) bst->push(a);
	n--;
	bs.pushn1(n->prefix);
	bs.pushn(a - n->first, n->bit);
	return true;
}

i64 _cdf::decoding(_bit_vector& bs) const noexcept
{
	const _mapf* d = &frd;
	while (d->next[0]) d = d->next[bs.pop1()];
	return d->first + bs.popn(d->bit);
}

_cdf::_cdf(const std::vector<_frequency>& a, _basic_statistics* b) : fr(a), bst(b)
{
	for (i64 i = 0; i < (i64)fr.size() - 1; i++)
	{
		_frequency f = fr[i];
		_mapf* d = &frd;
		while (f.prefix > 1)
		{
			u64 n = f.prefix & 1;
			f.prefix >>= 1;
			if (!d->next[n]) d->next[n] = new _mapf;
			d = d->next[n];
		}
		d->first = f.first;
		d->bit = f.bit;
	}
}

void _cdf::calc(const _statistics& st, i64 n, i64 min_value, i64 max_value)
{
	fr.resize(n + 1);

	struct _uuu
	{
		_iinterval o;
		i64 k = 0;
		uchar bit = 0;
		bool operator==(const _uuu& a) const noexcept { return (o == a.o); }
	};

	auto calc_poteri = [](const _uuu& left, const _uuu& right)
	{
		i64 r0 = right.k * right.bit + left.k * left.bit;
		i64 r = (right.k + left.k) * bit_for_value(right.o.max - left.o.min);
		return r - r0;
	};
	// uu - простые интервалы длиной 1, или с количеством 0
	std::vector<_uuu> ee;
	_uuu a;
	i64 pr;
	if (min_value < st.min_value())
	{
		pr = st.min_value();
		a.o = { min_value, pr };
		a.k = 0;
		a.bit = bit_for_value(a.o.size());
		ee.push_back(a);
	}
	else
		pr = min_value;
	for (auto i : st.data)
	{
		if (i.value > max_value) break;
		if (i.value < min_value) continue;
		if (i.value > pr) // нули
		{
			a.o = { pr, i.value };
			a.k = 0;
			a.bit = bit_for_value(a.o.size());
			ee.push_back(a);
		}
		a.o = i.value;
		a.k = i.number;
		a.bit = 0;
		ee.push_back(a);
		pr = i.value + 1;
	}
	if (max_value >= pr)
	{
		a.o = { pr, max_value + 1 };
		a.k = 0;
		a.bit = bit_for_value(a.o.size());
		ee.push_back(a);
	}

	struct _2uuu
	{
		_uuu u1, u2;
		std::multimap<i64, _2uuu>::iterator left, right;
	};

	std::multimap<i64, _2uuu> xxx;
	// подготовка стартовых пар интервалов отсортированных по минимум потерь при объединении
	std::multimap<i64, _2uuu>::iterator pr_it;
	for (i64 i = 1; i < (i64)ee.size(); i++)
	{
		_2uuu aa;
		aa.u1 = ee[i - 1];
		aa.u2 = ee[i];
		std::multimap<i64, _2uuu>::iterator it = xxx.insert({ calc_poteri(aa.u1, aa.u2), aa });
		if (i == 1)
			it->second.left = it;
		else
		{
			pr_it->second.right = it;
			it->second.left = pr_it;
		}
		pr_it = it;
	}
	pr_it->second.right = pr_it;
	// схлопывание пар интервалов, пока их не останется n-1
	while ((i64)xxx.size() >= n)
	{
		auto a_ = xxx.begin(); // минимальная пара
		auto aa = a_->second;
		auto i = aa.left; // левая пара
		auto j = aa.right; // правая пара
		aa.u1.k += aa.u2.k;
		aa.u1.o.max = aa.u2.o.max;
		aa.u1.bit = bit_for_value(aa.u1.o.size());
		_2uuu ii, jj;
		bool ina = (i != a_);
		bool jna = (j != a_);
		if (ina)
		{
			ii = i->second;
			ii.u2 = aa.u1;
			bool gran = (ii.left == i);
			xxx.erase(i);
			i = xxx.insert({ calc_poteri(ii.u1, ii.u2), ii });
			if (gran)
				i->second.left = i;
			else
				i->second.left->second.right = i;
		}
		if (jna)
		{
			jj = j->second;
			jj.u1 = aa.u1;
			bool gran = (jj.right == j);
			xxx.erase(j);
			j = xxx.insert({ calc_poteri(jj.u1, jj.u2), jj });
			j->second.left = (ina) ? i : j;
			if (gran)
				j->second.right = j;
			else
				j->second.right->second.left = j;
		}
		if (ina)
			i->second.right = (jna) ? j : i;
		xxx.erase(a_);
	}
	// подкотовка отсортированных интервалов
	std::map<i64, _uuu> xxx2;
	for (auto& i : xxx)
	{
		xxx2[i.second.u1.o.min] = i.second.u1;
		xxx2[i.second.u2.o.min] = i.second.u2;
	}
	// заполнение массива частот
	auto ii = xxx2.begin();
	for (i64 i = 0; i < n; i++)
	{
		fr[i].first = ii->second.o.min;
		fr[i].bit = ii->second.bit;
		fr[i].prefix = 1;
		++ii;
	}
	fr[n].first = max_value + 1;
	fr[n].bit = 0;
	fr[n].prefix = 1;
	// коррекция - дозаполнение маленьких интервалов, за счет больших соседей
	for (i64 i = 0; i < n - 1; i++)
	{
		i64 delta = (1ll << fr[i].bit) - (fr[i + 1].first - fr[i].first);
		if (delta == 0) continue;
		if (i > 0)
			if (fr[i].bit < fr[i - 1].bit)
			{
				fr[i].first -= delta;
				fr[i - 1].bit = bit_for_value(fr[i].first - fr[i - 1].first); // вдруг уменьшилось??
				continue;
			}
		if (fr[i].bit <= fr[i + 1].bit)
		{
			fr[i + 1].first += delta;
			fr[i + 1].bit = bit_for_value(fr[i + 2].first - fr[i + 1].first); // вдруг уменьшилось??
			continue;
		}
	}
	// вычисление префикса
	std::multimap<i64, std::vector<i64>> xxx3;
	for (i64 i = 0; i < n; i++) xxx3.insert({ st.number(fr[i].first, fr[i + 1].first), {i} });
	while (xxx3.size() > 1)
	{
		auto p1 = xxx3.begin();
		for (auto i : p1->second) fr[i].prefix <<= 1;
		i64 s = p1->first;
		std::vector<i64> v = p1->second;
		xxx3.erase(p1);
		p1 = xxx3.begin();
		for (auto i : p1->second) fr[i].prefix = (fr[i].prefix << 1) + 1;
		s += p1->first;
		v.insert(v.end(), p1->second.begin(), p1->second.end());
		xxx3.erase(p1);
		xxx3.insert({ s, v });
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

_cdf3::_cdf3(i64 start_, const std::vector<u64>& a, _basic_statistics* b) : start(start_), prefix(a), bst(b)
{
	for (i64 i = 0; i < (i64)prefix.size(); i++)
	{
		u64 f = prefix[i];
		_mapf* d = &frd;
		while (f > 1)
		{
			u64 n = f & 1;
			f >>= 1;
			if (!d->next[n]) d->next[n] = new _mapf;
			d = d->next[n];
		}
		d->first = start + i;
	}
}

void _cdf3::to_clipboard()
{
	std::stringstream a;
	a << start << ", {";
	for (auto& i : prefix) a << i << ", ";
	a << "}";
	::set_clipboard_text(a.str().c_str());
}

bool _cdf3::coding(i64 a, _bit_vector& bs) const noexcept
{
	if ((a < start) || (a - start >= (i64)prefix.size())) return false;
	if (bst) bst->push(a); // для переподбора
	bs.pushn1(prefix[a - start]);
	return true;
}

i64 _cdf3::decoding(_bit_vector& bs) const noexcept
{
	const _mapf* d = &frd;
	while (d->next[0]) d = d->next[bs.pop1()];
	return d->first;
}

void _cdf3::calc(const _statistics& st, i64 min_value, i64 max_value)
{
	start = min_value;
	i64 n = max_value - min_value + 1;
	prefix.resize(n);
	for (auto& i : prefix) i = 1;
	// вычисление префикса
	std::multimap<i64, std::vector<i64>> xxx3;
	for (i64 i = 0; i < n; i++) xxx3.insert({ st[start + i], {i} });
	while (xxx3.size() > 1)
	{
		auto p1 = xxx3.begin();
		for (auto i : p1->second) prefix[i] <<= 1;
		i64 s = p1->first;
		std::vector<i64> v = p1->second;
		xxx3.erase(p1);
		p1 = xxx3.begin();
		for (auto i : p1->second) prefix[i] = (prefix[i] << 1) + 1;
		s += p1->first;
		v.insert(v.end(), p1->second.begin(), p1->second.end());
		xxx3.erase(p1);
		xxx3.insert({ s, v });
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
