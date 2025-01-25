#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <assert.h>
#include <errno.h>
#include <unistd.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>
#include <sys/types.h>
#include <ctype.h>
#include <locale.h>
#include <stdbool.h>
#include <limits.h>
#include <locale.h>
#include <langinfo.h>
#include "peek_tree.h"

static int total_dirs = 0;
static int total_files = 0;
FILE *output_stream = NULL;


int main(int argc, char *argv[])
{
    setup_terminal_encoding();
    output_stream = stdout;

    Options opts = {
        .max_depth = DEFAULT_DEPTH,
        .exclude_patterns = NULL,
        .exclude_patterns_count = 0,
        .show_preview = false,
        .show_full_file = false,
        .preview_lines = PREVIEW_LINES,
        .show_hidden = false,
        .dirs_only = false,
        .full_path = false,
        .ignore_case = false,
        .stay_on_fs = false,
        .no_report = false,
        .show_size = false,
        .show_user = false,
        .show_group = false,
        .show_date = false,
        .show_inodes = false,
        .show_device = false,
        .append_type = false,
        .dirs_first = false,
        .reverse_sort = false,
        .start_device = 0,
        .colorize = false,
        .no_color = false,
        .indent_file_preview = false,
        .output_file = NULL,
        .visually_link_previews_with_tree_root = false,
    };

    char resolved_path[PATH_MAX];
    char *path = ".";

    if (getenv("NO_COLOR") != NULL)
    {
        opts.no_color = true;
    }

    for (int i = 1; i < argc; i++)
    {
        if (argv[i][0] != '-')
        {
            path = argv[i];
            break;
        }

        if (strcmp(argv[i], "-h") == 0)
        {
            print_usage();
            return 0;
        }
        else if (strcmp(argv[i], "--version") == 0)
        {
            print_version();
            return 0;
        }
        else if (strcmp(argv[i], "-l") == 0 && i + 1 < argc)
        {
            opts.max_depth = atoi(argv[++i]);
        }
        else if (strcmp(argv[i], "-I") == 0 && i + 1 < argc)
        {
            opts.exclude_patterns = split_by_commas(argv[++i], &opts.exclude_patterns_count);
        }
        else if (strcmp(argv[i], "-p") == 0)
        {
            opts.show_preview = true;
            if (i + 1 < argc && isdigit(argv[i + 1][0]))
            {
                opts.preview_lines = atoi(argv[++i]);
                if (opts.preview_lines < 1)
                {
                    fprintf(stderr, "Preview lines must be at least 1\n");
                    return 1;
                }
            }
        }
        else if (strcmp(argv[i], "-k") == 0)
        {
            opts.indent_file_preview = true;
        }
        else if (strcmp(argv[i], "--full-preview") == 0)
        {
            opts.show_full_file = true;
            opts.show_preview = true;
        }
        else if (strcmp(argv[i], "-f") == 0)
        {
            opts.full_path = true;
        }
        else if (strcmp(argv[i], "-a") == 0)
        {
            opts.show_hidden = true;
        }
        else if (strcmp(argv[i], "-d") == 0)
        {
            opts.dirs_only = true;
        }
        else if (strcmp(argv[i], "-i") == 0)
        {
            opts.ignore_case = true;
        }
        else if (strcmp(argv[i], "-x") == 0)
        {
            opts.stay_on_fs = true;
        }
        else if (strcmp(argv[i], "-s") == 0)
        {
            opts.show_size = true;
        }
        else if (strcmp(argv[i], "-u") == 0)
        {
            opts.show_user = true;
        }
        else if (strcmp(argv[i], "-g") == 0)
        {
            opts.show_group = true;
        }
        else if (strcmp(argv[i], "-D") == 0)
        {
            opts.show_date = true;
        }
        else if (strcmp(argv[i], "--inodes") == 0)
        {
            opts.show_inodes = true;
        }
        else if (strcmp(argv[i], "--device") == 0)
        {
            opts.show_device = true;
        }
        else if (strcmp(argv[i], "-F") == 0)
        {
            opts.append_type = true;
        }
        else if (strcmp(argv[i], "-r") == 0)
        {
            opts.reverse_sort = true;
        }
        else if (strcmp(argv[i], "--dirsfirst") == 0)
        {
            opts.dirs_first = true;
        }
        else if (strcmp(argv[i], "--noreport") == 0)
        {
            opts.no_report = true;
        }
        else if (strcmp(argv[i], "-C") == 0)
        {
            opts.colorize = true;
            opts.no_color = false;
        }
        else if (strcmp(argv[i], "-n") == 0)
        {
            opts.no_color = true;
            opts.colorize = false;
        }
        else if (strcmp(argv[i], "-o") == 0 && i + 1 < argc)
        {
            opts.output_file = argv[++i];

            if (opts.output_file)
            {
                output_stream = fopen(opts.output_file, "w");

                if (!output_stream)
                {
                    fprintf(stderr, "%sError opening output file '%s': %s%s\n", COLOR_RED, opts.output_file, strerror(errno), COLOR_RESET);
                    return 1;
                }

                fprintf(stdout, "Started printing in file: '%s'\n", opts.output_file);
                opts.no_color = true;
                opts.colorize = false;

            }
            else
            {
                fprintf(stderr, "%sError: Output file not specified%s\n", COLOR_RED, COLOR_RESET);
                output_stream = stdout;
            }
        }
        else if (strcmp(argv[i], "-j") == 0)
        {
            opts.visually_link_previews_with_tree_root = true;
            opts.indent_file_preview = true;
        }
        else if (argv[i][0] != '-')
        {
            path = argv[i];
        }
        else
        {
            fprintf(stderr, "Unknown option: %s\n", argv[i]);
            print_usage();
            return 1;
        }
    }

    if (argc > 1 && argv[argc-1][0] == '-' && !strchr(argv[argc-1], '/') && !strchr(argv[argc-1], '\\') && strcmp(path, ".") != 0)
    {
        fprintf(stderr, "%sError: Unexpected argument '%s' after path%s\n", COLOR_RED, argv[argc-1], COLOR_RESET);
        return 1;
    }



    if (realpath(path, resolved_path) == NULL)
    {
        fprintf(stderr, "%sError resolving path '%s': %s%s\n", COLOR_RED, path, strerror(errno), COLOR_RESET);
        return 1;
    }

    if (!opts.no_color && !opts.colorize)
    {
        opts.colorize = isatty(STDOUT_FILENO);
    }

    fprintf(output_stream, "```\n");
    fprintf(output_stream, "%s\n", resolved_path);

    int result = process_directory(resolved_path, &opts, 0, "");

    fprintf(output_stream, "```\n");

    if (result == 0 && !opts.no_report)
    {
        fprintf(output_stream, "\nTotal directories: %d, Total files: %d\n", total_dirs, total_files);
    }

    if (opts.output_file)
    {
        fprintf(stdout, "Finished printing in file: '%s'\n", opts.output_file);
        fclose(output_stream);
    }



    for (int i = 0; i < opts.exclude_patterns_count; i++)
    {
        free(opts.exclude_patterns[i]);
    }

    free(opts.exclude_patterns);

    return result;
}

void setup_terminal_encoding(void)
{
    setenv("LANG", "en_US.UTF-8", 1);
    setenv("LC_ALL", "en_US.UTF-8", 1);

    if (setlocale(LC_ALL, "") == NULL)
    {
        fprintf(stderr, "%sWarning: Could not set locale, some file previews might display incorrectly.\n%s", COLOR_YELLOW, COLOR_RESET);
    }

    if (strcmp(nl_langinfo(CODESET), "UTF-8") != 0)
    {
        fprintf(stderr, "%sWarning: Terminal does not support UTF-8 encoding, some characters may not display correctly.\n%s", COLOR_YELLOW, COLOR_RESET);
    }
}

void print_version(void)
{
    fprintf(stdout, "peek_tree version %s\n", VERSION);
}

void print_usage(void)
{
    fprintf(stdout, "Usage: peek_tree [OPTIONS] [PATH]\n\n"
        "PATH:\n"
        "  The directory to list (default: current directory)\n"
        "  Can be absolute or relative path\n"
        "  Must be the last argument if specified\n\n"
        "Options:\n"
        "  -l N              Limit tree depth to N levels\n"
        "  -I \"PATTERN,...\"  Exclude files/dirs matching patterns (comma-separated, quoted)\n"
        "                    Example: -I \"node_modules,.git,*.txt, test.ext\"\n"
        "  -p [N]            Show preview with N (optional) lines (default: 3)\n"
        "  -f                Show full path prefix for each entry\n"
        "  -a                Show hidden files and directories\n"
        "  -d                List directories only\n"
        "  -i                Ignore case when sorting\n"
        "  -x                Stay on current filesystem\n"
        "  -s                Show file sizes\n"
        "  -u                Show file owner\n"
        "  -g                Show file group\n"
        "  -D                Show modification date\n"
        "  --inodes         Show inode numbers\n"
        "  --device         Show device numbers\n"
        "  -F                Append type indicators\n"
        "  -r                Reverse sort\n"
        "  --dirsfirst      List directories before files\n"
        "  --noreport       Omit final report\n"
        "  -C                Force colorization on\n"
        "  -n                Force colorization off\n"
        "  -k                Indent file preview\n"
        "  --full-preview   Show full file content\n"
        "  -h                Show this help message\n"
        "  --version        Show version information\n\n"
        "Note: Options must be specified before the PATH argument\n");
}

char get_file_type_indicator(mode_t mode)
{
    if (S_ISDIR(mode))
        return '/';
    if (S_ISLNK(mode))
        return '@';
    if (S_ISSOCK(mode))
        return '=';
    if (S_ISFIFO(mode))
        return '|';
    if (mode & S_IXUSR)
        return '*';
    return ' ';
}

void print_full_file(const char *path, const char *prefix, bool indent, bool visually_link_previews_with_tree_root, bool no_color)
{
    FILE *file = fopen(path, "rb");

    if (!file)
    {
        if (indent)
        {
            if (visually_link_previews_with_tree_root)
            {
                if (no_color)
                {
                    fprintf(output_stream, "%s│     Error: %s\n", prefix, strerror(errno));
                }
                else
                {
                    fprintf(output_stream, "%s│     %sError: %s%s\n", prefix, COLOR_RED, strerror(errno), COLOR_RESET);
                }
            }
            else
            {
                if (no_color)
                {
                    fprintf(output_stream, "%s     Error: %s\n", prefix, strerror(errno));
                }
                else
                {
                    fprintf(output_stream, "%s     %sError: %s%s\n", prefix, COLOR_RED, strerror(errno), COLOR_RESET);
                }
            }
        }
        else
        {
            if (no_color)
            {
                fprintf(output_stream, "Error: %s\n", strerror(errno));
            }
            else
            {
                fprintf(output_stream, "%sError: %s%s\n", COLOR_RED, strerror(errno), COLOR_RESET);
            }
        }

        return;
    }

    char line[MAX_LINE_LENGTH];

    while (fgets(line, sizeof(line), file))
    {
        size_t len = strlen(line);

        if (len > 0 && line[len - 1] == '\n')
        {
            line[len - 1] = '\0';
        }

        char *p = line;

        while (*p)
        {
            if ((*p & 0x80) && !(*p & 0x40))
            {
                *p = ' ';
            }

            p++;
        }

        if (indent)
        {
            if (visually_link_previews_with_tree_root)
            {
                fprintf(output_stream, "%s│     %s\n", prefix, line);
            }
            else
            {
                fprintf(output_stream, "%s     %s\n", prefix, line);
            }
        }
        else
        {
            fprintf(output_stream, "%s\n", line);
        }
    }

    fclose(file);
}

const char *get_color_code(mode_t mode, const Options *opts)
{
    if (!opts->colorize || opts->no_color)
        return "";

    if (S_ISDIR(mode))
        return COLOR_BLUE;
    if (S_ISLNK(mode))
        return COLOR_CYAN;
    if (mode & S_IXUSR)
        return COLOR_GREEN;

    return COLOR_RESET;
}

bool should_exclude(const char *path, const Options *opts)
{
    for (int i = 0; i < opts->exclude_patterns_count; i++)
    {
        const char *pattern = opts->exclude_patterns[i];
        const char *basename = strrchr(path, '/');
        basename = basename ? basename + 1 : path;

        if (strcasecmp(basename, pattern) == 0)
        {
            return true;
        }

        if (strcasestr(basename, pattern) != NULL)
        {
            return true;
        }
    }

    return false;
}

bool is_binary_file(const char *path)
{
    FILE *file = fopen(path, "rb");

    if (!file)
        return false;

    unsigned char buffer[1024];
    size_t bytes_read = fread(buffer, 1, sizeof(buffer), file);
    fclose(file);

    for (size_t i = 0; i < bytes_read; i++)
    {
        if (buffer[i] == 0)
            return true;
    }

    return false;
}

char **split_by_commas(const char *input, int *count)
{
    char *copy = strdup(input);
    char **result = NULL;
    *count = 0;

    char *p = copy;

    while (*p)
    {
        if (*p == ',' && *(p + 1) == ' ')
        {
            memmove(p + 1, p + 2, strlen(p + 2) + 1);
        }

        p++;
    }

    char *token = strtok(copy, ",");
    while (token)
    {
        while (isspace(*token))
            token++;
        char *end = token + strlen(token) - 1;
        while (end > token && isspace(*end))
            end--;

        *(end + 1) = '\0';

        if (strlen(token) > 0)
        {
            result = realloc(result, sizeof(char *) * (*count + 1));
            result[*count] = strdup(token);
            (*count)++;
        }

        token = strtok(NULL, ",");
    }

    free(copy);
    return result;
}

void format_size(char *buf, size_t buf_size, off_t size)
{
    const char *units[] = {"B", "K", "M", "G", "T"};
    int unit_index = 0;
    double size_d = size;

    while (size_d >= 1024 && unit_index < 4)
    {
        size_d /= 1024;
        unit_index++;
    }

    if (unit_index == 0)
    {
        snprintf(buf, buf_size, "%lld%s", (long long)size, units[unit_index]);
    }
    else
    {
        snprintf(buf, buf_size, "%.1f%s", size_d, units[unit_index]);
    }
}

void print_file_preview(const char *path, const char *prefix, const Options *opts)
{
    if (opts->show_full_file)
    {
        print_full_file(path, prefix, opts->indent_file_preview, opts->visually_link_previews_with_tree_root, opts->no_color);
        return;
    }

    FILE *file = fopen(path, "rb");

    if (!file)
    {
        if (opts->indent_file_preview)
        {
            if (opts->visually_link_previews_with_tree_root)
            {
                if (opts->no_color)
                {
                    fprintf(output_stream, "%s│     Error: %s\n", prefix, strerror(errno));
                }
                else
                {
                    fprintf(output_stream, "%s│     %sError: %s%s\n", prefix, COLOR_RED, strerror(errno), COLOR_RESET);
                }
            }
            else
            {
                if (opts->no_color)
                {
                    fprintf(output_stream, "%s     Error: %s\n", prefix, strerror(errno));
                }
                else
                {
                    fprintf(output_stream, "%s     %sError: %s%s\n", prefix, COLOR_RED, strerror(errno), COLOR_RESET);
                }
            }
        }
        else
        {
            if (opts->no_color)
            {
                fprintf(output_stream, "Error: %s\n", strerror(errno));
            }
            else
            {
                fprintf(output_stream, "%sError: %s%s\n", COLOR_RED, strerror(errno), COLOR_RESET);
            }
        }

        return;
    }

    char line[MAX_LINE_LENGTH];
    int lines_printed = 0;
    bool is_truncated = false;

    while (fgets(line, sizeof(line), file))
    {
        size_t len = strlen(line);

        if (len > 0 && line[len - 1] == '\n')
        {
            line[len - 1] = '\0';
        }

        char *p = line;


        while (*p)
        {
            if ((*p & 0x80) && !(*p & 0x40))
            {
                *p = ' ';
            }

            p++;
        }

        if (len > 50)
        {
            int pos = 0;
            int chars = 0;
            while (pos < len && chars < 47)
            {
                if ((line[pos] & 0xC0) != 0x80)
                    chars++;
                pos++;
            }
            strcpy(&line[pos], "...");
        }

        if (opts->indent_file_preview)
        {
            if (opts->visually_link_previews_with_tree_root)
            {
                fprintf(output_stream, "%s│     %s\n", prefix, line);
            }
            else
            {
                fprintf(output_stream, "%s     %s\n", prefix, line);
            }
        }
        else
        {
            fprintf(output_stream, "%s\n", line);
        }

        lines_printed++;

        if (lines_printed >= opts->preview_lines)
        {
            is_truncated = !feof(file);
            break;
        }
    }

    if (is_truncated)
    {
        if (opts->indent_file_preview)
        {
            if (opts->visually_link_previews_with_tree_root)
            {
                fprintf(output_stream, "%s│     (Preview truncated)...\n", prefix);
            }
            else
            {
                fprintf(output_stream, "%s     (Preview truncated)...\n", prefix);
            }
        }
        else
        {
            fprintf(output_stream, "(Preview truncated)...\n");
        }
    }

    fclose(file);
}

int compare_entries(const void *a, const void *b, const Options *opts)
{
    const DirEntry *entry_a = (const DirEntry *) a;
    const DirEntry *entry_b = (const DirEntry *) b;

    if (opts->dirs_first && entry_a->is_dir != entry_b->is_dir)
    {
        return entry_b->is_dir - entry_a->is_dir;
    }

    if (opts->ignore_case)
    {
        int result = strcasecmp(entry_a->name, entry_b->name);
        return opts->reverse_sort ? -result : result;
    }
    else
    {
        int result = strcmp(entry_a->name, entry_b->name);
        return opts->reverse_sort ? -result : result;
    }
}

void sort_entries(DirEntry *entries, size_t count, const Options *opts)
{
    for (size_t i = 0; i < count - 1; i++)
    {
        for (size_t j = 0; j < count - i - 1; j++)
        {
            if (compare_entries(&entries[j], &entries[j + 1], opts) > 0)
            {
                DirEntry temp = entries[j];
                entries[j] = entries[j + 1];
                entries[j + 1] = temp;
            }
        }
    }
}

void print_entry_info(const DirEntry *entry, const Options *opts, const char *prefix, bool is_last)
{
    fprintf(output_stream, "%s%s── ", prefix, is_last ? "└" : "├");

    if (opts->show_inodes)
    {
        fprintf(output_stream, "[%lu] ", (unsigned long) entry->stat.st_ino);
    }

    if (opts->show_device)
    {
        fprintf(output_stream, "[%lu,%lu] ", (unsigned long) major(entry->stat.st_dev), (unsigned long) minor(entry->stat.st_dev));
    }

    const char *color_code = get_color_code(entry->stat.st_mode, opts);
    fprintf(output_stream, "%s%s%s", color_code, opts->full_path ? entry->full_path : entry->name, opts->colorize ? COLOR_RESET : "");

    if (S_ISLNK(entry->stat.st_mode))
    {
        char link_target[MAX_PATH_LENGTH];
        ssize_t len = readlink(entry->full_path, link_target, sizeof(link_target) - 1);

        if (len != -1)
        {
            link_target[len] = '\0';
            fprintf(output_stream, " -> %s", link_target);
        }
    }

    if (opts->append_type)
    {
        fprintf(output_stream, "%c", get_file_type_indicator(entry->stat.st_mode));
    }

    if (opts->show_size)
    {
        char size_buf[32];
        format_size(size_buf, sizeof(size_buf), entry->stat.st_size);
        fprintf(output_stream, " [%s]", size_buf);
    }

    if (opts->show_user || opts->show_group)
    {
        struct passwd *pw = getpwuid(entry->stat.st_uid);
        struct group *gr = getgrgid(entry->stat.st_gid);

        if (opts->show_user)
        {
            fprintf(output_stream, " %s", pw ? pw->pw_name : "unknown");
        }

        if (opts->show_group)
        {
            fprintf(output_stream, " %s", gr ? gr->gr_name : "unknown");
        }
    }

    if (opts->show_date)
    {
        char date_buf[64];
        strftime(date_buf, sizeof(date_buf), "%Y-%m-%d %H:%M:%S", localtime(&entry->stat.st_mtime));
        fprintf(output_stream, " [%s]", date_buf);
    }

    if ((opts->show_full_file || opts->show_preview) && !entry->is_dir && ( !(S_ISLNK(entry->stat.st_mode)) ) )
    {
        fprintf(output_stream, ":");
    }

    if (entry->is_dir)
    {
        fprintf(output_stream, "/");
    }

    fprintf(output_stream, "\n");
}

int process_directory(const char *path, const Options *opts, int current_depth, const char *prefix)
{
    if (opts->max_depth != DEFAULT_DEPTH && current_depth > opts->max_depth)
    {
        return 0;
    }

    DIR *dir = opendir(path);
    if (!dir)
    {
        if (opts->indent_file_preview)
        {
            if (opts->no_color)
            {
                fprintf(output_stream, "%s│     Error: %s\n", prefix, strerror(errno));
            }
            else
            {
                fprintf(output_stream, "%s%sError: %s%s\n", prefix, COLOR_RED, strerror(errno), COLOR_RESET);
            }
        }
        else
        {
            if (opts->no_color)
            {
                fprintf(output_stream, "Error: %s\n", strerror(errno));
            }
            else
            {
                fprintf(output_stream, "%sError: %s%s\n", COLOR_RED, strerror(errno), COLOR_RESET);
            }
        }

        return -1;
    }

    struct dirent *entry;
    size_t entry_count = 0;
    size_t entry_capacity = 16;
    DirEntry *entries = malloc(entry_capacity * sizeof(DirEntry));

    if (!entries)
    {
        closedir(dir);
        return -1;
    }

    while ((entry = readdir(dir)) != NULL)
    {
        if (!opts->show_hidden && entry->d_name[0] == '.')
        {
            continue;
        }

        if (opts->exclude_patterns_count > 0 && should_exclude(entry->d_name, opts))
        {
            continue;
        }

        if (entry_count >= entry_capacity)
        {
            entry_capacity *= 2;
            DirEntry *new_entries = realloc(entries, entry_capacity * sizeof(DirEntry));

            if (!new_entries)
            {
                free(entries);
                closedir(dir);
                return -1;
            }

            entries = new_entries;
        }

        char full_path[MAX_PATH_LENGTH];
        snprintf(full_path, sizeof(full_path), "%s/%s", path, entry->d_name);

        if (should_exclude(full_path, opts))
        {
            fprintf(output_stream, "%sSkipping excluded: %s\n", prefix, entry->d_name); // Debugging line
            continue;
        }

        if (lstat(full_path, &entries[entry_count].stat) == -1)
        {
            continue;
        }

        if (opts->stay_on_fs && current_depth == 0)
        {
            ((Options *) opts)->start_device = entries[entry_count].stat.st_dev;
        }
        else if (opts->stay_on_fs && entries[entry_count].stat.st_dev != opts->start_device)
        {
            continue;
        }

        if (opts->dirs_only && !S_ISDIR(entries[entry_count].stat.st_mode))
        {
            continue;
        }

        entries[entry_count].name = strdup(entry->d_name);
        entries[entry_count].full_path = strdup(full_path);
        entries[entry_count].is_dir = S_ISDIR(entries[entry_count].stat.st_mode);
        entry_count++;
    }

    rewinddir(dir);

    if (entry_count > 0)
    {
        sort_entries(entries, entry_count, opts);
    }

    for (size_t i = 0; i < entry_count; i++)
    {
        char next_prefix[MAX_PATH_LENGTH];
        snprintf(next_prefix, sizeof(next_prefix), "%s%s", prefix, (i == entry_count - 1) ? "    " : "│   ");
        print_entry_info(&entries[i], opts, prefix, i == entry_count - 1);

        if (entries[i].is_dir && strcmp(entries[i].name, ".") != 0 &&
            strcmp(entries[i].name, "..") != 0)
        {
            total_dirs++;
            process_directory(entries[i].full_path, opts, current_depth + 1, next_prefix);
        }
        else if (!entries[i].is_dir)
        {
            total_files++;

            if (opts->show_preview)
            {
                if (is_binary_file(entries[i].full_path))
                {
                    // TODO: Fix stupid function name "is_binary_file" is not a good name
                    fprintf(output_stream, "%s│     (Can't get pure text from this file, might fix later)\n", prefix);
                    // TODO: Implement file by file cases functions to preview pdf text, docx, csv, xlsx, .pages, .key, etc...
                }
                else
                {
                    print_file_preview(entries[i].full_path, prefix, opts);
                }
            }
        }

        free(entries[i].name);
        free(entries[i].full_path);
    }

    free(entries);
    closedir(dir);
    return 0;
}

