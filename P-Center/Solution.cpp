#pragma warning(disable:4996)
#include "Solution.h"
#include<iostream>
#include<fstream>
#include <fstream>
#include <sstream>
#include<vector>
#include<cstdlib> 
#include<ctime>
#include<cmath>
using namespace std;

SolveStruct::SolveStruct(string path,string out)
{
    //���ļ�
    string temp;
    ifstream fin(path);
    if (!fin)
    {
        printf("file %s is opened fail\n", path.c_str());
    }
    getline(fin, temp);
    sscanf(temp.c_str(), "%d%d", &nums, &P);
    client_cover_num = new int[nums];
    elements = new int* [nums];

    int count_client;
    for (int i = 0; i < nums; ++i)
    {
        getline(fin, temp);
        sscanf(temp.c_str(), "%d", &count_client);
        client_cover_num[i] = count_client;
        elements[i] = new int[count_client];

        getline(fin, temp);
        auto current_list = elements[i];
        stringstream ss(temp);
        for (int j = 0; j < count_client; ++j)
            ss >> current_list[j];
    }
    fin.close();
    output_path = out;
    Solution = new bool[nums];
    center = new int[P];
    covered_center_num = new int[nums];
    covered_by = new int[nums];
    uncovered_list = new int[nums];
    weight = new int[nums];
    f = best_f = uncovered_num = nums;
    delta = new int[nums];
    tabu_open = tabu_close = -1;
    best_delta_f = INT_MAX;
    for (int i = 0; i < nums; ++i)
    {
        Solution[i] = false;//һ��ʼû�б���ѡΪ���ĵĵ�
        covered_center_num[i] = 0;//ÿ���㶼û�б����ĸ���
        weight[i] = 1;//��ʼȨ��ȫ��Ϊ1
    }
}
SolveStruct::~SolveStruct()
{
    for (int i = 0; i < nums; ++i)
        delete[] elements[i];
    delete[] client_cover_num;
    delete[] Solution;
    delete[] center;
    delete[] covered_center_num;
    delete[] covered_by;
    delete[] uncovered_list;
    delete[] weight;
    delete[] delta;
}

void SolveStruct::Greedy()
{
    int max_uncovered;//��¼����ܸ��ǵ�δ�����ǽڵ���
    int cur_uncovered;//��ǰ�����ܸ��ǵ�δ����Ԫ����Ŀ
    vector<int> best_list;//��¼�ܹ��������δ���ǽڵ�Ľڵ��б�
    for (int ip = 0; ip < P; ++ip)
    {
        max_uncovered = 0;
        best_list.clear();
        for (int inum = 0; inum < nums; ++inum)
        {
            //���δ��ѡΪ���ľͼ��
            if (!Solution[inum])//��δѡΪ���ĵĽ�����ҵ���õ���Щ�����б�
            {
                //���㵱ǰ�ڵ��ܸ��Ƕ���δ�����ǵĽڵ�
                int res = 0;
                for (int i = 0; i < client_cover_num[inum]; ++i)
                    if (covered_center_num[elements[inum][i]] == 0)
                        ++res;
                cur_uncovered = res;

                if (cur_uncovered == max_uncovered)//�������ֵ�б�
                {
                    best_list.push_back(inum);
                }
                else if (cur_uncovered > max_uncovered)//�����֮ǰ�ҵ���maxֵ�󣬸���
                {
                    best_list.clear();
                    best_list.push_back(inum);//�ؼ����ֵ�б�
                    max_uncovered = cur_uncovered;
                }
            }
        }
        //���б������ѡһ��������
        int choose = best_list[rand() % best_list.size()];
        Solution[choose] = true;
        for (int i = 0; i < client_cover_num[choose]; ++i)
        {
            if (covered_center_num[elements[choose][i]] == 0)//�����δ��������ĸ���
            {
                --f;//����ڵ�
                --best_f;
                --uncovered_num;
                covered_by[elements[choose][i]] = choose;//��ʾ����һ�����ĸ��ǵĽڵ�
            }
            ++covered_center_num[elements[choose][i]];//��Ԫ�ر����ĸ�����++
        }
    }
    //����δ���ǵĽڵ������
    uncovered_num = 0;
    int isolx = 0;
    for (int i = 0; i < nums; ++i)
    {
        if (Solution[i] == true)//�����ѡΪ���ģ��������б��м�¼
            center[isolx++] = i;
        else if (covered_center_num[i] == 0)//����ǿͻ�����δ�����ǣ���δ���ǽڵ��м�¼
            uncovered_list[uncovered_num++] = i;
    }
    //��ʼ��delta��һ��ʼȨ�ض�Ϊ1������������������Ȩ�غͣ�
    InitialDelta();
}

void SolveStruct::Solve(int limit_s, int rand_seed)
{
    start_ms = clock();
    this->limit_s = limit_s;
    srand(rand_seed);
    Greedy();//̰��
    int iter;
    int last_uncovered_num = INT_MAX, best_uncovered_num = uncovered_num;
    int v_open, v_close;
    for (iter = 1; uncovered_num != 0 && (clock() - start_ms) / 1000 < limit_s; ++iter)
    {
        FindSwap(v_open, v_close);
        if (v_open == -1 || v_close == -1)//û�ҵ��ǽ���move��������ɽ�����һ��
            continue;
        SwapMove(v_open, v_close);//����move���Ƚ�
        if (uncovered_num < last_uncovered_num && uncovered_num < best_uncovered_num)
        {
            best_uncovered_num = uncovered_num;
            OutputFile();
        }
        else//���ϴν��Ҫ����Ȩ�ؽ���δ���ǽ��
        {
            for (int i = 0; i < uncovered_num; ++i)
            {
                int v = uncovered_list[i];
                ++weight[v], ++f;
                for (int ic = 0; ic < client_cover_num[v]; ++ic)  //�ھ�deltaֵ��Ӧ���
                    ++delta[elements[v][ic]] ;
            }
        }
        last_uncovered_num = uncovered_num;
    }
    end_ms = clock();
    tempiter = iter;
    tempnum = uncovered_num;
}

void SolveStruct::InitialDelta()
{
    for (int i = 0; i < nums; ++i)//o(n^2/p^2) ~ o(n^2)
    {
        int flag = Solution[i] ? 1 : 0;
        //����Ƿ����Ľڵ㣬deltaΪ����δ����Ԫ������
        //��������ģ�deltaΪ���ɸ����ĸ���Ԫ������
        delta[i] = 0;
        for (int j = 0; j < client_cover_num[i]; ++j)
            if (covered_center_num[elements[i][j]] == flag)
                ++delta[i];
    }
}

void SolveStruct::FindSwap(int& v_open, int& v_close)
{
    int choose = uncovered_list[rand() % uncovered_num];
    best_delta_f = INT_MAX;
    vector<int> best_open, best_close;
    int* delta_p = new int[P];//��¼����delta�����ڻ�ԭ 
    for (int i = 0; i < P; ++i)
    {
        delta_p[i] = delta[center[i]];
    }
    for (int ic = 0; ic < client_cover_num[choose]; ++ic)//o(n/p)�������ܹ�����δ���ǽڵ�ĵ�
    {
        int vc = elements[choose][ic];//try_open���(�ܹ�����δ���ǽڵ�������ܸ��ǵĽڵ�)
        //�ı���Ӱ�����ĵ�delta
        for (int jc = 0; jc < client_cover_num[vc]; ++jc)//o(n/p)
        {
            int vjc = elements[vc][jc];
            if (covered_center_num[vjc] == 1)//�����ǰ�ڵ��ܱ�ԭ�����ĸ���һ�Σ��ٴθ��Ǿ�Ҫ����ֵ
                delta[covered_by[vjc]] -= weight[vjc];
        }
        //����best_delta_f
        for (int ip = 0; ip < P; ++ip)
        {
            int cur_delta_f = delta[center[ip]] - delta[vc];//����ڵ�vc

            if (vc != tabu_open && center[ip] != tabu_close || f - cur_delta_f < best_f)
            {
                if (cur_delta_f < best_delta_f)//�ؼ����ֵ�б�
                {
                    best_delta_f = cur_delta_f;
                    best_open.clear();
                    best_open.push_back(vc);
                    best_close.clear();
                    best_close.push_back(center[ip]);
                }
                else if (cur_delta_f == best_delta_f)//�������ֵ�б�
                {
                    best_open.push_back(vc);
                    best_close.push_back(center[ip]);
                }
            }
        }
        for (int ip = 0; ip < P; ++ip)
        {
            delta[center[ip]] = delta_p[ip];
        }
    }
    delete[]delta_p;
    if (best_open.size() != 0)//���ѡȡһ������
    {
        choose = rand() % best_open.size();
        v_open = best_open[choose];
        v_close = best_close[choose];
    }
    else
    {
        v_open = v_close = -1;
        tabu_open = tabu_close = -1;
    }
}

void SolveStruct::SwapMove(int v_open, int v_close)
{
    Open(v_open); 
    Close(v_close);
    f = f + best_delta_f;
    if (f < best_f)best_f = f;
    tabu_open = v_close;
    tabu_close = v_open;
    //����
    uncovered_num = 0;
    int isolx = 0;
    for (int i = 0; i < nums; ++i)
    {
        if (Solution[i] == true)//�����ѡΪ���ģ��������б��м�¼
            center[isolx++] = i;
        else if (covered_center_num[i] == 0)//����ǿͻ�����δ�����ǣ���δ���ǽڵ��м�¼
            uncovered_list[uncovered_num++] = i;
    }
}

void SolveStruct::Open(int v)//������v��Ϊ����
{
    Solution[v] = true;
    //delta[v] = 0;
    //��������delta
    for (int ic = 0; ic < client_cover_num[v]; ++ic)//o(n/p)
    {
        int vc = elements[v][ic];
        if (covered_center_num[vc] == 1)//�ھ�vcԭ��Ψһ���ǵ�����delta--, o(1)
            delta[covered_by[vc]] -= weight[vc];
        else if (covered_center_num[vc] == 0)//�¸��ǽ����ھ�delta--
        {
            for (int jc = 0; jc < client_cover_num[vc]; ++jc)
            {
                delta[elements[vc][jc]] -= weight[vc];
            }
            covered_by[vc] = v;
            delta[v] += weight[vc];
        }
        ++covered_center_num[vc];
    }
}

void SolveStruct::Close(int v)
{
    Solution[v] = false;
    //��������delta
    for (int ic = 0; ic < client_cover_num[v]; ++ic)
    {
        int vc = elements[v][ic];
        if (covered_center_num[vc] == 1)//vc���δ���ǽ��
        {
            for (int jc = 0; jc < client_cover_num[vc]; ++jc)
                delta[elements[vc][jc]] += weight[vc];
            delta[v] -= weight[vc];
        }
        else if (covered_center_num[vc] == 2)//vc��Χ���ı��Ψһ����vc
        {
            for (int jc = 0; jc < client_cover_num[vc]; ++jc)
                if (Solution[elements[vc][jc]])
                {
                    delta[elements[vc][jc]] += weight[vc];
                    covered_by[vc] = elements[vc][jc];//���±�Ψһ��������
                    break;
                }
        }
        --covered_center_num[vc];
    }
}

bool SolveStruct::OutputFile()
{
    ofstream fout(output_path);
    if (!fout)
    {
        printf("output error :%s\n",output_path.c_str());
        return false;
    }
    for (int i = 0; i < P; ++i)
    {
        fout << center[i] << " ";
    }
    return true;
}

