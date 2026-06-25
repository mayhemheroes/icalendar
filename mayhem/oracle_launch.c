/* Thin ELF launcher for the functional self-test / behavioral oracle.
 *
 * mayhem/test.sh runs THIS binary (a project, NON-system ELF under /mayhem) rather than calling
 * python3 directly, so the anti-reward-hack sabotage check (verify-repo §6.3) can neuter it: the
 * sabotage LD_PRELOADs an _exit(0) constructor into every non-system executable. When neutered this
 * launcher exits before exec'ing python, the oracle prints nothing, and test.sh reports a failure —
 * proving the oracle asserts BEHAVIOR, not just exit status. (python3 lives under /usr/bin and is
 * deliberately spared by the sabotage, so a test that shelled out to it directly could never be
 * neutered.) Under normal runs it just execs python3 on oracle.py. */
#include <unistd.h>
#include <stdlib.h>

#ifndef PYTHON_BIN
#define PYTHON_BIN "/usr/bin/python3"
#endif
#ifndef ORACLE_PATH
#define ORACLE_PATH "/mayhem/mayhem/oracle.py"
#endif

int main(void) {
    char *a[] = {(char *)PYTHON_BIN, (char *)ORACLE_PATH, (char *)0};
    execv(PYTHON_BIN, a);
    _exit(127);
}
