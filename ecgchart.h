#ifndef ECGCHART_H
#define ECGCHART_H

#include <QList>
#include <qwt_plot.h>
#include <qwt_plot_curve.h>
#include <qwt_plot_grid.h>
#include <qwt_symbol.h>
#include <qwt_legend.h>
#include <qwt_scale_draw.h>
#include <qwt_scale_engine.h>
#include <qwt_plot_magnifier.h>

class TimeScaleDraw: public QwtScaleDraw
{
public:
    TimeScaleDraw(int sampleRate)
    {
        updateTime(sampleRate);
    }
    QwtText label(double v) const
    {
        //qDebug()<<QString::number(v);
        double fiveTicks = (_sampleRate*0.04)*5;
        QString minLabel = QString::number(v/fiveTicks,'f',3);
        QString label = QString::number(v/_sampleRate,'f',3);
        if(label.contains(".000")) return label;
        else if(minLabel.contains(".000")) return QString::number(v/_sampleRate,'f',3);
        else return "";

        //return QString::number(v/_sampleRate,'f',3);
    }
    void updateTime(int sampleRate)
    {
        invalidateCache();
        _sampleRate = sampleRate;
    }
private:
    int _sampleRate;
};

class EcgChart
{
public:
    explicit EcgChart(QwtPlot *plot,QList<double> *data,QString fileName,int sampleRate);
    void clearPlot();
    void drawPlot();
    QwtPlot *plot;
    void setSampleRate(int value);
    void setPlotColor(QColor color);
private:
    QPolygonF points;
    QwtPlotCurve *curve;
    QList<double> *plotData;
    int plotSampleRate;
};



#endif // ECGCHART_H
