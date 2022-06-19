/*
 * mainfunction.h
 *
 *  Created on: 2021��3��1��
 *      Author: ���Ӵ�
 */
#ifndef METRO_MAINFUNCTION_H
#define METRO_MAINFUNCTION_H

#include <fstream>
#include <iostream>
#include <iomanip>
#include <cstring>
#include <set>

#include "graph.h"
#include "compute.h"

//�ļ���
#define INPUT "InputData.txt"
#define DATA "MetroData.txt"
ofstream ofs;
//����ʱ��ʱ�Ƿ���ӵ����
bool considerCrowd;
//�Ƿ��Լ�����ӵ����
bool ChangeCrowd;
//�Ƿ�ѡ��ܿ��ض���ӵ������·
bool avoid;
//�趨��Ҫ�ܿ���ӵ��������
double setedCrowd;

using namespace std;


//���ջ
void clearStack(stack<int> &s){
    s = stack<int>();
}


//����·��Ϣ��վ����Ϣ�������ļ�����ϵͳ�������й�ͼ
void read(Graph *g){
    for (int l = 0; l < MAX_NUM; ++l) {
        for (int i = 0; i < MAX_NUM; ++i) {
            matrix[l][i] = INT16_MAX;
        }
    }
    for (int m = 0; m < MAX_NUM; ++m) {
        for (int i = 0; i < MAX_NUM; ++i) {
            timeMatrix[m][i] = INT16_MAX;
        }
    }
    //����s�����ж��Ƿ����ĳһվ
    set<string> s;
    //��ʼ����վ��Ϊ0
    total = 0;
    ifstream in;
    //���ļ�
    in.open(DATA);
    for(int i = 0; i < LINEACCOUNT; i++){
        //�������ݵı���
        int id,people,account;
        double length;
        //������
        int pre,index;
        //��ȡ��Ϣ
        in >> id >> people >> length >> account;
        stations[i] = account;
        double distance = length / (account - 1);
        distances[i] = distance;
        for(int j = 0; j < account; j++){
            index = Number(i, j);
            int number;
            string name;
            in >> number >> name;
            nameMap.insert(pair<int,string>(index,name));
            //֮ǰû�е����վ
            if(!s.count(name))
            {
                s.insert(name);
                total++;
            }
            g->totalVertex++;
            g->NodeList[index].name = name;
            g->NodeList[index].atLine = IndexToLine[i];
            //���j��Ϊ0 ���Ѿ���������������վ ��ʱpre�Ѿ�����ֵ
            if(j > 0){
                insert(g,pre,index,distance,IndexToLine[i]);
            }
            pre = index;
        }
    }
    //�ر��ļ�
    in.close();
    for(int i = 0; i < LINEACCOUNT; i++){
        for(int j = 1; j < stations[i] - 2;j++){
            timeMatrix[Number(i,j)][Number(i,j+1)] = STOP + DRIVE;
        }
        for(int j = stations[i] - 2; j > 0; j--){
            timeMatrix[Number(i,j)][Number(i,j-1)] = STOP + DRIVE;
        }
    }
    //��ʼ��nameMap��numberV
    //�ж�map�е�Ԫ���Ƿ���ӽ�vector��
    bool flags[MAX_NUM];
    for(bool & flag : flags){
        flag = false;
    }
    for(auto i = nameMap.begin();i != nameMap.end(); i++){
        vector<int> v;
        if(!flags[i->first]){
            v.push_back(i->first);
            flags[i->first] = true;
        }
        for(auto & j : nameMap){
            if(i->second == j.second && !flags[j.first]){
                v.push_back(j.first);
                flags[j.first] = true;
            }
        }
        if(!v.empty()){
            numberV.push_back(v);
        }
    }

    for(auto & i : numberV){
        if(i.size() > 1){
            for(auto j = i.begin(); j != i.end(); j++){
                //��ʼ��վ�Ƿ���Ի���
                g->NodeList[*j].canChange = true;
                //��������ͬindex��ͬ��վ�������� ������Ϊ0
                for(auto k = j; k != i.end(); k++){
                    if(k != j){
                        matrix[*k][*j] = 0;
                        matrix[*j][*k] = 0;
                        timeMatrix[*k][*j] = WALK;
                        timeMatrix[*j][*k] = WALK;
                        insertArcNode(g, *k, *j, 0, g->NodeList[*j].atLine);
                        insertArcNode(g, *j, *k, 0, g->NodeList[*k].atLine);
                        g->totalArc++;
                    }
                }
            }
        }
    }
    //��ʼ�������ڽӾ���
    for(int i = 0; i < MAX_NUM; i++){
        for (int j = 0; j < MAX_NUM; ++j) {
            if(g->NodeList[i].name == g->NodeList[j].name){
                changeMatrix[i][j] = 20;
            } else{
                changeMatrix[i][j] = timeMatrix[i][j];

            }
        }
    }
    //��ʼ������ӵ���ȵ�ʱ�����Ϊʱ���ڽӾ���
    for (int i = 0; i < MAX_NUM; ++i) {
        for (int j = 0; j < MAX_NUM; ++j) {
            timeMatrixCrowd[i][j] = timeMatrix[i][j];
        }
    }
    //��ʼ��//ÿһ���ߵĲ�ͬʱ���ӵ����
    /*���°����ͣ�1 ���ߣ�3 ���ߣ�
    7:30-9:00��ӵ���� 80%��16:30-18:30��ӵ���� 75%������ʱ��ӵ���� 40%��
    */
    crowds[0][0] = 0.8;
    crowds[0][1] = 0.75;
    crowds[0][2] = 0.4;
    crowds[2][0] = 0.8;
    crowds[2][1] = 0.75;
    crowds[2][2] = 0.4;
    /*�Ǽʽ�ͨ���ͣ�2 ���ߣ�4 ���ߣ�
    ȫ��ӵ���� 50%��
    */
    for (int i = 0; i < 3; ++i) {
        crowds[1][i] = 0.5;
        crowds[3][i] = 0.5;
    }
    /*
    �������ͣ�8 ���ߣ�
    9:30-15:00��ӵ���� 65%������ʱ��ӵ���� 20%
     */
    crowds[6][0] = 0.65;
    crowds[6][1] = 0.2;
    crowds[6][2] = 0.2;
    /*�������ͣ�6 ���ߣ�7 ���ߣ�
    19:00-22:00��ӵ���� 65%������ʱ��ӵ���� 15%��
    */
    crowds[4][0] = 0.65;
    crowds[4][1] = 0.15;
    crowds[4][2] = 0.15;
    crowds[5][0] = 0.65;
    crowds[5][1] = 0.15;
    crowds[5][2] = 0.15;
    clearStack(sDis);
    clearStack(sTime);
    clearStack(sTimeCrowd);
    clearStack(sChange);
}


//�ж�������·�Ƿ����
bool existLine(int line){
    return line == 1 || line == 2 || line == 3 || line == 4 || line == 6 || line == 7 || line == 8;
}


//�ж�����վ���Ƿ����
bool existStation(Graph g,string name){
    for(auto & i : numberV){
        if(g.NodeList[i[0]].name == name){
            return true;
        }
    }
    return false;
}


//��ָ����·��վ��������ʾ����
void print(Graph g){
    int line;
    while(true) {
        cout << "������Ҫ��ѯ����·��";
        cin >> line;
        ofs<<line<<endl;
        if(existLine(line)){
            break;
        }else{
            cout<<"������·�������������롣"<<endl;
        }
    }
    for (int i = 1; i <= stations[LineToIndex(line)]; ++i) {
        cout << setw(2) <<i<< "." << g.NodeList[Number(LineToIndex(line), i - 1)].name<<"\t";
        if (i % 5 == 0) {
            cout << endl;
        }
    }
    cout<<endl;
}


//����line����վ��name�����
int getStationNumber(Graph g,int line,const string& name){
    for(auto & i : nameMap){
        if(i.second == name){
            if(g.NodeList[i.first].atLine == line){
                return i.first;
            }
        }
    }
    return -1;
}


/***
 * ָ����·����ʼվ��
 * ���Ӹ�վ�㿪ʼ���ҳ���һվ��
 * ���ڻ���վ����л���
 * ʵ�ֶԵ���·��վ����������
 * */
void nextOrChange(Graph g){
    int startL;
    while(true){
        cout<<"��������ʼ��·��";
        cin>>startL;
        ofs<<startL<<endl;
        if(existLine(startL)){
            break;
        }else{
            cout<<"������·�������������롣"<<endl;
        }
    }
    string startStationName;
    while(true){
        cout<<"��������ʼվ�㣺";
        cin>>startStationName;
        ofs<<startStationName<<endl;
        if(existStation(g, startStationName)){
            break;
        }else{
            cout<<"����վ���������������롣"<<endl;
        }
    }
    int index = getStationNumber(g, startL, startStationName);
    while(true){
        //�ж��Ƿ���Ի���
        char in;
        if(g.NodeList[index].canChange){
            while(true){
                cout<<"��ǰվ���Ի��ˣ���ѡ���Ƿ񻻳ˣ��ǣ�y/Y,��n/N����";
                cin>>in;
                ofs<<in<<endl;
                if(in != 'y' && in != 'n' && in != 'Y' && in != 'N'){
                    cout<<"����ѡ����������������"<<endl;
                }
                else{
                    break;
                }
            }
            if(in == 'y' || in == 'Y'){
                int change = 0;
                while(true){
                    bool transfer = false;
                    cout<<"�ɻ��˵����У�"<<endl;
                    //����numberV�ҵ�index�ڵ�һά����
                    auto find = numberV.begin();
                    bool flag = false;
                    for(auto i = numberV.begin(); i != numberV.end(); i++){
                        if(i->size() > 1){
                            for(auto j = i->begin(); j != i->end(); j++){
                                if(*j == index){
                                    flag = true;
                                    find = i;
                                    break;
                                }
                            }
                        }
                        if(flag){
                            break;
                        }
                    }
                    for(auto i = find->begin(); i != find->end(); i++){
                        if(*i != index){
                            cout<<g.NodeList[*i].atLine<<"����"<<endl;
                        }
                    }
                    cout<<"��ѡ�񻻳��������ߣ�"<<endl;
                    cin>>change;
                    ofs<<change<<endl;
                    for(auto i = find->begin(); i != find->end(); i++){
                        if(*i != index){
                            if(g.NodeList[*i].atLine == change){
                                index = *i;
                                startL = change;
                                transfer = true;
                                break;
                            }
                        }
                    }
                    if(transfer){
                        cout<<"���ѻ�����"<<g.NodeList[index].atLine<<"����"<<endl;
                        break;
                    }
                    cout<<"������·�������������롣"<<endl;
                }
                //change��Ϊ0˵�����߳ɹ� ���ǻ��ߺ���ȻҪ�ж��Ƿ񻻳� ��֤ϵͳ������
                if(change != 0){
                    continue;
                }
            }
        }
        while(true){
            cout<<"�Ƿ��ѯ��һվ�㣨�ǣ�y/Y,��n/N����";
            cin>>in;
            ofs<<in<<endl;
            if(in != 'y' && in != 'n' && in != 'Y' && in != 'N'){
                cout<<"����ѡ����������������"<<endl;
            }
            else{
                break;
            }
        }
        if(in == 'N' || in == 'n'){
            break;
        }
        else{
            while(true){
                ArcNode* p = g.NodeList[index].firstArc;
                cout<<"��һվ�ǣ�"<<endl;
                while(p){
                    if(p->distance != 0){
                        cout<<g.NodeList[p->adjVex].name<<endl;
                    }
                    p = p->next;
                }
                cout<<"��������Ҫ��ѯ����һվ��";
                string next;
                cin>>next;
                ofs<<next<<endl;
                int to = getStationNumber(g, startL, next);
                if(to == -1){
                    cout<<"����վ���������������롣"<<endl;
                }else{
                    index = to;
                    cout<<"�ѱ��Ϊ"<<g.NodeList[index].name<<endl;
                    break;
                }
            }
        }
    }
}


/*
 * ����һ��ģ���人��������Ҫת����·ѯ��ϵͳ��
 * ����������ͬ����ʼվ���Ŀ��վ�㣬���õ�ǰʱ��
 * ���� 1-3 ��ת����·�Ľ���
 * ��������Ҫ���ѵ�ʱ���Ʊ��
 * �Լ����㲢��ʾ��·�и���·�ߵ�ӵ���̶�
 * */
//���õ�ǰʱ��
void setTime(){
    while(true){
        cout<<"�����뵱ǰʱ(0~23)��";
        cin>>hour;
        ofs<<hour<<endl;
        if(hour < 0 || hour > 23){
            cout<<"����������������롣"<<endl;
        }else{
            break;
        }
    }
    while(true){
        cout<<"�����뵱ǰ��(0~59)��";
        cin>>minute;
        ofs<<minute<<endl;
        if(minute < 0 || minute > 59){
            cout<<"����������������롣"<<endl;
        }else{
            break;
        }
    }
    hAndMin = 60 * hour + minute;
}


//ƥ�䵱ǰʱ�̵�ǰ����ٷֵļ����ߵĽ�վ�ͳ�վʱ��


void matchTime(Graph g){
    setTime();
    int line;
    int range;
    int index;
    string name;
    while(true){
        cout<<"������Ҫ��ѯ����·��";
        cin>>line;
        ofs<<line<<endl;
        cout<<"������Ҫ��ѯ��վ�㣺";
        cin>>name;
        ofs<<name<<endl;
        index = getStationNumber(g, line, name);
        if(index != -1){
            break;
        }else{
            cout<<"����վ�����·�������������롣"<<endl;
        }
    }
    while(true){
        cout<<"������Ҫ��ѯ��ʱ�䷶Χ����������λ�����ӣ���";
        cin>>range;
        ofs<<range<<endl;
        if(range < 0 || (hAndMin + range >= END) || (hAndMin - range <= START)){
            cout<<"����ʱ�䷶Χ�������������롣"<<endl;
        }else{
            break;
        }
    }
    //��ѯ��վλ�ڸ���·�ĵڼ���վ
    int num = 0;
    //����ѯ��·��Ϊ�ɲ������±�
    int indexL = LineToIndex(line);
    for(int i = 0; i < stations[indexL]; i++){
        if(Number(indexL,i) == index){
            num = i;
            break;
        }
    }
    //�����Ϊ0��һ��
    cout<<endl;
    cout<<"�������������������������������������������������г�ʱ�̱�����������������������������������������������"<<endl;
    cout<<"���￪��"<<g.NodeList[Number(indexL, stations[indexL] - 1)].name<<"�������"<<endl;
    //���������0��վ֮��վ�� �������г�ʱ��
    int t1 = (num - 0 + 1 - 1) *DRIVE + (num - 0 + 1 - 2) * STOP;
    vector<int> time;
    for(int i = 0;t1 + i * LAUNCH < END - START ; i++){
        time.push_back(START + t1 + i * LAUNCH);
    }
    cout<<"�г��뿪ʱ��Ϊ��"<<endl;
    for(int & i : time){
        if(i >= hAndMin - range && i <= hAndMin){
                cout<<setw(2) << ((i + 1) / 60) << "ʱ"<<setw(2) << ((i + 1) % 60) << "��" << "   ";
        }
    }
    cout<<endl;
    cout<<"�г�����ʱ��Ϊ��"<<endl;
    for(int & i : time){
        if(i <= hAndMin + range && i > hAndMin){
            cout<<setw(2)<<(i/60)<<"ʱ"<<setw(2)<<(i%60)<<"��"<<"   ";
        }
    }
    cout<<endl;
    cout<<"���￪��"<<g.NodeList[Number(indexL, 0)].name<<"�������"<<endl;
    int t2 = (stations[indexL] - 1 - num + 1 - 1) * DRIVE + (stations[indexL] - 1 - num + 1 - 2) * STOP;
    vector<int> time2;
    for(int i = 0; t2 + i * LAUNCH < END - START; i++){
        time2.push_back(START + t2 + i * LAUNCH);
    }
    cout<<"�г��뿪ʱ��Ϊ��"<<endl;
    for(int & i : time2){
        if(i >= hAndMin - range && i <= hAndMin){
            cout<<setw(2)<<((i+1)/60)<<"ʱ"<<setw(2)<<((i+1)%60)<<"��"<<"   ";
        }
    }
    cout<<endl;
    cout<<"�г�����ʱ��Ϊ��"<<endl;
    for(int & i : time2){
        if(i <= hAndMin + range && i > hAndMin){
            cout<<setw(2)<<(i/60)<<"ʱ"<<setw(2)<<(i%60)<<"��"<<"   ";
        }
    }
    cout<<endl;
    cout<<"����������������������������������������������������������������������������������������������������������������������"<<endl;
}


//�Զ����´�ӡջ�����·��
void printStack(Graph g,stack<int> s){
    while (!s.empty()){
        if(s.size()>1){
            cout<<g.NodeList[s.top()].name<<"("<<g.NodeList[s.top()].atLine<<"����)->";
        }else{
            cout<<g.NodeList[s.top()].name<<"("<<g.NodeList[s.top()].atLine<<"����)"<<endl;
        }
        s.pop();
    }
}


//����̳��ȵ�·��
void dijkstra(Graph g,int start,int end){
    //����������
    int n = MAX_NUM;
    // �ж��Ƿ��Ѵ���õ㵽S������ Ϊ�����ʾ�õ��Ѿ�ȷ���˾�����ʼ�����Сֵ
    bool s[n];
    for(int i = 0; i < n;i++){
        //��ʼ��d����Ϊ����֮��ľ���
        d[i] = matrix[start][i];
        // ��ʼ��δ�ù��õ�
        s[i] = false;
        //�����ʼ�����ӵ�վ���pre��Ϊ��ʼ�� �������� ��Ϊ-1
        if(d[i] == INT16_MAX){
            pre[i] = -1;
        }else{
            pre[i] = start;
        }
    }
    d[start] = 0;
    s[start] = true;
    //���˳�ʼ��ʣ��n-1���� ѭ��n-1��
    for (int i = 0; i < n - 1 ; ++i) {
        //ͨ��minDѰ����ÿһ��ѭ�����ʼ���������ĵ� ��min{d[i]}
        int minD = INT16_MAX;
        // �ҳ���ǰδʹ�õĵ�j��dist[j]��Сֵ
        int u = start;
        for(int j = 0; j < n; j++){
            if(!s[j] && d[j] < minD){
                //��i+1��ѭ�� Ѱ�ҵ�i+1�����ʼ���������ĵ�
                // u���浱ǰ�ڽӵ��о�����С�ĵ�ĺ��±�
                u = j;
                minD = d[j];
            }
        }
        // ��Ϊ��
        s[u] = true;
        //���ڴ�u���������б�(u,y)��û��ȷ��������ʼ����Сֵ������d[y] = min{d[y], d[u]+w(u,y)}
        for(int j = 0; j < n; j++){
            if(!s[j] && matrix[u][j] < INT16_MAX){
                //��ͨ���¼����u��·���ҵ���v0����̵�·��
                if(d[u] + matrix[u][j] < d[j]){
                    //����dist
                    d[j] = d[u] + matrix[u][j];
                    //��¼ǰ������
                    pre[j] = u;
                }
            }
        }
    }
    int a = end;
    while(a != start){
        sDis.push(a);
        a = pre[a];
    }
    sDis.push(start);
    priceDis = price(d[end]);
}


//��ӡ��̳��ȵ�·��
void printDis(Graph g,int start,int end){
    int a = end;
    int changeTime = 0;
    double pathTime = 0;
    while(a != start){
        pathTime += timeMatrix[pre[a]][a];
        if(g.NodeList[a].name == g.NodeList[pre[a]].name){
            changeTime++;
        }
        a = pre[a];
    }
    if(hAndMin + pathTime < 23 * 60){
        cout << "From: " << g.NodeList[start].name << " To: " << g.NodeList[end].name << endl << "(��·�̹���"
             << setiosflags(ios::fixed) << setprecision(2) << d[end] << "����,��ʱ�乲��" << pathTime << "����,Ʊ�۹���" << priceDis
             << "Ԫ,���˴�������" << changeTime << "��)" << endl;
        printStack(g, sDis);
        cout << "�����˳��ڼ��ӵ���̶�Ϊ��" << pathCrowd(g, sDis, hAndMin) * 100 << "%" << endl;
    }else{
        cout << "����ʱ�����⣬����ͣ���޷�����Ŀ�ĵأ�" << endl;
    }
}


//�����ʱ���·��
void dijkstraTime(Graph g,int start,int end){
    int n = MAX_NUM;
    bool s[n];
    for(int i = 0; i < n;i++){
        t[i] = timeMatrix[start][i];
        s[i] = false;
        if(t[i] == INT16_MAX){
            preT[i] = -1;
        }else{
            preT[i] = start;
        }
    }
    t[start] = 0;
    s[start] = true;
    for (int i = 0; i < n - 1 ; ++i) {
        double minT = INT16_MAX;
        int u = start;
        for(int j = 0; j < n; j++){
            if(!s[j] && t[j] < minT){
                u = j;
                minT = t[j];
            }
        }
        s[u] = true;
        for(int j = 0; j < n; j++){
            if(!s[j] && timeMatrix[u][j] < INT16_MAX){
                if(t[u] + timeMatrix[u][j] < t[j]){
                    t[j] = t[u] + timeMatrix[u][j];
                    preT[j] = u;
                }
            }
        }
    }
    int a = end;
    double pathDis = 0;
    while(a != start){
        sTime.push(a);
        pathDis += matrix[preT[a]][a];
        a = preT[a];
    }
    sTime.push(start);
    priceTime = price(pathDis);
}


//��ӡ���ʱ���·��
void printTime(Graph g,int start,int end){
    int a = end;
    double pathDis = 0;
    int changeTime = 0;
    while(a != start){
        pathDis += matrix[preT[a]][a];
        if(g.NodeList[a].name == g.NodeList[preT[a]].name){
            changeTime++;
        }
        a = preT[a];
    }
    cout<<"From: "<<g.NodeList[start].name<<" To: "<<g.NodeList[end].name
    <<endl<<"(��·�̹���"<<setiosflags(ios::fixed)<<setprecision(2)<<pathDis
    <<"����,��ʱ�乲��"<<t[end]<<"����,Ʊ�۹���"<<priceTime<<"Ԫ,���˴�������"
    <<changeTime<<"��)"<<endl;
    printStack(g,sTime);
    cout << "�����˳��ڼ��ӵ���̶�Ϊ��" << pathCrowd(g, sTime, hAndMin) * 100 << "%" << endl;
}


//�󻻳˴������ٵ�·��
void dijkstraChange(Graph g,int start,int end){
    int n = MAX_NUM;
    bool s[n];
    for(int i = 0; i < n;i++){
        dc[i] = changeMatrix[start][i];
        s[i] = false;
        if(dc[i] == INT16_MAX){
            preC[i] = -1;
        }else{
            preC[i] = start;
        }
    }
    dc[start] = 0;
    s[start] = true;

    for (int i = 0; i < n - 1 ; ++i) {
        double minDC = INT16_MAX;
        int u = start;
        for(int j = 0; j < n; j++){
            if(!s[j] && dc[j] < minDC){
                u = j;
                minDC = dc[j];
            }
        }
        s[u] = true;
        for(int j = 0; j < n; j++){
            if(!s[j] && changeMatrix[u][j] < INT16_MAX){
                if(dc[u] + changeMatrix[u][j] < dc[j]){
                    dc[j] = dc[u] + changeMatrix[u][j];
                    preC[j] = u;
                }
            }
        }
    }
    int a = end;
    double pathDis = 0;
    while(a != start){
        sChange.push(a);
        pathDis += matrix[preC[a]][a];
        a = preC[a];
    }
    sChange.push(start);
    priceChange = price(pathDis);
}


//��ӡ���˴������ٵ�·��
void printChange(Graph g,int start,int end){
    int a = end;
    double pathDis = 0;
    stack<int> st;
    //���˴���
    int changeTime = 0;
    while(a != start){
        st.push(a);
        pathDis += matrix[preC[a]][a];
        if(g.NodeList[a].name == g.NodeList[preC[a]].name){
            changeTime++;
        }
        a = preC[a];
    }
    if(hAndMin +dc[end] - changeTime * (20 - WALK) < 23 * 60) {
        cout << "From: " << g.NodeList[start].name << " To: " << g.NodeList[end].name << endl << "(��·�̹���"
             << setiosflags(ios::fixed) << setprecision(2) << pathDis << "����,��ʱ�乲��"
             << dc[end] - changeTime * (20 - WALK) << "����,Ʊ�۹���" << priceChange << "Ԫ,���˴�������" << changeTime << "��)"
             << endl;
        printStack(g, sChange);
        cout << "�����˳��ڼ��ӵ���̶�Ϊ��" << pathCrowd(g, sChange, hAndMin) * 100 << "%" << endl;
    }else{
        cout << "����ʱ�����⣬����ͣ���޷�����Ŀ�ĵأ�" << endl;
    }
}


//���ݵ�ǰʱ��hAndMin���¿���ӵ���ȵ�ʱ���ڽӾ���
void update(Graph g){
    for (int i = 0; i < MAX_NUM; ++i) {
        for (int j = 0; j < MAX_NUM; ++j) {
            timeMatrixCrowd[i][j] = timeMatrix[i][j];
        }
    }
    for (int i = 0; i < MAX_NUM; ++i) {
        for (int j = 0; j < MAX_NUM; ++j) {
            if(timeMatrixCrowd[i][j] != INT16_MAX){
                //���˵�ʱ����Ҫ����ӵ����
                if(g.NodeList[i].name != g.NodeList[j].name){
                    timeMatrixCrowd[i][j] *= timeRatio(crowd(g,hAndMin,i),avoid,setedCrowd);
                }
            }
        }
    }
}


//����·��s�ļ�Ȩʱ��
double pathWeightedTime(Graph g,stack<int> s){
    double weightedTime = 0;
    update(g);
    while (s.size() > 1){
        int a = s.top();
        s.pop();
        int b = s.top();
        weightedTime += timeMatrixCrowd[a][b];
        hAndMin += timeMatrix[a][b];
        update(g);
    }
    return weightedTime;
}


//����̼�Ȩʱ���·��
void dijkstraTimeCrowd(Graph g,int start,int end){
    int n = MAX_NUM;
    bool s[n];
    for (int i = 0; i < n; ++i) {
        tCrowd[i] = timeMatrixCrowd[start][i];
        s[i] = false;
        if(tCrowd[i] == INT16_MAX){
            preTCrowd[i] = -1;
        }else{
            preTCrowd[i] = start;
        }
    }
    tCrowd[start] = 0;
    s[start] = true;
    for (int i = 0; i < n - 1; ++i) {
        double min = INT16_MAX;
        int u = start;
        for (int j = 0; j < n; ++j) {
            if(!s[j] && tCrowd[j] < min){
                u = j;
                min = tCrowd[j];
            }
        }
        s[u] = true;
        //����timeMatrixCrowd����
        hAndMin += timeMatrix[preTCrowd[u]][u];
        update(g);
        for (int k = 0; k < n; ++k) {
            if(!s[k] && timeMatrixCrowd[u][k] != INT16_MAX){
                if(tCrowd[u] + timeMatrixCrowd[u][k] < tCrowd[k]){
                    tCrowd[k] = tCrowd[u] + timeMatrixCrowd[u][k];
                    preTCrowd[k] = u;
                }
            }
        }
    }
    int a = end;
    double pathDis = 0;
    while(a != start){
        sTimeCrowd.push(a);
        pathDis += matrix[preTCrowd[a]][a];
        a = preTCrowd[a];
    }
    sTimeCrowd.push(start);
    priceTimeCrowd = price(pathDis);
}


//��ӡ��̼�Ȩʱ��·��
void printTimeCrowd(Graph g,int start,int end){
    int a = end;
    double pathDis = 0;
    double pathTime = 0;
    int changeTime = 0;
    while(a != start) {
        pathDis += matrix[preTCrowd[a]][a];
        pathTime += timeMatrix[preTCrowd[a]][a];
        if (g.NodeList[preTCrowd[a]].name == g.NodeList[a].name) {
            changeTime++;
        }
        a = preTCrowd[a];
    }
    
    cout<<"From: "<<g.NodeList[start].name<<" To: "<<g.NodeList[end].name<<endl<<"(����ӵ���ȵ���̼�Ȩʱ�䣺"<<setiosflags(ios::fixed)<<setprecision(2)<<tCrowd[end]<<"����,ʵ��ʱ�乲��"<<pathTime<<"����,��·�̹���"<<pathDis<<"����,Ʊ�۹���"<<priceTimeCrowd<<"Ԫ,���˴�������"<<changeTime<<"��)"<<endl;
    printStack(g,sTimeCrowd);
}


//����ѯ��ϵͳ
void navigation(Graph g){
    setTime();
    double startTime = hAndMin;
    int startStation;
    int endStation;
    int startLine;
    int endLine;
    while(true){
        cout<<"��������ʼ��·��";
        cin>>startLine;
        ofs<<startLine<<endl;
        if(!existLine(startLine)){
            cout<<"������·�������������롣"<<endl;
            continue;
        }else{
            break;
        }
    }
    while(true){
        cout<<"��������ʼվ��ţ�";
        cin>>startStation;
        ofs<<startStation<<endl;
        if(startStation < 1 || startStation > stations[LineToIndex(startLine)]){
            cout<<"������ʼվ����������������롣"<<endl;
            continue;
        }else{
            break;
        }
    }
    while(true){
        cout<<"�������յ���·��";
        cin>>endLine;
        ofs<<endLine<<endl;
        if(!existLine(endLine)){
            cout<<"������·�������������롣"<<endl;
            continue;
        }else{
            break;
        }
    }
    while(true){
        cout<<"�������յ�վ��ţ�";
        cin>>endStation;
        ofs<<endStation<<endl;
        if(endStation < 1 || endStation > stations[LineToIndex(endLine)]){
            cout<<"�����յ�վ����������������롣"<<endl;
            continue;
        }else{
            break;
        }
    }
    int startIndex = Number(LineToIndex(startLine), startStation - 1);
    int endIndex = Number(LineToIndex(endLine), endStation - 1);

    char in;
    while(true){
        cout<<"�����Ƿ�ӵ������Ϊ���ؽ���ʱ���ۺ�������(��:Y/y,��:N/n)"<<endl;
        cin>>in;
        ofs<<in<<endl;
        if(in != 'y' && in != 'n' && in != 'Y' && in != 'N'){
            cout<<"����ѡ����������������"<<endl;
        }
        else{
            break;
        }
    }
    d[startIndex] = 0;
    dc[startIndex] = 0;
    t[startIndex] = 0;
    tCrowd[startIndex] = 0;
    for (int j = 0; j < MAX_NUM && j != startIndex; ++j) {
        d[j] = INT16_MAX;
        dc[j] = INT16_MAX;
        t[j] = INT16_MAX;
        tCrowd[j] = INT16_MAX;
    }
    considerCrowd = (in == 'y' || in == 'Y');
    if(!considerCrowd) {
        //�����ʱ��·��
        dijkstraTime(g, startIndex, endIndex);
        //����̳���·��
        dijkstra(g, startIndex, endIndex);
        //�����ٻ��ߴ���·��
        dijkstraChange(g, startIndex, endIndex);
        if (hAndMin + t[endIndex] > 23 * 60 || hAndMin < 6 * 60) {
            cout << "����ʱ�����⣬����ͣ���޷�����Ŀ�ĵأ�" << endl;
        } else {
            cout << "------------------------------------------------------------------------------------" << endl;
            cout<<"��һ���Ƽ�·�ߣ�"<<endl;
            printDis(g, startIndex, endIndex);
            cout << "------------------------------------------------------------------------------------" << endl;
            cout<<"�ڶ����Ƽ�·�ߣ�"<<endl;
            if(priceTime < priceChange){
                printTime(g, startIndex, endIndex);
                cout << "------------------------------------------------------------------------------------" << endl;
                cout<<"�������Ƽ�·�ߣ�"<<endl;
                printChange(g, startIndex, endIndex);
            }else{
                printChange(g, startIndex, endIndex);
                cout << "------------------------------------------------------------------------------------" << endl;
                cout<<"�������Ƽ�·�ߣ�"<<endl;
                printTime(g, startIndex, endIndex);
            }
            cout << "------------------------------------------------------------------------------------" << endl;
        }
    }else{
        char in2;
        while(true){
            cout<<"�����Ƿ��Լ����õ�ǰʱ��ӵ���ȣ�(��:Y/y,��:N/n)"<<endl;
            cin>>in2;
            ofs<<in2<<endl;
            if(in2 != 'y' && in2 != 'n' && in2 != 'Y' && in2 != 'N'){
                cout<<"����ѡ����������������"<<endl;
            }
            else{
                break;
            }
        }
        ChangeCrowd = (in2 == 'y' || in2 == 'Y');
        if(ChangeCrowd) {
            while (true){
                int line;
                char in3;
                cout<<"����������ӵ���ȵ���·��";
                cin>>line;
                ofs<<line<<endl;
                if(!existLine(line)){
                    cout<<"������·�������������롣"<<endl;
                    continue;
                }else{
                    double in4;
                    while(true){
                        cout<<"������Ҫ���õ�ӵ����ֵ��0~1����";
                        cin>>in4;
                        ofs<<in4<<endl;
                        if(in4 < 0 || in4 > 1){
                            cout<<"����ӵ�����������������롣"<<endl;
                            continue;
                        }else{
                            break;
                        }
                    }
                    for (int i = 0; i < 3; ++i) {
                        crowds[LineToIndex(line)][i] = in4;
                    }
                    while (true){
                        cout<<"�Ƿ��������ӵ���ȣ�(��:Y/y,��:N/n)";
                        cin>>in3;
                        ofs<<in3<<endl;
                        if(in3 != 'y' && in3 != 'n' && in3 != 'Y' && in3 != 'N'){
                            cout<<"����ѡ����������������"<<endl;
                        }
                        else{
                            break;
                        }
                    }
                    if(in3 == 'N' || in3 == 'n'){
                        break;
                    }else{
                        continue;
                    }
                }
            }
        }
        char in5;
        while(true){
            cout<<"�Ƿ�ѡ��ܿ��ض�ӵ������·��(��:Y/y,��:N/n)";
            cin>>in5;
            ofs<<in5<<endl;
            if(in5 != 'y' && in5 != 'n' && in5 != 'Y' && in5 != 'N'){
                cout<<"����ѡ���������������롣"<<endl;
            }else{
                break;
            }
        }
        avoid = (in5 == 'y' || in5 =='Y');
        if(avoid){
            while (true){
                cout<<"���������õ�ӵ��������(0~1)��";
                cin>>setedCrowd;
                ofs<<setedCrowd<<endl;
                if(setedCrowd > 1 || setedCrowd < 0){
                    cout<<"���������������������롣"<<endl;
                }else{
                    break;
                }
            }

        }
        tCrowd[startIndex] = 0;
        for (int i = 0; i < MAX_NUM && i != startIndex; ++i) {
            tCrowd[i] = INT16_MAX;
        }
        //���³�ʼʱ�俼��ӵ���ȵ�ʱ���ڽӾ���
        //���¾�������hAndMin ��Ҫ����һ��startTime����Ļ���
        startTime = hAndMin;
        update(g);
        dijkstraTimeCrowd(g, startIndex, endIndex);
        hAndMin = startTime;
        dijkstra(g, startIndex, endIndex);
        if (startTime + tCrowd[endIndex] > 23 * 60 || startTime < 6 * 60) {
            cout << "����ʱ�����⣬����ͣ���޷�����Ŀ�ĵأ�" << endl;
        } else {
            cout << "------------------------------------------------------------------------------------" << endl;
            cout << "��һ���Ƽ�·��:" << endl;
            printTimeCrowd(g, startIndex, endIndex);
            hAndMin = startTime;
            cout << "�����˳��ڼ��ӵ���̶�Ϊ��" << pathCrowd(g, sTimeCrowd, hAndMin) * 100 << "%" << endl;
            hAndMin = startTime;
            cout << "------------------------------------------------------------------------------------" << endl;
            if(pathWeightedTime(g,sDis) < INT16_MAX){
                hAndMin = startTime;
                cout << "�ڶ����Ƽ�·�ߣ�" << endl;
                printDis(g, startIndex, endIndex);
                hAndMin = startTime;
                cout << "------------------------------------------------------------------------------------" << endl;
            }else{
                hAndMin = startTime;
            }

        }
    }
}
#endif //METRO_MAINFUNCTION_H
