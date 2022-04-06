#include <WiFi.h>

/* Wifi Crdentials */
const char* ssid = "TPLINK2020";
const char* password = "2020TPLINK";


void startServer();

int motor1Pina = 12;
int motor1Pinb = 13;
 
int motor2Pina = 14; 
int motor2Pinb = 15;

void setup() {
  
  // sets the pins as outputs:
  pinMode(motor1Pina, OUTPUT);
  pinMode(motor1Pinb, OUTPUT);
  pinMode(motor2Pina, OUTPUT);
  pinMode(motor2Pinb, OUTPUT);
  
  Serial.begin(115200);
  Serial.setDebugOutput(true);
  Serial.println();


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
