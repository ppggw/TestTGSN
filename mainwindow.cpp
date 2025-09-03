#include "mainwindow.h"
#include "ui_mainwindow.h"


namespace{
    const int MessageDataPosInBytes = 4;
    const int MinPacketSizeBytes = 8;
    const int MaxPacketSizeBytes = 256;
    const uint16_t poly = 0xA001;
}


MainWindow::MainWindow(QWidget *parent): QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    MakeCRC16_Table();
    OpenSerialPort();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::MakeCRC16_Table(){
    for (int i = 0; i < 256; i++) {
        uint16_t crc = i;
        for (int j = 0; j < 8; j++) {
            if (crc & 1)
                crc = (crc >> 1) ^ poly;
            else
                crc >>= 1;
        }
        CRC16_Table[i] = crc;
    }
}


uint16_t MainWindow::ComputeCRC16_ControlSum(QByteArray message){
    uint16_t crc = 0xFFFF; // начальное значение

    for (uint8_t byte : message) {
        uint8_t idx = (crc ^ byte) & 0xFF;
        crc = (crc >> 8) ^ CRC16_Table[idx];
    }

    return crc;
}


void MainWindow::OpenSerialPort(){
    serial = new QSerialPort(this);
    serial->setBaudRate(QSerialPort::Baud115200);
    serial->setDataBits(QSerialPort::Data8);
    serial->setParity(QSerialPort::NoParity);
    serial->setStopBits(QSerialPort::OneStop);
    serial->setFlowControl(QSerialPort::NoFlowControl);

    foreach (const QSerialPortInfo &info, QSerialPortInfo::availablePorts())
    {
        if (info.portName() == "ttyUSB0")
        {
            serial->setPort(info);
        }
    }
    serial->open(QSerialPort::ReadWrite);

    connect(serial, SIGNAL(readyRead()), this, SLOT(ReadDataFromSlave()));
}


void MainWindow::on_pushButton_StartCaptureTarget_clicked()
{
    uint16_t CommandWord = 1;
    uint16_t ControlSum = 0;

    QByteArray message;
    QDataStream stream(&message, QIODevice::WriteOnly);
    stream.setByteOrder(QDataStream::LittleEndian);

    stream << CommandWord << ControlSum;

    serial->write(message);
}


void MainWindow::on_pushButton_StopCaptureTarget_clicked()
{
    uint16_t CommandWord = 2;
    uint16_t ControlSum = 0;

    QByteArray message;
    QDataStream stream(&message, QIODevice::WriteOnly);
    stream.setByteOrder(QDataStream::LittleEndian);

    stream << CommandWord << ControlSum;

    serial->write(message);
}


void MainWindow::on_pushButton_RequestLuminanceDetectorSettings_clicked()
{
    uint16_t CommandWord = 7;
    uint16_t ControlSum = 0;

    QByteArray message;
    QDataStream stream(&message, QIODevice::WriteOnly);
    stream.setByteOrder(QDataStream::LittleEndian);

    stream << CommandWord << ControlSum;

    serial->write(message);
}


void MainWindow::on_pushButton_SetLuminanceDetectorSettings_clicked()
{
    uint16_t CommandWord = 11;

    uint16_t Min = 200, Max = 250, Th = 100, MinSize = 5, MaxSize = 15;
    QByteArray settings;
    QDataStream streamSettings(&settings, QIODevice::WriteOnly);
    streamSettings.setByteOrder(QDataStream::LittleEndian);
    streamSettings << Min << Max << Th << MinSize << MaxSize;

    uint16_t ControlSum = ComputeCRC16_ControlSum(settings);

    QByteArray message;
    QDataStream stream(&message, QIODevice::WriteOnly);
    stream.setByteOrder(QDataStream::LittleEndian);

    stream << CommandWord << ControlSum;
    for(char& byte : settings){
        stream << (uchar)byte;
    }

    serial->write(message);
}


void MainWindow::on_pushButton_StartWriting_clicked()
{
    uint16_t CommandWord = 13;
    uint16_t ControlSum = 0;

    QByteArray message;
    QDataStream stream(&message, QIODevice::WriteOnly);
    stream.setByteOrder(QDataStream::LittleEndian);

    stream << CommandWord << ControlSum;

    serial->write(message);
}


void MainWindow::on_pushButton_StopWriting_clicked()
{
    uint16_t CommandWord = 14;
    uint16_t ControlSum = 0;

    QByteArray message;
    QDataStream stream(&message, QIODevice::WriteOnly);
    stream.setByteOrder(QDataStream::LittleEndian);

    stream << CommandWord << ControlSum;

    serial->write(message);
}



void MainWindow::on_pushButton_StartPlayingByName_clicked()
{
    uint16_t CommandWord = 15;

    QByteArray message;
    QDataStream stream(&message, QIODevice::WriteOnly);
    stream.setByteOrder(QDataStream::LittleEndian);
    stream << CommandWord;

    QString name = "1.mkv";
    QByteArray ByteName = name.toUtf8();
    std::reverse(ByteName.begin(), ByteName.end());
    uint16_t ControlSum = ComputeCRC16_ControlSum(ByteName);

    stream << ControlSum;
    for(char& byte : ByteName){
        stream << (uchar)byte;
    }
    stream << (uchar)0x00;

    serial->write(message);
}


void MainWindow::on_pushButton_StartPlayingByIndex_clicked()
{
    //если я правильно понял формирование этой посылки
    uint16_t CommandWord = 16;

    QByteArray message;
    QDataStream stream(&message, QIODevice::WriteOnly);
    stream.setByteOrder(QDataStream::LittleEndian);
    stream << CommandWord;

    QString name = "1";
    QByteArray ByteName = name.toUtf8();
    std::reverse(ByteName.begin(), ByteName.end());
    uint16_t ControlSum = ComputeCRC16_ControlSum(ByteName);

    stream << ControlSum;
    for(char& byte : ByteName){
        stream << (uchar)byte;
    }
    stream << (uchar)0x00;

    serial->write(message);
}


void MainWindow::on_pushButton_StopPlayingFiles_clicked()
{
    uint16_t CommandWord = 17;
    uint16_t ControlSum = 0;

    QByteArray message;
    QDataStream stream(&message, QIODevice::WriteOnly);
    stream.setByteOrder(QDataStream::LittleEndian);

    stream << CommandWord << ControlSum;

    serial->write(message);
}


void MainWindow::on_pushButton_RequesListOfFiles_clicked()
{
    uint16_t CommandWord = 18;
    uint16_t ControlSum = 0;

    QByteArray message;
    QDataStream stream(&message, QIODevice::WriteOnly);
    stream.setByteOrder(QDataStream::LittleEndian);

    stream << CommandWord << ControlSum;

    serial->write(message);
}


void MainWindow::on_pushButton_SetTime_clicked()
{
    uint16_t CommandWord = 19;

    QDateTime currentDateTime = QDateTime::currentDateTime();
    qint64 epochSeconds = currentDateTime.toSecsSinceEpoch();

    QByteArray seconds;
    QDataStream secondsStream(&seconds, QIODevice::WriteOnly);
    secondsStream.setByteOrder(QDataStream::LittleEndian);
    secondsStream << epochSeconds;

    uint16_t ControlSum = ComputeCRC16_ControlSum(seconds);

    QByteArray message;
    QDataStream stream(&message, QIODevice::WriteOnly);
    stream.setByteOrder(QDataStream::LittleEndian);

    stream << CommandWord << ControlSum << epochSeconds;

    serial->write(message);
}


void MainWindow::on_pushButton_manuallyRebootTGSN_clicked()
{
    uint16_t CommandWord = 20;

    QByteArray codeWords;
    codeWords.resize(5);
    codeWords[0] = 0xff;
    codeWords[1] = 0xaf;
    codeWords[2] = 0x04;
    codeWords[3] = 0xbc;
    codeWords[4] = 0xc8;

    uint16_t ControlSum = ComputeCRC16_ControlSum(codeWords);

    QByteArray message;
    QDataStream stream(&message, QIODevice::WriteOnly);
    stream.setByteOrder(QDataStream::LittleEndian);

    stream << CommandWord << ControlSum;
    for(char& byte : codeWords){
        stream << (uchar)byte;
    }

    serial->write(message);
}


void MainWindow::ReadDataFromSlave(){
    QByteArray buf = serial->readAll();

    if(!message.ReadComplete()){
       message.ba += buf;
    }

    if(message.ReadComplete()){
        qDebug() << message.ba.toHex();

//        QDataStream stream(message.ba);
//        stream.setByteOrder(QDataStream::LittleEndian);

//        uint8_t ByteOfStates, ByteOfCommandPurpose, buf;
//        uint16_t NumOfVideos, ControlSum;
//        stream >> ByteOfStates >> ByteOfCommandPurpose >> NumOfVideos;

//        QString name;
//        QByteArray ByteName{};
//        for(int i = 0; i != NumOfVideos; i++){
//            for(int j = 0; j != 5; j++){
//                stream >> buf;
//                ByteName += (uchar)buf;
//            }
//            qDebug() << ByteName.toHex();
//            std::reverse(ByteName.begin(), ByteName.end());
//            name = QString::fromUtf8(ByteName);
//            qDebug() << name;
//            if(i != (NumOfVideos-1)){ stream >> buf; }
//            ByteName.clear();
//        }
//        stream >> ControlSum;
//        qDebug() << ControlSum;

//        uint16_t buf;
//        int counter = 0;
//        while(counter != 6){
//            stream >> buf;
//            qDebug() << buf;
//            counter++;
//        }
//        qDebug() << "\n";

        message.ba.clear();
    }
}

