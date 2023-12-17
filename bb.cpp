#include <bits/stdc++.h>
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
    if (left.block == right.block)
        if (left.dir == Upp)
            return true;
        else if (left.dir == Low)
            if (right.dir != Upp)
                return true;
            else
                return false;
        else if (left.dir == Right)
            if (right.dir == Upp || right.dir == Low)
                return false;
            else
                return true;
        else
            return false;
    else
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

// 一方向(上)からの積み替え・取り出し
int bb1(Instance &instance, int UB_cur, clock_t start, int &T, int T_temp)
{
    // 節点の深さ
    static int depth = 0;
    // 最適解
    static int min_rel = 0;
    static int ans;
    if (((double)(clock() - start) / CLOCKS_PER_SEC) > 1800)
    {
        depth = 0;
        T = max(T, T_temp);
        return -1;
    }
    if (instance.config.UB1 == UB_cur)
    {
        depth = 0;
        T = instance.config.UBT;
        return min_rel = instance.config.UB1;
    }
    if (depth + instance.config.LB1 == UB_cur - 1)
    {
        int T_temp2 = T_temp;
        int UB_temp = instance.UB1(T_temp2);
        if (instance.config.UB1 > UB_temp + depth)
        {
            instance.config.UBT = max(instance.config.UBT, max(T, T_temp2));
            instance.config.UB1 = UB_temp + depth;
            if (instance.config.UB1 == UB_cur)
            {
                depth = 0;
                T = instance.config.UBT;
                return min_rel = instance.config.UB1;
            }
        }
    }
    int block1 = instance.config.count(Upp);
    while (block1 == 0)
    {
        T = max(T, T_temp);
        T_temp = 0;
        Point src = instance.config.pos[instance.config.priority - 1];
        instance.config.retrieve(src);

#if TEST == 0
        printf("Number Retrieval:%d\n", instance.config.priority - 1);
        instance.config.print();
#endif

        if (instance.config.priority == instance.nblock + 1)
        {
            min_rel = depth;
            depth = 0;
            T = max(T, T_temp);
            return min_rel;
        }
        block1 = instance.config.count(Upp);
    }
    // ブロッキングブロックを積み替える場合
    // 積み替え元
    Point src = {instance.config.pos[instance.config.priority - 1].x, TIER - 1};
    // 積み替えるブロック位置を特定
    while (!instance.config.block[src.x][src.y])
    {
        src.y--;
    }
    depth++;
    T_temp++;

#if TEST == 0
    printf("depth = %d\n", depth);
#endif

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
    if (!vec_dst.empty())
    {
        // 積み替え先を下界値の小さい順に並べる.下界値が等しい場合，最大優先度の降順に並べる
        sort(vec_dst.begin(), vec_dst.end(), asc_desc);
        if (block1 == 1)
        {
            Instance instance_temp;
            for (auto it = vec_dst.begin(); it != vec_dst.end(); it++)
            {
                if (it->LB + depth > UB_cur)
                {
                    break;
                }
                instance.config.relocate(src, it->dst);

#if TEST == 0
                instance.config.print();
#endif

                instance_temp = instance;
                ans = bb1(instance_temp, UB_cur, start, T, T_temp);
                if (ans != -1 && ans != 0)
                {
                    return min_rel;
                }
                else if (ans == -1)
                {
                    return -1;
                }
                instance.config.relocate(it->dst, src);
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

#if TEST == 0
                instance.config.print();
#endif

                ans = bb1(instance, UB_cur, start, T, T_temp);
                if (ans != -1 && ans != 0)
                {
                    return min_rel;
                }
                else if (ans == -1)
                {
                    return -1;
                }
                instance.config.relocate(it->dst, src);
            }
        }
    }
    depth--;
    T_temp--;
    if (depth == 0)
    {
        UB_cur++;

#if TEST == 0
        printf("UB_cur++\n");
#endif

        ans = bb1(instance, UB_cur, start, T, T_temp);
        if (ans != -1 && ans != 0)
        {
            return min_rel;
        }
        else if (ans == -1)
            return -1;
    }
    else
        return 0;
}

// 二方向(上右)からの積み替え・取り出し
int bb2(Instance &instance, int UB_cur, Direction dir, clock_t start, int &T, int T_temp)
{
    // 節点の深さ
    static int depth = 0;
    // 最適解
    static int min_rel = 0;
    // 積み替え先を決定するための変数
    static int LB_temp;
    static int p;
    static Dst temp;
    static int ans;
    if (((double)(clock() - start) / CLOCKS_PER_SEC) > 1800)
    {
        depth = 0;
        T = max(T, T_temp);
        return -1;
    }
    if (instance.config.UB2 == UB_cur)
    {
        depth = 0;
        T = instance.config.UBT;
        return min_rel = instance.config.UB2;
    }
    if (depth + instance.config.LB2 == UB_cur - 1)
    {
        int T_temp2 = T_temp;
        int UB_temp = instance.UB2(Upp, Right, T_temp2);
        if (instance.config.UB2 > UB_temp + depth)
        {
            instance.config.UBT = max(instance.config.UBT, max(T, T_temp2));
            instance.config.UB2 = UB_temp + depth;
            if (instance.config.UB2 == UB_cur)
            {
                depth = 0;
                T = instance.config.UBT;
                return min_rel = instance.config.UB2;
            }
        }
    }
    Point src;
    int block1 = instance.config.count(Upp);
    int block2 = instance.config.count(Right);
    if (dir == Any)
    {
        while ((block1 == 0) || (block2 == 0))
        {
            T = max(T, T_temp);
            T_temp = 0;
            src = instance.config.pos[instance.config.priority - 1];
            instance.config.retrieve(src);

#if TEST == 0
            printf("Number Retrieval:%d\n", instance.config.priority - 1);
            instance.config.print();
#endif

            if (instance.config.priority == instance.nblock + 1)
            {
                min_rel = depth;
                depth = 0;
                T = max(T, T_temp);
                return min_rel;
            }
            block1 = instance.config.count(Upp);
            block2 = instance.config.count(Right);
        }
        vector<Src> src_vec;
        Src src_temp;
        src_temp.block = block1;
        src_temp.dir = Upp;
        src_vec.push_back(src_temp);
        src_temp.block = block2;
        src_temp.dir = Right;
        src_vec.push_back(src_temp);
        sort(src_vec.begin(), src_vec.end(), block_asc);
        depth++;
        T_temp++;

#if TEST == 0
        printf("depth = %d\n", depth);
#endif

        for (int i = 0; i < 2; i++)
        {
            if (depth + src_vec[i].block - 1 > UB_cur)
            {
                break;
            }
            // ブロッキングブロックを積み替える場合
            // 積み替え元
            switch (src_vec[i].dir)
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
            p = instance.config.block[src.x][src.y];
            instance.config.block[src.x][src.y] = 0;
            // 積み替え先スタックを決定
            // 上側からの積み替え
            for (dst.x = 0; dst.x < STACK; dst.x++)
            {
                if (!instance.config.block[dst.x][TIER - 1])
                {
                    while (!instance.config.block[dst.x][dst.y - 1] && (dst.y > 0))
                        dst.y--;
                    if ((src.x == dst.x) && (src.y == dst.y))
                    {
                        dst.y = TIER - 1;
                        continue;
                    }
                    if (src_vec[i].dir == Right && src.y == dst.y && instance.config.pos[instance.config.priority - 1].x < dst.x)
                    {
                        dst.y = TIER - 1;
                        continue;
                    }
                    instance.config.block[dst.x][dst.y] = p;
                    instance.config.pos[p - 1] = dst;
                    LB_temp = instance.LB2(Upp, Right, src_vec[i].dir);
                    temp.dst = dst;
                    temp.LB = LB_temp;
                    temp.p = instance.config.P_UL[dst.x];
                    vec_dst.push_back(temp);
                    instance.config.block[dst.x][dst.y] = 0;
                    dst.y = TIER - 1;
                }
            }
            dst.x = STACK - 1;
            // 右側からの積み替え
            for (dst.y = 0; dst.y < TIER; dst.y++)
            {
                if (!instance.config.block[STACK - 1][dst.y])
                {
                    while (!instance.config.block[dst.x - 1][dst.y] && (dst.x > 0))
                        dst.x--;
                    if ((src.y == dst.y) && (src.x == dst.x))
                    {
                        dst.x = STACK - 1;
                        continue;
                    }
                    if (src_vec[i].dir == Upp && src.x == dst.x && instance.config.pos[instance.config.priority - 1].y < dst.y)
                    {
                        dst.x = STACK - 1;
                        continue;
                    }
                    instance.config.block[dst.x][dst.y] = p;
                    instance.config.pos[p - 1] = dst;
                    LB_temp = instance.LB2(Upp, Right, src_vec[i].dir);
                    temp.dst = dst;
                    temp.LB = LB_temp;
                    temp.p = instance.config.P_LR[dst.y];
                    vec_dst.push_back(temp);
                    instance.config.block[dst.x][dst.y] = 0;
                    dst.x = STACK - 1;
                }
            }
            instance.config.block[src.x][src.y] = p;
            instance.config.pos[p - 1] = src;
            if (vec_dst.empty())
                continue;
            // 積み替え先を下界値の小さい順に並べる.下界値が等しい場合，最大優先度の降順に並べる
            sort(vec_dst.begin(), vec_dst.end(), asc_desc);
            if (src_vec[i].block == 1)
            {
                Instance instance_temp;
                for (auto it = vec_dst.begin(); it != vec_dst.end(); it++)
                {
                    if (it->LB + depth > UB_cur)
                    {
                        break;
                    }
                    instance.config.relocate(src, it->dst);

#if TEST == 0
                    instance.config.print();
#endif

                    instance_temp = instance;
                    ans = bb2(instance_temp, UB_cur, Any, start, T, T_temp);
                    if (ans != -1 && ans != 0)
                    {
                        return min_rel;
                    }
                    else if (ans == -1)
                    {
                        return -1;
                    }
                    instance.config.relocate(it->dst, src);

#if TEST == 0
                    instance.config.print();
#endif
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

#if TEST == 0
                    instance.config.print();
#endif

                    ans = bb2(instance, UB_cur, src_vec[i].dir, start, T, T_temp);
                    if (ans != -1 && ans != 0)
                    {
                        return min_rel;
                    }
                    else if (ans == -1)
                    {
                        return -1;
                    }
                    instance.config.relocate(it->dst, src);

#if TEST == 0
                    instance.config.print();
#endif
                }
            }
        }
    }
    else
    {
        int block;
        depth++;
        T_temp++;

#if TEST == 0
        printf("depth = %d\n", depth);
#endif

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
        p = instance.config.block[src.x][src.y];
        instance.config.block[src.x][src.y] = 0;
        // 積み替え先スタックを決定
        // 上側からの積み替え
        for (dst.x = 0; dst.x < STACK; dst.x++)
        {
            if (!instance.config.block[dst.x][TIER - 1])
            {
                while (!instance.config.block[dst.x][dst.y - 1] && (dst.y > 0))
                    dst.y--;
                if ((src.x == dst.x) && (src.y == dst.y))
                {
                    dst.y = TIER - 1;
                    continue;
                }
                if (dir == Right && src.y == dst.y && instance.config.pos[instance.config.priority - 1].x < dst.x)
                {
                    dst.y = TIER - 1;
                    continue;
                }
                instance.config.block[dst.x][dst.y] = p;
                instance.config.pos[p - 1] = dst;
                LB_temp = instance.LB2(Upp, Right, dir);
                temp.dst = dst;
                temp.LB = LB_temp;
                temp.p = instance.config.P_UL[dst.x];
                vec_dst.push_back(temp);
                instance.config.block[dst.x][dst.y] = 0;
                dst.y = TIER - 1;
            }
        }
        dst.x = STACK - 1;
        // 右側からの積み替え
        for (dst.y = 0; dst.y < TIER; dst.y++)
        {
            if (!instance.config.block[STACK - 1][dst.y])
            {
                while (!instance.config.block[dst.x - 1][dst.y] && (dst.x > 0))
                    dst.x--;
                if ((src.y == dst.y) && (src.x == dst.x))
                {
                    dst.x = STACK - 1;
                    continue;
                }
                if (dir == Upp && src.x == dst.x && instance.config.pos[instance.config.priority - 1].y < dst.y)
                {
                    dst.x = STACK - 1;
                    continue;
                }
                instance.config.block[dst.x][dst.y] = p;
                instance.config.pos[p - 1] = dst;
                LB_temp = instance.LB2(Upp, Right, dir);
                temp.dst = dst;
                temp.LB = LB_temp;
                temp.p = instance.config.P_LR[dst.y];
                vec_dst.push_back(temp);
                instance.config.block[dst.x][dst.y] = 0;
                dst.x = STACK - 1;
            }
        }
        instance.config.block[src.x][src.y] = p;
        instance.config.pos[p - 1] = src;
        if (!vec_dst.empty())
        {
            // 積み替え先を下界値の小さい順に並べる.下界値が等しい場合，最大優先度の降順に並べる
            sort(vec_dst.begin(), vec_dst.end(), asc_desc);
            if (block == 1)
            {
                Instance instance_temp;
                for (auto it = vec_dst.begin(); it != vec_dst.end(); it++)
                {
                    if (it->LB + depth > UB_cur)
                    {
                        break;
                    }
                    instance.config.relocate(src, it->dst);

#if TEST == 0
                    instance.config.print();
#endif

                    instance_temp = instance;
                    ans = bb2(instance_temp, UB_cur, Any, start, T, T_temp);
                    if (ans != -1 && ans != 0)
                    {
                        return min_rel;
                    }
                    else if (ans == -1)
                    {
                        return -1;
                    }
                    instance.config.relocate(it->dst, src);

#if TEST == 0
                    instance.config.print();
#endif
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

#if TEST == 0
                    instance.config.print();
#endif

                    ans = bb2(instance, UB_cur, dir, start, T, T_temp);
                    if (ans != -1 && ans != 0)
                    {
                        return min_rel;
                    }
                    else if (ans == -1)
                    {
                        return -1;
                    }
                    instance.config.relocate(it->dst, src);

#if TEST == 0
                    instance.config.print();
#endif
                }
            }
        }
    }
    depth--;
    T_temp--;
    if (depth == 0)
    {
        UB_cur++;

#if TEST == 0
        printf("UB_cur++\n");
#endif

        ans = bb2(instance, UB_cur, Any, start, T, T_temp);
        if (ans != -1 && ans != 0)
        {
            return min_rel;
        }
        else if (ans == -1)
            return -1;
    }
    else
        return 0;
}

// 二方向(上下)からの積み替え・取り出し
int bb2a(Instance &instance, int UB_cur, Direction dir, clock_t start, int &T, int T_temp)
{
    // 節点の深さ
    static int depth = 0;
    // 最適解
    static int min_rel = 0;
    // 積み替え先を決定するための変数
    static int LB_temp;
    static int p;
    static Dst temp;
    static int ans;
    if (((double)(clock() - start) / CLOCKS_PER_SEC) > 1800)
    {
        depth = 0;
        T = max(T, T_temp);
        return -1;
    }
    if (instance.config.UB2s == UB_cur)
    {
        depth = 0;
        T = instance.config.UBT;
        return min_rel = instance.config.UB2s;
    }
    if (depth + instance.config.LB2a == UB_cur - 1)
    {
        int T_temp2 = T_temp;
        int UB_temp = instance.UB2s(Upp, Low, T_temp2);
        if (instance.config.UB2s > UB_temp + depth)
        {
            instance.config.UBT = max(instance.config.UBT, max(T, T_temp2));
            instance.config.UB2s = UB_temp + depth;
            if (instance.config.UB2s == UB_cur)
            {
                depth = 0;
                T = instance.config.UBT;
                return min_rel = instance.config.UB2s;
            }
        }
    }
    Point src;
    int block1 = instance.config.count(Upp);
    int block2 = instance.config.count(Low);
    if (dir == Any)
    {
        while ((block1 == 0) || (block2 == 0))
        {
            T = max(T, T_temp);
            T_temp = 0;
            src = instance.config.pos[instance.config.priority - 1];
            instance.config.retrieve(src);

#if TEST == 0
            printf("Number Retrieval:%d\n", instance.config.priority - 1);
            instance.config.print();
#endif

            if (instance.config.priority == instance.nblock + 1)
            {
                min_rel = depth;
                depth = 0;
                T = max(T, T_temp);
                return min_rel;
            }
            block1 = instance.config.count(Upp);
            block2 = instance.config.count(Low);
        }
        vector<Src> src_vec;
        Src src_temp;
        src_temp.block = block1;
        src_temp.dir = Upp;
        src_vec.push_back(src_temp);
        src_temp.block = block2;
        src_temp.dir = Low;
        src_vec.push_back(src_temp);
        sort(src_vec.begin(), src_vec.end(), block_asc);
        depth++;
        T_temp++;

#if TEST == 0
        printf("depth = %d\n", depth);
#endif

        for (int i = 0; i < 2; i++)
        {
            // ブロッキングブロックを積み替える場合
            // 積み替え元
            switch (src_vec[i].dir)
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
            case Low:
                src.x = instance.config.pos[instance.config.priority - 1].x;
                src.y = 0;
                // 積み替えるブロック位置を特定
                while (!instance.config.block[src.x][src.y])
                {
                    src.y++;
                }
                break;
            default:
                break;
            }
            vector<Dst> vec_dst;
            Point dst = {0, TIER - 1};
            p = instance.config.block[src.x][src.y];
            instance.config.block[src.x][src.y] = 0;
            // 積み替え先スタックを決定
            // 上側からの積み替え
            for (dst.x = 0; dst.x < STACK; dst.x++)
            {
                if (!instance.config.block[dst.x][TIER - 1])
                {
                    while (!instance.config.block[dst.x][dst.y - 1] && (dst.y > 0))
                        dst.y--;
                    if ((src.x == dst.x) && (src.y == dst.y))
                    {
                        dst.y = TIER - 1;
                        continue;
                    }
                    instance.config.block[dst.x][dst.y] = p;
                    instance.config.pos[p - 1] = dst;
                    LB_temp = instance.LB2a(src_vec[i].dir);
                    Dst temp = {dst, LB_temp, instance.config.P_UL[dst.x]};
                    vec_dst.push_back(temp);
                    instance.config.block[dst.x][dst.y] = 0;
                    dst.y = TIER - 1;
                }
            }
            dst.y = 0;
            // 下側からの積み替え
            for (dst.x = 0; dst.x < STACK; dst.x++)
            {
                if (!instance.config.block[dst.x][0])
                {
                    while (!instance.config.block[dst.x][dst.y + 1] && (dst.y < TIER - 1))
                        dst.y++;
                    if ((src.x == dst.x) && (src.y == dst.y))
                    {
                        dst.y = 0;
                        continue;
                    }
                    instance.config.block[dst.x][dst.y] = p;
                    instance.config.pos[p - 1] = dst;
                    LB_temp = instance.LB2a(src_vec[i].dir);
                    Dst temp = {dst, LB_temp, instance.config.P_UL[dst.x]};
                    vec_dst.push_back(temp);
                    instance.config.block[dst.x][dst.y] = 0;
                    dst.y = 0;
                }
            }
            instance.config.block[src.x][src.y] = p;
            instance.config.pos[p - 1] = src;
            if (vec_dst.empty())
                continue;
            // 積み替え先を下界値の小さい順に並べる.下界値が等しい場合，最大優先度の降順に並べる
            sort(vec_dst.begin(), vec_dst.end(), asc_desc);
            if (src_vec[i].block == 1)
            {
                Instance instance_temp;
                for (auto it = vec_dst.begin(); it != vec_dst.end(); it++)
                {
                    if (it->LB + depth > UB_cur)
                    {
                        break;
                    }
                    instance.config.relocate(src, it->dst);

#if TEST == 0
                    instance.config.print();
#endif

                    instance_temp = instance;
                    ans = bb2a(instance_temp, UB_cur, Any, start, T, T_temp);
                    if (ans != -1 && ans != 0)
                    {
                        return min_rel;
                    }
                    else if (ans == -1)
                    {
                        return -1;
                    }
                    instance.config.relocate(it->dst, src);
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

#if TEST == 0
                    instance.config.print();
#endif

                    ans = bb2a(instance, UB_cur, src_vec[i].dir, start, T, T_temp);
                    if (ans != -1 && ans != 0)
                    {
                        return min_rel;
                    }
                    else if (ans == -1)
                    {
                        return -1;
                    }
                    instance.config.relocate(it->dst, src);
                }
            }
        }
    }
    else
    {
        int block;
        depth++;
        T_temp++;

#if TEST == 0
        printf("depth = %d\n", depth);
#endif

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
        else if (dir == Low)
        {
            src.x = instance.config.pos[instance.config.priority - 1].x;
            src.y = 0;
            // 積み替えるブロック位置を特定
            while (!instance.config.block[src.x][src.y])
            {
                src.y++;
            }
            block = block2;
        }
        vector<Dst> vec_dst;
        Point dst = {0, TIER - 1};
        p = instance.config.block[src.x][src.y];
        instance.config.block[src.x][src.y] = 0;
        // 積み替え先スタックを決定
        // 上側からの積み替え
        for (dst.x = 0; dst.x < STACK; dst.x++)
        {
            if (!instance.config.block[dst.x][TIER - 1])
            {
                int LB_temp = 0;
                while (!instance.config.block[dst.x][dst.y - 1] && (dst.y > 0))
                    dst.y--;
                if ((src.x == dst.x) && (src.y == dst.y))
                {
                    dst.y = TIER - 1;
                    continue;
                }
                instance.config.block[dst.x][dst.y] = p;
                instance.config.pos[p - 1] = dst;
                LB_temp = instance.LB2a(dir);
                Dst temp = {dst, LB_temp, instance.config.P_UL[dst.x]};
                vec_dst.push_back(temp);
                instance.config.block[dst.x][dst.y] = 0;
                dst.y = TIER - 1;
            }
        }
        dst.y = 0;
        // 下側からの積み替え
        for (dst.x = 0; dst.x < STACK; dst.x++)
        {
            if (!instance.config.block[dst.x][0])
            {
                int LB_temp = 0;
                while (!instance.config.block[dst.x][dst.y + 1] && (dst.y < TIER - 1))
                    dst.y++;
                if ((src.x == dst.x) && (src.y == dst.y))
                {
                    dst.y = 0;
                    continue;
                }
                instance.config.block[dst.x][dst.y] = p;
                instance.config.pos[p - 1] = dst;
                LB_temp = instance.LB2a(dir);
                Dst temp = {dst, LB_temp, instance.config.P_UL[dst.x]};
                vec_dst.push_back(temp);
                instance.config.block[dst.x][dst.y] = 0;
                dst.y = 0;
            }
        }
        instance.config.block[src.x][src.y] = p;
        instance.config.pos[p - 1] = src;
        if (!vec_dst.empty())
        {
            // 積み替え先を下界値の小さい順に並べる.下界値が等しい場合，最大優先度の降順に並べる
            sort(vec_dst.begin(), vec_dst.end(), asc_desc);
            if (block == 1)
            {
                Instance instance_temp;
                for (auto it = vec_dst.begin(); it != vec_dst.end(); it++)
                {
                    if (it->LB + depth > UB_cur)
                    {
                        break;
                    }
                    instance.config.relocate(src, it->dst);

#if TEST == 0
                    instance.config.print();
#endif

                    instance_temp = instance;
                    ans = bb2a(instance_temp, UB_cur, Any, start, T, T_temp);
                    if (ans != -1 && ans != 0)
                    {
                        return min_rel;
                    }
                    else if (ans == -1)
                    {
                        return -1;
                    }
                    instance.config.relocate(it->dst, src);
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

#if TEST == 0
                    instance.config.print();
#endif

                    ans = bb2a(instance, UB_cur, dir, start, T, T_temp);
                    if (ans != -1 && ans != 0)
                    {
                        return min_rel;
                    }
                    else if (ans == -1)
                    {
                        return -1;
                    }
                    instance.config.relocate(it->dst, src);
                }
            }
        }
    }
    depth--;
    T_temp--;
    if (depth == 0)
    {
        UB_cur++;

#if TEST == 0
        printf("UB_cur++\n");
#endif

        ans = bb2a(instance, UB_cur, Any, start, T, T_temp);
        if (ans != -1 && ans != 0)
        {
            return min_rel;
        }
        else if (ans == -1)
            return -1;
    }
    else
        return 0;
}

// 三方向(上右下)からの積み替え・取り出し
int bb3(Instance &instance, int UB_cur, Direction dir, clock_t start, int &T, int T_temp)
{
    // 節点の深さ
    static int depth = 0;
    // 最適解
    static int min_rel = 0;
    // 積み替え先を決定するための変数
    static int LB_temp;
    static int p;
    static Dst temp;
    static int ans;
    if (((double)(clock() - start) / CLOCKS_PER_SEC) > 1800)
    {
        depth = 0;
        T = max(T, T_temp);
        return -1;
    }
    if (instance.config.UB3 == UB_cur)
    {
        depth = 0;
        T = instance.config.UBT;
        return min_rel = instance.config.UB3;
    }
    if (depth + instance.config.LB3 == UB_cur - 1)
    {
        int T_temp2 = T_temp;
        int UB_temp = instance.UB3(Upp, Right, Low, T_temp2);
        if (instance.config.UB3 > UB_temp + depth)
        {
            instance.config.UBT = max(instance.config.UBT, max(T, T_temp2));
            instance.config.UB3 = UB_temp + depth;
            if (instance.config.UB3 == UB_cur)
            {
                depth = 0;
                T = instance.config.UBT;
                return min_rel = instance.config.UB3;
            }
        }
    }
    Point src;
    int block1 = instance.config.count(Upp);
    int block2 = instance.config.count(Right);
    int block3 = instance.config.count(Low);
    if (dir == Any)
    {
        while ((block1 == 0) || (block2 == 0) || (block3 == 0))
        {
            T = max(T, T_temp);
            T_temp = 0;
            src = instance.config.pos[instance.config.priority - 1];
            instance.config.retrieve(src);

#if TEST == 0
            printf("Number Retrieval:%d\n", instance.config.priority - 1);
            instance.config.print();
#endif

            if (instance.config.priority == instance.nblock + 1)
            {
                min_rel = depth;
                depth = 0;
                T = max(T, T_temp);
                return min_rel;
            }
            block1 = instance.config.count(Upp);
            block2 = instance.config.count(Right);
            block3 = instance.config.count(Low);
        }
        vector<Src> src_vec;
        Src src_temp;
        src_temp.block = block1;
        src_temp.dir = Upp;
        src_vec.push_back(src_temp);
        src_temp.block = block2;
        src_temp.dir = Right;
        src_vec.push_back(src_temp);
        src_temp.block = block3;
        src_temp.dir = Low;
        src_vec.push_back(src_temp);
        sort(src_vec.begin(), src_vec.end(), block_asc);
        depth++;
        T_temp++;

#if TEST == 0
        printf("depth = %d\n", depth);
#endif

        for (int i = 0; i < 3; i++)
        {
            if (depth + src_vec[i].block - 1 > UB_cur)
            {
                break;
            }
            // ブロッキングブロックを積み替える場合
            // 積み替え元
            switch (src_vec[i].dir)
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
            case Low:
                src.x = instance.config.pos[instance.config.priority - 1].x;
                src.y = 0;
                // 積み替えるブロック位置を特定
                while (!instance.config.block[src.x][src.y])
                {
                    src.y++;
                }
                break;
            default:
                break;
            }
            vector<Dst> vec_dst;
            Point dst = {0, TIER - 1};
            p = instance.config.block[src.x][src.y];
            instance.config.block[src.x][src.y] = 0;
            // 積み替え先スタックを決定
            // 上側からの積み替え
            for (dst.x = 0; dst.x < STACK; dst.x++)
            {
                if (!instance.config.block[dst.x][TIER - 1])
                {
                    while (!instance.config.block[dst.x][dst.y - 1] && (dst.y > 0))
                        dst.y--;
                    if ((src.x == dst.x) && (src.y == dst.y))
                    {
                        dst.y = TIER - 1;
                        continue;
                    }
                    if (src_vec[i].dir == Right && src.y == dst.y && instance.config.pos[instance.config.priority - 1].x < dst.x)
                    {
                        dst.y = TIER - 1;
                        continue;
                    }
                    instance.config.block[dst.x][dst.y] = p;
                    instance.config.pos[p - 1] = dst;
                    LB_temp = instance.LB3(Upp, Right, Low, src_vec[i].dir);
                    temp.dst = dst;
                    temp.LB = LB_temp;
                    temp.p = instance.config.P_UL[dst.x];
                    vec_dst.push_back(temp);
                    instance.config.block[dst.x][dst.y] = 0;
                    dst.y = TIER - 1;
                }
            }
            dst.x = STACK - 1;
            // 右側からの積み替え
            for (dst.y = 0; dst.y < TIER; dst.y++)
            {
                if (!instance.config.block[STACK - 1][dst.y])
                {
                    while (!instance.config.block[dst.x - 1][dst.y] && (dst.x > 0))
                        dst.x--;
                    if ((src.y == dst.y) && (src.x == dst.x))
                    {
                        dst.x = STACK - 1;
                        continue;
                    }
                    if (dir == Upp && src.x == dst.x && instance.config.pos[instance.config.priority - 1].y < dst.y)
                    {
                        dst.x = STACK - 1;
                        continue;
                    }
                    if (dir == Low && src.x == dst.x && instance.config.pos[instance.config.priority - 1].y > dst.y)
                    {
                        dst.x = STACK - 1;
                        continue;
                    }
                    instance.config.block[dst.x][dst.y] = p;
                    instance.config.pos[p - 1] = dst;
                    LB_temp = instance.LB3(Upp, Right, Low, src_vec[i].dir);
                    temp.dst = dst;
                    temp.LB = LB_temp;
                    temp.p = instance.config.P_LR[dst.y];
                    vec_dst.push_back(temp);
                    instance.config.block[dst.x][dst.y] = 0;
                    dst.x = STACK - 1;
                }
            }
            dst.y = 0;
            // 下側からの積み替え
            for (dst.x = 0; dst.x < STACK; dst.x++)
            {
                if (!instance.config.block[dst.x][0])
                {
                    while (!instance.config.block[dst.x][dst.y + 1] && (dst.y < TIER - 1))
                        dst.y++;
                    if ((src.x == dst.x) && (src.y == dst.y))
                    {
                        dst.y = 0;
                        continue;
                    }
                    if (dir == Right && src.y == dst.y && instance.config.pos[instance.config.priority - 1].x < dst.x)
                    {
                        dst.y = 0;
                        continue;
                    }
                    instance.config.block[dst.x][dst.y] = p;
                    instance.config.pos[p - 1] = dst;
                    LB_temp = instance.LB3(Upp, Right, Low, src_vec[i].dir);
                    temp.dst = dst;
                    temp.LB = LB_temp;
                    temp.p = instance.config.P_UL[dst.x];
                    vec_dst.push_back(temp);
                    instance.config.block[dst.x][dst.y] = 0;
                    dst.y = 0;
                }
            }
            instance.config.block[src.x][src.y] = p;
            instance.config.pos[p - 1] = src;
            if (vec_dst.empty())
                continue;
            // 積み替え先を下界値の小さい順に並べる.下界値が等しい場合，最大優先度の降順に並べる
            sort(vec_dst.begin(), vec_dst.end(), asc_desc);
            if (src_vec[i].block == 1)
            {
                Instance instance_temp;
                for (auto it = vec_dst.begin(); it != vec_dst.end(); it++)
                {
                    if (it->LB + depth > UB_cur)
                    {
                        break;
                    }
                    instance.config.relocate(src, it->dst);

#if TEST == 0
                    instance.config.print();
#endif

                    instance_temp = instance;
                    ans = bb3(instance_temp, UB_cur, Any, start, T, T_temp);
                    if (ans != -1 && ans != 0)
                    {
                        return min_rel;
                    }
                    else if (ans == -1)
                    {
                        return -1;
                    }
                    instance.config.relocate(it->dst, src);

#if TEST == 0
                    instance.config.print();
#endif
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

#if TEST == 0
                    instance.config.print();
#endif

                    ans = bb3(instance, UB_cur, src_vec[i].dir, start, T, T_temp);
                    if (ans != -1 && ans != 0)
                    {
                        return min_rel;
                    }
                    else if (ans == -1)
                    {
                        return -1;
                    }
                    instance.config.relocate(it->dst, src);

#if TEST == 0
                    instance.config.print();
#endif
                }
            }
        }
    }
    else
    {
        int block;
        depth++;
        T_temp++;

#if TEST == 0
        printf("depth = %d\n", depth);
#endif

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
        else if (dir == Low)
        {
            src.x = instance.config.pos[instance.config.priority - 1].x;
            src.y = 0;
            // 積み替えるブロック位置を特定
            while (!instance.config.block[src.x][src.y])
            {
                src.y++;
            }
            block = block3;
        }
        vector<Dst> vec_dst;
        Point dst = {0, TIER - 1};
        p = instance.config.block[src.x][src.y];
        instance.config.block[src.x][src.y] = 0;
        // 積み替え先スタックを決定
        // 上側からの積み替え
        for (dst.x = 0; dst.x < STACK; dst.x++)
        {
            if (!instance.config.block[dst.x][TIER - 1])
            {
                while (!instance.config.block[dst.x][dst.y - 1] && (dst.y > 0))
                    dst.y--;
                if ((src.x == dst.x) && (src.y == dst.y))
                {
                    dst.y = TIER - 1;
                    continue;
                }
                if (dir == Right && src.y == dst.y && instance.config.pos[instance.config.priority - 1].x < dst.x)
                {
                    dst.y = TIER - 1;
                    continue;
                }
                instance.config.block[dst.x][dst.y] = p;
                instance.config.pos[p - 1] = dst;
                LB_temp = instance.LB3(Upp, Right, Low, dir);
                temp.dst = dst;
                temp.LB = LB_temp;
                temp.p = instance.config.P_UL[dst.x];
                vec_dst.push_back(temp);
                instance.config.block[dst.x][dst.y] = 0;
                dst.y = TIER - 1;
            }
        }
        dst.x = STACK - 1;
        // 右側からの積み替え
        for (dst.y = 0; dst.y < TIER; dst.y++)
        {
            if (!instance.config.block[STACK - 1][dst.y])
            {
                while (!instance.config.block[dst.x - 1][dst.y] && (dst.x > 0))
                    dst.x--;
                if ((src.y == dst.y) && (src.x == dst.x))
                {
                    dst.x = STACK - 1;
                    continue;
                }
                if (dir == Upp && src.x == dst.x && instance.config.pos[instance.config.priority - 1].y < dst.y)
                {
                    dst.x = STACK - 1;
                    continue;
                }
                if (dir == Low && src.x == dst.x && instance.config.pos[instance.config.priority - 1].y > dst.y)
                {
                    dst.x = STACK - 1;
                    continue;
                }
                instance.config.block[dst.x][dst.y] = p;
                instance.config.pos[p - 1] = dst;
                LB_temp = instance.LB3(Upp, Right, Low, dir);
                temp.dst = dst;
                temp.LB = LB_temp;
                temp.p = instance.config.P_LR[dst.y];
                vec_dst.push_back(temp);
                instance.config.block[dst.x][dst.y] = 0;
                dst.x = STACK - 1;
            }
        }
        dst.y = 0;
        // 下側からの積み替え
        for (dst.x = 0; dst.x < STACK; dst.x++)
        {
            if (!instance.config.block[dst.x][0])
            {
                while (!instance.config.block[dst.x][dst.y + 1] && (dst.y < TIER - 1))
                    dst.y++;
                if ((src.x == dst.x) && (src.y == dst.y))
                {
                    dst.y = 0;
                    continue;
                }
                if (dir == Right && src.y == dst.y && instance.config.pos[instance.config.priority - 1].x < dst.x)
                {
                    dst.y = 0;
                    continue;
                }
                instance.config.block[dst.x][dst.y] = p;
                instance.config.pos[p - 1] = dst;
                LB_temp = instance.LB3(Upp, Right, Low, dir);
                temp.dst = dst;
                temp.LB = LB_temp;
                temp.p = instance.config.P_UL[dst.x];
                vec_dst.push_back(temp);
                instance.config.block[dst.x][dst.y] = 0;
                dst.y = 0;
            }
        }
        instance.config.block[src.x][src.y] = p;
        instance.config.pos[p - 1] = src;
        if (!vec_dst.empty())
        {
            // 積み替え先を下界値の小さい順に並べる.下界値が等しい場合，最大優先度の降順に並べる
            sort(vec_dst.begin(), vec_dst.end(), asc_desc);
            if (block == 1)
            {
                Instance instance_temp;
                for (auto it = vec_dst.begin(); it != vec_dst.end(); it++)
                {
                    if (it->LB + depth > UB_cur)
                    {
                        break;
                    }
                    instance.config.relocate(src, it->dst);

#if TEST == 0
                    instance.config.print();
#endif

                    instance_temp = instance;
                    ans = bb3(instance_temp, UB_cur, Any, start, T, T_temp);
                    if (ans != -1 && ans != 0)
                    {
                        return min_rel;
                    }
                    else if (ans == -1)
                    {
                        return -1;
                    }
                    instance.config.relocate(it->dst, src);

#if TEST == 0
                    instance.config.print();
#endif
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

#if TEST == 0
                    instance.config.print();
#endif

                    ans = bb3(instance, UB_cur, dir, start, T, T_temp);
                    if (ans != -1 && ans != 0)
                    {
                        return min_rel;
                    }
                    else if (ans == -1)
                    {
                        return -1;
                    }
                    instance.config.relocate(it->dst, src);

#if TEST == 0
                    instance.config.print();
#endif
                }
            }
        }
    }
    depth--;
    T_temp--;
    if (depth == 0)
    {
        UB_cur++;

#if TEST == 0
        printf("UB_cur++\n");
#endif

        ans = bb3(instance, UB_cur, Any, start, T, T_temp);
        if (ans != -1 && ans != 0)
        {
            return min_rel;
        }
        else if (ans == -1)
            return -1;
    }
    else
        return 0;
}

// 四方向(上右下左)からの積み替え・取り出し
int bb4(Instance &instance, int UB_cur, Direction dir, clock_t start, int &T, int T_temp)
{
    // 節点の深さ
    static int depth = 0;
    // 最適解
    static int min_rel = 0;
    // 積み替え先を決定するための変数
    static int LB_temp;
    static int p;
    static Dst temp;
    static int ans;
    if (((double)(clock() - start) / CLOCKS_PER_SEC) > 1800)
    {
        depth = 0;
        T = max(T, T_temp);
        return -1;
    }
    if (instance.config.UB4 == UB_cur)
    {
        depth = 0;
        T = instance.config.UBT;
        return min_rel = instance.config.UB4;
    }
    if (depth + instance.config.LB4 == UB_cur - 1)
    {
        int T_temp2 = T_temp;
        int UB_temp = instance.UB4(Upp, Right, Low, Left, T_temp2);
        if (instance.config.UB4 > UB_temp + depth)
        {
            instance.config.UBT = max(instance.config.UBT, max(T, T_temp2));
            instance.config.UB4 = UB_temp + depth;
            if (instance.config.UB4 == UB_cur)
            {
                depth = 0;
                T = instance.config.UBT;
                return min_rel = instance.config.UB4;
            }
        }
    }
    Point src;
    int block1 = instance.config.count(Upp);
    int block2 = instance.config.count(Right);
    int block3 = instance.config.count(Low);
    int block4 = instance.config.count(Left);
    if (dir == Any)
    {
        while ((block1 == 0) || (block2 == 0) || (block3 == 0) || (block4 == 0))
        {
            T = max(T, T_temp);
            T_temp = 0;
            src = instance.config.pos[instance.config.priority - 1];
            instance.config.retrieve(src);

#if TEST == 0
            printf("Number Retrieval:%d\n", instance.config.priority - 1);
            instance.config.print();
#endif

            if (instance.config.priority == instance.nblock + 1)
            {
                min_rel = depth;
                depth = 0;
                T = max(T, T_temp);
                return min_rel;
            }
            block1 = instance.config.count(Upp);
            block2 = instance.config.count(Right);
            block3 = instance.config.count(Low);
            block4 = instance.config.count(Left);
        }
        vector<Src> src_vec;
        Src src_temp;
        src_temp.block = block1;
        src_temp.dir = Upp;
        src_vec.push_back(src_temp);
        src_temp.block = block2;
        src_temp.dir = Right;
        src_vec.push_back(src_temp);
        src_temp.block = block3;
        src_temp.dir = Low;
        src_vec.push_back(src_temp);
        src_temp.block = block4;
        src_temp.dir = Left;
        src_vec.push_back(src_temp);
        sort(src_vec.begin(), src_vec.end(), block_asc);
        depth++;
        T_temp++;

#if TEST == 0
        printf("depth = %d\n", depth);
#endif

        for (int i = 0; i < 4; i++)
        {
            if (depth + src_vec[i].block - 1 > UB_cur)
            {
                break;
            }
            // ブロッキングブロックを積み替える場合
            // 積み替え元
            switch (src_vec[i].dir)
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
            case Low:
                src.x = instance.config.pos[instance.config.priority - 1].x;
                src.y = 0;
                // 積み替えるブロック位置を特定
                while (!instance.config.block[src.x][src.y])
                {
                    src.y++;
                }
                break;
            case Left:
                src.y = instance.config.pos[instance.config.priority - 1].y;
                src.x = 0;
                // 積み替えるブロック位置を特定
                while (!instance.config.block[src.x][src.y])
                {
                    src.x++;
                }
                break;
            default:
                break;
            }
            vector<Dst> vec_dst;
            Point dst = {0, TIER - 1};
            p = instance.config.block[src.x][src.y];
            instance.config.block[src.x][src.y] = 0;
            // 積み替え先スタックを決定
            // 上側からの積み替え
            for (dst.x = 0; dst.x < STACK; dst.x++)
            {
                if (!instance.config.block[dst.x][TIER - 1])
                {
                    while (!instance.config.block[dst.x][dst.y - 1] && (dst.y > 0))
                        dst.y--;
                    if ((src.x == dst.x) && (src.y == dst.y))
                    {
                        dst.y = TIER - 1;
                        continue;
                    }
                    if (src_vec[i].dir == Right && src.y == dst.y && instance.config.pos[instance.config.priority - 1].x < dst.x)
                    {
                        dst.y = TIER - 1;
                        continue;
                    }
                    if (src_vec[i].dir == Left && src.y == dst.y && instance.config.pos[instance.config.priority - 1].x > dst.x)
                    {
                        dst.y = TIER - 1;
                        continue;
                    }
                    instance.config.block[dst.x][dst.y] = p;
                    instance.config.pos[p - 1] = dst;
                    LB_temp = instance.LB4(Upp, Right, Low, Left, src_vec[i].dir);
                    temp.dst = dst;
                    temp.LB = LB_temp;
                    temp.p = instance.config.P_UL[dst.x];
                    vec_dst.push_back(temp);
                    instance.config.block[dst.x][dst.y] = 0;
                    dst.y = TIER - 1;
                }
            }
            dst.x = STACK - 1;
            // 右側からの積み替え
            for (dst.y = 0; dst.y < TIER; dst.y++)
            {
                if (!instance.config.block[STACK - 1][dst.y])
                {
                    while (!instance.config.block[dst.x - 1][dst.y] && (dst.x > 0))
                        dst.x--;
                    if ((src.y == dst.y) && (src.x == dst.x))
                    {
                        dst.x = STACK - 1;
                        continue;
                    }
                    if (src_vec[i].dir == Upp && src.x == dst.x && instance.config.pos[instance.config.priority - 1].y < dst.y)
                    {
                        dst.x = STACK - 1;
                        continue;
                    }
                    if (src_vec[i].dir == Low && src.x == dst.x && instance.config.pos[instance.config.priority - 1].y > dst.y)
                    {
                        dst.x = STACK - 1;
                        continue;
                    }
                    instance.config.block[dst.x][dst.y] = p;
                    instance.config.pos[p - 1] = dst;
                    LB_temp = instance.LB4(Upp, Right, Low, Left, src_vec[i].dir);
                    temp.dst = dst;
                    temp.LB = LB_temp;
                    temp.p = instance.config.P_LR[dst.y];
                    vec_dst.push_back(temp);
                    instance.config.block[dst.x][dst.y] = 0;
                    dst.x = STACK - 1;
                }
            }
            dst.y = 0;
            // 下側からの積み替え
            for (dst.x = 0; dst.x < STACK; dst.x++)
            {
                if (!instance.config.block[dst.x][0])
                {
                    while (!instance.config.block[dst.x][dst.y + 1] && (dst.y < TIER - 1))
                        dst.y++;
                    if ((src.x == dst.x) && (src.y == dst.y))
                    {
                        dst.y = 0;
                        continue;
                    }
                    if (src_vec[i].dir == Right && src.y == dst.y && instance.config.pos[instance.config.priority - 1].x < dst.x)
                    {
                        dst.y = 0;
                        continue;
                    }
                    if (src_vec[i].dir == Left && src.y == dst.y && instance.config.pos[instance.config.priority - 1].x > dst.x)
                    {
                        dst.y = 0;
                        continue;
                    }
                    instance.config.block[dst.x][dst.y] = p;
                    instance.config.pos[p - 1] = dst;
                    LB_temp = instance.LB4(Upp, Right, Low, Left, src_vec[i].dir);
                    temp.dst = dst;
                    temp.LB = LB_temp;
                    temp.p = instance.config.P_UL[dst.x];
                    vec_dst.push_back(temp);
                    instance.config.block[dst.x][dst.y] = 0;
                    dst.y = 0;
                }
            }
            dst.x = 0;
            // 左側からの積み替え
            for (dst.y = 0; dst.y < TIER; dst.y++)
            {
                if (!instance.config.block[0][dst.y])
                {
                    while (!instance.config.block[dst.x + 1][dst.y] && (dst.x < STACK - 1))
                        dst.x++;
                    if ((src.y == dst.y) && (src.x == dst.x))
                    {
                        dst.x = 0;
                        continue;
                    }
                    if (src_vec[i].dir == Upp && src.x == dst.x && instance.config.pos[instance.config.priority - 1].y < dst.y)
                    {
                        dst.x = 0;
                        continue;
                    }
                    if (src_vec[i].dir == Low && src.x == dst.x && instance.config.pos[instance.config.priority - 1].y > dst.y)
                    {
                        dst.x = 0;
                        continue;
                    }
                    instance.config.block[dst.x][dst.y] = p;
                    instance.config.pos[p - 1] = dst;
                    LB_temp = instance.LB4(Upp, Right, Low, Left, src_vec[i].dir);
                    temp.dst = dst;
                    temp.LB = LB_temp;
                    temp.p = instance.config.P_LR[dst.y];
                    vec_dst.push_back(temp);
                    instance.config.block[dst.x][dst.y] = 0;
                    dst.x = 0;
                }
            }
            instance.config.block[src.x][src.y] = p;
            instance.config.pos[p - 1] = src;
            if (vec_dst.empty())
            {
                continue;
            }
            // 積み替え先を下界値の小さい順に並べる.下界値が等しい場合，最大優先度の降順に並べる
            sort(vec_dst.begin(), vec_dst.end(), asc_desc);
            if (src_vec[i].block == 1)
            {
                Instance instance_temp;
                for (auto it = vec_dst.begin(); it != vec_dst.end(); it++)
                {
                    if (it->LB + depth > UB_cur)
                    {
                        break;
                    }
                    instance.config.relocate(src, it->dst);

#if TEST == 0
                    instance.config.print();
#endif

                    instance_temp = instance;
                    ans = bb4(instance_temp, UB_cur, Any, start, T, T_temp);
                    if (ans != -1 && ans != 0)
                    {
                        return min_rel;
                    }
                    else if (ans == -1)
                    {
                        return -1;
                    }
                    instance.config.relocate(it->dst, src);

#if TEST == 0
                    instance.config.print();
#endif
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

#if TEST == 0
                    instance.config.print();
#endif

                    ans = bb4(instance, UB_cur, src_vec[i].dir, start, T, T_temp);
                    if (ans != -1 && ans != 0)
                    {
                        return min_rel;
                    }
                    else if (ans == -1)
                    {
                        return -1;
                    }

                    instance.config.relocate(it->dst, src);

#if TEST == 0
                    instance.config.print();
#endif
                }
            }
        }
    }
    else
    {
        int block;
        depth++;
        T_temp++;

#if TEST == 0
        printf("depth = %d\n", depth);
#endif

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
        else if (dir == Low)
        {
            src.x = instance.config.pos[instance.config.priority - 1].x;
            src.y = 0;
            // 積み替えるブロック位置を特定
            while (!instance.config.block[src.x][src.y])
            {
                src.y++;
            }
            block = block3;
        }
        else if (dir == Left)
        {
            src.y = instance.config.pos[instance.config.priority - 1].y;
            src.x = 0;
            // 積み替えるブロック位置を特定
            while (!instance.config.block[src.x][src.y])
            {
                src.x++;
            }
            block = block4;
        }
        vector<Dst> vec_dst;
        Point dst = {0, TIER - 1};
        p = instance.config.block[src.x][src.y];
        instance.config.block[src.x][src.y] = 0;
        // 積み替え先スタックを決定
        // 上側からの積み替え
        for (dst.x = 0; dst.x < STACK; dst.x++)
        {
            if (!instance.config.block[dst.x][TIER - 1])
            {
                while (!instance.config.block[dst.x][dst.y - 1] && (dst.y > 0))
                    dst.y--;
                if ((src.x == dst.x) && (src.y == dst.y))
                {
                    dst.y = TIER - 1;
                    continue;
                }
                if (dir == Right && src.y == dst.y && instance.config.pos[instance.config.priority - 1].x < dst.x)
                {
                    dst.y = TIER - 1;
                    continue;
                }
                if (dir == Left && src.y == dst.y && instance.config.pos[instance.config.priority - 1].x > dst.x)
                {
                    dst.y = TIER - 1;
                    continue;
                }
                instance.config.block[dst.x][dst.y] = p;
                instance.config.pos[p - 1] = dst;
                LB_temp = instance.LB4(Upp, Right, Low, Left, dir);
                temp.dst = dst;
                temp.LB = LB_temp;
                temp.p = instance.config.P_UL[dst.x];
                vec_dst.push_back(temp);
                instance.config.block[dst.x][dst.y] = 0;
                dst.y = TIER - 1;
            }
        }
        dst.x = STACK - 1;
        // 右側からの積み替え
        for (dst.y = 0; dst.y < TIER; dst.y++)
        {
            if (!instance.config.block[STACK - 1][dst.y])
            {
                while (!instance.config.block[dst.x - 1][dst.y] && (dst.x > 0))
                    dst.x--;
                if ((src.y == dst.y) && (src.x == dst.x))
                {
                    dst.x = STACK - 1;
                    continue;
                }
                if (dir == Upp && src.x == dst.x && instance.config.pos[instance.config.priority - 1].y < dst.y)
                {
                    dst.x = STACK - 1;
                    continue;
                }
                if (dir == Low && src.x == dst.x && instance.config.pos[instance.config.priority - 1].y > dst.y)
                {
                    dst.x = STACK - 1;
                    continue;
                }
                instance.config.block[dst.x][dst.y] = p;
                instance.config.pos[p - 1] = dst;
                LB_temp = instance.LB4(Upp, Right, Low, Left, dir);
                temp.dst = dst;
                temp.LB = LB_temp;
                temp.p = instance.config.P_LR[dst.y];
                vec_dst.push_back(temp);
                instance.config.block[dst.x][dst.y] = 0;
                dst.x = STACK - 1;
            }
        }
        dst.y = 0;
        // 下側からの積み替え
        for (dst.x = 0; dst.x < STACK; dst.x++)
        {
            if (!instance.config.block[dst.x][0])
            {
                while (!instance.config.block[dst.x][dst.y + 1] && (dst.y < TIER - 1))
                    dst.y++;
                if ((src.x == dst.x) && (src.y == dst.y))
                {
                    dst.y = 0;
                    continue;
                }
                if (dir == Right && src.y == dst.y && instance.config.pos[instance.config.priority - 1].x < dst.x)
                {
                    dst.y = 0;
                    continue;
                }
                if (dir == Left && src.y == dst.y && instance.config.pos[instance.config.priority - 1].x > dst.x)
                {
                    dst.y = 0;
                    continue;
                }
                instance.config.block[dst.x][dst.y] = p;
                instance.config.pos[p - 1] = dst;
                LB_temp = instance.LB4(Upp, Right, Low, Left, dir);
                temp.dst = dst;
                temp.LB = LB_temp;
                temp.p = instance.config.P_UL[dst.x];
                vec_dst.push_back(temp);
                instance.config.block[dst.x][dst.y] = 0;
                dst.y = 0;
            }
        }
        dst.x = 0;
        // 左側からの積み替え
        for (dst.y = 0; dst.y < TIER; dst.y++)
        {
            if (!instance.config.block[0][dst.y])
            {
                while (!instance.config.block[dst.x + 1][dst.y] && (dst.x < STACK - 1))
                    dst.x++;
                if ((src.y == dst.y) && (src.x == dst.x))
                {
                    dst.x = 0;
                    continue;
                }
                if (dir == Upp && src.x == dst.x && instance.config.pos[instance.config.priority - 1].y < dst.y)
                {
                    dst.x = 0;
                    continue;
                }
                if (dir == Low && src.x == dst.x && instance.config.pos[instance.config.priority - 1].y > dst.y)
                {
                    dst.x = 0;
                    continue;
                }
                instance.config.block[dst.x][dst.y] = p;
                instance.config.pos[p - 1] = dst;
                LB_temp = instance.LB4(Upp, Right, Low, Left, dir);
                temp.dst = dst;
                temp.LB = LB_temp;
                temp.p = instance.config.P_LR[dst.y];
                vec_dst.push_back(temp);
                instance.config.block[dst.x][dst.y] = 0;
                dst.x = 0;
            }
        }
        instance.config.block[src.x][src.y] = p;
        instance.config.pos[p - 1] = src;
        if (!vec_dst.empty())
        {
            // 積み替え先を下界値の小さい順に並べる.下界値が等しい場合，最大優先度の降順に並べる
            sort(vec_dst.begin(), vec_dst.end(), asc_desc);
            if (block == 1)
            {
                Instance instance_temp;
                for (auto it = vec_dst.begin(); it != vec_dst.end(); it++)
                {
                    if (it->LB + depth > UB_cur)
                    {
                        break;
                    }
                    instance.config.relocate(src, it->dst);

#if TEST == 0
                    instance.config.print();
#endif

                    instance_temp = instance;
                    ans = bb4(instance_temp, UB_cur, Any, start, T, T_temp);
                    if (ans != -1 && ans != 0)
                    {
                        return min_rel;
                    }
                    else if (ans == -1)
                    {
                        return -1;
                    }
                    instance.config.relocate(it->dst, src);

#if TEST == 0
                    instance.config.print();
#endif
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

#if TEST == 0
                    instance.config.print();
#endif

                    ans = bb4(instance, UB_cur, dir, start, T, T_temp);
                    if (ans != -1 && ans != 0)
                    {
                        return min_rel;
                    }
                    else if (ans == -1)
                    {
                        return -1;
                    }
                    instance.config.relocate(it->dst, src);

#if TEST == 0
                    instance.config.print();
#endif
                }
            }
        }
    }
    depth--;
    T_temp--;
    if (depth == 0)
    {
        UB_cur++;

#if TEST == 0
        printf("UB_cur++\n");
#endif

        ans = bb4(instance, UB_cur, Any, start, T, T_temp);
        if (ans != -1 && ans != 0)
        {
            return min_rel;
        }
        else if (ans == -1)
            return -1;
    }
    else
        return 0;
}