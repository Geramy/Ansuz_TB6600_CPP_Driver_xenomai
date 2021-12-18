#include "IStepperDriver.h"

IStepperDriver::IStepperDriver()
{
    //ctor
}

void IStepperDriver::SetMinimumPulseWidth(int us) {

}

void IStepperDriver::Move(int Steps, StepperDirection direction)
{

}
void IStepperDriver::MoveOnceAsyncRT(int Steps, StepperDirection direction)
{

}
void IStepperDriver::MoveOnceAsyncRTWait() {

}

IStepperDriver::~IStepperDriver()
{
    //dtor
}
