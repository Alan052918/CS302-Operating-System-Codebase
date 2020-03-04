#include <stdio.h>
#include <unistd.h>

int main() {
	printf("Return value of fork() is %d\n", fork());
	printf("My PID is %d, my parent's PID is %d\n", getpid(), getppid());
}
