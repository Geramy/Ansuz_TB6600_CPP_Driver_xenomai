#ifndef ISTEPPERDRIVER_H
#define ISTEPPERDRIVER_H


class IStepperDriver
{
    public:
        enum StepperDirection { Left = 0x01, Right = 0x00 };

        IStepperDriver();

        void SetMinimumPulseWidth(int us);

        virtual void Move(int Steps, StepperDirection direction);
        virtual void MoveOnceAsyncRT(int Steps, StepperDirection direction);
        virtual void MoveOnceAsyncRTWait();

        virtual ~IStepperDriver();

    protected:

    private:
};

#endif // ISTEPPERDRIVER_H
