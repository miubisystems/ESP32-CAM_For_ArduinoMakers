#define CAMERA_MODEL_AI_THINKER // Your Board Info

#include "esp_camera.h" // Contains Declarations of Function Calls releated to ESP-IDF Library for camera acess
#include "camera_pins.h" // Defines the GPIO Pin Numbers of ESP32 for which OV2640 Camera Module is interfaced
void startCameraServer(); // External Function present in Application  

#include <WiFi.h> // Contains Declarations of Function Calls releated to ESP-IDF and Arduino HookUps for Wifi Acess 
const char* ssid = "TPLINK2020";// Access Point User Name
const char* password = "2020TPLINK";//Password of the AccessPoint

 
void setup() { //First Function That would Run when any Arduino Based Board Starts
 
  Serial.begin(115200);//Set Baud Rate For Serial Communication
  Serial.setDebugOutput(true);// Let Debug Messages Appear
  Serial.println();//Just Prints a NewLine

  static camera_config_t config; 
  // Config Object is created to fill settings and Pin numbers. These Macros are part of camera_pins.h
  // In NutShell its the Object Created to Configure the Sensor
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
  config.pixel_format       = PIXFORMAT_JPEG;//Compression format from Sensor is selected as JPEG
  config.frame_size         = FRAMESIZE_SVGA;// Resolution SVGA 
  config.jpeg_quality       = 12;//lowest is better, lowest possible is 10
  config.fb_count           = 1;//Number of Frame Buffers at a time

  // camera init with required settings and pin numbers 
  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {//This Flag is Set Internally by Init Function in case of failure
    Serial.printf("Camera init failed with error 0x%x", err);//error number would tell us type of error
    return;
  }
  #if 1
  //Acquire Access to the sensor
  sensor_t * s = esp_camera_sensor_get();// Get Acess to tweak camera settings 
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
  #endif

  #if 1
  // Set your Static IP address of ESP32-CAM Node / Station.
  IPAddress STATIC_IP(192, 168, 0, 108);
  // Set your Gateway IP address
  IPAddress gway(192, 168, 0, 1);//Gateway Address, in our case the Gate way is router, this IP is common for any routers
  IPAddress sNet(255, 255, 0, 0);//Sub Net Mask, Default as we have no network divisions
  IPAddress pDNS(8, 8, 8, 8); //Primary DNS - This is Google's
  IPAddress sDNS(8, 8, 4, 4);//Secondary DNS - This is Google's
  
  if(!WiFi.config(STATIC_IP, gway, sNet, pDNS, sDNS)) {// Attempt for Configuration of Wifi Hardware with Settings
  Serial.println("Configuration with Static IP Failed");
  }
  #endif
  
  WiFi.begin(ssid, password);// Attempt for Wifi Connection Establishment with uname and password of Access Point

  while (WiFi.status() != WL_CONNECTED) {// Result of Wifi Connection Attempt
    delay(500);
    Serial.print(".");
  }
  
  Serial.println("");
  Serial.println("WiFi connected");

  startCameraServer();// Start the Camera Server / Streamer Expalined in Application File / Lecture.

  Serial.print("Camera Ready! Use 'http://");
  Serial.print(WiFi.localIP());// This will give you the IP Assigned to Device in General, here it should return the Static IP you have demanded
  Serial.println("' to connect");
}

void loop() {
  // put your main code here, to run repeatedly:
  delay(10000);
}
