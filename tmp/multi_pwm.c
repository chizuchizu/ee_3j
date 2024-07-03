#include <stdio.h>
#include <stdlib.h>
#include <pigpiod_if2.h>
#include <pthread.h>

#define HIGH 1
#define LOW 0
#define LEDPIN 17

typedef struct {
	double freq;
	int steps;
	int control;
} thread_input;


void * thread_func(void * arg){
	int pd;

	thread_input* args = (thread_input*)arg;
	double freq = args->freq;
	int steps = args->steps;
	int control = args->control;

	pd = pigpio_start(NULL, NULL);

	if (pd < 0){
		printf("Failed to connect to pigpiod\n");
		exit(EXIT_FAILURE);
	}

	set_mode(pd, LEDPIN, PI_OUTPUT);
	const double resolution = freq * steps;
	printf("High: %lf sec\n", control / resolution);
	printf("Low : %lf sec\n", (steps - control) / resolution);
	while (1){
		gpio_write(pd, LEDPIN, 1);
		time_sleep(control / resolution);
		gpio_write(pd, LEDPIN, 0);
		time_sleep((steps - control) / resolution);
	}

	pigpio_stop(pd);
}

int main(){
	pthread_t thread;
	double cycle, freq;
	int steps, control;
	printf("Cycle: ");
	scanf("%lf", &cycle);
	freq = 1 / cycle;
	printf("Steps: ");
	scanf("%d", &steps);
	steps--;
	printf("Input: ");
	scanf("%d", &control);

	thread_input args;
	args.freq = freq;
	args.steps = steps;
	args.control = control;

	pthread_create(&thread, NULL, thread_func, (void*)&args);
	pthread_join(thread, NULL);

	return 0;
}
