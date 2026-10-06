*This project has been created as part of the 42 curriculum by ndias-de*

## 🎯 Description

Codexion is a multithreaded resource-sharing simulation written in C. Multiple coders (threads) compete for limited USB dongles (shared resources) needed to compile quantum code.

**Core concepts:**
- 🔒 Thread synchronization (mutexes, condition variables, condition variables with timed waits)
- ⛓️ Deadlock prevention (lock ordering, atomic operations, careful unlock sequencing)
- 🏃 Starvation prevention (FIFO/EDF scheduling with priority heaps)
- ⚡ Resource arbitration with fair access and cooldown periods
- 🚨 Burnout detection with ±10ms precision via dedicated monitor thread

Coders cycle: compile (2 dongles) → debug → refactor → repeat. Simulation stops when all coders compile enough times or someone burns out.

---

## 📦 Instructions

### Compilation
```bash
make
```
**Flags:** `-Wall -Wextra -Werror -pthread` via `cc`

### Execution
```bash
./codexion <number_of_coders> <time_to_burnout> <time_to_compile> <time_to_debug> <time_to_refactor> <number_of_compiles_required> <dongle_cooldown> <scheduler>
```

**Example:**
```bash
./codexion 4 3000 200 200 200 5 0 fifo
./codexion 5 2000 200 200 200 3 200 edf
```

---

## 📚 Resources

**References:**
- POSIX Threads: https://pubs.opengroup.org/onlinepubs/9699919799/
- "The Art of Multiprocessor Programming" — Herlihy & Shavit (Ch. 2-3)
- Man pages: `pthread_mutex_lock(3)`, `pthread_cond_timedwait(3)`, `clock_gettime(3)`
- Coffman's Conditions: https://en.wikipedia.org/wiki/Deadlock
- Min-Heap implementation patterns for priority queues

**AI Usage**
- Heap implementation and heapify operations
- Race condition debugging and identification (particularly in multi-coder scenarios)
- Deadlock analysis and prevention strategies
- Stack trace analysis and memory safety verification

---

## 🛡️ Blocking Cases Handled

### Deadlock Prevention (Coffman's Four Conditions)

| Condition | Solution |
|-----------|----------|
| **Mutual Exclusion** | Mutex on each dongle (necessary for resource protection) |
| **Hold and Wait** | Insert coder in queue → release dongle → sleep; reacquire both atomically or nothing |
| **Circular Wait** | Lock by dongle ID ascending: `if (d1->id < d2->id)` always lock d1 first |
| **No Preemption** | Atomic check-and-take inside locked section; no interrupt between test and acquire |

### Starvation Prevention

- **FIFO:** Sequence counter per dongle (incremented inside lock during insert); ensures deterministic order even when multiple coders request at same millisecond
- **EDF:** Min-heap ordered by deadline (`last_compile_start + time_to_burnout`); tie-break by coder ID for deterministic ordering under equal deadlines
- **Liveness guarantee:** Under EDF with feasible parameters, coders nearest burnout are always served first from queue

### Additional Race Conditions Addressed

- 🧊 **Cooldown Handling:** `pthread_cond_timedwait()` wakes when first dongle's cooldown expires; timeout capped at 100ms to prevent stale waits
- 🎯 **Burnout Detection:** Dedicated monitor thread polls every 500µs; burnout log printed within ±10ms of actual deadline miss
- 📝 **Log Serialization:** Single `print_mutex` ensures multi-line "has taken dongle" + state transitions are atomic (no interleaved output)
- ⏱️ **Timing:** `clock_gettime(CLOCK_REALTIME)` for millisecond precision across all state transitions
- 🔒 **NULL Pointer Dereference in EDF:** Heap nodes checked for NULL before access in `can_acquire()` to prevent crashes when queue is corrupt or unsynchronized

---

## 🔗 Thread Synchronization Mechanisms

### Synchronization Primitives

**Mutexes:**
- **Per-dongle (N total):** Protect `taken` status, `last_release` timestamp, and `queue` heap
- **Per-coder (N total):** Protect `last_compile_start`, `compiles_done`, and `notified` flag
- **Global (2 total):** `print_mutex` (serializes output), `state_mutex` (protects `sim_stop` flag)

**Condition Variables (N total):**
- One per coder (`wake_up`): signals when dongle available or simulation stops
- Always paired with coder's own mutex (invariant: `pthread_cond_wait/timedwait` releases and re-acquires `coder->mutex`)

**Priority Queues (N total):**
- Min-heap per dongle (capacity = `number_of_coders * 2`)
- **FIFO mode:** key = insertion timestamp, tie-break = sequence counter
- **EDF mode:** key = deadline (last_compile_start + time_to_burnout), tie-break = coder ID
- Supports O(log n) insert and extract operations

### Race Condition Prevention Patterns

**Pattern 1: TOCTOU (Time-of-Check to Time-of-Use)**
- Releaser (put_dongles):
- ├─ Lock d1, d2
- ├─ Read top of queue (next1, next2) while locked
- └─ Unlock → call notify_coders(next1, next2)

- Waiter (take_dongles):
- ├─ Lock coder->mutex
- ├─ If !notified: pthread_cond_timedwait() [internally unlocks coder->mutex]
- ├─ Wake: coder->notified == true
- ├─ Clear notified = false
- └─ Unlock, re-lock d1/d2 for full recheck
Result: Signal cannot be missed (set before wake, cleared after)

**Pattern 3: Heap Corruption Prevention (EDF-specific)**
- Before accessing heap top:
└─ Check: if (!queue->nodes[0].coder) return false
Result: Prevents NULL dereference if heap became inconsistent

### Liveness Properties

- **FIFO:** No starvation if parameters allow. Coders served in arrival order.
- **EDF:** No starvation if parameters are feasible (all deadlines can be met). Coders nearest burnout always served first.
- **Cooldown:** Prevents resource oscillation; coders cannot re-acquire same dongle until cooldown expires.
- **Monitor:** Detects burnout within 10ms; stops simulation instantly to prevent further execution.

---

## ✅ Implementation Notes

- ✔️ All heap memory freed (valgrind/leaks clean)
- ✔️ No global variables (all state in `t_rules` and `t_coder` structs)
- ✔️ Compiles with `-Wall -Wextra -Werror -pthread` (zero warnings)
- ✔️ Norm 42 compliant
- ✔️ Tested with 2, 3, 4, and 5 coders across FIFO and EDF schedulers
- ✔️ Concurrent access via min-heap ensures fair scheduling under contention