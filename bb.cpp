#include <iostream>
#include "config.h"
#include "instance.h"

using namespace std;

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
        if (instance.config.priority == NUMBER)
        {
            min_rel = depth;
            depth = 0;
            return min_rel;
        }
        block1 = instance.config.count(Upp);
    }
    Config config_temp = instance.config;
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
            Point dst = {0, TIER - 1};
            // 積み替え先スタックを決定
            for (int i = 0; i < STACK; i++)
            {
                if ((i != config_temp.pos[n - 1].x) && !config_temp.block[i][TIER - 1] && (temp < config_temp.P_UL[i]))
                {
                    temp = config_temp.P_UL[i];
                    dst.x = i;
                }
            }
            while (!config_temp.block[dst.x][dst.y - 1] && (dst.y > 0))
                dst.y--;
            Point src = {config_temp.pos[n - 1].x, j};
            config_temp.relocate(src, dst);
        }
    }
    Point src = {config_temp.pos[n - 1].x, config_temp.pos[n - 1].y};
    config_temp.retrieve(src);
    return config.UB1;
}