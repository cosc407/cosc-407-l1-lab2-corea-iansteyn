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
typedef struct {
    pthread_mutex_t lock;
    pthread_cond_t  cv;
    int             n;        /* how many threads have to arrive */
    int             count;    /* how many have arrived           */
} bar_t;

static void *create(int nthreads)
{
    /* TODO: allocate it, initialise everything, and return it. Anything a
     *       thread might lock or wait on has to be ready BEFORE the first
     *       thread can reach it. */
       bar_t *b = malloc(sizeof *b);

    if (b == NULL) {
        return NULL;
    }

    if (pthread_mutex_init(&b->lock, NULL) != 0 ||
        pthread_cond_init(&b->cv, NULL) != 0) {
        fprintf(stderr, "barrier init failed\n");
        free(b);
        return NULL;
    }

    b->n     = nthreads;
    b->count = 0;

    return b;
}

static void wait_(void *p)
{
    /* TODO: the barrier. Write the invariant you are keeping in a comment
     *       above it, in one line, before you write the code -- your report
     *       and your oral both ask you to state it. */
    bar_t *b = (bar_t *)p;

    pthread_mutex_lock(&b->lock);

    b->count++;
    if (b->count == b->n) {
        /* last one in: reset the count and let everybody go */
        b->count = 0;
        pthread_cond_broadcast(&b->cv);
    } else {
        while (b->count < b->n) {
            pthread_cond_wait(&b->cv, &b->lock);
        }
    }

    pthread_mutex_unlock(&b->lock);
}

static void destroy(void *p)
{
    /* TODO: release what create() took. Every thread has been joined by the
     *       time this is called. */
    bar_t *b = (bar_t *)p;
    pthread_mutex_destroy(&b->lock);
    pthread_cond_destroy(&b->cv);
    free(b);
}

const bar_ops_t bar_alt = { "alt", create, wait_, destroy };
