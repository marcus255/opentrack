/* Copyright (c) 2014, Stanislaw Halik <sthalik@misaki.pl>

 * Permission to use, copy, modify, and/or distribute this
 * software for any purpose with or without fee is hereby granted,
 * provided that the above copyright notice and this permission
 * notice appear in all copies.
 */

#include "serial_tracker.h"
#include "serial_tracker_dialog.h"
#include "api/plugin-api.hpp"
#include "compat/math-imports.hpp"

#include <QPushButton>
#include <QStringList>
#include <QTextStream>

#include <cmath>
#include <QDebug>

serial_tracker::serial_tracker() = default;
serial_tracker::~serial_tracker() = default;

module_status serial_tracker::start_tracker(QFrame*)
{
    t.Log("Starting Tracker with data format 'yaw,pitch,x,y,z,roll\n'");
    serial_result ret = t.init_serial_port();
    t.start();

    timer.start();
    return {};
}

void serial_tracker::data(double *data)
{
    QString strData = t.getData();
    bool dataValid = false;
    double numbers[6] = {};
    if (!strData.isEmpty()) {
        QStringList elems = strData.split(',');
        if (elems.size() == 6) {
            dataValid = true;
            for (int i = 0; i < 6; i++)
            {
                bool ok;
                numbers[i] = elems[i].toDouble(&ok);
                if (!ok) {
                    qWarning() << "Format error at" << i << "element:" << strData;
                    dataValid = false;
                    break;
                }
            }
        }
        else {
            qWarning() << "Expected 6 elements, got:" << elems.size() << "from line:" << strData;
        }
    }

    if (dataValid) {
        for (int i = 0; i < 6; i++)
        {
            last[i] = numbers[i];
        }
        qDebug() << QString("Using data: %1 %2 %3 %4 %5 %6")
                .arg(last[0]).arg(last[1]).arg(last[2])
                .arg(last[3]).arg(last[4]).arg(last[5]);
    }
    else if (!strData.isEmpty())
    {
        qWarning() << "Correct data format is \"x,y,z,yaw,pitch,roll\\n\", values can be floats or integers";
    }

    for (int i = 0; i < 6; i++)
    {
        data[i] = last[i];
    }
}

OPENTRACK_DECLARE_TRACKER(serial_tracker, serial_tracker_dialog, serial_metadata)
