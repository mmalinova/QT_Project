#ifndef MAINWINDOWSERVER_H
#define MAINWINDOWSERVER_H

#include <QMainWindow>
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
#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>


QT_BEGIN_NAMESPACE
namespace Ui { class MainWindowServer; }
QT_END_NAMESPACE

class MainWindowServer : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindowServer(QWidget *parent = nullptr);
    ~MainWindowServer();

    QTcpServer *tcpServer;
    QTcpSocket *tcpServerConnection;
    //QStringList *fortunes;
    QImage *img;
    quint64 basize;

    /*------------GPS---------------*/
    QTcpServer *tcpGpsServer;
    QTcpSocket *tcpGpsServerConnection;
    //QStringList *fortunes;
    //QImage *img;
    quint64 gpsize;
    /*----------------------------------*/

public slots:
    void startServer();
    void stopServer();
    void newConnection();
    void newConnectionGps();
    void DisplayError(QAbstractSocket::SocketError socketError);
    void ReadData();
    void ShowImage(QByteArray ba);
    void recordVideo();
    void Screenshot();
    void ShowGpsData(QByteArray gba);

    /*------------GPS---------------*/
    void startGpsServer();
    void stopGpsServer();
    void DisplayErrorGps(QAbstractSocket::SocketError socketErrorGps);
    void ReadGpsData();
    void gpsParse(QByteArray GPSBuffer); // GPS

    /*------------GPS---------------*/

private:
    Ui::MainWindowServer *ui;
    int serverPort;
    int serverPortGps; // GPS
    cv::VideoWriter writer;
    bool beginWrite;
    bool isSaveImage;
};
#endif // MAINWINDOWSERVER_H
