# parallel-file-processor

`parallel-file-processor` is a Linux command-line utility for recursively searching, and optionally replacing, text inside files across one or more directories. It is designed to process files in parallel and provide flexible output ranging from a simple total count to detailed per-file match reports with highlighted matches.

The executable is called `frep`.

## Features

* Recursive search through directories
* Parallel processing with automatic or user-defined thread count
* Optional text replacement
* Multiple output detail levels
* Per-file statistics such as word count, line count, byte size, and processing time
* Case-sensitive or case-insensitive search
* Optional file extension filtering
* Linux-only

## Build

The project uses CMake.

```bash
git clone https://github.com/ilya4a/parallel-file-processor
cd parallel-file-processor
cmake -S . -B build
cmake --build build -j
```

If `CLI11` is included as a submodule in your repository, initialize submodules before configuring the project:

```bash
git submodule update --init --recursive
```

## Usage

```bash
frep -p <path> [-p <path> ...] -q <query> [options]
```

### Required arguments

* `-p, --path`  Root directory or file path to search in. Can be repeated.
* `-q, --query`  Search pattern.

### Optional arguments

* `-r, --replacement`  Replacement string. If omitted, the tool performs search only.
* `-v`  Increase output verbosity. May be repeated as `-vv` or `-vvv`.
* `-i, --file-info`  Show per-file statistics.
* `-e, --extensions`  File extensions to include, for example `.cpp .h`.
* `-s, --case-sensitive`  Enable case-sensitive search.
* `-S, --sequential`  Run in single-threaded mode.
* `-j, --threads`  Number of worker threads to use. `0` means automatic selection.

## Output modes

The tool supports three verbosity levels.

### `-v`

Shows only the total number of matches across all processed files.

Example:

```bash
frep -p ../src/ -p ../lib/ -p ../include/ -q "throw " -v
```

Output:

```text
MATCHES TOTAL: 17
```

### `-vv`

Shows per-file match counts.

Example:

```bash
frep -p ../src/ -p ../lib/ -p ../include/ -q "throw " -vv
```

Example output:

```text
[../lib/utils.cpp]
found: 6
============================================================
[../lib/FileProcessor.cpp]
found: 3
...
```

### `-vvv`

Shows full per-file output with matching lines and highlighted matches.

Example:

```bash
frep -p ../src/ -p ../lib/ -p ../include/ -q "throw " -vvv
```

Example output:

```text
[../lib/utils.cpp]
found: 6
17:             throw std::runtime_error("Utils: allowed_extensions is empty");
                ^^^^^
21:             throw std::runtime_error("Utils: path does not exist");
                ^^^^^
...
```

Matches are underlined with `^` characters in the detailed view.

## Statistics mode

When `-i` is enabled, the tool prints overall or per-file statistics depending on the selected verbosity level.

### With `-v -i`

```text
MATCHES TOTAL: 17
WORDS TOTAL: 3051 | BYTES TOTAL: 36078 | LINES TOTAL: 1231
TOTAL TIME: 1240 microseconds
```

### With `-vv -i`

```text
[../lib/utils.cpp]
found: 6
total_words: 397 | total_lines: 153 | time: 95 microseconds | total_size: 4876
```

## Examples

Search only:

```bash
frep -p ../src/ -p ../lib/ -p ../include/ -q "throw "
```

Search with file filtering:

```bash
frep -p ../src/ -p ../lib/ -p ../include/ -q "throw " -vv -i -e ".cpp .h"
```

Case-sensitive search:

```bash
frep -p ./src -q "TODO" -s -vv
```

Search and replace:

```bash
frep -p ./src -q "old_text" -r "new_text" -vv
```

Single-threaded execution:

```bash
frep -p ./src -q "throw " -S -vvv
```

## Notes

* Directories are processed recursively.
* The project targets Linux.
* `CLI11` is planned as a git submodule dependency.

## Project structure

```text
parallel-file-processor/
├── include/
│   ├── App.h
│   ├── Config.h
│   ├── Debug.h
│   ├── FileProcessor.h
│   ├── ReplaceOptions.h
│   ├── ThreadPool.h
│   └── utils.h
├── lib/
│   ├── App.cpp
│   ├── FileProcessor.cpp
│   ├── ThreadPool.cpp
│   └── utils.cpp
└── src/
    └── main.cpp
```

