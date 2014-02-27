#include "SilnikSterujacy.hpp"
#include <QSplitter>
#include <QTabBar>
#include <QTabWidget>
#include "WidgetListyPolaczen.h"
#include <QtGui/QMessageBox>


// TODO
// <NAPRAWIONE>
    // wyłączyć lampp i spróbować połączyć się ze stroną na localhost, bo tam jest chyba błąd zliczania o jeden i robić requesty
    // kolejne typu localhost/blog/index.php?kategoria=aa, localhost/blog/index.php?kategoria=aawf
    // tam jest chyba taka sytuacja ze jak sie nie uda polaczenie, to dany socket jest wykorzystywany ponownie
    // ale tez trzeba popatrzec, co zwraca QDebug
    // wogóle tam jak klikam w kolejne odnośniki to czasem nie dodaje zapytań
// <NAPRAWIONE/>

// <NAPRAWIONE>
    // trzeba zrobić żeby każdy socket mógł być tylko raz użyty a potem usunięty
// <NAPRAWIONE/>

//jak jest za dużo połączeń to jest błąd - trzeba by np sprawdzać ile max socketów można utworzyć w /proc
//wprowadzić kodowanie - zrobione

//GET /blog/index.php?kategoriP/1.10


// PRYWATNE DO PRYWATNYCH


// dorobić aktualizacje czasu

// możnaby szybko dorobić wyświetlanie czy zapytanie miało parametry
PunktWstrzykniecia::PunktWstrzykniecia()
{
    this->Aktywny=0;
    this->Skonczono=0;
    this->nr_aktualnegoPayloadu=0;


}

void SilnikSterujacy::ManualWyslijZapytanie()
{
    QTcpSocket *Socket = qobject_cast<QTcpSocket*>(sender());
    QByteArray requestData = Socket->property("requestData").toByteArray();
    Socket->write(requestData);


}

void SilnikSterujacy::FuzzingWyslijZapytanie()
{
    QTcpSocket *Socket = qobject_cast<QTcpSocket*>(sender());
    QByteArray requestData = Socket->property("requestData").toByteArray();
    Socket->write(requestData);



    int i=0;
    Polaczenie *p = 0;

    for(;i<listaPolaczenWynikowFuzzingu->Polaczenia.size();++i)
    {
        p = &listaPolaczenWynikowFuzzingu->Polaczenia[i];
        if(Socket == p->proxySocket)
            break;
    }

    if(p)
    {
        p->zapytanie+=requestData; // = ?



        DodajZapytanie(ModelPolaczenWynikow,QString::number(ModelPolaczenWynikow->rowCount()+1),p->host,p->metoda,p->url,QString::number(p->odpowiedz.length()),QDateTime::currentDateTime());

        if(p->nr==-1)
            p->nr=ModelPolaczenWynikow->rowCount()-1;
    }
}

void SilnikSterujacy::ManualtransferData()
{
    QTcpSocket *proxySocket = qobject_cast<QTcpSocket*>(sender());
    QByteArray dane = proxySocket->readAll();
    ManualOdpowiedzEdit->setText(ManualOdpowiedzEdit->toPlainText()+dane);
}

void SilnikSterujacy::FuzzingtransferData()
{
    QTcpSocket *proxySocket = qobject_cast<QTcpSocket*>(sender());
    QByteArray dane = proxySocket->readAll();

    int i=0;
    Polaczenie *p = 0;

    for(;i<listaPolaczenWynikowFuzzingu->Polaczenia.size();++i)
    {
        p = &listaPolaczenWynikowFuzzingu->Polaczenia[i];
        if(proxySocket == p->proxySocket)
            break;
    }

    if(p)
    {

        p->odpowiedz+=dane;
        int nr = ModelPolaczenWynikow->rowCount()-1 -p->nr;
        ModelPolaczenWynikow->setData(ModelPolaczenWynikow->index(nr, 4), p->odpowiedz.length());

    }


}

void SilnikSterujacy::closeConnection()
{
    QTcpSocket *proxySocket = qobject_cast<QTcpSocket*>(sender());
    if (proxySocket)
    {
        proxySocket->disconnectFromHost();
        if (proxySocket->error() != QTcpSocket::RemoteHostClosedError)
            qWarning() << "Error for:" << proxySocket->property("url").toUrl()
                    << proxySocket->errorString();
        proxySocket->deleteLater();;
    }
}

void SilnikSterujacy::PunktyWstrzykniecObslugaZdarzeniaComboboxa(int wybor)
{
    QComboBox *c = qobject_cast<QComboBox*>(sender());
    QString nazwa_comboboxa = c->currentText();
    if(nazwa_comboboxa==tr("Wczytaj własne"))
    {
        QString nazwa_pliku = QFileDialog::getOpenFileName(this,tr("Wczytaj własną listę payloadów"), "",tr("Wszystkie typy plików (*)"));
        for(int i=0;i<ListaComboboxow.size();++i)
        {
            if(ListaComboboxow[i]==c)
            {
                ListaPunktowWstrzyknkiec[i].plik_z_lista_payloadow=nazwa_pliku;
                break;
            }
        }
    }

}

void SilnikSterujacy::FuzzingKliknietoButtonStart()
{
    int iloscskonczonych = 0,iloscaktywnych=0;
    QByteArray requestData = FuzzingZapytanieEdit->toPlainText().toAscii();

    if(Fuzzingradio2->isChecked())
    {
        while(iloscskonczonych<ListaPunktowWstrzyknkiec.size())
        {

            for(int i = 0; i<ListaPunktowWstrzyknkiec.size();++i)
            {
                if(!ListaPunktowWstrzyknkiec[i].Skonczono)
                {
                    if(ListaComboboxow[i]->currentText() == "0-9" || ListaComboboxow[i]->currentText() == "a-z" || ListaComboboxow[i]->currentText() == "A-Z")
                    {
                        if(ListaPunktowWstrzyknkiec[i].Aktywny)
                        {


                            wchar_t *tablica = new wchar_t[ListaPunktowWstrzyknkiec[i].aktualnyPayload.size()];
                            ListaPunktowWstrzyknkiec[i].aktualnyPayload.toWCharArray(tablica);
                            //char a = tablica[0] + 1;
                            tablica[tablica[1]=0]++;
                            ListaPunktowWstrzyknkiec[i].poprzedniPayload = ListaPunktowWstrzyknkiec[i].aktualnyPayload;
                            ListaPunktowWstrzyknkiec[i].aktualnyPayload = QString::fromWCharArray(tablica);

                        }
                        else
                        {
                            ListaPunktowWstrzyknkiec[i].poprzedniPayload="$"+ListaPunktowWstrzyknkiec[i].text+"$";
                            ListaPunktowWstrzyknkiec[i].aktualnyPayload=ListaComboboxow[i]->currentText()[0];
                            ListaPunktowWstrzyknkiec[i].ostatniPayload=ListaComboboxow[i]->currentText()[2];
                            ListaPunktowWstrzyknkiec[i].Aktywny=1;
                            iloscaktywnych++;
                        }


                        requestData.replace(ListaPunktowWstrzyknkiec[i].nr_pozycji_w_zapytaniu,ListaPunktowWstrzyknkiec[i].poprzedniPayload.size(),ListaPunktowWstrzyknkiec[i].aktualnyPayload.toAscii());


                        for(int k=0;k<ListaPunktowWstrzyknkiec.size();++k)
                        {
                            if(ListaPunktowWstrzyknkiec[k].nr_pozycji_w_zapytaniu > ListaPunktowWstrzyknkiec[i].nr_pozycji_w_zapytaniu)
                            {
                                ListaPunktowWstrzyknkiec[k].nr_pozycji_w_zapytaniu += (ListaPunktowWstrzyknkiec[i].aktualnyPayload.size() - ListaPunktowWstrzyknkiec[i].poprzedniPayload.size());
                            }
                        }

                        // WYKONAJ
                        if(iloscaktywnych==ListaPunktowWstrzyknkiec.size())
                        {

                            //QString a = FuzzZapytanieEdit->toPlainText();
                            //QByteArray requestData = FuzzZapytanieEdit->toPlainText().toAscii();
                            // TU ZMIENIĆ
                            //requestData.replace(ListaPunktowWstrzyknkiec[i].nr_pozycji_w_zapytaniu,ListaPunktowWstrzyknkiec[i].poprzedniPayload.size(),ListaPunktowWstrzyknkiec[i].aktualnyPayload.toAscii());
                            QByteArray requestData2 = requestData;
                            if(!requestData2.isEmpty())
                            {
                                //OdpowiedzEdit->setText("");
                                QTcpSocket *proxySocket = new QTcpSocket();


                                requestData2.replace("\n","\r\n");
                                QByteArray temp = requestData2;
                                temp.remove(0,temp.indexOf("Host: ")+6);
                                temp.remove(temp.indexOf("\r\n"),temp.length());
                                requestData2.insert(requestData2.indexOf("GET")+3," http://"+temp);
                                if(requestData2.indexOf(" /")>1)
                                {
                                    requestData2.remove(requestData2.indexOf(" /"),1);
                                }

                                int pos = requestData2.indexOf("\r\n");
                                QByteArray requestLine = requestData2.left(pos);
                                requestData2.remove(0, pos + 2);

                                QList<QByteArray> entries = requestLine.split(' ');
                                QByteArray method = entries.value(0);
                                QByteArray address = entries.value(1);
                                QByteArray version = entries.value(2);

                                QUrl url = QUrl::fromEncoded(address);
                                if (!url.isValid())
                                {
                                    qWarning() << "Invalid URL:" << url;
                                }

                                QString host = url.host();
                                int port = (url.port() < 0) ? 80 : url.port();
                                QByteArray req = url.encodedPath();
                                if (url.hasQuery())
                                    req.append('?').append(url.encodedQuery());
                                requestLine = method + " " + req + " " + version + "\r\n";
                                requestData2.prepend(requestLine);

                                QString key = host + ':' + QString::number(port);

                                Polaczenie p;
                                p.proxySocket=proxySocket;
                                p.host=host;
                                p.metoda=method;
                                p.url=url.path();
                                p.nr=-1;
                                listaPolaczenWynikowFuzzingu->Polaczenia.push_back(p);



                                if(requestData2.indexOf("POST")==0)
                                {
                                    int pozycja_content_length = requestData2.indexOf("Content-Length: ");
                                    int rozmiar_post = requestData2.size()-requestData2.indexOf("\r\n\r\n") - 4;
                                    QString naglowek_content_length = "Content-Length: "+QString::number(rozmiar_post);
                                    requestData2.replace(pozycja_content_length,naglowek_content_length.length(),naglowek_content_length.toAscii());
                                }


                                proxySocket->setObjectName(key);
                                proxySocket->setProperty("url", url);
                                proxySocket->setProperty("requestData", requestData2);
                                connect(proxySocket, SIGNAL(connected()), this, SLOT(FuzzingWyslijZapytanie()));
                                connect(proxySocket, SIGNAL(readyRead()), this, SLOT(FuzzingtransferData()));
                                connect(proxySocket, SIGNAL(disconnected()), this, SLOT(closeConnection()));
                                connect(proxySocket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(closeConnection()));
                                proxySocket->connectToHost(host, port);
                            }
                        }
                        // koniec wykonywania
                        if(ListaPunktowWstrzyknkiec[i].aktualnyPayload==ListaPunktowWstrzyknkiec[i].ostatniPayload)
                        {
                            ListaPunktowWstrzyknkiec[i].Skonczono=1;
                            iloscskonczonych++;
                        }
                    }
                    else
                        if(ListaComboboxow[i]->currentText()=="XSS" || ListaComboboxow[i]->currentText()=="SQLi" || ListaComboboxow[i]->currentText()=="Path traversal" || ListaComboboxow[i]->currentText()==tr("Wczytaj własne"))
                        {
        // DOKOŃCZYĆ !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
                            if(ListaPunktowWstrzyknkiec[i].plik_z_lista_payloadow.isEmpty())
                                ListaPunktowWstrzyknkiec[i].plik_z_lista_payloadow=ListaComboboxow[i]->currentText()+".txt";

                            QFile file(ListaPunktowWstrzyknkiec[i].plik_z_lista_payloadow);
                            if(!file.open(QIODevice::ReadOnly))
                            {
                                QMessageBox::information(0, "error", file.errorString());
                            }

                            QTextStream in(&file);


                            while(!in.atEnd())
                            {
                                 //QString line =
                                 // !!!!!!!!!! DOROBIĆ : JEŻELI W OPCJACH KODOWANIE QUrl::toPercentEncoding
                                 ListaPunktowWstrzyknkiec[i].listaPayloadow.append((in.readLine()));
                             }

                        }
                }
            }
        }
    }

    else
    if(Fuzzingradio1->isChecked())
    {
        for(int i=0; i<ListaPunktowWstrzyknkiec.size();++i)
        {
            //if(!ListaPunktowWstrzyknkiec[i].Aktywny)
            if(ListaComboboxow[i]->currentText() == "0-9" || ListaComboboxow[i]->currentText() == "a-z" || ListaComboboxow[i]->currentText() == "A-Z")
            {
                ListaPunktowWstrzyknkiec[i].poprzedniPayload="$"+ListaPunktowWstrzyknkiec[i].text+"$";
                ListaPunktowWstrzyknkiec[i].domyslnyPayload=ListaPunktowWstrzyknkiec[i].text;
                ListaPunktowWstrzyknkiec[i].aktualnyPayload=ListaComboboxow[i]->currentText()[0];
                ListaPunktowWstrzyknkiec[i].pierwszyPayload=ListaPunktowWstrzyknkiec[i].aktualnyPayload;
                ListaPunktowWstrzyknkiec[i].ostatniPayload=ListaComboboxow[i]->currentText()[2];
                ListaPunktowWstrzyknkiec[i].Aktywny=1;



                requestData.replace(ListaPunktowWstrzyknkiec[i].nr_pozycji_w_zapytaniu,ListaPunktowWstrzyknkiec[i].poprzedniPayload.size(),ListaPunktowWstrzyknkiec[i].aktualnyPayload.toAscii());
                //ListaPunktowWstrzyknkiec[i].poprzedniPayload=ListaPunktowWstrzyknkiec[i].aktualnyPayload;

                for(int k=0;k<ListaPunktowWstrzyknkiec.size();++k)
                {
                    if(ListaPunktowWstrzyknkiec[k].nr_pozycji_w_zapytaniu > ListaPunktowWstrzyknkiec[i].nr_pozycji_w_zapytaniu)
                    {
                        ListaPunktowWstrzyknkiec[k].nr_pozycji_w_zapytaniu += (ListaPunktowWstrzyknkiec[i].aktualnyPayload.size() - ListaPunktowWstrzyknkiec[i].poprzedniPayload.size());
                    }
                }
            }
            else
                if(ListaComboboxow[i]->currentText()=="XSS" || ListaComboboxow[i]->currentText()=="SQLi" || ListaComboboxow[i]->currentText()=="Path traversal" || ListaComboboxow[i]->currentText()==tr("Wczytaj własne"))
                {
                    if(ListaPunktowWstrzyknkiec[i].plik_z_lista_payloadow.isEmpty())
                        ListaPunktowWstrzyknkiec[i].plik_z_lista_payloadow=ListaComboboxow[i]->currentText()+".txt";

                    QFile file(ListaPunktowWstrzyknkiec[i].plik_z_lista_payloadow);

                    if(!file.open(QIODevice::ReadOnly))
                    {
                        QMessageBox::information(0, "error", file.errorString());
                    }

                    QTextStream in(&file);

                    while(!in.atEnd())
                    {
                        //QString line = in.readLine();
                         // DOROBIĆ : JEŻELI W OPCJACH KODOWANIE :QUrl::toPercentEncoding
                        QString line = in.readLine();
                        QUrl url;
                        url.addQueryItem("x", line);
                        //QByteArray data = url.encodedQuery();
                        QString linia = QString(url.encodedQuery()).replace("%20", "+");
                        linia.remove(0,2);
                        ListaPunktowWstrzyknkiec[i].listaPayloadow.append(linia);







                    }

                    ListaPunktowWstrzyknkiec[i].poprzedniPayload="$"+ListaPunktowWstrzyknkiec[i].text+"$";
                    ListaPunktowWstrzyknkiec[i].domyslnyPayload=ListaPunktowWstrzyknkiec[i].text;
                    ListaPunktowWstrzyknkiec[i].aktualnyPayload=ListaPunktowWstrzyknkiec[i].listaPayloadow[0];
                    ListaPunktowWstrzyknkiec[i].pierwszyPayload=ListaPunktowWstrzyknkiec[i].aktualnyPayload;
                    ListaPunktowWstrzyknkiec[i].ostatniPayload=ListaPunktowWstrzyknkiec[i].listaPayloadow[ListaPunktowWstrzyknkiec[i].listaPayloadow.size()-1];
                    ListaPunktowWstrzyknkiec[i].Aktywny=1;

                    requestData.replace(ListaPunktowWstrzyknkiec[i].nr_pozycji_w_zapytaniu,ListaPunktowWstrzyknkiec[i].poprzedniPayload.size(),ListaPunktowWstrzyknkiec[i].aktualnyPayload.toAscii());


                    for(int k=0;k<ListaPunktowWstrzyknkiec.size();++k)
                    {
                        if(ListaPunktowWstrzyknkiec[k].nr_pozycji_w_zapytaniu > ListaPunktowWstrzyknkiec[i].nr_pozycji_w_zapytaniu)
                        {
                            ListaPunktowWstrzyknkiec[k].nr_pozycji_w_zapytaniu += (ListaPunktowWstrzyknkiec[i].aktualnyPayload.size() - ListaPunktowWstrzyknkiec[i].poprzedniPayload.size());
                        }
                    }

                }
        }

        for(int i=0; i<ListaPunktowWstrzyknkiec.size();++i)
        {
            do
            {
                {


                    //wykonaj

                    QByteArray requestData2 = requestData;
                    if(!requestData2.isEmpty())
                    {
                        //OdpowiedzEdit->setText("");
                        QTcpSocket *proxySocket = new QTcpSocket();


                        requestData2.replace("\n","\r\n");

                        QByteArray requestData3 = requestData2;

                        QByteArray temp = requestData2;
                        temp.remove(0,temp.indexOf("Host: ")+6);
                        temp.remove(temp.indexOf("\r\n"),temp.length());


                        if(requestData2.indexOf("GET")>=0)
                        {
                            requestData2.insert(requestData2.indexOf("GET")+3," http://"+temp); // TU MOŻE BYĆ BŁĄD
                            requestData3.replace(requestData3.indexOf("GET "),4,"GET http://"+temp);
                        }
                        else
                            if(requestData2.indexOf("POST")>=0)
                            {
                                requestData2.insert(requestData2.indexOf("POST")+4," http://"+temp);
                                requestData3.replace(requestData3.indexOf("POST "),5,"POST http://"+temp);
                            }

                        if(requestData2.indexOf(" /")>1)
                        {
                            requestData2.remove(requestData2.indexOf(" /"),1);
                        }


                        requestData2 = requestData3;
                        int pos = requestData2.indexOf("\r\n");
                        QByteArray requestLine = requestData2.left(pos);
                        requestData2.remove(0, pos + 2);

                        requestLine.remove(ListaPunktowWstrzyknkiec[i].nr_pozycji_w_zapytaniu+7+temp.size(),ListaPunktowWstrzyknkiec[i].aktualnyPayload.length());

                        QList<QByteArray> entries = requestLine.split(' ');
                        QByteArray method = entries.value(0);
                        QByteArray address = entries.value(1);
                                if(ListaPunktowWstrzyknkiec[i].nr_pozycji_w_zapytaniu < pos)
                                    address += ListaPunktowWstrzyknkiec[i].aktualnyPayload.toAscii();
                        QByteArray version = entries.value(2);

                        QUrl url = QUrl::fromEncoded(address);
                        if (!url.isValid())
                        {
                            qWarning() << "Invalid URL:" << url;
                        }

                        QString host = url.host();
                        int port = (url.port() < 0) ? 80 : url.port();
                        QByteArray req = url.encodedPath();
                        if (url.hasQuery())
                            req.append('?').append(url.encodedQuery());
                        requestLine = method + " " + req + " " + version + "\r\n";
                        requestData2.prepend(requestLine);

                        QString key = host + ':' + QString::number(port);

                        Polaczenie p;
                        p.proxySocket=proxySocket;
                        p.host=host;
                        p.metoda=method;
                        p.url=url.path();
                        p.nr=-1;
                        listaPolaczenWynikowFuzzingu->Polaczenia.push_back(p);

                        proxySocket->setObjectName(key);
//                        if(host!="localhost")
//                        {
//                            qWarning() << "Invalid hostname:" << host;
//                        }

                        if(requestData2.indexOf("POST")==0)
                        {
                            int pozycja_content_length = requestData2.indexOf("Content-Length: ");
                            int rozmiar_post = requestData2.size()-requestData2.indexOf("\r\n\r\n") - 4;
                            QString naglowek_content_length = "Content-Length: "+QString::number(rozmiar_post);
                            requestData2.replace(pozycja_content_length,naglowek_content_length.length(),naglowek_content_length.toAscii());
                        }


                        proxySocket->setProperty("url", url);
                        proxySocket->setProperty("requestData", requestData2);
                        connect(proxySocket, SIGNAL(connected()), this, SLOT(FuzzingWyslijZapytanie()));
                        connect(proxySocket, SIGNAL(readyRead()), this, SLOT(FuzzingtransferData()));
                        connect(proxySocket, SIGNAL(disconnected()), this, SLOT(closeConnection()));
                        connect(proxySocket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(closeConnection()));
                        proxySocket->connectToHost(host, port);
                    }
                }

                // koniec wykonywania



                if(ListaPunktowWstrzyknkiec[i].aktualnyPayload==ListaPunktowWstrzyknkiec[i].ostatniPayload)
                {
                    ListaPunktowWstrzyknkiec[i].Skonczono=1;
                    //ListaPunktowWstrzyknkiec[i].aktualnyPayload=ListaPunktowWstrzyknkiec[i].pierwszyPayload;

                    ListaPunktowWstrzyknkiec[i].poprzedniPayload = ListaPunktowWstrzyknkiec[i].aktualnyPayload;

                    ListaPunktowWstrzyknkiec[i].aktualnyPayload=ListaPunktowWstrzyknkiec[i].domyslnyPayload;
                    requestData.replace(ListaPunktowWstrzyknkiec[i].nr_pozycji_w_zapytaniu,ListaPunktowWstrzyknkiec[i].poprzedniPayload.size(),ListaPunktowWstrzyknkiec[i].aktualnyPayload.toAscii());
                    //ListaPunktowWstrzyknkiec[i].poprzedniPayload=ListaPunktowWstrzyknkiec[i].aktualnyPayload;

                    for(int k=0;k<ListaPunktowWstrzyknkiec.size();++k)
                    {
                        if(ListaPunktowWstrzyknkiec[k].nr_pozycji_w_zapytaniu > ListaPunktowWstrzyknkiec[i].nr_pozycji_w_zapytaniu)
                        {
                            ListaPunktowWstrzyknkiec[k].nr_pozycji_w_zapytaniu += (ListaPunktowWstrzyknkiec[i].aktualnyPayload.size() - ListaPunktowWstrzyknkiec[i].poprzedniPayload.size());
                        }
                    }
                 }  //iloscskonczonych++;
                else
                    if(ListaPunktowWstrzyknkiec[i].Aktywny)
                    {
                        if(ListaComboboxow[i]->currentText() == "0-9" || ListaComboboxow[i]->currentText() == "a-z" || ListaComboboxow[i]->currentText() == "A-Z")
                        {
                            wchar_t *tablica = new wchar_t[ListaPunktowWstrzyknkiec[i].aktualnyPayload.size()];
                            ListaPunktowWstrzyknkiec[i].aktualnyPayload.toWCharArray(tablica);
                            tablica[tablica[1]=0]++;
                            ListaPunktowWstrzyknkiec[i].poprzedniPayload = ListaPunktowWstrzyknkiec[i].aktualnyPayload;
                            ListaPunktowWstrzyknkiec[i].aktualnyPayload = QString::fromWCharArray(tablica);
                            delete[] tablica;
                        }
                        else
                            if(ListaComboboxow[i]->currentText()=="XSS" || ListaComboboxow[i]->currentText()=="SQLi" || ListaComboboxow[i]->currentText()=="Path traversal" || ListaComboboxow[i]->currentText()==tr("Wczytaj własne"))
                            {
                                ListaPunktowWstrzyknkiec[i].poprzedniPayload = ListaPunktowWstrzyknkiec[i].aktualnyPayload;

                                ListaPunktowWstrzyknkiec[i].nr_aktualnegoPayloadu+=1;

                                ListaPunktowWstrzyknkiec[i].aktualnyPayload=ListaPunktowWstrzyknkiec[i].listaPayloadow[ListaPunktowWstrzyknkiec[i].nr_aktualnegoPayloadu];

                            }


                        //!?
                        //CHYBA PO OSATATNIM PAYLOADZIE JEST PROBLEM Z PRZYWRÓCENIEM PRAWIDŁOWYCH POZYCJI
                        if(!ListaPunktowWstrzyknkiec[i].Skonczono)
                        {
                            requestData.replace(ListaPunktowWstrzyknkiec[i].nr_pozycji_w_zapytaniu,ListaPunktowWstrzyknkiec[i].poprzedniPayload.size(),ListaPunktowWstrzyknkiec[i].aktualnyPayload.toAscii());


                            for(int k=0;k<ListaPunktowWstrzyknkiec.size();++k)
                            {
                                if(ListaPunktowWstrzyknkiec[k].nr_pozycji_w_zapytaniu > ListaPunktowWstrzyknkiec[i].nr_pozycji_w_zapytaniu)
                                {
                                    ListaPunktowWstrzyknkiec[k].nr_pozycji_w_zapytaniu += (ListaPunktowWstrzyknkiec[i].aktualnyPayload.size() - ListaPunktowWstrzyknkiec[i].poprzedniPayload.size());
                                }
                            }
                        }
                    }
                    else
                    {

                        //iloscaktywnych++;
                    }




            }while(!ListaPunktowWstrzyknkiec[i].Skonczono);

        }

    }
}
 void SilnikSterujacy::ManualKliknietoButtonUtworz()
 {
    // QString a = ZapytanieEdit->toPlainText();
     QByteArray requestData = ManualZapytanieEdit->toPlainText().toAscii();

     QByteArray ManualrequestData2 = requestData;
     if(!ManualrequestData2.isEmpty())
     {
         ManualOdpowiedzEdit->setText("");
         QTcpSocket *proxySocket = new QTcpSocket();


         ManualrequestData2.replace("\n","\r\n");

         QByteArray requestData3 = ManualrequestData2;

         QByteArray temp = ManualrequestData2;
         temp.remove(0,temp.indexOf("Host: ")+6);
         temp.remove(temp.indexOf("\r\n"),temp.length());


         if(ManualrequestData2.indexOf("GET")>=0)
         {
             ManualrequestData2.insert(ManualrequestData2.indexOf("GET")+3," http://"+temp); // TU MOŻE BYĆ BŁĄD
             requestData3.replace(requestData3.indexOf("GET "),4,"GET http://"+temp);
         }
         else
             if(ManualrequestData2.indexOf("POST")>=0)
             {
                 ManualrequestData2.insert(ManualrequestData2.indexOf("POST")+4," http://"+temp);
                 requestData3.replace(requestData3.indexOf("POST "),5,"POST http://"+temp);
             }


         if(ManualrequestData2.indexOf(" /")>1)
         {
             ManualrequestData2.remove(ManualrequestData2.indexOf(" /"),1);
         }


         ManualrequestData2 = requestData3;
         int pos = ManualrequestData2.indexOf("\r\n");
         QByteArray requestLine = ManualrequestData2.left(pos);
         ManualrequestData2.remove(0, pos + 2);

         QList<QByteArray> entries = requestLine.split(' ');
         QByteArray method = entries.value(0);
         QByteArray address = entries.value(1);
         QByteArray version = entries.value(2);

         QUrl url = QUrl::fromEncoded(address);
         if (!url.isValid())
         {
             qWarning() << "Invalid URL:" << url;
         }

         QString host = url.host();
         int port = (url.port() < 0) ? 80 : url.port();
         QByteArray req = url.encodedPath();
         if (url.hasQuery())
             req.append('?').append(url.encodedQuery());
         requestLine = method + " " + req + " " + version + "\r\n";
         ManualrequestData2.prepend(requestLine);

         QString key = host + ':' + QString::number(port);




         proxySocket->setObjectName(key);
         proxySocket->setProperty("url", url);
         proxySocket->setProperty("requestData", ManualrequestData2);
         connect(proxySocket, SIGNAL(connected()), this, SLOT(ManualWyslijZapytanie()));
         connect(proxySocket, SIGNAL(readyRead()), this, SLOT(ManualtransferData()));
         connect(proxySocket, SIGNAL(disconnected()), this, SLOT(closeConnection()));
         connect(proxySocket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(closeConnection()));
         proxySocket->connectToHost(host, port);
    }



}

 void SilnikSterujacy::FuzzingKliknietoButtonDodaj()
 {
     QTextCursor cursor(FuzzingZapytanieEdit->textCursor());

     QByteArray Zapytanie = FuzzingZapytanieEdit->toPlainText().toAscii();
     //QString Zapytanie = FuzzZapytanieEdit->toPlainText();
     QList<QByteArray> Linie = Zapytanie.split('\n');
     int nr_linii = 0;
     int k=0;
     for(int i=0;i<=cursor.selectionStart();++i)
     {
         if(k==Linie[nr_linii].size())
         {
             nr_linii++;
             k=0;
         }
         else
         {
            k++;
         }

     }

     PunktWstrzykniecia p;
     p.nr_pozycji_w_zapytaniu=cursor.selectionStart();
     p.label = new QLabel;
     p.colour = "red";
     p.text=cursor.selectedText();

     p.textzfontem = tr("<font color='%1'>%2</font>");

     p.linia=Linie[nr_linii];



     p.label->setText(p.linia.replace(p.text,p.textzfontem.arg( p.colour, p.text ).toAscii()));
     p.linia=Linie[nr_linii];
     FuzzingPayloadyLayout->addWidget(p.label);

     ListaPunktowWstrzyknkiec.append(p);

     QComboBox *combobox = new QComboBox();

        combobox->addItem("0-9");
        combobox->addItem("a-z");
        combobox->addItem("A-Z");

        combobox->addItem("XSS");
        combobox->addItem("SQLi");
        combobox->addItem("Path traversal");
        combobox->addItem(tr("Wczytaj własne"));
        combobox->setMaximumWidth(200);
        combobox->currentText();
        connect(combobox, SIGNAL(activated(int)),this, SLOT(PunktyWstrzykniecObslugaZdarzeniaComboboxa(int)));
        FuzzingPayloadyLayout->addWidget(combobox);

        ListaComboboxow.append(combobox);

     FuzzingZapytanieEdit->setText(FuzzingZapytanieEdit->toPlainText().replace(cursor.selectionStart(),cursor.selectedText().length(),"$"+cursor.selectedText()+"$"));

     for(int i=0;i<ListaPunktowWstrzyknkiec.size();++i)
     {
         if(ListaPunktowWstrzyknkiec[i].nr_pozycji_w_zapytaniu > p.nr_pozycji_w_zapytaniu)
         {
            ListaPunktowWstrzyknkiec[i].nr_pozycji_w_zapytaniu+=2;
         }
     }


 }

void SilnikSterujacy::DodajZapytanie(QAbstractItemModel *model, const QString &Nr,
             const QString &Host, const QString &Metoda, const QString &URL, const QString &Dlugosc, const QDateTime &Data)
{
    model->insertRow(0);
    model->setData(model->index(0, 0), Nr.toInt());
    model->setData(model->index(0, 1), Host);
    model->setData(model->index(0, 2), Metoda);
    model->setData(model->index(0, 3), URL);
    model->setData(model->index(0, 4), Dlugosc);
    model->setData(model->index(0, 5), Data.toString());
    //model->setUserData();

}

QAbstractItemModel *utworzModelListyPolaczen(QObject *parent)
{
    QStandardItemModel *model = new QStandardItemModel(0, 6, parent);

    model->setHeaderData(0, Qt::Horizontal, QObject::tr("Nr"));
    model->setHeaderData(1, Qt::Horizontal, QObject::tr("Host"));
    model->setHeaderData(2, Qt::Horizontal, QObject::tr("Metoda"));
    model->setHeaderData(3, Qt::Horizontal, QObject::tr("URL"));
    model->setHeaderData(4, Qt::Horizontal, QObject::tr("Długość"));
    model->setHeaderData(5, Qt::Horizontal, QObject::tr("Czas"));

    return model;
}

SilnikSterujacy::SilnikSterujacy()
{
    this->KodowanieProcentowe=1;
    this->listaPolaczenWynikowFuzzingu = new ListaPolaczen();

    this->setGeometry(0,0,1024,768);

    MainTabs = new QTabWidget(this);
        Proxy = new QWidget();
            ProxyTabs = new QTabWidget(Proxy);
                Historia = new WidgetListyPolaczen();
                Historia->FuzzingListaPunktowWstrzyknkiec = &ListaPunktowWstrzyknkiec;
                Historia->PayloadyListaComboboxow = &ListaComboboxow;
                Historia->listaPolaczenWynikowFuzzingu=&listaPolaczenWynikowFuzzingu;


            ProxyTabs->addTab(Historia,"Historia");
                //Opcje = new QWidget();
            //ProxyTabs->addTab(Opcje,"Opcje");
        Manual = new QWidget();
            ManualZapytanieEdit = new QTextEdit();
            ManualOdpowiedzEdit = new QTextEdit();
            ManualButtonUtworzZapytanie = new QPushButton(tr("Utwórz własne zapytanie"),this);
        Fuzzing = new QWidget();
            FuzzingTabs = new QTabWidget(Fuzzing);
                FuzzingPunktyWstrzykniec = new QWidget(FuzzingTabs);
                Historia->FuzzingPunktyWstrzykniec=FuzzingPunktyWstrzykniec;
                    FuzzingZapytanieEdit = new QTextEdit();
                    FuzzingButtonUstawPunkt = new QPushButton("Dodaj punkt",FuzzingPunktyWstrzykniec);

                FuzzingPayloady = new QWidget(FuzzingTabs);
                    FuzzingPayloadyLayout = new QVBoxLayout();
                        this->FuzzingButtonStart = new QPushButton(tr("Uruchom fuzzing"));
                    FuzzingPayloadyLayout->addWidget(this->FuzzingButtonStart);
                            connect ( this->FuzzingButtonStart, SIGNAL( clicked() ), this, SLOT( FuzzingKliknietoButtonStart() ) );
                    FuzzingPayloadyLayout->addWidget(new QLabel(tr("Zdefiniuj payloady dla punktów wstrzyknięć")));
                    FuzzinggroupBox = new QGroupBox(tr("Wybór algorytmu"));
                        Fuzzingradio1 = new QRadioButton(tr("Algorytm 1 - testowanie każdej zmiennej osobno"));
                            Fuzzingradio1->setChecked(true);
                        Fuzzingradio2 = new QRadioButton(tr("Algorytm 2 - iteracja krokowa"));

                    QVBoxLayout *vbox = new QVBoxLayout;
                             vbox->addWidget(Fuzzingradio1);
                             vbox->addWidget(Fuzzingradio2);
                             vbox->addStretch(1);
                    FuzzinggroupBox->setLayout(vbox);

                    FuzzingPayloadyLayout->addWidget(FuzzinggroupBox);
                 FuzzingPayloady->setLayout(FuzzingPayloadyLayout);


                WynikiFuzzing = new WidgetListyPolaczen();

                Historia->WynikiFuzzingu=&WynikiFuzzing;
                Historia->PayloadyLayout=FuzzingPayloadyLayout;
                Historia->FuzzingTabs=FuzzingTabs;


                QHBoxLayout *PunktyWstrzykniecLayout = new QHBoxLayout();
                //PunktyWstrzykniecLayout->addWidget(new QLabel(tr("Zdefiniuj punkty wstrzyknięć")));
                PunktyWstrzykniecLayout->addWidget(FuzzingButtonUstawPunkt);
                PunktyWstrzykniecLayout->addWidget(FuzzingZapytanieEdit);
                FuzzingPunktyWstrzykniec->setLayout(PunktyWstrzykniecLayout);

        QVBoxLayout *ManualLayout = new QVBoxLayout();
        ManualLayout->addWidget(ManualButtonUtworzZapytanie);
        ManualLayout->addWidget(new QLabel("Zapytanie"));
        ManualLayout->addWidget(ManualZapytanieEdit);
        ManualLayout->addWidget(new QLabel(tr("Odpowiedź")));
        ManualLayout->addWidget(ManualOdpowiedzEdit);
        Manual->setLayout(ManualLayout);

        connect ( this->ManualButtonUtworzZapytanie, SIGNAL( clicked() ), this, SLOT( ManualKliknietoButtonUtworz() ) );
        connect ( this->FuzzingButtonUstawPunkt, SIGNAL( clicked() ), this, SLOT( FuzzingKliknietoButtonDodaj()) );

    FuzzingTabs->addTab(FuzzingPunktyWstrzykniec,tr("Punkty wstrzyknięć"));
    FuzzingTabs->addTab(FuzzingPayloady,"Payloady");
    FuzzingTabs->addTab(WynikiFuzzing,tr("Fuzzing - wyniki"));

    MainTabs->addTab(Proxy,"Proxy");
    MainTabs->addTab(Manual,"Manual");
    MainTabs->addTab(Fuzzing,tr("Fuzzing"));



    ModelPolaczenProxy = utworzModelListyPolaczen(Historia);
    Historia->UstawModelZrodlowy(ModelPolaczenProxy);

    ModelPolaczenWynikow = utworzModelListyPolaczen(WynikiFuzzing);
    WynikiFuzzing->UstawModelZrodlowy(ModelPolaczenWynikow);

    Historia->ModelPolaczenWynikow=&ModelPolaczenWynikow;

    QHBoxLayout *PolautomatLayout = new QHBoxLayout();
    PolautomatLayout->addWidget(FuzzingTabs);
    Fuzzing->setMaximumWidth(1500);
    Fuzzing->setLayout(PolautomatLayout);

    QHBoxLayout *ProxyLayout = new QHBoxLayout();
    ProxyLayout->addWidget(ProxyTabs);
    Proxy->setLayout(ProxyLayout);

    QHBoxLayout *mainLayout = new QHBoxLayout();
    mainLayout->addWidget(MainTabs);
    setLayout(mainLayout);



    //showMaximized();

}




