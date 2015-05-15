#include <unwrapper.h>

using namespace std;

Unwrapper::Unwrapper()
{
    this->lastPhase = 0.0f;
    this->currentPhase = 0.0f;
}

Unwrapper::~Unwrapper()
{
    return;
}

double Unwrapper::compute(const double &phase)
{
    double deltaPhase = phase - this->lastPhase;

    if (deltaPhase > 3.1416) // Overflow
        this->currentPhase += deltaPhase - 3.1416*2;
    else if (deltaPhase < -3.1416) // Underflow
        this->currentPhase += deltaPhase + 3.1416*2;
    else // There is no phase over/underflow
        this->currentPhase += deltaPhase;
    this->lastPhase = phase;

    return this->currentPhase;
}
