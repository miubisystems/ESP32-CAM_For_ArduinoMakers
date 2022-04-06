#include "esp_http_server.h" // Contains Declarations of Function Calls releated to ESP-IDF Library for HTTP Server Creation
#include "esp_camera.h" // Contains Declarations of Function Calls releated to ESP-IDF Library for camera acess
#include "Arduino.h" // Contains Declarations of Function Calls releated to Serial Port 
#include "esp_timer.h"

#define PART_BOUNDARY "123456789000000000000987654321" // A Unique Header to sperate payloads , which are JPEG Images in our case
// String which would passed to HTML Browser for indicating the stream as multipart.
static const char* _STREAM_CONTENT_TYPE = "multipart/x-mixed-replace;boundary=" PART_BOUNDARY; 
// Unique Header discussed will replaced in place of macro
static const char* _STREAM_BOUNDARY = "\r\n--" PART_BOUNDARY "\r\n";
// Strig which would passed to HTML Browser for indicating the stream as image and format as JPEG
static const char* _STREAM_PART = "Content-Type: image/jpeg\r\nContent-Length: %u\r\n\r\n"; 
httpd_handle_t stream_httpd = NULL;
httpd_handle_t server_httpd = NULL;

extern int motor1Pina ;
extern int motor1Pinb ;
extern int motor2Pina ; 
extern int motor2Pinb ; 
// Stream Handler Function mainly, takes images from camera and streams it over HTTP Protocol to the Client 
static esp_err_t stream_handler(httpd_req_t *req){
    camera_fb_t * fb = NULL; // Initialize the Frame Buffer
    esp_err_t res = ESP_OK; // Initialize the result variable
    size_t _jpg_buf_len = 0; // variable which shall hold the buffer length of jpeg compressed image
    uint8_t * _jpg_buf = NULL; // pointer to JPEG buffer
    char * part_buf[64]; // Temperory buffer for string creation 
    
    res = httpd_resp_set_type(req, _STREAM_CONTENT_TYPE); // Whe the browser opens the URL, it should know that the content which is going to come is a jpeg image steam.
    if(res != ESP_OK){
        return res;
    }

    while(true){
        fb = esp_camera_fb_get(); // Get the JPEG Frame from Image Sensor
        if (!fb) {
            Serial.println("Camera capture failed");
            res = ESP_FAIL;
        } else {
             _jpg_buf_len = fb->len;// Fill the Buffer length variable with size of compressed image
             _jpg_buf = fb->buf;// Pointer to the frame buffer is assigned to variable
        }  
        if(res == ESP_OK){
            size_t hlen = snprintf((char *)part_buf, 64, _STREAM_PART, _jpg_buf_len); // Create a string to communicate to client about length of buffer which is going to come
            res = httpd_resp_send_chunk(req, (const char *)part_buf, hlen);
        }
        if(res == ESP_OK){
            res = httpd_resp_send_chunk(req, (const char *)_jpg_buf, _jpg_buf_len);// Send the JPEG Image 
        }
        if(res == ESP_OK){
            res = httpd_resp_send_chunk(req, _STREAM_BOUNDARY, strlen(_STREAM_BOUNDARY));// Seperate each JPEG Image with a Unique Boundary string
        }
        if(fb){
            esp_camera_fb_return(fb);// Relinquish the Frame buffer memory allocation
            fb = NULL; // re-initialize the frame buffer variable
            _jpg_buf = NULL; // re initiazie the jpeg frame buffer variable
        } else if(_jpg_buf){
            free(_jpg_buf);// release the memory allotted to the buffer
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
    Serial.println("variable");
    res = -1;
    }
  }
  
  if (res) {
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
        <title>Flash LED Control</title>
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
    <br/>
    <div align=center> 
    <label class="label">Survillance Car</label>
    <br/>
    <div align=center> <img src='http://192.168.0.108:81/stream' style='width:300px; transform:rotate(180deg);'></div>
	<br/>
    <br/>
    <div align=center> 
    <button class="button" id="forward" ontouchstart="fetch(document.location.origin+'/control?var=ccoip&val=1');"ontouchend="fetch(document.location.origin+'/control?var=ccoip&val=3');" >Forward</button>
    </div>
    <br/>
    <div align=center> 
    <button class="button" id="turnleft" ontouchstart="fetch(document.location.origin+'/control?var=ccoip&val=2');"ontouchend="fetch(document.location.origin+'/control?var=ccoip&val=3');">Turn Left</button>
    <button class="button" id="stop" ontouchstart="fetch(document.location.origin+'/control?var=ccoip&val=3');">Stop</button>
    <button class="button" id="turnright" ontouchstart="fetch(document.location.origin+'/control?var=ccoip&val=4');"ontouchend="fetch(document.location.origin+'/control?var=ccoip&val=3');">Turn Right</button>
    </div>
    <br/>
     <div align=center> 
    <button class="button" id="backward" ontouchstart="fetch(document.location.origin+'/control?var=ccoip&val=5');"ontouchend="fetch(document.location.origin+'/control?var=ccoip&val=3');">Backward</button>
    </div>
    <br/>
    </body>
</html>
)rawliteral";

static esp_err_t index_handler(httpd_req_t *req){
    httpd_resp_set_type(req, "text/html");
    return httpd_resp_send(req, (const char *)INDEX_HTML, strlen(INDEX_HTML));
}


void startCameraServer(){
	httpd_config_t config = HTTPD_DEFAULT_CONFIG(); // Start with Configurating HTTP Server with default settings
	config.server_port = 80; // What is Special about Port 80, its the standard of HTTP where the browser interacts to server through this port
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
        .uri       = "/stream",//
        .method    = HTTP_GET,// Request which come to server from client have the method as HTTP_GET, GET is used by clients to request data.
        .handler   = stream_handler,// Function that shall be executed for GET request from clients
        .user_ctx  = NULL
    };
	Serial.printf("Starting web server on port: '%d'\n", config.server_port);
      if (httpd_start(&server_httpd, &config) == ESP_OK) {
          httpd_register_uri_handler(server_httpd, &index_uri);
          httpd_register_uri_handler(server_httpd, &cmd_uri);
      }
  config.server_port += 1;
  config.ctrl_port += 1;
  if (httpd_start(&stream_httpd, &config) == ESP_OK) {
       httpd_register_uri_handler(stream_httpd, &stream_uri);// Register the Hadler Created above in HTTP Server Library, so that request will be addressed by handler.
  }
}
