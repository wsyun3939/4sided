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
    Instance instance;
    char filename[BUFFER];
    snprintf(filename, BUFFER, "../Benchmark/%d-%d-%d/%05d.txt", TIER, STACK, NBLOCK, NUMBER);
    instance.readFile(filename);

    instance.config.print();
    instance.config.LB1 = instance.LB1();
    cout << instance.config.LB1 << endl;
    instance.config.LB2 = instance.LB2(Upp, Right);
    cout << instance.config.LB2 << endl;
    instance.config.LB3 = instance.LB3(Upp, Right, Low);
    cout << instance.config.LB3 << endl;
    instance.config.LB4 = instance.LB4(Upp, Right, Low, Left);
    cout << instance.config.LB4 << endl;
    instance.config.UB1 = instance.UB1();
    instance.config.UB2 = instance.UB2(Upp, Right);
    instance.config.UB3 = instance.UB3(Upp, Right, Low);
    instance.config.UB4 = instance.UB4(Upp, Right, Low, Left);
    cout << "UB1=" << instance.config.UB1 << endl;
    cout << "UB2=" << instance.config.UB2 << endl;
    cout << "UB3=" << instance.config.UB3 << endl;
    cout << "UB4=" << instance.config.UB4 << endl;
    cout << bb1(instance, instance.config.LB1) << endl;
    return 0;
}
