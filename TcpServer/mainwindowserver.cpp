#include "mainwindowserver.h"
#include "ui_mainwindowserver.h"
#include <stdio.h>
#include <unistd.h>

MainWindowServer::MainWindowServer(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindowServer)
{
    ui->setupUi(this);
    this->setWindowTitle("TcpServer");

    tcpServer = new QTcpServer(this);
    connect(ui->btn_start, SIGNAL(clicked()),this, SLOT(startServer()));
    connect(ui->btn_stop, SIGNAL(clicked()),this, SLOT(stopServer()));
    connect(tcpServer, SIGNAL(newConnection()), this, SLOT(newConnection()));
    connect(ui->btn_record, SIGNAL(clicked()), this, SLOT(recordVideo()));
    connect(ui->btn_screenshot, SIGNAL(clicked()), this, SLOT(Screenshot()));

    isSaveImage = false;
    beginWrite = false;

    /*------------GPS---------------*/
    tcpGpsServer = new QTcpServer(this);
    connect(ui->btn_startgps, SIGNAL(clicked()),this, SLOT(startGpsServer()));
    connect(ui->btn_stopgps, SIGNAL(clicked()),this, SLOT(stopGpsServer()));
    connect(tcpGpsServer, SIGNAL(newConnection()), this, SLOT(newConnectionGps()));

    /*------------------------------*/
}

MainWindowServer::~MainWindowServer()
{
    delete ui;
}

void MainWindowServer::startServer()
{
    if(tcpServer->isListening())
    {
        QMessageBox::information(this, "Info","Video Server(" + QString::number(serverPort) + ") already started!");
    }
    else
    {
        serverPort = ui->lineEdit_serverPort->text().toInt();
    if(!tcpServer->listen(QHostAddress::Any,serverPort))
    {
        QMessageBox::information(this,"Error","Unable to start the video server:"+tcpServer->errorString());
        close();
        return;
        }
    }
}

/*------------GPS---------------*/
void MainWindowServer::startGpsServer()
{
    if(tcpGpsServer->isListening())
    {
        QMessageBox::information(this, "Info","GPS Server(" + QString::number(serverPortGps) + ") already started!");
    }
    else
    {
        serverPortGps = ui->lineEdit_serverPortGps->text().toInt();
    if(!tcpGpsServer->listen(QHostAddress::Any,serverPortGps))
    {
        QMessageBox::information(this,"Error","Unable to start the GPS server:"+tcpGpsServer->errorString());
        close();
        return;
        }
    }
}
/*------------GPS---------------*/

void MainWindowServer::stopServer()
{
    if(tcpServer->isListening())
    {
        tcpServer->close();
    }
    else
    {
        QMessageBox::information(this, "Info","Video Server(" + QString::number(serverPort) + ") already close");
    }

    if(tcpServerConnection->isOpen())
    {
        tcpServerConnection->close();
    }
    else
    {
        QMessageBox::information(this, "Info","Video Server(" + QString::number(serverPort) + ") already close");
    }

    if(writer.isOpened())
    {
        writer.release();
        qDebug()<<"Writer released!";
        ui->btn_record->setText("Record Video");
    }
}

/*------------GPS---------------*/
void MainWindowServer::stopGpsServer()
{
    if(tcpGpsServer->isListening())
    {
        tcpGpsServer->close();
    }
    else
    {
        QMessageBox::information(this, "Info","GPS Server(" + QString::number(serverPortGps) + ") already close");
    }

    if(tcpGpsServerConnection->isOpen())
    {
        tcpGpsServerConnection->close();
    }
    else
    {
        QMessageBox::information(this, "Info","GPS Server(" + QString::number(serverPortGps) + ") already close");
    }

}
/*------------GPS---------------*/


void MainWindowServer::newConnection()
{
    basize=0;
    tcpServerConnection = tcpServer->nextPendingConnection();
    connect(tcpServerConnection,SIGNAL(readyRead()),this,SLOT(ReadData()));
    connect(tcpServerConnection,SIGNAL(error(QAbstractSocket::SocketError)),this,SLOT(DisplayError(QAbstractSocket::SocketError)));
}

/*------------GPS---------------*/
void MainWindowServer::newConnectionGps()
{
    gpsize=0;
    tcpGpsServerConnection = tcpGpsServer->nextPendingConnection();
    connect(tcpGpsServerConnection,SIGNAL(readyRead()),this,SLOT(ReadGpsData()));
    connect(tcpGpsServerConnection,SIGNAL(error(QAbstractSocket::SocketError)),this,SLOT(DisplayErrorGps(QAbstractSocket::SocketError)));
}
/*------------GPS---------------*/

void MainWindowServer::DisplayError(QAbstractSocket::SocketError socketError)
{
    tcpServerConnection->close();
    qDebug() << "video server connect error:" << socketError;
}

void MainWindowServer::DisplayErrorGps(QAbstractSocket::SocketError socketErrorGps)
{
    tcpGpsServerConnection->close();
    qDebug() << "gps server connect error:" << socketErrorGps;
}

void MainWindowServer::ReadData()
{
    QByteArray message;

    QDataStream in(tcpServerConnection);
    in.setVersion(QDataStream::Qt_5_11);
    if (basize==0)
    {
        //дали получавам 8 байта, размер на файла
        //до получаване на 8 пълни байта
    if (tcpServerConnection->bytesAvailable()<(int)sizeof(quint64))
    {
        return;
    }
        in>>basize;
    }

        //до получаване на пълен msg
    if (quint64(tcpServerConnection->bytesAvailable())<basize)
    {
        return;
    }
        in>>message;
        ShowImage(message);
}

void MainWindowServer::ShowImage(QByteArray receivedData)
{
    QString receivedString = QString::fromLatin1(receivedData.data(),receivedData.size());
    QByteArray decodeByteArray;
    decodeByteArray = QByteArray::fromBase64(receivedString.toLatin1());
    QByteArray uncompressedByteArray = qUncompress(decodeByteArray);
    QImage img;
    img.loadFromData(uncompressedByteArray, "JPEG");
    ui->image->setPixmap(QPixmap::fromImage(img));
    ui->image->resize(img.size());

    basize = 0;

    if(isSaveImage)
    {
        QString image_name = QString("%1.jpg").arg(QDateTime::currentDateTime().toString("dd_MM_yyyy_hh_mm_ss"));
        if(img.save(image_name))
        {
            qDebug() << "Happy Screenshot" << image_name;
            isSaveImage = false;
        }
        else {
            qDebug() << "Failed Screenshot" << image_name;
        }

    }


    if(beginWrite)
    {
        //qDebug() << QTime::currentTime().toString("hh:mm:ss.zzz") <<"img format: " << img.format();
        cv::Mat frame = cv::Mat(img.height(), img.width(), CV_8UC4, img.bits(), img.bytesPerLine()).clone();
        cv::Mat  matNoAlpha;
        cv::cvtColor( frame, matNoAlpha, cv::COLOR_BGRA2BGR );   //отпадане на изцяло белия alpha канал

    if(!writer.isOpened())
    {

        QString targetVideoPath = qApp->applicationDirPath();
        QString targetVideoName = "video_"+QDateTime::currentDateTime().toString("dd_MM_yyyy_hh_mm_ss")+".avi";
        QString targetVideo = targetVideoPath + "/" + targetVideoName;
        qDebug()<<"Save Video to: " + targetVideo;
        writer.open(targetVideo.toStdString(), cv::VideoWriter::fourcc('M', 'J', 'P', 'G'), 20, cv::Size(img.width(), img.height()), true);
        qDebug()<<"Writer Opened";
    }

    if(writer.isOpened())
    {
        writer.write(matNoAlpha); //записване на кадър в VideoWriter
    }
  }
}

void MainWindowServer::recordVideo()
{
    if(beginWrite)
    {
        ui->btn_record->setText("Record Video");
    if(writer.isOpened())
    {
        writer.release();
    }
        qDebug()<<"Writer released";
        beginWrite = false;
    }
    else
    {
        ui->btn_record->setText("Stop Record Video");
        beginWrite = true;
    }
}

void MainWindowServer::Screenshot()
{
    isSaveImage = true;
}

//GPS Read data--------------------------------------
void MainWindowServer::ReadGpsData()
{
    QByteArray gpsmessage;

    QDataStream in(tcpGpsServerConnection);
    in.setVersion(QDataStream::Qt_5_11);
    if (gpsize==0)
    {
        //дали получавам 8 байта, размер на файла
        //до получаване на 8 пълни байта
    if (tcpGpsServerConnection->bytesAvailable()<(int)sizeof(quint64))
    {
        return;
    }
        in>>gpsize;
    }

        //до получаване на пълен msg
    if (quint64(tcpGpsServerConnection->bytesAvailable())<gpsize)
    {
        return;
    }
        in>>gpsmessage;
        ShowGpsData(gpsmessage);
}

void MainWindowServer::ShowGpsData(QByteArray receivedGpsData)
{
    QString receivedGpsString = QString::fromLatin1(receivedGpsData.data(), receivedGpsData.size());
    QByteArray decodeGpsByteArray = QByteArray::fromBase64(receivedGpsString.toLatin1());
    ui->textEdit->append(decodeGpsByteArray);

    gpsParse(decodeGpsByteArray);
    //qDebug() << decodeGpsByteArray;
    gpsize = 0;
}

void MainWindowServer::gpsParse(QByteArray GPSBuffer)
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
