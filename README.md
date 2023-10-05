# Project Optimasi Pemeliharaan Burung Murai Batu Berbasis IoT
Dalam konteks optimalisasi dalam pemeliharaan burung murai batu, IoT dapat digunakan untuk memantau kondisi lingkungan sangkar burung, mengatur sistem pemberian dan penambahan pakan voer, mengirim notifikasi untuk jadwal pakan voer, pengisian air minum secara otomatis serta pengaturan suhu dan kelembaban yang optimal untuk meningkatkan performa burung.

Dengan menggunakan teknologi IoT dalam pemeliharaan burung murai batu, masyarakat dapat mengoptimalkan pengelolaan sangkar dan praktik pemeliharaan burung untuk menghasilkan burung murai yang lebih sehat dan berkualitas.

## Komponen Yang Digunakan
**Mikrokontroler :**
* ESP32

**Sensor :**
* DHT22
* Water Level
* Load Cell

**Input :**
* Push Button

**Output :**
* Relay 5V
* Lampu Pijar
* Pompa Air 12V
* Motor Servo
* Buzzer

## Skematik Rangkaian Komponen
![Logo](https://github.com/raffx29/Project-Optimasi-Pemeliharaan-Burung-Murai-Batu-Berbasis-IoT/blob/e354a753c8d52312e71a0a91e513582f81a7b5a5/LIB/Skematik.jpg)

Pin yang digunakan komponen sensor dan output pada ESP32 yaitu :
| Komponen | Pin GPIO    |
| :-------- | :------- |
| `DHT22` | `32` |
| `Water Level` | `35` |
| `Load Cell` | `18` `19` |
| `Relay 1` | `5` |
| `Relay 2` | `17` |
| `Relay 3` | `16` |
| `Push Button 1` | `33` |
| `Push Button 2` | `25` |
| `Push Button 3` | `26` |
| `Push Button 4` | `27` |
| `Buzzer` | `14` |

## Program ESP32
```c++
/*Comment this out to disable prints and save space*/
  #define BLYNK_PRINT Serial

/*Fill in information from Blynk Device Info here*/
  #define BLYNK_TEMPLATE_ID "TMPL6UZ6TU4yi"
  #define BLYNK_TEMPLATE_NAME "Controll Kandang"
  #define BLYNK_AUTH_TOKEN "vD1EYpOBTKm88imTifn4pTJ3wzkQS44X"

/*Library*/
  #include <WiFi.h>
  #include <BlynkSimpleEsp32.h>
  #include <DHT.h>
  #include <ESP32Servo.h>
  #include "HX711.h"
  #include <Wire.h>
  #include <LiquidCrystal_I2C.h>

/*Your WiFi credentials*/
  char ssid[] = "Your SSID";
  char pass[] = "Your Password";

/*PIN Sensor*/
  #define DHTPIN 32
  #define DHTTYPE DHT22
  #define WLPIN 35

/*PIN Relay*/
  #define RelayPin1 5
  #define RelayPin2 17
  #define RelayPin3 16

  int relay1_state = 0;
  int relay2_state = 0;
  int relay3_state = 0;

/*Push Button*/
  #define buttonPin1 33
  #define buttonPin2 25
  #define buttonPin3 26
  #define buttonPin4 27

/*Buzzer*/
  const int buzzer = 14;

/*DHT22*/
  int temp, hum;

/*Load Cell Variable*/
  long reading;
  const int LOADCELL_DOUT_PIN = 19;
  const int LOADCELL_SCK_PIN = 18;

/*Water Level Variable*/
  int nilaiSensor = 0;
  int tinggiAir = 0;

/*Servo*/
  int pinValue;
  int pos;

/*Class*/
  LiquidCrystal_I2C lcd(0x27, 16, 2);
  DHT dht(DHTPIN, DHTTYPE);
  BlynkTimer timer;
  HX711 scale;
  Servo myservo;

/*Milis*/
  unsigned long hitungan_milis;
  unsigned long milis_sekarang;
  const unsigned long nilai = 600;

/*Blynk Control*/
  BLYNK_CONNECTED(){
    Blynk.syncAll();
  }

  BLYNK_WRITE(V4){
    relay1_state = param.asInt();
    digitalWrite(RelayPin1, relay1_state);
  }

  BLYNK_WRITE(V5){
    relay2_state = param.asInt();
    digitalWrite(RelayPin2, relay2_state);
  }

  BLYNK_WRITE(V6){
    relay3_state = param.asInt();
    digitalWrite(RelayPin3, relay3_state);
  }

  BLYNK_WRITE(V7){
    pinValue = param.asInt();
    if (pinValue == 1){
      myservo.write(90);
    }
    else{
      myservo.write(0);
    }
  }

void setup(){
/*Millis*/
  hitungan_milis = 0;

/*Buzzer*/
  pinMode(buzzer, OUTPUT);

/*Push Button*/
  pinMode(buttonPin1, INPUT_PULLUP);
  pinMode(buttonPin2, INPUT_PULLUP);
  pinMode(buttonPin3, INPUT_PULLUP);
  pinMode(buttonPin4, INPUT_PULLUP);  

/*Relay*/
  pinMode(RelayPin1, OUTPUT);
  pinMode(RelayPin2, OUTPUT);
  pinMode(RelayPin3, OUTPUT);

  digitalWrite(RelayPin1, HIGH);
  digitalWrite(RelayPin2, HIGH);
  digitalWrite(RelayPin3, HIGH);

  Blynk.virtualWrite(V4, relay1_state);
  Blynk.virtualWrite(V5, relay2_state);
  Blynk.virtualWrite(V6, relay3_state);

  Serial.begin(9600);

/*Mulai WiFi on LCD*/
  lcd.begin(16,2);
  lcd.init();                 
  lcd.backlight();
  
  lcd.setCursor(0, 0);
  lcd.print("Connecting to :");
  lcd.setCursor(0, 1);
  lcd.print(ssid);

  WiFi.begin(ssid, pass);
  delay(2000);
  lcd.clear();
  
  for (int a = 0; a <= 15; a++){
    lcd.setCursor(a, 1);
    lcd.print(".");
    delay(300);
  }
  if (WiFi.status() == WL_CONNECTED){
    lcd.clear();
    lcd.setCursor(1, 0);
    lcd.print("WiFi connected");
    delay(2000);

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("IP address :");
    lcd.setCursor(0, 1);
    lcd.print(WiFi.localIP());
    delay(2000);
  }
  else {
    tone(buzzer, 1000);
    lcd.clear();
    lcd.setCursor(1, 0);
    lcd.print("Failed connect");
    lcd.setCursor(1, 1);
    lcd.print("Do Manual Mode");
    delay(2000);
    WiFi.disconnect();
  }
  lcd.clear();
  lcd.setCursor(1, 0);
  lcd.print("System Loading");
  for (int a = 0; a <= 15; a++){
    lcd.setCursor(a, 1);
    lcd.print(".");
    delay(300);
  }
  lcd.clear();

/*Blynk*/
  Blynk.config(BLYNK_AUTH_TOKEN);
  timer.setInterval(1000L, sendSensor);

/*Servo*/
  myservo.attach(23);

/*DHT22*/
  dht.begin();

/*LoadCell*/
  scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);
  scale.set_scale(395.8);
  scale.tare();

  noTone(buzzer);
}

void loop(){
  lcdRead();
  ListenPushButtons();
  error();

/*Blynk Run*/
  Blynk.run();
  timer.run();
}

void sendSensor(){
/*DHT22*/
  hum = dht.readHumidity();
  temp = dht.readTemperature();
  Blynk.virtualWrite(V0, temp);
  Blynk.virtualWrite(V1, hum);

/*Water Level*/
  nilaiSensor = analogRead(WLPIN);
  tinggiAir = map(nilaiSensor, 0, 3300, 0, 100);
  Blynk.virtualWrite(V2, tinggiAir);

/*Load Cell*/
  reading = scale.get_units();
  Blynk.virtualWrite(V3, reading);
}

void lcdRead(){
  milis_sekarang = millis();
/*ReadSensor*/
  lcd.setCursor(0,0);
  lcd.print("Temp : ");
  lcd.print(temp);
  lcd.print(" C ");
  lcd.print("Water : ");
  lcd.print(tinggiAir);
  lcd.print(" %");
  lcd.setCursor(0,1);
  lcd.print("Hum : ");
  lcd.print(hum);
  lcd.print(" %  ");
  lcd.print("Voer : ");
  lcd.print(reading);
  lcd.print(" g");

  if (milis_sekarang - hitungan_milis >= nilai){
    lcd.scrollDisplayLeft();
    hitungan_milis = milis_sekarang;
  }
}

void ListenPushButtons(){
  if(digitalRead(buttonPin1) == LOW){
    delay(200);
    control_relay(1);
    Blynk.virtualWrite(V4, relay1_state);
  }

  if (digitalRead(buttonPin2) == LOW){
    delay(200);
    control_relay(2);
    Blynk.virtualWrite(V5, relay2_state);
  }

  if (digitalRead(buttonPin3) == LOW){
    delay(200);
    control_relay(3);
    Blynk.virtualWrite(V6, relay3_state);
  }

  if (digitalRead(buttonPin4) == HIGH){
    delay(200);
    lcd.clear();
    lcd.print("Menambahkan Voer");
    for (pos = 0; pos <= 90; pos += 1){ 
    myservo.write(pos);
    delay(15);
    }
    for (pos = 90; pos >= 0; pos -= 1){
    myservo.write(pos);
    }
  }
}

void control_relay(int relay){
  if(relay == 1){
    relay1_state = !relay1_state;
    digitalWrite(RelayPin1, relay1_state);
    delay(50);
  }
  if(relay == 2){
    relay2_state = !relay2_state;
    digitalWrite(RelayPin2, relay2_state);
    delay(50);
  }
  if(relay == 3){
    relay3_state = !relay3_state;
    digitalWrite(RelayPin3, relay3_state);
    delay(50);
  }
}

void error(){
/*ErrorSensor*/
  if (isnan(hum) || isnan(temp)){
    tone(buzzer, HIGH);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Kerusakan Sensor");
    lcd.setCursor(4, 1);
    lcd.print("DHT22!");
    delay(1000);
  }
  else if (isnan(tinggiAir)){
    tone(buzzer, HIGH);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Kerusakan Sensor");
    lcd.setCursor(2, 1);
    lcd.print("Water Level!");
    delay(1000);
  }
  else if (isnan(reading)){
    tone(buzzer, HIGH);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Kerusakan Sensor");
    lcd.setCursor(3, 1);
    lcd.print("Load Cell!");
    delay(1000);
  }
}
```

Tipe data sensor yang digunakan yaitu :
| Komponen | Tipe     |
| :-------- | :------- |
| `DHT22` | `float` |
| `Water Level` | `int` |
| `Load Cell` | `int` |

## Program ESP32-CAM
```c++
#include "esp_camera.h"
#include <WiFi.h>
#include "esp_timer.h"
#include "img_converters.h"
#include "Arduino.h"
#include "fb_gfx.h"
#include "soc/soc.h" //disable brownout problems
#include "soc/rtc_cntl_reg.h"  //disable brownout problems
#include "esp_http_server.h"

//Replace with your network credentials
const char* ssid = "Your SSID";
const char* password = "Your Password";

#define PART_BOUNDARY "123456789000000000000987654321"

// This project was tested with the AI Thinker Model, M5STACK PSRAM Model and M5STACK WITHOUT PSRAM
#define CAMERA_MODEL_AI_THINKER

  #define PWDN_GPIO_NUM     32
  #define RESET_GPIO_NUM    -1
  #define XCLK_GPIO_NUM      0
  #define SIOD_GPIO_NUM     26
  #define SIOC_GPIO_NUM     27
  
  #define Y9_GPIO_NUM       35
  #define Y8_GPIO_NUM       34
  #define Y7_GPIO_NUM       39
  #define Y6_GPIO_NUM       36
  #define Y5_GPIO_NUM       21
  #define Y4_GPIO_NUM       19
  #define Y3_GPIO_NUM       18
  #define Y2_GPIO_NUM        5
  #define VSYNC_GPIO_NUM    25
  #define HREF_GPIO_NUM     23
  #define PCLK_GPIO_NUM     22

static const char* _STREAM_CONTENT_TYPE = "multipart/x-mixed-replace;boundary=" PART_BOUNDARY;
static const char* _STREAM_BOUNDARY = "\r\n--" PART_BOUNDARY "\r\n";
static const char* _STREAM_PART = "Content-Type: image/jpeg\r\nContent-Length: %u\r\n\r\n";

httpd_handle_t stream_httpd = NULL;

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
      if(fb->width > 400){
        if(fb->format != PIXFORMAT_JPEG){
          bool jpeg_converted = frame2jpg(fb, 80, &_jpg_buf, &_jpg_buf_len);
          esp_camera_fb_return(fb);
          fb = NULL;
          if(!jpeg_converted){
            Serial.println("JPEG compression failed");
            res = ESP_FAIL;
          }
        } else {
          _jpg_buf_len = fb->len;
          _jpg_buf = fb->buf;
        }
      }
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
    //Serial.printf("MJPG: %uB\n",(uint32_t)(_jpg_buf_len));
  }
  return res;
}

void startCameraServer(){
  httpd_config_t config = HTTPD_DEFAULT_CONFIG();
  config.server_port = 80;

  httpd_uri_t index_uri = {
    .uri       = "/",
    .method    = HTTP_GET,
    .handler   = stream_handler,
    .user_ctx  = NULL
  };
  
  //Serial.printf("Starting web server on port: '%d'\n", config.server_port);
  if (httpd_start(&stream_httpd, &config) == ESP_OK) {
    httpd_register_uri_handler(stream_httpd, &index_uri);
  }
}

void setup() {
  WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0); //disable brownout detector
 
  Serial.begin(115200);
  Serial.setDebugOutput(false);
  
  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = Y2_GPIO_NUM;
  config.pin_d1 = Y3_GPIO_NUM;
  config.pin_d2 = Y4_GPIO_NUM;
  config.pin_d3 = Y5_GPIO_NUM;
  config.pin_d4 = Y6_GPIO_NUM;
  config.pin_d5 = Y7_GPIO_NUM;
  config.pin_d6 = Y8_GPIO_NUM;
  config.pin_d7 = Y9_GPIO_NUM;
  config.pin_xclk = XCLK_GPIO_NUM;
  config.pin_pclk = PCLK_GPIO_NUM;
  config.pin_vsync = VSYNC_GPIO_NUM;
  config.pin_href = HREF_GPIO_NUM;
  config.pin_sscb_sda = SIOD_GPIO_NUM;
  config.pin_sscb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = 20000000;
  config.pixel_format = PIXFORMAT_JPEG; 
  
  if(psramFound()){
    config.frame_size = FRAMESIZE_UXGA;
    config.jpeg_quality = 10;
    config.fb_count = 2;
  } else {
    config.frame_size = FRAMESIZE_SVGA;
    config.jpeg_quality = 12;
    config.fb_count = 1;
  }
  
  // Camera init
  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("Camera init failed with error 0x%x", err);
    return;
  }
  // Wi-Fi connection
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  
  Serial.print("Camera Stream Ready! Go to: http://");
  Serial.print(WiFi.localIP());
  
  // Start streaming web server
  startCameraServer();
}

void loop() {
  delay(1);
}
```

## Tampilan Alat
![Logo](https://github.com/raffx29/Project-Optimasi-Pemeliharaan-Burung-Murai-Batu-Berbasis-IoT/blob/e354a753c8d52312e71a0a91e513582f81a7b5a5/LIB/IMG_0870.JPG)
![Logo](https://github.com/raffx29/Project-Optimasi-Pemeliharaan-Burung-Murai-Batu-Berbasis-IoT/blob/e354a753c8d52312e71a0a91e513582f81a7b5a5/LIB/IMG_0873.JPG)

## Interface Aplikasi
![Logo](https://github.com/raffx29/Project-Optimasi-Pemeliharaan-Burung-Murai-Batu-Berbasis-IoT/blob/e354a753c8d52312e71a0a91e513582f81a7b5a5/LIB/3.jpg)
