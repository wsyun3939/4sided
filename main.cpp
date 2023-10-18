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
    int T = 0;
    FILE *fp_csv = NULL;
    FILE *fp_csv2 = NULL;
    for (int a = NUMBER; a < NUMBER + 100 * TIER; a++)
    {
        start = clock();
        Instance *instance = new Instance();
        char filename[BUFFER];
        snprintf(filename, BUFFER, "../Benchmark/%d-%d-%d/%05d.txt", TIER, STACK, nblock, a);
        instance->readFile(filename);
        cout << filename << endl;
        instance->config.print();
        T = 0;

#if BB == 1
        instance->config.LB1 = instance->LB1();
        cout << "LB1=" << instance->config.LB1 << endl;
        instance->config.UB1 = instance->UB1(instance->config.UBT);
        cout << "UB1=" << instance->config.UB1 << endl;
#endif

#if BB == 2
        instance->config.LB2 = instance->LB2(Upp, Right, Any);
        cout << "LB2=" << instance->config.LB2 << endl;
        instance->config.UB2 = instance->UB2(Upp, Right, instance->config.UBT);
        cout << "UB2=" << instance->config.UB2 << endl;
#endif

#if BB == -2
        instance->config.LB2a = instance->LB2a(Any);
        cout << "LB2a=" << instance->config.LB2a << endl;
        instance->config.UB2 = instance->UB2(Upp, Low, instance->config.UBT);
        cout << "UB2=" << instance->config.UB2 << endl;
#endif

#if BB == 3
        instance->config.LB3 = instance->LB3(Upp, Right, Low, Any);
        cout << "LB3=" << instance->config.LB3 << endl;
        instance->config.UB3 = instance->UB3(Upp, Right, Low, instance->config.UBT);
        cout << "UB3=" << instance->config.UB3 << endl;
#endif

#if BB == 4
        instance->config.LB4 = instance->LB4(Upp, Right, Low, Left, Any);
        cout << "LB4=" << instance->config.LB4 << endl;
        instance->config.UB4 = instance->UB4(Upp, Right, Low, Left, instance->config.UBT);
        cout << "UB4=" << instance->config.UB4 << endl;
#endif

        T = 0;

#if BB == 1
        temp = bb1(*instance, instance->config.LB1, start, T, 0);
        cout << temp << endl;
        cout << "T=" << T << endl;
        // getchar();
        if (a % 100 == 1)
        {
            sprintf(filename, "../Benchmark/%d-%d-%d(bb1_unres).csv", TIER, STACK, nblock);
            fp_csv = fopen(filename, "w");
            sprintf(filename, "../Benchmark/%d-%d-%d(ip1_unresT).csv", TIER, STACK, nblock);
            fp_csv2 = fopen(filename, "w");
        }
#endif

#if BB == 2
        temp = bb2(*instance, instance->config.LB2, instance->config.LB2, start, T, 0);
        cout << temp << endl;
        cout << "T=" << T << endl;
        // getchar();
        if (a % 100 == 1)
        {
            sprintf(filename, "../Benchmark/%d-%d-%d(bb2_unres).csv", TIER, STACK, nblock);
            fp_csv = fopen(filename, "w");
            sprintf(filename, "../Benchmark/%d-%d-%d(ip2_unresT).csv", TIER, STACK, nblock);
            fp_csv2 = fopen(filename, "w");
        }
#endif

#if BB == -2
        temp = bb2a(*instance, instance->config.LB2a, instance->config.LB2a, start, T, 0);
        cout << temp << endl;
        cout << "T=" << T << endl;
        // getchar();
        if (a % 100 == 1)
        {
            sprintf(filename, "../Benchmark/%d-%d-%d(bb2s_unres).csv", TIER, STACK, nblock);
            fp_csv = fopen(filename, "w");
            sprintf(filename, "../Benchmark/%d-%d-%d(ip2s_unresT).csv", TIER, STACK, nblock);
            fp_csv2 = fopen(filename, "w");
        }
#endif

#if BB == 3
        temp = bb3(*instance, instance->config.LB3, instance->config.LB3, start, T, 0);
        cout << temp << endl;
        cout << "T=" << T << endl;
        // getchar();
        if (a % 100 == 1)
        {
            sprintf(filename, "../Benchmark/%d-%d-%d(bb3_unres).csv", TIER, STACK, nblock);
            fp_csv = fopen(filename, "w");
            sprintf(filename, "../Benchmark/%d-%d-%d(ip3_unresT).csv", TIER, STACK, nblock);
            fp_csv2 = fopen(filename, "w");
        }
#endif

#if BB == 4
        temp = bb4(*instance, instance->config.LB4, instance->config.LB4, start, T, 0);
        cout << temp << endl;
        cout << "T=" << T << endl;
        // getchar();
        if (a % 100 == 1)
        {
            sprintf(filename, "../Benchmark/%d-%d-%d(bb4_unres).csv", TIER, STACK, nblock);
            fp_csv = fopen(filename, "w");
            sprintf(filename, "../Benchmark/%d-%d-%d(ip4_unresT).csv", TIER, STACK, nblock);
            fp_csv2 = fopen(filename, "w");
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
        fprintf(fp_csv2, "%d\n", T);
        delete instance;
        if (a % 100 == 0)
        {
            nblock++;
            fclose(fp_csv);
        }
    }
    cout << "平均積み替え変え回数：" << (double)sum / (100 * TIER) << endl;
    cout << "infeasible:" << timeup << endl;
    cout << "最大計算時間:" << (double)max / CLOCKS_PER_SEC << endl;
    cout << "計算時間の平均:" << (double)sol_lapse / ((100 * TIER - timeup) * CLOCKS_PER_SEC) << endl;
    return 0;
}
