#include <iostream>
#include <queue>

using namespace std;

int n;
int grpcnt;             // number of groups
int dir[4][2] = {{1, 0}, {-1, 0}, {0, 1}, {0, -1}};
int board[30][30];
int grp[30][30];        // 어느 그룹?

struct Node {
    int row, col, num;
};
queue<Node> Q;
int visited[30][30];

void mark_group() {
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            visited[i][j] = 0;
        }
    }
    
    grpcnt = 0;
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            if (visited[i][j]) continue;

            Q.push({i, j, board[i][j]});
            visited[i][j] = 1;
            grp[i][j] = grpcnt;
            while (!Q.empty()) {
                Node cur = Q.front(); Q.pop();

                for (int d = 0; d < 4; d++) {
                    int newR = cur.row + dir[d][0];
                    int newC = cur.col + dir[d][1];

                    if (newR < 0 || newR >= n || newC < 0 || newC >= n) continue;   //OOR
                    if (visited[newR][newC]) continue;
                    if (cur.num != board[newR][newC]) continue;     //wrong color

                    Q.push({newR, newC, board[newR][newC]});
                    visited[newR][newC] = 1;
                    grp[newR][newC] = grpcnt;
                }
            }
            grpcnt++;
        }
    }
}

// 그룹마다의 조화 점수
int group_harmony(int grpA, int grpB) {
    int a_num = 0, b_num = 0, a_col, b_col, num_sides = 0;

    // calc the block # and color of each group
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            if (grp[i][j] == grpA) {
                a_col = board[i][j];
                a_num++;
            }
            if (grp[i][j] == grpB) {
                b_col = board[i][j];
                b_num++;
            }
        }
    }

    // calc the number of sides the groups share
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            if (grp[i][j] == grpA) {
                for (int d = 0; d < 4; d++) {
                    int newR = i + dir[d][0];
                    int newC = j + dir[d][1];

                    if (newR < 0 || newR >= n || newC < 0 || newC >= n) continue;   //OOR
                    if (grp[newR][newC] == grpB) num_sides++;
                }
            }
        }
    }
    //cout << a_num << " " << b_num << " " << a_col << " " << b_col << " " << num_sides << '\n';
    return (a_num + b_num) * a_col * b_col * num_sides;
}

int calc_art_point() {
    int sum = 0;
    for (int i = 0; i < grpcnt; i++) {
        for (int j = i + 1; j < grpcnt; j++) {
            sum += group_harmony(i, j);
        }
    }
    return sum;
}

// rotate subsquares 90 deg clockwise given start pos (startI, startJ)
void rotate_square(int startI, int startJ) {
    int size = n / 2;
    vector<vector<int>> tmp;

    tmp.resize(size);
    for (int i = 0; i < size; i++) {
        tmp[i].resize(size);
    }

    // [i][j] goes to [j][size - i - 1]
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            tmp[j][size - i - 1] = board[startI + i][startJ + j];
        }
    }

    // copy back to board
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            board[startI + i][startJ + j] = tmp[i][j];
        }
    }
}

void rotate() {
    int mid = n / 2;
    vector<int> tmp(n);

    // 90 deg ccw rotation on the cross part
    for (int i = 0; i < n; i++) {
        tmp[i] = board[mid][i];     // save horizontal line
    }
    for (int i = 0; i < n; i++) {
        board[mid][i] = board[i][mid];      // horizontal <- vertical
        board[i][mid] = tmp[n - i - 1];     // horizontal -> vertical
    }

    // rotate each of the subsquares
    rotate_square(0, 0);
    rotate_square(mid + 1, 0);
    rotate_square(0, mid + 1);
    rotate_square(mid + 1, mid + 1);
}

int main() {
    ios_base::sync_with_stdio(false);
    cin.tie(0); cout.tie(0);

    cin >> n;
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            cin >> board[i][j];
        }
    }

    int points_sum = 0;

    /*for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            cout << grp[i][j] << " ";
        }
        cout << '\n';
    }
    cout << '\n';
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            cout << board[i][j] << " ";
        }
        cout << '\n';
    }*/

    for (int r = 0; r < 4; r++) {
        mark_group();                       // mark the grops
        points_sum += calc_art_point();     // calculate point
        // cout << points_sum << '\n';
        rotate();                           // rotate
    }

    cout << points_sum << '\n';

    return 0;
}

// 예술 점수 계산 로직
    // 그룹의 구분
    // 맞닿은 변의 수 : 옆에 있으면 맞닿음. 
// 회전 로직

