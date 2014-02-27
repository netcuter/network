#ifndef POLAUTOMATWYNIKI_H
#define POLAUTOMATWYNIKI_H

#include "window.h"


using namespace std;


class Ramka : public QWidget
{
    Q_OBJECT

public:
    Ramka();

    QTextEdit *ManualZapytanieEdit;
    QTextEdit *FuzzZapytanieEdit;
    int nr_wiersza_kliknietego;

    QTextEdit *ZapytanieDane;
    QTextEdit *OdpowiedzDane;

    QTabWidget *Tabsy;
        QTabWidget *TabsyZapytanie;
        QTabWidget *TabsyOdpowiedz;
    void setSourceModel(QAbstractItemModel *model);

    ListaPolaczen * listaPolaczen2;

private slots:
    void filterRegExpChanged();
    void filterColumnChanged();
    void sortChanged();
    void selectionChanged();
    void ObslugaZdarzenia_customContextMenuRequested(const QPoint &pos);
    void ObslugaZdarzenia_AkcjiOdtworz();
    void ObslugaZdarzenia_AkcjiPolautomat();

private:
    QSortFilterProxyModel *proxyModel;

    QGroupBox *sourceGroupBox;
    QGroupBox *proxyGroupBox;
    QTreeView *sourceView;
    QTreeView *proxyView;
    QCheckBox *filterCaseSensitivityCheckBox;
    QCheckBox *sortCaseSensitivityCheckBox;
    QLabel *filterPatternLabel;
    QLabel *filterSyntaxLabel;
    QLabel *filterColumnLabel;
    QLineEdit *filterPatternLineEdit;
    QComboBox *filterSyntaxComboBox;
    QComboBox *filterColumnComboBox;
};

#endif // POLAUTOMATWYNIKI_H
