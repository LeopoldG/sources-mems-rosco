
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMessageBox>
#include <QLabel>
#include <QThread>
#include <QHash>
#include <QPair>
#include <QTimer>
#include <QAbstractTableModel>
#include <analogwidgets/manometer.h>
#include <qledindicator/qledindicator.h>
#include "optionsdialog.h"
#include "memsinterface.h"
#include "aboutbox.h"
#include "logger.h"
#include "commonunits.h"
#include "helpviewer.h"
#include <qapplication.h>
#include <qwidget.h>
#include <qlayout.h>
#include <qobject.h>
#include <QStatusBar>
#include <iostream>
#include <qcustomplot.h>
#include <QTreeWidget>
#include <qcursor.h>
#include <QMouseEvent>
#include <QPainter>
#include <QTranslator>
#include <cmath>
#include <QPushButton>
namespace Ui
{
    class MainWindow;

}



class MainWindow : public QMainWindow
{    
    Q_OBJECT



public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
   void setupLambdaPlot(QCustomPlot *customPlot);
   void setuplogplot(QCustomPlot *customplot);
   void setupcurves(QCustomPlot *customplot);
   void  setuptable_list(QTableWidget *table_list);
    void setupchartsview(QTableWidget *table_list);


    double rpm=0;
    double ambient=0;
   double battery=0;
   double idle=0;
   double map=0;
   double throttlepos=0;
   double coolant=0;
   double air_fuel_ratio=0;
   double carbon=0;
   double closed_loop=0;
   double coil=0;
   double crancs=0;
   double fc=0;
   double fc0=0;
   double fc1=0;
   double fc4=0;
   double fuel=0;
   double iac=0;
   double idle_base=0;
   double idle_error2=0;
   double hot=0;
   double idle_setpoint=0;
   double idl_dev=0;
   double ign_adv2=0;
   double ign_adv=0;
   double ign_adv_off=0;
   double igns_witch=0;
   double intake=0;
   double lambda_dutycycle=0;
   double lambda_frequency=0;
   double lambda_status=0;
   double lambda=0;
   double ltft=0;
   double park=0;

   double stft=0;
   double throttleangle=0;
   double uk1=0;
   double uk10=0;
   double uk11=0;
   double uk12=0;
   double uk13=0;
   double uk14=0;
   double uk15=0;
   double uk16=0;
   double uk17=0;
   double uk18=0;
   double uk19=0;
   double uk2=0;
   double uk4=0;
   double uk5=0;
   double uk6=0;
   double uk7=0;
   double uk8=0;
   double uk9=0;
   double idle_set_point=0;
   double idle_dev=0;
   int sp1,sp2;

public slots:
    void onDataReady();
    void onConnect();
    void onDisconnect();
    void onReadError();
    void onReadSuccess();
    void onFailedToConnect(QString dev);
    void onInterfaceThreadReady();
    void onNotConnected();
    void onEcuIdReceived(uint8_t* id);
    void onFuelPumpTestComplete();
    void onACRelayTestComplete();
    void onPTCRelayTestComplete();
    void onMoveIACComplete();
    void onCommandError();
    void showVals(QMouseEvent *event);
    void showHint(QMouseEvent *event);
    void showCursor(QMouseEvent *event);
    void cellSelectedSlot(int nRow, int nCol);
    void onFaultCodeClearComplete();
    void adjustmentsResetComplete();
    void onresetEcuComplete();
    void onmemsSaveComplete();
    void onresetEmiComplete();

    void chartsSlot();

 //   void createFolderPath();

 //   void openlogs();

private slots:



  void on_btnEnglish_clicked();
  void on_btnGerman_clicked();
  void on_btnFrench_clicked();
  void on_btnSpanish_clicked();


signals:
    void requestToStartPolling();
    void requestThreadShutdown();

    void fuelPumpTest();
    void ptcRelayTest();
    void acRelayTest();
    void injectorTest();
    void coilTest();
    void moveIAC(int desiredPos);
    void fuel_trim_plus();
   void fuel_trim_minus();
    void idle_decay_plus();
    void idle_decay_minus();
    void idle_speed_plus();
    void idle_speed_minus();
    void ignition_advance_plus();
    void ignition_advance_minus();

    void Purge_Valve_On();
    void O2Heater_On();
    void O2Heater_Off();
    void Boost_Valve_On();
    void Fan1_On();
    void Fan2_On();
   void open_IAC();
   void close_IAC();

    void fillText1();
    void  mems_clear_faults();
    void mems_reset_ECU();
    void mems_Save();
    void mems_resetADJ();
    void mems_resetEMI();
protected:
    void closeEvent(QCloseEvent *event);
    void mouseDoubleClickEvent();
    void cellDoubleClicked (int , int);
    void mousePressEvent();
     void changeEvent(QEvent*);
private:
    Ui::MainWindow *m_ui;

    QThread *m_memsThread;
    MEMSInterface *m_mems;
    OptionsDialog *m_options;
    AboutBox *m_aboutBox;
    QMessageBox *m_pleaseWaitBox;
    HelpViewer *m_helpViewerDialog;
    QTimer dataTimer;

    Logger *m_logger;
    QTranslator translatorFr;
     QTranslator translatorGe;
     QTranslator translatorEsp;
     QTranslator translatorDe;
    const float mapGaugeMaxPsi = 16.0;  // vorher static
    const float mapGaugeMaxKPa = 160.0; // vorher static
    bool m_actuatorTestsEnabled;
  char idString[20];
  char id;
  QString ecuid;
    int rate;
   int vrange;
    int l;
    double tick;
     QString field0,field1,field2,field3,field4,field5,field6,field7,field8,field9,field10,
             field11,field12,field13,field14,field15,field16,field17,field18,field19,field20,
              field21,field22,field23,field24,field25,field26,field27,field28,field29,field30,
              field31,field32,field33,field34,field35,field36,field37,field38,field39,field40,
              field41,field42,field43,field44,field45,field46,field47,field48,field49,field50,
             field51,field52,field53,field54,field55,field56,field57,field58,field59,field60;
    int line_count;
   uint8_t icmd;
    double time;
    double key;
    QVector<double> x,z, y1,y2,y3,y4,
     y5,y6,y7,y8,y9,y10,y11,y12,y13,y14,y15,y16,y17,
   y18,y19,y20,y21,y22,y23,y24,y25,y26,y27,y28,y29,y30,y31,y32,
     y33,y34,y35,y36,y37,y38,y39,y40,y41,y42,y43,y44,y45,y46,y47,
     y48,y49,y50,y51,y52,y53,y54,y55,y56,y57,y58,y59,y60,y61;
    QString tempUnitstr;
    QString Textfile;
    QString str;
    QString str2;
    QString Secondline;
    QStringList Headers;
    QString high;
    QString low;
    QString highlow;
    char Wert1;
    QHash<TemperatureUnits,QString> *m_tempUnitSuffix;
    QHash<TemperatureUnits,QPair<int,int> > *m_tempRange;
    QHash<TemperatureUnits,QPair<int,int> > *m_tempLimits;

    void buildSpeedAndTempUnitTables();
    void setupWidgets();
    int convertTemperature(int tempF);
    void addTreeRoot(QString name, QString description);
    void addTreeChild(QTreeWidgetItem *parent,
                      QString name, QString description);


private slots:
    void onExitSelected();
    void onEditOptionsClicked();
    void onHelpContentsClicked();
    void onHelpAboutClicked();
    void onConnectClicked();
    void onDisconnectClicked();
    void onStartLogging();
    void onStopLogging();
    void onMoveIACClicked();
    void onTestFuelPumpRelayClicked();
    void onTestACRelayClicked();
    void onTestPTCRelayClicked();
    void setActuatorTestsEnabled(bool enabled);
    void on_m_fuel_trim_plusButton_clicked();
    void on_m_fuel_trim_minusButton_clicked();
    void on_m_idle_decay_plusButton_clicked();
    void on_m_idle_decay_minusButton_clicked();
    void on_m_idle_speed_plusButton_clicked();
    void on_m_idle_speed_minusButton_clicked();
    void on_exitButton_clicked();
    void on_m_ignition_advance_plusButton_clicked();
    void on_m_ignition_advance_minusButton_clicked();
    void on_m_idle_air_plusButton_clicked();
    void on_m_idle_air_minusButton_clicked();
    void on_m_Purge_Valve_OnButton_clicked();
    void on_m_O2Heater_OnButton_clicked();
    void on_m_O2Heater_OffButton_clicked();
    void on_m_Boost_Valve_OnButton_clicked();
    void on_m_Fan1_OnButton_clicked();
    void on_m_Fan2_OnButton_clicked();
    void on_lambdalengthslider_sliderMoved(int position);
    void on_readlog_click_clicked();
   //  void on_readlog_click_2_clicked();

   void on_Refresh_button_clicked();
   void on_Refresh_log10_clicked();
   void on_Refresh_log50_clicked();
   void on_Refresh_log100_clicked();
   void on_Refresh_log200_clicked();
   void on_Refresh_log500_clicked();
   void on_Refresh_log1000_clicked();
   void on_Refresh_log2000_clicked();
   void on_Refresh_log6000_clicked();

void on_Refresh_chartslog10_clicked();
void on_Refresh_chartslog50_clicked();
void on_Refresh_chartslog100_clicked();
void on_Refresh_chartslog200_clicked();
void on_Refresh_chartslog500_clicked();
void on_Refresh_chartslog1000_clicked();
void on_Refresh_chartslog2000_clicked();


  // void on_logfilewidget_activated(const QModelIndex &index);

void cellSelectedSlot2(int nRow, int nCol);
void on_m_clearFaultsButton_clicked();
void on_m_resetadjustments_clicked();
void on_m_saveadjustments_clicked();






void on_m_resetECUButton_clicked();
};
class MainWidget : public QWidget
{

    Q_OBJECT

public:

    MainWidget(QWidget *parent = 0, const char *name = 0);




};
class Sleeper : public QThread
{
public:
    static void usleep(unsigned long usecs){QThread::usleep(usecs);}
    static void msleep(unsigned long msecs){QThread::msleep(msecs);}
    static void sleep(unsigned long secs){QThread::sleep(secs);}
};






#endif // MAINWINDOW_H

