/* MASTER */
#include <Keypad.h>
const byte ROWS = 4; // 4행
const byte COLS = 3; // 3열
char keys[ROWS][COLS] = { {'1','2','3'},
 {'4','5','6'},
 {'7','8','9'},
 {'*','0','#'} };
byte rowPins[ROWS] = {5, 4, 3, 2}; // 키패드의 행과 아두이노 보드 핀 연결
byte colPins[COLS] = {6, 7, 8}; // 키패드의 열과 아두이노 보드 핀 연결
Keypad keypad = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS ); 
volatile char key = '0';
#include <SoftwareSerial.h> // Mp3 시리얼 통신
SoftwareSerial mySerial (A4, A5); // A4, A5번 핀 사용
#include <DFPlayer_Mini_Mp3.h> // MP3 모듈 헤더
#include <Stepper.h> // 스텝 모터 헤더
const int stepsPerRevolution = 2048/6; // 360/6 = 60도
Stepper myStepper(stepsPerRevolution, 9, 11, 10, 12); // 9, 10, 11, 12번 핀 사용
int angle = 0; // 스텝모터 바퀴 수
int state = 0; // 현재 층수
int gogo = 0; // 이동하고자 하는 층수
void FLOORS(); // RFID로 사용자가 인식되었을 때 층수를 입력받는 함수
void RESET(); // * 버튼을 눌렀을 때 이전에 입력된 정보를 초기화하는 함수
void MOVE(); // 엘리베이터를 이동하는 함수
void NORFIDSIGNAL(); // RFID로 신호가 들어오지 않았을 때 층수를 입력받는 함수

void setup()
{ 
 Serial.begin(9600); // 시리얼 통신 시작
 pinMode(13, OUTPUT); // 진동 모터 입력 핀
 
 pinMode(A5, OUTPUT); // 스피커 센서 통신 핀에 A5 사용
 digitalWrite(A5, HIGH); // 아날로그 핀을 사용했기 때문에 HIGH 신호 입력
 pinMode(A4, OUTPUT);
 digitalWrite(A4, HIGH); 
 
 mp3_set_serial (Serial); // MP3 모듈과 스피커 시리얼 통신 시작
 delay(1);
 mp3_set_volume(15); // 스피커 볼륨
 myStepper.setSpeed(100); // 스텝 모터 회전 속도
} 

void loop()
{ 
 int received_data = Serial.read(); // 시리얼 통신을 이용하여 RFID 인식 정보를 받는다.
 
 Serial.println("안녕하세요");
 
 if(received_data > 0) // RFID 카드가 읽히면
 {
 Serial.println("몇 층으로 가시겠습니까?");
 
 char key = keypad.getKey(); // 키패드 입력받는 부분
 // 키패드를 통해 숫자를 입력받았을 경우
 if (key)
 { 
 Serial.println("이동하겠습니다.");
 Serial.print(key);
 Serial.print("층");
 Serial.println();
 if ( gogo == 0 )
 { 
 switch (key)
 {
 // 입력 받은 숫자에 따른 동작
 case '1':
 for ( int i = 0 ; i < 1 ; i++)
 {
 // 진동 모터 한 번 동작
 analogWrite(13, 200);
 delay(300);
 analogWrite(13, 0);
 delay(100); 
 // 101번 파일 재생
 mp3_play(101);
 delay(200);
 // 스텝 모터 작동을 위해 변수의 값 증가
 gogo++;
 angle++;
 }
 break;
 
 case '2':
 for ( int i = 0 ; i < 2 ; i++)
 {
 // 진동 두 번 동작
 analogWrite(13, 200);
 delay(300);
 analogWrite(13, 0);
 delay(100); 
 gogo++;
 angle++;
 }
 // 102번 파일 재생 
 mp3_play(102);
 delay(200);
 
 break;
 
 case '3':
 for ( int i = 0 ; i < 3 ; i++)
 {
 analogWrite(13, 200);
 delay(300);
 analogWrite(13, 0);
 delay(100); 
 
 gogo++;
 angle++;
 }
 
 mp3_play(103);
 delay(200); 
 break;
 
 case '4':
 for ( int i = 0 ; i < 4 ; i++)
 {
 analogWrite(13, 200);
 delay(300);
 analogWrite(13, 0);
 delay(100); 
 
 gogo++;
 angle++;
 }
 mp3_play(104);
 delay(200); 
 
 break;
 
 case '5':
 // 진동 수를 확실히 인식할 수 있도록 5층 이상의 층부터는 다섯 층을 긴 진동 한 번
으로 표현
 // 한 층은 작은 진동으로 표현
 analogWrite(13, 200);
 delay(600); // 진동의 길이 0.6초 -> 5층을 의미
 analogWrite(13, 0);
 delay(100);
 
 mp3_play(105);
 delay(200); 
 
 for ( int i = 0 ; i < 5 ; i++)
 { 
 gogo++;
 angle++;
 }
 break;
 
 case '6':
 // 0.6초 + 0.3초 = 두 번의 진동으로 6층을 의미하는 진동 메커니즘
 analogWrite(13, 200);
 delay(600); // 진동의 길이 0.6초 -> 5층을 의미
 analogWrite(13, 0);
 delay(100);
 analogWrite(13, 200);
 delay(300); // 진동의 길이 0.3초 -> 1층을 의미
 analogWrite(13, 0);
 delay(100); 
 
 mp3_play(106);
 delay(200); 
 for( int i = 0 ; i < 6 ; i++ )
 {
 gogo++;
 angle++;
 }
 break;
 
 case '7':
 analogWrite(13, 200);
 delay(600);
 analogWrite(13, 0);
 delay(100);
 
 for ( int i = 0 ; i < 2 ; i++)
 { 
 analogWrite(13, 200);
 delay(300);
 analogWrite(13, 0);
 delay(100); 
 }
 mp3_play(107);
 delay(200); 
 for( int i = 0 ; i < 7 ; i++ )
 {
 gogo++;
 angle++;
 }
 break;
 
 default:
 break;
 } // 스위치문 종료
 Serial.print(angle);
 Serial.println("층으로 이동합니다.");
 Serial.println();
 }
 // 키패드 입력 값 초기화
 key = '0';
 // 취소 버튼과 확인 버튼
 while(1)
 {
 char key = keypad.getKey();
 // * 버튼이 입력될 경우
 if(key == '*')
 {
 RESET(); // RESET 함수로 이동
 break;
 }
 
 // # 버튼이 입력될 경우
 else if ( key == '#')
 {
 MOVE(); // MOVE 함수로 이동
 break;
 }
 }
 }
}
 // RFID 카드가 인식되지 않았을 경우
 else if(received_data == -1)
 NORFIDSIGNAL();
}

// RFID 카드가 인식되지 않았을 경우
void NORFIDSIGNAL()
{
 // 입력 층수의 소리는 재생되지만 진동 모터는 작동하지 않도록 설계
 
 char key = keypad.getKey(); // 키패드 입력받는 부분
 if (key)
 { 
 Serial.print("입력받은 층수 : ");
 Serial.println(key);
 if ( gogo == 0 )
 { 
 switch (key)
 { 
 case '1':
 for ( int i = 0 ; i < 1 ; i++)
 {
 mp3_play(101);
 delay(200);
 gogo++;
 angle++;
 }
 break;
 
 case '2':
 for ( int i = 0 ; i < 2 ; i++)
 {
 mp3_play(102);
 delay(200);
 
 gogo++;
 angle++;
 }
 break;
 
 case '3':
 for ( int i = 0 ; i < 3 ; i++)
 {
 mp3_play(103);
 delay(200); 
 
 gogo++;
 angle++;
 }
 break;
 
 case '4':
 for ( int i = 0 ; i < 4 ; i++)
 {
 gogo++;
 angle++;
 }
 mp3_play(104);
 delay(200); 
 
 break;
 
 case '5':
 
 for ( int i = 0 ; i < 5 ; i++)
 { 
 gogo++;
 angle++;
 }
 mp3_play(105);
 delay(200); 
 
 break;
 
 case '6':
 mp3_play(106);
 delay(200); 
 for( int i = 0 ; i < 6 ; i++ )
 {
 gogo++;
 angle++;
 }
 break;
 
 case '7':
 mp3_play(107);
 delay(200); 
 for( int i = 0 ; i < 7 ; i++ )
 {
 gogo++;
 angle++;
 }
 break;
 
 default:
 break;
 } // 스위치문 종료
 }
 
 key = '0';
 while(1)
 {
 char key = keypad.getKey();
 if(key == '*')
 {
 RESET();
 break;
 }
 else if ( key == '#')
 {
 MOVE();
 break;
 }
 }
 }
}

// * 버튼을 눌렀을 경우 이전에 입력받은 키 값 초기화
void RESET()
{
 Serial.println("입력 초기화");
 gogo = 0;
 angle = 0;
}
// # 버튼을 눌렀을 경우 엘리베이터 작동

void MOVE()
{
 // 현재 층수가 가고 싶은 층보다 낮을 경우 (엘리베이터 상승)
 if ( gogo > state )
 {
 // 현재 층수에 따라 모터 바퀴 수 조절
 gogo -= state;
 // 입력 받은 층수만큼 모터 회전
 for (int x = 0; x < gogo ;x++)
 { 
 // 한 층에 3바퀴 회전
 for ( int k = 0 ; k < 3 ; k++)
 {
 myStepper.step(-stepsPerRevolution);
 delay(50); 
 }
 // 시리얼 모니터에 표시하기 위해 현재 층수 + 1을 더해줌
 state += 1;
 Serial.print("현재 층수 : ");
 Serial.println(state);
 }
 
 Serial.print(state);
 Serial.println("층에 도착했습니다.");
 }
 // 현재 층수가 가고 싶은 층보다 높을 경우 (엘리베이터 하강)
 else if ( gogo < state)
 {
 gogo -= state;
 for (int x = gogo ; x < 0 ; x++)
 {
 for ( int k = 0 ; k < 3 ; k++)
 { // 엘리베이터 상승 시 모터 방향의 역방향으로 회전
 myStepper.step(stepsPerRevolution);
 delay(50);
 }
 state -= 1;
 Serial.print("현재 층수 : ");
 Serial.println(state);
 } 
 Serial.print(state);
 Serial.println("층에 도착했습니다.");
 } 
 // 다음 엘리베이터 작동을 위해 입력 받은 층수 관련 정보 초기화
 gogo = 0;
}
