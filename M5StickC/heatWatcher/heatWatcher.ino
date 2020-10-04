/************************************************************************

  M5StackFire I2C Scanner

  The M5StackFire has a connector for I2C devices. 
  This program scans the addresses 1-127 continuosly and shows 
  the devices found on the TFT.

  The M5Stack fire has two internal I2C devices at address 0x68 and 0x75.
  
  If they do not appear on the TFT it could mean you made a short cut on 
  the I2C bus.

  October 2018, ChrisMicro

************************************************************************/
#include <M5StickC.h>
#include "Seeed_MCP9600.h"

MCP9600 sensor;

//err_t sensor_basic_config() {
//    err_t ret = NO_ERROR;
//    CHECK_RESULT(ret, sensor.set_filt_coefficients(FILT_MID));
//    CHECK_RESULT(ret, sensor.set_cold_junc_resolution(COLD_JUNC_RESOLUTION_0_25));
//    CHECK_RESULT(ret, sensor.set_ADC_meas_resolution(ADC_14BIT_RESOLUTION));
//    CHECK_RESULT(ret, sensor.set_burst_mode_samp(BURST_32_SAMPLE));
//    CHECK_RESULT(ret, sensor.set_sensor_mode(NORMAL_OPERATION));
//    return ret;
//}
//
//err_t get_temperature(float* value) {
//    err_t ret = NO_ERROR;
//    float hot_junc = 0;
//    float junc_delta = 0;
//    float cold_junc = 0;
//    CHECK_RESULT(ret, sensor.read_hot_junc(&hot_junc));
//    CHECK_RESULT(ret, sensor.read_junc_temp_delta(&junc_delta));
//
//    CHECK_RESULT(ret, sensor.read_cold_junc(&cold_junc));
//
//    // SERIAL.print("hot junc=");
//    // SERIAL.println(hot_junc);
//    // SERIAL.print("junc_delta=");
//    // SERIAL.println(junc_delta);
//    // SERIAL.print("cold_junc=");
//    // SERIAL.println(cold_junc);
//
//    *value = hot_junc;
//
//    return ret;
//}

err_t sensor_INT_config() {
    err_t ret = NO_ERROR;
    CHECK_RESULT(ret, sensor.set_filt_coefficients(FILT_MID));

    for (int i = 0; i < 4; i++) {
        /*Conver temp num to 16bit data*/
        CHECK_RESULT(ret, sensor.set_alert_limit(i, sensor.covert_temp_to_reg_form(28 + i)));
        /*  Set hysteresis.for example,set hysteresis to 2℃,when the INT limitation is 30℃,interruption will be generated when
            the temp ecceed limitation,and the interruption flag will stay unless the temp below 30-2(limitation-hysteresis) 28℃. */
        CHECK_RESULT(ret, sensor.set_alert_hys(i, 2));

        /*Set when interruption generated the pin's status*/
        CHECK_RESULT(ret, sensor.set_alert_bit(i, ACTIVE_LOW));

        CHECK_RESULT(ret, sensor.clear_int_flag(i));

        /*default is comparator mode*/
        CHECK_RESULT(ret, sensor.set_alert_mode_bit(i, COMPARE_MODE));

        /*Set alert pin ENABLE.*/
        CHECK_RESULT(ret, sensor.set_alert_enable(i, ENABLE));
    }

    /*device cfg*/
    CHECK_RESULT(ret, sensor.set_cold_junc_resolution(COLD_JUNC_RESOLUTION_0_25));
    CHECK_RESULT(ret, sensor.set_ADC_meas_resolution(ADC_14BIT_RESOLUTION));
    CHECK_RESULT(ret, sensor.set_burst_mode_samp(BURST_32_SAMPLE));
    CHECK_RESULT(ret, sensor.set_sensor_mode(NORMAL_OPERATION));

    return NO_ERROR;
}


err_t get_temperature(float* value) {
    err_t ret = NO_ERROR;
    float hot_junc = 0;
    float junc_delta = 0;
    float cold_junc = 0;
    bool stat = true;

    CHECK_RESULT(ret, sensor.check_data_update(&stat));
    if (stat) {
        CHECK_RESULT(ret, sensor.read_hot_junc(&hot_junc));
        CHECK_RESULT(ret, sensor.read_junc_temp_delta(&junc_delta));

        CHECK_RESULT(ret, sensor.read_cold_junc(&cold_junc));

        *value = hot_junc;
    } else {
        M5.Lcd.println("data not ready!!");
    }

    return NO_ERROR;
}

void setup() 
{
  M5.begin();
  M5.Lcd.begin();
  M5.Lcd.setRotation(3);
  M5.Lcd.fillScreen( BLACK );
  M5.Lcd.setCursor(0, 0);
  M5.Lcd.setTextColor(YELLOW);  
  M5.Lcd.setTextSize(2);

  M5.Lcd.fillScreen( BLACK );
  M5.Lcd.setCursor(0, 0);
  M5.Lcd.println("M5StickC I2C Tester");
  
  if(sensor.init(THER_TYPE_K))
  {
      M5.Lcd.println("sensor init failed!!");
  }
  sensor_INT_config();  
  Wire.begin();

  delay(6000);
  M5.Lcd.fillScreen( BLACK );
}

void loop() 
{
  float temp=0;
  u8 byte=0;
  u8 stat=0;
  M5.Lcd.setCursor(0, 0);
  get_temperature(&temp);
  M5.Lcd.print("temperature ===================>");
  M5.Lcd.println(temp);
  sensor.read_INT_stat(&stat);
  if(sensor.init(THER_TYPE_K))
  {
      M5.Lcd.println("sensor init failed!!");
  }
  M5.Lcd.println(" ");
  M5.Lcd.println(" ");
  delay(1000);
  M5.Lcd.fillScreen( BLACK );
  M5.Lcd.setCursor(0, 0);
}
