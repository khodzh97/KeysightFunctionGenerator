#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTime>
#include <QDebug>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    QString returnIP();
    int returnPort();

public slots:

    void receiveSocketState(QString state, bool socketState);

    void receiveParameters(QMap <QString, QString> parameters);

private slots:
    void on_setButton_clicked();

    void on_sendButton_clicked();

    void on_outputButton_clicked();

private:
    Ui::MainWindow *ui;

    bool socketStatus;

signals:

    void passSetParameters(QString waveform, QString frequencyUnit, QString unit, int frequency, float power);

    void passParcelToSend(QString parcelToSend);

    void setOutput(bool state);
};
#endif // MAINWINDOW_H
