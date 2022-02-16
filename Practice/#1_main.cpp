/**************************************************
* 2021年度伊豆大島宇宙イベント電装系メインプログラム *
***************************************************/
#include <mbed.h>
#include <stdio.h>
#include <errno.h>

/*******************************************************************************
 定数宣言
*******************************************************************************/
#define BURN_TIME       8.0f       // エンジン燃焼時間[s]
#define T_APOGEE        24.0f      // FLIGHTからSEPまでの制限時間[s]
#define T_HEATING       9.0f       // ニクロム線加熱時間[s]
#define T_RELAY_OFF     340.0f     // FLIGHTからRECOVERYまでの時間[s]

/*******************************************************************************
 関数のプロトタイプ宣言. main()より後で使うので予め宣言する.
*******************************************************************************/
void init();
void read();

/*******************************************************************************
 変数宣言
*******************************************************************************/
Serial pc(USBTX, USBRX, 115200);    // パソコンの画面に映すためのもの

Timer mission_timer; // 電源投入からの時間[s]
Timer flight_timer;  // 離床検知からの時間[s]
Timer sep_timer;     // 1段目の分離用のタイマー
Timer sep2_timer;    // 2段目の分離用のタイマー

DigitalIn flight_pin(p20);  // 離床検知用フライトピン（外部プルアップ）
DigitalOut sep1(p26);       // 1段目のニクロム線制御
DigitalOut sep2(p25);       // 2段目のニクロム線制御
DigitalOut relay(p24);      // リレーのオンオフ

bool launched = false;          // フライトピンが抜けたか？
bool burning = false;           // エンジンが燃焼中か？
bool apogee = false;            // 気圧による頂点検知したか？
bool first_separated  = false;  // 1段目が作動したか？
bool second_separated = false;  // 2段目が作動したか？
bool landed = false;            // 着地したか？

enum {
    SAFETY,      // 動作無し（コマンドでREADYへ移行可）
    READY,       // 離床検知待機（コマンドでSAFETY，FLIGHTへ移行可）
    FLIGHT,      // 離床検知（フライトピン）から分離まで（コマンドでSEP，EMERGENCYへ移行可）
    SEP1,        // 1段目作動フェーズ
    SEP2,        // 2段目作動フェーズ
    EMERGENCY,   // 離床失敗した場合の緊急停止（パラシュート分離を防ぐ. コマンドによる）
    RECOVERY     // 分離終了から回収まで
} phase;




/*******************************************************************************
 メインプログラムのmain()（必ず処理される関数）はここから
*******************************************************************************/
int main() 
{
    init(); // センサの準備などが書かれている(#2で書くよ)

    while(1) {
        read(); // センサ読み取りの関数(#2で書くよ)
        switch(phase) {
            case SAFETY:
                pc.printf("SAFETY\r\n");
                break;
            case READY:
                pc.printf("READY\r\n");
                if(flight_pin.read() == 1) phase = FLIGHT;
                break;
            case FLIGHT:
                pc.printf("FLIGHT\r\n");
                if(!launched) {
                    flight_timer.start();
                    launched = true;
                    burning = true;
                }
                if(flight_timer.read() > BURN_TIME) {
                    if(burning) {
                        burning = false;
                        peak_detection_ticker.attach(&peak_detection, 1/10.0f);
                    }
                }
                if(!burning && (apogee || (flight_timer.read() > T_APOGEE))) phase = SEP1;
                break;
            case SEP1:
                pc.printf("SEP1\r\n");
                peak_detection_ticker.detach();
                sep1 = 1;
                sep2 = 0;
                if(!first_separated){
                    sep_timer.start();
                    first_separated = true;
                }
                //現状では加熱時間が経過したと同時に二段目が動作
                if(sep_timer.read() > T_HEATING){
                    sep1 = 0;
                    sep_timer.stop();
                    phase = SEP2;
                }
                break;
            case SEP2:
                pc.printf("SEP2\r\n");
                sep1 = 0;
                sep2 = 1;
                if(!second_separated){
                    sep2_timer.start();
                    second_separated = true;
                }
                if(sep2_timer.read() > T_HEATING){
                    sep2 = 0;
                    sep2_timer.stop();
                    phase = RECOVERY;
                }
                break;
            case RECOVERY:
                pc.printf("RECOVERY\r\n");
                if(!landed){
                    if((press_LPF > ground_press) || (flight_timer.read() > T_RELAY_OFF)){
                        relay = 0;
                        landed = true;
                        flight_timer.stop();
                    }
                }
                break;
            case EMERGENCY:
                pc.printf("EMERGENCY...\r\n");
                mission_timer.stop();
                relay = 1;
                sep1 = 0;
                sep2 = 0;
                break;
        }
    }
}

void init(){

}