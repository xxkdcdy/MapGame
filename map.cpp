#include "map.h"
#include "ui_map.h"
#include "graphwidget.h"
#include <QApplication>
#include <QTime>
#include <QMainWindow>

Map::Map(QMainWindow *parent) :
    QDialog(parent),
    ui(new Ui::Map)
{
    ui->setupUi(this);
    this->parent = parent;
    ui->textEdit->setReadOnly(true);
}

Map::~Map()
{
    delete ui;
}

void Map::append(QString s){
    ui->textEdit->append(s);
}
