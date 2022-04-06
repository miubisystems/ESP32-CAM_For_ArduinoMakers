#include <WiFi.h> // Contains Declarations of Function Calls releated to ESP-IDF and Arduino HookUps for Wifi Acess 
const char* ssid = "TPLINK2020";// Access Point User Name
const char* password = "2020TPLINK";//Password of the AccessPoint


void StartServer();
const int FlashPin = 4;// GPIO 4 See Schematic Once

void initFlash() {
  /* Configuring motor PWM functionalities to generate the signal */
  /* ledcSetup(Channel, Frequency, Resolution) */
  ledcSetup(7, 2000, 8); /* 2000 hz PWM, 8-bit resolution and range from 0 to 255 */

  /* Attaching the channel to the GPIO to be controlled */
  /* ledcAttachPin(GPIO, Channel) */
  ledcAttachPin(FlashPin, 7);
}

void setup() {//First Function That would Run when any Arduino Based Board Starts
 
  Serial.begin(115200);//Set Baud Rate For Serial Communication
  Serial.setDebugOutput(true);// Let Debug Messages Appear
  Serial.println();//Just Prints a NewLine
  
  initFlash();

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

  StartServer();// Start the Server 

  Serial.print("Web Server Ready! Use 'http://");
  Serial.print(WiFi.localIP());// This will give you the IP Assigned to Device in General, here it should return the Static IP you have demanded
  Serial.println("' to connect");
  
}

void loop() {
  // put your main code here, to run repeatedly:

}
