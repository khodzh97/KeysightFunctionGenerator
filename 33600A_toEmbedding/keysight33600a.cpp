#include "keysight33600a.h"

Keysight33600A::Keysight33600A(QObject *parent) : QObject(parent)
{
    sock = new QTcpSocket(this);

    connect(sock, SIGNAL(connected()), SLOT(connection()));
    connect(sock, SIGNAL(disconnected()), SLOT(disconnection()));
    connect(sock, SIGNAL(readyRead()), SLOT(readyRead()));
    connect(sock, SIGNAL(error(QAbstractSocket::SocketError)),
            this, SLOT(error(QAbstractSocket::SocketError)));


    connect(&connectionTimer, SIGNAL(timeout()), SLOT(reconnect()));

    connect(this, SIGNAL(continueRequest()), SLOT(cycleRequest()));
}

void Keysight33600A::parse(QByteArray parcelToParse)
{
    QStringList strList = QString(parcelToParse).split("\r\n");

    if(strList.size() > 1)
    {
        switch (requestIterator) {
        case 0:
        {
            parameters.insert("frequency", strList[0]);
            break;
        }
        case 1:
        {
            parameters.insert("func", strList[0]);
            break;
        }
        case 2:
        {
            if(strList[0] == "0")
                parameters.insert("output", "false");
            else
                parameters.insert("output", "true");
            break;
        }
        case 3:
        {
            parameters.insert("power", strList[0]);
            break;
        }
            break;
        }
    }

    emit continueRequest();
//    qDebug() << Q_FUNC_INFO << parameters;
    if(!parameters.empty())
        emit passParameters(parameters);
}

void Keysight33600A::readyRead()
{
    parcel = sock->readAll().trimmed();
    parse(parcel);
}

void Keysight33600A::trySetConnection(QString IP, int port)
{
    localIP = IP;
    localPort = port;

    if(!sock->isOpen()){
        sock->connectToHost(IP, port);
        if(!sock->waitForConnected(300))
        {
            emit passSocketState("Ошибка! Соединение не произошло.");
            sock->close();

            connectionTimer.start(5000);
        }
        else
        {
            emit passSocketState("Есть соединение", true);
//            pingFailureCount = 0;
        }
    }
    else
    {
        sock->disconnectFromHost();
        emit passSocketState("Нет соединения");

        sock->close();
    }
}

void Keysight33600A::setParameters(QString waveform, QString frequencyUnit, QString unit, int frequency, float power)
{
    sock->write(QString("FUNC " + QString(waveform) + "\r").toUtf8());

    QTime itsTimeToStop = QTime::currentTime().addMSecs(1500);
    while (QTime::currentTime() < itsTimeToStop)
    {
        QCoreApplication::processEvents(QEventLoop::AllEvents);
    }

    sock->write(QString("VOLT:UNIT " + QString(unit) + "\r").toUtf8());

    itsTimeToStop = QTime::currentTime().addMSecs(1500);
    while (QTime::currentTime() < itsTimeToStop)
    {
        QCoreApplication::processEvents(QEventLoop::AllEvents);
    }

    sock->write(QString("FREQ " + QString::number(frequency) + " " + frequencyUnit + "\r").toUtf8());

    itsTimeToStop = QTime::currentTime().addMSecs(1500);
    while (QTime::currentTime() < itsTimeToStop)
    {
        QCoreApplication::processEvents(QEventLoop::AllEvents);
    }

    sock->write(QString("VOLT " + QString::number(power) + "\r").toUtf8());

    itsTimeToStop = QTime::currentTime().addMSecs(1500);
    while (QTime::currentTime() < itsTimeToStop)
    {
        QCoreApplication::processEvents(QEventLoop::AllEvents);
    }

//    setParameters(waveform, frequencyUnit, unit, frequency, power);
}

void Keysight33600A::send(QString parcelToSend)
{
    sock->write(QString(parcelToSend + "\r").toUtf8());
    qDebug() << QString(parcelToSend + "\r");
}

void Keysight33600A::manageOutput(bool state)
{
    if(state == true)
        sock->write(QString("output on\r").toUtf8());
    else if(state == false)
        sock->write(QString("output off\r").toUtf8());
}

void Keysight33600A::error(QAbstractSocket::SocketError err)
{
    qDebug() << err;
    if(err == QAbstractSocket::HostNotFoundError)
        emit passSocketState("HostNotFoundError");
    if(err == QAbstractSocket::RemoteHostClosedError)
        emit passSocketState("RemoteHostClosedError");
    if(err == QAbstractSocket::ConnectionRefusedError)
        emit passSocketState("ConnectionRefusedError");
}

void Keysight33600A::connection()
{
    emit continueRequest();
}

void Keysight33600A::disconnection()
{
    sock->close();

    emit passSocketState("Произошло отключение");

    QTime itsTimeToStop = QTime::currentTime().addMSecs(5000);
    while (QTime::currentTime() < itsTimeToStop)
    {
        QCoreApplication::processEvents(QEventLoop::AllEvents);
    }

    reconnect();
}

void Keysight33600A::reconnect()
{
    if(!sock->isOpen()){
        sock->connectToHost(localIP, localPort);
        if(!sock->waitForConnected(300))
        {
            emit passSocketState("Ошибка! Соединение не произошло.");
            sock->close();

            QTime itsTimeToStop = QTime::currentTime().addMSecs(5000);
            while (QTime::currentTime() < itsTimeToStop)
            {
                QCoreApplication::processEvents(QEventLoop::AllEvents);
            }
            reconnect();
        }
        else
        {
            emit passSocketState("Есть соединение", true);
//            pingFailureCount = 0;
        }
    }
}

void Keysight33600A::cycleRequest()
{
    if(sock->isOpen())
    {
        QString prc;

        // Отсрочка отправки посылки
        QTime itsTimeToStop = QTime::currentTime().addMSecs(1000);
        while (QTime::currentTime() < itsTimeToStop)
        {
            QCoreApplication::processEvents(QEventLoop::AllEvents);
        }

        switch (requestIterator) {
        case 0:
        {
            prc = "output?\r\n";
            requestIterator++;
            break;
        }
        case 1:
        {
            prc = "volt?\r\n";
            requestIterator++;
            break;
        }
        case 2:
        {
            prc = "freq?\r\n";
            requestIterator++;
            break;
        }
        case 3:
        {
            prc = "func?\r\n";
            requestIterator = 0;
            break;
        }
            break;
        }

        sock->write(prc.toUtf8());
    }
}
