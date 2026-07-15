#include <iostream>
#include <vector>
using namespace std;

int m, t, pacR, pacC;
int dir[8][2] = {{-1, 0}, {-1, -1}, {0, -1}, {1, -1}, {1, 0}, {1, 1}, {0, 1}, {-1, 1}};
int dir_pac[4][2] = {{-1, 0}, {0, -1}, {1, 0}, {0, 1}};     // 상 좌 하 우

vector<int> board_eggs[4][4];   // board of eggs. the number represents directions.
vector<int> board_live[4][4];   // board of monsters. the number represents directions.
vector<int> board_next[4][4];   // temporary space for board_live
int board_dead[4][4][3];        // board of dead bodies. 있는지 유무만 확인

bool found;
int max_eat, sum_eat;
int visited[4][4];
vector<pair<int, int>> route, max_route;
void dfs(int row, int col, int depth) {
    if (depth == 3) {
        if (!found || sum_eat > max_eat) {
            found = true;
            max_eat = sum_eat;
            max_route = route;
        }
        return;
    }
    for (int d = 0; d < 4; d++) {
        int newR = row + dir_pac[d][0];
        int newC = col + dir_pac[d][1];

        if (newR < 0 || newR > 3 || newC < 0 || newC > 3) continue;
        // if (visited[newR][newC]) continue;

        int old_visit = visited[newR][newC]; 
        sum_eat += old_visit ? 0 : board_live[newR][newC].size();   
        route.push_back({newR, newC});
        visited[newR][newC] = 1;

        dfs(newR, newC, depth + 1);

        sum_eat -= old_visit ? 0 : board_live[newR][newC].size();
        route.pop_back();
        visited[newR][newC] = old_visit;
    }
}


int main() {
    cin >> m >> t >> pacR >> pacC;

    pacR -= 1; pacC -= 1;

    for (int i = 0; i < m; i++) {
        int r, c, d; cin >> r >> c >> d;
        board_live[r-1][c-1].push_back(d-1);
    }

    // init board_dead
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            for (int k = 0; k < 3; k++) {
                board_dead[i][j][k] = 0;
            }
        }
    }

    for (int turn = 0; turn < t; turn++) {
        // [몬스터 복제 시도]
        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 4; j++) {
                board_eggs[i][j] = board_live[i][j];    // copy board_live to board_eggs
            }
        }

        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 4; j++) {
                board_next[i][j].clear();               // clean up board_next
            }
        }

        // [몬스터 이동]
        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 4; j++) {
                // for each of the monsters in i, j
                for (int d : board_live[i][j]) {
                    bool can_move = false;
                    for (int off = 0; off < 8; off++) {
                        int newR = i + dir[(d + off) % 8][0];
                        int newC = j + dir[(d + off) % 8][1];

                        if (newR < 0 || newR > 3 || newC < 0 || newC > 3) continue;                     // OOR
                        if (board_dead[newR][newC][0] > 0 || board_dead[newR][newC][1] > 0) continue;   // body exists
                        if (newR == pacR && newC == pacC) continue;                                     // pacman exists

                        can_move = true;
                        board_next[newR][newC].push_back((d + off) % 8);     // push the new direction
                        //cout << newR << " " << newC << " d : " << d << " off : " << off << '\n';
                        break;
                    }
                    if (!can_move) {
                        board_next[i][j].push_back(d);                          // stay put if cannot move
                    }
                }
            }
        }
        // copy board_next back to board_live
        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 4; j++) {
                board_live[i][j] = board_next[i][j];
            }
        }

        // [팩맨 이동]
        // clear up and init all data
        found = false;                          // 모든 루트에서 먹을 수 있는 몬스터가 0개일 수 있음, 따라서 방어 필요
        max_eat = 0, sum_eat = 0;
        route.clear(); max_route.clear();
        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 4; j++) {
                visited[i][j] = 0;
            }
        }
        // visited[pacR][pacC] = 1;             // 현재 자리의 몬스터를 먹지 못하므로 다시 와서 먹을 수 있게 해줘야
        dfs(pacR, pacC, 0);
        for (pair<int, int> p : max_route) {
            if (!board_live[p.first][p.second].empty()) {
                board_dead[p.first][p.second][2] = 1;       // freshly dead
                board_live[p.first][p.second].clear();      // clear up board_live
            }
        }
        pacR = max_route.back().first; pacC = max_route.back().second;                  // update pac*/
        
        // [몬스터 시체 소멸]
        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 4; j++) {
                board_dead[i][j][0] = board_dead[i][j][1];
                board_dead[i][j][1] = board_dead[i][j][2];
                board_dead[i][j][2] = 0;
            }
        }

        // [몬스터 복제 완성]
        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 4; j++) {
                board_live[i][j].insert(board_live[i][j].end(), board_eggs[i][j].begin(), board_eggs[i][j].end());
            }
        }
    }

    int cnt = 0;
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            cnt += board_live[i][j].size();
        }
    }
    cout << cnt << '\n';

    return 0;
}

// 한 칸에 여러개의 몬스터가 존재할 수 있음 (25개까지)
// 시체는 방향 필요없고 시간만 중요 -> board로 O(1)
// 몬스터는 vector로 가지고 있던지 board로 위치를 바로 알던지.
