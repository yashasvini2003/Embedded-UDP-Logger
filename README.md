# Embedded-UDP-Logger

A high-performance, embedded logging system for C++ applications that sends filtered log messages over UDP to a centralized log server. Designed to aid in real-time debugging and diagnostics, especially for issues that are hard to reproduce in the lab.

## Project Overview

This project simulates a real-world use case where a product undergoes various development phases and needs a robust mechanism to track logs in production environments. The logging system supports multiple severity levels and transmits qualified log messages to a central server for monitoring and analysis.

The system is split into two parts:
- **Logger Client** (embedded in C++ applications like `TravelSimulator`)
- **Log Server** (receives and stores logs centrally)

## Features

- Lightweight, non-blocking UDP-based logging
- Log filtering based on severity (DEBUG, WARNING, ERROR, CRITICAL)
- Timestamped logs with file, function, and line number
- Centralized server to collect logs and control filter level
- Server log dump and runtime filter level updates
- Thread-safe logging and server operation

## Getting Started

### Prerequisites

- Linux-based system
- g++
- Make
- Two machines or two terminal windows to simulate server and client

### Building the Logger and Server

#### 1. **Build the Log Server**
```bash
cd Embedded-UDP-Logger
make -f Makefile  # Compiles LogServer.cpp
```

#### 2. **Build the Client Application**
Make sure `Logger.cpp` and `Logger.h` are added to your application build (e.g., TravelSimulator).

Sample `Makefile` snippet for `TravelSimulator`:
```make
TravelSimulator: TravelSimulator.o Automobile.o Logger.o
    g++ -o TravelSimulator TravelSimulator.o Automobile.o Logger.o -lpthread
```

## Running the System

### Step 1: Start the Log Server
```bash
./LogServer
```

Server Menu Options:
1. Set the log level  
2. Dump the log file  
0. Shut down

### Step 2: Start the Client (TravelSimulator)
```bash
./TravelSimulator
```

It will:
- Initialize the logger
- Set the log level
- Generate logs with different severities
- Shut down the logger on exit

### Step 3: Observe Logs on the Server

Use the menu option to:
- Change the filter severity
- Dump the log file to view messages in real time

## Logger API (Client Side)

```cpp
int InitializeLog(); 
void SetLogLevel(LOG_LEVEL level); 
void Log(LOG_LEVEL level, const char* file, const char* func, int line, const char* message); 
void ExitLog();
```

### Enum for Severity
```cpp
typedef enum {
    DEBUG,
    WARNING,
    ERROR,
    CRITICAL
} LOG_LEVEL;
```

## Thread Safety & Performance

- Logger uses mutex protection for shared data
- UDP sockets are non-blocking for high performance
- Background thread for receiving filter updates from server

## Log Format

Each log line in the server log file:
```
Tue Apr 15 12:00:01 2025 DEBUG Automobile.cpp:StartEngine:42 Engine started successfully
```

Fields:
- Timestamp
- Severity
- Filename
- Function Name
- Line Number
- Message

## Future Enhancements

- Log rotation and archival on the server
- Authentication between client and server
- Support for TCP fallback if UDP fails
- Web dashboard for real-time monitoring
