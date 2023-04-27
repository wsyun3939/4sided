#include <iostream>

#define TIER 3
#define STACK 6
#define NBLOCK 15
#define NUMBER 1

using namespace std;

// 二次元座標を表す
struct Point
{
    int x;
    int y;
};

class Config
{
public:
    // ブロック配置
    int block[STACK][TIER];

    // 各優先順位に対するブロックの位置
    Point pos[NBLOCK];

    // ターゲットブロックの優先順位
    int priority;

    // 下界値
    int LB1, LB2, LB3, LB4;

    // コンストラクタ
    Config()
    {
        // メンバ配列 config を 0 で初期化する
        for (int i = 0; i < TIER; i++)
        {
            for (int j = 0; j < STACK; j++)
            {
                block[i][j] = 0;
            }
        }

        // ターゲットブロックの優先順位を１で初期化
        priority = 1;

        // 下界値を０で初期化
        LB1 = LB2 = LB3 = LB4 = 0;
    }

    // ブロック積み替え
    void
    relocate(Point src, Point dst)
    {
        block[dst.x][dst.y] = block[src.x][src.y];
        block[src.x][src.y] = 0;
    }

    // ブロック取り出し
    void retrieve(Point src)
    {
        block[src.x][src.y] = 0;
        priority++;
    }

    // ブロック配置を出力
    void print()
    {
        for (int j = TIER - 1; j >= 0; j--)
        {
            for (int i = 0; i < STACK; i++)
            {
                cout << setw(2) << right << block[i][j] << " ";
            }
            cout << endl;
        }
    }
};
