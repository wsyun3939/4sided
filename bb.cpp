#include <bits/stdc++.h>
#include "instance.h"

using namespace std;

// 積み替え元
struct Src
{
    // S^-積み替えかどうか
    int S;
    // 積み替え方向における優先順位
    int p;
    // 積み替え対象となるブロックの優先順位
    int p_block;
    // 積み替え元座標
    Point src;
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

// 積み替え元デックを決めるソート関数
bool src_asc(Src &left, Src &right)
{
    if (left.S != right.S)
    {
        return left.S > right.S;
    }
    else if (left.p != right.p)
    {
        return left.p < right.p;
    }
    else if (left.p_block != right.p_block)
    {
        return left.p_block < right.p_block;
    }
    return 0;
}

// ソートする際の比較関数
bool dst_desc(Dst &left, Dst &right)
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
int bb1(Instance &instance, int UB_cur, clock_t start)
{
    // 節点の深さ
    static int depth = 0;
    // 最適解
    static int min_rel = 0;
    static int ans;
    if (((double)(clock() - start) / CLOCKS_PER_SEC) > 1800)
    {
        depth = 0;
        return -1;
    }
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

#if TEST == 0
        instance.config.print();
#endif

        if (instance.config.priority == instance.nblock + 1)
        {
            min_rel = depth;
            depth = 0;
            return min_rel;
        }
        block1 = instance.config.count(Upp);
    }
    // 積み替え元ブロックを決定
    vector<Src> vec_src;
    for (int i = 0; i < STACK; i++)
    {
        if (instance.config.P_UL[i] == 100)
            continue;
        int y = TIER - 1;
        // 積み替えるブロック位置を特定
        while (!instance.config.block[i][y])
        {
            y--;
        }
        // S^-積み替えとなるかどうか
        int p_block = instance.config.block[i][y];
        instance.config.block[i][y] = 0;
        int LB_temp = instance.LB1();
        int S = instance.config.LB1 - LB_temp;
        instance.config.block[i][y] = p_block;
        Src temp = {S, instance.config.P_UL[i], p_block, {i, y}};
        vec_src.push_back(temp);
    }
    sort(vec_src.begin(), vec_src.end(), src_asc);
    depth++;
    // vec_srcをforループで回す
    for (auto src = vec_src.begin(); src != vec_src.end(); src++)
    {
        vector<Dst> vec_dst;
        Point dst = {0, TIER - 1};
        // 積み替え先スタックを決定
        for (dst.x = 0; dst.x < STACK; dst.x++)
        {
            if ((dst.x != src->src.x) && !instance.config.block[dst.x][TIER - 1])
            {
                int LB_temp = 0;
                while (!instance.config.block[dst.x][dst.y - 1] && (dst.y > 0))
                    dst.y--;
                instance.config.block[src->src.x][src->src.y] = 0;
                instance.config.block[dst.x][dst.y] = src->p_block;
                instance.config.pos[src->p_block - 1] = dst;
                LB_temp = instance.LB1();
                Dst temp = {dst, LB_temp, instance.config.P_UL[dst.x]};
                vec_dst.push_back(temp);
                instance.config.block[dst.x][dst.y] = 0;
                instance.config.block[src->src.x][src->src.y] = src->p_block;
                instance.config.pos[src->p_block - 1] = src->src;
                dst.y = TIER - 1;
            }
        }
        if (!vec_dst.empty())
        {
            // 積み替え先を下界値の小さい順に並べる.下界値が等しい場合，最大優先度の降順に並べる
            sort(vec_dst.begin(), vec_dst.end(), dst_desc);
            if (block1 == 1)
            {
                Instance instance_temp;
                for (auto it = vec_dst.begin(); it != vec_dst.end(); it++)
                {
                    if (it->LB + depth > UB_cur)
                    {
                        break;
                    }
                    instance.config.relocate(src->src, it->dst);

#if TEST == 0
                    instance.config.print();
#endif

                    instance_temp = instance;
                    ans = bb1(instance_temp, UB_cur, start);
                    if (ans != -1 && ans != 0)
                    {
                        return min_rel;
                    }
                    else if (ans == -1)
                    {
                        return -1;
                    }
                    instance.config.relocate(it->dst, src->src);
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
                    instance.config.relocate(src->src, it->dst);

#if TEST == 0
                    instance.config.print();
#endif

                    ans = bb1(instance, UB_cur, start);
                    if (ans != -1 && ans != 0)
                    {
                        return min_rel;
                    }
                    else if (ans == -1)
                    {
                        return -1;
                    }
                    instance.config.relocate(it->dst, src->src);
                }
            }
        }
    }
    depth--;
    if (depth == 0)
    {
        UB_cur++;
        ans = bb1(instance, UB_cur, start);
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
int bb2(Instance &instance, int LB, int UB_cur, clock_t start)
{
    // 節点の深さ
    static int depth = 0;
    // 最適解
    static int min_rel = 0;
    // 積み替え先を決定するための変数
    static int LB_temp;
    static Dst temp;
    static int ans;
    static int k = 0;
    k++;
    // cout << k << endl;
    if (k == 536800)
    {
        cout << "debug" << endl;
        instance.config.print();
    }
    if (!instance.isCorrect())
    {
        cout << "error" << endl;
        instance.config.print();
    }
    if (((double)(clock() - start) / CLOCKS_PER_SEC) > 1800)
    {
        depth = 0;
        return -1;
    }
    if (depth + LB == UB_cur - 1)
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
    while ((block1 == 0) || (block2 == 0))
    {
        src = instance.config.pos[instance.config.priority - 1];
        instance.config.retrieve(src);

#if TEST == 0
        instance.config.print();
#endif
        if (k == 536800)
        {
            instance.config.print();
        }

        if (instance.config.priority == instance.nblock + 1)
        {
            min_rel = depth;
            depth = 0;
            return min_rel;
        }
        block1 = instance.config.count(Upp);
        block2 = instance.config.count(Right);
    }
    // 積み替え元ブロックを決定
    vector<Src> vec_src;
    // 上側からの積み替え
    for (int i = 0; i < STACK; i++)
    {
        if (instance.config.P_UL[i] == 100)
            continue;
        int y = TIER - 1;
        // 積み替えるブロック位置を特定
        while (!instance.config.block[i][y])
        {
            y--;
        }
        // S^-積み替えとなるかどうか
        int p_block = instance.config.block[i][y];
        instance.config.block[i][y] = 0;
        int LB_temp = instance.LB2(Upp, Right, Any);
        int S = instance.config.LB2 - LB_temp;
        instance.config.block[i][y] = p_block;
        Src temp = {S, instance.config.P_UL[i], p_block, {i, y}};
        vec_src.push_back(temp);
    }
    // 右側からの積み替え
    for (int j = 0; j < TIER; j++)
    {
        if (instance.config.P_LR[j] == 100)
            continue;
        int x = STACK - 1;
        // 積み替えるブロック位置を特定
        while (!instance.config.block[x][j])
        {
            x--;
        }
        // S^-積み替えとなるかどうか
        int p_block = instance.config.block[x][j];
        instance.config.block[x][j] = 0;
        int LB_temp = instance.LB2(Upp, Right, Any);
        int S = instance.config.LB2 - LB_temp;
        instance.config.block[x][j] = p_block;
        Src temp = {S, instance.config.P_LR[j], p_block, {x, j}};
        vec_src.push_back(temp);
    }
    sort(vec_src.begin(), vec_src.end(), src_asc);
    depth++;
    for (auto src = vec_src.begin(); src != vec_src.end(); src++)
    {
        // 積み替え先ブロックを決定
        vector<Dst> vec_dst;
        Point dst = {0, TIER - 1};
        // 積み替え先スタックを決定
        // 上側からの積み替え
        for (dst.x = 0; dst.x < STACK; dst.x++)
        {
            if (!instance.config.block[dst.x][TIER - 1])
            {
                while (!instance.config.block[dst.x][dst.y - 1] && (dst.y > 0))
                    dst.y--;
                if ((src->src.x == dst.x) && (src->src.y + 1 == dst.y))
                {
                    dst.y = TIER - 1;
                    continue;
                }
                instance.config.block[src->src.x][src->src.y] = 0;
                instance.config.block[dst.x][dst.y] = src->p_block;
                instance.config.pos[src->p_block - 1] = dst;
                LB_temp = instance.LB2(Upp, Right, Any);
                temp.dst = dst;
                temp.LB = LB_temp;
                temp.p = instance.config.P_UL[dst.x];
                vec_dst.push_back(temp);
                instance.config.block[dst.x][dst.y] = 0;
                instance.config.block[src->src.x][src->src.y] = src->p_block;
                instance.config.pos[src->p_block - 1] = src->src;
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
                if ((src->src.y == dst.y) && (src->src.x + 1 == dst.x))
                {
                    dst.x = STACK - 1;
                    continue;
                }
                instance.config.block[src->src.x][src->src.y] = 0;
                instance.config.block[dst.x][dst.y] = src->p_block;
                instance.config.pos[src->p_block - 1] = dst;
                LB_temp = instance.LB2(Upp, Right, Any);
                temp.dst = dst;
                temp.LB = LB_temp;
                temp.p = instance.config.P_LR[dst.y];
                vec_dst.push_back(temp);
                instance.config.block[dst.x][dst.y] = 0;
                instance.config.block[src->src.x][src->src.y] = src->p_block;
                instance.config.pos[src->p_block - 1] = src->src;
                dst.x = STACK - 1;
            }
        }
        if (vec_dst.empty())
            continue;
        // 積み替え先を下界値の小さい順に並べる.下界値が等しい場合，最大優先度の降順に並べる
        sort(vec_dst.begin(), vec_dst.end(), dst_desc);
        // 積み替え元ブロックを取り出す
        instance.config.block[src->src.x][src->src.y] = 0;
        block1 = instance.config.count(Upp);
        block2 = instance.config.count(Right);
        instance.config.block[src->src.x][src->src.y] = src->p_block;
        if (block1 == 0 || block2 == 0)
        {
            Instance instance_temp;
            for (auto it = vec_dst.begin(); it != vec_dst.end(); it++)
            {
                if (it->LB + depth > UB_cur)
                {
                    break;
                }
                instance.config.relocate(src->src, it->dst);

#if TEST == 0
                cout << "depth:" << depth << endl;
                instance.config.print();
#endif
                if (k == 536800)
                {
                    cout << "debug" << endl;
                    instance.config.print();
                }

                instance_temp = instance;
                ans = bb2(instance_temp, it->LB, UB_cur, start);
                if (ans != -1 && ans != 0)
                {
                    return min_rel;
                }
                else if (ans == -1)
                {
                    return -1;
                }
                instance.config.relocate(it->dst, src->src);

#if TEST == 0
                instance.config.print();
#endif
                if (k == 536800)
                {
                    cout << "debug" << endl;
                    instance.config.print();
                }
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
                instance.config.relocate(src->src, it->dst);

#if TEST == 0
                cout << "depth:" << depth << endl;
                instance.config.print();
#endif
                if (k == 536800)
                {
                    cout << "debug" << endl;
                    instance.config.print();
                }

                ans = bb2(instance, it->LB, UB_cur, start);
                if (ans != -1 && ans != 0)
                {
                    return min_rel;
                }
                else if (ans == -1)
                {
                    return -1;
                }
                if (k == 536800)
                {
                    cout << "debug" << endl;
                    cout << src->p_block << endl;
                    instance.config.print();
                }
                instance.config.relocate(it->dst, src->src);

#if TEST == 0
                instance.config.print();
#endif
                if (k == 536800)
                {
                    cout << "debug" << endl;
                    instance.config.print();
                }
            }
        }
    }
    depth--;
    if (depth == 0)
    {
        UB_cur++;
        ans = bb2(instance, LB, UB_cur, start);
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

// // 二方向(上下)からの積み替え・取り出し
// int bb2a(Instance &instance, int UB_cur, Direction dir, clock_t start)
// {
//     // 節点の深さ
//     static int depth = 0;
//     // 最適解
//     static int min_rel = 0;
//     // 積み替え先を決定するための変数
//     static int LB_temp;
//     static int p;
//     static Dst temp;
//     static int ans;
//     if (((double)(clock() - start) / CLOCKS_PER_SEC) > 1800)
//     {
//         depth = 0;
//         return -1;
//     }
//     if (depth + instance.config.LB2a == UB_cur - 1)
//     {
//         int UB_temp = instance.UB2(Upp, Low);
//         if (instance.config.UB2 > UB_temp + depth)
//         {
//             instance.config.UB2 = UB_temp + depth;
//         }
//     }
//     if (instance.config.UB2 == UB_cur)
//     {
//         depth = 0;
//         return min_rel = instance.config.UB2;
//     }
//     Point src;
//     int block1 = instance.config.count(Upp);
//     int block2 = instance.config.count(Low);
//     if (dir == Any)
//     {
//         while ((block1 == 0) || (block2 == 0))
//         {
//             src = instance.config.pos[instance.config.priority - 1];
//             instance.config.retrieve(src);

// #if TEST == 0
//             instance.config.print();
// #endif

//             if (instance.config.priority == instance.nblock + 1)
//             {
//                 min_rel = depth;
//                 depth = 0;
//                 return min_rel;
//             }
//             block1 = instance.config.count(Upp);
//             block2 = instance.config.count(Low);
//         }
//         Src src_vec[2];
//         src_vec[0].block = block1;
//         src_vec[0].dir = Upp;
//         src_vec[1].block = block2;
//         src_vec[1].dir = Low;
//         sort(src_vec, src_vec + 1, block_asc);
//         depth++;
//         for (int i = 0; i < 2; i++)
//         {
//             // ブロッキングブロックを積み替える場合
//             // 積み替え元
//             switch (src_vec[i].dir)
//             {
//             case Upp:
//                 src.x = instance.config.pos[instance.config.priority - 1].x;
//                 src.y = TIER - 1;
//                 // 積み替えるブロック位置を特定
//                 while (!instance.config.block[src.x][src.y])
//                 {
//                     src.y--;
//                 }
//                 break;
//             case Low:
//                 src.x = instance.config.pos[instance.config.priority - 1].x;
//                 src.y = 0;
//                 // 積み替えるブロック位置を特定
//                 while (!instance.config.block[src.x][src.y])
//                 {
//                     src.y++;
//                 }
//                 break;
//             default:
//                 break;
//             }
//             vector<Dst> vec_dst;
//             Point dst = {0, TIER - 1};
//             // 積み替え先スタックを決定
//             // 上側からの積み替え
//             for (dst.x = 0; dst.x < STACK; dst.x++)
//             {
//                 if (!instance.config.block[dst.x][TIER - 1])
//                 {
//                     while (!instance.config.block[dst.x][dst.y - 1] && (dst.y > 0))
//                         dst.y--;
//                     if ((src.x == dst.x) && (src.y + 1 == dst.y))
//                     {
//                         dst.y = TIER - 1;
//                         continue;
//                     }
//                     p = instance.config.block[src.x][src.y];
//                     instance.config.block[src.x][src.y] = 0;
//                     instance.config.block[dst.x][dst.y] = p;
//                     instance.config.pos[p - 1] = dst;
//                     LB_temp = instance.LB2a(src_vec[i].dir);
//                     Dst temp = {dst, LB_temp, instance.config.P_UL[dst.x]};
//                     vec_dst.push_back(temp);
//                     instance.config.block[dst.x][dst.y] = 0;
//                     instance.config.block[src.x][src.y] = p;
//                     instance.config.pos[p - 1] = src;
//                     dst.y = TIER - 1;
//                 }
//             }
//             dst.y = 0;
//             // 下側からの積み替え
//             for (dst.x = 0; dst.x < STACK; dst.x++)
//             {
//                 if (!instance.config.block[dst.x][0])
//                 {
//                     while (!instance.config.block[dst.x][dst.y + 1] && (dst.y < TIER - 1))
//                         dst.y++;
//                     if ((src.x == dst.x) && (src.y - 1 == dst.y))
//                     {
//                         dst.y = 0;
//                         continue;
//                     }
//                     p = instance.config.block[src.x][src.y];
//                     instance.config.block[src.x][src.y] = 0;
//                     instance.config.block[dst.x][dst.y] = p;
//                     instance.config.pos[p - 1] = dst;
//                     LB_temp = instance.LB2a(src_vec[i].dir);
//                     Dst temp = {dst, LB_temp, instance.config.P_UL[dst.x]};
//                     vec_dst.push_back(temp);
//                     instance.config.block[dst.x][dst.y] = 0;
//                     instance.config.block[src.x][src.y] = p;
//                     instance.config.pos[p - 1] = src;
//                     dst.y = 0;
//                 }
//             }
//             if (vec_dst.empty())
//                 continue;
//             // 積み替え先を下界値の小さい順に並べる.下界値が等しい場合，最大優先度の降順に並べる
//             sort(vec_dst.begin(), vec_dst.end(), asc_desc);
//             if (src_vec[i].block == 1)
//             {
//                 Instance instance_temp;
//                 for (auto it = vec_dst.begin(); it != vec_dst.end(); it++)
//                 {
//                     if (it->LB + depth > UB_cur)
//                     {
//                         break;
//                     }
//                     instance.config.relocate(src, it->dst);

// #if TEST == 0
//                     instance.config.print();
// #endif

//                     instance_temp = instance;
//                     ans = bb2a(instance_temp, UB_cur, Any, start);
//                     if (ans != -1 && ans != 0)
//                     {
//                         return min_rel;
//                     }
//                     else if (ans == -1)
//                     {
//                         return -1;
//                     }
//                     instance.config.relocate(it->dst, src);
//                 }
//             }
//             else
//             {
//                 for (auto it = vec_dst.begin(); it != vec_dst.end(); it++)
//                 {
//                     if (it->LB + depth > UB_cur)
//                     {
//                         break;
//                     }
//                     instance.config.relocate(src, it->dst);

// #if TEST == 0
//                     instance.config.print();
// #endif

//                     ans = bb2a(instance, UB_cur, src_vec[i].dir, start);
//                     if (ans != -1 && ans != 0)
//                     {
//                         return min_rel;
//                     }
//                     else if (ans == -1)
//                     {
//                         return -1;
//                     }
//                     instance.config.relocate(it->dst, src);
//                 }
//             }
//         }
//     }
//     else
//     {
//         int block;
//         depth++;
//         if (dir == Upp)
//         {
//             src.x = instance.config.pos[instance.config.priority - 1].x;
//             src.y = TIER - 1;
//             // 積み替えるブロック位置を特定
//             while (!instance.config.block[src.x][src.y])
//             {
//                 src.y--;
//             }
//             block = block1;
//         }
//         else if (dir == Low)
//         {
//             src.x = instance.config.pos[instance.config.priority - 1].x;
//             src.y = 0;
//             // 積み替えるブロック位置を特定
//             while (!instance.config.block[src.x][src.y])
//             {
//                 src.y++;
//             }
//             block = block1;
//         }
//         vector<Dst> vec_dst;
//         Point dst = {0, TIER - 1};
//         // 積み替え先スタックを決定
//         // 上側からの積み替え
//         for (dst.x = 0; dst.x < STACK; dst.x++)
//         {
//             if (!instance.config.block[dst.x][TIER - 1])
//             {
//                 int LB_temp = 0;
//                 while (!instance.config.block[dst.x][dst.y - 1] && (dst.y > 0))
//                     dst.y--;
//                 if ((src.x == dst.x) && (src.y + 1 == dst.y))
//                 {
//                     dst.y = TIER - 1;
//                     continue;
//                 }
//                 p = instance.config.block[src.x][src.y];
//                 instance.config.block[src.x][src.y] = 0;
//                 instance.config.block[dst.x][dst.y] = p;
//                 instance.config.pos[p - 1] = dst;
//                 LB_temp = instance.LB2a(dir);
//                 Dst temp = {dst, LB_temp, instance.config.P_UL[dst.x]};
//                 vec_dst.push_back(temp);
//                 instance.config.block[dst.x][dst.y] = 0;
//                 instance.config.block[src.x][src.y] = p;
//                 instance.config.pos[p - 1] = src;
//                 dst.y = TIER - 1;
//             }
//         }
//         dst.y = 0;
//         // 下側からの積み替え
//         for (dst.x = 0; dst.x < STACK; dst.x++)
//         {
//             if (!instance.config.block[dst.x][0])
//             {
//                 int LB_temp = 0;
//                 while (!instance.config.block[dst.x][dst.y + 1] && (dst.y < TIER - 1))
//                     dst.y++;
//                 if ((src.x == dst.x) && (src.y - 1 == dst.y))
//                 {
//                     dst.y = 0;
//                     continue;
//                 }
//                 p = instance.config.block[src.x][src.y];
//                 instance.config.block[src.x][src.y] = 0;
//                 instance.config.block[dst.x][dst.y] = p;
//                 instance.config.pos[p - 1] = dst;
//                 LB_temp = instance.LB2a(dir);
//                 Dst temp = {dst, LB_temp, instance.config.P_UL[dst.x]};
//                 vec_dst.push_back(temp);
//                 instance.config.block[dst.x][dst.y] = 0;
//                 instance.config.block[src.x][src.y] = p;
//                 instance.config.pos[p - 1] = src;
//                 dst.y = 0;
//             }
//         }
//         if (!vec_dst.empty())
//         {
//             // 積み替え先を下界値の小さい順に並べる.下界値が等しい場合，最大優先度の降順に並べる
//             sort(vec_dst.begin(), vec_dst.end(), asc_desc);
//             if (block == 1)
//             {
//                 Instance instance_temp;
//                 for (auto it = vec_dst.begin(); it != vec_dst.end(); it++)
//                 {
//                     if (it->LB + depth > UB_cur)
//                     {
//                         break;
//                     }
//                     instance.config.relocate(src, it->dst);

// #if TEST == 0
//                     instance.config.print();
// #endif

//                     instance_temp = instance;
//                     ans = bb2a(instance_temp, UB_cur, Any, start);
//                     if (ans != -1 && ans != 0)
//                     {
//                         return min_rel;
//                     }
//                     else if (ans == -1)
//                     {
//                         return -1;
//                     }
//                     instance.config.relocate(it->dst, src);
//                 }
//             }
//             else
//             {
//                 for (auto it = vec_dst.begin(); it != vec_dst.end(); it++)
//                 {
//                     if (it->LB + depth > UB_cur)
//                     {
//                         break;
//                     }
//                     instance.config.relocate(src, it->dst);

// #if TEST == 0
//                     instance.config.print();
// #endif

//                     ans = bb2a(instance, UB_cur, dir, start);
//                     if (ans != -1 && ans != 0)
//                     {
//                         return min_rel;
//                     }
//                     else if (ans == -1)
//                     {
//                         return -1;
//                     }
//                     instance.config.relocate(it->dst, src);
//                 }
//             }
//         }
//     }
//     depth--;
//     if (depth == 0)
//     {
//         UB_cur++;
//         ans = bb2a(instance, UB_cur, Any, start);
//         if (ans != -1 && ans != 0)
//         {
//             return min_rel;
//         }
//         else if (ans == -1)
//             return -1;
//     }
//     else
//         return 0;
// }

// // 三方向(上右下)からの積み替え・取り出し
// int bb3(Instance &instance, int UB_cur, Direction dir, clock_t start)
// {
//     // 節点の深さ
//     static int depth = 0;
//     // 最適解
//     static int min_rel = 0;
//     // 積み替え先を決定するための変数
//     static int LB_temp;
//     static int p;
//     static Dst temp;
//     static int ans;
//     if (((double)(clock() - start) / CLOCKS_PER_SEC) > 1800)
//     {
//         depth = 0;
//         return -1;
//     }
//     if (depth + instance.config.LB3 == UB_cur - 1)
//     {
//         int UB_temp = instance.UB3(Upp, Right, Low);
//         if (instance.config.UB3 > UB_temp + depth)
//         {
//             instance.config.UB3 = UB_temp + depth;
//         }
//     }
//     if (instance.config.UB3 == UB_cur)
//     {
//         depth = 0;
//         return min_rel = instance.config.UB3;
//     }
//     Point src;
//     int block1 = instance.config.count(Upp);
//     int block2 = instance.config.count(Right);
//     int block3 = instance.config.count(Low);
//     if (dir == Any)
//     {
//         while ((block1 == 0) || (block2 == 0) || (block3 == 0))
//         {
//             src = instance.config.pos[instance.config.priority - 1];
//             instance.config.retrieve(src);

// #if TEST == 0
//             instance.config.print();
// #endif

//             if (instance.config.priority == instance.nblock + 1)
//             {
//                 min_rel = depth;
//                 depth = 0;
//                 return min_rel;
//             }
//             block1 = instance.config.count(Upp);
//             block2 = instance.config.count(Right);
//             block3 = instance.config.count(Low);
//         }
//         Src src_vec[3];
//         src_vec[0].block = block1;
//         src_vec[0].dir = Upp;
//         src_vec[1].block = block2;
//         src_vec[1].dir = Right;
//         src_vec[2].block = block3;
//         src_vec[2].dir = Low;
//         sort(src_vec, src_vec + 2, block_asc);
//         depth++;
//         for (int i = 0; i < 3; i++)
//         {
//             if (depth + src_vec[i].block - 1 > UB_cur)
//             {
//                 break;
//             }
//             // ブロッキングブロックを積み替える場合
//             // 積み替え元
//             switch (src_vec[i].dir)
//             {
//             case Upp:
//                 src.x = instance.config.pos[instance.config.priority - 1].x;
//                 src.y = TIER - 1;
//                 // 積み替えるブロック位置を特定
//                 while (!instance.config.block[src.x][src.y])
//                 {
//                     src.y--;
//                 }
//                 break;
//             case Right:
//                 src.y = instance.config.pos[instance.config.priority - 1].y;
//                 src.x = STACK - 1;
//                 // 積み替えるブロック位置を特定
//                 while (!instance.config.block[src.x][src.y])
//                 {
//                     src.x--;
//                 }
//                 break;
//             case Low:
//                 src.x = instance.config.pos[instance.config.priority - 1].x;
//                 src.y = 0;
//                 // 積み替えるブロック位置を特定
//                 while (!instance.config.block[src.x][src.y])
//                 {
//                     src.y++;
//                 }
//                 break;
//             default:
//                 break;
//             }
//             vector<Dst> vec_dst;
//             Point dst = {0, TIER - 1};
//             // 積み替え先スタックを決定
//             // 上側からの積み替え
//             for (dst.x = 0; dst.x < STACK; dst.x++)
//             {
//                 if (!instance.config.block[dst.x][TIER - 1])
//                 {
//                     while (!instance.config.block[dst.x][dst.y - 1] && (dst.y > 0))
//                         dst.y--;
//                     if ((src.x == dst.x) && (src.y + 1 == dst.y))
//                     {
//                         dst.y = TIER - 1;
//                         continue;
//                     }
//                     p = instance.config.block[src.x][src.y];
//                     instance.config.block[src.x][src.y] = 0;
//                     instance.config.block[dst.x][dst.y] = p;
//                     instance.config.pos[p - 1] = dst;
//                     LB_temp = instance.LB3(Upp, Right, Low, src_vec[i].dir);
//                     temp.dst = dst;
//                     temp.LB = LB_temp;
//                     temp.p = instance.config.P_UL[dst.x];
//                     vec_dst.push_back(temp);
//                     instance.config.block[dst.x][dst.y] = 0;
//                     instance.config.block[src.x][src.y] = p;
//                     instance.config.pos[p - 1] = src;
//                     dst.y = TIER - 1;
//                 }
//             }
//             dst.x = STACK - 1;
//             // 右側からの積み替え
//             for (dst.y = 0; dst.y < TIER; dst.y++)
//             {
//                 if (!instance.config.block[STACK - 1][dst.y])
//                 {
//                     while (!instance.config.block[dst.x - 1][dst.y] && (dst.x > 0))
//                         dst.x--;
//                     if ((src.y == dst.y) && (src.x + 1 == dst.x))
//                     {
//                         dst.x = STACK - 1;
//                         continue;
//                     }
//                     p = instance.config.block[src.x][src.y];
//                     instance.config.block[src.x][src.y] = 0;
//                     instance.config.block[dst.x][dst.y] = p;
//                     instance.config.pos[p - 1] = dst;
//                     LB_temp = instance.LB3(Upp, Right, Low, src_vec[i].dir);
//                     temp.dst = dst;
//                     temp.LB = LB_temp;
//                     temp.p = instance.config.P_LR[dst.y];
//                     vec_dst.push_back(temp);
//                     instance.config.block[dst.x][dst.y] = 0;
//                     instance.config.block[src.x][src.y] = p;
//                     instance.config.pos[p - 1] = src;
//                     dst.x = STACK - 1;
//                 }
//             }
//             dst.y = 0;
//             // 下側からの積み替え
//             for (dst.x = 0; dst.x < STACK; dst.x++)
//             {
//                 if (!instance.config.block[dst.x][0])
//                 {
//                     while (!instance.config.block[dst.x][dst.y + 1] && (dst.y < TIER - 1))
//                         dst.y++;
//                     if ((src.x == dst.x) && (src.y - 1 == dst.y))
//                     {
//                         dst.y = 0;
//                         continue;
//                     }
//                     p = instance.config.block[src.x][src.y];
//                     instance.config.block[src.x][src.y] = 0;
//                     instance.config.block[dst.x][dst.y] = p;
//                     instance.config.pos[p - 1] = dst;
//                     LB_temp = instance.LB3(Upp, Right, Low, src_vec[i].dir);
//                     temp.dst = dst;
//                     temp.LB = LB_temp;
//                     temp.p = instance.config.P_UL[dst.x];
//                     vec_dst.push_back(temp);
//                     instance.config.block[dst.x][dst.y] = 0;
//                     instance.config.block[src.x][src.y] = p;
//                     instance.config.pos[p - 1] = src;
//                     dst.y = 0;
//                 }
//             }
//             if (vec_dst.empty())
//                 continue;
//             // 積み替え先を下界値の小さい順に並べる.下界値が等しい場合，最大優先度の降順に並べる
//             sort(vec_dst.begin(), vec_dst.end(), asc_desc);
//             if (src_vec[i].block == 1)
//             {
//                 Instance instance_temp;
//                 for (auto it = vec_dst.begin(); it != vec_dst.end(); it++)
//                 {
//                     if (it->LB + depth > UB_cur)
//                     {
//                         break;
//                     }
//                     instance.config.relocate(src, it->dst);

// #if TEST == 0
//                     instance.config.print();
// #endif

//                     instance_temp = instance;
//                     ans = bb3(instance_temp, UB_cur, Any, start);
//                     if (ans != -1 && ans != 0)
//                     {
//                         return min_rel;
//                     }
//                     else if (ans == -1)
//                     {
//                         return -1;
//                     }
//                     instance.config.relocate(it->dst, src);

// #if TEST == 0
//                     instance.config.print();
// #endif
//                 }
//             }
//             else
//             {
//                 for (auto it = vec_dst.begin(); it != vec_dst.end(); it++)
//                 {
//                     if (it->LB + depth > UB_cur)
//                     {
//                         break;
//                     }
//                     instance.config.relocate(src, it->dst);

// #if TEST == 0
//                     instance.config.print();
// #endif

//                     ans = bb3(instance, UB_cur, src_vec[i].dir, start);
//                     if (ans != -1 && ans != 0)
//                     {
//                         return min_rel;
//                     }
//                     else if (ans == -1)
//                     {
//                         return -1;
//                     }
//                     instance.config.relocate(it->dst, src);

// #if TEST == 0
//                     instance.config.print();
// #endif
//                 }
//             }
//         }
//     }
//     else
//     {
//         int block;
//         depth++;
//         if (dir == Upp)
//         {
//             src.x = instance.config.pos[instance.config.priority - 1].x;
//             src.y = TIER - 1;
//             // 積み替えるブロック位置を特定
//             while (!instance.config.block[src.x][src.y])
//             {
//                 src.y--;
//             }
//             block = block1;
//         }
//         else if (dir == Right)
//         {
//             src.y = instance.config.pos[instance.config.priority - 1].y;
//             src.x = STACK - 1;
//             // 積み替えるブロック位置を特定
//             while (!instance.config.block[src.x][src.y])
//             {
//                 src.x--;
//             }
//             block = block2;
//         }
//         else if (dir == Low)
//         {
//             src.x = instance.config.pos[instance.config.priority - 1].x;
//             src.y = 0;
//             // 積み替えるブロック位置を特定
//             while (!instance.config.block[src.x][src.y])
//             {
//                 src.y++;
//             }
//             block = block3;
//         }
//         vector<Dst> vec_dst;
//         Point dst = {0, TIER - 1};
//         // 積み替え先スタックを決定
//         // 上側からの積み替え
//         for (dst.x = 0; dst.x < STACK; dst.x++)
//         {
//             if (!instance.config.block[dst.x][TIER - 1])
//             {
//                 while (!instance.config.block[dst.x][dst.y - 1] && (dst.y > 0))
//                     dst.y--;
//                 if ((src.x == dst.x) && (src.y + 1 == dst.y))
//                 {
//                     dst.y = TIER - 1;
//                     continue;
//                 }
//                 p = instance.config.block[src.x][src.y];
//                 instance.config.block[src.x][src.y] = 0;
//                 instance.config.block[dst.x][dst.y] = p;
//                 instance.config.pos[p - 1] = dst;
//                 LB_temp = instance.LB3(Upp, Right, Low, dir);
//                 temp.dst = dst;
//                 temp.LB = LB_temp;
//                 temp.p = instance.config.P_UL[dst.x];
//                 vec_dst.push_back(temp);
//                 instance.config.block[dst.x][dst.y] = 0;
//                 instance.config.block[src.x][src.y] = p;
//                 instance.config.pos[p - 1] = src;
//                 dst.y = TIER - 1;
//             }
//         }
//         dst.x = STACK - 1;
//         // 右側からの積み替え
//         for (dst.y = 0; dst.y < TIER; dst.y++)
//         {
//             if (!instance.config.block[STACK - 1][dst.y])
//             {
//                 while (!instance.config.block[dst.x - 1][dst.y] && (dst.x > 0))
//                     dst.x--;
//                 if ((src.y == dst.y) && (src.x + 1 == dst.x))
//                 {
//                     dst.x = STACK - 1;
//                     continue;
//                 }
//                 p = instance.config.block[src.x][src.y];
//                 instance.config.block[src.x][src.y] = 0;
//                 instance.config.block[dst.x][dst.y] = p;
//                 instance.config.pos[p - 1] = dst;
//                 LB_temp = instance.LB3(Upp, Right, Low, dir);
//                 temp.dst = dst;
//                 temp.LB = LB_temp;
//                 temp.p = instance.config.P_LR[dst.y];
//                 vec_dst.push_back(temp);
//                 instance.config.block[dst.x][dst.y] = 0;
//                 instance.config.block[src.x][src.y] = p;
//                 instance.config.pos[p - 1] = src;
//                 dst.x = STACK - 1;
//             }
//         }
//         dst.y = 0;
//         // 下側からの積み替え
//         for (dst.x = 0; dst.x < STACK; dst.x++)
//         {
//             if (!instance.config.block[dst.x][0])
//             {
//                 while (!instance.config.block[dst.x][dst.y + 1] && (dst.y < TIER - 1))
//                     dst.y++;
//                 if ((src.x == dst.x) && (src.y - 1 == dst.y))
//                 {
//                     dst.y = 0;
//                     continue;
//                 }
//                 p = instance.config.block[src.x][src.y];
//                 instance.config.block[src.x][src.y] = 0;
//                 instance.config.block[dst.x][dst.y] = p;
//                 instance.config.pos[p - 1] = dst;
//                 LB_temp = instance.LB3(Upp, Right, Low, dir);
//                 temp.dst = dst;
//                 temp.LB = LB_temp;
//                 temp.p = instance.config.P_UL[dst.x];
//                 vec_dst.push_back(temp);
//                 instance.config.block[dst.x][dst.y] = 0;
//                 instance.config.block[src.x][src.y] = p;
//                 instance.config.pos[p - 1] = src;
//                 dst.y = 0;
//             }
//         }
//         if (!vec_dst.empty())
//         {
//             // 積み替え先を下界値の小さい順に並べる.下界値が等しい場合，最大優先度の降順に並べる
//             sort(vec_dst.begin(), vec_dst.end(), asc_desc);
//             if (block == 1)
//             {
//                 Instance instance_temp;
//                 for (auto it = vec_dst.begin(); it != vec_dst.end(); it++)
//                 {
//                     if (it->LB + depth > UB_cur)
//                     {
//                         break;
//                     }
//                     instance.config.relocate(src, it->dst);

// #if TEST == 0
//                     instance.config.print();
// #endif

//                     instance_temp = instance;
//                     ans = bb3(instance_temp, UB_cur, Any, start);
//                     if (ans != -1 && ans != 0)
//                     {
//                         return min_rel;
//                     }
//                     else if (ans == -1)
//                     {
//                         return -1;
//                     }
//                     instance.config.relocate(it->dst, src);

// #if TEST == 0
//                     instance.config.print();
// #endif
//                 }
//             }
//             else
//             {
//                 for (auto it = vec_dst.begin(); it != vec_dst.end(); it++)
//                 {
//                     if (it->LB + depth > UB_cur)
//                     {
//                         break;
//                     }
//                     instance.config.relocate(src, it->dst);

// #if TEST == 0
//                     instance.config.print();
// #endif

//                     ans = bb3(instance, UB_cur, dir, start);
//                     if (ans != -1 && ans != 0)
//                     {
//                         return min_rel;
//                     }
//                     else if (ans == -1)
//                     {
//                         return -1;
//                     }
//                     instance.config.relocate(it->dst, src);

// #if TEST == 0
//                     instance.config.print();
// #endif
//                 }
//             }
//         }
//     }
//     depth--;
//     if (depth == 0)
//     {
//         UB_cur++;
//         ans = bb3(instance, UB_cur, Any, start);
//         if (ans != -1 && ans != 0)
//         {
//             return min_rel;
//         }
//         else if (ans == -1)
//             return -1;
//     }
//     else
//         return 0;
// }

// // 四方向(上右下左)からの積み替え・取り出し
// int bb4(Instance &instance, int UB_cur, Direction dir, clock_t start)
// {
//     // 節点の深さ
//     static int depth = 0;
//     // 最適解
//     static int min_rel = 0;
//     // 積み替え先を決定するための変数
//     static int LB_temp;
//     static int p;
//     static Dst temp;
//     static int ans;
//     if (((double)(clock() - start) / CLOCKS_PER_SEC) > 1800)
//     {
//         depth = 0;
//         return -1;
//     }
//     if (depth + instance.config.LB4 == UB_cur - 1)
//     {
//         int UB_temp = instance.UB4(Upp, Right, Low, Left);
//         if (instance.config.UB4 > UB_temp + depth)
//         {
//             instance.config.UB4 = UB_temp + depth;
//         }
//     }
//     if (instance.config.UB4 == UB_cur)
//     {
//         depth = 0;
//         return min_rel = instance.config.UB4;
//     }
//     Point src;
//     int block1 = instance.config.count(Upp);
//     int block2 = instance.config.count(Right);
//     int block3 = instance.config.count(Low);
//     int block4 = instance.config.count(Left);
//     if (dir == Any)
//     {
//         while ((block1 == 0) || (block2 == 0) || (block3 == 0) || (block4 == 0))
//         {
//             src = instance.config.pos[instance.config.priority - 1];
//             instance.config.retrieve(src);

// #if TEST == 0
//             instance.config.print();
// #endif

//             if (instance.config.priority == instance.nblock + 1)
//             {
//                 min_rel = depth;
//                 depth = 0;
//                 return min_rel;
//             }
//             block1 = instance.config.count(Upp);
//             block2 = instance.config.count(Right);
//             block3 = instance.config.count(Low);
//             block4 = instance.config.count(Left);
//         }
//         Src src_vec[4];
//         src_vec[0].block = block1;
//         src_vec[0].dir = Upp;
//         src_vec[1].block = block2;
//         src_vec[1].dir = Right;
//         src_vec[2].block = block3;
//         src_vec[2].dir = Low;
//         src_vec[3].block = block4;
//         src_vec[3].dir = Left;
//         sort(src_vec, src_vec + 3, block_asc);
//         depth++;
//         for (int i = 0; i < 4; i++)
//         {
//             if (depth + src_vec[i].block - 1 > UB_cur)
//             {
//                 break;
//             }
//             // ブロッキングブロックを積み替える場合
//             // 積み替え元
//             switch (src_vec[i].dir)
//             {
//             case Upp:
//                 src.x = instance.config.pos[instance.config.priority - 1].x;
//                 src.y = TIER - 1;
//                 // 積み替えるブロック位置を特定
//                 while (!instance.config.block[src.x][src.y])
//                 {
//                     src.y--;
//                 }
//                 break;
//             case Right:
//                 src.y = instance.config.pos[instance.config.priority - 1].y;
//                 src.x = STACK - 1;
//                 // 積み替えるブロック位置を特定
//                 while (!instance.config.block[src.x][src.y])
//                 {
//                     src.x--;
//                 }
//                 break;
//             case Low:
//                 src.x = instance.config.pos[instance.config.priority - 1].x;
//                 src.y = 0;
//                 // 積み替えるブロック位置を特定
//                 while (!instance.config.block[src.x][src.y])
//                 {
//                     src.y++;
//                 }
//                 break;
//             case Left:
//                 src.y = instance.config.pos[instance.config.priority - 1].y;
//                 src.x = 0;
//                 // 積み替えるブロック位置を特定
//                 while (!instance.config.block[src.x][src.y])
//                 {
//                     src.x++;
//                 }
//                 break;
//             default:
//                 break;
//             }
//             vector<Dst> vec_dst;
//             Point dst = {0, TIER - 1};
//             // 積み替え先スタックを決定
//             // 上側からの積み替え
//             for (dst.x = 0; dst.x < STACK; dst.x++)
//             {
//                 if (!instance.config.block[dst.x][TIER - 1])
//                 {
//                     while (!instance.config.block[dst.x][dst.y - 1] && (dst.y > 0))
//                         dst.y--;
//                     if ((src.x == dst.x) && (src.y + 1 == dst.y))
//                     {
//                         dst.y = TIER - 1;
//                         continue;
//                     }
//                     p = instance.config.block[src.x][src.y];
//                     instance.config.block[src.x][src.y] = 0;
//                     instance.config.block[dst.x][dst.y] = p;
//                     instance.config.pos[p - 1] = dst;
//                     LB_temp = instance.LB4(Upp, Right, Low, Left, src_vec[i].dir);
//                     temp.dst = dst;
//                     temp.LB = LB_temp;
//                     temp.p = instance.config.P_UL[dst.x];
//                     vec_dst.push_back(temp);
//                     instance.config.block[dst.x][dst.y] = 0;
//                     instance.config.block[src.x][src.y] = p;
//                     instance.config.pos[p - 1] = src;
//                     dst.y = TIER - 1;
//                 }
//             }
//             dst.x = STACK - 1;
//             // 右側からの積み替え
//             for (dst.y = 0; dst.y < TIER; dst.y++)
//             {
//                 if (!instance.config.block[STACK - 1][dst.y])
//                 {
//                     while (!instance.config.block[dst.x - 1][dst.y] && (dst.x > 0))
//                         dst.x--;
//                     if ((src.y == dst.y) && (src.x + 1 == dst.x))
//                     {
//                         dst.x = STACK - 1;
//                         continue;
//                     }
//                     p = instance.config.block[src.x][src.y];
//                     instance.config.block[src.x][src.y] = 0;
//                     instance.config.block[dst.x][dst.y] = p;
//                     instance.config.pos[p - 1] = dst;
//                     LB_temp = instance.LB4(Upp, Right, Low, Left, src_vec[i].dir);
//                     temp.dst = dst;
//                     temp.LB = LB_temp;
//                     temp.p = instance.config.P_LR[dst.y];
//                     vec_dst.push_back(temp);
//                     instance.config.block[dst.x][dst.y] = 0;
//                     instance.config.block[src.x][src.y] = p;
//                     instance.config.pos[p - 1] = src;
//                     dst.x = STACK - 1;
//                 }
//             }
//             dst.y = 0;
//             // 下側からの積み替え
//             for (dst.x = 0; dst.x < STACK; dst.x++)
//             {
//                 if (!instance.config.block[dst.x][0])
//                 {
//                     while (!instance.config.block[dst.x][dst.y + 1] && (dst.y < TIER - 1))
//                         dst.y++;
//                     if ((src.x == dst.x) && (src.y - 1 == dst.y))
//                     {
//                         dst.y = 0;
//                         continue;
//                     }
//                     p = instance.config.block[src.x][src.y];
//                     instance.config.block[src.x][src.y] = 0;
//                     instance.config.block[dst.x][dst.y] = p;
//                     instance.config.pos[p - 1] = dst;
//                     LB_temp = instance.LB4(Upp, Right, Low, Left, src_vec[i].dir);
//                     temp.dst = dst;
//                     temp.LB = LB_temp;
//                     temp.p = instance.config.P_UL[dst.x];
//                     vec_dst.push_back(temp);
//                     instance.config.block[dst.x][dst.y] = 0;
//                     instance.config.block[src.x][src.y] = p;
//                     instance.config.pos[p - 1] = src;
//                     dst.y = 0;
//                 }
//             }
//             dst.x = 0;
//             // 左側からの積み替え
//             for (dst.y = 0; dst.y < TIER; dst.y++)
//             {
//                 if (!instance.config.block[0][dst.y])
//                 {
//                     while (!instance.config.block[dst.x + 1][dst.y] && (dst.x < STACK - 1))
//                         dst.x++;
//                     if ((src.y == dst.y) && (src.x - 1 == dst.x))
//                     {
//                         dst.x = 0;
//                         continue;
//                     }
//                     p = instance.config.block[src.x][src.y];
//                     instance.config.block[src.x][src.y] = 0;
//                     instance.config.block[dst.x][dst.y] = p;
//                     instance.config.pos[p - 1] = dst;
//                     LB_temp = instance.LB4(Upp, Right, Low, Left, src_vec[i].dir);
//                     temp.dst = dst;
//                     temp.LB = LB_temp;
//                     temp.p = instance.config.P_LR[dst.y];
//                     vec_dst.push_back(temp);
//                     instance.config.block[dst.x][dst.y] = 0;
//                     instance.config.block[src.x][src.y] = p;
//                     instance.config.pos[p - 1] = src;
//                     dst.x = 0;
//                 }
//             }
//             if (vec_dst.empty())
//             {
//                 continue;
//             }
//             // 積み替え先を下界値の小さい順に並べる.下界値が等しい場合，最大優先度の降順に並べる
//             sort(vec_dst.begin(), vec_dst.end(), asc_desc);
//             if (src_vec[i].block == 1)
//             {
//                 Instance instance_temp;
//                 for (auto it = vec_dst.begin(); it != vec_dst.end(); it++)
//                 {
//                     if (it->LB + depth > UB_cur)
//                     {
//                         break;
//                     }
//                     instance.config.relocate(src, it->dst);

// #if TEST == 0
//                     instance.config.print();
// #endif

//                     instance_temp = instance;
//                     ans = bb4(instance_temp, UB_cur, Any, start);
//                     if (ans != -1 && ans != 0)
//                     {
//                         return min_rel;
//                     }
//                     else if (ans == -1)
//                     {
//                         return -1;
//                     }
//                     instance.config.relocate(it->dst, src);

// #if TEST == 0
//                     instance.config.print();
// #endif
//                 }
//             }
//             else
//             {
//                 for (auto it = vec_dst.begin(); it != vec_dst.end(); it++)
//                 {
//                     if (it->LB + depth > UB_cur)
//                     {
//                         break;
//                     }
//                     instance.config.relocate(src, it->dst);

// #if TEST == 0
//                     instance.config.print();
// #endif

//                     ans = bb4(instance, UB_cur, src_vec[i].dir, start);
//                     if (ans != -1 && ans != 0)
//                     {
//                         return min_rel;
//                     }
//                     else if (ans == -1)
//                     {
//                         return -1;
//                     }

//                     instance.config.relocate(it->dst, src);

// #if TEST == 0
//                     instance.config.print();
// #endif
//                 }
//             }
//         }
//     }
//     else
//     {
//         int block;
//         depth++;
//         if (dir == Upp)
//         {
//             src.x = instance.config.pos[instance.config.priority - 1].x;
//             src.y = TIER - 1;
//             // 積み替えるブロック位置を特定
//             while (!instance.config.block[src.x][src.y])
//             {
//                 src.y--;
//             }
//             block = block1;
//         }
//         else if (dir == Right)
//         {
//             src.y = instance.config.pos[instance.config.priority - 1].y;
//             src.x = STACK - 1;
//             // 積み替えるブロック位置を特定
//             while (!instance.config.block[src.x][src.y])
//             {
//                 src.x--;
//             }
//             block = block2;
//         }
//         else if (dir == Low)
//         {
//             src.x = instance.config.pos[instance.config.priority - 1].x;
//             src.y = 0;
//             // 積み替えるブロック位置を特定
//             while (!instance.config.block[src.x][src.y])
//             {
//                 src.y++;
//             }
//             block = block3;
//         }
//         else if (dir == Left)
//         {
//             src.y = instance.config.pos[instance.config.priority - 1].y;
//             src.x = 0;
//             // 積み替えるブロック位置を特定
//             while (!instance.config.block[src.x][src.y])
//             {
//                 src.x++;
//             }
//             block = block4;
//         }
//         vector<Dst> vec_dst;
//         Point dst = {0, TIER - 1};
//         // 積み替え先スタックを決定
//         // 上側からの積み替え
//         for (dst.x = 0; dst.x < STACK; dst.x++)
//         {
//             if (!instance.config.block[dst.x][TIER - 1])
//             {
//                 while (!instance.config.block[dst.x][dst.y - 1] && (dst.y > 0))
//                     dst.y--;
//                 if ((src.x == dst.x) && (src.y + 1 == dst.y))
//                 {
//                     dst.y = TIER - 1;
//                     continue;
//                 }
//                 p = instance.config.block[src.x][src.y];
//                 instance.config.block[src.x][src.y] = 0;
//                 instance.config.block[dst.x][dst.y] = p;
//                 instance.config.pos[p - 1] = dst;
//                 LB_temp = instance.LB3(Upp, Right, Low, dir);
//                 temp.dst = dst;
//                 temp.LB = LB_temp;
//                 temp.p = instance.config.P_UL[dst.x];
//                 vec_dst.push_back(temp);
//                 instance.config.block[dst.x][dst.y] = 0;
//                 instance.config.block[src.x][src.y] = p;
//                 instance.config.pos[p - 1] = src;
//                 dst.y = TIER - 1;
//             }
//         }
//         dst.x = STACK - 1;
//         // 右側からの積み替え
//         for (dst.y = 0; dst.y < TIER; dst.y++)
//         {
//             if (!instance.config.block[STACK - 1][dst.y])
//             {
//                 while (!instance.config.block[dst.x - 1][dst.y] && (dst.x > 0))
//                     dst.x--;
//                 if ((src.y == dst.y) && (src.x + 1 == dst.x))
//                 {
//                     dst.x = STACK - 1;
//                     continue;
//                 }
//                 p = instance.config.block[src.x][src.y];
//                 instance.config.block[src.x][src.y] = 0;
//                 instance.config.block[dst.x][dst.y] = p;
//                 instance.config.pos[p - 1] = dst;
//                 LB_temp = instance.LB3(Upp, Right, Low, dir);
//                 temp.dst = dst;
//                 temp.LB = LB_temp;
//                 temp.p = instance.config.P_LR[dst.y];
//                 vec_dst.push_back(temp);
//                 instance.config.block[dst.x][dst.y] = 0;
//                 instance.config.block[src.x][src.y] = p;
//                 instance.config.pos[p - 1] = src;
//                 dst.x = STACK - 1;
//             }
//         }
//         dst.y = 0;
//         // 下側からの積み替え
//         for (dst.x = 0; dst.x < STACK; dst.x++)
//         {
//             if (!instance.config.block[dst.x][0])
//             {
//                 while (!instance.config.block[dst.x][dst.y + 1] && (dst.y < TIER - 1))
//                     dst.y++;
//                 if ((src.x == dst.x) && (src.y - 1 == dst.y))
//                 {
//                     dst.y = 0;
//                     continue;
//                 }
//                 p = instance.config.block[src.x][src.y];
//                 instance.config.block[src.x][src.y] = 0;
//                 instance.config.block[dst.x][dst.y] = p;
//                 instance.config.pos[p - 1] = dst;
//                 LB_temp = instance.LB3(Upp, Right, Low, dir);
//                 temp.dst = dst;
//                 temp.LB = LB_temp;
//                 temp.p = instance.config.P_UL[dst.x];
//                 vec_dst.push_back(temp);
//                 instance.config.block[dst.x][dst.y] = 0;
//                 instance.config.block[src.x][src.y] = p;
//                 instance.config.pos[p - 1] = src;
//                 dst.y = 0;
//             }
//         }
//         dst.x = 0;
//         // 左側からの積み替え
//         for (dst.y = 0; dst.y < TIER; dst.y++)
//         {
//             if (!instance.config.block[0][dst.y])
//             {
//                 while (!instance.config.block[dst.x + 1][dst.y] && (dst.x < STACK - 1))
//                     dst.x++;
//                 if ((src.y == dst.y) && (src.x - 1 == dst.x))
//                 {
//                     dst.x = 0;
//                     continue;
//                 }
//                 p = instance.config.block[src.x][src.y];
//                 instance.config.block[src.x][src.y] = 0;
//                 instance.config.block[dst.x][dst.y] = p;
//                 instance.config.pos[p - 1] = dst;
//                 LB_temp = instance.LB4(Upp, Right, Low, Left, dir);
//                 temp.dst = dst;
//                 temp.LB = LB_temp;
//                 temp.p = instance.config.P_LR[dst.y];
//                 vec_dst.push_back(temp);
//                 instance.config.block[dst.x][dst.y] = 0;
//                 instance.config.block[src.x][src.y] = p;
//                 instance.config.pos[p - 1] = src;
//                 dst.x = 0;
//             }
//         }
//         if (!vec_dst.empty())
//         {
//             // 積み替え先を下界値の小さい順に並べる.下界値が等しい場合，最大優先度の降順に並べる
//             sort(vec_dst.begin(), vec_dst.end(), asc_desc);
//             if (block == 1)
//             {
//                 Instance instance_temp;
//                 for (auto it = vec_dst.begin(); it != vec_dst.end(); it++)
//                 {
//                     if (it->LB + depth > UB_cur)
//                     {
//                         break;
//                     }
//                     instance.config.relocate(src, it->dst);

// #if TEST == 0
//                     instance.config.print();
// #endif

//                     instance_temp = instance;
//                     ans = bb4(instance_temp, UB_cur, Any, start);
//                     if (ans != -1 && ans != 0)
//                     {
//                         return min_rel;
//                     }
//                     else if (ans == -1)
//                     {
//                         return -1;
//                     }
//                     instance.config.relocate(it->dst, src);

// #if TEST == 0
//                     instance.config.print();
// #endif
//                 }
//             }
//             else
//             {
//                 for (auto it = vec_dst.begin(); it != vec_dst.end(); it++)
//                 {
//                     if (it->LB + depth > UB_cur)
//                     {
//                         break;
//                     }
//                     instance.config.relocate(src, it->dst);

// #if TEST == 0
//                     instance.config.print();
// #endif

//                     ans = bb4(instance, UB_cur, dir, start);
//                     if (ans != -1 && ans != 0)
//                     {
//                         return min_rel;
//                     }
//                     else if (ans == -1)
//                     {
//                         return -1;
//                     }
//                     instance.config.relocate(it->dst, src);

// #if TEST == 0
//                     instance.config.print();
// #endif
//                 }
//             }
//         }
//     }
//     depth--;
//     if (depth == 0)
//     {
//         UB_cur++;
//         ans = bb4(instance, UB_cur, Any, start);
//         if (ans != -1 && ans != 0)
//         {
//             return min_rel;
//         }
//         else if (ans == -1)
//             return -1;
//     }
//     else
//         return 0;
// }