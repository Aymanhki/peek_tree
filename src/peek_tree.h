#ifndef PEEK_TREE_H
#define PEEK_TREE_H

#include <stdbool.h>
#include <sys/types.h>
#include <sys/stat.h>

#define DEFAULT_DEPTH -1
#define PREVIEW_LINES 3
#define MAX_PATH_LENGTH 4096
#define MAX_LINE_LENGTH 1024
#define PATH_MAX 4096
#define MAX_PREVIEW_WIDTH 80
#define VERSION "v1.0.9"
#define COLOR_RESET   "\x1b[0m"
#define COLOR_BLUE    "\x1b[34m"
#define COLOR_GREEN   "\x1b[32m"
#define COLOR_CYAN    "\x1b[36m"
#define COLOR_RED    "\x1b[31m"
#define COLOR_YELLOW  "\x1b[33m"

typedef struct {
    char *name;
    char *full_path;
    struct stat stat;
    bool is_dir;
} DirEntry;

typedef struct Options {
    int max_depth;              // -l [depth]
    bool show_preview;         // -p
    bool show_full_file;       // --full-preview
    int preview_lines;         // -p [lines]
    bool show_hidden;          // -a
    bool dirs_only;            // -d
    bool full_path;            // -f
    bool ignore_case;          // -i
    bool stay_on_fs;           // -x
    bool no_report;            // --noreport
    bool show_size;            // -s
    bool show_user;            // -u
    bool show_group;           // -g
    bool show_date;            // -D
    bool show_inodes;          // --inodes
    bool show_device;          // --device
    bool append_type;          // -F
    bool dirs_first;           // --dirsfirst
    bool reverse_sort;         // -r
    dev_t start_device;        // For -x option
    bool colorize;             // -C
    bool no_color;             // -n
    bool indent_file_preview;  // -k
    char **exclude_patterns;   // -I
    int exclude_patterns_count;
    char *output_file;         // -o Path to the output file
    bool visually_link_previews_with_tree_root; // -j
} Options;

void setup_terminal_encoding(void);
void print_usage(void);
bool is_binary_file(const char *path);
void print_version(void);
char get_file_type_indicator(mode_t mode);
void print_full_file(const char *path, const char *prefix, bool indent, bool visually_link_previews_with_tree_root, bool no_color);
const char *get_color_code(mode_t mode, const Options *opts);
bool should_exclude(const char *path, const Options *opts);
bool is_binary_file(const char *path);
char **split_by_commas(const char *input, int *count);
void format_size(char *buf, size_t buf_size, off_t size);
void print_file_preview(const char *path, const char *prefix, const Options *opts);
int compare_entries(const void *a, const void *b, const Options *opts);
void sort_entries(DirEntry *entries, size_t count, const Options *opts);
void print_entry_info(const DirEntry *entry, const Options *opts, const char *prefix, bool is_last);
int process_directory(const char *path, const Options *opts, int current_depth, const char *prefix);

#endif