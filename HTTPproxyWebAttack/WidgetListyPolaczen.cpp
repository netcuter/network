#include <QtGui>
#include <QTextCodec>
#include "WidgetListyPolaczen.h"




WidgetListyPolaczen::WidgetListyPolaczen()
{
    nr_wiersza_wybranego = -1;
    QTextCodec::setCodecForTr (QTextCodec::codecForName ("UTF-8"));

    ModelListyPolaczen = new QSortFilterProxyModel;
    ModelListyPolaczen->setDynamicSortFilter(true);

    WidgetListyPolaczenGroupBox = new QGroupBox(tr("Lista Połączeń"));


    WidokListyPolaczen = new QTreeView;
    WidokListyPolaczen->setRootIsDecorated(false);
    WidokListyPolaczen->setAlternatingRowColors(true);
    WidokListyPolaczen->setModel(ModelListyPolaczen);
    WidokListyPolaczen->setSortingEnabled(true);

    ModelWyswietlany=0;
        connect(WidokListyPolaczen->selectionModel(), SIGNAL(selectionChanged(const QItemSelection &, const QItemSelection &)),
                    this, SLOT(ObslugaZdarzeniaZmianySelekcjiPolaczen()));

        WidokListyPolaczen->setContextMenuPolicy(Qt::CustomContextMenu);

            connect(WidokListyPolaczen,
                SIGNAL(customContextMenuRequested(const QPoint &)),
                this,
                SLOT(ObslugaZdarzeniaMenuKontekstowego(const QPoint &)));



    FiltrowanieLineEdit = new QLineEdit;
    FiltrowanieLabel = new QLabel(tr("Fraza w odpowiedziach do filtrowania listy połączeń :"));
    FiltrowanieLabel->setBuddy(FiltrowanieLineEdit);

    connect(FiltrowanieLineEdit, SIGNAL(textChanged(QString)),
            this, SLOT(ObslugaZdarzeniaZmianyFrazyFiltrowania()));

    QGridLayout *proxyLayout = new QGridLayout;
    proxyLayout->addWidget(WidokListyPolaczen, 0, 0, 1, 3);
    proxyLayout->addWidget(FiltrowanieLabel, 1, 0);
    proxyLayout->addWidget(FiltrowanieLineEdit, 1, 1, 1, 2);
    WidgetListyPolaczenGroupBox->setLayout(proxyLayout);

    Tabsy=new QTabWidget(this);
        TabsyZapytanie=new QTabWidget(Tabsy);
            ZapytanieDane=new QTextEdit(TabsyZapytanie);
        TabsyZapytanie->addTab(ZapytanieDane,"Dane");
        //    QTextEdit *ZapytanieParametry=new QTextEdit(TabsyZapytanie);
        //TabsyZapytanie->addTab(ZapytanieParametry,"Parametry");
        //    QTextEdit *ZapytanieNaglowki=new QTextEdit(TabsyZapytanie);
        //TabsyZapytanie->addTab(ZapytanieNaglowki,tr("Nagłówki"));

        TabsyOdpowiedz=new QTabWidget(Tabsy);
            OdpowiedzDane=new QTextEdit(TabsyOdpowiedz);
        TabsyOdpowiedz->addTab(OdpowiedzDane,"Dane");
        //    QTextEdit *OdpowiedzNaglowki=new QTextEdit(TabsyOdpowiedz);
        //TabsyOdpowiedz->addTab(OdpowiedzNaglowki,tr("Nagłówki"));
        //    QTextEdit *Odpowiedzhtml=new QTextEdit(TabsyOdpowiedz);
        //TabsyOdpowiedz->addTab(Odpowiedzhtml,tr("html"));


    Tabsy->addTab(TabsyZapytanie,"Zapytanie");
    Tabsy->addTab(TabsyOdpowiedz,tr("Odpowiedź"));

    QSplitter *splitter = new QSplitter;
        //QTextEdit *memo = new QTextEdit;
    splitter->addWidget(WidgetListyPolaczenGroupBox);


    splitter->addWidget(Tabsy);
    //splitter->addWidget(memo);
    splitter->setOrientation(Qt::Vertical);


    QVBoxLayout *mainLayout = new QVBoxLayout;
    //mainLayout->addWidget(sourceGroupBox);
    mainLayout->addWidget(splitter);
    setLayout(mainLayout);

    setWindowTitle(tr("Basic Sort/Filter Model"));
    //resize(500, 450);

    WidokListyPolaczen->sortByColumn(0, Qt::AscendingOrder);

    FiltrowanieLineEdit->setText("");
}

void WidgetListyPolaczen::UstawModelZrodlowy(QAbstractItemModel *model)
{
    ModelListyPolaczen->setSourceModel(model);
    ModelWyswietlany=model;


}

QAbstractItemModel *StworzModelListyPolaczen(QObject *parent)
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

void DodajZapytanie(QAbstractItemModel *model, const QString &Nr,
                    const QString &Host, const QString &Metoda, const QString &URL, const QString &Dlugosc, const QDateTime &Data)
       {
           model->insertRow(0);
           model->setData(model->index(0, 0), Nr.toInt());
           model->setData(model->index(0, 1), Host);
           model->setData(model->index(0, 2), Metoda);
           model->setData(model->index(0, 3), URL);
           model->setData(model->index(0, 4), Dlugosc);
           model->setData(model->index(0, 5), Data.toString());

}

QAbstractItemModel *StworzModelWyszukiwania(QObject *parent)
{
    QStandardItemModel *model = new QStandardItemModel(0, 12, parent);

    model->setHeaderData(0, Qt::Horizontal, QObject::tr("Nr"));
    model->setHeaderData(1, Qt::Horizontal, QObject::tr("Zapytanie"));
    model->setHeaderData(2, Qt::Horizontal, QObject::tr("Odpowiedź"));

    return model;
}

void DodajPolaczenieModeluPrzeszukiwania(QAbstractItemModel *model, int Nr,
             const QString &Zapytanie, const QString &Odpowiedz)
{
    model->insertRow(0);
    model->setData(model->index(0, 0), Nr);
    model->setData(model->index(0, 1), Zapytanie);
    model->setData(model->index(0, 2), Odpowiedz);
    //model->setUserData();

}

void WidgetListyPolaczen::ObslugaZdarzeniaZmianyFrazyFiltrowania()
{

    QString wyszukiwana_fraza = FiltrowanieLineEdit->text();
    if(wyszukiwana_fraza!="")
    {
        ModelWyszukiwania = StworzModelListyPolaczen(this);
        for(int i=0;i<listaPolaczen->Polaczenia.size();++i)
        {
            Polaczenie p = listaPolaczen->Polaczenia[i];
            if(p.odpowiedz.indexOf(wyszukiwana_fraza)>-1)
            {
                DodajZapytanie(ModelWyszukiwania,QString::number(p.nr+1),p.host,p.metoda,p.url,QString::number(p.odpowiedz.length()),QDateTime::currentDateTime());
            }
        }


        ModelListyPolaczen->setSourceModel(ModelWyszukiwania);

    }
    else
    {
        if(ModelWyswietlany)
        {
            ModelListyPolaczen->setSourceModel(ModelWyswietlany);
        }
    }

}




void WidgetListyPolaczen::ObslugaZdarzeniaZmianySelekcjiPolaczen()
{
    if( WidokListyPolaczen->selectionModel()->hasSelection())
    {
        // Capture all rows to be deleted by it's row number in list
         QList<int> rowList;
         foreach( QModelIndex index, WidokListyPolaczen->selectionModel()->selectedIndexes() )
         {
             rowList << index.row();

             nr_wiersza_wybranego = index.row();
             QModelIndex parent = index.parent();
            // QString nr_polaczenia=proxyModel->data(proxyModel->index(0, 0));

             int nr_polaczenia = ModelListyPolaczen->data(ModelListyPolaczen->index(nr_wiersza_wybranego, 0, parent)).toInt(0) -1;

             Polaczenie p = listaPolaczen->Polaczenia[nr_polaczenia];
             ZapytanieDane->setText(p.zapytanie);
             OdpowiedzDane->setText(p.odpowiedz);

         }

    }

}

void WidgetListyPolaczen::ObslugaZdarzeniaMenuKontekstowego(const QPoint &pos)
{

        QMenu *menu = new QMenu();
        QAction *AkcjaOdtworz = new QAction(tr("Odtwórz zapytanie ręcznie"), this);
        QAction *AkcjaPolautomat = new QAction(tr("Wrzuć do fuzzera"), this);

        menu->addAction(AkcjaOdtworz);
        menu->addAction(AkcjaPolautomat);
        connect( AkcjaOdtworz, SIGNAL(triggered()), this, SLOT(ObslugaZdarzenia_AkcjiOdtworzMenuKontekstowego()) );
        connect( AkcjaPolautomat, SIGNAL(triggered()), this, SLOT(ObslugaZdarzenia_AkcjiFuzzingMenuKontekstowego()) );
        //setupAction->
        //menu->addAction(monitorAction);


        menu->exec(QCursor::pos());

}

void WidgetListyPolaczen::ObslugaZdarzenia_AkcjiOdtworzMenuKontekstowego()
{
    if(nr_wiersza_wybranego > -1)
    {
        ManualZapytanieEdit->setText(ZapytanieDane->toPlainText());
        ManualOdpowiedzEdit->setText("");
    }
}



void WidgetListyPolaczen::ObslugaZdarzenia_AkcjiFuzzingMenuKontekstowego()
{
    if(nr_wiersza_wybranego > -1)
    {
        for(int i=0;i<FuzzingListaPunktowWstrzyknkiec->size();++i)
        {
           // PunktWstrzykniecia *p;
            //p=(ListaPunktowWstrzyknkiec[i])

            QList<QComboBox*> *lista = PayloadyListaComboboxow;
            QComboBox *c = lista->value(lista->size()-1);
            c->deleteLater();
            lista->removeAt(lista->size()-1);
            FuzzingListaPunktowWstrzyknkiec->value(i).label->deleteLater();
            //PayloadyLayout->removeWidget(c);
        }

        if(FuzzingListaPunktowWstrzyknkiec->size()>0)
        {
            FuzzingListaPunktowWstrzyknkiec->clear();
            WidgetListyPolaczen *r = new WidgetListyPolaczen();
            *WynikiFuzzingu = r;
            *ModelPolaczenWynikow = StworzModelListyPolaczen(r);
            r->UstawModelZrodlowy(*ModelPolaczenWynikow);
            QString nazwa = tr("Wyniki Fuzzingu ");
            int nr = FuzzingTabs->count()+1-2;
            nazwa+=QString::number(nr);
            FuzzingTabs->addTab(r,nazwa);
            FuzzingTabs->setCurrentIndex(0);
            ListaPolaczen *l = new ListaPolaczen();
            *listaPolaczenWynikowFuzzingu = l;
            r->listaPolaczen=l;

        }
        FuzzZapytanieEdit->setText(ZapytanieDane->toPlainText());
    }
}
