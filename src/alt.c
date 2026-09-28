/* Lab 2, core A -- THE ALTERNATIVE named in BRIEF.md.
 *
 * given.c with ONE line added: the last thread in resets the counter to zero
 * before it broadcasts. Nothing else changes.
 *
 * IT IS SUPPOSED NOT TO WORK, and the autograder checks that it does not. Do
 * not repair it. It is evidence, not a submission -- it is the reason your
 * fixed.c needs the state it has, and S2.3 quotes its output.
 *
 * Watch what it does rather than what it prints, and note which thread count
 * is the first to show it.
 */
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

#include "barrier.h"

/* TODO: the barrier's state. What has to be shared between the threads, and
 *       what does each thread have to remember for itself? */

static void *create(int nthreads)
{
    /* TODO: allocate it, initialise everything, and return it. Anything a
     *       thread might lock or wait on has to be ready BEFORE the first
     *       thread can reach it. */
    (void)nthreads;
    return NULL;
}

static void wait_(void *p)
{
    /* TODO: the barrier. Write the invariant you are keeping in a comment
     *       above it, in one line, before you write the code -- your report
     *       and your oral both ask you to state it. */
    (void)p;
}

static void destroy(void *p)
{
    /* TODO: release what create() took. Every thread has been joined by the
     *       time this is called. */
    (void)p;
}

const bar_ops_t bar_alt = { "alt", create, wait_, destroy };
