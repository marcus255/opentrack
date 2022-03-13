#pragma once
#include "ui_serial_tracker_controls.h"
#include "thread.hpp"
#include "api/plugin-api.hpp"
#include "compat/timer.hpp"
#include "compat/macros.hpp"
#include <QSerialPort>
#include <QSerialPortInfo>

#include <cmath>

class serial_tracker : public ITracker
{
public:
    serial_tracker();
    ~serial_tracker() override;
    module_status start_tracker(QFrame *) override;
    void data(double *data) override;

private:
    double last[6] {};
    Timer timer;
    serial_thread t;
    QSerialPort serialPort;
};

class serial_metadata : public Metadata
{
    Q_OBJECT

    QString name() override { return tr("Serial tracker"); }
    QIcon icon() override { return QIcon(":/images/opentrack.png"); }
};

