#pragma once

#include "api/plugin-api.hpp"
#include "compat/timer.hpp"
#include "serial_tracker_settings.h"
#include "serial_tracker.h"
#include "ui_serial_tracker_controls.h"
#include <QObject>
#include <QWidget>
#include <QTimer>
#include <QMessageBox>
#include <QMetaType>

class serial_tracker_dialog: public ITrackerDialog
{
    Q_OBJECT
public:
    explicit serial_tracker_dialog();
    ~serial_tracker_dialog() override;
    void Initialize(QWidget *parent) ; // unused
    void register_tracker(ITracker *tracker) override;
    void unregister_tracker() override;

private:
    Ui::UISerialControls ui;
    serial_tracker *theTracker;
    Timer last_time;

public  slots:
    void doOK();
    void doCancel();

protected:
    TrackerSettings s;
};
