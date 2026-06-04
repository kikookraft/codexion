*This project has been created as part of the 42 curriculum by tobesson.*

## Description

`codexion` is a multi-threaded simulation in C where coders compete for limited USB
dongles to compile code. Each coder runs as a thread and must acquire
two adjacent dongles before compiling. The simulation enforces a dongle cooldown and
arbitrates access using either FIFO or EDF scheduling. A monitor thread detects burnout
when a coder exceeds its time limit without compiling. The program stops when every
coder has compiled the required number of times, or when a burnout occurs.

## Instructions

```
make
./codexion <coders> <burnout> <compile> <debug> <refactor> <compiles> <cooldown> <fifo|edf>
```

| Argument | Description |
|---|---|
| coders | Number of coders and dongles (min 1) |
| burnout | Max ms without starting a compile |
| compile | Compile duration in ms |
| debug | Debug duration in ms |
| refactor | Refactor duration in ms |
| compiles | How many times each coder must compile |
| cooldown | Dongle cooldown in ms after release |
| fifo/edf | Scheduler policy |

Example:
```
./codexion 5 200 50 100 100 3 10 fifo
```

## How it works

### Startup
1. Arguments are parsed and validated (all must be positive, scheduler must be `fifo` or `edf`).
2. The simulation structure is allocated, dongles are initialized with their own mutex and condition variable.
3. Coder threads are created, each with an odd/even dongle ordering to prevent deadlock.
4. The burnout monitor thread is started.

### Coder lifecycle (each iteration)
1. **Acquire left dongle** — locks the dongle mutex, enqueues the coder in a priority queue sorted by EDF deadline or FIFO arrival. If the dongle is busy or the coder is not first in queue, the thread waits on the condition variable. If the coder just used this dongle and another coder is waiting, it yields its position.
2. **Acquire right dongle** — same process. If it fails, the left dongle is released.
3. **Compile** — both dongles are held for `compile` ms. `last_compile_start` is updated, resetting the burnout timer.
4. **Release dongles** — both dongles are freed with a timestamp for cooldown tracking, then a broadcast wakes waiting coders.
5. **Debug** — wait for `debug` ms.
6. **Refactor** — wait for `refactor` ms.
7. Loop until `compiles` iterations are done or the simulation stops.

### Dongle priority queue
Each dongle holds a 2-slot waitlist sorted by the active scheduler:
- **FIFO**: first to enqueue is served first.
- **EDF**: the coder with the earliest burnout deadline (`last_compile_start + burnout`) is served first. Equal deadlines break by lower coder ID.

### Burnout detection
A dedicated monitor thread loops every 100 µs, checking every coder. A coder burns out if it is not currently waiting for a dongle and the time since its last compile start exceeds the burnout limit. On detection, the simulation is stopped: `is_running` is set to 0 under `sim_lock`, the burnout message is printed under `print_lock`, and all dongle condition variables are broadcast so waiting threads can exit.

### Shutdown
After all coder threads have joined, the main thread checks whether `is_running` is still true — meaning everyone finished their compiles before anyone burned out — and prints a success message. All mutexes and condition variables are destroyed, and memory is freed.

## Concurrency handling

### Deadlock prevention
Coders with even IDs grab their left dongle first, odd IDs grab their right first. This asymmetric ordering prevents circular wait.

### Starvation prevention
When a coder releases a dongle and immediately tries to reacquire it while another coder is waiting, it moves itself to the back of the queue. The dongle cooldown ensures no coder can monopolize a resource.

### Cooldown enforcement
After release, a dongle records a `last_used` timestamp. Any coder attempting to take the dongle must wait until `cooldown` ms have elapsed, using `pthread_cond_timedwait`.

### Log serialization
All state change messages go through a single `print_lock` mutex, guaranteeing no two messages interleave on stdout.

### Precise burnout detection
The monitor thread checks burnout conditions under `sim_lock`. The burnout log is printed under `print_lock`, and `is_running` is set to 0 before broadcasting dongle condition variables. Every state-change function (`compile`, `debug`, `refactor`, `take_dongle_finish`) re-checks `is_running` under `print_lock` before printing, preventing stale messages from appearing after a burnout log.

### Race condition on `is_running`
The `is_running` flag is declared `volatile` and is read through `is_simulation_running()`, which acquires `sim_lock`. The shutdown sequence writes `is_running = 0` under both `sim_lock` and each `dongle_lock`, then broadcasts on the dongle condition variables. This ensures that threads blocked in `pthread_cond_wait` see the updated value when they wake up and re-evaluate their while conditions.

## Thread synchronization mechanisms

- `pthread_mutex_t` for each dongle (`dongle_lock`), the simulation state (`sim_lock`), each coder's internal state (`coder_lock`), and log output (`print_lock`).
- `pthread_cond_t` per dongle (`dongle_cond`) for signaling waiting coders when a dongle becomes available.
- `pthread_cond_wait` to block a coder until a dongle is free and it is first in queue.
- `pthread_cond_timedwait` to enforce the dongle cooldown without busy-waiting.
- `pthread_cond_broadcast` in `release_dongle` and `end_simulation` to wake all waiters.

Coders communicate with the monitor implicitly through shared state: `is_waiting` tells the monitor to exclude a coder from burnout checks while it is queued for a dongle. The monitor communicates shutdown to coders via `is_running` and dongle broadcasts.

## AI usage

AI was used to fix concurrency issues: race conditions on `is_running`, deadlock detection in the burnout monitor and Readme.

