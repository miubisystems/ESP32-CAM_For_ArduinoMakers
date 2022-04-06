#define CAMERA_MODEL_AI_THINKER

#include "esp_wifi.h"
#include "esp_camera.h"
#include <WiFi.h>
#include "camera_pins.h"
#include "soc/soc.h"             // disable brownout problems
#include "soc/rtc_cntl_reg.h"    // disable brownout problems
/* Wifi Crdentials */
const char* ssid = "TPLINK2020";
const char* password = "2020TPLINK";


void startServer();

int motor1Pina = 13;
int motor1Pinb = 12;
 
int motor2Pina = 15; 
int motor2Pinb = 14;

extern int servoPin;

void setup() {

  WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0); //disable brownout detector

  // sets the pins as outputs:
  pinMode(motor1Pina, OUTPUT);
  pinMode(motor1Pinb, OUTPUT);
  pinMode(motor2Pina, OUTPUT);
  pinMode(motor2Pinb, OUTPUT);
  pinMode(servoPin, OUTPUT);

  Serial.begin(115200);
  Serial.setDebugOutput(true);
  Serial.println();

static camera_config_t config;
  // Config Object is created to fill settings and Pin numbers. These Macros are part of camera_pins.h
  config.ledc_channel       = LEDC_CHANNEL_0;
  config.ledc_timer         = LEDC_TIMER_0;
  config.pin_d0             = Y2_GPIO_NUM;
  config.pin_d1             = Y3_GPIO_NUM;
  config.pin_d2             = Y4_GPIO_NUM;
  config.pin_d3             = Y5_GPIO_NUM;
  config.pin_d4             = Y6_GPIO_NUM;
  config.pin_d5             = Y7_GPIO_NUM;
  config.pin_d6             = Y8_GPIO_NUM;
  config.pin_d7             = Y9_GPIO_NUM;
  config.pin_xclk           = XCLK_GPIO_NUM;
  config.pin_pclk           = PCLK_GPIO_NUM;
  config.pin_vsync          = VSYNC_GPIO_NUM;
  config.pin_href           = HREF_GPIO_NUM;
  config.pin_sscb_sda       = SIOD_GPIO_NUM;
  config.pin_sscb_scl       = SIOC_GPIO_NUM;
  config.pin_pwdn           = PWDN_GPIO_NUM;
  config.pin_reset          = RESET_GPIO_NUM;
  config.xclk_freq_hz       = 20000000;//SPI Clock Frequency set to 20MHz
  config.pixel_format       = PIXFORMAT_JPEG;//Compression format from Sensor
  config.frame_size         = FRAMESIZE_VGA;
  config.jpeg_quality       = 10;
  config.fb_count           = 1;
  
  // camera init with required settings and pin numbers 
  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("Camera init failed with error 0x%x", err);
    return;
  }
//Acquire Access to the sensor
  sensor_t * s = esp_camera_sensor_get();
  s->set_brightness(s, 0);     // -2 to 2
  s->set_contrast(s, 0);       // -2 to 2
  s->set_saturation(s, 0);     // -2 to 2
  s->set_special_effect(s, 0); // 0 to 6 (0 - No Effect, 1 - Negative, 2 - Grayscale, 3 - Red Tint, 4 - Green Tint, 5 - Blue Tint, 6 - Sepia)
  s->set_whitebal(s, 1);       // 0 = disable , 1 = enable
  s->set_awb_gain(s, 1);       // 0 = disable , 1 = enable
  s->set_wb_mode(s, 0);        // 0 to 4 - if awb_gain enabled (0 - Auto, 1 - Sunny, 2 - Cloudy, 3 - Office, 4 - Home)
  s->set_exposure_ctrl(s, 1);  // 0 = disable , 1 = enable
  s->set_aec2(s, 1);           // 0 = disable , 1 = enable
  s->set_ae_level(s, 0);       // -2 to 2
  s->set_aec_value(s, 1200);    // 0 to 1200
  s->set_gain_ctrl(s, 1);      // 0 = disable , 1 = enable
  s->set_agc_gain(s, 30);       // 0 to 30
  s->set_gainceiling(s, (gainceiling_t)6);  // 0 to 6
  s->set_bpc(s, 1);            // 0 = disable , 1 = enable
  s->set_wpc(s, 1);            // 0 = disable , 1 = enable
  s->set_raw_gma(s, 1);        // 0 = disable , 1 = enable
  s->set_lenc(s, 1);           // 0 = disable , 1 = enable
  s->set_hmirror(s, 0);        // 0 = disable , 1 = enable
  s->set_vflip(s,0);          // 0 = disable , 1 = enable
  s->set_dcw(s, 1);            // 0 = disable , 1 = enable
  s->set_colorbar(s, 0);       // 0 = disable , 1 = enable


#if 1
  // Set your Static IP address
  IPAddress STATIC_IP(192, 168, 0, 108);
  // Set your Gateway IP address
  IPAddress gway(192, 168, 0, 1);//Gateway
  IPAddress sNet(255, 255, 0, 0);//Mask
  IPAddress pDNS(8, 8, 8, 8); //Primary DNS
  IPAddress sDNS(8, 8, 4, 4);//Secondary DNS 
  
  if(!WiFi.config(STATIC_IP, gway, sNet, pDNS, sDNS)) {
  Serial.println("Configuration with Static IP Failed");
  }
#endif
  /* Connecting to WiFi */
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");

  startServer();/* Starting camera server */

  Serial.print(WiFi.localIP());
  Serial.println("' to connect");

  
}

void loop() {
  // put your main code here, to run repeatedly:

}
