/* SLAVE 아두이노 */
//RFID 등록된 태그 ID로 LED 제어하기
#include <SPI.h>
#include <MFRC522.h>
//SS(Slave Selector)핀 및 reset 핀 번호 정의
#define SS_PIN 10
#define RST_PIN 9
//MFRC522 클래스로 rfid 객체 선언
MFRC522 rfid(SS_PIN, RST_PIN);
//등록할 태그 ID 배열
int tagId_on_card[4] = {19, 52, 88, 3};
int tagId_on_button[4] = {28, 247,182,22};
int LED = 8; //LED 연결 핀 번호
int on = 0; //아두이노1에 보낼 신호
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
int Vo = A0;
int V_LED = 2;
int same = 0;
unsigned long duration;
unsigned long starttime;
unsigned long sampletime_ms = 3000;
float ugm3 = 0;
float Vo_value = 0;
float Voltage = 0;
float dustDensity = 0;
LiquidCrystal_I2C lcd(0x27,16,2); // address, column, row

void setup() 
{
 //시리얼 통신 및 SPI 초기화
 Serial.begin(9600);
 SPI.begin();
 //MFRC522 초기화
 rfid.PCD_Init();
 //LED 핀 모드
 pinMode(LED, OUTPUT);
 
 pinMode(V_LED,OUTPUT);
 pinMode(Vo,INPUT);
 starttime = millis();
 lcd.init();
 lcd.backlight();
 lcd.clear();
 lcd.setCursor(0, 1);
 if ( ugm3 == 0) 
 {
 starttime = millis();
 }
}

void loop() 
{
 dust();
 
 //태그가 접촉되지 않았거나 ID가 읽혀지지 않았을 때
 if(!rfid.PICC_IsNewCardPresent() || !rfid.PICC_ReadCardSerial())
 {
 delay(300);
 return;
 }
 else
 rfid_set();
}

void rfid_set()
{
 //태그의 ID 출력하기(rfid.uid.uidByte[0] ~ rfid.uid.uidByte[3] 출력)
 Serial.print("Card Tag ID: ");
 
 for(byte i=0; i<4; i++)
 {
 Serial.print(rfid.uid.uidByte[i]);
 Serial.print(" ");
 //인식된 태그와 등록된 태그 번호 일치 여부
 if(rfid.uid.uidByte[i] == tagId_on_card[i] || rfid.uid.uidByte[i] 
 == tagId_on_button[i])
 same++; //모두 맞다면 same 변수는 4가 됨.
 
 else 
 same =0;
 }
 
 Serial.println();
 
 if(same == 4)
 {
 Serial.println("태그 ID가 일치합니다.");
 on = 1; 
 Serial.write(on); // MASTER 아두이노에 신호 전송
 digitalWrite(LED, HIGH);
 Serial.println(on); 
 }
 
 else 
 {
 on = 0;
 Serial.write(on);
 digitalWrite(LED, LOW);
 Serial.println(on);
 }
 delay(500);
 digitalWrite(LED, LOW);
}

// 미세먼지 측정 센서
void dust() 
{
 // 동시동작
 if ((millis()-starttime) >= sampletime_ms) 
 {
 // 미세먼지 측정
 delayMicroseconds(280); // 펄스폭을 유지하기 위해 필요한 코드
 Vo_value = analogRead(Vo); // 미세먼지 값 읽기
 delayMicroseconds(40);
 delayMicroseconds(9680);
 // 측정값(전압)을 미세먼지 농도(ug/m3)으로 변환
 Voltage = Vo_value * 5.0 / 1024.0;
 dustDensity = (Voltage - 0.3)/0.005;
 
 Serial.print(" c: ");
 Serial.println(Voltage);
 Serial.print(" Dust Density: ");
 Serial.println(dustDensity);
 
 delay(1000);
 ugm3 = dustDensity;
 
 // 미세먼지가 측정될 경우 다음 문장 출력
 if (ugm3 > 0.01 ) 
 {
 lcd.clear();
 lcd.print("Dust:");
 lcd.print(ugm3);
 lcd.print("ug/m3");
 starttime = millis();
 }
 // 미세먼지 값에 따라 다른 문장 출력
 if (ugm3 > 0.01 && ugm3 <= 50) 
 {
 lcd.setCursor(0, 1);
 lcd.print("Good!");
 }
 else if (ugm3 > 30 && ugm3 <= 100) 
 {
 lcd.setCursor(0, 1);
 lcd.print("SoSo!");
 }
 else if (ugm3 > 80 && ugm3 <= 150) 
 {
 lcd.setCursor(0, 1);
 lcd.print("Bad!");
 }
 else if (ugm3 > 150) 
 { 
 lcd.setCursor(0, 1);
 lcd.print("So bad!");
 }
 }
}
