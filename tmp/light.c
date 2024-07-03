#include <stdio.h>
#include <stdlib.h>

#include <pigpiod_if2.h>


#define HIGH 1
#define LOW 0

#define LEDPIN 17

#define LOOPCOUNT 10

int main(){
	int pd,t;
	pd = pigpio_start(NULL, NULL);

	if (pd < 0){
		printf("Failed to connect to pigpiod\n");
		exit(EXIT_FAILURE);
	}

	printf("====\n");
	set_mode(pd, LEDPIN, PI_OUTPUT);
	t = 0;
	while (t < LOOPCOUNT){
		gpio_write(pd, LEDPIN, 1);
		time_sleep(0.1);
		gpio_write(pd, LEDPIN, 0);
		time_sleep(0.1);
		t++;
	}

	pigpio_stop(pd);
	return 0;
}
