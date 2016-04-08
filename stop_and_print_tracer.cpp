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

#include "stop_and_print_tracer.h"
#include <xpcc/utils/arithmetic_traits.hpp>
#include <xpcc/processing/timer.hpp>
#include <xpcc/architecture.hpp>
#include <xpcc/debug/logger.hpp>



class StopAndPrintTracer {
public:
	void trace(const char* newName = nullptr, const char* newDescription = nullptr);
	void printTrace(const char* newName, const char* newDescription);
	static uint32_t
	calculateCycles(const uint32_t startCount, const uint32_t stopCount, const uint32_t offset);
	static uint32_t
	calculateMilliseconds(const xpcc::Timestamp& startTime, const xpcc::Timestamp& stopTime);
	void calibrate();
	inline uint32_t getConstantCycleOffset() { return constant_cycle_offset; }
private:
	xpcc::Timestamp startTime;
	uint32_t cycles = 0;
	uint32_t milliseconds = 0;
	uint32_t constant_cycle_offset = TRACE_CYCLE_OFFSET;
	bool firstTrace = true;
	const char* name = nullptr;
	const char* description = nullptr;
	bool silent = false;
};


// ----------------------------------------------------------------------------
// Set the log level
#undef	XPCC_LOG_LEVEL
#define	XPCC_LOG_LEVEL xpcc::log::DEBUG

volatile uint32_t startValue;
volatile uint32_t stopValue;

void StopAndPrintTracer::trace(const char* newName, const char* newDescription)
{
	const xpcc::Timestamp stopTime = xpcc::Clock::now();

	const uint32_t stopCount  = stopValue;
	const uint32_t startCount = startValue;

	//
	if(firstTrace || this->description == nullptr) {
		firstTrace = false;
	} else {
		cycles = calculateCycles(startCount, stopCount, constant_cycle_offset);
		milliseconds = calculateMilliseconds(startTime, stopTime);
		if(!silent) {
			printTrace(newName, newDescription);
		}
	}
	this->name = newName;
	this->description = newDescription;
	//
	startTime = xpcc::Clock::now();
}

void StopAndPrintTracer::printTrace(const char* newName, const char* newDescription)
{
	// const uint32_t us = cycles / 72;
	XPCC_LOG_DEBUG << cycles << " ";
	if(name && newName) {
		XPCC_LOG_DEBUG << "[" << name << "] -> [" << newName << "] (";
	} else if(name) {
		XPCC_LOG_DEBUG << "[" << name << "] -> [END] (";
	} else if(newName) {
		XPCC_LOG_DEBUG << "[" << newName << "] (";
	}
	XPCC_LOG_DEBUG << milliseconds << "ms)";
	if(description) {
		XPCC_LOG_DEBUG << ": \"" << description << "\"";
	}
	XPCC_LOG_DEBUG << xpcc::endl;
}

uint32_t
StopAndPrintTracer::calculateCycles(const uint32_t startCount, const uint32_t stopCount, const uint32_t offset)
{
	return ((stopCount > startCount)?
			(stopCount - startCount):
			(xpcc::ArithmeticTraits<uint32_t>::max - startCount + stopCount + 1))
			- offset;
}

uint32_t
StopAndPrintTracer::calculateMilliseconds(const xpcc::Timestamp& startTime, const xpcc::Timestamp& stopTime)
{
	return (stopTime - startTime).getTime();
}


StopAndPrintTracer defaultTracer;

extern "C"
void StopAndPrintTracer_trace(const char* newName, const char* newDescription) {
	defaultTracer.trace(newName, newDescription);
}
extern "C"
void StopAndPrintTracer_trace_end(const char* newName) {
	defaultTracer.trace(newName);
}

void StopAndPrintTracer_init() {
	// enable 8MHz Timer2 (on STM32F072 with correct configuration and clock setup...)
	xpcc::stm32::Timer2::enable();
	xpcc::stm32::Timer2::setMode(xpcc::stm32::Timer2::Mode::UpCounter);
	xpcc::stm32::Timer2::setOverflow(0xffffffff);
	xpcc::stm32::Timer2::applyAndReset();
	xpcc::stm32::Timer2::start();
}
