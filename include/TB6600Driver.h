#ifndef TB6600DRIVER_H
#define TB6600DRIVER_H
#include <cobalt/time.h>
#include <cobalt/boilerplate/sched.h>
#include <cobalt/pthread.h>

//#include <alchemy/timer.h>
#include <alchemy/task.h>


class TB6600Driver
{
    public:
        enum TB6600Direction { None = 0x00, Left = 0x01, Right = 0x02 };
        TB6600Driver(int en, int direction, int pulse, int MicroSteps);
        void InitializeDriver();

        void MoveSync(int value, TB6600Direction direction);
        void MoveAsync(int Steps, TB6600Direction dir);
        void MoveAsyncRT(int Steps, TB6600Direction dir);
        void MoveAsyncRT(int Steps, TB6600Direction dir, bool Smooth, int AccelSteps, int MaxSpeedPercent);
        void WaitUntilFinished();
        void WaitUntilFinishedRT();

        virtual ~TB6600Driver();
    protected:
        RT_TASK_MCB taskMessages, callerMessages;
        int MicroSteps = 1;
        int NextSteps = 0;
        TB6600Direction NextDir;
        TB6600Direction LastDir = None;
        int *StepsDelayTable = nullptr;
        static void DoRtMotorTask1(void *arg);
        static void* DoAsyncThread(void *arg);
    private:
    pthread_t moveMotorThread;
    RT_TASK RtMotorTask;

    int EnablePin = 0;
    int PulsePin = 0;
    int DirectionPin = 0;
    bool SmoothMovement = false;

    int aSteps = 0;
    int mSpeedPercent = 90;

    const static int turnLeft = 1;
    const static int turnRight = 0;
    const static int Locked = 1;
    const static int Released = 0;
    const static int MaximumSpeedHz = 800000;
};

#endif // TB6600DRIVER_H
