#include <stdio.h>
#include <stdlib.h>
#include <pigpiod_if2.h>

#define HIGH 1
#define LOW 0
#define LEDPIN 17

int main(){
	int pd;
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
	return 0;
}
