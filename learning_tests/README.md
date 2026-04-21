# Concurrency learning tests for Codexion

These small programs are isolated sandboxes to learn the core ideas required by the subject.

## Build everything

- `make`

## Programs

- `./01_thread_lifecycle`
  - Basic `pthread_create`, `pthread_join`, and detached thread usage.
- `./02_mutex_counter`
  - Shows race conditions and how `pthread_mutex_lock` fixes them.
- `./03_dongle_locking`
  - Simulates coders/dongles and compares unsafe vs safe lock ordering.
- `./04_monitor_and_scheduler`
  - Demonstrates condition variables, scheduler wakeups, and burnout monitoring.

## Notes

- Edit constants at the top of each file to experiment quickly.
- Each concept is kept in separate functions for easy learning and rewrites.
