#include "gamer.h"
#include <QFile>
#include <QTextStream>
#include <QDebug>

Gamer::Gamer(GraphWidget* g){   //初始化角色，从文件读取游戏数据
    this->health = 100;
    this->wealth = 0;
    this->nowPos = 0;

    //打开文件，读取游戏数据
    QFile file("D:\\c++\\slesson\\MapGame\\map.txt");
    if(! file.open(QIODevice::ReadOnly|QIODevice::Text))
        qDebug()<<file.errorString();
    else
         qDebug()<<"openok";
    file.seek(0);

    //读取到城堡和道路的信息
    QTextStream in(&file);
    in >> this->nodeNum >> this->edgeNum;    //读取城堡数目和道路数目
    //qDebug()<<this->nodeNum << this->edgeNum;

    //读取城堡名称和对应财富值
    this->nodeList = new Node*[this->nodeNum];
    char name;
    int w;
    in >> name;    //读掉换行

    //构建邻接矩阵
    this->isGone = new bool[this->nodeNum];    //是否到过该城堡
    this->graph = new int*[this->nodeNum];
    for(int i = 0; i < this->nodeNum; i++){
        graph[i] = new int[this->nodeNum];
        isGone[i] = false;
        for(int j = 0; j < this->nodeNum; j++)
            graph[i][j] = 0x7fffffff;
    }
    isGone[0] = true;

    for(int i = 0; i < this->nodeNum; i++){
        in >> name >> w;
        nodeList[i] = new Node(g,i,name,w);
        qDebug() << name << w;
        in >> name;   //读掉换行
    }

    this->edgeList = new Edge*[this->edgeNum];
    int startId,endId,weight;
    for(int i = 0; i < this->edgeNum; i++){
        in >> startId >> endId >> weight;
        edgeList[i] = new Edge(nodeList[startId], nodeList[endId], weight);
        //qDebug() << startId << endId << weight;
        this->graph[startId][endId] = this->graph[endId][startId] = weight;
        in >> name;   //读掉换行
    }
}

bool Gamer::isFinish(){
    if(this->nowPos == nodeNum-1)
        return true;
    else
        return false;
}

bool Gamer::isFail(){
    if(this->health <= 0)
        return true;
    else
        return false;
}


int Gamer::goThere(int pos){
    //没有路，则不能走（状态字为0）
    if(this->graph[nowPos][pos] == 0x7fffffff){
        return 0;
    }
    //有路，且还有生命值，且尚未到达终点，就走过去（状态字为1）
     else{
        this->health -= graph[nowPos][pos];
        if(!isGone[pos]){
            isGone[pos] = true;
            this->wealth += nodeList[pos]->getWealth();
        }
        setPlace(pos);
        return 1;
    }
}

void Gamer::clear(){
    this->health = 100;
    this->wealth =0;
    this->nowPos = 0;
    for(int i = 0; i < nodeNum; i++)
        isGone[i] = false;
    isGone[0] = true;
}

//迪杰斯特拉算法求最多生命值的走法，给出角色应该走的结点序列
QVector<int>* Gamer::dij(QVector<int>*& result){
    //初始化
    int maxInt = 0x7fffffff;
    dis = new int[this->nodeNum];
    vis = new bool[this->nodeNum];
    road = new QVector<int>[this->nodeNum];
    for(int i = 1; i < this->nodeNum; i++){
        dis[i] = graph[0][i];
        vis[i] = false;
        //road[i].append(0);
        if(dis[i] != 0x7fffffff) road[i].append(i);
    }
    dis[0] = maxInt;
    vis[0] = true;

    //开始进行迭代
    while(!vis[this->nodeNum-1]){
        //选择路径数组中最小，且没有被选中的结点
        int tmp = maxInt;  //保存当前最小的路径值
        int flag = 0;      //对应的下标
        for(int i = 1; i < this->nodeNum; i++){
            if(dis[i] < tmp && !vis[i]){
                tmp = dis[i];
                flag = i;
            }
        }

        //修改路径数组和选择数组，记录一个当前的最小路径
        vis[flag] = true;
        for(int i = 0; i < this->nodeNum; i++){
            //如果新加入的结点有更短能到其他结点的路径的，就更新路径数组
            if(graph[flag][i] != 0x7fffffff && dis[flag] + graph[flag][i] < dis[i]){
                dis[i] = dis[flag] + graph[flag][i];
                road[i].erase(road[i].begin(),road[i].end());
                road[i] = road[flag];
                road[i].append(i);
            }
        }
    }
    qDebug() << road[this->nodeNum-1];
    result = &road[this->nodeNum-1];
    return &road[this->nodeNum];
}

//动态规划算法,计算从第0个结点到第no个结点，剩余heal生命值时能够得到的最大财富值，记录在planWealth数组中
QVector<int>* Gamer::dp(QVector<int> *&result){
    //初始化,定义第0个结点到第0个结点财富值为0，生命值为0时财富值为0
    this->planWealth = new int*[this->nodeNum];
    for(int i = 0; i <= this->nodeNum; i++){
        this->planWealth[i] = new int[101];
        planWealth[i][0] = 0;
    }
    for(int i = 0; i <= 100; i++) planWealth[0][i] = 0;

    //动态规划，计算是否经过该结点从而获得最大值
    for(int i = 1; i < this->nodeNum; i++){
        int tmpWealth = 0;   //记录当前最大价值
        for(int j = 1; j <= 100; j++){
            //查找从0开始每一条能走到i的路线
            for(int k = 0; k < i; k++){
                if(graph[k][i] != 0x7fffffff){    //如果有路
                    //如果生命值满足，就找当前生命值能达到的最大财富值
                    if(graph[k][i] < j){
                        if(planWealth[k][j - graph[k][i]] + nodeList[i]->getWealth() > tmpWealth)
                            tmpWealth = planWealth[k][j - graph[k][i]] + nodeList[i]->getWealth();
                    }
                }
            }
            //保存最大值
            planWealth[i][j] = tmpWealth;
        }
    }

    //结果显示
    for(int i = 0; i < nodeNum; i++)
        for(int j = 0; j <=100; j++)
            qDebug() << i << j << planWealth[i][j];

    //倒推回最优解
    this->road = new QVector<int>;

    //最后一个点和第一个点一定在序列中
    road->append(nodeNum - 1);
    int roadWealth = planWealth[nodeNum - 1][100] - nodeList[nodeNum - 1]->getWealth();  //记录当前的财富值
    int roadHealth = 100;      //记录当前的血量
    int flag = nodeNum - 1;    //记录当前所在的位置
    for(int i = nodeNum - 2; i >= 0; i--){
        if(graph[i][flag] != 0x7fffffff){
            int tmp = roadHealth - graph[i][flag];
            if(planWealth[i][tmp] == roadWealth && tmp > 0){
                //确定在该位置上取到了最优值,且保证生命值足够
                road->push_front(i);
                flag = i;
                roadHealth = tmp;
                roadWealth -= nodeList[i]->getWealth();
                qDebug() << i << roadHealth << roadWealth;
            }
        }
    }

    //去掉第一个结点（因为默认已经到达了）
    road->pop_front();
    qDebug() << roadWealth << road;
    result = &road[0];
    return &road[0];
}
