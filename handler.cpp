#include "handler.h"

#include <QTimer>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QDebug>

#include "com/indeema/QPredix/qpredix.h"

#include <QDebug>
#include <QJsonDocument>

Handler::Handler(QObject *parent) : QObject(parent)
{
    initPredix();

    //get all connected devices to USB
    QList<QSerialPortInfo> lList = QSerialPortInfo::availablePorts();
    QSerialPortInfo lInfo;

    //find if there is connected arduino device
    foreach (QSerialPortInfo info, lList) {
        if (info.manufacturer().contains("Arduino")) {
            // first of all intialize setial port to be able read data
            lInfo = info;
            mSerialPort = new QSerialPort;
            mSerialPort->setPort(lInfo);
            mSerialPort->setDataBits(QSerialPort::Data8);
            mSerialPort->setFlowControl(QSerialPort::NoFlowControl);
            mSerialPort->setParity(QSerialPort::NoParity);
            mSerialPort->setStopBits(QSerialPort::OneStop);
            mSerialPort->setBaudRate(QSerialPort::Baud57600);
            //connect serialport to readyread to read data received from serial port
            connect(mSerialPort, &QSerialPort::readyRead, this, &Handler::readyRead);

            // start timer that will start reading each 2 seconds
            mTimer = new QTimer(this);
            mTimer->setInterval(2000);
            mTimer->setSingleShot(false);
            connect(mTimer, &QTimer::timeout, this, &Handler::startReading);
            mTimer->start();

            //leave constructor after initialisation
            return;
        }
    }
}

Handler::~Handler()
{
    //stop and delete mTimer to clear memmory
    if (mTimer->isActive()) {
        mTimer->stop();
    }

    delete mTimer;

    //close and delete mSerialPort to clear memmory
    if (mSerialPort->isOpen()) {
        mSerialPort->close();
    }

    delete mSerialPort;
}

void Handler::readyRead()
{
    // read received data from serial port
    QByteArray lArray = mSerialPort->readAll();
    //all temperature values are devided with new line \n, so let's separate all temperatures by this symbol
    QStringList lList = QString(lArray).split("\n");

    //delete first and last symbol as it could be corrupted
    if (lList.count() > 1) {
        lList.removeFirst();
        lList.removeLast();
    } else {
        lList.clear();
    }

    //convert all values to data and store in global list to be able use it in different method
    foreach (QString value, lList) {
        mDataList.append(value.toDouble());
    }

    // check if we have enough values to send to cloud
    if (mDataList.count() > 10) {
        mSerialPort->close();
        sendDataToCloud();
    }
}

void Handler::startReading()
{
    //clear global list with vlaues and open port to start reading data
    mDataList.clear();
    if (!mSerialPort->open(QIODevice::ReadOnly)) {
        qDebug() << "Couldn't start device";
    }
}

void Handler::sendDataToCloud()
{
    //let's find the evarage value of temperature
    double lTmp = 0;
    foreach (double value, mDataList) {
        lTmp += value;
    }

    lTmp = lTmp/mDataList.count();
    // after we have avarage temperature we can clear global temp list
    mDataList.clear();

    qDebug() << lTmp;

    // let's add attributes and send data to cloud
    QMap<QString, QString> lAtt;
    lAtt.insert("city", "Lviv");
    mTimeSeries->sendData("temp", lTmp, 3, lAtt);
}

void Handler::initPredix()
{
    // initialising Predix UAA service to have access to cloud
    QUaaInfo lInfo;
    lInfo.setUaaUrl("Your Uaa URI");
    lInfo.setClientCredential("login", "Secret");

    QUaaUserInfo lUser;
    lUser.setLogin("login");
    lUser.setPassword("secret");

    mUaa = new QUaa(lInfo);
    mUaa->loginWithCredentials(lUser);

    //set timeseries zineid to get access to data storage
    mTimeSeries = new QTimeSeries(mUaa);
    mTimeSeries->setZoneID("zoneId");
}
