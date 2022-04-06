#include "esp_http_server.h" // Contains Declarations of Function Calls releated to ESP-IDF Library for HTTP Server Creation
#include "esp_camera.h" // Contains Declarations of Function Calls releated to ESP-IDF Library for camera acess
#include "Arduino.h" // Contains Declarations of Function Calls releated to Serial Port 

#define PART_BOUNDARY "123456789000000000000987654321" // A Unique Header to sperate payloads , which are JPEG Images in our case
// String which would passed to HTML Browser for indicating the stream as multipart.
static const char* _STREAM_CONTENT_TYPE = "multipart/x-mixed-replace;boundary=" PART_BOUNDARY; 
// Unique Header discussed will replaced in place of macro
static const char* _STREAM_BOUNDARY = "\r\n--" PART_BOUNDARY "\r\n";
// Strig which would passed to HTML Browser for indicating the stream as image and format as JPEG
static const char* _STREAM_PART = "Content-Type: image/jpeg\r\nContent-Length: %u\r\n\r\n"; 
httpd_handle_t stream_httpd = NULL;


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

void startCameraServer(){
  httpd_config_t config = HTTPD_DEFAULT_CONFIG(); // Start with Configurating HTTP Server with default settings
	config.server_port = 80; // What is Special about Port 80, its the standard of HTTP where the browser interacts to server through this port
	httpd_uri_t stream_uri = {
        .uri       = "/",//Just Root, no sub folder or nodes
        .method    = HTTP_GET,// Request which come to server from client have the method as HTTP_GET, GET is used by clients to request data.
        .handler   = stream_handler,// Function that shall be executed for GET request from clients
        .user_ctx  = NULL
    };

  if (httpd_start(&stream_httpd, &config) == ESP_OK) {
       httpd_register_uri_handler(stream_httpd, &stream_uri);// Register the Hadler Created above in HTTP Server Library, so that request will be addressed by handler.
  }
}
