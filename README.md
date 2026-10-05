# LiteRedis

A high-performance, thread-safe In-Memory Key-Value Database built in C++ (Ubuntu WSL).

## Features (Work in Progress)
* O(1) Time Complexity Cache using `std::unordered_map` & `std::list`.
* LRU (Least Recently Used) Eviction Policy.
* Thread-safety architecture using `std::shared_mutex` (Reader-Writer locks).