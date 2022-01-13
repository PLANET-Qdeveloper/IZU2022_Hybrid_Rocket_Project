/*******************************************************************************
 定数宣言（追加）
*******************************************************************************/
#define coef 0.01

/*******************************************************************************
 変数宣言（追加）
*******************************************************************************/
int press_count;    // 移動中央値計算用
float press_LPF;
float press_prev_LPF;
float press_buf[10];
float press_median;
float ground_press;     // 射点での気圧[hPa]
int count;  // 頂点検知回数


/*************************************************************
 * read()にローパスフィルター関連を追加で書き込む
 * **********************************************************/      
void read(){

    if(f_lps){
        // 移動中央値を求める
        press_buf[press_count] = press;
        press_count++;
        if(press_count > 10) {
            press_count = 0;
        }

        // 配列のコピー
        float buf[10];
        for(int i = 0; i < 10; i++) {
            buf[i] = press_buf[i];
        }

        // バブルソート
        for(int i = 0; i < 9; i++) {
            for(int j = 9; j > i; j--) {
                if(buf[j] < buf[j - 1]) {
                    float temp = buf[j];
                    buf[j] = buf[j - 1];
                    buf[j - 1] = temp;
                }
            }
        }
        // 中央値
        press_median = (buf[4] + buf[5]) / 2.0f;
        // ローパスフィルタ
        press_LPF = press_median * coef + press_prev_LPF * (1 - coef);
        press_prev_LPF = press_LPF;
    }   
}

/*******************************************************************************
 * 頂点検知プログラム
 * ****************************************************************************/
void peak_detection(){
    if(press_prev_LPF > press_LPF) count++;
    if(count == 5) apogee = true;
}
