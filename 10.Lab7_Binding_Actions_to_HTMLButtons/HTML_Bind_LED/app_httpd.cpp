#include "esp_http_server.h"
#include "Arduino.h"
httpd_handle_t server_httpd = NULL;

static esp_err_t cmd_handler(httpd_req_t * req)
{
  char*  buf;
  size_t buf_len;
  char variable[32] = {0,};
  char value[32] = {0,};

  buf_len = httpd_req_get_url_query_len(req)+1;
  Serial.print(buf_len);
  Serial.print("is Buffer Length");

  if(buf_len>1){
    buf=(char*)malloc(buf_len);
    if (!buf) {
      httpd_resp_send_500(req);
      return ESP_FAIL;
    }
  
  if (httpd_req_get_url_query_str(req, buf, buf_len) == ESP_OK) {// load the incoming request in to buffer
      if (httpd_query_key_value(buf, "var", variable, sizeof(variable)) == ESP_OK && // check for string var and load its value in to 'variable'
          httpd_query_key_value(buf, "val", value, sizeof(value)) == ESP_OK) {// check for string val and load its value in to 'value'
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
  }else {
    httpd_resp_send_404(req);
    return ESP_FAIL;
  }

  int val = atoi(value);
  Serial.print("Variable : ");
  Serial.print(variable);
  Serial.print(" With Value : ");
  Serial.print(val);

  if (!strcmp(variable, "flash"))
  {
    ledcWrite(7, val);//Write the PWM value directly, which is equivalent to controlling brightness
  }
  if (!strcmp(variable, "led"))
  {
    ledcWrite(7, val);
  }
  else
  {
    Serial.println("variable");
  }
  
  httpd_resp_set_hdr(req, "Access-Control-Allow-Origin", "*");
  return httpd_resp_send(req, NULL, 0);  
  
}

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
            .slider {
            appearance: none;
            width: 70%;
            height: 15px;
            border-radius: 10px;
            background: #FF0101;
            outline:none
            }
            .slider::-webkit-slider-thumb {
            appearance: none;
            appearance: none;
            width: 30px;
            height: 30px;
            border-radius: 50%;
            background: #FFFFFF;
            border-radius:3px; 
            border: 1px solid #000000;
            box-shadow: 1px 1px 1px #000000, 0px 0px 1px #0d0d0d;
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
    <label class="label">Flash</label>
    <input type="range" min="0" max="250" value="50" class="slider" id="myRange" onchange="try{fetch(document.location.origin+'/control?var=flash&val='+this.value);}catch(e){}">
    <output>255</output>
    <br/>
    </div>
    <div align=center> 
    <button class="button" id="on" onclick="fetch(document.location.origin+'/control?var=led&val=255');">ON</button>
    <button class="button" id="off" onclick="fetch(document.location.origin+'/control?var=led&val=0');">OFF</button>
    </div>
    </body>
</html>
)rawliteral";

static esp_err_t index_handler(httpd_req_t *req)
{
  httpd_resp_set_type(req,"text/html");
  return httpd_resp_send(req,(const char*)INDEX_HTML,strlen(INDEX_HTML));
}
void StartServer()
{
  httpd_config_t config = HTTPD_DEFAULT_CONFIG();

  httpd_uri_t index_uri = {// index handler details structure
        .uri       = "/",
        .method    = HTTP_GET,
        .handler   = index_handler,
        .user_ctx  = NULL
  };
  httpd_uri_t cmd_uri = { // cmd handler details tructure
        .uri       = "/control",
        .method    = HTTP_GET,
        .handler   = cmd_handler,
        .user_ctx  = NULL
    };
    Serial.printf("Starting web server on port: '%d'\n", config.server_port);
    if (httpd_start(&server_httpd, &config) == ESP_OK) {
        httpd_register_uri_handler(server_httpd, &index_uri);// Register index handling structure
        httpd_register_uri_handler(server_httpd, &cmd_uri);// Register cmd handling structure
    }
  
}
