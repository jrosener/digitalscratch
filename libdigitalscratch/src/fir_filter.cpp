#include <fir_filter.h>

using namespace std;

FIR_filter::FIR_filter(QVector<double> h)
{
    this->h = h; // Copy filter kernel
    this->x.reserve(h.length()); // Create delay line
    for (int i = 0; i < h.length(); i++)
    {
        this->x << 0;
    }
}

FIR_filter::~FIR_filter()
{
    return;
}

double FIR_filter::compute(double sample)
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
