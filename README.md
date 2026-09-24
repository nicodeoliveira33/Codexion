*This project has been created as part of the 42 curriculum by ndias-de.*


# 💻 Codexion

## 📖 Description
**Codexion** is a concurrency simulation project developed in C that models a group of coders sitting around a shared table, competing for limited resources (dongles) to perform essential development phases: *compiling*, *debugging*, and *refactoring*. 

The core goal of the project is to master multi-threading in C, ensuring that all coders can complete their required compilation tasks without running into **deadlocks**, **resource starvation**, or unexpected **burnouts** (time-outs without activity). The simulation relies on precise timing, mutex locks, condition variables, and custom scheduling policies (**FIFO** and **EDF**).

---

## 🚀 Instructions

### ⚙️ Compilation & Makefile Rules

To compile the project, run the provided `Makefile` at the root of the repository using the rules below depending on your needs:

* **`make` (or `make all`):** Compiles all source files using strict flags (`-Wall -Wextra -Werror -pthread`), generates necessary object files, and links them to produce the final executable `codexion`. It is optimized to prevent unnecessary relinking.
* **`make clean`:** Removes all temporary object files (`.o`) generated during the compilation process, cleaning up the working directory.
* **`make fclean`:** Performs a deep cleanup by removing both the temporary object files (`.o`) and the compiled binary executable (`codexion`).
* **`make re`:** Acts as a shortcut for `fclean` followed immediately by `make`, forcing a complete re-compilation of the entire project from scratch.

### 🎮 Execution
The program takes 8 mandatory arguments:
```bash
./codexion [number_of_coders] [time_to_burnout] [time_to_compile] [time_to_debug] [time_to_refactor] [number_of_compiles_required] [dongle_cooldown] [scheduler]
```

* **number_of_coders**: Total number of coders (and dongles) at the table.
* **time_to_burnout (ms)**: Maximum time a coder can survive without starting a new compilation phase.
* **time_to_compile (ms)**: Time it takes to complete a compilation phase.
* **time_to_debug (ms)**: Time it takes to complete a debugging phase.
* **time_to_refactor (ms)**: Time it takes to complete a refactoring phase.
* **number_of_compiles_required**: Target number of successful compilations per coder to finish the simulation.
* **dongle_cooldown (ms)**: Mandatory cooldown period before a recently released dongle can be reused.
* **scheduler**: Scheduling policy to manage resource queues (`fifo` or `edf`).

**Examples:**
```bash
./codexion 4 800 200 100 100 5 10 fifo
./codexion 4 800 200 100 100 5 10 edf
```

---

## 🛡️ Blocking Cases Handled

Our implementation addresses and mitigates several critical concurrency challenges:

* **Deadlock Prevention & Coffman’s Conditions:** Deadlocks around the circular table are completely eliminated by enforcing a strict resource acquisition order based on the IDs of each coder's adjacent dongles (always acquiring the lower-ID dongle first between left and right), preventing circular wait conditions.
* **Starvation Prevention:** Handled via priority queues managed by Min-Heaps, ensuring that coders waiting the longest (FIFO) or with the tightest deadlines (EDF) get fair access to resources.
* **Cooldown Handling:** Integrated a rigorous timestamp check (`last_released + dongle_cooldown`) during resource acquisition to ensure dongles cannot be instantly re-grabbed.
* **Precise Burnout Detection:** A dedicated monitor thread continuously checks each coder's last active timestamp against `time_to_burnout`, ensuring exact, non-delayed termination when a burnout occurs.
* **Log Serialization:** Output logs are protected by a dedicated print mutex to prevent race conditions and interleaved or corrupted prints on standard output.

---

## 🧵 Thread Synchronization Mechanisms

Concurrency control in Codexion is built using POSIX threads (`pthreads`) and low-level synchronization primitives:

* **`pthread_mutex_t` (Mutexes):** 
  * Each dongle features its own mutex to control access and modification of its state (`taken`, `last_released`).
  * A global print mutex serializes access to `stdout`, preventing race conditions when multiple threads attempt to print status logs simultaneously.
  * Shared simulation flags (such as stop triggers) are protected to ensure consistent reads across threads.
* **`pthread_cond_t` (Condition Variables):** 
  * Used in conjunction with mutexes to put waiting threads to sleep efficiently during resource unavailability or cooldown periods, avoiding busy-waiting and heavy CPU usage.
* **Thread Communication & Coordination:** 
  * **Coders $\leftrightarrow$ Monitor:** Coders update their structural state variables (timestamps, phase changes) under mutex protection. The monitor thread polls these states safely to evaluate health metrics and coordinate global shutdowns.
  * **Race Condition Mitigation:** Every shared data modification (such as state flags or resource availability) is strictly enclosed within critical sections defined by appropriate mutex locks.

---

## 📚 Resources & AI Usage

### References
* POSIX Threads Programming Documentation (`pthread` man pages).
* The Dining Philosophers Problem (Edsger Dijkstra).
* Advanced Programming in the UNIX Environment (W. Richard Stevens).

### 🤖 AI Usage Disclosure
Artificial Intelligence (AI) tools were utilized during the development of this project as a collaborative programming assistant for:
* **Understanding New Concepts & Tools:** Exploring advanced multi-threading paradigms, condition variables, and custom scheduling architectures.
* **Debugging Concurrency Logic:** Analyzing complex race conditions and edge cases (such as single-coder behaviors and deadlock scenarios).
