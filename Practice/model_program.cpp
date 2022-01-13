/******************************************
* 2021年度伊豆大島電装系メインプログラム *
******************************************/

#include <mbed.h>   // C++でmbedを使うために必須
#include <stdio.h>  // C++の標準ライブラリが入っている
#include <errno.h>  // エラーが出た時わかりやすく教えてくれる

#include "SDFileSystem.h"   // SDカードを使うためのライブラリ

// ↓ここから先輩方が作られた自作ライブラリ
#include "PQ_ADXL375.h"
#include "PQ_GPS.h"
#include "PQ_EEPROM.h"
#include "PQ_ES920.h"
#include "PQ_INA226.h"
#include "PQ_LPS22HB.h"
#include "PQ_MPU9250.h"

/*******************************************************************************
 設定値。プログラムを最後まで書いてくと、どこで使われてるかわかるよ
*******************************************************************************/
#define BURN_TIME       8.0f       // エンジン燃焼時間[s]
#define T_APOGEE        24.0f     //FLIGHTからSEPまでの制限時間[s]
#define T_HEATING       9.0f       // ニクロム線加熱時間[s]
#define T_RELAY_OFF     340.0f      //FLIGHTからRECOVERYまでの時間[s]

#define DEBUG_RATE      1     // printfデバッグの頻度[Hz]
#define READ_RATE       20    // データの取得スピード[Hz]
#define LOG_RATE        1     // データの保存スピード[Hz]
#define DOWNLINK_RATE   1     // ダウンリンクのレート[Hz]（ES920の使用時は1Hz以下にすること）

#define LOG_HEADER      0x11
#define DOWNLINK_HEADER 0x12
#define UPLINK_HEADER   0x13

#define TIME_LSB        54.931640625    // [s/LSB]
#define VOLTAGE_LSB     1.25    //[mV/LSB]
#define CURRENT_LSB     1.0986328125
#define PRESS_LSB       0.0384521484375
#define TEMP_LSB        0.002593994140625
#define ACCEL_LSB       0.00048828125
#define GYRO_LSB        0.06103515625
#define MAG_LSB         0.146484375

#define coef 0.01

/*******************************************************************************
 コンストラクタ
*******************************************************************************/
Serial pc(USBTX, USBRX, 115200);
Serial es_serial(p9, p10, 115200);
Serial gps_serial(p13, p14, 115200);

I2C i2c(p28, p27);

SDFileSystem sd(p5, p6, p7, p8, "sd");

/*
SDBlockDevice blockDevice(p5, p6, p7, p8);  //mosi, miso, sck, cs
FATFileSystem fileSystem("fp");
*/

ES920 es(es_serial);
GPS gps(gps_serial);

EEPROM eeprom(i2c); // 24FC1025*4（0x000000~0x07FFFF）
ADXL375 adxl(i2c, ADXL375::ALT_ADDRESS_HIGH); // 3軸高加速度
LPS22HB lps(i2c, LPS22HB::SA0_LOW); // 気圧，温度
MPU9250 mpu(i2c, MPU9250::AD0_HIGH); // 3軸加速度，3軸角速度，3軸地磁気
INA226 ina_in(i2c, INA226::A0_VS, INA226::A1_VS);   // 電圧，電流（バッテリー）
INA226 ina_ex(i2c, INA226::A0_GND, INA226::A1_GND); // 電圧，電流（外部電源）

Timer mission_timer; // 電源投入からの時間[s]
Timer flight_timer;  // 離床検知からの時間[s]
Timer sd_timer;      // SD用（fcloseのため）
Timer sep_timer;     // 分離時間
Timer reef_timer;

Ticker peak_detection_ticker;   // 頂点検知関数用
Ticker downlink_ticker; // ダウンリンク
Ticker record_ticker;   // データ保存

DigitalIn debug_pin(p19);   // 外部プルアップ：LOW=printfデバッグ
DigitalIn flight_pin(p20);  // 離床検知用フライトピン（外部プルアップ）

DigitalOut sep1(p26);    // 分離機構のニクロム線制御
DigitalOut sep2(p25);    // リーフィング機構のニクロム線制御
DigitalOut relay(p24);   // リレーのオンオフ
DigitalOut mission_sig1(p22); // ミッションへの信号線(カメラ起動に利用)
DigitalOut mission_sig2(p21);

BusOut led(LED1, LED2, LED3, LED4);

/*******************************************************************************
 関数のプロトタイプ宣言。main()より後で使うことになるから予め宣言する。
*******************************************************************************/
void init();
void read();
void peak_detection();
void commamd_handler(char *command);    //アップリンクコマンドの処理
void downlink();
void record();
void dump();

