#pragma once

#include <QSerialPort>
#include <QThread>
#include <QMutex>

#include <QFile>
#include <QList>
#include <QCoreApplication>

enum results
{
    result_ok,
    result_open_error,
    result_error,
};


struct serial_result
{
    serial_result() : code(result_ok) {}
    serial_result(results code, const QString& error) : error(error), code(code) {}

    QString error;
    results code { result_error };
};

class serial_thread : public QThread
{
    Q_OBJECT
    using serial_t = QSerialPort;

    QByteArray data_read;
    QString current_data;
    serial_t com_port;
    QStringList values;
    char buf[1024];

    void run() override;

private slots:
    void on_serial_read();
    void teardown_serial();

    serial_result init_serial_port_impl();

signals:
    serial_result init_serial_port();

public:
    void start();
    QString getData();
    ~serial_thread() override;
    serial_thread();

    void Log(const QString& message);

    QMutex data_mtx;
};
