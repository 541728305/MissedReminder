#include "mytcp.h"
#include <assert.h>
#include <QTcpServer>
#include <QTcpSocket>
#include <QDebug>
#include <QDataStream>
#include <QTime>


void HostInfoDemo::lookUp(const QHostInfo& oHostInfo)
{
    if (oHostInfo.error() != QHostInfo::NoError)
    {
        qDebug() << "failed: " << oHostInfo.errorString();
        return;
    }

    const auto addresses = oHostInfo.addresses();
    for (const QHostAddress &oAddress : addresses)
    {
        qDebug() << "asynchronous Found address: " << oAddress.toString();
    }
}

void testHostInfoDemo()
{
    //异步方式
    qDebug() << QStringLiteral("异步方式...");
    HostInfoDemo* pDemo = new HostInfoDemo;
    QHostInfo::lookupHost("www.kde.org", pDemo, SLOT(lookUp(QHostInfo)));

    //同步方式
    qDebug() << QStringLiteral("同步方式...");
    QHostInfo oHostInfo = QHostInfo::fromName("www.kde.org");
    const auto addresses = oHostInfo.addresses();
    for (const QHostAddress &oAddress : addresses)
    {
        qDebug() << "synchronous Found address: " << oAddress.toString();
    }
}




//服务端
SimpleTcpSocketServerDemo::SimpleTcpSocketServerDemo()
{
    //初始换原始数据
    m_oData << tr("You've been leading a dog's life. Stay off the furniture.")
                    << tr("You've got to think about tomorrow.")
                    << tr("You will be surprised by a loud noise.")
                    << tr("You will feel hungry again in another hour.")
                    << tr("You might have mail.")
                    << tr("You cannot kill time without injuring eternity.")
                    << tr("Computers are not intelligent. They only think they are.");

    //1. 创建TCP对象
    m_pTcpServer = new QTcpServer(this);
    //2. 新连接、错误信号
    connect(m_pTcpServer, &QTcpServer::newConnection, this, &SimpleTcpSocketServerDemo::sendData);
    connect(m_pTcpServer, &QTcpServer::acceptError, this, &SimpleTcpSocketServerDemo::displayError);
    //3. 启动服务端
    if (!m_pTcpServer->listen(QHostAddress::Any, 8888))
    {
        qDebug() << "m_pTcpServer->listen() error";
        assert(false);
    }
}

void SimpleTcpSocketServerDemo::sendData()
{
    //获取服务端数据
    QString sWriteData = m_oData.at(qrand() % m_oData.size());
    //获取与客户端通信的socket
    QTcpSocket* pClientConnection = m_pTcpServer->nextPendingConnection();
    //从客户端读数据
    QString sReadData = pClientConnection->readAll();
    qDebug() << "SimpleTcpSocketServerDemo::readDataFromClient " << pClientConnection;
    //与客户端写数据
    qDebug() << "SimpleTcpSocketServerDemo::writeDataToClient " << sWriteData;
    pClientConnection->write(sWriteData.toUtf8());
// 	//与客户端断开连接
// 	connect(pClientConnection, &QTcpSocket::disconnected, this, &SimpleTcpSocketServerDemo::deleteLater);
// 	pClientConnection->disconnectFromHost();
}

void SimpleTcpSocketServerDemo::displayError(QAbstractSocket::SocketError socketError)
{
    qDebug() << "SimpleTcpSocketServerDemo::displayError " << socketError;
}

void testSimpleTcpSocketServerDemo()
{
    //这样写会内存泄漏，如此写方便测试。
    SimpleTcpSocketServerDemo* pSimpleTcpSocketServer = new SimpleTcpSocketServerDemo;
}

//客户端

SimpleTcpSocketClientDemo::SimpleTcpSocketClientDemo()
{
    //1. 创建TCP套接字对象
    m_pTcpSocket = new QTcpSocket(this);
    //2. 已连接、数据可读、失败信号连接
    connect(m_pTcpSocket, &QTcpSocket::connected, this, &SimpleTcpSocketClientDemo::connected);
    connect(m_pTcpSocket, &QIODevice::readyRead, this, &SimpleTcpSocketClientDemo::readyRead);
    typedef void (QAbstractSocket::*QAbstractSocketErrorSignal)(QAbstractSocket::SocketError);
    connect(m_pTcpSocket, static_cast<QAbstractSocketErrorSignal>(&QTcpSocket::error), this, &SimpleTcpSocketClientDemo::error);
    //3. 与服务器端建立连接
    m_pTcpSocket->connectToHost("127.0.0.1", 8888);
    //4. 同步处理-等待数据可读
    m_pTcpSocket->waitForReadyRead();
}

void SimpleTcpSocketClientDemo::readyRead()
{
    qDebug() << "SimpleTcpSocketClientDemo::readyRead " << m_pTcpSocket->readAll();
}

void SimpleTcpSocketClientDemo::connected()
{
    qDebug() << "SimpleTcpSocketClientDemo::connected  successfully";
}

void SimpleTcpSocketClientDemo::error(QAbstractSocket::SocketError socketError)
{
    qDebug() << "SimpleTcpSocketClientDemo::error " << socketError;
}

void ClientRunnable::run()
{
    //这样写会内存泄漏，如此写方便测试。
    SimpleTcpSocketClientDemo* pSimpleTcpSocketClient = new SimpleTcpSocketClientDemo;
}

#define CLINET_COUNT 2000  //客户端的数量
void testSimpleTcpSocketClientDemo()
{
    QTime oTime;
    oTime.start();

    //同步线程池的方式模拟多个客户端与服务器端交互
    for (int nIndex = 0; nIndex < CLINET_COUNT; ++nIndex)
    {
        ClientRunnable* pRunnable = new ClientRunnable;
        pRunnable->setAutoDelete(false);
        QThreadPool::globalInstance()->start(pRunnable);
    }

    QThreadPool::globalInstance()->waitForDone(30 * 1000);
    qDebug() << "connect count: " << CLINET_COUNT << "total time: " << (double)oTime.elapsed() / double(1000) << "s";
}

