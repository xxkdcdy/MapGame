#include <QApplication>
#include <QTime>
#include <QMainWindow>
#include <iostream>
#include <QTime>
#include <QDebug>
#include "graphwidget.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "gamer.h"

#include <Windows.h>
#pragma comment(lib,"user32.lib")

bool fac = false;
QString globaldata;
int nodeNumber;
int edgeNumber;
Node** nodeList;
Edge** edgeList;
int nowPos;
int nextPos;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_gameStarter_clicked()
{
    //开始游戏按钮被按下
    qsrand(QTime(0,0,0).secsTo(QTime::currentTime()));

    //角色和游戏数据加载
    this->gamer = new Gamer(g);
    nowPos = nextPos = 0;

    //游戏提示窗口加载
    m = new Map(this);
    m->setModal(false);
    m->show();
    m->append("start\n");
    m->move(QPoint(-10,0));

    //游戏界面加载
    GraphWidget *widget = new GraphWidget(this);
    this->g = widget;
    this->setCentralWidget(g);
    this->setWindowFlags(Qt::WindowStaysOnTopHint);
    this->show();
    move(400,80);
    fac = true;
}

void MainWindow::on_bestRoad_clicked()
{
    on_gameStarter_clicked();
    QTime time;
    time.start();
    while(time.elapsed() < 1000)             //等待时间流逝1秒钟
        QCoreApplication::processEvents();   //处理事件

    QVector<int>* list;
    if(ui->radioButton->isChecked()){
        m->append("以最大生命值完成路径");
        gamer->dij(list);
        while(!list->empty()){
            nextPos = *list->begin();
            list->pop_front();
            int flag = gamer->goThere(nextPos);
            if(flag == 1){
                keybd_event('Y',0,0,0);
                keybd_event('Y',0,2,0);
            }
            else if(flag == 0){
                //不满足过路条件，返回
                keybd_event('N',0,0,0);
                keybd_event('N',0,2,0);
                nextPos = nowPos;
            }

            time.start();
            while(time.elapsed() < 1000)             //等待时间流逝1秒钟
                QCoreApplication::processEvents();   //处理事件
        }
        qDebug() << list;
    }
    else{
        m->append("以最多财富值完成路径");
        gamer->dp(list);
        while(!list->empty()){
            nextPos = *list->begin();
            list->pop_front();
            int flag = gamer->goThere(nextPos);
            if(flag == 1){
                keybd_event('Y',0,0,0);
                keybd_event('Y',0,2,0);
            }
            else if(flag == 0){
                //不满足过路条件，返回
                keybd_event('N',0,0,0);
                keybd_event('N',0,2,0);
                nextPos = nowPos;
            }

            time.start();
            while(time.elapsed() < 1000)             //等待时间流逝1秒钟
                QCoreApplication::processEvents();   //处理事件
        }
        qDebug() << list;
    }
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    // 是否按下Ctrl键
    if(event->modifiers() == Qt::ControlModifier)
    {
        // 是否按下M键
        if(event->key() == Qt::Key_Q)
            on_gameStarter_clicked();
            // 窗口最大化
            //setWindowState(Qt::WindowMaximized);
        if(event->key() == Qt::Key_W)
            on_bestRoad_clicked();
    }
    else if(event->key() == Qt::Key_Escape && fac){
        m->append("end");
        ui->setupUi(this);
        fac = false;
    }
    else if(event->key() == Qt::Key_Q && fac){
        on_gameStarter_clicked();
    }
    else if(event->key() == Qt::Key_A && fac){
        int flag = gamer->goThere(nextPos);
        if(flag == 1){
            keybd_event('Y',0,0,0);
            keybd_event('Y',0,2,0);
        }
        else if(flag == 0){
            //不满足过路条件，返回
            keybd_event('N',0,0,0);
            keybd_event('N',0,2,0);
            nextPos = nowPos;
        }
    }
    else if(event->key() == Qt::Key_C && fac){
        m->append(globaldata);
        int h = gamer->getHealth();
        int w = gamer->getWealth();
        globaldata = QString("当前生命值为 %1， 财富值为 %2")
                .arg(QString::number(h)).arg(QString::number(w));
        m->append(globaldata);
        if(gamer->isFail()){
            globaldata = QString("血量不足，游戏失败！");
            m->append(globaldata);
            QTime time;
            time.start();
            ui->setupUi(this);
            fac = false;
        }
        if(gamer->isFinish() && !gamer->isFail()){
            globaldata = QString("到达终点，游戏成功！");
            m->append(globaldata);
            QTime time;
            time.start();
            while(time.elapsed() < 3000)             //等待时间流逝3秒钟
                QCoreApplication::processEvents();   //处理事件
            this->takeCentralWidget();
            ui->setupUi(this);
            fac = false;
        }
    }
    else QWidget::keyPressEvent(event);
}
