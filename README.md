# Pintos Projects

This repository contains a series of projects for the Pintos operating system, which is designed for instructional purposes and runs on the 80x86 architecture. The projects build upon each other, adding features and complexity to the Pintos OS.

## Project 1: System Calls and Processes

### Overview
Project 1 focuses on enhancing the Pintos operating system by implementing argument passing, system call handling, and user memory protection.

### Features
- **Argument Passing**: Enable the kernel to pass arguments to user programs and verify with `hex_dump()`.
- **User Memory Access**: Protect user memory accesses during system calls.
- **System Call Handler**: Implement `syscall_handler()` to handle system calls.
- **System Call Implementation**: Implement system calls such as `exec()`, `exit()`, `write()`, `read()`, `wait()`, and `halt`. Additional system calls will be added later.

## Project 2: File System Calls

### Overview
Project 2 involves implementing system calls related to the file system in Pintos. This project requires understanding the base file system provided by Pintos and interfacing with the file system code.

### Features
- **File System System Calls**: Implement system calls for file operations including `create`, `remove`, `open`, `close`, `filesize`, `read`, `write`, `seek`, and `tell`.
- **Base File System Understanding**: Gain familiarity with Pintos' file system by exploring `filesys.h` and `file.h` in the `filesys/` directory.
- **File System Call Implementation**: No need to modify the file system code. The focus is on implementing system calls that interface with the existing file system.

### Critical Section Issue
- **Unresolved Critical Sections**: Despite progress in file system calls, critical section issues remain unresolved in Project 2. These issues require further attention to ensure system stability and efficiency.


## Project 3: Alarm Clock and Priority Scheduling

### Overview
Project 3 introduces enhancements to the Pintos operating system by improving the efficiency of the alarm clock and implementing priority scheduling.

### Features

#### Alarm Clock
- **Efficient Sleeping**: Modify `timer_sleep()` to avoid inefficiency. Instead of cycling between RUNNING and READY states, block the thread if the "ticks" are not yet passed.
- **Thread Management**: Implement a new queue to manage threads that are sleeping, saving their wake-up time.
- **Wake-up Mechanism**: Utilize `timer_interrupt()` to wake up threads whose sleep duration has elapsed and insert them back into the ready queue.

#### Priority Scheduling
- **Priority-Based Scheduling**: Transition from round-robin scheduling to a priority-based approach, where threads are selected based on their priority.
- **Immediate Yielding**: When a new thread with higher priority than the current running thread arrives, the current thread immediately yields the CPU.
- **Aging Technique**: Implement aging to prevent starvation of low-priority processes. Increase priority over time to ensure fairness.

### Challenges
- **Advanced Scheduler**: The implementation of an advanced scheduler considering thread aging was not completed. The aging technique is activated only when the Pintos kernel receives the '-aging' option and sets the aging flag to TRUE.

