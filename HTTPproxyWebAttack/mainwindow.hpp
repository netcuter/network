#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP

#include <QMainWindow>
#include <QDialog>
#include <QWidget>
#include <QTextEdit>
#include <QtGui>
#include "window.h"
#include <QtNetwork>
#include <vector>

using namespace std;


class QButton:public QObject
{
    Q_OBJECT
public:
    QButton(QString tekst);

    QPushButton *Button;
public slots:
    void ButtonClicked();
};


class MainWindow : public QWidget
{
    Q_OBJECT
public:
    MainWindow();
    void DodajZapytanie(QAbstractItemModel *model, const QString &Nr,
                 const QString &Host, const QString &Metoda, const QString &URL, const QString &Status, const QString &Dlugosc, const QString &TypMIME, const QString &Rozszerzenie, const QString &Tytul, const QString &IP, const QString &Ciasteczko, const QDateTime &Data);
    QAbstractItemModel *model;
    //QTextEdit *Edit1;
   // QTabBar *TabBar;
    QTabWidget *MainTabs;
     QWidget *Proxy;
      QTabWidget *ProxyTabs;
       QWidget *Opcje;
       Window * Historia;
     QWidget *Manual;
      //QTabWidget *ManualTabs;
        QTextEdit * ZapytanieEdit;
        QTextEdit * OdpowiedzEdit;
        QPushButton *ButtonUtworz;
     QWidget *Polautomat;
      QTabWidget *PolautomatTabs;
        QWidget *PunktyWstrzykniec;
         QTextEdit * FuzzZapytanieEdit;
         QPushButton * ButtonUstawPunkt;
        QWidget *Payloady;
     QWidget *Automat;
      QTabWidget *AutomatTabs;

public slots:
    void KliknietoButtonUtworz();
    void KliknietoButtonDodaj();
    void WyslijZapytanie();
    void transferData();
    void closeConnection();

private:
    void createMenu();
    QMenuBar *menuBar;
    QMenu *fileMenu;
    QAction *exitAction;
    QVBoxLayout *mainLayout;
    QHBoxLayout *ProxyLayout;


};

#endif // MAINWINDOW_HPP
