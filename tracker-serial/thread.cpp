#include "thread.hpp"
#include "compat/base-path.hpp"
#include "compat/sleep.hpp"

#include <utility>
#include <cstring>

#include <QTextStream>
#include <QTime>
#include <QByteArray>

#include <QDebug>


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
    const QString serialPortName = "COM2";
    com_port.setPortName(serialPortName);

    Log(tr("Opening serial port"));
    if (com_port.open(QIODevice::ReadWrite))
    {
        Log(tr("Port Open"));
        if (
            com_port.setBaudRate(QSerialPort::Baud9600)
            && com_port.setDataBits(QSerialPort::Data8)
            && com_port.setParity(QSerialPort::NoParity)
            && com_port.setStopBits(QSerialPort::OneStop)
//            && com_port.setFlowControl(QSerialPort::HardwareControl)
            && com_port.clear(QSerialPort::AllDirections)
           )
        {
            Log(tr("Port Parameters set"));
            qDebug()  << QTime::currentTime()
                      << " HAT OPEN on"
                      << com_port.portName()
                      << com_port.baudRate()
                      << com_port.dataBits()
                      << com_port.parity()
                      << com_port.stopBits()
                      << com_port.flowControl();
            return serial_result();
        }
        else
        {
            return serial_result(result_error, com_port.errorString());
        }
    }
    else
        return serial_result(result_open_error, com_port.errorString());
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
    const int sz = (int)com_port.readLine(buf, sizeof(buf));
//    qDebug() << QString("Got %1 bytes").arg(sz);
    if (sz > 0)
    {
        if (buf[sz - 1] == '\n') {
            buf[sz - 1] = '\0';
//            qDebug() << "Appending " << strlen(buf);
            current_data.append(buf);
            qDebug() << "data ready: " << current_data;
            data_mtx.lock();
            values.append(current_data);
            data_mtx.unlock();
            current_data.clear();
        }
        else if (sz < 1024){
            buf[sz] = '\0';
            current_data.append(buf);
        }
    }


    if (sz <= 0)
    {
        // qt can fire QSerialPort::readyRead() needlessly, causing a busy loop.
        // see https://github.com/opentrack/opentrack/issues/327#issuecomment-207941003

        // this probably happens with flaky BT/usb-to-serial converters (?)
//        constexpr int hz = 90;
//        constexpr int ms = 1000/hz;
//        portable::sleep(ms);
    }
}
