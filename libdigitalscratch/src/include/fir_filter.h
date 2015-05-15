#pragma once

#include <QVector>

class FIR_filter
{
 private:
        QVector<double> x;
        QVector<double> h;

 public:
        FIR_filter(QVector<double> h);
        virtual ~FIR_filter();

 public:
    double compute(double sample);
};
