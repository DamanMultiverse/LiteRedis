# 🚀 LiteRedis: Multithreaded In-Memory Key-Value Store

A high-performance, multithreaded in-memory key-value database built from scratch in C++17. LiteRedis features a custom TCP socket server, thread-safe concurrent client handling, O(1) LRU eviction, and lazy TTL expiration. 

## 🧠 System Architecture & Features

* **Network Layer:** Custom TCP/IP Server using Linux POSIX Sockets (`<sys/socket.h>`), running on port 6380.
* **Concurrency:** Thread-per-client model utilizing `std::thread` to handle multiple simultaneous client connections without blocking the main event loop.
* **Thread-Safe Data Structures:** Employs `std::shared_mutex` (Reader-Writer locks) to ensure ACID properties and prevent race conditions during concurrent read/write operations.
* **O(1) LRU Eviction Policy:** Implemented using a combination of `std::unordered_map` and doubly-linked list (`std::list::splice`) for true O(1) cache promotion and eviction.
* **Time-To-Live (TTL):** "Lazy Eviction" strategy using `<chrono>` to automatically invalidate expired keys upon subsequent access, optimizing CPU and memory usage.
* **Telemetry & Metrics:** Atomic counters (`std::atomic`) to track cache hits, misses, and hit ratio dynamically.

## 🛠️ Tech Stack
* **Language:** C++17 (STL, Threading, Chrono)
* **Networking:** POSIX Sockets (TCP/IP)
* **Environment:** Linux (Ubuntu/WSL)
* **Build System:** Makefile

## 📡 Supported Commands

Clients can interact with the database using any standard TCP client (like `nc` or `telnet`).

| Command | Syntax | Description |
| :--- | :--- | :--- |
| **PING** | `PING` | Health check. Returns `+PONG`. |
| **SET** | `SET <key> <value> [ttl]` | Stores a key-value pair. Optional TTL in seconds. |
| **GET** | `GET <key>` | Retrieves the value. Promotes key to MRU. |
| **DEL** | `DEL <key>` | Removes the key from the database. |
| **EXIT** | `EXIT` | Closes the client connection. |

## 🚀 Getting Started

### Prerequisites
* `g++` (Compiler supporting C++17)
* `make` 

### Build & Run
1. Clone the repository and compile the source code:
```bash
make