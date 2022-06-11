#ifndef MAINWINDOW_H
#define MAINWINDOW_H


#include <iostream>
#include <QList>

#include <QWidget>
#include <QImage>
#include <QPixmap>
#include <QTimer>
#include <QDebug>
#include <QString>
#include <QDateTime>
#include <QtNetwork/QtNetwork>
#include <QtNetwork/QTcpServer>
#include <QtNetwork/QTcpSocket>
#include <QException>
#include <QMessageBox>
#include <QtSerialPort/QSerialPort>
#include <QCoreApplication>

#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

#include <QMainWindow>


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

public slots:
    void openCamera();
    void readFrameSendData(); // read from camera send to TCP server
    void closeCamera();
    void recordVideo();
    void connectServer();
    void disconnectServer();

    /*------------GPS---------------*/
    void connectGpsServer();
    void disconnectGpsServer();
    void serialReadSendData(); // GPS
    void gpsParse(QByteArray GPSBuffer); // GPS


    /*------------GPS---------------*/


    //------------Camera Settings----------------
    void SetCameraBrightness(int value);
    void SetCameraSaturation(int value);
    void SetCameraHue(int value);

private:
    Ui::MainWindow *ui;
    QTimer *timer;
    QImage qimage;
    cv::VideoCapture capture;
    cv::Mat cvframe;
    cv::VideoWriter writer;
    QTcpSocket* tcpSocket;
    QTcpSocket* tcpSocketGps; // Socket gps data
    int count;
    QString serverIp;
    int serverPort;
    int serverPortGps; // GPS
    QSerialPort *serial;
};

#endif // MAINWINDOW_H
