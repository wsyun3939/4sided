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

// 積み替え・取り出し方向
typedef enum
{
    Upp,
    Low,
    Left,
    Right
} Direction;

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

    // 上界値
    int UB1, UB2, UB3, UB4;

    // 各方向における最大優先度
    int P_UL[STACK];
    int P_LR[TIER];
    int P[STACK][TIER];

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

        // 各方向における最大優先度を100で初期化
        for (int i = 0; i < STACK; i++)
        {
            P_UL[i] = 100;
        }
        for (int j = 0; j < TIER; j++)
        {
            P_LR[j] = 100;
        }

        // ターゲットブロックの優先順位を１で初期化
        priority = 1;

        // 下界値を０で初期化
        LB1 = LB2 = LB3 = LB4 = 0;
    }

    // ブロック積み替え
    void
    relocate(Direction dir, Point src, int dst)
    {
        switch (dir)
        {
        case Upp:
            for (int j = TIER - 1; j >= 0; j--)
            {
                // ブロックを下まで押し込む
                if (block[dst][j])
                {
                    block[dst][j + 1] = block[src.x][src.y];
                    pos[block[dst][j + 1]].x = dst;
                    pos[block[dst][j + 1]].y = j + 1;
                }
            }
            break;
        case Low:
            for (int j = 0; j < TIER; j++)
            {
                // ブロックを上まで押し込む
                if (block[dst][j])
                {
                    block[dst][j - 1] = block[src.x][src.y];
                    pos[block[dst][j - 1]].x = dst;
                    pos[block[dst][j - 1]].y = j - 1;
                }
            }
            break;
        case Left:
            for (int i = 0; i < STACK; i++)
            {
                // ブロックを右まで押し込む
                if (block[i][dst])
                {
                    block[i - 1][dst] = block[src.x][src.y];
                    pos[block[i - 1][dst]].x = i - 1;
                    pos[block[i - 1][dst]].y = dst;
                }
            }
            break;
        case Right:
            for (int i = STACK - 1; i >= 0; i--)
            {
                // ブロックを右まで押し込む
                if (block[i][dst])
                {
                    block[i + 1][dst] = block[src.x][src.y];
                    pos[block[i + 1][dst]].x = i + 1;
                    pos[block[i + 1][dst]].y = dst;
                }
            }
            break;
        }
        if (P_LR[src.y] == block[src.x][src.y])
        {
            P_LR[src.y] = 100;
            for (int i = 0; i < STACK; i++)
            {
                if (P_LR[src.y] > block[i][src.y])
                    P_LR[src.y] = block[i][src.y];
            }
        }
        if (P_LR[pos[block[src.x][src.y]].y] > block[src.x][src.y])
            P_LR[pos[block[src.x][src.y]].y] = block[src.x][src.y];
        if (P_UL[src.x] == block[src.x][src.y])
        {
            P_UL[src.x] = 100;
            for (int j = 0; j < TIER; j++)
            {
                if (P_UL[src.x] > block[src.x][j])
                    P_UL[src.x] = block[src.x][j];
            }
        }
        if (P_UL[pos[block[src.x][src.y]].x] > block[src.x][src.y])
            P_UL[pos[block[src.x][src.y]].x] = block[src.x][src.y];
        block[src.x][src.y] = 0;
    }

    // ブロック取り出し
    void retrieve(Point src)
    {
        if (P_LR[src.y] == block[src.x][src.y])
        {
            P_LR[src.y] = 100;
            for (int i = 0; i < STACK; i++)
            {
                if (P_LR[src.y] > block[i][src.y])
                    P_LR[src.y] = block[i][src.y];
            }
        }
        if (P_UL[src.x] == block[src.x][src.y])
        {
            P_UL[src.x] = 100;
            for (int j = 0; j < TIER; j++)
            {
                if (P_UL[src.x] > block[src.x][j])
                    P_UL[src.x] = block[src.x][j];
            }
        }
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
        cout << endl;
    }
};
