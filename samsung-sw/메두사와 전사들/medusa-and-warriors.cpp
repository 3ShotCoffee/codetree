#include <iostream>
#include <vector>
#include <queue>
#include <set>
#include <algorithm>
using namespace std;

#define pii pair<int, int>
#define DIST(aR, aC, bR, bC) (aR - bR) * (aR - bR) + (aC - bC) * (aC - bC)

int N, M;
int medR, medC, parR, parC;

int road[52][52];               // 도로 0 벽 1
int board[52][52];              // 메두사의 눈에 보이는 곳 1 보이지 않는 곳 0
int dir[4][3][2] = {
    {{-1, 0}, {-1, -1}, {-1, 1}},       // 상
    {{1, 0}, {1, -1}, {1, 1}},          // 하
    {{0, -1}, {-1, -1}, {1, -1}},       // 좌
    {{0, 1}, {-1, 1}, {1, 1}},          // 우
};
// 상 하 좌 우 + 각각의 대각방향

struct Node {
    int row, col;
    int stone;          // 0 생존 1 돌
    int dead;           // 0 생존 1 사망
};
vector<Node> warriers;      // 1-base. 1 <= id <= M

// 메두사의 집에서 공원까지 최단경로 구하기
int visited[52][52];
int parent_dir[52][52];     // 부모에게로 가는 방향
queue<pii> Q;
bool shortest_path_medusa() {
    // init visited
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            visited[i][j] = 0;
        }
    }

    Q.push({medR, medC});
    parent_dir[medR][medC] = -1;
    visited[medR][medC] = 1;
    while (!Q.empty()) {
        pii cur = Q.front(); Q.pop();

        for (int d = 0; d < 4; d++) {
            int newR = cur.first + dir[d][0][0];
            int newC = cur.second + dir[d][0][1];

            if (newR < 0 || newR >= N || newC < 0 || newC >= N) continue;   // OOR
            if (road[newR][newC]) continue;                                 // 길이 없음
            if (visited[newR][newC]) continue;                              // 이미 방문

            Q.push({newR, newC});
            parent_dir[newR][newC] = (d % 2 == 0) ? d + 1 : d - 1;          // 반대방향으로 가면 부모가 있음
            visited[newR][newC] = 1;

            if (newR == parR && newC == parC) return true;                    // 길이 있으면 true 반환
        }
    }
    return false;       // 길이 없으면 false 반환
}

vector<pii> medusa_path;
void build_medusa_path() {
    int row = parR;
    int col = parC;

    while(parent_dir[row][col] != -1) {
        int d = parent_dir[row][col];
        int oldR = row + dir[d][0][0];
        int oldC = col + dir[d][0][1];

        medusa_path.push_back({row, col});
        row = oldR, col = oldC;
    }

    reverse(medusa_path.begin(), medusa_path.end());
}

void print_board() {
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            cout << board[i][j] << " ";
        }
        cout << '\n';
    }
    cout << '\n';
}

// 메두사가 d 방향을 바라볼 때 target 기준으로 보이는 면적을 1로 채우기 / 0으로 지우기
void warriers_in_view(int targetR, int targetC, int d) {
    // 각 전사의 메두사에 대한 방향/기울기만 계산하면 시야각에 들어오는지 판단 가능   

    bool fill = (targetR == medR && targetC == medC);       // target이 메두사면 1로 채워야

    // 일단 타겟의 전체 시야각에 들어오는지만 판단
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            int diffR = i - targetR;
            int diffC = j - targetC;

            // 메두사가 직면하는 방향부터 (상하좌우) 확인.
            // 시야각에 들어오려면 메두사의 시야 방향으로 한 칸 이상은 나아가 있어야 함
            int deltaR = dir[d][0][0];
            int deltaC = dir[d][0][1];
            if (diffR * deltaR + diffC * deltaC <= 0) continue;  

            // 이때 전사가 타겟와 같은 행/열에 있으면 시야각에 포함됨
            if (i == targetR || j == targetC) {
                board[i][j] = fill ? 1 : 0;
                continue;
            }   

            // 메두사가 향하는 방향이 정의하는 2개의 대각 방향에서
            for (int w = 1; w < 3; w++) {
                deltaR = dir[d][w][0];
                deltaC = dir[d][w][1];

                // 전사가 가리는 면적을 지우는 경우 메두사 기준의 전사 방향만 의미가 있음
                if (!fill && ((targetR - medR) * deltaR <= 0 || (targetC - medC) * deltaC <= 0)) continue;

                // 그 방향들과 R, C 방향이 같고
                if (diffR * deltaR > 0 && diffC * deltaC > 0) {
                    // 양방향 90도 각 안에 있으면 시야각 안에 있는 전사로 간주    
                    if (d / 2 == 0 && abs(diffR) >= abs(diffC) || d / 2 == 1 && abs(diffR) <= abs(diffC)) {
                        board[i][j] = fill ? 1 : 0;
                    }
                }
            }
        }
    }
}

// 메두사가 d 방향을 바라볼 때 최종적으로 보이는 면적을 계산
// 다른 전사들에 의해 가려지는 면적은 제외
void warriers_seen_final(int d) {
    // init board
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            board[i][j] = 0;
        }
    }

    warriers_in_view(medR, medC, d);       // 단순 시야각 내 면적을 1로 만들기
    //print_board();

    for (int id = 1; id <= M; id++) {
        if (warriers[id].dead) continue;                             // 죽어서 사라진 전사라면 볼 필요가 없다
        if (!board[warriers[id].row][warriers[id].col]) continue;    // 원래 시야각 안에 없었거나 가려지는 전사라면 볼 필요가 없다

        warriers_in_view(warriers[id].row, warriers[id].col, d);     // id에 의해 가려지는 면적을 0으로 지우기
    }
    //print_board();
}

vector<int> list_of_warriers_seen() {
    vector<int> ret;
    for (int id = 1; id <= M; id++) {
        if (warriers[id].dead) continue;
        if (board[warriers[id].row][warriers[id].col]) ret.push_back(id);
    }
    return ret;
}

int main() {
    cin >> N >> M >> medR >> medC >> parR >> parC;

    // init warriers
    warriers.push_back({0, 0, 0, 0});      // dummy
    for (int i = 0; i < M; i++) {
        int r, c; cin >> r >> c;
        warriers.push_back({r, c, 0, 0});
    }

    // init road
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            cin >> road[i][j];
        }
    }

    // 메두사의 공원까지의 최단경로 찾기
    bool found = shortest_path_medusa();

    if (!found) {           // 메두사의 집에서 공원까지 루트가 없다면 끝
        cout << "-1\n";
        return 0;
    }

    // shortest_path_medusa()를 기반으로 경로 만들어두기 (medR, medC 미포함)
    build_medusa_path();

    for (int turn = 0;; turn++) {
        int move_cnt = 0, stone_cnt = 0, attack_cnt = 0;
        // [메두사의 이동]
        // medusa_path에서 하나씩 꺼내쓰기
        medR = medusa_path[turn].first;
        medC = medusa_path[turn].second;

        if (medR == parR && medC == parC) {         // 메두사가 공원에 도착함
            cout << "0\n";
            return 0;
        }

        // 메두사가 이동한 칸에 전사가 있을 경우 전사가 사라짐
        for (int id = 1; id <= M; id++) {
            if (warriers[id].row == medR && warriers[id].col == medC) {
                warriers[id].dead = 1;
            }
        }

        // [메두사의 시선]
        int max_board[52][52]; 
        vector<int> max_stop_list;
        for (int d = 0; d < 4; d++) {
            //cout << "direction " << d << '\n';
            warriers_seen_final(d);                 // 메두사가 볼 수 있는 면적을 1로 마킹

            vector<int> stop_list = list_of_warriers_seen();
            
            //for (int s : stop_list) cout << s << " ";
            //cout << '\n';

            if (stop_list.size() > stone_cnt) {
                stone_cnt = stop_list.size();
                max_stop_list = stop_list;
                for (int i = 0; i < N; i++) {
                    for (int j = 0; j < N; j++) {
                        max_board[i][j] = board[i][j];      // 최종적인 시야는 max_board에 저장
                    }
                }
            }
        }
        for (int id : max_stop_list) {
            warriers[id].stone = 1;             // 메두사의 시야에 있는 전사들은 돌이 된다
        }

        // [전사들의 이동]
        for (int id = 1; id <= M; id++) {
            if (warriers[id].stone) continue;       // 돌이면 무시

            int dist = DIST(warriers[id].row, warriers[id].col, medR, medC);
            for (int step = 0; step < 2; step++) {  // 두 칸을 간다
                if (warriers[id].dead) continue;    // 죽었으면 무시 (첫 이동에서 사망 가능)

                for (int d = 0; d < 4; d++) {
                    int newR = warriers[id].row + dir[(d + 2 * step) % 4][0][0];
                    int newC = warriers[id].col + dir[(d + 2 * step) % 4][0][1];

                    if (newR < 0 || newR >= N || newC < 0 || newC >= N) continue;   // OOR
                    if (max_board[newR][newC]) continue;                            // 메두사의 시야에 들어오는 곳
                    if (DIST(newR, newC, medR, medC) >= dist) continue;             // 메두사와의 거리가 좁혀지지 않음

                    warriers[id].row = newR;
                    warriers[id].col = newC;
                    dist = DIST(newR, newC, medR, medC);
                    move_cnt++;

                    if (newR == medR && newC == medC) {     // 이동한 칸에 메두사가 있으면
                        warriers[id].dead = 1;
                        attack_cnt++;
                    }

                    break;                                  // 움직였으면 끝
                }
            }
        }

        // [돌에서 풀려나기]
        for (int id = 1; id <= M; id++) {
            warriers[id].stone = 0;
        }

        cout << move_cnt << " " << stone_cnt << " " << attack_cnt << '\n'; 
    }

    return 0;
}

// 0-base