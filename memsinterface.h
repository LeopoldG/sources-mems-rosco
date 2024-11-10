#ifndef MEMSINTERFACE_H
#define MEMSINTERFACE_H

#include <QObject>
#include <QString>
#include <QHash>
#include <QByteArray>
#include <QHash>
#include "rosco.h"
#include "commonunits.h"

class MEMSInterface : public QObject
{
    Q_OBJECT
public:
    explicit MEMSInterface(QString device, QObject *parent = 0);
    ~MEMSInterface();

    void setSerialDevice(QString device) { m_deviceName = device; }

    QString getSerialDevice() { return m_deviceName; }
    int getIntervalMsecs();

    bool isConnected();
    void disconnectFromECU();

    mems_data* getData()          { return &m_data; }
    librosco_version getVersion() { return mems_get_lib_version(); }

    void cancelRead();

public slots:
    void onParentThreadStarted();
    void onStartPollingRequest();
    void onShutdownThreadRequest();
    void onFuelPumpTest();
    void onPTCRelayTest();
    void onACRelayTest();
    void onIgnitionCoilTest();
    void onFuelInjectorTest();
    void onIdleAirControlMovementRequest(int desiredPos);
    void on_m_fuel_trim_plusButton_clicked();
    void on_m_fuel_trim_minusButton_clicked();
    void on_m_idle_decay_plusButton_clicked();
    void on_m_idle_decay_minusButton_clicked();
    void on_m_idle_speed_plusButton_clicked();
    void on_m_idle_speed_minusButton_clicked();
    void on_m_ignition_advance_plusButton_clicked();
    void on_m_ignition_advance_minusButton_clicked();
    void on_m_Purge_Valve_OnButton_clicked();
    void on_m_O2Heater_OnButton_clicked();
    void on_m_O2Heater_OffButton_clicked();
    void on_m_Boost_Valve_OnButton_clicked();
    void on_m_Fan1_OnButton_clicked();
    void on_m_Fan2_OnButton_clicked();
    void onFaultCodesClearRequested();
    void on_m_resetadjustments_clicked();
    void on_m_resetECUButton_clicked();
    void on_EmiResetRequested();
    void on_m_saveadjustments_clicked();

signals:
    void dataReady();
    void connected();
    void disconnected();
    void readError();
    void readSuccess();
    void faultCodesClearComplete();
    void adjustmentsResetComplete();
    void EcuResetComplete();
    void EmiResetComplete();
    void EmiSaveComplete();
    void failedToConnect(QString dev);
    void interfaceThreadReady();
    void notConnected();
    void gotEcuId(uint8_t* id_buffer);
    void errorSendingCommand();
    void fuelPumpTestComplete();
    void ptcRelayTestComplete();
    void acRelayTestComplete();
    void moveIACComplete();
    void fuel_trim_plus();
    void fuel_trim_minus();
    void idle_decay_plus();
    void idle_decay_minus();
    void idle_speed_plus();
    void idle_speed_minus();
    void ignition_advance_plus();
    void ignition_advance_minus();

private:
    mems_data m_data;
    QString m_deviceName;
    mems_info m_memsinfo;
    bool m_stopPolling;
    bool m_shutdownThread;
    bool m_initComplete;
    bool m_serviceLoopRunning;
    uint8_t m_d0_response_buffer[4];

    void runServiceLoop();
    bool connectToECU();
    bool actuatorOnOffDelayTest(actuator_cmd onCmd, actuator_cmd offCmd);
   bool writeinteractive();
};

#endif // CUXINTERFACE_H
