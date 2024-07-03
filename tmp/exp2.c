#include <stdio.h>
#include <stdlib.h>

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
	int pd,t;
	pd = pigpio_start(NULL, NULL);
	init(pd);

	if (pd < 0){
		printf("Failed to connect to pigpiod\n");
		exit(EXIT_FAILURE);
	}

	printf("====\n");
	t = 0;
	int last_0 = 0;
	int i = 0;
	while (1){
		if (gpio_read(pd, Bt)) {
			int cnt = 0;
			while (cnt < 30){
				if (gpio_read(pd, Bt) == 0){
					cnt++;
				} else {
					cnt = 0;
				}
			}
			i++;
			printf("%d\n", i);
			time_sleep(0.01);
		} 
		// if (gpio_read(pd, Bt) == 1 && last_0){
		// 	i++;
		// 	last_0 = 0;
		// 	printf("%d\n", i);
		// 	// time_sleep(0.1);
		// } else if (gpio_read(pd, Bt) == 0){
		// 	last_0 = 1;
		// } else{
		//        	last_0 = 0;
		// }
		control_number(pd, i % 10);
	}

	pigpio_stop(pd);
	return 0;
}
