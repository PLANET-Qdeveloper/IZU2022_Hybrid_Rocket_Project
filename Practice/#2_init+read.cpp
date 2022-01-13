/*******************************************************************************
 定数宣言（追加）
*******************************************************************************/
#define DEBUG_RATE      1     // printfデバッグの頻度[Hz]
#define READ_RATE       20    // データの取得スピード[Hz]
#define LOG_RATE        1     // データの保存スピード[Hz]
#define DOWNLINK_RATE   1     // ダウンリンクのレート[Hz]（ES920の使用時は1Hz以下にすること）

/*******************************************************************************
 変数宣言（追加）
*******************************************************************************/
//動作確認用のフラグ：1=有効, 0=無効
char f_sd;      // SDカード
char f_gps;     // GPS
char f_adxl;    // ADXL375
char f_ina_in;  // INA226（バッテリー）
char f_ina_ex;  // INA226（外部電源）
char f_lps;     // LPS22HB
char f_mpu;     // MPU9250

int mission_timer_reset;    // mission_timerをリセットした回数
float mission_time;
float flight_time;

float lat;          // 緯度[deg]
float lon;          // 経度[deg]
int sat;            // 捕捉衛星数
int fix;            // 位置特定品質(0だと位置特定できない)
float hdop;         // 水平精度低下率
float alt;          // 海抜高度[m]
float geoid;        // ジオイド[m]
float high_accel[3];// 3軸高加速度[G}
float voltage_in;   // バッテリー電圧[mV]
float current_in;   // バッテリー電流[mA]
float voltage_ex;   // 外部電源電圧[mV]
float current_ex;   // 外部電源電流[mA]
float accel[3];     // 3軸加速度[G]
float gyro[3];      // 3軸角速度[rad/s]
float press;        // 気圧[hPa]
float temp;         // 温度[C]


void init()
{
    mission_timer.start();

    relay = 1;

    wait(0.5);

    es.attach(&command_handler);    //ES920LRと接続開始
    downlink_ticker.attach(&downlink, 1.0f/DOWNLINK_RATE);
    record_ticker.attach(&record, 1.0f/LOG_RATE);

    ina_in.begin();
    ina_ex.begin();
    lps.begin();
    adxl.begin();
    mpu.begin();
}

void read()
{
    //タイマーがオーバーフローしないために30分おきにリセット
    if(mission_timer.read() >= 30*60) {
        mission_timer.reset();
        mission_timer_reset ++;
    }
    mission_time = mission_timer.read() + mission_timer_reset*30*60;
    flight_time  = flight_timer.read();

    f_sd = (bool)fp;
    f_gps = (bool)fix;
    lat   = gps.get_lat();
    lon   = gps.get_lon();
    sat   = gps.get_sat();
    fix   = gps.get_fix();
    hdop  = gps.get_hdop();
    alt   = gps.get_alt();
    geoid = gps.get_geoid();

    f_adxl = adxl.test();
    if(f_adxl) {
        adxl.read(high_accel);
    }

    f_ina_in = ina_in.test();
    if(f_ina_in) {
        ina_in.read_voltage(&voltage_in);
        ina_in.read_current(&current_in);
    }

    f_ina_ex = ina_ex.test();
    if(f_ina_ex) {
        ina_ex.read_voltage(&voltage_ex);
        ina_ex.read_current(&current_ex);
    }

    f_mpu = mpu.test();
    if(f_mpu) {
        mpu.read_accel(accel);
        mpu.read_gyro(gyro);
    }

    f_lps = lps.test();
    if(f_lps) {
        lps.read_press(&press);
        lps.read_temp(&temp);
    }
}