#include <sys/time.h>
#include <sys/resource.h>
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#define N 1000

struct timeval ttime [N];
struct rusage usage [N];

int n = 0;
int period;
int budget;
char* filename;

void* control_thread (void* param)
{
	fprintf(stderr, "Running thread!\n");
	while (1) {
		if(getrusage(RUSAGE_SELF, &(usage[n]))) {
			fprintf(stderr, "Error in getrusage!\n");
			exit(-1);
		}
		if(gettimeofday(&(ttime[n]), NULL)) {
			fprintf(stderr, "Error in gettimeofday!\n");
			exit(-1);
		}
		n++;

		if (n == N) {
			int i;
			fprintf(stderr, "Time to exit!\n");
			FILE * fd;
			fd = fopen(filename, "w");
			for (i = 1; i < N; i++) {
				unsigned long long t = ((ttime[i].tv_sec - ttime[i-1].tv_sec)*1000000) + \
							(ttime[i].tv_usec - ttime[i-1].tv_usec);
				unsigned long long u = ((usage[i].ru_utime.tv_sec  - usage[i-1].ru_utime.tv_sec)*1000000) + \
						       (usage[i].ru_utime.tv_usec - usage[i-1].ru_utime.tv_usec) + \
						       ((usage[i].ru_stime.tv_sec - usage[i-1].ru_stime.tv_sec)*1000000) + \
						       (usage[i].ru_stime.tv_usec - usage[i-1].ru_stime.tv_usec);
				fprintf(fd, "%llu\t%llu", t, u);
				if (u < budget)
					fprintf(fd, "\t*** DEADLINE MISS ***");
				fprintf(fd, "\n");

			}
			fclose(fd);
			exit (0);
		}
		usleep (period);
	}
	return 0;
}


int main (int argc, char* argv[])
{
	pthread_t tid;
	int argument;
	if (argc != 4) {
		printf ("Usage: %s <budget> <period_msec> <filename>\n", argv[0]);
		exit(1);
	}
	budget = 1000*atoi(argv[1]);
	period = 1000*atoi(argv[2]);
	filename = argv[3];
	sleep(5);
	pthread_create(&tid, NULL, control_thread, &argument);
	fprintf(stderr, "Running...\n");
	for(;;);
	return 0;
}

