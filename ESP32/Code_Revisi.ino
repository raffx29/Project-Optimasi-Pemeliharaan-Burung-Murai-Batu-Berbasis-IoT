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
  char ssid[] = "EVERYDAY 2.4G";
  char pass[] = "twentynine";

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
