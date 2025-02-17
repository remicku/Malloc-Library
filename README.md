# Malloc Library

## Description
The **Malloc Library** is a custom implementation of standard memory management functions in C: `malloc`, `free`, `calloc`, and `realloc`. This project aims to deepen the understanding of dynamic memory allocation, pointer manipulation, and system-level programming.

## Features
- **malloc**: Allocates a block of memory of a specified size.
- **free**: Releases a previously allocated block of memory.
- **calloc**: Allocates and zeroes multiple blocks of memory.
- **realloc**: Resizes an existing memory block while preserving its data if possible.
- **Bucket-based allocation**: Uses fixed-size buckets to minimize fragmentation.
- **Optimized alignment**: Ensures blocks are aligned to `long double` for better performance.
- **Automatic page release**: Unmaps empty pages using `munmap`.
- **`LD_PRELOAD` compatible**: Can replace `malloc` in external programs.

## Project Structure

```bash
malloc/
├── src/
│   ├── bucket.c        # Bucket management for memory blocks
│   ├── malloc.c        # Public interface for malloc, free, realloc, calloc
│   ├── my_malloc.c     # Core implementation of the memory allocator
│   ├── utils.c         # Utility functions (alignment, page management, etc.)
│   ├── bucket.h        # Header for bucket structures
│   ├── my_malloc.h     # Header for memory functions
│   ├── utils.h         # Header for utility functions
│   ├── Makefile        # Build system for compiling the library
├── tests/            # Provided test binaries for verification
│   ├── corruptionproof     # Tests metadata integrity
│   ├── memoryfootprint     # Tests memory efficiency
│   ├── speed              # Tests allocator performance
└── README.md         # This file!
```

## Getting Started

### Prerequisites
- GCC or another compatible C compiler.
- A Unix-based system is recommended for testing (Linux, macOS).

### Clone the Repository
```bash
git clone https://github.com/remicku/Malloc-Library.git
cd Malloc-Library
```

### Compilation
A `Makefile` is provided to simplify the build process. Compile the code by running:
```bash
make all
```

Available targets:

-   **`all`**: Compiles the library and examples.
-   **`library`**: Compiles the library
-   **`clean`**: Removes all compiled files.
-   **`debug`**: Enables debugging with gdb.

To clean the project:

```bash
make clean
```

## Usage

To use this implementation instead of the standard `malloc`:

```bash
LD_PRELOAD=./libmalloc.so ./your_program
```

Examples:

```bash
LD_PRELOAD=./libmalloc.so ls
LD_PRELOAD=./libmalloc.so ./tests/memoryfootprint
```

You can test the library with various programs by preloading it:

```bash
factor 10 20 30 40 50
cat Makefile
ip a
ls
ls -a
find /
tree /
clang --help
firefox
discord
```

## Implementation Details

### Memory Allocation Strategy

This allocator is based on a **bucket system**, where each bucket manages blocks of a fixed size.

#### Allocation Process (`malloc`)

1.  Determines the nearest power-of-two block size.
2.  Checks for an existing bucket with available space.
3.  If none is found, creates a new bucket using `mmap`.
4.  Allocates a free block from the selected bucket.

#### Freeing Memory (`free`)

1.  Marks the block as free.
2.  If the entire bucket is empty, it is `munmap`ped to free system memory.

#### `realloc` Optimization

-   If the requested size fits in the same bucket, reuse the block.
-   If not, allocate a new block, copy data, and free the old one.

## Testing

A set of **precompiled test binaries** is provided to validate your implementation.

Run tests using:

```bash
LD_PRELOAD=./libmalloc.so ./tests/<test_binary>
```

### Provided Tests

| Test Binary | Description |
|---------------------|-------------|
| `speed` | Ensures the allocator runs within a fixed time limit. |
| `memoryfootprint` | Evaluates memory fragmentation and efficiency. |
| `corruptionproof` | Checks for metadata corruption resistance. |

#### Example Test Runs

```bash
LD_PRELOAD=./libmalloc.so ./tests/speed; echo $?
# Expected output: 0 (success)
```

```bash
LD_PRELOAD=./libmalloc.so ./tests/memoryfootprint 512; echo $?
# Expected output: 50%+ memory efficiency (varies by implementation)
```

```bash
LD_PRELOAD=./libmalloc.so ./tests/corruptionproof; echo $?
# Expected output: 0 (success)
```

## Debugging

Debugging a shared library requires special handling.

### Using GDB with a linked binary

```bash
make debug
gdb ./your_program
(gdb) set env LD_LIBRARY_PATH=.
(gdb) run
```

### Using GDB with `LD_PRELOAD`


```bash
make debug
gdb ls
(gdb) set exec-wrapper env 'LD_PRELOAD=./libmalloc.so'
(gdb) run
```

## Contributing

Contributions are welcome! Feel free to open an issue or submit a pull request if you have ideas for improvements or new features.
