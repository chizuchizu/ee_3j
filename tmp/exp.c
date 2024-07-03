#include <stdio.h>
#include <stdlib.h>

#include <pigpiod_if2.h>

#define HIGH 1
#define LOW 0

#define Bt 19

#define LOOPCOUNT 10

void init(int pd){
	set_mode(pd, Bt, PI_INPUT);
}

int last_0 = 0;

void button_callback(int pd, int level, uint32_t tick){
	printf("%d\n", level);
}

int main(){
	int pd,t;
	pd = pigpio_start(NULL, NULL);
	init(pd);

	if (pd < 0){
		printf("Failed to connect to pigpiod\n");
		exit(EXIT_FAILURE);
	}

	printf("====\n");
	int i = 0;
	t = 0;
	int last_0 = 0;
	while (1){
		if (gpio_read(pd, Bt) == 1 && last_0){
			i++;
			last_0 = 0;
			printf("%d\n", i);
			// time_sleep(0.1);
		} else if (gpio_read(pd, Bt) == 0){
			last_0 = 1;
		} else{
		       	last_0 = 0;
		}
		// control_number(pd, i % 10);
		//for (int i = 0; i < 10; i++){
		//	control_number(pd, i);
		//	time_sleep(1);
		//}
	}

	pigpio_stop(pd);
	return 0;
}
