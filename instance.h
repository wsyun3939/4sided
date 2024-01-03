#ifndef ___instance
#define ___instance
#include <bits/stdc++.h>
#include <fstream>
#include <string>
#include <vector>
#include "config.h"

using namespace std;

class Instance
{
public:
    // ブロック配置
    Config config;
    // 総ブロック数
    int nblock;

    // コンストラクタ
    Instance()
    {
        nblock = 0;
    }

    // ファイル読み込み
    void readFile2D(const std::string &path)
    {
        std::ifstream file(path);
        if (!file.is_open())
        {
            std::cerr << "Failed to open file: " << path << std::endl;
            return;
        }

        std::string line;
        int i = 0;

        while (std::getline(file, line))
        {
            std::istringstream iss(line);
            int value;
            for (int j = 0; j < TIER; j++)
            {
                iss >> value;
                if (value != 0)
                {
                    nblock++;
                    if (config.P_UL[i] > value)
                        config.P_UL[i] = value;
                    config.pos[value - 1].x = i;
                    config.pos[value - 1].y = j;
                }
                config.block[i][j] = value;
            }
            i++;
        }
        file.close();

        // 横方向の最大優先度を設定
        for (int j = 0; j < TIER; j++)
        {
            for (int i = 0; i < STACK; i++)
            {
                if (config.block[i][j] == 0)
                    continue;
                if (config.P_LR[j] > config.block[i][j])
                    config.P_LR[j] = config.block[i][j];
            }
        }
        // 各ブロックスペースに対して，最大優先度を設定
        for (int i = 0; i < STACK; i++)
        {
            for (int j = 0; j < TIER; j++)
            {
                config.P[i][j] = config.P_UL[i] + config.P_LR[j];
            }
        }
    }

    // LB1(1方向)を計算
    int LB1()
    {
        int LB1 = 0;
        Config config_temp = config;
        for (int n = config_temp.priority; n <= nblock; n++)
        {
            if (config_temp.block[config_temp.pos[n - 1].x][config_temp.pos[n - 1].y])
            {
                config_temp.block[config_temp.pos[n - 1].x][config_temp.pos[n - 1].y] = 0;
                for (int j = config_temp.pos[n - 1].y + 1; j < TIER; j++)
                {
                    if (config_temp.block[config_temp.pos[n - 1].x][j])
                    {
                        LB1++;
                        config_temp.block[config_temp.pos[n - 1].x][j] = 0;
                    }
                    else
                        break;
                }
            }
        }
        return LB1;
    }

    int LB2(Direction dir1, Direction dir2, Direction rel)
    {
        int LB2 = 0;
        Config config_temp = config;
        if (rel != Any)
        {
            int block = config_temp.count(rel);
            LB2 += block;
            config_temp.delete_block(rel);
            config_temp.priority++;
        }
        while (config_temp.priority != nblock)
        {
            while (config_temp.block[config_temp.pos[config_temp.priority - 1].x][config_temp.pos[config_temp.priority - 1].y] == 0)
            {
                if (config_temp.priority == nblock - 1)
                    return LB2;
                config_temp.priority++;
            }
            int block1 = config_temp.count(dir1);
            int block2 = config_temp.count(dir2);
            while ((block1 == 0) || (block2 == 0))
            {
                config_temp.block[config_temp.pos[config_temp.priority - 1].x][config_temp.pos[config_temp.priority - 1].y] = 0;
                if (config_temp.priority == nblock)
                    return LB2;
                config_temp.priority++;
                block1 = config_temp.count(dir1);
                block2 = config_temp.count(dir2);
            }
            config_temp.delete_block(dir1);
            config_temp.delete_block(dir2);
            config_temp.priority++;
            LB2 += (block1 < block2) ? block1 : block2;
        }
        return LB2;
    }

    // 上下からの積み替えを考慮した場合
    int LB2a(Direction rel)
    {
        int LB2 = 0;
        Config config_temp = config;
        if (rel != Any)
        {
            int block = config_temp.count(rel);
            LB2 += block;
            config_temp.delete_block(rel);
            config_temp.priority++;
        }
        while (config_temp.priority != nblock)
        {
            while (config_temp.block[config_temp.pos[config_temp.priority - 1].x][config_temp.pos[config_temp.priority - 1].y] == 0)
            {
                if (config_temp.priority == nblock - 1)
                    return LB2;
                config_temp.priority++;
            }
            int block1 = config_temp.count(Upp);
            int block2 = config_temp.count(Low);
            while ((block1 == 0) || (block2 == 0))
            {
                config_temp.block[config_temp.pos[config_temp.priority - 1].x][config_temp.pos[config_temp.priority - 1].y] = 0;
                if (config_temp.priority == nblock)
                    return LB2;
                config_temp.priority++;
                block1 = config_temp.count(Upp);
                block2 = config_temp.count(Low);
            }
            if (block1 < block2)
            {
                config_temp.delete_block(Upp);
                LB2 += block1;
                config_temp.priority++;
            }
            else
            {
                config_temp.delete_block(Low);
                LB2 += block2;
                config_temp.priority++;
            }
        }
        return LB2;
    }

    int LB3(Direction dir1, Direction dir2, Direction dir3, Direction rel)
    {
        Config config_temp = config;
        int LB3 = 0;
        if (rel != Any)
        {
            int block = config_temp.count(rel);
            LB3 += block;
            config_temp.delete_block(rel);
            config_temp.priority++;
        }
        while (config_temp.priority != nblock)
        {
            while (config_temp.block[config_temp.pos[config_temp.priority - 1].x][config_temp.pos[config_temp.priority - 1].y] == 0)
            {
                if (config_temp.priority == nblock - 1)
                    return LB3;
                config_temp.priority++;
            }
            int block1 = config_temp.count(dir1);
            int block2 = config_temp.count(dir2);
            int block3 = config_temp.count(dir3);
            while ((block1 == 0) || (block2 == 0) || (block3 == 0))
            {
                config_temp.block[config_temp.pos[config_temp.priority - 1].x][config_temp.pos[config_temp.priority - 1].y] = 0;
                if (config_temp.priority == nblock)
                    return LB3;
                config_temp.priority++;
                block1 = config_temp.count(dir1);
                block2 = config_temp.count(dir2);
                block3 = config_temp.count(dir3);
            }
            config_temp.delete_block(dir1);
            config_temp.delete_block(dir2);
            config_temp.delete_block(dir3);
            config_temp.priority++;
            LB3 += min(min(block1, block2), block3);
        }
        return LB3;
    }

    int LB4(Direction dir1, Direction dir2, Direction dir3, Direction dir4, Direction rel)
    {
        Config config_temp = config;
        int LB4 = 0;
        if (rel != Any)
        {
            int block = config_temp.count(rel);
            LB4 += block;
            config_temp.delete_block(rel);
            config_temp.priority++;
        }
        while (config_temp.priority != nblock)
        {
            while (config_temp.block[config_temp.pos[config_temp.priority - 1].x][config_temp.pos[config_temp.priority - 1].y] == 0)
            {
                if (config_temp.priority == nblock - 1)
                    return LB4;
                config_temp.priority++;
            }
            int block1 = config_temp.count(dir1);
            int block2 = config_temp.count(dir2);
            int block3 = config_temp.count(dir3);
            int block4 = config_temp.count(dir4);
            while ((block1 == 0) || (block2 == 0) || (block3 == 0) || (block4 == 0))
            {
                config_temp.block[config_temp.pos[config_temp.priority - 1].x][config_temp.pos[config_temp.priority - 1].y] = 0;
                if (config_temp.priority == nblock)
                    return LB4;
                config_temp.priority++;
                block1 = config_temp.count(dir1);
                block2 = config_temp.count(dir2);
                block3 = config_temp.count(dir3);
                block4 = config_temp.count(dir4);
            }
            config_temp.delete_block(dir1);
            config_temp.delete_block(dir2);
            config_temp.delete_block(dir3);
            config_temp.delete_block(dir4);
            LB4 += min(min(block1, block2), min(block3, block4));
        }
        return LB4;
    }

    int UB1(int &T)
    {
        Config config_temp = config;
        int UB1 = 0;
        int T_temp = T;
        for (int n = config_temp.priority; n <= nblock; n++)
        {
            // ブロッキングブロックを積み替える場合
            for (int j = TIER - 1; j >= config_temp.pos[n - 1].y + 1; j--)
            {
                // ブロックが配置されていた場合，ブロックを積み替える
                if (config_temp.block[config_temp.pos[n - 1].x][j])
                {
                    UB1++;
                    T_temp++;
                    // 最大優先度が最も低いスタックを格納
                    int temp = 0;
                    // 積み替え先
                    Point dst = config_temp.pos[n - 1];
                    // 積み替え先スタックを決定
                    for (int i = 0; i < STACK; i++)
                    {
                        if ((i != config_temp.pos[n - 1].x) && !config_temp.block[i][TIER - 1] && (temp < config_temp.P_UL[i]))
                        {
                            temp = config_temp.P_UL[i];
                            dst.x = i;
                            int dst_y = TIER - 1;
                            while (!config_temp.block[dst.x][dst_y - 1] && (dst_y > 0))
                                dst_y--;
                            dst.y = dst_y;
                        }
                    }
                    Point src = {config_temp.pos[n - 1].x, j};
                    if (config_temp.pos[n - 1] == dst)
                    {
                        return 100;
                    }
                    config_temp.relocate(src, dst);

#if UB_TEST == 0
                    config_temp.print();
#endif
                }
            }
            Point src = {config_temp.pos[n - 1].x, config_temp.pos[n - 1].y};
            config_temp.retrieve(src);

#if UB_TEST == 0
            config_temp.print();
#endif

            T = max(T, T_temp);
            T_temp = 0;
        }
        return UB1;
    }

    int UB2(Direction dir1, Direction dir2, int &T, Direction rel)
    {
        Config config_temp = config;
        int UB2 = 0;
        int T_temp = T;
        int p;
        for (int n = config_temp.priority; n <= nblock; n++)
        {
            int block1 = config_temp.count(dir1);
            int block2 = config_temp.count(dir2);
            if ((block1 == 0) || (block2 == 0))
            {
                T = max(T, T_temp);
                T_temp = 0;
                Point src = {config_temp.pos[n - 1].x, config_temp.pos[n - 1].y};
                config_temp.retrieve(src);
            }
            else
            {
                Direction dir_rel;
                if (rel != Any)
                {
                    dir_rel = rel;
                }
                else
                {
                    if (block1 != block2)
                        dir_rel = (block1 < block2) ? dir1 : dir2;
                    else
                    {
                        dir_rel = Upp;
                    }
                }
                switch (dir_rel)
                {
                case Upp:
                    // ブロッキングブロックを積み替える場合
                    for (int j = TIER - 1; j >= config_temp.pos[n - 1].y + 1; j--)
                    {
                        // ブロックが配置されていた場合，ブロックを積み替える
                        if (config_temp.block[config_temp.pos[n - 1].x][j])
                        {
                            UB2++;
                            T_temp++;
                            p = config_temp.block[config_temp.pos[n - 1].x][j];
                            config_temp.block[config_temp.pos[n - 1].x][j] = 0;
                            // 最大優先度が最も低いスタックを格納
                            int temp = 0;
                            // 積み替え先
                            Point dst = config_temp.pos[n - 1];
                            // 積み替え先を決定
                            for (int i = 0; i < STACK; i++)
                            {
                                int dst_y = TIER - 1;
                                if ((i != config_temp.pos[n - 1].x) && !config_temp.block[i][TIER - 1])
                                {
                                    while (!config_temp.block[i][dst_y - 1] && (dst_y > 0))
                                        dst_y--;
                                    if (temp < config_temp.P[i][dst_y])
                                    {
                                        temp = config_temp.P[i][dst_y];
                                        dst.x = i;
                                        dst.y = dst_y;
                                    }
                                }
                            }
                            for (int i = 0; i < TIER; i++)
                            {
                                int dst_x = STACK - 1;
                                if (!config_temp.block[STACK - 1][i])
                                {
                                    while (!config_temp.block[dst_x - 1][i] && (dst_x > 0))
                                        dst_x--;
                                    if ((dst_x == config_temp.pos[n - 1].x) && (i > config_temp.pos[n - 1].y))
                                        continue;
                                    if (temp < config_temp.P[dst_x][i])
                                    {
                                        temp = config_temp.P[dst_x][i];
                                        dst.x = dst_x;
                                        dst.y = i;
                                    }
                                }
                            }
                            config_temp.block[config_temp.pos[n - 1].x][j] = p;
                            Point src = {config_temp.pos[n - 1].x, j};
                            if (config_temp.pos[n - 1] == dst)
                            {
                                return 100;
                            }
                            config_temp.relocate(src, dst);

#if UB_TEST == 0
                            config_temp.print();
#endif
                        }
                    }
                    break;
                case Right:
                    // ブロッキングブロックを積み替える場合
                    for (int j = STACK - 1; j >= config_temp.pos[n - 1].x + 1; j--)
                    {
                        // ブロックが配置されていた場合，ブロックを積み替える
                        if (config_temp.block[j][config_temp.pos[n - 1].y])
                        {
                            UB2++;
                            T_temp++;
                            p = config_temp.block[j][config_temp.pos[n - 1].y];
                            config_temp.block[j][config_temp.pos[n - 1].y] = 0;
                            // 最大優先度が最も低いスタックを格納
                            int temp = 0;
                            // 積み替え先
                            Point dst = config_temp.pos[n - 1];
                            // 積み替え先を決定
                            for (int i = 0; i < STACK; i++)
                            {
                                int dst_y = TIER - 1;
                                if (!config_temp.block[i][TIER - 1])
                                {
                                    while (!config_temp.block[i][dst_y - 1] && (dst_y > 0))
                                        dst_y--;
                                    if ((i > config_temp.pos[n - 1].x) && (dst_y == config_temp.pos[n - 1].y))
                                        continue;
                                    if (temp < config_temp.P[i][dst_y])
                                    {
                                        temp = config_temp.P[i][dst_y];
                                        dst.x = i;
                                        dst.y = dst_y;
                                    }
                                }
                            }
                            for (int i = 0; i < TIER; i++)
                            {
                                int dst_x = STACK - 1;
                                if ((i != config_temp.pos[n - 1].y) && !config_temp.block[STACK - 1][i])
                                {
                                    while (!config_temp.block[dst_x - 1][i] && (dst_x > 0))
                                        dst_x--;
                                    if (temp < config_temp.P[dst_x][i])
                                    {
                                        temp = config_temp.P[dst_x][i];
                                        dst.x = dst_x;
                                        dst.y = i;
                                    }
                                }
                            }
                            Point src = {j, config_temp.pos[n - 1].y};
                            config_temp.block[j][config_temp.pos[n - 1].y] = p;
                            if (config_temp.pos[n - 1] == dst)
                            {
                                return 100;
                            }
                            config_temp.relocate(src, dst);

#if UB_TEST == 0
                            config_temp.print();
#endif
                        }
                    }
                    break;
                default:
                    break;
                }
                Point src = {config_temp.pos[n - 1].x, config_temp.pos[n - 1].y};
                config_temp.retrieve(src);
#if UB_TEST == 0
                config_temp.print();
#endif
                T = max(T, T_temp);
                T_temp = 0;
            }
        }
        T = max(T, T_temp);
        return UB2;
    }

    int UB2s(Direction dir1, Direction dir2, int &T, Direction rel)
    {
        Config config_temp = config;
        int UB2s = 0;
        int T_temp = T;
        int p;
        for (int n = config_temp.priority; n <= nblock; n++)
        {
            int block1 = config_temp.count(dir1);
            int block2 = config_temp.count(dir2);
            if ((block1 == 0) || (block2 == 0))
            {
                T = max(T, T_temp);
                T_temp = 0;
                Point src = {config_temp.pos[n - 1].x, config_temp.pos[n - 1].y};
                config_temp.retrieve(src);
            }
            else
            {
                Direction dir_rel;
                if (rel != Any)
                {
                    dir_rel = rel;
                }
                else
                {
                    if (block1 != block2)
                        dir_rel = (block1 < block2) ? dir1 : dir2;
                    else
                    {
                        dir_rel = Upp;
                    }
                }
                switch (dir_rel)
                {
                case Upp:
                    // ブロッキングブロックを積み替える場合
                    for (int j = TIER - 1; j >= config_temp.pos[n - 1].y + 1; j--)
                    {
                        // ブロックが配置されていた場合，ブロックを積み替える
                        if (config_temp.block[config_temp.pos[n - 1].x][j])
                        {
                            UB2s++;
                            T_temp++;
                            p = config_temp.block[config_temp.pos[n - 1].x][j];
                            config_temp.block[config_temp.pos[n - 1].x][j] = 0;
                            // 最大優先度が最も低いスタックを格納
                            int temp = 0;
                            // 積み替え先
                            Point dst = config_temp.pos[n - 1];
                            // 積み替え先を決定
                            for (int i = 0; i < STACK; i++)
                            {
                                int dst_y = TIER - 1;
                                if ((i != config_temp.pos[n - 1].x) && !config_temp.block[i][TIER - 1])
                                {
                                    while (!config_temp.block[i][dst_y - 1] && (dst_y > 0))
                                        dst_y--;
                                    if (temp < config_temp.P[i][dst_y])
                                    {
                                        temp = config_temp.P[i][dst_y];
                                        dst.x = i;
                                        dst.y = dst_y;
                                    }
                                }
                            }
                            for (int i = 0; i < STACK; i++)
                            {
                                int dst_y = 0;
                                if ((i != config_temp.pos[n - 1].x) && !config_temp.block[i][0])
                                {
                                    while (!config_temp.block[i][dst_y + 1] && (dst_y < TIER - 1))
                                        dst_y++;
                                    if (temp < config_temp.P[i][dst_y])
                                    {
                                        temp = config_temp.P[i][dst_y];
                                        dst.x = i;
                                        dst.y = dst_y;
                                    }
                                }
                            }
                            config_temp.block[config_temp.pos[n - 1].x][j] = p;
                            Point src = {config_temp.pos[n - 1].x, j};
                            if (config_temp.pos[n - 1] == dst)
                            {
                                return 100;
                            }
                            config_temp.relocate(src, dst);

#if UB_TEST == 0
                            config_temp.print();
#endif
                        }
                    }
                    break;
                case Low:
                    // ブロッキングブロックを積み替える場合
                    for (int j = 0; j <= config_temp.pos[n - 1].y - 1; j++)
                    {
                        // ブロックが配置されていた場合，ブロックを積み替える
                        if (config_temp.block[config_temp.pos[n - 1].x][j])
                        {
                            UB2s++;
                            T_temp++;
                            p = config_temp.block[config_temp.pos[n - 1].x][j];
                            config_temp.block[config_temp.pos[n - 1].x][j] = 0;
                            // 最大優先度が最も低いスタックを格納
                            int temp = 0;
                            // 積み替え先
                            Point dst = config_temp.pos[n - 1];
                            // 積み替え先を決定
                            for (int i = 0; i < STACK; i++)
                            {
                                int dst_y = 0;
                                if ((i != config_temp.pos[n - 1].x) && !config_temp.block[i][0])
                                {
                                    while (!config_temp.block[i][dst_y + 1] && (dst_y < TIER - 1))
                                        dst_y++;
                                    if (temp < config_temp.P[i][dst_y])
                                    {
                                        temp = config_temp.P[i][dst_y];
                                        dst.x = i;
                                        dst.y = dst_y;
                                    }
                                }
                            }
                            for (int i = 0; i < STACK; i++)
                            {
                                int dst_y = TIER - 1;
                                if ((i != config_temp.pos[n - 1].x) && !config_temp.block[i][TIER - 1])
                                {
                                    while (!config_temp.block[i][dst_y - 1] && (dst_y > 0))
                                        dst_y--;
                                    if (temp < config_temp.P[i][dst_y])
                                    {
                                        temp = config_temp.P[i][dst_y];
                                        dst.x = i;
                                        dst.y = dst_y;
                                    }
                                }
                            }
                            config_temp.block[config_temp.pos[n - 1].x][j] = p;
                            Point src = {config_temp.pos[n - 1].x, j};
                            if (config_temp.pos[n - 1] == dst)
                            {
                                return 100;
                            }
                            config_temp.relocate(src, dst);

#if UB_TEST == 0
                            config_temp.print();
#endif
                        }
                    }
                    break;
                default:
                    break;
                }
                Point src = {config_temp.pos[n - 1].x, config_temp.pos[n - 1].y};
                config_temp.retrieve(src);
#if UB_TEST == 0
                config_temp.print();
#endif
                T = max(T, T_temp);
                T_temp = 0;
            }
        }
        T = max(T, T_temp);
        return UB2s;
    }

    int UB3(Direction dir1, Direction dir2, Direction dir3, int &T, Direction rel)
    {
        Config config_temp = config;
        int UB3 = 0;
        int T_temp = T;
        int p;
        for (int n = config_temp.priority; n <= nblock; n++)
        {

#if UB_TEST == 0
            config_temp.print();
#endif

            int block1 = config_temp.count(dir1);
            int block2 = config_temp.count(dir2);
            int block3 = config_temp.count(dir3);
            if ((block1 == 0) || (block2 == 0) || (block3 == 0))
            {
                T = max(T, T_temp);
                T_temp = 0;
                Point src = {config_temp.pos[n - 1].x, config_temp.pos[n - 1].y};
                config_temp.retrieve(src);
            }
            else
            {
                Direction dir_rel;
                if (rel != Any)
                {
                    dir_rel = rel;
                }
                else
                {
                    if (block1 <= block2 && block1 <= block3)
                        dir_rel = Upp;
                    else
                    {
                        int a = min(min(block1, block2), block3);
                        if (a == block1)
                            dir_rel = dir1;
                        else if (a == block2)
                            dir_rel = dir2;
                        else
                            dir_rel = dir3;
                    }
                }
                switch (dir_rel)
                {
                case Upp:
                    // ブロッキングブロックを積み替える場合
                    for (int j = TIER - 1; j >= config_temp.pos[n - 1].y + 1; j--)
                    {
                        // ブロックが配置されていた場合，ブロックを積み替える
                        if (config_temp.block[config_temp.pos[n - 1].x][j])
                        {
                            UB3++;
                            T_temp++;
                            p = config_temp.block[config_temp.pos[n - 1].x][j];
                            config_temp.block[config_temp.pos[n - 1].x][j] = 0;
                            // 最大優先度が最も低いスタックを格納
                            int temp = 0;
                            // 積み替え先
                            Point dst = config_temp.pos[n - 1];
                            // 積み替え先を決定
                            // 上方向
                            for (int i = 0; i < STACK; i++)
                            {
                                int dst_y = TIER - 1;
                                if ((i != config_temp.pos[n - 1].x) && !config_temp.block[i][TIER - 1])
                                {
                                    while (!config_temp.block[i][dst_y - 1] && (dst_y > 0))
                                        dst_y--;
                                    if (temp < config_temp.P[i][dst_y])
                                    {
                                        temp = config_temp.P[i][dst_y];
                                        dst.x = i;
                                        dst.y = dst_y;
                                    }
                                }
                            }
                            // 右方向
                            for (int i = 0; i < TIER; i++)
                            {
                                int dst_x = STACK - 1;
                                if (!config_temp.block[STACK - 1][i])
                                {
                                    while (!config_temp.block[dst_x - 1][i] && (dst_x > 0))
                                        dst_x--;
                                    if ((dst_x == config_temp.pos[n - 1].x) && (i > config_temp.pos[n - 1].y))
                                        continue;
                                    if (temp < config_temp.P[dst_x][i])
                                    {
                                        temp = config_temp.P[dst_x][i];
                                        dst.x = dst_x;
                                        dst.y = i;
                                    }
                                }
                            }
                            // 下方向
                            for (int i = 0; i < STACK; i++)
                            {
                                int dst_y = 0;
                                if (!config_temp.block[i][0])
                                {
                                    while (!config_temp.block[i][dst_y + 1] && (dst_y < TIER - 1))
                                        dst_y++;
                                    if (temp < config_temp.P[i][dst_y])
                                    {
                                        temp = config_temp.P[i][dst_y];
                                        dst.x = i;
                                        dst.y = dst_y;
                                    }
                                }
                            }
                            Point src = {config_temp.pos[n - 1].x, j};
                            config_temp.block[config_temp.pos[n - 1].x][j] = p;
                            if (config_temp.pos[n - 1] == dst)
                            {
                                return 100;
                            }
                            config_temp.relocate(src, dst);
#if UB_TEST == 0
                            config_temp.print();
#endif
                        }
                    }
                    break;
                case Right:
                    // ブロッキングブロックを積み替える場合
                    for (int j = STACK - 1; j >= config_temp.pos[n - 1].x + 1; j--)
                    {
                        // ブロックが配置されていた場合，ブロックを積み替える
                        if (config_temp.block[j][config_temp.pos[n - 1].y])
                        {
                            UB3++;
                            T_temp++;
                            p = config_temp.block[j][config_temp.pos[n - 1].y];
                            config_temp.block[j][config_temp.pos[n - 1].y] = 0;
                            // 最大優先度が最も低いスタックを格納
                            int temp = 0;
                            // 積み替え先
                            Point dst = config_temp.pos[n - 1];
                            // 積み替え先を決定
                            // 上方向
                            for (int i = 0; i < STACK; i++)
                            {
                                int dst_y = TIER - 1;
                                if (!config_temp.block[i][TIER - 1])
                                {
                                    while (!config_temp.block[i][dst_y - 1] && (dst_y > 0))
                                        dst_y--;
                                    if ((i > config_temp.pos[n - 1].x) && (dst_y == config_temp.pos[n - 1].y))
                                        continue;
                                    if (temp < config_temp.P[i][dst_y])
                                    {
                                        temp = config_temp.P[i][dst_y];
                                        dst.x = i;
                                        dst.y = dst_y;
                                    }
                                }
                            }
                            // 右方向
                            for (int i = 0; i < TIER; i++)
                            {
                                int dst_x = STACK - 1;
                                if ((i != config_temp.pos[n - 1].y) && !config_temp.block[STACK - 1][i])
                                {
                                    while (!config_temp.block[dst_x - 1][i] && (dst_x > 0))
                                        dst_x--;
                                    if (temp < config_temp.P[dst_x][i])
                                    {
                                        temp = config_temp.P[dst_x][i];
                                        dst.x = dst_x;
                                        dst.y = i;
                                    }
                                }
                            }
                            // 下方向
                            for (int i = 0; i < STACK; i++)
                            {
                                int dst_y = 0;
                                if (!config_temp.block[i][0])
                                {
                                    while (!config_temp.block[i][dst_y + 1] && (dst_y < TIER - 1))
                                        dst_y++;
                                    if ((i > config_temp.pos[n - 1].x) && (dst_y == config_temp.pos[n - 1].y))
                                        continue;
                                    if (temp < config_temp.P[i][dst_y])
                                    {
                                        temp = config_temp.P[i][dst_y];
                                        dst.x = i;
                                        dst.y = dst_y;
                                    }
                                }
                            }
                            Point src = {j, config_temp.pos[n - 1].y};
                            config_temp.block[j][config_temp.pos[n - 1].y] = p;
                            if (config_temp.pos[n - 1] == dst)
                            {
                                return 100;
                            }
                            config_temp.relocate(src, dst);
#if UB_TEST == 0
                            config_temp.print();
#endif
                        }
                    }
                    break;
                case Low:
                    // ブロッキングブロックを積み替える場合
                    for (int j = 0; j <= config_temp.pos[n - 1].y - 1; j++)
                    {
                        // ブロックが配置されていた場合，ブロックを積み替える
                        if (config_temp.block[config_temp.pos[n - 1].x][j])
                        {
                            UB3++;
                            T_temp++;
                            p = config_temp.block[config_temp.pos[n - 1].x][j];
                            config_temp.block[config_temp.pos[n - 1].x][j] = 0;
                            // 最大優先度が最も低いスタックを格納
                            int temp = 0;
                            // 積み替え先
                            Point dst = config_temp.pos[n - 1];
                            // 積み替え先を決定
                            // 上方向
                            for (int i = 0; i < STACK; i++)
                            {
                                int dst_y = TIER - 1;
                                if (!config_temp.block[i][TIER - 1])
                                {
                                    while (!config_temp.block[i][dst_y - 1] && (dst_y > 0))
                                        dst_y--;
                                    if (temp < config_temp.P[i][dst_y])
                                    {
                                        temp = config_temp.P[i][dst_y];
                                        dst.x = i;
                                        dst.y = dst_y;
                                    }
                                }
                            }
                            // 右方向
                            for (int i = 0; i < TIER; i++)
                            {
                                int dst_x = STACK - 1;
                                if (!config_temp.block[STACK - 1][i])
                                {
                                    while (!config_temp.block[dst_x - 1][i] && (dst_x > 0))
                                        dst_x--;
                                    if ((dst_x == config_temp.pos[n - 1].x) && (i < config_temp.pos[n - 1].y))
                                        continue;
                                    if (temp < config_temp.P[dst_x][i])
                                    {
                                        temp = config_temp.P[dst_x][i];
                                        dst.x = dst_x;
                                        dst.y = i;
                                    }
                                }
                            }
                            // 下方向
                            for (int i = 0; i < STACK; i++)
                            {
                                int dst_y = 0;
                                if ((i != config_temp.pos[n - 1].x) && !config_temp.block[i][0])
                                {
                                    while (!config_temp.block[i][dst_y + 1] && (dst_y < TIER - 1))
                                        dst_y++;
                                    if (temp < config_temp.P[i][dst_y])
                                    {
                                        temp = config_temp.P[i][dst_y];
                                        dst.x = i;
                                        dst.y = dst_y;
                                    }
                                }
                            }
                            Point src = {config_temp.pos[n - 1].x, j};
                            config_temp.block[config_temp.pos[n - 1].x][j] = p;
                            if (config_temp.pos[n - 1] == dst)
                            {
                                return 100;
                            }
                            config_temp.relocate(src, dst);
#if UB_TEST == 0
                            config_temp.print();
#endif
                        }
                    }
                    break;
                default:
                    break;
                }
                T = max(T, T_temp);
                T_temp = 0;
                Point src = {config_temp.pos[n - 1].x, config_temp.pos[n - 1].y};
                config_temp.retrieve(src);
            }
        }
        T = max(T, T_temp);
        T_temp = 0;
        return UB3;
    }

    int UB4(Direction dir1, Direction dir2, Direction dir3, Direction dir4, int &T, Direction rel)
    {
        Config config_temp = config;
        int UB4 = 0;
        int T_temp = 0;
        int p;
        for (int n = config_temp.priority; n <= nblock; n++)
        {
            int block1 = config_temp.count(dir1);
            int block2 = config_temp.count(dir2);
            int block3 = config_temp.count(dir3);
            int block4 = config_temp.count(dir4);
            if ((block1 == 0) || (block2 == 0) || (block3 == 0) || (block4 == 0))
            {
                T = max(T, T_temp);
                T_temp = 0;
                Point src = {config_temp.pos[n - 1].x, config_temp.pos[n - 1].y};
                config_temp.retrieve(src);
#if UB_TEST == 0
                config_temp.print();
#endif
            }
            else
            {
                Direction dir_rel;
                if (block1 <= block2 && block1 <= block3 && block1 <= block4)
                    dir_rel = Upp;
                else
                {
                    if (rel != Any)
                    {
                        dir_rel = rel;
                    }
                    else
                    {
                        int a = min(min(block1, block2), min(block3, block4));
                        if (a == block1)
                            dir_rel = dir1;
                        else if (a == block2)
                            dir_rel = dir2;
                        else if (a == block3)
                            dir_rel = dir3;
                        else
                            dir_rel = dir4;
                    }
                }
                switch (dir_rel)
                {
                case Upp:
                    // ブロッキングブロックを積み替える場合
                    for (int j = TIER - 1; j >= config_temp.pos[n - 1].y + 1; j--)
                    {
                        // ブロックが配置されていた場合，ブロックを積み替える
                        if (config_temp.block[config_temp.pos[n - 1].x][j])
                        {
                            UB4++;
                            T_temp++;
                            p = config_temp.block[config_temp.pos[n - 1].x][j];
                            config_temp.block[config_temp.pos[n - 1].x][j] = 0;
                            // 最大優先度が最も低いスタックを格納
                            int temp = 0;
                            // 積み替え先
                            Point dst = config_temp.pos[n - 1];
                            // 積み替え先を決定
                            // 上方向
                            for (int i = 0; i < STACK; i++)
                            {
                                int dst_y = TIER - 1;
                                if ((i != config_temp.pos[n - 1].x) && !config_temp.block[i][TIER - 1])
                                {
                                    while (!config_temp.block[i][dst_y - 1] && (dst_y > 0))
                                        dst_y--;
                                    if (temp < config_temp.P[i][dst_y])
                                    {
                                        temp = config_temp.P[i][dst_y];
                                        dst.x = i;
                                        dst.y = dst_y;
                                    }
                                }
                            }
                            // 右方向
                            for (int i = 0; i < TIER; i++)
                            {
                                int dst_x = STACK - 1;
                                if (!config_temp.block[STACK - 1][i])
                                {
                                    while (!config_temp.block[dst_x - 1][i] && (dst_x > 0))
                                        dst_x--;
                                    if ((dst_x == config_temp.pos[n - 1].x) && (i > config_temp.pos[n - 1].y))
                                        continue;
                                    if (temp < config_temp.P[dst_x][i])
                                    {
                                        temp = config_temp.P[dst_x][i];
                                        dst.x = dst_x;
                                        dst.y = i;
                                    }
                                }
                            }
                            // 下方向
                            for (int i = 0; i < STACK; i++)
                            {
                                int dst_y = 0;
                                if (!config_temp.block[i][0])
                                {
                                    while (!config_temp.block[i][dst_y + 1] && (dst_y < TIER - 1))
                                        dst_y++;
                                    if (temp < config_temp.P[i][dst_y])
                                    {
                                        temp = config_temp.P[i][dst_y];
                                        dst.x = i;
                                        dst.y = dst_y;
                                    }
                                }
                            }
                            // 左方向
                            for (int i = 0; i < TIER; i++)
                            {
                                int dst_x = 0;
                                if (!config_temp.block[0][i])
                                {
                                    while (!config_temp.block[dst_x + 1][i] && (dst_x < STACK - 1))
                                        dst_x++;
                                    if ((dst_x == config_temp.pos[n - 1].x) && (i > config_temp.pos[n - 1].y))
                                        continue;
                                    if (temp < config_temp.P[dst_x][i])
                                    {
                                        temp = config_temp.P[dst_x][i];
                                        dst.x = dst_x;
                                        dst.y = i;
                                    }
                                }
                            }
                            Point src = {config_temp.pos[n - 1].x, j};
                            config_temp.block[config_temp.pos[n - 1].x][j] = p;
                            if (config_temp.pos[n - 1] == dst)
                            {
                                return 100;
                            }
                            config_temp.relocate(src, dst);
#if UB_TEST == 0
                            config_temp.print();
#endif
                        }
                    }
                    break;
                case Right:
                    // ブロッキングブロックを積み替える場合
                    for (int j = STACK - 1; j >= config_temp.pos[n - 1].x + 1; j--)
                    {
                        // ブロックが配置されていた場合，ブロックを積み替える
                        if (config_temp.block[j][config_temp.pos[n - 1].y])
                        {
                            UB4++;
                            T_temp++;
                            p = config_temp.block[j][config_temp.pos[n - 1].y];
                            config_temp.block[j][config_temp.pos[n - 1].y] = 0;
                            // 最大優先度が最も低いスタックを格納
                            int temp = 0;
                            // 積み替え先
                            Point dst = config_temp.pos[n - 1];
                            // 積み替え先を決定
                            // 上方向
                            for (int i = 0; i < STACK; i++)
                            {
                                int dst_y = TIER - 1;
                                if (!config_temp.block[i][TIER - 1])
                                {
                                    while (!config_temp.block[i][dst_y - 1] && (dst_y > 0))
                                        dst_y--;
                                    if ((i > config_temp.pos[n - 1].x) && (dst_y == config_temp.pos[n - 1].y))
                                        continue;
                                    if (temp < config_temp.P[i][dst_y])
                                    {
                                        temp = config_temp.P[i][dst_y];
                                        dst.x = i;
                                        dst.y = dst_y;
                                    }
                                }
                            }
                            // 右方向
                            for (int i = 0; i < TIER; i++)
                            {
                                int dst_x = STACK - 1;
                                if ((i != config_temp.pos[n - 1].y) && !config_temp.block[STACK - 1][i])
                                {
                                    while (!config_temp.block[dst_x - 1][i] && (dst_x > 0))
                                        dst_x--;
                                    if (temp < config_temp.P[dst_x][i])
                                    {
                                        temp = config_temp.P[dst_x][i];
                                        dst.x = dst_x;
                                        dst.y = i;
                                    }
                                }
                            }
                            // 下方向
                            for (int i = 0; i < STACK; i++)
                            {
                                int dst_y = 0;
                                if (!config_temp.block[i][0])
                                {
                                    while (!config_temp.block[i][dst_y + 1] && (dst_y < TIER - 1))
                                        dst_y++;
                                    if ((i > config_temp.pos[n - 1].x) && (dst_y == config_temp.pos[n - 1].y))
                                        continue;
                                    if (temp < config_temp.P[i][dst_y])
                                    {
                                        temp = config_temp.P[i][dst_y];
                                        dst.x = i;
                                        dst.y = dst_y;
                                    }
                                }
                            }
                            // 左方向
                            for (int i = 0; i < TIER; i++)
                            {
                                int dst_x = 0;
                                if (!config_temp.block[0][i])
                                {
                                    while (!config_temp.block[dst_x + 1][i] && (dst_x < STACK - 1))
                                        dst_x++;
                                    if (temp < config_temp.P[dst_x][i])
                                    {
                                        temp = config_temp.P[dst_x][i];
                                        dst.x = dst_x;
                                        dst.y = i;
                                    }
                                }
                            }
                            Point src = {j, config_temp.pos[n - 1].y};
                            config_temp.block[j][config_temp.pos[n - 1].y] = p;
                            if (config_temp.pos[n - 1] == dst)
                            {
                                return 100;
                            }
                            config_temp.relocate(src, dst);
#if UB_TEST == 0
                            config_temp.print();
#endif
                        }
                    }
                    break;
                case Low:
                    // ブロッキングブロックを積み替える場合
                    for (int j = 0; j <= config_temp.pos[n - 1].y - 1; j++)
                    {
                        // ブロックが配置されていた場合，ブロックを積み替える
                        if (config_temp.block[config_temp.pos[n - 1].x][j])
                        {
                            UB4++;
                            T_temp++;
                            p = config_temp.block[config_temp.pos[n - 1].x][j];
                            config_temp.block[config_temp.pos[n - 1].x][j] = 0;
                            // 最大優先度が最も低いスタックを格納
                            int temp = 0;
                            // 積み替え先
                            Point dst = config_temp.pos[n - 1];
                            // 積み替え先を決定
                            // 上方向
                            for (int i = 0; i < STACK; i++)
                            {
                                int dst_y = TIER - 1;
                                if (!config_temp.block[i][TIER - 1])
                                {
                                    while (!config_temp.block[i][dst_y - 1] && (dst_y > 0))
                                        dst_y--;
                                    if (temp < config_temp.P[i][dst_y])
                                    {
                                        temp = config_temp.P[i][dst_y];
                                        dst.x = i;
                                        dst.y = dst_y;
                                    }
                                }
                            }
                            // 右方向
                            for (int i = 0; i < TIER; i++)
                            {
                                int dst_x = STACK - 1;
                                if (!config_temp.block[STACK - 1][i])
                                {
                                    while (!config_temp.block[dst_x - 1][i] && (dst_x > 0))
                                        dst_x--;
                                    if ((dst_x == config_temp.pos[n - 1].x) && (i < config_temp.pos[n - 1].y))
                                        continue;
                                    if (temp < config_temp.P[dst_x][i])
                                    {
                                        temp = config_temp.P[dst_x][i];
                                        dst.x = dst_x;
                                        dst.y = i;
                                    }
                                }
                            }
                            // 下方向
                            for (int i = 0; i < STACK; i++)
                            {
                                int dst_y = 0;
                                if ((i != config_temp.pos[n - 1].x) && !config_temp.block[i][0])
                                {
                                    while (!config_temp.block[i][dst_y + 1] && (dst_y < TIER - 1))
                                        dst_y++;
                                    if (temp < config_temp.P[i][dst_y])
                                    {
                                        temp = config_temp.P[i][dst_y];
                                        dst.x = i;
                                        dst.y = dst_y;
                                    }
                                }
                            }
                            // 左方向
                            for (int i = 0; i < TIER; i++)
                            {
                                int dst_x = 0;
                                if (!config_temp.block[0][i])
                                {
                                    while (!config_temp.block[dst_x + 1][i] && (dst_x < STACK - 1))
                                        dst_x++;
                                    if ((dst_x == config_temp.pos[n - 1].x) && (i < config_temp.pos[n - 1].y))
                                        continue;
                                    if (temp < config_temp.P[dst_x][i])
                                    {
                                        temp = config_temp.P[dst_x][i];
                                        dst.x = dst_x;
                                        dst.y = i;
                                    }
                                }
                            }
                            Point src = {config_temp.pos[n - 1].x, j};
                            config_temp.block[config_temp.pos[n - 1].x][j] = p;
                            if (config_temp.pos[n - 1] == dst)
                            {
                                return 100;
                            }
                            config_temp.relocate(src, dst);
#if UB_TEST == 0
                            config_temp.print();
#endif
                        }
                    }
                    break;
                case Left:
                    // ブロッキングブロックを積み替える場合
                    for (int j = 0; j <= config_temp.pos[n - 1].x - 1; j++)
                    {
                        // ブロックが配置されていた場合，ブロックを積み替える
                        if (config_temp.block[j][config_temp.pos[n - 1].y])
                        {
                            UB4++;
                            T_temp++;
                            p = config_temp.block[j][config_temp.pos[n - 1].y];
                            config_temp.block[j][config_temp.pos[n - 1].y] = 0;
                            // 最大優先度が最も低いスタックを格納
                            int temp = 0;
                            // 積み替え先
                            Point dst = config_temp.pos[n - 1];
                            // 積み替え先を決定
                            // 上方向
                            for (int i = 0; i < STACK; i++)
                            {
                                int dst_y = TIER - 1;
                                if (!config_temp.block[i][TIER - 1])
                                {
                                    while (!config_temp.block[i][dst_y - 1] && (dst_y > 0))
                                        dst_y--;
                                    if ((i < config_temp.pos[n - 1].x) && (dst_y == config_temp.pos[n - 1].y))
                                        continue;
                                    if (temp < config_temp.P[i][dst_y])
                                    {
                                        temp = config_temp.P[i][dst_y];
                                        dst.x = i;
                                        dst.y = dst_y;
                                    }
                                }
                            }
                            // 右方向
                            for (int i = 0; i < TIER; i++)
                            {
                                int dst_x = STACK - 1;
                                if (!config_temp.block[STACK - 1][i])
                                {
                                    while (!config_temp.block[dst_x - 1][i] && (dst_x > 0))
                                        dst_x--;
                                    if (temp < config_temp.P[dst_x][i])
                                    {
                                        temp = config_temp.P[dst_x][i];
                                        dst.x = dst_x;
                                        dst.y = i;
                                    }
                                }
                            }
                            // 下方向
                            for (int i = 0; i < STACK; i++)
                            {
                                int dst_y = 0;
                                if (!config_temp.block[i][0])
                                {
                                    while (!config_temp.block[i][dst_y + 1] && (dst_y < TIER - 1))
                                        dst_y++;
                                    if ((i < config_temp.pos[n - 1].x) && (dst_y == config_temp.pos[n - 1].y))
                                        continue;
                                    if (temp < config_temp.P[i][dst_y])
                                    {
                                        temp = config_temp.P[i][dst_y];
                                        dst.x = i;
                                        dst.y = dst_y;
                                    }
                                }
                            }
                            // 左方向
                            for (int i = 0; i < TIER; i++)
                            {
                                int dst_x = 0;
                                if ((i != config_temp.pos[n - 1].y) && !config_temp.block[0][i])
                                {
                                    while (!config_temp.block[dst_x + 1][i] && (dst_x < STACK - 1))
                                        dst_x++;
                                    if (temp < config_temp.P[dst_x][i])
                                    {
                                        temp = config_temp.P[dst_x][i];
                                        dst.x = dst_x;
                                        dst.y = i;
                                    }
                                }
                            }
                            Point src = {j, config_temp.pos[n - 1].y};
                            config_temp.block[j][config_temp.pos[n - 1].y] = p;
                            if (config_temp.pos[n - 1] == dst)
                            {
                                return 100;
                            }
                            config_temp.relocate(src, dst);
#if UB_TEST == 0
                            config_temp.print();
#endif
                        }
                    }
                    break;
                }
                T = max(T, T_temp);
                T_temp = 0;
                Point src = {config_temp.pos[n - 1].x, config_temp.pos[n - 1].y};
                config_temp.retrieve(src);
#if UB_TEST == 0
                config_temp.print();
#endif
            }
        }
        return UB4;
    }
};
#endif
