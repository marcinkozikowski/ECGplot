#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFileDialog>
#include <QFile>
#include <QLabel>
#include <QStandardItemModel>
#include "ecgchart.h"
#include <qwt_plot.h>
#include <QCheckBox>
#include <QLineEdit>
#include <QComboBox>
#include <QFutureWatcher>
#include <QMessageBox>
#include <QtConcurrent/QtConcurrentRun>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void createNewChart(QList<double> data);
    QLabel *statusLabel;
    QLabel *widthValueLabel;
    QLabel *heightValueLabel;
    QCheckBox *aspectCheckBox;
    QLineEdit *heightLineEdit;
    QLineEdit *widthLineEdit;
    QLineEdit *sampleRateLineEdit;
    void setTableViewData(QList<double> data);
    bool checkFile(QFile *file);

private:
    QMap<QString, QColor> colors;
    bool isPlotLoaded;
    Ui::MainWindow *ui;
    QFileDialog openFileDialog;
    QList<double> graphData;
    int lastHorizontalScrollbarValue;
    QString currentFile;
    EcgChart *chart;
    QwtPlot *chartView;
    QWidget * currentChart;
    double initAspectRatio;
    double currentAspectRation;
    double initWidth;
    double initHeight;
    int zoomStepWidth;
    int zoomStepHeight;
    int sampleRate;
    double zoomLevel;
    int currentWidth;
    void canPlotAction(bool value);
    void initMainWindow();
    void initStatusBarWidgets();
    void initToolBarItems();
    void setInfoValues(int width,int height);
    int retriveSampleRate();
    void resetPlotSizeConstraints();
    int performSaveChart(QString fileName);
    QMessageBox *msgDlg;
    QFuture<int> m_future;
    QFutureWatcher<void> m_future_Watcher;

public slots:
    void openNewFile();
    void closeMainWIndow();
    void saveAsPng();
    void zoomInClick();
    void zoomOutClick();
    void setInitPlotSize();
    void newWidthValueEntered();
    void newHeightValueEntered();
    void newSampleRateValueEntered();
    void selectedColorChanged(QString color);
    void markerSelectedColorChanged(QString color);
    void onChartSaved();
    void showMarkerChanged(int state);
private slots:
    void on_ecgTableView_clicked(const QModelIndex &index);
    void on_plotLineWidthLineEdit_returnPressed();
    void on_markerSizeLineEdit_returnPressed();
};

#endif // MAINWINDOW_H
