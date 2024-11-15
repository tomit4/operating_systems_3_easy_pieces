# README - `process-run.py`

This program, called `process-run.py`, allows you to see how the state of a
process state changes as it runs on a CPU. As described in the chapter,
processes can be in a few different states:

- RUNNING - the process is using the CPU right now

- READY - the process could be using the CPU right now but (alas) some other
  process is

- WAITING - the process is waiting on I/O (e.g., it issued a request to a disk)

- DONE - the process is finished executing

In this homework, we'll see how these process states change as a program runs,
and thus learn a little bit better how these things work.

To run the program and get its options, do this:

```sh
prompt> ./process-run.py -h
```

If this doesn't work, type "python" before the command, like this:

```sh
prompt> python process-run.py -h
```

What you should see is this:

```
Usage: process-run.py [options]

Options:
  -h, --help            show this help message and exit
  -s SEED, --seed=SEED  the random seed
  -l PROCESS_LIST, --processlist=PROCESS_LIST
                        a comma-separated list of processes to run, in the
                        form X1:Y1,X2:Y2,... where X is the number of
                        instructions that process should run, and Y the
                        chances (from 0 to 100) that an instruction will use
                        the CPU or issue an IO
  -L IO_LENGTH, --iolength=IO_LENGTH
                        how long an IO takes
  -S PROCESS_SWITCH_BEHAVIOR, --switch=PROCESS_SWITCH_BEHAVIOR
                        when to switch between processes: SWITCH_ON_IO,
                        SWITCH_ON_END
  -I IO_DONE_BEHAVIOR, --iodone=IO_DONE_BEHAVIOR
                        type of behavior when IO ends: IO_RUN_LATER,
                        IO_RUN_IMMEDIATE
  -c                    compute answers for me
  -p, --printstats      print statistics at end; only useful with -c flag
                        (otherwise stats are not printed)
```

The most important option to understand is the PROCESS_LIST (as specified by the
-l or --processlist flags) which specifies exactly what each running program (or
"process") will do. A process consists of instructions, and each instruction can
just do one of two things:

- use the CPU
- issue an IO (and wait for it to complete)

When a process uses the CPU (and does no IO at all), it should simply alternate
between RUNNING on the CPU or being READY to run. For example, here is a simple
run that just has one program being run, and that program only uses the CPU (it
does no IO).

```
prompt> ./process-run.py -l 5:100
Produce a trace of what would happen when you run these processes:
Process 0
  cpu
  cpu
  cpu
  cpu
  cpu
```

Important behaviors: System will switch when the current process is FINISHED or
ISSUES AN IO After IOs, the process issuing the IO will run LATER (when it is
its turn)

```
prompt>
```

Here, the process we specified is "5:100" which means it should consist of 5
instructions, and the chances that each instruction is a CPU instruction are
100%.

You can see what happens to the process by using the -c flag, which computes the
answers for you:

```
prompt> ./process-run.py -l 5:100 -c
Time     PID: 0        CPU        IOs
  1     RUN:cpu          1
  2     RUN:cpu          1
  3     RUN:cpu          1
  4     RUN:cpu          1
  5     RUN:cpu          1
```

This result is not too interesting: the process is simple in the RUN state and
then finishes, using the CPU the whole time and thus keeping the CPU busy the
entire run, and not doing any I/Os.

Let's make it slightly more complex by running two processes:

```
prompt> ./process-run.py -l 5:100,5:100
Produce a trace of what would happen when you run these processes:
Process 0
  cpu
  cpu
  cpu
  cpu
  cpu

Process 1
  cpu
  cpu
  cpu
  cpu
  cpu
```

Important behaviors: Scheduler will switch when the current process is FINISHED
or ISSUES AN IO After IOs, the process issuing the IO will run LATER (when it is
its turn)

In this case, two different processes run, each again just using the CPU. What
happens when the operating system runs them? Let's find out:

```
prompt> ./process-run.py -l 5:100,5:100 -c
Time     PID: 0     PID: 1        CPU        IOs
  1     RUN:cpu      READY          1
  2     RUN:cpu      READY          1
  3     RUN:cpu      READY          1
  4     RUN:cpu      READY          1
  5     RUN:cpu      READY          1
  6        DONE    RUN:cpu          1
  7        DONE    RUN:cpu          1
  8        DONE    RUN:cpu          1
  9        DONE    RUN:cpu          1
 10        DONE    RUN:cpu          1
```

As you can see above, first the process with "process ID" (or "PID") 0 runs,
while process 1 is READY to run but just waits until 0 is done. When 0 is
finished, it moves to the DONE state, while 1 runs. When 1 finishes, the trace
is done.

Let's look at one more example before getting to some questions. In this
example, the process just issues I/O requests. We specify here tht I/Os take 5
time units to complete with the flag -L.

```
prompt> ./process-run.py -l 3:0 -L 5
Produce a trace of what would happen when you run these processes:
Process 0
  io-start
  io-start
  io-start
```

Important behaviors: System will switch when the current process is FINISHED or
ISSUES AN IO After IOs, the process issuing the IO will run LATER (when it is
its turn)

What do you think the execution trace will look like? Let's find out:

```
prompt> ./process-run.py -l 3:0 -L 5 -c
Time     PID: 0        CPU        IOs
  1  RUN:io-start          1
  2     WAITING                     1
  3     WAITING                     1
  4     WAITING                     1
  5     WAITING                     1
  6* RUN:io-start          1
  7     WAITING                     1
  8     WAITING                     1
  9     WAITING                     1
 10     WAITING                     1
 11* RUN:io-start          1
 12     WAITING                     1
 13     WAITING                     1
 14     WAITING                     1
 15     WAITING                     1
 16*       DONE
```

As you can see, the program just issues three I/Os. When each I/O is issued, the
process moves to a WAITING state, and while the device is busy servicing the
I/O, the CPU is idle.

Let's print some stats (run the same command as above, but with the -p flag) to
see some overall behaviors:

```
Stats: Total Time 16
Stats: CPU Busy 3 (18.75%)
Stats: IO Busy  12 (75.00%)
```

As you can see, the trace took 16 clock ticks to run, but the CPU was only busy
less than 20% of the time. The IO device, on the other hand, was quite busy. In
general, we'd like to keep all the devices busy, as that is a better use of
resources.

There are a few other important flags:

```
  -s SEED, --seed=SEED  the random seed
    this gives you way to create a bunch of different jobs randomly

  -L IO_LENGTH, --iolength=IO_LENGTH
    this determines how long IOs take to complete (default is 5 ticks)

  -S PROCESS_SWITCH_BEHAVIOR, --switch=PROCESS_SWITCH_BEHAVIOR
                        when to switch between processes: SWITCH_ON_IO, SWITCH_ON_END
    this determines when we switch to another process:
    - SWITCH_ON_IO, the system will switch when a process issues an IO
    - SWITCH_ON_END, the system will only switch when the current process is done

  -I IO_DONE_BEHAVIOR, --iodone=IO_DONE_BEHAVIOR
                        type of behavior when IO ends: IO_RUN_LATER, IO_RUN_IMMEDIATE
    this determines when a process runs after it issues an IO:
    - IO_RUN_IMMEDIATE: switch to this process right now
    - IO_RUN_LATER: switch to this process when it is natural to
      (e.g., depending on process-switching behavior)
```

Now go answer the questions at the back of the chapter to learn more.

## Homework (Simulation)

This program, `process-run.py`, allows you to see how process states change as
programs run and either use the CPU (e.g., perform an add instruction) or do I/O
(e.g., send a request to a disk and wait for it to complete). See the README for
details.

**Questions**

1. Run `process-run.py` with the following flags: `-l 5:100,5:100`. What should
   the CPU utilization be (e.g., the percent of time the CPU is in use?) Why do
   you know this? Use the `-c` and `-p` flags to see if you were right.

   A: The CPU utilization should be at 100%. By running the `-h` flag, we see
   that running `5:100` after the `-l` flag tells the script to utilize 100% of
   the cpu to run, in this case 5 instructions. By comma separating the
   instructions, we are telling the CPU to run 2 processes, with 5 instructions
   each, with a 100% chance that the instruction will use the CPU or issue an
   IO.

   Running `-c -p` outputs:

```
Time	PID: 0		1		CPU	IOs
1	RUN:cpu		READY		1

2	RUN:cpu		READY		1

3	RUN:cpu		READY		1

4	RUN:cpu		READY		1

5	RUN:cpu		READY		1

6	DONE		RUN:cpu		1

7	DONE		RUN:cpu		1

8	DONE		RUN:cpu		1

9	DONE		RUN:cpu		1

10	DONE		RUN:cpu		1


Stats: Total Time 10
Stats: CPU Busy 10 (100.0%)
Stats: IO Busy  0 (0.0%)
```

2. Now run with these flags: `.process-run.py -l 4:100,1:0`. These flags specify
   one rpocess with 4 instructions (all to use the CPU), and one that simply
   issues an I/O and waits for it to be done. How long does it take to complete
   both processes? Use `-c` and `-p` to find out if you were right.

   A: It takes the CPU 4 cycles to complete the first set of instructions, as it
   has a 100% chance of running these instructions(i.e. `4:100`), while the
   second set of instructions (i.e. `1:0`) gives only one instruction, but has a
   0% chance of running, meaning it will <em>not</em> run. Runnhing it as such
   outputs:

```
Time	PID: 0		1		CPU	IOs
1	RUN:io		READY		1

2	WAITING		RUN:io		1
	1
3	WAITING		WAITING			2
4	WAITING		WAITING			2
5	WAITING		WAITING			2
6	RUN:io		WAITING		1
	1
7	WAITING		DONE			1
8	WAITING		DONE			1
9	WAITING		DONE			1
10	WAITING		DONE			1
11	RUN:io		DONE		1

12	WAITING		DONE			1
13	WAITING		DONE			1
14	WAITING		DONE			1
15	WAITING		DONE			1
16	RUN:io		DONE		1

17	WAITING		DONE			1
18	WAITING		DONE			1
19	WAITING		DONE			1
20	WAITING		DONE			1
21	DONE		DONE

Stats: Total Time 21
Stats: CPU Busy 5 (23.80952380952381%)
Stats: IO Busy  17 (80.95238095238095%)
```

3. Switch the order of the processes: `-l 1:0, 4:100`. What happens now? Does
   switching the order matter? Why? (As always, use `-c` and `-p` to see if you
   were right.)

   A: Switching the order does <em>not</em> matter, as the CPU loads the
   instructions into memory before executing them. The instructions
   <em>are</em> blocking, but they block only after the instructions have been
   loaded into memory. Because the first process has a 0% chance of running, it
   immediately loads the next set of 4 instructions, having no affect on the
   output:

```
Time	PID: 0		1		CPU	IOs
1	RUN:io		READY		1

2	WAITING		RUN:io		1
	1
3	WAITING		WAITING			2
4	WAITING		WAITING			2
5	WAITING		WAITING			2
6	DONE		WAITING			1
7	DONE		RUN:io		1

8	DONE		WAITING			1
9	DONE		WAITING			1
10	DONE		WAITING			1
11	DONE		WAITING			1
12	DONE		RUN:io		1

13	DONE		WAITING			1
14	DONE		WAITING			1
15	DONE		WAITING			1
16	DONE		WAITING			1
17	DONE		RUN:io		1

18	DONE		WAITING			1
19	DONE		WAITING			1
20	DONE		WAITING			1
21	DONE		WAITING			1
22	DONE		DONE

Stats: Total Time 22
Stats: CPU Busy 5 (22.727272727272727%)
Stats: IO Busy  17 (77.27272727272727%)
```

4. We'll now explore some of the other flags. One important flag is `-S`, which
   determines how the system reacts when a process issues an I/O. With the flag
   set to `SWITCH_ON_END`, the system will NOT switch to another process while
   one is doing I/O, instead waiting until the process is completely finished.
   What happens when you run the following two processess
   (`-l 1:0, 4:100 -c -S SWITCH_ON_END`), one doing I/O and the other doing CPU
   work?

   A: The total time will take longer in this case, as we are explicitly telling
   the script to wait for one process's IO to finish prior to the other one
   running IO. this is exemplified in the output of the IO always being `1` when
   printed out:

```
Time	PID: 0		1		CPU	IOs
1	RUN:io		READY		1

2	WAITING		READY			1
3	WAITING		READY			1
4	WAITING		READY			1
5	WAITING		READY			1
6	DONE		RUN:io		1

7	DONE		WAITING			1
8	DONE		WAITING			1
9	DONE		WAITING			1
10	DONE		WAITING			1
11	DONE		RUN:io		1

12	DONE		WAITING			1
13	DONE		WAITING			1
14	DONE		WAITING			1
15	DONE		WAITING			1
16	DONE		RUN:io		1

17	DONE		WAITING			1
18	DONE		WAITING			1
19	DONE		WAITING			1
20	DONE		WAITING			1
21	DONE		RUN:io		1

22	DONE		WAITING			1
23	DONE		WAITING			1
24	DONE		WAITING			1
25	DONE		WAITING			1
26	DONE		DONE

Stats: Total Time 26
Stats: CPU Busy 5 (19.23076923076923%)
Stats: IO Busy  20 (76.92307692307693%)
```

5. Now, run the same processes, but with the switching behavior set to switch to
   another process whenever one is WAITING for I/O
   (`-l 1:0,4:100 -c -S SWITCH_ON_IO`). What happens now? Use `-c` and `-p` to
   confirm that you are right.

   A: It appears we are telling the script to switch to the next CPU instruction
   when an IO has finished. This means that we can have 2 IO processes running
   at once, which is what the output indicates:

```
Time	PID: 0		1		CPU	IOs
1	RUN:io		READY		1

2	WAITING		RUN:io		1
	1
3	WAITING		WAITING			2
4	WAITING		WAITING			2
5	WAITING		WAITING			2
6	DONE		WAITING			1
7	DONE		RUN:io		1

8	DONE		WAITING			1
9	DONE		WAITING			1
10	DONE		WAITING			1
11	DONE		WAITING			1
12	DONE		RUN:io		1

13	DONE		WAITING			1
14	DONE		WAITING			1
15	DONE		WAITING			1
16	DONE		WAITING			1
17	DONE		RUN:io		1

18	DONE		WAITING			1
19	DONE		WAITING			1
20	DONE		WAITING			1
21	DONE		WAITING			1
22	DONE		DONE

Stats: Total Time 22
Stats: CPU Busy 5 (22.727272727272727%)
Stats: IO Busy  17 (77.27272727272727%)
```

6. One other important behavior is what to do when an I/O completes. With
   `-I
   IO_RUN_LATER`, when an I/O completes, the process that issued it is
   not necessarily run right away; rather, whatever was running at the time
   keeps running. What happens when you run this combination of processes?
   (`./process-run.py -l 3:0,5:100,5:100,5:100 -S SWITCH_ON_IO -c -p -I IO_RUN_LATER`)
   Are system resources being effectively utilized?

   A: This appears to load all of the CPU instructions into memory first, then
   afterwards, processes the io operations afterwards. My assumption is that
   this is not an effective use of system resources, as we are not loading
   processes and executing instructions in tandem, but rather loading all
   processes prior to executing instructions, taking up a lot of CPU processing
   power at first, and then taking up a lot of memory second during processing
   of IO.

```
Time	PID: 0		1		2		3		CPU	IOs
1	RUN:io		READY		READY		READY		1

2	WAITING		RUN:cpu		READY		READY		1
	1
3	WAITING		RUN:cpu		READY		READY		1
	1
4	WAITING		RUN:cpu		READY		READY		1
	1
5	WAITING		RUN:cpu		READY		READY		1
	1
6	READY		RUN:cpu		READY		READY		1

7	READY		DONE		RUN:cpu		READY		1

8	READY		DONE		RUN:cpu		READY		1

9	READY		DONE		RUN:cpu		READY		1

10	READY		DONE		RUN:cpu		READY		1

11	READY		DONE		RUN:cpu		READY		1

12	READY		DONE		DONE		RUN:cpu		1

13	READY		DONE		DONE		RUN:cpu		1

14	READY		DONE		DONE		RUN:cpu		1

15	READY		DONE		DONE		RUN:cpu		1

16	READY		DONE		DONE		RUN:cpu		1

17	RUN:io		DONE		DONE		DONE		1

18	WAITING		DONE		DONE		DONE			1
19	WAITING		DONE		DONE		DONE			1
20	WAITING		DONE		DONE		DONE			1
21	WAITING		DONE		DONE		DONE			1
22	RUN:io		DONE		DONE		DONE		1

23	WAITING		DONE		DONE		DONE			1
24	WAITING		DONE		DONE		DONE			1
25	WAITING		DONE		DONE		DONE			1
26	WAITING		DONE		DONE		DONE			1
27	DONE		DONE		DONE		DONE

Stats: Total Time 27
Stats: CPU Busy 18 (66.66666666666667%)
Stats: IO Busy  12 (44.44444444444444%)
```

7. Now run the same procesess, but with `-I IO_RUN_IMMEDIATE` set, which
   immediately runs the process that issued the I/O. How does this behavior
   differ? Why might running a process that just completed an I/O again be a
   good idea?

   A: This appears to do the exact opposite, loading the instructions
   immediately into Memory and executing them as soon as they are ready. This
   appears to be more efficient in that it is probably clearing memory and
   registers as soon as the instructions are finished executing, efficiently
   utilizing resources in a sequential manner (essentially a queue).

```
Time	PID: 0		1		2		3		CPU	IOs
1	RUN:io		READY		READY		READY		1

2	WAITING		RUN:cpu		READY		READY		1
	1
3	WAITING		RUN:cpu		READY		READY		1
	1
4	WAITING		RUN:cpu		READY		READY		1
	1
5	WAITING		RUN:cpu		READY		READY		1
	1
6	RUN:io		READY		READY		READY		1

7	WAITING		RUN:cpu		READY		READY		1
	1
8	WAITING		DONE		RUN:cpu		READY		1
	1
9	WAITING		DONE		RUN:cpu		READY		1
	1
10	WAITING		DONE		RUN:cpu		READY		1
	1
11	RUN:io		DONE		READY		READY		1

12	WAITING		DONE		RUN:cpu		READY		1
	1
13	WAITING		DONE		RUN:cpu		READY		1
	1
14	WAITING		DONE		DONE		RUN:cpu		1
	1
15	WAITING		DONE		DONE		RUN:cpu		1
	1
16	DONE		DONE		DONE		RUN:cpu		1

17	DONE		DONE		DONE		RUN:cpu		1

18	DONE		DONE		DONE		RUN:cpu		1


Stats: Total Time 18
Stats: CPU Busy 18 (100.0%)
Stats: IO Busy  12 (66.66666666666666%)
```

8. Now run with some randomly generated processes using flags
   `-s 1 -l 3:50,3:50` or `-s 2 -l 3:50,3:50` or `-s 3 -l 3:50,3:50`. See if you
   can predict how the trace will turn out. What happens when you use the flag
   `-I IO_RUN_IMMEDIATE` verses that flag `-I IO_RUN_LATER`? What happens when
   you use the flag `-S SWITCH_ON_IO` versus `-S SWITCH_ON_END`?

```
prompt> ./proccess-run.py -l 3:50,3:50 -c -p -I IO_RUN_LATER
Time	PID: 0		1		CPU	IOs
1	RUN:io		READY		1

2	WAITING		RUN:cpu		1
	1
3	WAITING		RUN:io		1
	1
4	WAITING		WAITING			2
5	WAITING		WAITING			2
6	RUN:io		WAITING		1
	1
7	WAITING		WAITING			2
8	WAITING		RUN:cpu		1
	1
9	WAITING		DONE			1
10	WAITING		DONE			1
11	RUN:cpu		DONE		1


Stats: Total Time 11
Stats: CPU Busy 6 (54.54545454545455%)
Stats: IO Busy  9 (81.81818181818183%)
```

```
prompt> ./proccess-run.py -s 2 -l 3:50,3:50 -c -p
Time	PID: 0		1		CPU	IOs
1	RUN:io		READY		1

2	WAITING		RUN:cpu		1
	1
3	WAITING		RUN:io		1
	1
4	WAITING		WAITING			2
5	WAITING		WAITING			2
6	RUN:io		WAITING		1
	1
7	WAITING		WAITING			2
8	WAITING		RUN:io		1
	1
9	WAITING		WAITING			2
10	WAITING		WAITING			2
11	RUN:cpu		WAITING		1
	1
12	DONE		WAITING			1
13	DONE		DONE

Stats: Total Time 13
Stats: CPU Busy 6 (46.15384615384615%)
Stats: IO Busy  11 (84.61538461538461%)
```

```
prompt> ./proccess-run.py -s 3 -l 3:50,3:50 -c -p
Time	PID: 0		1		CPU	IOs
1	RUN:cpu		READY		1

2	RUN:io		READY		1

3	WAITING		RUN:io		1
	1
4	WAITING		WAITING			2
5	WAITING		WAITING			2
6	WAITING		WAITING			2
7	RUN:cpu		WAITING		1
	1
8	DONE		RUN:io		1

9	DONE		WAITING			1
10	DONE		WAITING			1
11	DONE		WAITING			1
12	DONE		WAITING			1
13	DONE		RUN:cpu		1


Stats: Total Time 13
Stats: CPU Busy 6 (46.15384615384615%)
Stats: IO Busy  9 (69.23076923076923%)
```
