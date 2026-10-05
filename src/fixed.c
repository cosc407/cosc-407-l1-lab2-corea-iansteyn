/* MODEL ANSWER -- Lab 2 core A, instructor copy. Do not release until the
 * Lab 2 oral window has closed.
 *
 * The minimal correction: the counter cannot also be the wake-up condition,
 * because it has to be reset before the next round and a woken thread would
 * then see the reset value. So add one more word of state -- a generation
 * number -- and wait on THAT.
 *
 * Nine lines changed. Everything else, including the cost (one lock and one
 * broadcast per round), is unchanged from given.c, which is the point of S2.3.
 */
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

#include "barrier.h"

typedef struct {
    pthread_mutex_t lock;
    pthread_cond_t  cv;
    int             n;
    int             count;
    unsigned long   gen;      /* THE FIX: which round this barrier is on */
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
    b->gen   = 0;
    return b;
}

static void wait_(void *p)
{
    bar_t *b = (bar_t *)p;

    pthread_mutex_lock(&b->lock);

    unsigned long mine = b->gen;      /* the round I am waiting to leave */

    b->count++;
    if (b->count == b->n) {
        b->count = 0;                 /* re-arm for the next round       */
        b->gen++;                     /* ... and say so, exactly once    */
        pthread_cond_broadcast(&b->cv);
    } else {
        /* A while loop, not an if: cond_wait can return without a matching
         * broadcast, and `gen != mine` is the only thing that means "my round
         * is over". It is monotone, so a thread that is slow to be scheduled
         * still sees that it has been released -- which is what the counter
         * could not do. */
        while (b->gen == mine) {
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

const bar_ops_t bar_fixed = { "fixed", create, wait_, destroy };
