# WATCHDOG - watchdog timer for critical code.

DESCRIPTION

Watchdog is a program that verifies the execution of a specified piece of code.

FEATURES

The program will start after call WDStart() before the critical piece of code.
The program ensure the crirical code is running until call WDStop function.
If the critical code will terminated from any reason - the WD program will re-run
it from the beggining.  

INSTALLATION

- GCC compiler
- Linux-based system

FILES
- wd.so
- wd.h
- wd_process.out

STEPS
Attach the files to your compilation command. Add WDStart function (add the relevent
arguments according to the API) before your critical piece of code, and WDStop after
your critical piece of code.

Pay attention - the program used SIGUSR1 and SIGUSR2 during it's operation.
		in addition, in case of fail during activate the program, it's used SIGTERM to terminate the process.

LICENSE
This project is licensed under the Infinity Labs License, and (c) Elyashiv Horden.


