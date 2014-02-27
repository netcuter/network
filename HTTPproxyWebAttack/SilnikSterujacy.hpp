#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP

#include <QMainWindow>
#include <QDialog>
#include <QWidget>
#include <QTextEdit>
#include <QtGui>
#include "WidgetListyPolaczen.h"
#include <QtNetwork>
#include <vector>

using namespace std;


class SilnikSterujacy : public QWidget
{
    Q_OBJECT
public:
    SilnikSterujacy();
    QList<PunktWstrzykniecia> ListaPunktowWstrzyknkiec;
    QList<QComboBox*> ListaComboboxow;
    void DodajZapytanie(QAbstractItemModel *ModelPolaczenProxy, const QString &Nr,
                 const QString &Host, const QString &Metoda, const QString &URL, const QString &Dlugosc, const QDateTime &Data);
    QAbstractItemModel *ModelPolaczenProxy;
    QAbstractItemModel *ModelPolaczenWynikow;
    //QTextEdit *Edit1;
   // QTabBar *TabBar;
    QTabWidget *MainTabs;
     QWidget *Proxy;
      QTabWidget *ProxyTabs;
       QWidget *Opcje;
       WidgetListyPolaczen * Historia;
     QWidget *Manual;
      //QTabWidget *ManualTabs;
        QTextEdit * ManualZapytanieEdit;
        QTextEdit * ManualOdpowiedzEdit;
        QPushButton *ManualButtonUtworzZapytanie;
     QWidget *Fuzzing;
      QTabWidget *FuzzingTabs;
        QWidget *FuzzingPunktyWstrzykniec;
         QTextEdit * FuzzingZapytanieEdit;
         QPushButton * FuzzingButtonUstawPunkt;
        QWidget *FuzzingPayloady;
            QVBoxLayout *FuzzingPayloadyLayout;
            QPushButton *FuzzingButtonStart;
            QGroupBox *FuzzinggroupBox;
                QRadioButton *Fuzzingradio1;
                QRadioButton *Fuzzingradio2;
        WidgetListyPolaczen * WynikiFuzzing;
        ListaPolaczen *listaPolaczenWynikowFuzzingu;

        bool KodowanieProcentowe;
//     QWidget *Automat;
//      QTabWidget *AutomatTabs;

public slots:
    void ManualKliknietoButtonUtworz();
    void FuzzingKliknietoButtonDodaj();
    void FuzzingKliknietoButtonStart();
    void ManualWyslijZapytanie();
    void FuzzingWyslijZapytanie();
    void ManualtransferData();
    void FuzzingtransferData();
    void closeConnection();
    void PunktyWstrzykniecObslugaZdarzeniaComboboxa(int wybor);

private:
    //void createMenu();
    //QMenuBar *menuBar;
    //QMenu *fileMenu;
    //QAction *exitAction;
    QVBoxLayout *mainLayout;
    QHBoxLayout *ProxyLayout;


};

#endif // MAINWINDOW_HPP
