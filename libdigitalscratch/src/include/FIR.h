#pragma once

#include <QVector>

class FIR
{
 private:
        QVector<double> x;
        QVector<double> h;

 public:
        FIR(QVector<double> h);
        virtual ~FIR();

 public:
    double compute(double sample);
};
