/*
 * launcher.c — tiny ELF shim so the Mayhem target `cmd` is a native executable.
 *
 * icalendar is fuzzed with Google's Atheris (a coverage-guided Python fuzzer that emulates the
 * libFuzzer CLI). Mayhem, however, requires the target `cmd` to be an ELF — it rejects script /
 * wrapper targets, and fuzz-smoke checks the ELF magic — so this shim exec()s the CPython
 * interpreter on the Atheris harness, forwarding every argument unchanged. exec() replaces the
 * process image, so the running process IS the Atheris/libFuzzer harness (transparent to Mayhem).
 *
 * When invoked with libFuzzer flags (`-runs=...`, `-max_total_time=...`) the harness iterates like
 * any libFuzzer target; when invoked with a single file argument it replays that input once — so the
 * SAME binary is both the fuzz target and the standalone reproducer.
 *
 * The harness path is fixed at compile time (-DSCRIPT_PATH=...) by the image layout (the repo is
 * COPYed to /mayhem).
 */
#include <unistd.h>
#include <stdlib.h>

#ifndef PYTHON_BIN
#define PYTHON_BIN "/usr/bin/python3"
#endif
#ifndef SCRIPT_PATH
#define SCRIPT_PATH "/mayhem/mayhem/fuzz_cal.py"
#endif

int main(int argc, char **argv) {
    /* new argv: python3 SCRIPT_PATH <forwarded args...> NULL */
    char **a = (char **)calloc((size_t)argc + 2, sizeof(char *));
    if (!a) return 1;
    int n = 0;
    a[n++] = (char *)PYTHON_BIN;
    a[n++] = (char *)SCRIPT_PATH;
    for (int i = 1; i < argc; i++) a[n++] = argv[i];
    a[n] = (char *)0;
    execv(PYTHON_BIN, a);
    /* execv only returns on error */
    return 127;
}
