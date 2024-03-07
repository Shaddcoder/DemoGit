#define BLYNK_TEMPLATE_ID "TMPL6G4InBkIu"
#define BLYNK_TEMPLATE_NAME "ESP8266NewTest"
#define BLYNK_AUTH_TOKEN "Q5iW-Ar7xOPQPKj1n6oZQzg1RNDZq9UU"
#define BLYNK_FIRMWARE_VERSION "1.3.2"
#define BLYNK_PRINT Serial
#define BLYNK_DEBUG
#include <Servo.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp8266.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <DHT.h>
#define DHTPIN 12 // D6
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);
LiquidCrystal_I2C lcd(0x27,16,2);
BlynkTimer timer;
bool isNetworkConnected = false;
WidgetLED led(V6);  // Create a Blynk LCD widget on V10
// toke && wifi
char auth[] = "Q5iW-Ar7xOPQPKj1n6oZQzg1RNDZq9UU";
char ssid[] = "Shadd";
char pass[] = "Duc654321";

Servo servo; 
const int mqPin = A0; // analog pin for mp-
const int servoPin = D5;
#define pinLed1 5 // chan D0 led phong khách
#define pinLed2  3 // chan D1 led phòng ngủ 
#define pinFan1  4//   quat phong khach D2
#define pinFan  13 //  quạt nhà bếp D7
//#define pinBuzzer 1// s3
const int sensorPin = D8;  // Replace D2 with the actual digital pin you are using
const int ledLight = 16;   
int gasThreshold = 150;
void checkNetworkConnection() {
  if (WiFi.status() == WL_CONNECTED) {
    if (!isNetworkConnected) {
      isNetworkConnected = true;
      lcd.clear(); 
       lcd.setCursor(0, 0);  // Set the cursor to the first column of the first row
      lcd.print("Connected to");
      lcd.setCursor(0, 1);
      lcd.print("WiFi");
      led.on();
      delay(2000);
     // Blynk.virtualWrite(V10, "Online"); // Update a Blynk Value Display widget (V2) with "Online"
    }
  } else {
    if (isNetworkConnected) {
      isNetworkConnected = false;
      lcd.clear(); 
      lcd.setCursor(0, 0);  // Set the cursor to the first column of the first row
      lcd.print("WiFi Disconnected");
      led.off();
      delay(2000);
     //Blynk.virtualWrite(V10, "Offline"); // Update a Blynk Value Display widget (V2) with "Offline"
    }
  }
}
void setup() {
Serial.begin(115200);
timer.setInterval(10000L, checkNetworkConnection); // Check network connection every 10 seconds
dht.begin();
Blynk.begin(auth, ssid, pass);
//pin mode device
pinMode(pinLed1, OUTPUT);
pinMode(pinFan, OUTPUT);
pinMode(pinLed2, OUTPUT);
pinMode(pinFan1, OUTPUT);
//pinMode(pinBuzzer, OUTPUT);
pinMode(sensorPin, INPUT);  // Set the sensor pin as input
pinMode(ledLight, OUTPUT); 
servo.attach(servoPin);
//digitalWrite(pinBuzzer,LOW); 
WiFi.status();
Wire.begin(D4,D3); 
// thiết lập chân kết nối 
lcd.init();
lcd.backlight();
lcd.clear();
// Display a message on the LCD
lcd.setCursor(0, 0);  // Set the cursor to the first column of the first row
lcd.print("Luong Minh Duc");
lcd.setCursor(0, 1);  // Set the cursor to the first column of the second row
lcd.print("418h0397");
delay(2000);
}
void loop() 
{
Blynk.run();
timer.run();
delay(50); 
//sensor dht11
readTemp();
//sensor gas mq-9
readGas ();
//light sensor
lightSensor();
}
bool autoMode = false;
BLYNK_WRITE(V9) {
   autoMode = param.asInt();
   if (autoMode==1){
    Blynk.virtualWrite(V10,"Auto");
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Auto");
    delay(2000);
   }
   else{
    Blynk.virtualWrite(V10,"Normal");
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Normal");
    delay(2000);
   }
}
void readTemp(){
float humidity = dht.readHumidity();
float temperature = dht.readTemperature();
  if (isnan(humidity) || isnan(temperature)) {
    Serial.println("Failed to read from DHT sensor");
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Error reading data");
  }else {
    Blynk.virtualWrite(V4,temperature);
    Blynk.virtualWrite(V5,humidity);
    Serial.print("Humidity: ");
    Serial.print(humidity);
    Serial.print(" %\t");
    Serial.print("Temperature: ");
    Serial.print(temperature);
    Serial.println(" °C");
// lcd dht11 display
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Temp: ");
    lcd.print(temperature, 1);
    lcd.print(" C");
    lcd.setCursor(0, 1);
    lcd.print("Humidity: ");
    lcd.print(humidity, 1);
    lcd.print(" %");
    delay(2000);
  }
// modeauto
if (autoMode==1){
  if(temperature >=34){
    delay(50);
    digitalWrite(pinFan1, HIGH); 
  }
  else{
    digitalWrite(pinFan1, LOW); 
  } 
}
else {
digitalWrite(pinFan1, LOW); 
}
}
//
void readGas (){
int mq2_value = analogRead(mqPin);
Blynk.virtualWrite(V7,mq2_value);
Serial.print("MQ-9 Sensor Value: ");
Serial.println(mq2_value);
lcd.clear();
lcd.setCursor(0, 0);
lcd.print("Gas: ");
lcd.print(mq2_value);
delay(2000);
// auto
if (autoMode == 1) {
if (mq2_value > gasThreshold) {
  delay(50);
  //digitalWrite(pinBuzzer, HIGH); 
  Blynk.logEvent("gaswarning");
  digitalWrite(pinFan, HIGH);  
  // servo mo cua bep 
  servo.write(90);
  delay(1000);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Gas Warning");
  delay(10000); 
}
  else 
  {
    delay(50);
    servo.write(0);
    delay(1000);
   // digitalWrite(pinBuzzer, LOW); 
    digitalWrite(pinFan, LOW);
    Blynk.virtualWrite(V1,LOW);
  }
}
}
void lightSensor(){
  int sensorValue = digitalRead(sensorPin);
  if (sensorValue == HIGH) {
    digitalWrite(ledLight, HIGH);  // Turn on the LED
  } else {
    digitalWrite(ledLight, LOW);   // Turn off the LED
  }
  Serial.println(sensorValue);
  delay(100);
}
BLYNK_WRITE(V0) // hàm gọi khi wiget V0 thay đổi trạng thái
{
  int pinValue = param.asInt(); // gán giá trị của chân V0 cho biến pinValue
  if(pinValue == 1){
  digitalWrite(pinLed1, HIGH);
  lcd.clear(); 
  lcd.setCursor(0, 0);  // Set the cursor to the first column of the first row
  lcd.print("LED1 ON");
  Blynk.virtualWrite(V10,"LED1 ON");
  delay(2000);
      }
  else{

      digitalWrite(pinLed1, LOW);
    Blynk.virtualWrite(V10,"LED1 OFF");
    lcd.clear(); 
    lcd.setCursor(0, 0);  // Set the cursor to the first column of the first row
    lcd.print("LED1 OFF");
    delay(2000);
  }
}
  
BLYNK_WRITE(V1) // hàm gọi khi wiget V0 thay đổi trạng thái
{
  int pinValue = param.asInt(); // gán giá trị của chân V0 cho biến pinValue
  if(pinValue == 1){
  digitalWrite(pinFan, HIGH); 
  lcd.clear(); 
  lcd.setCursor(0, 0);  // Set the cursor to the first column of the first row
  lcd.print("FAN Kitchen ON");
  Blynk.virtualWrite(V10,"FAN Kitchen ON");
  delay(2000);
      }
  else{

    digitalWrite(pinFan, LOW);
    lcd.clear(); 
    lcd.setCursor(0, 0);  // Set the cursor to the first column of the first row
    lcd.print("FAN Kitchen OFF");
    Blynk.virtualWrite(V10,"FAN Kitchen OFF");
    delay(2000);

  }
}

BLYNK_WRITE(V2) // hàm gọi khi wiget V0 thay đổi trạng thái
{
  int pinValue = param.asInt(); // gán giá trị của chân V0 cho biến pinValue
  if(pinValue == 1){
    digitalWrite(pinLed2, HIGH); 
     lcd.clear(); 
    lcd.setCursor(0, 0);  // Set the cursor to the first column of the first row
    lcd.print("LED2 ON");
  Blynk.virtualWrite(V10,"LED2 ON");
  delay(2000);
      }
  else{

    digitalWrite(pinLed2, LOW);
     lcd.clear(); 
    lcd.setCursor(0, 0);  // Set the cursor to the first column of the first row
    lcd.print("LED2 OFF");
    Blynk.virtualWrite(V10,"LED2 OFF");
    delay(2000);

  }
}

BLYNK_WRITE(V3) // hàm gọi khi wiget thay đổi trạng thái
{
  int pinValue = param.asInt(); // gán giá trị của chân V0 cho biến pinValue
  if(pinValue == 1){
    digitalWrite(pinFan1, HIGH); 
    lcd.clear(); 
    lcd.setCursor(0, 0);  // Set the cursor to the first column of the first row
    lcd.print("FanLivingroomON");
    Blynk.virtualWrite(V10,"FanLivingroomON");
    delay(2000);
      }
  else{
    digitalWrite(pinFan1, LOW);
    lcd.clear(); 
    lcd.setCursor(0, 0);  // Set the cursor to the first column of the first row
    lcd.print("FanLivingroomOFF");
    Blynk.virtualWrite(V10,"FanLivingroomOFF");
    delay(2000);
  }
}


