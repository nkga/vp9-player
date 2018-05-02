#pragma once
#include "core.h"
#include <Windows.h>

namespace sys {

struct config_t;
struct frame_t;

struct timer_t {
	timer_t();
	~timer_t();
	i64 query();

protected:
	LARGE_INTEGER freq;
};

}