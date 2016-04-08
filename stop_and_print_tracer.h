// coding: utf-8
// -----------------------------------------------------------------------------
/*
 * Copyright (C) 2015 Kevin Laeufer <kevin.laeufer@rwth-aachen.de>
 *
 * This program is free software; you can redistribute it and/or modify it under
 * the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 3 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program; if not, see <http://www.gnu.org/licenses/>.
 */
// -----------------------------------------------------------------------------

#ifndef STOP_AND_PRINT_TRACE_HPP
#define STOP_AND_PRINT_TRACE_HPP

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

extern volatile uint32_t startValue;
extern volatile uint32_t stopValue;

void StopAndPrintTracer_init();
void StopAndPrintTracer_trace(const char* newName, const char* newDescription);
void StopAndPrintTracer_trace_end(const char* newName);



// Settings
// 0x40000024 is the location of the Timer2 Counter register on STM32F072
#define TRACE_COUNTER() *((volatile uint32_t*) 0x40000024)
#define TRACE_CYCLE_OFFSET 14

#ifndef STRINGIFY
#define STRINGIFY(s) STRINGIFY_(s)
#define STRINGIFY_(s) #s
#endif

#define TRACER_FILE_INFO STRINGIFY(BASENAME) "(" STRINGIFY(__LINE__) ")"

#define TRACER_INIT() StopAndPrintTracer_init()

#define TRACE_LINE(description) {                        \
stopValue = TRACE_COUNTER();                             \
StopAndPrintTracer_trace(TRACER_FILE_INFO, description); \
startValue = TRACE_COUNTER();                            \
}

#define TRACE_END() {                                    \
stopValue = TRACE_COUNTER();                             \
StopAndPrintTracer_trace_end(TRACER_FILE_INFO);          \
startValue = TRACE_COUNTER();                            \
}

#ifdef __cplusplus
}
#endif

#endif // STOP_AND_PRINT_TRACE_HPP
