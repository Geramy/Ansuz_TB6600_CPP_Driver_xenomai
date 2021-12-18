#include "TB6600Driver.h"
#include "wiringPi.h"
#include <chrono>
#include <thread>

TB6600Driver::TB6600Driver(int en, int direction, int pulse, int microSteps)
{
    this->EnablePin = en;
    this->PulsePin = pulse;
    this->DirectionPin = direction;
    this->MicroSteps = microSteps;
}

void TB6600Driver::InitializeDriver()
{
    pinMode(this->EnablePin, OUTPUT);		// Configure GPIO0 as an output
    pinMode(this->PulsePin, OUTPUT);		// Configure GPIO1 as an input
    pinMode(this->DirectionPin, OUTPUT);		// Configure GPIO1 as an input

    digitalWrite(this->EnablePin, Released);
    digitalWrite(this->DirectionPin, turnRight);
    //char msgBuf[MAX_MESSAGE_LENGTH];
    digitalWrite(this->EnablePin, Locked);
    //this->callerMessages.data = (caddr_t)msgBuf;
    //this->callerMessages.size= sizeof(msgBuf);

    //int err= pthread_create(&moveMotorThread,NULL,&TB6600Driver::DoAsyncThread,this);
}

void TB6600Driver::WaitUntilFinishedRT() {

    rt_task_join(&RtMotorTask);
    //rt_task_recieve(callingMessageHandler, TM_INFINITE);
}

void TB6600Driver::WaitUntilFinished() {
    pthread_join(moveMotorThread, NULL);
}

void TB6600Driver::DoRtMotorTask1(void *arg) {
    TB6600Driver* _this = (TB6600Driver*)arg;
    if(_this->SmoothMovement) {
        //Calculate movement table
        _this->StepsDelayTable = new int[_this->NextSteps];
        int MaximumUserSpeed = ((TB6600Driver::MaximumSpeedHz / _this->MicroSteps) / 100) * _this->mSpeedPercent;
        int MaximumAddUserSpeed = (TB6600Driver::MaximumSpeedHz / _this->MicroSteps) - MaximumUserSpeed;
        int FinalSpeed = (TB6600Driver::MaximumSpeedHz / _this->MicroSteps) + MaximumAddUserSpeed;
        int ReverseStepCount = 1;
        for(int i = 1; i <= _this->NextSteps; i++) {
            if(i < _this->aSteps)
                _this->StepsDelayTable[i - 1] = FinalSpeed + (FinalSpeed / _this->aSteps) * (_this->aSteps - i); //_this->aSteps) * (_this->NextSteps - i);
            else if(_this->NextSteps - i > _this->aSteps - 1) {
                //Maximum speed
                _this->StepsDelayTable[i - 1] = FinalSpeed;
            }
            else {
                //Calculate deacceleration

                _this->StepsDelayTable[i - 1] = FinalSpeed + (FinalSpeed / _this->aSteps) * (ReverseStepCount++);
            }
        }
        printf("First Speed %d, Second Speed: %d, Third Speed %d, Final Speed %d\r\n\n", _this->StepsDelayTable[0], _this->StepsDelayTable[1], _this->StepsDelayTable[2], _this->StepsDelayTable[_this->aSteps - 1]);
    }
    rt_task_sleep(200);  //sleep 20 us
    if(_this->LastDir != _this->NextDir) {
        switch(_this->NextDir) {
            case Left:
                digitalWrite(_this->DirectionPin, turnLeft);
                rt_task_sleep(200);  //sleep 5 us
                break;
            case Right:
                digitalWrite(_this->DirectionPin, turnRight);
                rt_task_sleep(200);  //sleep 5 us
                break;
            default:
                return;
        }
        _this->LastDir = _this->NextDir;
    }
    for(int i =0; i < _this->NextSteps; i++) {
        digitalWrite(_this->PulsePin, 1);
        rt_task_sleep(1000);  //sleep 5 us
        digitalWrite(_this->PulsePin, 0);
        rt_task_sleep(_this->StepsDelayTable[i]);  //sleep 0.900 ms);
        //sleep here
    }
    delete [] _this->StepsDelayTable;
    rt_task_sleep(200);  //sleep 20 us
}

void TB6600Driver::MoveAsyncRT(int Steps, TB6600Direction dir, bool Smooth = false, int AccelSteps = 0, int MaxSpeedPercent = 90) {
    this->NextSteps = Steps;
    this->NextDir = dir;
    this->SmoothMovement = Smooth;
    this->mSpeedPercent = MaxSpeedPercent;
    this->aSteps = AccelSteps;
    rt_task_create(&RtMotorTask, "MoveMotorTask", 0, 99, T_JOINABLE);
    rt_task_start(&RtMotorTask, &DoRtMotorTask1, this);
}

void TB6600Driver::MoveAsyncRT(int Steps, TB6600Direction dir) {
    this->NextSteps = Steps;
    this->NextDir = dir;
    rt_task_create(&RtMotorTask, "MoveMotorTask", 0, 99, T_JOINABLE);
    rt_task_start(&RtMotorTask, &DoRtMotorTask1, this);
}

void *TB6600Driver::DoAsyncThread(void *arg) {
    TB6600Driver* _this = (TB6600Driver*)arg;
    digitalWrite(_this->EnablePin, Locked);
    switch(_this->NextDir) {
        case Left:
            digitalWrite(_this->DirectionPin, turnLeft);
            break;
        case Right:
            digitalWrite(_this->DirectionPin, turnRight);
            break;
        default:
        return nullptr;
    }

    for(int i =0; i < _this->NextSteps; i++) {
        digitalWrite(_this->PulsePin, 1);
        std::this_thread::sleep_for(std::chrono::microseconds(10));
        digitalWrite(_this->PulsePin, 0);
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
        //sleep here
    }
    digitalWrite(_this->EnablePin, Released);
}

void TB6600Driver::MoveAsync(int Steps, TB6600Direction dir) {
    this->NextSteps = Steps;
    this->NextDir = dir;
    int err = pthread_create(&moveMotorThread,NULL,&TB6600Driver::DoAsyncThread,this);
}

void TB6600Driver::MoveSync(int value, TB6600Direction direction)
{
    digitalWrite(this->EnablePin, Locked);
    switch(direction) {
        case Left:
            digitalWrite(this->DirectionPin, turnLeft);
            break;
        case Right:
            digitalWrite(this->DirectionPin, turnRight);
            break;
        default:
        return;
    }

    for(int i =0; i < value; i++) {
        digitalWrite(this->PulsePin, 1);
        //delay(1);
        std::this_thread::sleep_for(std::chrono::microseconds(5));
        digitalWrite(this->PulsePin, 0);
        std::this_thread::sleep_for(std::chrono::microseconds(900));
        //delay(5);
        //sleep here
    }
    digitalWrite(this->EnablePin, Released);
}

TB6600Driver::~TB6600Driver()
{
    digitalWrite(this->EnablePin, Released);
}
//digitalWrite(pin number, state);
