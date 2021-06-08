#pragma once
#include<iostream>
using namespace std;

class SolveStruct
{
private:
    //ͼ�ṹ
    int* client_cover_num;//ÿ�����ܹ����ǵĽڵ���
    int** elements;//ÿ���㸲�ǵĽڵ�
    int nums, P;//�ڵ���������������

    //��¼����
    bool* Solution;//�Ƿ���ѡΪ���ģ���⣩
    int* center;//��¼���Ľڵ�
    int* covered_center_num;//ÿ��Ԫ�ر����ٸ����ĸ���
    int* covered_by;//����һ�����ĸ��ǵĵ�
    int uncovered_num;//��¼û�б����ǵĽڵ���������uncovered_list���±�
    int* uncovered_list;//��¼û�б����ǵĽڵ�
    int* weight;//ÿ���ڵ�Ȩ�أ�����δ����ʱ������������
    int f, best_f, best_delta_f; //f��������¼��Ȩ��
    int* delta;//��¼���i�������Ļ���ɾ�����Ķ�f��Ӱ��
    int tabu_open, tabu_close;//������Ϊ1����¼����Ԫ��

    //����ļ�·����ʱ������
    string output_path;
public:
    int tempiter;//��ʱ����ͳ�Ƶ�������
    int tempnum;//��¼uncovered_num
    int start_ms, end_ms, limit_s;

    //���캯������������
    SolveStruct(string path,string out);//���ļ�����ʼ������
    ~SolveStruct();
    //��Ҫʵ��
    void Greedy();
    void Solve(int limit_s, int rand_seed);
    void FindSwap(int& v_open, int& v_close);//Ѱ�ҽ�����
    void SwapMove(int v_open, int v_close);
    //���һЩ���̺���
    void InitialDelta();//��ʼ��delta
    void Open(int v);//�򿪽ڵ�
    void Close(int v);//�رսڵ�
    bool OutputFile();//����ļ�
};