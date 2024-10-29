#include <pigpiod_if2.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
// コンパイル方法（bmp180.c をコンパイルして bmp180 を生成する例）
// gcc -Wall -pthread -o bmp180 bmp180.c -lpigpiod_if2 -lrt
// I2C バス番号
#define I2CBUS 1
// BMP180 のI2C インタフェースのアドレス
// 確認方法：i2cdetect -y 1
#define BMP180ADDR 0x77
// レジスタアドレスの定義
// キャリブレーションデータレジスタのアドレス
// AC1-AC6,B1,B2,MB,MC,MD 全て2 バイト（16 ビット）長
// 先頭アドレス
#define CALSADR 0xaa
// 終了アドレス
#define CALEADR 0xbf
// キャリブレーションデータの個数
#define CALDATANUM 11
// キャリブレーション用のデータ配列参照用
#define AC1 0
#define AC2 1
#define AC3 2
#define AC4 3
#define AC5 4
#define AC6 5
#define B1 6
#define B2 7
#define MB 8
#define MC 9
#define MD 10
// データレジスタのアドレス
#define DATAMSB 0xF6
#define DATALSB 0xF7
#define DATAXLSB 0xF8
// コントロールレジスタのアドレス
// b7,b6:oss b5:sco b4-b0:measurement
// oss->０:lowpower,1:standard,2:highres,3:ultrahighres
#define CTRLREG 0xf4
// 測定開始コマンドの定義
#define TEMP 0x2e
#define PRESS0 0x34
#define PRESS1 0x74
#define PRESS2 0xb4
#define PRESS3 0xf4
// リセットレジスタ（書き込み専用）のアドレス
// 0xb6 を書き込むとパワーオンリセット動作
#define RESETREG 0xe0
// ID レジスタ
// 機能チェック用、正常なら読み出すと常に0x55
#define IDREG 0xd0
// 測定モードの定義（oss）
#define LOWPOWER 0
#define STANDARD 1
#define HIGHRES 2
#define ULTRARES 3
// BMP180 の機能チェック用
#define CHECKOK 0
#define CHECKNG -1
// キャリブレーションレジスタ名のテーブル
const char *calregname[11] = {"AC1", "AC2", "AC3", "AC4", "AC5", "AC6",
                              "B1",  "B2",  "MB",  "MC",  "MD"};
// 以下の定数は補正アルゴリズム中にある数値をそのまま入れたもの
// 実際の測定値や補正データの代わりに使えば、期待した動作かどうかの検証ができる
const int testcaldata[11] = {408,  -72, -14383, 32741, 32757, 23153,
                             6190, 4,   -32768, -8711, 2868};
const int testut = 27898;
const int testup = 23843;
const int testoss = 0;
// プロトタイプ宣言
int get_press(int ut, int up, int oss, int *caldata);
int get_temp(int ut, int *caldata);
int get_raw_press(int pd, int fd, int oss);
int get_raw_temp(int pd, int fd);
int check_bmp180_function(int pd, int fd);
int *read_caldata(int pd, int fd, int *caldata);
int main() {
    int pd; // pigpiod に接続したときの接続番号、クライアントの識別番号
    int fd; // ハンドル値、I2C デバイスごとにつく識別番号
    int oss = 1; // 気圧標準測定（2 回サンプリング平均）
    int caldata[CALDATANUM]; // 補正データを格納する配列
    int ut, up; // 生の温度・圧力値（ADC の変換結果そのもの）
    int t, p;   // 補正した後の温度・圧力値
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
    // I2C インタフェースの初期化
    fd = i2c_open(pd, I2CBUS, BMP180ADDR, 0);
    if (fd < 0) {
        printf("I2C 初期化エラー！¥n");
        exit(EXIT_FAILURE);
    }
    // 動作チェック機能を使った動作確認
    if (check_bmp180_function(pd, fd) != CHECKOK) {
        printf("BMP180 の動作確認が不良です。¥n");
        exit(EXIT_FAILURE);
    }
    // printf("BMP180 の動作確認OK¥n¥n");
    read_caldata(pd, fd, caldata);   // 補正データの読み出し
    ut = get_raw_temp(pd, fd);       // 温度測定値の読み出し
    // up = get_raw_press(pd, fd, oss); // 気圧測定値の読み出し
    // t = get_temp(ut, caldata); // 補正計算を行って補正した温度を求める
    // // t = get_temp(testut, testcaldata); のようにすると補正の動作確認ができる
    // printf("気温（補正済み） = %4.1f ℃, ", (float)t / 10.0);
    // p = get_press(ut, up, oss, caldata); // 補正計算を行って補正した気圧を求める
    // // p = get_press(testut, testup, testoss,testcaldata);
    // // で補正の動作確認ができる
    // printf("気圧（補正済み） = %6.2f hPa¥n", (float)p / 100.0);
    // 最後に使用したI2C デバイスをクローズする
    i2c_close(pd, fd);
    // pigpiod との接続を終了する
    pigpio_stop(pd);
    return 0;
}
int get_press(int ut, int up, int oss, int *caldata)
// 測定した温度と気圧データから、気圧の補正計算を行う関数
// 戻り値は補正した気圧の値
{
    // アルゴリズムにしたがってプログラムを書く
    return p;
}
int get_temp(int ut, int *caldata)
// 温度測定値と補正データを引数にとって補正した温度を求める関数
// 戻り値は補正した温度（整数計算のため、真値の10 倍になっているはず）
{
    // アルゴリズムにしたがってプログラムを書く
    return t;
}
int get_raw_press(int pd, int fd, int oss)
// 気圧の測定値を求める関数
// oss で測定時の変換回数を指定する
{
    int m, l, x; // MSB, LSB, XLSB を入れる変数
    int up;      // 計算して求めた値を入れる変数
    // oss の範囲は0 から3 まで
    if (oss < 0)
        oss = 0;
    if (oss > 3)
        oss = 3;
    wiringPiI2CWriteReg8(fd, CTRLREG, PRESS0 + (oss << 6)); // 変換開始
    // 変換時間待ち、oss の値（変換回数=2^oss）によって待ち時間が異なる
    // 時間待ちはtime_sleep( )関数を使用し、正しい引数を指定すること
    switch (oss) {
    case 0:
        time_sleep();
        break; // 5ms 待つ
    case 1:
        time_sleep();
        break; // 8ms 待つ
    case 2:
        time_sleep();
        break; // 14ms 待つ
    default:
        time_sleep(); // 26ms 待つ
    }
    // ここにデータレジスタからMSB, LSB, XLSB を読み出すコードを書く
    // 気圧は3 バイトのデータ値から計算することになるので注意
    m = ;
    l = ;
    x = ;
    // ここに読み出したm, l, x から値を計算するコードを書く
    up = ;
    return up;
}
int get_raw_temp(int pd, int fd) {
    int m, l; // 読み出したMSB, LSB を入れる変数
    int ut;   // 計算で求めた測定温度を入れる変数
    i2c_write_byte_data(pd, fd, CTRLREG, TEMP); // 温度の測定開始
    time_sleep(); // 変換時間待ち、最大変換時間は4.5ms、引数を正しく指定
    // ここにデータレジスタからMSB, LSB を読み出すコードを書く
    // 気圧は2 バイトのデータ値から計算することになるので注意
    m = ;
    l = ;
    // ここに読み出したm, l から値を計算するコードを書く
    ut = ;
    return ut;
}
int check_bmp180_function(int pd, int fd)
// BMP180 の機能チェックを行う関数
{
    int r;
    if (i2c_read_byte_data(pd, fd, IDREG) == 0x55)
        r = CHECKOK;
    else
        r = CHECKNG;
    return r;
}
int *read_caldata(int pd, int fd, int *caldata)
// キャリブレーションデータの読み出し関数
{
    int i;
    int l, m;
    // printf("キャリブレーション値を読み込みます。¥n");
    for (i = 0; i < CALDATANUM; i++) {
        m = i2c_read_byte_data(pd, fd, CALSADR + i * 2);
        l = i2c_read_byte_data(pd, fd, CALSADR + i * 2 + 1);
        caldata[i] = (m << 8) + l; // 符号なし16 ビットに変換
        if ((i != AC4) && (i != AC5) && (i != AC6)) {
            // AC4, AC5, AC6 以外は符号付き16 ビットデータなので対処が必要
            // その対処をここに書く
	    caldata[i] = ~caldata[i] + 1;
        }
    }
    return caldata;
}
