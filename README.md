# Watchdog

**Watchdog** is a A lightweight system utility designed to monitor and automatically restart a target process if it crashes or becomes unresponsive.

---

## Motivation

This project was created to ensure the stability and continuity of user applications during critical sections of execution.

---

## Prerequisites

- Linux OS

---

## API

### `StartWD`

```c
wd_status_t WDStart(int argc, const char* argv[], size_t interval, int tolerance);
```

**Description:**\
Initializes the watchdog mechanism.

**Parameters:**

- `argc` — Argument count for the watchdog process.
- `argv` — Argument list for the watchdog process.
- `tolerance` — Number of consecutive missed signals allowed before revival starts.
- `interval` — Interval (in secons) between signals sent between the two processes.

**Returns:**

- WD_SUCCESS on success
- WD_FAIL on failure

---

### `StopWD`

```c
void WDStop();
```

**Description:**\
Stops the watchdog cleanly, sends termination signals to both processes, and frees resources.

---

## Setup & Usage

### Build Instructions

```bash

cd Watchdog
make
```

executables files:

1. test_wd.out
2. wd_process.out

---

### Running the Program

Basic usage:

```bash
./test_wd.out
```

---

## How It Works

1. The user app starts the watchdog using `WDStart()`, specifying signal interval, tolerance, and args.
2. The app forks and runs `wd_process.out`, while also starting a worker thread.
3. `Origin` processes send `SIGUSR1` signal and the `wd_process` send `SIGUSR2` each other at the specified interval.
4. If a process misses `tolerance` signals, the other process revives it:
   - The user app restarts the watchdog.
   - The watchdog takes over and restarts the user app.
5. Calling `WDStop()` shuts down both processes and cleans up.

## Pay attention - the program used SIGUSR1 and SIGUSR2 during it's operation. in addition, in case of fail during activate the program, it's used SIGTERM to terminate the process.
