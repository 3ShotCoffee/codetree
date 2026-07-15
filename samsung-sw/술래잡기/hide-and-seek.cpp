#include <iostream>
#include <vector>
#include <algorithm>
#include <cmath>
using namespace std;

int n, m, h, k;
int soolR, soolC;

vector<int> board[100][100];        // 0/1/2/3
vector<int> board_next[100][100];
int tree[100][100];                 // 0 empty / 1 tree
int dir[4][2] = {{0, 1}, {1, 0}, {0, -1}, {-1, 0}};     // 오른쪽 아래쪽 왼족 위쪽

int main() {
    cin >> n >> m >> h >> k;

    for (int i = 0; i < m; i++) {
        int x, y, d; cin >> x >> y >> d;
        board[x - 1][y - 1].push_back(d - 1);
    }

    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            tree[i][j] = 0;
        }
    }
    for (int i = 0; i < h; i++) {
        int x, y; cin >> x >> y;
        tree[x - 1][y - 1] = 1;
    }

    // 나가는 방향 + 들어오는 방향의 이동 방향을 만들어 놓기
    vector<int> soolD, tmp;
    int direction = 3;
    for (int i = 0; i < 2 * n - 2; i++) {
        for (int j = 0; j < i / 2 + 1; j++) {   // 이만큼을 같은 방향으로 움직임
            soolD.push_back(direction);
        }
        direction = (direction + 1) % 4;
    }
    for (int j = 0; j < n - 1; j++) {
        soolD.push_back(direction);
    }

    tmp = soolD; 
    reverse(tmp.begin(), tmp.end());

    for (int direction : tmp) soolD.push_back((direction + 2) % 4);

    //cout << soolD.size() << '\n';
    //for (int i : soolD) cout << i << " ";
    //cout << '\n';

    int points = 0;
    soolR = n / 2; soolC = soolR;
    for (int turn = 0 ; turn < k; turn++) {
        // 도망자들의 턴
        for (int i = 0; i < n; i++) {
            for (int j = 0; j < n; j++) {
                board_next[i][j].clear();
            }
        }
        for (int i = 0; i < n; i++) {
            for (int j = 0; j < n; j++) {
                // i, j에 위치한 모든 도망자들에 대해
                if (abs(i - soolR) + abs(j - soolC) > 3) {      // 술래와의 거리가 3보다 크면 그대로 더함
                    board_next[i][j].insert(board_next[i][j].end(), board[i][j].begin(), board[i][j].end()); 
                    continue;
                }

                for (int d : board[i][j]) {
                    int newR = i + dir[d][0];
                    int newC = j + dir[d][1];
                    
                    if (newR < 0 || newR >= n || newC < 0 || newC >= n) {
                        d = (d + 2) % 4;    // 반대방향으로 틀어줌
                        newR = i + dir[d][0];
                        newC = j + dir[d][1];
                    }

                    if (newR == soolR && newC == soolC) {   // 술래가 있는 경우 움직이지 않음
                        board_next[i][j].push_back(d);
                    }
                    else {
                        board_next[newR][newC].push_back(d);
                    }
                }
            }
        }
        for (int i = 0; i < n; i++) {
            for (int j = 0; j < n; j++) {
                board[i][j] = board_next[i][j];             // 다시 board로 복사
            }
        }

        /*for (int i = 0; i < n; i++) {
            for (int j = 0; j < n; j++) {
                cout << board[i][j].size() << " ";
            }
            cout << '\n';
        }*/

        // 술래의 턴
        int way = soolD[turn % soolD.size()];               // 현재 술래가 향하는 방향
        soolR += dir[way][0]; soolC += dir[way][1];         // 방향대로 한 칸 움직이기

        way = soolD[(turn + 1) % soolD.size()];    // 술래가 다음으로 향하는 방향
        for (int i = 0; i < 3; i++) {
            int newR = soolR + i * dir[way][0];
            int newC = soolC + i * dir[way][1];

            if (newR < 0 || newR >= n || newC < 0 || newC >= n) continue;
            if (tree[newR][newC]) continue;

            points += (turn + 1) * board[newR][newC].size();
            board[newR][newC].clear();
        }
    }

    cout << points << '\n';
    
    return 0;
}

// 이동 도중 도망자들은 겹칠 수 있음
// 도망자는 다 같으므로 vector로 유지할 필요가 없음. -> board
// 1, 1, 2, 2, 3, 3, 4, 4, 4 -> 4, 4, 4, 3, 3, 2, 2, 1, 1
// 3, 0, 1, 2, 3, 0, 1, 2, 3 -> 1, 0, 3, 2, 1, 0
