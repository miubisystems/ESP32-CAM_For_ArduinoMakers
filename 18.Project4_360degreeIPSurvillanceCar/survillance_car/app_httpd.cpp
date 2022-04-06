#include "dl_lib_matrix3d.h"
#include <esp32-hal-ledc.h>
#include "esp_http_server.h"
#include "esp_timer.h"
#include "esp_camera.h"
#include "img_converters.h"
#include "Arduino.h"
#include "fb_gfx.h"
#include "fd_forward.h"
#include "fr_forward.h"
#include <Servo.h>
  
#define STEP 15
#define PART_BOUNDARY "123456789000000000000987654321"

static const char* _STREAM_CONTENT_TYPE = "multipart/x-mixed-replace;boundary=" PART_BOUNDARY;
static const char* _STREAM_BOUNDARY = "\r\n--" PART_BOUNDARY "\r\n";
static const char* _STREAM_PART = "Content-Type: image/jpeg\r\nContent-Length: %u\r\n\r\n";
						

httpd_handle_t stream_httpd = NULL;
httpd_handle_t camera_httpd = NULL;

 int pos=90;
 
extern int motor1Pina ;
extern int motor1Pinb ;
 
extern int motor2Pina ; 
extern int motor2Pinb ; 

Servo myservo;  // create servo object to control a servo

int servoPin = 2;


static esp_err_t stream_handler(httpd_req_t *req){
    camera_fb_t * fb = NULL;
    esp_err_t res = ESP_OK;
    size_t _jpg_buf_len = 0;
    uint8_t * _jpg_buf = NULL;
    char * part_buf[64];
    res = httpd_resp_set_type(req, _STREAM_CONTENT_TYPE);
    if(res != ESP_OK){
       return res;
    }
    while(true){
        fb = esp_camera_fb_get();
        if (!fb) {
            Serial.println("Camera capture failed");
            res = ESP_FAIL;
        } else {
            _jpg_buf_len = fb->len;
            _jpg_buf = fb->buf;
        }	
        if(res == ESP_OK){
            size_t hlen = snprintf((char *)part_buf, 64, _STREAM_PART, _jpg_buf_len);
            res = httpd_resp_send_chunk(req, (const char *)part_buf, hlen);
        }
        if(res == ESP_OK){
            res = httpd_resp_send_chunk(req, (const char *)_jpg_buf, _jpg_buf_len);
        }
        if(res == ESP_OK){
            res = httpd_resp_send_chunk(req, _STREAM_BOUNDARY, strlen(_STREAM_BOUNDARY));
        }
        if(fb){
            esp_camera_fb_return(fb);
            fb = NULL;
            _jpg_buf = NULL;
        } else if(_jpg_buf){
            free(_jpg_buf);
            _jpg_buf = NULL;
        }
        if(res != ESP_OK){
            break;
        }
        
    }
    return res;
}

static esp_err_t cmd_handler(httpd_req_t *req)
{
  char*  buf;
  size_t buf_len;
  char variable[32] = {0,};
  char value[32] = {0,};
  int res = 0;

  buf_len = httpd_req_get_url_query_len(req) + 1;
  Serial.println();
  Serial.print(buf_len);
  Serial.println(" is Buffer Length");

  if (buf_len > 1) {
    buf = (char*)malloc(buf_len);
    if (!buf) {
      httpd_resp_send_500(req);
      return ESP_FAIL;
    }
    if (httpd_req_get_url_query_str(req, buf, buf_len) == ESP_OK) {
      if (httpd_query_key_value(buf, "var", variable, sizeof(variable)) == ESP_OK &&
          httpd_query_key_value(buf, "val", value, sizeof(value)) == ESP_OK) {
      } else {
        free(buf);
        httpd_resp_send_404(req);
        return ESP_FAIL;
      }
    } else {
      free(buf);
      httpd_resp_send_404(req);
      return ESP_FAIL;
    }
    free(buf);
  } else {
    httpd_resp_send_404(req);
    return ESP_FAIL;
  }

  int val = atoi(value);
  Serial.print("Variable : ");
  Serial.print(variable);
  Serial.print(" With Value : ");
  Serial.print(val);
  static int iLoop=0;
  if (!strcmp(variable, "ccoip"))
  {
    if (val == 1){//forward
      Serial.println("Moving Forward");
      digitalWrite(motor1Pina, LOW);
      digitalWrite(motor1Pinb, HIGH); 
      digitalWrite(motor2Pina, HIGH);
      digitalWrite(motor2Pinb, LOW);
      
    }
    else if(val ==2){//left
      Serial.println("Moving Left");
      digitalWrite(motor2Pina, HIGH);
      digitalWrite(motor2Pinb, LOW);
    }
    else if(val ==3){//stop
      Serial.println("Stop");
      digitalWrite(motor1Pina, LOW);
      digitalWrite(motor1Pinb, LOW); 
      digitalWrite(motor2Pina, LOW);
      digitalWrite(motor2Pinb, LOW);   
    }
    else if(val ==4){//right
      Serial.println("Moving Right");
      digitalWrite(motor1Pina, LOW);
      digitalWrite(motor1Pinb, HIGH);
    }
    else if(val ==5){//backward
      Serial.println("Moving Backward");
      digitalWrite(motor1Pina, HIGH);
      digitalWrite(motor1Pinb, LOW); 
      digitalWrite(motor2Pina, LOW);
      digitalWrite(motor2Pinb, HIGH);
    }
    else
    {
    Serial.println("Result -1");
    res = -1;
    }
  }

  if (!strcmp(variable, "pt"))
  {
    if (val == 1){//forward
      Serial.println("Moving Tilt - ");
      pos = pos + STEP;
      if (pos>=180)
        pos=180;
      myservo.write(pos);
      delay(50);
    }
    else if(val ==2){//Tilt +
      Serial.println("Moving Tilt + ");
      pos = pos - STEP;
      if (pos<=0)
        pos=0;
      myservo.write(pos);    
      delay(50);
    }
    else if(val ==3){//Home
      Serial.println("Going Home");
      pos = 90;
      myservo.write(pos);
      delay(250);    
    }
    else
    {
    Serial.println("variable");
    res = -1;
    }
  }
  if (res) {
    Serial.println("Result -1");
    return httpd_resp_send_500(req);
  }
  
  httpd_resp_set_hdr(req, "Access-Control-Allow-Origin", "*");
  return httpd_resp_send(req, NULL, 0);
}


/* Index page */
static const char PROGMEM INDEX_HTML[] = R"rawliteral(
<!doctype html>
<html>
    <head>
        <meta charset="utf-8">
        <meta name="viewport" content="width=device-width,initial-scale=1">
        <title>Survillance Car</title>
        <style>
            .button {
            background-color: #FF0101;
            border: none;
            border-radius: 4px;
            color: white;
            padding: 10px 25px;
            text-align: center;
            font-size: 16px;
            margin: 4px 2px;
            cursor: pointer;
            }
                       
            .label {
            color: #FF0101;
            font-size: 18px;
            }
        </style>
    </head>
    <body>
    <div align=center> <label class="label"> Survillance Car with Pan and Tilt Over IP</label></div>
    <br/>
    <br/>
    <div align=center> <img src='http://192.168.0.108:81/stream' style='width:300px; transform:rotate(90deg);'></div>
    <br/>
    <br/>
    <br/>
    <div align=center> 
    <button class="button" id="forward" ontouchstart="fetch(document.location.origin+'/control?var=ccoip&val=1');"ontouchend="fetch(document.location.origin+'/control?var=ccoip&val=3');" >Forward</button>
    </div>
    <div align=center> 
    <button class="button" id="turnleft" ontouchstart="fetch(document.location.origin+'/control?var=ccoip&val=2');"ontouchend="fetch(document.location.origin+'/control?var=ccoip&val=3');">Turn Left</button>
    <button class="button" id="stop" ontouchstart="fetch(document.location.origin+'/control?var=ccoip&val=3');">Stop</button>
    <button class="button" id="turnright" ontouchstart="fetch(document.location.origin+'/control?var=ccoip&val=4');"ontouchend="fetch(document.location.origin+'/control?var=ccoip&val=3');">Turn Right</button>
    </div>
    <div align=center> 
    <button class="button" id="backward" ontouchstart="fetch(document.location.origin+'/control?var=ccoip&val=5');"ontouchend="fetch(document.location.origin+'/control?var=ccoip&val=3');">Backward</button>
    </div>
    <div align=center> 
    <button class="button" id="servo_n" ontouchstart="fetch(document.location.origin+'/control?var=pt&val=1');">TILT - </button>
    <button class="button" id="home" ontouchstart="fetch(document.location.origin+'/control?var=pt&val=3');">HOME</button>
    <button class="button" id="servo_p" ontouchstart="fetch(document.location.origin+'/control?var=pt&val=2');">TILT + </button>
    </div>
    <br/>
    </body>
</html>
)rawliteral";

static esp_err_t index_handler(httpd_req_t *req){
    httpd_resp_set_type(req, "text/html");
    return httpd_resp_send(req, (const char *)INDEX_HTML, strlen(INDEX_HTML));
}

void startServer()
{

 myservo.attach(servoPin,2); // attaches the servo on pin to the servo object, for selected timer
  myservo.write(90);
  delay(150);             // waits 150ms for the servo to reach the position

 
  httpd_config_t config = HTTPD_DEFAULT_CONFIG();

  httpd_uri_t index_uri = {
      .uri       = "/",
      .method    = HTTP_GET,
      .handler   = index_handler,
      .user_ctx  = NULL
  };

  httpd_uri_t cmd_uri = {
      .uri       = "/control",
      .method    = HTTP_GET,
      .handler   = cmd_handler,
      .user_ctx  = NULL
  };
  
	httpd_uri_t stream_uri = {
        .uri       = "/stream",
        .method    = HTTP_GET,
        .handler   = stream_handler,
        .user_ctx  = NULL
    };
	Serial.printf("Starting web server on port: '%d'\n", config.server_port);
    if (httpd_start(&camera_httpd, &config) == ESP_OK) {
        httpd_register_uri_handler(camera_httpd, &index_uri);
        httpd_register_uri_handler(camera_httpd, &cmd_uri);
    }
	config.server_port += 1;
  config.ctrl_port += 1;
  Serial.printf("Starting stream server on port: '%d'\n", config.server_port);
  if (httpd_start(&stream_httpd, &config) == ESP_OK) {
       httpd_register_uri_handler(stream_httpd, &stream_uri);
  }
  
}
