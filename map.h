#ifndef MAP_H
#define MAP_H

#include <QDialog>
#include <QMainWindow>

namespace Ui {
class Map;
}

class Map : public QDialog
{
    Q_OBJECT

public:
    explicit Map(QMainWindow *parent = 0);
    ~Map();
    QMainWindow* getParent(){return parent;}
    void append(QString s);
private:
    Ui::Map *ui;
    QMainWindow* parent;
};

#endif // MAP_H
