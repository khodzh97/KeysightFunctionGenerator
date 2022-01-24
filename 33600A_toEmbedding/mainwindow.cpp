#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

QString MainWindow::returnIP()
{
    return ui->ip->text();
}

int MainWindow::returnPort()
{
    return ui->port->text().toInt();
}

void MainWindow::receiveSocketState(QString state, bool socketState)
{
    ui->log->setText(QTime::currentTime().toString("hh:mm:ss ") + state);
    socketStatus = socketState;
}

void MainWindow::receiveParameters(QMap<QString, QString> parameters)
{
    // QString::number(parameters.value("power").toDouble(), 'f') - костыль для перевода
    // представления числа из научной нотации в фиксированную
    ui->receivedPower->setText(QString::number(parameters.value("power").toDouble(), 'd'));
    ui->receivedWaveform->setText(parameters.value("func"));
    ui->receivedFrequency->setText(QString::number(parameters.value("frequency").toDouble(), 'f'));
    if(parameters.value("output") == "true")
    {
        ui->outputButton->setText("Output off");
//        qDebug() << parameters.value("output");
    }
    else if(parameters.value("output") == "false")
    {
        ui->outputButton->setText("Output on");
//        qDebug() << parameters.value("output");
    }
}

void MainWindow::on_setButton_clicked()
{
    emit passSetParameters(ui->waveformBox->currentText(), ui->frequencyBox->currentText() ,
                           ui->unitBox->currentText(), ui->freq->text().toInt(), ui->power->text().toFloat());
}

void MainWindow::on_sendButton_clicked()
{
    passParcelToSend(ui->sendEdit->text());
    ui->sendEdit->clear();
}

void MainWindow::on_outputButton_clicked()
{
    if(ui->outputButton->text() == "Output on")
        emit setOutput(true);
    else if (ui->outputButton->text() == "Output off")
    {
        emit setOutput(false);
    }
}
