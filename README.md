# HTTP Stress Test Tool

![Tool Preview](https://i.postimg.cc/D0CyQfBG/1787092098407-01a016fb-b44d-70a6-bda7-42e8784b84b8.jpg)

High-performance HTTP stress testing utility built with C++17, optimized for native architecture execution.

## Features

- Multi-threaded request handling
- Native architecture optimization (`-march=native`)
- Real-time terminal UI via ncurses
- Asynchronous HTTP requests via libcurl
- Minimal dependencies footprint

## Prerequisites

- Clang compiler
- libcurl development headers
- ncurses development headers
- GNU Make

## Installation

### Termux (Android)

```bash
pkg update && pkg install -y libcurl ncurses-dev clang make
```

### Debian/Ubuntu

```bash
sudo apt update && sudo apt install -y libcurl4-openssl-dev libncurses5-dev clang make
```

### Arch Linux

```bash
sudo pacman -S curl ncurses clang make
```

## Build

```bash
make -f Makefile dos
```

## Usage

```bash
./dos
```

## Configuration

Edit `Makefile` to adjust compilation flags:

```makefile
CC = clang++
CFLAGS = -O3 -march=native -std=c++17
LIBS = -lcurl -lncurses -lpthread
```

## Clean Build

```bash
make clean
```

## Technical Stack

- **Language**: C++17
- **Compiler**: Clang with O3 optimization
- **Networking**: libcurl (multi-interface)
- **UI**: ncurses for terminal rendering
- **Concurrency**: pthreads for parallel execution

## Notes

- Requires root/administrator privileges for high-volume tests
- Monitor system resources during execution
- Respect target server rate limits and terms of service
