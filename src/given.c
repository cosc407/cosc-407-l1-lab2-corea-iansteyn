/* COSC 407/507 Lab 2, core A -- the barrier you were handed.
 *
 * *** DO NOT EDIT. *** It is hashed by the autograder, and your report has to
 * compare against the code you were handed. Work in fixed.c and alt.c.
 *
 * ---------------------------------------------------------------------------
 * What the author believed, in their own words:
 *
 *   "A barrier is a counter and a condition variable, and that is all it is.
 *    A thread arriving takes the lock and increments the count. If it is the
 *    last one in, it wakes everybody; if it is not, it waits until the count
 *    has reached n.
 *
 *    When the last thread broadcasts, all n threads are inside the barrier and
 *    every one of them is released, so the counter is back where it started
 *    and the barrier is ready to be used again. I tested it: every thread
 *    comes out of the first round with the same data, which is the whole point
 *    of a barrier. It can be used as many times as you like."
 *
 * Exactly one of the claims in that paragraph is false.
 * ---------------------------------------------------------------------------
 */
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

#include "barrier.h"

typedef struct {
    pthread_mutex_t lock;
    pthread_cond_t  cv;
    int             n;        /* how many threads have to arrive */
    int             count;    /* how many have arrived           */
} bar_t;

static void *create(int nthreads)
{
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
    bar_t *b = (bar_t *)p;

    pthread_mutex_lock(&b->lock);

    b->count++;
    if (b->count == b->n) {
        /* last one in: let everybody go */
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
    bar_t *b = (bar_t *)p;
    pthread_mutex_destroy(&b->lock);
    pthread_cond_destroy(&b->cv);
    free(b);
}

const bar_ops_t bar_given = { "given", create, wait_, destroy };
