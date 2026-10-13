# 🚀 LiteRedis: Multithreaded In-Memory Key-Value Store

A high-performance, production-grade, multithreaded in-memory key-value database built from scratch in C++17. LiteRedis features a custom TCP socket server, thread-safe concurrent client handling, O(1) LRU eviction, lazy TTL expiration, and RDB-style disk persistence.

## 🧠 Architecture & Key Features

* **Network Layer:** Custom TCP/IP Server utilizing Linux POSIX Sockets (`<sys/socket.h>`), binding to port 6380.
* **Concurrency:** Thread-per-client model employing `std::thread` to manage multiple simultaneous connections non-blockingly.
* **Thread-Safety:** Granular synchronization using `std::shared_mutex` (Reader-Writer locks) to prevent race conditions during concurrent data access.
* **O(1) LRU Eviction:** Engineered with `std::unordered_map` and a doubly-linked list (`std::list::splice`) for strict O(1) time complexity on cache promotion and eviction.
* **Lazy Expiration (TTL):** Time-to-Live functionality using `<chrono>` to automatically invalidate expired keys upon access, optimizing resource utilization.
* **Data Persistence (Disk I/O):** RDB-style snapshotting via standard File I/O (`<fstream>`). Saves the in-memory state to disk (`dump.rdb`) and automatically recovers data upon server restarts.
* **Containerization:** Fully Dockerized environment for seamless deployment and execution across isolated environments.

## 🛠️ Tech Stack
* **Language:** C++17 (STL, Multithreading, Chrono, File I/O)
* **Networking:** POSIX Sockets (TCP/IP)
* **Deployment:** Docker, Containerization
* **Environment:** Linux (Ubuntu)
* **Build System:** GNU Make

## 📡 Supported Commands

Connect via standard TCP clients (e.g., `nc`, `telnet`).

| Command | Syntax | Description |
| :--- | :--- | :--- |
| **PING** | `PING` | Health check. Returns `+PONG`. |
| **SET** | `SET <key> <value> [ttl]` | Stores a key-value pair. Optional TTL in seconds. |
| **GET** | `GET <key>` | Retrieves the value. Promotes key to MRU. |
| **DEL** | `DEL <key>` | Removes the key from the database. |
| **SAVE** | `SAVE` | Triggers a disk snapshot, saving the current state. |
| **EXIT** | `EXIT` | Terminates the client connection gracefully. |

## 🚀 Getting Started

### Option A: Run via Docker (Recommended)
1. Build the Docker image:
   ```bash
   docker build -t literedis .