#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include "config.h"

using namespace std;

// 積み替え・取り出し方向
typedef enum
{
    Upp,
    Low,
    Left,
    Right
} Direction;

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
        while (config_temp.block[config_temp.pos[config_temp.priority - 1].x][config_temp.pos[config_temp.priority - 1].y] == 0)
        {
            if (config_temp.priority == NBLOCK)
                return config.LB2;
            config_temp.priority++;
        }
        int block1 = count(dir1, config_temp);
        int block2 = count(dir2, config_temp);
        while ((block1 == 0) && (block2 == 0))
        {
            config_temp.block[config_temp.pos[config_temp.priority - 1].x][config_temp.pos[config_temp.priority - 1].y] = 0;
            config_temp.priority++;
            if (config_temp.priority == NBLOCK)
                return config.LB2;
            block1 = count(dir1, config_temp);
            block2 = count(dir2, config_temp);
        }
        if (block1 < block2)
        {
            delete_block(dir1, config_temp);
            config.LB2 += block1 + LB2(dir1, dir2, config_temp);
        }
        else if (block1 > block2)
        {
            delete_block(dir2, config_temp);
            config.LB2 += block2 + LB2(dir1, dir2, config_temp);
        }
        else
        {
            Config config_temp2 = config_temp;
            delete_block(dir1, config_temp);
            delete_block(dir2, config_temp2);
            config_temp.LB2 = block1 + LB2(dir1, dir2, config_temp);
            config_temp2.LB2 = block2 + LB2(dir1, dir2, config_temp2);
            config.LB2 += (config_temp.LB2 < config_temp2.LB2) ? config_temp.LB2 : config_temp2.LB2;
        }
        return config.LB2;
    }

    int LB3(Direction dir1, Direction dir2, Direction dir3, Config &config)
    {
        Config config_temp = config;
        config_temp.LB3 = 0;
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
        // 取り出し方向とブロッキングブロックの個数をペアとする
        vector<pair<int, Direction> > v;
        v.push_back(make_pair(block1, dir1));
        v.push_back(make_pair(block2, dir2));
        v.push_back(make_pair(block3, dir3));

        // ブロッキングブロックの個数で昇順にソートする
        sort(v.begin(), v.end());
        if (v[0].first < v[1].first)
        {
            delete_block(v[0].second, config_temp);
            config.LB3 += v[0].first + LB3(dir1, dir2, dir3, config_temp);
        }
        else
        {
            if (v[1].first < v[2].first)
            {
                Config config_temp2 = config_temp;
                delete_block(v[0].second, config_temp);
                delete_block(v[1].second, config_temp2);
                config_temp.LB3 = v[0].first + LB3(dir1, dir2, dir3, config_temp);
                config_temp2.LB3 = v[1].first + LB3(dir1, dir2, dir3, config_temp2);
                config.LB3 += (config_temp.LB3 < config_temp2.LB3) ? config_temp.LB3 : config_temp2.LB3;
            }
            else
            {
                Config config_temp2 = config_temp;
                Config config_temp3 = config_temp;
                delete_block(v[0].second, config_temp);
                delete_block(v[1].second, config_temp2);
                delete_block(v[2].second, config_temp2);
                config_temp.LB3 = v[0].first + LB3(dir1, dir2, dir3, config_temp);
                config_temp2.LB3 = v[1].first + LB3(dir1, dir2, dir3, config_temp2);
                config_temp3.LB3 = v[2].first + LB3(dir1, dir2, dir3, config_temp3);
                config.LB3 += min(std::min(config_temp.LB3, config_temp2.LB3), config_temp3.LB3);
            }
        }
        return config.LB3;
    }

    int LB4(Direction dir1, Direction dir2, Direction dir3, Direction dir4, Config &config)
    {
        Config config_temp = config;
        config_temp.LB4 = 0;
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
        // 取り出し方向とブロッキングブロックの個数をペアとする
        vector<pair<int, Direction> > v;
        v.push_back(make_pair(block1, dir1));
        v.push_back(make_pair(block2, dir2));
        v.push_back(make_pair(block3, dir3));
        v.push_back(make_pair(block4, dir4));

        // ブロッキングブロックの個数で昇順にソートする
        sort(v.begin(), v.end());
        if (v[0].first < v[1].first)
        {
            delete_block(v[0].second, config_temp);
            config.LB4 += v[0].first + LB4(dir1, dir2, dir3, dir4, config_temp);
        }
        else
        {
            if (v[1].first < v[2].first)
            {
                Config config_temp2 = config_temp;
                delete_block(v[0].second, config_temp);
                delete_block(v[1].second, config_temp2);
                config_temp.LB4 = v[0].first + LB4(dir1, dir2, dir3, dir4, config_temp);
                config_temp2.LB4 = v[1].first + LB4(dir1, dir2, dir3, dir4, config_temp2);
                config.LB4 += (config_temp.LB4 < config_temp2.LB4) ? config_temp.LB4 : config_temp2.LB4;
            }
            else
            {
                if (v[2].first < v[3].first)
                {
                    Config config_temp2 = config_temp;
                    Config config_temp3 = config_temp;
                    delete_block(v[0].second, config_temp);
                    delete_block(v[1].second, config_temp2);
                    delete_block(v[2].second, config_temp3);
                    config_temp.LB4 = v[0].first + LB4(dir1, dir2, dir3, dir4, config_temp);
                    config_temp2.LB4 = v[1].first + LB4(dir1, dir2, dir3, dir4, config_temp2);
                    config_temp3.LB4 = v[2].first + LB4(dir1, dir2, dir3, dir4, config_temp3);
                    config.LB4 += min(std::min(config_temp.LB4, config_temp2.LB4), config_temp3.LB4);
                }
                else
                {
                    Config config_temp2 = config_temp;
                    Config config_temp3 = config_temp;
                    Config config_temp4 = config_temp;
                    delete_block(v[0].second, config_temp);
                    delete_block(v[1].second, config_temp2);
                    delete_block(v[2].second, config_temp3);
                    delete_block(v[3].second, config_temp4);
                    config_temp.LB4 = v[0].first + LB4(dir1, dir2, dir3, dir4, config_temp);
                    config_temp2.LB4 = v[1].first + LB4(dir1, dir2, dir3, dir4, config_temp2);
                    config_temp3.LB4 = v[2].first + LB4(dir1, dir2, dir3, dir4, config_temp3);
                    config_temp4.LB4 = v[3].first + LB4(dir1, dir2, dir3, dir4, config_temp4);
                    config.LB4 += min(min(config_temp.LB4, config_temp2.LB4), min(config_temp3.LB4, config_temp4.LB4));
                }
            }
        }
        return config.LB4;
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
        config.priority++;
    }
};
