#pragma once

class Unwrapper
{
 private:
    double lastPhase;
    double currentPhase;

 public:
        Unwrapper();
        virtual ~Unwrapper();

 public:
    double compute(const double &phase);
};
