#include <IIR.h>
#include <iterator>

using namespace std;

IIR::IIR(QVector<double> a, QVector<double> b)
{
    if (a.length() == 0)
        this->a << 0;
    this->a = a; // Copy filter kernel
    this->b = b; // Copy filter kernel
    for (int i = 0; i < b.length(); i++) this->x << 0;
    for (int i = 0; i < a.length(); i++) this->y << 0;
}

IIR::~IIR()
{
    return;
}

double IIR::compute(double sample)
{
    double y = 0.0f;

    // Shift delay line
    for (int i = this->x.length() - 1; i > 0; i--)
    {
        this->x[i] = this->x[i-1];
    }
    this->x[0] = sample;
    for (int i = this->y.length() - 1; i > 0; i--)
    {
        this->y[i] = this->y[i-1];
    }

    // Compute filter output
    for (int i = 0; i < this->x.length(); i++)
    {
        y += (this->x[i] * this->b[i]);
    }
    for (int i = 1; i < this->y.length(); i++)
    {
        y -= (this->y[i] * this->a[i]);
    }
    y /= this->a[0];
    this->y[0] = y;

    return y;
}
