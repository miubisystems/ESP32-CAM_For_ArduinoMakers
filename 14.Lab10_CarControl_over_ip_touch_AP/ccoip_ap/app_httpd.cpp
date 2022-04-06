#include <esp32-hal-ledc.h>

#include "esp_http_server.h"
#include "esp_timer.h"
#include "Arduino.h"

httpd_handle_t server_httpd = NULL;

extern int motor1Pina ;
extern int motor1Pinb ;
 
extern int motor2Pina ; 
extern int motor2Pinb ; 

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
    <label class="label">Car Control Over IP</label>
    <br/>
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

void startServer()
{
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

   Serial.printf("Starting web server on port: '%d'\n", config.server_port);
    if (httpd_start(&server_httpd, &config) == ESP_OK) {
        httpd_register_uri_handler(server_httpd, &index_uri);
        httpd_register_uri_handler(server_httpd, &cmd_uri);
    }
}
