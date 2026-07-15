#include <iostream>
#include <vector>
#include <set>
#include <cassert>
using namespace std;

#define pii pair<int, int>

int L, N, Q;
int board[42][42];          // 빈칸, 함정, 벽
int knights_id[42][42];        // (i, j)에 위치한 기사의 id
int dir[4][2] = {{-1, 0}, {0, 1}, {1, 0}, {0, -1}};     // 위쪽, 오른쪽, 아래쪽, 왼쪽

struct Node {
    int r, c, h, w, k;
    int damage;
};
vector<Node> knights;

void update_knights_id() {
    // init knights_id
    for (int i = 1; i <= L; i++) {
        for (int j = 1; j <= L; j++) {
            knights_id[i][j] = 0;
        }
    }
    // update knights_id
    for (int id = 1; id <= N; id++) {
        if (knights[id].damage >= knights[id].k) continue;      // 만약 i가 사라진 기사이면 continue
        for (int i = knights[id].r; i < knights[id].r + knights[id].h; i++) {
            for (int j = knights[id].c; j < knights[id].c + knights[id].w; j++) {
                assert(knights_id[i][j] == 0);      // 기사끼리 겹치지 않아야 함
                assert(board[i][j] != 2);           // 벽이 없어야 함
                knights_id[i][j] = id;              // mark the id
            }
        }
    }
}

void build_checklist(int id, int ori, vector<pii> &checklist) {
    int row, col;
    switch (ori) {
        case 0: {       // 위쪽
            row = knights[id].r - 1;
            col = knights[id].c;
            for (int i = 0; i < knights[id].w; i++) {
                checklist.push_back({row, col + i});
            }
            break;
        }
        case 1: {       // 오른쪽
            row = knights[id].r;
            col = knights[id].c + knights[id].w;
            for (int i = 0; i < knights[id].h; i++) {
                checklist.push_back({row + i, col});
            }
            break;
        }
        case 2: {       // 아래쪽
            row = knights[id].r + knights[id].h;
            col = knights[id].c;
            for (int i = 0; i < knights[id].w; i++) {
                checklist.push_back({row, col + i});
            }
            break;
        }
        case 3: {       // 왼쪽
            row = knights[id].r;
            col = knights[id].c - 1;
            for (int i = 0; i < knights[id].h; i++) {
                checklist.push_back({row + i, col});
            }
            break;
        }
    }
}

// 기사 id가 ori 방향으로 한 칸 이동 가능한가?
// 이동해야 하는 기사들의 목록을 유지
set<int> move_list;
bool is_movable(int id, int ori) {
    // 해당 기사가 차지하는 변만큼 해당 방향으로 빈칸이면 이동 가능
    // 혹은 그 공간을 차지하는 기사가 모두 이동 가능하면 이동 가능
    // 벽이 하나라도 있으면 이동이 불가능
    // 모든 칸을 다 체크한다 치면 (1 + 2 + .. + 40) * 40, 쿼리 최대 100. 시간복잡도 최대 3,200,000

    vector<pii> checklist;
    build_checklist(id, ori, checklist);    // 확인해야 하는 칸들의 목록

    for (pii pos : checklist) {
        if (pos.first < 1 || pos.first > L || pos.second < 1 || pos.second > L) return false;   // OOR
        if (board[pos.first][pos.second] == 2) return false;                                    // 벽 있음

        int next_id = knights_id[pos.first][pos.second];
        if (next_id != 0 && !is_movable(next_id, ori)) return false;                            // 목적지의 기사가 같은 방향으로 움직일 수 없으면
    }
    move_list.insert(id);        // 나는 움직일 수 있음을 표시
    return true;
}

int count_traps(int row, int col, int width, int height) {
    int ret = 0;
    for (int i = row; i < row + height; i++) {
        for (int j = col; j < col + width; j++) {
            if (board[i][j] == 1) ret++;
        }
    }
    return ret;
}

int main() {
    cin >> L >> N >> Q;
    
    // init board
    for (int i = 1; i <= L; i++) {
        for (int j = 1; j <= L; j++) {
            cin >> board[i][j];
        }
    }

    knights.push_back({0, 0, 0, 0, 0, 0});      // dummy knight for 1-base
    for (int i = 1; i <= N; i++) {
        int r, c, h, w, k; cin >> r >> c >> h >> w >> k;
        knights.push_back({r, c, h, w, k, 0});
    }

    for (int q = 0; q < Q; q++) {
        int i, d; cin >> i >> d;        // knight i moves one step in direction d

        update_knights_id();            // knights_id 채우기

        // 만약 i가 사라진 기사이면 continue
        if (knights[i].damage >= knights[i].k) continue;

        // 해당 방향으로 이동할 수 있는지 확인
        move_list.clear();
        if (!is_movable(i, d)) continue;

        // 이동 및 데미지
        for (int id : move_list) {
            knights[id].r += dir[d][0];
            knights[id].c += dir[d][1];

            // 명령을 받은 기사가 아니라면 데미지를 입는다
            if (id != i) {
                int trapCnt = count_traps(knights[id].r, knights[id].c, knights[id].w, knights[id].h);
                knights[id].damage += trapCnt;
            }
        }
    }

    int total_damage = 0;
    for (int i = 1; i <= N; i++) {
        if (knights[i].damage >= knights[i].k) continue;
        total_damage += knights[i].damage;
    }
    cout << total_damage;

    return 0;
}

// 1-base board
// r, c, h, w, k Node? + damage
// "생존한" 기사들의 총 데미지
