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

        // 上界値を0で初期化
        UB1 = UB2 = UB3 = UB4 = 0;
    }

    // // ブロック積み替え
    // void
    // relocate(Direction dir, Point src, int dst)
    // {
    //     int p = block[src.x][src.y];
    //     block[src.x][src.y] = 0;
    //     switch (dir)
    //     {
    //     case Upp:
    //         if (P_UL[dst] == 100)
    //         {
    //             block[dst][0] = p;
    //             pos[p - 1].x = dst;
    //             pos[p - 1].y = 0;
    //             break;
    //         }
    //         for (int j = TIER - 1; j >= 0; j--)
    //         {
    //             // ブロックを下まで押し込む
    //             if (block[dst][j])
    //             {
    //                 block[dst][j + 1] = p;
    //                 pos[p - 1].x = dst;
    //                 pos[p - 1].y = j + 1;
    //                 break;
    //             }
    //         }
    //         break;
    //     case Low:
    //         if (P_UL[dst] == 100)
    //         {
    //             block[dst][TIER - 1] = p;
    //             pos[p - 1].x = dst;
    //             pos[p - 1].y = TIER - 1;
    //             break;
    //         }
    //         for (int j = 0; j < TIER; j++)
    //         {
    //             // ブロックを上まで押し込む
    //             if (block[dst][j])
    //             {
    //                 block[dst][j - 1] = p;
    //                 pos[p - 1].x = dst;
    //                 pos[p - 1].y = j - 1;
    //                 break;
    //             }
    //         }
    //         break;
    //     case Left:
    //         if (P_LR[dst] == 100)
    //         {
    //             block[STACK - 1][dst] = p;
    //             pos[p - 1].x = STACK - 1;
    //             pos[p - 1].y = dst;
    //             break;
    //         }
    //         for (int i = 0; i < STACK; i++)
    //         {
    //             // ブロックを右まで押し込む
    //             if (block[i][dst])
    //             {
    //                 block[i - 1][dst] = p;
    //                 pos[p - 1].x = i - 1;
    //                 pos[p - 1].y = dst;
    //                 break;
    //             }
    //         }
    //         break;
    //     case Right:
    //         if (P_LR[dst] == 100)
    //         {
    //             block[0][dst] = p;
    //             pos[p - 1].x = 0;
    //             pos[p - 1].y = dst;
    //             break;
    //         }
    //         for (int i = STACK - 1; i >= 0; i--)
    //         {
    //             // ブロックを右まで押し込む
    //             if (block[i][dst])
    //             {
    //                 block[i + 1][dst] = p;
    //                 pos[p - 1].x = i + 1;
    //                 pos[p - 1].y = dst;
    //                 break;
    //             }
    //         }
    //         break;
    //     }

    //     // 最大優先度の更新
    //     // 横方向
    //     if (P_LR[src.y] == p)
    //     {
    //         P_LR[src.y] = 100;
    //         for (int i = 0; i < STACK; i++)
    //         {
    //             if (!block[i][src.y])
    //                 continue;
    //             if (P_LR[src.y] > block[i][src.y])
    //                 P_LR[src.y] = block[i][src.y];
    //         }
    //         for (int i = 0; i < STACK; i++)
    //         {
    //             P[i][src.y] = P_UL[i] + P_LR[src.y];
    //         }
    //     }
    //     if (P_LR[pos[p - 1].y] > p)
    //     {
    //         P_LR[pos[p - 1].y] = p;
    //         for (int i = 0; i < STACK; i++)
    //         {
    //             P[i][pos[p - 1].y] = P_UL[i] + P_LR[pos[p - 1].y];
    //         }
    //     }
    //     // 縦方向
    //     if (P_UL[src.x] == p)
    //     {
    //         P_UL[src.x] = 100;
    //         for (int j = 0; j < TIER; j++)
    //         {
    //             if (!block[src.x][j])
    //                 continue;
    //             if (P_UL[src.x] > block[src.x][j])
    //                 P_UL[src.x] = block[src.x][j];
    //         }
    //         for (int j = 0; j < TIER; j++)
    //         {
    //             P[src.x][j] = P_UL[src.x] + P_LR[j];
    //         }
    //     }
    //     else if (P_UL[pos[p - 1].x] > p)
    //     {
    //         P_UL[pos[p - 1].x] = p;
    //         for (int j = 0; j < TIER; j++)
    //         {
    //             P[src.x][j] = P_UL[pos[p - 1].x] + P_LR[j];
    //         }
    //     }
    // }

    // ブロック積み替え
    void
    relocate(Point src, Point dst)
    {
        int p = block[src.x][src.y];
        block[src.x][src.y] = 0;
        block[dst.x][dst.y] = p;
        pos[p - 1] = dst;

        // 最大優先度の更新
        // 横方向
        if (P_LR[src.y] == p)
        {
            P_LR[src.y] = 100;
            for (int i = 0; i < STACK; i++)
            {
                if (!block[i][src.y])
                    continue;
                if (P_LR[src.y] > block[i][src.y])
                    P_LR[src.y] = block[i][src.y];
            }
            for (int i = 0; i < STACK; i++)
            {
                P[i][src.y] = P_UL[i] + P_LR[src.y];
            }
        }
        if (P_LR[pos[p - 1].y] > p)
        {
            P_LR[pos[p - 1].y] = p;
            for (int i = 0; i < STACK; i++)
            {
                P[i][pos[p - 1].y] = P_UL[i] + P_LR[pos[p - 1].y];
            }
        }
        // 縦方向
        if (P_UL[src.x] == p)
        {
            P_UL[src.x] = 100;
            for (int j = 0; j < TIER; j++)
            {
                if (!block[src.x][j])
                    continue;
                if (P_UL[src.x] > block[src.x][j])
                    P_UL[src.x] = block[src.x][j];
            }
            for (int j = 0; j < TIER; j++)
            {
                P[src.x][j] = P_UL[src.x] + P_LR[j];
            }
        }
        else if (P_UL[pos[p - 1].x] > p)
        {
            P_UL[pos[p - 1].x] = p;
            for (int j = 0; j < TIER; j++)
            {
                P[src.x][j] = P_UL[pos[p - 1].x] + P_LR[j];
            }
        }
    }

    // ブロック取り出し
    void retrieve(Point src)
    {
        int p = block[src.x][src.y];
        block[src.x][src.y] = 0;
        // 横方向
        if (P_LR[src.y] == p)
        {
            P_LR[src.y] = 100;
            for (int i = 0; i < STACK; i++)
            {
                if (!block[i][src.y])
                    continue;
                if (P_LR[src.y] > block[i][src.y])
                    P_LR[src.y] = block[i][src.y];
            }
            for (int i = 0; i < STACK; i++)
            {
                P[i][src.y] = P_UL[i] + P_LR[src.y];
            }
        }
        // 縦方向
        if (P_UL[src.x] == p)
        {
            P_UL[src.x] = 100;
            for (int j = 0; j < TIER; j++)
            {
                if (!block[src.x][j])
                    continue;
                if (P_UL[src.x] > block[src.x][j])
                    P_UL[src.x] = block[src.x][j];
            }
            for (int j = 0; j < TIER; j++)
            {
                P[src.x][j] = P_UL[src.x] + P_LR[j];
            }
        }

        priority++;
    }

    // ブロッキングブロックの個数を数える
    int count(Direction dir)
    {
        int sum = 0;
        switch (dir)
        {
        case (Left):
            for (int i = pos[priority - 1].x - 1; i >= 0; i--)
            {
                if (block[i][pos[priority - 1].y])
                    sum++;
            }
            break;
        case (Right):
            for (int i = pos[priority - 1].x + 1; i < STACK; i++)
            {
                if (block[i][pos[priority - 1].y])
                    sum++;
            }
            break;
        case (Low):
            for (int j = pos[priority - 1].y - 1; j >= 0; j--)
            {
                if (block[pos[priority - 1].x][j])
                    sum++;
            }
            break;
        case (Upp):
            for (int j = pos[priority - 1].y + 1; j < TIER; j++)
            {
                if (block[pos[priority - 1].x][j])
                    sum++;
            }
            break;
        default:
            break;
        }
        return sum;
    }

    // ブロッキングブロックを削除
    void delete_block(Direction dir)
    {
        switch (dir)
        {
        case (Left):
            for (int i = pos[priority - 1].x; i >= 0; i--)
            {
                block[i][pos[priority - 1].y] = 0;
            }
            break;
        case (Right):
            for (int i = pos[priority - 1].x; i < STACK; i++)
            {
                block[i][pos[priority - 1].y] = 0;
            }
            break;
        case (Low):
            for (int j = pos[priority - 1].y; j >= 0; j--)
            {
                block[pos[priority - 1].x][j] = 0;
            }
            break;
        case (Upp):
            for (int j = pos[priority - 1].y; j < TIER; j++)
            {
                block[pos[priority - 1].x][j] = 0;
            }
            break;
        default:
            break;
        }
        return;
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
