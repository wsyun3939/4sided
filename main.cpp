#include <bits/stdc++.h>
#include <fstream>
#include <sstream>
#include <vector>
#include <time.h>
#include "instance.h"
#include "bb.cpp"

using namespace std;

#define BUFFER 256

int main()
{
    // 計算時間を測る
    clock_t start, calc_time;
    clock_t max = 0;
    clock_t sol_lapse = 0;
    // 30分以内に解けなかった例題数
    int timeup = 0;
    // 積み替え回数の総和
    int sum = 0;
    // 最適解と下界値の差
    int gap = 0;
    int temp;
    int nblock = NBLOCK;
    FILE *fp_csv = NULL;
    for (int a = NUMBER; a < NUMBER + 100 * TIER; a++)
    {
        start = clock();
        Instance *instance = new Instance();
        char filename[BUFFER];
        snprintf(filename, BUFFER, "../Benchmark/%d-%d-%d/%05d.txt", TIER, STACK, nblock, a);
        instance->readFile(filename);
        cout << filename << endl;
        instance->config.print();
        instance->config.LB1 = instance->LB1();
        cout << "LB1=" << instance->config.LB1 << endl;
        instance->config.LB2 = instance->LB2(Upp, Right, Any);
        cout << "LB2=" << instance->config.LB2 << endl;
        instance->config.LB2a = instance->LB2a(Any);
        cout << "LB2a=" << instance->config.LB2a << endl;
        instance->config.LB3 = instance->LB3(Upp, Right, Low, Any);
        cout << "LB3=" << instance->config.LB3 << endl;
        instance->config.LB4 = instance->LB4(Upp, Right, Low, Left, Any);
        cout << "LB4=" << instance->config.LB4 << endl;
        instance->config.UB1 = instance->UB1();
        instance->config.UB2 = instance->UB2(Upp, Right);
        instance->config.UB3 = instance->UB3(Upp, Right, Low);
        instance->config.UB4 = instance->UB4(Upp, Right, Low, Left);
        cout << "UB1=" << instance->config.UB1 << endl;
        cout << "UB2=" << instance->config.UB2 << endl;
        cout << "UB3=" << instance->config.UB3 << endl;
        cout << "UB4=" << instance->config.UB4 << endl;

#if BB == 1
        temp = bb1(*instance, instance->config.LB1, start);
        cout << temp << endl;
        if (a % 100 == 1)
        {
            sprintf(filename, "../Benchmark/%d-%d-%d(bb1).csv", TIER, STACK, nblock);
            fp_csv = fopen(filename, "w");
        }
#endif

#if BB == 2
        temp = bb2(*instance, instance->config.LB2, Any, start);
        cout << temp << endl;
        if (a % 100 == 1)
        {
            sprintf(filename, "../Benchmark/%d-%d-%d(bb2).csv", TIER, STACK, nblock);
            fp_csv = fopen(filename, "w");
        }
#endif

#if BB == -2
        temp = bb2a(*instance, instance->config.LB2a, Any, start);
        cout << temp << endl;
        if (a % 100 == 1)
        {
            sprintf(filename, "../Benchmark/%d-%d-%d(bb2a).csv", TIER, STACK, nblock);
            fp_csv = fopen(filename, "w");
        }
#endif

#if BB == 3
        temp = bb3(*instance, instance->config.LB3, Any, start);
        cout << temp << endl;
        if (a % 100 == 1)
        {
            sprintf(filename, "../Benchmark/%d-%d-%d(bb3).csv", TIER, STACK, nblock);
            fp_csv = fopen(filename, "w");
        }
#endif

#if BB == 4
        temp = bb4(*instance, instance->config.LB4, Any, start);
        cout << temp << endl;
        if (a % 100 == 1)
        {
            sprintf(filename, "../Benchmark/%d-%d-%d(bb4).csv", TIER, STACK, nblock);
            fp_csv = fopen(filename, "w");
        }
#endif

        if (temp == -1)
        {
            timeup++;
        }
        else
        {
            calc_time = clock() - start;
            sol_lapse += calc_time;
            if (max < calc_time)
            {
                max = calc_time;
            }
            sum += temp;
        }
        fprintf(fp_csv, "%d\n", temp);
        delete instance;
        if (a % 100 == 0)
        {
            nblock++;
            fclose(fp_csv);
        }
    }
    cout << "平均積み替え変え回数：" << (double)sum / (100 * TIER) << endl;
    return 0;
}
