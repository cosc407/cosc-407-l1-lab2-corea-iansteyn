# Sealed core **A**

Read `lab2-core.md` first. Other sections were given a different core.

```sh
make
./bar given 1 2000         # always start with one thread
./bar given 8 2000         # twice
```

**The alternative** (`src/alt.c`): `given.c` again, with the one repair every
reader of it proposes first — **the last thread in resets the counter to zero
before it broadcasts**, and nothing else changes. One line added.
**It does not work, and the autograder checks that it does not.** Do not repair
it further. It will not print a wrong answer; it will stop, and the watchdog
will tell you so.

The question it answers: **a counter that has to be reset cannot also be the
thing a thread waits on.** Getting that sentence out of your own failed
alternative is worth more than being told it, which is why you are made to
write it.

**Your four questions, for this core**

- **S2.1 mechanism.** Which claim in the header is false, and what is the
  machine doing? State the barrier's invariant in one line first — then say
  which half of it this code keeps, and for how long.
- **S2.2 proof.** An interleaving of **two** threads, as two columns of steps,
  in which a thread is let through a barrier it should have waited at. Then
  three things the output tells you: why `bad` is a different number every run,
  why one thread never shows it, and what `firstbad` is — *and why it is not
  round 1.* Count the waits in a round before you answer that.
- **S2.3 minimality.** Your `fixed` adds state that `given` did not have. Say
  exactly how much, what it is for, and why the counter could not do that job —
  your `alt` is the evidence for the last part, so quote its output. Then the
  other direction: what does your fix cost per round, in locks and wake-ups,
  next to `given`?
- **S3.2 ship it.** `given` is the fastest thing you will measure today and it
  is wrong, so this is about your `fixed`: it is the only correct version you
  have. Would you ship it, and what measurement would change your mind? No
  second half, half the marks.

**One question you may get in the oral.** *Your barrier was right in the first
round and wrong afterwards. Show me the line that makes that true, and tell me
what a second thread was doing at the moment the first one went round again.*
