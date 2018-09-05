#include "ecgchart.h"

EcgChart::EcgChart(QwtPlot *plot,QList<double> *data,QString fileName,int sampleRate)
{
    this->plot = plot;
    QStringList fileNamePath = fileName.split('/');
    fileName = fileNamePath.at(fileNamePath.length()-1);
    //ui->chartView->setTitle("Jakis prosty wykres");
    plot->setTitle( "Wykres EKG "+fileName );
    plot->setCanvasBackground( Qt::white );

    plotData = data;
    plotSampleRate = sampleRate;
}

void EcgChart::drawPlot(){

    plot->enableAxis(QwtPlot::xBottom, true);
    plot->enableAxis(QwtPlot::yLeft,true);

    //Min and max value for Y axis
    bool ok = false;
    QString min = QString::number(*std::min_element(plotData->begin(), plotData->end()),'f',1);
    QString max = QString::number(*std::max_element(plotData->begin(), plotData->end()),'f',1);

    int a = (min.toDouble(&ok)-0.1)*10;
    int b = (max.toDouble(&ok)+0.1)*10;
    int ticks = std::abs(a-b);

    plot->setAxisScale( QwtPlot::yLeft, min.toDouble(&ok)-0.1, max.toDouble(&ok)+0.1,0.1 );
    plot->setAxisMaxMinor(QwtPlot::yLeft,2);


    //Min and max value for X axis
    plot->setAxisScale( QwtPlot::xBottom,0, plotData->length(),plotSampleRate);
    TimeScaleDraw *timeScaleDraw = new TimeScaleDraw(plotSampleRate);
    plot->setAxisScaleDraw(QwtPlot::xBottom, timeScaleDraw);


    QwtScaleDiv divX;
    QwtLinearScaleEngine *lineX = new QwtLinearScaleEngine();
    divX = lineX->divideScale(0,plotData->length(),plotData->length()/plotSampleRate,0,plotSampleRate*0.04);

    //Set major ticks
//    QList<double> majorTicks;
//    QList<double> minorTicks;
//    double tickStep = plotSampleRate*0.04;
//    qDebug()<<"X Tick step: "+QString::number(tickStep);
//    for(int i=0 ;i<(plotData->length()/plotSampleRate)*25;i++)
//    {
//        if(i%5 == 0)
//        {
//            majorTicks<<i*tickStep;
//            qDebug()<<"Major: "+QString::number(i);
//        }
//        else
//        {
//            minorTicks<<i*tickStep;
//            qDebug()<<i;
//        }
//    }
//    divX.setTicks(QwtScaleDiv::MajorTick,majorTicks);
//    divX.setTicks(QwtScaleDiv::MinorTick, minorTicks);


    plot->setAxisScaleDiv(QwtPlot::xBottom,divX);

//    QList<double> majorTicksY;
//    majorTicksY<<-2<<-1.5<<-1<<0<<0.5<<1<<1.5<<2;
//    QList<double> minorTicksY;
//    minorTicksY<<-1.9<<-1.8<<-1.7<<-1.6<<-1.5<<-1.4<<-1.3<<-1.2<<-1.1<<-0.9<<-0.8<<-0.7<<-0.6<<-0.4<<-0.3<<-0.2<<-0.1<<0.1<<0.2<<0.3<<0.4<<0.5<<0.6<<0.7<<0.8<<0.9<<1.1<<1.2<<1.3<<1.4<<1.5<<1.6<<1.7<<1.8<<1.9;
    QwtScaleDiv divY;
    QwtLinearScaleEngine *lineY = new QwtLinearScaleEngine();
    divY = lineY->divideScale(min.toDouble(&ok)-0.1,max.toDouble(&ok)+0.1,ticks,0);
//    divY.setTicks(QwtScaleDiv::MajorTick,majorTicksY);
//    divY.setTicks(QwtScaleDiv::MinorTick,minorTicksY);
    plot->setAxisScaleDiv(QwtPlot::yLeft,divY);

    plot->setAxisTitle(QwtPlot::xBottom,"Time [s]");
    plot->setAxisTitle(QwtPlot::yLeft,"Millivolts [mV]");

    //dorysuj grid
    QwtPlotGrid *grid = new QwtPlotGrid();
    grid->enableX(true);
    grid->enableXMin(true);
    grid->setMajorPen(QPen(Qt::black, 0, Qt::DotLine));
    grid->setMinorPen(QPen(Qt::black, 0, Qt::DotLine));

    grid->attach(plot);

    curve = new QwtPlotCurve();
    //curve->setTitle( "Some Points" );
    curve->setPen( Qt::red, 3 ),
    curve->setRenderHint( QwtPlotItem::RenderAntialiased, true );


    for(int i=0;i<plotData->length();i++)
    {
        points.append(QPointF(i,plotData->at(i)));
    }

    curve->setSamples( points );
    curve->attach( plot );
}

void EcgChart::clearPlot()
{
    plot->detachItems();
}

void EcgChart::setSampleRate(int value)
{
    plotSampleRate = value;
}

void EcgChart::setPlotColor(QColor color)
{
    curve->setPen(color,3);
    plot->replot();
}


