#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include "instance.h"

using namespace std;

#define BUFFER 256

int main()
{
    Instance instance;
    char filename[BUFFER];
    snprintf(filename, BUFFER, "../Benchmark/%d-%d-%d/%05d.txt", TIER, STACK, NBLOCK, NUMBER);
    instance.readFile(filename);

    Point src = {0, 0};
    Point dst = {0, 2};
    // instance.config.print();
    // instance.config.relocate(src, dst);
    // instance.config.retrieve(dst);
    instance.config.print();
    instance.LB1();
    cout << instance.config.LB1 << endl;
    instance.LB2(Upp, Right, instance.config);
    cout << instance.config.LB2 << endl;
    instance.LB3(Upp, Right, Low, instance.config);
    cout << instance.config.LB3 << endl;
    instance.LB4(Upp, Right, Low, Left, instance.config);
    cout << instance.config.LB4 << endl;
    return 0;
}
