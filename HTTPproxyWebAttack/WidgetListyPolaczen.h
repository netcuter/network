#ifndef WINDOW_H
#define WINDOW_H

#include <QWidget>
#include <QtGui>
#include <QTabWidget>
#include <vector>
#include <QtNetwork>

using namespace std;
class Polaczenie
{
public:
    QTcpSocket *proxySocket;
    QTcpSocket *socket;
    QByteArray zapytanie;
    QByteArray odpowiedz;
    QString host;
    QByteArray metoda;
    QString url;
    int nr;
};

QT_BEGIN_NAMESPACE
class QAbstractItemModel;
class QCheckBox;
class QComboBox;
class QGroupBox;
class QLabel;
class QLineEdit;
class QSortFilterProxyModel;
class QTreeView;
QT_END_NAMESPACE

class ListaPolaczen
{
public:
    vector <Polaczenie> Polaczenia;
};

class PunktWstrzykniecia
{
    public:
    PunktWstrzykniecia();
        int nr_linii;
        int nr_pozycji_w_zapytaniu;
        QByteArray linia;
        QLabel * label;
        QString colour;
        QString text;
        QString textzfontem;
        QComboBox *combobox;
        QString aktualnyPayload;
        QString domyslnyPayload;
        QString poprzedniPayload;
        bool Aktywny;
        bool Skonczono;
        QString pierwszyPayload;
        QString ostatniPayload;

        QList<QString> listaPayloadow;
        QString plik_z_lista_payloadow;
        int nr_aktualnegoPayloadu;
};




class WidgetListyPolaczen : public QWidget
{
    Q_OBJECT

public:
    WidgetListyPolaczen();

    QList<PunktWstrzykniecia> *FuzzingListaPunktowWstrzyknkiec;
    QList<QComboBox*> *PayloadyListaComboboxow;
    QVBoxLayout *PayloadyLayout;
    QTabWidget *FuzzingTabs;
    WidgetListyPolaczen **WynikiFuzzingu;
    QAbstractItemModel **ModelPolaczenWynikow;
    QAbstractItemModel *ModelWyszukiwania;
    QAbstractItemModel *ModelWyswietlany;
    QWidget *FuzzingPunktyWstrzykniec;

    QTextEdit *ManualZapytanieEdit;
    QTextEdit *ManualOdpowiedzEdit;
    QTextEdit *FuzzZapytanieEdit;
    int nr_wiersza_wybranego;

    QTextEdit *ZapytanieDane;
    QTextEdit *OdpowiedzDane;

    QTabWidget *Tabsy;
        QTabWidget *TabsyZapytanie;
        QTabWidget *TabsyOdpowiedz;
    void UstawModelZrodlowy(QAbstractItemModel *model);

    ListaPolaczen * listaPolaczen;

    ListaPolaczen **listaPolaczenWynikowFuzzingu;

private slots:
    void ObslugaZdarzeniaZmianyFrazyFiltrowania();
    void ObslugaZdarzeniaZmianySelekcjiPolaczen();
    void ObslugaZdarzeniaMenuKontekstowego(const QPoint &pos);
    void ObslugaZdarzenia_AkcjiOdtworzMenuKontekstowego();
    void ObslugaZdarzenia_AkcjiFuzzingMenuKontekstowego();

private:
    QSortFilterProxyModel *ModelListyPolaczen;

    QGroupBox *WidgetListyPolaczenGroupBox;
    QTreeView *WidokListyPolaczen;
    QLabel *FiltrowanieLabel;
    QLineEdit *FiltrowanieLineEdit;
};



#endif
