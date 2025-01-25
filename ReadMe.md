# peek_tree

A modern file tree viewer with preview capabilities, inspired by the classic `tree` command but with additional features for developers and system administrators.

Designed mainly to create coding related prompts for generative models that do not natively support indexing context files within an ide, such as OpenAI's GPT.

## Features

- 🌲 Traditional tree-style directory visualization
- 👀 File content preview (-p)
- 📂 Directory-only view (-d)
- 🔍 Pattern-based exclusion (-I)
- 🎨 Colorized output (-C)
- 📊 Detailed file information (size, permissions, dates)
- 🔗 Stay on filesystem (-x)
- 📝 Full file preview support (--full-preview)
- 📤 Output to file (-o)

## Installation

### Prerequisites

- C compiler (clang or gcc)
- make
- POSIX-compliant system

### Building from source

```bash
git clone https://github.com/aymanhki/peek_tree.git
cd peek_tree/src
make
```

The compiled binary will be available as `peek_tree` in the src directory.

## Usage

```bash
peek_tree [OPTIONS] [PATH]
```

### Basic Examples

```bash
# View current directory
peek_tree

# View specific directory with 2 levels depth
peek_tree -l 2 /path/to/directory

# Show hidden files
peek_tree -a

# Show file previews
peek_tree -p

# Show file previews with custom number of lines
peek_tree -p 5

# Exclude specific patterns
peek_tree -I "node_modules,.git,*.txt"
```

### Options

#### Display Options
- `-l N` - Limit tree depth to N levels
- `-p [N]` - Show preview with N (optional) lines (default: 3)
- `-f` - Show full path prefix for each entry
- `-a` - Show hidden files and directories
- `-d` - List directories only
- `-i` - Ignore case when sorting
- `-x` - Stay on current filesystem
- `-C` - Force colorization on
- `-n` - Force colorization off
- `-k` - Indent file preview
- `-j` - Visually link previews with tree root

#### File Information
- `-s` - Show file sizes
- `-u` - Show file owner
- `-g` - Show file group
- `-D` - Show modification date
- `--inodes` - Show inode numbers
- `--device` - Show device numbers
- `-F` - Append type indicators

#### Sorting and Filtering
- `-r` - Reverse sort
- `--dirsfirst` - List directories before files
- `-I "PATTERN,..."` - Exclude files/dirs matching patterns

#### Output Control
- `--noreport` - Omit final report
- `--full-preview` - Show full file content
- `-o FILE` - Output to file

#### Help and Version
- `-h` - Show help message
- `--version` - Show version information

## Environment Variables

- `NO_COLOR` - If set, disables color output regardless of other options

## Contributing

Contributions are welcome! Please feel free to submit a Pull Request.

## License

MIT License

## Author

aymanhki


