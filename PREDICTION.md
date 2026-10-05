# Prediction sheet — push by 0:20, before you compile

> Marked on **having predicted** and on reconciling it in S3.1 — **not on being
> right.** A confident wrong prediction you then explain is full marks. A blank
> page is none. A page timestamped after your first run is worse than none.
>
> Read `src/given.c` and `BRIEF.md`. Run nothing.

Cores: 8
Lab 0 spread: 38.5%

> **P1.** `./bar given` on **one** thread — does it come out right? Yes/no, one
> sentence why.

Yes. Because a single thread never has to wait for any others; it is essentially a serial program.

> **P2.** On **8** threads, pick one and commit to it: right answer / wrong
> answer / it stops. If wrong, roughly how big is `bad`? If it stops, say at
> which of the two waits in a round.

It will hang/stop at the second wait in a round. Because if some thread resets the counter to 0, some other thread may have already entered the second barrier in that same time, and its increment to count there will be lost. Which would mean no threads can ever get past the second barrier.

> **P3.** Three runs at 8 threads — **identical** numbers, or different? Think
> about this one before you write it; it is the most useful line on the page.

Well, I predicted that it stops, so not sure if there would be any numbers... but if there are, I would guess there are different numbers every time.

> **P4.** Seconds, before measuring. Orders of magnitude are what matter. `cpu`
> is process CPU time over all threads, so `cpu`/`time` is how many cores were
> busy — one number per box.

|         | 1 thread: time | 8 threads: time | 8 threads: cpu/time |
|---------|---|---|---|
| `given` | 4 | 12 | 5|
| `fixed` | 5 | 2 | 4 |
| `alt`   | 5 | 19 | 6 |

> **P5.** Fastest and slowest at 8 threads? Name anything you expect to get
> **slower** as threads are added, and anything you expect to stop altogether.

Alt will stop altogether since it resets the counter to 0.
Fixed will be fastest at 8 threads.
Given will get slower as threads are added.
