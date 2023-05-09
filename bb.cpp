#include <iostream>
#include "instance.h"

using namespace std;

// 積み替え元
struct Src
{
    // ブロッキングブロック数
    int block;
    // 積み替え方向
    Direction dir;
};

// 積み替え先
struct Dst
{
    // 積み替え先座標
    Point dst;
    // 下界値
    int LB;
    // 最大優先度
    int p;
};

// ブロッキングブロック数でひ比較
bool block_asc(Src &left, Src &right)
{
    return left.block < right.block;
}

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
    if (block1 == 1)
    {
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
            if (it->LB + depth > UB_cur)
            {
                break;
            }
            instance_temp.config.relocate(src, it->dst);
            if (bb1(instance_temp, UB_cur))
            {
                return min_rel;
            }
            instance_temp.config.relocate(it->dst, src);
        }
    }
    else
    {
        // ブロッキングブロックを積み替える場合
        // 積み替え元
        Point src = {instance.config.pos[instance.config.priority - 1].x, TIER - 1};
        // 積み替えるブロック位置を特定
        while (!instance.config.block[src.x][src.y])
        {
            src.y--;
        }
        depth++;
        vector<Dst> vec_dst;
        Point dst = {0, TIER - 1};
        // 積み替え先スタックを決定
        for (dst.x = 0; dst.x < STACK; dst.x++)
        {
            if ((dst.x != src.x) && !instance.config.block[dst.x][TIER - 1])
            {
                int LB_temp = 0;
                while (!instance.config.block[dst.x][dst.y - 1] && (dst.y > 0))
                    dst.y--;
                int p = instance.config.block[src.x][src.y];
                instance.config.block[src.x][src.y] = 0;
                instance.config.block[dst.x][dst.y] = p;
                instance.config.pos[p - 1] = dst;
                LB_temp = instance.LB1();
                Dst temp = {dst, LB_temp, instance.config.P_UL[dst.x]};
                vec_dst.push_back(temp);
                instance.config.block[dst.x][dst.y] = 0;
                instance.config.block[src.x][src.y] = p;
                instance.config.pos[p - 1] = src;
                dst.y = TIER - 1;
            }
        }
        // 積み替え先を下界値の小さい順に並べる.下界値が等しい場合，最大優先度の降順に並べる
        sort(vec_dst.begin(), vec_dst.end(), asc_desc);
        instance.config.print();
        for (auto it = vec_dst.begin(); it != vec_dst.end(); it++)
        {
            if (it->LB + depth > UB_cur)
            {
                break;
            }
            instance.config.relocate(src, it->dst);
            if (bb1(instance, UB_cur))
            {
                return min_rel;
            }
            instance.config.relocate(it->dst, src);
        }
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

int bb2(Instance &instance, int UB_cur, Direction dir)
{
    // 節点の深さ
    static int depth = 0;
    // 最適解
    static int min_rel = 0;
    if (depth + instance.config.LB2 == UB_cur - 1)
    {
        int UB_temp = instance.UB2(Upp, Right);
        if (instance.config.UB2 > UB_temp + depth)
        {
            instance.config.UB2 = UB_temp + depth;
        }
    }
    if (instance.config.UB2 == UB_cur)
    {
        depth = 0;
        return min_rel = instance.config.UB2;
    }
    Point src;
    int block1 = instance.config.count(Upp);
    int block2 = instance.config.count(Right);
    if (dir == Any)
    {
        while ((block1 == 0) || (block2 == 0))
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
            block2 = instance.config.count(Right);
        }
        instance.config.print();
        vector<Src> src_vec;
        Src temp1 = {block1, Upp};
        Src temp2 = {block2, Right};
        src_vec.push_back(temp1);
        src_vec.push_back(temp2);
        sort(src_vec.begin(), src_vec.end(), block_asc);
        depth++;
        for (auto element : src_vec)
        {
            // ブロッキングブロックを積み替える場合
            // 積み替え元
            switch (element.dir)
            {
            case Upp:
                src.x = instance.config.pos[instance.config.priority - 1].x;
                src.y = TIER - 1;
                // 積み替えるブロック位置を特定
                while (!instance.config.block[src.x][src.y])
                {
                    src.y--;
                }
                break;
            case Right:
                src.y = instance.config.pos[instance.config.priority - 1].y;
                src.x = STACK - 1;
                // 積み替えるブロック位置を特定
                while (!instance.config.block[src.x][src.y])
                {
                    src.x--;
                }
                break;
            default:
                break;
            }
            vector<Dst> vec_dst;
            Point dst = {0, TIER - 1};
            // 積み替え先スタックを決定
            // 上側からの積み替え
            for (dst.x = 0; dst.x < STACK; dst.x++)
            {
                if (!instance.config.block[dst.x][TIER - 1])
                {
                    int LB_temp = 0;
                    while (!instance.config.block[dst.x][dst.y - 1] && (dst.y > 0))
                        dst.y--;
                    if (src == dst)
                    {
                        dst.y = TIER - 1;
                        continue;
                    }
                    int p = instance.config.block[src.x][src.y];
                    instance.config.block[src.x][src.y] = 0;
                    instance.config.block[dst.x][dst.y] = p;
                    instance.config.pos[p - 1] = dst;
                    LB_temp = instance.LB2(Upp, Right);
                    Dst temp = {dst, LB_temp, instance.config.P_UL[dst.x]};
                    vec_dst.push_back(temp);
                    instance.config.block[dst.x][dst.y] = 0;
                    instance.config.block[src.x][src.y] = p;
                    instance.config.pos[p - 1] = src;
                    dst.y = TIER - 1;
                }
            }
            dst.x = STACK - 1;
            // 右側からの積み替え
            for (dst.y = 0; dst.y < TIER; dst.y++)
            {
                if (!instance.config.block[STACK - 1][dst.y])
                {
                    int LB_temp = 0;
                    while (!instance.config.block[dst.x - 1][dst.y] && (dst.x > 0))
                        dst.x--;
                    if (src == dst)
                    {
                        dst.x = STACK - 1;
                        continue;
                    }
                    int p = instance.config.block[src.x][src.y];
                    instance.config.block[src.x][src.y] = 0;
                    instance.config.block[dst.x][dst.y] = p;
                    instance.config.pos[p - 1] = dst;
                    LB_temp = instance.LB2(Upp, Right);
                    Dst temp = {dst, LB_temp, instance.config.P_UL[dst.x]};
                    vec_dst.push_back(temp);
                    instance.config.block[dst.x][dst.y] = 0;
                    instance.config.block[src.x][src.y] = p;
                    instance.config.pos[p - 1] = src;
                    dst.x = STACK - 1;
                }
            }
            // 積み替え先を下界値の小さい順に並べる.下界値が等しい場合，最大優先度の降順に並べる
            sort(vec_dst.begin(), vec_dst.end(), asc_desc);
            if (element.block == 1)
            {
                Instance instance_temp = instance;
                for (auto it = vec_dst.begin(); it != vec_dst.end(); it++)
                {
                    if (it->LB + depth > UB_cur)
                    {
                        break;
                    }
                    instance_temp.config.relocate(src, it->dst);
                    if (bb2(instance_temp, UB_cur, Any))
                    {
                        return min_rel;
                    }
                    instance_temp.config.relocate(it->dst, src);
                }
            }
            else
            {
                for (auto it = vec_dst.begin(); it != vec_dst.end(); it++)
                {
                    if (it->LB + depth > UB_cur)
                    {
                        break;
                    }
                    instance.config.relocate(src, it->dst);
                    if (bb2(instance, UB_cur, element.dir))
                    {
                        return min_rel;
                    }
                    instance.config.relocate(it->dst, src);
                }
            }
        }
    }
    else
    {
        int block;
        if (dir == Upp)
        {
            src.x = instance.config.pos[instance.config.priority - 1].x;
            src.y = TIER - 1;
            // 積み替えるブロック位置を特定
            while (!instance.config.block[src.x][src.y])
            {
                src.y--;
            }
            block = block1;
        }
        else if (dir == Right)
        {
            src.y = instance.config.pos[instance.config.priority - 1].y;
            src.x = STACK - 1;
            // 積み替えるブロック位置を特定
            while (!instance.config.block[src.x][src.y])
            {
                src.x--;
            }
            block = block2;
        }
        vector<Dst> vec_dst;
        Point dst = {0, TIER - 1};
        // 積み替え先スタックを決定
        // 上側からの積み替え
        for (dst.x = 0; dst.x < STACK; dst.x++)
        {
            if (!instance.config.block[dst.x][TIER - 1])
            {
                int LB_temp = 0;
                while (!instance.config.block[dst.x][dst.y - 1] && (dst.y > 0))
                    dst.y--;
                if (src == dst)
                {
                    dst.y = TIER - 1;
                    continue;
                }
                int p = instance.config.block[src.x][src.y];
                instance.config.block[src.x][src.y] = 0;
                instance.config.block[dst.x][dst.y] = p;
                instance.config.pos[p - 1] = dst;
                LB_temp = instance.LB2(Upp, Right);
                Dst temp = {dst, LB_temp, instance.config.P_UL[dst.x]};
                vec_dst.push_back(temp);
                instance.config.block[dst.x][dst.y] = 0;
                instance.config.block[src.x][src.y] = p;
                instance.config.pos[p - 1] = src;
                dst.y = TIER - 1;
            }
        }
        dst.x = STACK - 1;
        // 右側からの積み替え
        for (dst.y = 0; dst.y < TIER; dst.y++)
        {
            if (!instance.config.block[STACK - 1][dst.y])
            {
                int LB_temp = 0;
                while (!instance.config.block[dst.x - 1][dst.y] && (dst.x > 0))
                    dst.x--;
                if (src == dst)
                {
                    dst.x = STACK - 1;
                    continue;
                }
                int p = instance.config.block[src.x][src.y];
                instance.config.block[src.x][src.y] = 0;
                instance.config.block[dst.x][dst.y] = p;
                instance.config.pos[p - 1] = dst;
                LB_temp = instance.LB2(Upp, Right);
                Dst temp = {dst, LB_temp, instance.config.P_UL[dst.x]};
                vec_dst.push_back(temp);
                instance.config.block[dst.x][dst.y] = 0;
                instance.config.block[src.x][src.y] = p;
                instance.config.pos[p - 1] = src;
                dst.x = STACK - 1;
            }
        }
        // 積み替え先を下界値の小さい順に並べる.下界値が等しい場合，最大優先度の降順に並べる
        sort(vec_dst.begin(), vec_dst.end(), asc_desc);
        if (block == 1)
        {
            Instance instance_temp = instance;
            for (auto it = vec_dst.begin(); it != vec_dst.end(); it++)
            {
                if (it->LB + depth > UB_cur)
                {
                    break;
                }
                instance_temp.config.relocate(src, it->dst);
                if (bb2(instance_temp, UB_cur, Any))
                {
                    return min_rel;
                }
                instance_temp.config.relocate(it->dst, src);
            }
        }
        else
        {
            for (auto it = vec_dst.begin(); it != vec_dst.end(); it++)
            {
                if (it->LB + depth > UB_cur)
                {
                    break;
                }
                instance.config.relocate(src, it->dst);
                if (bb2(instance, UB_cur, dir))
                {
                    return min_rel;
                }
                instance.config.relocate(it->dst, src);
            }
        }
    }
    depth--;
    if (depth == 0)
    {
        UB_cur++;
        if (bb2(instance, UB_cur, Any))
        {
            return min_rel;
        }
        else
            return -1;
    }
    else
        return 0;
}