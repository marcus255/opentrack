#include "thread.hpp"
#include "compat/base-path.hpp"
#include "compat/sleep.hpp"

#include <utility>
#include <cstring>

#include <QTextStream>
#include <QTime>
#include <QByteArray>

#include <QDebug>

#define MAX_DATA_LINE_SIZE 100


void serial_thread::Log(const QString& message)
{
    qDebug() << message;
}

void serial_thread::start()
{
    QThread::start();
}

serial_thread::~serial_thread()
{
    quit();
    wait();
}

serial_thread::serial_thread()
{
    connect(this, &QThread::finished, this, &serial_thread::teardown_serial, Qt::DirectConnection);
    connect(this, &serial_thread::init_serial_port, this, &serial_thread::init_serial_port_impl, Qt::QueuedConnection);

    connect(&com_port, &serial_t::readyRead, this, &serial_thread::on_serial_read, Qt::DirectConnection);
}

void serial_thread::teardown_serial()
{
    if (isRunning() && com_port.isOpen())
    {
        Log("Tracker shut down");
        com_port.close();
    }
}

void serial_thread::run()
{
    (void) exec();
}

serial_result serial_thread::init_serial_port_impl()
{
    Log(tr("Setting serial port name"));
    const QString serialPortName = s.QSerialPortName;
    qDebug() << "Port name:" << serialPortName;
    com_port.setPortName(serialPortName);

    Log(tr("Opening serial port"));
    if (com_port.open(QIODevice::ReadOnly))
    {
        Log(tr("Port Open"));
        if (com_port.setBaudRate((QSerialPort::BaudRate)s.pBaudRate)
                && com_port.setDataBits((QSerialPort::DataBits)s.pDataBits)
                && com_port.setParity((QSerialPort::Parity)s.pParity)
                && com_port.setStopBits((QSerialPort::StopBits)s.pStopBits)
                && com_port.setFlowControl((QSerialPort::FlowControl)s.pFlowControl)
                && com_port.clear(QSerialPort::AllDirections))
        {
            Log(tr("Port Parameters set"));
            qDebug() << com_port.portName()
                     << com_port.baudRate()
                     << com_port.dataBits()
                     << com_port.parity()
                     << com_port.stopBits()
                     << com_port.flowControl();
            return serial_result();
        }
        else
        {
            Log(tr("Failed to set port parameters"));
            qDebug() << com_port.errorString();
            return serial_result(result_error, com_port.errorString());
        }
    }
    else
    {
        Log(tr("Failed to open serial port"));
        qDebug() << com_port.errorString();
        return serial_result(result_open_error, com_port.errorString());
    }
}

QString serial_thread::getData()
{
    data_mtx.lock();
    QString retStr = "";
    if (!values.isEmpty())
        retStr = values.takeFirst();
    data_mtx.unlock();
    return retStr;
}

void serial_thread::on_serial_read()
{
    uint64_t bytes = com_port.readLine(buf, sizeof(buf));
    if (bytes > 0)
    {
        if (buf[bytes - 1] == '\n') {
            buf[bytes - 1] = '\0';
            current_data.append(buf);
            data_mtx.lock();
            values.append(current_data);
            data_mtx.unlock();
            current_data.clear();
        }
        else {
            buf[bytes] = '\0';
            current_data.append(buf);
            if (current_data.size() >= MAX_DATA_LINE_SIZE)
            {
                qWarning() << "Invalid data format. Got" << current_data.size()
                           << "bytes of data but no newline char (\\n). Discarding data."
                           << "Correct data format is \"x,y,z,yaw,pitch,roll\\n\", values can be floats or integers";
                current_data.clear();
            }
        }
    }
}
