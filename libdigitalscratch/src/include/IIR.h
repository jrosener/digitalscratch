#pragma once

#include <QVector>

class IIR
{
 private:
        QVector<double> x;
        QVector<double> y;
        QVector<double> b;
        QVector<double> a;

 public:
        IIR(QVector<double> a, QVector<double> b);
        virtual ~IIR();

 public:
    double compute(double sample);
};
