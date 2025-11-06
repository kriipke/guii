#define _POSIX_C_SOURCE 200809L

#include <limits.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include "config.h"

static volatile sig_atomic_t running = 1;

static void
handle_signal(int sig)
{
    (void)sig;
    running = 0;
}

static void
log_line(const char *event)
{
    time_t now = time(NULL);
    struct tm tm_now;
    char stamp[64];

    if (localtime_r(&now, &tm_now) &&
        strftime(stamp, sizeof(stamp), "%Y-%m-%d %H:%M:%S", &tm_now)) {
        fprintf(stdout, "%s [%s] %s\n", stamp, iiwm_name, event);
    } else {
        fprintf(stdout, "%ld [%s] %s\n", (long)now, iiwm_name, event);
    }
    fflush(stdout);
}

static int
print_usage(const char *argv0)
{
    fprintf(stdout,
            "%s %s\n\n"
            "This repository vendors a lightweight placeholder for dwm.\n"
            "The stub does not manage X11 windows; it simply emits heartbeat\n"
            "messages so downstream packaging and dispatch logic can be tested.\n\n"
            "Usage: %s [--version] [--once] [--heartbeat SECONDS]\n\n"
            "  --version            Print version text and exit.\n"
            "  --once               Print a single heartbeat then exit.\n"
            "  --heartbeat SECONDS  Override the heartbeat interval.\n",
            iiwm_name, iiwm_version, argv0);
    return 0;
}

int
dwm_main(int argc, char **argv)
{
    unsigned int interval = iiwm_idle_seconds;
    int once = 0;

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--version") == 0) {
            fprintf(stdout, "%s %s\n%s\n", iiwm_name, iiwm_version, iiwm_message);
            return 0;
        }
        if (strcmp(argv[i], "--help") == 0) {
            return print_usage(argv[0]);
        }
        if (strcmp(argv[i], "--once") == 0) {
            once = 1;
            continue;
        }
        if (strcmp(argv[i], "--heartbeat") == 0) {
            if (i + 1 >= argc) {
                fprintf(stderr, "%s: --heartbeat requires an argument\n", argv[0]);
                return 1;
            }
            char *end = NULL;
            unsigned long parsed = strtoul(argv[++i], &end, 10);
            /* Ensure parsed value fits in unsigned int and is nonzero */
            if (!end || *end || parsed == 0 || parsed > UINT_MAX) {
                fprintf(stderr, "%s: invalid heartbeat interval '%s'\n", argv[0], argv[i]);
                return 1;
            }
            interval = (unsigned int)parsed;
            continue;
        }
        fprintf(stderr, "%s: unknown argument '%s'\n", argv[0], argv[i]);
        return 1;
    }

    struct sigaction sa = {0};
    sa.sa_handler = handle_signal;
    sigemptyset(&sa.sa_mask);
    sigaction(SIGINT, &sa, NULL);
    sigaction(SIGTERM, &sa, NULL);

    log_line(iiwm_message);

    do {
        log_line("heartbeat");
        if (once)
            break;
        unsigned int remaining = interval;
        while (running && remaining > 0) {
            remaining = sleep(remaining);
        }
    } while (running);

    log_line("exiting");
    return 0;
}
