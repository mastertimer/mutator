#include "basic_tetrons.h"

void _one_tetron::push(_wjson& b)
{
	_tetron::push(b);
	b.add_mem("a", c, 128);
}

void _one_tetron::pop(_rjson& b)
{
	_tetron::pop(b);
	b.read_mem("a", c, 128);
}
