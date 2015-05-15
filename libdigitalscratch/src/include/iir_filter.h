#pragma once

#include <QVector>

class IIR_filter
{
 private:
        QVector<double> x;
        QVector<double> y;
        QVector<double> b;
        QVector<double> a;

 public:
        IIR_filter(QVector<double> a, QVector<double> b);
        virtual ~IIR_filter();

 public:
    double compute(double sample);
};
