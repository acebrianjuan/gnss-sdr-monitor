#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include <QMainWindow>
#include <QAbstractTableModel>
#include <QtNetwork/QUdpSocket>
#include <QSettings>
#include <QQmlApplicationEngine>
#include <QQuickWidget>
#include <QQuickView>
#include <QQuickItem>
#include <QGeoCoordinate>

#include "gnss_synchro.h"
#include "channel_table_model.h"
#include "qcustomplot.h"

namespace Ui {
class Main_Window;
}

class Main_Window : public QMainWindow
{
    Q_OBJECT
    //Q_PROPERTY(QGeoCoordinate position READ position WRITE set_position NOTIFY position_changed)

public:
    explicit Main_Window(QWidget *parent = nullptr);
    ~Main_Window();
    std::vector<Gnss_Synchro> read_gnss_synchro(char buff[], int bytes);
    void save_settings();
    void load_settings();

private:
    Ui::Main_Window *ui;
    //QQmlApplicationEngine *engine;
    QQuickWidget *map_widget;
    //QQuickView *map;
    QQuickItem *item;
    QTimer *timer;
    Channel_Table_Model *model;
    QUdpSocket *socket;
    std::vector<Gnss_Synchro> stocks;
    std::vector<int> channels;
    unsigned int port;
    QSettings settings;

    QAction *start;
    QAction *stop;
    QAction *clear;

    int buffer_size;

public slots:
    void toggle_capture();
    void add_entry();
    void clear_entries();
    //void set_map_location();
    void quit();
    void show_preferences();
    void set_port();

};

#endif // MAIN_WINDOW_H
