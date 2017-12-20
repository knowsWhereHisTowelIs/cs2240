#include <sys/types.h>
#include <signal.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ptrace.h>
#include <sys/reg.h>
#include <sys/syscall.h>
#include <sys/user.h>
#include <sys/wait.h>
#include <unistd.h>

// These shouldn't be global; sue me
static int brks, segvs, traps;

// Prototypes for tested functions
extern void *nu_malloc(size_t);
extern void *nu_free(void *);
extern void *nu_calloc(size_t, size_t);
extern void *nu_realloc(void *, size_t);

// Tests

int test0() {
	int *ptr = nu_malloc(sizeof(int));
	if (ptr == NULL)
	{
		printf("Memory failed to allocate!\n");
		return 1;
	}

	*ptr = 4;
	nu_free(ptr);

	printf("Memory was allocated, used, and freed!\n");
	return 0;
}

int test1() {
#define CHARS 100
	int i;
	nu_malloc(1);

	char *c = nu_calloc(CHARS, sizeof(char));
	for (i = 0; i < CHARS; i++) {
		if (c[i] != 0) {
			printf("Calloc memory was not zeroed!\n");
			return 1;
		}
	}

	nu_free(c);

	printf("Memory was calloc'ded, used, and freed!\n");
	return 0;
#undef CHARS
}

int test2() {
#define TOTAL_ALLOCS 200000
#define ALLOC_SIZE 1024*1024

	nu_malloc(1);

	int i;
	void *ptr = NULL;

	for (i = 0; i < TOTAL_ALLOCS; i++)
	{
		ptr = nu_malloc(ALLOC_SIZE);
		if (ptr == NULL)
		{
			printf("Memory failed to allocate!\n");
			return 1;
		}

		nu_free(ptr);
	}

	printf("Memory was allocated and freed!\n");
	return 0;
#undef TOTAL_ALLOCS
#undef ALLOC_SIZE
}

int test3() {
#define TOTAL_ALLOCS 50000

	nu_malloc(1);

	int i;
	int **arr = nu_malloc(TOTAL_ALLOCS * sizeof(int *));
	if (arr == NULL)
	{
		printf("Memory failed to allocate!\n");
		return 1;
	}

	for (i = 0; i < TOTAL_ALLOCS; i++)
	{
		arr[i] = nu_malloc(sizeof(int));
		if (arr[i] == NULL)
		{
			printf("Memory failed to allocate!\n");
			return 1;
		}

		*(arr[i]) = i;
	}

	for (i = 0; i < TOTAL_ALLOCS; i++)
	{
		if (*(arr[i]) != i)
		{
			printf("Memory failed to contain correct data after many allocations!\n");
			return 2;
		}
	}

	for (i = 0; i < TOTAL_ALLOCS; i++)
		nu_free(arr[i]);

	nu_free(arr);
	printf("Memory was allocated, used, and freed!\n");	
	return 0;
#undef TOTAL_ALLOCS
}


#define START_MALLOC_SIZE 1024*1024
#define STOP_MALLOC_SIZE  1024
void *reduce(void *ptr, int size)
{
	if (size > STOP_MALLOC_SIZE)
	{
		void *ptr1 = nu_realloc(ptr, size / 2);
		void *ptr2 = nu_malloc(size / 2);

		if (ptr1 == NULL || ptr2 == NULL)
		{
			printf("Memory failed to allocate!\n");
			exit(1);
		}

		ptr1 = reduce(ptr1, size / 2);
		ptr2 = reduce(ptr2, size / 2);

		if (*((int *)ptr1) != size / 2 || *((int *)ptr2) != size / 2)
		{
			printf("Memory failed to contain correct data after many allocations!\n");
			exit(2);
		}

		ptr1 = nu_realloc(ptr1, size);
		nu_free(ptr2);

		if (*((int *)ptr1) != size / 2)
		{
			printf("Memory failed to contain correct data after realloc()!\n");
			exit(3);
		}

		*((int *)ptr1) = size;
		return ptr1;
	}
	else
	{
		*((int *)ptr) = size;
		return ptr;
	}


}

int test4()
{
	nu_malloc(1);

	int size = START_MALLOC_SIZE;
	while (size > STOP_MALLOC_SIZE)
	{
		void *ptr = nu_malloc(size);
		ptr = reduce(ptr, size / 2);
		nu_free(ptr);

		size /= 2;
	}

	printf("Memory was allocated, used, and freed!\n");
	return 0;
}
#undef START_MALLOC_SIZE
#undef STOP_MALLOC_SIZE

// The tracer

int tracee(int (*test)(void)) {
	// Indicate to parent we want to be traced, and then stop
	// ourselves to give them a chance to trace us.
	ptrace(PTRACE_TRACEME);
	kill(getpid(), SIGSTOP);
	// Test code; if the debugger properly installs a breakpoint
	// at this location we will receive SIGTRAP.
	exit(test());
}

bool wait_syscall(pid_t child) {
	int status, inject_signal = 0;
	long dr6, ip;
	while (1) {
		// Continue child, stopping at any normal signal, but also
		// generating a special SIGTRAP signal before the entrance or
		// exit of any syscall (thus two such events occur per
		// syscall).
		ptrace(PTRACE_SYSCALL, child, 0, inject_signal);
		// Wait on state to change, as specified above.
		waitpid(child, &status, 0);
		if (WIFEXITED(status)) {
			fprintf(stderr, "child exited with status=%d\n",
					WEXITSTATUS(status));
			return false;
		} else if (WIFSIGNALED(status)) {
			fprintf(stderr, "child terminated by signal=%d\n",
					WTERMSIG(status));
			return false;
		} else if WIFSTOPPED(status) {
			inject_signal = WSTOPSIG(status);
			if (inject_signal == (SIGTRAP | 0x80)) {
				// This originated as a syscall-stop
				return true;
			} else if (inject_signal == SIGTRAP) {
				// This is a true SIGTRAP
				dr6 = ptrace(PTRACE_PEEKUSER, child,
					offsetof(struct user, u_debugreg[6]));
				if (dr6 & 0x1) {
					//fprintf(stderr, "invalid access to brk\n");
					traps++;
					// Suppress signal
					inject_signal = 0;
				}
			} else if (inject_signal == SIGSEGV) {
				fprintf(stderr, "segfault\n");
				segvs++;
				/*
				ip = ptrace(PTRACE_PEEKUSER, child,
					offsetof(struct user_regs_struct, rip));
				ip++;
				ptrace(PTRACE_POKEUSER, child,
					offsetof(struct user_regs_struct, rip));
				*/
				return false;
			}
		} else {
			fprintf(stderr, "unexpected status=%d\n", status);
			return false;
		}
	}
}

void tracer(pid_t child) {
	int status;
	long syscall, arg1, retval;

	// Wait on child to request to be traced.
	if (waitpid(child, &status, 0) != child)
		perror("initial waitpid");
	// Request syscall-stop SIGTRAPs have bit 7 set, distinguishing
	// them from normal SIGTRAPs, which we will use for watchpoints.
	ptrace(PTRACE_SETOPTIONS, child, 0, PTRACE_O_TRACESYSGOOD);

	while (1) {
		// Wait on syscall-enter-stop
		if (!wait_syscall(child)) break;

		syscall = ptrace(PTRACE_PEEKUSER, child,
				offsetof(struct user_regs_struct, orig_rax));
		arg1 = ptrace(PTRACE_PEEKUSER, child,
				offsetof(struct user_regs_struct, rdi));

		// Wait on syscall-exit-stop
		if (!wait_syscall(child)) break;

		retval = ptrace(PTRACE_PEEKUSER, child,
				offsetof(struct user_regs_struct, rax));

		if (syscall == SYS_brk) {
			//fprintf(stderr, "brk(%p) = %p\n", (void *)arg1, (void *)retval);
			brks++;
			// Setup x86 debug register 0 to watch the first
			// invalid location after brk.
			ptrace(PTRACE_POKEUSER, child,
				offsetof(struct user, u_debugreg[0]), arg1);
			// Enable debug register 0 and ensure status register
			// is clear.
			long dr7 = 0;
			dr7 |= 0x1; // enable dr0
			dr7 |= (0x3 << 16); // trigger dr0 on read&write
			ptrace(PTRACE_POKEUSER, child,
				offsetof(struct user, u_debugreg[7]), dr7);
			ptrace(PTRACE_POKEUSER, child,
				offsetof(struct user, u_debugreg[6]), 0);
		}

	}
}

int main(int argc, char *argv[]) {
	int i;
	int (*tests[])(void) = {
		test0,
		test1,
		test2,
		test3,
		test4,
		NULL
	};
	for (i = 0; tests[i] != NULL; i++) {
		fprintf(stderr, "running test %d\n", i);
		pid_t child = fork();
		if (child == 0) {
			return tracee(tests[i]);
		} else {
			tracer(child);
			fprintf(stderr, "summary: brks=%d segvs=%d traps=%d\n\n",
					brks, segvs, traps);
			brks = segvs = traps = 0;
		}
	}
	return 0;
}
