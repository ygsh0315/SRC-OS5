#include "controller.h"
char buffer[8] = "";
#pragma region variables
DigitalOut DirL(PC_7);
DigitalOut DirR(PB_6);
PwmOut PwmL(PB_4);
PwmOut PwmR(PB_5);
#pragma endregion variables

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


Controller::Controller()
{
    SetState(RoboState::START);
};

Controller::RoboState Controller::GetState()
{
    return robo_state;
};

void Controller::SetState(RoboState state)
{
 robo_state = state;
};

float Controller::GetSpeedL()
{
    return speedL;
};

float Controller::GetSpeedR()
{
    return speedR;
};

void Controller::SetSpeed(float speed)
{
    speedL = speed;
    speedR = speed;
};
void Controller::SetSpeed(float sL,float sR)
{
    speedL = sL;
    speedR = sR;
};

bool Controller::GetEnemyState()
{
    return enemy;
};

void Controller::SetEnemyState(bool enemyState)
{
    enemy = enemyState;
};

float Controller::GetHD()
{
    return enemy_horizontal_distance;
};

void Controller::SetHD(float HD)
{
    enemy_horizontal_distance = HD;
}

void Controller::Start()
{
    PwmL.period_us(66);
    PwmR.period_us(66);
    SetState(RoboState::IDLE);
};

void Controller::Idle()
{
    if(isSafe)
    {
        SetState(RoboState::DETECT);
    }else
    {
        SetState(RoboState::ESCAPE);
    }
};

void Controller::Detect()
{
    if(GetEnemyState()==false)
        {
            SetSpeed(0.5,-0.5);
        }
    else
        {
            SetSpeed(0);
            SetState(RoboState::ATTACK); 
        }
};

void Controller::Attack()
{
    SetSpeed(MAXSPEED);
};

void Controller::Escape()
{
    SetSpeed(ESCAPESPEED);
};

void Controller::Move(float sL, float sR){
    if(sL<0) DirL = 0;
    else DirL = 1;

    if(sR<0) DirR = 0;
    else DirR = 1;

    PwmL = abs(sL);
    PwmR = abs(sR);
};

void Controller:: EnemyDetect()
{
if (device.readable() > 0) { // device로부터 데이터를 받을 수 있는지 확인
        char ch = device.getc(); // 데이터 한 바이트 읽기
        // if (ch == '-') {
        //     isNegative = true; // 부호가 음수인 경우 표시
        // } else if (ch == '.') {
        //     decimalPointSeen = true; // 소수점 발견 시, 이후의 숫자는 무시
        // } else if (ch >= '0' && ch <= '9' && !decimalPointSeen) {
        //     // 소수점 전의 숫자만 PC로 전송
        //     pc.putc(ch);
        // } else if (ch == '/') {
            
        //     // '/' 문자를 만나면 새로운 줄로 이동하기 위해 줄바꿈 문자 출력
        //     if (isNegative) {
        //         pc.printf("-"); // 부호 정보 출력
        //     }
        //     pc.printf("\r\n");
        //     decimalPointSeen = false; // 소수점 발견 여부 초기화
        //     isNegative = false; // 부호 정보 초기화
        //     //continue; // '/' 문자는 무시하고 다음 데이터로 이동
        // }
        if(ch=='*'){
        SetEnemyState(false);
        }else{
            SetEnemyState(true);
        }
        // Raspberry Pi로 받은 데이터를 다시 보냄 (필요한 경우)
        device.putc(ch);
        float hd = (float)(ch);
        SetHD(hd);
        
    }
}
// float lerp(float p1, float p2, float d1)
// {
// return (((1-d1) * p1) + (d1 * p2));
// }