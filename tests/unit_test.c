/* Lab 2, core A -- YOUR OWN unit tests for bar_fixed and bar_alt.
 *
 * Unlike src/main.c, which drives the whole workload through ./bar, these
 * tests call create()/wait()/destroy() directly on a small, controlled
 * scenario -- no subprocess, no parsing printed output, and no timing noise.
 *
 * EVERY TEST HAS A TIMEOUT (`.timeout = BAR_TIMEOUT` below), the same
 * protection tests/test_mysem.c uses in Part A: if a barrier you wrote
 * deadlocks, Criterion reports "Timed out" instead of hanging your terminal
 * or this check. That line IS useful information -- it means some thread is
 * waiting for a release that will never come.
 *
 * Two helpers are given, below, and you do not need to change them:
 *
 *   run_reuse_check(ops, nthreads, rounds)  runs several threads through
 *       several rounds and returns how many times a thread saw a slot from
 *       the wrong round. 0 means the barrier held everyone back correctly,
 *       every round.
 *
 *   expect_stuck(ops, bar, nthreads)  for a barrier that is SUPPOSED to
 *       deadlock (see below) -- starts nthreads threads on it, waits one
 *       second, and returns how many of them got out. Use this one, not
 *       run_reuse_check, wherever a barrier is meant to trap its callers.
 *
 * One worked example is done for each of bar_fixed and bar_alt. Write at
 * least TWO MORE cases per function, covering:
 *   - a thread count of 1 (the trivial case: nothing to actually wait for)
 *   - more threads than this machine has cores (barrier.h's MAX_THREADS=128
 *     exists for exactly this -- a barrier that only works when every thread
 *     gets a real core is not a barrier)
 *
 * Build and run: make unit-test
 */
#include <criterion/criterion.h>

#include <pthread.h>
#include <stdlib.h>
#include <time.h>

#include "barrier.h"

#define BAR_TIMEOUT      5.0
#define BAR_TIMEOUT_SLOW 20.0  /* spin/poll barriers under heavy oversubscription can be genuinely, correctly slow -- not stuck */

/* Shared state for one run of the reusability check below. Each thread
 * writes the round number into its own slot, then (after both waits) every
 * thread's slot must say the SAME round -- see main.c's own comment on why
 * this needs two waits per round, not one. */
typedef struct {
    const bar_ops_t *ops;
    void             *bar;
    int               id;
    int               nthreads;
    long long         rounds;
    volatile int     *slot;
    int               bad;
} task_t;

static void *reuse_worker(void *p)
{
    task_t *t = (task_t *)p;
    for (long long r = 0; r < t->rounds; r++) {
        t->slot[t->id] = (int)r;
        t->ops->wait(t->bar);                 /* everyone has written */
        for (int j = 0; j < t->nthreads; j++) {
            if (t->slot[j] != (int)r) {
                t->bad++;
            }
        }
        t->ops->wait(t->bar);                 /* everyone has read, safe to write r+1 */
    }
    return NULL;
}

/* GIVEN. run_reuse_check(ops, nthreads, rounds) -> total bad sightings over
 * everyone. 0 means the barrier held every thread back on every round,
 * exactly as many times as it was reused. */
static int run_reuse_check(const bar_ops_t *ops, int nthreads, long long rounds)
{
    void         *bar  = ops->create(nthreads);
    pthread_t     tid[MAX_THREADS];
    task_t        task[MAX_THREADS];
    volatile int  slot[MAX_THREADS];

    cr_assert_not_null(bar, "create() returned NULL");

    for (int i = 0; i < nthreads; i++) {
        task[i].ops = ops;
        task[i].bar = bar;
        task[i].id = i;
        task[i].nthreads = nthreads;
        task[i].rounds = rounds;
        task[i].slot = slot;
        task[i].bad = 0;
        pthread_create(&tid[i], NULL, reuse_worker, &task[i]);
    }
    int total_bad = 0;
    for (int i = 0; i < nthreads; i++) {
        pthread_join(tid[i], NULL);
        total_bad += task[i].bad;
    }
    ops->destroy(bar);
    return total_bad;
}

/* GIVEN. This core's alt is SUPPOSED to deadlock above one thread -- see
 * tests/expect.env's ALT_CORRECT and BRIEF.md. Proving that means proving
 * the opposite of run_reuse_check: that most threads never return at all.
 * Detached, not joined -- a thread stuck forever cannot safely be joined, but
 * Criterion forks every test into its own process, so a still-stuck thread
 * is simply killed when this test's process exits. Nothing leaks. */
static volatile int expect_finished;

static void *deadlock_worker(void *p)
{
    void **ctx = (void **)p;
    ((const bar_ops_t *)ctx[0])->wait(ctx[1]);
    __sync_fetch_and_add(&expect_finished, 1);
    return NULL;
}

/* GIVEN. expect_stuck(ops, bar, nthreads) -> how many threads got OUT of
 * wait() within one second. Call this on a fresh barrier expected to trap
 * most of its callers. */
static int expect_stuck(const bar_ops_t *ops, void *bar, int nthreads)
{
    pthread_t tid[MAX_THREADS];
    void     *ctx[MAX_THREADS][2];
    expect_finished = 0;

    for (int i = 0; i < nthreads; i++) {
        ctx[i][0] = (void *)ops;
        ctx[i][1] = bar;
        pthread_create(&tid[i], NULL, deadlock_worker, ctx[i]);
        pthread_detach(tid[i]);
    }
    struct timespec ts = { .tv_sec = 1, .tv_nsec = 0 };
    nanosleep(&ts, NULL);
    return expect_finished;
}

/* ---------------------------------------------------------- bar_fixed --- */

/* WORKED EXAMPLE -- the pattern every fixed case below follows: run several
 * threads through several rounds, assert nobody ever saw a stale slot. */
Test(fixed, reusable_across_several_rounds, .timeout = BAR_TIMEOUT)
{
    cr_assert_eq(run_reuse_check(&bar_fixed, 4, 20), 0,
                 "a reused bar_fixed let a thread through before everyone "
                 "had left the previous round");
}

/* TODO: single_thread -- nthreads=1. Nothing to actually wait for, but
 * create()/wait()/destroy() still have to work. */
Test(fixed, single_thread, .timeout = BAR_TIMEOUT)
{
    cr_assert_fail("TODO: write this case -- see the worked example above");
}

/* TODO: more_threads_than_cores -- pick an nthreads well above what this
 * machine actually has (nproc), still under MAX_THREADS. This is exactly
 * the case barrier.h's comment on MAX_THREADS=128 exists for. */
Test(fixed, more_threads_than_cores, .timeout = BAR_TIMEOUT_SLOW)
{
    cr_assert_fail("TODO: write this case -- see the worked example above");
}

/* ------------------------------------------------------------ bar_alt --- */
/* THIS CORE'S alt IS SUPPOSED TO DEADLOCK above one thread -- see
 * tests/expect.env's ALT_CORRECT and BRIEF.md. These assert that it traps
 * its callers, not that it computes anything -- asserting the opposite of
 * this core's alt would mean it got accidentally fixed. */

/* WORKED EXAMPLE -- uses expect_stuck, not run_reuse_check, because this
 * function is supposed to fail its callers, not serve them correctly. */
Test(alt, reusable_across_several_rounds, .timeout = BAR_TIMEOUT)
{
    void *bar = bar_alt.create(4);
    int finished = expect_stuck(&bar_alt, bar, 4);
    cr_assert_lt(finished, 4,
                 "bar_alt let all 4 threads out -- it isn't supposed to");
}

/* TODO: single_thread -- read alt.c's own comment on what is supposed to
 * happen at exactly one thread. It is not the same answer as above. */
Test(alt, single_thread, .timeout = BAR_TIMEOUT)
{
    cr_assert_fail("TODO: write this case -- see the worked example above");
}

/* TODO: more_threads_than_cores -- same idea as the worked example, at a
 * higher thread count. */
Test(alt, more_threads_than_cores, .timeout = BAR_TIMEOUT_SLOW)
{
    cr_assert_fail("TODO: write this case -- see the worked example above");
}
