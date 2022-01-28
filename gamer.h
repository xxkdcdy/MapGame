#ifndef GAMER_H
#define GAMER_H

#include "node.h"
#include "edge.h"
#include "graphwidget.h"
#include <QVector>

//Gamer类，主要负责文件读取和游戏的进行。
class Gamer{
public:
    Gamer(GraphWidget* g);

    void setHealth(int h){this->health = h;}
    int getHealth(){return this->health;}
    void setWealth(int w){this->wealth = w;}
    int getWealth(){return this->wealth;}
    void setPlace(int pos){this->nowPos = pos;}
    void clear();
    bool isFinish();
    bool isFail();

    int goThere(int pos);    //主要实现角色每一步的行动

    QVector<int> *dij(QVector<int> *&result);              //迪杰斯特拉算法实现寻找生命值最高的路径值

    QVector<int> *dp(QVector<int> *&result);    //动态规划计算最大财富值路线

private:
    int health;   //生命值
    int wealth;   //财富值
    int nowPos;   //当前所在位置
    int nodeNum;  //城堡（结点）数目
    int edgeNum;  //道路（边）数目
    int** graph;  //邻接矩阵
    Node** nodeList;
    Edge** edgeList;
    bool* isGone; //结点是否已经走过

    //最大生命值到达终点（迪杰斯特拉算法）
    int* dis;     //保存起点到每个点的最短距离
    bool* vis;    //保存是否被选中
    QVector<int>* road; //保存最短路径的顺序

    //最多财富值到达终点（动态规划算法）
    int** planWealth;   //动态规划数组

};
#endif // GAMER_H
