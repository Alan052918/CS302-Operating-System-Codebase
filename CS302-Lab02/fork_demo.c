#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>

int main() {
	printf("A\n");
	if (fork()) {
		wait(NULL);
	} else {
		printf("B\n");
		if (fork()) {
			wait(NULL);
		} else {
			printf("C\n");
		}
	}
}
