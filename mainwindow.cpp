        #include <QtCore>
    #include <QMessageBox>
    #include <QList>
    #include <QDateTime>
    #include <QDir>
    #include <QHBoxLayout>
    #include <QThread>
    #include <QFileDialog>
    #include <QDesktopWidget>
    #include <QCryptographicHash>
    #include <QGraphicsOpacityEffect>
    #include <mainwindow.h>
    #include <ui_mainwindow.h>
    #include <qapplication.h>
    #include <QLCDNumber>
    #include <qcustomplot.h>
    #include <QDebug>
    #include <QScreen>
    #include <QMetaEnum>
    #include <QVector>
    #include <QString>
    #include <qlistview.h>
    #include <QActionEvent>
    #include <QMouseEvent>
    #include <QSettings>
    #include <QtGlobal>
    #include <qcursor.h>
    #include <QTableWidget>
    #include <qprintdialog.h>
    #include <qprinter.h>
    #include <QPainter>
    #include <QTime>
    #include <qtimer.h>
    #include <QBuffer>
    #include <QStatusBar>
    #include <QTranslator>
    #include "mainwindow.h"
    #include <QVariant>
    #include <QApplication>
    #include <QScrollArea>
    #include <QCoreApplication>
    #include <QHeaderView>
    #include <QMessageBox>
    MainWindow::MainWindow(QWidget* parent):QMainWindow(parent),
    m_ui(new Ui::MainWindow),
    m_memsThread(0),
    m_mems(0),
    m_options(0),
    m_aboutBox(0),
    m_pleaseWaitBox(0),
    m_helpViewerDialog(0),
    m_actuatorTestsEnabled(false)

    {

      buildSpeedAndTempUnitTables();
      m_ui->setupUi(this);
    //  info=new mems_info;

      setGeometry(400, 250, 542, 390);
     if(translatorFr.load("Transfile_fr.qm"))  //("c:/qt-examples/mems-rosco2.2.9/Transfile_fr.qm"))
          qDebug()<<"successfully load translation_fr file.";

      if(translatorGe.load("Transfile_ge.qm"))  //("c:/qt-examples/mems-rosco2.2.9/Transfile_ge.qm"))
         qDebug()<<"successfully load translation_ge file.";
       if(translatorEsp.load("Transfile_esp.qm"))  //("c:/qt-examples/mems-rosco2.2.9/Transfile_esp.qm"))
          qDebug()<<"successfully load translation_esp file.";
     //  if(translatorDe.load("Transfile_de.qm"))  //("c:/qt-examples/mems-rosco2.2.9/Transfile_de.qm"))
      //    qDebug()<<"successfully load translation_de file.";

     this->setWindowTitle("mems-rosco"+ QString(" ") +
        QString::number(VER_MAJOR) + "." +
          QString::number(VER_MINOR) + "." +
          QString::number(VER_PATCH));

      m_options = new OptionsDialog(this->windowTitle(), this);
      m_mems = new MEMSInterface(m_options->getSerialDeviceName());
      m_logger = new Logger(m_mems);
    onStartLogging();
    //Plot

       setupLambdaPlot(m_ui->LambdaPlot);
        QTableWidget* chartsview=new QTableWidget();
        setupchartsview(m_ui->chartsview);//line7220
       m_ui->LambdaPlot->replot();
      setuptable_list(m_ui->table_list);
      //QTableWidget* ecucomands=new QTableWidget();


      //alter Bestand

      connect(m_mems, SIGNAL(dataReady()), this, SLOT(onDataReady()));
      connect(m_mems, SIGNAL(connected()), this, SLOT(onConnect()));
      connect(m_mems, SIGNAL(disconnected()), this, SLOT(onDisconnect()));
      connect(m_mems, SIGNAL(readError()), this, SLOT(onReadError()));
      connect(m_mems, SIGNAL(readSuccess()), this, SLOT(onReadSuccess()));
      connect(m_mems, SIGNAL(failedToConnect(QString)), this, SLOT(onFailedToConnect(QString)));
      connect(m_mems, SIGNAL(interfaceThreadReady()), this, SLOT(onInterfaceThreadReady()));
      connect(m_mems, SIGNAL(notConnected()), this, SLOT(onNotConnected()));
      connect(m_mems, SIGNAL(gotEcuId(uint8_t *)), this, SLOT(onEcuIdReceived(uint8_t *)));
      connect(m_mems, SIGNAL(errorSendingCommand()), this, SLOT(onCommandError()));

      connect(m_mems, SIGNAL(fuelPumpTestComplete()), this, SLOT(onFuelPumpTestComplete()));
      connect(m_mems, SIGNAL(acRelayTestComplete()), this, SLOT(onACRelayTestComplete()));
      connect(m_mems, SIGNAL(ptcRelayTestComplete()), this, SLOT(onPTCRelayTestComplete()));
      connect(m_mems, SIGNAL(moveIACComplete()), this, SLOT(onMoveIACComplete()));
      connect(this, SIGNAL(moveIAC(int)), m_mems, SLOT(onIdleAirControlMovementRequest(int)));

      connect(this, SIGNAL(fuelPumpTest()), m_mems, SLOT(onFuelPumpTest()));
      connect(this, SIGNAL(acRelayTest()), m_mems, SLOT(onACRelayTest()));
      connect(this, SIGNAL(ptcRelayTest()), m_mems, SLOT(onPTCRelayTest()));
      connect(this, SIGNAL(requestToStartPolling()), m_mems, SLOT(onStartPollingRequest()));
      connect(this, SIGNAL(requestThreadShutdown()), m_mems, SLOT(onShutdownThreadRequest()));

      connect(this, SIGNAL(Purge_Valve_On()), m_mems, SLOT(on_m_Purge_Valve_OnButton_clicked()));
      connect(this, SIGNAL(O2Heater_On()), m_mems, SLOT(on_m_O2Heater_OnButton_clicked()));
      connect(this, SIGNAL(O2Heater_Off()), m_mems, SLOT(on_m_O2Heater_OffButton_clicked()));
      connect(this, SIGNAL(Boost_Valve_On()), m_mems, SLOT(on_m_Boost_Valve_OnButton_clicked()));
      connect(this, SIGNAL(Fan1_On()), m_mems, SLOT(on_m_Fan1_OnButton_clicked()));
      connect(this, SIGNAL(Fan2_On()), m_mems, SLOT(on_m_Fan2_OnButton_clicked()));

      connect(this, SIGNAL(fuel_trim_plus()), m_mems, SLOT(on_m_fuel_trim_plusButton_clicked()));
      connect(this, SIGNAL(fuel_trim_minus()), m_mems, SLOT(on_m_fuel_trim_minusButton_clicked()));
      connect(this, SIGNAL(idle_decay_plus()), m_mems, SLOT(on_m_idle_decay_plusButton_clicked()));
      connect(this, SIGNAL(idle_decay_minus()), m_mems, SLOT(on_m_idle_decay_minusButton_clicked()));
      connect(this, SIGNAL(idle_speed_plus()), m_mems, SLOT(on_m_idle_speed_plusButton_clicked()));
      connect(this, SIGNAL(idle_speed_minus()), m_mems, SLOT(on_m_idle_speed_minusButton_clicked()));
       connect(this, SIGNAL(ignition_advance_plus()), m_mems, SLOT(on_m_ignition_advance_plusButton_clicked()));
       connect(this, SIGNAL(ignition_advance_minus()), m_mems, SLOT(on_m_ignition_advance_minusButton_clicked()));


      connect(m_mems, SIGNAL(faultCodesClearComplete()), this, SLOT(onFaultCodeClearComplete()));
     connect(m_mems, SIGNAL(adjustmentsResetComplete()), this, SLOT(adjustmentsResetComplete()));
      connect(m_mems, SIGNAL(EcuResetComplete()), this, SLOT(onresetEcuComplete()));
      connect(m_ui->m_connectButton, SIGNAL(clicked()), this, SLOT(onConnectClicked()));
      connect(m_ui->m_disconnectButton, SIGNAL(clicked()), this, SLOT(onDisconnectClicked()));
      connect(m_ui->m_startLoggingButton, SIGNAL(clicked()), this, SLOT(onStartLogging()));
      connect(m_ui->m_stopLoggingButton, SIGNAL(clicked()), this, SLOT(onStopLogging()));
      connect(m_ui->m_idle_air_plusButton,SIGNAL(clicked()),this,SLOT(on_m_idle_air_plusButton_clicked()));
      connect(m_ui->m_idle_air_minusButton,SIGNAL(clicked()),this,SLOT(on_m_idle_air_minusButton_clicked()));
      connect(m_ui->m_resetadjustments,SIGNAL(clicked()), this, SLOT ( on_m_resetadjustments_clicked()));
     setWindowIcon(QIcon(":/icons/key.png"));

      setupWidgets();
    }

    MainWindow::~MainWindow()
    {
      delete m_tempLimits;
      delete m_tempRange;
      delete m_tempUnitSuffix;
      delete m_aboutBox;
      delete m_options;
      delete m_mems;
      delete m_memsThread;
    }




    /**
     * Populates hash tables with unit-of-measure suffixes and temperature thresholds
     */
    void MainWindow::buildSpeedAndTempUnitTables()
    {
      m_tempUnitSuffix = new QHash <TemperatureUnits, QString>;
      m_tempUnitSuffix->insert(Fahrenheit, " F");
      m_tempUnitSuffix->insert(Celsius, " C");

      m_tempRange = new QHash <TemperatureUnits, QPair <int, int> >;

      m_tempRange->insert(Fahrenheit, qMakePair(-40, 280));
      m_tempRange->insert(Celsius, qMakePair(-40, 140));

      m_tempLimits = new QHash <TemperatureUnits, QPair <int, int> >;

      m_tempLimits->insert(Fahrenheit, qMakePair(180, 210));
      m_tempLimits->insert(Celsius, qMakePair(80, 98));
    }

    /**
     * Instantiates widgets used in the main window.
     */
    void MainWindow::setupWidgets()
    {
      // set menu and button icons
      m_ui->m_exitAction->setIcon(style()->standardIcon(QStyle::SP_DialogCloseButton));
      m_ui->m_editSettingsAction->setIcon(style()->standardIcon(QStyle::SP_ComputerIcon));
      m_ui->m_helpContentsAction->setIcon(style()->standardIcon(QStyle::SP_DialogHelpButton));
      m_ui->m_helpAboutAction->setIcon(style()->standardIcon(QStyle::SP_MessageBoxInformation));
      m_ui->m_startLoggingButton->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));
      m_ui->m_stopLoggingButton->setIcon(style()->standardIcon(QStyle::SP_MediaStop));

      // connect menu item signals
      connect(m_ui->m_exitAction, SIGNAL(triggered()), this, SLOT(onExitSelected()));
      connect(m_ui->m_editSettingsAction_2, SIGNAL(triggered()), this, SLOT(onEditOptionsClicked()));
      connect(m_ui->m_helpContentsAction, SIGNAL(triggered()), this, SLOT(onHelpContentsClicked()));
      connect(m_ui->m_helpAboutAction, SIGNAL(triggered()), this, SLOT(onHelpAboutClicked()));



      connect(m_ui->m_testACRelayButton, SIGNAL(clicked()), this, SLOT(onTestACRelayClicked()));
      connect(m_ui->m_testFuelPumpRelayButton, SIGNAL(clicked()), this, SLOT(onTestFuelPumpRelayClicked()));
      connect(m_ui->m_testPTCRelayButton, SIGNAL(clicked()), this, SLOT(onTestPTCRelayClicked()));
      connect(m_ui->m_testIgnitionCoilButton, SIGNAL(clicked()), m_mems, SLOT(onIgnitionCoilTest()));
      connect(m_ui->m_testFuelInjectorButton, SIGNAL(clicked()), m_mems, SLOT(onFuelInjectorTest()));
      connect(m_ui->m_moveIACButton, SIGNAL(clicked()), this, SLOT(onMoveIACClicked()));
     connect(m_ui->m_resetadjustments, SIGNAL(clicked()), m_mems, SLOT(on_m_resetadjustments_clicked()));
      connect(m_ui->m_clearFaultsButton, SIGNAL(clicked()), m_mems, SLOT(onFaultCodesClearRequested()));
      connect(m_ui->m_resetECUButton, SIGNAL(clicked()), m_mems, SLOT(on_m_resetECUButton_clicked()));


      connect(m_ui->m_saveadjustments, SIGNAL(clicked()), m_mems, SLOT(on_m_saveadjustments_clicked()));
      //    connect(m_ui->m_Save_Button, SIGNAL(clicked()), m_mems, SLOT(onEmiSaveButton_clicked()));
      connect(m_ui->m_Purge_Valve_OnButton, SIGNAL(clicked()), this, SLOT(on_m_Purge_Valve_OnButton_clicked()));
      connect(m_ui->m_O2Heater_OnButton, SIGNAL(clicked()), this, SLOT(on_m_O2Heater_OnButton_clicked()));
      connect(m_ui->m_O2Heater_OffButton, SIGNAL(clicked()), this, SLOT(on_m_O2Heater_OffButton_clicked()));
      connect(m_ui->m_Boost_Valve_OnButton, SIGNAL(clicked()), this, SLOT(on_m_Boost_Valve_OnButton_clicked()));
      connect(m_ui->m_Fan1_OnButton, SIGNAL(clicked()), this, SLOT(on_m_Fan1_OnButton_clicked()));
      connect(m_ui->m_Fan2_OnButton, SIGNAL(clicked()), this, SLOT(on_m_Fan2_OnButton_clicked()));


      connect(m_ui->m_fuel_trim_plusButton, SIGNAL(clicked()), this, SLOT(on_m_fuel_trim_plusButton_clicked()));
      connect(m_ui->m_fuel_trim_minusButton, SIGNAL(clicked()), this, SLOT(on_m_fuel_trim_minusButton_clicked()));
      connect(m_ui->m_idle_decay_plusButton, SIGNAL(clicked()), this, SLOT(on_m_idle_decay_plusButton_clicked()));
      connect(m_ui->m_idle_decay_minusButton, SIGNAL(clicked()), this, SLOT(on_m_idle_decay_minusButton_clicked()));
      connect(m_ui->m_idle_speed_plusButton, SIGNAL(clicked()), this, SLOT(on_m_idle_speed_plusButton_clicked()));
      connect(m_ui->m_idle_speed_minusButton, SIGNAL(clicked()), this, SLOT(on_m_idle_speed_minusButton_clicked()));
      connect(m_ui->m_ignition_advance_plusButton, SIGNAL(clicked()), this, SLOT(on_m_ignition_advance_plusButton_clicked()));
      connect(m_ui->m_ignition_advance_minusButton, SIGNAL(clicked()), this, SLOT(on_m_ignition_advance_minusButton_clicked()));
     connect(m_ui->logplot, SIGNAL(mouseDoubleClick(QMouseEvent*)), this, SLOT(showCursor(QMouseEvent*)));
     connect(m_ui->logplot, SIGNAL(mousePress(QMouseEvent*)), this, SLOT(showHint(QMouseEvent*)));
      connect( m_ui->table_list, SIGNAL( cellChanged(int,int)),this, SLOT( cellSelectedSlot(int,int)) );
      connect(m_ui->chartsview,SIGNAL(cellChanged(int,int)),this, SLOT( cellSelectedSlot2(int,int)) );

      // set the LED colors
      m_ui->m_commsGoodLed->setOnColor1(QColor(102, 255, 102));
      m_ui->m_commsGoodLed->setOnColor2(QColor(82, 204, 82));
      m_ui->m_commsGoodLed->setOffColor1(QColor(0, 102, 0));
      m_ui->m_commsGoodLed->setOffColor2(QColor(0, 51, 0));
      m_ui->m_commsBadLed->setOnColor1(QColor(255, 0, 0));
      m_ui->m_commsBadLed->setOnColor2(QColor(176, 0, 2));
      m_ui->m_commsBadLed->setOffColor1(QColor(20, 0, 0));
      m_ui->m_commsBadLed->setOffColor2(QColor(90, 0, 2));

      m_ui->m_idleSwitchLed->setOnColor1(QColor(102, 255, 102));
      m_ui->m_idleSwitchLed->setOnColor2(QColor(82, 204, 82));
      m_ui->m_idleSwitchLed->setOffColor1(QColor(0, 102, 0));
      m_ui->m_idleSwitchLed->setOffColor2(QColor(0, 51, 0));

      m_ui->m_neutralSwitchLed->setOnColor1(QColor(102, 255, 102));
      m_ui->m_neutralSwitchLed->setOnColor2(QColor(82, 204, 82));
      m_ui->m_neutralSwitchLed->setOffColor1(QColor(0, 102, 0));
      m_ui->m_neutralSwitchLed->setOffColor2(QColor(0, 51, 0));

      m_ui->m_lambdastatusLed->setOnColor1(QColor(102, 255, 102));
      m_ui->m_lambdastatusLed->setOnColor2(QColor(82, 204, 82));
      m_ui->m_lambdastatusLed->setOffColor1(QColor(255, 0, 0));
      m_ui->m_lambdastatusLed->setOffColor2(QColor(82, 204, 82));

      m_ui->m_closedLoopLed->setOnColor1(QColor(102, 255, 102));
      m_ui->m_closedLoopLed->setOnColor2(QColor(82, 204, 82));
      m_ui->m_closedLoopLed->setOffColor1(QColor(255, 0, 0));
      m_ui->m_closedLoopLed->setOffColor2(QColor(82, 204, 82));

      m_ui->closedloop_led->setOnColor1(QColor(102, 255, 102));
      m_ui->closedloop_led->setOnColor2(QColor(82, 204, 82));
      m_ui->closedloop_led->setOffColor1(QColor(255, 0, 0));
      m_ui->closedloop_led->setOffColor2(QColor(0, 51, 0));

      m_ui->closedloop_led_3->setOnColor1(QColor(102, 255, 102));
      m_ui->closedloop_led_3->setOnColor2(QColor(82, 204, 82));
      m_ui->closedloop_led_3->setOffColor1(QColor(255, 0, 0));
      m_ui->closedloop_led_3->setOffColor2(QColor(0, 51, 0));

      m_ui->m_faultLedATS->setOnColor1(QColor(255, 0, 0));
      m_ui->m_faultLedATS->setOnColor2(QColor(176, 0, 2));
      m_ui->m_faultLedATS->setOffColor1(QColor(20, 0, 0));
      m_ui->m_faultLedATS->setOffColor2(QColor(90, 0, 2));
      m_ui->m_faultLedCTS->setOnColor1(QColor(255, 0, 0));
      m_ui->m_faultLedCTS->setOnColor2(QColor(176, 0, 2));
      m_ui->m_faultLedCTS->setOffColor1(QColor(20, 0, 0));
      m_ui->m_faultLedCTS->setOffColor2(QColor(90, 0, 2));
      m_ui->m_faultLedFuelPump->setOnColor1(QColor(255, 0, 0));
      m_ui->m_faultLedFuelPump->setOnColor2(QColor(176, 0, 2));
      m_ui->m_faultLedFuelPump->setOffColor1(QColor(20, 0, 0));
      m_ui->m_faultLedFuelPump->setOffColor2(QColor(90, 0, 2));
      m_ui->m_faultLedTps->setOnColor1(QColor(255, 0, 0));
      m_ui->m_faultLedTps->setOnColor2(QColor(176, 0, 2));
      m_ui->m_faultLedTps->setOffColor1(QColor(20, 0, 0));
      m_ui->m_faultLedTps->setOffColor2(QColor(90, 0, 2));
      m_ui->m_faultLedfan1->setOnColor1(QColor(255, 0, 0));
      m_ui->m_faultLedfan1->setOnColor2(QColor(176, 0, 2));
      m_ui->m_faultLedfan1->setOffColor1(QColor(20, 0, 0));
      m_ui->m_faultLedfan1->setOffColor2(QColor(90, 0, 2));
      m_ui->m_faultLedFan2->setOnColor1(QColor(255, 0, 0));
      m_ui->m_faultLedFan2->setOnColor2(QColor(176, 0, 2));
      m_ui->m_faultLedFan2->setOffColor1(QColor(20, 0, 0));
      m_ui->m_faultLedFan2->setOffColor2(QColor(90, 0, 2));
      m_ui->m_faultLedTurbo->setOnColor1(QColor(255, 0, 0));
      m_ui->m_faultLedTurbo->setOnColor2(QColor(176, 0, 2));
      m_ui->m_faultLedTurbo->setOffColor1(QColor(20, 0, 0));
      m_ui->m_faultLedTurbo->setOffColor2(QColor(90, 0, 2));
      m_ui->m_faultLedaircon->setOnColor1(QColor(255, 0, 0));
      m_ui->m_faultLedaircon->setOnColor2(QColor(176, 0, 2));
      m_ui->m_faultLedaircon->setOffColor1(QColor(20, 0, 0));
      m_ui->m_faultLedaircon->setOffColor2(QColor(90, 0, 2));
         m_ui->m_faultLedambient->setOnColor1(QColor(255, 0, 0));
      m_ui->m_faultLedambient->setOnColor2(QColor(176, 0, 2));
      m_ui->m_faultLedambient->setOffColor1(QColor(20, 0, 0));
      m_ui->m_faultLedambient->setOffColor2(QColor(90, 0, 2));
      m_ui->m_faultLedboost->setOnColor1(QColor(255, 0, 0));
      m_ui->m_faultLedboost->setOnColor2(QColor(176, 0, 2));
      m_ui->m_faultLedboost->setOffColor1(QColor(20, 0, 0));
      m_ui->m_faultLedboost->setOffColor2(QColor(90, 0, 2));
      m_ui->m_faultLedcrankshaft->setOnColor1(QColor(255, 0, 0));
      m_ui->m_faultLedcrankshaft->setOnColor2(QColor(176, 0, 2));
      m_ui->m_faultLedcrankshaft->setOffColor1(QColor(20, 0, 0));
      m_ui->m_faultLedcrankshaft->setOffColor2(QColor(90, 0, 2));
      m_ui->m_faultLedcruisecontrol->setOnColor1(QColor(255, 0, 0));
      m_ui->m_faultLedcruisecontrol->setOnColor2(QColor(176, 0, 2));
      m_ui->m_faultLedcruisecontrol->setOffColor1(QColor(20, 0, 0));
      m_ui->m_faultLedcruisecontrol->setOffColor2(QColor(90, 0, 2));
      m_ui->m_faultLedenginecompFancontrol->setOnColor1(QColor(255, 0, 0));
      m_ui->m_faultLedenginecompFancontrol->setOnColor2(QColor(176, 0, 2));
      m_ui->m_faultLedenginecompFancontrol->setOffColor1(QColor(20, 0, 0));
      m_ui->m_faultLedenginecompFancontrol->setOffColor2(QColor(90, 0, 2));
      m_ui->m_faultLedenginecompfanrelais->setOnColor1(QColor(255, 0, 0));
      m_ui->m_faultLedenginecompfanrelais->setOnColor2(QColor(176, 0, 2));
      m_ui->m_faultLedenginecompfanrelais->setOffColor1(QColor(20, 0, 0));
      m_ui->m_faultLedenginecompfanrelais->setOffColor2(QColor(90, 0, 2));
      m_ui->m_faultLedfuel->setOnColor1(QColor(255, 0, 0));
      m_ui->m_faultLedfuel->setOnColor2(QColor(176, 0, 2));
      m_ui->m_faultLedfuel->setOffColor1(QColor(20, 0, 0));
      m_ui->m_faultLedfuel->setOffColor2(QColor(90, 0, 2));
      m_ui->m_faultLedgauge->setOnColor1(QColor(255, 0, 0));
      m_ui->m_faultLedgauge->setOnColor2(QColor(176, 0, 2));
      m_ui->m_faultLedgauge->setOffColor1(QColor(20, 0, 0));
      m_ui->m_faultLedgauge->setOffColor2(QColor(90, 0, 2));
      m_ui->m_faultLedhillassist->setOnColor1(QColor(255, 0, 0));
      m_ui->m_faultLedhillassist->setOnColor2(QColor(176, 0, 2));
      m_ui->m_faultLedhillassist->setOffColor1(QColor(20, 0, 0));
      m_ui->m_faultLedhillassist->setOffColor2(QColor(90, 0, 2));
      m_ui->m_faultLedimmobiliser->setOnColor1(QColor(255, 0, 0));
      m_ui->m_faultLedimmobiliser->setOnColor2(QColor(176, 0, 2));
      m_ui->m_faultLedimmobiliser->setOffColor1(QColor(20, 0, 0));
      m_ui->m_faultLedimmobiliser->setOffColor2(QColor(90, 0, 2));
      m_ui->m_faultLedinjec1->setOnColor1(QColor(255, 0, 0));
      m_ui->m_faultLedinjec1->setOnColor2(QColor(176, 0, 2));
      m_ui->m_faultLedinjec1->setOffColor1(QColor(20, 0, 0));
      m_ui->m_faultLedinjec1->setOffColor2(QColor(90, 0, 2));
      m_ui->m_faultLedinjec2->setOnColor1(QColor(255, 0, 0));
      m_ui->m_faultLedinjec2->setOnColor2(QColor(176, 0, 2));
      m_ui->m_faultLedinjec2->setOffColor1(QColor(20, 0, 0));
      m_ui->m_faultLedinjec2->setOffColor2(QColor(90, 0, 2));
      m_ui->m_faultLedknock->setOnColor1(QColor(255, 0, 0));
      m_ui->m_faultLedknock->setOnColor2(QColor(176, 0, 2));
      m_ui->m_faultLedknock->setOffColor1(QColor(20, 0, 0));
      m_ui->m_faultLedknock->setOffColor2(QColor(90, 0, 2));
      m_ui->m_faultLedlambdaheat->setOnColor1(QColor(255, 0, 0));
      m_ui->m_faultLedlambdaheat->setOnColor2(QColor(176, 0, 2));
      m_ui->m_faultLedlambdaheat->setOffColor1(QColor(20, 0, 0));
      m_ui->m_faultLedlambdaheat->setOffColor2(QColor(90, 0, 2));
      m_ui->m_faultLedmap->setOnColor1(QColor(255, 0, 0));
      m_ui->m_faultLedmap->setOnColor2(QColor(176, 0, 2));
      m_ui->m_faultLedmap->setOffColor1(QColor(20, 0, 0));
      m_ui->m_faultLedmap->setOffColor2(QColor(90, 0, 2));
      m_ui->m_faultLedpurge->setOnColor1(QColor(255, 0, 0));
      m_ui->m_faultLedpurge->setOnColor2(QColor(176, 0, 2));
      m_ui->m_faultLedpurge->setOffColor1(QColor(20, 0, 0));
      m_ui->m_faultLedpurge->setOffColor2(QColor(90, 0, 2));
     //...................................................
      m_ui->m_logFileNameBox->setText(QDateTime::currentDateTime().toString("yyyy-MM-dd_hh.mm"));


      m_ui->m_mapGauge->setMinimum(0.0);
      m_ui->m_mapGauge->setMaximum(210.0);
      m_ui->m_mapGauge->setSuffix("kPa");
      m_ui->m_mapGauge->setNominal(30.0);
      m_ui->m_mapGauge->setCritical(100.0);


      m_ui->m_revCounter->setMinimum(0.0);
      m_ui->m_revCounter->setMaximum(8000);
      m_ui->m_revCounter->setSuffix(" RPM");
      m_ui->m_revCounter->setNominal(6000.0);
      m_ui->m_revCounter->setCritical(6000);

      m_ui->m_revCounter_exhaust->setMinimum(0.0);
      m_ui->m_revCounter_exhaust->setMaximum(1700);
      m_ui->m_revCounter_exhaust->setSuffix(" RPM");
      m_ui->m_revCounter_exhaust->setNominal(800.0);
      m_ui->m_revCounter_exhaust->setCritical(875.0);

      TemperatureUnits tempUnits = m_options->getTemperatureUnits();
      int tempMin = m_tempRange->value(tempUnits).first;
      int tempMax = m_tempRange->value(tempUnits).second;

      m_ui->m_waterTempGauge->setValue(tempMin);
      m_ui->m_waterTempGauge->setMaximum(tempMax);
      m_ui->m_waterTempGauge->setMinimum(tempMin);
      m_ui->m_waterTempGauge->setSuffix(m_tempUnitSuffix->value(tempUnits));
      m_ui->m_waterTempGauge->setNominal(m_tempLimits->value(tempUnits).first);
      m_ui->m_waterTempGauge->setCritical(m_tempLimits->value(tempUnits).second);

      m_ui->m_airTempGauge->setValue(tempMin);
      m_ui->m_airTempGauge->setMaximum(tempMax);
      m_ui->m_airTempGauge->setMinimum(tempMin);
      m_ui->m_airTempGauge->setSuffix(m_tempUnitSuffix->value(tempUnits));
      m_ui->m_airTempGauge->setNominal(110.0);
      m_ui->m_airTempGauge->setCritical(110.0);

      m_ui->m_throttle_pos->setMinimum(0.0);
      m_ui->m_throttle_pos->setMaximum(90.0);
      m_ui->m_throttle_pos->setSuffix(" degree");
      m_ui->m_throttle_pos->setNominal(90);
      m_ui->m_throttle_pos->setCritical(90);

      m_ui->m_lambda_voltage->setMinimum(0.0);
      m_ui->m_lambda_voltage->setMaximum(1000);
      m_ui->m_lambda_voltage->setSuffix("mV");
      m_ui->m_lambda_voltage->setNominal(1000);
      m_ui->m_lambda_voltage->setCritical(1000);

    }

    /**
     * Attempts to open the serial device connected to the ECU,
     * and starts updating the display with data if successful.
     */
    void MainWindow::onConnectClicked()
    {
      // If the worker thread hasn't been created yet, do that now.
      if (m_memsThread == 0)
      {
        m_memsThread = new QThread(this);
        m_mems->moveToThread(m_memsThread);
        connect(m_memsThread, SIGNAL(started()), m_mems, SLOT(onParentThreadStarted()));
      }

      // If the worker thread is alreay running, ask it to start polling the ECU.
      // Otherwise, start the worker thread, but don't ask it to begin polling
      // yet; it'll signal us when it's ready.
      if (m_memsThread->isRunning())
      {
        emit requestToStartPolling();
      }
      else
      {
        m_memsThread->start();
      }
    }

    void MainWindow::onInterfaceThreadReady()
    {
      emit requestToStartPolling();
    }

    void MainWindow::onEcuIdReceived(uint8_t* id)
    {
      char idString[20];

      sprintf(idString, "ECU ID: %02X %02X %02X %02X", id[0], id[1], id[2], id[3]);
      m_ui->m_ecuIdLabel->setText(QString(idString));
    ecuid=m_ui->m_ecuIdLabel->text();

    }

    /**
     * Sets a flag in the worker thread that tells it to disconnect from the
     * serial device.
     */
    void MainWindow::onDisconnectClicked()
    {

      m_ui->m_disconnectButton->setEnabled(false);
      m_mems->disconnectFromECU();
    }

    /**
     * Closes the main window and terminates the application.
     */
    void MainWindow::onExitSelected()
    {

      this->close();
    }

    /**
     * Converts temperature in Fahrenheit degrees to the desired units.
     * @param tempF Temperature in Fahrenheit degrees
     * @return Temperature in the desired units
     */
    int MainWindow::convertTemperature(int tempF)
    {
      double temp = tempF;

      switch (m_options->getTemperatureUnits())
      {
      case Celsius:
        temp = (temp - 32) * (0.5555556);
        break;
      case Fahrenheit:;
      default:Celsius;
        break;
      }

      return (int)temp;
    }

    void MainWindow::setActuatorTestsEnabled(bool enabled)
    {
      m_ui->m_testACRelayButton->setEnabled(enabled);
      m_ui->m_testFuelInjectorButton->setEnabled(enabled);
      m_ui->m_testFuelPumpRelayButton->setEnabled(enabled);
      m_ui->m_moveIACButton->setEnabled(enabled);
      m_ui->m_testIgnitionCoilButton->setEnabled(enabled);
      m_ui->m_testPTCRelayButton->setEnabled(enabled);
      m_actuatorTestsEnabled = enabled;
    }

    /**
     * Updates the gauges and indicators with the latest data available from
     * the ECU.
     */
    void MainWindow::onDataReady()
    {
      mems_data* data = m_mems->getData();
      int corrected_iac = (data->iac_position > IAC_MAXIMUM) ? IAC_MAXIMUM : data->iac_position;
      float corrected_throttle = (data->throttle_pot_voltage > 5.0) ? 5.0 : data->throttle_pot_voltage;

      if ((data->engine_rpm == 0) && !m_actuatorTestsEnabled)
      {
        setActuatorTestsEnabled(true);

      }
      else if ((data->engine_rpm > 0) && m_actuatorTestsEnabled)
      {
        setActuatorTestsEnabled(false);


      }

      m_ui->m_throttleBar->setValue(corrected_throttle / 5.00 * 100);
      m_ui->m_throttlePotVolts->setText(QString::number(data->throttle_pot_voltage, 'f', 2) + "V");
      m_ui->m_idleBypassPosBar->setValue((float)corrected_iac / (float)IAC_MAXIMUM * 100);
      m_ui->m_idleBypassPosBar_2->setValue((float)corrected_iac) ; /*/ (float)IAC_MAXIMUM * 100);*/
      m_ui->m_idle_base_pos->setValue(data->idle_base_pos);
      m_ui->m_iacPositionSteps->setText(QString::number(data->iac_position));
      m_ui->m_revCounter->setValue(data->engine_rpm);
      m_ui->m_revCounter_exhaust->setValue(data->engine_rpm);
      m_ui->m_throttle_pos->setValue(data->throttle_angle/2);
      m_ui->m_mapGauge->setValue(data->map_kpa);
      m_ui->m_waterTempGauge->setValue(convertTemperature(data->coolant_temp_f));
      m_ui->m_airTempGauge->setValue(convertTemperature(data->intake_air_temp_f));
      m_ui->m_voltage->setText(QString::number(data->battery_voltage, 'f', 1) + "V");

      //fauldleds
      m_ui->m_faultLedCTS->setChecked((data->dtc0 & 0x01) != 0);
      m_ui->m_faultLedATS->setChecked((data->dtc0 & 0x02) != 0);
      m_ui->m_faultLedTurbo->setChecked((data->dtc0 & 0x08) != 0);
      m_ui->m_faultLedambient->setChecked((data->dtc0 & 0x10) != 0);
      m_ui->m_faultLedfuel->setChecked((data->dtc0 & 0x20) != 0);
      m_ui->m_faultLedknock->setChecked((data->dtc0& 0x40) != 0);
      m_ui->m_faultLedgauge->setChecked((data->dtc1 & 0x01) != 0);
      m_ui->m_faultLedFuelPump->setChecked((data->dtc1 & 0x02) != 0);
      m_ui->m_faultLedaircon->setChecked((data->dtc1 & 0x08) != 0);
      m_ui->m_faultLedpurge->setChecked((data->dtc1 & 0x10) != 0);
      m_ui->m_faultLedmap->setChecked((data->dtc1 & 0x20) != 0);
      m_ui->m_faultLedboost->setChecked((data->dtc1 & 0x40) != 0);
      m_ui->m_faultLedTps->setChecked((data->dtc1 & 0x80) != 0);
      m_ui->m_faultLedlambdaheat->setChecked((data->fault_code0 & 0x08) != 0);
      m_ui->m_faultLedcrankshaft->setChecked((data->fault_code0 & 0x10) != 0);
      m_ui->m_faultLedfan1->setChecked((data->fault_code0 & 0x20) != 0);
      m_ui->m_faultLedimmobiliser->setChecked((data->fault_code0 & 0x40) != 0);
      m_ui->m_faultLedFan2->setChecked((data->fault_code0 & 0x80) != 0);
      m_ui->m_faultLedinjec1->setChecked((data->fault_code4 & 0x02) != 0);
      m_ui->m_faultLedinjec2->setChecked((data->fault_code4 & 0x04) != 0);
      m_ui->m_faultLedenginecompFancontrol->setChecked((data->fault_code4 & 0x08) != 0);
      m_ui->m_faultLedenginecompfanrelais->setChecked((data->fault_code4 & 0x10) != 0);
      m_ui->m_faultLedhillassist->setChecked((data->fault_code4 & 0x20) != 0);
      m_ui->m_faultLedcruisecontrol->setChecked((data->fault_code4 & 0x40) != 0);






        if ((data->idle_switch ) != 0)
        {
            m_ui->m_idleSwitchLed->setChecked(true);
        }
        else
        {
             m_ui->m_idleSwitchLed->setChecked(false);
        }
        if((data->park_neutral_switch )!= 0)
        {
             m_ui->m_neutralSwitchLed->setChecked(true);
        }
        else
        {
             m_ui->m_neutralSwitchLed->setChecked(false);
        }
        if ((data->closed_loop)!= 0)
        {
            m_ui->m_closedLoopLed->setChecked(true);
             m_ui->closedloop_led->setChecked(true);
             m_ui->closedloop_led_3->setChecked(true);

        }
        else
        {
             m_ui->m_closedLoopLed->setChecked(false);
             m_ui->closedloop_led->setChecked(false);
             m_ui->closedloop_led_3->setChecked(false);

        }
        if ((data->lambda_sensor_status)!= 0)
        {
            m_ui->m_lambdastatusLed->setChecked(true);
        }
        else
        {
             m_ui->m_lambdastatusLed->setChecked(false);
         }

     m_ui->m_rpm_2->setText(QString::number(data->engine_rpm));
     m_ui->m_rpmplus->setText(QString::number((data->engine_rpm)+(data->idle_error)));

      m_ui->m_waterT_2->setText(QString::number(convertTemperature(data->coolant_temp_f)));
      m_ui->m_ambientT_2->setText(QString::number(convertTemperature(data->ambient_temp_f)));
      m_ui->m_airT_2->setText(QString::number(convertTemperature(data->intake_air_temp_f)));
      m_ui->m_fuelT_2->setText(QString::number(data->fuel_temp_f));
      m_ui->m_map_2->setText(QString::number(data->map_kpa));
      m_ui->m_voltage_2->setText(QString::number(data->battery_voltage));
     m_ui->m_throttlepot_2->setText(QString::number(data->throttle_pot_voltage));
     m_ui->m_idleswitch_2->setText(QString::number(data->idle_switch));
     m_ui->m_uk1_2->setText(QString::number(data->uk1));
     m_ui->m_park_2->setText(QString::number(data->park_neutral_switch));
     m_ui->m_fcodes_2->setText(QString::number(data->fault_codes));
     m_ui->m_idlesetpoint_2->setText(QString::number(data->idle_set_point));
      m_ui->m_idlehot_2->setText(QString::number(data->idle_hot));
    m_ui->m_uk4_2->setText(QString::number(data->uk2));
     m_ui->m_iacpos_2->setText(QString::number(data->iac_position));
      m_ui->m_idlespeeddev_2->setText(QString::number(data->idle_error));
       m_ui->m_ignadvanceoffset_2->setText(QString::number(data->ignition_advance_offset));
        m_ui->m_ignadvance_2->setText(QString::number(data->ignition_advance));
     m_ui->m_coiltime_2->setText(QString::number(data->coil_time));
     m_ui->m_uk6_2->setText(QString::number(data->crancs));
    m_ui->m_uk7_2->setText(QString::number(data->uk4));
    m_ui->m_uk8_2->setText(QString::number(data->uk5));

    m_ui->m_ignitionswitch_2->setText(QString::number(data->ignition_switch));
     m_ui->m_throttleangle_2->setText(QString::number(data->throttle_angle/2));

     m_ui->m_uk11_2->setText(QString::number(data->uk6));
      m_ui->m_airratio_2->setText(QString::number(data->air_fuel_ratio));
     m_ui->m_faultcode0_2->setText(QString::number(data->fault_code0));
     m_ui->m_lambdavoltage_2->setText(QString::number(data->lambda_voltage_mv*5));
     m_ui->m_lambdafrequency_2->setText(QString::number(data->lambda_sensor_frequency));
     m_ui->m_lambdadutycycle_2->setText(QString::number(data->lambda_sensor_dutycycle));
      m_ui->m_lambdastatus_2->setText(QString::number(data->lambda_sensor_status));
     m_ui->m_closedloop_2->setText(QString::number(data->closed_loop));
     m_ui->m_longtermftrim_2->setText(QString::number(data->long_term_fuel_trim));
     m_ui->m_shorttermftrim_2->setText(QString::number(data->short_term_fuel_trim));
      m_ui->m_fueltrimdev->setText(QString::number((data->long_term_fuel_trim)-(data->short_term_fuel_trim)));
     m_ui->m_carbondutycycle_2->setText(QString::number(data->carbon_canister_dutycycle));
      m_ui->m_faultcode1_2->setText(QString::number(data->fault_code1));
      m_ui->m_idlebase_2->setText(QString::number(data->idle_base_pos));
      m_ui->m_uk15_2->setText(QString::number(data->uk7));
      m_ui->m_uk16_2->setText(QString::number(data->uk8));
     m_ui->m_ignadvance2_2->setText(QString::number(data->ignition_advance2));
     m_ui->m_uk18_2->setText(QString::number(data->uk9));
     m_ui->m_idleerror2_2->setText(QString::number(data->idle_error2));
     m_ui->m_uk20_2->setText(QString::number(data->uk10));
     m_ui->m_faultcode4_2->setText(QString::number(data->fault_code4));
     m_ui->m_uk22_2->setText(QString::number(data->uk11));
     m_ui->m_uk23_2->setText(QString::number(data->uk12));
     m_ui->m_uk24_2->setText(QString::number(data->uk13));
     m_ui->m_uk25_2->setText(QString::number(data->uk14));
     m_ui->m_uk26_2->setText(QString::number(data->uk15));
     m_ui->m_uk27_2->setText(QString::number(data->uk16));
     m_ui->m_uk28_2->setText(QString::number(data->uk1A));
     m_ui->m_uk29_2->setText(QString::number(data->uk1B));
     m_ui->m_uk30_2->setText(QString::number(data->uk1C));

       m_ui->e_lambdavoltage_lcd->display(data->lambda_voltage_mv*5) ;
     m_ui->m_fuel_trim->setText(QString::number(data->short_term_fuel_trim, 'f', 1));
    m_ui->shorttimefueltrim_lcd->display(data->short_term_fuel_trim);

      m_ui->m_long_trim->setText(QString::number(data->long_term_fuel_trim, 'f', 1) );
      m_ui->m_lambda_voltage->setValue(data->lambda_voltage_mv*5 );
      m_ui->m_idlehot_2->setText(QString::number(data->idle_hot));
      m_ui->emi_longtermftrim->setText(QString::number(data->long_term_fuel_trim, 'f', 1));
      m_ui->emi_rpm->setText(QString::number(data->engine_rpm));
     // m_ui->emi_idledecay->setText(QString::number(data->))
    m_ui->emi_ignadvance->setText(QString::number(data->ignition_advance));
    m_ui->emi_IAC->setText(QString::number(data->iac_position));
    m_ui->emi_hot_idle->setText(QString::number(data->idle_hot));
     //values for charts///////////////////////////////////////////////

     rpm=data->engine_rpm;
     coolant=data->coolant_temp_f;

     ambient=data->ambient_temp_f;
     intake=data->intake_air_temp_f;
     fuel=data->fuel_temp_f;
      map=data->map_kpa;
     battery=data->battery_voltage;
     throttlepos=data->throttle_pot_voltage;
      idle=data->idle_switch;
      hot=data->idle_hot;
      uk2=data->uk2;
      iac=data->iac_position;
      ign_adv_off=data->ignition_advance_offset;
      coil=data->coil_time;
     crancs=data->crancs;
      uk4=data->uk4;
     uk5=data->uk5;
     uk6=data->uk6;
      air_fuel_ratio=data->air_fuel_ratio;
      fc0=data->fault_code0;
      lambda = data->lambda_voltage_mv*5;
      lambda_frequency=data->lambda_sensor_frequency;
     lambda_dutycycle=data->lambda_sensor_dutycycle;
     lambda_status=data->lambda_sensor_status;
     closed_loop=data->closed_loop;
     ltft=data->long_term_fuel_trim;
      stft=data->short_term_fuel_trim;
      carbon=data->carbon_canister_dutycycle;
      fc1=data->fault_code1;
     idle_base=data->idle_base_pos;
     uk7=data->uk7;
      uk8=data->uk8;
     uk9=data->uk9;
     uk10=data->uk10;
      uk11=data->uk11;
      uk12=data->uk12;
     uk13=data->uk13;
     uk14=data->uk14;
      uk15=data->uk15;
     uk16=data->uk16;
      uk17=data->uk1A;
     uk18=data->uk1B;
      uk19=data->uk1C;
      park=data->park_neutral_switch;
      throttleangle=data->throttle_angle;
      idle_set_point=data->idle_set_point;
      fc=data->fault_codes;
      ign_adv2=data->ignition_advance2;
      ign_adv=data->ignition_advance;
      idl_dev=data->idle_error;
      uk1=data->uk1;
      igns_witch=data->ignition_switch;
      idle_error2=data->idle_error2;
     fc4=data->fault_code4;


      m_logger->logData();
    }

    /**
     * Opens the settings dialog, where the user may change the
     * serial device, timer interval, gauge settings, and
     * correction factors.
     */
    void MainWindow::onEditOptionsClicked()
    {
      // if the user doesn't cancel the options dialog...
      if (m_options->exec() == QDialog::Accepted)
      {
        TemperatureUnits tempUnits = m_options->getTemperatureUnits();
        QString tempUnitStr = m_tempUnitSuffix->value(tempUnits);

        int tempMin = m_tempRange->value(tempUnits).first;
        int tempMax = m_tempRange->value(tempUnits).second;
        int tempNominal = m_tempLimits->value(tempUnits).first;
        int tempCritical = m_tempLimits->value(tempUnits).second;

        m_logger->setTemperatureUnits(tempUnits);

        m_ui->m_airTempGauge->setSuffix(tempUnitStr);
        m_ui->m_airTempGauge->setValue(tempMin);
        m_ui->m_airTempGauge->setMaximum(tempMax);
        m_ui->m_airTempGauge->setMinimum(tempMin);
        m_ui->m_airTempGauge->repaint();

        m_ui->m_waterTempGauge->setSuffix(tempUnitStr);
        m_ui->m_waterTempGauge->setValue(tempMin);
        m_ui->m_waterTempGauge->setMaximum(tempMax);
        m_ui->m_waterTempGauge->setMinimum(tempMin);
        m_ui->m_waterTempGauge->setNominal(tempNominal);
        m_ui->m_waterTempGauge->setCritical(tempCritical);
        m_ui->m_waterTempGauge->repaint();

        // if the user changed the serial device name and/or the polling
        // interval, stop the timer, re-connect to the ECU (if neccessary),
        // and restart the timer
        if (m_options->getSerialDeviceChanged())
        {
          if (m_mems->isConnected())
          {
            m_mems->disconnectFromECU();
          }
          m_mems->setSerialDevice(m_options->getSerialDeviceName());
        }
      }
    }

    /**
     * Responds to a 'close' event on the main window by first shutting down
     * the other thread.
     * @param event The event itself.
     */
    void MainWindow::closeEvent(QCloseEvent* event)
    {
      m_logger->closeLog();

      if ((m_memsThread != 0) && m_memsThread->isRunning())
      {
        emit requestThreadShutdown();

        m_memsThread->wait(2000);
      }

      event->accept();
    }

    /**
     * Reponds to the "connect" signal from the MEMSInterface by enabling/disabling
     * the appropriate buttons and setting a message in the status bar.
     */
    void MainWindow::onConnect()
    {
      m_ui->m_connectButton->setEnabled(false);
      m_ui->m_disconnectButton->setEnabled(true);
      m_ui->m_commsGoodLed->setChecked(false);
      m_ui->m_commsBadLed->setChecked(false);
       m_ui->m_idleSwitchLed->setChecked(false);
        m_ui->m_closedLoopLed->setChecked(false);
      m_ui->m_clearFaultsButton->setEnabled(true);
      m_ui->m_resetadjustments->setEnabled(true);
       m_ui->m_resetECUButton->setEnabled(true);

    }

    /**
     * Reponds to the "disconnect" signal from the MEMSInterface by enabling/disabling
     * the appropriate buttons and setting a message in the status bar.
     */
    void MainWindow::onDisconnect()
    {
      m_ui->m_connectButton->setEnabled(true);
      m_ui->m_disconnectButton->setEnabled(false);
      m_ui->m_commsGoodLed->setChecked(false);
      m_ui->m_commsBadLed->setChecked(false);
      m_ui->m_ecuIdLabel->setText("ECU ID:");

      m_ui->m_mapGauge->setValue(0.0);
      m_ui->m_revCounter->setValue(0.0);
      m_ui->m_waterTempGauge->setValue(m_ui->m_waterTempGauge->minimum());
      m_ui->m_airTempGauge->setValue(m_ui->m_airTempGauge->minimum());
      m_ui->m_throttleBar->setValue(0);
      m_ui->m_idleBypassPosBar->setValue(0);
       m_ui->m_idleBypassPosBar_2->setValue(0);
      m_ui->m_idleSwitchLed->setChecked(false);
      m_ui->m_neutralSwitchLed->setChecked(false);
      m_ui->m_throttlePotVolts->setText("0.00V");
      m_ui->m_iacPositionSteps->setText("0");
      m_ui->m_voltage->setText("0.0V");
      m_ui->m_faultLedCTS->setChecked(false);
      m_ui->m_faultLedATS->setChecked(false);
      m_ui->m_faultLedFuelPump->setChecked(false);
      m_ui->m_faultLedTps->setChecked(false);
      m_ui->closedloop_led->setChecked(false);
      m_ui->m_closedLoopLed->setChecked(false);
      setActuatorTestsEnabled(false);

      m_ui->m_clearFaultsButton->setEnabled(false);
      m_ui->m_resetadjustments->setEnabled(false);
      m_ui->m_resetECUButton->setEnabled(false);


    }

    /**
     * Responds to the "read error" signal from the worker thread by turning
     * on a cyan lamp.
     */
    void MainWindow::onReadError()
    {
      m_ui->m_commsGoodLed->setChecked(false);
      m_ui->m_commsBadLed->setChecked(true);
    }

    /**
     * Responds to the "read success" signal from the worker thread by turning
     * on a green lamp.
     */
    void MainWindow::onReadSuccess()
    {
      m_ui->m_commsGoodLed->setChecked(true);
      m_ui->m_commsBadLed->setChecked(false);
    }

    /**
     * Opens the log file for writing.
     */
    void MainWindow::onStartLogging()
    {
      if (m_logger->openLog(m_ui->m_logFileNameBox->text()))
      {
        m_ui->m_startLoggingButton->setEnabled(false);
        m_ui->m_stopLoggingButton->setEnabled(true);


      }
      else
      {
        QMessageBox::warning(this, "Error", "Failed to open log file (" + m_logger->getLogPath() + ")", QMessageBox::Ok);
      }
    }

    /**
     * Closes the open log file.
     */
    void MainWindow::onStopLogging()
    {
      m_logger->closeLog();
      m_ui->m_stopLoggingButton->setEnabled(false);
      m_ui->m_startLoggingButton->setEnabled(true);


    }

    /**
     * Displays an dialog box with information about the program.
     */
    void MainWindow::onHelpAboutClicked()
    {
      if (m_aboutBox == 0)
      {
        m_aboutBox = new AboutBox(style(), this->windowTitle(), m_mems->getVersion(), this);
      }
      m_aboutBox->exec();
    }

    /**
     * Displays the online help.
     */
    void MainWindow::onHelpContentsClicked()
    {
      if (m_helpViewerDialog == 0)
      {
        m_helpViewerDialog = new HelpViewer(this->windowTitle(), this);
      }
      m_helpViewerDialog->show();
    }

    /**
     * Displays a message box indicating that an error in connecting to the
     * serial device.
     * @param Name of the serial device that the software attempted to open
     */
    void MainWindow::onFailedToConnect(QString dev)
    {
      if (dev.isEmpty() || dev.isNull())
      {
        QMessageBox::warning(this, "Error",
                             QString("Error connecting to ECU. No serial port name specified.\n\n") +
                             QString("Set a serial device by selecting \"Edit Settings\" from the \"Options\" menu."),
                             QMessageBox::Ok);
      }
      else
      {
        QMessageBox::warning(this, "Error",
                             "Error connecting to ECU on port " + dev + ".\nCheck cable wiring and check that ECU is on.",
                             QMessageBox::Ok);
      }
    }

    /**
     * Displays a message box indicating that an action cannot be completed due
     * to the software not being connected to the ECU.
     */
    void MainWindow::onNotConnected()
    {
      if (m_pleaseWaitBox != 0)
      {
        m_pleaseWaitBox->hide();
      }

      QMessageBox::warning(this, "Error",
                           "This requires that the software first be connected to the ECU (using the \"Connect\" button.)",
                           QMessageBox::Ok);
    }


    void MainWindow::onFuelPumpTestComplete()
    {
      m_ui->m_testFuelPumpRelayButton->setEnabled(true);
    }

    void MainWindow::onACRelayTestComplete()
    {
      m_ui->m_testACRelayButton->setEnabled(true);
    }

    void MainWindow::onPTCRelayTestComplete()
    {
      m_ui->m_testPTCRelayButton->setEnabled(true);
    }

    void MainWindow::onTestFuelPumpRelayClicked()
    {
      m_ui->m_testFuelPumpRelayButton->setEnabled(false);
      emit fuelPumpTest();
    }

    void MainWindow::onTestACRelayClicked()
    {
      m_ui->m_testACRelayButton->setEnabled(false);
      emit acRelayTest();
    }

    void MainWindow::onTestPTCRelayClicked()
    {
      m_ui->m_testPTCRelayButton->setEnabled(false);
      emit ptcRelayTest();
    }

    void MainWindow::onMoveIACClicked()
    {
      m_ui->m_moveIACButton->setEnabled(false);
      QApplication::setOverrideCursor(Qt::WaitCursor);
      float percent = (float)m_ui->m_iacPositionSlider->value()/ 100.0;
      int desiredPos = (int)((float)IAC_MAXIMUM * percent);
      emit moveIAC(desiredPos);
    }

    void MainWindow::onMoveIACComplete()
    {
      QApplication::restoreOverrideCursor();
      m_ui->m_moveIACButton->setEnabled(true);
    }

    void MainWindow::onCommandError()
    {
      QMessageBox::warning(this, "Error", "Error sending command.", QMessageBox::Ok);
    }

    void MainWindow::onFaultCodeClearComplete()
    {
      QMessageBox::information(this, "Complete", "Successfully cleared fault codes.", QMessageBox::Ok);
    }
    void MainWindow::adjustmentsResetComplete()
    {
      QMessageBox::information(this, "Complete", "Successfully reset all adjustments.", QMessageBox::Ok);
    }
    void MainWindow::onresetEcuComplete()
    {
      QMessageBox::information(this, "Complete", "Successfully reset ECU.", QMessageBox::Ok);
    }

    void MainWindow::onresetEmiComplete()
    {
      QMessageBox::information(this, "Complete", "Successfully reset all emission-adjustments.", QMessageBox::Ok);
    }

    void MainWindow::onmemsSaveComplete()
    {
      QMessageBox::information(this, "Complete", "Successfully saved all emission-adjustments.", QMessageBox::Ok);
    }
    void MainWindow::on_m_fuel_trim_plusButton_clicked()

    {
      //  int i;
        m_ui->m_fuel_trim_plusButton->setEnabled(false);
        QApplication::setOverrideCursor(Qt::WaitCursor);
     emit fuel_trim_plus();

    QApplication::restoreOverrideCursor();
         m_ui->m_fuel_trim_plusButton->setEnabled(true);
    }

    void MainWindow::on_m_fuel_trim_minusButton_clicked()
    {
      emit fuel_trim_minus();

    }

    void MainWindow::on_m_idle_decay_plusButton_clicked()
    {
       emit idle_decay_plus();

    }

    void MainWindow::on_m_idle_decay_minusButton_clicked()
    {
       emit idle_decay_minus();

    }

    void MainWindow::on_m_idle_speed_plusButton_clicked()
    {
       emit idle_speed_plus();

    }

    void MainWindow::on_m_idle_speed_minusButton_clicked()
    {
       emit idle_speed_minus();

    }


    void MainWindow::on_exitButton_clicked() //auf readlog-seite
    {

      this->close();
    }


    void MainWindow::on_m_ignition_advance_plusButton_clicked()
    {
        emit ignition_advance_plus();

    }

    void MainWindow::on_m_ignition_advance_minusButton_clicked()
    {
        emit ignition_advance_minus();

    }



    void MainWindow::on_m_idle_air_plusButton_clicked()
    {

      emit moveIAC(+1);

    }

    void MainWindow::on_m_idle_air_minusButton_clicked()
    {

        emit moveIAC(-1);

    }






    void MainWindow::on_m_Purge_Valve_OnButton_clicked()
    {
        emit Purge_Valve_On();
    }

    void MainWindow::on_m_O2Heater_OnButton_clicked()
    {
        emit O2Heater_On();
    }

    void MainWindow::on_m_Boost_Valve_OnButton_clicked()
    {
        emit Boost_Valve_On();
    }

    void MainWindow::on_m_Fan1_OnButton_clicked()
    {
        emit Fan1_On();
    }

    void MainWindow::on_m_Fan2_OnButton_clicked()
    {
        emit Fan2_On();
    }

    //:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::


     void MainWindow::showCursor(QMouseEvent *event)
    {
         QCPItemLine *cursor = new QCPItemLine(m_ui->logplot);
        double mouseX = m_ui->logplot->xAxis->pixelToCoord(event->pos().x());

        double start = m_ui->logplot->yAxis->pixelToCoord(0);
        double end = m_ui->logplot->yAxis->pixelToCoord(m_ui->logplot->size().height());

       cursor->start->setCoords(mouseX, start);
        cursor->end->setCoords(mouseX, end);
        m_ui->logplot->replot();
    }


    void MainWindow::showVals(QMouseEvent* event)
    {
     QCPItemLine *cursor = new QCPItemLine(m_ui->logplot);
     if(event->button() == Qt::RightButton)
       { //for my "cursor"########
        double mouseX = m_ui->logplot->xAxis->pixelToCoord(event->pos().x());
        double startPos = m_ui->logplot->yAxis->pixelToCoord(0);
      double endPos =m_ui->logplot->yAxis->pixelToCoord(m_ui->logplot->maximumHeight());
        cursor->start->setCoords(mouseX, startPos);
        cursor->end->setCoords(mouseX, endPos);

        //for the  graph values #####
        for(int i=0; i<m_ui->logplot->graphCount(); ++i)
        {


            int index = m_ui->logplot->graph(i)->findBegin(mouseX);
            double x = m_ui->logplot->graph(i)->dataMainKey(index);
            double y = m_ui->logplot->graph(i)->dataMainValue(index);

            if(mouseX >= 0 &&( mouseX <= (m_ui->logplot->graph(i)->dataMainKey(m_ui->logplot->graph(i)->dataCount() - 1) + 1)))
            {
                qDebug() << index << "X:" << x << "Y:" << y;
            }
            else
            {
                qDebug() << index << "X:" << 0 << "Y:" << 0;
            }

        }
        m_ui->logplot->replot();
    }}
    //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::



    void MainWindow::on_m_O2Heater_OffButton_clicked()
    {
        emit O2Heater_Off();
    }



    void MainWindow::on_readlog_click_clicked()
    {
        QString dir = QFileDialog::getExistingDirectory(this, tr("Open Directory"),
                                                        "/mems_rosco",
                                                        QFileDialog::ShowDirsOnly
                                                        | QFileDialog::DontResolveSymlinks);

          m_ui->logplot->clearGraphs();
        m_ui->logplot->clearItems();


        m_ui->logplot->replot();

        m_ui->label_text1->setText(" ");
        m_ui->label_text2->setText(" ");
        m_ui->label_text3->setText(" ");
        m_ui->label_text4->setText(" ");
        m_ui->text_1->clear();
         m_ui->text_2->clear();
          m_ui->text_3->clear();
           m_ui->text_4->clear();

     Textfile=QFileDialog::getOpenFileName(this,("open logfile"),("*/logs") , ("*.txt *.csv *.fcr"));//Filefinder


        setuplogplot(m_ui->logplot);
         m_ui->logplot->replot();
       setuptable_list(m_ui->table_list);
    }







    //__________________________________________________________
    void MainWindow::setuplogplot(QCustomPlot *logplot)


        {

            m_ui->logplot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom );

         // include this section to fully disable antialiasing for higher performance:

            m_ui->logplot->setNotAntialiasedElements(QCP::aeAll);
            QFont font;
            font.setStyleStrategy(QFont::NoAntialias);
            m_ui->logplot->xAxis->setTickLabelFont(font);
            m_ui->logplot->yAxis->setTickLabelFont(font);

            m_ui->logplot->legend->setVisible(false);
            m_ui->logplot->legend->setFont(font);

            m_ui->logplot->addGraph( m_ui->logplot->xAxis,  m_ui->logplot->axisRect()->axis(QCPAxis::atRight, 0));//0
            m_ui->logplot->graph(0)->setPen(QPen(Qt::black));
            m_ui->logplot->graph(0)->setVisible(true);
            m_ui->logplot->yAxis2->setVisible(true);
            m_ui->logplot->yAxis2->setRange(0,6500);

            m_ui->logplot->yAxis2->setTickLabelFont(font);
            m_ui->logplot->yAxis2->setTickLabels(true);
            //m_ui->checkBox_rpm->setChecked(true);

            m_ui->logplot->rescaleAxes();

            m_ui->logplot->addGraph(m_ui->logplot->xAxis,m_ui->logplot->yAxis);//1
            m_ui->logplot->rescaleAxes();
            m_ui->logplot->yAxis->setRange(0,260);
            m_ui->logplot->graph(1)->setVisible(false);
            m_ui->logplot->rescaleAxes();
            m_ui->logplot->addGraph();//2
            m_ui->logplot->graph(2)->setVisible(false);
            m_ui->logplot->addGraph();//3
             m_ui->logplot->graph(3)->setVisible(false);
            m_ui-> logplot->addGraph();//4
             m_ui->logplot->graph(4)->setVisible(false);
            m_ui-> logplot->addGraph();//5
             m_ui->logplot->graph(5)->setVisible(false);
            m_ui-> logplot->addGraph();//6
             m_ui->logplot->graph(6)->setVisible(false);
            m_ui-> logplot->addGraph();//7
             m_ui->logplot->graph(7)->setVisible(false);
            m_ui-> logplot->addGraph();//8
             m_ui->logplot->graph(8)->setVisible(false);
            m_ui-> logplot->addGraph();//9
             m_ui->logplot->graph(9)->setVisible(false);
            m_ui-> logplot->addGraph();//10
             m_ui->logplot->graph(10)->setVisible(false);
            m_ui-> logplot->addGraph();//11
             m_ui->logplot->graph(11)->setVisible(false);
            m_ui-> logplot->addGraph();//12
             m_ui->logplot->graph(12)->setVisible(false);
            m_ui-> logplot->addGraph();//13
             m_ui->logplot->graph(13)->setVisible(false);
            m_ui-> logplot->addGraph();//14
             m_ui->logplot->graph(14)->setVisible(false);
            m_ui-> logplot->addGraph();//15
             m_ui->logplot->graph(15)->setVisible(false);
            m_ui-> logplot->addGraph();//16
             m_ui->logplot->graph(16)->setVisible(false);
            m_ui-> logplot->addGraph();//17
             m_ui->logplot->graph(17)->setVisible(false);
            m_ui-> logplot->addGraph();//18
             m_ui->logplot->graph(18)->setVisible(false);
            m_ui-> logplot->addGraph();//19
             m_ui->logplot->graph(19)->setVisible(false);
            m_ui-> logplot->addGraph();//20
             m_ui->logplot->graph(20)->setVisible(false);
            m_ui-> logplot->addGraph();//21
             m_ui->logplot->graph(21)->setVisible(false);
            m_ui-> logplot->addGraph();//22
             m_ui->logplot->graph(22)->setVisible(false);
            m_ui-> logplot->addGraph();//23
             m_ui->logplot->graph(23)->setVisible(false);
            m_ui-> logplot->addGraph();//24
             m_ui->logplot->graph(24)->setVisible(false);
            m_ui-> logplot->addGraph();//25
             m_ui->logplot->graph(25)->setVisible(false);
            m_ui-> logplot->addGraph();//26
             m_ui->logplot->graph(26)->setVisible(false);
            m_ui-> logplot->addGraph();//27
             m_ui->logplot->graph(27)->setVisible(false);
            m_ui-> logplot->addGraph();//28
             m_ui->logplot->graph(28)->setVisible(false);
            m_ui-> logplot->addGraph();//29
             m_ui->logplot->graph(29)->setVisible(false);
            m_ui-> logplot->addGraph();//30
             m_ui->logplot->graph(30)->setVisible(false);
            m_ui-> logplot->addGraph();//31
             m_ui->logplot->graph(31)->setVisible(false);
            m_ui-> logplot->addGraph();//32
             m_ui->logplot->graph(32)->setVisible(false);
            m_ui-> logplot->addGraph();//33
             m_ui->logplot->graph(33)->setVisible(false);
            m_ui-> logplot->addGraph();//34
             m_ui->logplot->graph(34)->setVisible(false);
            m_ui-> logplot->addGraph();//35
             m_ui->logplot->graph(35)->setVisible(false);
            m_ui-> logplot->addGraph();//36
             m_ui->logplot->graph(36)->setVisible(false);
            m_ui-> logplot->addGraph();//37
             m_ui->logplot->graph(37)->setVisible(false);
            m_ui-> logplot->addGraph();//38
             m_ui->logplot->graph(38)->setVisible(false);
            m_ui-> logplot->addGraph();//39
             m_ui->logplot->graph(39)->setVisible(false);
            m_ui-> logplot->addGraph();//40
             m_ui->logplot->graph(40)->setVisible(false);
            m_ui-> logplot->addGraph();//41
             m_ui->logplot->graph(41)->setVisible(false);
            m_ui-> logplot->addGraph();//42
             m_ui->logplot->graph(42)->setVisible(false);
            m_ui-> logplot->addGraph();//43
             m_ui->logplot->graph(43)->setVisible(false);
            m_ui-> logplot->addGraph();//44
             m_ui->logplot->graph(44)->setVisible(false);
            m_ui-> logplot->addGraph();//45
             m_ui->logplot->graph(45)->setVisible(false);
            m_ui-> logplot->addGraph();//46
             m_ui->logplot->graph(46)->setVisible(false);
            m_ui-> logplot->addGraph();//47
             m_ui->logplot->graph(47)->setVisible(false);
            m_ui-> logplot->addGraph();//48
             m_ui->logplot->graph(48)->setVisible(false);
            m_ui-> logplot->addGraph();//49
             m_ui->logplot->graph(49)->setVisible(false);
            m_ui-> logplot->addGraph();//50
             m_ui->logplot->graph(50)->setVisible(false);
            m_ui-> logplot->addGraph();//51
             m_ui->logplot->graph(51)->setVisible(false);
            m_ui-> logplot->addGraph();//52
             m_ui->logplot->graph(52)->setVisible(false);
            m_ui-> logplot->addGraph();//53
             m_ui->logplot->graph(53)->setVisible(false);
             m_ui-> logplot->addGraph();//54
             m_ui->logplot->graph(54)->setVisible(false);
             m_ui-> logplot->addGraph();//55
             m_ui->logplot->graph(55)->setVisible(false);
             m_ui-> logplot->addGraph();//56
             m_ui->logplot->graph(56)->setVisible(false);
             m_ui-> logplot->addGraph();//57
             m_ui->logplot->graph(57)->setVisible(false);
             m_ui-> logplot->addGraph();//58
             m_ui->logplot->graph(58)->setVisible(false);
             m_ui-> logplot->addGraph();//59
             m_ui->logplot->graph(59)->setVisible(false);
             m_ui-> logplot->addGraph();//60
             m_ui->logplot->graph(60)->setVisible(false);

            QSharedPointer<QCPAxisTickerTime> timeTicker(new QCPAxisTickerTime);
            timeTicker->setTimeFormat("%h:%m:%s");
            m_ui->logplot->xAxis->setTicker(timeTicker);
            m_ui->logplot->axisRect()->setupFullAxesBox();

              QVector<double>(x).clear();
               QVector<double>(z).clear();
               QVector<double>(y1).clear();
               QVector<double>(y2).clear();
               QVector<double>(y3).clear();
               QVector<double>(y4).clear();
               QVector<double>(y5).clear();
               QVector<double>(y6).clear();
               QVector<double>(y7).clear();
               QVector<double>(y8).clear();
               QVector<double>(y9).clear();
               QVector<double>(y10).clear();
               QVector<double>(y11).clear();
               QVector<double>(y12).clear();
               QVector<double>(y13).clear();
               QVector<double>(y14).clear();
               QVector<double>(y15).clear();
               QVector<double>(y16).clear();
               QVector<double>(y17).clear();
               QVector<double>(y18).clear();
               QVector<double>(y19).clear();
               QVector<double>(y20).clear();
               QVector<double>(y21).clear();
               QVector<double>(y22).clear();
               QVector<double>(y23).clear();
               QVector<double>(y24).clear();
               QVector<double>(y25).clear();
               QVector<double>(y26).clear();
               QVector<double>(y27).clear();
               QVector<double>(y28).clear();
               QVector<double>(y29).clear();
               QVector<double>(y30).clear();
               QVector<double>(y31).clear();
               QVector<double>(y32).clear();
               QVector<double>(y33).clear();
               QVector<double>(y34).clear();
               QVector<double>(y35).clear();
               QVector<double>(y36).clear();
               QVector<double>(y37).clear();
               QVector<double>(y38).clear();
               QVector<double>(y39).clear();
               QVector<double>(y40).clear();
               QVector<double>(y41).clear();
               QVector<double>(y42).clear();
               QVector<double>(y43).clear();
               QVector<double>(y44).clear();
               QVector<double>(y45).clear();
               QVector<double>(y46).clear();
               QVector<double>(y47).clear();
               QVector<double>(y48).clear();
               QVector<double>(y49).clear();
               QVector<double>(y50).clear();
               QVector<double>(y51).clear();
               QVector<double>(y52).clear();
               QVector<double>(y53).clear();
               QVector<double>(y54).clear();
               QVector<double>(y55).clear();
               QVector<double>(y56).clear();
               QVector<double>(y57).clear();
               QVector<double>(y58).clear();
               QVector<double>(y59).clear();
               QVector<double>(y60).clear();
               QVector<double>(y61).clear();
     /*          QVector<double>(y62).clear();
               QVector<double>(y63).clear();
               QVector<double>(y64).clear();
               QVector<double>(y65).clear();
               QVector<double>(y66).clear();
               QVector<double>(y67).clear();
               QVector<double>(y68).clear();
    */
            // prepare data:

 // prepare data:

        int line_count=1;
        int zeile;
        QString q,hex;
        QString line0;
        int tick=1;
        int i=0;
        QString Firstcharacters;
        QString Feld1;
        QString Firstline;
        QString type;
        QString Firstfield;
        QString ECUID;
        QStringList rpm;
        QStringList mbar;
        //open file
         QBuffer buffer;
        QFile file(Textfile);



        if(!file.open(QIODevice::ReadOnly | QIODevice::Text))

          {
          QMessageBox::warning(this, "Error", "Failed to open log file (" + m_logger->getLogPath() + ")", QMessageBox::Ok);
          }
          {
              line_count=0;
              QTextStream in(&file);

              Firstline =in.readLine(0);
              qDebug()<<"Zeile 1597:"<<Firstline;
              Firstcharacters=Firstline.at(0);
                qDebug()<< "Zeile 1599"<<Firstcharacters;
              Firstcharacters.append(Firstline.at(1));
                qDebug()<< "Zeile 1601"<<Firstcharacters;
              Firstcharacters.append(Firstline.at(2));
                qDebug()<< "Zeile 1603"<<Firstcharacters;
              Firstcharacters.append(Firstline.at(3));
                qDebug()<< "Zeile 1605"<<Firstcharacters;
              Firstcharacters.append(Firstline.at(4));
                qDebug()<< "Zeile 1607"<<Firstcharacters;
              Firstcharacters.append(Firstline.at(5));


              qDebug()<< "Zeile 1611"<<Firstcharacters;



//****************mems-rosco-new*******************************

 if(Firstcharacters=="Ecu Id")
                  {
                      type="mems-rosco-new";
                      qDebug()<<"Zeile1603"<<type;
                      m_ui->label_ecuid->setText(Firstline);
                      m_ui->label_type->setText(type);
                    Secondline = in.readLine();
                     QStringList fields = Secondline.split(',');
                                 qDebug()<<"Zeile 1608 secondline"<<Secondline;
                                 qDebug()<<"Zeile 1609 fields"<<fields;
                      int r=fields.count();
                                qDebug()<<"fields zeile1611"<<r;
                      m_ui->logfilewidget->clear();
                      m_ui->logfilewidget->horizontalHeader()->isVisible();
                      m_ui->logfilewidget->setColumnCount(r);
                         for (int i = 0; i < r; ++i)
                             {

                             m_ui->logfilewidget->setHorizontalHeaderItem(i,new QTableWidgetItem(fields.at(i)));
                             }
                            int l=line_count;
                           // tick=1;
                          while(!in.atEnd())
                          {
                                    l=line_count - 3;
                              QString line = in.readLine();
                              qDebug() << "Zeile1653 line:"<<line;
                              qDebug() << "Zeile1654 linecount:"<<line_count;
                              QStringList fields = line.split(',');

                                 m_ui->logfilewidget->insertRow(l);
                              if (line_count > 3)

                              {
                                  x.append(QTime::fromString(fields.at(0),"hh:mm:ss").msecsSinceStartOfDay()/1000.0);//time
                                // z.append(tick);
                                // tick++;
                                  // take first value and stored into x
                                     m_ui->logfilewidget->setItem(l, 0,new QTableWidgetItem(fields.at(0)));

                                    qDebug()<<"Zeile1640 line="<<fields.at(0);
                                   qDebug()<<"Zeile1641 Zeit x="<<x;
                                    qDebug()<<"Zeile11642 Zeit z="<<z;
                                  y1.append(fields.at(1).toDouble()); // rpm
                                   m_ui->logfilewidget->setItem(l,1, new QTableWidgetItem(fields.at(1)));
                                  y55.append(fields.at(1).toDouble());//rpm<1500
                                  //     double t=(fields.at(2).toDouble());//
                                  //     double u=(t-32)*55556/100000;// calculate celsius from fahrenheit
                                  y2.append(fields.at(2).toDouble());//coolant temp
                                   m_ui->logfilewidget->setItem(l,2, new QTableWidgetItem(fields.at(2)));
                                 qDebug() << "Zeile1563:"<<y2;
                                  //     double a=(fields.at(3).toDouble());
                                  //     double b=(a-32)*55556/100000;
                                  y3.append(fields.at(3).toDouble());//ambient air
                                   m_ui->logfilewidget->setItem(l,3, new QTableWidgetItem(fields.at(3)));

                                   //    double c=(fields.at(4).toDouble());
                                   //    double d=(c-32)*55556/100000;
                                  y4.append(fields.at(4).toDouble());//intake air
                                   m_ui->logfilewidget->setItem(l,4, new QTableWidgetItem(fields.at(4)));
                                  //     double e=(fields.at(5).toDouble());
                                   //    double f=(e-32)*55556/100000;
                                  y5.append(fields.at(5).toDouble());//fuel temp
                                   m_ui->logfilewidget->setItem(l,5, new QTableWidgetItem(fields.at(5)));
                                  y6.append(fields.at(6).toDouble());//map
                                   m_ui->logfilewidget->setItem(l,6, new QTableWidgetItem(fields.at(6)));
                                  y7.append(fields.at(7).toDouble());//battery
                                   m_ui->logfilewidget->setItem(l,7, new QTableWidgetItem(fields.at(7)));
                                  y8.append(fields.at(8).toDouble());//throttle pot
                                   m_ui->logfilewidget->setItem(l,8, new QTableWidgetItem(fields.at(8)));
                                   int is=(fields.at(9).toDouble());//80x0A_idle_switch
                                  is=is*10;
                                  y9.append(is);
                                   m_ui->logfilewidget->setItem(l,9, new QTableWidgetItem(fields.at(9)));
                                  y10.append(fields.at(10).toDouble());//uk1
                                   m_ui->logfilewidget->setItem(l,10, new QTableWidgetItem(fields.at(10)));
                                  y11.append(fields.at(11).toDouble()); // park nut
                                   m_ui->logfilewidget->setItem(l,11, new QTableWidgetItem(fields.at(11)));
                                  y12.append(fields.at(12).toDouble()); // fault code
                                   m_ui->logfilewidget->setItem(l,12, new QTableWidgetItem(fields.at(12)));
                                  y13.append(fields.at(13).toDouble());//idle setpoint
                                   m_ui->logfilewidget->setItem(l,13, new QTableWidgetItem(fields.at(13)));
                                  y14.append(fields.at(14).toDouble());//idle hot
                                  m_ui->logfilewidget->setItem(l,14, new QTableWidgetItem(fields.at(14)));
                                  y15.append(fields.at(15).toDouble());//uk2
                                   m_ui->logfilewidget->setItem(l,15, new QTableWidgetItem(fields.at(15)));
                                  y16.append(fields.at(16).toDouble());//iac
                                   m_ui->logfilewidget->setItem(l,16, new QTableWidgetItem(fields.at(16)));
                                  y17.append(fields.at(17).toDouble());//idle error
                                   m_ui->logfilewidget->setItem(l,17, new QTableWidgetItem(fields.at(17)));
                                  y18.append(fields.at(18).toDouble());//ignadv offset
                                   m_ui->logfilewidget->setItem(l,18, new QTableWidgetItem(fields.at(18)));
                                  y19.append(fields.at(19).toDouble());// ign adv
                                   m_ui->logfilewidget->setItem(l,19, new QTableWidgetItem(fields.at(19)));
                                  y20.append(fields.at(20).toDouble());//coil time
                                   m_ui->logfilewidget->setItem(l,20, new QTableWidgetItem(fields.at(20)));
                                  y21.append(fields.at(21).toDouble()); //cranks
                                   m_ui->logfilewidget->setItem(l,21, new QTableWidgetItem(fields.at(21)));
                                  y22.append(fields.at(22).toDouble()); //uk4
                                   m_ui->logfilewidget->setItem(l,22, new QTableWidgetItem(fields.at(22)));
                                  y23.append(fields.at(23).toDouble());//uk5
                                   m_ui->logfilewidget->setItem(l,23, new QTableWidgetItem(fields.at(23)));
                                  y24.append(fields.at(24).toDouble());//ign switch
                                   m_ui->logfilewidget->setItem(l,24, new QTableWidgetItem(fields.at(24)));
                                  y25.append(fields.at(25).toDouble());//throttle angle
                                   m_ui->logfilewidget->setItem(l,25, new QTableWidgetItem(fields.at(25)));
                                  y26.append(fields.at(26).toDouble());//uk6
                                   m_ui->logfilewidget->setItem(l,26, new QTableWidgetItem(fields.at(26)));

                                  double u =(fields.at(27).toDouble());
                                  u=u/10;
                                  y27.append(u);//air fuel ratio
                                   m_ui->logfilewidget->setItem(l,27, new QTableWidgetItem(fields.at(27)));
                                  y28.append(fields.at(28).toDouble());//fault code 0
                                   m_ui->logfilewidget->setItem(l,28, new QTableWidgetItem(fields.at(28)));
                                  double g=(fields.at(29).toDouble());//lambda volt

                                  double h=g*5;
                                  y29.append(h);
                                   m_ui->logfilewidget->setItem(l,29, new QTableWidgetItem(fields.at(29)));
                                  y30.append(fields.at(30).toDouble());//lamda freq
                                   m_ui->logfilewidget->setItem(l,30, new QTableWidgetItem(fields.at(30)));
                                  y31.append(fields.at(31).toDouble()); // lambdadutycycle
                                   m_ui->logfilewidget->setItem(l,31, new QTableWidgetItem(fields.at(31)));
                                  y32.append(fields.at(32).toDouble()); // lambdastatus
                                   m_ui->logfilewidget->setItem(l,32, new QTableWidgetItem(fields.at(32)));
                                  y33.append(fields.at(33).toDouble());// closed loop
                                   m_ui->logfilewidget->setItem(l,33, new QTableWidgetItem(fields.at(33)));
                                  y34.append(fields.at(34).toDouble());//long term
                                   m_ui->logfilewidget->setItem(l,34, new QTableWidgetItem(fields.at(34)));
                                  y35.append(fields.at(35).toDouble());// short term
                                   m_ui->logfilewidget->setItem(l,35, new QTableWidgetItem(fields.at(35)));
                                  y36.append(fields.at(36).toDouble()); // carbon canister
                                   m_ui->logfilewidget->setItem(l,36, new QTableWidgetItem(fields.at(36)));
                                  y37.append(fields.at(37).toDouble());//faultcode1
                                   m_ui->logfilewidget->setItem(l,37, new QTableWidgetItem(fields.at(37)));
                                  y38.append(fields.at(38).toDouble());//idle base pos
                                   m_ui->logfilewidget->setItem(l,38, new QTableWidgetItem(fields.at(38)));
                                  y39.append(fields.at(39).toDouble()); // uk7
                                   m_ui->logfilewidget->setItem(l,39, new QTableWidgetItem(fields.at(39)));
                                  y40.append(fields.at(40).toDouble());//uk8
                                   m_ui->logfilewidget->setItem(l,40, new QTableWidgetItem(fields.at(40)));
                                  y41.append(fields.at(41).toDouble()); // ignadv2
                                   m_ui->logfilewidget->setItem(l,41, new QTableWidgetItem(fields.at(41)));
                                  y42.append(fields.at(42).toDouble()); //uk9
                                   m_ui->logfilewidget->setItem(l,42, new QTableWidgetItem(fields.at(42)));
                                  y43.append(fields.at(43).toDouble());//idle error2
                                   m_ui->logfilewidget->setItem(l,43, new QTableWidgetItem(fields.at(43)));
                                  y44.append(fields.at(44).toDouble());//uk10
                                   m_ui->logfilewidget->setItem(l,44, new QTableWidgetItem(fields.at(44)));
                                  y45.append(fields.at(45).toDouble());//faultcode4
                                   m_ui->logfilewidget->setItem(l,45, new QTableWidgetItem(fields.at(45)));
                                  y46.append(fields.at(46).toDouble());//uk11
                                   m_ui->logfilewidget->setItem(l,46, new QTableWidgetItem(fields.at(46)));
                                  y47.append(fields.at(47).toDouble());//uk12
                                   m_ui->logfilewidget->setItem(l,47, new QTableWidgetItem(fields.at(47)));
                                  y48.append(fields.at(48).toDouble()); //uk13
                                   m_ui->logfilewidget->setItem(l,48, new QTableWidgetItem(fields.at(48)));
                                  y49.append(fields.at(49).toDouble());//uk14
                                   m_ui->logfilewidget->setItem(l,49, new QTableWidgetItem(fields.at(49)));
                                  y50.append(fields.at(50).toDouble());//uk15
                                   m_ui->logfilewidget->setItem(l,50, new QTableWidgetItem(fields.at(50)));
                                  y51.append(fields.at(51).toDouble());//uk16
                                   m_ui->logfilewidget->setItem(l,51, new QTableWidgetItem(fields.at(51)));
                                  y52.append(fields.at(52).toDouble());//uk17
                                  m_ui->logfilewidget->setItem(l,52, new QTableWidgetItem(fields.at(52)));
                                  y53.append(fields.at(53).toDouble());//uk18
                                   m_ui->logfilewidget->setItem(l,53, new QTableWidgetItem(fields.at(53)));
                                   y54.append(fields.at(54).toDouble());//uk19 jack count
                                    m_ui->logfilewidget->setItem(l,54, new QTableWidgetItem(fields.at(54)));

                              }

                              line_count++;

                          }
                          file.close();
                 }
//****************************Haro-new**********************************
   else if(Firstcharacters=="ECU ID")
                  {
                  type="mems-scan-new";
                  qDebug()<<"Zeile1744"<<type;
                  m_ui->label_ecuid->setText(Firstline);
                  m_ui->label_type->setText(type);
                  QString line = in.readLine();
                   qDebug() << "Zeile1749:"<<line_count <<"::"<<line;

                                 QStringList fields = line.split(',');
                                 qDebug()<<"Zeile 1752 fields"<<fields;
                                  int r=fields.count();
                                            qDebug()<<"fields"<<r;
                                  m_ui->logfilewidget->clear();
                                  m_ui->logfilewidget->horizontalHeader()->isVisible();
                                  m_ui->logfilewidget->setColumnCount(r);
                                     for (int i = 0; i < r; ++i)
                                         {

                                         m_ui->logfilewidget->setHorizontalHeaderItem(i,new QTableWidgetItem(fields.at(i)));
                                         }





                      while(!in.atEnd())
                          {
                          QString line = in.readLine();
                          qDebug() << "Zeile1825:"<<line_count <<"::"<<line;
                          QStringList fields = line.split(',');
                          int l=line_count;
                          m_ui->logfilewidget->insertRow(l);



                                  x.append(QTime::fromString(fields.at(0),"hh:mm:ss").msecsSinceStartOfDay()/1000.0);//time
                                   m_ui->logfilewidget->setItem(l, 0,new QTableWidgetItem(fields.at(0)));
                                  // take first value and stored into x
                                 // x.append(tick);
                                 // tick++;
                                  y1.append(fields.at(1).toDouble()); // 80x01-02_engine-rpm
                                    m_ui->logfilewidget->setItem(l,1, new QTableWidgetItem(fields.at(1)));
                                  y55.append(fields.at(1).toDouble());//rpm<1500

                                  y2.append(fields.at(2).toDouble());//80x03_coolant_temp
                                  m_ui->logfilewidget->setItem(l,2, new QTableWidgetItem(fields.at(2)));
                                  y3.append(fields.at(3).toDouble());//80x04_ambient_temp
                                  m_ui->logfilewidget->setItem(l,3, new QTableWidgetItem(fields.at(3)));
                                  y4.append(fields.at(4).toDouble());//80x05_intake_air_temp
                                   m_ui->logfilewidget->setItem(l,4, new QTableWidgetItem(fields.at(4)));
                                  y5.append(fields.at(5).toDouble());//80x06_fuel_temp
                                   m_ui->logfilewidget->setItem(l,5, new QTableWidgetItem(fields.at(5)));
                                  y6.append(fields.at(6).toDouble());//80x07_map_kpa
                                   m_ui->logfilewidget->setItem(l,6, new QTableWidgetItem(fields.at(6)));
                                  y7.append(fields.at(7).toDouble());//80x08_battery_voltage
                                   m_ui->logfilewidget->setItem(l,7, new QTableWidgetItem(fields.at(7)));
                                  y8.append(fields.at(8).toDouble());//80x09_throttle_pot
                                   m_ui->logfilewidget->setItem(l,8, new QTableWidgetItem(fields.at(8)));

                                   int is=(fields.at(9).toDouble());//80x0A_idle_switch
                                  is=is*10;
                                  y9.append(is);
                                   m_ui->logfilewidget->setItem(l,9, new QTableWidgetItem(fields.at(9)));
                                  y10.append(fields.at(10).toDouble());//80x0B_uk1
                                   m_ui->logfilewidget->setItem(l,10, new QTableWidgetItem(fields.at(10)));
                                  y11.append(fields.at(11).toDouble()); // 80x0C_park_neutral_switch
                                   m_ui->logfilewidget->setItem(l,11, new QTableWidgetItem(fields.at(11)));
                                  y12.append(fields.at(12).toDouble()); // 80x0D-0E_fault_codes
                                   m_ui->logfilewidget->setItem(l,12, new QTableWidgetItem(fields.at(12)));
                                  y13.append(fields.at(13).toDouble());//80x0F_idle_set_point
                                   m_ui->logfilewidget->setItem(l,13, new QTableWidgetItem(fields.at(13)));
                                  y14.append(fields.at(14).toDouble());//80x10_idle_hot
                                   m_ui->logfilewidget->setItem(l,14, new QTableWidgetItem(fields.at(14)));
                                  y15.append(fields.at(15).toDouble());//80x11_uk2
                                   m_ui->logfilewidget->setItem(l,15, new QTableWidgetItem(fields.at(15)));
                                  y16.append(fields.at(16).toDouble());//80x12_iac_position
                                   m_ui->logfilewidget->setItem(l,16, new QTableWidgetItem(fields.at(16)));
                                  y17.append(fields.at(17).toDouble());//80x13-14_idle_error
                                   m_ui->logfilewidget->setItem(l,17, new QTableWidgetItem(fields.at(17)));
                                  y18.append(fields.at(18).toDouble());//80x15_ignition_advance_offset
                                   m_ui->logfilewidget->setItem(l,18, new QTableWidgetItem(fields.at(18)));
                                  y19.append(fields.at(19).toDouble());// 80x16_ignition_advance
                                   m_ui->logfilewidget->setItem(l,19, new QTableWidgetItem(fields.at(19)));
                                  y20.append(fields.at(20).toDouble());//80x17-18_coil_time
                                   m_ui->logfilewidget->setItem(l,20, new QTableWidgetItem(fields.at(20)));
                                  y21.append(fields.at(21).toDouble()); //80x19_crankshaft_position_sensor
                                   m_ui->logfilewidget->setItem(l,21, new QTableWidgetItem(fields.at(21)));
                                  y22.append(fields.at(22).toDouble()); //80x1A_uk4
                                   m_ui->logfilewidget->setItem(l,22, new QTableWidgetItem(fields.at(22)));
                                  y23.append(fields.at(23).toDouble());//80x1B_uk5
                                   m_ui->logfilewidget->setItem(l,23, new QTableWidgetItem(fields.at(23)));
                                  y24.append(fields.at(24).toDouble());//7dx01_ignition_switch
                                   m_ui->logfilewidget->setItem(l,24, new QTableWidgetItem(fields.at(24)));
                                  y25.append(fields.at(25).toDouble());//7dx02_throttle_angle
                                   m_ui->logfilewidget->setItem(l,25, new QTableWidgetItem(fields.at(25)));
                                  y26.append(fields.at(26).toDouble());//7dx03_uk6
                                   m_ui->logfilewidget->setItem(l,26, new QTableWidgetItem(fields.at(26)));
                                  y27.append(fields.at(27).toDouble());//7dx04_air_fuel_ratio
                                   m_ui->logfilewidget->setItem(l,27, new QTableWidgetItem(fields.at(27)));
                                  y28.append(fields.at(28).toDouble());//7dx05_dtc2
                                   m_ui->logfilewidget->setItem(l,28, new QTableWidgetItem(fields.at(28)));
                                  y29.append(fields.at(29).toDouble());//7dx06_lambda_voltage
                                   m_ui->logfilewidget->setItem(l,29, new QTableWidgetItem(fields.at(29)));
                                  y30.append(fields.at(30).toDouble());//7dx07_lambda_sensor_frequency
                                   m_ui->logfilewidget->setItem(l,30, new QTableWidgetItem(fields.at(30)));
                                  y31.append(fields.at(31).toDouble()); //7dx08_lambda_sensor_dutycycle
                                   m_ui->logfilewidget->setItem(l,31, new QTableWidgetItem(fields.at(31)));
                                  y32.append(fields.at(32).toDouble()); // 7dx09_lambda_sensor_status
                                   m_ui->logfilewidget->setItem(l,32, new QTableWidgetItem(fields.at(32)));
                                  y33.append(fields.at(33).toDouble());// 7dx0A_closed_loop
                                   m_ui->logfilewidget->setItem(l,33, new QTableWidgetItem(fields.at(33)));
                                  y34.append(fields.at(34).toDouble());//7dx0B_long_term_fuel_trim
                                   m_ui->logfilewidget->setItem(l,34, new QTableWidgetItem(fields.at(34)));
                                  y35.append(fields.at(35).toDouble());// 7dx0C_short_term_fuel_trim
                                   m_ui->logfilewidget->setItem(l,35, new QTableWidgetItem(fields.at(35)));
                                  y36.append(fields.at(36).toDouble()); //7dx0D_carbon_canister_dutycycle
                                   m_ui->logfilewidget->setItem(l,36, new QTableWidgetItem(fields.at(36)));
                                  y37.append(fields.at(37).toDouble());//7dx0E_dtc3
                                   m_ui->logfilewidget->setItem(l,37, new QTableWidgetItem(fields.at(37)));
                                  y38.append(fields.at(38).toDouble());//7dx0F_idle_base_pos
                                   m_ui->logfilewidget->setItem(l,38, new QTableWidgetItem(fields.at(38)));
                                  y39.append(fields.at(39).toDouble()); //7dx10_uk7
                                   m_ui->logfilewidget->setItem(l,39, new QTableWidgetItem(fields.at(39)));
                                  y40.append(fields.at(40).toDouble());//7dx11_dtc4
                                   m_ui->logfilewidget->setItem(l,40, new QTableWidgetItem(fields.at(40)));
                                  y41.append(fields.at(41).toDouble()); //7dx12_ignition_advance2
                                   m_ui->logfilewidget->setItem(l,41, new QTableWidgetItem(fields.at(41)));
                                  y42.append(fields.at(42).toDouble()); //7dx13_idle_speed_offset
                                   m_ui->logfilewidget->setItem(l,42, new QTableWidgetItem(fields.at(42)));
                                  y43.append(fields.at(43).toDouble());//7dx14_idle_error2
                                   m_ui->logfilewidget->setItem(l,43, new QTableWidgetItem(fields.at(43)));
                                  y44.append(fields.at(44).toDouble());//7dx14-15_uk10
                                   m_ui->logfilewidget->setItem(l,44, new QTableWidgetItem(fields.at(44)));
                                  y45.append(fields.at(45).toDouble());//7dx16_dtc5
                                   m_ui->logfilewidget->setItem(l,45, new QTableWidgetItem(fields.at(45)));
                                  y46.append(fields.at(46).toDouble());//7dx17_uk11
                                   m_ui->logfilewidget->setItem(l,46, new QTableWidgetItem(fields.at(46)));
                                  y47.append(fields.at(47).toDouble());//7dx18_uk12
                                   m_ui->logfilewidget->setItem(l,47, new QTableWidgetItem(fields.at(47)));
                                  y48.append(fields.at(48).toDouble()); //7dx19_uk13
                                   m_ui->logfilewidget->setItem(l,48, new QTableWidgetItem(fields.at(48)));
                                  y49.append(fields.at(49).toDouble());//7dx1A_uk14
                                   m_ui->logfilewidget->setItem(l,49, new QTableWidgetItem(fields.at(49)));
                                  y50.append(fields.at(50).toDouble());//7dx1B_uk15
                                   m_ui->logfilewidget->setItem(l,50, new QTableWidgetItem(fields.at(50)));
                                  y51.append(fields.at(51).toDouble());//7dx1C_uk16
                                   m_ui->logfilewidget->setItem(l,51, new QTableWidgetItem(fields.at(51)));
                                  y52.append(fields.at(52).toDouble());//7dx1D_uk17
                                   m_ui->logfilewidget->setItem(l,52, new QTableWidgetItem(fields.at(52)));
                                  y53.append(fields.at(53).toDouble());//7dx1E_uk18
                                   m_ui->logfilewidget->setItem(l,53, new QTableWidgetItem(fields.at(53)));
                                  y54.append(fields.at(54).toDouble());//7dx1F_uk19
                                   m_ui->logfilewidget->setItem(l,54, new QTableWidgetItem(fields.at(54)));


                                  line_count++;

                              }


                      file.close();
                  }
    // *********** mems-gauge-old**und memsfcr.csv******************************

  else if (Firstcharacters=="#time,")
                  {  QStringList fields = Firstline.split(',');
                      Feld1.append(fields.at(0));
                      int m=fields.count();



                      qDebug()<<"Feld1,Zeile 1968:"<<Feld1;
                      qDebug()<<"felder,Zeile1969"<<m;
                      qDebug()<<"Zeile 1970 fields"<<fields;

                         m_ui->logfilewidget->clear();
                         m_ui->logfilewidget->horizontalHeader()->isVisible();
                         m_ui->logfilewidget->setColumnCount(m);
                            for (int i = 0; i < m; ++i)
                                {

                                m_ui->logfilewidget->setHorizontalHeaderItem(i,new QTableWidgetItem(fields.at(i)));
                                }
                            int l=1;
                              if (m==11)
                                  {
                                       type="mems-gauge-old";
                                        qDebug()<<"zeile 1984:"<<type;
                                      m_ui->label_type->setText(type);
                                      // m_ui->label_ecuid->setText("");

                                      while(!in.atEnd())
                                      {
                                         QString line = in.readLine();// read first line and so on
                                          qDebug() <<"1991"<< line;
                                          qDebug() << "Zeile1992:"<<line_count;
                                          QStringList fields = line.split(',');
                                         // int l=line_count-3;
                                          m_ui->logfilewidget->insertRow(l);
                                          if (line_count > 3)
                                           x.append(QTime::fromString(fields.at(0),"hh:mm:ss").msecsSinceStartOfDay()/1000.0);//time
                                           m_ui->logfilewidget->setItem(l, 0,new QTableWidgetItem(fields.at(0)));
                                          // take first value and stored into x
                                          // x.append(tick);

                                          //   tick++;
                                           y1.append(fields.at(1).toDouble()); // rpm
                                            m_ui->logfilewidget->setItem(l, 0,new QTableWidgetItem(fields.at(0)));
                                           y54.append(fields.at(1).toDouble());//rpm<1500


                                           y2.append(fields.at(2).toDouble());//watertemp
                                            m_ui->logfilewidget->setItem(l, 2,new QTableWidgetItem(fields.at(2)));

                                           y4.append(fields.at(3).toDouble());//intake air
                                            m_ui->logfilewidget->setItem(l, 3,new QTableWidgetItem(fields.at(3)));

                                           y8.append(fields.at(4).toDouble());//throttle pot
                                            m_ui->logfilewidget->setItem(l, 4,new QTableWidgetItem(fields.at(4)));
                                           y6.append(fields.at(5).toDouble());//map
                                            m_ui->logfilewidget->setItem(l, 5,new QTableWidgetItem(fields.at(5)));
                                           y16.append(fields.at(6).toDouble());//AIC
                                            m_ui->logfilewidget->setItem(l, 6,new QTableWidgetItem(fields.at(6)));
                                           y7.append(fields.at(7).toDouble());//battery
                                            m_ui->logfilewidget->setItem(l, 7,new QTableWidgetItem(fields.at(7)));
                                           y9.append(fields.at(8).toDouble());//idleswitch
                                            m_ui->logfilewidget->setItem(l, 8,new QTableWidgetItem(fields.at(8)));
                                           y33.append(fields.at(9).toDouble());//closed loop
                                            m_ui->logfilewidget->setItem(l, 9,new QTableWidgetItem(fields.at(9)));
                                           y29.append(fields.at(10).toDouble());//lambda voltage
                                            m_ui->logfilewidget->setItem(l,10,new QTableWidgetItem(fields.at(10)));
                                          l++;

                                      }
                                      file.close();

}

                         else if (m==81)//if(m==57)old version

                              {
                              type="memsfcr";
                              qDebug()<<"Zeile2039"<<type;
                              m_ui->label_type->setText(type);
                                  int lstatus;
                                  int closedl;
                                  int l=0;
                                  while(!in.atEnd())
                                      {
                                      l=line_count;
                                      QString line = in.readLine();
                                      qDebug() << "Zeile2047:"<<l <<"::"<<line;
                                      QStringList fields = line.split(',');

                                      m_ui->logfilewidget->insertRow(l);

                                      QString item = fields.at(0);
                                       qDebug()<<"Zeile2053"<<item;

                                     item = item.right(12);
                                     item = item.left(8);
                                        qDebug()<<"Zeile2057item"<<item;
                                      // QTime Zeit=item;
                                x.append(QTime::fromString(item,"hh:mm:ss").msecsSinceStartOfDay()/1000.0);

                                            m_ui->logfilewidget->setItem(l, 0,new QTableWidgetItem(fields.at(0)));
                                     qDebug()<<"Zeile2062Zeit"<<x;


                                                y1.append(fields.at(1).toDouble()); // 80x01-02_engine-rpm

                                                  m_ui->logfilewidget->setItem(l,1, new QTableWidgetItem(fields.at(1)));
                                                y55.append(fields.at(1).toDouble());//rpm<1500

                                                y2.append(fields.at(2).toDouble());//80x03_coolant_temp
                                                m_ui->logfilewidget->setItem(l,2, new QTableWidgetItem(fields.at(2)));
                                                y3.append(fields.at(3).toDouble());//80x04_ambient_temp
                                                m_ui->logfilewidget->setItem(l,3, new QTableWidgetItem(fields.at(3)));
                                                y4.append(fields.at(4).toDouble());//80x05_intake_air_temp
                                                 m_ui->logfilewidget->setItem(l,4, new QTableWidgetItem(fields.at(4)));
                                                y5.append(fields.at(5).toDouble());//80x06_fuel_temp
                                                 m_ui->logfilewidget->setItem(l,5, new QTableWidgetItem(fields.at(5)));
                                                y6.append(fields.at(6).toDouble());//80x07_map_kpa
                                                 m_ui->logfilewidget->setItem(l,6, new QTableWidgetItem(fields.at(6)));
                                                y7.append(fields.at(7).toDouble());//80x08_battery_voltage
                                                 m_ui->logfilewidget->setItem(l,7, new QTableWidgetItem(fields.at(7)));
                                                y8.append(fields.at(8).toDouble());//80x09_throttle_pot
                                                 m_ui->logfilewidget->setItem(l,8, new QTableWidgetItem(fields.at(8)));
                                                 int is=(fields.at(9).toDouble());//80x0A_idle_switch
                                                is=is*10;
                                                y9.append(is);
                                                 m_ui->logfilewidget->setItem(l,9, new QTableWidgetItem(fields.at(9)));
                                                y10.append(fields.at(10).toDouble());//80x0B_uk1
                                                 m_ui->logfilewidget->setItem(l,10, new QTableWidgetItem(fields.at(10)));
                                                y11.append(fields.at(11).toDouble()); // 80x0C_park_neutral_switch
                                                 m_ui->logfilewidget->setItem(l,11, new QTableWidgetItem(fields.at(11)));
                                                y12.append(fields.at(12).toDouble()); // 80x0D-0E_fault_codes
                                                 m_ui->logfilewidget->setItem(l,12, new QTableWidgetItem(fields.at(12)));
                                                y13.append(fields.at(13).toDouble());//80x0F_idle_set_point
                                                 m_ui->logfilewidget->setItem(l,13, new QTableWidgetItem(fields.at(13)));
                                                y14.append(fields.at(14).toDouble());//80x10_idle_hot
                                                 m_ui->logfilewidget->setItem(l,14, new QTableWidgetItem(fields.at(14)));
                                                y15.append(fields.at(15).toDouble());//80x11_uk2
                                                 m_ui->logfilewidget->setItem(l,15, new QTableWidgetItem(fields.at(15)));
                                                y16.append(fields.at(16).toDouble());//80x12_iac_position
                                                 m_ui->logfilewidget->setItem(l,16, new QTableWidgetItem(fields.at(16)));
                                                y17.append(fields.at(17).toDouble());//80x13-14_idle_error
                                                 m_ui->logfilewidget->setItem(l,17, new QTableWidgetItem(fields.at(17)));
                                                y18.append(fields.at(18).toDouble());//80x15_ignition_advance_offset
                                                 m_ui->logfilewidget->setItem(l,18, new QTableWidgetItem(fields.at(18)));
                                                y19.append(fields.at(19).toDouble());// 80x16_ignition_advance
                                                 m_ui->logfilewidget->setItem(l,19, new QTableWidgetItem(fields.at(19)));
                                                y20.append(fields.at(20).toDouble());//80x17-18_coil_time
                                                 m_ui->logfilewidget->setItem(l,20, new QTableWidgetItem(fields.at(20)));
                                                y21.append(fields.at(21).toDouble()); //80x19_crankshaft_position_sensor
                                                 m_ui->logfilewidget->setItem(l,21, new QTableWidgetItem(fields.at(21)));
                                                y22.append(fields.at(22).toDouble()); //80x1A_uk4
                                                 m_ui->logfilewidget->setItem(l,22, new QTableWidgetItem(fields.at(22)));
                                                y23.append(fields.at(23).toDouble());//80x1B_uk5
                                                 m_ui->logfilewidget->setItem(l,23, new QTableWidgetItem(fields.at(23)));
                                                y24.append(fields.at(24).toDouble());//7dx01_ignition_switch
                                                 m_ui->logfilewidget->setItem(l,24, new QTableWidgetItem(fields.at(24)));
                                                y25.append(fields.at(25).toDouble());//7dx02_throttle_angle
                                                 m_ui->logfilewidget->setItem(l,25, new QTableWidgetItem(fields.at(25)));
                                                y26.append(fields.at(26).toDouble());//7dx03_uk6
                                                 m_ui->logfilewidget->setItem(l,26, new QTableWidgetItem(fields.at(26)));
                                                y27.append(fields.at(27).toDouble());//7dx04_air_fuel_ratio
                                                 m_ui->logfilewidget->setItem(l,27, new QTableWidgetItem(fields.at(27)));
                                                y28.append(fields.at(28).toDouble());//7dx05_dtc2
                                                 m_ui->logfilewidget->setItem(l,28, new QTableWidgetItem(fields.at(28)));
                                                y29.append(fields.at(29).toDouble());//7dx06_lambda_voltage
                                                 m_ui->logfilewidget->setItem(l,29, new QTableWidgetItem(fields.at(29)));
                                                y30.append(fields.at(30).toDouble());//7dx07_lambda_sensor_frequency
                                                 m_ui->logfilewidget->setItem(l,30, new QTableWidgetItem(fields.at(30)));
                                                y31.append(fields.at(31).toDouble()); //7dx08_lambda_sensor_dutycycle
                                                 m_ui->logfilewidget->setItem(l,31, new QTableWidgetItem(fields.at(31)));
                                                lstatus=(fields.at(32).toDouble()); // 7dx09_lambda_sensor_status
                                                lstatus=lstatus*10;
                                                 y32.append(lstatus);
                                                 m_ui->logfilewidget->setItem(l,32, new QTableWidgetItem(fields.at(32)));
                                                 closedl=(fields.at(33).toDouble());// 7dx0A_closed_loop
                                                 closedl=closedl*10;
                                                 y33.append(closedl);
                                                 m_ui->logfilewidget->setItem(l,33, new QTableWidgetItem(fields.at(33)));
                                                y34.append(fields.at(34).toDouble());//7dx0B_long_term_fuel_trim
                                                 m_ui->logfilewidget->setItem(l,34, new QTableWidgetItem(fields.at(34)));
                                                y35.append(fields.at(35).toDouble());// 7dx0C_short_term_fuel_trim
                                                 m_ui->logfilewidget->setItem(l,35, new QTableWidgetItem(fields.at(35)));
                                                y36.append(fields.at(36).toDouble()); //7dx0D_carbon_canister_dutycycle
                                                 m_ui->logfilewidget->setItem(l,36, new QTableWidgetItem(fields.at(36)));
                                                y37.append(fields.at(37).toDouble());//7dx0E_dtc3
                                                 m_ui->logfilewidget->setItem(l,37, new QTableWidgetItem(fields.at(37)));
                                                y38.append(fields.at(38).toDouble());//7dx0F_idle_base_pos
                                                 m_ui->logfilewidget->setItem(l,38, new QTableWidgetItem(fields.at(38)));
                                                y39.append(fields.at(39).toDouble()); //7dx10_uk7
                                                 m_ui->logfilewidget->setItem(l,39, new QTableWidgetItem(fields.at(39)));
                                                y40.append(fields.at(40).toDouble());//7dx11_dtc4
                                                 m_ui->logfilewidget->setItem(l,40, new QTableWidgetItem(fields.at(40)));
                                                y41.append(fields.at(41).toDouble()); //7dx12_ignition_advance2
                                                 m_ui->logfilewidget->setItem(l,41, new QTableWidgetItem(fields.at(41)));
                                                y42.append(fields.at(42).toDouble()); //7dx13_idle_speed_offset
                                                 m_ui->logfilewidget->setItem(l,42, new QTableWidgetItem(fields.at(42)));
                                                y43.append(fields.at(43).toDouble());//7dx14_idle_error2
                                                 m_ui->logfilewidget->setItem(l,43, new QTableWidgetItem(fields.at(43)));
                                                y44.append(fields.at(44).toDouble());//7dx14-15_uk10
                                                 m_ui->logfilewidget->setItem(l,44, new QTableWidgetItem(fields.at(44)));
                                                y45.append(fields.at(45).toDouble());//7dx16_dtc5
                                                 m_ui->logfilewidget->setItem(l,45, new QTableWidgetItem(fields.at(45)));
                                                y46.append(fields.at(46).toDouble());//7dx17_uk11
                                                 m_ui->logfilewidget->setItem(l,46, new QTableWidgetItem(fields.at(46)));
                                                y47.append(fields.at(47).toDouble());//7dx18_uk12
                                                 m_ui->logfilewidget->setItem(l,47, new QTableWidgetItem(fields.at(47)));
                                                y48.append(fields.at(48).toDouble()); //7dx19_uk13
                                                 m_ui->logfilewidget->setItem(l,48, new QTableWidgetItem(fields.at(48)));
                                                y49.append(fields.at(49).toDouble());//7dx1A_uk14
                                                 m_ui->logfilewidget->setItem(l,49, new QTableWidgetItem(fields.at(49)));
                                                y50.append(fields.at(50).toDouble());//7dx1B_uk15
                                                 m_ui->logfilewidget->setItem(l,50, new QTableWidgetItem(fields.at(50)));
                                                y51.append(fields.at(51).toDouble());//7dx1C_uk16
                                                 m_ui->logfilewidget->setItem(l,51, new QTableWidgetItem(fields.at(51)));
                                                y52.append(fields.at(52).toDouble());//7dx1D_uk17
                                                 m_ui->logfilewidget->setItem(l,52, new QTableWidgetItem(fields.at(52)));
                                                y53.append(fields.at(53).toDouble());//7dx1E_uk18
                                                 m_ui->logfilewidget->setItem(l,53, new QTableWidgetItem(fields.at(53)));
                                                y54.append(fields.at(54).toDouble());//7dx1F_uk19
                                                 m_ui->logfilewidget->setItem(l,54, new QTableWidgetItem(fields.at(54)));
                                                 m_ui->logfilewidget->setItem(l,55, new QTableWidgetItem(fields.at(55)));
                                                 m_ui->logfilewidget->setItem(l,56, new QTableWidgetItem(fields.at(56)));
                                                 m_ui->logfilewidget->setItem(l,57, new QTableWidgetItem(fields.at(57)));
                                                 m_ui->logfilewidget->setItem(l,58, new QTableWidgetItem(fields.at(58)));
                                                 m_ui->logfilewidget->setItem(l,59, new QTableWidgetItem(fields.at(59)));
                                                 m_ui->logfilewidget->setItem(l,60, new QTableWidgetItem(fields.at(60)));
                                                 m_ui->logfilewidget->setItem(l,61, new QTableWidgetItem(fields.at(61)));
                                                 m_ui->logfilewidget->setItem(l,62, new QTableWidgetItem(fields.at(62)));
                                                 m_ui->logfilewidget->setItem(l,63, new QTableWidgetItem(fields.at(63)));
                                                 m_ui->logfilewidget->setItem(l,64, new QTableWidgetItem(fields.at(64)));
                                                 m_ui->logfilewidget->setItem(l,65, new QTableWidgetItem(fields.at(65)));
                                                 m_ui->logfilewidget->setItem(l,66, new QTableWidgetItem(fields.at(66)));
                                                 m_ui->logfilewidget->setItem(l,67, new QTableWidgetItem(fields.at(67)));
                                                 m_ui->logfilewidget->setItem(l,68, new QTableWidgetItem(fields.at(68)));
                                                 m_ui->logfilewidget->setItem(l,69, new QTableWidgetItem(fields.at(69)));
                                                 m_ui->logfilewidget->setItem(l,70, new QTableWidgetItem(fields.at(70)));
                                                 m_ui->logfilewidget->setItem(l,71, new QTableWidgetItem(fields.at(71)));
                                                 m_ui->logfilewidget->setItem(l,72, new QTableWidgetItem(fields.at(72)));
                                                 m_ui->logfilewidget->setItem(l,73, new QTableWidgetItem(fields.at(73)));
                                                 m_ui->logfilewidget->setItem(l,74, new QTableWidgetItem(fields.at(74)));
                                                 m_ui->logfilewidget->setItem(l,75, new QTableWidgetItem(fields.at(75)));
                                                 m_ui->logfilewidget->setItem(l,76, new QTableWidgetItem(fields.at(76)));
                                                 m_ui->logfilewidget->setItem(l,77, new QTableWidgetItem(fields.at(77)));
                                                 m_ui->logfilewidget->setItem(l,78, new QTableWidgetItem(fields.at(78)));
                                                 m_ui->logfilewidget->setItem(l,79, new QTableWidgetItem(fields.at(79)));
                                                 m_ui->logfilewidget->setItem(l,80, new QTableWidgetItem(fields.at(80)));
                                               //  m_ui->logfilewidget->setItem(l,81, new QTableWidgetItem(fields.at(81)));

                                               line_count++;

                                          }
                                  file.close();

}
                          else if(m==57)

                                                            {
                                                            type="memsfcr";
                                                            qDebug()<<"Zeile2220"<<type;
                                                            m_ui->label_type->setText(type);
                              tick=1;
                                                                while(!in.atEnd())
                                                                    {
                                                                    l=line_count;
                                                                    QString line = in.readLine();
                                                                    qDebug() << "Zeile2226:"<<line_count <<"::"<<line;
                                                                    QStringList fields = line.split(',');

                                                                    m_ui->logfilewidget->insertRow(l);
                                                                   // x.append(tick);
                                                                   // tick++;
                                                                    QString item = fields.at(0);
                                                                     qDebug()<<"Zeile2234"<<item;

                                                                   item = item.left(8);
                                                                      qDebug()<<"Zeile2237item"<<item;
                                                                    // QTime Zeit=item;
                                                                   //x.append(QTime::fromString(item,"hh:mm:ss").msecsSinceStartOfDay()/1000.0);
                                                                  qDebug()<<"Zeile2240"<<z;
                                                                            x.append(QTime::fromString(fields.at(0),"hh:mm:ss").msecsSinceStartOfDay()/1000.0);//time
                                                                             m_ui->logfilewidget->setItem(l, 0,new QTableWidgetItem(fields.at(0)));

                                                                              qDebug()<<"Zeile2244"<<x;


                                                                              y1.append(fields.at(1).toDouble()); // 80x01-02_engine-rpm
                                                                                m_ui->logfilewidget->setItem(l,1, new QTableWidgetItem(fields.at(1)));
                                                                              y55.append(fields.at(1).toDouble());//rpm<1500

                                                                              y2.append(fields.at(2).toDouble());//80x03_coolant_temp
                                                                              m_ui->logfilewidget->setItem(l,2, new QTableWidgetItem(fields.at(2)));
                                                                              y3.append(fields.at(3).toDouble());//80x04_ambient_temp
                                                                              m_ui->logfilewidget->setItem(l,3, new QTableWidgetItem(fields.at(3)));
                                                                              y4.append(fields.at(4).toDouble());//80x05_intake_air_temp
                                                                               m_ui->logfilewidget->setItem(l,4, new QTableWidgetItem(fields.at(4)));
                                                                              y5.append(fields.at(5).toDouble());//80x06_fuel_temp
                                                                               m_ui->logfilewidget->setItem(l,5, new QTableWidgetItem(fields.at(5)));
                                                                              y6.append(fields.at(6).toDouble());//80x07_map_kpa
                                                                               m_ui->logfilewidget->setItem(l,6, new QTableWidgetItem(fields.at(6)));
                                                                              y7.append(fields.at(7).toDouble());//80x08_battery_voltage
                                                                               m_ui->logfilewidget->setItem(l,7, new QTableWidgetItem(fields.at(7)));
                                                                              y8.append(fields.at(8).toDouble());//80x09_throttle_pot
                                                                               m_ui->logfilewidget->setItem(l,8, new QTableWidgetItem(fields.at(8)));
                                                                               int is=(fields.at(9).toDouble());//80x0A_idle_switch
                                                                              is=is*10;
                                                                              y9.append(is);
                                                                               m_ui->logfilewidget->setItem(l,9, new QTableWidgetItem(fields.at(9)));
                                                                              y10.append(fields.at(10).toDouble());//80x0B_uk1
                                                                               m_ui->logfilewidget->setItem(l,10, new QTableWidgetItem(fields.at(10)));
                                                                              y11.append(fields.at(11).toDouble()); // 80x0C_park_neutral_switch
                                                                               m_ui->logfilewidget->setItem(l,11, new QTableWidgetItem(fields.at(11)));
                                                                              y12.append(fields.at(12).toDouble()); // 80x0D-0E_fault_codes
                                                                               m_ui->logfilewidget->setItem(l,12, new QTableWidgetItem(fields.at(12)));
                                                                              y13.append(fields.at(13).toDouble());//80x0F_idle_set_point
                                                                               m_ui->logfilewidget->setItem(l,13, new QTableWidgetItem(fields.at(13)));
                                                                              y14.append(fields.at(14).toDouble());//80x10_idle_hot
                                                                               m_ui->logfilewidget->setItem(l,14, new QTableWidgetItem(fields.at(14)));
                                                                              y15.append(fields.at(15).toDouble());//80x11_uk2
                                                                               m_ui->logfilewidget->setItem(l,15, new QTableWidgetItem(fields.at(15)));
                                                                              y16.append(fields.at(16).toDouble());//80x12_iac_position
                                                                               m_ui->logfilewidget->setItem(l,16, new QTableWidgetItem(fields.at(16)));
                                                                              y17.append(fields.at(17).toDouble());//80x13-14_idle_error
                                                                               m_ui->logfilewidget->setItem(l,17, new QTableWidgetItem(fields.at(17)));
                                                                              y18.append(fields.at(18).toDouble());//80x15_ignition_advance_offset
                                                                               m_ui->logfilewidget->setItem(l,18, new QTableWidgetItem(fields.at(18)));
                                                                              y19.append(fields.at(19).toDouble());// 80x16_ignition_advance
                                                                               m_ui->logfilewidget->setItem(l,19, new QTableWidgetItem(fields.at(19)));
                                                                              y20.append(fields.at(20).toDouble());//80x17-18_coil_time
                                                                               m_ui->logfilewidget->setItem(l,20, new QTableWidgetItem(fields.at(20)));
                                                                              y21.append(fields.at(21).toDouble()); //80x19_crankshaft_position_sensor
                                                                               m_ui->logfilewidget->setItem(l,21, new QTableWidgetItem(fields.at(21)));
                                                                              y22.append(fields.at(22).toDouble()); //80x1A_uk4
                                                                               m_ui->logfilewidget->setItem(l,22, new QTableWidgetItem(fields.at(22)));
                                                                              y23.append(fields.at(23).toDouble());//80x1B_uk5
                                                                               m_ui->logfilewidget->setItem(l,23, new QTableWidgetItem(fields.at(23)));
                                                                              y24.append(fields.at(24).toDouble());//7dx01_ignition_switch
                                                                               m_ui->logfilewidget->setItem(l,24, new QTableWidgetItem(fields.at(24)));
                                                                              y25.append(fields.at(25).toDouble());//7dx02_throttle_angle
                                                                               m_ui->logfilewidget->setItem(l,25, new QTableWidgetItem(fields.at(25)));
                                                                              y26.append(fields.at(26).toDouble());//7dx03_uk6
                                                                               m_ui->logfilewidget->setItem(l,26, new QTableWidgetItem(fields.at(26)));
                                                                              y27.append(fields.at(27).toDouble());//7dx04_air_fuel_ratio
                                                                               m_ui->logfilewidget->setItem(l,27, new QTableWidgetItem(fields.at(27)));
                                                                              y28.append(fields.at(28).toDouble());//7dx05_dtc2
                                                                               m_ui->logfilewidget->setItem(l,28, new QTableWidgetItem(fields.at(28)));
                                                                              y29.append(fields.at(29).toDouble());//7dx06_lambda_voltage
                                                                               m_ui->logfilewidget->setItem(l,29, new QTableWidgetItem(fields.at(29)));
                                                                              y30.append(fields.at(30).toDouble());//7dx07_lambda_sensor_frequency
                                                                               m_ui->logfilewidget->setItem(l,30, new QTableWidgetItem(fields.at(30)));
                                                                              y31.append(fields.at(31).toDouble()); //7dx08_lambda_sensor_dutycycle
                                                                               m_ui->logfilewidget->setItem(l,31, new QTableWidgetItem(fields.at(31)));
                                                                              y32.append(fields.at(32).toDouble()); // 7dx09_lambda_sensor_status
                                                                               m_ui->logfilewidget->setItem(l,32, new QTableWidgetItem(fields.at(32)));
                                                                              y33.append(fields.at(33).toDouble());// 7dx0A_closed_loop
                                                                               m_ui->logfilewidget->setItem(l,33, new QTableWidgetItem(fields.at(33)));
                                                                              y34.append(fields.at(34).toDouble());//7dx0B_long_term_fuel_trim
                                                                               m_ui->logfilewidget->setItem(l,34, new QTableWidgetItem(fields.at(34)));
                                                                              y35.append(fields.at(35).toDouble());// 7dx0C_short_term_fuel_trim
                                                                               m_ui->logfilewidget->setItem(l,35, new QTableWidgetItem(fields.at(35)));
                                                                              y36.append(fields.at(36).toDouble()); //7dx0D_carbon_canister_dutycycle
                                                                               m_ui->logfilewidget->setItem(l,36, new QTableWidgetItem(fields.at(36)));
                                                                              y37.append(fields.at(37).toDouble());//7dx0E_dtc3
                                                                               m_ui->logfilewidget->setItem(l,37, new QTableWidgetItem(fields.at(37)));
                                                                              y38.append(fields.at(38).toDouble());//7dx0F_idle_base_pos
                                                                               m_ui->logfilewidget->setItem(l,38, new QTableWidgetItem(fields.at(38)));
                                                                              y39.append(fields.at(39).toDouble()); //7dx10_uk7
                                                                               m_ui->logfilewidget->setItem(l,39, new QTableWidgetItem(fields.at(39)));
                                                                              y40.append(fields.at(40).toDouble());//7dx11_dtc4
                                                                               m_ui->logfilewidget->setItem(l,40, new QTableWidgetItem(fields.at(40)));
                                                                              y41.append(fields.at(41).toDouble()); //7dx12_ignition_advance2
                                                                               m_ui->logfilewidget->setItem(l,41, new QTableWidgetItem(fields.at(41)));
                                                                              y42.append(fields.at(42).toDouble()); //7dx13_idle_speed_offset
                                                                               m_ui->logfilewidget->setItem(l,42, new QTableWidgetItem(fields.at(42)));
                                                                              y43.append(fields.at(43).toDouble());//7dx14_idle_error2
                                                                               m_ui->logfilewidget->setItem(l,43, new QTableWidgetItem(fields.at(43)));
                                                                              y44.append(fields.at(44).toDouble());//7dx14-15_uk10
                                                                               m_ui->logfilewidget->setItem(l,44, new QTableWidgetItem(fields.at(44)));
                                                                              y45.append(fields.at(45).toDouble());//7dx16_dtc5
                                                                               m_ui->logfilewidget->setItem(l,45, new QTableWidgetItem(fields.at(45)));
                                                                              y46.append(fields.at(46).toDouble());//7dx17_uk11
                                                                               m_ui->logfilewidget->setItem(l,46, new QTableWidgetItem(fields.at(46)));
                                                                              y47.append(fields.at(47).toDouble());//7dx18_uk12
                                                                               m_ui->logfilewidget->setItem(l,47, new QTableWidgetItem(fields.at(47)));
                                                                              y48.append(fields.at(48).toDouble()); //7dx19_uk13
                                                                               m_ui->logfilewidget->setItem(l,48, new QTableWidgetItem(fields.at(48)));
                                                                              y49.append(fields.at(49).toDouble());//7dx1A_uk14
                                                                               m_ui->logfilewidget->setItem(l,49, new QTableWidgetItem(fields.at(49)));
                                                                              y50.append(fields.at(50).toDouble());//7dx1B_uk15
                                                                               m_ui->logfilewidget->setItem(l,50, new QTableWidgetItem(fields.at(50)));
                                                                              y51.append(fields.at(51).toDouble());//7dx1C_uk16
                                                                               m_ui->logfilewidget->setItem(l,51, new QTableWidgetItem(fields.at(51)));
                                                                              y52.append(fields.at(52).toDouble());//7dx1D_uk17
                                                                               m_ui->logfilewidget->setItem(l,52, new QTableWidgetItem(fields.at(52)));
                                                                              y53.append(fields.at(53).toDouble());//7dx1E_uk18
                                                                               m_ui->logfilewidget->setItem(l,53, new QTableWidgetItem(fields.at(53)));
                                                                              y54.append(fields.at(54).toDouble());//7dx1F_uk19
                                                                               m_ui->logfilewidget->setItem(l,54, new QTableWidgetItem(fields.at(54)));
                                                                              // y55 vorbelegt mit rpm2
                                                                               y56.append(fields.at(55).toDouble());//7dx1F_uk19
                                                                                m_ui->logfilewidget->setItem(l,55, new QTableWidgetItem(fields.at(55)));
                                                                                y57.append(fields.at(56).toDouble());//7dx1F_uk19
                                                                                 m_ui->logfilewidget->setItem(l,56, new QTableWidgetItem(fields.at(56)));
                                                                              l++;
                                                                        }

                                                              file.close();
                                            }
                                else if (m==55)

                                     {
                                     type="mems-gauge";
                                     qDebug()<<"Zeile2372"<<type;
                                     m_ui->label_type->setText(type);
       tick=1;
                                         while(!in.atEnd())
                                             {
                                             l=line_count;
                                             QString line = in.readLine();
                                             qDebug() << "Zeile2379:"<<line_count <<"::"<<line;
                                             QStringList fields = line.split(',');

                                             m_ui->logfilewidget->insertRow(l);
                                            // x.append(tick);
                                            // tick++;
                                             QString item = fields.at(0);
                                              qDebug()<<"Zeile2386"<<item;

                                            item = item.left(8);
                                               qDebug()<<"Zeile2389item"<<item;
                                             // QTime Zeit=item;
                                            x.append(QTime::fromString(item,"hh:mm:ss").msecsSinceStartOfDay()/1000.0);
                                           qDebug()<<"Zeile2392"<<z;
                                                     // x.append(QTime::fromString(fields.at(0),"hh:mm:ss:ms").msecsSinceStartOfDay()/1000.0);//time
                                                      m_ui->logfilewidget->setItem(l, 0,new QTableWidgetItem(fields.at(0)));

                                                       qDebug()<<"Zeile2396"<<x;


                                                       y1.append(fields.at(1).toDouble()); // 80x01-02_engine-rpm
                                                         m_ui->logfilewidget->setItem(l,1, new QTableWidgetItem(fields.at(1)));
                                                       y55.append(fields.at(1).toDouble());//rpm<1500

                                                       y2.append(fields.at(2).toDouble());//80x03_coolant_temp
                                                       m_ui->logfilewidget->setItem(l,2, new QTableWidgetItem(fields.at(2)));
                                                       y3.append(fields.at(3).toDouble());//80x04_ambient_temp
                                                       m_ui->logfilewidget->setItem(l,3, new QTableWidgetItem(fields.at(3)));
                                                       y4.append(fields.at(4).toDouble());//80x05_intake_air_temp
                                                        m_ui->logfilewidget->setItem(l,4, new QTableWidgetItem(fields.at(4)));
                                                       y5.append(fields.at(5).toDouble());//80x06_fuel_temp
                                                        m_ui->logfilewidget->setItem(l,5, new QTableWidgetItem(fields.at(5)));
                                                       y6.append(fields.at(6).toDouble());//80x07_map_kpa
                                                        m_ui->logfilewidget->setItem(l,6, new QTableWidgetItem(fields.at(6)));
                                                       y7.append(fields.at(7).toDouble());//80x08_battery_voltage
                                                        m_ui->logfilewidget->setItem(l,7, new QTableWidgetItem(fields.at(7)));
                                                       y8.append(fields.at(8).toDouble());//80x09_throttle_pot
                                                        m_ui->logfilewidget->setItem(l,8, new QTableWidgetItem(fields.at(8)));
                                                       y9.append(fields.at(9).toDouble());//80x0A_idle_switch
                                                        m_ui->logfilewidget->setItem(l,9, new QTableWidgetItem(fields.at(9)));
                                                       y10.append(fields.at(10).toDouble());//80x0B_uk1
                                                        m_ui->logfilewidget->setItem(l,10, new QTableWidgetItem(fields.at(10)));
                                                       y11.append(fields.at(11).toDouble()); // 80x0C_park_neutral_switch
                                                        m_ui->logfilewidget->setItem(l,11, new QTableWidgetItem(fields.at(11)));
                                                       y12.append(fields.at(12).toDouble()); // 80x0D-0E_fault_codes
                                                        m_ui->logfilewidget->setItem(l,12, new QTableWidgetItem(fields.at(12)));
                                                       y13.append(fields.at(13).toDouble());//80x0F_idle_set_point
                                                        m_ui->logfilewidget->setItem(l,13, new QTableWidgetItem(fields.at(13)));
                                                       y14.append(fields.at(14).toDouble());//80x10_idle_hot
                                                        m_ui->logfilewidget->setItem(l,14, new QTableWidgetItem(fields.at(14)));
                                                       y15.append(fields.at(15).toDouble());//80x11_uk2
                                                        m_ui->logfilewidget->setItem(l,15, new QTableWidgetItem(fields.at(15)));
                                                       y16.append(fields.at(16).toDouble());//80x12_iac_position
                                                        m_ui->logfilewidget->setItem(l,16, new QTableWidgetItem(fields.at(16)));
                                                       y17.append(fields.at(17).toDouble());//80x13-14_idle_error
                                                        m_ui->logfilewidget->setItem(l,17, new QTableWidgetItem(fields.at(17)));
                                                       y18.append(fields.at(18).toDouble());//80x15_ignition_advance_offset
                                                        m_ui->logfilewidget->setItem(l,18, new QTableWidgetItem(fields.at(18)));
                                                       y19.append(fields.at(19).toDouble());// 80x16_ignition_advance
                                                        m_ui->logfilewidget->setItem(l,19, new QTableWidgetItem(fields.at(19)));
                                                       y20.append(fields.at(20).toDouble());//80x17-18_coil_time
                                                        m_ui->logfilewidget->setItem(l,20, new QTableWidgetItem(fields.at(20)));
                                                       y21.append(fields.at(21).toDouble()); //80x19_crankshaft_position_sensor
                                                        m_ui->logfilewidget->setItem(l,21, new QTableWidgetItem(fields.at(21)));
                                                       y22.append(fields.at(22).toDouble()); //80x1A_uk4
                                                        m_ui->logfilewidget->setItem(l,22, new QTableWidgetItem(fields.at(22)));
                                                       y23.append(fields.at(23).toDouble());//80x1B_uk5
                                                        m_ui->logfilewidget->setItem(l,23, new QTableWidgetItem(fields.at(23)));
                                                       y24.append(fields.at(24).toDouble());//7dx01_ignition_switch
                                                        m_ui->logfilewidget->setItem(l,24, new QTableWidgetItem(fields.at(24)));
                                                       y25.append(fields.at(25).toDouble());//7dx02_throttle_angle
                                                        m_ui->logfilewidget->setItem(l,25, new QTableWidgetItem(fields.at(25)));
                                                       y26.append(fields.at(26).toDouble());//7dx03_uk6
                                                        m_ui->logfilewidget->setItem(l,26, new QTableWidgetItem(fields.at(26)));
                                                       y27.append(fields.at(27).toDouble());//7dx04_air_fuel_ratio
                                                        m_ui->logfilewidget->setItem(l,27, new QTableWidgetItem(fields.at(27)));
                                                       y28.append(fields.at(28).toDouble());//7dx05_dtc2
                                                        m_ui->logfilewidget->setItem(l,28, new QTableWidgetItem(fields.at(28)));
                                                       y29.append(fields.at(29).toDouble());//7dx06_lambda_voltage
                                                        m_ui->logfilewidget->setItem(l,29, new QTableWidgetItem(fields.at(29)));
                                                       y30.append(fields.at(30).toDouble());//7dx07_lambda_sensor_frequency
                                                        m_ui->logfilewidget->setItem(l,30, new QTableWidgetItem(fields.at(30)));
                                                       y31.append(fields.at(31).toDouble()); //7dx08_lambda_sensor_dutycycle
                                                        m_ui->logfilewidget->setItem(l,31, new QTableWidgetItem(fields.at(31)));
                                                       y32.append(fields.at(32).toDouble()); // 7dx09_lambda_sensor_status
                                                        m_ui->logfilewidget->setItem(l,32, new QTableWidgetItem(fields.at(32)));
                                                       y33.append(fields.at(33).toDouble());// 7dx0A_closed_loop
                                                        m_ui->logfilewidget->setItem(l,33, new QTableWidgetItem(fields.at(33)));
                                                       y34.append(fields.at(34).toDouble());//7dx0B_long_term_fuel_trim
                                                        m_ui->logfilewidget->setItem(l,34, new QTableWidgetItem(fields.at(34)));
                                                       y35.append(fields.at(35).toDouble());// 7dx0C_short_term_fuel_trim
                                                        m_ui->logfilewidget->setItem(l,35, new QTableWidgetItem(fields.at(35)));
                                                       y36.append(fields.at(36).toDouble()); //7dx0D_carbon_canister_dutycycle
                                                        m_ui->logfilewidget->setItem(l,36, new QTableWidgetItem(fields.at(36)));
                                                       y37.append(fields.at(37).toDouble());//7dx0E_dtc3
                                                        m_ui->logfilewidget->setItem(l,37, new QTableWidgetItem(fields.at(37)));
                                                       y38.append(fields.at(38).toDouble());//7dx0F_idle_base_pos
                                                        m_ui->logfilewidget->setItem(l,38, new QTableWidgetItem(fields.at(38)));
                                                       y39.append(fields.at(39).toDouble()); //7dx10_uk7
                                                        m_ui->logfilewidget->setItem(l,39, new QTableWidgetItem(fields.at(39)));
                                                       y40.append(fields.at(40).toDouble());//7dx11_dtc4
                                                        m_ui->logfilewidget->setItem(l,40, new QTableWidgetItem(fields.at(40)));
                                                       y41.append(fields.at(41).toDouble()); //7dx12_ignition_advance2
                                                        m_ui->logfilewidget->setItem(l,41, new QTableWidgetItem(fields.at(41)));
                                                       y42.append(fields.at(42).toDouble()); //7dx13_idle_speed_offset
                                                        m_ui->logfilewidget->setItem(l,42, new QTableWidgetItem(fields.at(42)));
                                                       y43.append(fields.at(43).toDouble());//7dx14_idle_error2
                                                        m_ui->logfilewidget->setItem(l,43, new QTableWidgetItem(fields.at(43)));
                                                       y44.append(fields.at(44).toDouble());//7dx14-15_uk10
                                                        m_ui->logfilewidget->setItem(l,44, new QTableWidgetItem(fields.at(44)));
                                                       y45.append(fields.at(45).toDouble());//7dx16_dtc5
                                                        m_ui->logfilewidget->setItem(l,45, new QTableWidgetItem(fields.at(45)));
                                                       y46.append(fields.at(46).toDouble());//7dx17_uk11
                                                        m_ui->logfilewidget->setItem(l,46, new QTableWidgetItem(fields.at(46)));
                                                       y47.append(fields.at(47).toDouble());//7dx18_uk12
                                                        m_ui->logfilewidget->setItem(l,47, new QTableWidgetItem(fields.at(47)));
                                                       y48.append(fields.at(48).toDouble()); //7dx19_uk13
                                                        m_ui->logfilewidget->setItem(l,48, new QTableWidgetItem(fields.at(48)));
                                                       y49.append(fields.at(49).toDouble());//7dx1A_uk14
                                                        m_ui->logfilewidget->setItem(l,49, new QTableWidgetItem(fields.at(49)));
                                                       y50.append(fields.at(50).toDouble());//7dx1B_uk15
                                                        m_ui->logfilewidget->setItem(l,50, new QTableWidgetItem(fields.at(50)));
                                                       y51.append(fields.at(51).toDouble());//7dx1C_uk16
                                                        m_ui->logfilewidget->setItem(l,51, new QTableWidgetItem(fields.at(51)));
                                                       y52.append(fields.at(52).toDouble());//7dx1D_uk17
                                                        m_ui->logfilewidget->setItem(l,52, new QTableWidgetItem(fields.at(52)));
                                                       y53.append(fields.at(53).toDouble());//7dx1E_uk18
                                                        m_ui->logfilewidget->setItem(l,53, new QTableWidgetItem(fields.at(53)));
                                                       y54.append(fields.at(54).toDouble());//7dx1F_uk19
                                                        m_ui->logfilewidget->setItem(l,54, new QTableWidgetItem(fields.at(54)));
                                                       // y55 vorbelegt mit rpm2

                                                       l++;
                                                 }
                                         file.close();
                              }
                                         else if (m==41)

                                              {
                                              type="mfcr";
                                              qDebug()<<"Zeile2519"<<type;
                                              m_ui->label_type->setText(type);
                tick=1;
                                                  while(!in.atEnd())
                                                      {
                                                      l=line_count;
                                                      QString line = in.readLine();
                                                      qDebug() << "Zeile2526:"<<line_count <<"::"<<line;
                                                      QStringList fields = line.split(',');

                                                      m_ui->logfilewidget->insertRow(l);
                                                    //  x.append(tick);
                                                     tick++;
                                                      QString item = fields.at(0);
                                                       qDebug()<<"Zeile2532"<<item;

                                                     item = item.right(12);
                                                      qDebug()<<"Zeile2536item"<<item;
                                                     item = item.left(8);
                                                        qDebug()<<"Zeile2539item"<<item;

                                                  x.append(QTime::fromString(item,"hh:mm:ss").msecsSinceStartOfDay()/1000.0);
                                                        // x.append(QTime::fromString(item).msecsSinceStartOfDay()/1000.0);

                                                           //    x.append(QTime::fromString(fields.at(0),right(11)"hh:mm:ss:ms").msecsSinceStartOfDay()/1000.0);//time
                                                               m_ui->logfilewidget->setItem(l, 0,new QTableWidgetItem(fields.at(0)));

                                                                qDebug()<<"Zeile2546"<<x;


                                                                y1.append(fields.at(1).toDouble()); // 80x01-02_engine-rpm
                                                                  m_ui->logfilewidget->setItem(l,1, new QTableWidgetItem(fields.at(1)));
                                                                y55.append(fields.at(1).toDouble());//rpm<1500

                                                                y2.append(fields.at(2).toDouble());//80x03_coolant_temp
                                                                m_ui->logfilewidget->setItem(l,2, new QTableWidgetItem(fields.at(2)));
                                                                y3.append(fields.at(3).toDouble());//80x04_ambient_temp
                                                                m_ui->logfilewidget->setItem(l,3, new QTableWidgetItem(fields.at(3)));
                                                                y4.append(fields.at(4).toDouble());//80x05_intake_air_temp
                                                                 m_ui->logfilewidget->setItem(l,4, new QTableWidgetItem(fields.at(4)));
                                                                y5.append(fields.at(5).toDouble());//80x06_fuel_temp
                                                                 m_ui->logfilewidget->setItem(l,5, new QTableWidgetItem(fields.at(5)));
                                                                y6.append(fields.at(6).toDouble());//80x07_map_kpa
                                                                 m_ui->logfilewidget->setItem(l,6, new QTableWidgetItem(fields.at(6)));
                                                                y7.append(fields.at(7).toDouble());//80x08_battery_voltage
                                                                 m_ui->logfilewidget->setItem(l,7, new QTableWidgetItem(fields.at(7)));
                                                                y8.append(fields.at(8).toDouble());//80x09_throttle_pot
                                                                 m_ui->logfilewidget->setItem(l,8, new QTableWidgetItem(fields.at(8)));
                                                                y9.append(fields.at(9).toDouble());//80x0A_idle_switch
                                                                 m_ui->logfilewidget->setItem(l,9, new QTableWidgetItem(fields.at(9)));
                                                                y10.append(fields.at(10).toDouble());//80x0B_aircon
                                                                 m_ui->logfilewidget->setItem(l,10, new QTableWidgetItem(fields.at(10)));
                                                                y11.append(fields.at(11).toDouble()); // 80x0C_park_neutral_switch
                                                                 m_ui->logfilewidget->setItem(l,11, new QTableWidgetItem(fields.at(11)));
                                                        //      y12.append(fields.at(12).toDouble()); // 80x0D-0E_fault_codes
                                                        //         m_ui->logfilewidget->setItem(l,12, new QTableWidgetItem(fields.at(12)));
                                                                y13.append(fields.at(12).toDouble());//80x0F_idle_set_point
                                                                 m_ui->logfilewidget->setItem(l,12, new QTableWidgetItem(fields.at(12)));
                                                                y14.append(fields.at(13).toDouble());//80x10_idle_hot
                                                                 m_ui->logfilewidget->setItem(l,13, new QTableWidgetItem(fields.at(13)));
                                                       //         y15.append(fields.at(15).toDouble());//80x11_uk2
                                                       //          m_ui->logfilewidget->setItem(l,15, new QTableWidgetItem(fields.at(15)));
                                                                y16.append(fields.at(14).toDouble());//80x12_iac_position
                                                                 m_ui->logfilewidget->setItem(l,14, new QTableWidgetItem(fields.at(14)));
                                                                y17.append(fields.at(15).toDouble());//80x13-14_idle_error
                                                                 m_ui->logfilewidget->setItem(l,15, new QTableWidgetItem(fields.at(15)));
                                                                y18.append(fields.at(16).toDouble());//80x15_ignition_advance_offset
                                                                 m_ui->logfilewidget->setItem(l,16, new QTableWidgetItem(fields.at(16)));
                                                                y19.append(fields.at(17).toDouble());// 80x16_ignition_advance
                                                                 m_ui->logfilewidget->setItem(l,17, new QTableWidgetItem(fields.at(17)));
                                                                y20.append(fields.at(18).toDouble());//80x17-18_coil_time
                                                                 m_ui->logfilewidget->setItem(l,18, new QTableWidgetItem(fields.at(18)));
                                                                y21.append(fields.at(19).toDouble()); //80x19_crankshaft_position_sensor
                                                                m_ui->logfilewidget->setItem(l,19, new QTableWidgetItem(fields.at(19)));
                                                         //      y22.append(fields.at(22).toDouble()); //80x1A_uk4
                                                         //        m_ui->logfilewidget->setItem(l,22, new QTableWidgetItem(fields.at(22)));
                                                         //       y23.append(fields.at(23).toDouble());//80x1B_uk5
                                                         //        m_ui->logfilewidget->setItem(l,23, new QTableWidgetItem(fields.at(23)));
                                                                y24.append(fields.at(25).toDouble());//7dx01_ignition_switch
                                                                 m_ui->logfilewidget->setItem(l,25, new QTableWidgetItem(fields.at(25)));
                                                                y25.append(fields.at(26).toDouble());//7dx02_throttle_angle
                                                                 m_ui->logfilewidget->setItem(l,26, new QTableWidgetItem(fields.at(26)));
                                                         //       y26.append(fields.at(26).toDouble());//7dx03_uk6
                                                         //        m_ui->logfilewidget->setItem(l,26, new QTableWidgetItem(fields.at(26)));
                                                                y27.append(fields.at(27).toDouble());//7dx04_air_fuel_ratio
                                                                 m_ui->logfilewidget->setItem(l,27, new QTableWidgetItem(fields.at(27)));
                                                        //        y28.append(fields.at(28).toDouble());//7dx05_dtc2
                                                        //         m_ui->logfilewidget->setItem(l,28, new QTableWidgetItem(fields.at(28)));
                                                                y29.append(fields.at(28).toDouble());//7dx06_lambda_voltage
                                                                 m_ui->logfilewidget->setItem(l,28, new QTableWidgetItem(fields.at(28)));
                                                                y30.append(fields.at(29).toDouble());//7dx07_lambda_sensor_frequency
                                                                 m_ui->logfilewidget->setItem(l,29, new QTableWidgetItem(fields.at(29)));
                                                                y31.append(fields.at(30).toDouble()); //7dx08_lambda_sensor_dutycycle
                                                                 m_ui->logfilewidget->setItem(l,30, new QTableWidgetItem(fields.at(30)));
                                                                y32.append(fields.at(31).toDouble()); // 7dx09_lambda_sensor_status
                                                                 m_ui->logfilewidget->setItem(l,31, new QTableWidgetItem(fields.at(31)));
                                                                y33.append(fields.at(32).toDouble());// 7dx0A_closed_loop
                                                                 m_ui->logfilewidget->setItem(l,32, new QTableWidgetItem(fields.at(32)));
                                                                y34.append(fields.at(33).toDouble());//7dx0B_long_term_fuel_trim
                                                                 m_ui->logfilewidget->setItem(l,33, new QTableWidgetItem(fields.at(33)));
                                                                y35.append(fields.at(34).toDouble());// 7dx0C_short_term_fuel_trim
                                                                 m_ui->logfilewidget->setItem(l,34, new QTableWidgetItem(fields.at(34)));
                                                                y36.append(fields.at(35).toDouble()); //7dx0D_carbon_canister_dutycycle
                                                                 m_ui->logfilewidget->setItem(l,35, new QTableWidgetItem(fields.at(35)));
                                                             //   y37.append(fields.at(37).toDouble());//7dx0E_dtc3
                                                             //    m_ui->logfilewidget->setItem(l,37, new QTableWidgetItem(fields.at(37)));
                                                             //   y38.append(fields.at(38).toDouble());//7dx0F_idle_base_pos
                                                             //    m_ui->logfilewidget->setItem(l,38, new QTableWidgetItem(fields.at(38)));
                                                             //   y39.append(fields.at(39).toDouble()); //7dx10_uk7
                                                             //    m_ui->logfilewidget->setItem(l,39, new QTableWidgetItem(fields.at(39)));
                                                             //   y40.append(fields.at(40).toDouble());//7dx11_dtc4
                                                             //    m_ui->logfilewidget->setItem(l,40, new QTableWidgetItem(fields.at(40)));
                                                                y41.append(fields.at(36).toDouble()); //7dx12_ignition_advance2
                                                                 m_ui->logfilewidget->setItem(l,36, new QTableWidgetItem(fields.at(36)));
                                                                y42.append(fields.at(37).toDouble()); //7dx13_idle_speed_offset
                                                                 m_ui->logfilewidget->setItem(l,37, new QTableWidgetItem(fields.at(37)));
                                                           //     y43.append(fields.at(43).toDouble());//7dx14_idle_error2
                                                           //      m_ui->logfilewidget->setItem(l,43, new QTableWidgetItem(fields.at(43)));
                                                           //     y44.append(fields.at(44).toDouble());//7dx14-15_uk10
                                                           //      m_ui->logfilewidget->setItem(l,44, new QTableWidgetItem(fields.at(44)));
                                                           //     y45.append(fields.at(45).toDouble());//7dx16_dtc5
                                                           //      m_ui->logfilewidget->setItem(l,45, new QTableWidgetItem(fields.at(45)));
                                                           //     y46.append(fields.at(46).toDouble());//7dx17_uk11
                                                           //      m_ui->logfilewidget->setItem(l,46, new QTableWidgetItem(fields.at(46)));
                                                           //     y47.append(fields.at(47).toDouble());//7dx18_uk12
                                                           //      m_ui->logfilewidget->setItem(l,47, new QTableWidgetItem(fields.at(47)));
                                                           //     y48.append(fields.at(48).toDouble()); //7dx19_uk13
                                                           //      m_ui->logfilewidget->setItem(l,48, new QTableWidgetItem(fields.at(48)));
                                                           //     y49.append(fields.at(49).toDouble());//7dx1A_uk14
                                                           //      m_ui->logfilewidget->setItem(l,49, new QTableWidgetItem(fields.at(49)));
                                                           //     y50.append(fields.at(50).toDouble());//7dx1B_uk15
                                                           //      m_ui->logfilewidget->setItem(l,50, new QTableWidgetItem(fields.at(50)));
                                                           //     y51.append(fields.at(51).toDouble());//7dx1C_uk16
                                                           //      m_ui->logfilewidget->setItem(l,51, new QTableWidgetItem(fields.at(51)));
                                                           //     y52.append(fields.at(52).toDouble());//7dx1D_uk17
                                                           //      m_ui->logfilewidget->setItem(l,52, new QTableWidgetItem(fields.at(52)));
                                                           //     y53.append(fields.at(53).toDouble());//7dx1E_uk18
                                                           //      m_ui->logfilewidget->setItem(l,53, new QTableWidgetItem(fields.at(53)));
                                                               y54.append(fields.at(38).toDouble());//7dx1F_jack count
                                                                 m_ui->logfilewidget->setItem(l,38, new QTableWidgetItem(fields.at(38)));
                                                                // y55 vorbelegt mit rpm2

                                                                l++;
                                                          }
                                     }
                                       file.close();

					}	

//***********************************mems-diag jackson******ok*************************************
           else if (Firstcharacters=="limest")
                                  {

                                    //  QStringList fields = Firstline.split(QRegExp("\\s+"), QString::SkipEmptyParts);// oder ("\\t+")
                                    //   QStringList fields = Firstline.split(QRegExp(";"), QString::SkipEmptyParts);// oder ("\\t+")
                                        QStringList fields = Firstline.split(QRegExp(","), QString::SkipEmptyParts);// oder ("\\t+")
                                      Feld1.append(fields.at(0));
                                      int r=fields.count();
                                      qDebug()<<"Feld1,Zeile 2626:"<<Feld1;

                                      qDebug()<<"Zeile 2628 fields"<<fields;

                                        qDebug()<<"fields"<<r;
                                        m_ui->logfilewidget->clear();
                                        m_ui->logfilewidget->horizontalHeader()->isVisible();
                                        m_ui->logfilewidget->setColumnCount(r);
                                           for (int i = 0; i < r; ++i)
                                               {

                                               m_ui->logfilewidget->setHorizontalHeaderItem(i,new QTableWidgetItem(fields.at(i)));
                                               }
                                              //if (r==34)
                                                   if (r==46)
                                                  {
                                                      type="ps-scan";
                                                        qDebug()<<"felder,Zeile2643"<<r;
                                                      qDebug()<<"zeile 2644:"<<type;
                                                      m_ui->label_type->setText(type);
                                                      m_ui->label_ecuid->setText("some variables with ps-scan not availiable!");

                                                     qDebug() <<"Zeile2647, Zeile"<< r;
                                                     QString line = in.readLine();
                                                     tick=1;
                                                     QVector<double>(x).clear();

                                                     int l=line_count;
                                                     while(!in.atEnd())
                                                     {

                                                          m_ui->logfilewidget->insertRow(l);
                                                          line = in.readLine();// read first line and so on
                                                         qDebug() <<"Zeile2658"<< line;
                                                          qDebug() <<"Zeile2659"<< l;
                                                          //  QStringList fields = line.split("\\s+");
                                                          QStringList fields = line.split(QRegExp("\\t+"), QString::SkipEmptyParts);
                                                          qDebug() <<"Zeile2663"<< fields;
                                                        for (int i = 1; i < r; ++i)
                                                            {
                                                                 m_ui->logfilewidget->setItem(l+1, i,new QTableWidgetItem(fields.at(i)));
                                                            }
                                                                // x.append(QTime::fromString(fields.at(0),"hh:mm:ss").msecsSinceStartOfDay()/10000.0);//time
                                                                // take first value and stored into x

                                                                x.append(tick);
                                                                qDebug()<<"Zeile2624"<<"ticks"<< x;
                                                                tick++;
                                                                y1.append(fields.at(1).toDouble()); // rpm
                                                                m_ui->logfilewidget->setItem(1, 1,new QTableWidgetItem(fields.at(1)));
                                                                y54.append(fields.at(1).toDouble());//rpm<1500

                                                                y2.append(fields.at(2).toDouble());//watertemp

                                                                //y3.append("no log");//ambient air

                                                                //      double c=(fields.at(4).toDouble());
                                                                //     double d=(c-32)*55556/100000;
                                                                 y4.append(fields.at(3).toDouble());//intake air


                                                                 // y5.append(f);//fuel temp
                                                                 y6.append(fields.at(4).toDouble());//map
                                                                 y7.append(fields.at(5).toDouble());//battery
                                                                 y8.append(fields.at(6).toDouble());//throttle pot
                                                                 y9.append(fields.at(8).toDouble());//idle switch
                                                                // y10.append(fields.at(10).toDouble());//uk1
                                                                 y11.append(fields.at(9).toDouble()); // park nut
                                                                 y12.append(fields.at(10).toDouble()); // fault code
                                                               // y13.append(fields.at().toDouble());//idle setpoint
                                                               // y14.append(fields.at().toDouble());//idle hot
                                                               // y15.append(fields.at().toDouble());//uk2
                                                                  y16.append(fields.at(33).toDouble());//iac
                                                                  y17.append(fields.at(16).toDouble());//idle error
                                                               // y18.append(fields.at(0).toDouble());//ignadv offset
                                                                  y19.append(fields.at(17).toDouble());// ign adv
                                                                  y20.append(fields.at(32).toDouble());//coil time
                                                               // y21.append(fields.at(0).toDouble()); //cranks
                                                               // y22.append(fields.at(0).toDouble()); //uk4
                                                               // y23.append(fields.at(0).toDouble());//uk5
                                                               // y24.append(fields.at(0).toDouble());//ign switch
                                                                  y25.append(fields.at(31).toDouble());//throttle angle
                                                               // y26.append(fields.at(0).toDouble());//uk6
                                                                  y27.append(fields.at(18).toDouble());//air fuel ratio
                                                               //  y28.append(fields.at(28).toDouble());//fault code 0
                                                                  double g=(fields.at(20).toDouble());//lambda volt
                                                                  double h=g*5;
                                                                  y29.append(h);
                                                                  y30.append(fields.at(22).toDouble());//lamda freq
                                                                  y31.append(fields.at(23).toDouble()); // lambdadutycycle
                                                                  y32.append(fields.at(24).toDouble()); // lambdastatus
                                                                  y33.append(fields.at(25).toDouble());// closed loop
                                                                  y34.append(fields.at(26).toDouble());//long term
                                                                  y35.append(fields.at(27).toDouble());// short term
                                                                  y36.append(fields.at(28).toDouble()); // carbon canister
                                                                  line_count++;
                                                     }

                                                      }
                                                      file.close();
                                   }

//************************************mems-diag**********ok***********************

   else if (Firstcharacters=="Time,R")
                     {
                          type="mems-diag";
                          qDebug()<<"Zeile 2089:"<<type;
                          m_ui->label_type->setText(type);
                         //  m_ui->label_ecuid->setText("some variables with mems-diag not availiable!");
                          QStringList fields = Firstline.split(',');

                                  qDebug()<<"Zeile 2093 Firstline"<<Firstline;
                                   qDebug()<<"Zeile 2194 fields"<<fields;
                                          int r=fields.count();
                                          qDebug()<<"fields"<<r;
                                          m_ui->logfilewidget->clear();
                                          m_ui->logfilewidget->horizontalHeader()->isVisible();
                                m_ui->logfilewidget->setColumnCount(r);
                           for (int i = 0; i < r; ++i)
                           {

                               m_ui->logfilewidget->setHorizontalHeaderItem(i,new QTableWidgetItem(fields.at(i)));
                          }
                          QString line = in.readLine();
                          while(!in.atEnd())
                          {
                              int l=line_count;
                              m_ui->logfilewidget->insertRow(l);
                              line = in.readLine();// read first line and so on

                              qDebug() <<"2112"<< line;
                              qDebug() << "Zeile2138:"<<line_count;
                              QStringList fields = line.split(',');
                               qDebug() <<"2115"<< line;
                              for (int i = 0; i < r; ++i)
                                  {
                                       m_ui->logfilewidget->setItem(l-1, i,new QTableWidgetItem(fields.at(i)));
                                  }
                              x.append(tick);
                              tick++;
                                m_ui->logfilewidget->setItem(l, 0,new QTableWidgetItem(fields.at(0)));
                               y1.append(fields.at(1).toDouble()); // rpm
                                 m_ui->logfilewidget->setItem(l,1,new QTableWidgetItem(fields.at(1)));
                               y55.append(fields.at(1).toDouble());//rpm<1500
                               //  m_ui->logfilewidget->setItem(l, 0,new QTableWidgetItem(fields.at(0)));
                               y17.append(fields.at(2).toDouble());//idle error
                                 m_ui->logfilewidget->setItem(l, 2,new QTableWidgetItem(fields.at(2)));
                               y16.append(fields.at(3).toDouble());//idle Pos steps
                                 m_ui->logfilewidget->setItem(l, 3,new QTableWidgetItem(fields.at(3)));
                               y9.append(fields.at(4).toDouble());// idle switch
                                 m_ui->logfilewidget->setItem(l, 4,new QTableWidgetItem(fields.at(4)));
                               y2.append(fields.at(5).toDouble());//water temp
                                 m_ui->logfilewidget->setItem(l, 5,new QTableWidgetItem(fields.at(5)));
                               y6.append(fields.at(6).toDouble());//Map
                                 m_ui->logfilewidget->setItem(l, 6,new QTableWidgetItem(fields.at(6)));
                               y19.append(fields.at(7).toDouble());//Ignadv
                                 m_ui->logfilewidget->setItem(l, 7,new QTableWidgetItem(fields.at(7)));
                               y4.append(fields.at(8).toDouble());//inlet air
                                 m_ui->logfilewidget->setItem(l,8,new QTableWidgetItem(fields.at(8)));
                               y8.append(fields.at(9).toDouble());//Throttle volt
                                 m_ui->logfilewidget->setItem(l, 9,new QTableWidgetItem(fields.at(9)));
                               y7.append(fields.at(10).toDouble());//Battery
                                 m_ui->logfilewidget->setItem(l, 10,new QTableWidgetItem(fields.at(10)));
                               y20.append(fields.at(11).toDouble()); //coil time
                                 m_ui->logfilewidget->setItem(l,11,new QTableWidgetItem(fields.at(11)));
                               y33.append(fields.at(12).toDouble()); // closed loop
                                 m_ui->logfilewidget->setItem(l, 12,new QTableWidgetItem(fields.at(12)));
                               y29.append(fields.at(13).toDouble());//lambda volt
                                 m_ui->logfilewidget->setItem(l,13,new QTableWidgetItem(fields.at(13)));
                               y35.append(fields.at(14).toDouble());//fuel trim
                                 m_ui->logfilewidget->setItem(l, 14,new QTableWidgetItem(fields.at(14)));
                               y38.append(fields.at(15).toDouble());// idle base
                                 m_ui->logfilewidget->setItem(l,15,new QTableWidgetItem(fields.at(15)));
                               y28.append(fields.at(17).toDouble());//F1
                                 m_ui->logfilewidget->setItem(l, 17,new QTableWidgetItem(fields.at(17)));
                              // y38.append(fields.at(18).toDouble());//F2
                                   m_ui->logfilewidget->setItem(l, 16,new QTableWidgetItem(fields.at(16)));
                              //   y19.append(fields.at(19).toDouble());//Byte1
                                     m_ui->logfilewidget->setItem(l,18,new QTableWidgetItem(fields.at(18)));
                                      m_ui->logfilewidget->setItem(l,19,new QTableWidgetItem(fields.at(19)));
                               y45.append(fields.at(20).toDouble());//Byte 2
                                 m_ui->logfilewidget->setItem(l, 20,new QTableWidgetItem(fields.at(20)));
                               y54.append(fields.at(21).toDouble()); // rpm raw
                                 m_ui->logfilewidget->setItem(l, 21,new QTableWidgetItem(fields.at(21)));
                               y56.append(fields.at(22).toDouble()); // unknown load?
                                 m_ui->logfilewidget->setItem(l,22,new QTableWidgetItem(fields.at(22)));
                               y57.append(fields.at(23).toDouble());// ign_tune
                                 m_ui->logfilewidget->setItem(l, 23,new QTableWidgetItem(fields.at(23)));
                               y58.append(fields.at(24).toDouble());// idlerpm_tune
                                 m_ui->logfilewidget->setItem(l, 24,new QTableWidgetItem(fields.at(24)));
                               y59.append(fields.at(25).toDouble());//fuel_tune
                                 m_ui->logfilewidget->setItem(l,25,new QTableWidgetItem(fields.at(25)));
                              //	 y26.append(fields.at(26).toDouble());//Virtual Maf
                                   m_ui->logfilewidget->setItem(l,26,new QTableWidgetItem(fields.at(26)));
                              //   y27.append(fields.at(27).toDouble());// Virtual fuel l/s
                                     m_ui->logfilewidget->setItem(l,27,new QTableWidgetItem(fields.at(27)));
                              //	 y28.append(fields.at(28).toDouble());//virtual fuel l/h
                                       m_ui->logfilewidget->setItem(l,28,new QTableWidgetItem(fields.at(28)));
                              //   y29.append(fields.at(29).toDouble());//speed
                                         m_ui->logfilewidget->setItem(l, 29,new QTableWidgetItem(fields.at(29)));
                              //	 y30.append(fields.at(30).toDouble());// Gps long
                                           m_ui->logfilewidget->setItem(l, 30,new QTableWidgetItem(fields.at(30)));
                              //	 y31.append(fields.at(31).toDouble()); // gps lat
                                             m_ui->logfilewidget->setItem(l,31,new QTableWidgetItem(fields.at(31)));
                                             m_ui->logfilewidget->setItem(l,32,new QTableWidgetItem(fields.at(32)));
                              line_count++;

                          }
                          file.close();
                      }

//**************************************mems-analyzer******not running**********************

     else if (Firstcharacters=="Runnin")
                 {
                      type="mems-analyzer";
                      qDebug()<<"Zeile2198"<<type;
                      m_ui->label_type->setText(type);
                  m_ui->label_ecuid->setText("");
                                 m_ui->logfilewidget->setVisible(true);

                                  m_ui->logfilewidget->clear();
                                  m_ui->logfilewidget->horizontalHeader()->isVisible();
                      QStringList fields;
                      fields<<" "<<"1=rpm"<<"2=coolant"<<"3=ambient"<<"4=intake"<<"5=fuel"<<"6=map"<<"7=battery"<<"8=throttlepot"
                      <<"9=idlesw"<<"10=uk1"<<"11=nutrualsw"<<"12=fc2"<<"13=fc3"<<"14=idlesetpoint"<<"15=idlehot"<<"16=uk2"<<"17=iac"
                      <<"18=idledev"<<"19=ignadvoff"<<"20=ignadv"<<"21=coiltime"<<"22=crancs"<<"23=uk4"<<"24=uk5"<<"25=throttleangle"
                      <<"26=ignswitch"<<"27=afr"<<"28=uk"<<"lambdavolt"<<"lambdafreq"<<"lambdacycle"
					  <<"lambdastat"<<"loop"<<"ltft"<<"stft"<<"carbon"<<"fc1"<<"idlebase"<<"uk"<<"uk"
					  <<"ignadv2"<<"uk"<<"idleerror"<<"uk"<<"fc4"<<"uk"<<"uk"<<"uk"<<"uk"<<"uk"<<"uk"<<"uk"<<"uk"<<"jackcount";

                       int r=fields.count();
                     qDebug()<<"Zeile 2208 fields"<<fields;
                         m_ui->logfilewidget->setColumnCount(r);

                       for (int i = 0; i < r; ++i)
                 {

                       m_ui->logfilewidget->setHorizontalHeaderItem(i,new QTableWidgetItem(fields.at(i)));

                 }

                      QString line = in.readLine();

                        tick=1;
						line_count=0;
                        l=line_count;
                      while(!in.atEnd())
                      {
                         l=line_count;
                         m_ui->logfilewidget->insertRow(l);
                        qDebug()<<"Zeile2227 l=" <<l;
                          line = in.readLine();// read first line and so on
                          qDebug() <<"2229:line"<< line;
                          qDebug() << "Zeile2230:linecount"<<line_count;
                          qDebug() << "Zeile2232:field0"<<tick;
                          x.append(tick); // this will be x-line in a plot
                            field0=tick;
                           qDebug()<<"Zeile2234 tick=" <<tick;
                           m_ui->logfilewidget->setItem(l,0,new QTableWidgetItem(field0));
                           tick++;

                          int h=0;
                          const QRegularExpression eingangExp(QStringLiteral("^\\s*([0-9a-fA-F]{2}):"));
                          const QRegularExpression hexRegExp(QStringLiteral("([0-9a-fA-F]{2})(?!:)"));
                          const QRegularExpressionMatch eingangMatch = eingangExp.match(line);
                          const QRegularExpressionMatch hexMatch;
                          if(eingangMatch.hasMatch())
                          {

                              if(eingangMatch.capturedRef(1).compare(QString("80"),Qt::CaseInsensitive)==0)
                              {    // Eingang==("80:")
                                  qDebug() <<"Zeile2249:"<< eingangMatch;
                                  for(QRegularExpressionMatchIterator i = hexRegExp.globalMatch(line);
                                      i.hasNext();
                                      ++h)


                                          {
                                              //  qDebug()<<"Zeile2256:"<< "h="<<h;
                                              const QRegularExpressionMatch hexMatch = i.next();
                                             // qDebug()<<"Zeile2258:i.next" << hexMatch.capturedRef(0).toUInt(Q_NULLPTR,16);

                                              switch(h)
                                                      {
                                                          case 1 :  {high="";
                                                                    high.append(hexMatch.capturedRef(0));}break;
                                                                     //  qDebug()<<"high==: Zeile2265"<<high;break;//thisbreak was missing
                                                          case 2:  {low="";
                                                                      low.append(hexMatch.capturedRef(0));
                                                                     //  qDebug()<<"low==: Zeile2267"<<low;
                                                                      highlow=QString("%1%2").arg(high).arg(low);
                                                                     //   qDebug()<<"high2==:Zeile2269"<<highlow;
                                                                      y1.append(highlow.toUInt(Q_NULLPTR,16));//rpm right

                                                                      field1.setNum(highlow.toUInt(Q_NULLPTR,16));

                                                                      y55.append(highlow.toUInt(Q_NULLPTR,16));//rpm left

                                                                    //    qDebug()<<"y1 in Zeile2277"<<y1;
                                                                    //    qDebug()<<"field1 in Zeile 2278"<<field1;
                                                                       m_ui->logfilewidget->setItem(l,1,new QTableWidgetItem (field1));
                                                                      }break; //rpm
                                                          case 3 :  { qreal t=hexMatch.capturedRef(0).toUInt(Q_NULLPTR,16);//coolant temp
                                                                    qreal u=(t-55);// calculate celsius from fahrenheit
                                                                     y2.append(u);

                                                                     field2.setNum(u);
                                                                      m_ui->logfilewidget->setItem(l,2,new QTableWidgetItem (field2));

                                                                     qDebug()<<"y2 in Zeile 2293"<<(highlow.toUInt(Q_NULLPTR,16));
                                                                     } break;//coolant t
                                                                     //   case 3 : y2.append(hexMatch.capturedRef(0).toUInt(Q_NULLPTR,16));//coolant temp
                                                          case 4 : {qreal t=hexMatch.capturedRef(0).toUInt(Q_NULLPTR,16);//ambient temp
                                                                    qreal u=(t-55);
                                                                    y3.append(u);
                                                                    field3.setNum(u);
                                                                     m_ui->logfilewidget->setItem(l,3,new QTableWidgetItem (field3));



                                                                     qDebug()<<"y3 in Zeile 2299"<<(highlow.toUInt(Q_NULLPTR,16));
                                                                      }break;//ambient
                                                           case 5 : {qreal t=hexMatch.capturedRef(0).toUInt(Q_NULLPTR,16);//intake temp
                                                                    qreal u=(t-55);

                                                                    field4.setNum(u);

                                                                    y4.append(u);
                                                                     m_ui->logfilewidget->setItem(l,4,new QTableWidgetItem (field4));
                                                                     } break;//intake
                                                          case 6 : {qreal t=hexMatch.capturedRef(0).toUInt(Q_NULLPTR,16);//fuel temp
                                                                    qreal u=(t-55);
                                                                    field5.setNum(u);
                                                                    y5.append(u);
                                                                      m_ui->logfilewidget->setItem(l,5,new QTableWidgetItem (field5));} break;//fuel
                                                          case 7 :  y6.append(hexMatch.capturedRef(0).toUInt(Q_NULLPTR,16));
                                                                    field6.setNum(hexMatch.capturedRef(0).toUInt(Q_NULLPTR,16));
                                                                     m_ui->logfilewidget->setItem(l,6,new QTableWidgetItem (field6));
                                                                    break;//map

                                                          case 8 :  {qreal t=hexMatch.capturedRef(0).toUInt(Q_NULLPTR,16);
                                                                    qreal u=t/10;
                                                                    field7.setNum(u);

                                                                    m_ui->logfilewidget->setItem(l,7,new QTableWidgetItem (field7));
                                                                     y7.append(u);} break;//battery
                                                          case 9 : {qreal t=hexMatch.capturedRef(0).toUInt(Q_NULLPTR,16);
                                                                    qreal u=(t/10);
                                                                    field8.setNum(u);

                                                                    m_ui->logfilewidget->setItem(l,8,new QTableWidgetItem (field8));
                                                                    y8.append(u);break;}//(hexMatch.capturedRef(0).toUInt(Q_NULLPTR,16));} break;//throttlepot
                                                          case 10 : y9.append(hexMatch.capturedRef(0).toUInt(Q_NULLPTR,16));
                                                                     field9.setNum(hexMatch.capturedRef(0).toUInt(Q_NULLPTR,16));
                                                                     m_ui->logfilewidget->setItem(l,9,new QTableWidgetItem (field9));
                                                                     break;//idleswitch
                                                          case 11 : y10.append(hexMatch.capturedRef(0).toUInt(Q_NULLPTR,16));
                                                                     field10.setNum(hexMatch.capturedRef(0).toUInt(Q_NULLPTR,16));
                                                                     m_ui->logfilewidget->setItem(l,10,new QTableWidgetItem (field10));break;//uk1
                                                          case 12 : y11.append(hexMatch.capturedRef(0).toUInt(Q_NULLPTR,16));
																	field11.setNum(hexMatch.capturedRef(0).toUInt(Q_NULLPTR,16));
                                                                     m_ui->logfilewidget->setItem(l,11,new QTableWidgetItem (field11)); break;//park nut
                                                          case 13 : y12.append(hexMatch.capturedRef(0).toUInt(Q_NULLPTR,16));
																	field12.setNum(hexMatch.capturedRef(0).toUInt(Q_NULLPTR,16));
                                                                     m_ui->logfilewidget->setItem(l,12,new QTableWidgetItem (field12)); break;//faultcodecoolant and air
                                                          case 14 : //y13.append(hexMatch.capturedRef(0).toUInt(Q_NULLPTR,16));
																	field13.setNum(hexMatch.capturedRef(0).toUInt(Q_NULLPTR,16));
                                                                     m_ui->logfilewidget->setItem(l,13,new QTableWidgetItem (field13)); break;//fault codes fuel pump and throttle pot
                                                          case 15 : y13.append(hexMatch.capturedRef(0).toUInt(Q_NULLPTR,16));
																	field14.setNum(hexMatch.capturedRef(0).toUInt(Q_NULLPTR,16));
                                                                     m_ui->logfilewidget->setItem(l,14,new QTableWidgetItem (field14)); break;//idlsetpoint
                                                          case 16 : y14.append(hexMatch.capturedRef(0).toUInt(Q_NULLPTR,16));
																	field15.setNum(hexMatch.capturedRef(0).toUInt(Q_NULLPTR,16));
                                                                     m_ui->logfilewidget->setItem(l,15,new QTableWidgetItem (field15)); break;//idlehot
                                                          case 17 : y15.append(hexMatch.capturedRef(0).toUInt(Q_NULLPTR,16));
																	field16.setNum(hexMatch.capturedRef(0).toUInt(Q_NULLPTR,16));
                                                                     m_ui->logfilewidget->setItem(l,16,new QTableWidgetItem (field16)); break;//uk2
                                                          case 18 : y16.append(hexMatch.capturedRef(0).toUInt(Q_NULLPTR,16));
																	field17.setNum(hexMatch.capturedRef(0).toUInt(Q_NULLPTR,16));
                                                                     m_ui->logfilewidget->setItem(l,17,new QTableWidgetItem (field17)); break;//iac
                                                          case 19 : high="";high.append(hexMatch.capturedRef(0));
                                                                    break ;
                                                          case 20 : {low="";
                                                                    low.append(hexMatch.capturedRef(0));
                                                                    highlow=QString("%1%2").arg(high).arg(low);
                                                                    //   qDebug()<<"high2=="<<highlow;
                                                                    y17.append(highlow.toUInt(Q_NULLPTR,16));
                                                                     field18.setNum(hexMatch.capturedRef(0).toUInt(Q_NULLPTR,16));
                                                                     m_ui->logfilewidget->setItem(l,18,new QTableWidgetItem (field18));}break;//idlespeeddev

                                                          case 21 : y18 .append(hexMatch.capturedRef(0).toUInt(Q_NULLPTR,16));
                                                                      field19.setNum(hexMatch.capturedRef(0).toUInt(Q_NULLPTR,16));
                                                                     m_ui->logfilewidget->setItem(l,19,new QTableWidgetItem (field19)); break;//ignadvoffset
                                                          case 22 : y19.append(hexMatch.capturedRef(0).toUInt(Q_NULLPTR,16));
                                                                      field20.setNum(hexMatch.capturedRef(0).toUInt(Q_NULLPTR,16));
                                                                     m_ui->logfilewidget->setItem(l,20,new QTableWidgetItem (field20)); break;//ign adv
                                                          case 23:  high="";high.append(hexMatch.capturedRef(0));
                                                                                                                                                                         //   qDebug()<<"high=="<<high;break;//thisbreak was missing
                                                          case 24 : {low="";
                                                                    low.append(hexMatch.capturedRef(0));
                                                                    highlow=QString("%1%2").arg(high).arg(low);
                                                                    //   qDebug()<<"high2=="<<highlow;
                                                                    y20.append(highlow.toUInt(Q_NULLPTR,16));
                                                                     field21.setNum(highlow.toUInt(Q_NULLPTR,16));
                                                                     m_ui->logfilewidget->setItem(l,21,new QTableWidgetItem (field21));}break;//coiltime

                                                          case 25 : y21.append(hexMatch.capturedRef(0).toUInt(Q_NULLPTR,16));
                                                                     field22.setNum(hexMatch.capturedRef(0).toUInt(Q_NULLPTR,16));
                                                                     m_ui->logfilewidget->setItem(l,22,new QTableWidgetItem (field22)); break;//crank pos
                                                          case 26 : y22.append(hexMatch.capturedRef(0).toUInt(Q_NULLPTR,16));
                                                                    field23.setNum(hexMatch.capturedRef(0).toUInt(Q_NULLPTR,16));
                                                                     m_ui->logfilewidget->setItem(l,23,new QTableWidgetItem (field23)); break;//uk4
                                                          case 27 : y23.append(hexMatch.capturedRef(0).toUInt(Q_NULLPTR,16));
                                                                    field24.setNum(hexMatch.capturedRef(0).toUInt(Q_NULLPTR,16));
                                                                     m_ui->logfilewidget->setItem(l,24,new QTableWidgetItem (field24)); break;//uk5

                                                          default: break;

                                                      }

                                          }
                              }




                                  //:::::::::::::::::::::::::::::::::::::::::::::::::::::::


                          

                         if(eingangMatch.capturedRef(1).compare(QString("7D"),Qt::CaseInsensitive)==0)
                                          {
                                              qDebug() <<"Zeile2045"<< eingangMatch;
                                               for(QRegularExpressionMatchIterator i = hexRegExp.globalMatch(line);
                                                      i.hasNext();
                                                      ++h)
                                                      {//	qDebug()<<"Zeile2415"<< "h2="<<h;
                                                          const QRegularExpressionMatch hexMatch = i.next();
                                                       //   qDebug()<<"Zeile2417" << hexMatch.capturedRef(0).toUInt(Q_NULLPTR,16);
                                                          switch(h)
                                                          {  case 1: // data frame
                                                            case 2: y25.append(hexMatch.capturedRef(0).toUInt(Q_NULLPTR,16));// y25 und Y26 getauscht wegen falscher Zuordnung im Graph
                                                              field25.setNum(hexMatch.capturedRef(0).toUInt(Q_NULLPTR,16));
                                                               m_ui->logfilewidget->setItem(l,25,new QTableWidgetItem (field25));break;//ignswitch
                                                            case 3: y24.append(hexMatch.capturedRef(0).toUInt(Q_NULLPTR,16));
                                                              field26.setNum(hexMatch.capturedRef(0).toUInt(Q_NULLPTR,16));
                                                               m_ui->logfilewidget->setItem(l,26,new QTableWidgetItem (field26));break;//throttle angle
                                                            case 4: {qreal t=hexMatch.capturedRef(0).toUInt(Q_NULLPTR,16);
                                                              qreal u=t/10;
                                                              y27.append(u);
                                                              field27.setNum(u);
                                                               m_ui->logfilewidget->setItem(l,27,new QTableWidgetItem (field27));}break;//uk6
                                                             case 5: y26.append(hexMatch.capturedRef(0).toUInt(Q_NULLPTR,16));
                                                              field28.setNum(hexMatch.capturedRef(0).toUInt(Q_NULLPTR,16));
                                                               m_ui->logfilewidget->setItem(l,28,new QTableWidgetItem (field28));break;//air fuel ratio
                                                      //      case 6: y29.append(hexMatch.capturedRef(0).toUInt(Q_NULLPTR,16)); break;//fc0
                                                           case 6: {qreal t=hexMatch.capturedRef(0).toUInt(Q_NULLPTR,16);
                                                                    qreal u=t*5;
                                                                    y29.append(u);//
                                                              field29.setNum(u);
                                                               m_ui->logfilewidget->setItem(l,29,new QTableWidgetItem (field29));}break;//lambda voltage
                                                            case 7: y30.append(hexMatch.capturedRef(0).toUInt(Q_NULLPTR,16));
                                                              field30.setNum(hexMatch.capturedRef(0).toUInt(Q_NULLPTR,16));
                                                               m_ui->logfilewidget->setItem(l,30,new QTableWidgetItem (field30));break;//lambda freq
                                                            case 8: y31.append(hexMatch.capturedRef(0).toUInt(Q_NULLPTR,16));
                                                              field31.setNum(hexMatch.capturedRef(0).toUInt(Q_NULLPTR,16));
                                                               m_ui->logfilewidget->setItem(l,31,new QTableWidgetItem (field31));break;//lambda duty
                                                            case 9: y32.append(hexMatch.capturedRef(0).toUInt(Q_NULLPTR,16));
                                                              field32.setNum(hexMatch.capturedRef(0).toUInt(Q_NULLPTR,16));
                                                               m_ui->logfilewidget->setItem(l,32,new QTableWidgetItem (field32));break;//lambda status
                                                            case 10: y33.append(hexMatch.capturedRef(0).toUInt(Q_NULLPTR,16));
                                                              field33.setNum(hexMatch.capturedRef(0).toUInt(Q_NULLPTR,16));
                                                               m_ui->logfilewidget->setItem(l,33,new QTableWidgetItem (field33));break;//lambda loop
                                                            case 11: y34.append(hexMatch.capturedRef(0).toUInt(Q_NULLPTR,16));
                                                              field34.setNum(hexMatch.capturedRef(0).toUInt(Q_NULLPTR,16));
                                                               m_ui->logfilewidget->setItem(l,34,new QTableWidgetItem (field34));break;// ltft

                                                            case 12: y35.append(hexMatch.capturedRef(0).toUInt(Q_NULLPTR,16));
                                                              field35.setNum(hexMatch.capturedRef(0).toUInt(Q_NULLPTR,16));
                                                               m_ui->logfilewidget->setItem(l,35,new QTableWidgetItem (field35));break;// stft
                                                            case 13: y36.append(hexMatch.capturedRef(0).toUInt(Q_NULLPTR,16));
                                                              field36.setNum(hexMatch.capturedRef(0).toUInt(Q_NULLPTR,16));
                                                               m_ui->logfilewidget->setItem(l,36,new QTableWidgetItem (field36));break;// carboncan
                                                            case 14: y37.append(hexMatch.capturedRef(0).toUInt(Q_NULLPTR,16));
                                                              field37.setNum(hexMatch.capturedRef(0).toUInt(Q_NULLPTR,16));
                                                               m_ui->logfilewidget->setItem(l,37,new QTableWidgetItem (field37));break;//fc1
                                                            case 15: y38.append(hexMatch.capturedRef(0).toUInt(Q_NULLPTR,16));
                                                              field38.setNum(hexMatch.capturedRef(0).toUInt(Q_NULLPTR,16));
                                                               m_ui->logfilewidget->setItem(l,38,new QTableWidgetItem (field38));break;//idle base
                                                            case 16: y39.append(hexMatch.capturedRef(0).toUInt(Q_NULLPTR,16));
                                                              field39.setNum(hexMatch.capturedRef(0).toUInt(Q_NULLPTR,16));
                                                               m_ui->logfilewidget->setItem(l,39,new QTableWidgetItem (field39));break;//uk7
                                                            case 17: y40.append(hexMatch.capturedRef(0).toUInt(Q_NULLPTR,16));
                                                              field40.setNum(hexMatch.capturedRef(0).toUInt(Q_NULLPTR,16));
                                                               m_ui->logfilewidget->setItem(l,40,new QTableWidgetItem (field40));break;//uk8
                                                            case 18: y41.append(hexMatch.capturedRef(0).toUInt(Q_NULLPTR,16));
                                                              field41.setNum(hexMatch.capturedRef(0).toUInt(Q_NULLPTR,16));
                                                               m_ui->logfilewidget->setItem(l,41,new QTableWidgetItem (field41));break;//uk9
                                                            case 19: y42.append(hexMatch.capturedRef(0).toUInt(Q_NULLPTR,16));
                                                              field42.setNum(hexMatch.capturedRef(0).toUInt(Q_NULLPTR,16));
                                                               m_ui->logfilewidget->setItem(l,42,new QTableWidgetItem (field42));break;//idleerror2
                                                            case 20: y43.append(hexMatch.capturedRef(0).toUInt(Q_NULLPTR,16));
                                                              field43.setNum(hexMatch.capturedRef(0).toUInt(Q_NULLPTR,16));
                                                               m_ui->logfilewidget->setItem(l,43,new QTableWidgetItem (field43));break;//uk10
                                                            case 21: y44.append(hexMatch.capturedRef(0).toUInt(Q_NULLPTR,16));
                                                              field44.setNum(hexMatch.capturedRef(0).toUInt(Q_NULLPTR,16));
                                                               m_ui->logfilewidget->setItem(l,44,new QTableWidgetItem (field44));break;//fc4
                                                            case 22: y45.append(hexMatch.capturedRef(0).toUInt(Q_NULLPTR,16));
                                                              field45.setNum(hexMatch.capturedRef(0).toUInt(Q_NULLPTR,16));
                                                               m_ui->logfilewidget->setItem(l,45,new QTableWidgetItem (field45));break;//uk11
                                                            case 23: y46.append(hexMatch.capturedRef(0).toUInt(Q_NULLPTR,16));
                                                              field46.setNum(hexMatch.capturedRef(0).toUInt(Q_NULLPTR,16));
                                                               m_ui->logfilewidget->setItem(l,46,new QTableWidgetItem (field46));break;//uk12
                                                            case 24: y47.append(hexMatch.capturedRef(0).toUInt(Q_NULLPTR,16));
                                                              field47.setNum(hexMatch.capturedRef(0).toUInt(Q_NULLPTR,16));
                                                               m_ui->logfilewidget->setItem(l,47,new QTableWidgetItem (field47));break;//uk13
                                                            case 25: y48.append(hexMatch.capturedRef(0).toUInt(Q_NULLPTR,16));
                                                              field48.setNum(hexMatch.capturedRef(0).toUInt(Q_NULLPTR,16));
                                                               m_ui->logfilewidget->setItem(l,48,new QTableWidgetItem (field48));break;//uk14
                                                            case 26: y49.append(hexMatch.capturedRef(0).toUInt(Q_NULLPTR,16));
                                                              field49.setNum(hexMatch.capturedRef(0).toUInt(Q_NULLPTR,16));
                                                               m_ui->logfilewidget->setItem(l,49,new QTableWidgetItem (field49));break;//uk15
                                                            case 27: y50.append(hexMatch.capturedRef(0).toUInt(Q_NULLPTR,16));
                                                              field50.setNum(hexMatch.capturedRef(0).toUInt(Q_NULLPTR,16));
                                                               m_ui->logfilewidget->setItem(l,50,new QTableWidgetItem (field50));break;//uk16
                                                            case 28: y51.append(hexMatch.capturedRef(0).toUInt(Q_NULLPTR,16));
                                                              field51.setNum(hexMatch.capturedRef(0).toUInt(Q_NULLPTR,16));
                                                               m_ui->logfilewidget->setItem(l,51,new QTableWidgetItem (field51));break;//uk1A
                                                            case 29: y52.append(hexMatch.capturedRef(0).toUInt(Q_NULLPTR,16));
                                                              field52.setNum(hexMatch.capturedRef(0).toUInt(Q_NULLPTR,16));
                                                               m_ui->logfilewidget->setItem(l,52,new QTableWidgetItem (field52));break;//uk1B
                                                            case 30: y53.append(hexMatch.capturedRef(0).toUInt(Q_NULLPTR,16));
                                                              field53.setNum(hexMatch.capturedRef(0).toUInt(Q_NULLPTR,16));
                                                               m_ui->logfilewidget->setItem(l,53,new QTableWidgetItem (field53));break;//uk1C
                                                            case 31: y54.append(hexMatch.capturedRef(0).toUInt(Q_NULLPTR,16));
                                                              field54.setNum(hexMatch.capturedRef(0).toUInt(Q_NULLPTR,16));
                                                               m_ui->logfilewidget->setItem(l,54,new QTableWidgetItem (field54));break;//uk1C
                                                            default: break;
                                                          }
                                                      }
                                          }

                          line_count++;
                          qDebug()<<"Zeilen 2522"<<line_count;
                        }
                          file.close();
                            }


           }
                  //****************mems-read*****0k**************************

else if (Firstcharacters== "\"times")

              {

                  type="mems-read";
                  qDebug()<<"Zeile3236"<<type;
                  //m_ui->label_ecuid->setText(Firstline);
                  m_ui->label_type->setText(type);
                  QString line = in.readLine();

                        QStringList fields =Firstline.split(',');

                           qDebug()<<"Zeile 3243 secondline"<<Firstline;
                            qDebug()<<"Zeile 3244fields"<<fields;
                                   int r=fields.count();
                                   qDebug()<<"Zeile 3246fields"<<r;
                                   m_ui->logfilewidget->clear();
                                   m_ui->logfilewidget->horizontalHeader()->isVisible();
                         m_ui->logfilewidget->setColumnCount(r);
                 for (int i = 0; i < r; ++i)
                     {

                      m_ui->logfilewidget->setHorizontalHeaderItem(i,new QTableWidgetItem(fields.at(i)));
                }

                      tick=1;

                      while(!in.atEnd())
                     {
                        line = in.readLine();
                        int l=line_count;
                        qDebug() << "Zeile2396:"<<line;
                       qDebug() << "Zeile2397:"<<line_count;
                     QStringList fields = line.split(QRegExp(","), QString::SkipEmptyParts);




                                 m_ui->logfilewidget->insertRow(l);
                                 for (int i=0;i<r;++i)
                                 {
                                      m_ui->logfilewidget->setItem(l, i,new QTableWidgetItem(fields.at(i)));
                                 }

                                 //  x.append(QTime::fromString(fields.at(0),"hh:mm:ss").msecsSinceStartOfDay()/10000.0);//time
                                 // take first value and stored into x
                                 x.append(tick);

                                 tick++;
                                 QString item= fields.at(1);
                                 item = item.mid(1, item.length() -2 );

                                 y1.append(item.toDouble());//rpm

                                 y55.append(item.toDouble());
                                 item= fields.at(2);
                                 item = item.mid(1, item.length() -2 );

                                 y2.append(item.toDouble()); //coolanttemp

                                 item= fields.at(34);
                                 item = item.mid(1, item.length() -2 );

                                 y3.append(item.toDouble());
                                 item= fields.at(3);
                                 item = item.mid(1, item.length() -2 );

                                 y4.append(item.toDouble()); //intake air
                                  item= fields.at(35);
                                 item = item.mid(1, item.length() -2 );

                                 y5.append(item.toDouble());//fuel temp
                                  item= fields.at(4);
                                 item = item.mid(1, item.length() -2 );

                                 y6.append(item.toDouble());//map
                                  item= fields.at(5);
                                 item = item.mid(1, item.length() -2 );

                                 y7.append(item.toDouble());//battery
                                  item= fields.at(6);
                                 item = item.mid(1, item.length() -2 );

                                 y8.append(item.toDouble());//throttlepot
                                  item= fields.at(8);
                                 item = item.mid(1, item.length() -2 );

                                 y9.append(item.toDouble());//idleswitch
                                  item= fields.at(9);
                                 item = item.mid(1, item.length() -2 );

                                 y11.append(item.toDouble());//parknut
                                  item= fields.at(10);
                                 item = item.mid(1, item.length() -2 );

                                 y12.append(item.toDouble());//faultcode
                                  item= fields.at(13);
                                 item = item.mid(1, item.length() -2 );

                                 y13.append(item.toDouble());//idle setpoint
                                 item= fields.at(14);
                                 item = item.mid(1, item.length() -2 );

                                 y14.append(item.toDouble());//idlehot
                                  item= fields.at(33);
                                 item = item.mid(1, item.length() -2 );

                                 y16.append(item.toDouble());//iac
                                 item= fields.at(16);
                                 item = item.mid(1, item.length() -2 );

                                 y17.append(item.toDouble());//idle error
                                  item= fields.at(17);
                                 item = item.mid(1, item.length() -2 );

                                 y19.append(item.toDouble());//ign adv
                                  item= fields.at(32);
                                 item = item.mid(1, item.length() -2 );
                                 double a=item.toDouble();
                                 a=a/100;

                                 y20.append(a);//coil time
                                 item= fields.at(31);
                                 item = item.mid(1, item.length() -2 );

                                 y25.append(item.toDouble());//throttle angle
                                  item= fields.at(18);
                                 item = item.mid(1, item.length() -2 );
                                 double u=(item.toDouble());
                                 u=u/10;

                                 y27.append(u);//air fuel ratio
                                 item= fields.at(28);
                                 item = item.mid(1, item.length() -2 );

                                 y28.append(item.toDouble());//fc0
                                  item= fields.at(20);
                                 item = item.mid(1, item.length() -2 );
                                 double g=(item.toDouble());
                                 double h=g*5;

                                 y29.append(h);//lambda volt
                                 item= fields.at(22);
                                 item = item.mid(1, item.length() -2 );

                                 y30.append(item.toDouble());//lambda freq
                                  item= fields.at(23);
                                 item = item.mid(1, item.length() -2 );

                                 y31.append(item.toDouble());//lambdaduty
                                 item= fields.at(24);
                                 item = item.mid(1, item.length() -2 );

                                 y32.append(item.toDouble());//lambdastatus
                                item= fields.at(25);
                                 item = item.mid(1, item.length() -2 );

                                 y33.append(item.toDouble());//closed loop
                                 item= fields.at(26);
                                 item = item.mid(1, item.length() -2 );

                                 y34.append(item.toDouble());//long term
                                  item= fields.at(27);
                                 item = item.mid(1, item.length() -2 );

                                 y35.append(item.toDouble());//short term
                                  item= fields.at(28);
                                 item = item.mid(1, item.length() -2 );

                                 y36.append(item.toDouble());//carbon canister
                                 item= fields.at(29);
                                 item = item.mid(1, item.length() -2 );

                                 y38.append(item.toDouble());//idle base
                                  item= fields.at(30);
                                 item = item.mid(1, item.length() -2 );

                                 y43.append(item.toDouble());//idle error2

                                 line_count++;
                                 }



                                                  qDebug()<<"zeile 2205:"<<line_count;



                                            file.close();
                                   }
              //****************mems-mpi*******************************

else if(Firstcharacters=="row,ac")
                                {
                                    type="jackson mpi";
                                    qDebug()<<"Zeile3088"<<type;
                                    //m_ui->label_ecuid->setText(Firstline);
                                    m_ui->label_type->setText(type);

                                   QStringList fields = Firstline.split(",");
                                               qDebug()<<"Zeile 3093 firstline"<<Firstline;
                                               qDebug()<<"Zeile 3094 fields"<<fields;
                                    int r=fields.count();
                                              qDebug()<<"fields zeile3096"<<r;  //35
                                    m_ui->logfilewidget->clear();
                                    m_ui->logfilewidget->horizontalHeader()->isVisible();
                                    m_ui->logfilewidget->setColumnCount(r);
                                       for (int i = 0; i < r; ++i) // i=1 um rows nict zu übertragen
                                           {

                                           m_ui->logfilewidget->setHorizontalHeaderItem(i,new QTableWidgetItem(fields.at(i)));
                                           }

                                           int l=0;

                                        while(!in.atEnd())
                                        {

                                            qDebug()<<"Zeile 3120 linecount"<<l;
                                            QString line = in.readLine();
                                            qDebug() << "Zeile3121 line:"<<line;
                                            line.replace("\"", ""); // get rid of "
                                            QStringList fields = line.split(","); // split at commas
                                            qDebug().noquote() <<"zeile3124 fields"<< fields;  // notice the noquote as else qDebug shows with "" which could confuse us
                                             m_ui->logfilewidget->insertRow(l);

                                            x.append(fields.at(0).toDouble());




                                                 m_ui->logfilewidget->setItem(l, 0,new QTableWidgetItem(fields.at(0)));


                                                y1.append(fields.at(1).toDouble()); // rpm
                                                 m_ui->logfilewidget->setItem(l,1, new QTableWidgetItem(fields.at(1)));
												y55.append(fields.at(1).toDouble());//rpm<1500		
                                                y2.append(fields.at(2).toDouble());//coolant temp
                                                 m_ui->logfilewidget->setItem(l,2, new QTableWidgetItem(fields.at(2)));
                                                y3.append(fields.at(3).toDouble());//ambient air
                                                 m_ui->logfilewidget->setItem(l,3, new QTableWidgetItem(fields.at(3)));

                                                 y4.append(fields.at(4).toDouble());//intake air
                                                 m_ui->logfilewidget->setItem(l,4, new QTableWidgetItem(fields.at(4)));

                                                y5.append(fields.at(5).toDouble());//fuel temp
                                                 m_ui->logfilewidget->setItem(l,5, new QTableWidgetItem(fields.at(5)));
                                                y6.append(fields.at(6).toDouble());//map
                                                 m_ui->logfilewidget->setItem(l,6, new QTableWidgetItem(fields.at(6)));
                                                y7.append(fields.at(7).toDouble());//battery
                                                 m_ui->logfilewidget->setItem(l,7, new QTableWidgetItem(fields.at(7)));
                                                y8.append(fields.at(8).toDouble());//throttle pot
                                                 m_ui->logfilewidget->setItem(l,8, new QTableWidgetItem(fields.at(8)));
                                                y9.append(fields.at(9).toDouble());//idle switch
                                                 m_ui->logfilewidget->setItem(l,9, new QTableWidgetItem(fields.at(9)));
                                                y10.append(fields.at(10).toDouble());//uk1
                                                 m_ui->logfilewidget->setItem(l,10, new QTableWidgetItem(fields.at(10)));
                                                y11.append(fields.at(11).toDouble()); // park nut
                                                 m_ui->logfilewidget->setItem(l,11, new QTableWidgetItem(fields.at(11)));
                                                y12.append(fields.at(12).toDouble()); // fault code
                                                 m_ui->logfilewidget->setItem(l,12, new QTableWidgetItem(fields.at(12)));
                                                y13.append(fields.at(13).toDouble());//idle setpoint
                                                 m_ui->logfilewidget->setItem(l,13, new QTableWidgetItem(fields.at(13)));
                                                y14.append(fields.at(14).toDouble());//idle hot
                                                m_ui->logfilewidget->setItem(l,14, new QTableWidgetItem(fields.at(14)));
                                                y15.append(fields.at(15).toDouble());//uk2
                                                 m_ui->logfilewidget->setItem(l,15, new QTableWidgetItem(fields.at(15)));
                                                y16.append(fields.at(16).toDouble());//iac
                                                 m_ui->logfilewidget->setItem(l,16, new QTableWidgetItem(fields.at(16)));
                                                y17.append(fields.at(17).toDouble());//idle error
                                                 m_ui->logfilewidget->setItem(l,17, new QTableWidgetItem(fields.at(17)));
                                                y18.append(fields.at(18).toDouble());//ignadv offset
                                                 m_ui->logfilewidget->setItem(l,18, new QTableWidgetItem(fields.at(18)));
                                                y19.append(fields.at(19).toDouble());// ign adv
                                                 m_ui->logfilewidget->setItem(l,19, new QTableWidgetItem(fields.at(19)));
                                                y20.append(fields.at(20).toDouble());//coil time
                                                 m_ui->logfilewidget->setItem(l,20, new QTableWidgetItem(fields.at(20)));
                                                y21.append(fields.at(21).toDouble()); //cranks
                                                 m_ui->logfilewidget->setItem(l,21, new QTableWidgetItem(fields.at(21)));
                                                y22.append(fields.at(22).toDouble()); //uk4
                                                 m_ui->logfilewidget->setItem(l,22, new QTableWidgetItem(fields.at(22)));
                                                y23.append(fields.at(23).toDouble());//uk5
                                                 m_ui->logfilewidget->setItem(l,23, new QTableWidgetItem(fields.at(23)));
                                                y24.append(fields.at(24).toDouble());//ign switch
                                                 m_ui->logfilewidget->setItem(l,24, new QTableWidgetItem(fields.at(24)));
                                                y25.append(fields.at(25).toDouble());//throttle angle
                                                 m_ui->logfilewidget->setItem(l,25, new QTableWidgetItem(fields.at(25)));
                                                y26.append(fields.at(26).toDouble());//uk6
                                                 m_ui->logfilewidget->setItem(l,26, new QTableWidgetItem(fields.at(26)));


                                                y27.append(fields.at(27).toDouble());//uk6
                                                 m_ui->logfilewidget->setItem(l,27, new QTableWidgetItem(fields.at(27)));
                                                y28.append(fields.at(28).toDouble());//fault code 0
                                                 m_ui->logfilewidget->setItem(l,28, new QTableWidgetItem(fields.at(28)));

                                                y29.append(fields.at(29).toDouble());//uk6;
                                                 m_ui->logfilewidget->setItem(l,29, new QTableWidgetItem(fields.at(29)));
                                                y30.append(fields.at(30).toDouble());//lamda freq
                                                 m_ui->logfilewidget->setItem(l,30, new QTableWidgetItem(fields.at(30)));
                                                y31.append(fields.at(31).toDouble()); // lambdadutycycle
                                                 m_ui->logfilewidget->setItem(l,31, new QTableWidgetItem(fields.at(31)));
                                                y32.append(fields.at(32).toDouble()); // lambdastatus
                                                 m_ui->logfilewidget->setItem(l,32, new QTableWidgetItem(fields.at(32)));
                                                y33.append(fields.at(33).toDouble());// closed loop
                                                 m_ui->logfilewidget->setItem(l,33, new QTableWidgetItem(fields.at(33)));
                                                y34.append(fields.at(34).toDouble());//long term
                                                 m_ui->logfilewidget->setItem(l,34, new QTableWidgetItem(fields.at(34)));

                                                y35.append(0);// short term

                                                y36.append(0); // carbon canister

                                                y37.append(0);//faultcode1

                                                y38.append(0);//idle base pos

                                                y39.append(0); // uk7

                                                y40.append(0);//uk8

                                                y41.append(0); // ignadv2

                                                y42.append(0); //uk9

                                                y43.append(0);//idle error2

                                                y44.append(0);//uk10

                                                y45.append(0);//faultcode4

                                                y46.append(0);//uk11

                                                y47.append(0);//uk12

                                                y48.append(0); //uk13

                                                y49.append(0);//uk14

                                                y50.append(0);//uk15

                                                y51.append(0);//uk16

                                                y52.append(0);//uk17

                                                y53.append(0);//uk18

                                                 y54.append(0);//uk19 jack count



                                            l++;

                                        }
                                        file.close();
}



 //***********************************Pscan*******************************************
                   else if (Firstcharacters=="timest")
                   {  type="psscan";
                   int m;
                     QStringList fields = Firstline.split(",");
                       //   QStringList fields = Firstline.split(QRegExp("\\t+"), QString::SkipEmptyParts);
                          Feld1.append(fields.at(0));
                             m=fields.count();
                             int r=fields.count();
                                       qDebug()<<"fields zeile3096"<<r;  //35
                             m_ui->logfilewidget->clear();
                             m_ui->logfilewidget->horizontalHeader()->isVisible();
                             m_ui->logfilewidget->setColumnCount(r);
                                for (int i = 0; i < r; ++i) // i=1 um rows nict zu übertragen
                                    {

                                    m_ui->logfilewidget->setHorizontalHeaderItem(i,new QTableWidgetItem(fields.at(i)));
                                    }

                                    int l=0;
                            qDebug() << "Zeile3548:"<<m;
                               if (m==46)
                                  {  QString line = in.readLine();
                                     while(!in.atEnd())
                                      {
                                       QString line = in.readLine();
                                        l=line_count;                                      int l=line_count;
                                       qDebug() << "Zeile3554:"<<line;
                                       qDebug() << "Zeile3555:"<<l;
                                       QStringList fields = line.split(",");
                                         m_ui->logfilewidget->insertRow(l);


                                                          //  x.append(QTime::fromString(fields.at(0),"hh:mm:ss").msecsSinceStartOfDay()/10000.0);//time
                                                          // take first value and stored into x
                                         m_ui->logfilewidget->setItem(l, 0,new QTableWidgetItem(fields.at(0)));

                                                          x.append(tick);
                                                          tick++;
                                                          y1.append(fields.at(1).toDouble()); // rp
                                                          m_ui->logfilewidget->setItem(l,1, new QTableWidgetItem(fields.at(1)));
                                                           qDebug() << "Zeile3565:"<<x;
                                                            qDebug() << "Zeile3566:"<<y1;
                                                          y55.append(fields.at(1).toDouble());//rpm<1500
                                                          //     double t=(fields.at(2).toDouble());//
                                                          //     double u=(t-32)*55556/100000;// calculate celsius from fahrenheit
                                                          y2.append(fields.at(2).toDouble());//coolant temp
                                                          m_ui->logfilewidget->setItem(l,2, new QTableWidgetItem(fields.at(2)));
                                                          //     double a=(fields.at(3).toDouble());
                                                          //     double b=(a-32)*55556/100000;
                                                          y3.append(fields.at(34).toDouble());//ambient air
                                                            m_ui->logfilewidget->setItem(l,34, new QTableWidgetItem(fields.at(34)));
                                                           //    double c=(fields.at(4).toDouble());
                                                           //    double d=(c-32)*55556/100000;
                                                          y4.append(fields.at(3).toDouble());//intake air
                                                           m_ui->logfilewidget->setItem(l,3, new QTableWidgetItem(fields.at(3)));
                                                          //     double e=(fields.at(5).toDouble());
                                                           //    double f=(e-32)*55556/100000;
                                                         // y5.append(fields.at(5).toDouble());//fuel temp
                                                          y6.append(fields.at(4).toDouble());//map
                                                          m_ui->logfilewidget->setItem(l,4, new QTableWidgetItem(fields.at(4)));
                                                          y7.append(fields.at(5).toDouble());//battery
                                                          m_ui->logfilewidget->setItem(l,5, new QTableWidgetItem(fields.at(5)));
                                                          y8.append(fields.at(6).toDouble());//throttle pot
                                                          m_ui->logfilewidget->setItem(l,6, new QTableWidgetItem(fields.at(6)));
                                                          y9.append(fields.at(8).toDouble());//idle switch
                                                          m_ui->logfilewidget->setItem(l,8, new QTableWidgetItem(fields.at(8)));
                                                         // y10.append(fields.at(10).toDouble());//uk1
                                                          y11.append(fields.at(9).toDouble()); // park nut
                                                          m_ui->logfilewidget->setItem(l,9, new QTableWidgetItem(fields.at(9)));
                                                            y12.append(fields.at(10).toDouble()); // fault code
                                                            m_ui->logfilewidget->setItem(l,10, new QTableWidgetItem(fields.at(10)));
                                                         y13.append(fields.at(29).toDouble());//idle setpoint
                                                         m_ui->logfilewidget->setItem(l,29, new QTableWidgetItem(fields.at(29)));
                                                      //    y14.append(fields.at(14).toDouble());//idle hot
                                                      //    y15.append(fields.at(15).toDouble());//uk2
                                                         y16.append(fields.at(33).toDouble());//iac
                                                         m_ui->logfilewidget->setItem(l,33, new QTableWidgetItem(fields.at(33)));
                                                          y17.append(fields.at(30).toDouble());//idle error
                                                          m_ui->logfilewidget->setItem(l,30, new QTableWidgetItem(fields.at(30)));
                                                      //    y18.append(fields.at(18).toDouble());//ignadv offset
                                                            y19.append(fields.at(17).toDouble());// ign adv
                                                            m_ui->logfilewidget->setItem(l,17, new QTableWidgetItem(fields.at(17)));
                                                          y20.append(fields.at(32).toDouble());//coil time
                                                          m_ui->logfilewidget->setItem(l,20, new QTableWidgetItem(fields.at(20)));
                                                     //     y21.append(fields.at(21).toDouble()); //cranks
                                                     //     y22.append(fields.at(22).toDouble()); //uk4
                                                     //     y23.append(fields.at(23).toDouble());//uk5
                                                     //     y24.append(fields.at(24).toDouble());//ign switch
                                                          y25.append(fields.at(31).toDouble());//throttle angle
                                                          m_ui->logfilewidget->setItem(l,31, new QTableWidgetItem(fields.at(31)));
                                                     //     y26.append(fields.at(26).toDouble());//uk6
                                                            double u =(fields.at(18).toDouble());
                                                            u=u/10;
                                                            y27.append(u);//air fuel ratio
                                                            m_ui->logfilewidget->setItem(l,18, new QTableWidgetItem(fields.at(18)));
                                                     //     y28.append(fields.at(28).toDouble());//fault code 0
                                                            double g=(fields.at(20).toDouble());//lambda volt
                                                            double h=g*5;
                                                            y29.append(h);
                                                            m_ui->logfilewidget->setItem(l,20, new QTableWidgetItem(fields.at(20)));
                                                     //     y30.append(fields.at(30).toDouble());//lamda freq
                                                     //     y31.append(fields.at(31).toDouble()); // lambdadutycycle
                                                     //     y32.append(fields.at(32).toDouble()); // lambdastatus
                                                            y33.append(fields.at(25).toDouble());// closed loop
                                                            m_ui->logfilewidget->setItem(l,25, new QTableWidgetItem(fields.at(25)));
                                                            y34.append(fields.at(26).toDouble());//long term
                                                            m_ui->logfilewidget->setItem(l,26, new QTableWidgetItem(fields.at(26)));
                                                            y35.append(fields.at(27).toDouble());// short term
                                                            m_ui->logfilewidget->setItem(l,27, new QTableWidgetItem(fields.at(27)));
                                                            y36.append(fields.at(28).toDouble()); // carbon canister
                                                            m_ui->logfilewidget->setItem(l,28, new QTableWidgetItem(fields.at(28)));
                                                    //    y37.append(fields.at(37).toDouble());//faultcode1
                                                          y38.append(fields.at(29).toDouble());//idle base pos
                                                          m_ui->logfilewidget->setItem(l,29, new QTableWidgetItem(fields.at(29)));
                                                    //    y39.append(fields.at(39).toDouble()); // uk7
                                                    //    y40.append(fields.at(40).toDouble());//uk8
                                                    //    y41.append(fields.at(41).toDouble()); // ignadv2
                                                    //    y42.append(fields.at(42).toDouble()); //uk9
                                                    //       y43.append(fields.at(33).toDouble());//idle error2
                                                    //    y44.append(fields.at(44).toDouble());//uk10
                                                    //    y45.append(fields.at(45).toDouble());//faultcode4
                                                    //    y46.append(fields.at(46).toDouble());//uk11
                                                    //    y47.append(fields.at(47).toDouble());//uk12
                                                    //    y48.append(fields.at(48).toDouble()); //uk13
                                                    //    y49.append(fields.at(49).toDouble());//uk14
                                                    //    y50.append(fields.at(50).toDouble());//uk15
                                                    //    y51.append(fields.at(51).toDouble());//uk16
                                                    //    y52.append(fields.at(52).toDouble());//uk17
                                                    //    y53.append(fields.at(53).toDouble());//uk18
                                                    //    y54.append(fields.at(54).toDouble());
                                                          l++;

                                }

                                                                          line_count++;

 file.close();
                   }}
// *******************************mems-rosco-old****************************

else
                  {
                      type="mems-rosco-old";
                      qDebug()<<"zeile 3592:"<<type;
                      m_ui->label_type->setText(type);
                       m_ui->label_ecuid->setText("");
                       QString line = in.readLine();
                          QStringList fields = line.split(',');
                       int r=fields.count();
                       qDebug()<<"Zeile 3598fields"<<r;
                       m_ui->logfilewidget->clear();
                       m_ui->logfilewidget->horizontalHeader()->isVisible();
             m_ui->logfilewidget->setColumnCount(r);
     for (int i = 0; i < r; ++i)
         {

          m_ui->logfilewidget->setHorizontalHeaderItem(i,new QTableWidgetItem(fields.at(i)));
    }
                      while(!in.atEnd())
                      {
                          QString line = in.readLine();

                          qDebug() << "Zeile3611:"<<line;
                          qDebug() << "Zeile3612:"<<line_count;
                          QStringList fields = line.split(',');
                          m_ui->logfilewidget->insertRow(l);
                          for (int i=0;i<r;++i)
                          {
                               m_ui->logfilewidget->setItem(l, i,new QTableWidgetItem(fields.at(i)));
                          }
                          x.append(QTime::fromString(fields.at(0),"hh:mm:ss").msecsSinceStartOfDay()/10000.0);//time
                          // take first value and stored into x

                          y1.append(fields.at(1).toDouble()); // rpm
                          y55.append(fields.at(1).toDouble());//rpm<1500

                               double t=(fields.at(2).toDouble());//
                               double u=(t-32)*55556/100000;// calculate celsius from fahrenheit
                          y2.append(u);//coolant temp

                               double a=(fields.at(3).toDouble());
                               double b=(a-32)*55556/100000;
                          y3.append(b);//ambient air

                               double c=(fields.at(4).toDouble());
                               double d=(c-32)*55556/100000;
                          y4.append(d);//intake air

                               double e=(fields.at(5).toDouble());
                               double f=(e-32)*55556/100000;
                          y5.append(f);//fuel temp
                          y6.append(fields.at(6).toDouble());//map
                          y7.append(fields.at(7).toDouble());//battery
                          y8.append(fields.at(8).toDouble());//throttle pot
                          y9.append(fields.at(9).toDouble());//idle switch
                          y10.append(fields.at(10).toDouble());//uk1
                          y11.append(fields.at(11).toDouble()); // park nut
                          y12.append(fields.at(12).toDouble()); // fault code
                          y13.append(fields.at(13).toDouble());//idle setpoint
                          y14.append(fields.at(14).toDouble());//idle hot
                          y15.append(fields.at(15).toDouble());//uk2
                          y16.append(fields.at(16).toDouble());//iac
                          y17.append(fields.at(17).toDouble());//idle error
                          y18.append(fields.at(18).toDouble());//ignadv offset
                          y19.append(fields.at(19).toDouble());// ign adv
                          y20.append(fields.at(20).toDouble());//coil time
                          y21.append(fields.at(21).toDouble()); //cranks
                          y22.append(fields.at(22).toDouble()); //uk4
                          y23.append(fields.at(23).toDouble());//uk5
                          y24.append(fields.at(24).toDouble());//ign switch
                          y25.append(fields.at(25).toDouble());//throttle angle
                          y26.append(fields.at(26).toDouble());//uk6
                          y27.append(fields.at(27).toDouble());//air fuel ratio
                          y28.append(fields.at(28).toDouble());//fault code 0
                          double g=(fields.at(29).toDouble());//lambda volt
                          double h=g*5;
                          y29.append(h);
                          y30.append(fields.at(30).toDouble());//lamda freq
                          y31.append(fields.at(31).toDouble()); // lambdadutycycle
                          y32.append(fields.at(32).toDouble()); // lambdastatus
                          y33.append(fields.at(33).toDouble());// closed loop
                          y34.append(fields.at(34).toDouble());//long term
                          y35.append(fields.at(35).toDouble());// short term
                          y36.append(fields.at(36).toDouble()); // carbon canister
                          y37.append(fields.at(37).toDouble());//faultcode1
                          y38.append(fields.at(38).toDouble());//idle base pos
                          y39.append(fields.at(39).toDouble()); // uk7
                          y40.append(fields.at(40).toDouble());//uk8
                          y41.append(fields.at(41).toDouble()); // ignadv2
                          y42.append(fields.at(42).toDouble()); //uk9
                          y43.append(fields.at(43).toDouble());//idle error2
                          y44.append(fields.at(44).toDouble());//uk10
                          y45.append(fields.at(45).toDouble());//faultcode4
                          y46.append(fields.at(46).toDouble());//uk11
                          y47.append(fields.at(47).toDouble());//uk12
                          y48.append(fields.at(48).toDouble()); //uk13
                          y49.append(fields.at(49).toDouble());//uk14
                          y50.append(fields.at(50).toDouble());//uk15
                          y51.append(fields.at(51).toDouble());//uk16
                          y52.append(fields.at(52).toDouble());//uk17
                          y53.append(fields.at(53).toDouble());//uk18
                          y54.append(fields.at(54).toDouble());//uk19 jackcount
                          line_count++;
                      }
                      file.close();
                  }





          


    qDebug()<<"Zeile3759:x"<< x;
               
}



 //::::::::::::::::::::::::::::::::::::::::::::
            { // add the text label at the top:
            QCPItemText *textLabel2 = new QCPItemText(m_ui->logplot);
            // m_ui->logplot->removeItem(textLabel2);
             textLabel2->setPositionAlignment(Qt::AlignTop|Qt::AlignHCenter);
             textLabel2->position->setType(QCPItemPosition::ptAxisRectRatio);
             textLabel2->position->setCoords(0.6, 0); // place position at center/top of axis rect


              textLabel2->setText(Textfile );
         //  textLabel->setFont(QFont(font().family(), 16)); // make font a bit larger
             textLabel2->setPen(QPen(Qt::black)); // show black border around text
             }
    //:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
                  m_ui->logplot->graph(0)->setData( x,y1);
                  m_ui->logplot->graph(0)->rescaleAxes();
                  m_ui->logplot->graph(0)->rescaleValueAxis();

                  m_ui->logplot->graph(1)->setData( x,y2);
                  m_ui->logplot->graph(1)->rescaleAxes();
                  m_ui->logplot->graph(1)->rescaleValueAxis();
                  m_ui->logplot->graph(2)->setData( x,y3);
                  m_ui->logplot->graph(2)->rescaleAxes();
                  m_ui->logplot->graph(2)->rescaleValueAxis();
                  m_ui->logplot->graph(3)->setData( x,y4);
                  m_ui->logplot->graph(3)->rescaleAxes();
                  m_ui->logplot->graph(3)->rescaleValueAxis();
                  m_ui->logplot->graph(4)->setData( x,y5);
                  m_ui->logplot->graph(4)->rescaleAxes();
                  m_ui->logplot->graph(4)->rescaleValueAxis();
                  m_ui->logplot->graph(5)->setData( x,y6);
                  m_ui->logplot->graph(5)->rescaleAxes();
                  m_ui->logplot->graph(5)->rescaleValueAxis();
                  m_ui->logplot->graph(6)->setData( x,y7);
                  m_ui->logplot->graph(6)->rescaleAxes();
                  m_ui->logplot->graph(6)->rescaleValueAxis();
                  m_ui->logplot->graph(7)->setData( x,y8);
                  m_ui->logplot->graph(7)->rescaleAxes();
                  m_ui->logplot->graph(7)->rescaleValueAxis();
                  m_ui->logplot->graph(8)->setData( x,y9);
                  m_ui->logplot->graph(8)->rescaleAxes();
                  m_ui->logplot->graph(8)->rescaleValueAxis();
                  m_ui->logplot->graph(9)->setData( x,y10);
                  m_ui->logplot->graph(9)->rescaleAxes();
                  m_ui->logplot->graph(9)->rescaleValueAxis();
                  m_ui->logplot->graph(10)->setData( x,y11);
                  m_ui->logplot->graph(10)->rescaleAxes();
                  m_ui->logplot->graph(10)->rescaleValueAxis();
                  m_ui->logplot->graph(11)->setData( x,y12);
                  m_ui->logplot->graph(11)->rescaleAxes();
                  m_ui->logplot->graph(11)->rescaleValueAxis();
                  m_ui->logplot->graph(12)->setData( x,y13);
                  m_ui->logplot->graph(12)->rescaleAxes();
                  m_ui->logplot->graph(12)->rescaleValueAxis();
                  m_ui->logplot->graph(13)->setData( x,y14);
                  m_ui->logplot->graph(13)->rescaleAxes();
                  m_ui->logplot->graph(13)->rescaleValueAxis();
                  m_ui->logplot->graph(14)->setData( x,y15);
                  m_ui->logplot->graph(14)->rescaleAxes();
                  m_ui->logplot->graph(14)->rescaleValueAxis();
                  m_ui->logplot->graph(15)->setData( x,y16);
                  m_ui->logplot->graph(15)->rescaleAxes();
                  m_ui->logplot->graph(15)->rescaleValueAxis();
                  m_ui->logplot->graph(16)->setData( x,y17);
                  m_ui->logplot->graph(16)->rescaleAxes();
                  m_ui->logplot->graph(16)->rescaleValueAxis();
                  m_ui->logplot->graph(17)->setData( x,y18);
                  m_ui->logplot->graph(17)->rescaleAxes();
                  m_ui->logplot->graph(17)->rescaleValueAxis();
                  m_ui->logplot->graph(18)->setData( x,y19);
                  m_ui->logplot->graph(18)->rescaleAxes();
                  m_ui->logplot->graph(18)->rescaleValueAxis();
                  m_ui->logplot->graph(19)->setData( x,y20);
                  m_ui->logplot->graph(19)->rescaleAxes();
                  m_ui->logplot->graph(19)->rescaleValueAxis();
                  m_ui->logplot->graph(20)->setData( x,y21);
                  m_ui->logplot->graph(20)->rescaleAxes();
                  m_ui->logplot->graph(20)->rescaleValueAxis();
                  m_ui->logplot->graph(21)->setData( x,y22);
                  m_ui->logplot->graph(21)->rescaleAxes();
                  m_ui->logplot->graph(21)->rescaleValueAxis();
                  m_ui->logplot->graph(22)->setData( x,y23);
                  m_ui->logplot->graph(22)->rescaleAxes();
                  m_ui->logplot->graph(22)->rescaleValueAxis();
                  m_ui->logplot->graph(23)->setData( x,y24);
                  m_ui->logplot->graph(23)->rescaleAxes();
                  m_ui->logplot->graph(23)->rescaleValueAxis();
                  m_ui->logplot->graph(24)->setData( x,y25);
                  m_ui->logplot->graph(24)->rescaleAxes();
                  m_ui->logplot->graph(24)->rescaleValueAxis();
                  m_ui->logplot->graph(25)->setData( x,y26);
                  m_ui->logplot->graph(25)->rescaleAxes();
                  m_ui->logplot->graph(25)->rescaleValueAxis();
                  m_ui->logplot->graph(26)->setData( x,y27);
                  m_ui->logplot->graph(26)->rescaleAxes();
                  m_ui->logplot->graph(26)->rescaleValueAxis();
                  m_ui->logplot->graph(27)->setData( x,y28);
                  m_ui->logplot->graph(27)->rescaleAxes();
                  m_ui->logplot->graph(27)->rescaleValueAxis();
                  m_ui->logplot->graph(28)->setData( x,y29);
                  m_ui->logplot->graph(28)->rescaleAxes();
                  m_ui->logplot->graph(28)->rescaleValueAxis();
                  m_ui->logplot->graph(29)->setData( x,y30);
                  m_ui->logplot->graph(29)->rescaleAxes();
                  m_ui->logplot->graph(29)->rescaleValueAxis();
                  m_ui->logplot->graph(30)->setData( x,y31);
                  m_ui->logplot->graph(30)->rescaleAxes();
                  m_ui->logplot->graph(30)->rescaleValueAxis();
                  m_ui->logplot->graph(31)->setData( x,y32);
                  m_ui->logplot->graph(31)->rescaleAxes();
                  m_ui->logplot->graph(31)->rescaleValueAxis();
                  m_ui->logplot->graph(32)->setData( x,y33);
                  m_ui->logplot->graph(32)->rescaleAxes();
                  m_ui->logplot->graph(32)->rescaleValueAxis();
                  m_ui->logplot->graph(33)->setData( x,y34);
                  m_ui->logplot->graph(33)->rescaleAxes();
                  m_ui->logplot->graph(33)->rescaleValueAxis();
                  m_ui->logplot->graph(34)->setData( x,y35);
                  m_ui->logplot->graph(34)->rescaleAxes();
                  m_ui->logplot->graph(34)->rescaleValueAxis();
                  m_ui->logplot->graph(35)->setData( x,y36);
                  m_ui->logplot->graph(35)->rescaleAxes();
                  m_ui->logplot->graph(35)->rescaleValueAxis();
                  m_ui->logplot->graph(36)->setData( x,y37);
                  m_ui->logplot->graph(36)->rescaleAxes();
                  m_ui->logplot->graph(36)->rescaleValueAxis();
                  m_ui->logplot->graph(37)->setData( x,y38);
                  m_ui->logplot->graph(37)->rescaleAxes();
                  m_ui->logplot->graph(37)->rescaleValueAxis();
                  m_ui->logplot->graph(38)->setData( x,y39);
                  m_ui->logplot->graph(38)->rescaleAxes();
                  m_ui->logplot->graph(38)->rescaleValueAxis();
                  m_ui->logplot->graph(39)->setData( x,y40);
                  m_ui->logplot->graph(39)->rescaleAxes();
                  m_ui->logplot->graph(39)->rescaleValueAxis();
                  m_ui->logplot->graph(40)->setData( x,y41);
                  m_ui->logplot->graph(40)->rescaleAxes();
                  m_ui->logplot->graph(40)->rescaleValueAxis();
                  m_ui->logplot->graph(41)->setData( x,y42);
                  m_ui->logplot->graph(41)->rescaleAxes();
                  m_ui->logplot->graph(41)->rescaleValueAxis();
                  m_ui->logplot->graph(42)->setData( x,y43);
                  m_ui->logplot->graph(42)->rescaleAxes();
                  m_ui->logplot->graph(42)->rescaleValueAxis();
                  m_ui->logplot->graph(43)->setData( x,y44);
                  m_ui->logplot->graph(43)->rescaleAxes();
                  m_ui->logplot->graph(43)->rescaleValueAxis();
                  m_ui->logplot->graph(44)->setData( x,y45);
                  m_ui->logplot->graph(44)->rescaleAxes();
                  m_ui->logplot->graph(44)->rescaleValueAxis();
                  m_ui->logplot->graph(45)->setData( x,y46);
                  m_ui->logplot->graph(45)->rescaleAxes();
                  m_ui->logplot->graph(45)->rescaleValueAxis();
                  m_ui->logplot->graph(46)->setData( x,y47);
                  m_ui->logplot->graph(46)->rescaleAxes();
                  m_ui->logplot->graph(46)->rescaleValueAxis();
                  m_ui->logplot->graph(47)->setData( x,y48);
                  m_ui->logplot->graph(47)->rescaleAxes();
                  m_ui->logplot->graph(47)->rescaleValueAxis();
                  m_ui->logplot->graph(48)->setData( x,y49);
                  m_ui->logplot->graph(48)->rescaleAxes();
                  m_ui->logplot->graph(48)->rescaleValueAxis();
                  m_ui->logplot->graph(49)->setData( x,y50);
                  m_ui->logplot->graph(49)->rescaleAxes();
                  m_ui->logplot->graph(49)->rescaleValueAxis();
                  m_ui->logplot->graph(50)->setData( x,y51);
                  m_ui->logplot->graph(50)->rescaleAxes();
                  m_ui->logplot->graph(50)->rescaleValueAxis();
                  m_ui->logplot->graph(51)->setData( x,y52);
                  m_ui->logplot->graph(51)->rescaleAxes();
                  m_ui->logplot->graph(51)->rescaleValueAxis();
                  m_ui->logplot->graph(52)->setData( x,y53);
                  m_ui->logplot->graph(52)->rescaleAxes();
                  m_ui->logplot->graph(52)->rescaleValueAxis();
                  m_ui->logplot->graph(53)->setData( x,y54);
                  m_ui->logplot->graph(53)->rescaleAxes();
                  m_ui->logplot->graph(53)->rescaleValueAxis();
                  m_ui->logplot->graph(54)->setData( x,y55);
                  m_ui->logplot->graph(54)->rescaleAxes();
                  m_ui->logplot->graph(54)->rescaleValueAxis();
                  m_ui->logplot->graph(55)->setData( x,y1);
                  m_ui->logplot->graph(55)->rescaleAxes();
                  m_ui->logplot->graph(55)->rescaleValueAxis();
                  m_ui->logplot->graph(56)->setData( x,y56);
                  m_ui->logplot->graph(56)->rescaleAxes();
                  m_ui->logplot->graph(56)->rescaleValueAxis();
                  m_ui->logplot->graph(57)->setData( x,y57);
                  m_ui->logplot->graph(57)->rescaleAxes();
                  m_ui->logplot->graph(57)->rescaleValueAxis();
                  m_ui->logplot->graph(58)->setData( x,y58);
                  m_ui->logplot->graph(58)->rescaleAxes();
                  m_ui->logplot->graph(58)->rescaleValueAxis();
                  m_ui->logplot->graph(59)->setData( x,y59);
                  m_ui->logplot->graph(59)->rescaleAxes();
                  m_ui->logplot->graph(59)->rescaleValueAxis();
                  m_ui->logplot->graph(60)->setData( x,y60);
                  m_ui->logplot->graph(60)->rescaleAxes();
                  m_ui->logplot->graph(60)->rescaleValueAxis();



                  m_ui->logplot->replot();
                  m_ui->logplot->show();
                    m_ui->sB->setValue(0);
                    m_ui->label_text1->setText(" ");
                    m_ui->label_text2->setText(" ");
                    m_ui->label_text3->setText(" ");
                    m_ui->label_text4->setText(" ");

        }










    void MainWindow::showHint(QMouseEvent *event)
    {
        QCPAbstractPlottable *plottable = m_ui->logplot->plottableAt(event->localPos());
    if(event->button() == Qt::RightButton){
        if(plottable)
        {
            double x =m_ui-> logplot->xAxis->pixelToCoord(event->localPos().x());
            double y = m_ui->logplot->yAxis->pixelToCoord(event->localPos().y());

            QCPGraph *graph =  qobject_cast<QCPGraph*>(plottable);
            if (graph)
            {
                double key = 0;
                double value = 0;
                bool ok = false;
                double maxx = std::numeric_limits<double>::max();
                double maxy = std::numeric_limits<double>::max();

                QCPDataRange dataRange = graph->data()->dataRange();
                QCPGraphDataContainer::const_iterator begin = graph->data()->at(dataRange.begin());
                QCPGraphDataContainer::const_iterator end = graph->data()->at(dataRange.end());

                int n = end-begin;
                if (n>0)
                {
                    double *dx = new double[n];
                    double *dy = new double[n];

                    int index =0;
                    for (QCPGraphDataContainer::const_iterator it=begin; it<end; it++)
                    {
                        dx[index] = qAbs(x - it->key);
                        dy[index] = qAbs(y - it->value);
                        if ((dx[index] < maxx) && (dy[index] < maxy))
                        {
                            key = it->key;
                            value = it->value;
                            ok = true;
                            maxx = dx[index];
                            maxy = dy[index];
                        }
                        index++;
                    }
                    delete dy;
                    delete dx;

                    if (ok)
                    {
                        QToolTip::showText(event->globalPos(),
                        tr("<table>"
                             "<tr>"
                               "<td>%L1:</td>" "<td>%L2</td>"
                             "</tr>"

                          "</table>").arg("Value").arg(value),m_ui->logplot,m_ui-> logplot->rect());
                    }
                }
            }
            m_ui->logplot->replot();
        }
        else
            QToolTip::hideText();
    }
    }



    void MainWindow::setuptable_list(QTableWidget *table_list)
    {
        m_ui->table_list->setShowGrid(true);
        m_ui->table_list->setSelectionMode(QAbstractItemView::SingleSelection);
        QTableWidgetItem* tempItem = new QTableWidgetItem;
        tempItem->setFlags(tempItem->flags() | Qt::ItemIsUserCheckable);
        tempItem->setData(Qt::DisplayRole, QStringLiteral("1 rpm right axis"));
        tempItem->setData(Qt::CheckStateRole, Qt::Checked);

        m_ui->table_list->setItem(0,0,tempItem);
        tempItem = new QTableWidgetItem;
        tempItem->setFlags(tempItem->flags() | Qt::ItemIsUserCheckable);
        tempItem->setData(Qt::DisplayRole, QStringLiteral("55  rpm<1500  D "));
        tempItem->setData(Qt::CheckStateRole, Qt::Unchecked);
        m_ui->table_list->setItem(1,0,tempItem);
        tempItem = new QTableWidgetItem;
        tempItem->setFlags(tempItem->flags() | Qt::ItemIsUserCheckable);
        tempItem->setData(Qt::DisplayRole, QStringLiteral("2  watertemp  D"));
        tempItem->setData(Qt::CheckStateRole, Qt::Unchecked);
        m_ui->table_list->setItem(2,0,tempItem);
        tempItem = new QTableWidgetItem;
        tempItem->setFlags(tempItem->flags() | Qt::ItemIsUserCheckable);
        tempItem->setData(Qt::DisplayRole, QStringLiteral("3  ambient"));
        tempItem->setData(Qt::CheckStateRole, Qt::Unchecked);
        m_ui->table_list->setItem(3,0,tempItem);
        tempItem = new QTableWidgetItem;
        tempItem->setFlags(tempItem->flags() | Qt::ItemIsUserCheckable);
        tempItem->setData(Qt::DisplayRole, QStringLiteral("4  intakeAir   D"));
        tempItem->setData(Qt::CheckStateRole, Qt::Unchecked);
        m_ui->table_list->setItem(4,0,tempItem);
        tempItem = new QTableWidgetItem;
        tempItem->setFlags(tempItem->flags() | Qt::ItemIsUserCheckable);
        tempItem->setData(Qt::DisplayRole, QStringLiteral("5  fueltemp"));
        tempItem->setData(Qt::CheckStateRole, Qt::Unchecked);
        m_ui->table_list->setItem(5,0,tempItem);
        tempItem = new QTableWidgetItem;
        tempItem->setFlags(tempItem->flags() | Qt::ItemIsUserCheckable);
        tempItem->setData(Qt::DisplayRole, QStringLiteral("6  map   D"));
        tempItem->setData(Qt::CheckStateRole, Qt::Unchecked);
        m_ui->table_list->setItem(6,0,tempItem);
        tempItem = new QTableWidgetItem;
        tempItem->setFlags(tempItem->flags() | Qt::ItemIsUserCheckable);
        tempItem->setData(Qt::DisplayRole, QStringLiteral("7  battery   D"));
        tempItem->setData(Qt::CheckStateRole, Qt::Unchecked);
        m_ui->table_list->setItem(0,1,tempItem);

        tempItem = new QTableWidgetItem;
        tempItem->setFlags(tempItem->flags() | Qt::ItemIsUserCheckable);
        tempItem->setData(Qt::DisplayRole, QStringLiteral("8  throttlepot   D"));
        tempItem->setData(Qt::CheckStateRole, Qt::Unchecked);
        m_ui->table_list->setItem(1,1,tempItem);
        tempItem = new QTableWidgetItem;
        tempItem->setFlags(tempItem->flags() | Qt::ItemIsUserCheckable);
        tempItem->setData(Qt::DisplayRole, QStringLiteral("9  idleswitch   D"));
        tempItem->setData(Qt::CheckStateRole, Qt::Unchecked);
        m_ui->table_list->setItem(2,1,tempItem);
        tempItem = new QTableWidgetItem;
        tempItem->setFlags(tempItem->flags() | Qt::ItemIsUserCheckable);
        tempItem->setData(Qt::DisplayRole, QStringLiteral("10  uk1"));
        tempItem->setData(Qt::CheckStateRole, Qt::Unchecked);
        m_ui->table_list->setItem(3,1,tempItem);
        tempItem = new QTableWidgetItem;
        tempItem->setFlags(tempItem->flags() | Qt::ItemIsUserCheckable);
        tempItem->setData(Qt::DisplayRole, QStringLiteral("11  park/nut"));
        tempItem->setData(Qt::CheckStateRole, Qt::Unchecked);
        m_ui->table_list->setItem(4,1,tempItem);
        tempItem = new QTableWidgetItem;
        tempItem->setFlags(tempItem->flags() | Qt::ItemIsUserCheckable);
        tempItem->setData(Qt::DisplayRole, QStringLiteral("12  faultcodes"));
        tempItem->setData(Qt::CheckStateRole, Qt::Unchecked);
        m_ui->table_list->setItem(5,1,tempItem);
        tempItem = new QTableWidgetItem;
        tempItem->setFlags(tempItem->flags() | Qt::ItemIsUserCheckable);
        tempItem->setData(Qt::DisplayRole, QStringLiteral("13  idlesetpoint  D"));
        tempItem->setData(Qt::CheckStateRole, Qt::Unchecked);
        m_ui->table_list->setItem(6,1,tempItem);
        tempItem = new QTableWidgetItem;
        tempItem->setFlags(tempItem->flags() | Qt::ItemIsUserCheckable);
        tempItem->setData(Qt::DisplayRole, QStringLiteral("14  idlehot"));
        tempItem->setData(Qt::CheckStateRole, Qt::Unchecked);
        m_ui->table_list->setItem(0,2,tempItem);
        tempItem = new QTableWidgetItem;
        tempItem->setFlags(tempItem->flags() | Qt::ItemIsUserCheckable);
        tempItem->setData(Qt::DisplayRole, QStringLiteral("15  uk2"));
        tempItem->setData(Qt::CheckStateRole, Qt::Unchecked);
        m_ui->table_list->setItem(1,2,tempItem);

        tempItem = new QTableWidgetItem;
        tempItem->setFlags(tempItem->flags() | Qt::ItemIsUserCheckable);
        tempItem->setData(Qt::DisplayRole, QStringLiteral("16  iac   D"));
        tempItem->setData(Qt::CheckStateRole, Qt::Unchecked);
        m_ui->table_list->setItem(2,2,tempItem);
        tempItem = new QTableWidgetItem;
        tempItem->setFlags(tempItem->flags() | Qt::ItemIsUserCheckable);
        tempItem->setData(Qt::DisplayRole, QStringLiteral("17  idle deviation   D"));
        tempItem->setData(Qt::CheckStateRole, Qt::Unchecked);
        m_ui->table_list->setItem(3,2,tempItem);
        tempItem = new QTableWidgetItem;
        tempItem->setFlags(tempItem->flags() | Qt::ItemIsUserCheckable);
        tempItem->setData(Qt::DisplayRole, QStringLiteral("18  ignadvoffset"));
        tempItem->setData(Qt::CheckStateRole, Qt::Unchecked);
        m_ui->table_list->setItem(4,2,tempItem);
        tempItem = new QTableWidgetItem;
        tempItem->setFlags(tempItem->flags() | Qt::ItemIsUserCheckable);
        tempItem->setData(Qt::DisplayRole, QStringLiteral("19  ignadv   D"));
        tempItem->setData(Qt::CheckStateRole, Qt::Unchecked);
        m_ui->table_list->setItem(5,2,tempItem);
        tempItem = new QTableWidgetItem;
        tempItem->setFlags(tempItem->flags() | Qt::ItemIsUserCheckable);
        tempItem->setData(Qt::DisplayRole, QStringLiteral("20  coiltime   D"));
        tempItem->setData(Qt::CheckStateRole, Qt::Unchecked);
        m_ui->table_list->setItem(6,2,tempItem);
        tempItem = new QTableWidgetItem;
        tempItem->setFlags(tempItem->flags() | Qt::ItemIsUserCheckable);
        tempItem->setData(Qt::DisplayRole, QStringLiteral("21  crank"));
        tempItem->setData(Qt::CheckStateRole, Qt::Unchecked);
        m_ui->table_list->setItem(0,3,tempItem);
        tempItem = new QTableWidgetItem;
        tempItem->setFlags(tempItem->flags() | Qt::ItemIsUserCheckable);
        tempItem->setData(Qt::DisplayRole, QStringLiteral("22  uk4"));
        tempItem->setData(Qt::CheckStateRole, Qt::Unchecked);
        m_ui->table_list->setItem(1,3,tempItem);
        tempItem = new QTableWidgetItem;
        tempItem->setFlags(tempItem->flags() | Qt::ItemIsUserCheckable);
        tempItem->setData(Qt::DisplayRole, QStringLiteral("23  uk5"));
        tempItem->setData(Qt::CheckStateRole, Qt::Unchecked);
        m_ui->table_list->setItem(2,3,tempItem);

        tempItem = new QTableWidgetItem;
        tempItem->setFlags(tempItem->flags() | Qt::ItemIsUserCheckable);
        tempItem->setData(Qt::DisplayRole, QStringLiteral("24  ignswitch"));
        tempItem->setData(Qt::CheckStateRole, Qt::Unchecked);
        m_ui->table_list->setItem(3,3,tempItem);
        tempItem = new QTableWidgetItem;
        tempItem->setFlags(tempItem->flags() | Qt::ItemIsUserCheckable);
        tempItem->setData(Qt::DisplayRole, QStringLiteral("25  throttleangle "));
        tempItem->setData(Qt::CheckStateRole, Qt::Unchecked);
        m_ui->table_list->setItem(4,3,tempItem);
        tempItem = new QTableWidgetItem;
        tempItem->setFlags(tempItem->flags() | Qt::ItemIsUserCheckable);
        tempItem->setData(Qt::DisplayRole, QStringLiteral("26  uk6"));
        tempItem->setData(Qt::CheckStateRole, Qt::Unchecked);
        m_ui->table_list->setItem(5,3,tempItem);
        tempItem = new QTableWidgetItem;
        tempItem->setFlags(tempItem->flags() | Qt::ItemIsUserCheckable);
        tempItem->setData(Qt::DisplayRole, QStringLiteral("27  airfuelratio"));
        tempItem->setData(Qt::CheckStateRole, Qt::Unchecked);
        m_ui->table_list->setItem(6,3,tempItem);
        tempItem = new QTableWidgetItem;
        tempItem->setFlags(tempItem->flags() | Qt::ItemIsUserCheckable);
        tempItem->setData(Qt::DisplayRole, QStringLiteral("28  fc0"));
        tempItem->setData(Qt::CheckStateRole, Qt::Unchecked);
        m_ui->table_list->setItem(0,4,tempItem);
        tempItem = new QTableWidgetItem;
        tempItem->setFlags(tempItem->flags() | Qt::ItemIsUserCheckable);
        tempItem->setData(Qt::DisplayRole, QStringLiteral("29  lambda voltage   D"));
        tempItem->setData(Qt::CheckStateRole, Qt::Unchecked);
        m_ui->table_list->setItem(1,4,tempItem);
        tempItem = new QTableWidgetItem;
        tempItem->setFlags(tempItem->flags() | Qt::ItemIsUserCheckable);
        tempItem->setData(Qt::DisplayRole, QStringLiteral("30  lambda frequency"));
        tempItem->setData(Qt::CheckStateRole, Qt::Unchecked);
        m_ui->table_list->setItem(2,4,tempItem);
        tempItem = new QTableWidgetItem;
        tempItem->setFlags(tempItem->flags() | Qt::ItemIsUserCheckable);
        tempItem->setData(Qt::DisplayRole, QStringLiteral("31  lambda dudycycle"));
        tempItem->setData(Qt::CheckStateRole, Qt::Unchecked);
        m_ui->table_list->setItem(3,4,tempItem);

        tempItem = new QTableWidgetItem;
        tempItem->setFlags(tempItem->flags() | Qt::ItemIsUserCheckable);
        tempItem->setData(Qt::DisplayRole, QStringLiteral("32  lambdastatus"));
        tempItem->setData(Qt::CheckStateRole, Qt::Unchecked);
        m_ui->table_list->setItem(4,4,tempItem);
        tempItem = new QTableWidgetItem;
        tempItem->setFlags(tempItem->flags() | Qt::ItemIsUserCheckable);
        tempItem->setData(Qt::DisplayRole, QStringLiteral("33  closedloop   D"));
        tempItem->setData(Qt::CheckStateRole, Qt::Unchecked);
        m_ui->table_list->setItem(5,4,tempItem);
        tempItem = new QTableWidgetItem;
        tempItem->setFlags(tempItem->flags() | Qt::ItemIsUserCheckable);
        tempItem->setData(Qt::DisplayRole, QStringLiteral("34  ltftr"));
        tempItem->setData(Qt::CheckStateRole, Qt::Unchecked);
        m_ui->table_list->setItem(6,4,tempItem);
        tempItem = new QTableWidgetItem;
        tempItem->setFlags(tempItem->flags() | Qt::ItemIsUserCheckable);
        tempItem->setData(Qt::DisplayRole, QStringLiteral("35  stftr   D"));
        tempItem->setData(Qt::CheckStateRole, Qt::Unchecked);
        m_ui->table_list->setItem(0,5,tempItem);
        tempItem = new QTableWidgetItem;
        tempItem->setFlags(tempItem->flags() | Qt::ItemIsUserCheckable);
        tempItem->setData(Qt::DisplayRole, QStringLiteral("36  carboncanister"));
        tempItem->setData(Qt::CheckStateRole, Qt::Unchecked);
        m_ui->table_list->setItem(1,5,tempItem);
        tempItem = new QTableWidgetItem;
        tempItem->setFlags(tempItem->flags() | Qt::ItemIsUserCheckable);
        tempItem->setData(Qt::DisplayRole, QStringLiteral("37  fc1"));
        tempItem->setData(Qt::CheckStateRole, Qt::Unchecked);
        m_ui->table_list->setItem(2,5,tempItem);
        tempItem = new QTableWidgetItem;
        tempItem->setFlags(tempItem->flags() | Qt::ItemIsUserCheckable);
        tempItem->setData(Qt::DisplayRole, QStringLiteral("38  idlebase"));
        tempItem->setData(Qt::CheckStateRole, Qt::Unchecked);
        m_ui->table_list->setItem(3,5,tempItem);
        tempItem = new QTableWidgetItem;
        tempItem->setFlags(tempItem->flags() | Qt::ItemIsUserCheckable);
        tempItem->setData(Qt::DisplayRole, QStringLiteral("39  uk7"));
        tempItem->setData(Qt::CheckStateRole, Qt::Unchecked);
        m_ui->table_list->setItem(4,5,tempItem);

        tempItem = new QTableWidgetItem;
        tempItem->setFlags(tempItem->flags() | Qt::ItemIsUserCheckable);
        tempItem->setData(Qt::DisplayRole, QStringLiteral("40  uk8"));
        tempItem->setData(Qt::CheckStateRole, Qt::Unchecked);
        m_ui->table_list->setItem(5,5,tempItem);
        tempItem = new QTableWidgetItem;
        tempItem->setFlags(tempItem->flags() | Qt::ItemIsUserCheckable);
        tempItem->setData(Qt::DisplayRole, QStringLiteral("41  ignadv2"));
        tempItem->setData(Qt::CheckStateRole, Qt::Unchecked);
        m_ui->table_list->setItem(6,5,tempItem);
        tempItem = new QTableWidgetItem;
        tempItem->setFlags(tempItem->flags() | Qt::ItemIsUserCheckable);
        tempItem->setData(Qt::DisplayRole, QStringLiteral("42  uk9"));
        tempItem->setData(Qt::CheckStateRole, Qt::Unchecked);
        m_ui->table_list->setItem(0,6,tempItem);
        tempItem = new QTableWidgetItem;
        tempItem->setFlags(tempItem->flags() | Qt::ItemIsUserCheckable);
        tempItem->setData(Qt::DisplayRole, QStringLiteral("43  idle error2"));
        tempItem->setData(Qt::CheckStateRole, Qt::Unchecked);
        m_ui->table_list->setItem(1,6,tempItem);
        tempItem = new QTableWidgetItem;
        tempItem->setFlags(tempItem->flags() | Qt::ItemIsUserCheckable);
        tempItem->setData(Qt::DisplayRole, QStringLiteral("44  uk10"));
        tempItem->setData(Qt::CheckStateRole, Qt::Unchecked);
        m_ui->table_list->setItem(2,6,tempItem);
        tempItem = new QTableWidgetItem;
        tempItem->setFlags(tempItem->flags() | Qt::ItemIsUserCheckable);
        tempItem->setData(Qt::DisplayRole, QStringLiteral("45  fc4"));
        tempItem->setData(Qt::CheckStateRole, Qt::Unchecked);
        m_ui->table_list->setItem(3,6,tempItem);
        tempItem = new QTableWidgetItem;
        tempItem->setFlags(tempItem->flags() | Qt::ItemIsUserCheckable);
        tempItem->setData(Qt::DisplayRole, QStringLiteral("46  uk11"));
        tempItem->setData(Qt::CheckStateRole, Qt::Unchecked);
        m_ui->table_list->setItem(4,6,tempItem);
        tempItem = new QTableWidgetItem;
        tempItem->setFlags(tempItem->flags() | Qt::ItemIsUserCheckable);
        tempItem->setData(Qt::DisplayRole, QStringLiteral("47  uk12"));
        tempItem->setData(Qt::CheckStateRole, Qt::Unchecked);
        m_ui->table_list->setItem(5,6,tempItem);

        tempItem = new QTableWidgetItem;
        tempItem->setFlags(tempItem->flags() | Qt::ItemIsUserCheckable);
        tempItem->setData(Qt::DisplayRole, QStringLiteral("48  uk13"));
        tempItem->setData(Qt::CheckStateRole, Qt::Unchecked);
        m_ui->table_list->setItem(6,6,tempItem);
        tempItem = new QTableWidgetItem;
        tempItem->setFlags(tempItem->flags() | Qt::ItemIsUserCheckable);
        tempItem->setData(Qt::DisplayRole, QStringLiteral("49  uk14"));
        tempItem->setData(Qt::CheckStateRole, Qt::Unchecked);
        m_ui->table_list->setItem(0,7,tempItem);
        tempItem = new QTableWidgetItem;
        tempItem->setFlags(tempItem->flags() | Qt::ItemIsUserCheckable);
        tempItem->setData(Qt::DisplayRole, QStringLiteral("50  uk15"));
        tempItem->setData(Qt::CheckStateRole, Qt::Unchecked);
        m_ui->table_list->setItem(1,7,tempItem);
        tempItem = new QTableWidgetItem;
        tempItem->setFlags(tempItem->flags() | Qt::ItemIsUserCheckable);
        tempItem->setData(Qt::DisplayRole, QStringLiteral("51  uk16"));
        tempItem->setData(Qt::CheckStateRole, Qt::Unchecked);
        m_ui->table_list->setItem(2,7,tempItem);
        tempItem = new QTableWidgetItem;
        tempItem->setFlags(tempItem->flags() | Qt::ItemIsUserCheckable);
        tempItem->setData(Qt::DisplayRole, QStringLiteral("52  uk17"));
        tempItem->setData(Qt::CheckStateRole, Qt::Unchecked);
        m_ui->table_list->setItem(3,7,tempItem);
        tempItem = new QTableWidgetItem;
        tempItem->setFlags(tempItem->flags() | Qt::ItemIsUserCheckable);
        tempItem->setData(Qt::DisplayRole, QStringLiteral("53  uk18"));
        tempItem->setData(Qt::CheckStateRole, Qt::Unchecked);
        m_ui->table_list->setItem(4,7,tempItem);
        tempItem = new QTableWidgetItem;
        tempItem->setFlags(tempItem->flags() | Qt::ItemIsUserCheckable);
        tempItem->setData(Qt::DisplayRole, QStringLiteral("54  jack count"));
        tempItem->setData(Qt::CheckStateRole, Qt::Unchecked);
        m_ui->table_list->setItem(5,7,tempItem);
        tempItem = new QTableWidgetItem;
        tempItem->setFlags(tempItem->flags() | Qt::ItemIsUserCheckable);
        tempItem->setData(Qt::DisplayRole, QStringLiteral("55 rpm-low(mems-diag) "));
        tempItem->setData(Qt::CheckStateRole, Qt::Unchecked);
        m_ui->table_list->setItem(6,7,tempItem);
    }



    void MainWindow::cellSelectedSlot(int nRow, int nCol)
    {


        int Reihe=nRow ;
        int Spalte=nCol;
     //   qDebug()<<Reihe <<Spalte;
     QString str2=" ";

       //1:::::::::::::::::::::::::::::::::::::::::::::::::
        if ((Reihe== 0)&&(Spalte==0)&&(m_ui->sB->value()<=4))
        {
                    if (m_ui->table_list->model()->index(Reihe,Spalte).data(Qt::CheckStateRole)==Qt::Checked)
                                {
                                     m_ui->logplot->graph(0)->setName("rpm");
                                      m_ui->logplot->legend->item(0)->setVisible(true);
                                     m_ui->logplot->graph(0)->setVisible(true);
                                     m_ui->logplot->graph(0)->setPen(QPen(Qt::black));
                                     m_ui->logplot->yAxis2->setVisible(true);
                                     m_ui->logplot->yAxis2->setRange(0,6500);
                                      m_ui->logplot->yAxis->setRange(0,300);
                                     m_ui->logplot->yAxis2->atRight;
                                  //   m_ui->logplot->yAxis2->setTickLabelFont(font);
                                     m_ui->logplot->yAxis2->setTickLabels(true);
                                     m_ui->logplot->replot();

                                }

                              else
                                {

                                    m_ui->logplot->graph(0)->setVisible(false);
                                    m_ui->logplot->legend->item(0)->setVisible(false);
                                    m_ui->logplot->replot();
                                }
                          }
        //2:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
     else if (((Reihe== 1)&&(Spalte==0))&&(m_ui->sB->value()<=4))
               {

                    if ((m_ui->table_list->model()->index(Reihe,Spalte).data(Qt::CheckStateRole)==Qt::Checked)&&(m_ui->sB->value()<4))
                                {
                                    m_ui->sB->setValue(m_ui->sB->value()+1);
                                    m_ui->logplot->graph(55)->setName("rpm<1500");
                                    m_ui->logplot->legend->item(55)->setVisible(true);
                                    m_ui->logplot->graph(55)->setVisible(true);
                                     m_ui->logplot->graph(55)->setVisible(true);
                                    switch (m_ui->sB->value()){
                                    case 1:  m_ui->logplot->graph(55)->setPen(QPen(Qt::blue));
                                         m_ui->label_text5->setText("blue");break;
                                    case 2: m_ui->logplot->graph(55)->setPen(QPen(Qt::cyan));
                                        m_ui->label_text6->setText("cyan");break;
                                    case 3: m_ui->logplot->graph(55)->setPen(QPen(Qt::red));
                                        m_ui->label_text7->setText("red");break;
                                    case 4: m_ui->logplot->graph(55)->setPen(QPen(Qt::green));
                                        m_ui->label_text8->setText("green");break;
                                    }
                                    m_ui->logplot->yAxis->setVisible(true);
                                   m_ui->logplot->yAxis->setRange(0,2000);
                                    m_ui->logplot->yAxis->atLeft;
                                    //   m_ui->logplot->yAxis2->setTickLabelFont(font);
                                    m_ui->logplot->yAxis->setTickLabels(true);
                                    m_ui->logplot->graph(55)->rescaleValueAxis(true);
                                    m_ui->logplot->replot();
                                    str=" ";

                                    for (int i = 0; i < y1.size(); ++i)
                                        {
                                         if (i > 0)
                                            str += " ";
                                            str += QString::number(y1[i]);
                                        }
                                        switch (m_ui->sB->value()){

                                         case 1 : m_ui->text_1->setText(str2);
                                                m_ui->text_1->setText(str);
                                                m_ui->label_text1->setText(m_ui->logplot->graph(55)->name());break;
                                          case 2 : m_ui->text_2->setText(str2);
                                                  m_ui->text_2->setText(str);
                                                  m_ui->label_text2->setText(m_ui->logplot->graph(55)->name());break;
                                         case 3 : m_ui->text_3->setText(str2);
                                                 m_ui->text_3->setText(str);
                                                 m_ui->label_text3->setText(m_ui->logplot->graph(55)->name());break;
                                         case 4 : m_ui->text_4->setText(str2);
                                                m_ui->text_4->setText(str);
                                                m_ui->label_text4->setText(m_ui->logplot->graph(55)->name());break;
                                                 break;return;
                                        }
                                }
                                else
                                  {
                                       switch (m_ui->sB->value())
                                       {

                                        case 1 : m_ui->text_1->setText(str2);
                                                 m_ui->label_text1->setText("");break;
                                        case 2 : m_ui->text_2->setText(str2);
                                                 m_ui->label_text2->setText("");break;
                                        case 3 : m_ui->text_3->setText(str2);
                                                 m_ui->label_text3->setText("");break;
                                        case 4 : m_ui->text_4->setText(str2);
                                                 m_ui->label_text4->setText("");break;
                                                 break;return;
                                       }
                                    m_ui->logplot->graph(55)->setVisible(false);
                                    m_ui->logplot->legend->item(55)->setVisible(false);
                                    m_ui->sB->setValue(m_ui->sB->value()-1);
                                    m_ui->logplot->replot();
                                  }
                             }


    //2::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
     else if (((Reihe== 2)&&(Spalte==0))&&(m_ui->sB->value()<=4))
        {

            if ((m_ui->table_list->model()->index(Reihe,Spalte).data(Qt::CheckStateRole)==Qt::Checked)&&(m_ui->sB->value()<4))
                {

                    m_ui->sB->setValue(m_ui->sB->value()+1);
                    m_ui->logplot->graph(1)->setName("Watertemp");
                    m_ui->logplot->legend->item(1)->setVisible(true);
                    m_ui->logplot->graph(1)->setVisible(true);
                        switch (m_ui->sB->value()){
                                    case 1:  m_ui->logplot->graph(1)->setPen(QPen(Qt::blue));
                                         m_ui->label_text5->setText("blue");break;
                                    case 2: m_ui->logplot->graph(1)->setPen(QPen(Qt::cyan));
                                        m_ui->label_text6->setText("cyan");break;
                                    case 3: m_ui->logplot->graph(1)->setPen(QPen(Qt::red));
                                        m_ui->label_text7->setText("red");break;
                                    case 4: m_ui->logplot->graph(1)->setPen(QPen(Qt::green));
                                        m_ui->label_text8->setText("green");break;
                                    }
                    m_ui->logplot->yAxis->setVisible(true);
                    m_ui->logplot->yAxis->setRange(0,100);
                    m_ui->logplot->yAxis->atLeft;
                    //   m_ui->logplot->yAxis2->setTickLabelFont(font);
                    m_ui->logplot->yAxis->setTickLabels(true);
                    m_ui->logplot->graph(1)->rescaleValueAxis(true);
                    m_ui->logplot->replot();
                    str=" ";

                    for (int i = 0; i < y2.size(); ++i)
                        {
                         if (i > 0)
                            str += " ";
                            str += QString::number(y2[i]);
                        }
                                              switch (m_ui->sB->value()){

                                         case 1 : m_ui->text_1->setText(str2);
                                                m_ui->text_1->setText(str);
                                                m_ui->label_text1->setText(m_ui->logplot->graph(1)->name());break;
                                          case 2 : m_ui->text_2->setText(str2);
                                                  m_ui->text_2->setText(str);
                                                  m_ui->label_text2->setText(m_ui->logplot->graph(1)->name());break;
                                         case 3 : m_ui->text_3->setText(str2);
                                                 m_ui->text_3->setText(str);
                                                 m_ui->label_text3->setText(m_ui->logplot->graph(1)->name());break;
                                         case 4 : m_ui->text_4->setText(str2);
                                                m_ui->text_4->setText(str);
                                                m_ui->label_text4->setText(m_ui->logplot->graph(1)->name());break;
                                                 break;return;
                                        }

                }
                else
                  {
                switch (m_ui->sB->value())
                {

                 case 1 : m_ui->text_1->setText(str2);
                          m_ui->label_text1->setText("");break;
                 case 2 : m_ui->text_2->setText(str2);
                          m_ui->label_text2->setText("");break;
                 case 3 : m_ui->text_3->setText(str2);
                          m_ui->label_text3->setText("");break;
                 case 4 : m_ui->text_4->setText(str2);
                          m_ui->label_text4->setText("");break;
                          break;return;
                }
                    m_ui->logplot->graph(1)->setVisible(false);
                    m_ui->logplot->legend->item(1)->setVisible(false);
                     m_ui->sB->setValue(m_ui->sB->value()-1);
                    qDebug()<<m_ui->sB->value();
                    m_ui->logplot->replot();
                  }
            }

    //3::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
     else if (((Reihe== 3)&&(Spalte==0))&&(m_ui->sB->value()<=4))
        {

        if( (m_ui->table_list->model()->index(Reihe,Spalte).data(Qt::CheckStateRole)==Qt::Checked)&&(m_ui->sB->value()<4))
          {

            m_ui->sB->setValue(m_ui->sB->value()+1);
            m_ui->logplot->graph(2)->setVisible(true);
            m_ui->logplot->graph(2)->setName("ambientAir");
            m_ui->logplot->legend->item(2)->setVisible(true);
            m_ui->logplot->graph(2)->valueAxis()->setRange(0,100);
                    switch (m_ui->sB->value()){
                                    case 1:  m_ui->logplot->graph(2)->setPen(QPen(Qt::blue));
                                         m_ui->label_text5->setText("blue");break;
                                    case 2: m_ui->logplot->graph(2)->setPen(QPen(Qt::cyan));
                                        m_ui->label_text6->setText("cyan");break;
                                    case 3: m_ui->logplot->graph(2)->setPen(QPen(Qt::red));
                                        m_ui->label_text7->setText("red");break;
                                    case 4: m_ui->logplot->graph(2)->setPen(QPen(Qt::green));
                                        m_ui->label_text8->setText("green");break;
                                    }

            m_ui->logplot->graph(2)->rescaleValueAxis(true);
            m_ui->logplot->replot();
            str=" ";
            for (int i = 0; i < y3.size(); ++i)
                        {
                         if (i > 0)
                            str += " ";
                            str += QString::number(y3[i]);
                        }
                                        switch (m_ui->sB->value()){

                                         case 1 : m_ui->text_1->setText(str2);
                                                m_ui->text_1->setText(str);
                                                m_ui->label_text1->setText(m_ui->logplot->graph(2)->name());break;
                                          case 2 : m_ui->text_2->setText(str2);
                                                  m_ui->text_2->setText(str);
                                                  m_ui->label_text2->setText(m_ui->logplot->graph(2)->name());break;
                                         case 3 : m_ui->text_3->setText(str2);
                                                 m_ui->text_3->setText(str);
                                                 m_ui->label_text3->setText(m_ui->logplot->graph(2)->name());break;
                                         case 4 : m_ui->text_4->setText(str2);
                                                m_ui->text_4->setText(str);
                                                m_ui->label_text4->setText(m_ui->logplot->graph(2)->name());break;
                                                 break;return;
                                        }

         }
            else
            {
            switch (m_ui->sB->value())
            {

             case 1 : m_ui->text_1->setText(str2);
                      m_ui->label_text1->setText("");break;
             case 2 : m_ui->text_2->setText(str2);
                      m_ui->label_text2->setText("");break;
             case 3 : m_ui->text_3->setText(str2);
                      m_ui->label_text3->setText("");break;
             case 4 : m_ui->text_4->setText(str2);
                      m_ui->label_text4->setText("");break;
                      break;return;
            }
                         m_ui->logplot->graph(2)->setVisible(false);
                        m_ui->logplot->legend->item(2)->setVisible(false);
                         m_ui->sB->setValue(m_ui->sB->value()-1);
                        qDebug()<<m_ui->sB->value();
                        m_ui->logplot->replot();

            }
     }
    //4::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
        else if (((Reihe== 4)&&(Spalte==0))&&(m_ui->sB->value()<=4))
           {

                    if((m_ui->table_list->model()->index(Reihe,Spalte).data(Qt::CheckStateRole)==Qt::Checked)&&(m_ui->sB->value()<4))
                    {

                        m_ui->sB->setValue(m_ui->sB->value()+1);
                        m_ui->logplot->graph(3)->setVisible(true);
                        m_ui->logplot->graph(3)->setName("IntakeAir");
                        m_ui->logplot->legend->item(3)->setVisible(true);
                        m_ui->logplot->graph(3)->valueAxis()->setRange(0,100);
                             switch (m_ui->sB->value()){
                                    case1:  m_ui->logplot->graph(3)->setPen(QPen(Qt::blue));
                                         m_ui->label_text5->setText("blue");break;
                                    case 2: m_ui->logplot->graph(3)->setPen(QPen(Qt::cyan));
                                        m_ui->label_text6->setText("cyan");break;
                                    case 3: m_ui->logplot->graph(3)->setPen(QPen(Qt::red));
                                        m_ui->label_text7->setText("red");break;
                                    case 4: m_ui->logplot->graph(3)->setPen(QPen(Qt::green));
                                        m_ui->label_text8->setText("green");break;
                                    }

                        m_ui->logplot->graph(3)->rescaleValueAxis(true);
                        m_ui->logplot->replot();
                        str="";
                        for (int i = 0; i < y4.size(); ++i)
                        {
                         if (i > 0)
                            str += " ";
                            str += QString::number(y4[i]);
                        }
                                                switch (m_ui->sB->value()){

                                         case 1 : m_ui->text_1->setText(str2);
                                                m_ui->text_1->setText(str);
                                                m_ui->label_text1->setText(m_ui->logplot->graph(3)->name());break;
                                          case 2 : m_ui->text_2->setText(str2);
                                                  m_ui->text_2->setText(str);
                                                  m_ui->label_text2->setText(m_ui->logplot->graph(3)->name());break;
                                         case 3 : m_ui->text_3->setText(str2);
                                                 m_ui->text_3->setText(str);
                                                 m_ui->label_text3->setText(m_ui->logplot->graph(3)->name());break;
                                         case 4 : m_ui->text_4->setText(str2);
                                                m_ui->text_4->setText(str);
                                                m_ui->label_text4->setText(m_ui->logplot->graph(3)->name());break;
                                                 break;return;
                                        }

                    }
                    else
                        {
                        switch (m_ui->sB->value())
                        {

                         case 1 : m_ui->text_1->setText(str2);
                                  m_ui->label_text1->setText("");break;
                         case 2 : m_ui->text_2->setText(str2);
                                  m_ui->label_text2->setText("");break;
                         case 3 : m_ui->text_3->setText(str2);
                                  m_ui->label_text3->setText("");break;
                         case 4 : m_ui->text_4->setText(str2);
                                  m_ui->label_text4->setText("");break;
                                  break;return;
                        }
                            m_ui->logplot->graph(3)->setVisible(false);
                            m_ui->logplot->legend->item(3)->setVisible(false);
                            m_ui->sB->setValue(m_ui->sB->value()-1);
                            m_ui->logplot->replot();

                    }

                }
    //5:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
        else if (((Reihe== 5)&&(Spalte==0))&&(m_ui->sB->value()<=4))
           {

            if((m_ui->table_list->model()->index(Reihe,Spalte).data(Qt::CheckStateRole)==Qt::Checked)&&(m_ui->sB->value()<4))
            {

               m_ui->sB->setValue(m_ui->sB->value()+1);
                    m_ui->logplot->graph(4)->setVisible(true);
                m_ui->logplot->graph(4)->setName("fuelTemp");
                m_ui->logplot->yAxis->setRange(0,100);
                m_ui->logplot->legend->item(4)->setVisible(true);
                  switch (m_ui->sB->value()){
                                    case 1:  m_ui->logplot->graph(4)->setPen(QPen(Qt::blue));
                                         m_ui->label_text5->setText("blue");break;
                                    case 2: m_ui->logplot->graph(4)->setPen(QPen(Qt::cyan));
                                        m_ui->label_text6->setText("cyan");break;
                                    case 3: m_ui->logplot->graph(4)->setPen(QPen(Qt::red));
                                        m_ui->label_text7->setText("red");break;
                                    case 4: m_ui->logplot->graph(4)->setPen(QPen(Qt::green));
                                        m_ui->label_text8->setText("green");break;
                                    }
                    m_ui->logplot->graph(4)->setVisible(true);
                    m_ui->logplot->graph(4)->setName("fueltemp");
            //        m_ui->logplot->graph(4)->valueAxis()->setRange(0,100);
                    m_ui->logplot->graph(4)->setPen(QPen(Qt::green));
                    m_ui->logplot->graph(4)->rescaleValueAxis(true);
                    m_ui->logplot->replot();
                    str=" ";
                        for (int i = 0; i < y5.size(); ++i)
                        {
                         if (i > 0)
                            str += " ";
                            str += QString::number(y5[i]);
                        }
                                                      switch (m_ui->sB->value()){

                                         case 1 : m_ui->text_1->setText(str2);
                                                m_ui->text_1->setText(str);
                                                m_ui->label_text1->setText(m_ui->logplot->graph(4)->name());break;
                                          case 2 : m_ui->text_2->setText(str2);
                                                  m_ui->text_2->setText(str);
                                                  m_ui->label_text2->setText(m_ui->logplot->graph(4)->name());break;
                                         case 3 : m_ui->text_3->setText(str2);
                                                 m_ui->text_3->setText(str);
                                                 m_ui->label_text3->setText(m_ui->logplot->graph(4)->name());break;
                                         case 4 : m_ui->text_4->setText(str2);
                                                m_ui->text_4->setText(str);
                                                m_ui->label_text4->setText(m_ui->logplot->graph(4)->name());break;
                                                 break;return;
                                        }

                }
             else
             {
                switch (m_ui->sB->value())
                {

                 case 1 : m_ui->text_1->setText(str2);
                          m_ui->label_text1->setText("");break;
                 case 2 : m_ui->text_2->setText(str2);
                          m_ui->label_text2->setText("");break;
                 case 3 : m_ui->text_3->setText(str2);
                          m_ui->label_text3->setText("");break;
                 case 4 : m_ui->text_4->setText(str2);
                          m_ui->label_text4->setText("");break;
                          break;return;
                }
                m_ui->logplot->graph(4)->setVisible(false);
                m_ui->logplot->legend->item(4)->setVisible(false);
                m_ui->sB->setValue(m_ui->sB->value()-1);
                qDebug()<<m_ui->sB->value();
                m_ui->logplot->replot();

                }
        }

    //6::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
        else if (((Reihe== 6)&&(Spalte==0))&&(m_ui->sB->value()<=4))
           {

            if( (m_ui->table_list->model()->index(Reihe,Spalte).data(Qt::CheckStateRole)==Qt::Checked)&&(m_ui->sB->value()<4))

            { m_ui->sB->setValue(m_ui->sB->value()+1);

                m_ui->logplot->graph(5)->setName("Map");
                m_ui->logplot->legend->item(5)->setVisible(true);
                m_ui->logplot->graph(5)->setVisible(true);
                m_ui->logplot->graph(5)->valueAxis()->setRange(0,100);
                   switch (m_ui->sB->value()){
                                    case 1:  m_ui->logplot->graph(5)->setPen(QPen(Qt::blue));
                                         m_ui->label_text5->setText("blue");break;
                                    case 2: m_ui->logplot->graph(5)->setPen(QPen(Qt::cyan));
                                        m_ui->label_text6->setText("cyan");break;
                                    case 3: m_ui->logplot->graph(5)->setPen(QPen(Qt::red));
                                        m_ui->label_text7->setText("red");break;
                                    case 4: m_ui->logplot->graph(5)->setPen(QPen(Qt::green));
                                        m_ui->label_text8->setText("green");break;
                                    }

                m_ui->logplot->graph(5)->rescaleValueAxis(true);
                m_ui->logplot->replot();
                str=" ";
                for (int i = 0; i < y6.size(); ++i)
                {
                 if (i > 0)
                    str += " ";
                    str += QString::number(y6[i]);
                }
                                        switch (m_ui->sB->value()){

                                         case 1 : m_ui->text_1->setText(str2);
                                                m_ui->text_1->setText(str);
                                                m_ui->label_text1->setText(m_ui->logplot->graph(5)->name());break;
                                          case 2 : m_ui->text_2->setText(str2);
                                                  m_ui->text_2->setText(str);
                                                  m_ui->label_text2->setText(m_ui->logplot->graph(5)->name());break;
                                         case 3 : m_ui->text_3->setText(str2);
                                                 m_ui->text_3->setText(str);
                                                 m_ui->label_text3->setText(m_ui->logplot->graph(5)->name());break;
                                         case 4 : m_ui->text_4->setText(str2);
                                                m_ui->text_4->setText(str);
                                                m_ui->label_text4->setText(m_ui->logplot->graph(5)->name());break;
                                                 break;return;
                                        }
                }
            else
            {
                switch (m_ui->sB->value())
                {

                 case 1 : m_ui->text_1->setText(str2);
                          m_ui->label_text1->setText("");break;
                 case 2 : m_ui->text_2->setText(str2);
                          m_ui->label_text2->setText("");break;
                 case 3 : m_ui->text_3->setText(str2);
                          m_ui->label_text3->setText("");break;
                 case 4 : m_ui->text_4->setText(str2);
                          m_ui->label_text4->setText("");break;
                          break;return;
                }
                m_ui->logplot->graph(5)->setVisible(false);
                m_ui->logplot->legend->item(5)->setVisible(false);
                m_ui->sB->setValue(m_ui->sB->value()-1);
                qDebug()<<m_ui->sB->value();
                m_ui->logplot->replot();
           }
        }

        //7::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
    else if (((Reihe== 0)&&(Spalte==1))&&(m_ui->sB->value()<=4))
        {

            if( (m_ui->table_list->model()->index(Reihe,Spalte).data(Qt::CheckStateRole)==Qt::Checked)&&(m_ui->sB->value()<4))
         {m_ui->sB->setValue(m_ui->sB->value()+1);


                m_ui->logplot->graph(6)->setName("Battery voltage");
                 m_ui->logplot->graph(5)->valueAxis()->setRange(0,20);
                m_ui->logplot->legend->item(6)->setVisible(true);
                m_ui->logplot->graph(6)->setVisible(true);
                      switch (m_ui->sB->value()){
                                    case 1:  m_ui->logplot->graph(6)->setPen(QPen(Qt::blue));
                                         m_ui->label_text5->setText("blue");break;
                                    case 2: m_ui->logplot->graph(6)->setPen(QPen(Qt::cyan));
                                        m_ui->label_text6->setText("cyan");break;
                                    case 3: m_ui->logplot->graph(6)->setPen(QPen(Qt::red));
                                        m_ui->label_text7->setText("red");break;
                                    case 4: m_ui->logplot->graph(6)->setPen(QPen(Qt::green));
                                        m_ui->label_text8->setText("green");break;
                                    }

                m_ui->logplot->graph(6)->rescaleValueAxis(true);
                m_ui->logplot->replot();
                str=" ";
                for (int i = 0; i < y7.size(); ++i)
                {
                 if (i > 0)
                    str += " ";
                    str += QString::number(y7[i]);
                }
                                        switch (m_ui->sB->value()){

                                         case 1 : m_ui->text_1->setText(str2);
                                                m_ui->text_1->setText(str);
                                                m_ui->label_text1->setText(m_ui->logplot->graph(6)->name());break;
                                          case 2 : m_ui->text_2->setText(str2);
                                                  m_ui->text_2->setText(str);
                                                  m_ui->label_text2->setText(m_ui->logplot->graph(6)->name());break;
                                         case 3 : m_ui->text_3->setText(str2);
                                                 m_ui->text_3->setText(str);
                                                 m_ui->label_text3->setText(m_ui->logplot->graph(6)->name());break;
                                         case 4 : m_ui->text_4->setText(str2);
                                                m_ui->text_4->setText(str);
                                                m_ui->label_text4->setText(m_ui->logplot->graph(6)->name());break;
                                                 break;return;
                                        }
                }
           else{
                switch (m_ui->sB->value())
                {

                 case 1 : m_ui->text_1->setText(str2);
                          m_ui->label_text1->setText("");break;
                 case 2 : m_ui->text_2->setText(str2);
                          m_ui->label_text2->setText("");break;
                 case 3 : m_ui->text_3->setText(str2);
                          m_ui->label_text3->setText("");break;
                 case 4 : m_ui->text_4->setText(str2);
                          m_ui->label_text4->setText("");break;
                          break;return;
                }
                m_ui->logplot->graph(6)->setVisible(false);
                m_ui->logplot->legend->item(6)->setVisible(false);
                m_ui->sB->setValue(m_ui->sB->value()-1);
                m_ui->logplot->replot();

          }
     }
     //8:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
    else if (((Reihe== 1)&&(Spalte==1))&&(m_ui->sB->value()<=4))
        {

            if( (m_ui->table_list->model()->index(Reihe,Spalte).data(Qt::CheckStateRole)==Qt::Checked)&&(m_ui->sB->value()<4))
         { m_ui->sB->setValue(m_ui->sB->value()+1);


             m_ui->logplot->graph(7)->setName("Throttle Pot");
                m_ui->logplot->graph(5)->valueAxis()->setRange(0,10);
                m_ui->logplot->graph(7)->setVisible(true);
             m_ui->logplot->legend->item(7)->setVisible(true);
                    switch (m_ui->sB->value()){
                                    case 1:  m_ui->logplot->graph(7)->setPen(QPen(Qt::blue));
                                         m_ui->label_text5->setText("blue");break;
                                    case 2: m_ui->logplot->graph(7)->setPen(QPen(Qt::cyan));
                                        m_ui->label_text6->setText("cyan");break;
                                    case 3: m_ui->logplot->graph(7)->setPen(QPen(Qt::red));
                                        m_ui->label_text7->setText("red");break;
                                    case 4: m_ui->logplot->graph(7)->setPen(QPen(Qt::green));
                                        m_ui->label_text8->setText("green");break;
                                    }

             m_ui->logplot->graph(7)->rescaleValueAxis(true);
             m_ui->logplot->replot();
             str="";
                 for (int i = 0; i < y8.size(); ++i)
                {
                 if (i > 0)
                    str += " ";
                    str += QString::number(y8[i]);
                }
                                             switch (m_ui->sB->value()){

                                         case 1 : m_ui->text_1->setText(str2);
                                                m_ui->text_1->setText(str);
                                                m_ui->label_text1->setText(m_ui->logplot->graph(7)->name());break;
                                          case 2 : m_ui->text_2->setText(str2);
                                                  m_ui->text_2->setText(str);
                                                  m_ui->label_text2->setText(m_ui->logplot->graph(7)->name());break;
                                         case 3 : m_ui->text_3->setText(str2);
                                                 m_ui->text_3->setText(str);
                                                 m_ui->label_text3->setText(m_ui->logplot->graph(7)->name());break;
                                         case 4 : m_ui->text_4->setText(str2);
                                                m_ui->text_4->setText(str);
                                                m_ui->label_text4->setText(m_ui->logplot->graph(7)->name());break;
                                                 break;return;
                                        }
             }
          else{
                switch (m_ui->sB->value())
                {

                 case 1 : m_ui->text_1->setText(str2);
                          m_ui->label_text1->setText("");break;
                 case 2 : m_ui->text_2->setText(str2);
                          m_ui->label_text2->setText("");break;
                 case 3 : m_ui->text_3->setText(str2);
                          m_ui->label_text3->setText("");break;
                 case 4 : m_ui->text_4->setText(str2);
                          m_ui->label_text4->setText("");break;
                          break;return;
                }
                m_ui->logplot->graph(7)->setVisible(false);
          m_ui->logplot->legend->item(7)->setVisible(false);
            m_ui->sB->setValue(m_ui->sB->value()-1);

               m_ui->logplot->replot();

               }
     }

    //9::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
    else if (((Reihe== 2)&&(Spalte==1))&&(m_ui->sB->value()<=4))
        {

            if( (m_ui->table_list->model()->index(Reihe,Spalte).data(Qt::CheckStateRole)==Qt::Checked)&&(m_ui->sB->value()<4))
         { m_ui->sB->setValue(m_ui->sB->value()+1);

                 m_ui->logplot->graph(8)->setName("Idle Switch");
                m_ui->logplot->legend->item(8)->setVisible(true);
                 m_ui->logplot->graph(8)->setVisible(true);
                       switch (m_ui->sB->value()){
                                    case 1:  m_ui->logplot->graph(8)->setPen(QPen(Qt::blue));
                                         m_ui->label_text5->setText("blue");break;
                                    case 2: m_ui->logplot->graph(8)->setPen(QPen(Qt::cyan));
                                        m_ui->label_text6->setText("cyan");break;
                                    case 3: m_ui->logplot->graph(8)->setPen(QPen(Qt::red));
                                        m_ui->label_text7->setText("red");break;
                                    case 4: m_ui->logplot->graph(8)->setPen(QPen(Qt::green));
                                        m_ui->label_text8->setText("green");break;
                                    }

                 m_ui->logplot->graph(8)->rescaleValueAxis(true);
                 m_ui->logplot->replot();
                       str="";
                 for (int i = 0; i < y9.size(); ++i)
                {
                 if (i > 0)
                    str += " ";
                    str += QString::number(y9[i]);
                }
                                             switch (m_ui->sB->value()){

                                         case 1 : m_ui->text_1->setText(str2);
                                                m_ui->text_1->setText(str);
                                                m_ui->label_text1->setText(m_ui->logplot->graph(8)->name());break;
                                          case 2 : m_ui->text_2->setText(str2);
                                                  m_ui->text_2->setText(str);
                                                  m_ui->label_text2->setText(m_ui->logplot->graph(8)->name());break;
                                         case 3 : m_ui->text_3->setText(str2);
                                                 m_ui->text_3->setText(str);
                                                 m_ui->label_text3->setText(m_ui->logplot->graph(8)->name());break;
                                         case 4 : m_ui->text_4->setText(str2);
                                                m_ui->text_4->setText(str);
                                                m_ui->label_text4->setText(m_ui->logplot->graph(8)->name());break;
                                                 break;return;
                                        }
            }
          else{
                switch (m_ui->sB->value())
                {

                 case 1 : m_ui->text_1->setText(str2);
                          m_ui->label_text1->setText("");break;
                 case 2 : m_ui->text_2->setText(str2);
                          m_ui->label_text2->setText("");break;
                 case 3 : m_ui->text_3->setText(str2);
                          m_ui->label_text3->setText("");break;
                 case 4 : m_ui->text_4->setText(str2);
                          m_ui->label_text4->setText("");break;
                          break;return;
                }
                m_ui->logplot->graph(8)->setVisible(false);
               m_ui->logplot->legend->item(8)->setVisible(false);
               m_ui->sB->setValue(m_ui->sB->value()-1);
               m_ui->logplot->replot();

               }
    }
    //10::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
    else if (((Reihe== 3)&&(Spalte==1))&&(m_ui->sB->value()<=4))
        {

            if( (m_ui->table_list->model()->index(Reihe,Spalte).data(Qt::CheckStateRole)==Qt::Checked)&&(m_ui->sB->value()<4))
         { m_ui->sB->setValue(m_ui->sB->value()+1);


         m_ui->logplot->graph(9)->setName("unknown1");
              m_ui->logplot->legend->item(9)->setVisible(true);
         m_ui->logplot->graph(9)->setVisible(true);
                  switch (m_ui->sB->value()){
                                    case 1:  m_ui->logplot->graph(9)->setPen(QPen(Qt::blue));
                                         m_ui->label_text5->setText("blue");break;
                                    case 2: m_ui->logplot->graph(9)->setPen(QPen(Qt::cyan));
                                        m_ui->label_text6->setText("cyan");break;
                                    case 3: m_ui->logplot->graph(9)->setPen(QPen(Qt::red));
                                        m_ui->label_text7->setText("red");break;
                                    case 4: m_ui->logplot->graph(9)->setPen(QPen(Qt::green));
                                        m_ui->label_text8->setText("green");break;
                                    }

          m_ui->logplot->graph(9)->rescaleValueAxis(true);
            m_ui->logplot->replot();
                  str="";
                 for (int i = 0; i < y10.size(); ++i)
                {
                 if (i > 0)
                    str += " ";
                    str += QString::number(y10[i]);
                }
                                             switch (m_ui->sB->value()){

                                         case 1 : m_ui->text_1->setText(str2);
                                                m_ui->text_1->setText(str);
                                                m_ui->label_text1->setText(m_ui->logplot->graph(9)->name());break;
                                          case 2 : m_ui->text_2->setText(str2);
                                                  m_ui->text_2->setText(str);
                                                  m_ui->label_text2->setText(m_ui->logplot->graph(9)->name());break;
                                         case 3 : m_ui->text_3->setText(str2);
                                                 m_ui->text_3->setText(str);
                                                 m_ui->label_text3->setText(m_ui->logplot->graph(9)->name());break;
                                         case 4 : m_ui->text_4->setText(str2);
                                                m_ui->text_4->setText(str);
                                                m_ui->label_text4->setText(m_ui->logplot->graph(9)->name());break;
                                                 break;return;
                                        }
            }

          else{
                switch (m_ui->sB->value())
                {

                 case 1 : m_ui->text_1->setText(str2);
                          m_ui->label_text1->setText("");break;
                 case 2 : m_ui->text_2->setText(str2);
                          m_ui->label_text2->setText("");break;
                 case 3 : m_ui->text_3->setText(str2);
                          m_ui->label_text3->setText("");break;
                 case 4 : m_ui->text_4->setText(str2);
                          m_ui->label_text4->setText("");break;
                          break;return;
                }
                m_ui->logplot->graph(9)->setVisible(false);
            m_ui->logplot->legend->item(9)->setVisible(false);
       m_ui->sB->setValue(m_ui->sB->value()-1);
         m_ui->logplot->replot();

         }
    }

    //11:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
    else if (((Reihe== 4)&&(Spalte==1))&&(m_ui->sB->value()<=4))
        {

            if( (m_ui->table_list->model()->index(Reihe,Spalte).data(Qt::CheckStateRole)==Qt::Checked)&&(m_ui->sB->value()<4))
         { m_ui->sB->setValue(m_ui->sB->value()+1);


         m_ui->logplot->graph(10)->setName("park nut switch");
            m_ui->logplot->legend->item(10)->setVisible(true);
         m_ui->logplot->graph(10)->setVisible(true);
                   switch (m_ui->sB->value()){
                                    case 1:  m_ui->logplot->graph(10)->setPen(QPen(Qt::blue));
                                         m_ui->label_text5->setText("blue");break;
                                    case 2: m_ui->logplot->graph(10)->setPen(QPen(Qt::cyan));
                                        m_ui->label_text6->setText("cyan");break;
                                    case 3: m_ui->logplot->graph(10)->setPen(QPen(Qt::red));
                                        m_ui->label_text7->setText("red");break;
                                    case 4: m_ui->logplot->graph(10)->setPen(QPen(Qt::green));
                                        m_ui->label_text8->setText("green");break;
                                    }

          m_ui->logplot->graph(10)->rescaleValueAxis(true);
            m_ui->logplot->replot();
                          str="";
                 for (int i = 0; i < y11.size(); ++i)
                {
                 if (i > 0)
                    str += " ";
                    str += QString::number(y11[i]);
                }
                                             switch (m_ui->sB->value()){

                                         case 1 : m_ui->text_1->setText(str2);
                                                m_ui->text_1->setText(str);
                                                m_ui->label_text1->setText(m_ui->logplot->graph(10)->name());break;
                                          case 2 : m_ui->text_2->setText(str2);
                                                  m_ui->text_2->setText(str);
                                                  m_ui->label_text2->setText(m_ui->logplot->graph(10)->name());break;
                                         case 3 : m_ui->text_3->setText(str2);
                                                 m_ui->text_3->setText(str);
                                                 m_ui->label_text3->setText(m_ui->logplot->graph(10)->name());break;
                                         case 4 : m_ui->text_4->setText(str2);
                                                m_ui->text_4->setText(str);
                                                m_ui->label_text4->setText(m_ui->logplot->graph(10)->name());break;
                                                 break;return;
                                        }
            }
          else{
                switch (m_ui->sB->value())
                {

                 case 1 : m_ui->text_1->setText(str2);
                          m_ui->label_text1->setText("");break;
                 case 2 : m_ui->text_2->setText(str2);
                          m_ui->label_text2->setText("");break;
                 case 3 : m_ui->text_3->setText(str2);
                          m_ui->label_text3->setText("");break;
                 case 4 : m_ui->text_4->setText(str2);
                          m_ui->label_text4->setText("");break;
                          break;return;
                }
                m_ui->logplot->graph(10)->setVisible(false);
            m_ui->logplot->legend->item(10)->setVisible(false);
            m_ui->sB->setValue(m_ui->sB->value()-1);
            m_ui->logplot->replot();


         }
    }
    //12::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
    else if (((Reihe== 5)&&(Spalte==1))&&(m_ui->sB->value()<=4))
        {

            if( (m_ui->table_list->model()->index(Reihe,Spalte).data(Qt::CheckStateRole)==Qt::Checked)&&(m_ui->sB->value()<4))
         {m_ui->sB->setValue(m_ui->sB->value()+1);


         m_ui->logplot->graph(11)->setName("faultcode");
          m_ui->logplot->legend->item(11)->setVisible(true);
                switch (m_ui->sB->value()){
                                    case 1:  m_ui->logplot->graph(11)->setPen(QPen(Qt::blue));
                                         m_ui->label_text5->setText("blue");break;
                                    case 2: m_ui->logplot->graph(11)->setPen(QPen(Qt::cyan));
                                        m_ui->label_text6->setText("cyan");break;
                                    case 3: m_ui->logplot->graph(11)->setPen(QPen(Qt::red));
                                        m_ui->label_text7->setText("red");break;
                                    case 4: m_ui->logplot->graph(11)->setPen(QPen(Qt::green));
                                        m_ui->label_text8->setText("green");break;
                                    }
        m_ui->logplot->graph(11)->setVisible(true);
          m_ui->logplot->graph(11)->rescaleValueAxis(true);
            m_ui->logplot->replot();
                          str="";
                 for (int i = 0; i < y12.size(); ++i)
                {
                 if (i > 0)
                    str += " ";
                    str += QString::number(y12[i]);
                }
                                             switch (m_ui->sB->value()){

                                         case 1 : m_ui->text_1->setText(str2);
                                                m_ui->text_1->setText(str);
                                                m_ui->label_text1->setText(m_ui->logplot->graph(11)->name());break;
                                          case 2 : m_ui->text_2->setText(str2);
                                                  m_ui->text_2->setText(str);
                                                  m_ui->label_text2->setText(m_ui->logplot->graph(11)->name());break;
                                         case 3 : m_ui->text_3->setText(str2);
                                                 m_ui->text_3->setText(str);
                                                 m_ui->label_text3->setText(m_ui->logplot->graph(11)->name());break;
                                         case 4 : m_ui->text_4->setText(str2);
                                                m_ui->text_4->setText(str);
                                                m_ui->label_text4->setText(m_ui->logplot->graph(11)->name());break;
                                                 break;return;
                                        }
            }
          else{
                switch (m_ui->sB->value())
                {

                 case 1 : m_ui->text_1->setText(str2);
                          m_ui->label_text1->setText("");break;
                 case 2 : m_ui->text_2->setText(str2);
                          m_ui->label_text2->setText("");break;
                 case 3 : m_ui->text_3->setText(str2);
                          m_ui->label_text3->setText("");break;
                 case 4 : m_ui->text_4->setText(str2);
                          m_ui->label_text4->setText("");break;
                          break;return;
                }

                m_ui->logplot->graph(11)->setVisible(false);
            m_ui->logplot->legend->item(11)->setVisible(false);
            m_ui->sB->setValue(m_ui->sB->value()-1);
         m_ui->logplot->replot();


         }
    }
      //13::::::::::::::::::::::::::::::::::
    else if (((Reihe== 6)&&(Spalte==1))&&(m_ui->sB->value()<=4))
        {

            if( (m_ui->table_list->model()->index(Reihe,Spalte).data(Qt::CheckStateRole)==Qt::Checked)&&(m_ui->sB->value()<4))
         {		 m_ui->sB->setValue(m_ui->sB->value()+1);


         m_ui->logplot->graph(12)->setName("idlesetpoint");
          m_ui->logplot->legend->item(12)->setVisible(true);
                   switch (m_ui->sB->value()){
                                    case 1:  m_ui->logplot->graph(12)->setPen(QPen(Qt::blue));
                                         m_ui->label_text5->setText("blue");break;
                                    case 2: m_ui->logplot->graph(12)->setPen(QPen(Qt::cyan));
                                        m_ui->label_text6->setText("cyan");break;
                                    case 3: m_ui->logplot->graph(12)->setPen(QPen(Qt::red));
                                        m_ui->label_text7->setText("red");break;
                                    case 4: m_ui->logplot->graph(12)->setPen(QPen(Qt::green));
                                        m_ui->label_text8->setText("green");break;
                                    }
         m_ui->logplot->graph(12)->setVisible(true);

          m_ui->logplot->graph(12)->rescaleValueAxis(true);
            m_ui->logplot->replot();
                                  str="";
                 for (int i = 0; i < y13.size(); ++i)
                {
                 if (i > 0)
                    str += " ";
                    str += QString::number(y13[i]);
                }
                                             switch (m_ui->sB->value()){

                                         case 1 : m_ui->text_1->setText(str2);
                                                m_ui->text_1->setText(str);
                                                m_ui->label_text1->setText(m_ui->logplot->graph(12)->name());break;
                                          case 2 : m_ui->text_2->setText(str2);
                                                  m_ui->text_2->setText(str);
                                                  m_ui->label_text2->setText(m_ui->logplot->graph(12)->name());break;
                                         case 3 : m_ui->text_3->setText(str2);
                                                 m_ui->text_3->setText(str);
                                                 m_ui->label_text3->setText(m_ui->logplot->graph(12)->name());break;
                                         case 4 : m_ui->text_4->setText(str2);
                                                m_ui->text_4->setText(str);
                                                m_ui->label_text4->setText(m_ui->logplot->graph(12)->name());break;
                                                 break;return;
                                        }
            }
          else{switch (m_ui->sB->value())
                {

                 case 1 : m_ui->text_1->setText(str2);
                          m_ui->label_text1->setText("");break;
                 case 2 : m_ui->text_2->setText(str2);
                          m_ui->label_text2->setText("");break;
                 case 3 : m_ui->text_3->setText(str2);
                          m_ui->label_text3->setText("");break;
                 case 4 : m_ui->text_4->setText(str2);
                          m_ui->label_text4->setText("");break;
                          break;return;
                }
                m_ui->logplot->graph(12)->setVisible(false);
            m_ui->logplot->legend->item(12)->setVisible(false);
            m_ui->sB->setValue(m_ui->sB->value()-1);
            m_ui->logplot->replot();

    }}
    //14::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
    else if (((Reihe== 0)&&(Spalte==2))&&(m_ui->sB->value()<=4))
        {

            if( (m_ui->table_list->model()->index(Reihe,Spalte).data(Qt::CheckStateRole)==Qt::Checked)&&(m_ui->sB->value()<4))
         {	 m_ui->sB->setValue(m_ui->sB->value()+1);


         m_ui->logplot->graph(13)->setName("idlehot");
          m_ui->logplot->legend->item(13)->setVisible(true);
                switch (m_ui->sB->value()){
                                    case 1:  m_ui->logplot->graph(13)->setPen(QPen(Qt::blue));
                                         m_ui->label_text5->setText("blue");break;
                                    case 2: m_ui->logplot->graph(13)->setPen(QPen(Qt::cyan));
                                        m_ui->label_text6->setText("cyan");break;
                                    case 3: m_ui->logplot->graph(13)->setPen(QPen(Qt::red));
                                        m_ui->label_text7->setText("red");break;
                                    case 4: m_ui->logplot->graph(13)->setPen(QPen(Qt::green));
                                        m_ui->label_text8->setText("green");break;
                                    }
         m_ui->logplot->graph(13)->setVisible(true);

          m_ui->logplot->graph(13)->rescaleValueAxis(true);
            m_ui->logplot->replot();
                                  str="";
                 for (int i = 0; i < y14.size(); ++i)
                {
                 if (i > 0)
                    str += " ";
                    str += QString::number(y14[i]);
                }
                                             switch (m_ui->sB->value()){

                                         case 1 : m_ui->text_1->setText(str2);
                                                m_ui->text_1->setText(str);
                                                m_ui->label_text1->setText(m_ui->logplot->graph(13)->name());break;
                                          case 2 : m_ui->text_2->setText(str2);
                                                  m_ui->text_2->setText(str);
                                                  m_ui->label_text2->setText(m_ui->logplot->graph(13)->name());break;
                                         case 3 : m_ui->text_3->setText(str2);
                                                 m_ui->text_3->setText(str);
                                                 m_ui->label_text3->setText(m_ui->logplot->graph(13)->name());break;
                                         case 4 : m_ui->text_4->setText(str2);
                                                m_ui->text_4->setText(str);
                                                m_ui->label_text4->setText(m_ui->logplot->graph(13)->name());break;
                                                 break;return;
                                        }
            }
          else{switch (m_ui->sB->value())
                {

                 case 1 : m_ui->text_1->setText(str2);
                          m_ui->label_text1->setText("");break;
                 case 2 : m_ui->text_2->setText(str2);
                          m_ui->label_text2->setText("");break;
                 case 3 : m_ui->text_3->setText(str2);
                          m_ui->label_text3->setText("");break;
                 case 4 : m_ui->text_4->setText(str2);
                          m_ui->label_text4->setText("");break;
                          break;return;
                }
                m_ui->logplot->graph(13)->setVisible(false);
            m_ui->logplot->legend->item(13)->setVisible(false);
            m_ui->sB->setValue(m_ui->sB->value()-1);
         m_ui->logplot->replot();

         }
    }
    //15:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
    else if (((Reihe== 1)&&(Spalte==2))&&(m_ui->sB->value()<=4))
        {

            if( (m_ui->table_list->model()->index(Reihe,Spalte).data(Qt::CheckStateRole)==Qt::Checked)&&(m_ui->sB->value()<4))
         { m_ui->sB->setValue(m_ui->sB->value()+1);


         m_ui->logplot->graph(14)->setName("unknown2");
          m_ui->logplot->legend->item(14)->setVisible(true);
                switch (m_ui->sB->value()){
                                    case 1:  m_ui->logplot->graph(14)->setPen(QPen(Qt::blue));
                                         m_ui->label_text5->setText("blue");break;
                                    case 2: m_ui->logplot->graph(14)->setPen(QPen(Qt::cyan));
                                        m_ui->label_text6->setText("cyan");break;
                                    case 3: m_ui->logplot->graph(14)->setPen(QPen(Qt::red));
                                        m_ui->label_text7->setText("red");break;
                                    case 4: m_ui->logplot->graph(14)->setPen(QPen(Qt::green));
                                        m_ui->label_text8->setText("green");break;
                                    }
         m_ui->logplot->graph(14)->setVisible(true);
          m_ui->logplot->graph(14)->rescaleValueAxis(true);
            m_ui->logplot->replot();
                                  str="";
                 for (int i = 0; i < y15.size(); ++i)
                {
                 if (i > 0)
                    str += " ";
                    str += QString::number(y15[i]);
                }
                                             switch (m_ui->sB->value()){

                                         case 1 : m_ui->text_1->setText(str2);
                                                m_ui->text_1->setText(str);
                                                m_ui->label_text1->setText(m_ui->logplot->graph(14)->name());break;
                                          case 2 : m_ui->text_2->setText(str2);
                                                  m_ui->text_2->setText(str);
                                                  m_ui->label_text2->setText(m_ui->logplot->graph(14)->name());break;
                                         case 3 : m_ui->text_3->setText(str2);
                                                 m_ui->text_3->setText(str);
                                                 m_ui->label_text3->setText(m_ui->logplot->graph(14)->name());break;
                                         case 4 : m_ui->text_4->setText(str2);
                                                m_ui->text_4->setText(str);
                                                m_ui->label_text4->setText(m_ui->logplot->graph(14)->name());break;
                                                 break;return;
                                        }
            }
          else{switch (m_ui->sB->value())
                {

                 case 1 : m_ui->text_1->setText(str2);
                          m_ui->label_text1->setText("");break;
                 case 2 : m_ui->text_2->setText(str2);
                          m_ui->label_text2->setText("");break;
                 case 3 : m_ui->text_3->setText(str2);
                          m_ui->label_text3->setText("");break;
                 case 4 : m_ui->text_4->setText(str2);
                          m_ui->label_text4->setText("");break;
                          break;return;
                }
                m_ui->logplot->graph(14)->setVisible(false);
            m_ui->logplot->legend->item(14)->setVisible(false);
            m_ui->sB->setValue(m_ui->sB->value()-1);
         m_ui->logplot->replot();

         }
    }
    //16::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
    else if (((Reihe== 2)&&(Spalte==2))&&(m_ui->sB->value()<=4))
        {

            if( (m_ui->table_list->model()->index(Reihe,Spalte).data(Qt::CheckStateRole)==Qt::Checked)&&(m_ui->sB->value()<4))
      {		m_ui->sB->setValue(m_ui->sB->value()+1);


         m_ui->logplot->graph(15)->setName("IAC");
          m_ui->logplot->legend->item(15)->setVisible(true);
                switch (m_ui->sB->value()){
                                    case 1:  m_ui->logplot->graph(15)->setPen(QPen(Qt::blue));
                                         m_ui->label_text5->setText("blue");break;
                                    case 2: m_ui->logplot->graph(15)->setPen(QPen(Qt::cyan));
                                        m_ui->label_text6->setText("cyan");break;
                                    case 3: m_ui->logplot->graph(15)->setPen(QPen(Qt::red));
                                        m_ui->label_text7->setText("red");break;
                                    case 4: m_ui->logplot->graph(15)->setPen(QPen(Qt::green));
                                        m_ui->label_text8->setText("green");break;
                                    }
         m_ui->logplot->graph(15)->setVisible(true);
            m_ui->logplot->graph(15)->valueAxis()->setRange(0,100);
          m_ui->logplot->graph(15)->rescaleValueAxis(true);
            m_ui->logplot->replot();
                                          str="";
                 for (int i = 0; i < y16.size(); ++i)
                {
                 if (i > 0)
                    str += " ";
                    str += QString::number(y16[i]);
                }
                                             switch (m_ui->sB->value()){

                                         case 1 : m_ui->text_1->setText(str2);
                                                m_ui->text_1->setText(str);
                                                m_ui->label_text1->setText(m_ui->logplot->graph(15)->name());break;
                                          case 2 : m_ui->text_2->setText(str2);
                                                  m_ui->text_2->setText(str);
                                                  m_ui->label_text2->setText(m_ui->logplot->graph(15)->name());break;
                                         case 3 : m_ui->text_3->setText(str2);
                                                 m_ui->text_3->setText(str);
                                                 m_ui->label_text3->setText(m_ui->logplot->graph(15)->name());break;
                                         case 4 : m_ui->text_4->setText(str2);
                                                m_ui->text_4->setText(str);
                                                m_ui->label_text4->setText(m_ui->logplot->graph(15)->name());break;
                                                 break;return;
                                        }
            }
          else{switch (m_ui->sB->value())
                {

                 case 1 : m_ui->text_1->setText(str2);
                          m_ui->label_text1->setText("");break;
                 case 2 : m_ui->text_2->setText(str2);
                          m_ui->label_text2->setText("");break;
                 case 3 : m_ui->text_3->setText(str2);
                          m_ui->label_text3->setText("");break;
                 case 4 : m_ui->text_4->setText(str2);
                          m_ui->label_text4->setText("");break;
                          break;return;
                }
                m_ui->logplot->graph(15)->setVisible(false);
            m_ui->logplot->legend->item(15)->setVisible(false);
            m_ui->sB->setValue(m_ui->sB->value()-1);
         m_ui->logplot->replot();

         }
    }
        //17:::::::::::::::::::::::::::::::::::::::::::::::
    else if (((Reihe== 3)&&(Spalte==2))&&(m_ui->sB->value()<=4))
        {

            if( (m_ui->table_list->model()->index(Reihe,Spalte).data(Qt::CheckStateRole)==Qt::Checked)&&(m_ui->sB->value()<4))
     {m_ui->sB->setValue(m_ui->sB->value()+1);

         m_ui->logplot->graph(16)->setName("idlespeeddev");
          m_ui->logplot->legend->item(16)->setVisible(true);
                 switch (m_ui->sB->value()){
                                    case 1:  m_ui->logplot->graph(16)->setPen(QPen(Qt::blue));
                                         m_ui->label_text5->setText("blue");break;
                                    case 2: m_ui->logplot->graph(16)->setPen(QPen(Qt::cyan));
                                        m_ui->label_text6->setText("cyan");break;
                                    case 3: m_ui->logplot->graph(16)->setPen(QPen(Qt::red));
                                        m_ui->label_text7->setText("red");break;
                                    case 4: m_ui->logplot->graph(16)->setPen(QPen(Qt::green));
                                        m_ui->label_text8->setText("green");break;
                                    }
         m_ui->logplot->graph(16)->setVisible(true);
            m_ui->logplot->graph(16)->valueAxis()->setRange(0,100);
          m_ui->logplot->graph(16)->rescaleValueAxis(true);
            m_ui->logplot->replot();
                                          str="";
                 for (int i = 0; i < y17.size(); ++i)
                {
                 if (i > 0)
                    str += " ";
                    str += QString::number(y17[i]);
                }
                                             switch (m_ui->sB->value()){

                                         case 1 : m_ui->text_1->setText(str2);
                                                m_ui->text_1->setText(str);
                                                m_ui->label_text1->setText(m_ui->logplot->graph(16)->name());break;
                                          case 2 : m_ui->text_2->setText(str2);
                                                  m_ui->text_2->setText(str);
                                                  m_ui->label_text2->setText(m_ui->logplot->graph(16)->name());break;
                                         case 3 : m_ui->text_3->setText(str2);
                                                 m_ui->text_3->setText(str);
                                                 m_ui->label_text3->setText(m_ui->logplot->graph(16)->name());break;
                                         case 4 : m_ui->text_4->setText(str2);
                                                m_ui->text_4->setText(str);
                                                m_ui->label_text4->setText(m_ui->logplot->graph(16)->name());break;
                                                 break;return;
                                        }
            }
          else
            {switch (m_ui->sB->value())
                {

                 case 1 : m_ui->text_1->setText(str2);
                          m_ui->label_text1->setText("");break;
                 case 2 : m_ui->text_2->setText(str2);
                          m_ui->label_text2->setText("");break;
                 case 3 : m_ui->text_3->setText(str2);
                          m_ui->label_text3->setText("");break;
                 case 4 : m_ui->text_4->setText(str2);
                          m_ui->label_text4->setText("");break;
                          break;return;
                }
                m_ui->logplot->graph(16)->setVisible(false);
              m_ui->logplot->legend->item(16)->setVisible(false);
              m_ui->sB->setValue(m_ui->sB->value()-1);
              m_ui->logplot->replot();

          }
    }
     //18::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
    else if (((Reihe== 4)&&(Spalte==2))&&(m_ui->sB->value()<=4))
        {

            if( (m_ui->table_list->model()->index(Reihe,Spalte).data(Qt::CheckStateRole)==Qt::Checked)&&(m_ui->sB->value()<4))
              { m_ui->sB->setValue(m_ui->sB->value()+1);


         m_ui->logplot->graph(17)->setName("ign advance offset");
          m_ui->logplot->legend->item(17)->setVisible(true);
                switch (m_ui->sB->value()){
                                    case 1:  m_ui->logplot->graph(17)->setPen(QPen(Qt::blue));
                                         m_ui->label_text5->setText("blue");break;
                                    case 2: m_ui->logplot->graph(17)->setPen(QPen(Qt::cyan));
                                        m_ui->label_text6->setText("cyan");break;
                                    case 3: m_ui->logplot->graph(17)->setPen(QPen(Qt::red));
                                        m_ui->label_text7->setText("red");break;
                                    case 4: m_ui->logplot->graph(17)->setPen(QPen(Qt::green));
                                        m_ui->label_text8->setText("green");break;
                                    }
         m_ui->logplot->graph(17)->setVisible(true);
          m_ui->logplot->graph(17)->valueAxis()->setRange(0,100);
          m_ui->logplot->graph(17)->rescaleValueAxis(true);
            m_ui->logplot->replot();
                 str="";
                 for (int i = 0; i < y18.size(); ++i)
                {
                 if (i > 0)
                    str += " ";
                    str += QString::number(y18[i]);
                }
                switch (m_ui->sB->value()){
                    case 1 : m_ui->text_1->setText(str2);
                             m_ui->text_1->setText(str);
                             m_ui->label_text1->setText(m_ui->logplot->graph(17)->name());break;
                    case 2 : m_ui->text_2->setText(str2);
                             m_ui->text_2->setText(str);
                             m_ui->label_text2->setText(m_ui->logplot->graph(17)->name());break;
                    case 3 : m_ui->text_3->setText(str2);
                             m_ui->text_3->setText(str);
                             m_ui->label_text3->setText(m_ui->logplot->graph(17)->name());break;
                    case 4 : m_ui->text_4->setText(str2);
                             m_ui->text_4->setText(str);
                             m_ui->label_text4->setText(m_ui->logplot->graph(17)->name());break;
                             break;return;
                    }
            }
          else{switch (m_ui->sB->value())
                {

                 case 1 : m_ui->text_1->setText(str2);
                          m_ui->label_text1->setText("");break;
                 case 2 : m_ui->text_2->setText(str2);
                          m_ui->label_text2->setText("");break;
                 case 3 : m_ui->text_3->setText(str2);
                          m_ui->label_text3->setText("");break;
                 case 4 : m_ui->text_4->setText(str2);
                          m_ui->label_text4->setText("");break;
                          break;return;
                }
                m_ui->logplot->graph(17)->setVisible(false);
              m_ui->logplot->legend->item(17)->setVisible(false);
              m_ui->sB->setValue(m_ui->sB->value()-1);
               m_ui->logplot->replot();

         }
    }
              //19::::::::::::::::::::::::::::::::::::::::::::::::::
    else if (((Reihe== 5)&&(Spalte==2))&&(m_ui->sB->value()<=4))
        {

            if( (m_ui->table_list->model()->index(Reihe,Spalte).data(Qt::CheckStateRole)==Qt::Checked)&&(m_ui->sB->value()<4))
         {
              m_ui->sB->setValue(m_ui->sB->value()+1);

         m_ui->logplot->graph(18)->setName("ignitionadvance");
          m_ui->logplot->legend->item(18)->setVisible(true);
                switch (m_ui->sB->value()){
                                    case 1:  m_ui->logplot->graph(18)->setPen(QPen(Qt::blue));
                                         m_ui->label_text5->setText("blue");break;
                                    case 2: m_ui->logplot->graph(18)->setPen(QPen(Qt::cyan));
                                        m_ui->label_text6->setText("cyan");break;
                                    case 3: m_ui->logplot->graph(18)->setPen(QPen(Qt::red));
                                        m_ui->label_text7->setText("red");break;
                                    case 4: m_ui->logplot->graph(18)->setPen(QPen(Qt::green));
                                        m_ui->label_text8->setText("green");break;
                                    }
         m_ui->logplot->graph(18)->setVisible(true);
          m_ui->logplot->graph(18)->valueAxis()->setRange(0,50);
          m_ui->logplot->graph(18)->rescaleValueAxis(true);
            m_ui->logplot->replot();
                         str="";
                 for (int i = 0; i < y19.size(); ++i)
                {
                 if (i > 0)
                    str += " ";
                    str += QString::number(y19[i]);
                }
                switch (m_ui->sB->value()){
                    case 1 : m_ui->text_1->setText(str2);
                             m_ui->text_1->setText(str);
                             m_ui->label_text1->setText(m_ui->logplot->graph(18)->name());break;
                    case 2 : m_ui->text_2->setText(str2);
                             m_ui->text_2->setText(str);
                             m_ui->label_text2->setText(m_ui->logplot->graph(18)->name());break;
                    case 3 : m_ui->text_3->setText(str2);
                             m_ui->text_3->setText(str);
                             m_ui->label_text3->setText(m_ui->logplot->graph(18)->name());break;
                    case 4 : m_ui->text_4->setText(str2);
                             m_ui->text_4->setText(str);
                             m_ui->label_text4->setText(m_ui->logplot->graph(18)->name());break;
                             break;return;
                    }
            }
          else{switch (m_ui->sB->value())
                {

                 case 1 : m_ui->text_1->setText(str2);
                          m_ui->label_text1->setText("");break;
                 case 2 : m_ui->text_2->setText(str2);
                          m_ui->label_text2->setText("");break;
                 case 3 : m_ui->text_3->setText(str2);
                          m_ui->label_text3->setText("");break;
                 case 4 : m_ui->text_4->setText(str2);
                          m_ui->label_text4->setText("");break;
                          break;return;
                }
                m_ui->logplot->graph(18)->setVisible(false);
            m_ui->logplot->legend->item(18)->setVisible(false);
            m_ui->sB->setValue(m_ui->sB->value()-1);
         m_ui->logplot->replot();

         }
    }

     //20:::::::::::::::::::::::::::::::::::::::::::::::::
    else if (((Reihe== 6)&&(Spalte==2))&&(m_ui->sB->value()<=4))
        {

            if( (m_ui->table_list->model()->index(Reihe,Spalte).data(Qt::CheckStateRole)==Qt::Checked)&&(m_ui->sB->value()<4))
                 {m_ui->sB->setValue(m_ui->sB->value()+1);


                     m_ui->logplot->graph(19)->setName("coiltime");
                      m_ui->logplot->legend->item(19)->setVisible(true);
                  switch (m_ui->sB->value()){
                                    case 1:  m_ui->logplot->graph(19)->setPen(QPen(Qt::blue));
                                         m_ui->label_text5->setText("blue");break;
                                    case 2: m_ui->logplot->graph(19)->setPen(QPen(Qt::cyan));
                                        m_ui->label_text6->setText("cyan");break;
                                    case 3: m_ui->logplot->graph(19)->setPen(QPen(Qt::red));
                                        m_ui->label_text7->setText("red");break;
                                    case 4: m_ui->logplot->graph(19)->setPen(QPen(Qt::green));
                                        m_ui->label_text8->setText("green");break;
                                    }
                     m_ui->logplot->graph(19)->setVisible(true);
                     m_ui->logplot->graph(19)->valueAxis()->setRange(0,50 );
                     m_ui->logplot->graph(19)->rescaleValueAxis(true);
                     m_ui->logplot->replot();
                                 str="";
                 for (int i = 0; i < y20.size(); ++i)
                {
                 if (i > 0)
                    str += " ";
                    str += QString::number(y20[i]);
                }
                switch (m_ui->sB->value()){
                    case 1 : m_ui->text_1->setText(str2);
                             m_ui->text_1->setText(str);
                             m_ui->label_text1->setText(m_ui->logplot->graph(19)->name());break;
                    case 2 : m_ui->text_2->setText(str2);
                             m_ui->text_2->setText(str);
                             m_ui->label_text2->setText(m_ui->logplot->graph(19)->name());break;
                    case 3 : m_ui->text_3->setText(str2);
                             m_ui->text_3->setText(str);
                             m_ui->label_text3->setText(m_ui->logplot->graph(19)->name());break;
                    case 4 : m_ui->text_4->setText(str2);
                             m_ui->text_4->setText(str);
                             m_ui->label_text4->setText(m_ui->logplot->graph(19)->name());break;
                             break;return;
                    }
                     }
                  else{switch (m_ui->sB->value())
                {

                 case 1 : m_ui->text_1->setText(str2);
                          m_ui->label_text1->setText("");break;
                 case 2 : m_ui->text_2->setText(str2);
                          m_ui->label_text2->setText("");break;
                 case 3 : m_ui->text_3->setText(str2);
                          m_ui->label_text3->setText("");break;
                 case 4 : m_ui->text_4->setText(str2);
                          m_ui->label_text4->setText("");break;
                          break;return;
                }
                m_ui->logplot->graph(19)->setVisible(false);
                    m_ui->logplot->legend->item(19)->setVisible(false);
                    m_ui->sB->setValue(m_ui->sB->value()-1);
                    m_ui->logplot->replot();

                      }
         }
    //21::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

    else if (((Reihe== 0)&&(Spalte==3))&&(m_ui->sB->value()<=4))
        {

            if( (m_ui->table_list->model()->index(Reihe,Spalte).data(Qt::CheckStateRole)==Qt::Checked)&&(m_ui->sB->value()<4))
                 {m_ui->sB->setValue(m_ui->sB->value()+1);

                 m_ui->logplot->graph(20)->setName("cranks");
                  m_ui->logplot->legend->item(20)->setVisible(true);
                       switch (m_ui->sB->value()){
                                    case 1:  m_ui->logplot->graph(20)->setPen(QPen(Qt::blue));
                                         m_ui->label_text5->setText("blue");break;
                                    case 2: m_ui->logplot->graph(20)->setPen(QPen(Qt::cyan));
                                        m_ui->label_text6->setText("cyan");break;
                                    case 3: m_ui->logplot->graph(20)->setPen(QPen(Qt::red));
                                        m_ui->label_text7->setText("red");break;
                                    case 4: m_ui->logplot->graph(20)->setPen(QPen(Qt::green));
                                        m_ui->label_text8->setText("green");break;
                                    }
                 m_ui->logplot->graph(20)->setVisible(true);
                  m_ui->logplot->graph(20)->rescaleValueAxis(true);
                    m_ui->logplot->replot();
                                 str="";
                 for (int i = 0; i < y21.size(); ++i)
                {
                 if (i > 0)
                    str += " ";
                    str += QString::number(y21[i]);
                }
                switch (m_ui->sB->value()){
                    case 1 : m_ui->text_1->setText(str2);
                             m_ui->text_1->setText(str);
                             m_ui->label_text1->setText(m_ui->logplot->graph(20)->name());break;
                    case 2 : m_ui->text_2->setText(str2);
                             m_ui->text_2->setText(str);
                             m_ui->label_text2->setText(m_ui->logplot->graph(20)->name());break;
                    case 3 : m_ui->text_3->setText(str2);
                             m_ui->text_3->setText(str);
                             m_ui->label_text3->setText(m_ui->logplot->graph(20)->name());break;
                    case 4 : m_ui->text_4->setText(str2);
                             m_ui->text_4->setText(str);
                             m_ui->label_text4->setText(m_ui->logplot->graph(20)->name());break;
                             break;return;
                    }
                    }
                  else{switch (m_ui->sB->value())
                {

                 case 1 : m_ui->text_1->setText(str2);
                          m_ui->label_text1->setText("");break;
                 case 2 : m_ui->text_2->setText(str2);
                          m_ui->label_text2->setText("");break;
                 case 3 : m_ui->text_3->setText(str2);
                          m_ui->label_text3->setText("");break;
                 case 4 : m_ui->text_4->setText(str2);
                          m_ui->label_text4->setText("");break;
                          break;return;
                }
                m_ui->logplot->graph(20)->setVisible(false);
                    m_ui->logplot->legend->item(20)->setVisible(false);
                    m_ui->sB->setValue(m_ui->sB->value()-1);
                    m_ui->logplot->replot();

                 }
         }
     //22:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
    else if (((Reihe== 1)&&(Spalte==3))&&(m_ui->sB->value()<=4))
        {

            if( (m_ui->table_list->model()->index(Reihe,Spalte).data(Qt::CheckStateRole)==Qt::Checked)&&(m_ui->sB->value()<4))
                 {m_ui->sB->setValue(m_ui->sB->value()+1);


                 m_ui->logplot->graph(21)->setName("unknown4");
                  m_ui->logplot->legend->item(21)->setVisible(true);
                    switch (m_ui->sB->value()){
                                    case 1:  m_ui->logplot->graph(21)->setPen(QPen(Qt::blue));
                                         m_ui->label_text5->setText("blue");break;
                                    case 2: m_ui->logplot->graph(21)->setPen(QPen(Qt::cyan));
                                        m_ui->label_text6->setText("cyan");break;
                                    case 3: m_ui->logplot->graph(21)->setPen(QPen(Qt::red));
                                        m_ui->label_text7->setText("red");break;
                                    case 4: m_ui->logplot->graph(21)->setPen(QPen(Qt::green));
                                        m_ui->label_text8->setText("green");break;
                                    }
                 m_ui->logplot->graph(21)->setVisible(true);
                 m_ui->logplot->graph(21)->rescaleValueAxis(true);
                    m_ui->logplot->replot();
                                 str="";
                 for (int i = 0; i < y22.size(); ++i)
                {
                 if (i > 0)
                    str += " ";
                    str += QString::number(y22[i]);
                }
                switch (m_ui->sB->value()){
                    case 1 : m_ui->text_1->setText(str2);
                             m_ui->text_1->setText(str);
                             m_ui->label_text1->setText(m_ui->logplot->graph(21)->name());break;
                    case 2 : m_ui->text_2->setText(str2);
                             m_ui->text_2->setText(str);
                             m_ui->label_text2->setText(m_ui->logplot->graph(21)->name());break;
                    case 3 : m_ui->text_3->setText(str2);
                             m_ui->text_3->setText(str);
                             m_ui->label_text3->setText(m_ui->logplot->graph(21)->name());break;
                    case 4 : m_ui->text_4->setText(str2);
                             m_ui->text_4->setText(str);
                             m_ui->label_text4->setText(m_ui->logplot->graph(21)->name());break;
                             break;return;
                    }
                    }
                  else{switch (m_ui->sB->value())
                {

                 case 1 : m_ui->text_1->setText(str2);
                          m_ui->label_text1->setText("");break;
                 case 2 : m_ui->text_2->setText(str2);
                          m_ui->label_text2->setText("");break;
                 case 3 : m_ui->text_3->setText(str2);
                          m_ui->label_text3->setText("");break;
                 case 4 : m_ui->text_4->setText(str2);
                          m_ui->label_text4->setText("");break;
                          break;return;
                }
                m_ui->logplot->graph(21)->setVisible(false);
                    m_ui->logplot->legend->item(21)->setVisible(false);
                    m_ui->sB->setValue(m_ui->sB->value()-1);
                    m_ui->logplot->replot();

                              }
         }
     //23:::::::::::::::::::::::::::::::::::::::::
    else if (((Reihe== 2)&&(Spalte==3))&&(m_ui->sB->value()<=4))
        {

            if( (m_ui->table_list->model()->index(Reihe,Spalte).data(Qt::CheckStateRole)==Qt::Checked)&&(m_ui->sB->value()<4))
           { m_ui->sB->setValue(m_ui->sB->value()+1);

                  m_ui->logplot->legend->item(22)->setVisible(true);
               switch (m_ui->sB->value()){
                                    case 1:  m_ui->logplot->graph(22)->setPen(QPen(Qt::blue));
                                         m_ui->label_text5->setText("blue");break;
                                    case 2: m_ui->logplot->graph(22)->setPen(QPen(Qt::cyan));
                                        m_ui->label_text6->setText("cyan");break;
                                    case 3: m_ui->logplot->graph(22)->setPen(QPen(Qt::red));
                                        m_ui->label_text7->setText("red");break;
                                    case 4: m_ui->logplot->graph(22)->setPen(QPen(Qt::green));
                                        m_ui->label_text8->setText("green");break;
                                    }
               m_ui->logplot->graph(22)->setName("unknown5");

               m_ui->logplot->graph(22)->setVisible(true);
               m_ui->logplot->graph(22)->rescaleValueAxis(true);
               m_ui->logplot->replot();
                         str="";
                 for (int i = 0; i < y23.size(); ++i)
                {
                 if (i > 0)
                    str += " ";
                    str += QString::number(y23[i]);
                }
                switch (m_ui->sB->value()){
                    case 1 : m_ui->text_1->setText(str2);
                             m_ui->text_1->setText(str);
                             m_ui->label_text1->setText(m_ui->logplot->graph(22)->name());break;
                    case 2 : m_ui->text_2->setText(str2);
                             m_ui->text_2->setText(str);
                             m_ui->label_text2->setText(m_ui->logplot->graph(22)->name());break;
                    case 3 : m_ui->text_3->setText(str2);
                             m_ui->text_3->setText(str);
                             m_ui->label_text3->setText(m_ui->logplot->graph(22)->name());break;
                    case 4 : m_ui->text_4->setText(str2);
                             m_ui->text_4->setText(str);
                             m_ui->label_text4->setText(m_ui->logplot->graph(22)->name());break;
                             break;return;
                    }
               }
            else{switch (m_ui->sB->value())
                {

                 case 1 : m_ui->text_1->setText(str2);
                          m_ui->label_text1->setText("");break;
                 case 2 : m_ui->text_2->setText(str2);
                          m_ui->label_text2->setText("");break;
                 case 3 : m_ui->text_3->setText(str2);
                          m_ui->label_text3->setText("");break;
                 case 4 : m_ui->text_4->setText(str2);
                          m_ui->label_text4->setText("");break;
                          break;return;
                }
                m_ui->logplot->graph(22)->setVisible(false);
              m_ui->logplot->legend->item(22)->setVisible(false);
              m_ui->sB->setValue(m_ui->sB->value()-1);
                 m_ui->logplot->replot();

        }
        }
        //24::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

    else if (((Reihe== 3)&&(Spalte==3))&&(m_ui->sB->value()<=4))
         {

            if( (m_ui->table_list->model()->index(Reihe,Spalte).data(Qt::CheckStateRole)==Qt::Checked)&&(m_ui->sB->value()<4))
                 {m_ui->sB->setValue(m_ui->sB->value()+1);


              switch (m_ui->sB->value()){
                                    case 1:  m_ui->logplot->graph(23)->setPen(QPen(Qt::blue));
                                         m_ui->label_text5->setText("blue");break;
                                    case 2: m_ui->logplot->graph(23)->setPen(QPen(Qt::cyan));
                                        m_ui->label_text6->setText("cyan");break;
                                    case 3: m_ui->logplot->graph(23)->setPen(QPen(Qt::red));
                                        m_ui->label_text7->setText("red");break;
                                    case 4: m_ui->logplot->graph(23)->setPen(QPen(Qt::green));
                                        m_ui->label_text8->setText("green");break;
                                    }
                 m_ui->logplot->graph(23)->setName("ignitionswitch");
                m_ui->logplot->legend->item(23)->setVisible(true);
                 m_ui->logplot->graph(23)->setVisible(true);
                 m_ui->logplot->graph(23)->valueAxis()->setRange(0,100);
                 m_ui->logplot->graph(23)->rescaleValueAxis(true);
                    m_ui->logplot->replot();
                                 str="";
                 for (int i = 0; i < y24.size(); ++i)
                {
                 if (i > 0)
                    str += " ";
                    str += QString::number(y24[i]);
                }
                switch (m_ui->sB->value()){
                    case 1 : m_ui->text_1->setText(str2);
                             m_ui->text_1->setText(str);
                             m_ui->label_text1->setText(m_ui->logplot->graph(23)->name());break;
                    case 2 : m_ui->text_2->setText(str2);
                             m_ui->text_2->setText(str);
                             m_ui->label_text2->setText(m_ui->logplot->graph(23)->name());break;
                    case 3 : m_ui->text_3->setText(str2);
                             m_ui->text_3->setText(str);
                             m_ui->label_text3->setText(m_ui->logplot->graph(23)->name());break;
                    case 4 : m_ui->text_4->setText(str2);
                             m_ui->text_4->setText(str);
                             m_ui->label_text4->setText(m_ui->logplot->graph(23)->name());break;
                             break;return;
                    }
                    }
                  else{switch (m_ui->sB->value())
                {

                 case 1 : m_ui->text_1->setText(str2);
                          m_ui->label_text1->setText("");break;
                 case 2 : m_ui->text_2->setText(str2);
                          m_ui->label_text2->setText("");break;
                 case 3 : m_ui->text_3->setText(str2);
                          m_ui->label_text3->setText("");break;
                 case 4 : m_ui->text_4->setText(str2);
                          m_ui->label_text4->setText("");break;
                          break;return;
                }
                m_ui->logplot->graph(23)->setVisible(false);
                    m_ui->logplot->legend->item(23)->setVisible(false);
                    m_ui->sB->setValue(m_ui->sB->value()-1);
                    m_ui->logplot->replot();
                  }
         }

    //25::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
    else if (((Reihe== 4)&&(Spalte==3))&&(m_ui->sB->value()<=4))
         {

            if( (m_ui->table_list->model()->index(Reihe,Spalte).data(Qt::CheckStateRole)==Qt::Checked)&&(m_ui->sB->value()<4))
               {m_ui->sB->setValue(m_ui->sB->value()+1);


             switch (m_ui->sB->value()){
                                    case 1:  m_ui->logplot->graph(24)->setPen(QPen(Qt::blue));
                                         m_ui->label_text5->setText("blue");break;
                                    case 2: m_ui->logplot->graph(24)->setPen(QPen(Qt::cyan));
                                        m_ui->label_text6->setText("cyan");break;
                                    case 3: m_ui->logplot->graph(24)->setPen(QPen(Qt::red));
                                        m_ui->label_text7->setText("red");break;
                                    case 4: m_ui->logplot->graph(24)->setPen(QPen(Qt::green));
                                        m_ui->label_text8->setText("green");break;
                                    }
                    m_ui->logplot->graph(24)->setName("throttleangle");
                     m_ui->logplot->legend->item(24)->setVisible(true);
                    m_ui->logplot->graph(24)->setVisible(true);
                    m_ui->logplot->graph(24)->valueAxis()->setRange(0,100);
                    m_ui->logplot->graph(24)->rescaleValueAxis(true);
                    m_ui->logplot->replot();
                                                 str="";
                 for (int i = 0; i < y25.size(); ++i)
                {
                 if (i > 0)
                    str += " ";
                    str += QString::number(y25[i]);
                }
                switch (m_ui->sB->value()){
                    case 1 : m_ui->text_1->setText(str2);
                             m_ui->text_1->setText(str);
                             m_ui->label_text1->setText(m_ui->logplot->graph(24)->name());break;
                    case 2 : m_ui->text_2->setText(str2);
                             m_ui->text_2->setText(str);
                             m_ui->label_text2->setText(m_ui->logplot->graph(24)->name());break;
                    case 3 : m_ui->text_3->setText(str2);
                             m_ui->text_3->setText(str);
                             m_ui->label_text3->setText(m_ui->logplot->graph(24)->name());break;
                    case 4 : m_ui->text_4->setText(str2);
                             m_ui->text_4->setText(str);
                             m_ui->label_text4->setText(m_ui->logplot->graph(24)->name());break;
                             break;return;
                    }
                    }
                else{switch (m_ui->sB->value())
                {

                 case 1 : m_ui->text_1->setText(str2);
                          m_ui->label_text1->setText("");break;
                 case 2 : m_ui->text_2->setText(str2);
                          m_ui->label_text2->setText("");break;
                 case 3 : m_ui->text_3->setText(str2);
                          m_ui->label_text3->setText("");break;
                 case 4 : m_ui->text_4->setText(str2);
                          m_ui->label_text4->setText("");break;
                          break;return;
                }
                m_ui->logplot->graph(24)->setVisible(false);
                  m_ui->logplot->legend->item(24)->setVisible(false);
                  m_ui->sB->setValue(m_ui->sB->value()-1);
                    m_ui->logplot->replot();
                  }
         }
                //26::::::::::::::::::::::::::::::::::::::::::::::::::
    else if (((Reihe== 5)&&(Spalte==3))&&(m_ui->sB->value()<=4))
         {

            if( (m_ui->table_list->model()->index(Reihe,Spalte).data(Qt::CheckStateRole)==Qt::Checked)&&(m_ui->sB->value()<4))
                 {	m_ui->sB->setValue(m_ui->sB->value()+1);
             switch (m_ui->sB->value()){
                                    case 1:  m_ui->logplot->graph(25)->setPen(QPen(Qt::blue));
                                         m_ui->label_text5->setText("blue");break;
                                    case 2: m_ui->logplot->graph(25)->setPen(QPen(Qt::cyan));
                                        m_ui->label_text6->setText("cyan");break;
                                    case 3: m_ui->logplot->graph(25)->setPen(QPen(Qt::red));
                                        m_ui->label_text7->setText("red");break;
                                    case 4: m_ui->logplot->graph(25)->setPen(QPen(Qt::green));
                                        m_ui->label_text8->setText("green");break;
                                    }
                 m_ui->logplot->graph(25)->setName("uk6");
                 m_ui->logplot->legend->item(25)->setVisible(true);
                m_ui->logplot->graph(25)->setVisible(true);
                  m_ui->logplot->graph(25)->valueAxis()->setRange(0,100);
                 m_ui->logplot->graph(25)->rescaleValueAxis(true);
                    m_ui->logplot->replot();
                                                 str="";
                 for (int i = 0; i < y26.size(); ++i)
                {
                 if (i > 0)
                    str += " ";
                    str += QString::number(y26[i]);
                }
                switch (m_ui->sB->value()){
                    case 1 : m_ui->text_1->setText(str2);
                             m_ui->text_1->setText(str);
                             m_ui->label_text1->setText(m_ui->logplot->graph(25)->name());break;
                    case 2 : m_ui->text_2->setText(str2);
                             m_ui->text_2->setText(str);
                             m_ui->label_text2->setText(m_ui->logplot->graph(25)->name());break;
                    case 3 : m_ui->text_3->setText(str2);
                             m_ui->text_3->setText(str);
                             m_ui->label_text3->setText(m_ui->logplot->graph(25)->name());break;
                    case 4 : m_ui->text_4->setText(str2);
                             m_ui->text_4->setText(str);
                             m_ui->label_text4->setText(m_ui->logplot->graph(25)->name());break;
                             break;return;
                    }
                    }
                  else{switch (m_ui->sB->value())
                {

                 case 1 : m_ui->text_1->setText(str2);
                          m_ui->label_text1->setText("");break;
                 case 2 : m_ui->text_2->setText(str2);
                          m_ui->label_text2->setText("");break;
                 case 3 : m_ui->text_3->setText(str2);
                          m_ui->label_text3->setText("");break;
                 case 4 : m_ui->text_4->setText(str2);
                          m_ui->label_text4->setText("");break;
                          break;return;
                }
                m_ui->logplot->graph(25)->setVisible(false);
                    m_ui->logplot->legend->item(25)->setVisible(false);
                    m_ui->sB->setValue(m_ui->sB->value()-1);
                     m_ui->logplot->replot();
                 }
         }
                    //27:::::::::::::::::::::::::::::::::::::::::::::::::::::::

    else if (((Reihe== 6)&&(Spalte==3))&&(m_ui->sB->value()<=4))
         {

            if( (m_ui->table_list->model()->index(Reihe,Spalte).data(Qt::CheckStateRole)==Qt::Checked)&&(m_ui->sB->value()<4))
                 {m_ui->sB->setValue(m_ui->sB->value()+1);
              switch (m_ui->sB->value()){
                                    case 1:  m_ui->logplot->graph(26)->setPen(QPen(Qt::blue));
                                         m_ui->label_text5->setText("blue");break;
                                    case 2: m_ui->logplot->graph(26)->setPen(QPen(Qt::cyan));
                                        m_ui->label_text6->setText("cyan");break;
                                    case 3: m_ui->logplot->graph(26)->setPen(QPen(Qt::red));
                                        m_ui->label_text7->setText("red");break;
                                    case 4: m_ui->logplot->graph(26)->setPen(QPen(Qt::green));
                                        m_ui->label_text8->setText("green");break;
                                    }
                 m_ui->logplot->graph(26)->setName("airfuelratio");
                m_ui->logplot->legend->item(26)->setVisible(true);
                 m_ui->logplot->graph(26)->setVisible(true);
                  m_ui->logplot->graph(26)->valueAxis()->setRange(0,100);
                 m_ui->logplot->graph(26)->rescaleValueAxis(true);
                    m_ui->logplot->replot();
                                                 str="";
                 for (int i = 0; i < y27.size(); ++i)
                {
                 if (i > 0)
                    str += " ";
                    str += QString::number(y27[i]);
                }
                switch (m_ui->sB->value()){
                    case 1 : m_ui->text_1->setText(str2);
                             m_ui->text_1->setText(str);
                             m_ui->label_text1->setText(m_ui->logplot->graph(26)->name());break;
                    case 2 : m_ui->text_2->setText(str2);
                             m_ui->text_2->setText(str);
                             m_ui->label_text2->setText(m_ui->logplot->graph(26)->name());break;
                    case 3 : m_ui->text_3->setText(str2);
                             m_ui->text_3->setText(str);
                             m_ui->label_text3->setText(m_ui->logplot->graph(26)->name());break;
                    case 4 : m_ui->text_4->setText(str2);
                             m_ui->text_4->setText(str);
                             m_ui->label_text4->setText(m_ui->logplot->graph(26)->name());break;
                             break;return;
                    }
                    }
                  else{switch (m_ui->sB->value())
                {

                 case 1 : m_ui->text_1->setText(str2);
                          m_ui->label_text1->setText("");break;
                 case 2 : m_ui->text_2->setText(str2);
                          m_ui->label_text2->setText("");break;
                 case 3 : m_ui->text_3->setText(str2);
                          m_ui->label_text3->setText("");break;
                 case 4 : m_ui->text_4->setText(str2);
                          m_ui->label_text4->setText("");break;
                          break;return;
                }
                m_ui->logplot->graph(26)->setVisible(false);
                    m_ui->logplot->legend->item(26)->setVisible(false);
          m_ui->sB->setValue(m_ui->sB->value()-1);
                 m_ui->logplot->replot();
             }
         }
        //28:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
    else if (((Reihe== 0)&&(Spalte==4))&&(m_ui->sB->value()<=4))
         {

            if( (m_ui->table_list->model()->index(Reihe,Spalte).data(Qt::CheckStateRole)==Qt::Checked)&&(m_ui->sB->value()<4))
             {m_ui->sB->setValue(m_ui->sB->value()+1);

                switch (m_ui->sB->value()){
                                    case 1:  m_ui->logplot->graph(27)->setPen(QPen(Qt::blue));
                                         m_ui->label_text5->setText("blue");break;
                                    case 2: m_ui->logplot->graph(27)->setPen(QPen(Qt::cyan));
                                        m_ui->label_text6->setText("cyan");break;
                                    case 3: m_ui->logplot->graph(27)->setPen(QPen(Qt::red));
                                        m_ui->label_text7->setText("red");break;
                                    case 4: m_ui->logplot->graph(27)->setPen(QPen(Qt::green));
                                        m_ui->label_text8->setText("green");break;
                                    }
                  m_ui->logplot->graph(27)->valueAxis()->setRange(0,100);
                 m_ui->logplot->graph(27)->setName("faultcode0");
                 m_ui->logplot->legend->item(27)->setVisible(true);
                 m_ui->logplot->graph(27)->setVisible(true);
                 m_ui->logplot->graph(27)->rescaleValueAxis(true);
                    m_ui->logplot->replot();
                                                 str="";
                 for (int i = 0; i < y28.size(); ++i)
                {
                 if (i > 0)
                    str += " ";
                    str += QString::number(y28[i]);
                }
                switch (m_ui->sB->value()){
                    case 1 : m_ui->text_1->setText(str2);
                             m_ui->text_1->setText(str);
                             m_ui->label_text1->setText(m_ui->logplot->graph(27)->name());break;
                    case 2 : m_ui->text_2->setText(str2);
                             m_ui->text_2->setText(str);
                             m_ui->label_text2->setText(m_ui->logplot->graph(27)->name());break;
                    case 3 : m_ui->text_3->setText(str2);
                             m_ui->text_3->setText(str);
                             m_ui->label_text3->setText(m_ui->logplot->graph(27)->name());break;
                    case 4 : m_ui->text_4->setText(str2);
                             m_ui->text_4->setText(str);
                             m_ui->label_text4->setText(m_ui->logplot->graph(27)->name());break;
                             break;return;
                    }
                    }
                  else{switch (m_ui->sB->value())
                {

                 case 1 : m_ui->text_1->setText(str2);
                          m_ui->label_text1->setText("");break;
                 case 2 : m_ui->text_2->setText(str2);
                          m_ui->label_text2->setText("");break;
                 case 3 : m_ui->text_3->setText(str2);
                          m_ui->label_text3->setText("");break;
                 case 4 : m_ui->text_4->setText(str2);
                          m_ui->label_text4->setText("");break;
                          break;return;
                }
                m_ui->logplot->graph(27)->setVisible(false);
                    m_ui->logplot->legend->item(27)->setVisible(false);
          m_ui->sB->setValue(m_ui->sB->value()-1);
                 m_ui->logplot->replot();
               }
         }
            //29::::::::::::::::::::::::::::::::::::::::::::::::::::
    else if (((Reihe== 1)&&(Spalte==4))&&(m_ui->sB->value()<=4))
        {

            if( (m_ui->table_list->model()->index(Reihe,Spalte).data(Qt::CheckStateRole)==Qt::Checked)&&(m_ui->sB->value()<4))
                 {
                 m_ui->sB->setValue(m_ui->sB->value()+1);
                 m_ui->logplot->graph(28)->setName("lambdavoltage");
                  m_ui->logplot->legend->item(28)->setVisible(true);
             switch (m_ui->sB->value()){
                                    case 1:  m_ui->logplot->graph(28)->setPen(QPen(Qt::blue));
                                         m_ui->label_text5->setText("blue");break;
                                    case 2: m_ui->logplot->graph(28)->setPen(QPen(Qt::cyan));
                                        m_ui->label_text6->setText("cyan");break;
                                    case 3: m_ui->logplot->graph(28)->setPen(QPen(Qt::red));
                                        m_ui->label_text7->setText("red");break;
                                    case 4: m_ui->logplot->graph(28)->setPen(QPen(Qt::green));
                                        m_ui->label_text8->setText("green");break;
                                    }

                 m_ui->logplot->graph(28)->setVisible(true);
                  m_ui->logplot->graph(28)->valueAxis()->setRange(0,1000);
                 m_ui->logplot->graph(28)->rescaleValueAxis(true);
                    m_ui->logplot->replot();
                                                 str="";
                 for (int i = 0; i < y29.size(); ++i)
                {
                 if (i > 0)
                    str += " ";
                    str += QString::number(y29[i]);
                }
                switch (m_ui->sB->value()){
                    case 1 : m_ui->text_1->setText(str2);
                             m_ui->text_1->setText(str);
                             m_ui->label_text1->setText(m_ui->logplot->graph(28)->name());break;
                    case 2 : m_ui->text_2->setText(str2);
                             m_ui->text_2->setText(str);
                             m_ui->label_text2->setText(m_ui->logplot->graph(28)->name());break;
                    case 3 : m_ui->text_3->setText(str2);
                             m_ui->text_3->setText(str);
                             m_ui->label_text3->setText(m_ui->logplot->graph(28)->name());break;
                    case 4 : m_ui->text_4->setText(str2);
                             m_ui->text_4->setText(str);
                             m_ui->label_text4->setText(m_ui->logplot->graph(28)->name());break;
                             break;return;
                    }
                    }
                  else{switch (m_ui->sB->value())
                {

                 case 1 : m_ui->text_1->setText(str2);
                          m_ui->label_text1->setText("");break;
                 case 2 : m_ui->text_2->setText(str2);
                          m_ui->label_text2->setText("");break;
                 case 3 : m_ui->text_3->setText(str2);
                          m_ui->label_text3->setText("");break;
                 case 4 : m_ui->text_4->setText(str2);
                          m_ui->label_text4->setText("");break;
                          break;return;
                }
                m_ui->logplot->graph(28)->valueAxis()->setRange(0,100);
                 m_ui->logplot->graph(28)->setVisible(false);
                   m_ui->logplot->legend->item(28)->setVisible(false);
          m_ui->sB->setValue(m_ui->sB->value()-1);
                 m_ui->logplot->replot();
             }
         }

                //30::::::::::::::::::::::::::::::::::::::::::::::::::::

    else if (((Reihe== 2)&&(Spalte==4))&&(m_ui->sB->value()<=4))
         {

            if( (m_ui->table_list->model()->index(Reihe,Spalte).data(Qt::CheckStateRole)==Qt::Checked)&&(m_ui->sB->value()<4))
                 {m_ui->sB->setValue(m_ui->sB->value()+1);
                switch (m_ui->sB->value()){
                                    case 1:  m_ui->logplot->graph(29)->setPen(QPen(Qt::blue));
                                         m_ui->label_text5->setText("blue");break;
                                    case 2: m_ui->logplot->graph(29)->setPen(QPen(Qt::cyan));
                                        m_ui->label_text6->setText("cyan");break;
                                    case 3: m_ui->logplot->graph(29)->setPen(QPen(Qt::red));
                                        m_ui->label_text7->setText("red");break;
                                    case 4: m_ui->logplot->graph(29)->setPen(QPen(Qt::green));
                                        m_ui->label_text8->setText("green");break;
                                    }

                 m_ui->logplot->graph(29)->setName("lambdafrequency");
                 m_ui->logplot->legend->item(29)->setVisible(true);
                 m_ui->logplot->graph(29)->setVisible(true);
                  m_ui->logplot->graph(29)->valueAxis()->setRange(0,100);
                 m_ui->logplot->graph(29)->rescaleValueAxis(true);
                    m_ui->logplot->replot();
                                                 str="";
                 for (int i = 0; i < y30.size(); ++i)
                {
                 if (i > 0)
                    str += " ";
                    str += QString::number(y30[i]);
                }
                switch (m_ui->sB->value()){
                    case 1 : m_ui->text_1->setText(str2);
                             m_ui->text_1->setText(str);
                             m_ui->label_text1->setText(m_ui->logplot->graph(29)->name());break;
                    case 2 : m_ui->text_2->setText(str2);
                             m_ui->text_2->setText(str);
                             m_ui->label_text1->setText(m_ui->logplot->graph(29)->name());break;
                    case 3 : m_ui->text_3->setText(str2);
                             m_ui->text_3->setText(str);
                             m_ui->label_text1->setText(m_ui->logplot->graph(29)->name());break;
                    case 4 : m_ui->text_4->setText(str2);
                             m_ui->text_4->setText(str);
                             m_ui->label_text1->setText(m_ui->logplot->graph(29)->name());break;
                             break;return;
                    }
                    }
                  else{switch (m_ui->sB->value())
                {

                 case 1 : m_ui->text_1->setText(str2);
                          m_ui->label_text1->setText("");break;
                 case 2 : m_ui->text_2->setText(str2);
                          m_ui->label_text2->setText("");break;
                 case 3 : m_ui->text_3->setText(str2);
                          m_ui->label_text3->setText("");break;
                 case 4 : m_ui->text_4->setText(str2);
                          m_ui->label_text4->setText("");break;
                          break;return;
                }
                m_ui->logplot->graph(29)->setVisible(false);
                    m_ui->logplot->legend->item(29)->setVisible(false);
          m_ui->sB->setValue(m_ui->sB->value()-1);
                 m_ui->logplot->replot();
            }
         }

      //31:::::::::::::::::::::::::::::::::::::::::::::::
    else if (((Reihe== 3)&&(Spalte==4))&&(m_ui->sB->value()<=4))
         {

                if ((m_ui->table_list->model()->index(Reihe,Spalte).data(Qt::CheckStateRole)==Qt::Checked)&&(m_ui->sB->value()<4))
                { m_ui->sB->setValue(m_ui->sB->value()+1);
                     switch (m_ui->sB->value()){
                                    case 1:  m_ui->logplot->graph(30)->setPen(QPen(Qt::blue));
                                         m_ui->label_text5->setText("blue");break;
                                    case 2: m_ui->logplot->graph(30)->setPen(QPen(Qt::cyan));
                                        m_ui->label_text6->setText("cyan");break;
                                    case 3: m_ui->logplot->graph(30)->setPen(QPen(Qt::red));
                                        m_ui->label_text7->setText("red");break;
                                    case 4: m_ui->logplot->graph(30)->setPen(QPen(Qt::green));
                                        m_ui->label_text8->setText("green");break;
                                    }
                  m_ui->logplot->graph(30)->valueAxis()->setRange(0,100);
                     m_ui->logplot->graph(30)->setName("lambdadutycycle");
                     m_ui->logplot->legend->item(30)->setVisible(true);
                     m_ui->logplot->graph(30)->setVisible(true);

                     m_ui->logplot->graph(30)->rescaleValueAxis(true);
                        m_ui->logplot->replot();
                                                     str="";
                     for (int i = 0; i < y31.size(); ++i)
                    {
                     if (i > 0)
                        str += " ";
                        str += QString::number(y31[i]);
                    }
                    switch (m_ui->sB->value())
                    {
                        case 1 : m_ui->text_1->setText(str2);
                                 m_ui->text_1->setText(str);
                                 m_ui->label_text1->setText(m_ui->logplot->graph(30)->name());break;
                        case 2 : m_ui->text_2->setText(str2);
                                 m_ui->text_2->setText(str);
                                 m_ui->label_text1->setText(m_ui->logplot->graph(30)->name());break;
                        case 3 : m_ui->text_3->setText(str2);
                                 m_ui->text_3->setText(str);
                                 m_ui->label_text1->setText(m_ui->logplot->graph(30)->name());break;
                        case 4 : m_ui->text_4->setText(str2);
                                 m_ui->text_4->setText(str);
                                 m_ui->label_text1->setText(m_ui->logplot->graph(30)->name());break;
                                 break;return;
                    }
                }
                  else
                  {switch (m_ui->sB->value())
                    {

                     case 1 : m_ui->text_1->setText(str2);
                              m_ui->label_text1->setText("");break;
                     case 2 : m_ui->text_2->setText(str2);
                              m_ui->label_text2->setText("");break;
                     case 3 : m_ui->text_3->setText(str2);
                              m_ui->label_text3->setText("");break;
                     case 4 : m_ui->text_4->setText(str2);
                              m_ui->label_text4->setText("");break;
                              break;return;
                    }
                    m_ui->logplot->graph(30)->setVisible(false);
                    m_ui->logplot->legend->item(30)->setVisible(false);
                    m_ui->sB->setValue(m_ui->sB->value()-1);
                    m_ui->logplot->replot();
                  }
         }
                        //32::::::::::::::::::::::::::::::::::::::::::::::
    else if (((Reihe== 4)&&(Spalte==4))&&(m_ui->sB->value()<=4))
         {

            if( (m_ui->table_list->model()->index(Reihe,Spalte).data(Qt::CheckStateRole)==Qt::Checked)&&(m_ui->sB->value()<4))
                 { m_ui->sB->setValue(m_ui->sB->value()+1);
                     switch (m_ui->sB->value()){
                                    case 1:  m_ui->logplot->graph(31)->setPen(QPen(Qt::blue));
                                         m_ui->label_text5->setText("blue");break;
                                    case 2: m_ui->logplot->graph(31)->setPen(QPen(Qt::cyan));
                                        m_ui->label_text6->setText("cyan");break;
                                    case 3: m_ui->logplot->graph(31)->setPen(QPen(Qt::red));
                                        m_ui->label_text7->setText("red");break;
                                    case 4: m_ui->logplot->graph(31)->setPen(QPen(Qt::green));
                                        m_ui->label_text8->setText("green");break;
                                    }

                 m_ui->logplot->graph(31)->setName("lambdastatus");
                 m_ui->logplot->legend->item(31)->setVisible(true);
                 m_ui->logplot->graph(31)->setVisible(true);
                  m_ui->logplot->graph(31)->valueAxis()->setRange(0,10);
                 m_ui->logplot->graph(31)->rescaleValueAxis(true);
                    m_ui->logplot->replot();

                                                 str="";
                 for (int i = 0; i < y32.size(); ++i)
                {
                 if (i > 0)
                    str += " ";
                    str += QString::number(y32[i]);
                }
                switch (m_ui->sB->value()){
                    case 1 : m_ui->text_1->setText(str2);
                             m_ui->text_1->setText(str);
                             m_ui->label_text1->setText(m_ui->logplot->graph(31)->name());break;
                    case 2 : m_ui->text_2->setText(str2);
                             m_ui->text_2->setText(str);
                             m_ui->label_text2->setText(m_ui->logplot->graph(31)->name());break;
                    case 3 : m_ui->text_3->setText(str2);
                             m_ui->text_3->setText(str);
                             m_ui->label_text3->setText(m_ui->logplot->graph(31)->name());break;
                    case 4 : m_ui->text_4->setText(str2);
                             m_ui->text_4->setText(str);
                             m_ui->label_text4->setText(m_ui->logplot->graph(31)->name());break;
                             break;return;
                    }
                    }
                  else{switch (m_ui->sB->value())
                {

                 case 1 : m_ui->text_1->setText(str2);
                          m_ui->label_text1->setText("");break;
                 case 2 : m_ui->text_2->setText(str2);
                          m_ui->label_text2->setText("");break;
                 case 3 : m_ui->text_3->setText(str2);
                          m_ui->label_text3->setText("");break;
                 case 4 : m_ui->text_4->setText(str2);
                          m_ui->label_text4->setText("");break;
                          break;return;
                }
                m_ui->logplot->graph(31)->setVisible(false);
                        m_ui->logplot->legend->item(31)->setVisible(false);
                        m_ui->sB->setValue(m_ui->sB->value()-1);
                        m_ui->logplot->replot();
                  }
         }
    //33:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
    else if (((Reihe== 5)&&(Spalte==4))&&(m_ui->sB->value()<=4))
         {

            if( (m_ui->table_list->model()->index(Reihe,Spalte).data(Qt::CheckStateRole)==Qt::Checked)&&(m_ui->sB->value()<4))
                 {m_ui->sB->setValue(m_ui->sB->value()+1);

                         switch (m_ui->sB->value()){
                                    case 1:  m_ui->logplot->graph(32)->setPen(QPen(Qt::blue));
                                         m_ui->label_text5->setText("blue");break;
                                    case 2: m_ui->logplot->graph(32)->setPen(QPen(Qt::cyan));
                                        m_ui->label_text6->setText("cyan");break;
                                    case 3: m_ui->logplot->graph(32)->setPen(QPen(Qt::red));
                                        m_ui->label_text7->setText("red");break;
                                    case 4: m_ui->logplot->graph(32)->setPen(QPen(Qt::green));
                                        m_ui->label_text8->setText("green");break;
                                    }


                 m_ui->logplot->graph(32)->setName("closedloop");
                 m_ui->logplot->legend->item(32)->setVisible(true);
                 m_ui->logplot->graph(32)->setVisible(true);
                  m_ui->logplot->graph(32)->valueAxis()->setRange(0,10);
                 m_ui->logplot->graph(32)->rescaleValueAxis(true);
                    m_ui->logplot->replot();

                                                 str="";
                 for (int i = 0; i < y33.size(); ++i)
                {
                 if (i > 0)
                    str += " ";
                    str += QString::number(y33[i]);
                }
                switch (m_ui->sB->value()){
                    case 1 : m_ui->text_1->setText(str2);
                             m_ui->text_1->setText(str);
                             m_ui->label_text1->setText(m_ui->logplot->graph(32)->name());break;
                    case 2 : m_ui->text_2->setText(str2);
                             m_ui->text_2->setText(str);
                             m_ui->label_text2->setText(m_ui->logplot->graph(32)->name());break;
                    case 3 : m_ui->text_3->setText(str2);
                             m_ui->text_3->setText(str);
                             m_ui->label_text3->setText(m_ui->logplot->graph(32)->name());break;
                    case 4 : m_ui->text_4->setText(str2);
                             m_ui->text_4->setText(str);
                             m_ui->label_text4->setText(m_ui->logplot->graph(32)->name());break;
                             break;return;
                    }
                    }
                  else{switch (m_ui->sB->value())
                {

                 case 1 : m_ui->text_1->setText(str2);
                          m_ui->label_text1->setText("");break;
                 case 2 : m_ui->text_2->setText(str2);
                          m_ui->label_text2->setText("");break;
                 case 3 : m_ui->text_3->setText(str2);
                          m_ui->label_text3->setText("");break;
                 case 4 : m_ui->text_4->setText(str2);
                          m_ui->label_text4->setText("");break;
                          break;return;
                }
                m_ui->logplot->graph(32)->setVisible(false);
                    m_ui->logplot->legend->item(32)->setVisible(false);
          m_ui->sB->setValue(m_ui->sB->value()-1);
                 m_ui->logplot->replot();
                 }
         }
     //34:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
    else if (((Reihe==6)&&(Spalte==4))&&(m_ui->sB->value()<=4))
         {

            if( (m_ui->table_list->model()->index(Reihe,Spalte).data(Qt::CheckStateRole)==Qt::Checked)&&(m_ui->sB->value()<4))
                 {m_ui->sB->setValue(m_ui->sB->value()+1);
                     switch (m_ui->sB->value()){
                                    case 1:  m_ui->logplot->graph(33)->setPen(QPen(Qt::blue));
                                         m_ui->label_text5->setText("blue");break;
                                    case 2: m_ui->logplot->graph(33)->setPen(QPen(Qt::cyan));
                                        m_ui->label_text6->setText("cyan");break;
                                    case 3: m_ui->logplot->graph(33)->setPen(QPen(Qt::red));
                                        m_ui->label_text7->setText("red");break;
                                    case 4: m_ui->logplot->graph(33)->setPen(QPen(Qt::green));
                                        m_ui->label_text8->setText("green");break;
                                    }
                  m_ui->logplot->graph(33)->valueAxis()->setRange(0,150);

                 m_ui->logplot->graph(33)->setName("long term fT");
                 m_ui->logplot->legend->item(33)->setVisible(true);
                 m_ui->logplot->graph(33)->setVisible(true);
                 m_ui->logplot->graph(33)->rescaleValueAxis(true);
                    m_ui->logplot->replot();

                                                 str="";
                 for (int i = 0; i < y34.size(); ++i)
                {
                 if (i > 0)
                    str += " ";
                    str += QString::number(y34[i]);
                }
                switch (m_ui->sB->value()){
                    case 1 : m_ui->text_1->setText(str2);
                             m_ui->text_1->setText(str);
                             m_ui->label_text1->setText(m_ui->logplot->graph(33)->name());break;
                    case 2 : m_ui->text_2->setText(str2);
                             m_ui->text_2->setText(str);
                             m_ui->label_text2->setText(m_ui->logplot->graph(33)->name());break;
                    case 3 : m_ui->text_3->setText(str2);
                             m_ui->text_3->setText(str);
                             m_ui->label_text3->setText(m_ui->logplot->graph(33)->name());break;
                    case 4 : m_ui->text_4->setText(str2);
                             m_ui->text_4->setText(str);
                             m_ui->label_text4->setText(m_ui->logplot->graph(33)->name());break;
                             break;return;
                    }
                    }
                  else{switch (m_ui->sB->value())
                {

                 case 1 : m_ui->text_1->setText(str2);
                          m_ui->label_text1->setText("");break;
                 case 2 : m_ui->text_2->setText(str2);
                          m_ui->label_text2->setText("");break;
                 case 3 : m_ui->text_3->setText(str2);
                          m_ui->label_text3->setText("");break;
                 case 4 : m_ui->text_4->setText(str2);
                          m_ui->label_text4->setText("");break;
                          break;return;
                }
                  m_ui->logplot->graph(33)->setVisible(false);
                    m_ui->logplot->legend->item(33)->setVisible(false);
          m_ui->sB->setValue(m_ui->sB->value()-1);
                 m_ui->logplot->replot();
                  }
         }
                //35:::::::::::::::::::::::::::::::::::::::::::::::::
    else if (((Reihe== 0)&&(Spalte==5))&&(m_ui->sB->value()<=4))
         {

            if( (m_ui->table_list->model()->index(Reihe,Spalte).data(Qt::CheckStateRole)==Qt::Checked)&&(m_ui->sB->value()<4))
                 {m_ui->sB->setValue(m_ui->sB->value()+1);
                     switch (m_ui->sB->value()){
                                    case 1:  m_ui->logplot->graph(34)->setPen(QPen(Qt::blue));
                                         m_ui->label_text5->setText("blue");break;
                                    case 2: m_ui->logplot->graph(34)->setPen(QPen(Qt::cyan));
                                        m_ui->label_text6->setText("cyan");break;
                                    case 3: m_ui->logplot->graph(34)->setPen(QPen(Qt::red));
                                        m_ui->label_text7->setText("red");break;
                                    case 4: m_ui->logplot->graph(34)->setPen(QPen(Qt::green));
                                        m_ui->label_text8->setText("green");break;
                                    }


                 m_ui->logplot->graph(34)->setName("shorttermfueltrim");
                 m_ui->logplot->legend->item(34)->setVisible(true);
               m_ui->logplot->graph(34)->setVisible(true);
                  m_ui->logplot->graph(34)->valueAxis()->setRange(0,150);
                  m_ui->logplot->graph(34)->rescaleValueAxis(true);
                    m_ui->logplot->replot();

                                                 str="";
                 for (int i = 0; i < y35.size(); ++i)
                {
                 if (i > 0)
                    str += " ";
                    str += QString::number(y35[i]);
                }
                switch (m_ui->sB->value()){
                    case 1 : m_ui->text_1->setText(str2);
                             m_ui->text_1->setText(str);
                             m_ui->label_text1->setText(m_ui->logplot->graph(34)->name());break;
                    case 2 : m_ui->text_2->setText(str2);
                             m_ui->text_2->setText(str);
                             m_ui->label_text2->setText(m_ui->logplot->graph(34)->name());break;
                    case 3 : m_ui->text_3->setText(str2);
                             m_ui->text_3->setText(str);
                             m_ui->label_text3->setText(m_ui->logplot->graph(34)->name());break;
                    case 4 : m_ui->text_4->setText(str2);
                             m_ui->text_4->setText(str);
                             m_ui->label_text4->setText(m_ui->logplot->graph(34)->name());break;
                             break;return;
                    }
                    }
                  else{switch (m_ui->sB->value())
                {

                 case 1 : m_ui->text_1->setText(str2);
                          m_ui->label_text1->setText("");break;
                 case 2 : m_ui->text_2->setText(str2);
                          m_ui->label_text2->setText("");break;
                 case 3 : m_ui->text_3->setText(str2);
                          m_ui->label_text3->setText("");break;
                 case 4 : m_ui->text_4->setText(str2);
                          m_ui->label_text4->setText("");break;
                          break;return;
                }
                 m_ui->logplot->graph(34)->setVisible(false);
                   m_ui->logplot->legend->item(34)->setVisible(false);
          m_ui->sB->setValue(m_ui->sB->value()-1);
                 m_ui->logplot->replot();
              }
         }
      //36:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
    else if (((Reihe== 1)&&(Spalte==5))&&(m_ui->sB->value()<=4))
         {

            if( (m_ui->table_list->model()->index(Reihe,Spalte).data(Qt::CheckStateRole)==Qt::Checked)&&(m_ui->sB->value()<4))
                 {m_ui->sB->setValue(m_ui->sB->value()+1);
                     switch (m_ui->sB->value()){
                                    case 1:  m_ui->logplot->graph(35)->setPen(QPen(Qt::blue));
                                         m_ui->label_text5->setText("blue");break;
                                    case 2: m_ui->logplot->graph(35)->setPen(QPen(Qt::cyan));
                                        m_ui->label_text6->setText("cyan");break;
                                    case 3: m_ui->logplot->graph(35)->setPen(QPen(Qt::red));
                                        m_ui->label_text7->setText("red");break;
                                    case 4: m_ui->logplot->graph(35)->setPen(QPen(Qt::green));
                                        m_ui->label_text8->setText("green");break;
                                    }


                 m_ui->logplot->graph(35)->setName("carboncanister");
                 m_ui->logplot->legend->item(35)->setVisible(true);
                 m_ui->logplot->graph(35)->setVisible(true);
                  m_ui->logplot->graph(35)->valueAxis()->setRange(0,100);
                  m_ui->logplot->graph(35)->rescaleValueAxis(true);
                    m_ui->logplot->replot();

                                                 str="";
                 for (int i = 0; i < y36.size(); ++i)
                {
                 if (i > 0)
                    str += " ";
                    str += QString::number(y36[i]);
                }
                switch (m_ui->sB->value()){
                    case 1 : m_ui->text_1->setText(str2);
                             m_ui->text_1->setText(str);
                             m_ui->label_text1->setText(m_ui->logplot->graph(35)->name());break;
                    case 2 : m_ui->text_2->setText(str2);
                             m_ui->text_2->setText(str);
                             m_ui->label_text2->setText(m_ui->logplot->graph(35)->name());break;
                    case 3 : m_ui->text_3->setText(str2);
                             m_ui->text_3->setText(str);
                             m_ui->label_text3->setText(m_ui->logplot->graph(35)->name());break;
                    case 4 : m_ui->text_4->setText(str2);
                             m_ui->text_4->setText(str);
                             m_ui->label_text4->setText(m_ui->logplot->graph(35)->name());break;
                             break;return;
                    }
                    }
                  else{switch (m_ui->sB->value())
                {

                 case 1 : m_ui->text_1->setText(str2);
                          m_ui->label_text1->setText("");break;
                 case 2 : m_ui->text_2->setText(str2);
                          m_ui->label_text2->setText("");break;
                 case 3 : m_ui->text_3->setText(str2);
                          m_ui->label_text3->setText("");break;
                 case 4 : m_ui->text_4->setText(str2);
                          m_ui->label_text4->setText("");break;
                          break;return;
                }
                m_ui->logplot->graph(35)->setVisible(false);
                    m_ui->logplot->legend->item(35)->setVisible(false);
          m_ui->sB->setValue(m_ui->sB->value()-1);
                 m_ui->logplot->replot();
               }
         }
    //37:::::::::::::::::::::::::::::::::::::::::::::::::::::::::

    else if (((Reihe== 2)&&(Spalte==5))&&(m_ui->sB->value()<=4))
         {

            if( (m_ui->table_list->model()->index(Reihe,Spalte).data(Qt::CheckStateRole)==Qt::Checked)&&(m_ui->sB->value()<4))
                 {m_ui->sB->setValue(m_ui->sB->value()+1);
                     switch (m_ui->sB->value()){
                                    case 1:  m_ui->logplot->graph(36)->setPen(QPen(Qt::blue));
                                         m_ui->label_text5->setText("blue");break;
                                    case 2: m_ui->logplot->graph(36)->setPen(QPen(Qt::cyan));
                                        m_ui->label_text6->setText("cyan");break;
                                    case 3: m_ui->logplot->graph(36)->setPen(QPen(Qt::red));
                                        m_ui->label_text7->setText("red");break;
                                    case 4: m_ui->logplot->graph(36)->setPen(QPen(Qt::green));
                                        m_ui->label_text8->setText("green");break;
                                    }
                  m_ui->logplot->graph(36)->valueAxis()->setRange(0,100);

                 m_ui->logplot->graph(36)->setName("faultcode 1");
                 m_ui->logplot->legend->item(36)->setVisible(true);
                m_ui->logplot->graph(36)->setVisible(true);
                  m_ui->logplot->graph(36)->rescaleValueAxis(true);
                    m_ui->logplot->replot();

                                                 str="";
                 for (int i = 0; i < y37.size(); ++i)
                {
                 if (i > 0)
                    str += " ";
                    str += QString::number(y37[i]);
                }
                switch (m_ui->sB->value()){
                    case 1 : m_ui->text_1->setText(str2);
                             m_ui->text_1->setText(str);
                             m_ui->label_text1->setText(m_ui->logplot->graph(36)->name());break;
                    case 2 : m_ui->text_2->setText(str2);
                             m_ui->text_2->setText(str);
                             m_ui->label_text2->setText(m_ui->logplot->graph(36)->name());break;
                    case 3 : m_ui->text_3->setText(str2);
                             m_ui->text_3->setText(str);
                             m_ui->label_text3->setText(m_ui->logplot->graph(36)->name());break;
                    case 4 : m_ui->text_4->setText(str2);
                             m_ui->text_4->setText(str);
                             m_ui->label_text4->setText(m_ui->logplot->graph(36)->name());break;
                             break;return;
                    }
                    }
                  else{switch (m_ui->sB->value())
                {

                 case 1 : m_ui->text_1->setText(str2);
                          m_ui->label_text1->setText("");break;
                 case 2 : m_ui->text_2->setText(str2);
                          m_ui->label_text2->setText("");break;
                 case 3 : m_ui->text_3->setText(str2);
                          m_ui->label_text3->setText("");break;
                 case 4 : m_ui->text_4->setText(str2);
                          m_ui->label_text4->setText("");break;
                          break;return;
                }
                m_ui->logplot->graph(36)->setVisible(false);
                    m_ui->logplot->legend->item(36)->setVisible(false);
          m_ui->sB->setValue(m_ui->sB->value()-1);
                 m_ui->logplot->replot();
               }
         }
      //38:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
    else if (((Reihe== 3)&&(Spalte==5))&&(m_ui->sB->value()<=4))
         {

            if( (m_ui->table_list->model()->index(Reihe,Spalte).data(Qt::CheckStateRole)==Qt::Checked)&&(m_ui->sB->value()<4))
         {m_ui->sB->setValue(m_ui->sB->value()+1);

                     switch (m_ui->sB->value()){
                                    case 1:  m_ui->logplot->graph(37)->setPen(QPen(Qt::blue));
                                         m_ui->label_text5->setText("blue");break;
                                    case 2: m_ui->logplot->graph(37)->setPen(QPen(Qt::cyan));
                                        m_ui->label_text6->setText("cyan");break;
                                    case 3: m_ui->logplot->graph(37)->setPen(QPen(Qt::red));
                                        m_ui->label_text7->setText("red");break;
                                    case 4: m_ui->logplot->graph(37)->setPen(QPen(Qt::green));
                                        m_ui->label_text8->setText("green");break;
                                    }


                 m_ui->logplot->graph(37)->setName("idlebase");
                  m_ui->logplot->legend->item(37)->setVisible(true);
                 m_ui->logplot->graph(37)->setVisible(true);
                  m_ui->logplot->graph(37)->valueAxis()->setRange(0,100);
                  m_ui->logplot->graph(37)->rescaleValueAxis(true);
                    m_ui->logplot->replot();

                                                 str="";
                 for (int i = 0; i < y38.size(); ++i)
                {
                 if (i > 0)
                    str += " ";
                    str += QString::number(y38[i]);
                }
                switch (m_ui->sB->value()){
                    case 1 : m_ui->text_1->setText(str2);
                             m_ui->text_1->setText(str);
                             m_ui->label_text1->setText(m_ui->logplot->graph(37)->name());break;
                    case 2 : m_ui->text_2->setText(str2);
                             m_ui->text_2->setText(str);
                             m_ui->label_text2->setText(m_ui->logplot->graph(37)->name());break;
                    case 3 : m_ui->text_3->setText(str2);
                             m_ui->text_3->setText(str);
                             m_ui->label_text3->setText(m_ui->logplot->graph(37)->name());break;
                    case 4 : m_ui->text_4->setText(str2);
                             m_ui->text_4->setText(str);
                             m_ui->label_text4->setText(m_ui->logplot->graph(37)->name());break;
                             break;return;
                    }
                    }
                  else{switch (m_ui->sB->value())
                {

                 case 1 : m_ui->text_1->setText(str2);
                          m_ui->label_text1->setText("");break;
                 case 2 : m_ui->text_2->setText(str2);
                          m_ui->label_text2->setText("");break;
                 case 3 : m_ui->text_3->setText(str2);
                          m_ui->label_text3->setText("");break;
                 case 4 : m_ui->text_4->setText(str2);
                          m_ui->label_text4->setText("");break;
                          break;return;
                }
                m_ui->logplot->graph(37)->setVisible(false);
                    m_ui->logplot->legend->item(37)->setVisible(false);
          m_ui->sB->setValue(m_ui->sB->value()-1);
                 m_ui->logplot->replot();
                }
         }
          //39::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
    else if (((Reihe== 4)&&(Spalte==5))&&(m_ui->sB->value()<=4))
         {

            if( (m_ui->table_list->model()->index(Reihe,Spalte).data(Qt::CheckStateRole)==Qt::Checked)&&(m_ui->sB->value()<4))
             {m_ui->sB->setValue(m_ui->sB->value()+1);

                     switch (m_ui->sB->value()){
                                    case 1:  m_ui->logplot->graph(38)->setPen(QPen(Qt::blue));
                                         m_ui->label_text5->setText("blue");break;
                                    case 2: m_ui->logplot->graph(38)->setPen(QPen(Qt::cyan));
                                        m_ui->label_text6->setText("cyan");break;
                                    case 3: m_ui->logplot->graph(38)->setPen(QPen(Qt::red));
                                        m_ui->label_text7->setText("red");break;
                                    case 4: m_ui->logplot->graph(38)->setPen(QPen(Qt::green));
                                        m_ui->label_text8->setText("green");break;
                                    }
                  m_ui->logplot->graph(38)->valueAxis()->setRange(0,100);

                 m_ui->logplot->graph(38)->setName("unknown7");
                  m_ui->logplot->legend->item(38)->setVisible(true);

                 m_ui->logplot->graph(38)->setVisible(true);
                  m_ui->logplot->graph(38)->rescaleValueAxis(true);
                    m_ui->logplot->replot();

                                                 str="";
                 for (int i = 0; i < y39.size(); ++i)
                {
                 if (i > 0)
                    str += " ";
                    str += QString::number(y39[i]);
                }
                switch (m_ui->sB->value()){
                    case 1 : m_ui->text_1->setText(str2);
                             m_ui->text_1->setText(str);
                             m_ui->label_text1->setText(m_ui->logplot->graph(38)->name());break;
                    case 2 : m_ui->text_2->setText(str2);
                             m_ui->text_2->setText(str);
                             m_ui->label_text2->setText(m_ui->logplot->graph(38)->name());break;
                    case 3 : m_ui->text_3->setText(str2);
                             m_ui->text_3->setText(str);
                             m_ui->label_text3->setText(m_ui->logplot->graph(38)->name());break;
                    case 4 : m_ui->text_4->setText(str2);
                             m_ui->text_4->setText(str);
                             m_ui->label_text4->setText(m_ui->logplot->graph(38)->name());break;
                             break;return;
                    }
                    }
                  else{switch (m_ui->sB->value())
                {

                 case 1 : m_ui->text_1->setText(str2);
                          m_ui->label_text1->setText("");break;
                 case 2 : m_ui->text_2->setText(str2);
                          m_ui->label_text2->setText("");break;
                 case 3 : m_ui->text_3->setText(str2);
                          m_ui->label_text3->setText("");break;
                 case 4 : m_ui->text_4->setText(str2);
                          m_ui->label_text4->setText("");break;
                          break;return;
                }
                m_ui->logplot->graph(38)->setVisible(false);
                    m_ui->logplot->legend->item(38)->setVisible(false);
          m_ui->sB->setValue(m_ui->sB->value()-1);
                 m_ui->logplot->replot();
                }
         }
              //40:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
    else if (((Reihe== 5)&&(Spalte==5))&&(m_ui->sB->value()<=4))
         {

            if( (m_ui->table_list->model()->index(Reihe,Spalte).data(Qt::CheckStateRole)==Qt::Checked)&&(m_ui->sB->value()<4))
                 {m_ui->sB->setValue(m_ui->sB->value()+1);
                         switch (m_ui->sB->value()){
                                    case 1:  m_ui->logplot->graph(39)->setPen(QPen(Qt::blue));
                                         m_ui->label_text5->setText("blue");break;
                                    case 2: m_ui->logplot->graph(39)->setPen(QPen(Qt::cyan));
                                        m_ui->label_text6->setText("cyan");break;
                                    case 3: m_ui->logplot->graph(39)->setPen(QPen(Qt::red));
                                        m_ui->label_text7->setText("red");break;
                                    case 4: m_ui->logplot->graph(39)->setPen(QPen(Qt::green));
                                        m_ui->label_text8->setText("green");break;
                                    }
                  m_ui->logplot->graph(39)->valueAxis()->setRange(0,100);

                 m_ui->logplot->graph(39)->setName("unknown 8");
                  m_ui->logplot->legend->item(39)->setVisible(true);

                 m_ui->logplot->graph(39)->setVisible(true);
                  m_ui->logplot->graph(39)->rescaleValueAxis(true);
                    m_ui->logplot->replot();

                                                 str="";
                 for (int i = 0; i < y40.size(); ++i)
                {
                 if (i > 0)
                    str += " ";
                    str += QString::number(y40[i]);
                }
                switch (m_ui->sB->value()){
                    case 1 : m_ui->text_1->setText(str2);
                             m_ui->text_1->setText(str);
                             m_ui->label_text1->setText(m_ui->logplot->graph(39)->name());break;
                    case 2 : m_ui->text_2->setText(str2);
                             m_ui->text_2->setText(str);
                             m_ui->label_text2->setText(m_ui->logplot->graph(39)->name());break;
                    case 3 : m_ui->text_3->setText(str2);
                             m_ui->text_3->setText(str);
                             m_ui->label_text3->setText(m_ui->logplot->graph(39)->name());break;
                    case 4 : m_ui->text_4->setText(str2);
                             m_ui->text_4->setText(str);
                             m_ui->label_text4->setText(m_ui->logplot->graph(39)->name());break;
                             break;return;
                    }
                    }
                  else{switch (m_ui->sB->value())
                {

                 case 1 : m_ui->text_1->setText(str2);
                          m_ui->label_text1->setText("");break;
                 case 2 : m_ui->text_2->setText(str2);
                          m_ui->label_text2->setText("");break;
                 case 3 : m_ui->text_3->setText(str2);
                          m_ui->label_text3->setText("");break;
                 case 4 : m_ui->text_4->setText(str2);
                          m_ui->label_text4->setText("");break;
                          break;return;
                }
                m_ui->logplot->graph(39)->setVisible(false);
                    m_ui->logplot->legend->item(39)->setVisible(false);
          m_ui->sB->setValue(m_ui->sB->value()-1);
                 m_ui->logplot->replot();
                }
         }




      //41:::::::::::::::::::::::::::::::::::::::::::::::
    else if (((Reihe== 6)&&(Spalte==5))&&(m_ui->sB->value()<=4))
         {

            if( (m_ui->table_list->model()->index(Reihe,Spalte).data(Qt::CheckStateRole)==Qt::Checked)&&(m_ui->sB->value()<4))
                 {m_ui->sB->setValue(m_ui->sB->value()+1);

                     switch (m_ui->sB->value()){
                                    case 1:  m_ui->logplot->graph(40)->setPen(QPen(Qt::blue));
                                         m_ui->label_text5->setText("blue");break;
                                    case 2: m_ui->logplot->graph(40)->setPen(QPen(Qt::cyan));
                                        m_ui->label_text6->setText("cyan");break;
                                    case 3: m_ui->logplot->graph(40)->setPen(QPen(Qt::red));
                                        m_ui->label_text7->setText("red");break;
                                    case 4: m_ui->logplot->graph(40)->setPen(QPen(Qt::green));
                                        m_ui->label_text8->setText("green");break;
                                    }
                  m_ui->logplot->graph(40)->valueAxis()->setRange(0,100);

                 m_ui->logplot->graph(40)->setName("ignadv2");
                  m_ui->logplot->legend->item(40)->setVisible(true);

                m_ui->logplot->graph(40)->setVisible(true);
                 m_ui->logplot->graph(40)->rescaleValueAxis(true);
                    m_ui->logplot->replot();
                                                                 str="";
                 for (int i = 0; i < y41.size(); ++i)
                {
                 if (i > 0)
                    str += " ";
                    str += QString::number(y41[i]);
                }
                switch (m_ui->sB->value()){
                    case 1 : m_ui->text_1->setText(str2);
                             m_ui->text_1->setText(str);
                             m_ui->label_text1->setText(m_ui->logplot->graph(40)->name());break;
                    case 2 : m_ui->text_2->setText(str2);
                             m_ui->text_2->setText(str);
                             m_ui->label_text2->setText(m_ui->logplot->graph(40)->name());break;
                    case 3 : m_ui->text_3->setText(str2);
                             m_ui->text_3->setText(str);
                             m_ui->label_text3->setText(m_ui->logplot->graph(40)->name());break;
                    case 4 : m_ui->text_4->setText(str2);
                             m_ui->text_4->setText(str);
                             m_ui->label_text4->setText(m_ui->logplot->graph(40)->name());break;
                             break;return;
                    }
                }
                  else{switch (m_ui->sB->value())
                {

                 case 1 : m_ui->text_1->setText(str2);
                          m_ui->label_text1->setText("");break;
                 case 2 : m_ui->text_2->setText(str2);
                          m_ui->label_text2->setText("");break;
                 case 3 : m_ui->text_3->setText(str2);
                          m_ui->label_text3->setText("");break;
                 case 4 : m_ui->text_4->setText(str2);
                          m_ui->label_text4->setText("");break;
                          break;return;
                }
                m_ui->logplot->graph(40)->setVisible(false);
                    m_ui->logplot->legend->item(40)->setVisible(false);
          m_ui->sB->setValue(m_ui->sB->value()-1);
                 m_ui->logplot->replot();
               }
         }
     //42:::::::::::::::::::::::::::::::::::::::::::::
    else if (((Reihe== 0)&&(Spalte==6))&&(m_ui->sB->value()<=4))
         {

            if( (m_ui->table_list->model()->index(Reihe,Spalte).data(Qt::CheckStateRole)==Qt::Checked)&&(m_ui->sB->value()<4))
                 {m_ui->sB->setValue(m_ui->sB->value()+1);

                         switch (m_ui->sB->value()){
                                    case 1:  m_ui->logplot->graph(41)->setPen(QPen(Qt::blue));
                                         m_ui->label_text5->setText("blue");break;
                                    case 2: m_ui->logplot->graph(41)->setPen(QPen(Qt::cyan));
                                        m_ui->label_text6->setText("cyan");break;
                                    case 3: m_ui->logplot->graph(41)->setPen(QPen(Qt::red));
                                        m_ui->label_text7->setText("red");break;
                                    case 4: m_ui->logplot->graph(41)->setPen(QPen(Qt::green));
                                        m_ui->label_text8->setText("green");break;
                                    }
                  m_ui->logplot->graph(41)->valueAxis()->setRange(0,100);

                 m_ui->logplot->graph(41)->setName("uk9");
                  m_ui->logplot->legend->item(41)->setVisible(true);

                 m_ui->logplot->graph(41)->setVisible(true);

                 m_ui->logplot->graph(41)->rescaleValueAxis(true);
                    m_ui->logplot->replot();
                                                                 str="";
                 for (int i = 0; i < y42.size(); ++i)
                {
                 if (i > 0)
                    str += " ";
                    str += QString::number(y42[i]);
                }
                switch (m_ui->sB->value()){
                    case 1 : m_ui->text_1->setText(str2);
                             m_ui->text_1->setText(str);
                             m_ui->label_text1->setText(m_ui->logplot->graph(41)->name());break;
                    case 2 : m_ui->text_2->setText(str2);
                             m_ui->text_2->setText(str);
                             m_ui->label_text2->setText(m_ui->logplot->graph(41)->name());break;
                    case 3 : m_ui->text_3->setText(str2);
                             m_ui->text_3->setText(str);
                             m_ui->label_text3->setText(m_ui->logplot->graph(41)->name());break;
                    case 4 : m_ui->text_4->setText(str2);
                             m_ui->text_4->setText(str);
                             m_ui->label_text4->setText(m_ui->logplot->graph(41)->name());break;
                             break;return;
                    }
                    }
                  else{switch (m_ui->sB->value())
                {

                 case 1 : m_ui->text_1->setText(str2);
                          m_ui->label_text1->setText("");break;
                 case 2 : m_ui->text_2->setText(str2);
                          m_ui->label_text2->setText("");break;
                 case 3 : m_ui->text_3->setText(str2);
                          m_ui->label_text3->setText("");break;
                 case 4 : m_ui->text_4->setText(str2);
                          m_ui->label_text4->setText("");break;
                          break;return;
                }
                m_ui->logplot->graph(41)->setVisible(false);
                    m_ui->logplot->legend->item(41)->setVisible(false);
          m_ui->sB->setValue(m_ui->sB->value()-1);
                 m_ui->logplot->replot();
                 }
         }
      //43::::::::::::::::::::::::::::::::::::::::
    else if (((Reihe== 1)&&(Spalte==6))&&(m_ui->sB->value()<=4))
         {

            if( (m_ui->table_list->model()->index(Reihe,Spalte).data(Qt::CheckStateRole)==Qt::Checked)&&(m_ui->sB->value()<4))
                 {m_ui->sB->setValue(m_ui->sB->value()+1);

                         switch (m_ui->sB->value()){
                                    case 1:  m_ui->logplot->graph(42)->setPen(QPen(Qt::blue));
                                         m_ui->label_text5->setText("blue");break;
                                    case 2: m_ui->logplot->graph(42)->setPen(QPen(Qt::cyan));
                                        m_ui->label_text6->setText("cyan");break;
                                    case 3: m_ui->logplot->graph(42)->setPen(QPen(Qt::red));
                                        m_ui->label_text7->setText("red");break;
                                    case 4: m_ui->logplot->graph(42)->setPen(QPen(Qt::green));
                                        m_ui->label_text8->setText("green");break;
                                    }
                  m_ui->logplot->graph(42)->valueAxis()->setRange(0,100);

                 m_ui->logplot->graph(42)->setName("idleerror2");
                  m_ui->logplot->legend->item(42)->setVisible(true);

                 m_ui->logplot->graph(42)->setVisible(true);
                 m_ui->logplot->graph(42)->rescaleValueAxis(true);
                    m_ui->logplot->replot();
                                                                                 str="";
                 for (int i = 0; i < y43.size(); ++i)
                {
                 if (i > 0)
                    str += " ";
                    str += QString::number(y43[i]);
                }
                switch (m_ui->sB->value()){
                    case 1 : m_ui->text_1->setText(str2);
                             m_ui->text_1->setText(str);
                             m_ui->label_text1->setText(m_ui->logplot->graph(42)->name());break;
                    case 2 : m_ui->text_2->setText(str2);
                             m_ui->text_2->setText(str);
                             m_ui->label_text2->setText(m_ui->logplot->graph(42)->name());break;
                    case 3 : m_ui->text_3->setText(str2);
                             m_ui->text_3->setText(str);
                             m_ui->label_text3->setText(m_ui->logplot->graph(42)->name());break;
                    case 4 : m_ui->text_4->setText(str2);
                             m_ui->text_4->setText(str);
                             m_ui->label_text4->setText(m_ui->logplot->graph(42)->name());break;
                             break;return;
                    }
                    }
                  else{switch (m_ui->sB->value())
                {

                 case 1 : m_ui->text_1->setText(str2);
                          m_ui->label_text1->setText("");break;
                 case 2 : m_ui->text_2->setText(str2);
                          m_ui->label_text2->setText("");break;
                 case 3 : m_ui->text_3->setText(str2);
                          m_ui->label_text3->setText("");break;
                 case 4 : m_ui->text_4->setText(str2);
                          m_ui->label_text4->setText("");break;
                          break;return;
                }
                m_ui->logplot->graph(42)->setVisible(false);
                    m_ui->logplot->legend->item(42)->setVisible(false);
          m_ui->sB->setValue(m_ui->sB->value()-1);
                 m_ui->logplot->replot();
              }
         }
     //44::::::::::::::::::::::::::::::::::::::::::::::::::::::::
    else if (((Reihe== 2)&&(Spalte==6))&&(m_ui->sB->value()<=4))
         {

            if( (m_ui->table_list->model()->index(Reihe,Spalte).data(Qt::CheckStateRole)==Qt::Checked)&&(m_ui->sB->value()<4))
                {m_ui->sB->setValue(m_ui->sB->value()+1);

                         switch (m_ui->sB->value()){
                                    case 1:  m_ui->logplot->graph(43)->setPen(QPen(Qt::blue));
                                         m_ui->label_text5->setText("blue");break;
                                    case 2: m_ui->logplot->graph(43)->setPen(QPen(Qt::cyan));
                                        m_ui->label_text6->setText("cyan");break;
                                    case 3: m_ui->logplot->graph(43)->setPen(QPen(Qt::red));
                                        m_ui->label_text7->setText("red");break;
                                    case 4: m_ui->logplot->graph(43)->setPen(QPen(Qt::green));
                                        m_ui->label_text8->setText("green");break;
                                    }
                  m_ui->logplot->graph(43)->valueAxis()->setRange(0,100);

                 m_ui->logplot->graph(43)->setName("uk10");
                  m_ui->logplot->legend->item(43)->setVisible(true);

                 m_ui->logplot->graph(43)->setVisible(true);
                 m_ui->logplot->graph(43)->rescaleValueAxis(true);
                    m_ui->logplot->replot();

                                                                                 str="";
                 for (int i = 0; i < y44.size(); ++i)
                {
                 if (i > 0)
                    str += " ";
                    str += QString::number(y44[i]);
                }
                switch (m_ui->sB->value()){
                    case 1 : m_ui->text_1->setText(str2);
                             m_ui->text_1->setText(str);
                             m_ui->label_text1->setText(m_ui->logplot->graph(43)->name());break;
                    case 2 : m_ui->text_2->setText(str2);
                             m_ui->text_2->setText(str);
                             m_ui->label_text2->setText(m_ui->logplot->graph(43)->name());break;
                    case 3 : m_ui->text_3->setText(str2);
                             m_ui->text_3->setText(str);
                             m_ui->label_text3->setText(m_ui->logplot->graph(43)->name());break;
                    case 4 : m_ui->text_4->setText(str2);
                             m_ui->text_4->setText(str);
                             m_ui->label_text4->setText(m_ui->logplot->graph(43)->name());break;
                             break;return;
                    }}
                  else{switch (m_ui->sB->value())
                {

                 case 1 : m_ui->text_1->setText(str2);
                          m_ui->label_text1->setText("");break;
                 case 2 : m_ui->text_2->setText(str2);
                          m_ui->label_text2->setText("");break;
                 case 3 : m_ui->text_3->setText(str2);
                          m_ui->label_text3->setText("");break;
                 case 4 : m_ui->text_4->setText(str2);
                          m_ui->label_text4->setText("");break;
                          break;return;
                }
                m_ui->logplot->graph(43)->setVisible(false);
                    m_ui->logplot->legend->item(43)->setVisible(false);
          m_ui->sB->setValue(m_ui->sB->value()-1);
                 m_ui->logplot->replot();
            }
         }
     //45::::::::::::::::::::::::::::::::::::::::::::::::::::::
    else if (((Reihe== 3)&&(Spalte==6))&&(m_ui->sB->value()<=4))
         {

            if( (m_ui->table_list->model()->index(Reihe,Spalte).data(Qt::CheckStateRole)==Qt::Checked)&&(m_ui->sB->value()<4))
            {m_ui->sB->setValue(m_ui->sB->value()+1);
                     switch (m_ui->sB->value()){
                                    case 1:  m_ui->logplot->graph(44)->setPen(QPen(Qt::blue));
                                         m_ui->label_text5->setText("blue");break;
                                    case 2: m_ui->logplot->graph(44)->setPen(QPen(Qt::cyan));
                                        m_ui->label_text6->setText("cyan");break;
                                    case 3: m_ui->logplot->graph(44)->setPen(QPen(Qt::red));
                                        m_ui->label_text7->setText("red");break;
                                    case 4: m_ui->logplot->graph(44)->setPen(QPen(Qt::green));
                                        m_ui->label_text8->setText("green");break;
                                    }
                  m_ui->logplot->graph(44)->valueAxis()->setRange(0,100);

                 m_ui->logplot->graph(44)->setName("fc4");
                  m_ui->logplot->legend->item(44)->setVisible(true);

                 m_ui->logplot->graph(44)->setVisible(true);
                 m_ui->logplot->graph(44)->rescaleValueAxis(true);
                    m_ui->logplot->replot();
                                                                                 str="";
                 for (int i = 0; i < y45.size(); ++i)
                {
                 if (i > 0)
                    str += " ";
                    str += QString::number(y45[i]);
                }
                switch (m_ui->sB->value()){
                    case 1 : m_ui->text_1->setText(str2);
                             m_ui->text_1->setText(str);
                             m_ui->label_text1->setText(m_ui->logplot->graph(44)->name());break;
                    case 2 : m_ui->text_2->setText(str2);
                             m_ui->text_2->setText(str);
                             m_ui->label_text2->setText(m_ui->logplot->graph(44)->name());break;
                    case 3 : m_ui->text_3->setText(str2);
                             m_ui->text_3->setText(str);
                             m_ui->label_text3->setText(m_ui->logplot->graph(44)->name());break;
                    case 4 : m_ui->text_4->setText(str2);
                             m_ui->text_4->setText(str);
                             m_ui->label_text4->setText(m_ui->logplot->graph(44)->name());break;
                             break;return;
                    }
                    }
                  else{switch (m_ui->sB->value())
                {

                 case 1 : m_ui->text_1->setText(str2);
                          m_ui->label_text1->setText("");break;
                 case 2 : m_ui->text_2->setText(str2);
                          m_ui->label_text2->setText("");break;
                 case 3 : m_ui->text_3->setText(str2);
                          m_ui->label_text3->setText("");break;
                 case 4 : m_ui->text_4->setText(str2);
                          m_ui->label_text4->setText("");break;
                          break;return;
                }
                m_ui->logplot->graph(44)->setVisible(false);
                    m_ui->logplot->legend->item(44)->setVisible(false);
          m_ui->sB->setValue(m_ui->sB->value()-1);
                 m_ui->logplot->replot();
             }
         }
    //46::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
    else if (((Reihe== 4)&&(Spalte==6))&&(m_ui->sB->value()<=4))
         {

            if( (m_ui->table_list->model()->index(Reihe,Spalte).data(Qt::CheckStateRole)==Qt::Checked)&&(m_ui->sB->value()<4))
                 {
            m_ui->sB->setValue(m_ui->sB->value()+1);
                         switch (m_ui->sB->value()){
                                    case 1:  m_ui->logplot->graph(45)->setPen(QPen(Qt::blue));
                                         m_ui->label_text5->setText("blue");break;
                                    case 2: m_ui->logplot->graph(45)->setPen(QPen(Qt::cyan));
                                        m_ui->label_text6->setText("cyan");break;
                                    case 3: m_ui->logplot->graph(45)->setPen(QPen(Qt::red));
                                        m_ui->label_text7->setText("red");break;
                                    case 4: m_ui->logplot->graph(45)->setPen(QPen(Qt::green));
                                        m_ui->label_text8->setText("green");break;
                                    }
                  m_ui->logplot->graph(45)->valueAxis()->setRange(0,100);

                 m_ui->logplot->graph(45)->setName("unknown 11");
                  m_ui->logplot->legend->item(45)->setVisible(true);
                m_ui->logplot->graph(45)->setVisible(true);
                 m_ui->logplot->graph(45)->rescaleValueAxis(true);
                    m_ui->logplot->replot();
                                                                                 str="";
                 for (int i = 0; i < y46.size(); ++i)
                {
                 if (i > 0)
                    str += " ";
                    str += QString::number(y46[i]);
                }
                switch (m_ui->sB->value()){
                    case 1 : m_ui->text_1->setText(str2);
                             m_ui->text_1->setText(str);
                             m_ui->label_text1->setText(m_ui->logplot->graph(45)->name());break;
                    case 2 : m_ui->text_2->setText(str2);
                             m_ui->text_2->setText(str);
                             m_ui->label_text2->setText(m_ui->logplot->graph(45)->name());break;
                    case 3 : m_ui->text_3->setText(str2);
                             m_ui->text_3->setText(str);
                             m_ui->label_text3->setText(m_ui->logplot->graph(45)->name());break;
                    case 4 : m_ui->text_4->setText(str2);
                             m_ui->text_4->setText(str);
                             m_ui->label_text4->setText(m_ui->logplot->graph(45)->name());break;
                             break;return;
                    }
                    }
                  else{switch (m_ui->sB->value())
                {

                 case 1 : m_ui->text_1->setText(str2);
                          m_ui->label_text1->setText("");break;
                 case 2 : m_ui->text_2->setText(str2);
                          m_ui->label_text2->setText("");break;
                 case 3 : m_ui->text_3->setText(str2);
                          m_ui->label_text3->setText("");break;
                 case 4 : m_ui->text_4->setText(str2);
                          m_ui->label_text4->setText("");break;
                          break;return;
                }
                m_ui->logplot->graph(45)->setVisible(false);
                    m_ui->logplot->legend->item(45)->setVisible(false);
          m_ui->sB->setValue(m_ui->sB->value()-1);
                 m_ui->logplot->replot();
                }
         }
       //47:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
    else if (((Reihe== 5)&&(Spalte==6))&&(m_ui->sB->value()<=4))
        {

            if( (m_ui->table_list->model()->index(Reihe,Spalte).data(Qt::CheckStateRole)==Qt::Checked)&&(m_ui->sB->value()<4))
                 {m_ui->sB->setValue(m_ui->sB->value()+1);

                         switch (m_ui->sB->value()){
                                    case 1:  m_ui->logplot->graph(46)->setPen(QPen(Qt::blue));
                                         m_ui->label_text5->setText("blue");break;
                                    case 2: m_ui->logplot->graph(46)->setPen(QPen(Qt::cyan));
                                        m_ui->label_text6->setText("cyan");break;
                                    case 3: m_ui->logplot->graph(46)->setPen(QPen(Qt::red));
                                        m_ui->label_text7->setText("red");break;
                                    case 4: m_ui->logplot->graph(46)->setPen(QPen(Qt::green));
                                        m_ui->label_text8->setText("green");break;
                                    }
                  m_ui->logplot->graph(46)->valueAxis()->setRange(0,100);

                 m_ui->logplot->graph(46)->setName("unknown 12");
                  m_ui->logplot->legend->item(46)->setVisible(true);
                 m_ui->logplot->graph(46)->setVisible(true);
                 m_ui->logplot->graph(46)->rescaleValueAxis(true);
                    m_ui->logplot->replot();

                                                                                 str="";
                 for (int i = 0; i < y47.size(); ++i)
                {
                 if (i > 0)
                    str += " ";
                    str += QString::number(y47[i]);
                }
                switch (m_ui->sB->value()){
                    case 1 : m_ui->text_1->setText(str2);
                             m_ui->text_1->setText(str);
                             m_ui->label_text1->setText(m_ui->logplot->graph(46)->name());break;
                    case 2 : m_ui->text_2->setText(str2);
                             m_ui->text_2->setText(str);
                             m_ui->label_text2->setText(m_ui->logplot->graph(46)->name());break;
                    case 3 : m_ui->text_3->setText(str2);
                             m_ui->text_3->setText(str);
                             m_ui->label_text3->setText(m_ui->logplot->graph(46)->name());break;
                    case 4 : m_ui->text_4->setText(str2);
                             m_ui->text_4->setText(str);
                             m_ui->label_text4->setText(m_ui->logplot->graph(46)->name());break;
                             break;return;
                    }}
                  else{switch (m_ui->sB->value())
                {

                 case 1 : m_ui->text_1->setText(str2);
                          m_ui->label_text1->setText("");break;
                 case 2 : m_ui->text_2->setText(str2);
                          m_ui->label_text2->setText("");break;
                 case 3 : m_ui->text_3->setText(str2);
                          m_ui->label_text3->setText("");break;
                 case 4 : m_ui->text_4->setText(str2);
                          m_ui->label_text4->setText("");break;
                          break;return;
                }
                m_ui->logplot->graph(46)->setVisible(false);
                    m_ui->logplot->legend->item(46)->setVisible(false);
          m_ui->sB->setValue(m_ui->sB->value()-1);
                 m_ui->logplot->replot();
              }
         }
        //48:::::::::::::::::::::::::::::::::::::::::::::::::::::
    else if (((Reihe== 6)&&(Spalte==6))&&(m_ui->sB->value()<=4))
         {

            if( (m_ui->table_list->model()->index(Reihe,Spalte).data(Qt::CheckStateRole)==Qt::Checked)&&(m_ui->sB->value()<4))
                 {m_ui->sB->setValue(m_ui->sB->value()+1);

                             switch (m_ui->sB->value()){
                                    case 1:  m_ui->logplot->graph(47)->setPen(QPen(Qt::blue));
                                         m_ui->label_text5->setText("blue");break;
                                    case 2: m_ui->logplot->graph(47)->setPen(QPen(Qt::cyan));
                                        m_ui->label_text6->setText("cyan");break;
                                    case 3: m_ui->logplot->graph(47)->setPen(QPen(Qt::red));
                                        m_ui->label_text7->setText("red");break;
                                    case 4: m_ui->logplot->graph(47)->setPen(QPen(Qt::green));
                                        m_ui->label_text8->setText("green");break;
                                    }
                  m_ui->logplot->graph(47)->valueAxis()->setRange(0,100);

                 m_ui->logplot->graph(47)->setName("unknown13");
                  m_ui->logplot->legend->item(47)->setVisible(true);
                 m_ui->logplot->graph(47)->setVisible(true);
                 m_ui->logplot->graph(47)->rescaleValueAxis(true);
                    m_ui->logplot->replot();
                                                                                 str="";
                 for (int i = 0; i < y48.size(); ++i)
                {
                 if (i > 0)
                    str += " ";
                    str += QString::number(y48[i]);
                }
                switch (m_ui->sB->value()){
                    case 1 : m_ui->text_1->setText(str2);
                             m_ui->text_1->setText(str);
                             m_ui->label_text1->setText(m_ui->logplot->graph(47)->name());break;
                    case 2 : m_ui->text_2->setText(str2);
                             m_ui->text_2->setText(str);
                             m_ui->label_text2->setText(m_ui->logplot->graph(47)->name());break;
                    case 3 : m_ui->text_3->setText(str2);
                             m_ui->text_3->setText(str);
                             m_ui->label_text3->setText(m_ui->logplot->graph(47)->name());break;
                    case 4 : m_ui->text_4->setText(str2);
                             m_ui->text_4->setText(str);
                             m_ui->label_text4->setText(m_ui->logplot->graph(47)->name());break;
                             break;return;
                    }
                    }
                  else{switch (m_ui->sB->value())
                {

                 case 1 : m_ui->text_1->setText(str2);
                          m_ui->label_text1->setText("");break;
                 case 2 : m_ui->text_2->setText(str2);
                          m_ui->label_text2->setText("");break;
                 case 3 : m_ui->text_3->setText(str2);
                          m_ui->label_text3->setText("");break;
                 case 4 : m_ui->text_4->setText(str2);
                          m_ui->label_text4->setText("");break;
                          break;return;
                }
                m_ui->logplot->graph(47)->setVisible(false);
                    m_ui->logplot->legend->item(47)->setVisible(false);
          m_ui->sB->setValue(m_ui->sB->value()-1);
                 m_ui->logplot->replot();
                }
         }
     //49::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
    else if (((Reihe== 0)&&(Spalte==7))&&(m_ui->sB->value()<=4))
         {

            if( (m_ui->table_list->model()->index(Reihe,Spalte).data(Qt::CheckStateRole)==Qt::Checked)&&(m_ui->sB->value()<4))
                     {m_ui->sB->setValue(m_ui->sB->value()+1);
                     switch (m_ui->sB->value()){
                                    case 1:  m_ui->logplot->graph(48)->setPen(QPen(Qt::blue));
                                         m_ui->label_text5->setText("blue");break;
                                    case 2: m_ui->logplot->graph(48)->setPen(QPen(Qt::cyan));
                                        m_ui->label_text6->setText("cyan");break;
                                    case 3: m_ui->logplot->graph(48)->setPen(QPen(Qt::red));
                                        m_ui->label_text7->setText("red");break;
                                    case 4: m_ui->logplot->graph(48)->setPen(QPen(Qt::green));
                                        m_ui->label_text8->setText("green");break;
                                    }
                  m_ui->logplot->graph(48)->valueAxis()->setRange(0,100);

                 m_ui->logplot->graph(48)->setName("unknown 14");
                  m_ui->logplot->legend->item(48)->setVisible(true);
                 m_ui->logplot->graph(48)->setVisible(true);
                 m_ui->logplot->graph(48)->rescaleValueAxis(true);
                    m_ui->logplot->replot();
                                                                                 str="";
                 for (int i = 0; i < y49.size(); ++i)
                {
                 if (i > 0)
                    str += " ";
                    str += QString::number(y49[i]);
                }
                switch (m_ui->sB->value()){
                    case 1 : m_ui->text_1->setText(str2);
                             m_ui->text_1->setText(str);
                             m_ui->label_text1->setText(m_ui->logplot->graph(48)->name());break;
                    case 2 : m_ui->text_2->setText(str2);
                             m_ui->text_2->setText(str);
                             m_ui->label_text2->setText(m_ui->logplot->graph(48)->name());break;
                    case 3 : m_ui->text_3->setText(str2);
                             m_ui->text_3->setText(str);
                             m_ui->label_text3->setText(m_ui->logplot->graph(48)->name());break;
                    case 4 : m_ui->text_4->setText(str2);
                             m_ui->text_4->setText(str);
                             m_ui->label_text4->setText(m_ui->logplot->graph(48)->name());break;
                             break;return;
                    }
                    }
                  else{switch (m_ui->sB->value())
                {

                 case 1 : m_ui->text_1->setText(str2);
                          m_ui->label_text1->setText("");break;
                 case 2 : m_ui->text_2->setText(str2);
                          m_ui->label_text2->setText("");break;
                 case 3 : m_ui->text_3->setText(str2);
                          m_ui->label_text3->setText("");break;
                 case 4 : m_ui->text_4->setText(str2);
                          m_ui->label_text4->setText("");break;
                          break;return;
                }
                m_ui->logplot->graph(48)->setVisible(false);
                    m_ui->logplot->legend->item(48)->setVisible(false);
          m_ui->sB->setValue(m_ui->sB->value()-1);
                 m_ui->logplot->replot();
                }
         }
     //50:::::::::::::::::::::::::::::::::::::::::::::::::::
    else if (((Reihe== 1)&&(Spalte==7))&&(m_ui->sB->value()<=4))
         {

            if( (m_ui->table_list->model()->index(Reihe,Spalte).data(Qt::CheckStateRole)==Qt::Checked)&&(m_ui->sB->value()<4))
                 {m_ui->sB->setValue(m_ui->sB->value()+1);
                             switch (m_ui->sB->value()){
                                    case 1:  m_ui->logplot->graph(49)->setPen(QPen(Qt::blue));
                                         m_ui->label_text5->setText("blue");break;
                                    case 2: m_ui->logplot->graph(49)->setPen(QPen(Qt::cyan));
                                        m_ui->label_text6->setText("cyan");break;
                                    case 3: m_ui->logplot->graph(49)->setPen(QPen(Qt::red));
                                        m_ui->label_text7->setText("red");break;
                                    case 4: m_ui->logplot->graph(49)->setPen(QPen(Qt::green));
                                        m_ui->label_text8->setText("green");break;
                                    }
                  m_ui->logplot->graph(49)->valueAxis()->setRange(0,100);

                 m_ui->logplot->graph(49)->setName("unknown 15");
                  m_ui->logplot->legend->item(49)->setVisible(true);

                 m_ui->logplot->graph(49)->setVisible(true);
                 m_ui->logplot->graph(49)->rescaleValueAxis(true);
                    m_ui->logplot->replot();
                                                                                 str="";
                 for (int i = 0; i < y50.size(); ++i)
                {
                 if (i > 0)
                    str += " ";
                    str += QString::number(y50[i]);
                }
                switch (m_ui->sB->value()){
                    case 1 : m_ui->text_1->setText(str2);
                             m_ui->text_1->setText(str);
                             m_ui->label_text1->setText(m_ui->logplot->graph(49)->name());break;
                    case 2 : m_ui->text_2->setText(str2);
                             m_ui->text_2->setText(str);
                             m_ui->label_text2->setText(m_ui->logplot->graph(49)->name());break;
                    case 3 : m_ui->text_3->setText(str2);
                             m_ui->text_3->setText(str);
                             m_ui->label_text3->setText(m_ui->logplot->graph(49)->name());break;
                    case 4 : m_ui->text_4->setText(str2);
                             m_ui->text_4->setText(str);
                             m_ui->label_text4->setText(m_ui->logplot->graph(49)->name());break;
                             break;return;
                    }
                    }
                  else{switch (m_ui->sB->value())
                {

                 case 1 : m_ui->text_1->setText(str2);
                          m_ui->label_text1->setText("");break;
                 case 2 : m_ui->text_2->setText(str2);
                          m_ui->label_text2->setText("");break;
                 case 3 : m_ui->text_3->setText(str2);
                          m_ui->label_text3->setText("");break;
                 case 4 : m_ui->text_4->setText(str2);
                          m_ui->label_text4->setText("");break;
                          break;return;
                }
                m_ui->logplot->graph(49)->setVisible(false);
                    m_ui->logplot->legend->item(49)->setVisible(false);
          m_ui->sB->setValue(m_ui->sB->value()-1);
                 m_ui->logplot->replot();
                  }
         }
      //51:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
    else if (((Reihe== 2)&&(Spalte==7))&&(m_ui->sB->value()<=4))
         {
            if( (m_ui->table_list->model()->index(Reihe,Spalte).data(Qt::CheckStateRole)==Qt::Checked)&&(m_ui->sB->value()<4))
                 {m_ui->sB->setValue(m_ui->sB->value()+1);

                         switch (m_ui->sB->value()){
                                    case 1:  m_ui->logplot->graph(50)->setPen(QPen(Qt::blue));
                                         m_ui->label_text5->setText("blue");break;
                                    case 2: m_ui->logplot->graph(50)->setPen(QPen(Qt::cyan));
                                        m_ui->label_text6->setText("cyan");break;
                                    case 3: m_ui->logplot->graph(50)->setPen(QPen(Qt::red));
                                        m_ui->label_text7->setText("red");break;
                                    case 4: m_ui->logplot->graph(50)->setPen(QPen(Qt::green));
                                        m_ui->label_text8->setText("green");break;
                                    }
                  m_ui->logplot->graph(50)->valueAxis()->setRange(0,100);

                 m_ui->logplot->graph(50)->setName("unknown 16");
                  m_ui->logplot->legend->item(50)->setVisible(true);

                 m_ui->logplot->graph(50)->setVisible(true);
                 m_ui->logplot->graph(50)->rescaleValueAxis(true);
                    m_ui->logplot->replot();

                                                                                 str="";
                 for (int i = 0; i < y51.size(); ++i)
                {
                 if (i > 0)
                    str += " ";
                    str += QString::number(y51[i]);
                }
                switch (m_ui->sB->value()){
                    case 1 : m_ui->text_1->setText(str2);
                             m_ui->text_1->setText(str);
                             m_ui->label_text1->setText(m_ui->logplot->graph(50)->name());break;
                    case 2 : m_ui->text_2->setText(str2);
                             m_ui->text_2->setText(str);
                             m_ui->label_text2->setText(m_ui->logplot->graph(50)->name());break;
                    case 3 : m_ui->text_3->setText(str2);
                             m_ui->text_3->setText(str);
                             m_ui->label_text3->setText(m_ui->logplot->graph(50)->name());break;
                    case 4 : m_ui->text_4->setText(str2);
                             m_ui->text_4->setText(str);
                             m_ui->label_text4->setText(m_ui->logplot->graph(50)->name());break;
                             break;return;
                    }}
                  else{switch (m_ui->sB->value())
                {

                 case 1 : m_ui->text_1->setText(str2);
                          m_ui->label_text1->setText("");break;
                 case 2 : m_ui->text_2->setText(str2);
                          m_ui->label_text2->setText("");break;
                 case 3 : m_ui->text_3->setText(str2);
                          m_ui->label_text3->setText("");break;
                 case 4 : m_ui->text_4->setText(str2);
                          m_ui->label_text4->setText("");break;
                          break;return;
                }
                m_ui->logplot->graph(50)->setVisible(false);
                    m_ui->logplot->legend->item(50)->setVisible(false);
          m_ui->sB->setValue(m_ui->sB->value()-1);
                 m_ui->logplot->replot();
                  }
         }
          //52::::::::::::::::::::::::::::::::::::::::::::::::::::::::
    else if (((Reihe== 3)&&(Spalte==7))&&(m_ui->sB->value()<=4))
        {
            if( (m_ui->table_list->model()->index(Reihe,Spalte).data(Qt::CheckStateRole)==Qt::Checked)&&(m_ui->sB->value()<4))
                 {m_ui->sB->setValue(m_ui->sB->value()+1);

                         switch (m_ui->sB->value()){
                                    case 1:  m_ui->logplot->graph(51)->setPen(QPen(Qt::blue));
                                         m_ui->label_text5->setText("blue");break;
                                    case 2: m_ui->logplot->graph(51)->setPen(QPen(Qt::cyan));
                                        m_ui->label_text6->setText("cyan");break;
                                    case 3: m_ui->logplot->graph(51)->setPen(QPen(Qt::red));
                                        m_ui->label_text7->setText("red");break;
                                    case 4: m_ui->logplot->graph(51)->setPen(QPen(Qt::green));
                                        m_ui->label_text8->setText("green");break;
                                    }
                  m_ui->logplot->graph(51)->valueAxis()->setRange(0,100);

                 m_ui->logplot->graph(51)->setName("unknown 17");
                  m_ui->logplot->legend->item(51)->setVisible(true);


              m_ui->logplot->graph(51)->setVisible(true);
              m_ui->logplot->graph(51)->rescaleValueAxis(true);
                 m_ui->logplot->replot();
                                                                             str="";
                 for (int i = 0; i < y52.size(); ++i)
                {
                 if (i > 0)
                    str += " ";
                    str += QString::number(y52[i]);
                }
                switch (m_ui->sB->value()){
                    case 1 : m_ui->text_1->setText(str2);
                             m_ui->text_1->setText(str);
                             m_ui->label_text1->setText(m_ui->logplot->graph(51)->name());break;
                    case 2 : m_ui->text_2->setText(str2);
                             m_ui->text_2->setText(str);
                             m_ui->label_text2->setText(m_ui->logplot->graph(51)->name());break;
                    case 3 : m_ui->text_3->setText(str2);
                             m_ui->text_3->setText(str);
                             m_ui->label_text3->setText(m_ui->logplot->graph(51)->name());break;
                    case 4 : m_ui->text_4->setText(str2);
                             m_ui->text_4->setText(str);
                             m_ui->label_text4->setText(m_ui->logplot->graph(51)->name());break;
                             break;return;
                    }
                 }
               else{switch (m_ui->sB->value())
                {

                 case 1 : m_ui->text_1->setText(str2);
                          m_ui->label_text1->setText("");break;
                 case 2 : m_ui->text_2->setText(str2);
                          m_ui->label_text2->setText("");break;
                 case 3 : m_ui->text_3->setText(str2);
                          m_ui->label_text3->setText("");break;
                 case 4 : m_ui->text_4->setText(str2);
                          m_ui->label_text4->setText("");break;
                          break;return;
                }
                m_ui->logplot->graph(51)->setVisible(false);
                 m_ui->logplot->legend->item(51)->setVisible(false);
                 m_ui->sB->setValue(m_ui->sB->value()-1);
              m_ui->logplot->replot();
             }
      }

    //53::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
    else if (((Reihe== 4)&&(Spalte==7))&&(m_ui->sB->value()<=4))
         {
            if( (m_ui->table_list->model()->index(Reihe,Spalte).data(Qt::CheckStateRole)==Qt::Checked)&&(m_ui->sB->value()<4))
            {m_ui->sB->setValue(m_ui->sB->value()+1);

                         switch (m_ui->sB->value()){
                                    case 1:  m_ui->logplot->graph(52)->setPen(QPen(Qt::blue));
                                         m_ui->label_text5->setText("blue");break;
                                    case 2: m_ui->logplot->graph(52)->setPen(QPen(Qt::cyan));
                                        m_ui->label_text6->setText("cyan");break;
                                    case 3: m_ui->logplot->graph(52)->setPen(QPen(Qt::red));
                                        m_ui->label_text7->setText("red");break;
                                    case 4: m_ui->logplot->graph(52)->setPen(QPen(Qt::green));
                                        m_ui->label_text8->setText("green");break;
                                    }
                  m_ui->logplot->graph(52)->valueAxis()->setRange(0,100);

              m_ui->logplot->graph(52)->setName("unknown 18");
               m_ui->logplot->legend->item(52)->setVisible(true);

              m_ui->logplot->graph(52)->setVisible(true);
              m_ui->logplot->graph(52)->rescaleValueAxis(true);
              m_ui->logplot->replot();
                                                                         str="";
                 for (int i = 0; i < y53.size(); ++i)
                {
                 if (i > 0)
                    str += " ";
                    str += QString::number(y53[i]);
                }
                switch (m_ui->sB->value()){
                    case 1 : m_ui->text_1->setText(str2);
                             m_ui->text_1->setText(str);
                             m_ui->label_text1->setText(m_ui->logplot->graph(52)->name());break;
                    case 2 : m_ui->text_2->setText(str2);
                             m_ui->text_2->setText(str);
                             m_ui->label_text2->setText(m_ui->logplot->graph(52)->name());break;
                    case 3 : m_ui->text_3->setText(str2);
                             m_ui->text_3->setText(str);
                             m_ui->label_text3->setText(m_ui->logplot->graph(52)->name());break;
                    case 4 : m_ui->text_4->setText(str2);
                             m_ui->text_4->setText(str);
                             m_ui->label_text4->setText(m_ui->logplot->graph(52)->name());break;
                             break;return;
                    }

            }
            else{switch (m_ui->sB->value())
                {

                 case 1 : m_ui->text_1->setText(str2);
                          m_ui->label_text1->setText("");break;
                 case 2 : m_ui->text_2->setText(str2);
                          m_ui->label_text2->setText("");break;
                 case 3 : m_ui->text_3->setText(str2);
                          m_ui->label_text3->setText("");break;
                 case 4 : m_ui->text_4->setText(str2);
                          m_ui->label_text4->setText("");break;
                          break;return;
                }
                m_ui->logplot->graph(52)->setVisible(false);
              m_ui->logplot->legend->item(52)->setVisible(false);
                 m_ui->sB->setValue(m_ui->sB->value()-1);
                 m_ui->logplot->replot();

             }
         }

     //54::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
    else if (((Reihe== 5)&&(Spalte==7)&&(m_ui->sB->value()<=4)))
         {
            if( (m_ui->table_list->model()->index(Reihe,Spalte).data(Qt::CheckStateRole)==Qt::Checked)&&(m_ui->sB->value()<4))
             {m_ui->sB->setValue(m_ui->sB->value()+1);
                             switch (m_ui->sB->value()){
                                    case 1:  m_ui->logplot->graph(53)->setPen(QPen(Qt::blue));
                                         m_ui->label_text5->setText("blue");break;
                                    case 2: m_ui->logplot->graph(53)->setPen(QPen(Qt::cyan));
                                        m_ui->label_text6->setText("cyan");break;
                                    case 3: m_ui->logplot->graph(53)->setPen(QPen(Qt::red));
                                        m_ui->label_text7->setText("red");break;
                                    case 4: m_ui->logplot->graph(53)->setPen(QPen(Qt::green));
                                        m_ui->label_text8->setText("green");break;
                                    }
                  m_ui->logplot->graph(53)->valueAxis()->setRange(0,100);

               m_ui->logplot->graph(53)->setName("jack count");
                m_ui->logplot->legend->item(53)->setVisible(true);

               m_ui->logplot->graph(53)->setVisible(true);
               m_ui->logplot->graph(53)->rescaleValueAxis(true);
               m_ui->logplot->replot();
                                                                         str="";
                 for (int i = 0; i < y54.size(); ++i)
                {
                 if (i > 0)
                    str += " ";
                    str += QString::number(y54[i]);
                }
                switch (m_ui->sB->value()){
                    case 1 : m_ui->text_1->setText(str2);
                             m_ui->text_1->setText(str);
                             m_ui->label_text1->setText(m_ui->logplot->graph(53)->name());break;
                    case 2 : m_ui->text_2->setText(str2);
                             m_ui->text_2->setText(str);
                             m_ui->label_text2->setText(m_ui->logplot->graph(53)->name());break;
                    case 3 : m_ui->text_3->setText(str2);
                             m_ui->text_3->setText(str);
                             m_ui->label_text3->setText(m_ui->logplot->graph(53)->name());break;
                    case 4 : m_ui->text_4->setText(str2);
                             m_ui->text_4->setText(str);
                             m_ui->label_text4->setText(m_ui->logplot->graph(53)->name());break;
                             break;return;
                    }
             }
             else{switch (m_ui->sB->value())
                {

                 case 1 : m_ui->text_1->setText(str2);
                          m_ui->label_text1->setText("");break;
                 case 2 : m_ui->text_2->setText(str2);
                          m_ui->label_text2->setText("");break;
                 case 3 : m_ui->text_3->setText(str2);
                          m_ui->label_text3->setText("");break;
                 case 4 : m_ui->text_4->setText(str2);
                          m_ui->label_text4->setText("");break;
                          break;return;
                }
                m_ui->logplot->graph(53)->setVisible(false);
               m_ui->logplot->legend->item(53)->setVisible(false);
                 m_ui->sB->setValue(m_ui->sB->value()-1);
                  m_ui->logplot->replot();
                 ;
              }
          }
    //55:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
    else if (((Reihe== 6)&&(Spalte==7))&&(m_ui->sB->value()<=4))
         {

            if( (m_ui->table_list->model()->index(Reihe,Spalte).data(Qt::CheckStateRole)==Qt::Checked)&&(m_ui->sB->value()<4))
                 {m_ui->sB->setValue(m_ui->sB->value()+1);

                         switch (m_ui->sB->value()){
                                    case 1:  m_ui->logplot->graph(55)->setPen(QPen(Qt::blue));
                                         m_ui->label_text5->setText("blue");break;
                                    case 2: m_ui->logplot->graph(55)->setPen(QPen(Qt::cyan));
                                        m_ui->label_text6->setText("cyan");break;
                                    case 3: m_ui->logplot->graph(55)->setPen(QPen(Qt::red));
                                        m_ui->label_text7->setText("red");break;
                                    case 4: m_ui->logplot->graph(55)->setPen(QPen(Qt::green));
                                        m_ui->label_text8->setText("green");break;
                                    }
                  m_ui->logplot->graph(55)->valueAxis()->setRange(0,100);

                 m_ui->logplot->graph(55)->setName("rpm read-raw D ");
                  m_ui->logplot->legend->item(55)->setVisible(true);

                m_ui->logplot->graph(55)->setVisible(true);

                  m_ui->logplot->graph(55)->rescaleValueAxis(true);
                    m_ui->logplot->replot();

                                                 str="";
                 for (int i = 0; i < y55.size(); ++i)
                {
                 if (i > 0)
                    str += " ";
                    str += QString::number(y55[i]);
                }
                switch (m_ui->sB->value()){
                    case 1 : m_ui->text_1->setText(str2);
                             m_ui->text_1->setText(str);
                             m_ui->label_text1->setText(m_ui->logplot->graph(55)->name());break;
                    case 2 : m_ui->text_2->setText(str2);
                             m_ui->text_2->setText(str);
                             m_ui->label_text2->setText(m_ui->logplot->graph(55)->name());break;
                    case 3 : m_ui->text_3->setText(str2);
                             m_ui->text_3->setText(str);
                             m_ui->label_text3->setText(m_ui->logplot->graph(55)->name());break;
                    case 4 : m_ui->text_4->setText(str2);
                             m_ui->text_4->setText(str);
                             m_ui->label_text4->setText(m_ui->logplot->graph(55)->name());break;
                             break;return;
                    }}
                  else{switch (m_ui->sB->value())
                {

                 case 1 : m_ui->text_1->setText(str2);
                          m_ui->label_text1->setText("");break;
                 case 2 : m_ui->text_2->setText(str2);
                          m_ui->label_text2->setText("");break;
                 case 3 : m_ui->text_3->setText(str2);
                          m_ui->label_text3->setText("");break;
                 case 4 : m_ui->text_4->setText(str2);
                          m_ui->label_text4->setText("");break;
                          break;return;
                }
                m_ui->logplot->graph(55)->setVisible(false);
                    m_ui->logplot->legend->item(55)->setVisible(false);
                     m_ui->sB->setValue(m_ui->sB->value()-1);
                     m_ui->logplot->replot();
                 }
         }

    }

    /*
      void MainWindow::printPreview()
    {
      QPrinter printer;
      QPrintPreviewDialog previewDialog(&printer, this);
      connect(&previewDialog, SIGNAL(paintRequested(QPrinter*)), SLOT(renderPlot(QPrinter*)));
      previewDialog.exec();
    }

    void MainWindow::renderPlot(QPrinter *printer)
    {
      printer->setPageSize(QPrinter::A4);
      QCPPainter painter(printer);
      QRectF pageRect = printer->pageRect(QPrinter::DevicePixel);

      int plotWidth =m_ui->logplot->viewport().width();
      int plotHeight = m_ui->logplot->viewport().height();
      double scale = pageRect.width()/(double)plotWidth;

      painter.setMode(QCPPainter::pmVectorized);
      painter.setMode(QCPPainter::pmNoCaching);
      painter.setMode(QCPPainter::pmNonCosmetic); // comment this out if you want cosmetic thin lines (always 1 pixel thick independent of pdf zoom level)

      painter.scale(scale, scale);
      m_ui->logplot->toPainter(&painter, plotWidth, plotHeight);
    }





    void MainWindow::on_push_print_clicked()
    {
        printPreview();
    }

    m_ui->table_list->setShowGrid(true);
    m_ui->table_list->setSelectionMode(QAbstractItemView::SingleSelection);
    QTableWidgetItem* tempItem = new QTableWidgetItem;
    tempItem->setFlags(tempItem->flags() | Qt::ItemIsUserCheckable);
    tempItem->setData(Qt::DisplayRole, QStringLiteral("1 rpm right axis"));
    tempItem->setData(Qt::CheckStateRole, Qt::Checked);
    */
    //::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::



    void MainWindow::changeEvent(QEvent* event)
    {
        if(event->type() == QEvent::LanguageChange)
        {
          m_ui->retranslateUi(this);
        }

        QWidget::changeEvent(event);
    }

    void MainWindow::on_btnEnglish_clicked()
    {
        qApp->removeTranslator(&translatorFr);
        qApp->removeTranslator(&translatorGe);
         qApp->removeTranslator(&translatorEsp);
    }

    void MainWindow::on_btnGerman_clicked()
    {
        qApp->installTranslator(&translatorGe);
    }

    void MainWindow::on_btnFrench_clicked()
    {
        qApp->installTranslator(&translatorFr);
    }
    void MainWindow::on_btnSpanish_clicked()
    {
        qApp->installTranslator(&translatorEsp);
    }
    /*
    charts
    */
    void MainWindow::setupchartsview(QTableWidget *table_list)
    {
        m_ui->chartsview->setShowGrid(true);
        m_ui->chartsview->setSelectionMode(QAbstractItemView::SingleSelection);
        QTableWidgetItem* tempItem = new QTableWidgetItem;
        tempItem->setFlags(tempItem->flags() | Qt::ItemIsUserCheckable);
        tempItem->setData(Qt::DisplayRole, QStringLiteral("1 rpm"));
        tempItem->setData(Qt::CheckStateRole, Qt::Unchecked);
        m_ui->chartsview->setItem(0,0,tempItem);

        tempItem = new QTableWidgetItem;
        tempItem->setFlags(tempItem->flags() | Qt::ItemIsUserCheckable);
        tempItem->setData(Qt::DisplayRole, QStringLiteral("2  watertemp "));
        tempItem->setData(Qt::CheckStateRole, Qt::Unchecked);
        m_ui->chartsview->setItem(1,0,tempItem);
        tempItem = new QTableWidgetItem;
        tempItem->setFlags(tempItem->flags() | Qt::ItemIsUserCheckable);
        tempItem->setData(Qt::DisplayRole, QStringLiteral("3  ambient"));
        tempItem->setData(Qt::CheckStateRole, Qt::Unchecked);
        m_ui->chartsview->setItem(2,0,tempItem);
        tempItem = new QTableWidgetItem;
        tempItem->setFlags(tempItem->flags() | Qt::ItemIsUserCheckable);
        tempItem->setData(Qt::DisplayRole, QStringLiteral("4  intakeAir "));
        tempItem->setData(Qt::CheckStateRole, Qt::Unchecked);
        m_ui->chartsview->setItem(3,0,tempItem);
        tempItem = new QTableWidgetItem;
        tempItem->setFlags(tempItem->flags() | Qt::ItemIsUserCheckable);
        tempItem->setData(Qt::DisplayRole, QStringLiteral("5  fueltemp"));
        tempItem->setData(Qt::CheckStateRole, Qt::Unchecked);
        m_ui->chartsview->setItem(4,0,tempItem);
        tempItem = new QTableWidgetItem;
        tempItem->setFlags(tempItem->flags() | Qt::ItemIsUserCheckable);
        tempItem->setData(Qt::DisplayRole, QStringLiteral("6  map"));
        tempItem->setData(Qt::CheckStateRole, Qt::Unchecked);
        m_ui->chartsview->setItem(5,0,tempItem);
        tempItem = new QTableWidgetItem;
        tempItem->setFlags(tempItem->flags() | Qt::ItemIsUserCheckable);
        tempItem->setData(Qt::DisplayRole, QStringLiteral("7  battery"));
        tempItem->setData(Qt::CheckStateRole, Qt::Unchecked);
        m_ui->chartsview->setItem(0,1,tempItem);

        tempItem = new QTableWidgetItem;
        tempItem->setFlags(tempItem->flags() | Qt::ItemIsUserCheckable);
        tempItem->setData(Qt::DisplayRole, QStringLiteral("8  throttlepot"));
        tempItem->setData(Qt::CheckStateRole, Qt::Unchecked);
        m_ui->chartsview->setItem(1,1,tempItem);
        tempItem = new QTableWidgetItem;
        tempItem->setFlags(tempItem->flags() | Qt::ItemIsUserCheckable);
        tempItem->setData(Qt::DisplayRole, QStringLiteral("9  idleswitch"));
        tempItem->setData(Qt::CheckStateRole, Qt::Unchecked);
        m_ui->chartsview->setItem(2,1,tempItem);
        tempItem = new QTableWidgetItem;
        tempItem->setFlags(tempItem->flags() | Qt::ItemIsUserCheckable);
        tempItem->setData(Qt::DisplayRole, QStringLiteral("10  uk1"));
        tempItem->setData(Qt::CheckStateRole, Qt::Unchecked);
        m_ui->chartsview->setItem(3,1,tempItem);
        tempItem = new QTableWidgetItem;
        tempItem->setFlags(tempItem->flags() | Qt::ItemIsUserCheckable);
        tempItem->setData(Qt::DisplayRole, QStringLiteral("11  park/nut"));
        tempItem->setData(Qt::CheckStateRole, Qt::Unchecked);
        m_ui->chartsview->setItem(4,1,tempItem);
        tempItem = new QTableWidgetItem;
        tempItem->setFlags(tempItem->flags() | Qt::ItemIsUserCheckable);
        tempItem->setData(Qt::DisplayRole, QStringLiteral("12  faultcodes"));
        tempItem->setData(Qt::CheckStateRole, Qt::Unchecked);
        m_ui->chartsview->setItem(5,1,tempItem);
        tempItem = new QTableWidgetItem;
        tempItem->setFlags(tempItem->flags() | Qt::ItemIsUserCheckable);
        tempItem->setData(Qt::DisplayRole, QStringLiteral("13  idlesetpoint"));
        tempItem->setData(Qt::CheckStateRole, Qt::Unchecked);
        m_ui->chartsview->setItem(6,1,tempItem);
        tempItem = new QTableWidgetItem;
        tempItem->setFlags(tempItem->flags() | Qt::ItemIsUserCheckable);
        tempItem->setData(Qt::DisplayRole, QStringLiteral("14  idlehot"));
        tempItem->setData(Qt::CheckStateRole, Qt::Unchecked);
        m_ui->chartsview->setItem(0,2,tempItem);
        tempItem = new QTableWidgetItem;
        tempItem->setFlags(tempItem->flags() | Qt::ItemIsUserCheckable);
        tempItem->setData(Qt::DisplayRole, QStringLiteral("15  uk2"));
        tempItem->setData(Qt::CheckStateRole, Qt::Unchecked);
        m_ui->chartsview->setItem(1,2,tempItem);

        tempItem = new QTableWidgetItem;
        tempItem->setFlags(tempItem->flags() | Qt::ItemIsUserCheckable);
        tempItem->setData(Qt::DisplayRole, QStringLiteral("16  iac"));
        tempItem->setData(Qt::CheckStateRole, Qt::Unchecked);
        m_ui->chartsview->setItem(2,2,tempItem);
        tempItem = new QTableWidgetItem;
        tempItem->setFlags(tempItem->flags() | Qt::ItemIsUserCheckable);
        tempItem->setData(Qt::DisplayRole, QStringLiteral("17  idle deviation"));
        tempItem->setData(Qt::CheckStateRole, Qt::Unchecked);
        m_ui->chartsview->setItem(3,2,tempItem);
        tempItem = new QTableWidgetItem;
        tempItem->setFlags(tempItem->flags() | Qt::ItemIsUserCheckable);
        tempItem->setData(Qt::DisplayRole, QStringLiteral("18  ignadvoffset"));
        tempItem->setData(Qt::CheckStateRole, Qt::Unchecked);
        m_ui->chartsview->setItem(4,2,tempItem);
        tempItem = new QTableWidgetItem;
        tempItem->setFlags(tempItem->flags() | Qt::ItemIsUserCheckable);
        tempItem->setData(Qt::DisplayRole, QStringLiteral("19  ignadv"));
        tempItem->setData(Qt::CheckStateRole, Qt::Unchecked);
        m_ui->chartsview->setItem(5,2,tempItem);
        tempItem = new QTableWidgetItem;
        tempItem->setFlags(tempItem->flags() | Qt::ItemIsUserCheckable);
        tempItem->setData(Qt::DisplayRole, QStringLiteral("20  coiltime"));
        tempItem->setData(Qt::CheckStateRole, Qt::Unchecked);
        m_ui->chartsview->setItem(6,2,tempItem);
        tempItem = new QTableWidgetItem;
        tempItem->setFlags(tempItem->flags() | Qt::ItemIsUserCheckable);
        tempItem->setData(Qt::DisplayRole, QStringLiteral("21  crank"));
        tempItem->setData(Qt::CheckStateRole, Qt::Unchecked);
        m_ui->chartsview->setItem(0,3,tempItem);
        tempItem = new QTableWidgetItem;
        tempItem->setFlags(tempItem->flags() | Qt::ItemIsUserCheckable);
        tempItem->setData(Qt::DisplayRole, QStringLiteral("22  uk4"));
        tempItem->setData(Qt::CheckStateRole, Qt::Unchecked);
        m_ui->chartsview->setItem(1,3,tempItem);
        tempItem = new QTableWidgetItem;
        tempItem->setFlags(tempItem->flags() | Qt::ItemIsUserCheckable);
        tempItem->setData(Qt::DisplayRole, QStringLiteral("23  uk5"));
        tempItem->setData(Qt::CheckStateRole, Qt::Unchecked);
        m_ui->chartsview->setItem(2,3,tempItem);

        tempItem = new QTableWidgetItem;
        tempItem->setFlags(tempItem->flags() | Qt::ItemIsUserCheckable);
        tempItem->setData(Qt::DisplayRole, QStringLiteral("24  ignswitch"));
        tempItem->setData(Qt::CheckStateRole, Qt::Unchecked);
        m_ui->chartsview->setItem(3,3,tempItem);
        tempItem = new QTableWidgetItem;
        tempItem->setFlags(tempItem->flags() | Qt::ItemIsUserCheckable);
        tempItem->setData(Qt::DisplayRole, QStringLiteral("25  throttleangle"));
        tempItem->setData(Qt::CheckStateRole, Qt::Unchecked);
        m_ui->chartsview->setItem(4,3,tempItem);
        tempItem = new QTableWidgetItem;
        tempItem->setFlags(tempItem->flags() | Qt::ItemIsUserCheckable);
        tempItem->setData(Qt::DisplayRole, QStringLiteral("26  uk6"));
        tempItem->setData(Qt::CheckStateRole, Qt::Unchecked);
        m_ui->chartsview->setItem(5,3,tempItem);
        tempItem = new QTableWidgetItem;
        tempItem->setFlags(tempItem->flags() | Qt::ItemIsUserCheckable);
        tempItem->setData(Qt::DisplayRole, QStringLiteral("27  airfuelratio"));
        tempItem->setData(Qt::CheckStateRole, Qt::Unchecked);
        m_ui->chartsview->setItem(6,3,tempItem);
        tempItem = new QTableWidgetItem;
        tempItem->setFlags(tempItem->flags() | Qt::ItemIsUserCheckable);
        tempItem->setData(Qt::DisplayRole, QStringLiteral("28  fc0"));
        tempItem->setData(Qt::CheckStateRole, Qt::Unchecked);
        m_ui->chartsview->setItem(0,4,tempItem);
        tempItem = new QTableWidgetItem;
        tempItem->setFlags(tempItem->flags() | Qt::ItemIsUserCheckable);
        tempItem->setData(Qt::DisplayRole, QStringLiteral("29  lambda voltage"));
        tempItem->setData(Qt::CheckStateRole, Qt::Unchecked);
        m_ui->chartsview->setItem(1,4,tempItem);
        tempItem = new QTableWidgetItem;
        tempItem->setFlags(tempItem->flags() | Qt::ItemIsUserCheckable);
        tempItem->setData(Qt::DisplayRole, QStringLiteral("30  lambda frequency"));
        tempItem->setData(Qt::CheckStateRole, Qt::Unchecked);
        m_ui->chartsview->setItem(2,4,tempItem);
        tempItem = new QTableWidgetItem;
        tempItem->setFlags(tempItem->flags() | Qt::ItemIsUserCheckable);
        tempItem->setData(Qt::DisplayRole, QStringLiteral("31  lambda dudycycle"));
        tempItem->setData(Qt::CheckStateRole, Qt::Unchecked);
        m_ui->chartsview->setItem(3,4,tempItem);

        tempItem = new QTableWidgetItem;
        tempItem->setFlags(tempItem->flags() | Qt::ItemIsUserCheckable);
        tempItem->setData(Qt::DisplayRole, QStringLiteral("32  lambdastatus"));
        tempItem->setData(Qt::CheckStateRole, Qt::Unchecked);
        m_ui->chartsview->setItem(4,4,tempItem);
        tempItem = new QTableWidgetItem;
        tempItem->setFlags(tempItem->flags() | Qt::ItemIsUserCheckable);
        tempItem->setData(Qt::DisplayRole, QStringLiteral("33  closedloop"));
        tempItem->setData(Qt::CheckStateRole, Qt::Unchecked);
        m_ui->chartsview->setItem(5,4,tempItem);
        tempItem = new QTableWidgetItem;
        tempItem->setFlags(tempItem->flags() | Qt::ItemIsUserCheckable);
        tempItem->setData(Qt::DisplayRole, QStringLiteral("34  ltftr"));
        tempItem->setData(Qt::CheckStateRole, Qt::Unchecked);
        m_ui->chartsview->setItem(6,4,tempItem);
        tempItem = new QTableWidgetItem;
        tempItem->setFlags(tempItem->flags() | Qt::ItemIsUserCheckable);
        tempItem->setData(Qt::DisplayRole, QStringLiteral("35  stftr"));
        tempItem->setData(Qt::CheckStateRole, Qt::Unchecked);
        m_ui->chartsview->setItem(0,5,tempItem);
        tempItem = new QTableWidgetItem;
        tempItem->setFlags(tempItem->flags() | Qt::ItemIsUserCheckable);
        tempItem->setData(Qt::DisplayRole, QStringLiteral("36  carboncanister"));
        tempItem->setData(Qt::CheckStateRole, Qt::Unchecked);
        m_ui->chartsview->setItem(1,5,tempItem);
        tempItem = new QTableWidgetItem;
        tempItem->setFlags(tempItem->flags() | Qt::ItemIsUserCheckable);
        tempItem->setData(Qt::DisplayRole, QStringLiteral("37  fc1"));
        tempItem->setData(Qt::CheckStateRole, Qt::Unchecked);
        m_ui->chartsview->setItem(2,5,tempItem);
        tempItem = new QTableWidgetItem;
        tempItem->setFlags(tempItem->flags() | Qt::ItemIsUserCheckable);
        tempItem->setData(Qt::DisplayRole, QStringLiteral("38  idlebase"));
        tempItem->setData(Qt::CheckStateRole, Qt::Unchecked);
        m_ui->chartsview->setItem(3,5,tempItem);
        tempItem = new QTableWidgetItem;
        tempItem->setFlags(tempItem->flags() | Qt::ItemIsUserCheckable);
        tempItem->setData(Qt::DisplayRole, QStringLiteral("39  uk7"));
        tempItem->setData(Qt::CheckStateRole, Qt::Unchecked);
        m_ui->chartsview->setItem(4,5,tempItem);

        tempItem = new QTableWidgetItem;
        tempItem->setFlags(tempItem->flags() | Qt::ItemIsUserCheckable);
        tempItem->setData(Qt::DisplayRole, QStringLiteral("40  uk8"));
        tempItem->setData(Qt::CheckStateRole, Qt::Unchecked);
        m_ui->chartsview->setItem(5,5,tempItem);
        tempItem = new QTableWidgetItem;
        tempItem->setFlags(tempItem->flags() | Qt::ItemIsUserCheckable);
        tempItem->setData(Qt::DisplayRole, QStringLiteral("41  ignadv2"));
        tempItem->setData(Qt::CheckStateRole, Qt::Unchecked);
        m_ui->chartsview->setItem(6,5,tempItem);
        tempItem = new QTableWidgetItem;
        tempItem->setFlags(tempItem->flags() | Qt::ItemIsUserCheckable);
        tempItem->setData(Qt::DisplayRole, QStringLiteral("42  uk9"));
        tempItem->setData(Qt::CheckStateRole, Qt::Unchecked);
        m_ui->chartsview->setItem(0,6,tempItem);
        tempItem = new QTableWidgetItem;
        tempItem->setFlags(tempItem->flags() | Qt::ItemIsUserCheckable);
        tempItem->setData(Qt::DisplayRole, QStringLiteral("43  idle error2"));
        tempItem->setData(Qt::CheckStateRole, Qt::Unchecked);
        m_ui->chartsview->setItem(1,6,tempItem);
        tempItem = new QTableWidgetItem;
        tempItem->setFlags(tempItem->flags() | Qt::ItemIsUserCheckable);
        tempItem->setData(Qt::DisplayRole, QStringLiteral("44  uk10"));
        tempItem->setData(Qt::CheckStateRole, Qt::Unchecked);
        m_ui->chartsview->setItem(2,6,tempItem);
        tempItem = new QTableWidgetItem;
        tempItem->setFlags(tempItem->flags() | Qt::ItemIsUserCheckable);
        tempItem->setData(Qt::DisplayRole, QStringLiteral("45  fc4"));
        tempItem->setData(Qt::CheckStateRole, Qt::Unchecked);
        m_ui->chartsview->setItem(3,6,tempItem);
        tempItem = new QTableWidgetItem;
        tempItem->setFlags(tempItem->flags() | Qt::ItemIsUserCheckable);
        tempItem->setData(Qt::DisplayRole, QStringLiteral("46  uk11"));
        tempItem->setData(Qt::CheckStateRole, Qt::Unchecked);
        m_ui->chartsview->setItem(4,6,tempItem);
        tempItem = new QTableWidgetItem;
        tempItem->setFlags(tempItem->flags() | Qt::ItemIsUserCheckable);
        tempItem->setData(Qt::DisplayRole, QStringLiteral("47  uk12"));
        tempItem->setData(Qt::CheckStateRole, Qt::Unchecked);
        m_ui->chartsview->setItem(5,6,tempItem);

        tempItem = new QTableWidgetItem;
        tempItem->setFlags(tempItem->flags() | Qt::ItemIsUserCheckable);
        tempItem->setData(Qt::DisplayRole, QStringLiteral("48  uk13"));
        tempItem->setData(Qt::CheckStateRole, Qt::Unchecked);
        m_ui->chartsview->setItem(6,6,tempItem);
        tempItem = new QTableWidgetItem;
        tempItem->setFlags(tempItem->flags() | Qt::ItemIsUserCheckable);
        tempItem->setData(Qt::DisplayRole, QStringLiteral("49  uk14"));
        tempItem->setData(Qt::CheckStateRole, Qt::Unchecked);
        m_ui->chartsview->setItem(0,7,tempItem);
        tempItem = new QTableWidgetItem;
        tempItem->setFlags(tempItem->flags() | Qt::ItemIsUserCheckable);
        tempItem->setData(Qt::DisplayRole, QStringLiteral("50  uk15"));
        tempItem->setData(Qt::CheckStateRole, Qt::Unchecked);
        m_ui->chartsview->setItem(1,7,tempItem);
        tempItem = new QTableWidgetItem;
        tempItem->setFlags(tempItem->flags() | Qt::ItemIsUserCheckable);
        tempItem->setData(Qt::DisplayRole, QStringLiteral("51  uk16"));
        tempItem->setData(Qt::CheckStateRole, Qt::Unchecked);
        m_ui->chartsview->setItem(2,7,tempItem);
        tempItem = new QTableWidgetItem;
        tempItem->setFlags(tempItem->flags() | Qt::ItemIsUserCheckable);
        tempItem->setData(Qt::DisplayRole, QStringLiteral("52  uk17"));
        tempItem->setData(Qt::CheckStateRole, Qt::Unchecked);
        m_ui->chartsview->setItem(3,7,tempItem);
        tempItem = new QTableWidgetItem;
        tempItem->setFlags(tempItem->flags() | Qt::ItemIsUserCheckable);
        tempItem->setData(Qt::DisplayRole, QStringLiteral("53  uk18"));
        tempItem->setData(Qt::CheckStateRole, Qt::Unchecked);
        m_ui->chartsview->setItem(4,7,tempItem);
        tempItem = new QTableWidgetItem;
        tempItem->setFlags(tempItem->flags() | Qt::ItemIsUserCheckable);
        tempItem->setData(Qt::DisplayRole, QStringLiteral("54  jack count"));
        tempItem->setData(Qt::CheckStateRole, Qt::Unchecked);
        m_ui->chartsview->setItem(5,7,tempItem);

    }


    void MainWindow::setupLambdaPlot(QCustomPlot *LambdaPlot)


         {
     m_ui->LambdaPlot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom );

           // include this section to fully disable antialiasing for higher performance:

    //       customPlot->setNotAntialiasedElements(QCP::aeAll);
           QFont font;
           font.setStyleStrategy(QFont::NoAntialias);
    //       customPlot->xAxis->setTickLabelFont(font);
     //      customPlot->yAxis->setTickLabelFont(font);

           m_ui->LambdaPlot->legend->setVisible(true);


            key=0;
           QVector<double> x,y;
           m_ui->LambdaPlot->addGraph(m_ui->LambdaPlot->xAxis,m_ui->LambdaPlot->yAxis);//0
           m_ui->LambdaPlot->graph(0)->setVisible(false);
           m_ui->LambdaPlot->addGraph(); //1
           m_ui->LambdaPlot->graph(1)->setVisible(false);

           m_ui->LambdaPlot->addGraph(); //1
           m_ui->LambdaPlot->graph(2)->setVisible(false);

           m_ui->LambdaPlot->addGraph(); //1
           m_ui->LambdaPlot->graph(3)->setVisible(false);

           m_ui->LambdaPlot->addGraph(); //1
           m_ui->LambdaPlot->graph(4)->setVisible(false);

           m_ui->LambdaPlot->addGraph(); //1
           m_ui->LambdaPlot->graph(5)->setVisible(false);

           m_ui->LambdaPlot->addGraph(); //1
           m_ui->LambdaPlot->graph(6)->setVisible(false);

           m_ui->LambdaPlot->addGraph(); //1
           m_ui->LambdaPlot->graph(7)->setVisible(false);

           m_ui->LambdaPlot->addGraph(); //1
           m_ui->LambdaPlot->graph(8)->setVisible(false);

           m_ui->LambdaPlot->addGraph(); //1
           m_ui->LambdaPlot->graph(9)->setVisible(false);

           m_ui->LambdaPlot->addGraph(); //1
           m_ui->LambdaPlot->graph(10)->setVisible(false);

           m_ui->LambdaPlot->addGraph(); //1
           m_ui->LambdaPlot->graph(11)->setVisible(false);

            m_ui->LambdaPlot->addGraph(); //1
           m_ui->LambdaPlot->graph(12)->setVisible(false);

           m_ui->LambdaPlot->addGraph(); //1
           m_ui->LambdaPlot->graph(13)->setVisible(false);

           m_ui->LambdaPlot->addGraph(); //1
           m_ui->LambdaPlot->graph(14)->setVisible(false);

           m_ui->LambdaPlot->addGraph(); //1
           m_ui->LambdaPlot->graph(15)->setVisible(false);

           m_ui->LambdaPlot->addGraph(); //1
           m_ui->LambdaPlot->graph(16)->setVisible(false);

           m_ui->LambdaPlot->addGraph(); //1
           m_ui->LambdaPlot->graph(17)->setVisible(false);

           m_ui->LambdaPlot->addGraph(); //1
           m_ui->LambdaPlot->graph(18)->setVisible(false);

           m_ui->LambdaPlot->addGraph(); //1
           m_ui->LambdaPlot->graph(19)->setVisible(false);

           m_ui->LambdaPlot->addGraph(); //1
           m_ui->LambdaPlot->graph(20)->setVisible(false);

           m_ui->LambdaPlot->addGraph(); //1
           m_ui->LambdaPlot->graph(21)->setVisible(false);

           m_ui->LambdaPlot->addGraph(); //1
           m_ui->LambdaPlot->graph(22)->setVisible(false);

           m_ui->LambdaPlot->addGraph(); //1
           m_ui->LambdaPlot->graph(23)->setVisible(false);

           m_ui->LambdaPlot->addGraph(); //1
           m_ui->LambdaPlot->graph(24)->setVisible(false);

           m_ui->LambdaPlot->addGraph(); //1
           m_ui->LambdaPlot->graph(25)->setVisible(false);

           m_ui->LambdaPlot->addGraph(); //1
           m_ui->LambdaPlot->graph(26)->setVisible(false);

           m_ui->LambdaPlot->addGraph(); //1
           m_ui->LambdaPlot->graph(27)->setVisible(false);

           m_ui->LambdaPlot->addGraph(); //1
           m_ui->LambdaPlot->graph(28)->setVisible(false);

           m_ui->LambdaPlot->addGraph(); //1
           m_ui->LambdaPlot->graph(29)->setVisible(false);

           m_ui->LambdaPlot->addGraph(); //1
           m_ui->LambdaPlot->graph(30)->setVisible(false);

           m_ui->LambdaPlot->addGraph(); //1
           m_ui->LambdaPlot->graph(31)->setVisible(false);

            m_ui->LambdaPlot->addGraph(); //1
           m_ui->LambdaPlot->graph(32)->setVisible(false);

           m_ui->LambdaPlot->addGraph(); //1
           m_ui->LambdaPlot->graph(33)->setVisible(false);

           m_ui->LambdaPlot->addGraph(); //1
           m_ui->LambdaPlot->graph(34)->setVisible(false);

           m_ui->LambdaPlot->addGraph(); //1
           m_ui->LambdaPlot->graph(35)->setVisible(false);

           m_ui->LambdaPlot->addGraph(); //1
           m_ui->LambdaPlot->graph(36)->setVisible(false);

           m_ui->LambdaPlot->addGraph(); //1
           m_ui->LambdaPlot->graph(37)->setVisible(false);

           m_ui->LambdaPlot->addGraph(); //1
           m_ui->LambdaPlot->graph(38)->setVisible(false);

           m_ui->LambdaPlot->addGraph(); //1
           m_ui->LambdaPlot->graph(39)->setVisible(false);

           m_ui->LambdaPlot->addGraph(); //1
           m_ui->LambdaPlot->graph(40)->setVisible(false);
           m_ui->LambdaPlot->addGraph(); //1
           m_ui->LambdaPlot->graph(41)->setVisible(false);

           m_ui->LambdaPlot->addGraph(); //1
           m_ui->LambdaPlot->graph(42)->setVisible(false);

           m_ui->LambdaPlot->addGraph(); //1
           m_ui->LambdaPlot->graph(43)->setVisible(false);

           m_ui->LambdaPlot->addGraph(); //1
           m_ui->LambdaPlot->graph(44)->setVisible(false);

           m_ui->LambdaPlot->addGraph(); //1
           m_ui->LambdaPlot->graph(45)->setVisible(false);

           m_ui->LambdaPlot->addGraph(); //1
           m_ui->LambdaPlot->graph(46)->setVisible(false);

           m_ui->LambdaPlot->addGraph(); //1
           m_ui->LambdaPlot->graph(47)->setVisible(false);

           m_ui->LambdaPlot->addGraph(); //1
           m_ui->LambdaPlot->graph(48)->setVisible(false);

           m_ui->LambdaPlot->addGraph(); //1
           m_ui->LambdaPlot->graph(49)->setVisible(false);

           m_ui->LambdaPlot->addGraph(); //1
           m_ui->LambdaPlot->graph(50)->setVisible(false);

           m_ui->LambdaPlot->addGraph(); //1
           m_ui->LambdaPlot->graph(51)->setVisible(false);

            m_ui->LambdaPlot->addGraph(); //1
           m_ui->LambdaPlot->graph(52)->setVisible(false);

           m_ui->LambdaPlot->addGraph(); //1
           m_ui->LambdaPlot->graph(53)->setVisible(false);

           m_ui->LambdaPlot->addGraph(); //1
           m_ui->LambdaPlot->graph(54)->setVisible(false);

           m_ui->LambdaPlot->addGraph(); //1
           m_ui->LambdaPlot->graph(55)->setVisible(false);





          QSharedPointer<QCPAxisTickerTime> timeTicker(new QCPAxisTickerTime);
           timeTicker->setTimeFormat("%h:%m:%s");
          m_ui->LambdaPlot->xAxis->setTicker(timeTicker);
          m_ui->LambdaPlot->axisRect()->setupFullAxesBox();
          m_ui->LambdaPlot->yAxis->setRange(0,1000);

           // make left and bottom axes transfer their ranges to right and top axes:
           connect(m_ui->LambdaPlot->xAxis, SIGNAL(rangeChanged(QCPRange)),m_ui->LambdaPlot->xAxis2, SLOT(setRange(QCPRange)));
          connect(m_ui->LambdaPlot->yAxis, SIGNAL(rangeChanged(QCPRange)),m_ui->LambdaPlot->yAxis2, SLOT(setRange(QCPRange)));

         // setup a timer that repeatedly calls MainWindow::chartsSlot:
          connect(&dataTimer, SIGNAL(timeout()), this, SLOT(chartsSlot()));
          dataTimer.start(0); // Interval 0 means to refresh as fast as possible
         }
    int samplerate = 500;
        int length=8;

        ///////////////////////////////////////////////////////////////////////
        /// \brief MainWindow::chartsSlot
        ////
        ///

    void MainWindow::chartsSlot()
    {
    QVector<double> x,y;
      static QTime time(QTime::currentTime());
      // calculate two new data points:
     double key = time.elapsed()/samplerate; // time elapsed since start of demo, in seconds
     // double key=QDateTime::currentDateTime("hh:mm:ss");
      static double lastPointKey = 0;
      if (key-lastPointKey > 0.002) // at most add point every 2 ms
      {

        // add data to lines:zeile 714
           m_ui->LambdaPlot->graph(0)->addData(key,rpm);//1
           m_ui->LambdaPlot->graph(1)->addData(key,coolant);//2
           m_ui->LambdaPlot->graph(2)->addData(key,ambient);//3
           m_ui->LambdaPlot->graph(3)->addData(key,intake);//4
           m_ui->LambdaPlot->graph(4)->addData(key,fuel);//5
           m_ui->LambdaPlot->graph(5)->addData(key,map);//6
           m_ui->LambdaPlot->graph(6)->addData(key,battery);//7
           m_ui->LambdaPlot->graph(7)->addData(key,throttlepos);//8
           m_ui->LambdaPlot->graph(8)->addData(key,idle);//9
           m_ui->LambdaPlot->graph(9)->addData(key,uk1);//10
           m_ui->LambdaPlot->graph(10)->addData(key,park);//11
           m_ui->LambdaPlot->graph(11)->addData(key,fc);//12
           m_ui->LambdaPlot->graph(12)->addData(key,idle_set_point);//13
           m_ui->LambdaPlot->graph(13)->addData(key,hot);//14
           m_ui->LambdaPlot->graph(14)->addData(key,uk2);//15
           m_ui->LambdaPlot->graph(15)->addData(key,iac);//16
           m_ui->LambdaPlot->graph(16)->addData(key,idle_dev);//17
           m_ui->LambdaPlot->graph(17)->addData(key,ign_adv_off);//18
           m_ui->LambdaPlot->graph(18)->addData(key,ign_adv);//19
           m_ui->LambdaPlot->graph(19)->addData(key,coil);//20
           m_ui->LambdaPlot->graph(20)->addData(key,crancs);//21
           m_ui->LambdaPlot->graph(21)->addData(key,uk4);//22
           m_ui->LambdaPlot->graph(22)->addData(key,uk5);//23
           m_ui->LambdaPlot->graph(23)->addData(key,igns_witch);//24
           m_ui->LambdaPlot->graph(24)->addData(key,throttleangle);//25
           m_ui->LambdaPlot->graph(25)->addData(key,uk6);//26
           m_ui->LambdaPlot->graph(26)->addData(key,air_fuel_ratio);//27
           m_ui->LambdaPlot->graph(27)->addData(key,fc0);//28
           m_ui->LambdaPlot->graph(28)->addData(key,lambda);//29
           m_ui->LambdaPlot->graph(29)->addData(key,lambda_frequency);//30
           m_ui->LambdaPlot->graph(30)->addData(key,lambda_dutycycle);//31
           m_ui->LambdaPlot->graph(31)->addData(key,lambda_status);//32
           m_ui->LambdaPlot->graph(32)->addData(key,closed_loop);//33
           m_ui->LambdaPlot->graph(33)->addData(key,ltft);//34
           m_ui->LambdaPlot->graph(34)->addData(key,stft);//35
           m_ui->LambdaPlot->graph(35)->addData(key,carbon);//36
           m_ui->LambdaPlot->graph(36)->addData(key,fc1);//37
           m_ui->LambdaPlot->graph(37)->addData(key,idle_base);//38
           m_ui->LambdaPlot->graph(38)->addData(key,uk7);//39
           m_ui->LambdaPlot->graph(39)->addData(key,uk8);//40
           m_ui->LambdaPlot->graph(40)->addData(key,ign_adv2);//41
           m_ui->LambdaPlot->graph(41)->addData(key,uk9);//42
           m_ui->LambdaPlot->graph(42)->addData(key,idle_error2);//43
           m_ui->LambdaPlot->graph(43)->addData(key,uk10);//44
           m_ui->LambdaPlot->graph(44)->addData(key,fc4);//45
           m_ui->LambdaPlot->graph(45)->addData(key,uk11);//46
           m_ui->LambdaPlot->graph(46)->addData(key,uk12);//47
           m_ui->LambdaPlot->graph(47)->addData(key,uk13);//48
           m_ui->LambdaPlot->graph(48)->addData(key,uk14);//49
           m_ui->LambdaPlot->graph(49)->addData(key,uk15);//50
           m_ui->LambdaPlot->graph(50)->addData(key,uk16);//51
           m_ui->LambdaPlot->graph(51)->addData(key,uk17);//52
           m_ui->LambdaPlot->graph(52)->addData(key,uk18);//53
           m_ui->LambdaPlot->graph(53)->addData(key,uk19);//54

        // rescale value (vertical) axis to fit the current data:
        m_ui->LambdaPlot->graph(0)->rescaleValueAxis();

        lastPointKey = key;
      }
      // make key axis range scroll with the data (at a constant range size of 8):
      m_ui->LambdaPlot->xAxis->setRange(key, length, Qt::AlignRight);
      m_ui->LambdaPlot->replot();

    /* calculate frames per second:
      static double lastFpsKey;
      static int frameCount;
      ++frameCount;
      if (key-lastFpsKey > 2) // average fps over 2 seconds
      {
       m_ui->statusBar->showMessage(
              QString("%1 FPS, Total Data points: %2")
              .arg(frameCount/(key-lastFpsKey), 0, 'f', 0)
              .arg(m_ui->LambdaPlot->graph(0)->data()->size()+m_ui->LambdaPlot->graph(1)->data()->size())
              , 0);
        lastFpsKey = key;
        frameCount = 0;
      }
    */
    }
    void MainWindow::on_lambdalengthslider_sliderMoved(int position)
    {
       length=position;
        m_ui->slider_two->setText(QString::number(m_ui->lambdalengthslider->value()));
    }


    void MainWindow::cellSelectedSlot2(int nRow, int nCol)
    {


        int Reihe=nRow ;
        int Spalte=nCol;
        qDebug()<<Reihe <<Spalte;


       //1:::::::::::::::::::::::::::::::::::::::::::::::::

        //2:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
    if ((Reihe== 0)&&(Spalte==0))
               {
       // double u = rpm;

                    if ((m_ui->chartsview->model()->index(Reihe,Spalte).data(Qt::CheckStateRole)==Qt::Checked)&&(m_ui->sB_2->value()<4))
                                {
                                    m_ui->sB_2->setValue(m_ui->sB_2->value()+1);
                                  //  m_ui->LambdaPlot->graph(0)->setName("rpm");
                                  //  m_ui->LambdaPlot->legend->item(0)->setVisible(true);
                                    m_ui->LambdaPlot->graph(0)->setVisible(true);


                                    m_ui->LambdaPlot->yAxis->setVisible(true);
                                 //  m_ui->LambdaPlot->yAxis->setRange(0,2000);
                                    m_ui->LambdaPlot->yAxis->atRight;
                                    m_ui->LambdaPlot->yAxis->setTickLabels(true);
                                    m_ui->LambdaPlot->graph(0)->rescaleValueAxis(true);
                                    m_ui->LambdaPlot->replot();
                                 qDebug()<<m_ui->sB_2->value();
                                    if (m_ui->sB_2->value()==1){
                                                   m_ui->LambdaPlot->graph(0)->setPen(QPen(Qt::blue));
                                                   m_ui->l_charts_1->setText("rpm");
                                                   m_ui->b_charts_1->setText(QString::number(rpm));}
                                    else if (m_ui->sB_2->value()==2){
                                                   m_ui->LambdaPlot->graph(0)->setPen(QPen(Qt::yellow));
                                                   m_ui->l_charts_2->setText("rpm");
                                                   m_ui->b_charts_2->setText(QString::number(rpm));}
                                   else if (m_ui->sB_2->value()==3){
                                                   m_ui->LambdaPlot->graph(0)->setPen(QPen(Qt::red));
                                                   m_ui->l_charts_3->setText("rpm");
                                                   m_ui->b_charts_3->setText(QString::number(rpm));}
                                   else if (m_ui->sB_2->value()==4){
                                                    m_ui->LambdaPlot->graph(0)->setPen(QPen(Qt::green));
                                                    m_ui->l_charts_4->setText("rpm");
                                                    m_ui->b_charts_4->setText(QString::number(rpm));}
                                    else qDebug()<<"spinbox zuhoch";





                                }
                                else
                                  {

                                    m_ui->LambdaPlot->graph(0)->setVisible(false);
                                   // m_ui->LambdaPlot->legend->item(0)->setVisible(false);
                                    m_ui->sB_2->setValue(m_ui->sB_2->value()-1);
                                    m_ui->LambdaPlot->replot();
                                  }
                }


    //2::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
     else if ((Reihe== 1)&&(Spalte==0))
        {
        // double u = coolant;

            if ((m_ui->chartsview->model()->index(Reihe,Spalte).data(Qt::CheckStateRole)==Qt::Checked)&&(m_ui->sB_2->value()<4))
                {

                    m_ui->sB_2->setValue(m_ui->sB_2->value()+1);
                   // m_ui->LambdaPlot->graph(1)->setName("Watertemp");
                   // m_ui->LambdaPlot->legend->item(1)->setVisible(true);
                    m_ui->LambdaPlot->graph(1)->setVisible(true);

                    m_ui->LambdaPlot->yAxis->setVisible(true);
                    m_ui->LambdaPlot->yAxis->setRange(0,100);
                    m_ui->LambdaPlot->yAxis->atLeft;
                    //   m_ui->LambdaPlot->yAxis2->setTickLabelFont(font);
                    m_ui->LambdaPlot->yAxis->setTickLabels(true);
                    m_ui->LambdaPlot->graph(1)->rescaleValueAxis(true);
                    m_ui->LambdaPlot->replot();
                     qDebug()<<m_ui->sB_2->value();
                     if (m_ui->sB_2->value()==1){
                                    m_ui->LambdaPlot->graph(0)->setPen(QPen(Qt::blue));
                                    m_ui->l_charts_1->setText("coolant");
                                    m_ui->b_charts_1->setText(QString::number(coolant));}
                     else if (m_ui->sB_2->value()==2){
                                    m_ui->LambdaPlot->graph(0)->setPen(QPen(Qt::yellow));
                                    m_ui->l_charts_2->setText("coolant");
                                    m_ui->b_charts_2->setText(QString::number(coolant));}
                    else if (m_ui->sB_2->value()==3){
                                    m_ui->LambdaPlot->graph(0)->setPen(QPen(Qt::red));
                                    m_ui->l_charts_3->setText("coolant");
                                    m_ui->b_charts_3->setText(QString::number(coolant));}
                    else if (m_ui->sB_2->value()==4){
                                     m_ui->LambdaPlot->graph(0)->setPen(QPen(Qt::green));
                                     m_ui->l_charts_4->setText("coolant");
                                     m_ui->b_charts_4->setText(QString::number(coolant));}
                     else qDebug()<<"spinbox zuhoch";






                }
                else
                  {

                    m_ui->LambdaPlot->graph(1)->setVisible(false);
                  //  m_ui->LambdaPlot->legend->item(1)->setVisible(false);
                     m_ui->sB_2->setValue(m_ui->sB_2->value()-1);
                    qDebug()<<m_ui->sB_2->value();
                    m_ui->LambdaPlot->replot();
                  }
        }

    //3::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
     else if ((Reihe== 2)&&(Spalte==0))
        {
        // double u = ambient;

        if( (m_ui->chartsview->model()->index(Reihe,Spalte).data(Qt::CheckStateRole)==Qt::Checked)&&(m_ui->sB_2->value()<4))
          {

            m_ui->sB_2->setValue(m_ui->sB_2->value()+1);
            m_ui->LambdaPlot->graph(2)->setVisible(true);
          //  m_ui->LambdaPlot->graph(2)->setName("ambientAir");
          //  m_ui->LambdaPlot->legend->item(2)->setVisible(true);
            m_ui->LambdaPlot->graph(2)->valueAxis()->setRange(0,100);
            m_ui->LambdaPlot->graph(2)->rescaleValueAxis(true);
            m_ui->LambdaPlot->replot();
             qDebug()<<m_ui->sB_2->value();
             if (m_ui->sB_2->value()==1){
                            m_ui->LambdaPlot->graph(0)->setPen(QPen(Qt::blue));
                            m_ui->l_charts_1->setText("ambient");
                            m_ui->b_charts_1->setText(QString::number(ambient));}
             else if (m_ui->sB_2->value()==2){
                            m_ui->LambdaPlot->graph(0)->setPen(QPen(Qt::yellow));
                            m_ui->l_charts_2->setText("ambient");
                            m_ui->b_charts_2->setText(QString::number(ambient));}
            else if (m_ui->sB_2->value()==3){
                            m_ui->LambdaPlot->graph(0)->setPen(QPen(Qt::red));
                            m_ui->l_charts_3->setText("ambient");
                            m_ui->b_charts_3->setText(QString::number(ambient));}
            else if (m_ui->sB_2->value()==4){
                             m_ui->LambdaPlot->graph(0)->setPen(QPen(Qt::green));
                             m_ui->l_charts_4->setText("ambient");
                             m_ui->b_charts_4->setText(QString::number(ambient));}
             else qDebug()<<"spinbox zuhoch";


         }
            else
            {

                         m_ui->LambdaPlot->graph(2)->setVisible(false);
             //           m_ui->LambdaPlot->legend->item(2)->setVisible(false);
                         m_ui->sB_2->setValue(m_ui->sB_2->value()-1);
                        qDebug()<<m_ui->sB_2->value();
                        m_ui->LambdaPlot->replot();

            }
        }
    //4::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
        else if ((Reihe==3)&&(Spalte==0))
           {
        //        double u = intake;

                    if((m_ui->chartsview->model()->index(Reihe,Spalte).data(Qt::CheckStateRole)==Qt::Checked)&&(m_ui->sB_2->value()<4))
                    {

                        m_ui->sB_2->setValue(m_ui->sB_2->value()+1);
                        m_ui->LambdaPlot->graph(3)->setVisible(true);
             //           m_ui->LambdaPlot->graph(3)->setName("IntakeAir");
             //           m_ui->LambdaPlot->legend->item(3)->setVisible(true);
                        m_ui->LambdaPlot->graph(3)->valueAxis()->setRange(0,100);
                        m_ui->LambdaPlot->graph(3)->rescaleValueAxis(true);
                        m_ui->LambdaPlot->replot();

                        if (m_ui->sB_2->value()==1){
                                       m_ui->LambdaPlot->graph(0)->setPen(QPen(Qt::blue));
                                       m_ui->l_charts_1->setText("intake");
                                       m_ui->b_charts_1->setText(QString::number(intake));}
                        else if (m_ui->sB_2->value()==2){
                                       m_ui->LambdaPlot->graph(0)->setPen(QPen(Qt::yellow));
                                       m_ui->l_charts_2->setText("intake");
                                       m_ui->b_charts_2->setText(QString::number(intake));}
                       else if (m_ui->sB_2->value()==3){
                                       m_ui->LambdaPlot->graph(0)->setPen(QPen(Qt::red));
                                       m_ui->l_charts_3->setText("intake");
                                       m_ui->b_charts_3->setText(QString::number(intake));}
                       else if (m_ui->sB_2->value()==4){
                                        m_ui->LambdaPlot->graph(0)->setPen(QPen(Qt::green));
                                        m_ui->l_charts_4->setText("intake");
                                        m_ui->b_charts_4->setText(QString::number(intake));}
                        else qDebug()<<"spinbox zuhoch";

                    }
                    else
                        {

                            m_ui->LambdaPlot->graph(3)->setVisible(false);
                 //           m_ui->LambdaPlot->legend->item(3)->setVisible(false);
                            m_ui->sB_2->setValue(m_ui->sB_2->value()-1);
                            m_ui->LambdaPlot->replot();

                        }

            }
    //5:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
        else if ((Reihe== 4)&&(Spalte==0))
           {
      //      double u = fuel;

            if((m_ui->chartsview->model()->index(Reihe,Spalte).data(Qt::CheckStateRole)==Qt::Checked)&&(m_ui->sB_2->value()<4))
                {

                   m_ui->sB_2->setValue(m_ui->sB_2->value()+1);
                        m_ui->LambdaPlot->graph(4)->setVisible(true);
                //    m_ui->LambdaPlot->graph(4)->setName("fuelTemp");
                //    m_ui->LambdaPlot->yAxis->setRange(0,100);
                //    m_ui->LambdaPlot->legend->item(4)->setVisible(true);

                    m_ui->LambdaPlot->graph(4)->rescaleValueAxis(true);
                    m_ui->LambdaPlot->replot();
                    if (m_ui->sB_2->value()==1){
                                   m_ui->LambdaPlot->graph(0)->setPen(QPen(Qt::blue));
                                   m_ui->l_charts_1->setText("fuel");
                                   m_ui->b_charts_1->setText(QString::number(fuel));}
                    else if (m_ui->sB_2->value()==2){
                                   m_ui->LambdaPlot->graph(0)->setPen(QPen(Qt::yellow));
                                   m_ui->l_charts_2->setText("fuel");
                                   m_ui->b_charts_2->setText(QString::number(fuel));}
                   else if (m_ui->sB_2->value()==3){
                                   m_ui->LambdaPlot->graph(0)->setPen(QPen(Qt::red));
                                   m_ui->l_charts_3->setText("fuel");
                                   m_ui->b_charts_3->setText(QString::number(fuel));}
                   else if (m_ui->sB_2->value()==4){
                                    m_ui->LambdaPlot->graph(0)->setPen(QPen(Qt::green));
                                    m_ui->l_charts_4->setText("fuel");
                                    m_ui->b_charts_4->setText(QString::number(fuel));}
                    else qDebug()<<"spinbox zuhoch";


                }
             else
                {

                m_ui->LambdaPlot->graph(4)->setVisible(false);
          //      m_ui->LambdaPlot->legend->item(4)->setVisible(false);
                m_ui->sB_2->setValue(m_ui->sB_2->value()-1);
                qDebug()<<m_ui->sB_2->value();
                m_ui->LambdaPlot->replot();

                }
            }

    //6::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
        else if ((Reihe==5)&&(Spalte==0))
        {
     //    double u = map;

            if( (m_ui->chartsview->model()->index(Reihe,Spalte).data(Qt::CheckStateRole)==Qt::Checked)&&(m_ui->sB_2->value()<4))

                { m_ui->sB_2->setValue(m_ui->sB_2->value()+1);

               //     m_ui->LambdaPlot->graph(5)->setName("Map");
                //    m_ui->LambdaPlot->legend->item(5)->setVisible(true);
                    m_ui->LambdaPlot->graph(5)->setVisible(true);
                    m_ui->LambdaPlot->graph(5)->valueAxis()->setRange(0,100);
                            m_ui->LambdaPlot->graph(5)->rescaleValueAxis(true);
                            m_ui->LambdaPlot->replot();
                            if (m_ui->sB_2->value()==1){
                                           m_ui->LambdaPlot->graph(0)->setPen(QPen(Qt::blue));
                                           m_ui->l_charts_1->setText("map");
                                           m_ui->b_charts_1->setText(QString::number(map));}
                            else if (m_ui->sB_2->value()==2){
                                           m_ui->LambdaPlot->graph(0)->setPen(QPen(Qt::yellow));
                                           m_ui->l_charts_2->setText("map");
                                           m_ui->b_charts_2->setText(QString::number(map));}
                           else if (m_ui->sB_2->value()==3){
                                           m_ui->LambdaPlot->graph(0)->setPen(QPen(Qt::red));
                                           m_ui->l_charts_3->setText("map");
                                           m_ui->b_charts_3->setText(QString::number(map));}
                           else if (m_ui->sB_2->value()==4){
                                            m_ui->LambdaPlot->graph(0)->setPen(QPen(Qt::green));
                                            m_ui->l_charts_4->setText("map");
                                            m_ui->b_charts_4->setText(QString::number(map));}
                            else qDebug()<<"spinbox zuhoch";

                }
            else
            {

                m_ui->LambdaPlot->graph(5)->setVisible(false);
           //     m_ui->LambdaPlot->legend->item(5)->setVisible(false);
                m_ui->sB_2->setValue(m_ui->sB_2->value()-1);
                qDebug()<<m_ui->sB_2->value();
                m_ui->LambdaPlot->replot();
           }
        }

        //7::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
    else if ((Reihe== 0)&&(Spalte==1))
        {
      //   double u = battery;

            if( (m_ui->chartsview->model()->index(Reihe,Spalte).data(Qt::CheckStateRole)==Qt::Checked)&&(m_ui->sB_2->value()<4))
         {m_ui->sB_2->setValue(m_ui->sB_2->value()+1);


            //    m_ui->LambdaPlot->graph(6)->setName("Battery voltage");
                 m_ui->LambdaPlot->graph(5)->valueAxis()->setRange(0,20);
           //     m_ui->LambdaPlot->legend->item(6)->setVisible(true);
                m_ui->LambdaPlot->graph(6)->setVisible(true);


                m_ui->LambdaPlot->graph(6)->rescaleValueAxis(true);
                m_ui->LambdaPlot->replot();
                if (m_ui->sB_2->value()==1){
                               m_ui->LambdaPlot->graph(0)->setPen(QPen(Qt::blue));
                               m_ui->l_charts_1->setText("battery");
                               m_ui->b_charts_1->setText(QString::number(battery));}
                else if (m_ui->sB_2->value()==2){
                               m_ui->LambdaPlot->graph(0)->setPen(QPen(Qt::yellow));
                               m_ui->l_charts_2->setText("battery");
                               m_ui->b_charts_2->setText(QString::number(battery));}
               else if (m_ui->sB_2->value()==3){
                               m_ui->LambdaPlot->graph(0)->setPen(QPen(Qt::red));
                               m_ui->l_charts_3->setText("battery");
                               m_ui->b_charts_3->setText(QString::number(battery));}
               else if (m_ui->sB_2->value()==4){
                                m_ui->LambdaPlot->graph(0)->setPen(QPen(Qt::green));
                                m_ui->l_charts_4->setText("battery");
                                m_ui->b_charts_4->setText(QString::number(battery));}
                else qDebug()<<"spinbox zuhoch";

            }
           else{

                m_ui->LambdaPlot->graph(6)->setVisible(false);
            //    m_ui->LambdaPlot->legend->item(6)->setVisible(false);
                m_ui->sB_2->setValue(m_ui->sB_2->value()-1);
                m_ui->LambdaPlot->replot();

                }
     }
     //8:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
    else if ((Reihe== 1)&&(Spalte==1))
        {
      //   double u = throttle_pos;

            if( (m_ui->chartsview->model()->index(Reihe,Spalte).data(Qt::CheckStateRole)==Qt::Checked)&&(m_ui->sB_2->value()<4))
         { m_ui->sB_2->setValue(m_ui->sB_2->value()+1);


        //     m_ui->LambdaPlot->graph(7)->setName("Throttle Pot");
                m_ui->LambdaPlot->graph(5)->valueAxis()->setRange(0,10);
                m_ui->LambdaPlot->graph(7)->setVisible(true);
         //    m_ui->LambdaPlot->legend->item(7)->setVisible(true);


             m_ui->LambdaPlot->graph(7)->rescaleValueAxis(true);
             m_ui->LambdaPlot->replot();
             if (m_ui->sB_2->value()==1){
                            m_ui->LambdaPlot->graph(0)->setPen(QPen(Qt::blue));
                            m_ui->l_charts_1->setText("throttle_pos");
                            m_ui->b_charts_1->setText(QString::number(throttlepos));}
             else if (m_ui->sB_2->value()==2){
                            m_ui->LambdaPlot->graph(0)->setPen(QPen(Qt::yellow));
                            m_ui->l_charts_2->setText("throttle_pos");
                            m_ui->b_charts_2->setText(QString::number(throttlepos));}
            else if (m_ui->sB_2->value()==3){
                            m_ui->LambdaPlot->graph(0)->setPen(QPen(Qt::red));
                            m_ui->l_charts_3->setText("throttle_pos");
                            m_ui->b_charts_3->setText(QString::number(throttlepos));}
            else if (m_ui->sB_2->value()==4){
                             m_ui->LambdaPlot->graph(0)->setPen(QPen(Qt::green));
                             m_ui->l_charts_4->setText("throttle_pos");
                             m_ui->b_charts_4->setText(QString::number(throttlepos));}
             else qDebug()<<"spinbox zuhoch";

             }
          else{

                m_ui->LambdaPlot->graph(7)->setVisible(false);
       //   m_ui->LambdaPlot->legend->item(7)->setVisible(false);
            m_ui->sB_2->setValue(m_ui->sB_2->value()-1);

               m_ui->LambdaPlot->replot();

               }
     }

    //9::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
    else if ((Reihe== 2)&&(Spalte==1))
        {
     //    double u = idle;

            if( (m_ui->chartsview->model()->index(Reihe,Spalte).data(Qt::CheckStateRole)==Qt::Checked)&&(m_ui->sB_2->value()<4))
             { 		m_ui->sB_2->setValue(m_ui->sB_2->value()+1);

             //        m_ui->LambdaPlot->graph(8)->setName("Idle Switch");
             //       m_ui->LambdaPlot->legend->item(8)->setVisible(true);
                     m_ui->LambdaPlot->graph(8)->setVisible(true);


                     m_ui->LambdaPlot->graph(8)->rescaleValueAxis(true);
                     m_ui->LambdaPlot->replot();
                     if (m_ui->sB_2->value()==1){
                                    m_ui->LambdaPlot->graph(0)->setPen(QPen(Qt::blue));
                                    m_ui->l_charts_1->setText("idle");
                                    m_ui->b_charts_1->setText(QString::number(idle));}
                     else if (m_ui->sB_2->value()==2){
                                    m_ui->LambdaPlot->graph(0)->setPen(QPen(Qt::yellow));
                                    m_ui->l_charts_2->setText("idle");
                                    m_ui->b_charts_2->setText(QString::number(idle));}
                    else if (m_ui->sB_2->value()==3){
                                    m_ui->LambdaPlot->graph(0)->setPen(QPen(Qt::red));
                                    m_ui->l_charts_3->setText("idle");
                                    m_ui->b_charts_3->setText(QString::number(idle));}
                    else if (m_ui->sB_2->value()==4){
                                     m_ui->LambdaPlot->graph(0)->setPen(QPen(Qt::green));
                                     m_ui->l_charts_4->setText("idle");
                                     m_ui->b_charts_4->setText(QString::number(idle));}
                     else qDebug()<<"spinbox zuhoch";

            }
          else{

                m_ui->LambdaPlot->graph(8)->setVisible(false);
        //       m_ui->LambdaPlot->legend->item(8)->setVisible(false);
               m_ui->sB_2->setValue(m_ui->sB_2->value()-1);
               m_ui->LambdaPlot->replot();

               }
    }
    //10::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
    else if ((Reihe== 3)&&(Spalte==1))
        {
     //    double u =uk1;

            if( (m_ui->chartsview->model()->index(Reihe,Spalte).data(Qt::CheckStateRole)==Qt::Checked)&&(m_ui->sB_2->value()<4))
         {
              m_ui->sB_2->setValue(m_ui->sB_2->value()+1);


              //  m_ui->LambdaPlot->graph(9)->setName("unknown1");
              //  m_ui->LambdaPlot->legend->item(9)->setVisible(true);
                m_ui->LambdaPlot->graph(9)->setVisible(true);


                m_ui->LambdaPlot->graph(9)->rescaleValueAxis(true);
                 m_ui->LambdaPlot->replot();
                 if (m_ui->sB_2->value()==1){
                                m_ui->LambdaPlot->graph(0)->setPen(QPen(Qt::blue));
                                m_ui->l_charts_1->setText("uk1");
                                m_ui->b_charts_1->setText(QString::number(uk1));}
                 else if (m_ui->sB_2->value()==2){
                                m_ui->LambdaPlot->graph(0)->setPen(QPen(Qt::yellow));
                                m_ui->l_charts_2->setText("uk1");
                                m_ui->b_charts_2->setText(QString::number(uk1));}
                else if (m_ui->sB_2->value()==3){
                                m_ui->LambdaPlot->graph(0)->setPen(QPen(Qt::red));
                                m_ui->l_charts_3->setText("uk1");
                                m_ui->b_charts_3->setText(QString::number(uk1));}
                else if (m_ui->sB_2->value()==4){
                                 m_ui->LambdaPlot->graph(0)->setPen(QPen(Qt::green));
                                 m_ui->l_charts_4->setText("uk1");
                                 m_ui->b_charts_4->setText(QString::number(uk1));}
                 else qDebug()<<"spinbox zuhoch";

            }

          else{

                m_ui->LambdaPlot->graph(9)->setVisible(false);
           //     m_ui->LambdaPlot->legend->item(9)->setVisible(false);
                m_ui->sB_2->setValue(m_ui->sB_2->value()-1);
                m_ui->LambdaPlot->replot();

              }
    }

    //11:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
    else if ((Reihe== 4)&&(Spalte==1))
        {
      //   double u =park;

            if( (m_ui->chartsview->model()->index(Reihe,Spalte).data(Qt::CheckStateRole)==Qt::Checked)&&(m_ui->sB_2->value()<4))
         { m_ui->sB_2->setValue(m_ui->sB_2->value()+1);


       //  m_ui->LambdaPlot->graph(10)->setName("park nut switch");
       //     m_ui->LambdaPlot->legend->item(10)->setVisible(true);
         m_ui->LambdaPlot->graph(10)->setVisible(true);


          m_ui->LambdaPlot->graph(10)->rescaleValueAxis(true);
            m_ui->LambdaPlot->replot();
            if (m_ui->sB_2->value()==1){
                           m_ui->LambdaPlot->graph(0)->setPen(QPen(Qt::blue));
                           m_ui->l_charts_1->setText("park");
                           m_ui->b_charts_1->setText(QString::number(park));}
            else if (m_ui->sB_2->value()==2){
                           m_ui->LambdaPlot->graph(0)->setPen(QPen(Qt::yellow));
                           m_ui->l_charts_2->setText("park");
                           m_ui->b_charts_2->setText(QString::number(park));}
           else if (m_ui->sB_2->value()==3){
                           m_ui->LambdaPlot->graph(0)->setPen(QPen(Qt::red));
                           m_ui->l_charts_3->setText("park");
                           m_ui->b_charts_3->setText(QString::number(park));}
           else if (m_ui->sB_2->value()==4){
                            m_ui->LambdaPlot->graph(0)->setPen(QPen(Qt::green));
                            m_ui->l_charts_4->setText("park");
                            m_ui->b_charts_4->setText(QString::number(park));}
            else qDebug()<<"spinbox zuhoch";

            }
          else{

                m_ui->LambdaPlot->graph(10)->setVisible(false);
            //    m_ui->LambdaPlot->legend->item(10)->setVisible(false);
                m_ui->sB_2->setValue(m_ui->sB_2->value()-1);
                m_ui->LambdaPlot->replot();


            }
    }
    //12::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
    else if ((Reihe== 5)&&(Spalte==1))
        {
      //   double u = fc;

            if( (m_ui->chartsview->model()->index(Reihe,Spalte).data(Qt::CheckStateRole)==Qt::Checked)&&(m_ui->sB_2->value()<4))
         {m_ui->sB_2->setValue(m_ui->sB_2->value()+1);


       //  m_ui->LambdaPlot->graph(11)->setName("faultcode");
       //   m_ui->LambdaPlot->legend->item(11)->setVisible(true);

        m_ui->LambdaPlot->graph(11)->setVisible(true);
          m_ui->LambdaPlot->graph(11)->rescaleValueAxis(true);
            m_ui->LambdaPlot->replot();
            if (m_ui->sB_2->value()==1){
                           m_ui->LambdaPlot->graph(0)->setPen(QPen(Qt::blue));
                           m_ui->l_charts_1->setText("fc");
                           m_ui->b_charts_1->setText(QString::number(fc));}
            else if (m_ui->sB_2->value()==2){
                           m_ui->LambdaPlot->graph(0)->setPen(QPen(Qt::yellow));
                           m_ui->l_charts_2->setText("fc");
                           m_ui->b_charts_2->setText(QString::number(fc));}
           else if (m_ui->sB_2->value()==3){
                           m_ui->LambdaPlot->graph(0)->setPen(QPen(Qt::red));
                           m_ui->l_charts_3->setText("fc");
                           m_ui->b_charts_3->setText(QString::number(fc));}
           else if (m_ui->sB_2->value()==4){
                            m_ui->LambdaPlot->graph(0)->setPen(QPen(Qt::green));
                            m_ui->l_charts_4->setText("fc");
                            m_ui->b_charts_4->setText(QString::number(fc));}
            else qDebug()<<"spinbox zuhoch";

            }
          else{


                m_ui->LambdaPlot->graph(11)->setVisible(false);
       //         m_ui->LambdaPlot->legend->item(11)->setVisible(false);
                m_ui->sB_2->setValue(m_ui->sB_2->value()-1);
                m_ui->LambdaPlot->replot();


             }
    }
      //13::::::::::::::::::::::::::::::::::
    else if ((Reihe== 6)&&(Spalte==1))
        {
    //     double u =idle_setpoint;

            if( (m_ui->chartsview->model()->index(Reihe,Spalte).data(Qt::CheckStateRole)==Qt::Checked)&&(m_ui->sB_2->value()<4))
         {		 m_ui->sB_2->setValue(m_ui->sB_2->value()+1);


            //       m_ui->LambdaPlot->graph(12)->setName("idlesetpoint");
            //       m_ui->LambdaPlot->legend->item(12)->setVisible(true);

                m_ui->LambdaPlot->graph(12)->setVisible(true);

                m_ui->LambdaPlot->graph(12)->rescaleValueAxis(true);
                m_ui->LambdaPlot->replot();
                if (m_ui->sB_2->value()==1){
                               m_ui->LambdaPlot->graph(0)->setPen(QPen(Qt::blue));
                               m_ui->l_charts_1->setText("idle_setpoint");
                               m_ui->b_charts_1->setText(QString::number(idle_setpoint));}
                else if (m_ui->sB_2->value()==2){
                               m_ui->LambdaPlot->graph(0)->setPen(QPen(Qt::yellow));
                               m_ui->l_charts_2->setText("idle_setpoint");
                               m_ui->b_charts_2->setText(QString::number(idle_setpoint));}
               else if (m_ui->sB_2->value()==3){
                               m_ui->LambdaPlot->graph(0)->setPen(QPen(Qt::red));
                               m_ui->l_charts_3->setText("idle_setpoint");
                               m_ui->b_charts_3->setText(QString::number(idle_setpoint));}
               else if (m_ui->sB_2->value()==4){
                                m_ui->LambdaPlot->graph(0)->setPen(QPen(Qt::green));
                                m_ui->l_charts_4->setText("idle_setpoint");
                                m_ui->b_charts_4->setText(QString::number(idle_setpoint));}
                else qDebug()<<"spinbox zuhoch";

            }
            else{
                m_ui->LambdaPlot->graph(12)->setVisible(false);
         //   m_ui->LambdaPlot->legend->item(12)->setVisible(false);
            m_ui->sB_2->setValue(m_ui->sB_2->value()-1);
            m_ui->LambdaPlot->replot();
            }
    }
    //14::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
    else if ((Reihe== 0)&&(Spalte==2))
        {
    //		 double u =hot;

                if( (m_ui->chartsview->model()->index(Reihe,Spalte).data(Qt::CheckStateRole)==Qt::Checked)&&(m_ui->sB_2->value()<4))
             {	 m_ui->sB_2->setValue(m_ui->sB_2->value()+1);


            //    m_ui->LambdaPlot->graph(13)->setName("idlehot");
            //    m_ui->LambdaPlot->legend->item(13)->setVisible(true);

                m_ui->LambdaPlot->graph(13)->setVisible(true);

                m_ui->LambdaPlot->graph(13)->rescaleValueAxis(true);
                m_ui->LambdaPlot->replot();
                if (m_ui->sB_2->value()==1){
                               m_ui->LambdaPlot->graph(0)->setPen(QPen(Qt::blue));
                               m_ui->l_charts_1->setText("hot");
                               m_ui->b_charts_1->setText(QString::number(hot));}
                else if (m_ui->sB_2->value()==2){
                               m_ui->LambdaPlot->graph(0)->setPen(QPen(Qt::yellow));
                               m_ui->l_charts_2->setText("hot");
                               m_ui->b_charts_2->setText(QString::number(hot));}
               else if (m_ui->sB_2->value()==3){
                               m_ui->LambdaPlot->graph(0)->setPen(QPen(Qt::red));
                               m_ui->l_charts_3->setText("hot");
                               m_ui->b_charts_3->setText(QString::number(hot));}
               else if (m_ui->sB_2->value()==4){
                                m_ui->LambdaPlot->graph(0)->setPen(QPen(Qt::green));
                                m_ui->l_charts_4->setText("hot");
                                m_ui->b_charts_4->setText(QString::number(hot));}
                else qDebug()<<"spinbox zuhoch";

            }
            else{
                    m_ui->LambdaPlot->graph(13)->setVisible(false);
          //          m_ui->LambdaPlot->legend->item(13)->setVisible(false);
                    m_ui->sB_2->setValue(m_ui->sB_2->value()-1);
                    m_ui->LambdaPlot->replot();
                }
         }

    //15:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
    else if ((Reihe== 1)&&(Spalte==2))
        {
    //     double u = uk2;

                if( (m_ui->chartsview->model()->index(Reihe,Spalte).data(Qt::CheckStateRole)==Qt::Checked)&&(m_ui->sB_2->value()<4))
             {
                    m_ui->sB_2->setValue(m_ui->sB_2->value()+1);
           //         m_ui->LambdaPlot->graph(14)->setName("unknown2");
           //         m_ui->LambdaPlot->legend->item(14)->setVisible(true);

                    m_ui->LambdaPlot->graph(14)->setVisible(true);
                    m_ui->LambdaPlot->graph(14)->rescaleValueAxis(true);
                    m_ui->LambdaPlot->replot();
                    if (m_ui->sB_2->value()==1){
                                   m_ui->LambdaPlot->graph(0)->setPen(QPen(Qt::blue));
                                   m_ui->l_charts_1->setText("uk2");
                                   m_ui->b_charts_1->setText(QString::number(uk2));}
                    else if (m_ui->sB_2->value()==2){
                                   m_ui->LambdaPlot->graph(0)->setPen(QPen(Qt::yellow));
                                   m_ui->l_charts_2->setText("uk2");
                                   m_ui->b_charts_2->setText(QString::number(uk2));}
                   else if (m_ui->sB_2->value()==3){
                                   m_ui->LambdaPlot->graph(0)->setPen(QPen(Qt::red));
                                   m_ui->l_charts_3->setText("uk2");
                                   m_ui->b_charts_3->setText(QString::number(uk2));}
                   else if (m_ui->sB_2->value()==4){
                                    m_ui->LambdaPlot->graph(0)->setPen(QPen(Qt::green));
                                    m_ui->l_charts_4->setText("uk2");
                                    m_ui->b_charts_4->setText(QString::number(uk2));}
                    else qDebug()<<"spinbox zuhoch";

                }
                else{
                    m_ui->LambdaPlot->graph(14)->setVisible(false);
          //      m_ui->LambdaPlot->legend->item(14)->setVisible(false);
                m_ui->sB_2->setValue(m_ui->sB_2->value()-1);
             m_ui->LambdaPlot->replot();
            }

         }

    //16::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
    else if ((Reihe== 2)&&(Spalte==2))
        {
     //    double u =iac;

            if( (m_ui->chartsview->model()->index(Reihe,Spalte).data(Qt::CheckStateRole)==Qt::Checked)&&(m_ui->sB_2->value()<4))
      {		m_ui->sB_2->setValue(m_ui->sB_2->value()+1);


      //   m_ui->LambdaPlot->graph(15)->setName("IAC");
      //    m_ui->LambdaPlot->legend->item(15)->setVisible(true);

         m_ui->LambdaPlot->graph(15)->setVisible(true);
            m_ui->LambdaPlot->graph(15)->valueAxis()->setRange(0,100);
          m_ui->LambdaPlot->graph(15)->rescaleValueAxis(true);
            m_ui->LambdaPlot->replot();
            if (m_ui->sB_2->value()==1){
                           m_ui->LambdaPlot->graph(0)->setPen(QPen(Qt::blue));
                           m_ui->l_charts_1->setText("iac");
                           m_ui->b_charts_1->setText(QString::number(iac));}
            else if (m_ui->sB_2->value()==2){
                           m_ui->LambdaPlot->graph(0)->setPen(QPen(Qt::yellow));
                           m_ui->l_charts_2->setText("iac");
                           m_ui->b_charts_2->setText(QString::number(iac));}
           else if (m_ui->sB_2->value()==3){
                           m_ui->LambdaPlot->graph(0)->setPen(QPen(Qt::red));
                           m_ui->l_charts_3->setText("iac");
                           m_ui->b_charts_3->setText(QString::number(iac));}
           else if (m_ui->sB_2->value()==4){
                            m_ui->LambdaPlot->graph(0)->setPen(QPen(Qt::green));
                            m_ui->l_charts_4->setText("iac");
                            m_ui->b_charts_4->setText(QString::number(iac));}
            else qDebug()<<"spinbox zuhoch";

            }
            else{
                m_ui->LambdaPlot->graph(15)->setVisible(false);
       //     m_ui->LambdaPlot->legend->item(15)->setVisible(false);
            m_ui->sB_2->setValue(m_ui->sB_2->value()-1);
             m_ui->LambdaPlot->replot();
            }
         }
        //17:::::::::::::::::::::::::::::::::::::::::::::::
    else if ((Reihe== 3)&&(Spalte==2))
        {
    //     double u = idl_dev;

            if( (m_ui->chartsview->model()->index(Reihe,Spalte).data(Qt::CheckStateRole)==Qt::Checked)&&(m_ui->sB_2->value()<4))
     {m_ui->sB_2->setValue(m_ui->sB_2->value()+1);

       //  m_ui->LambdaPlot->graph(16)->setName("idlespeeddev");
      //    m_ui->LambdaPlot->legend->item(16)->setVisible(true);

         m_ui->LambdaPlot->graph(16)->setVisible(true);
            m_ui->LambdaPlot->graph(16)->valueAxis()->setRange(0,200);
          m_ui->LambdaPlot->graph(16)->rescaleValueAxis(true);
            m_ui->LambdaPlot->replot();

            if (m_ui->sB_2->value()==1){
                           m_ui->LambdaPlot->graph(0)->setPen(QPen(Qt::blue));
                           m_ui->l_charts_1->setText("idlespeeddev");
                           m_ui->b_charts_1->setText(QString::number(idl_dev));}
            else if (m_ui->sB_2->value()==2){
                           m_ui->LambdaPlot->graph(0)->setPen(QPen(Qt::yellow));
                           m_ui->l_charts_2->setText("idlespeeddev");
                           m_ui->b_charts_2->setText(QString::number(idl_dev));}
           else if (m_ui->sB_2->value()==3){
                           m_ui->LambdaPlot->graph(0)->setPen(QPen(Qt::red));
                           m_ui->l_charts_3->setText("idlespeeddev");
                           m_ui->b_charts_3->setText(QString::number(idl_dev));}
           else if (m_ui->sB_2->value()==4){
                            m_ui->LambdaPlot->graph(0)->setPen(QPen(Qt::green));
                            m_ui->l_charts_4->setText("idlespeeddev");
                            m_ui->b_charts_4->setText(QString::number(idl_dev));}
            else qDebug()<<"spinbox zuhoch";
            }
          else{

                m_ui->LambdaPlot->graph(16)->setVisible(false);
         //     m_ui->LambdaPlot->legend->item(16)->setVisible(false);
              m_ui->sB_2->setValue(m_ui->sB_2->value()-1);
              m_ui->LambdaPlot->replot();

         }
    }
     //18::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
    else if ((Reihe== 4)&&(Spalte==2))
        {
       //  double u =ign_adv_off;

            if( (m_ui->chartsview->model()->index(Reihe,Spalte).data(Qt::CheckStateRole)==Qt::Checked)&&(m_ui->sB_2->value()<4))
              { m_ui->sB_2->setValue(m_ui->sB_2->value()+1);


        // m_ui->LambdaPlot->graph(17)->setName("ign advance offset");
        //  m_ui->LambdaPlot->legend->item(17)->setVisible(true);

         m_ui->LambdaPlot->graph(17)->setVisible(true);
          m_ui->LambdaPlot->graph(17)->valueAxis()->setRange(0,100);
          m_ui->LambdaPlot->graph(17)->rescaleValueAxis(true);
            m_ui->LambdaPlot->replot();
            if (m_ui->sB_2->value()==1){
                           m_ui->LambdaPlot->graph(0)->setPen(QPen(Qt::blue));
                           m_ui->l_charts_1->setText("ign advance offset");
                           m_ui->b_charts_1->setText(QString::number(ign_adv_off));}
            else if (m_ui->sB_2->value()==2){
                           m_ui->LambdaPlot->graph(0)->setPen(QPen(Qt::yellow));
                           m_ui->l_charts_2->setText("ign advance offset");
                           m_ui->b_charts_2->setText(QString::number(ign_adv_off));}
           else if (m_ui->sB_2->value()==3){
                           m_ui->LambdaPlot->graph(0)->setPen(QPen(Qt::red));
                           m_ui->l_charts_3->setText("ign advance offset");
                           m_ui->b_charts_3->setText(QString::number(ign_adv_off));}
           else if (m_ui->sB_2->value()==4){
                            m_ui->LambdaPlot->graph(0)->setPen(QPen(Qt::green));
                            m_ui->l_charts_4->setText("ign advance offset");
                            m_ui->b_charts_4->setText(QString::number(ign_adv_off));}
            else qDebug()<<"spinbox zuhoch";
                  }
          else{
                m_ui->LambdaPlot->graph(17)->setVisible(false);
            //  m_ui->LambdaPlot->legend->item(17)->setVisible(false);
              m_ui->sB_2->setValue(m_ui->sB_2->value()-1);
               m_ui->LambdaPlot->replot();
            }

    }
              //19::::::::::::::::::::::::::::::::::::::::::::::::::
    else if ((Reihe== 5)&&(Spalte==2))
        {
     //    double u =ign_adv;
            if( (m_ui->chartsview->model()->index(Reihe,Spalte).data(Qt::CheckStateRole)==Qt::Checked)&&(m_ui->sB_2->value()<4))
         {
              m_ui->sB_2->setValue(m_ui->sB_2->value()+1);

    //     m_ui->LambdaPlot->graph(18)->setName("ignitionadvance");
    //      m_ui->LambdaPlot->legend->item(18)->setVisible(true);

         m_ui->LambdaPlot->graph(18)->setVisible(true);
          m_ui->LambdaPlot->graph(18)->valueAxis()->setRange(0,50);
          m_ui->LambdaPlot->graph(18)->rescaleValueAxis(true);
            m_ui->LambdaPlot->replot();
            if (m_ui->sB_2->value()==1){
                           m_ui->LambdaPlot->graph(0)->setPen(QPen(Qt::blue));
                           m_ui->l_charts_1->setText("ign_adv");
                           m_ui->b_charts_1->setText(QString::number(ign_adv));}
            else if (m_ui->sB_2->value()==2){
                           m_ui->LambdaPlot->graph(0)->setPen(QPen(Qt::yellow));
                           m_ui->l_charts_2->setText("ign_adv");
                           m_ui->b_charts_2->setText(QString::number(ign_adv));}
           else if (m_ui->sB_2->value()==3){
                           m_ui->LambdaPlot->graph(0)->setPen(QPen(Qt::red));
                           m_ui->l_charts_3->setText("ign_adv");
                           m_ui->b_charts_3->setText(QString::number(ign_adv));}
           else if (m_ui->sB_2->value()==4){
                            m_ui->LambdaPlot->graph(0)->setPen(QPen(Qt::green));
                            m_ui->l_charts_4->setText("ign_adv");
                            m_ui->b_charts_4->setText(QString::number(ign_adv));}
            else qDebug()<<"spinbox zuhoch";

            }
          else{
                m_ui->LambdaPlot->graph(18)->setVisible(false);
       //     m_ui->LambdaPlot->legend->item(18)->setVisible(false);
            m_ui->sB_2->setValue(m_ui->sB_2->value()-1);
            m_ui->LambdaPlot->replot();
            }

    }

     //20:::::::::::::::::::::::::::::::::::::::::::::::::
    else if ((Reihe== 6)&&(Spalte==2))
        {
       //  double u =coil;

            if( (m_ui->chartsview->model()->index(Reihe,Spalte).data(Qt::CheckStateRole)==Qt::Checked)&&(m_ui->sB_2->value()<4))
                 {
                  m_ui->sB_2->setValue(m_ui->sB_2->value()+1);


         //            m_ui->LambdaPlot->graph(19)->setName("coiltime");
         //             m_ui->LambdaPlot->legend->item(19)->setVisible(true);

                     m_ui->LambdaPlot->graph(19)->setVisible(true);
                     m_ui->LambdaPlot->graph(19)->valueAxis()->setRange(0,50 );
                     m_ui->LambdaPlot->graph(19)->rescaleValueAxis(true);
                     m_ui->LambdaPlot->replot();
                     if (m_ui->sB_2->value()==1){
                                    m_ui->LambdaPlot->graph(0)->setPen(QPen(Qt::blue));
                                    m_ui->l_charts_1->setText("coiltime");
                                    m_ui->b_charts_1->setText(QString::number(coil));}
                     else if (m_ui->sB_2->value()==2){
                                    m_ui->LambdaPlot->graph(0)->setPen(QPen(Qt::yellow));
                                    m_ui->l_charts_2->setText("coiltime");
                                    m_ui->b_charts_2->setText(QString::number(coil));}
                    else if (m_ui->sB_2->value()==3){
                                    m_ui->LambdaPlot->graph(0)->setPen(QPen(Qt::red));
                                    m_ui->l_charts_3->setText("coiltime");
                                    m_ui->b_charts_3->setText(QString::number(coil));}
                    else if (m_ui->sB_2->value()==4){
                                     m_ui->LambdaPlot->graph(0)->setPen(QPen(Qt::green));
                                     m_ui->l_charts_4->setText("coiltime");
                                     m_ui->b_charts_4->setText(QString::number(coil));}
                     else qDebug()<<"spinbox zuhoch";
                }
                  else{
                    m_ui->LambdaPlot->graph(19)->setVisible(false);
               //     m_ui->LambdaPlot->legend->item(19)->setVisible(false);
                    m_ui->sB_2->setValue(m_ui->sB_2->value()-1);
                    m_ui->LambdaPlot->replot();
                    }

         }
    //21::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

    else if ((Reihe== 0)&&(Spalte==3))
        {
       //  double u = crancs;

            if( (m_ui->chartsview->model()->index(Reihe,Spalte).data(Qt::CheckStateRole)==Qt::Checked)&&(m_ui->sB_2->value()<4))
                 {m_ui->sB_2->setValue(m_ui->sB_2->value()+1);

          //       m_ui->LambdaPlot->graph(20)->setName("cranks");
          //        m_ui->LambdaPlot->legend->item(20)->setVisible(true);

                 m_ui->LambdaPlot->graph(20)->setVisible(true);
                  m_ui->LambdaPlot->graph(20)->rescaleValueAxis(true);
                    m_ui->LambdaPlot->replot();
                    if (m_ui->sB_2->value()==1){
                                   m_ui->LambdaPlot->graph(0)->setPen(QPen(Qt::blue));
                                   m_ui->l_charts_1->setText("crancshaftsensor");
                                   m_ui->b_charts_1->setText(QString::number(crancs));}
                    else if (m_ui->sB_2->value()==2){
                                   m_ui->LambdaPlot->graph(0)->setPen(QPen(Qt::yellow));
                                   m_ui->l_charts_2->setText("crancshaftsensor");
                                   m_ui->b_charts_2->setText(QString::number(crancs));}
                   else if (m_ui->sB_2->value()==3){
                                   m_ui->LambdaPlot->graph(0)->setPen(QPen(Qt::red));
                                   m_ui->l_charts_3->setText("crancshaftsensor");
                                   m_ui->b_charts_3->setText(QString::number(crancs));}
                   else if (m_ui->sB_2->value()==4){
                                    m_ui->LambdaPlot->graph(0)->setPen(QPen(Qt::green));
                                    m_ui->l_charts_4->setText("crancshaftsensor");
                                    m_ui->b_charts_4->setText(QString::number(crancs));}
                    else qDebug()<<"spinbox zuhoch";

                    }
                  else{
                m_ui->LambdaPlot->graph(20)->setVisible(false);
                   // m_ui->LambdaPlot->legend->item(20)->setVisible(false);
                    m_ui->sB_2->setValue(m_ui->sB_2->value()-1);
                    m_ui->LambdaPlot->replot();

                 }
         }
     //22:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
    else if ((Reihe== 1)&&(Spalte==3))
        {
       //  double u = uk4;

            if( (m_ui->chartsview->model()->index(Reihe,Spalte).data(Qt::CheckStateRole)==Qt::Checked)&&(m_ui->sB_2->value()<4))
                 {m_ui->sB_2->setValue(m_ui->sB_2->value()+1);


           //      m_ui->LambdaPlot->graph(21)->setName("unknown4");
           //       m_ui->LambdaPlot->legend->item(21)->setVisible(true);

                 m_ui->LambdaPlot->graph(21)->setVisible(true);
                 m_ui->LambdaPlot->graph(21)->rescaleValueAxis(true);
                    m_ui->LambdaPlot->replot();
                    if (m_ui->sB_2->value()==1){
                                   m_ui->LambdaPlot->graph(0)->setPen(QPen(Qt::blue));
                                   m_ui->l_charts_1->setText("uk4");
                                   m_ui->b_charts_1->setText(QString::number(uk4));}
                    else if (m_ui->sB_2->value()==2){
                                   m_ui->LambdaPlot->graph(0)->setPen(QPen(Qt::yellow));
                                   m_ui->l_charts_2->setText("uk4");
                                   m_ui->b_charts_2->setText(QString::number(uk4));}
                   else if (m_ui->sB_2->value()==3){
                                   m_ui->LambdaPlot->graph(0)->setPen(QPen(Qt::red));
                                   m_ui->l_charts_3->setText("uk4");
                                   m_ui->b_charts_3->setText(QString::number(uk4));}
                   else if (m_ui->sB_2->value()==4){
                                    m_ui->LambdaPlot->graph(0)->setPen(QPen(Qt::green));
                                    m_ui->l_charts_4->setText("uk4");
                                    m_ui->b_charts_4->setText(QString::number(uk4));}
                    else qDebug()<<"spinbox zuhoch";

                    }
                  else{
                m_ui->LambdaPlot->graph(21)->setVisible(false);
               //     m_ui->LambdaPlot->legend->item(21)->setVisible(false);
                    m_ui->sB_2->setValue(m_ui->sB_2->value()-1);
                    m_ui->LambdaPlot->replot();

                              }
         }
     //23:::::::::::::::::::::::::::::::::::::::::
    else if ((Reihe== 2)&&(Spalte==3))
        {
       //23  double u =uk5;

            if( (m_ui->chartsview->model()->index(Reihe,Spalte).data(Qt::CheckStateRole)==Qt::Checked)&&(m_ui->sB_2->value()<4))
           { m_ui->sB_2->setValue(m_ui->sB_2->value()+1);

        //          m_ui->LambdaPlot->legend->item(22)->setVisible(true);

        //       m_ui->LambdaPlot->graph(22)->setName("unknown5");

               m_ui->LambdaPlot->graph(22)->setVisible(true);
               m_ui->LambdaPlot->graph(22)->rescaleValueAxis(true);
               m_ui->LambdaPlot->replot();
               if (m_ui->sB_2->value()==1){
                              m_ui->LambdaPlot->graph(0)->setPen(QPen(Qt::blue));
                              m_ui->l_charts_1->setText("uk5");
                              m_ui->b_charts_1->setText(QString::number(uk5));}
               else if (m_ui->sB_2->value()==2){
                              m_ui->LambdaPlot->graph(0)->setPen(QPen(Qt::yellow));
                              m_ui->l_charts_2->setText("uk5");
                              m_ui->b_charts_2->setText(QString::number(uk5));}
              else if (m_ui->sB_2->value()==3){
                              m_ui->LambdaPlot->graph(0)->setPen(QPen(Qt::red));
                              m_ui->l_charts_3->setText("uk5");
                              m_ui->b_charts_3->setText(QString::number(uk5));}
              else if (m_ui->sB_2->value()==4){
                               m_ui->LambdaPlot->graph(0)->setPen(QPen(Qt::green));
                               m_ui->l_charts_4->setText("uk5");
                               m_ui->b_charts_4->setText(QString::number(uk5));}
               else qDebug()<<"spinbox zuhoch";
             }


            else{
                m_ui->LambdaPlot->graph(22)->setVisible(false);
           //   m_ui->LambdaPlot->legend->item(22)->setVisible(false);
              m_ui->sB_2->setValue(m_ui->sB_2->value()-1);
                 m_ui->LambdaPlot->replot();

            }
        }
        //24::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

    else if ((Reihe== 3)&&(Spalte==3))
         {
      //   double u =igns_witch;

            if( (m_ui->chartsview->model()->index(Reihe,Spalte).data(Qt::CheckStateRole)==Qt::Checked)&&(m_ui->sB_2->value()<4))
                 {m_ui->sB_2->setValue(m_ui->sB_2->value()+1);



            //     m_ui->LambdaPlot->graph(23)->setName("ignitionswitch");
            //    m_ui->LambdaPlot->legend->item(23)->setVisible(true);
                 m_ui->LambdaPlot->graph(23)->setVisible(true);
                 m_ui->LambdaPlot->graph(23)->valueAxis()->setRange(0,100);
                 m_ui->LambdaPlot->graph(23)->rescaleValueAxis(true);
                    m_ui->LambdaPlot->replot();
                    if (m_ui->sB_2->value()==1){
                                   m_ui->LambdaPlot->graph(0)->setPen(QPen(Qt::blue));
                                   m_ui->l_charts_1->setText("ignitionswitch");
                                   m_ui->b_charts_1->setText(QString::number(igns_witch));}
                    else if (m_ui->sB_2->value()==2){
                                   m_ui->LambdaPlot->graph(0)->setPen(QPen(Qt::yellow));
                                   m_ui->l_charts_2->setText("ignitionswitch");
                                   m_ui->b_charts_2->setText(QString::number(igns_witch));}
                   else if (m_ui->sB_2->value()==3){
                                   m_ui->LambdaPlot->graph(0)->setPen(QPen(Qt::red));
                                   m_ui->l_charts_3->setText("ignitionswitch");
                                   m_ui->b_charts_3->setText(QString::number(igns_witch));}
                   else if (m_ui->sB_2->value()==4){
                                    m_ui->LambdaPlot->graph(0)->setPen(QPen(Qt::green));
                                    m_ui->l_charts_4->setText("ignitionswitch");
                                    m_ui->b_charts_4->setText(QString::number(igns_witch));}
                    else qDebug()<<"spinbox zuhoch";

                    }
                  else{
                m_ui->LambdaPlot->graph(23)->setVisible(false);
             //       m_ui->LambdaPlot->legend->item(23)->setVisible(false);
                    m_ui->sB_2->setValue(m_ui->sB_2->value()-1);
                    m_ui->LambdaPlot->replot();
                  }
         }

    //25::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
    else if ((Reihe== 4)&&(Spalte==3))
         {
     //    double u = throttleangle;

            if( (m_ui->chartsview->model()->index(Reihe,Spalte).data(Qt::CheckStateRole)==Qt::Checked)&&(m_ui->sB_2->value()<4))
               {m_ui->sB_2->setValue(m_ui->sB_2->value()+1);



        //            m_ui->LambdaPlot->graph(24)->setName("throttleangle");
        //             m_ui->LambdaPlot->legend->item(24)->setVisible(true);
                    m_ui->LambdaPlot->graph(24)->setVisible(true);
                    m_ui->LambdaPlot->graph(24)->valueAxis()->setRange(0,100);
                    m_ui->LambdaPlot->graph(24)->rescaleValueAxis(true);
                    m_ui->LambdaPlot->replot();
                    if (m_ui->sB_2->value()==1){
                                   m_ui->LambdaPlot->graph(0)->setPen(QPen(Qt::blue));
                                   m_ui->l_charts_1->setText("throttleangle");
                                   m_ui->b_charts_1->setText(QString::number(throttleangle));}
                    else if (m_ui->sB_2->value()==2){
                                   m_ui->LambdaPlot->graph(0)->setPen(QPen(Qt::yellow));
                                   m_ui->l_charts_2->setText("throttleangle");
                                   m_ui->b_charts_2->setText(QString::number(throttleangle));}
                   else if (m_ui->sB_2->value()==3){
                                   m_ui->LambdaPlot->graph(0)->setPen(QPen(Qt::red));
                                   m_ui->l_charts_3->setText("throttleangle");
                                   m_ui->b_charts_3->setText(QString::number(throttleangle));}
                   else if (m_ui->sB_2->value()==4){
                                    m_ui->LambdaPlot->graph(0)->setPen(QPen(Qt::green));
                                    m_ui->l_charts_4->setText("throttleangle");
                                    m_ui->b_charts_4->setText(QString::number(throttleangle));}
                    else qDebug()<<"spinbox zuhoch";

                    }
                else{
                m_ui->LambdaPlot->graph(24)->setVisible(false);
            //      m_ui->LambdaPlot->legend->item(24)->setVisible(false);
                  m_ui->sB_2->setValue(m_ui->sB_2->value()-1);
                    m_ui->LambdaPlot->replot();
                  }
         }
                //26::::::::::::::::::::::::::::::::::::::::::::::::::
    else if ((Reihe== 5)&&(Spalte==3))
         {
       //  double u =uk6;

            if( (m_ui->chartsview->model()->index(Reihe,Spalte).data(Qt::CheckStateRole)==Qt::Checked)&&(m_ui->sB_2->value()<4))
                 {	m_ui->sB_2->setValue(m_ui->sB_2->value()+1);

       //          m_ui->LambdaPlot->graph(25)->setName("uk6");
       //          m_ui->LambdaPlot->legend->item(25)->setVisible(true);
                m_ui->LambdaPlot->graph(25)->setVisible(true);
                  m_ui->LambdaPlot->graph(25)->valueAxis()->setRange(0,100);
                 m_ui->LambdaPlot->graph(25)->rescaleValueAxis(true);
                    m_ui->LambdaPlot->replot();
                    if (m_ui->sB_2->value()==1){
                                   m_ui->LambdaPlot->graph(0)->setPen(QPen(Qt::blue));
                                   m_ui->l_charts_1->setText("uk6");
                                   m_ui->b_charts_1->setText(QString::number(uk6));}
                    else if (m_ui->sB_2->value()==2){
                                   m_ui->LambdaPlot->graph(0)->setPen(QPen(Qt::yellow));
                                   m_ui->l_charts_2->setText("uk6");
                                   m_ui->b_charts_2->setText(QString::number(uk6));}
                   else if (m_ui->sB_2->value()==3){
                                   m_ui->LambdaPlot->graph(0)->setPen(QPen(Qt::red));
                                   m_ui->l_charts_3->setText("uk6");
                                   m_ui->b_charts_3->setText(QString::number(uk6));}
                   else if (m_ui->sB_2->value()==4){
                                    m_ui->LambdaPlot->graph(0)->setPen(QPen(Qt::green));
                                    m_ui->l_charts_4->setText("uk6");
                                    m_ui->b_charts_4->setText(QString::number(uk6));}
                    else qDebug()<<"spinbox zuhoch";

                    }
                  else{
                m_ui->LambdaPlot->graph(25)->setVisible(false);
            //        m_ui->LambdaPlot->legend->item(25)->setVisible(false);
                    m_ui->sB_2->setValue(m_ui->sB_2->value()-1);
                     m_ui->LambdaPlot->replot();
                 }
         }
                    //27:::::::::::::::::::::::::::::::::::::::::::::::::::::::

    else if ((Reihe== 6)&&(Spalte==3))
         {
     //    double u =air_fuel_ratio;

            if( (m_ui->chartsview->model()->index(Reihe,Spalte).data(Qt::CheckStateRole)==Qt::Checked)&&(m_ui->sB_2->value()<4))
                 {m_ui->sB_2->setValue(m_ui->sB_2->value()+1);

        //         m_ui->LambdaPlot->graph(26)->setName("airfuelratio");
        //        m_ui->LambdaPlot->legend->item(26)->setVisible(true);
                 m_ui->LambdaPlot->graph(26)->setVisible(true);
                  m_ui->LambdaPlot->graph(26)->valueAxis()->setRange(0,100);
                 m_ui->LambdaPlot->graph(26)->rescaleValueAxis(true);
                    m_ui->LambdaPlot->replot();
                    if (m_ui->sB_2->value()==1){
                                   m_ui->LambdaPlot->graph(0)->setPen(QPen(Qt::blue));
                                   m_ui->l_charts_1->setText("air_fuel_ratio");
                                   m_ui->b_charts_1->setText(QString::number(air_fuel_ratio));}
                    else if (m_ui->sB_2->value()==2){
                                   m_ui->LambdaPlot->graph(0)->setPen(QPen(Qt::yellow));
                                   m_ui->l_charts_2->setText("air_fuel_ratio");
                                   m_ui->b_charts_2->setText(QString::number(air_fuel_ratio));}
                   else if (m_ui->sB_2->value()==3){
                                   m_ui->LambdaPlot->graph(0)->setPen(QPen(Qt::red));
                                   m_ui->l_charts_3->setText("air_fuel_ratio");
                                   m_ui->b_charts_3->setText(QString::number(air_fuel_ratio));}
                   else if (m_ui->sB_2->value()==4){
                                    m_ui->LambdaPlot->graph(0)->setPen(QPen(Qt::green));
                                    m_ui->l_charts_4->setText("air_fuel_ratio");
                                    m_ui->b_charts_4->setText(QString::number(air_fuel_ratio));}
                    else qDebug()<<"spinbox zuhoch";

                    }
                  else{
                      m_ui->LambdaPlot->graph(26)->setVisible(false);
      //              m_ui->LambdaPlot->legend->item(26)->setVisible(false);
                      m_ui->sB_2->setValue(m_ui->sB_2->value()-1);
                      m_ui->LambdaPlot->replot();
             }
         }
        //28:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
    else if ((Reihe== 0)&&(Spalte==4))
         {
     //    double u = fc0;

            if( (m_ui->chartsview->model()->index(Reihe,Spalte).data(Qt::CheckStateRole)==Qt::Checked)&&(m_ui->sB_2->value()<4))
             {m_ui->sB_2->setValue(m_ui->sB_2->value()+1);


                  m_ui->LambdaPlot->graph(27)->valueAxis()->setRange(0,100);
            //     m_ui->LambdaPlot->graph(27)->setName("faultcode0");
             //    m_ui->LambdaPlot->legend->item(27)->setVisible(true);
                 m_ui->LambdaPlot->graph(27)->setVisible(true);
                 m_ui->LambdaPlot->graph(27)->rescaleValueAxis(true);
                    m_ui->LambdaPlot->replot();
                    if (m_ui->sB_2->value()==1){
                                   m_ui->LambdaPlot->graph(0)->setPen(QPen(Qt::blue));
                                   m_ui->l_charts_1->setText("fc0");
                                   m_ui->b_charts_1->setText(QString::number(fc0));}
                    else if (m_ui->sB_2->value()==2){
                                   m_ui->LambdaPlot->graph(0)->setPen(QPen(Qt::yellow));
                                   m_ui->l_charts_2->setText("fc0");
                                   m_ui->b_charts_2->setText(QString::number(fc0));}
                   else if (m_ui->sB_2->value()==3){
                                   m_ui->LambdaPlot->graph(0)->setPen(QPen(Qt::red));
                                   m_ui->l_charts_3->setText("fc0");
                                   m_ui->b_charts_3->setText(QString::number(fc0));}
                   else if (m_ui->sB_2->value()==4){
                                    m_ui->LambdaPlot->graph(0)->setPen(QPen(Qt::green));
                                    m_ui->l_charts_4->setText("fc0");
                                    m_ui->b_charts_4->setText(QString::number(fc0));}
                    else qDebug()<<"spinbox zuhoch";

                    }
                  else{
                      m_ui->LambdaPlot->graph(27)->setVisible(false);
          //          m_ui->LambdaPlot->legend->item(27)->setVisible(false);
                       m_ui->sB_2->setValue(m_ui->sB_2->value()-1);
                      m_ui->LambdaPlot->replot();
               }
         }
            //29::::::::::::::::::::::::::::::::::::::::::::::::::::
    else if ((Reihe== 1)&&(Spalte==4))
        {
     //    double u =lambda;

            if( (m_ui->chartsview->model()->index(Reihe,Spalte).data(Qt::CheckStateRole)==Qt::Checked)&&(m_ui->sB_2->value()<4))
                 {
                 m_ui->sB_2->setValue(m_ui->sB_2->value()+1);
           //      m_ui->LambdaPlot->graph(28)->setName("lambdavoltage");
           //       m_ui->LambdaPlot->legend->item(28)->setVisible(true);


                 m_ui->LambdaPlot->graph(28)->setVisible(true);
                  m_ui->LambdaPlot->graph(28)->valueAxis()->setRange(0,1000);
                 m_ui->LambdaPlot->graph(28)->rescaleValueAxis(true);
                    m_ui->LambdaPlot->replot();
                    if (m_ui->sB_2->value()==1){
                                   m_ui->LambdaPlot->graph(0)->setPen(QPen(Qt::blue));
                                   m_ui->l_charts_1->setText("lambda");
                                   m_ui->b_charts_1->setText(QString::number(lambda));}
                    else if (m_ui->sB_2->value()==2){
                                   m_ui->LambdaPlot->graph(0)->setPen(QPen(Qt::yellow));
                                   m_ui->l_charts_2->setText("lambda");
                                   m_ui->b_charts_2->setText(QString::number(lambda));}
                   else if (m_ui->sB_2->value()==3){
                                   m_ui->LambdaPlot->graph(0)->setPen(QPen(Qt::red));
                                   m_ui->l_charts_3->setText("lambda");
                                   m_ui->b_charts_3->setText(QString::number(lambda));}
                   else if (m_ui->sB_2->value()==4){
                                    m_ui->LambdaPlot->graph(0)->setPen(QPen(Qt::green));
                                    m_ui->l_charts_4->setText("lambda");
                                    m_ui->b_charts_4->setText(QString::number(lambda));}
                    else qDebug()<<"spinbox zuhoch";

                    }
                  else{
                   m_ui->LambdaPlot->graph(28)->valueAxis()->setRange(0,100);
            //     m_ui->LambdaPlot->graph(28)->setVisible(false);
                   m_ui->LambdaPlot->legend->item(28)->setVisible(false);
                   m_ui->sB_2->setValue(m_ui->sB_2->value()-1);
                   m_ui->LambdaPlot->replot();
             }
         }

                //30::::::::::::::::::::::::::::::::::::::::::::::::::::

    else if ((Reihe== 2)&&(Spalte==4))
         {
      //30   double u = lambda_frequency;

            if( (m_ui->chartsview->model()->index(Reihe,Spalte).data(Qt::CheckStateRole)==Qt::Checked)&&(m_ui->sB_2->value()<4))
                 {m_ui->sB_2->setValue(m_ui->sB_2->value()+1);


             //    m_ui->LambdaPlot->graph(29)->setName("lambdafrequency");
              //   m_ui->LambdaPlot->legend->item(29)->setVisible(true);
                 m_ui->LambdaPlot->graph(29)->setVisible(true);
                  m_ui->LambdaPlot->graph(29)->valueAxis()->setRange(0,100);
                 m_ui->LambdaPlot->graph(29)->rescaleValueAxis(true);
                    m_ui->LambdaPlot->replot();

                    if (m_ui->sB_2->value()==1){
                                   m_ui->LambdaPlot->graph(0)->setPen(QPen(Qt::blue));
                                   m_ui->l_charts_1->setText("lambda_frequency");
                                   m_ui->b_charts_1->setText(QString::number(lambda_frequency));}
                    else if (m_ui->sB_2->value()==2){
                                   m_ui->LambdaPlot->graph(0)->setPen(QPen(Qt::yellow));
                                   m_ui->l_charts_2->setText("lambda_frequency");
                                   m_ui->b_charts_2->setText(QString::number(lambda_frequency));}
                   else if (m_ui->sB_2->value()==3){
                                   m_ui->LambdaPlot->graph(0)->setPen(QPen(Qt::red));
                                   m_ui->l_charts_3->setText("lambda_frequency");
                                   m_ui->b_charts_3->setText(QString::number(lambda_frequency));}
                   else if (m_ui->sB_2->value()==4){
                                    m_ui->LambdaPlot->graph(0)->setPen(QPen(Qt::green));
                                    m_ui->l_charts_4->setText("lambda_frequency");
                                    m_ui->b_charts_4->setText(QString::number(lambda_frequency));}
                    else qDebug()<<"spinbox zuhoch";
                    }
                  else{
                      m_ui->LambdaPlot->graph(29)->setVisible(false);
         //           m_ui->LambdaPlot->legend->item(29)->setVisible(false);
                      m_ui->sB_2->setValue(m_ui->sB_2->value()-1);
                      m_ui->LambdaPlot->replot();
            }
         }

      //31:::::::::::::::::::::::::::::::::::::::::::::::
    else if ((Reihe== 3)&&(Spalte==4))
         {
        // double u = lambda_dutycycle;
                if ((m_ui->chartsview->model()->index(Reihe,Spalte).data(Qt::CheckStateRole)==Qt::Checked)&&(m_ui->sB_2->value()<4))
                { m_ui->sB_2->setValue(m_ui->sB_2->value()+1);

                  m_ui->LambdaPlot->graph(30)->valueAxis()->setRange(0,100);
            //         m_ui->LambdaPlot->graph(30)->setName("lambdadutycycle");
            //         m_ui->LambdaPlot->legend->item(30)->setVisible(true);
                     m_ui->LambdaPlot->graph(30)->setVisible(true);

                     m_ui->LambdaPlot->graph(30)->rescaleValueAxis(true);
                        m_ui->LambdaPlot->replot();

                        if (m_ui->sB_2->value()==1){
                                       m_ui->LambdaPlot->graph(0)->setPen(QPen(Qt::blue));
                                       m_ui->l_charts_1->setText("lambda_dutycycle");
                                       m_ui->b_charts_1->setText(QString::number(lambda_dutycycle));}
                        else if (m_ui->sB_2->value()==2){
                                       m_ui->LambdaPlot->graph(0)->setPen(QPen(Qt::yellow));
                                       m_ui->l_charts_2->setText("lambda_dutycycle");
                                       m_ui->b_charts_2->setText(QString::number(lambda_dutycycle));}
                       else if (m_ui->sB_2->value()==3){
                                       m_ui->LambdaPlot->graph(0)->setPen(QPen(Qt::red));
                                       m_ui->l_charts_3->setText("lambda_dutycycle");
                                       m_ui->b_charts_3->setText(QString::number(lambda_dutycycle));}
                       else if (m_ui->sB_2->value()==4){
                                        m_ui->LambdaPlot->graph(0)->setPen(QPen(Qt::green));
                                        m_ui->l_charts_4->setText("lambda_dutycycle");
                                        m_ui->b_charts_4->setText(QString::number(lambda_dutycycle));}
                        else qDebug()<<"spinbox zuhoch";
                }
                  else
                  {
                    m_ui->LambdaPlot->graph(30)->setVisible(false);
            //        m_ui->LambdaPlot->legend->item(30)->setVisible(false);
                    m_ui->sB_2->setValue(m_ui->sB_2->value()-1);
                    m_ui->LambdaPlot->replot();
                  }
         }
                        //32::::::::::::::::::::::::::::::::::::::::::::::
    else if ((Reihe== 4)&&(Spalte==4))
         {
       //  double u =lambda_status;

            if( (m_ui->chartsview->model()->index(Reihe,Spalte).data(Qt::CheckStateRole)==Qt::Checked)&&(m_ui->sB_2->value()<4))
                 { m_ui->sB_2->setValue(m_ui->sB_2->value()+1);


       //          m_ui->LambdaPlot->graph(31)->setName("lambdastatus");
       //          m_ui->LambdaPlot->legend->item(31)->setVisible(true);
                 m_ui->LambdaPlot->graph(31)->setVisible(true);
                  m_ui->LambdaPlot->graph(31)->valueAxis()->setRange(0,100);
                 m_ui->LambdaPlot->graph(31)->rescaleValueAxis(true);
                    m_ui->LambdaPlot->replot();

                    if (m_ui->sB_2->value()==1){
                                   m_ui->LambdaPlot->graph(0)->setPen(QPen(Qt::blue));
                                   m_ui->l_charts_1->setText("lambda_status");
                                   m_ui->b_charts_1->setText(QString::number(lambda_status));}
                    else if (m_ui->sB_2->value()==2){
                                   m_ui->LambdaPlot->graph(0)->setPen(QPen(Qt::yellow));
                                   m_ui->l_charts_2->setText("lambda_status");
                                   m_ui->b_charts_2->setText(QString::number(lambda_status));}
                   else if (m_ui->sB_2->value()==3){
                                   m_ui->LambdaPlot->graph(0)->setPen(QPen(Qt::red));
                                   m_ui->l_charts_3->setText("lambda_status");
                                   m_ui->b_charts_3->setText(QString::number(lambda_status));}
                   else if (m_ui->sB_2->value()==4){
                                    m_ui->LambdaPlot->graph(0)->setPen(QPen(Qt::green));
                                    m_ui->l_charts_4->setText("lambda_status");
                                    m_ui->b_charts_4->setText(QString::number(lambda_status));}
                    else qDebug()<<"spinbox zuhoch";

                    }
                  else{
                        m_ui->LambdaPlot->graph(31)->setVisible(false);
         //               m_ui->LambdaPlot->legend->item(31)->setVisible(false);
                        m_ui->sB_2->setValue(m_ui->sB_2->value()-1);
                        m_ui->LambdaPlot->replot();
                  }
         }
    //33:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
    else if ((Reihe== 5)&&(Spalte==4))
         {
     //    double u =closed_loop;

            if( (m_ui->chartsview->model()->index(Reihe,Spalte).data(Qt::CheckStateRole)==Qt::Checked)&&(m_ui->sB_2->value()<4))
                 {m_ui->sB_2->setValue(m_ui->sB_2->value()+1);



         //        m_ui->LambdaPlot->graph(32)->setName("closedloop");
          //       m_ui->LambdaPlot->legend->item(32)->setVisible(true);
                 m_ui->LambdaPlot->graph(32)->setVisible(true);
                  m_ui->LambdaPlot->graph(32)->valueAxis()->setRange(0,100);
                 m_ui->LambdaPlot->graph(32)->rescaleValueAxis(true);
                    m_ui->LambdaPlot->replot();
                    if (m_ui->sB_2->value()==1){
                                   m_ui->LambdaPlot->graph(0)->setPen(QPen(Qt::blue));
                                   m_ui->l_charts_1->setText("closed_loop");
                                   m_ui->b_charts_1->setText(QString::number(closed_loop));}
                    else if (m_ui->sB_2->value()==2){
                                   m_ui->LambdaPlot->graph(0)->setPen(QPen(Qt::yellow));
                                   m_ui->l_charts_2->setText("closed_loop");
                                   m_ui->b_charts_2->setText(QString::number(closed_loop));}
                   else if (m_ui->sB_2->value()==3){
                                   m_ui->LambdaPlot->graph(0)->setPen(QPen(Qt::red));
                                   m_ui->l_charts_3->setText("closed_loop");
                                   m_ui->b_charts_3->setText(QString::number(closed_loop));}
                   else if (m_ui->sB_2->value()==4){
                                    m_ui->LambdaPlot->graph(0)->setPen(QPen(Qt::green));
                                    m_ui->l_charts_4->setText("closed_loop");
                                    m_ui->b_charts_4->setText(QString::number(closed_loop));}
                    else qDebug()<<"spinbox zuhoch";


                    }
                  else{
                      m_ui->LambdaPlot->graph(32)->setVisible(false);
         //           m_ui->LambdaPlot->legend->item(32)->setVisible(false);
                      m_ui->sB_2->setValue(m_ui->sB_2->value()-1);
                      m_ui->LambdaPlot->replot();
                 }
         }
     //34:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
    else if ((Reihe==6)&&(Spalte==4))
         {
     //    double u =ltft;

            if( (m_ui->chartsview->model()->index(Reihe,Spalte).data(Qt::CheckStateRole)==Qt::Checked)&&(m_ui->sB_2->value()<4))
                 {m_ui->sB_2->setValue(m_ui->sB_2->value()+1);

                  m_ui->LambdaPlot->graph(33)->valueAxis()->setRange(0,150);

          //       m_ui->LambdaPlot->graph(33)->setName("long term fT");
          //       m_ui->LambdaPlot->legend->item(33)->setVisible(true);
                 m_ui->LambdaPlot->graph(33)->setVisible(true);
                 m_ui->LambdaPlot->graph(33)->rescaleValueAxis(true);
                    m_ui->LambdaPlot->replot();

                    if (m_ui->sB_2->value()==1){
                                   m_ui->LambdaPlot->graph(0)->setPen(QPen(Qt::blue));
                                   m_ui->l_charts_1->setText("ltft");
                                   m_ui->b_charts_1->setText(QString::number(ltft));}
                    else if (m_ui->sB_2->value()==2){
                                   m_ui->LambdaPlot->graph(0)->setPen(QPen(Qt::yellow));
                                   m_ui->l_charts_2->setText("ltft");
                                   m_ui->b_charts_2->setText(QString::number(ltft));}
                   else if (m_ui->sB_2->value()==3){
                                   m_ui->LambdaPlot->graph(0)->setPen(QPen(Qt::red));
                                   m_ui->l_charts_3->setText("ltft");
                                   m_ui->b_charts_3->setText(QString::number(ltft));}
                   else if (m_ui->sB_2->value()==4){
                                    m_ui->LambdaPlot->graph(0)->setPen(QPen(Qt::green));
                                    m_ui->l_charts_4->setText("ltft");
                                    m_ui->b_charts_4->setText(QString::number(ltft));}
                    else qDebug()<<"spinbox zuhoch";

                    }
                  else{
                      m_ui->LambdaPlot->graph(33)->setVisible(false);
          //          m_ui->LambdaPlot->legend->item(33)->setVisible(false);
                      m_ui->sB_2->setValue(m_ui->sB_2->value()-1);
                      m_ui->LambdaPlot->replot();
                  }
         }
                //35:::::::::::::::::::::::::::::::::::::::::::::::::
    else if ((Reihe== 0)&&(Spalte==5))
         {
       //  double u = stft;

            if( (m_ui->chartsview->model()->index(Reihe,Spalte).data(Qt::CheckStateRole)==Qt::Checked)&&(m_ui->sB_2->value()<4))
                 {m_ui->sB_2->setValue(m_ui->sB_2->value()+1);



                // m_ui->LambdaPlot->graph(34)->setName("shorttermfueltrim");
              //   m_ui->LambdaPlot->legend->item(34)->setVisible(true);
                   m_ui->LambdaPlot->graph(34)->setVisible(true);
                  m_ui->LambdaPlot->graph(34)->valueAxis()->setRange(0,150);
                  m_ui->LambdaPlot->graph(34)->rescaleValueAxis(true);
                    m_ui->LambdaPlot->replot();
                    if (m_ui->sB_2->value()==1){
                                   m_ui->LambdaPlot->graph(0)->setPen(QPen(Qt::blue));
                                   m_ui->l_charts_1->setText("stft");
                                   m_ui->b_charts_1->setText(QString::number(stft));}
                    else if (m_ui->sB_2->value()==2){
                                   m_ui->LambdaPlot->graph(0)->setPen(QPen(Qt::yellow));
                                   m_ui->l_charts_2->setText("stft");
                                   m_ui->b_charts_2->setText(QString::number(stft));}
                   else if (m_ui->sB_2->value()==3){
                                   m_ui->LambdaPlot->graph(0)->setPen(QPen(Qt::red));
                                   m_ui->l_charts_3->setText("stft");
                                   m_ui->b_charts_3->setText(QString::number(stft));}
                   else if (m_ui->sB_2->value()==4){
                                    m_ui->LambdaPlot->graph(0)->setPen(QPen(Qt::green));
                                    m_ui->l_charts_4->setText("stft");
                                    m_ui->b_charts_4->setText(QString::number(stft));}
                    else qDebug()<<"spinbox zuhoch";


                    }
                  else{
                     m_ui->LambdaPlot->graph(34)->setVisible(false);
            //       m_ui->LambdaPlot->legend->item(34)->setVisible(false);
                     m_ui->sB_2->setValue(m_ui->sB_2->value()-1);
                     m_ui->LambdaPlot->replot();
              }
         }
      //36:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
    else if ((Reihe== 1)&&(Spalte==5))
         {
      //   double u =carbon;

            if( (m_ui->chartsview->model()->index(Reihe,Spalte).data(Qt::CheckStateRole)==Qt::Checked)&&(m_ui->sB_2->value()<4))
                 {m_ui->sB_2->setValue(m_ui->sB_2->value()+1);



       //          m_ui->LambdaPlot->graph(35)->setName("carboncanister");
       //          m_ui->LambdaPlot->legend->item(35)->setVisible(true);
                 m_ui->LambdaPlot->graph(35)->setVisible(true);
                  m_ui->LambdaPlot->graph(35)->valueAxis()->setRange(0,100);
                  m_ui->LambdaPlot->graph(35)->rescaleValueAxis(true);
                    m_ui->LambdaPlot->replot();

                    if (m_ui->sB_2->value()==1){
                                   m_ui->LambdaPlot->graph(0)->setPen(QPen(Qt::blue));
                                   m_ui->l_charts_1->setText("carboncanister");
                                   m_ui->b_charts_1->setText(QString::number(carbon));}
                    else if (m_ui->sB_2->value()==2){
                                   m_ui->LambdaPlot->graph(0)->setPen(QPen(Qt::yellow));
                                   m_ui->l_charts_2->setText("carboncanister");
                                   m_ui->b_charts_2->setText(QString::number(carbon));}
                   else if (m_ui->sB_2->value()==3){
                                   m_ui->LambdaPlot->graph(0)->setPen(QPen(Qt::red));
                                   m_ui->l_charts_3->setText("carboncanister");
                                   m_ui->b_charts_3->setText(QString::number(carbon));}
                   else if (m_ui->sB_2->value()==4){
                                    m_ui->LambdaPlot->graph(0)->setPen(QPen(Qt::green));
                                    m_ui->l_charts_4->setText("carboncanister");
                                    m_ui->b_charts_4->setText(QString::number(carbon));}
                    else qDebug()<<"spinbox zuhoch";

                    }
                  else{
                      m_ui->LambdaPlot->graph(35)->setVisible(false);
       //             m_ui->LambdaPlot->legend->item(35)->setVisible(false);
                      m_ui->sB_2->setValue(m_ui->sB_2->value()-1);
                      m_ui->LambdaPlot->replot();
               }
         }
    //37:::::::::::::::::::::::::::::::::::::::::::::::::::::::::

    else if ((Reihe== 2)&&(Spalte==5))
         {
     //    double u =fc1;

            if( (m_ui->chartsview->model()->index(Reihe,Spalte).data(Qt::CheckStateRole)==Qt::Checked)&&(m_ui->sB_2->value()<4))
                 {m_ui->sB_2->setValue(m_ui->sB_2->value()+1);

                  m_ui->LambdaPlot->graph(36)->valueAxis()->setRange(0,100);

           //      m_ui->LambdaPlot->graph(36)->setName("faultcode 1");
           //      m_ui->LambdaPlot->legend->item(36)->setVisible(true);
                  m_ui->LambdaPlot->graph(36)->setVisible(true);
                  m_ui->LambdaPlot->graph(36)->rescaleValueAxis(true);
                    m_ui->LambdaPlot->replot();
                    if (m_ui->sB_2->value()==1){
                                   m_ui->LambdaPlot->graph(0)->setPen(QPen(Qt::blue));
                                   m_ui->l_charts_1->setText("fc1");
                                   m_ui->b_charts_1->setText(QString::number(fc1));}
                    else if (m_ui->sB_2->value()==2){
                                   m_ui->LambdaPlot->graph(0)->setPen(QPen(Qt::yellow));
                                   m_ui->l_charts_2->setText("fc1");
                                   m_ui->b_charts_2->setText(QString::number(fc1));}
                   else if (m_ui->sB_2->value()==3){
                                   m_ui->LambdaPlot->graph(0)->setPen(QPen(Qt::red));
                                   m_ui->l_charts_3->setText("fc1");
                                   m_ui->b_charts_3->setText(QString::number(fc1));}
                   else if (m_ui->sB_2->value()==4){
                                    m_ui->LambdaPlot->graph(0)->setPen(QPen(Qt::green));
                                    m_ui->l_charts_4->setText("fc1");
                                    m_ui->b_charts_4->setText(QString::number(fc1));}
                    else qDebug()<<"spinbox zuhoch";

                    }
                  else{
                      m_ui->LambdaPlot->graph(36)->setVisible(false);
         //           m_ui->LambdaPlot->legend->item(36)->setVisible(false);
                      m_ui->sB_2->setValue(m_ui->sB_2->value()-1);
                      m_ui->LambdaPlot->replot();
               }
         }
      //38:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
    else if ((Reihe== 3)&&(Spalte==5))
         {
     //    double u = idle_base;

            if( (m_ui->chartsview->model()->index(Reihe,Spalte).data(Qt::CheckStateRole)==Qt::Checked)&&(m_ui->sB_2->value()<4))
         {m_ui->sB_2->setValue(m_ui->sB_2->value()+1);



        //      m_ui->LambdaPlot->graph(37)->setName("idlebase");
        //          m_ui->LambdaPlot->legend->item(37)->setVisible(true);
                 m_ui->LambdaPlot->graph(37)->setVisible(true);
                  m_ui->LambdaPlot->graph(37)->valueAxis()->setRange(0,100);
                  m_ui->LambdaPlot->graph(37)->rescaleValueAxis(true);
                    m_ui->LambdaPlot->replot();
                    if (m_ui->sB_2->value()==1){
                                   m_ui->LambdaPlot->graph(0)->setPen(QPen(Qt::blue));
                                   m_ui->l_charts_1->setText("idle_base");
                                   m_ui->b_charts_1->setText(QString::number(idle_base));}
                    else if (m_ui->sB_2->value()==2){
                                   m_ui->LambdaPlot->graph(0)->setPen(QPen(Qt::yellow));
                                   m_ui->l_charts_2->setText("idle_base");
                                   m_ui->b_charts_2->setText(QString::number(idle_base));}
                   else if (m_ui->sB_2->value()==3){
                                   m_ui->LambdaPlot->graph(0)->setPen(QPen(Qt::red));
                                   m_ui->l_charts_3->setText("idle_base");
                                   m_ui->b_charts_3->setText(QString::number(idle_base));}
                   else if (m_ui->sB_2->value()==4){
                                    m_ui->LambdaPlot->graph(0)->setPen(QPen(Qt::green));
                                    m_ui->l_charts_4->setText("idle_base");
                                    m_ui->b_charts_4->setText(QString::number(idle_base));}
                    else qDebug()<<"spinbox zuhoch";


                    }
                  else{
                      m_ui->LambdaPlot->graph(37)->setVisible(false);
       //             m_ui->LambdaPlot->legend->item(37)->setVisible(false);
                      m_ui->sB_2->setValue(m_ui->sB_2->value()-1);
                      m_ui->LambdaPlot->replot();
                }
         }
          //39::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
    else if ((Reihe== 4)&&(Spalte==5))
         {
      //   double u =uk7;

            if( (m_ui->chartsview->model()->index(Reihe,Spalte).data(Qt::CheckStateRole)==Qt::Checked)&&(m_ui->sB_2->value()<4))
             {m_ui->sB_2->setValue(m_ui->sB_2->value()+1);


                  m_ui->LambdaPlot->graph(38)->valueAxis()->setRange(0,100);

           //      m_ui->LambdaPlot->graph(38)->setName("unknown7");
            //      m_ui->LambdaPlot->legend->item(38)->setVisible(true);

                 m_ui->LambdaPlot->graph(38)->setVisible(true);
                  m_ui->LambdaPlot->graph(38)->rescaleValueAxis(true);
                    m_ui->LambdaPlot->replot();
                    if (m_ui->sB_2->value()==1){
                                   m_ui->LambdaPlot->graph(0)->setPen(QPen(Qt::blue));
                                   m_ui->l_charts_1->setText("uk7");
                                   m_ui->b_charts_1->setText(QString::number(uk7));}
                    else if (m_ui->sB_2->value()==2){
                                   m_ui->LambdaPlot->graph(0)->setPen(QPen(Qt::yellow));
                                   m_ui->l_charts_2->setText("uk7");
                                   m_ui->b_charts_2->setText(QString::number(uk7));}
                   else if (m_ui->sB_2->value()==3){
                                   m_ui->LambdaPlot->graph(0)->setPen(QPen(Qt::red));
                                   m_ui->l_charts_3->setText("uk7");
                                   m_ui->b_charts_3->setText(QString::number(uk7));}
                   else if (m_ui->sB_2->value()==4){
                                    m_ui->LambdaPlot->graph(0)->setPen(QPen(Qt::green));
                                    m_ui->l_charts_4->setText("uk7");
                                    m_ui->b_charts_4->setText(QString::number(uk7));}
                    else qDebug()<<"spinbox zuhoch";

                    }
                  else{
                      m_ui->LambdaPlot->graph(38)->setVisible(false);
         //           m_ui->LambdaPlot->legend->item(38)->setVisible(false);
                      m_ui->sB_2->setValue(m_ui->sB_2->value()-1);
                      m_ui->LambdaPlot->replot();
                }
         }
              //40:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
    else if ((Reihe== 5)&&(Spalte==5))
         {
     //    double u =uk8;

            if( (m_ui->chartsview->model()->index(Reihe,Spalte).data(Qt::CheckStateRole)==Qt::Checked)&&(m_ui->sB_2->value()<4))
                 {m_ui->sB_2->setValue(m_ui->sB_2->value()+1);

                  m_ui->LambdaPlot->graph(39)->valueAxis()->setRange(0,100);

           //      m_ui->LambdaPlot->graph(39)->setName("unknown 8");
           //       m_ui->LambdaPlot->legend->item(39)->setVisible(true);

                 m_ui->LambdaPlot->graph(39)->setVisible(true);
                  m_ui->LambdaPlot->graph(39)->rescaleValueAxis(true);
                    m_ui->LambdaPlot->replot();

                    if (m_ui->sB_2->value()==1){
                                   m_ui->LambdaPlot->graph(0)->setPen(QPen(Qt::blue));
                                   m_ui->l_charts_1->setText("uk8");
                                   m_ui->b_charts_1->setText(QString::number(uk8));}
                    else if (m_ui->sB_2->value()==2){
                                   m_ui->LambdaPlot->graph(0)->setPen(QPen(Qt::yellow));
                                   m_ui->l_charts_2->setText("uk8");
                                   m_ui->b_charts_2->setText(QString::number(uk8));}
                   else if (m_ui->sB_2->value()==3){
                                   m_ui->LambdaPlot->graph(0)->setPen(QPen(Qt::red));
                                   m_ui->l_charts_3->setText("uk8");
                                   m_ui->b_charts_3->setText(QString::number(uk8));}
                   else if (m_ui->sB_2->value()==4){
                                    m_ui->LambdaPlot->graph(0)->setPen(QPen(Qt::green));
                                    m_ui->l_charts_4->setText("uk8");
                                    m_ui->b_charts_4->setText(QString::number(uk8));}
                    else qDebug()<<"spinbox zuhoch";
                    }
                  else{
                      m_ui->LambdaPlot->graph(39)->setVisible(false);
        //            m_ui->LambdaPlot->legend->item(39)->setVisible(false);
                      m_ui->sB_2->setValue(m_ui->sB_2->value()-1);
                      m_ui->LambdaPlot->replot();
                }
         }




      //41:::::::::::::::::::::::::::::::::::::::::::::::
    else if ((Reihe== 6)&&(Spalte==5))
         {
      //    double u = ign_adv2;

            if( (m_ui->chartsview->model()->index(Reihe,Spalte).data(Qt::CheckStateRole)==Qt::Checked)&&(m_ui->sB_2->value()<4))
                 {m_ui->sB_2->setValue(m_ui->sB_2->value()+1);


                  m_ui->LambdaPlot->graph(40)->valueAxis()->setRange(0,100);

            //     m_ui->LambdaPlot->graph(40)->setName("ignadv2");
            //      m_ui->LambdaPlot->legend->item(40)->setVisible(true);

                m_ui->LambdaPlot->graph(40)->setVisible(true);
                 m_ui->LambdaPlot->graph(40)->rescaleValueAxis(true);
                    m_ui->LambdaPlot->replot();
                    if (m_ui->sB_2->value()==1){
                                   m_ui->LambdaPlot->graph(0)->setPen(QPen(Qt::blue));
                                   m_ui->l_charts_1->setText("ign_adv2");
                                   m_ui->b_charts_1->setText(QString::number(ign_adv2));}
                    else if (m_ui->sB_2->value()==2){
                                   m_ui->LambdaPlot->graph(0)->setPen(QPen(Qt::yellow));
                                   m_ui->l_charts_2->setText("ign_adv2");
                                   m_ui->b_charts_2->setText(QString::number(ign_adv2));}
                   else if (m_ui->sB_2->value()==3){
                                   m_ui->LambdaPlot->graph(0)->setPen(QPen(Qt::red));
                                   m_ui->l_charts_3->setText("ign_adv2");
                                   m_ui->b_charts_3->setText(QString::number(ign_adv2));}
                   else if (m_ui->sB_2->value()==4){
                                    m_ui->LambdaPlot->graph(0)->setPen(QPen(Qt::green));
                                    m_ui->l_charts_4->setText("ign_adv2");
                                    m_ui->b_charts_4->setText(QString::number(ign_adv2));}
                    else qDebug()<<"spinbox zuhoch";

                }
                  else{
                     m_ui->LambdaPlot->graph(40)->setVisible(false);
           //         m_ui->LambdaPlot->legend->item(40)->setVisible(false);
                     m_ui->sB_2->setValue(m_ui->sB_2->value()-1);
                     m_ui->LambdaPlot->replot();
               }
         }
     //42:::::::::::::::::::::::::::::::::::::::::::::
    else if ((Reihe== 0)&&(Spalte==6))
         {
     //     double u = uk9;

            if( (m_ui->chartsview->model()->index(Reihe,Spalte).data(Qt::CheckStateRole)==Qt::Checked)&&(m_ui->sB_2->value()<4))
                 {m_ui->sB_2->setValue(m_ui->sB_2->value()+1);

                  m_ui->LambdaPlot->graph(41)->valueAxis()->setRange(0,100);

          //       m_ui->LambdaPlot->graph(41)->setName("uk9");
         //         m_ui->LambdaPlot->legend->item(41)->setVisible(true);

                 m_ui->LambdaPlot->graph(41)->setVisible(true);

                 m_ui->LambdaPlot->graph(41)->rescaleValueAxis(true);
                    m_ui->LambdaPlot->replot();
                    if (m_ui->sB_2->value()==1){
                                   m_ui->LambdaPlot->graph(0)->setPen(QPen(Qt::blue));
                                   m_ui->l_charts_1->setText("uk9");
                                   m_ui->b_charts_1->setText(QString::number(uk9));}
                    else if (m_ui->sB_2->value()==2){
                                   m_ui->LambdaPlot->graph(0)->setPen(QPen(Qt::yellow));
                                   m_ui->l_charts_2->setText("uk9");
                                   m_ui->b_charts_2->setText(QString::number(uk9));}
                   else if (m_ui->sB_2->value()==3){
                                   m_ui->LambdaPlot->graph(0)->setPen(QPen(Qt::red));
                                   m_ui->l_charts_3->setText("uk9");
                                   m_ui->b_charts_3->setText(QString::number(uk9));}
                   else if (m_ui->sB_2->value()==4){
                                    m_ui->LambdaPlot->graph(0)->setPen(QPen(Qt::green));
                                    m_ui->l_charts_4->setText("uk9");
                                    m_ui->b_charts_4->setText(QString::number(uk9));}
                    else qDebug()<<"spinbox zuhoch";

                    }
                  else{
                      m_ui->LambdaPlot->graph(41)->setVisible(false);
       //             m_ui->LambdaPlot->legend->item(41)->setVisible(false);
                      m_ui->sB_2->setValue(m_ui->sB_2->value()-1);
                      m_ui->LambdaPlot->replot();
                 }
         }
      //43::::::::::::::::::::::::::::::::::::::::
    else if ((Reihe== 1)&&(Spalte==6))
         {
       //   double u = idle_error2;

            if( (m_ui->chartsview->model()->index(Reihe,Spalte).data(Qt::CheckStateRole)==Qt::Checked)&&(m_ui->sB_2->value()<4))
                 {m_ui->sB_2->setValue(m_ui->sB_2->value()+1);


                  m_ui->LambdaPlot->graph(42)->valueAxis()->setRange(0,100);

        //         m_ui->LambdaPlot->graph(42)->setName("idleerror2");
        //          m_ui->LambdaPlot->legend->item(42)->setVisible(true);

                 m_ui->LambdaPlot->graph(42)->setVisible(true);
                 m_ui->LambdaPlot->graph(42)->rescaleValueAxis(true);
                    m_ui->LambdaPlot->replot();
                    if (m_ui->sB_2->value()==1){
                                   m_ui->LambdaPlot->graph(0)->setPen(QPen(Qt::blue));
                                   m_ui->l_charts_1->setText("idle_error2");
                                   m_ui->b_charts_1->setText(QString::number(idle_error2));}
                    else if (m_ui->sB_2->value()==2){
                                   m_ui->LambdaPlot->graph(0)->setPen(QPen(Qt::yellow));
                                   m_ui->l_charts_2->setText("idle_error2");
                                   m_ui->b_charts_2->setText(QString::number(idle_error2));}
                   else if (m_ui->sB_2->value()==3){
                                   m_ui->LambdaPlot->graph(0)->setPen(QPen(Qt::red));
                                   m_ui->l_charts_3->setText("idle_error2");
                                   m_ui->b_charts_3->setText(QString::number(idle_error2));}
                   else if (m_ui->sB_2->value()==4){
                                    m_ui->LambdaPlot->graph(0)->setPen(QPen(Qt::green));
                                    m_ui->l_charts_4->setText("idle_error2");
                                    m_ui->b_charts_4->setText(QString::number(idle_error2));}
                    else qDebug()<<"spinbox zuhoch";

                    }
                  else{
                     m_ui->LambdaPlot->graph(42)->setVisible(false);
       //             m_ui->LambdaPlot->legend->item(42)->setVisible(false);
                     m_ui->sB_2->setValue(m_ui->sB_2->value()-1);
                     m_ui->LambdaPlot->replot();
              }
         }
     //44::::::::::::::::::::::::::::::::::::::::::::::::::::::::
    else if ((Reihe== 2)&&(Spalte==6))
         {
      //    double u = uk10;

            if( (m_ui->chartsview->model()->index(Reihe,Spalte).data(Qt::CheckStateRole)==Qt::Checked)&&(m_ui->sB_2->value()<4))
                {m_ui->sB_2->setValue(m_ui->sB_2->value()+1);


                  m_ui->LambdaPlot->graph(43)->valueAxis()->setRange(0,100);

         //        m_ui->LambdaPlot->graph(43)->setName("uk10");
         //         m_ui->LambdaPlot->legend->item(43)->setVisible(true);

                 m_ui->LambdaPlot->graph(43)->setVisible(true);
                 m_ui->LambdaPlot->graph(43)->rescaleValueAxis(true);
                    m_ui->LambdaPlot->replot();
                    if (m_ui->sB_2->value()==1){
                                   m_ui->LambdaPlot->graph(0)->setPen(QPen(Qt::blue));
                                   m_ui->l_charts_1->setText("uk10");
                                   m_ui->b_charts_1->setText(QString::number(uk10));}
                    else if (m_ui->sB_2->value()==2){
                                   m_ui->LambdaPlot->graph(0)->setPen(QPen(Qt::yellow));
                                   m_ui->l_charts_2->setText("uk10");
                                   m_ui->b_charts_2->setText(QString::number(uk10));}
                   else if (m_ui->sB_2->value()==3){
                                   m_ui->LambdaPlot->graph(0)->setPen(QPen(Qt::red));
                                   m_ui->l_charts_3->setText("uk10");
                                   m_ui->b_charts_3->setText(QString::number(uk10));}
                   else if (m_ui->sB_2->value()==4){
                                    m_ui->LambdaPlot->graph(0)->setPen(QPen(Qt::green));
                                    m_ui->l_charts_4->setText("uk10");
                                    m_ui->b_charts_4->setText(QString::number(uk10));}
                    else qDebug()<<"spinbox zuhoch";

              }
                  else{
                      m_ui->LambdaPlot->graph(43)->setVisible(false);
       //             m_ui->LambdaPlot->legend->item(43)->setVisible(false);
                      m_ui->sB_2->setValue(m_ui->sB_2->value()-1);
                      m_ui->LambdaPlot->replot();
            }
         }
     //45::::::::::::::::::::::::::::::::::::::::::::::::::::::
    else if ((Reihe== 3)&&(Spalte==6))
         {
      //    double u = fc4;

            if( (m_ui->chartsview->model()->index(Reihe,Spalte).data(Qt::CheckStateRole)==Qt::Checked)&&(m_ui->sB_2->value()<4))
            {m_ui->sB_2->setValue(m_ui->sB_2->value()+1);

                  m_ui->LambdaPlot->graph(44)->valueAxis()->setRange(0,100);

           //      m_ui->LambdaPlot->graph(44)->setName("fc4");
           //       m_ui->LambdaPlot->legend->item(44)->setVisible(true);

                 m_ui->LambdaPlot->graph(44)->setVisible(true);
                 m_ui->LambdaPlot->graph(44)->rescaleValueAxis(true);
                    m_ui->LambdaPlot->replot();

                    if (m_ui->sB_2->value()==1){
                                   m_ui->LambdaPlot->graph(0)->setPen(QPen(Qt::blue));
                                   m_ui->l_charts_1->setText("fc4");
                                   m_ui->b_charts_1->setText(QString::number(fc4));}
                    else if (m_ui->sB_2->value()==2){
                                   m_ui->LambdaPlot->graph(0)->setPen(QPen(Qt::yellow));
                                   m_ui->l_charts_2->setText("fc4");
                                   m_ui->b_charts_2->setText(QString::number(fc4));}
                   else if (m_ui->sB_2->value()==3){
                                   m_ui->LambdaPlot->graph(0)->setPen(QPen(Qt::red));
                                   m_ui->l_charts_3->setText("fc4");
                                   m_ui->b_charts_3->setText(QString::number(fc4));}
                   else if (m_ui->sB_2->value()==4){
                                    m_ui->LambdaPlot->graph(0)->setPen(QPen(Qt::green));
                                    m_ui->l_charts_4->setText("fc4");
                                    m_ui->b_charts_4->setText(QString::number(fc4));}
                    else qDebug()<<"spinbox zuhoch";
                    }
                  else{
                      m_ui->LambdaPlot->graph(44)->setVisible(false);
        //            m_ui->LambdaPlot->legend->item(44)->setVisible(false);
                      m_ui->sB_2->setValue(m_ui->sB_2->value()-1);
                      m_ui->LambdaPlot->replot();
             }
         }
    //46::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
    else if ((Reihe== 4)&&(Spalte==6))
         {
         // double u = uk11;

            if( (m_ui->chartsview->model()->index(Reihe,Spalte).data(Qt::CheckStateRole)==Qt::Checked)&&(m_ui->sB_2->value()<4))
                 {
            m_ui->sB_2->setValue(m_ui->sB_2->value()+1);

                  m_ui->LambdaPlot->graph(45)->valueAxis()->setRange(0,100);

             //    m_ui->LambdaPlot->graph(45)->setName("unknown 11");
            //      m_ui->LambdaPlot->legend->item(45)->setVisible(true);
                  m_ui->LambdaPlot->graph(45)->setVisible(true);
                  m_ui->LambdaPlot->graph(45)->rescaleValueAxis(true);
                    m_ui->LambdaPlot->replot();

                    if (m_ui->sB_2->value()==1){
                                   m_ui->LambdaPlot->graph(0)->setPen(QPen(Qt::blue));
                                   m_ui->l_charts_1->setText("uk11");
                                   m_ui->b_charts_1->setText(QString::number(uk11));}
                    else if (m_ui->sB_2->value()==2){
                                   m_ui->LambdaPlot->graph(0)->setPen(QPen(Qt::yellow));
                                   m_ui->l_charts_2->setText("uk11");
                                   m_ui->b_charts_2->setText(QString::number(uk11));}
                   else if (m_ui->sB_2->value()==3){
                                   m_ui->LambdaPlot->graph(0)->setPen(QPen(Qt::red));
                                   m_ui->l_charts_3->setText("uk11");
                                   m_ui->b_charts_3->setText(QString::number(uk11));}
                   else if (m_ui->sB_2->value()==4){
                                    m_ui->LambdaPlot->graph(0)->setPen(QPen(Qt::green));
                                    m_ui->l_charts_4->setText("uk11");
                                    m_ui->b_charts_4->setText(QString::number(uk11));}
                    else qDebug()<<"spinbox zuhoch";
                    }
                  else{
                      m_ui->LambdaPlot->graph(45)->setVisible(false);
            //        m_ui->LambdaPlot->legend->item(45)->setVisible(false);
                      m_ui->sB_2->setValue(m_ui->sB_2->value()-1);
                      m_ui->LambdaPlot->replot();
                }
         }
       //47:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
    else if ((Reihe== 5)&&(Spalte==6))
        {
        //  double u =uk12;

            if( (m_ui->chartsview->model()->index(Reihe,Spalte).data(Qt::CheckStateRole)==Qt::Checked)&&(m_ui->sB_2->value()<4))
                 {m_ui->sB_2->setValue(m_ui->sB_2->value()+1);


                  m_ui->LambdaPlot->graph(46)->valueAxis()->setRange(0,100);

            //     m_ui->LambdaPlot->graph(46)->setName("unknown 12");
            //      m_ui->LambdaPlot->legend->item(46)->setVisible(true);
                 m_ui->LambdaPlot->graph(46)->setVisible(true);
                 m_ui->LambdaPlot->graph(46)->rescaleValueAxis(true);
                    m_ui->LambdaPlot->replot();
                    if (m_ui->sB_2->value()==1){
                                   m_ui->LambdaPlot->graph(0)->setPen(QPen(Qt::blue));
                                   m_ui->l_charts_1->setText("uk12");
                                   m_ui->b_charts_1->setText(QString::number(uk12));}
                    else if (m_ui->sB_2->value()==2){
                                   m_ui->LambdaPlot->graph(0)->setPen(QPen(Qt::yellow));
                                   m_ui->l_charts_2->setText("uk12");
                                   m_ui->b_charts_2->setText(QString::number(uk12));}
                   else if (m_ui->sB_2->value()==3){
                                   m_ui->LambdaPlot->graph(0)->setPen(QPen(Qt::red));
                                   m_ui->l_charts_3->setText("uk12");
                                   m_ui->b_charts_3->setText(QString::number(uk12));}
                   else if (m_ui->sB_2->value()==4){
                                    m_ui->LambdaPlot->graph(0)->setPen(QPen(Qt::green));
                                    m_ui->l_charts_4->setText("uk12");
                                    m_ui->b_charts_4->setText(QString::number(uk12));}
                    else qDebug()<<"spinbox zuhoch";
               }
                  else{
                     m_ui->LambdaPlot->graph(46)->setVisible(false);
        //            m_ui->LambdaPlot->legend->item(46)->setVisible(false);
                     m_ui->sB_2->setValue(m_ui->sB_2->value()-1);
                     m_ui->LambdaPlot->replot();
              }
         }
        //48:::::::::::::::::::::::::::::::::::::::::::::::::::::
    else if ((Reihe== 6)&&(Spalte==6))
         {
       //   double u = uk13;

            if( (m_ui->chartsview->model()->index(Reihe,Spalte).data(Qt::CheckStateRole)==Qt::Checked)&&(m_ui->sB_2->value()<4))
                 {m_ui->sB_2->setValue(m_ui->sB_2->value()+1);


                  m_ui->LambdaPlot->graph(47)->valueAxis()->setRange(0,100);

           //      m_ui->LambdaPlot->graph(47)->setName("unknown13");
           //       m_ui->LambdaPlot->legend->item(47)->setVisible(true);
                   m_ui->LambdaPlot->graph(47)->setVisible(true);
                   m_ui->LambdaPlot->graph(47)->rescaleValueAxis(true);
                    m_ui->LambdaPlot->replot();
                    if (m_ui->sB_2->value()==1){
                                   m_ui->LambdaPlot->graph(0)->setPen(QPen(Qt::blue));
                                   m_ui->l_charts_1->setText("uk13");
                                   m_ui->b_charts_1->setText(QString::number(uk13));}
                    else if (m_ui->sB_2->value()==2){
                                   m_ui->LambdaPlot->graph(0)->setPen(QPen(Qt::yellow));
                                   m_ui->l_charts_2->setText("uk13");
                                   m_ui->b_charts_2->setText(QString::number(uk13));}
                   else if (m_ui->sB_2->value()==3){
                                   m_ui->LambdaPlot->graph(0)->setPen(QPen(Qt::red));
                                   m_ui->l_charts_3->setText("uk13");
                                   m_ui->b_charts_3->setText(QString::number(uk13));}
                   else if (m_ui->sB_2->value()==4){
                                    m_ui->LambdaPlot->graph(0)->setPen(QPen(Qt::green));
                                    m_ui->l_charts_4->setText("uk13");
                                    m_ui->b_charts_4->setText(QString::number(uk13));}
                    else qDebug()<<"spinbox zuhoch";

                    }
                  else{
                      m_ui->LambdaPlot->graph(47)->setVisible(false);
     //               m_ui->LambdaPlot->legend->item(47)->setVisible(false);
                      m_ui->sB_2->setValue(m_ui->sB_2->value()-1);
                      m_ui->LambdaPlot->replot();
                }
         }
     //49::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
    else if ((Reihe== 0)&&(Spalte==7))
         {
       //   double u = uk14;

            if( (m_ui->chartsview->model()->index(Reihe,Spalte).data(Qt::CheckStateRole)==Qt::Checked)&&(m_ui->sB_2->value()<4))
                     {m_ui->sB_2->setValue(m_ui->sB_2->value()+1);

                  m_ui->LambdaPlot->graph(48)->valueAxis()->setRange(0,100);

           //      m_ui->LambdaPlot->graph(48)->setName("unknown 14");
           //       m_ui->LambdaPlot->legend->item(48)->setVisible(true);
                 m_ui->LambdaPlot->graph(48)->setVisible(true);
                 m_ui->LambdaPlot->graph(48)->rescaleValueAxis(true);
                    m_ui->LambdaPlot->replot();

                    if (m_ui->sB_2->value()==1){
                                   m_ui->LambdaPlot->graph(0)->setPen(QPen(Qt::blue));
                                   m_ui->l_charts_1->setText("uk14");
                                   m_ui->b_charts_1->setText(QString::number(uk14));}
                    else if (m_ui->sB_2->value()==2){
                                   m_ui->LambdaPlot->graph(0)->setPen(QPen(Qt::yellow));
                                   m_ui->l_charts_2->setText("uk14");
                                   m_ui->b_charts_2->setText(QString::number(uk14));}
                   else if (m_ui->sB_2->value()==3){
                                   m_ui->LambdaPlot->graph(0)->setPen(QPen(Qt::red));
                                   m_ui->l_charts_3->setText("uk14");
                                   m_ui->b_charts_3->setText(QString::number(uk14));}
                   else if (m_ui->sB_2->value()==4){
                                    m_ui->LambdaPlot->graph(0)->setPen(QPen(Qt::green));
                                    m_ui->l_charts_4->setText("uk14");
                                    m_ui->b_charts_4->setText(QString::number(uk14));}
                    else qDebug()<<"spinbox zuhoch";
                    }
                  else{
                      m_ui->LambdaPlot->graph(48)->setVisible(false);
         //           m_ui->LambdaPlot->legend->item(48)->setVisible(false);
                      m_ui->sB_2->setValue(m_ui->sB_2->value()-1);
                      m_ui->LambdaPlot->replot();
                }
         }
     //50:::::::::::::::::::::::::::::::::::::::::::::::::::
    else if ((Reihe== 1)&&(Spalte==7))
         {
       //  double u =uk15;

            if( (m_ui->chartsview->model()->index(Reihe,Spalte).data(Qt::CheckStateRole)==Qt::Checked)&&(m_ui->sB_2->value()<4))
                 {m_ui->sB_2->setValue(m_ui->sB_2->value()+1);

                  m_ui->LambdaPlot->graph(49)->valueAxis()->setRange(0,100);

            //     m_ui->LambdaPlot->graph(49)->setName("unknown 15");
          //        m_ui->LambdaPlot->legend->item(49)->setVisible(true);

                 m_ui->LambdaPlot->graph(49)->setVisible(true);
                 m_ui->LambdaPlot->graph(49)->rescaleValueAxis(true);
                    m_ui->LambdaPlot->replot();
                    if (m_ui->sB_2->value()==1){
                                   m_ui->LambdaPlot->graph(0)->setPen(QPen(Qt::blue));
                                   m_ui->l_charts_1->setText("uk15");
                                   m_ui->b_charts_1->setText(QString::number(uk15));}
                    else if (m_ui->sB_2->value()==2){
                                   m_ui->LambdaPlot->graph(0)->setPen(QPen(Qt::yellow));
                                   m_ui->l_charts_2->setText("uk15");
                                   m_ui->b_charts_2->setText(QString::number(uk15));}
                   else if (m_ui->sB_2->value()==3){
                                   m_ui->LambdaPlot->graph(0)->setPen(QPen(Qt::red));
                                   m_ui->l_charts_3->setText("uk15");
                                   m_ui->b_charts_3->setText(QString::number(uk15));}
                   else if (m_ui->sB_2->value()==4){
                                    m_ui->LambdaPlot->graph(0)->setPen(QPen(Qt::green));
                                    m_ui->l_charts_4->setText("uk15");
                                    m_ui->b_charts_4->setText(QString::number(uk15));}
                    else qDebug()<<"spinbox zuhoch";

                    }
                  else{
                      m_ui->LambdaPlot->graph(49)->setVisible(false);
      //              m_ui->LambdaPlot->legend->item(49)->setVisible(false);
                      m_ui->sB_2->setValue(m_ui->sB_2->value()-1);
                      m_ui->LambdaPlot->replot();
                  }
         }
      //51:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
    else if ((Reihe== 2)&&(Spalte==7))
         {
      //    double u = uk16;
            if( (m_ui->chartsview->model()->index(Reihe,Spalte).data(Qt::CheckStateRole)==Qt::Checked)&&(m_ui->sB_2->value()<4))
                 {m_ui->sB_2->setValue(m_ui->sB_2->value()+1);


                  m_ui->LambdaPlot->graph(50)->valueAxis()->setRange(0,100);

          //       m_ui->LambdaPlot->graph(50)->setName("unknown 16");
           //       m_ui->LambdaPlot->legend->item(50)->setVisible(true);

                 m_ui->LambdaPlot->graph(50)->setVisible(true);
                 m_ui->LambdaPlot->graph(50)->rescaleValueAxis(true);
                    m_ui->LambdaPlot->replot();
                    if (m_ui->sB_2->value()==1){
                                   m_ui->LambdaPlot->graph(0)->setPen(QPen(Qt::blue));
                                   m_ui->l_charts_1->setText("uk16");
                                   m_ui->b_charts_1->setText(QString::number(uk16));}
                    else if (m_ui->sB_2->value()==2){
                                   m_ui->LambdaPlot->graph(0)->setPen(QPen(Qt::yellow));
                                   m_ui->l_charts_2->setText("uk16");
                                   m_ui->b_charts_2->setText(QString::number(uk16));}
                   else if (m_ui->sB_2->value()==3){
                                   m_ui->LambdaPlot->graph(0)->setPen(QPen(Qt::red));
                                   m_ui->l_charts_3->setText("uk16");
                                   m_ui->b_charts_3->setText(QString::number(uk16));}
                   else if (m_ui->sB_2->value()==4){
                                    m_ui->LambdaPlot->graph(0)->setPen(QPen(Qt::green));
                                    m_ui->l_charts_4->setText("uk16");
                                    m_ui->b_charts_4->setText(QString::number(uk16));}
                    else qDebug()<<"spinbox zuhoch";
            }
                  else{
                      m_ui->LambdaPlot->graph(50)->setVisible(false);
         //           m_ui->LambdaPlot->legend->item(50)->setVisible(false);
                      m_ui->sB_2->setValue(m_ui->sB_2->value()-1);
                      m_ui->LambdaPlot->replot();
                  }
         }
          //52::::::::::::::::::::::::::::::::::::::::::::::::::::::::
    else if ((Reihe== 3)&&(Spalte==7))
        {
        //  double u =uk17;
            if( (m_ui->chartsview->model()->index(Reihe,Spalte).data(Qt::CheckStateRole)==Qt::Checked)&&(m_ui->sB_2->value()<4))
                 {m_ui->sB_2->setValue(m_ui->sB_2->value()+1);


                  m_ui->LambdaPlot->graph(51)->valueAxis()->setRange(0,100);

      //           m_ui->LambdaPlot->graph(51)->setName("unknown 17");
      //            m_ui->LambdaPlot->legend->item(51)->setVisible(true);


              m_ui->LambdaPlot->graph(51)->setVisible(true);
              m_ui->LambdaPlot->graph(51)->rescaleValueAxis(true);
                 m_ui->LambdaPlot->replot();

                 if (m_ui->sB_2->value()==1){
                                m_ui->LambdaPlot->graph(0)->setPen(QPen(Qt::blue));
                                m_ui->l_charts_1->setText("uk17");
                                m_ui->b_charts_1->setText(QString::number(uk17));}
                 else if (m_ui->sB_2->value()==2){
                                m_ui->LambdaPlot->graph(0)->setPen(QPen(Qt::yellow));
                                m_ui->l_charts_2->setText("uk17");
                                m_ui->b_charts_2->setText(QString::number(uk17));}
                else if (m_ui->sB_2->value()==3){
                                m_ui->LambdaPlot->graph(0)->setPen(QPen(Qt::red));
                                m_ui->l_charts_3->setText("uk17");
                                m_ui->b_charts_3->setText(QString::number(uk17));}
                else if (m_ui->sB_2->value()==4){
                                 m_ui->LambdaPlot->graph(0)->setPen(QPen(Qt::green));
                                 m_ui->l_charts_4->setText("uk17");
                                 m_ui->b_charts_4->setText(QString::number(uk17));}
                 else qDebug()<<"spinbox zuhoch";
                 }
               else{
                   m_ui->LambdaPlot->graph(51)->setVisible(false);
       //          m_ui->LambdaPlot->legend->item(51)->setVisible(false);
                   m_ui->sB_2->setValue(m_ui->sB_2->value()-1);
                   m_ui->LambdaPlot->replot();
             }
      }

    //53::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
    else if ((Reihe== 4)&&(Spalte==7))
         {
      //    double u = uk18;
            if( (m_ui->chartsview->model()->index(Reihe,Spalte).data(Qt::CheckStateRole)==Qt::Checked)&&(m_ui->sB_2->value()<4))
            {m_ui->sB_2->setValue(m_ui->sB_2->value()+1);

                  m_ui->LambdaPlot->graph(52)->valueAxis()->setRange(0,100);

        //      m_ui->LambdaPlot->graph(52)->setName("unknown 18");
          //     m_ui->LambdaPlot->legend->item(52)->setVisible(true);

              m_ui->LambdaPlot->graph(52)->setVisible(true);
              m_ui->LambdaPlot->graph(52)->rescaleValueAxis(true);
              m_ui->LambdaPlot->replot();

              if (m_ui->sB_2->value()==1){
                             m_ui->LambdaPlot->graph(0)->setPen(QPen(Qt::blue));
                             m_ui->l_charts_1->setText("uk18");
                             m_ui->b_charts_1->setText(QString::number(uk18));}
              else if (m_ui->sB_2->value()==2){
                             m_ui->LambdaPlot->graph(0)->setPen(QPen(Qt::yellow));
                             m_ui->l_charts_2->setText("uk18");
                             m_ui->b_charts_2->setText(QString::number(uk18));}
             else if (m_ui->sB_2->value()==3){
                             m_ui->LambdaPlot->graph(0)->setPen(QPen(Qt::red));
                             m_ui->l_charts_3->setText("uk18");
                             m_ui->b_charts_3->setText(QString::number(uk18));}
             else if (m_ui->sB_2->value()==4){
                              m_ui->LambdaPlot->graph(0)->setPen(QPen(Qt::green));
                              m_ui->l_charts_4->setText("uk18");
                              m_ui->b_charts_4->setText(QString::number(uk18));}
              else qDebug()<<"spinbox zuhoch";

            }
            else{
                m_ui->LambdaPlot->graph(52)->setVisible(false);
        //      m_ui->LambdaPlot->legend->item(52)->setVisible(false);
                 m_ui->sB_2->setValue(m_ui->sB_2->value()-1);
                 m_ui->LambdaPlot->replot();

             }
         }

     //54::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
    else if ((Reihe== 5)&&(Spalte==7))
         {
      //    double u =uk19;
            if( (m_ui->chartsview->model()->index(Reihe,Spalte).data(Qt::CheckStateRole)==Qt::Checked)&&(m_ui->sB_2->value()<4))
             {m_ui->sB_2->setValue(m_ui->sB_2->value()+1);

                  m_ui->LambdaPlot->graph(53)->valueAxis()->setRange(0,100);

      //         m_ui->LambdaPlot->graph(53)->setName("jack count");
      //          m_ui->LambdaPlot->legend->item(53)->setVisible(true);

               m_ui->LambdaPlot->graph(53)->setVisible(true);
               m_ui->LambdaPlot->graph(53)->rescaleValueAxis(true);
               m_ui->LambdaPlot->replot();
               if (m_ui->sB_2->value()==1){
                              m_ui->LambdaPlot->graph(0)->setPen(QPen(Qt::blue));
                              m_ui->l_charts_1->setText("uk19");
                              m_ui->b_charts_1->setText(QString::number(uk19));}
               else if (m_ui->sB_2->value()==2){
                              m_ui->LambdaPlot->graph(0)->setPen(QPen(Qt::yellow));
                              m_ui->l_charts_2->setText("uk19");
                              m_ui->b_charts_2->setText(QString::number(uk19));}
              else if (m_ui->sB_2->value()==3){
                              m_ui->LambdaPlot->graph(0)->setPen(QPen(Qt::red));
                              m_ui->l_charts_3->setText("uk19");
                              m_ui->b_charts_3->setText(QString::number(uk19));}
              else if (m_ui->sB_2->value()==4){
                               m_ui->LambdaPlot->graph(0)->setPen(QPen(Qt::green));
                               m_ui->l_charts_4->setText("uk19");
                               m_ui->b_charts_4->setText(QString::number(uk19));}
               else qDebug()<<"spinbox zuhoch";

             }
             else{
                m_ui->LambdaPlot->graph(53)->setVisible(false);
            //   m_ui->LambdaPlot->legend->item(53)->setVisible(false);
                 m_ui->sB_2->setValue(m_ui->sB_2->value()-1);
                  m_ui->LambdaPlot->replot();
                 ;
              }
          }



    }

    void MainWindow::on_Refresh_button_clicked()
    {
          m_ui->LambdaPlot->graph(m_ui->sB_2->value())->valueAxis()->setRange(0,1000);
         m_ui->LambdaPlot->graph(m_ui->sB_2->value())->rescaleValueAxis(true);
          m_ui->LambdaPlot->replot();
    }



    void MainWindow::on_Refresh_log10_clicked()
    {
          m_ui->logplot->graph(m_ui->sB->value())->valueAxis()->setRange(0,10);

         m_ui->logplot->graph(m_ui->sB->value())->rescaleValueAxis(true);
         m_ui->logplot->replot();
    }

    void MainWindow::on_Refresh_log50_clicked()
    {
          m_ui->logplot->graph(m_ui->sB->value())->valueAxis()->setRange(0,50);
         m_ui->logplot->graph(m_ui->sB->value())->rescaleValueAxis(true);
          m_ui->logplot->replot();
    }

    void MainWindow::on_Refresh_log100_clicked()
    {
         m_ui->logplot->graph(m_ui->sB->value())->valueAxis()->setRange(0,100);
          m_ui->logplot->graph(m_ui->sB->value())->rescaleValueAxis(true);
           m_ui->logplot->replot();
    }
    void MainWindow::on_Refresh_log200_clicked()
    {
        m_ui->logplot->graph(m_ui->sB->value())->valueAxis()->setRange(0,200);
       m_ui->logplot->graph(m_ui->sB->value())->rescaleValueAxis(true);
        m_ui->logplot->replot();
    }

    void MainWindow::on_Refresh_log500_clicked()
    {
       m_ui->logplot->graph(m_ui->sB->value())->valueAxis()->setRange(0,500);
         m_ui->logplot->graph(m_ui->sB->value())->rescaleValueAxis(true);
          m_ui->logplot->replot();
    }

    void MainWindow::on_Refresh_log1000_clicked()
    {
          m_ui->logplot->graph(m_ui->sB->value())->valueAxis()->setRange(0,1000);
         m_ui->logplot->graph(m_ui->sB->value())->rescaleValueAxis(true);
          m_ui->logplot->replot();
    }


    void MainWindow::on_Refresh_log2000_clicked()
    {
        m_ui->logplot->graph(m_ui->sB->value())->valueAxis()->setRange(0,2000);
       m_ui->logplot->graph(m_ui->sB->value())->rescaleValueAxis(true);
        m_ui->logplot->replot();
    }

    void MainWindow::on_Refresh_log6000_clicked()
    {
        m_ui->logplot->graph(m_ui->sB->value())->valueAxis()->setRange(0,6000);
       m_ui->logplot->graph(m_ui->sB->value())->rescaleValueAxis(true);
        m_ui->logplot->replot();
    }
    // Größe der chart_x-Achse
     void MainWindow::on_Refresh_chartslog10_clicked()
    {
          m_ui->LambdaPlot->graph(m_ui->sB->value())->valueAxis()->setRange(0,10);

         m_ui->LambdaPlot->graph(m_ui->sB->value())->rescaleValueAxis(true);
         m_ui->LambdaPlot->replot();
    }

    void MainWindow::on_Refresh_chartslog50_clicked()
    {
          m_ui->LambdaPlot->graph(m_ui->sB->value())->valueAxis()->setRange(0,50);
         m_ui->LambdaPlot->graph(m_ui->sB->value())->rescaleValueAxis(true);
          m_ui->LambdaPlot->replot();
    }

    void MainWindow::on_Refresh_chartslog100_clicked()
    {
         m_ui->LambdaPlot->graph(m_ui->sB->value())->valueAxis()->setRange(0,100);
          m_ui->LambdaPlot->graph(m_ui->sB->value())->rescaleValueAxis(true);
           m_ui->LambdaPlot->replot();
    }
    void MainWindow::on_Refresh_chartslog200_clicked()
    {
        m_ui->LambdaPlot->graph(m_ui->sB->value())->valueAxis()->setRange(0,200);
       m_ui->LambdaPlot->graph(m_ui->sB->value())->rescaleValueAxis(true);
        m_ui->LambdaPlot->replot();
    }

    void MainWindow::on_Refresh_chartslog500_clicked()
    {
       m_ui->LambdaPlot->graph(m_ui->sB->value())->valueAxis()->setRange(0,500);
         m_ui->LambdaPlot->graph(m_ui->sB->value())->rescaleValueAxis(true);
          m_ui->LambdaPlot->replot();
    }

    void MainWindow::on_Refresh_chartslog1000_clicked()
    {
          m_ui->LambdaPlot->graph(m_ui->sB->value())->valueAxis()->setRange(0,1000);
         m_ui->LambdaPlot->graph(m_ui->sB->value())->rescaleValueAxis(true);
          m_ui->LambdaPlot->replot();
    }


    void MainWindow::on_Refresh_chartslog2000_clicked()
    {
        m_ui->LambdaPlot->graph(m_ui->sB->value())->valueAxis()->setRange(0,2000);
       m_ui->LambdaPlot->graph(m_ui->sB->value())->rescaleValueAxis(true);
        m_ui->LambdaPlot->replot();
    }

 //   void MainWindow::on_Refresh_chartslog6000_clicked()
 //   {
 //       m_ui->LambdaPlot->graph(m_ui->sB->value())->valueAxis()->setRange(0,6000);
 //      m_ui->LambdaPlot->graph(m_ui->sB->value())->rescaleValueAxis(true);
 //       m_ui->LambdaPlot->replot();
 //   }










    /*
     * 10.6.
     * csv wieder gelöscht
     *03.05.2020
     * readlog für ps-scan hinzugefügt
     * 13.04.2020
     * "about box" repariert und neu edidiert,
     * reset Vorgänge abgeändert und repariert
     * Mems save eingefügt
     * iac unter emi auf steps umgestelt
     * hot_idle eingefügt
     * */








    //resets::::::::::::::::::::::::::::::::::::::::::::::


    void MainWindow::on_m_resetadjustments_clicked()
    {
      emit mems_reset_ADJ();
    }

    void MainWindow::on_m_saveadjustments_clicked()
    {
         emit mems_Save() ;
    }


    void MainWindow::on_m_clearFaultsButton_clicked()
    {
        emit mems_clear_faults();
    }




void MainWindow::on_m_resetECUButton_clicked()
{
     emit mems_reset_ECU();
}
