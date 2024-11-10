#include <QDir>
#include <QDateTime>
#include "logger.h"
#include "mainwindow.h"
/**
 * Constructor. Sets the interface class pointer as
 * well as log directory and log file extension.
 */
Logger::Logger(MEMSInterface* memsiface):
m_logExtension(".txt"), m_logDir("logs")
{
  m_mems = memsiface;
}
/*
 * gets EcuId

*/
bool Logger::EcuIdTransmitted(QString id)
{
    ecuid = id;
}

/**
 * Attempts to open a log file with the name specified.
 * @return True on success, false otherwise
 */
bool Logger::openLog(QString fileName)
{
  bool success = false;

  m_lastAttemptedLog = m_logDir + QDir::separator() + fileName + m_logExtension;

  // if the 'logs' directory exists, or if we're able to create it...
  if (!m_logFile.isOpen() && (QDir(m_logDir).exists() || QDir().mkdir(m_logDir)))
  {
    // set the name of the log file and open it for writing
    bool alreadyExists = QFileInfo(m_lastAttemptedLog).exists();

    m_logFile.setFileName(m_lastAttemptedLog);
    if (m_logFile.open(QFile::WriteOnly | QFile::Append))
    {
      m_logFileStream.setDevice(&m_logFile);

      if (!alreadyExists)
      {
         m_logFileStream <<"Ecu Id:"<< ecuid<<endl;
       m_logFileStream << "#time,"
                          "engine-rpm,"
                          "coolant_temp,"
                          "ambient_temp,"
                          "intake_air_temp,"
                          "fuel_temp,"
                          "map_kpa,"
                          "battery_voltage,"
                          "throttle_pot_voltage,"
                          "idle_switch,"
                          "uk1,"
                          "park_neutral_switch,"
                          "fault_codes,"
                          "idle_set_point,"
                          "idle_hot,"
                          "uk2,"
                          "iac_position,"
                          "idle_error,"
                          "ignition_advance_offset,"
                          "ignition_advance,"
                          "coil_time,"
                          "crancs,"
                          "uk4,"
                          "uk5,"
                          "ignition_switch,"
                          "throttle_angle,"
                          "uk6,"
                          "air_fuel_ratio,"
                          "fault_code0,"
                          "lambda_voltage_mv,"
                          "lambda_sensor_frequency,"
                          "lambda_sensor_dutycycle,"
                          "lambda_sensor_status,"
                          "closed_loop,"
                          "long_term_fuel_trim,"
                          "short_term_fuel_trim,"
                          "carbon_canister_dutycycle,"
                          "fault_code1,"
                          "idle_base_pos,"
                          "uk7,"
                          "uk8,"
                          "ignition_advance2,"
                          "uk9,"
                          "idle_error2,"
                          "uk10,"
                          "fault_code4,"
                          "uk11,"
                          "uk12,"
                          "uk13,"
                          "uk14,"
                          "uk15,"
                          "uk16,"
                          "uk17,"
                          "uk18,"
                          "uk19"<< endl;
    }
      success = true;
    }
  }

  return success;
}

/**
 * Close the log file.
 */
void Logger::closeLog()
{
  m_logFile.close();
}

/**
 * Converts degrees F to degrees C if necessary
 */
uint8_t Logger::convertTemp(uint8_t degrees)
{
  if (m_tempUnits == Celsius)
  {
    return ((degrees - 32) * 0.555556);
  }
  else
  {
    return degrees;
  }
}


/**
 * Commands the logger to query the interface for the currently
 * buffered data, and write it to the file.
 */
void Logger::logData()
{
  mems_data* data = m_mems->getData();

  if (m_logFile.isOpen() && (m_logFileStream.status() == QTextStream::Ok))
      {
    
      m_logFileStream << QDateTime::currentDateTime().toString("hh:mm:ss") << "," <<
      data->engine_rpm << "," <<
      convertTemp(data->coolant_temp_f) << "," <<
      convertTemp(data->ambient_temp_f)<< "," <<
      convertTemp(data->intake_air_temp_f)<< "," <<
      convertTemp(data->fuel_temp_f)<< "," <<
      data->map_kpa<< "," <<
      data->battery_voltage<< "," <<
      data->throttle_pot_voltage<< "," <<
      data->idle_switch<< "," <<
      data->uk1<< "," <<
      data->park_neutral_switch<< "," <<
      data->fault_codes<< "," <<
      data->idle_set_point<< "," <<
      data->idle_hot<< "," <<
      data->uk2<< "," <<
      data->iac_position<< "," <<
      data->idle_error<< "," <<
      data->ignition_advance_offset<< "," <<
      data->ignition_advance<< "," <<
      data->coil_time<< "," <<
      data->crancs<< "," <<
      data->uk4<< "," <<
      data->uk5<< "," <<
      data->ignition_switch<< "," <<
      data->throttle_angle<< "," <<
      data->uk6<< "," <<
      data->air_fuel_ratio<< "," <<
      data->fault_code0<< "," <<
      data->lambda_voltage_mv<< "," <<
      data->lambda_sensor_frequency<< "," <<
      data->lambda_sensor_dutycycle<< "," <<
      data->lambda_sensor_status<< "," <<
      data->closed_loop<< "," <<
      data->long_term_fuel_trim<< "," <<
      data->short_term_fuel_trim<< "," <<
      data->carbon_canister_dutycycle<< "," <<
      data->fault_code1<< "," <<
      data->idle_base_pos<< "," <<
      data->uk7<< "," <<
      data->uk8<< "," <<
      data->ignition_advance2<< "," <<
      data->uk9<< "," <<
      data->idle_error2<< "," <<
      data->uk10<< "," <<
      data->fault_code4<< "," <<
      data->uk11<< "," <<
      data->uk12<< "," <<
      data->uk13<< "," <<
      data->uk14<< "," <<
      data->uk15<< "," <<
      data->uk16<< "," <<
      data->uk1A<< "," <<
      data->uk1B<< "," <<
      data->uk1C<< endl;
    }
}
/**
 * Returns the full path to the last log that we attempted to open.
 * @return Full path to last log file
 */
   QString Logger::getLogPath()
 {
   return m_lastAttemptedLog;
 }
