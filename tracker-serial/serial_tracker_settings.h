/* Copyright (c) 2012 Patrick Ruoff
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 */

#pragma once

#include <QSerialPort>
#include "options/options.hpp"

using namespace options;

struct TrackerSettings : opts
{
    value<QString> QSerialPortName;

    value<QSerialPort::BaudRate> pBaudRate;
    value<QSerialPort::DataBits> pDataBits;
    value<QSerialPort::Parity> pParity;
    value<QSerialPort::StopBits> pStopBits;
    value<QSerialPort::FlowControl> pFlowControl;

    TrackerSettings() :
        opts("serial-tracker"),
        QSerialPortName(b, "serial-port-name", ""),
        pBaudRate(b, "baud-rate", QSerialPort::Baud115200),
        pDataBits(b, "data-bits", QSerialPort::Data8),
        pParity(b, "parity", QSerialPort::NoParity),
        pStopBits(b, "stop-bits", QSerialPort::OneStop),
        pFlowControl(b, "flow-control", QSerialPort::HardwareControl)
    {}
};
