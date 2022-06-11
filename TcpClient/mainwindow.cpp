#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <stdio.h>
#include <stdlib.h>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setWindowTitle("TcpClient");

    timer = new QTimer(this);
    tcpSocket = new QTcpSocket(this); // Video
    tcpSocketGps = new QTcpSocket(this); // GPS socket
    serial = new QSerialPort(this); // GPS

    //buttons -> functions
    connect(timer, SIGNAL(timeout()), this, SLOT(readFrameSendData()));
    connect(ui->btn_start, SIGNAL(clicked()),this, SLOT(openCamera()));
    connect(ui->btn_stop, SIGNAL(clicked()), this, SLOT(closeCamera()));
    connect(ui->btn_record, SIGNAL(clicked()), this, SLOT(recordVideo()));
    connect(ui->btn_connect, SIGNAL(clicked()), this, SLOT(connectServer()));
    connect(ui->btn_disconnect, SIGNAL(clicked()), this, SLOT(disconnectServer()));

    //camera/image settings

    //--Brightness------
    connect(ui->horizontalSlidersbrightnessMin, SIGNAL(valueChanged(int)), ui->spinBox_brightnessMin, SLOT(setValue(int)));
    connect(ui->horizontalSlidersbrightnessMin, SIGNAL(valueChanged(int)), this, SLOT(SetCameraBrightness(int)));
    connect(ui->spinBox_brightnessMin, SIGNAL(valueChanged(int)), ui->horizontalSlidersbrightnessMin, SLOT(setValue(int)));
    connect(ui->spinBox_brightnessMin, SIGNAL(valueChanged(int)), this, SLOT(SetCameraBrightness(int)));
    connect(ui->horizontalSlidersbrightnessMax, SIGNAL(valueChanged(int)), ui->spinBox_brightnessMax, SLOT(setValue(int)));
    connect(ui->horizontalSlidersbrightnessMax, SIGNAL(valueChanged(int)), this, SLOT(SetCameraBrightness(int)));
    connect(ui->spinBox_brightnessMax, SIGNAL(valueChanged(int)), ui->horizontalSlidersbrightnessMax, SLOT(setValue(int)));
    connect(ui->spinBox_brightnessMax, SIGNAL(valueChanged(int)), this, SLOT(SetCameraBrightness(int)));
    //--Brightness------

    //--Saturation------
    connect(ui->horizontalSliderssaturationMin, SIGNAL(valueChanged(int)), ui->spinBox_saturationMin, SLOT(setValue(int)));
    connect(ui->horizontalSliderssaturationMin, SIGNAL(valueChanged(int)), this, SLOT(SetCameraSaturation(int)));
    connect(ui->spinBox_saturationMin, SIGNAL(valueChanged(int)), ui->horizontalSliderssaturationMin, SLOT(setValue(int)));
    connect(ui->spinBox_saturationMin, SIGNAL(valueChanged(int)), this, SLOT(SetCameraSaturation(int)));
    connect(ui->horizontalSliderssaturationMax, SIGNAL(valueChanged(int)), ui->spinBox_saturationMax, SLOT(setValue(int)));
    connect(ui->horizontalSliderssaturationMax, SIGNAL(valueChanged(int)), this, SLOT(SetCameraSaturation(int)));
    connect(ui->spinBox_saturationMax, SIGNAL(valueChanged(int)), ui->horizontalSliderssaturationMax, SLOT(setValue(int)));
    connect(ui->spinBox_saturationMax, SIGNAL(valueChanged(int)), this, SLOT(SetCameraSaturation(int)));
    //--Saturation------

    //--HUE------
    connect(ui->horizontalSlidershueMin, SIGNAL(valueChanged(int)), ui->spinBox_hueMin, SLOT(setValue(int)));
    connect(ui->horizontalSlidershueMin, SIGNAL(valueChanged(int)), this, SLOT(SetCameraHue(int)));
    connect(ui->spinBox_hueMin, SIGNAL(valueChanged(int)), ui->horizontalSlidershueMin, SLOT(setValue(int)));
    connect(ui->spinBox_hueMin, SIGNAL(valueChanged(int)), this, SLOT(SetCameraHue(int)));
    connect(ui->horizontalSlidershueMax, SIGNAL(valueChanged(int)), ui->spinBox_hueMax, SLOT(setValue(int)));
    connect(ui->horizontalSlidershueMax, SIGNAL(valueChanged(int)), this, SLOT(SetCameraHue(int)));
    connect(ui->spinBox_hueMax, SIGNAL(valueChanged(int)), ui->horizontalSlidershueMax, SLOT(setValue(int)));
    connect(ui->spinBox_hueMax, SIGNAL(valueChanged(int)), this, SLOT(SetCameraHue(int)));
    //--HUE------

    //---set values----------
    ui->spinBox_brightnessMin->setValue(0);
    ui->spinBox_brightnessMax->setValue(255);
    ui->spinBox_saturationMin->setValue(0);
    ui->spinBox_saturationMax->setValue(255);
    ui->spinBox_hueMin->setValue(0);
    ui->spinBox_hueMax->setValue(255);

    /*------------GPS---------------*/
    connect(ui->btn_connectgps, SIGNAL(clicked()), this, SLOT(connectGpsServer()));
    connect(ui->btn_disconnectgps, SIGNAL(clicked()), this, SLOT(disconnectGpsServer()));
    /*------------GPS---------------*/


    // init serial port ----------------------------------------- GPS
    connect(serial, SIGNAL(readyRead()), this, SLOT(serialReadSendData()));
    serial->setPortName("/dev/ttyACM0"); //
    serial->setBaudRate(QSerialPort::Baud9600);
    serial->setDataBits(QSerialPort::Data8);
    serial->setParity(QSerialPort::NoParity);
    serial->setStopBits(QSerialPort::OneStop);
    serial->setFlowControl(QSerialPort::NoFlowControl);
    //serial->open(QIODevice::ReadWrite);
    //serial->write("Working");
    // init serial port ----------------------------------------- GPS

}


MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::connectServer()
{
    qDebug()<<"btn connect clicked!";
    serverIp = ui->lineEdit_ip->text();
    serverPort = ui->lineEdit_port->text().toInt();
    qDebug()<<"ip: "<< serverIp << ", port " << serverPort;

    if(tcpSocket->state() == QTcpSocket::ConnectedState)
    {
        QMessageBox::information(this, "Warning","Video Server(" + serverIp + ": "+ QString::number(serverPort) + ") already connected!");
        return;
    }

    try
    {
        tcpSocket->abort();
        tcpSocket->connectToHost(serverIp, serverPort);

        if(tcpSocket->waitForConnected(5000))
        {
           qDebug() << "Video Server connect successful!";
        }
        else
        {
           qDebug()<<"Video Server connect failed!";
           QMessageBox::information(this, "Warning","Video Server(" + serverIp + ": "+ QString::number(serverPort) + ") connect failed!");
        }
        count = 0;
    }
    catch(QException &e)
    {
        qDebug()<<"exception: "<< e.what();
    }
}

/*------------GPS---------------*/
void MainWindow::connectGpsServer()
{
    qDebug()<<"btn connect clicked!";
    serverIp = ui->lineEdit_ip->text();
    serverPortGps = ui->lineEdit_portgps->text().toInt();
    qDebug()<<"ip: "<< serverIp << ", port " << serverPortGps;

    if(tcpSocketGps->state() == QTcpSocket::ConnectedState)
    {
        QMessageBox::information(this, "Warning","GPS Server(" + serverIp + ": "+ QString::number(serverPortGps) + ") already connected!");
        return;
    }

    try
    {
        tcpSocketGps->abort();
        tcpSocketGps->connectToHost(serverIp, serverPortGps);

        if(tcpSocketGps->waitForConnected(3000))
        {
           qDebug() << "GPS Server connect successful!";
        }
        else
        {
           qDebug()<<"GPS Server connect failed!";
           QMessageBox::information(this, "Warning","GPS Server(" + serverIp + ": "+ QString::number(serverPortGps) + ") connect failed!");
        }
        count = 0;
    }
    catch(QException &e)
    {
        qDebug()<<"exception: "<< e.what();
    }
}
/*------------GPS---------------*/

void MainWindow::disconnectServer()
{
    if(tcpSocket->state() == QTcpSocket::UnconnectedState || tcpSocket->state() == QTcpSocket::ClosingState)
    {
        QMessageBox::information(this, "Warning","Video Server already disconnected!");
    }
    else
    {
        tcpSocket->close();
    }
}

/*------------GPS---------------*/
void MainWindow::disconnectGpsServer()
{
    if(tcpSocketGps->state() == QTcpSocket::UnconnectedState || tcpSocketGps->state() == QTcpSocket::ClosingState)
    {
        QMessageBox::information(this, "Warning","GPS Server already disconnected!");
    }
    else
    {
        tcpSocketGps->close();
    }
}
/*------------GPS---------------*/

void MainWindow::readFrameSendData()
{
    capture >> cvframe;
    cv::cvtColor(cvframe, cvframe, cv::COLOR_BGR2RGB);
    qimage = QImage((const uchar*)(cvframe.data), cvframe.cols, cvframe.rows, QImage::Format_RGB888);
    ui->textLabel->setPixmap(QPixmap::fromImage(qimage));
    ui->textLabel->resize(qimage.size());

    //запис във файл
    if(writer.isOpened())
    {
        writer << cvframe;
    }

    if(tcpSocket->state() == QTcpSocket::ConnectedState && count == 0)
    {
        count = 1;
        //qDebug() << QTime::currentTime().toString("hh:mm:ss.zzz") << ": Server connected, sending data...";
        QByteArray byte;
        QBuffer buf(&byte);
        qimage.save(&buf,"JPEG");
        QByteArray compressByteArray = qCompress(byte,1);
        QByteArray encodedByteArray = compressByteArray.toBase64();

        QByteArray sendByteArray;
        QDataStream outStream(&sendByteArray,QIODevice::WriteOnly);
        outStream.setVersion(QDataStream::Qt_5_11);

        quint64 dataSize = (quint64)(encodedByteArray.size());
        outStream << dataSize;
        outStream << encodedByteArray;
        tcpSocket->write(sendByteArray);
        tcpSocket->flush();

        count = 0;
    }
    else if(count == 1)
    {
        qDebug()<<"Video Count still remain at 1, ignore send data!";
    }
}

void MainWindow::openCamera()
{
    if(!capture.isOpened())
    {
        capture = cv::VideoCapture(0);
        serial->open(QIODevice::ReadWrite); // + Open GPS Data Read
    }
    else
    {
        qDebug()<<"Capture already opened!";
    }

    timer->start(30);
}

void MainWindow::closeCamera()
{
    timer->stop();
    serial->close(); // + Close GPS Data Read

    if(capture.isOpened())
    {
        capture.release();
    }
    else
    {
        qDebug()<<"Capture already released!";
    }

    if(writer.isOpened())
    {
        writer.release();
        qDebug()<<"Writer released!";
        ui->btn_record->setText("Record Video");
    }

}

//-----------VIDEO RECORD--------------------
void MainWindow::recordVideo()
{
    if(!writer.isOpened())
    {
        openCamera();
        cv::Size size = cv::Size(capture.get(cv::CAP_PROP_FRAME_WIDTH), capture.get(cv::CAP_PROP_FRAME_HEIGHT));

        QString targetVideoPath = qApp->applicationDirPath();
        QString targetVideoName = "video_"+QDateTime::currentDateTime().toString("dd_MM_yyyy_hh_mm_ss")+".avi";
        QString targetVideo = targetVideoPath + "/" + targetVideoName;
        qDebug()<<"Save Video to: " + targetVideo;
        writer.open(targetVideo.toStdString(), cv::VideoWriter::fourcc('M', 'J', 'P', 'G'), 20, size, true);
        qDebug()<<"Writer opened";
        ui->btn_record->setText("Stop Record Video");
    }
    else
    {
        writer.release();
        qDebug()<<"Writer released!";
        ui->btn_record->setText("Record Video");
    }
}
//-----------------------------------------

//------------Camera Settings----------------
void MainWindow::SetCameraBrightness(int value)
{
    if(!capture.isOpened())
    {
        qDebug() << "Camera is not turned on!";
        return;
    }
    capture.set(cv::CAP_PROP_BRIGHTNESS, value);
}

void MainWindow::SetCameraSaturation(int value)
{
    if(!capture.isOpened())
    {
        qDebug() << "Camera is not turned on!";
        return;
    }
    capture.set(cv::CAP_PROP_SATURATION, value);
}

void MainWindow::SetCameraHue(int value)
{
    if(!capture.isOpened())
    {
        qDebug() << "Camera is not turned on!";
        return;
    }
    capture.set(cv::CAP_PROP_HUE, value);
}

//------------Camera Settings----------------

// Serial communication--------------------
void MainWindow::serialReadSendData()
{
    QByteArray qa = serial->readAll();

    ui->textEdit->append(qa);

    if(tcpSocketGps->state() == QTcpSocket::ConnectedState && count == 0)
    {
        count = 1;
        //qDebug() << QTime::currentTime().toString("hh:mm:ss.zzz") << ": Server connected, sending data...";
        QByteArray encodedGpsByteArray = qa.toBase64();

        QByteArray sendGpsByteArray;
        QDataStream outStream(&sendGpsByteArray,QIODevice::WriteOnly);
        outStream.setVersion(QDataStream::Qt_5_11);

        quint64 dataGpsSize = (quint64)(encodedGpsByteArray.size());
        outStream << dataGpsSize;
        outStream << encodedGpsByteArray;
        tcpSocketGps->write(sendGpsByteArray);
        tcpSocketGps->flush();

        count = 0;
    }
    else if(count == 1)
    {
        qDebug()<<"GPS Count still remain at 1, ignore send data!";
    }

    gpsParse(qa);
}

// Parse GPS Data---------------------------
void MainWindow::gpsParse(QByteArray GPSBuffer)
{

    //qDebug()<<GPSBuffer.size();

    if(GPSBuffer.contains("$GPRMC"))
    {

    QList<QByteArray> gpsByteArrays = GPSBuffer.split(',');
    //int count = gpsByteArrays.count();

    int  gpsLat_1 = static_cast<int>(gpsByteArrays.at(3).toDouble()/100);
    double gpsLat_2 = (gpsByteArrays.at(3).toDouble() - gpsLat_1 * 100)/60;
    double gpslat=gpsLat_1 + gpsLat_2;

    int gpsLong_1 = static_cast<int>(gpsByteArrays.at(5).toDouble()/100);
    double gpsLong_2 = (gpsByteArrays.at(5).toDouble()-gpsLong_1 * 100)/60;
    double gpsLong = gpsLong_1 + gpsLong_2;

    double time = gpsByteArrays.at(1).toDouble();
    time = time + 30000; //converting the time to EEST time zone
    QString time_to_string  = QString::number(time).rightJustified(6, '0'); //adding zeroes padding at the start
    QString hours = time_to_string.left(2);
    QString minutes = time_to_string.mid(2,2);
    QString seconds = time_to_string.right(2);

    ui->timelineEdit->setText(hours + ":" + minutes + ":" + seconds);
    ui->latlineEdit->setText(QString::number(gpslat,'g',9));
    ui->longlineEdit->setText(QString::number(gpsLong,'g',10));


    }

}




