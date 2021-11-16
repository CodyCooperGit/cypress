#include "TanitaManager.h"

//#include "../../data/BodyCompositionAnalyzerTest.h"

#include <QDateTime>
#include <QDebug>
#include <QJsonArray>
#include <QJsonObject>
#include <QSerialPortInfo>
#include <QSettings>
#include <QtMath>

TanitaManager::TanitaManager(QObject *parent) : SerialPortManager(parent)
{
}

void TanitaManager::buildModel(QStandardItemModel* model) const
{
    /*
    for(int row = 0; row < m_test.getMaximumNumberOfMeasurements(); row++)
    {
        QString s = "NA";
        WeightMeasurement m = m_test.getMeasurement(row);
        if(m.isValid())
           s = m.toString();
        QStandardItem* item = model->item(row,0);
        item->setData(s, Qt::DisplayRole);
    }
    */
}

void TanitaManager::clearData()
{
    m_deviceData.reset();
    m_test.reset();

    emit dataChanged();
}

bool TanitaManager::hasEndCode(const QByteArray &arr)
{
    if( arr.size() < 2 ) return false;
    // try and interpret the last 2 bytes
    int size = arr.size();
    return (
       0x0d == arr.at(size-2) && //\r
       0x0a == arr.at(size-1) ); //\n
}

void TanitaManager::connectDevice()
{
    if("simulate" == m_mode)
    {
        m_request = QByteArray("i");
        writeDevice();
        return;
    }

    if(m_port.open(QSerialPort::ReadWrite))
    {
      m_port.setDataBits(QSerialPort::Data8);
      m_port.setParity(QSerialPort::NoParity);
      m_port.setStopBits(QSerialPort::OneStop);
      m_port.setBaudRate(QSerialPort::Baud4800);

      connect(&m_port, &QSerialPort::readyRead,
               this, &TanitaManager::readDevice);

      connect(&m_port, &QSerialPort::errorOccurred,
              this,[this](QSerialPort::SerialPortError error){
                  Q_UNUSED(error)
                  qDebug() << "AN ERROR OCCURED: " << m_port.errorString();
              });

      connect(&m_port, &QSerialPort::dataTerminalReadyChanged,
              this,[](bool set){
          qDebug() << "data terminal ready DTR changed to " << (set?"high":"low");
      });

      connect(&m_port, &QSerialPort::requestToSendChanged,
              this,[](bool set){
          qDebug() << "request to send RTS changed to " << (set?"high":"low");
      });

      // try and read the scale ID, if we can do that then emit the
      // canMeasure signal
      // the canMeasure signal is emitted from readDevice slot on successful read
      //
      m_request = QByteArray("i");
      writeDevice();
    }
}

void TanitaManager::zeroDevice()
{
    m_request = QByteArray("z");
    writeDevice();
}

void TanitaManager::measure()
{
    m_request = QByteArray("p");
    writeDevice();
}

void TanitaManager::readDevice()
{
    if("simulate" == m_mode)
    {
        QString simdata;
        if("i" == QString(m_request))
         {
           simdata = "12345";
         }
         else if("z" == QString(m_request))
         {
           simdata = "0.0 C body";
         }
         else if("p" == QString(m_request))
         {
            simdata = "36.1 C body";
         }
         m_buffer = QByteArray(simdata.toUtf8());
    }
    else
    {
      QByteArray data = m_port.readAll();
      m_buffer += data;
    }
    if(m_verbose)
      qDebug() << "read device received buffer " << QString(m_buffer);

    if(!m_buffer.isEmpty())
    {
      if("i" == QString(m_request))
      {
        m_deviceData.setCharacteristic("software ID", QString(m_buffer.simplified()));
        emit canMeasure();
      }
      else if("p" == QString(m_request))
      {
         m_test.fromArray(m_buffer);
         if(m_test.isValid())
         {
             // emit the can write signal
             emit canWrite();
         }
      }
      else if("z" == QString(m_request))
      {
          /*
          BodyCompositionMeasurement m;
          m.fromArray(m_buffer);
          if(m.isZero())
            emit canMeasure();
            */
      }

      emit dataChanged();
    }
}

QJsonObject TanitaManager::toJsonObject() const
{
    QJsonObject json = m_test.toJsonObject();
    json.insert("device",m_deviceData.toJsonObject());
    return json;
}