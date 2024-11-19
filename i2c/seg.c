#include <pigpiod_if2.h>
#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>

#define D1 26
#define D2 21
#define D3 20
#define D4 18
#define SEG_A 24
#define SEG_B 19
#define SEG_C 16
#define SEG_D 12
#define SEG_E 6
#define SEG_F 23
#define SEG_G 17
#define SEG_DP 13

const int SEG_PINS[8] = {SEG_A, SEG_B, SEG_C, SEG_D, SEG_E, SEG_F, SEG_G, SEG_DP};
const int D_PINS[4] = {D1, D2, D3, D4};

int display_data[4] = {1, 2, 3, 4};
int pd;

int seg_map[10][8] = {
 //  A  B  C  D  E  F  G  DP 
    {0, 0, 0, 0, 0, 0, 1, 1}, // 0
    {1, 0, 0, 1, 1, 1, 1, 1}, // 1
    {0, 0, 1, 0, 0, 1, 0, 1}, // 2
    {0, 0, 0, 0, 1, 1, 0, 1}, // 3
    {1, 0, 0, 1, 1, 0, 0, 1}, // 4
    {0, 1, 0, 0, 1, 0, 0, 1}, // 5
    {0, 1, 0, 0, 0, 0, 0, 1}, // 6
    {0, 0, 0, 1, 1, 0, 1, 1}, // 7
    {0, 0, 0, 0, 0, 0, 0, 1}, // 8
    {0, 0, 0, 1, 1, 0, 0, 1}, // 9
};

void init(int pd){
    set_mode(pd, D1, PI_OUTPUT);
    set_mode(pd, D2, PI_OUTPUT);
    set_mode(pd, D3, PI_OUTPUT);
    set_mode(pd, D4, PI_OUTPUT);
    set_mode(pd, SEG_A, PI_OUTPUT);
    set_mode(pd, SEG_B, PI_OUTPUT);
    set_mode(pd, SEG_C, PI_OUTPUT);
    set_mode(pd, SEG_D, PI_OUTPUT);
    set_mode(pd, SEG_E, PI_OUTPUT);
    set_mode(pd, SEG_F, PI_OUTPUT);
    set_mode(pd, SEG_G, PI_OUTPUT);
    set_mode(pd, SEG_DP, PI_OUTPUT);
}

void *display_thread(void *arg){
    while (1){
        for (int digit = 0; digit < 4; digit++){
	    for (int i = 0; i < 4; i++){
	        gpio_write(pd, D_PINS[i], 0);
	    }
	    gpio_write(pd, D_PINS[digit], 1);

	    for (int i = 0; i < 8; i++){
	        gpio_write(pd, SEG_PINS[i], seg_map[display_data[digit]][i]);
	    }

	    time_sleep(0.5);
	}
    }
}

int main() {
    int fd; // ハンドル値、I2C デバイスごとにつく識別番号
    // 最初にpigpiod に接続する（NULL: localhost, NULL: default port）
    pd = pigpio_start(NULL, NULL);
    if (pd < 0) {
        printf("pigpiod の接続に失敗しました。¥n");
        printf("pigpiod が起動しているかどうか確認して下さい。¥n");
        printf(" 毎回手動で起動する方法：sudo pigpiod¥n");
        printf(" 自動的に起動させる方法：sudo systemctl enable "
               "pigpiod.service; reboot");
        exit(EXIT_FAILURE);
    }
    init(pd);

    pthread_t thread;
    pthread_create(&thread, NULL, display_thread, NULL);
    while (1){

    }

    // pigpiod との接続を終了する
    pigpio_stop(pd);
    return 0;
}
