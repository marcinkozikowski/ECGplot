#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <QMessageBox>
#include "ecgchart.h"
#include <QScrollBar>

#include <qwt_plot.h>
#include <qwt_plot_curve.h>
#include <qwt_plot_grid.h>
#include <qwt_symbol.h>
#include <qwt_legend.h>
#include <qwt_scale_draw.h>



MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    initMainWindow();
    initStatusBarWidgets();
    initToolBarItems();
}

void MainWindow::initMainWindow(){
    sampleRate=0;
    isPlotLoaded = false;
    lastHorizontalScrollbarValue = 0;
    zoomLevel=100;
    connect(ui->actionOpenFile,SIGNAL(triggered()),this,SLOT(openNewFile()));
    connect(ui->actionCloseApp,SIGNAL(triggered()),this,SLOT(closeMainWIndow()));
    connect(ui->actionSave,SIGNAL(triggered()),this,SLOT(saveAsPng()));
    connect(ui->actionZoom_In,SIGNAL(triggered()),this,SLOT(zoomInClick()));
    connect(ui->actionZoom_Out,SIGNAL(triggered()),this,SLOT(zoomOutClick()));
    connect(ui->actionPlotInitSize,SIGNAL(triggered()),this,SLOT(setInitPlotSize()));
    ui->splitter->setStretchFactor(0,1);
    ui->splitter->setStretchFactor(1, 0);
}

void MainWindow::initStatusBarWidgets()
{
    statusLabel = new QLabel(this);
    statusLabel->setText("To view the graph, load the measurement file. File -> Open ");
    statusLabel->setAlignment(Qt::AlignLeft);
    ui->statusBar->addPermanentWidget(statusLabel,1);

    QLabel *widthLabel = new QLabel(this);
    widthLabel->setText("Width: ");
    widthLabel->setAlignment(Qt::AlignLeft);
    ui->statusBar->addPermanentWidget(widthLabel,0);

    widthValueLabel = new QLabel(this);
    widthValueLabel->setText("0");
    widthValueLabel->setAlignment(Qt::AlignLeft);
    ui->statusBar->addPermanentWidget(widthValueLabel,0);

    QLabel *heightLabel = new QLabel(this);
    heightLabel->setText("Height: ");
    heightLabel->setAlignment(Qt::AlignLeft);
    ui->statusBar->addPermanentWidget(heightLabel,0);


    heightValueLabel = new QLabel(this);
    heightValueLabel->setText("0");
    heightValueLabel->setAlignment(Qt::AlignLeft);
    ui->statusBar->addPermanentWidget(heightValueLabel,0);
}

void MainWindow::initToolBarItems()
{
    //Szerokosc
    QLabel *widthLabel = new QLabel(this);
    widthLabel->setText("Width: ");
    widthLabel->setFont(QFont("Consolas",10));
    widthLabel->setAlignment(Qt::AlignLeft|Qt::AlignCenter);
    ui->mainToolBar->addWidget(widthLabel);
    widthLineEdit = new QLineEdit(this);
    widthLineEdit->setMaximumWidth(80);
    widthLineEdit->setStyleSheet("QLineEdit { border-radius: 5px; border: 1px solid gray; }");
    connect(widthLineEdit,SIGNAL(returnPressed()),this,SLOT(newWidthValueEntered()));
    ui->mainToolBar->addWidget(widthLineEdit);

    //Wysokosc
    QLabel *heightLabel = new QLabel(this);
    heightLabel->setText("Height: ");
    heightLabel->setAlignment(Qt::AlignCenter);
    heightLabel->setMargin(4);
    heightLabel->setFont(QFont("Consolas",10));
    ui->mainToolBar->addWidget(heightLabel);
    heightLineEdit = new QLineEdit(this);
    heightLineEdit->setMaximumWidth(80);
    heightLineEdit->setStyleSheet("QLineEdit { border-radius: 5px; border: 1px solid gray; }");
    connect(heightLineEdit,SIGNAL(returnPressed()),this,SLOT(newHeightValueEntered()));
    ui->mainToolBar->addWidget(heightLineEdit);

    QLabel *space = new QLabel(this);
    space->setMaximumWidth(4);
    ui->mainToolBar->addWidget(space);

    aspectCheckBox = new QCheckBox(this);
    aspectCheckBox->setStyleSheet("QComboBox {border: 1px solid gray;border-radius: 5px;}");
    aspectCheckBox->setText("Aspect ratio");
    aspectCheckBox->setFont(QFont("Consolas",10));
    ui->mainToolBar->addWidget(aspectCheckBox);
    ui->mainToolBar->addSeparator();

    QLabel *sampleRateLabel = new QLabel(this);
    sampleRateLabel->setText("Sample rate (Hz): ");
    sampleRateLabel->setFont(QFont("Consolas",10));
    sampleRateLabel->setAlignment(Qt::AlignCenter);
    sampleRateLabel->setMargin(4);
    ui->mainToolBar->addWidget(sampleRateLabel);
    sampleRateLineEdit = new QLineEdit(this);
    sampleRateLineEdit->setStyleSheet("QLineEdit { border-radius: 5px; border: 1px solid gray; }");
    sampleRateLineEdit->setMaximumWidth(80);
    connect(sampleRateLineEdit,SIGNAL(returnPressed()),this,SLOT(newSampleRateValueEntered()));
    ui->mainToolBar->addWidget(sampleRateLineEdit);
    ui->mainToolBar->addSeparator();

    ui->plotColorComboBox->setStyleSheet("QComboBox { background-color: white; }");
    ui->markerColorComboBox->setStyleSheet("QComboBox { background-color: white; }");

    colors.insert("Red",QColor(255, 0, 0));
    colors.insert("Green",QColor(0, 255, 0));
    colors.insert("Blue",QColor(0, 0, 255));
    colors.insert("Black",QColor(0,0,0));

    ui->plotColorComboBox->addItems(colors.keys());
    ui->plotColorComboBox->setCurrentIndex(0);
    ui->markerColorComboBox->addItems(colors.keys());
    ui->markerColorComboBox->setCurrentIndex(3);
    ui->showMarkerCheckBox->setChecked(true);
    ui->markerSizeLineEdit->setText(QString::number(15));
    ui->plotLineWidthLineEdit->setText(QString::number(3));
    connect(ui->plotColorComboBox,SIGNAL(currentIndexChanged(QString)),this,SLOT(selectedColorChanged(QString)));
    connect(ui->markerColorComboBox,SIGNAL(currentIndexChanged(QString)),this,SLOT(markerSelectedColorChanged(QString)));
    connect(ui->showMarkerCheckBox,SIGNAL(stateChanged(int)),this,SLOT(showMarkerChanged(int)));

    aspectCheckBox->setChecked(true);
    canPlotAction(false);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::openNewFile()
{
    currentFile = openFileDialog.getOpenFileName( this,
                                                  "Open file with measurments",
                                                  "C:\\",
                                                  tr("Text files txt (*.txt);;Files ecg (*.ecg)"));
    if(currentFile!="")
    {
        if(isPlotLoaded){
            resetPlotSizeConstraints();
        }
        graphData.clear();
        ui->filePathLabel->setText(currentFile);
        QFile file(currentFile);
        if(checkFile(&file))
        {
            file.open(QIODevice::ReadOnly | QIODevice::Text);
            QTextStream in(&file);
            while (!in.atEnd()) {
                QString line = in.readLine();
                bool ok(false);
                double d = line.toDouble(&ok);
                if (ok) {
                    graphData.append(d);
                }
            }
            ui->samplesNumberLabel->setText(QString::number(graphData.length()));
            createNewChart(graphData);
            ui->minmvLabel->setText(chart->minMV+" mV");
            ui->maxmvLabel->setText(chart->maxMV+ "mV");
            setTableViewData(graphData);
            canPlotAction(true);
            isPlotLoaded = true;
            statusLabel->setText("File: \""+currentFile+"\" was sucesfully loaded.");
            ui->sampleRateLabel->setText(QString::number(retriveSampleRate())+" Hz");
            ui->secondsLabel->setText(QString::number(graphData.length()/retriveSampleRate())+" s");
        }
        else
        {
            QMessageBox *msgBox1 = new QMessageBox;
            msgBox1->setWindowTitle("Incorrect file format");
            msgBox1->setIcon(QMessageBox::Warning);
            msgBox1->setText("Unfortunately, the indicated file can not be read correctly, our team is already working on it to solve this problem as quickly as possible;]");
            msgBox1->exec();
            delete msgBox1;
        }
    }
}

int MainWindow::retriveSampleRate(){
    if(sampleRateLineEdit->text()!="")
    {
        bool ok=false;
        sampleRate = sampleRateLineEdit->text().toInt(&ok);
        if(!ok){
            QMessageBox *msgBox1 = new QMessageBox;
            msgBox1->setWindowTitle("Incorrect sampling value");
            msgBox1->setIcon(QMessageBox::Warning);
            msgBox1->setText("The entered sampling value is incorrect !");
            msgBox1->exec();
            delete msgBox1;
            sampleRate = 320;
        }
    }
    else
    {
        sampleRate = 320;
    }
    sampleRateLineEdit->setText(QString::number(sampleRate));
    return sampleRate;
}

bool MainWindow::checkFile(QFile *file){
    if (!file->open(QIODevice::ReadOnly | QIODevice::Text)) return false;
    QTextStream in(file);
    QString line = in.readLine();
    file->close();
    if(line.split(';').length()>1) return false;
    bool ok = false;
    line.toDouble(&ok);
    if(ok) return true;
    else return false;
}

void MainWindow::createNewChart(QList<double> data)
{
    chart = new EcgChart(ui->chartView,&data,currentFile,retriveSampleRate());
    chart->drawPlot();
    ui->chartView->replot();
    currentChart = chart->plot;
    currentChart->setLayout( new QVBoxLayout );
    currentChart->layout()->setAlignment( Qt::AlignTop );
    ui->scrollArea->setWidget( currentChart );
    ui->scrollArea->setWidgetResizable( true );
    ui->actionSave->setEnabled(true);
    initHeight = chart->plot->size().height();
    initWidth = chart->plot->size().width();
    setInfoValues(initWidth,initHeight);
    initAspectRatio = double(chart->plot->size().height())/double(chart->plot->size().width());
    zoomStepWidth = chart->plot->size().width()*initAspectRatio;
    zoomStepHeight = chart->plot->size().height()*initAspectRatio;
    currentWidth = initWidth;
    ui->showMarkerCheckBox->setChecked(true);
}

void MainWindow::setTableViewData(QList<double> data)
{
    QStandardItemModel *model = new QStandardItemModel(data.length(),2,this); //Rows and 2 Columns
    model->setHorizontalHeaderItem(0, new QStandardItem(QString("Sample")));
    model->setHorizontalHeaderItem(1, new QStandardItem(QString("Value")));

    for(int i = 0; i < data.length() ; i++)
    {
        model->setItem(i,0,new QStandardItem(QString::number(i)));
        model->setItem(i,1,new QStandardItem(QString::number(data.at(i))));
    }
    ui->ecgTableView->setModel(model);
    ui->ecgTableView->setColumnWidth(0,50);
}

void MainWindow::closeMainWIndow()
{
    QApplication::quit();
}

void MainWindow::saveAsPng()
{    
    QString fileName =  QFileDialog::getSaveFileName(this, "Export to .png", "wykres_ekg", "PNG(*.png)");

    if(fileName!="")
    {
        msgDlg = new QMessageBox(QMessageBox::Information,"Exporting to .png","Exporting ecg plot to the .png file, wait patiently for the end...",QMessageBox::NoButton,this);
        msgDlg->show();
        ui->chartView->grab().save(fileName);
        msgDlg->close();
        delete msgDlg;
    }
}

int MainWindow::performSaveChart(QString fileName){
    ui->chartView->grab().save(fileName);
    return 1;
}


void MainWindow::onChartSaved(){
    msgDlg->close();
    disconnect(&m_future_Watcher,SIGNAL(finished()),this,SLOT(onChartSaved()));
}

void MainWindow::zoomInClick()
{
    double aspectRatio = double(chart->plot->size().height())/double(chart->plot->size().width());
    int stepWidth = chart->plot->size().width()*aspectRatio;
    int stepHeight = chart->plot->size().height()*aspectRatio;

    int width = chart->plot->size().width()+stepWidth;
    int height = chart->plot->size().height()+stepHeight;
    zoomLevel = double((double(width)/(double)currentWidth)*100);
    if(zoomLevel<301)
    {
        ui->actionZoomLabel->setText(QString::number(zoomLevel,'f',0)+"%");
        currentChart->setFixedSize(QSize(width,height));
        setInfoValues(width,height);
    }
}

void MainWindow::zoomOutClick(){

    double aspectRatio = double(chart->plot->size().height())/double(chart->plot->size().width());
    int stepWidth = chart->plot->size().width()*aspectRatio;
    int stepHeight = chart->plot->size().height()*aspectRatio;

    int width = chart->plot->size().width()-stepWidth;
    int height = chart->plot->size().height()-stepHeight;
    zoomLevel = double((double(width)/(double)currentWidth)*100);
    if(zoomLevel>9)
    {
        ui->actionZoomLabel->setText(QString::number(zoomLevel,'f',0)+"%");
        currentChart->setFixedSize(QSize(width,height));
        setInfoValues(width,height);
    }
}

void MainWindow::setInitPlotSize(){
    currentChart->setFixedSize(QSize(initWidth,initHeight));
    ui->actionZoomLabel->setText("100%");
    setInfoValues(initWidth,initHeight);
    currentWidth = initWidth;
}

void MainWindow::canPlotAction(bool value){
    ui->actionSave->setEnabled(value);
    ui->actionZoom_In->setEnabled(value);
    ui->actionZoom_Out->setEnabled(value);
    ui->actionPlotInitSize->setEnabled(value);
    sampleRateLineEdit->setEnabled(value);
    widthLineEdit->setEnabled(value);
    heightLineEdit->setEnabled(value);
    ui->plotColorComboBox->setEnabled(value);
    ui->markerColorComboBox->setEnabled(value);
    ui->markerSizeLineEdit->setEnabled(value);
    ui->plotLineWidthLineEdit->setEnabled(value);
}

void MainWindow::setInfoValues(int width, int height)
{
    widthValueLabel->setText(QString::number(width));
    heightValueLabel->setText(QString::number(height));
    widthLineEdit->setText(QString::number(width));
    heightLineEdit->setText(QString::number(height));
}

void MainWindow::newWidthValueEntered()
{
    int height = 0;
    int width = 0;
    if(aspectCheckBox->isChecked())
    {
        width = widthLineEdit->text().toInt();
        height = width*initAspectRatio;
        ui->chartView->setFixedSize(width,height);
        setInfoValues(width,height);
    }
    else
    {
        height = heightLineEdit->text().toInt();
        width = widthLineEdit->text().toInt();
        currentChart->setFixedSize(QSize(width,height));
        setInfoValues(width,height);
    }
    currentWidth=width;
}

void MainWindow::newHeightValueEntered(){
    int height = 0;
    int width = 0;
    if(aspectCheckBox->isChecked())
    {
        height = heightLineEdit->text().toInt();
        width = height/initAspectRatio;
        currentChart->setFixedSize(QSize(width,height));
        setInfoValues(width,height);
    }
    else
    {
        height = heightLineEdit->text().toInt();
        width = widthLineEdit->text().toInt();
        currentChart->setFixedSize(QSize(width,height));
        setInfoValues(width,height);
    }
    currentWidth=width;
}

void MainWindow::newSampleRateValueEntered()
{
    resetPlotSizeConstraints();
    createNewChart(graphData);
    ui->sampleRateLabel->setText(QString::number(retriveSampleRate())+" Hz");
    ui->secondsLabel->setText(QString::number(graphData.length()/retriveSampleRate())+" s");
    //    sampleRate = retriveSampleRate();
    //    chart->setSampleRate(sampleRate);
}

void MainWindow::resetPlotSizeConstraints(){
    chart->clearPlot();
    ui->chartView->setMinimumHeight(0);
    ui->chartView->setMaximumHeight(16777215);
    ui->chartView->setMinimumWidth(0);
    ui->chartView->setMaximumWidth(16777215);
    ui->actionZoomLabel->setText("100%");
}

void MainWindow::selectedColorChanged(QString color)
{
    chart->setPlotColor(colors[color]);
}

void MainWindow::markerSelectedColorChanged(QString color)
{
    chart->setMarkerColor(colors[color]);
}

void MainWindow::on_ecgTableView_clicked(const QModelIndex &index)
{
    if(ui->showMarkerCheckBox->isChecked())
    {
        chart->setMarkerPosition(index.row(),graphData.at(index.row()));
        QScrollBar *h = ui->scrollArea->horizontalScrollBar();
        QScrollBar *v = ui->scrollArea->verticalScrollBar();
        int positionH = ((ui->chartView->width()/graphData.length())*index.row())-h->width()/2;
        int positionV = ((ui->chartView->height()/chart->mVTicks)/std::abs(graphData.at(index.row())))-v->width()/2;
        h->setValue(positionH);
        v->setValue(positionV);
    }
}

void MainWindow::on_plotLineWidthLineEdit_returnPressed()
{
    bool ok = false;
    int size = ui->plotLineWidthLineEdit->text().toInt(&ok);
    if(ok) chart->setPlotLineWidth(size);
}

void MainWindow::on_markerSizeLineEdit_returnPressed()
{
    bool ok = false;
    int size = ui->markerSizeLineEdit->text().toInt(&ok);
    if(ok) chart->setMarkerSize(size);
}

void MainWindow::showMarkerChanged(int state)
{
    chart->isMarkerVisible(state);
}

