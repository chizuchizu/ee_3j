# gcc -Wall -pthread -o prog light.c -lpigpiod_if2 -lrt
# gcc -Wall -pthread -o pwm pwm.c -lpigpiod_if2 -lrt
# gcc -Wall -pthread -o multi_pwm multi_pwm.c -lpigpiod_if2 -lrt
# gcc -Wall -pthread -o seg seg.c -lpigpiod_if2 -lrt
gcc -Wall -pthread -o ex2 exp2.c -lpigpiod_if2 -lrt
