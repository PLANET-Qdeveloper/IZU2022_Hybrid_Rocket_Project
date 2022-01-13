/*******************************************************************************
 記録
*******************************************************************************/
void record()
{
    // EEPROM
    if((phase >= FLIGHT) && !landed) {
        char data[128]; //書き込みページサイズは1バイト(0-127)なのでchar型.
        data[0] = ((char*)&mission_time)[0];
        data[1] = ((char*)&mission_time)[1];
        data[2] = ((char*)&mission_time)[2];
        data[3] = ((char*)&mission_time)[3];
        data[4] = ((char*)&flight_time)[0];
        data[5] = ((char*)&flight_time)[1];
        data[6] = ((char*)&flight_time)[2];
        data[7] = ((char*)&flight_time)[3];
        data[8] = phase;
        data[9] = 
        data[10] = apogee;
        data[11] = landed;
        data[12] = signal.read();
        data[13] = mission.read();
        data[14] = relay.read();
        data[15] = sep1.read();
        data[16] = sep2.read();
        data[17] = flight.read();
        data[18] = f_sd;
        data[19] = f_gps;
        data[20] = f_adxl;
        data[21] = f_ina_in;
        data[22] = f_ina_ex;
        data[23] = f_lps;
        data[24] = f_mpu;
        data[25] = ((char*)&lat)[0];    //GPS↓
        data[26] = ((char*)&lat)[1];
        data[27] = ((char*)&lat)[2];
        data[28] = ((char*)&lat)[3];
        data[29] = ((char*)&lon)[0];
        data[30] = ((char*)&lon)[1];
        data[31] = ((char*)&lon)[2];
        data[32] = ((char*)&lon)[3];
        data[33] = ((char*)&sat)[0];
        data[34] = ((char*)&sat)[1];
        data[35] = ((char*)&sat)[2];
        data[36] = ((char*)&sat)[3];
        data[37] = ((char*)&fix)[0];
        data[38] = ((char*)&fix)[1];
        data[39] = ((char*)&fix)[2];
        data[40] = ((char*)&fix)[3];
        data[41] = ((char*)&hdop)[0];
        data[42] = ((char*)&hdop)[1];
        data[43] = ((char*)&hdop)[2];
        data[44] = ((char*)&hdop)[3];
        data[45] = ((char*)&alt)[0];
        data[46] = ((char*)&alt)[1];
        data[47] = ((char*)&alt)[2];
        data[48] = ((char*)&alt)[3];
        data[49] = ((char*)&geoid)[0];
        data[50] = ((char*)&geoid)[1];
        data[51] = ((char*)&geoid)[2];
        data[52] = ((char*)&geoid)[3];
        data[53] = ((char*)&high_accel[0])[0];  //高加速度センサ↓
        data[54] = ((char*)&high_accel[0])[1];
        data[55] = ((char*)&high_accel[0])[2];
        data[56] = ((char*)&high_accel[0])[3];
        data[57] = ((char*)&high_accel[1])[0];
        data[58] = ((char*)&high_accel[1])[1];
        data[59] = ((char*)&high_accel[1])[2];
        data[60] = ((char*)&high_accel[1])[3];
        data[61] = ((char*)&high_accel[2])[0];
        data[62] = ((char*)&high_accel[2])[1];
        data[63] = ((char*)&high_accel[2])[2];
        data[64] = ((char*)&high_accel[2])[3];
        data[65] = ((char*)&voltage_in)[0];      //電圧電流センサ↓
        data[66] = ((char*)&voltage_in)[1];
        data[67] = ((char*)&voltage_in)[2];
        data[68] = ((char*)&voltage_in)[3];
        data[69] = ((char*)&current_in)[0];
        data[70] = ((char*)&current_in)[1];
        data[71] = ((char*)&current_in)[2];
        data[72] = ((char*)&current_in)[3];
        data[73] = ((char*)&voltage_ex)[0];
        data[74] = ((char*)&voltage_ex)[1];
        data[75] = ((char*)&voltage_ex)[2];
        data[76] = ((char*)&voltage_ex)[3];
        data[77] = ((char*)&current_ex)[0];
        data[78] = ((char*)&current_ex)[1];
        data[79] = ((char*)&current_ex)[2];
        data[80] = ((char*)&current_ex)[3];
        data[81] = ((char*)&press)[0];          //気圧センサ↓
        data[82] = ((char*)&press)[1];
        data[83] = ((char*)&press)[2];
        data[84] = ((char*)&press)[3];
        data[85] = ((char*)&temp)[0];
        data[86] = ((char*)&temp)[1];
        data[87] = ((char*)&temp)[2];
        data[88] = ((char*)&temp)[3];
        data[89] = ((char*)&accel[0])[0];       //9軸加速度センサ↓
        data[90] = ((char*)&accel[0])[1];
        data[91] = ((char*)&accel[0])[2];
        data[92] = ((char*)&accel[0])[3];
        data[93] = ((char*)&accel[1])[0];
        data[94] = ((char*)&accel[1])[1];
        data[95] = ((char*)&accel[1])[2];
        data[96] = ((char*)&accel[1])[3];
        data[97] = ((char*)&accel[2])[0];
        data[98] = ((char*)&accel[2])[1];
        data[99] = ((char*)&accel[2])[2];
        data[100] = ((char*)&accel[2])[3];
        data[101] = ((char*)&gyro[0])[0];
        data[102] = ((char*)&gyro[0])[1];
        data[103] = ((char*)&gyro[0])[2];
        data[104] = ((char*)&gyro[0])[3];
        data[105] = ((char*)&gyro[1])[0];
        data[106] = ((char*)&gyro[1])[1];
        data[107] = ((char*)&gyro[1])[2];
        data[108] = ((char*)&gyro[1])[3];
        data[109] = ((char*)&gyro[2])[0];
        data[110] = ((char*)&gyro[2])[1];
        data[111] = ((char*)&gyro[2])[2];
        data[112] = ((char*)&gyro[2])[3];
        data[113] = 0;
        data[114] = 0;
        data[115] = 0;
        data[116] = 0;
        data[117] = 0;
        data[118] = 0;
        data[119] = 0;
        data[120] = 0;
        data[121] = 0;
        data[122] = 0;
        data[123] = 0;
        data[124] = 0;
        data[125] = 0;
        data[126] = 0;
        data[127] = 0;

        eeprom.write(addr, data, 128);
        addr += 0x80;
    }
    
    if(fp) {
        char *phase_names[] =  {"SAFETY", "READY", "FLIGHT", "SEP", "REEFING", "EMERGENCY", "RECOVERY"};
        fprintf(fp, "%.3f,%.3f,%s,", mission_time, flight_time, phase_names[phase]);
        fprintf(fp, "%d,%d,%d,%d,%d,%d,%d,", relay.read(), mission_sig1.read(), sep1.read(), sep2.read(), apogee, reefed, landed);
        fprintf(fp, "%d,%d,%d,%d,%d,%d,%d,", f_sd, f_gps, f_adxl, f_ina_in, f_ina_ex, f_lps, f_mpu);
        fprintf(fp, "%.6f,%.6f,%d,%d,%f,%f,%f,", lat, lon, sat, fix, hdop, alt, geoid);
        fprintf(fp, "%f,%f,%f,", high_accel[0], high_accel[1], high_accel[2]);
        fprintf(fp, "%f,%f,", voltage_in, current_in);
        fprintf(fp, "%f,%f,", voltage_ex, current_ex);
        fprintf(fp, "%f,%f,", press, temp);
        fprintf(fp, "%f,%f,%f,", accel[0], accel[1], accel[2]);
        fprintf(fp, "%f,%f,%f", gyro[0], gyro[1], gyro[2]);
        fprintf(fp, "\r\n");
    }
    
    if(sd_timer.read() >= 60*30){  //30分ごとにfcloseする
        sd_timer.reset();
        if(fp){
            fclose(fp);
            fp = fopen(file_name, "a");
        }
    }
}
