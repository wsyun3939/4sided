#include <iostream>
#include "instance.h"

using namespace std;

// 積み替え先(積み替え先，下界値)
struct Dst
{
    // 積み替え先座標
    Point dst;
    // 下界値
    int LB;
    // 最大優先度
    int p;
};

// ソートする際の比較関数
bool asc_desc(Dst &left, Dst &right)
{
    if (left.LB == right.LB)
    {
        return right.p < left.p;
    }
    else
    {
        return left.LB < right.LB;
    }
}

int bb1(Instance &instance, int UB_cur)
{
    // 節点の深さ
    static int depth = 0;
    // 最適解
    static int min_rel = 0;
    if (depth + instance.config.LB1 == UB_cur - 1)
    {
        int UB_temp = instance.UB1();
        if (instance.config.UB1 > UB_temp + depth)
        {
            instance.config.UB1 = UB_temp + depth;
        }
    }
    if (instance.config.UB1 == UB_cur)
    {
        depth = 0;
        return min_rel = instance.config.UB1;
    }
    int block1 = instance.config.count(Upp);
    while (block1 == 0)
    {
        Point src = instance.config.pos[instance.config.priority - 1];
        instance.config.retrieve(src);
        if (instance.config.priority == NBLOCK + 1)
        {
            min_rel = depth;
            depth = 0;
            return min_rel;
        }
        block1 = instance.config.count(Upp);
    }
    Instance instance_temp = instance;
    // ブロッキングブロックを積み替える場合
    // 積み替え元
    Point src = {instance_temp.config.pos[instance_temp.config.priority - 1].x, TIER - 1};
    // 積み替えるブロック位置を特定
    while (!instance_temp.config.block[src.x][src.y])
    {
        src.y--;
    }
    depth++;
    cout << endl;
    vector<Dst> vec_dst;
    Point dst = {0, TIER - 1};
    // 積み替え先スタックを決定
    for (dst.x = 0; dst.x < STACK; dst.x++)
    {
        if ((dst.x != src.x) && !instance_temp.config.block[dst.x][TIER - 1])
        {
            int LB_temp = 0;
            while (!instance_temp.config.block[dst.x][dst.y - 1] && (dst.y > 0))
                dst.y--;
            int p = instance_temp.config.block[src.x][src.y];
            instance_temp.config.block[src.x][src.y] = 0;
            instance_temp.config.block[dst.x][dst.y] = p;
            instance_temp.config.pos[p - 1] = dst;
            LB_temp = instance_temp.LB1();
            Dst temp = {dst, LB_temp, instance_temp.config.P_UL[dst.x]};
            vec_dst.push_back(temp);
            instance_temp.config.block[dst.x][dst.y] = 0;
            instance_temp.config.block[src.x][src.y] = p;
            instance_temp.config.pos[p - 1] = src;
            dst.y = TIER - 1;
        }
    }
    // 積み替え先を下界値の小さい順に並べる.下界値が等しい場合，最大優先度の降順に並べる
    sort(vec_dst.begin(), vec_dst.end(), asc_desc);
    instance.config.print();
    for (auto it = vec_dst.begin(); it != vec_dst.end(); it++)
    {
        instance_temp.config.relocate(src, it->dst);
        if (bb1(instance_temp, UB_cur))
        {
            return min_rel;
        }
        instance_temp.config.relocate(it->dst, src);
    }
    depth--;
    if (depth == 0)
    {
        UB_cur++;
        if (bb1(instance, UB_cur))
        {
            return min_rel;
        }
        else
            return -1;
    }
    else
        return 0;
}