/*
 * graph.h
 *
 *  Created on: 2021��3��1��
 *      Author: ���Ӵ�
 */

/*------------Ԥ����-----------*/
#ifndef GRAPH_H_
#define GRAPH_H_
//վ���������
#define MAX_NUM 280
//һ��7����·
#define LINEACCOUNT 7
//ÿ���ߵ�վ���������
#define MAXSTATIONATLINE 40
//���㿪ʼ����
#define START 6*60
//��ʮ���������Ӫ
#define END 23*60
//��վ���г�ʱ��������
#define DRIVE 2
//ͣ��ʱ��һ����
#define STOP 1
//ÿ����ӷ�һ����
#define LAUNCH 5
//����ʱ��
#define WALK 3.5

/*------------ͷ�ļ�-----------*/
#include <cstdlib>
#include <vector>
#include <map>
#include <stack>
using namespace std;

/*-----------���ݽṹ-----------*/
// �߱�
typedef struct ArcNode
{
    // �ڽӵ��������е�λ���±�
    int adjVex;
    // ָ����һ���ڽӵ��ָ��
    struct ArcNode* next;
    //�ñ�����������
    int line;
    //��վ��ƽ������
    double distance;

}ArcNode;


//�����
typedef struct VerNode
{
    //վ��
    string name;
    //������·
    int atLine;
    //�Ƿ���Ի���
    bool canChange;
    // ����ĵ�һ����
    struct ArcNode* firstArc;
}VerNode;


//ͼ
typedef struct Graph
{
    // ��������
    struct VerNode NodeList[MAX_NUM];
    // �ܶ�����
    int totalVertex;
    // �ܱ���
    int totalArc;
}Graph;


//��line���߻�Ϊ�����±�
int LineToIndex(int id){
    switch(id){
        case 1:
            return 0;
        case 2:
            return 1;
        case 3:
            return 2;
        case 4:
            return 3;
        case 6:
            return 4;
        case 7:
            return 5;
        case 8:
            return 6;
        default:
            return -1;
    }
}
//ʱ
double hour;
//��
double minute;
//ʱ�ӷ�ȫ��ת��Ϊ��
double hAndMin;
//ÿһ���ߵĲ�ͬʱ���ӵ����
double crowds[7][3];
//ȥ���ظ�����վ��
int total;
//����վ��ID������
int IndexToLine[LINEACCOUNT] = {1,2,3,4,6,7,8};
//����ÿһ���ߵ���վƽ�����
double distances[LINEACCOUNT];
//����ÿһ���ߵ�վ����
int stations[LINEACCOUNT];
//ÿһ�㵽��ʼ�����̾���(dijkstra��)
double d[MAX_NUM];
//ÿһ�㵽��ʼ������ʱ��(dijkstraTime��)
double t[MAX_NUM];
//ÿһ�㵽��ʼ������ʱ��(dijkstraTimeCrowd��)
double tCrowd[MAX_NUM];
//ÿһ�㵽��ʼ�����̾���(dijkstraChange��)
double dc[MAX_NUM];
//����վ������ҵ����� ��ȡ�ļ���
map<int,string> nameMap;
//����վ����ŵĶ�ά���� ͬ����վ��ŷ���һ��һά������ ��ȡ�ļ���
vector<vector<int>> numberV;
//ͼ�ľ����ڽӾ���
double matrix[MAX_NUM][MAX_NUM];
//ͼ��ʱ���ڽӾ���
double timeMatrix[MAX_NUM][MAX_NUM];
//ͼ�Ŀ���ӵ���ȵ�ʱ���ڽӾ���
double timeMatrixCrowd[MAX_NUM][MAX_NUM];
//ͼ�Ļ����ڽӾ���ʱ���ڽӾ��󻻳�վ3.5��Ϊ20��
double changeMatrix[MAX_NUM][MAX_NUM];
/*------��һ��Ϊ������ӵ���ȵ����ݽṹ �ڶ���Ϊ����ӵ���ȵ����ݽṹ------*/
//dijkstra�õ��±�����
int pre[MAX_NUM];
//dijkstraTime�õ��±�����
int preT[MAX_NUM];
int preTCrowd[MAX_NUM];
//dijkstraChange�õ��±�����
int preC[MAX_NUM];
//���·��Ʊ��
int priceDis;
//���ʱ��Ʊ��
int priceTime;
int priceTimeCrowd;
//���ٻ��˴���Ʊ��
int priceChange;
//������̳���·����ջ
stack<int> sDis;
//�������ʱ��·����ջ
stack<int> sTime;
stack<int> sTimeCrowd;
//�������ٻ��ߴ���·����ջ
stack<int> sChange;

/*-----------���ܺ���-----------*/
//Ϊline����countλվ�������
int Number(int lineIndex,int count){
    return MAXSTATIONATLINE * lineIndex + count;
}


//ͼ��ʼ��
void initGraph(Graph *g){
    g->totalArc = 0;
    g->totalVertex = 0;
    for(int i = 0; i < MAX_NUM; i++){
        g->NodeList[i].canChange = false;
        g->NodeList[i].firstArc = nullptr;
        g->NodeList[i].name = "";
        g->NodeList[i].count = 1;
        for(int j = 0; j < LINEACCOUNT; j++){
            g->NodeList[i].atLine = 0;
        }
    }
}


// NumberΪpre���ڽӱ߱�����ڵ�index
void insertArcNode(Graph *g, int pre, int index, double distance,int line) {
    if (g && pre < MAX_NUM && index < MAX_NUM) {
        ArcNode* p,* q;
        //��̬�����ڴ�
        p = (ArcNode*)malloc(sizeof(ArcNode));
        //��̬�����ڴ�ʧ��
        if (!p) {
            return;
        }
        p->adjVex = index;
        p->line = line;
        p->distance = distance;
        p->next = nullptr;
        //��ʼ����ڵ�
        q = g->NodeList[pre].firstArc;
        //����׽ڵ����,�ҵ���β��Ȼ�����
        if (q) {
            while (q->next) {
                q = q->next;
            }
            q->next = p;
        }
            //����׽ڵ㲻���ڣ����׽ڵ�λ�ò���
        else {
            g->NodeList[pre].firstArc = p;
        }
        g->NodeList[pre].count++;

    }
}


// ͼ�Ĳ��뺯������������ڵ��ڽӣ����˴˲���Է����ڽӱ߱�
void insert(Graph *g, int index1, int index2, double distance,int line) {
    if (g && index1 < MAX_NUM && index2 < MAX_NUM) {
        matrix[index1][index2] = distance;
        matrix[index2][index1] = distance;
        timeMatrix[index1][index2] = DRIVE;
        timeMatrix[index2][index1] = DRIVE;
        insertArcNode(g, index1, index2, distance, line);
        insertArcNode(g, index2, index1, distance, line);
        g->totalArc++;
    }
}

#endif /* GRAPH_H_ */
