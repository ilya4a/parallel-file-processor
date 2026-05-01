# parallel-file-processor

`parallel-file-processor` is a Linux command-line utility for recursively searching, and optionally replacing, text inside files across one or more directories. It is designed to process files in parallel and provide flexible output ranging from a simple total count to detailed per-file match reports with highlighted matches.

The executable is called `frep`.

## Features

* Recursive search through directories
* Parallel processing with automatic or user-defined thread count
* Optional text replacement
* Multiple output detail levels
* Per-file statistics such as word count, line count, byte size, and processing time
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
frep -p ../lib/  -q "std::cout" # default -v
```

Output:

```text
MATCHES TOTAL: 16
```

### `-vv`

Shows per-file match counts.

Example:

```bash
frep -p ../lib/  -q "std::cout" -vv
```

Example output:

```text
[../lib/App.cpp]
found: 15
========================================================================================================
[../lib/example.txt]
found: 1
========================================================================================================
MATCHES TOTAL: 16
```

### `-vvv`

Shows full per-file output with matching lines and highlighted matches.

Example:

```bash
frep -p ../lib/  -q "std::cout" -vvv
```

Example output:

```text
[../lib/App.cpp]
found: 15
80:     std::cout << prefix << new_line << '\n';
        ^^^^^^^^^                               
96:     std::cout << underline << '\n';
        ^^^^^^^^^                      

...

357:         std::cout << "TOTAL TIME: " << processing_time_us << " microseconds" << std::endl;
             ^^^^^^^^^                                                                         
========================================================================================================
[../lib/example.txt]
found: 1
1: std::cout << "example of another file"
   ^^^^^^^^^                             
========================================================================================================
MATCHES TOTAL: 16

```

Matches are underlined with `^` characters in the detailed view.

## Statistics mode

When `-i` is enabled, the tool prints overall or per-file statistics depending on the selected verbosity level.

### With `-v -i`

```text
MATCHES TOTAL: 16
WORDS TOTAL: 2219 | BYTES TOTAL: 26059 | LINES TOTAL: 835
TOTAL TIME: 1711 microseconds
```

### With `-vv -i`

```text
[../lib/App.cpp]
found: 15
total_words: 1115 | total_lines: 360 | time: 212 microseconds | total_size: 12079
========================================================================================================
[../lib/example.txt]
found: 1
total_words:    6 | total_lines:   1 | time:  18 microseconds | total_size:    38
========================================================================================================
MATCHES TOTAL: 16
WORDS TOTAL: 2219 | BYTES TOTAL: 26059 | LINES TOTAL: 835
TOTAL TIME: 1020 microseconds
```
### With `-vvv -i`
```text
...

[../lib/example.txt]
found: 1
1: std::cout << "example of another file"
   ^^^^^^^^^                             
--------------------------------------------------------------------------------------------------------
total_words: 6 | total_lines: 1 | time: 41 microseconds | total_size: 38
========================================================================================================
MATCHES TOTAL: 16
WORDS TOTAL: 2219 | BYTES TOTAL: 26059 | LINES TOTAL: 835
TOTAL TIME: 1140 microseconds

```

## Examples

Search only:

```bash
frep -p /path/to/dir/ -p /path/to/file -q "query"
```

Search with file filtering and case-sensitive:

```bash
frep -p /path/ -q "query" -e ".cpp .h" -s
```

Search and replace:

```bash
frep -p /path/ -q "old_text" -r "new_text" -vv -i
```

Single-threaded execution:

```bash
frep -p /path/ -q "query " -S -i
```

