/* Homepage         http://facetracknoir.sourceforge.net/home/default.htm        *
 *                                                                               *
 * ISC License (ISC)                                                             *
 *                                                                               *
 * Copyright (c) 2015, Wim Vriend                                                *
 *                                                                               *
 * Permission to use, copy, modify, and/or distribute this software for any      *
 * purpose with or without fee is hereby granted, provided that the above        *
 * copyright notice and this permission notice appear in all copies.             *
 */

#include "serial_tracker_dialog.h"
#include "api/plugin-api.hpp"
#include "serial_tracker.h"
#include "serial_tracker_dialog.h"

#include <QScrollBar>

serial_tracker_dialog::serial_tracker_dialog() : theTracker(nullptr)
{
    ui.setupUi(this);

    // make SerialPort list
    ui.cbSerialPort->clear();
    for (const QSerialPortInfo& port_info : QSerialPortInfo::availablePorts() ) {
        ui.cbSerialPort->addItem(port_info.portName());
    }

    // Serial config
    ui.QCB_Serial_baudRate->clear();
    ui.QCB_Serial_baudRate->addItem(QLatin1String("9600"),QSerialPort::Baud9600);
    ui.QCB_Serial_baudRate->addItem(QLatin1String("19200"),QSerialPort::Baud19200);
    ui.QCB_Serial_baudRate->addItem(QLatin1String("38400"),QSerialPort::Baud38400);
    ui.QCB_Serial_baudRate->addItem(QLatin1String("57600"),QSerialPort:: Baud57600);
    ui.QCB_Serial_baudRate->addItem(QLatin1String("115200"),QSerialPort::Baud115200);

    ui.QCB_Serial_dataBits->clear();
    ui.QCB_Serial_dataBits->addItem(QLatin1String("5"), QSerialPort::Data5);
    ui.QCB_Serial_dataBits->addItem(QLatin1String("6"), QSerialPort::Data6);
    ui.QCB_Serial_dataBits->addItem(QLatin1String("7"), QSerialPort::Data7);
    ui.QCB_Serial_dataBits->addItem(QLatin1String("8"), QSerialPort::Data8);

    ui.QCB_Serial_parity->clear();
    ui.QCB_Serial_parity->addItem(QLatin1String("None"), QSerialPort::NoParity);
    ui.QCB_Serial_parity->addItem(QLatin1String("Even"), QSerialPort::EvenParity);
    ui.QCB_Serial_parity->addItem(QLatin1String("Odd"), QSerialPort::OddParity);
    ui.QCB_Serial_parity->addItem(QLatin1String("Space"), QSerialPort::SpaceParity);
    ui.QCB_Serial_parity->addItem(QLatin1String("Mark"), QSerialPort::MarkParity);

    ui.QCB_Serial_stopBits->clear();
    ui.QCB_Serial_stopBits->addItem(QLatin1String("1"), QSerialPort::OneStop);
    ui.QCB_Serial_stopBits->addItem(QLatin1String("1.5"), QSerialPort::OneAndHalfStop);
    ui.QCB_Serial_stopBits->addItem(QLatin1String("2"), QSerialPort::TwoStop);

    ui.QCB_Serial_flowControl->clear();
    ui.QCB_Serial_flowControl->addItem(QLatin1String("None"), QSerialPort::NoFlowControl);
    ui.QCB_Serial_flowControl->addItem(QLatin1String("RTS/CTS"), QSerialPort::HardwareControl);
    ui.QCB_Serial_flowControl->addItem(QLatin1String("XON/XOFF"), QSerialPort::SoftwareControl);

    tie_setting(s.QSerialPortName, ui.cbSerialPort);
    tie_setting(s.pBaudRate, ui.QCB_Serial_baudRate);
    tie_setting(s.pDataBits, ui.QCB_Serial_dataBits);
    tie_setting(s.pFlowControl, ui.QCB_Serial_flowControl);
    tie_setting(s.pParity, ui.QCB_Serial_parity);
    tie_setting(s.pStopBits, ui.QCB_Serial_stopBits);

    connect(ui.buttonBox, SIGNAL(accepted()), this, SLOT(doOK()));
    connect(ui.buttonBox, SIGNAL(rejected()), this, SLOT(doCancel()));
}

serial_tracker_dialog::~serial_tracker_dialog() = default;

void serial_tracker_dialog::Initialize(QWidget *parent)
{
    QPoint offsetpos(100, 100);
    if (parent)
        move(parent->pos() + offsetpos);
    show();
}

void serial_tracker_dialog::doOK()
{
    s.b->save();
    close();
}

void serial_tracker_dialog::doCancel()
{
    close();
}

void serial_tracker_dialog::register_tracker(ITracker *tracker)
{
    theTracker = static_cast<serial_tracker*>(tracker);
    ui.parametersBox->setEnabled(false);
}

void serial_tracker_dialog::unregister_tracker()
{
    theTracker = nullptr;
    ui.parametersBox->setEnabled(true);
}
