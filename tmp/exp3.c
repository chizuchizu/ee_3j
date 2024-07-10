#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

#include <pigpiod_if2.h>

#define HIGH 1
#define LOW 0

#define LEDPIN 17
#define Pa 17
#define Pb 4
#define Pc 23
#define Pd 25
#define Pe 12
#define Pf 27
#define Pg 22
#define Pdp 18
#define Bt 19

#define LOOPCOUNT 10
#define BOUNCE_TIME 2000  // micro sec

void init(int pd){
	set_mode(pd, Pa, PI_OUTPUT);
	set_mode(pd, Pb, PI_OUTPUT);
	set_mode(pd, Pc, PI_OUTPUT);
	set_mode(pd, Pd, PI_OUTPUT);
	set_mode(pd, Pe, PI_OUTPUT);
	set_mode(pd, Pf, PI_OUTPUT);
	set_mode(pd, Pg, PI_OUTPUT);
	set_mode(pd, Pdp, PI_OUTPUT);
	set_mode(pd, Bt, PI_INPUT);
}

uint64_t current_time_us(){
	struct timespec ts;
	clock_gettime(CLOCK_MONOTONIC, &ts);
	return (uint64_t)ts.tv_sec * 1000000L + ts.tv_nsec / 1000;
}

void control_number(int pd, int num){
	switch (num){
		case 0:
			gpio_write(pd, Pa, HIGH);
			gpio_write(pd, Pb, HIGH);
			gpio_write(pd, Pc, HIGH);
			gpio_write(pd, Pd, HIGH);
			gpio_write(pd, Pe, HIGH);
			gpio_write(pd, Pf, HIGH);
			gpio_write(pd, Pg, LOW);
			gpio_write(pd, Pdp, LOW);
			break;
		case 1:
			gpio_write(pd, Pa, LOW);
			gpio_write(pd, Pb, HIGH);
			gpio_write(pd, Pc, HIGH);
			gpio_write(pd, Pd, LOW);
			gpio_write(pd, Pe, LOW);
			gpio_write(pd, Pf, LOW);
			gpio_write(pd, Pg, LOW);
			gpio_write(pd, Pdp, LOW);
			break;
		case 2:
			gpio_write(pd, Pa, HIGH);
			gpio_write(pd, Pb, HIGH);
			gpio_write(pd, Pc, LOW);
			gpio_write(pd, Pd, HIGH);
			gpio_write(pd, Pe, HIGH);
			gpio_write(pd, Pf, LOW);
			gpio_write(pd, Pg, HIGH);
			gpio_write(pd, Pdp, LOW);
			break;
		case 3:
			gpio_write(pd, Pa, HIGH);
			gpio_write(pd, Pb, HIGH);
			gpio_write(pd, Pc, HIGH);
			gpio_write(pd, Pd, HIGH);
			gpio_write(pd, Pe, LOW);
			gpio_write(pd, Pf, LOW);
			gpio_write(pd, Pg, HIGH);
			gpio_write(pd, Pdp, LOW);
			break;
		case 4:
			gpio_write(pd, Pa, LOW);
			gpio_write(pd, Pb, HIGH);
			gpio_write(pd, Pc, HIGH);
			gpio_write(pd, Pd, LOW);
			gpio_write(pd, Pe, LOW);
			gpio_write(pd, Pf, HIGH);
			gpio_write(pd, Pg, HIGH);
			gpio_write(pd, Pdp, LOW);
			break;
		case 5:
			gpio_write(pd, Pa, HIGH);
			gpio_write(pd, Pb, LOW);
			gpio_write(pd, Pc, HIGH);
			gpio_write(pd, Pd, HIGH);
			gpio_write(pd, Pe, LOW);
			gpio_write(pd, Pf, HIGH);
			gpio_write(pd, Pg, HIGH);
			gpio_write(pd, Pdp, LOW);
			break;
		case 6:
			gpio_write(pd, Pa, HIGH);
			gpio_write(pd, Pb, LOW);
			gpio_write(pd, Pc, HIGH);
			gpio_write(pd, Pd, HIGH);
			gpio_write(pd, Pe, HIGH);
			gpio_write(pd, Pf, HIGH);
			gpio_write(pd, Pg, HIGH);
			gpio_write(pd, Pdp, LOW);
			break;
		case 7:
			gpio_write(pd, Pa, HIGH);
			gpio_write(pd, Pb, HIGH);
			gpio_write(pd, Pc, HIGH);
			gpio_write(pd, Pd, LOW);
			gpio_write(pd, Pe, LOW);
			gpio_write(pd, Pf, HIGH);
			gpio_write(pd, Pg, LOW);
			gpio_write(pd, Pdp, LOW);
			break;
		case 8:
			gpio_write(pd, Pa, HIGH);
			gpio_write(pd, Pb, HIGH);
			gpio_write(pd, Pc, HIGH);
			gpio_write(pd, Pd, HIGH);
			gpio_write(pd, Pe, HIGH);
			gpio_write(pd, Pf, HIGH);
			gpio_write(pd, Pg, HIGH);
			gpio_write(pd, Pdp, LOW);
			break;
		case 9:
			gpio_write(pd, Pa, HIGH);
			gpio_write(pd, Pb, HIGH);
			gpio_write(pd, Pc, HIGH);
			gpio_write(pd, Pd, HIGH);
			gpio_write(pd, Pe, LOW);
			gpio_write(pd, Pf, HIGH);
			gpio_write(pd, Pg, HIGH);
			gpio_write(pd, Pdp, LOW);
			break;
		default:
			break;
	}	

}



int main(){
	int pd;
	pd = pigpio_start(NULL, NULL);
	init(pd);

	if (pd < 0){
		printf("Failed to connect to pigpiod\n");
		exit(EXIT_FAILURE);
	}

	int last_state = HIGH;
	int count = 0;
	int global_count = 0;
	uint64_t last_time = current_time_us();
	while (1){
		int current_state = gpio_read(pd, Bt);
		uint32_t current_time = current_time_us();
		// printf("current: %d, last: %d\n", current_state, last_state);

		if (current_state == LOW && last_state == HIGH){
			if ((current_time - last_time) > BOUNCE_TIME){
				count++;
				printf("%d\n", count);
				last_time = current_time;
				time_sleep(2);
			}
		}
		last_state = current_state;
		control_number(pd, global_count / 20  % 6 + 1);
		global_count++;
		usleep(1000);
	}
	pigpio_stop(pd);
	return 0;
}
