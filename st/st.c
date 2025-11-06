#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "config.h"

static void
print_usage(const char *argv0)
{
    fprintf(stdout,
            "%s %s\n\n"
            "Usage: %s [--version] [-e command [args...]]\n\n"
            "Without -e the stub launches an interactive shell defined in\n"
            "config.h and waits for it to exit.  The implementation is\n"
            "intentionally small so that guii builds remain self-contained.\n",
            iist_name, iist_version, argv0);
}

static int
run_command(const char *path, char *const argv[])
{
    execvp(path, argv);
    fprintf(stderr, "%s: failed to exec '%s': %s\n", iist_name, path, strerror(errno));
    return 1;
}

int
st_main(int argc, char **argv)
{
    if (argc > 1) {
        if (strcmp(argv[1], "--help") == 0) {
            print_usage(argv[0]);
            return 0;
        }
        if (strcmp(argv[1], "--version") == 0) {
            fprintf(stdout, "%s %s\n%s\n", iist_name, iist_version, iist_message);
            return 0;
        }
        if (strcmp(argv[1], "-e") == 0) {
            if (argc <= 2) {
                fprintf(stderr, "%s: -e expects a command to run\n", argv[0]);
                return 1;
            }
            return run_command(argv[2], &argv[2]);
        }
        fprintf(stderr, "%s: unknown argument '%s'\n", argv[0], argv[1]);
        return 1;
    }

    char *shell_argv[] = {
        (char *)(iist_shell_arg0 && *iist_shell_arg0 ? iist_shell_arg0 : iist_shell),
        (char *)iist_shell_flag,
        NULL
    };

    if (!(iist_shell_flag && *iist_shell_flag)) {
        shell_argv[1] = NULL;
    }

    return run_command(iist_shell, shell_argv);
}
