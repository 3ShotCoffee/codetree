#include <iostream>
#include <queue>
#include <vector>
#include <cmath>
using namespace std;

int N, M, K;
int exitR, exitC;
int miro[11][11];                     // 0, 1-9, -1
int board[11][11] = {0,};             // (i, j)에 위치한 참가자들의 수
int board_next[11][11] = {0,};

int dir[4][2] = {{-1, 0}, {1, 0}, {0, -1}, {0, 1}};     // 상하좌우

int main() {
    cin >> N >> M >> K;

    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            cin >> miro[i][j];
        }
    }

    for (int i = 0; i < M; i++) {
        int r, c; cin >> r >> c;
        board[r - 1][c - 1]++;
    }

    cin >> exitR >> exitC; exitR--, exitC--;

    int total_move = 0;
    for (int sec = 0; sec < K; sec++) {
        // [참가자 동시에 움직임]
        // init board_next
        for (int i = 0; i < N; i++) {
            for (int j = 0; j < N; j++) {
                board_next[i][j] = 0;
            }
        }

        for (int i = 0; i < N; i++) {
            for (int j = 0; j < N; j++) {
                if (board[i][j] == 0) continue;                 // don't bother if empty
                
                int dist = abs(i - exitR) + abs(j - exitC);
                int direction;
                bool can_move = false;
                for (int d = 0; d < 4; d++) {
                    int newR = i + dir[d][0];
                    int newC = j + dir[d][1];

                    if (newR < 0 || newR >= N || newC < 0 || newC >= N) continue;   // OOR
                    if (miro[newR][newC] > 0) continue;                             // is a wall

                    int new_dist = abs(newR - exitR) + abs(newC - exitC);
                    if (new_dist >= dist) continue;

                    can_move = true;
                    direction = d;
                    break;
                }
                if (can_move) {
                    board_next[i + dir[direction][0]][j + dir[direction][1]] += board[i][j];
                    total_move += board[i][j];
                }
                else {
                    board_next[i][j] += board[i][j];
                }
            } 
        }
        //cout << " round " << sec + 1 << " after move : \n";
        for (int i = 0; i < N; i++) {
            for (int j = 0; j < N; j++) {
                if (i == exitR && j == exitC) board[i][j] = 0;     // 출구라면 굳이 더하지 말자
                else board[i][j] = board_next[i][j];
                //cout << board[i][j] << " ";
            }
            //cout << '\n';
        }

        // check if all participants have succeeded in getting out
        int all_out = true;
        for (int i = 0; i < N; i++) {
            for (int j = 0; j < N; j++) {
                if (board[i][j] > 0) {
                    all_out = false;
                    break;
                }
            }
        }
        if (all_out) break;

        // [미로의 회전]
        // find the minimum sized square that contains both the exit and a participant located at (i, j)
        /*int min_size = 100;
        pair<int, int> upleft = {100, 100};
        for (int i = 0; i < N; i++) {
            for (int j = 0; j < N; j++) {
                if (board[i][j] == 0) continue;                     // no participants

                int size = max(abs(i - exitR) + 1, abs(j - exitC) + 1);
                if (size > min_size) continue;

                int upleftR = max(i, exitR) + 1 - size; upleftR = min(max(upleftR, 0), N - 1); 
                int upleftC = max(j, exitC) + 1 - size; upleftC = min(max(upleftC, 0), N - 1);
                if (upleftR > upleft.first || (upleftR == upleft.first && upleftC > upleft.second)) continue;   // 우선시되는 상자

                min_size = size;
                upleft = {upleftR, upleftC};
            }
        }*/
        bool found = false;
        int min_size;
        pair<int, int> upleft;
        for (int size = 2; size <= N; size++) {
            for (int i = 0; i <= N - size; i++) {
                for (int j = 0; j <= N - size; j++) {
                    if (found) continue;
                    // inspect a box with upper left starting at (i, j) and with size "size"
                    bool is_exit = false;
                    bool is_part = false;
                    for (int r = 0; r < size; r++) {
                        for (int c = 0; c < size; c++) {
                            if (i + r == exitR && j + c == exitC) is_exit = true;
                            if (board[i + r][j + c] > 0) is_part = true;
                        }
                    }
                    if (is_exit && is_part) {
                        found = true;
                        upleft = {i, j};
                        min_size = size;
                        break;
                    }
                }
            }
        }

        //cout << upleft.first << " " << upleft.second << " " << min_size << '\n';

        // rotate miro, board and the location of exit
        vector<vector<int>> tmp_miro, tmp_board;
        tmp_miro.resize(min_size); 
        tmp_board.resize(min_size);
        for (int i = 0; i < min_size; i++) {
            tmp_miro[i].resize(min_size);
            tmp_board[i].resize(min_size);
        }

        for (int i = 0; i < min_size; i++) {
            for (int j = 0; j < min_size; j++) {
                tmp_miro[i][j] = miro[upleft.first + i][upleft.second + j];
                tmp_board[i][j] = board[upleft.first + i][upleft.second + j];
            }
        }
        for (int i = 0; i < min_size; i++) {
            for (int j = 0; j < min_size; j++) {
                miro[upleft.first + i][upleft.second + j] = tmp_miro[min_size - 1 - j][i];
                if (miro[upleft.first + i][upleft.second + j] > 0) miro[upleft.first + i][upleft.second + j]--;
                board[upleft.first + i][upleft.second + j] = tmp_board[min_size - 1 - j][i];
            }
        }

        int offR = exitR - upleft.first; int tmp = offR;
        int offC = exitC - upleft.second;
        offR = offC; offC = min_size - 1 - tmp;

        exitR = upleft.first + offR;
        exitC = upleft.second + offC;
    }

    cout << total_move << '\n';
    cout << exitR + 1 << " " << exitC + 1 << '\n';

    return 0;
}

// 1base
// 한 칸에 2명 이상의 참가자가 있을 수 있습니다.
// 칸마다의 최단 거리를 미리 계산해놓을 수 있다!
