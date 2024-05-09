#include "mbed.h"
#include "controller.h"

#pragma region Serial Variables
// PC와의 통신을 위한 Serial 객체 생성
Serial pc(USBTX, USBRX, 115200);
// Raspberry Pi와의 통신 설정 (TX, RX, baud rate)
Serial device(D8, D2, 9600);
// 소수점 발견 여부를 추적하기 위한 변수
bool decimalPointSeen = false; 
// 부호 정보를 추적하기 위한 변수
bool isNegative = false; 
#pragma endregion Serial Variables
// main() runs in its own thread in the OS
int main()
{
    Controller controller;
 while (true) {
        if (device.readable()) { // device로부터 데이터를 받을 수 있는지 확인
            char ch = device.getc(); // 데이터 한 바이트 읽기
            if (ch == '-') {
                isNegative = true; // 부호가 음수인 경우 표시
            } else if (ch == '.') {
                decimalPointSeen = true; // 소수점 발견 시, 이후의 숫자는 무시
            } else if (ch >= '0' && ch <= '9' && !decimalPointSeen) {
                // 소수점 전의 숫자만 PC로 전송
                pc.putc(ch);
            } else if (ch == '/') {
                // '/' 문자를 만나면 새로운 줄로 이동하기 위해 줄바꿈 문자 출력
                if (isNegative) {
                    pc.printf("-"); // 부호 정보 출력
                }
                pc.printf("\r\n");
                decimalPointSeen = false; // 소수점 발견 여부 초기화
                isNegative = false; // 부호 정보 초기화
                continue; // '/' 문자는 무시하고 다음 데이터로 이동
            }
            // Raspberry Pi로 받은 데이터를 다시 보냄 (필요한 경우)
            device.putc(ch);
        }
        switch(controller.GetState())
        {
            case Controller::RoboState::START:
            controller.Start();
            break;
            case Controller::RoboState::IDLE:
            controller.Idle();
            break;
            case Controller::RoboState::DETECT:
            controller.Detect();
            break;
            case Controller::RoboState::ATTACK:
            controller.Attack();
            break;
            case Controller::RoboState::ESCAPE:
            controller.Escape();
            break;
        }
        controller.Move(controller.GetSpeedL(),controller.GetSpeedR());
    }
}
