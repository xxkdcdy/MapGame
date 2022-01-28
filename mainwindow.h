#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <map.h>
#include <QKeyEvent>
#include "graphwidget.h"
#include "gamer.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void keyPressEvent(QKeyEvent *event);
    Map* getMap(){return m;}
    GraphWidget* getGraph(){return g;}
private slots:
    void on_gameStarter_clicked();

    void on_bestRoad_clicked();

private:
    Ui::MainWindow *ui;
    Map* m;
    GraphWidget* g;
    Gamer* gamer;
};

#endif // MAINWINDOW_H
