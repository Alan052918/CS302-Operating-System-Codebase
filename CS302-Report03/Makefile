all: deq.out enq.out scheduler.out stat.out

deq.out: deq.c
	gcc -o deq.out deq.c

enq.out: enq.c
	gcc -o enq.out enq.c

scheduler.out: scheduler.c
	gcc -o scheduler.out scheduler.c

stat.out: stat.c
	gcc -o stat.out stat.c

.PHONY: clean
clean:
	rm *.out
