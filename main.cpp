#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include "instance.h"
#include "bb.cpp"

using namespace std;

#define BUFFER 256

int main()
{
    // 積み替え回数の総和
    int sum = 0;
    // 最適解と下界値の差
    int gap = 0;
    int temp;
    int nblock = NBLOCK;
    FILE *fp_csv = NULL;
    for (int a = NUMBER; a < NUMBER + 100 * TIER; a++)
    {
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

        // temp = bb1(*instance, instance->config.LB1);
        // cout << temp << endl;

        temp = bb2a(*instance, instance->config.LB2, Any);
        cout << temp << endl;
        if (a % 100 == 1)
        {
            sprintf(filename, "../Benchmark/%d-%d-%d(bb2a).csv", TIER, STACK, nblock);
            fp_csv = fopen(filename, "w");
        }
        fprintf(fp_csv, "%d\n", temp);
        sum += temp;
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
