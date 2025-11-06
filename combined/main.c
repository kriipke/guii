// Multicall wrapper to run iiwm (dwm) or iist (st) from a single binary.
#include <string.h>
#include <stdio.h>

int dwm_main(int argc, char *argv[]);
int st_main(int argc, char *argv[]);

static int
chain_exec(int (*fn)(int, char **), const char *progname, int argc, char **argv, int drop)
{
    if (drop > argc)
        drop = argc;
    argv += drop;
    argc -= drop;

    if (argc <= 0) {
        char *fallback[2] = {(char *)progname, NULL};
        return fn(1, fallback);
    }

    argv[0] = (char *)progname;
    return fn(argc, argv);
}

static int endswith(const char *s, const char *suffix) {
    if (!s || !suffix) return 0;
    size_t ls = strlen(s);
    size_t lsf = strlen(suffix);
    if (lsf > ls) return 0;
    return strcmp(s + (ls - lsf), suffix) == 0;
}

int main(int argc, char *argv[]) {
    const char *argv0 = argv[0];
    if (argv0) {
        // Dispatch based on invoked name or suffix
        if (endswith(argv0, "/iiwm") || strcmp(argv0, "iiwm") == 0)
            return chain_exec(dwm_main, "iiwm", argc, argv, 0);
        if (endswith(argv0, "/iist") || strcmp(argv0, "iist") == 0)
            return chain_exec(st_main, "iist", argc, argv, 0);
        if (endswith(argv0, "/guii") || strcmp(argv0, "guii") == 0) {
            // Prefer window manager by default when called as guii
            // Use --iist or --iiwm to force mode
            if (argc > 1) {
                if (strcmp(argv[1], "--iist") == 0)
                    return chain_exec(st_main, "iist", argc, argv, 1);
                if (strcmp(argv[1], "--iiwm") == 0)
                    return chain_exec(dwm_main, "iiwm", argc, argv, 1);
            }
            return chain_exec(dwm_main, "iiwm", argc, argv, 0);
        }
    }
    // Fallback: detect via args
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--iist") == 0)
            return chain_exec(st_main, "iist", argc, argv, i);
        if (strcmp(argv[i], "--iiwm") == 0)
            return chain_exec(dwm_main, "iiwm", argc, argv, i);
    }
    // Default to dwm if nothing matches
    return chain_exec(dwm_main, "iiwm", argc, argv, 0);
}
