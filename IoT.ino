#include <ArduinoJson.h>
#include<string.h>
#include <WiFi.h>
#include <HTTPClient.h>

const char* ssid = "aaa";
const char* password =  "1234567890";

String result = "";
float logOfNumber = 0;
const int sampleWindow = 50; // 샘플링 시간(50 mS = 20Hz)
unsigned int sample;
int redpin = 17;          // redpin 핀 번호
int greenpin = 26;
int bluepin = 16;
int sensor;               // sensor의 값이 0~1023 사이에서 나옵니다.
int non = 0; // number of Noise 약어 non/ 소음 발생 횟수값 선언
int intz = 0; // initialization 약어 intz/ 초기화 할 기준값 선언
int re = 0;
void setup() {



  Serial.begin(115200);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println("Connecting to WiFi..");
  }

  Serial.println("Connected to the WiFi network");
  pinMode(redpin, OUTPUT);
  pinMode(greenpin, OUTPUT);
  pinMode(bluepin, OUTPUT);

}


void loop() {
    digitalWrite(bluepin, HIGH);

  
  String url = "?sound=" + String(logOfNumber) + "&non=" + int(sensor);

  if ((WiFi.status() == WL_CONNECTED)) { //Check the current connection status

    HTTPClient http;

    http.begin("http://192.168.137.1:8090/1st_Project_Oh_Sound/max9814" + url); //Specify the URL
    int httpCode = http.GET();                                        //Make the request

    if (httpCode > 0) { //Check for the returning code
      Serial.println(url);
      Serial.println(httpCode);
      result = http.getString();
      Serial.println(result);
  

      unsigned long startMillis = millis(); // 샘플링 시작
      unsigned int peakToPeak = 0;   // 음성신호의 진폭
      unsigned int signalMax = 0;    // 최대크기 초기값 0
      unsigned int signalMin = 1024; // 최소크기 초기값 1024

      int displayPeak = 0;
      
      
      
      while (millis() - startMillis < sampleWindow) // 50ms 마다 데이터 수집
      {
        sample = analogRead(39);  // 마이크 증폭모듈로부터 받아오는 아날로그 값 저장
        if (sample < 1024)  // 값이 1024 보다 작을때
        {
          if (sample > signalMax) // 0보다 크면
          {
            signalMax = sample;  // signalMax에 저장
          }
          else if (sample < signalMin) // 1024보다 작으면
          {
            signalMin = sample;  // signalMin에 저장
          }
        }
      }

      peakToPeak = signalMax - signalMin;  // 최대- 최소 = 진폭값
      double sound = (peakToPeak * 5.0) / 9;  // // 전압 단위로 변환 = 소리 크기로 변환
      logOfNumber = 28 * log10(sound) / 1.5;

      if (logOfNumber < 45) {
        re = 0;
        //digitalWrite(redpin, LOW);
        //digitalWrite(greenpin, HIGH);

      } else if (logOfNumber >= 45) {
        re++;
        // if (re > 3) {
        //digitalWrite(greenpin, LOW);
        //digitalWrite(redpin, HIGH);
        //}
      }
      Serial.print(" peakToPeak : ");
      Serial.print(peakToPeak);
      Serial.print("\t\t sound  : ");
      Serial.print(logOfNumber);
      Serial.print("dB");
      Serial.println();
      delay(500);


      sensor = analogRead(34);
      if (sensor < 4095) {
        Serial.print("Sensor Value: "); // 시리얼 모니터에 값을 보여줍니다.
        Serial.println(sensor);
        non = non + 1;
        // tft.setTextSize(8);
        //tft.println("Vibration");
        //tft.setTextSize(10);
        // tft.print(non);
        //tft.println("sec");

        if (non > 60) {
          intz = 0;
        }
      }
      else {
        Serial.print("Sensor Value: ");
        Serial.println(sensor);
        intz = intz + 1;
        //tft.setTextSize(15);
        //tft.println("No");
        //tft.setTextSize(8);
        // tft.println("Vibration");

        if (intz > 180) {
          non = 0;
        }
      }
      Serial.print("non Value: ");
      Serial.println(non);
      Serial.print("intz value : ");
      Serial.println(intz);
      if (non > 60) {
        
      } else if (non < 60) {
       
      }

      //tft.fillScreen(BLACK);
      unsigned long start = micros();
      //tft.setCursor(0, 0);
      
      
      if (sensor < 4095 || re > 3) {
        digitalWrite(greenpin, LOW);
        digitalWrite(redpin, HIGH);
      } else {
        digitalWrite(redpin, LOW);
        digitalWrite(greenpin, HIGH);
      }


      //tft.setTextColor(WHITE);
      delay(1000);













      DynamicJsonBuffer jsonBuffer;
      JsonObject& root = jsonBuffer.parseObject(result);
      String result_bw = root["led"];
      int led1 = result_bw.toInt();
      int led2 = led1;





    }
    else {
      Serial.println("Error on HTTP request");
    }

    http.end(); //Free the resources
  }
  delay(1000);
}
