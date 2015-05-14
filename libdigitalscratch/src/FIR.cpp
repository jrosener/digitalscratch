#include <FIR.h>
#include <iterator>

using namespace std;

FIR::FIR(QVector<double> h)
{
    this->h = h; // Copy filter kernel
    this->x.reserve(h.length()); // Create delay line
    for (int i = 0; i < h.length(); i++)
    {
        this->x << 0;
    }
}

FIR::~FIR()
{
    return;
}

double FIR::compute(double sample)
{
    double y = 0.0f;

    // Shift delay line
    for(int i = this->x.length() - 1; i > 0; i--)
    {
        this->x[i] = this->x[i-1];
    }
    this->x[0] = sample;

    // Compute filter output
    for(int i = 0; i < this->x.length(); i++)
    {
        y += (this->x[i] * this->h[i]);
    }

    return y;
}
