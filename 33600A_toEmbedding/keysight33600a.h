#ifndef KEYSIGHT33600A_H
#define KEYSIGHT33600A_H

#include <QObject>
#include <QTcpSocket>
#include <QCoreApplication>
#include <QTimer>
#include <QTime>
#include <QDebug>

class Keysight33600A : public QObject
{
    Q_OBJECT
public:
    explicit Keysight33600A(QObject *parent = nullptr);

private:

    QTcpSocket *sock;
    QByteArray parcel;
    QTimer connectionTimer;

    QString localIP;
    int localPort;

    int pingFailureCount = 0;

    int requestIterator = 0;
    // 0 - output
    // 1 - power
    // 2 - frequency
    // 3 - waveform

    QMap <QString, QString> parameters;

    void parse(QByteArray parcelToParse);



public slots:

    void trySetConnection(QString IP, int port);

    void setParameters(QString waveform, QString frequencyUnit, QString unit, int frequency, float power);

    void send(QString parcelToSend);

    void manageOutput(bool state);

private slots:

    void error(QAbstractSocket::SocketError err);

    void connection();

    void disconnection();

    void reconnect();

    void cycleRequest();

    void readyRead();

signals:

    void passSocketState(QString state, bool socketState = false);

    void passParameters(QMap <QString, QString> parameters);

    void continueRequest();

};

#endif // KEYSIGHT33600A_H
