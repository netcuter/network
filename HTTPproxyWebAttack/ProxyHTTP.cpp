#include <QtNetwork>
#include <QApplication>
#include <QMainWindow>
#include <vector>

#include "SilnikSterujacy.hpp"
using namespace std;


class WebProxy: public QObject
{
public:

    SilnikSterujacy *silnikSterujacy;
    ListaPolaczen *listaPolaczen;
    Q_OBJECT

public:
    WebProxy(QObject *parent = 0)
        : QObject(parent)
    {
        QTcpServer *proxyServer = new QTcpServer(this);
        proxyServer->listen(QHostAddress::Any, 8080);
        connect(proxyServer, SIGNAL(newConnection()), this, SLOT(manageQuery()));
        qDebug() << ("Serwer proxy dziala na porcie") << proxyServer->serverPort();
        listaPolaczen=new ListaPolaczen();
    }





private slots:

    void manageQuery()
    {
        QTcpServer *proxyServer = qobject_cast<QTcpServer*>(sender());
        QTcpSocket *socket = proxyServer->nextPendingConnection();
        connect(socket, SIGNAL(readyRead()), this, SLOT(processQuery()));
        connect(socket, SIGNAL(disconnected()), socket, SLOT(deleteLater()));
    }

    void processQuery()
    {
        QTcpSocket *socket = qobject_cast<QTcpSocket*>(sender());
        QByteArray requestData = socket->readAll();

        int pos = requestData.indexOf("\r\n");
        QByteArray requestLine = requestData.left(pos);
        requestData.remove(0, pos + 2);

        QList<QByteArray> entries = requestLine.split(' ');
        QByteArray method = entries.value(0);
        QByteArray address = entries.value(1);
        QByteArray version = entries.value(2);

        QUrl url = QUrl::fromEncoded(address);
        if (!url.isValid()) {
            qWarning() << tr("Nieprawidłowy URL:") << url;
            socket->disconnectFromHost();
            return;
        }

        QString host = url.host();
        int port = (url.port() < 0) ? 80 : url.port();
        QByteArray req = url.encodedPath();
        if (url.hasQuery())
            req.append('?').append(url.encodedQuery());
        requestLine = method + " " + req + " " + version + "\r\n";
        requestData.prepend(requestLine);

        QString key = host + ':' + QString::number(port);
        //QTcpSocket *proxySocket = socket->findChild<QTcpSocket*>(key);
        //if (proxySocket)
        {
            /*
            proxySocket->setObjectName(key);
            proxySocket->setProperty("url", url);
            proxySocket->setProperty("requestData", requestData);
            proxySocket->write(requestData);
            */
        }
        //else
        {
            QTcpSocket *proxySocket = new QTcpSocket(socket);

            Polaczenie p;
            p.proxySocket=proxySocket;
            p.socket=socket;
            p.host=host;
            p.metoda=method;
            p.url=url.path();
            p.nr=-1;
            listaPolaczen->Polaczenia.push_back(p);

            proxySocket->setObjectName(key);
            proxySocket->setProperty("url", url);
            proxySocket->setProperty("requestData", requestData);
            connect(proxySocket, SIGNAL(connected()), this, SLOT(sendRequest()));
            connect(proxySocket, SIGNAL(readyRead()), this, SLOT(transferData()));
            connect(proxySocket, SIGNAL(disconnected()), this, SLOT(closeConnection()));
            connect(proxySocket, SIGNAL(error(QAbstractSocket::SocketError)),
                    this, SLOT(closeConnection()));
            proxySocket->connectToHost(host, port);
        }
    }

    void sendRequest()
    {
        QTcpSocket *proxySocket = qobject_cast<QTcpSocket*>(sender());
        QByteArray requestData = proxySocket->property("requestData").toByteArray();
        proxySocket->write(requestData);

        int i=0;
        Polaczenie *p = 0;


        for(;i<listaPolaczen->Polaczenia.size();++i)
        {
            p = &listaPolaczen->Polaczenia[i];
            if(proxySocket == p->proxySocket)
                break;
        }
        if(p)
        {
            p->zapytanie+=requestData; // = ?
            silnikSterujacy->DodajZapytanie(silnikSterujacy->ModelPolaczenProxy,QString::number(silnikSterujacy->ModelPolaczenProxy->rowCount()+1),p->host,p->metoda,p->url,QString::number(p->odpowiedz.length()),QDateTime::currentDateTime());
// TU MOŻE BYĆ BŁĄD
            if(p->nr==-1)
                p->nr=silnikSterujacy->ModelPolaczenProxy->rowCount()-1;
        }
    }

    void transferData()
    {
        QTcpSocket *proxySocket = qobject_cast<QTcpSocket*>(sender());
        QTcpSocket *socket = qobject_cast<QTcpSocket*>(proxySocket->parent());
        QByteArray dane = proxySocket->readAll();
        socket->write(dane);

        int i=0;
        Polaczenie *p = 0;

        for(;i<listaPolaczen->Polaczenia.size();++i)
        {
            p = &listaPolaczen->Polaczenia[i];
            if(proxySocket == p->proxySocket && socket == p->socket)
                break;
        }

        if(p)
        {
            p->odpowiedz+=dane;
            silnikSterujacy->ModelPolaczenProxy->setData(silnikSterujacy->ModelPolaczenProxy->index(silnikSterujacy->ModelPolaczenProxy->rowCount()-1 -p->nr, 4), p->odpowiedz.length());
        }

    }

    void closeConnection()
    {
        QTcpSocket *proxySocket = qobject_cast<QTcpSocket*>(sender());
        if (proxySocket)
        {
            QTcpSocket *socket = qobject_cast<QTcpSocket*>(proxySocket->parent());
            if (socket)
            {




                socket->disconnectFromHost();
            }
            if (proxySocket->error() != QTcpSocket::RemoteHostClosedError)
                qWarning() << "Blad dla:" << proxySocket->property("url").toUrl()
                        << proxySocket->errorString();
            proxySocket->deleteLater();;
        }
    }
};

#include "webproxy.moc"



int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    SilnikSterujacy Silnik;
    WebProxy proxy;
    proxy.silnikSterujacy = &Silnik;
    Silnik.Historia->listaPolaczen=proxy.listaPolaczen;
    Silnik.Historia->ManualZapytanieEdit=Silnik.ManualZapytanieEdit;
    Silnik.Historia->ManualOdpowiedzEdit=Silnik.ManualOdpowiedzEdit;
    Silnik.Historia->FuzzZapytanieEdit=Silnik.FuzzingZapytanieEdit;
    Silnik.WynikiFuzzing->listaPolaczen=Silnik.listaPolaczenWynikowFuzzingu;
    Silnik.show();
    return app.exec();
}

