#ifndef PERF_H_
#define PERF_H_

#include <err.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/resource.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

static const char *_perf_lib         = "unknown";
static int          _perf_trial       = 0;
static int          _perf_mute        = 0;
static int          _perf_iters       = 1;
static const char *_perf_test_group  = "";

static struct timespec _tt0;
static struct timespec _rt0;

#define _PERF_START()                                                                                        \
	if (clock_gettime(CLOCK_THREAD_CPUTIME_ID, &_tt0)) errx(1, "clock_gettime CLOCK_THREAD_CPUTIME_ID"); \
	if (clock_gettime(CLOCK_REALTIME, &_rt0)) errx(1, "clock_gettime CLOCK_REALTIME");

#define _PERF_INIT(_name_)                                                                                           \
	do {                                                                                                         \
		char *_name = _name_;                                                                                \
		struct timespec _tt1;                                                                                \
		struct timespec _rt1;                                                                                \
		if (clock_gettime(CLOCK_THREAD_CPUTIME_ID, &_tt1)) errx(1, "clock_gettime CLOCK_THREAD_CPUTIME_ID"); \
		if (clock_gettime(CLOCK_REALTIME, &_rt1)) errx(1, "clock_gettime CLOCK_REALTIME");

#define _PERF_FINI()                                                                                           \
		struct timespec _tt2;                                                                                  \
		struct timespec _rt2;                                                                                  \
		if (clock_gettime(CLOCK_THREAD_CPUTIME_ID, &_tt2)) errx(1, "clock_gettime CLOCK_THREAD_CPUTIME_ID");   \
		if (clock_gettime(CLOCK_REALTIME, &_rt2)) errx(1, "clock_gettime CLOCK_REALTIME");                     \
		if (!_perf_mute) {                                                                                    \
			printf("%s,%d,%s,perf,%s,%.3f,%.3f,%d,,,\n",                                                \
			       _perf_lib, _perf_trial, _perf_test_group, _name,                                     \
			       (_tt2.tv_sec - _tt1.tv_sec) * 1e3 + (_tt2.tv_nsec - _tt1.tv_nsec) * 1e-6,             \
			       (_rt2.tv_sec - _rt1.tv_sec) * 1e3 + (_rt2.tv_nsec - _rt1.tv_nsec) * 1e-6,             \
			       _perf_iters);                                                                          \
		}                                                                                                     \
		}                                                                                                      \
		while (0)

int
func_perf(char *debug_func_name, int (*foo)(void *), void *ctx)
{
	struct rusage usage;
	int who = RUSAGE_CHILDREN;
	int status;
	struct timespec rt0, rt1;

	if (foo == NULL) err(EXIT_FAILURE, "func_perf foo is null");

	clock_gettime(CLOCK_REALTIME, &rt0);

	int pid = fork();

	if (pid < 0) err(EXIT_FAILURE, "fork");
	if (pid == 0) {
		exit(foo(ctx));
	}

	if (waitpid(pid, &status, 0) < 0) err(EXIT_FAILURE, "waitpid");
	if (getrusage(who, &usage) < 0) err(EXIT_FAILURE, "getrusage");
	clock_gettime(CLOCK_REALTIME, &rt1);

	double total_cpu = (usage.ru_utime.tv_sec + usage.ru_stime.tv_sec) * 1e3
	                  + (usage.ru_utime.tv_usec + usage.ru_stime.tv_usec) * 1e-3;
	double total_real = (rt1.tv_sec - rt0.tv_sec) * 1e3
	                  + (rt1.tv_nsec - rt0.tv_nsec) * 1e-6;

	printf("%s,%d,%s,function,%s,,,1,%.3f,%.3f,%ld,%ld\n",
	       _perf_lib, _perf_trial, _perf_test_group,
	       debug_func_name, total_cpu, total_real,
	       usage.ru_minflt, usage.ru_majflt);

	fprintf(stderr, "----------------------------------------\n");
	fprintf(stderr, "Report for function %s\n", debug_func_name);
	fprintf(stderr, "- User   CPU time  %3.3fms\n", usage.ru_utime.tv_sec * 1e3 + usage.ru_utime.tv_usec * 1e-3);
	fprintf(stderr, "- System CPU time  %3.3fms\n", usage.ru_stime.tv_sec * 1e3 + usage.ru_stime.tv_usec * 1e-3);
	fprintf(stderr, "- Soft page faults %ld\n", usage.ru_minflt);
	fprintf(stderr, "- Hard page faults %ld\n", usage.ru_majflt);
	fprintf(stderr, "----------------------------------------\n");

	return status;
}

#endif /* PERF_H_ */
