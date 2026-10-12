*This project has been created as part of the 42 curriculum by ndias-de.*

# 💻 Codexion

> *Master the race for resources before the deadline masters you.* ⏱️🔥

## 🎯 Description

Codexion is a multithreaded resource-sharing simulation written in C with POSIX threads. 🧵 N coders sit in a circle with N USB dongles 🔌 between them (one dongle between each pair of neighbours). To compile, a coder needs **two** dongles at the same time (left and right). The cycle of each coder is:

`🔌 take 2 dongles → ⚙️ compile → 🔓 release dongles → 🐞 debug → 🧹 refactor → 🔁 repeat`

The simulation stops when:

- 🔥 a coder burns out (did not **start** compiling within `time_to_burnout` ms of their previous compile start, or of the simulation start), or
- 🏁 every coder has compiled at least `number_of_compiles_required` times.

When several coders compete for the same dongles, access is arbitrated by a scheduler:

- 📥 **fifo** - the request that arrived first is served first.
- ⏰ **edf** (Earliest Deadline First) - the coder with the earliest deadline (`last_compile_start + time_to_burnout`) is served first.

🧊 After a dongle is released, it cannot be taken again until `dongle_cooldown` ms have passed.

### 🗂️ Project structure

| 📄 File | 🧩 Role |
|------|------|
| `main.c` | Entry point: validates `argc`, parses, initialises, runs, cleans up |
| `parsing.c` | Argument validation (digits only, range check, `fifo`/`edf`) |
| `init_sim.c` | Allocation and initialisation of coders, dongles, mutexes, conditions, heap |
| `simulation.c` | Thread creation/join, simulation stop flag |
| `routine.c` | Life cycle of a coder (compile / debug / refactor), single-coder case |
| `dongles.c`, `dongles_utils.c` | Taking/releasing dongles, cooldown, rival detection, wake-ups |
| `heap.c`, `heap_utils.c` | Min-heap priority queue and the fifo/edf comparison |
| `monitor.c` | Monitor thread: burnout detection and "all done" detection |
| `print_utils.c` | Serialised logging and usage message |
| `time.c` | `get_time`, `ft_usleep`, ms → `timespec` conversion |
| `clean_and_destroy.c` | Destruction of mutexes/conditions and freeing of memory |

---

## 🚀 Instructions

### 🔨 Compilation

```bash
make          # builds ./codexion with cc -Wall -Wextra -Werror -pthread
make clean    # removes object files
make fclean   # removes object files and the binary
make re       # fclean + all
```

### ▶️ Execution

```bash
./codexion number_of_coders time_to_burnout time_to_compile time_to_debug \
           time_to_refactor number_of_compiles_required dongle_cooldown scheduler
```

| 🔢 Argument | 🏷️ Type | 📖 Meaning |
|----------|------|---------|
| `number_of_coders` | integer >= 1 | Number of coders (and dongles) |
| `time_to_burnout` | integer >= 1 (ms) | Max time between two compile starts |
| `time_to_compile` | integer >= 0 (ms) | Duration of a compile (2 dongles held) |
| `time_to_debug` | integer >= 0 (ms) | Duration of debugging |
| `time_to_refactor` | integer >= 0 (ms) | Duration of refactoring |
| `number_of_compiles_required` | integer >= 1 | Compiles each coder must reach for the simulation to end |
| `dongle_cooldown` | integer >= 0 (ms) | Time a released dongle stays unavailable |
| `scheduler` | `fifo` or `edf` | Arbitration policy (exact, lowercase) |

⚠️ All values must be plain non-negative decimal integers that fit in an `int`. Negative numbers, decimals, empty strings, values above `2147483647`, a wrong number of arguments or any other scheduler name are rejected with a message on `stderr` and exit code `1`.

### 💡 Examples

```bash
./codexion 4 800 150 150 150 5 10 fifo
./codexion 5 2000 200 200 200 3 200 edf
./codexion 1 300 100 100 100 3 0 fifo     # single coder: takes one dongle and burns out
```

### 📜 Log format

```
timestamp_in_ms X has taken a dongle
timestamp_in_ms X is compiling
timestamp_in_ms X is debugging
timestamp_in_ms X is refactoring
timestamp_in_ms X burned out
```

The timestamp is in milliseconds since the start of the simulation, `X` is the coder number (1 to `number_of_coders`). Each compile is preceded by two `has taken a dongle` lines. Once the simulation is stopped, no other line is printed after `burned out`.

---

## 🛡️ Blocking cases handled

### 🔒 Deadlock prevention (Coffman's four conditions)

| ⚖️ Condition | 🛠️ How it is handled |
|-----------|-------------------|
| **Mutual exclusion** | A dongle has a `taken` flag, only read/written while holding `global_mutex`. This condition is inherent to the problem and is kept. |
| **Hold and wait** ✅ | Removed. In `take_dongles` a coder checks **both** dongles inside one critical section and takes both together or none. A coder never holds one dongle while waiting for the other. |
| **No preemption** | Kept by design: dongles are released only by their owner in `put_dongles`. This is safe because the other conditions are broken. |
| **Circular wait** ✅ | Impossible: since nobody waits while holding a dongle, no cycle of "holds A, waits for B" can form. No lock ordering of dongles is needed. |

🔗 To avoid deadlocks between **mutexes**, a fixed nesting order is respected everywhere:
`global_mutex` → `print_mutex` → `stop_sim_mutex`. The per-coder mutex (`coder->mutex`) is never held while another mutex is taken.

### 🍽️ Starvation prevention

- 📥 **fifo:** each request receives a ticket (`request_counter`, incremented under `global_mutex`). The heap is ordered by this ticket, so arrival order is respected even if two requests happen in the same millisecond.
- ⏰ **edf:** the heap is ordered by `last_compile_start + time_to_burnout`. Ties are broken by the request ticket, so the order is always deterministic.
- 🥇 **Rival rule (`has_rival`):** a coder does not take a dongle while a waiting neighbour (one who shares a dongle) has higher priority **and** both of that neighbour's dongles are currently free. This stops a lower-priority coder from "stealing" a dongle from the coder that should go first.
- ❤️ **Liveness:** with feasible parameters, the coder closest to burnout is always served first under `edf`, and no coder is overtaken indefinitely under `fifo`.

### 🧊 Cooldown handling

When a coder releases dongles, `put_dongles` stores the release time (`last_release`) in both dongles. `is_available` only accepts a dongle if `now - last_release >= dongle_cooldown`. While waiting only for a cooldown to expire (dongles free, no rival), the coder sleeps with `pthread_cond_timedwait` up to the moment the cooldown ends, instead of busy-waiting. In every other case the coder sleeps with `pthread_cond_wait` and is woken by the neighbour that releases a shared dongle.

### 🎯 Precise burnout detection

A dedicated monitor thread 👀 loops over all coders every 100 microseconds (`usleep(100)`) and checks `now - last_compile_start >= time_to_burnout`. On a burnout it first sets the stop flag and then prints `burned out`. In our tests with `time_to_burnout = 200`, the log appeared with a delay of 0-1 ms, well inside the 10 ms limit.

### 🖨️ Log serialisation

Every line is printed while holding `print_mutex`, so two messages never mix. `print_status` re-checks the stop flag **inside** the print mutex, which guarantees that no state message is printed after the `burned out` line.

### 🧩 Other cases

- 1️⃣ **One coder:** there is a single dongle; the coder takes it, can never compile, and burns out after `time_to_burnout` ms.
- 🛑 **Clean shutdown:** `set_simulation_stop` sets the flag and signals every coder's condition variable, so coders sleeping in `pthread_cond_wait` wake up and leave. `ft_usleep` sleeps in slices (at most 100 ms) and checks the stop flag between slices, so long sleeps do not delay the end of the program.
- 🧵 **Thread creation failure:** `start_sim` stops the simulation and joins only the threads that were created.

---

## 🔗 Thread synchronization mechanisms

### 🔐 Mutexes

| 🔒 Mutex | 🛡️ Protects |
|-------|----------|
| `rules->global_mutex` | Dongle state (`taken`, `last_release`), the wait queue (heap) and `request_counter` |
| `rules->print_mutex` | Output on `stdout` (one line at a time) |
| `rules->stop_sim_mutex` | The `sim_stop` flag |
| `coder->mutex` (one per coder) | `last_compile_start` and `compiles_done`, written by the coder and read by the monitor |

### 📣 Condition variables

- One `wake_up` per coder. It is always waited on together with `global_mutex` (`pthread_cond_wait` / `pthread_cond_timedwait` atomically release the mutex while sleeping and take it again on wake-up).
- A coder is woken up by: a neighbour releasing a shared dongle (`wake_rivals`), the end of a cooldown (timeout), or the stop of the simulation (`set_simulation_stop`).
- Wake-ups can be spurious or early, so the wait is always inside `while (!is_available(...) && !is_simulation_stopped(...))`: the condition is re-checked every time.

### 🌳 Priority queue

A single min-heap (`wait_queue`, capacity = `number_of_coders`) holds the coders that are waiting for dongles. `heap_insert` and `heap_remove` are O(log n) after the position is known (`heap_remove` first searches the coder linearly). The comparison `has_priority` uses the key of the selected scheduler (ticket for fifo, deadline for edf) and the ticket as tie-breaker. The heap is only accessed with `global_mutex` held.

### 🏁 Examples of race conditions prevented

**1️⃣ Two coders taking the same dongle.** The check "both dongles free" and the assignment `taken = true` happen in the same critical section of `take_dongles`. No other thread can slip in between the test and the take.

**2️⃣ Lost wake-up.** A coder only goes to sleep after checking the condition while holding `global_mutex`, and the condition wait releases that mutex atomically. `put_dongles` changes the state and signals while holding the same mutex, so the signal cannot happen "between" the check and the sleep.

**3️⃣ Monitor reading data being written by a coder.** `last_compile_start` and `compiles_done` are always accessed with the coder's own mutex. The monitor copies the value under the lock and compares outside of it.

**4️⃣ Output after burnout.** The stop flag is set before `burned out` is printed, and `print_status` checks the flag again inside `print_mutex`.

**5️⃣ Stop signal vs. sleeping coders.** The flag is written under `stop_sim_mutex` and read through `is_simulation_stopped`; after setting it, every coder is signalled under `global_mutex`, so nobody stays asleep.

### 💬 Communication between coders and monitor

Coders never talk to each other. 🤐 The monitor reads `last_compile_start` and `compiles_done` of each coder (under the coder's mutex) and communicates back only through the `sim_stop` flag, which every coder checks in its loop, in `ft_usleep`, in `take_dongles` and before printing.

---

## 🧪 Testing

The project was compiled and tested with the following checks:

| 🔬 Test | ✅ Result |
|------|--------|
| `make`, then `make` again | No warnings; no relink on the second run |
| 11 invalid argument sets (no args, too few/many, negative, decimal, zero, `> INT_MAX`, `FIFO`, unknown scheduler) | All rejected, exit code 1 |
| 1 coder | Takes one dongle, `burned out` at `time_to_burnout` |
| 2, 3, 4, 5 coders, fifo and edf, with and without cooldown | End normally when every coder reaches the required compiles |
| 200 and 201 coders | End normally; every log line matches the required format |
| 30 runs of `4 600 100 100 100 4 0` (15 fifo + 15 edf) | No burnout |
| Forced burnout (`4 200 150 150 150 5 0 fifo`) | `burned out` printed with 0-1 ms of delay |
| `valgrind --leak-check=full` (fifo, edf with burnout, 1 coder) | 0 errors, no leaks |
| `valgrind --tool=helgrind` (4 coders, edf) | 0 errors |

---

## 📚 Resources

**📖 References**

- POSIX Threads specification: https://pubs.opengroup.org/onlinepubs/9699919799/
- Man pages: `pthread_create(3)`, `pthread_mutex_lock(3)`, `pthread_cond_wait(3)`, `pthread_cond_timedwait(3)`, `gettimeofday(2)`
- Coffman conditions: https://en.wikipedia.org/wiki/Deadlock
- Earliest deadline first scheduling: https://en.wikipedia.org/wiki/Earliest_deadline_first_scheduling
- Binary heap / priority queue: https://en.wikipedia.org/wiki/Binary_heap
- Herlihy & Shavit, *The Art of Multiprocessor Programming*

**🤖 Use of AI**

AI was used as a mentor and reviewer, not as a code generator for the final solution:

- 🧠 Explanation of concepts (mutexes, condition variables, Coffman's conditions, heaps).
- 🐛 Race condition and deadlock analysis and debugging suggestions.
