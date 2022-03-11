//これ色々切り貼りしてるんでよろしく
#include <SPI.h>
#include <SD.h>
#include <Wire.h>               //I2C ライブラリ
#include <Adafruit_ADS1X15.h>

// 差圧センサ
Adafruit_ADS1115 ads;   // ADコンバータのコンストラクタを生成


// 変数の定義
//float   multiplier = 3.0F;    /* ADS1015 @ +/- 6.144V gain (12-bit results) */
float multiplier = 0.1875F; /* ADS1115  @ +/- 6.144V gain (16-bit results) */

const float ADC_mV = 4.8828125;       // convesion multiplier from Arduino ADC value to voltage in mV
const float SensorOffset = 200.0;     // in mV taken from datasheet
const float sensitivity = 4.413;      // in mV/mmH2O taken from datasheet
const float mmh2O_cmH2O = 10;         // divide by this figure to convert mmH2O to cmH2O
const float mmh2O_kpa = 0.00981;      // convesion multiplier from mmH2O to kPa
float first_colect;
float average_colect;
int16_t results;
float sensorValue;
float P;
float v;

//SD
const int chipSelect = 7; //ここを変える

//EEPROM
unsigned int startADR = 0x0000;
unsigned int endADR = 0x3FFFE;      // アドレス上限指定 (24LC64なら0x1FFF)→24LC1250なので0x1FFFFにしてみました(131071番地まで)２個はその２倍
unsigned int ADR;
byte data;

// 関数のプロトタイプ宣言
void i2cEEPROM_write(int i2cADR, unsigned int eeADR, byte data );

void setup() {
    //差圧センサ
    if (!ads.begin()) {
        Serial.println("Failed to initialize ADS.");
        while (1);
    }
    for(int i = 0; i < 10; i++){
        first_colect += ads.readADC_SingleEnded(0);
    }
    average_colect = first_colect/10; 


    // シリアルモニタの初期化
    Serial.begin(9600);
    while (!Serial);

    　//SD
    pinMode(chipSelect, OUTPUT);

    if (!SD.begin(chipSelect)) {
        Serial.println("SDカードを読み込むことができません…");
        return;
    }

    //I2C通信の初期化
    Wire.begin();


    /*
    Serial.println("EEPROM dump");

    for (ADR = startADR; ADR <= endADR; ADR++) {    // 全アドレス範囲をダンプ
        if ((ADR & 0x00FF) == 0x00) {                 // 256バイト毎に
            Serial.println();                          // 一行開ける
        }
        if ((ADR % 16) == 0) {                        // 行の先頭なら
            printADR(ADR);                              // アドレス表示
        }
        data = i2cEEPROM_read(0x50, ADR);             // 外付けEEPROMから読み出し
        Serial.print(data >> 4, HEX);                // 上位4ビットを16表示
        Serial.print(data & 0x0F, HEX);              // 下位4ビット
        Serial.print(" ");                           // データ間にスペース入れ
        if ((ADR % 16) == 15) {                       // 行末なら
            Serial.println();                          // 改行
        }
    }
    */
}

void loop() {
    //差圧センサ

    //results = ads.readADC_Differential_0_1();
    results = ads.readADC_SingleEnded(0) - average_colect;
    // ((sensorValue in mV -sensorOffset)/sensitivity in mV/mmH2O)*kPa conversion constant
    //float sensorValue = ((analogRead(A0) * ADC_mV - SensorOffset) / sensitivity * mmh2O_kpa);     // result in kPa
    sensorValue = results;

    Serial.print(millis());
    Serial.print(",");
    //Serial.println(sensorValue, 2);
    // using serial print format above, you can copy your data from the terminal,
    // paste it into NotePad app, than save as a .CSV file which can be opened in Excel

    P = (sensorValue/5-0.04)/0.09;
    P = abs(P);
    v = sqrt(2*P/1.29);
    v = v*0.4483-1.6141;

    Serial.println(v, 6);

    //SDに関するところ
    delay(100);
    
    // SDに保存したい文字列を""内に入力
    float dataString = v;//ここにvが入ってくる

    File dataFile = SD.open("datalog.txt", FILE_WRITE);

    // dataFileが存在したら書き込む
    if (dataFile) {
        dataFile.println(dataString);
        dataFile.close();
        //Serial.println(data);     //パソコン画面にも同じ内容を出力
    } else {
        Serial.println("datalog.txtを開くことができません");
    }

    // EEPROMに関するところ. 値を書き込みたい場合はコメントアウトを外す
    // vが10以上のときのみ書き込む
    while (10 <= v) {
        for (ADR = startADR; ADR <= endADR; ADR++) {  // 全アドレス範囲に順番に書き込む
            //差圧センサ
            int16_t results;

            results = ads.readADC_SingleEnded(0) - average_colect;
            float sensorValue = results;

            float P = (sensorValue/5-0.04)/0.09;
            P = abs(P);
            float v = sqrt(2*P/1.29);
            v = v*0.4483-1.6141;

            data = v;                           // アドレスを適当に細工してデーターを作る
            i2cEEPROM_write( 0x51, ADR, data);  // データーを書き込む（アドレス/256の値）
            if ((ADR & 0xFF) == 0) {
                //Serial.print('.');            // 書き込み進行中表示
                //dataFile.close();
                Serial.println(data);           // パソコン画面にも同じ内容を出力
                //Serial.println(dataString)
            }
        }
    }
}


//eeprom
void i2cEEPROM_write(int i2cADR, unsigned int eeADR, byte data ) {
    Wire.beginTransmission(i2cADR);                // i2cアドレス指定
    Wire.write((int)(eeADR >> 8));                 // EEPROM内アドレス指定 MSB
    Wire.write((int)(eeADR & 0xFF));               // LSB
    Wire.write(data);
    Wire.endTransmission();
    delay(5);                                      // 書き込み完了待ち(5 ms)
}


/* 
        byte i2cEEPROM_read( int i2cADR, unsigned int eeADR ) {
        Wire.beginTransmission(i2cADR);               // i2cアドレス指定
        Wire.write((int)(eeADR >> 8));                // EEPROM内アドレス指定 MSB
        Wire.write((int)(eeADR & 0xFF));              // LSB
        Wire.endTransmission();
        Wire.requestFrom(i2cADR, 1);                  // 1バイトデータリクエスト
        while (Wire.available()) {                    //
        data = Wire.read();                         // データ受信
        }
    return data;
    }

void printADR(unsigned int a) {         // アドレスを4桁表示
    String b = "";
    if (a < 0x1000) {
        b = "0";
    }
    if (a < 0x100) {
        b = "00";
    }
    if ( a < 0x10) {
        b = "000";
    }
    Serial.print(b);
    Serial.print(a, HEX);
    Serial.print(" ");
}
*/