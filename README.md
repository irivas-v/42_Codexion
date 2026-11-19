*This project has been created as part of the 42 curriculum by irivas-v.*

# Codexion

A high-performance multithreaded concurrency simulation built in C (C89), modeling resource contention, dynamic priority scheduling, and deadlock avoidance among concurrent processes.

---

## Description

**Codexion** is an advanced concurrency project at 42 Madrid, extending the classical Dining Philosophers problem by introducing real-world distributed system constraints: shared hardware devices (dongles), priority scheduling via Min-Heap priority queues, hardware cooldown periods, and real-time burnout monitoring.

### Problem Overview
- **Coders and Hardware Keys**: $N$ coders ($N \ge 1$) sit in a ring. Between each pair of coders lies a single shared hardware dongle (total $N$ dongles). To compile their code, a coder must simultaneously hold two dongles: their left dongle and their right dongle.
- **Coder Life Cycle**: Each coder continuously cycles through three states:
  1. **Compiling**: The coder holds both dongles and compiles for `time_to_compile` ms.
  2. **Debugging**: Upon releasing both dongles, the coder debugs for `time_to_debug` ms.
  3. **Refactoring**: The coder refactors their code for `time_to_refactor` ms before competing for dongles again.
- **Burnout Mechanism**: If a coder does not start compiling within `time_to_burnout` ms from the start of their last compilation (or simulation start), they suffer a burnout (`burned out`) and the simulation terminates immediately.
- **Dongle Queuing & Priority Schedulers**: Contention on each dongle is managed through a priority wait queue implemented as a binary Min-Heap. Two scheduling policies are supported:
  - **FIFO (First-In, First-Out)**: Prioritizes coders based on request arrival timestamp.
  - **EDF (Earliest Deadline First)**: Prioritizes coders based on their imminent burnout deadline (`last_compile_time + time_to_burnout`), preventing starvation of threatened threads.
- **Dongle Cooldown**: After being released, a dongle enters a cooldown state for `dongle_cooldown` ms, during which it cannot be assigned to any waiting coder.
- **Constraints**: Written strictly in C89 complying with 42 Norm 4.1, with zero global variables, zero memory leaks, zero data races, and deterministic deadlock freedom.

---

## Instructions

### Compilation
The project includes a standard `Makefile` compiling with flags `-Wall -Wextra -Werror -pthread` without relinking:

```bash
# Compile the executable (codexion)
make

# Remove intermediate object files
make clean

# Remove object files and the binary
make fclean

# Recompile the complete project
make re
```

### Execution Syntax
```bash
./codexion number_of_coders time_to_burnout time_to_compile time_to_debug time_to_refactor [number_of_compiles_required] dongle_cooldown scheduler
```

### Parameters
| Parameter | Description |
|:---|:---|
| `number_of_coders` | Total number of coders and dongles ($N \ge 1$). |
| `time_to_burnout` | Maximum time (in ms) a coder can go without compiling before burning out. |
| `time_to_compile` | Time (in ms) spent compiling while holding two dongles. |
| `time_to_debug` | Time (in ms) spent debugging after releasing dongles. |
| `time_to_refactor` | Time (in ms) spent refactoring before requesting dongles again. |
| `[number_of_compiles_required]` | *(Optional)* If specified, simulation stops cleanly once all coders compile this many times. |
| `dongle_cooldown` | Time (in ms) a dongle must cool down after release before being acquired again. |
| `scheduler` | Priority scheduling algorithm: `fifo` or `edf`. |

### Execution Examples
```bash
# 4 coders, 800ms burnout, 60ms compile/debug/refactor, target 2 compiles, 10ms cooldown, EDF policy
./codexion 4 800 60 60 60 2 10 edf

# 4 coders under FIFO policy
./codexion 4 800 60 60 60 2 10 fifo

# 1 coder edge case (burnout is inevitable as 2 dongles are required)
./codexion 1 800 200 100 100 2 10 edf
```

---

## Resources

### Academic & Technical References
- **Dijkstra, Edsger W. (1965)**: *"Hierarchical ordering of sequential processes"*. Established the Dining Philosophers problem and the resource hierarchy solution for deadlock prevention.
- **Coffman, Edward G. Jr., Elphick, Michael J., & Shoshani, Arie (1971)**: *"System Deadlocks"*, ACM Computing Surveys. Defined the four necessary and sufficient conditions for deadlock.
- **Liu, C. L., & Layland, James W. (1973)**: *"Scheduling Algorithms for Multiprogramming in a Hard-Real-Time Environment"*, Journal of the ACM. Established optimality theorems for Earliest Deadline First (EDF) scheduling.
- **Cormen, Thomas H., Leiserson, Charles E., Rivest, Ronald L., & Stein, Clifford**: *"Introduction to Algorithms (CLRS)"*, MIT Press. Reference for binary Min-Heap priority queue implementations ($O(\log k)$ push/pop).
- **IEEE Std 1003.1-2001 (POSIX.1)**: Standard for Information Technology — Portable Operating System Interface (`pthreads` mutexes, condition variables, and thread life cycle).

### AI Usage Details
In accordance with 42 curriculum guidelines, artificial intelligence (Google Antigravity / Gemini) was utilized during the development of this project as an interactive Socratic programming tutor and technical reviewer:
- **Architectural Design**: Validating the modular separation of responsibilities (`src/heap/`, `src/init/`, `src/routine/`, `src/monitor/`, `src/utils/`).
- **Concurrency Analysis**: Verifying strict lock ordering (`log_lock` $\rightarrow$ `stop_lock` $\rightarrow$ `state_lock` $\rightarrow$ `dongle->lock`) to prevent lock inversion.
- **Static Norm Auditing**: Ensuring full adherence to 42 Norm 4.1 standards ($\le 80$ characters/line, $\le 25$ lines/function, function documentation structure).
- **Automated Verification**: Formulating testing regimens with Valgrind (`--leak-check=full`) and Helgrind (`--tool=helgrind`) to confirm zero leaks and zero race conditions.

---

## Blocking Cases Handled

### 1. Deadlock Prevention & Coffman's Conditions
A deadlock can only occur if all four Coffman conditions are simultaneously met. Codexion structurally breaks the **Circular Wait** condition:
- **Mutual Exclusion**: Hardware dongles are non-shareable resources protected by mutex locks.
- **Hold and Wait**: Coders require two dongles. However, threads cannot enter an unresolvable hold-and-wait state due to resource hierarchy ordering.
- **No Preemption**: Dongles in active use cannot be preempted.
- **Circular Wait (Broken)**: Dongles are assigned deterministic IDs ($0$ to $N-1$). Every coder identifies their left and right dongles, but **always acquires the dongle with the lower ID first** (`min(left, right)`) before acquiring the dongle with the higher ID (`max(left, right)`). This asymmetric acquisition breaks the cyclic dependency graph, making deadlocks mathematically impossible.

### 2. Starvation Prevention (Priority Queuing)
Under high contention, naive mutex acquisition can starve slower threads. Codexion mitigates this by assigning an intrusive binary Min-Heap queue to each dongle:
- **FIFO Policy**: Sorts requests by `arrival_time`. The thread waiting the longest is guaranteed to be at the root of the heap and granted the dongle next upon release.
- **EDF Policy**: Sorts requests by deadline (`last_compile_time + time_to_burnout`). Coders closest to burning out are prioritized. Equal deadlines are resolved by a deterministic tie-breaker preferring higher coder IDs, ensuring strictly total ordering.

### 3. Hardware Cooldown Handling
When a dongle is released, its release timestamp is recorded. Any thread reaching the front of the queue verifies that `dongle_cooldown` milliseconds have elapsed since release. If cooldown is still active, the thread waits for the remainder of the cooldown duration before taking ownership, ensuring hardware constraints are respected without releasing queue priority.

### 4. Precise Burnout Detection
Burnout monitoring is handled by a dedicated monitor thread running independently from the coder threads:
- **Timestamp Synchronization**: All coders have their `last_compile_time` initialized to the shared simulation start time *prior* to thread creation, eliminating startup race conditions.
- **High-Precision Sampling**: The monitor polls coders with sub-millisecond intervals (`usleep(500)`), keeping detection latency within $\le 10$ ms of the exact burnout threshold.
- **Goal Completion Precedence**: When checking coders, completion of required compiles is evaluated *before* burnout checking to avoid reporting false deaths on coders that finished and exited.

### 5. Log Serialization & Post-Death Output Suppression
Console output is serialized via a dedicated `log_lock` mutex to avoid interleaved output lines:
- When a burnout occurs, `stop_with_burnout()` acquires `log_lock`, checks that the simulation is not already stopped, atomically marks the simulation stopped via `set_simulation_stop()`, prints `<timestamp> <id> burned out`, and releases `log_lock`.
- Every logging call in `log_action()` checks `is_simulation_stopped()` while holding `log_lock`. This strictly guarantees that **no message is ever output after a burnout log**.

---

## Thread Synchronization Mechanisms

### Synchronization Primitives
The implementation relies on granular POSIX threading primitives to coordinate access to shared state without global variables:

1. **`pthread_mutex_t`**:
   - `dongle->lock`: Guards dongle state (`is_in_use`, `owner_id`, `last_used_time`) and its binary Min-Heap request queue.
   - `coder->state_lock`: Protects individual telemetry variables (`compile_count`, `last_compile_time`) against data races between worker updates and monitor reads.
   - `rules->stop_lock`: Protects read/write access to the `simulation_stopped` boolean flag.
   - `rules->log_lock`: Serializes formatted terminal output and enforces the post-burnout silence guarantee.

2. **`pthread_cond_t`**:
   - `dongle->cond`: Used by coders waiting for a dongle to become available and for their request to reach the heap root.
   - **Broadcast Awakening**: When the simulation terminates (due to burnout or target completion), `broadcast_all_dongles()` sends `pthread_cond_broadcast()` to all dongle condition variables. This instantly unblocks all sleeping coders, allowing them to notice the stop flag and exit cleanly.

### Lock Acquisition Hierarchy
To prevent AB-BA deadlocks between logging, simulation state, and resource management, the system strictly respects the following lock hierarchy:

$$\text{log\_lock} \longrightarrow \text{stop\_lock} \longrightarrow \text{coder->state\_lock} \longrightarrow \text{dongle->lock}$$

Locks are always acquired in this order and released in strict reverse order.

### Thread Clean Shutdown & Memory Safety
- **Double-Checked Termination**: Coder routines verify `is_simulation_stopped()` before attempting dongle acquisition, while holding condition wait locks, and between state transitions.
- **Coordinated Joining**: The main thread coordinates simulation termination: it waits for the monitor thread and all coder threads to finish using `pthread_join()`.
- **Resource Cleanup**: Once all threads have joined, `cleanup_simulation()` safely destroys all mutexes, condition variables, and dynamic memory allocations, guaranteeing $0$ memory leaks and $0$ dangling synchronization objects.
