# Stop and Print Tracer

from my thesis:

## Measurement Technique

The measurements presented in the following sections aim at building a better understanding of how much CPU time every component
of the mote runtime needs.
To be able to evaluate the `awap` code without interference from the underlying operating system, all tests were
performed using the xpcc microcontroller library to provide a minimal support system, that echos packets to the local `awap`
system and implements timeouts using timestamps and a fake clock, that can be incremented by a function call.
Global interrupts are disabled in order to archive a deterministic system behavior.

All measurements were made on a STM32F072RB microcontroller with the Cortex-M0 core and the central memory bus clocked at 8MHz.
I used a `stop and print tracer` to keep track of the cycles used.
The idea behind this system is to save the value of a hardware timer to the global `stopTime` variable, then compute the number of
cycles elapsed, print the result, and just before returning to the user code, save the value of the hardware timer to the global `startTime` variable.
This functionality is available through a C macro:

~~~{.c}
#define TRACE_LINE(description) {                        \
stopValue = TRACE_COUNTER();                             \
StopAndPrintTracer_trace(TRACER_FILE_INFO, description); \
startValue = TRACE_COUNTER();                            \
}
~~~

The number of cycles needed to store the values was calibrated by measuring the cycles for the execution of 40 move operations, that take one cycle each.
Unfortunately, the cycles needed to access the global variables can depend on the register usage in the previous code and are not always constant.
Trying to put more register pressure on the compiler in the calibration code resulted in two additional cycles.
Tracing the `awap` code, uncertainties of about five cycles per call to the trace function were observed.
Running at 8MHz, one cycle corresponds to 125ns, at 48MHz, one cycle takes about 21ns.
As most traces are concerned with several 1000  or 10000 cycles, an uncertainty of five or ten cycles is acceptable.

Instead of performing certain tasks over and over again, as is done for traditional benchmarks on e.g., a personal computer, I only measure once,
because with disabled interrupts and without any external input, as agent message are just echoed back to the local agents, the system is very deterministic.
Only the garbage collector can sometimes change measurements, thus it will be indicated, when the garbage collector was run.

