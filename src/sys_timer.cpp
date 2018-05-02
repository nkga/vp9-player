#include "sys_timer.h"
#include <Windows.h>
#include <mmsystem.h>

namespace sys {

timer_t::timer_t() {
	timeBeginPeriod(1);
	QueryPerformanceFrequency(&freq);
}

timer_t::~timer_t() {
	timeEndPeriod(1);
}

i64 timer_t::query() {
	LARGE_INTEGER tick;
	QueryPerformanceCounter(&tick);
	return (tick.QuadPart * 1000000ll) / freq.QuadPart;
}

}
