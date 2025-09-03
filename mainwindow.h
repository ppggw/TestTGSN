#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtSerialPort>
#include <QDebug>
#include <QByteArray>
#include <QDateTime>


QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE





struct ByfferForReceive{
    int TrueSizeOfMessage = 0;
    QByteArray ba{};

    ByfferForReceive(int TrueSizeOfMessage_):TrueSizeOfMessage(TrueSizeOfMessage_){ba.resize(0);}

    bool ReadComplete(){ return ba.size() == TrueSizeOfMessage; }
};


class MainWindow : public QMainWindow
{
    Q_OBJECT

private:
    QSerialPort *serial;
    uint16_t CRC16_Table[256];

    ByfferForReceive message{9};

    void OpenSerialPort();
    void MakeCRC16_Table();
    uint16_t ComputeCRC16_ControlSum(QByteArray);
    bool ReadAndCompareCRC16_ControlSum(QByteArray, uint16_t);

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_pushButton_StartCaptureTarget_clicked();

    void on_pushButton_StopCaptureTarget_clicked();

    void on_pushButton_RequestLuminanceDetectorSettings_clicked();

    void on_pushButton_SetLuminanceDetectorSettings_clicked();

    void on_pushButton_StartWriting_clicked();

    void on_pushButton_StopWriting_clicked();

    void on_pushButton_StartPlayingByIndex_clicked();

    void on_pushButton_StartPlayingByName_clicked();

    void on_pushButton_StopPlayingFiles_clicked();

    void on_pushButton_RequesListOfFiles_clicked();

    void on_pushButton_SetTime_clicked();

    void on_pushButton_manuallyRebootTGSN_clicked();

    void ReadDataFromSlave();

private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
