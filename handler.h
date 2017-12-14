#ifndef HANDLER_H
#define HANDLER_H

#include <QObject>
#include <QList>

class QTimer;
class QSerialPort;
class QUaa;
class QTimeSeries;

class Handler : public QObject
{
    Q_OBJECT
public:
    explicit Handler(QObject *parent = 0);
    ~Handler();

private slots:
    void readyRead();
    void startReading();

private:
    void sendDataToCloud();
    void initPredix();

private:
    QList<double> mDataList;
    QTimer *mTimer;
    QSerialPort *mSerialPort;

    QUaa *mUaa;
    QTimeSeries *mTimeSeries;
};

#endif // HANDLER_H
