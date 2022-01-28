/****************************************************************************
**
** Copyright (C) 2016 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** BSD License Usage
** Alternatively, you may use this file under the terms of the BSD license
** as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of The Qt Company Ltd nor the names of its
**     contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "graphwidget.h"
#include "edge.h"
#include "node.h"

#include <math.h>
#include <iostream>
#include <QKeyEvent>
#include <QDebug>
#include <Windows.h>
#pragma comment(lib,"user32.lib")

extern QString globaldata;
extern int nodeNumber;
extern int edgeNumber;
extern Node** nodeList;
extern Edge** edgeList;
extern int nowPos;
extern int nextPos;

void GraphWidget::Loading(){
    //打开文件，读取游戏数据
    QFile file("D:\\c++\\slesson\\MapGame\\map.txt");
    if(! file.open(QIODevice::ReadOnly|QIODevice::Text))
        qDebug()<<file.errorString();
    else
         qDebug()<<"openok";
    file.seek(0);

    //读取到城堡和道路的信息
    QTextStream in(&file);
    in >> nodeNumber >> edgeNumber;    //读取城堡数目和道路数目
    //qDebug()<<this->nodeNum << this->edgeNum;

    //读取城堡名称和对应财富值
    nodeList = new Node*[nodeNumber];
    char name;
    int w;
    in >> name;    //读掉换行

    for(int i = 0; i < nodeNumber; i++){
        in >> name >> w;
        nodeList[i] = new Node(this,i,name,w);
        //qDebug() << name << w;
        in >> name;   //读掉换行
    }

    edgeList = new Edge*[edgeNumber];
    int startId,endId,weight;
    for(int i = 0; i < edgeNumber; i++){
        in >> startId >> endId >> weight;
        edgeList[i] = new Edge(nodeList[startId], nodeList[endId], weight);
        qDebug() << startId << endId << weight;
        in >> name;   //读掉换行
    }
}

//! [0]
GraphWidget::GraphWidget(QMainWindow *parent)
    : QGraphicsView(parent), timerId(0)
{
    Loading();

    QGraphicsScene *scene = new QGraphicsScene(this);
    scene->setItemIndexMethod(QGraphicsScene::NoIndex);
    scene->setSceneRect(-300, -300, 600, 600);
    setScene(scene);
    setCacheMode(CacheBackground);
    setViewportUpdateMode(BoundingRectViewportUpdate);
    setRenderHint(QPainter::Antialiasing);
    setTransformationAnchor(AnchorUnderMouse);
    scale(qreal(0.8), qreal(0.8));
    setMinimumSize(600, 600);
    setWindowTitle(tr("Elastic Nodes"));
    //nodeNumber = 15;
    this->parent = parent;
//! [0]

//! [1]

    //nodeList = new Node*[nodeNumber];
    for(int i = 0; i < nodeNumber; i++){
        //nodeList[i] = new Node(this,i);
        scene->addItem(nodeList[i]);
    }
    nodeList[0]->setPos(-50, -50);
    centerNode = nodeList[0];
    centerNode->is_choosed = true;
    centerNode->is_placed = true;

    for(int i = 0; i < edgeNumber; i++){
        scene->addItem(edgeList[i]);
    }
}
//! [1]

//! [2]
void GraphWidget::itemMoved()
{
    if (!timerId)
        timerId = startTimer(1000 / 25);
}
//! [2]

//! [3]
void GraphWidget::keyPressEvent(QKeyEvent *event)
{
    centerNode->update();
    int next = (centerNode->getId()+1) % nodeNumber;
    int last = (centerNode->getId()-1+nodeNumber) % nodeNumber;
    switch (event->key()) {
    case Qt::Key_Up:
        centerNode = nodeList[next];
        centerNode->moveBy(0, -20);
        break;
    case Qt::Key_Down:
        centerNode = nodeList[last];
        centerNode->moveBy(0, 20);
        break;
    case Qt::Key_Left:
        centerNode->moveBy(-20, 0);
        break;
    case Qt::Key_Right:
        centerNode->moveBy(20, 0);
        break;
    case Qt::Key_Plus:
        zoomIn();
        break;
    case Qt::Key_Minus:
        zoomOut();
        break;

    case Qt::Key_M:
        nextPos = centerNode->getId();
        qDebug() << nowPos << nextPos;

        //模拟按键，使外部接收到信息，判断能否走这条路
        keybd_event('A',0,0,0);
        keybd_event('A',0,2,0);
        break;
    case Qt::Key_Y:
        nodeList[nextPos]->is_choosed = true;
        nodeList[nextPos]->is_placed = true;
        nodeList[nowPos]->is_placed = false;
        nodeList[nextPos]->update();
        nodeList[nowPos]->update();
        globaldata = QString("form %1 to %2")
                .arg(nodeList[nowPos]->getName()).arg(nodeList[nextPos]->getName());
        keybd_event('C',0,0,0);
        keybd_event('C',0,2,0);
        nowPos = nextPos;
        break;
    case Qt::Key_N:
        nextPos = nowPos;
        globaldata = QString("没有能够到达的路，当前位置 %1")
                .arg(nodeList[nowPos]->getName());
        keybd_event('C',0,0,0);
        keybd_event('C',0,2,0);
        break;
    case Qt::Key_Space:
        shuffle();
        break;
    default:
        QGraphicsView::keyPressEvent(event);
    }
}
//! [3]

//! [4]
void GraphWidget::timerEvent(QTimerEvent *event)
{
    Q_UNUSED(event);

    QList<Node *> nodes;
    foreach (QGraphicsItem *item, scene()->items()) {
        if (Node *node = qgraphicsitem_cast<Node *>(item))
            nodes << node;
    }

    foreach (Node *node, nodes)
        node->calculateForces();

    bool itemsMoved = false;
    foreach (Node *node, nodes) {
        if (node->advance())
            itemsMoved = true;
    }

    if (!itemsMoved) {
        killTimer(timerId);
        timerId = 0;
    }
}
//! [4]

#if QT_CONFIG(wheelevent)
//! [5]
void GraphWidget::wheelEvent(QWheelEvent *event)
{
    scaleView(pow((double)2, -event->delta() / 240.0));
}
//! [5]
#endif

//! [6]
void GraphWidget::drawBackground(QPainter *painter, const QRectF &rect)
{
    Q_UNUSED(rect);

    // Shadow
    QRectF sceneRect = this->sceneRect();
    QRectF rightShadow(sceneRect.right(), sceneRect.top() + 5, 5, sceneRect.height());
    QRectF bottomShadow(sceneRect.left() + 5, sceneRect.bottom(), sceneRect.width(), 5);
    if (rightShadow.intersects(rect) || rightShadow.contains(rect))
        painter->fillRect(rightShadow, Qt::darkGray);
    if (bottomShadow.intersects(rect) || bottomShadow.contains(rect))
        painter->fillRect(bottomShadow, Qt::darkGray);

    // Fill
    QLinearGradient gradient(sceneRect.topLeft(), sceneRect.bottomRight());
    gradient.setColorAt(0, Qt::white);
    gradient.setColorAt(1, Qt::lightGray);
    painter->fillRect(rect.intersected(sceneRect), gradient);
    painter->setBrush(Qt::NoBrush);
    painter->drawRect(sceneRect);

    // Text
    QRectF textRect(sceneRect.left() + 4, sceneRect.top() + 4,
                    sceneRect.width() - 4, sceneRect.height() - 4);
    QString message(tr("黄色代表角色当前位置，红色代表尚未到达过，绿色代表已经到达过。\n "
                       "上下方向键选择目标城堡，按M前往城堡"));

    QFont font = painter->font();
    font.setBold(true);
    font.setPointSize(14);
    painter->setFont(font);
    painter->setPen(Qt::lightGray);
    painter->drawText(textRect.translated(2, 2), message);
    painter->setPen(Qt::black);
    painter->drawText(textRect, message);
}
//! [6]

//! [7]
void GraphWidget::scaleView(qreal scaleFactor)
{
    qreal factor = transform().scale(scaleFactor, scaleFactor).mapRect(QRectF(0, 0, 1, 1)).width();
    if (factor < 0.07 || factor > 100)
        return;

    scale(scaleFactor, scaleFactor);
}
//! [7]

void GraphWidget::shuffle()
{
    foreach (QGraphicsItem *item, scene()->items()) {
        if (qgraphicsitem_cast<Node *>(item))
            item->setPos(-150 + qrand() % 300, -150 + qrand() % 300);
    }
}

void GraphWidget::zoomIn()
{
    scaleView(qreal(1.2));
}

void GraphWidget::zoomOut()
{
    scaleView(1 / qreal(1.2));
}
