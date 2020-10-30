#ifndef MYTCP_H
#define MYTCP_H

#include <QObject>
#include <QAbstractSocket>

//get host
#include <QHostInfo>

class HostInfoDemo : public QObject
{
    Q_OBJECT

private slots:
    void lookUp(const QHostInfo& oHostInfo);
};

void testHostInfoDemo();


//服务端
class QTcpServer;
class SimpleTcpSocketServerDemo : public QObject
{
    Q_OBJECT

public:
    SimpleTcpSocketServerDemo();

private slots:
    void sendData();
    void displayError(QAbstractSocket::SocketError socketError);

private:
    QStringList m_oData;
    QTcpServer *m_pTcpServer;
};

void testSimpleTcpSocketServerDemo();

//客户端
#include <QObject>
#include <QAbstractSocket>
#include <QRunnable>
#include <QThreadPool>

class QTcpSocket;
class SimpleTcpSocketClientDemo : public QObject
{
    Q_OBJECT

public:
    SimpleTcpSocketClientDemo();

private slots:
    void connected();
    void readyRead();
    void error(QAbstractSocket::SocketError socketError);

private:
    QTcpSocket* m_pTcpSocket;
};

class ClientRunnable : public QRunnable
{
public:
    void run();
};

void testSimpleTcpSocketClientDemo();
#endif // MYTCP_H




