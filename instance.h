#include <iostream>
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

    // コンストラクタ
    Instance()
    {
    }

    // ファイル読み込み
    void readFile(const std::string &path)
    {
        std::ifstream file(path);
        if (!file.is_open())
        {
            std::cerr << "Failed to open file: " << path << std::endl;
            return;
        }

        std::string line;
        int i = 0;
        int j = 0;
        // 最初の１行を捨てる
        getline(file, line);

        while (std::getline(file, line))
        {
            std::istringstream iss(line);
            int num, value;
            iss >> num;
            for (int k = 0; k < num; k++)
            {
                iss >> value;
                if (value != 0)
                {
                    if (config.P_UL[i] > value)
                        config.P_UL[i] = value;
                    config.pos[value - 1].x = i;
                    config.pos[value - 1].y = j;
                }
                config.block[i][j] = value;
                j++;
            }
            j = 0;
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
    void LB1()
    {
        Config config_temp = config;
        for (int n = config_temp.priority; n <= NBLOCK; n++)
        {
            if (config_temp.block[config_temp.pos[n - 1].x][config_temp.pos[n - 1].y])
            {
                config_temp.block[config_temp.pos[n - 1].x][config_temp.pos[n - 1].y] = 0;
                for (int j = config_temp.pos[n - 1].y + 1; j < TIER; j++)
                {
                    if (config_temp.block[config_temp.pos[n - 1].x][j])
                    {
                        config.LB1++;
                        config_temp.block[config_temp.pos[n - 1].x][j] = 0;
                    }
                    else
                        break;
                }
            }
        }
    }

    int LB2(Direction dir1, Direction dir2, Config &config)
    {
        Config config_temp = config;
        config_temp.LB2 = 0;
        while (config_temp.priority != NBLOCK)
        {
            while (config_temp.block[config_temp.pos[config_temp.priority - 1].x][config_temp.pos[config_temp.priority - 1].y] == 0)
            {
                if (config_temp.priority == NBLOCK)
                    return config.LB2;
                config_temp.priority++;
            }
            int block1 = count(dir1, config_temp);
            int block2 = count(dir2, config_temp);
            while ((block1 == 0) || (block2 == 0))
            {
                config_temp.block[config_temp.pos[config_temp.priority - 1].x][config_temp.pos[config_temp.priority - 1].y] = 0;
                config_temp.priority++;
                if (config_temp.priority == NBLOCK)
                    return config.LB2;
                block1 = count(dir1, config_temp);
                block2 = count(dir2, config_temp);
            }
            delete_block(dir1, config_temp);
            delete_block(dir2, config_temp);
            config_temp.priority++;
            config.LB2 += (block1 < block2) ? block1 : block2;
        }
        return config.LB2;
    }

    int LB3(Direction dir1, Direction dir2, Direction dir3, Config &config)
    {
        Config config_temp = config;
        config_temp.LB3 = 0;
        while (config_temp.priority != NBLOCK)
        {
            while (config_temp.block[config_temp.pos[config_temp.priority - 1].x][config_temp.pos[config_temp.priority - 1].y] == 0)
            {
                if (config_temp.priority == NBLOCK)
                    return config.LB3;
                config_temp.priority++;
            }
            int block1 = count(dir1, config_temp);
            int block2 = count(dir2, config_temp);
            int block3 = count(dir3, config_temp);
            while ((block1 == 0) || (block2 == 0) || (block3 == 0))
            {
                config_temp.block[config_temp.pos[config_temp.priority - 1].x][config_temp.pos[config_temp.priority - 1].y] = 0;
                config_temp.priority++;
                if (config_temp.priority == NBLOCK)
                    return config.LB3;
                block1 = count(dir1, config_temp);
                block2 = count(dir2, config_temp);
                block3 = count(dir3, config_temp);
            }
            delete_block(dir1, config_temp);
            delete_block(dir2, config_temp);
            delete_block(dir3, config_temp);
            config_temp.priority++;
            config.LB3 += min(min(block1, block2), block3);
        }
        return config.LB3;
    }

    int LB4(Direction dir1, Direction dir2, Direction dir3, Direction dir4, Config &config)
    {
        Config config_temp = config;
        config_temp.LB4 = 0;
        while (config_temp.priority != NBLOCK)
        {
            while (config_temp.block[config_temp.pos[config_temp.priority - 1].x][config_temp.pos[config_temp.priority - 1].y] == 0)
            {
                if (config_temp.priority == NBLOCK)
                    return config.LB4;
                config_temp.priority++;
            }
            int block1 = count(dir1, config_temp);
            int block2 = count(dir2, config_temp);
            int block3 = count(dir3, config_temp);
            int block4 = count(dir4, config_temp);
            while ((block1 == 0) || (block2 == 0) || (block3 == 0) || (block4 == 0))
            {
                config_temp.block[config_temp.pos[config_temp.priority - 1].x][config_temp.pos[config_temp.priority - 1].y] = 0;
                config_temp.priority++;
                if (config_temp.priority == NBLOCK)
                    return config.LB4;
                block1 = count(dir1, config_temp);
                block2 = count(dir2, config_temp);
                block3 = count(dir3, config_temp);
                block4 = count(dir4, config_temp);
            }
            delete_block(dir1, config_temp);
            delete_block(dir2, config_temp);
            delete_block(dir3, config_temp);
            delete_block(dir4, config_temp);
            config.LB4 += min(min(block1, block2), min(block3, block4));
        }
        return config.LB4;
    }

    int UB1()
    {
        Config config_temp = config;
        for (int n = config_temp.priority; n <= NBLOCK; n++)
        {
            // ブロッキングブロックを積み替える場合
            for (int j = TIER - 1; j >= config_temp.pos[n - 1].y + 1; j--)
            {
                // ブロックが配置されていた場合，ブロックを積み替える
                if (config_temp.block[config_temp.pos[n - 1].x][j])
                {
                    config.UB1++;
                    // 最大優先度が最も低いスタックを格納
                    int temp = 0;
                    // 積み替え先
                    int dst;
                    // 積み替え先スタックを決定
                    for (int i = 0; i < STACK; i++)
                    {
                        if ((i != config_temp.pos[n - 1].x) && !config_temp.block[i][TIER - 1] && (temp < config_temp.P_UL[i]))
                        {
                            temp = config_temp.P_UL[i];
                            dst = i;
                        }
                    }
                    Point src = {config_temp.pos[n - 1].x, j};
                    config_temp.relocate(Upp, src, dst);
                }
                config_temp.print();
            }
            Point src = {config_temp.pos[n - 1].x, config_temp.pos[n - 1].y};
            config_temp.retrieve(src);
            config_temp.print();
        }
        return config.UB1;
    }

    // ブロッキングブロックの個数を数える
    int count(Direction dir, Config &config)
    {
        int sum = 0;
        switch (dir)
        {
        case (Left):
            for (int i = config.pos[config.priority - 1].x - 1; i >= 0; i--)
            {
                if (config.block[i][config.pos[config.priority - 1].y])
                    sum++;
            }
            break;
        case (Right):
            for (int i = config.pos[config.priority - 1].x + 1; i < STACK; i++)
            {
                if (config.block[i][config.pos[config.priority - 1].y])
                    sum++;
            }
            break;
        case (Low):
            for (int j = config.pos[config.priority - 1].y - 1; j >= 0; j--)
            {
                if (config.block[config.pos[config.priority - 1].x][j])
                    sum++;
            }
            break;
        case (Upp):
            for (int j = config.pos[config.priority - 1].y + 1; j < TIER; j++)
            {
                if (config.block[config.pos[config.priority - 1].x][j])
                    sum++;
            }
            break;
        default:
            break;
        }
        return sum;
    }

    // ブロッキングブロックを削除
    void delete_block(Direction dir, Config &config)
    {
        switch (dir)
        {
        case (Left):
            for (int i = config.pos[config.priority - 1].x; i >= 0; i--)
            {
                config.block[i][config.pos[config.priority - 1].y] = 0;
            }
            break;
        case (Right):
            for (int i = config.pos[config.priority - 1].x; i < STACK; i++)
            {
                config.block[i][config.pos[config.priority - 1].y] = 0;
            }
            break;
        case (Low):
            for (int j = config.pos[config.priority - 1].y; j >= 0; j--)
            {
                config.block[config.pos[config.priority - 1].x][j] = 0;
            }
            break;
        case (Upp):
            for (int j = config.pos[config.priority - 1].y; j < TIER; j++)
            {
                config.block[config.pos[config.priority - 1].x][j] = 0;
            }
            break;
        default:
            break;
        }
        return;
    }
};
