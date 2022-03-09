/* Copyright (c) 2014, Stanislaw Halik <sthalik@misaki.pl>

 * Permission to use, copy, modify, and/or distribute this
 * software for any purpose with or without fee is hereby granted,
 * provided that the above copyright notice and this permission
 * notice appear in all copies.
 */

#include "serial.h"
#include "serialportreader.h"
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
    t.Log("Starting Tracker");
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
            for (int i = 0; i < 6; i++)
            {
                bool ok;
                numbers[i] = elems[i].toDouble(&ok);
                if (!ok) {
                    qDebug() << "   FORMAT error: " << elems[i];
                    break;
                }
            }
            dataValid = true;
        }
        else {
            qDebug() << "Expected 6 elements, got: " << elems.size() << " from " << strData;
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

    for (int i = 0; i < 6; i++)
    {
        data[i] = last[i];
    }
}

test_dialog::test_dialog() // NOLINT(cppcoreguidelines-pro-type-member-init)
{
    ui.setupUi(this);

    connect(ui.buttonBox, &QDialogButtonBox::clicked, [this](QAbstractButton* btn) {
        if (btn == ui.buttonBox->button(QDialogButtonBox::Abort))
            *(volatile int*)nullptr /*NOLINT*/ = 0;
    });

    connect(ui.buttonBox, SIGNAL(accepted()), this, SLOT(doOK()));
    connect(ui.buttonBox, SIGNAL(rejected()), this, SLOT(doCancel()));
}

void test_dialog::doOK()
{
    //s.b->save();
    close();
}

void test_dialog::doCancel()
{
    close();
}

OPENTRACK_DECLARE_TRACKER(serial_tracker, test_dialog, test_metadata)
