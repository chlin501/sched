/* sched: a simple scheduler written in C.
 * Copyright (C) 2015 Cyphar
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * 1. The above copyright notice and this permission notice shall be included in
 *    all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#if !defined(__SCHED_H__)
#define __SCHED_H__

/* feel free to change this, but it is proportional to time and space complexity for a bunch of operations */
#if !defined(SCHED_BUFFER_SIZE)
#	define SCHED_BUFFER_SIZE 100
#endif

/* the amount of time in milliseconds that the scheduler must be idle in order for idle tasks to be run */
#if !defined(SCHED_IDLE_WINDOW)
#	define SCHED_IDLE_WINDOW 200
#endif

/* task type enum */
enum {
	NOOP,      /* do not execute, it may be overwritten by the next register call */
	IDLE,      /* run at the next idle tick (time related variables are ignored, but should be set to 0) */
	ONCE,      /* run once at (the specified time + the time of registering the task) milliseconds */
	PERIODIC,  /* run every (specified time) milliseconds (starting at the first tick after being registered). */
};

/* task state enum */
enum {
	BORN,   /* new task -- newly registered task that has not yet been run */
	ALIVE,  /* active task -- should only be set for the *currently running task* */
	FROZEN, /* paused task -- tasks that are in a blocking operation and have been rescheduled */
	DEAD,   /* finished task -- tasks that have yielded control and are to be deregistered */
}

/* TODO: make this multi-architecture */
/* NOTE: The following *LITERALLY* only works for x86_64. */

#define FRAME_STACK_SIZE 60
#include "arch/x86_64.h"

typedef void (*task_fp)(void *);

/* generic "task" structure */
/* in order for two task structures to be considered equivalent, all attributes must be equal */
/* any changes made to a _* attribute may (and probably will) be overwritten by the scheduler (or, if you're lucky, crash the scheduler) */
struct task_t {
	/* the task function pointer to be run with the given argument */
	task_fp task;
	void *task_arg;

	/* This is all dark magic. Abandon all hope, ye who enter here. */
	/* used when storing the task state and rescheduling */
	struct frame_t _frame;
	int _state;

	/* a time-related variable, which has a different meaning depending on the flag */
	long mtime;

	/* represents the "next" scheduled time it is meant to run (allows for period tasks) */
	long _next_mtime;

	/* flags for a task, used to provide context for the mtime element */
	int flags;
};

/* overall scheduler structure */
struct sched_t {
	struct task_t *_current_task;
	struct task_t _tasks[SCHED_BUFFER_SIZE];
};

void task_clear(struct task_t *);
void sched_init(struct sched_t *);
int sched_deregister(struct sched_t *, struct task_t);
int sched_register(struct sched_t *, struct task_t);
void sched_tick(struct sched_t *);

#endif
