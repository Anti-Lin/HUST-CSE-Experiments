//main.cpp
#include<stdio.h>
#include<stdlib.h>
#define NUM 1000
#define N 10

char name[NUM][10], in_name[N];
short score[NUM][4];//һ��short�����ֽ�

extern "C" int calculate(short* s);
extern "C" int continueOrQuit(char* s1);
extern "C" int match(char* s1, char* s2);

int cpy(char* s1, const char s2[]) {
    int j = 0, i = 0;;
    for (j = 0; j < N; j++) {//��s1���г�ʼ��
        *(s1 + j) = 0;
    }
    while (s2[i]) {
        *(s1 + i) = s2[i];//��s2�ַ������Ƶ�s1
        i++;
    }
    return 0;
}

int main() {
    int flag = 0;
    int  num = 0, c = 0, i = 0, j = 0;

    cpy(name[0], "zhangsan");//��ʼ��0-999��ѧ��
    score[0][0] = 100;
    score[0][1] = 85;
    score[0][2] = 80;
    cpy(name[1], "lisi");
    score[1][0] = 80;
    score[1][1] = 100;
    score[1][2] = 70;
    cpy(name[2], "xzc");
    score[2][0] = 80;
    score[2][1] = 85;
    score[2][2] = 100;

    for (i = 3; i < NUM; i++) {
        cpy(name[i], "TempValue");
        score[i][0] = 80;
        score[i][1] = 90;
        score[i][2] = 95;
    }

    for (i = 0; i < NUM; i++) {//����1000���˵�ƽ���ɼ�
        calculate(score[i]);//score[i]�ǵ�i��ѧ�������ĳɼ���ַ
    }

    while (1) {
        flag = 0, num = 0;
        for (j = 0; j < N; j++) {
            in_name[j] = 0;//��ʼ��in_name�ַ���
        }
        printf("Please input the student's name:\n");
        printf("(Enter q to quit and enter a to calculate the average grade)\n");
        i = 0;
        c = getchar();
        while (c != '\n' && i < N - 1) {
            in_name[i] = c;
            i++;
            c = getchar();//ͨ��getchar�������
        }
        int status = continueOrQuit(in_name);
        if (status == 1) {
            continue;//��������ѭ�� ������ʾ����
        }
        if (status == 2) {
            printf("Exit successfully!");
            break;//����ѭ��

        }
        if (status == 3) {
            for (i = 0; i < NUM; i++) {
                calculate(score[i]);
            }
            printf("Calculate all the average grades successfully!\n");
            flag = 2;
        }
            for (j = 0; j < 1000; j++) {
                if (match(in_name, name[j]) == 1) {
                    num = j;
                    flag = 1;
                    break;//������ֵ�λ�� ����forѭ��
                }
            }

        if (flag == 0) {
            printf("%s does not exist!\n", in_name);
        }
        else if (flag == 1) {
            int t = score[num][3];
            printf("The grade of %s is %c !\n", in_name, t > 90 ? 'A' : t > 80 ? 'B' : t > 70 ? 'C' : t > 60 ? 'D' : 'F');
        }
    }
    return 0;
}   
      
