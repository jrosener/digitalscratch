#pragma once

class Unwrap
{
 private:
    double lastPhase;
    double currentPhase;

 public:
        Unwrap();
        virtual ~Unwrap();

 public:
    double compute(const double &phase);
};
