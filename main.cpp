#include <iostream>
#include "wiringPi.h"
#include <xeno_config.h>
#include <xenomai/init.h>
#include <malloc.h>
#include "TB6600Driver.h"
#include<unistd.h>

using namespace std;

int main(int argc, char *const *argv)
{
    xenomai_init(&argc, &argv);
    unsigned int microsecond = 1000000;
    wiringPiSetup();			// Setup the library
    //GPIO 17 = 11
    //GPIO 22 = 15
    //GPIO 27 = 13
    //StepperMotor1 = new TB6600(ctl, 11, 15, 13);
    //17, 22, 27
    //public TB6600(GpioController ctx, int ena, int pul, int dir)
    TB6600Driver *motor1 = new TB6600Driver(0, 2, 3, 16);
    motor1->InitializeDriver();
    while(true) {
        cout << "Moving Motor 6400 steps 20 times.\r\n";
        int MaximumSteps = 6400 * 5;
        motor1->MoveAsyncRT(MaximumSteps, TB6600Driver::TB6600Direction::Left, true, MaximumSteps / 100 * 10, 99);
        cout << "Waiting to finish.\r\n";
        motor1->WaitUntilFinishedRT();
        int StepsPerRevolution = 6400;
        int RandomPassesBeforePause = 20;

        for(int i = 0; i < RandomPassesBeforePause; i++) {
            srand(time(NULL));  //Changed from rand(). srand() seeds rand for you.
            int Movements = rand() % 6400 * 10 + 1000;
            if(Movements < 10)
                continue;
            int Speed = rand() % 100 + 10;
            srand(time(NULL));
            int RampSteps = rand() % Movements + 9;
            srand(time(NULL));
            int Direction = rand() % 50;
            printf("Moving %d Steps, Speed: %d, RampSteps %d, Direction, %d\r\n", Movements, Speed, RampSteps, Direction);
            if(Direction > 25)
                motor1->MoveAsyncRT(Movements, TB6600Driver::TB6600Direction::Left, true, RampSteps, Speed);
            else
                motor1->MoveAsyncRT(Movements, TB6600Driver::TB6600Direction::Right, true, RampSteps, Speed);
            cout << "Waiting to finish.\r\n";
            motor1->WaitUntilFinishedRT();
            cout << "Finished\r\n";
        }

        cout << "Finished waiting and now sleeping.\r\n";
        usleep(3 * microsecond);//sleeps for 3 second
        cout << "waking up now.\r\n\n";
    }


    cout << "Hello world!" << endl;
    return 0;
}
//
