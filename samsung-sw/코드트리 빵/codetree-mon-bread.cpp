#include <iostream>
#include <vector>
#include <queue>
#include <algorithm>
using namespace std;

#define pii pair<int, int>
#define x first
#define y second
#define MAX_DIST 4000

int n, m;
int basecamp[16][16];
int blocked[16][16];        // 갈 수 없는 곳
vector<pii> conv;           // 편의점의 위치
int dir[4][2] = {{-1, 0}, {0, -1}, {0, 1}, {1, 0}};     // 상 좌 우 하

struct Person {
    int row, col;
    int arrived;
};
vector<Person> people;

struct Node {
    int row, col, time;
};
queue<Node> Q;
int visited[16][16];
int distance(int fromR, int fromC, int toR, int toC) {
    // init visited
    for (int i = 1; i <= n; i++) {
        for (int j = 1; j <= n; j++) {
            visited[i][j] = 0;
        }
    }

    Q.push({fromR, fromC, 0});
    visited[fromR][fromC] = 1;
    while (!Q.empty()) {
        Node cur = Q.front(); Q.pop();

        if (cur.row == toR && cur.col == toC) {
            while (!Q.empty()) Q.pop();
            return cur.time;          // 도달하기까지의 시간 반환
        }

        for (int d = 0; d < 4; d++) {
            int newR = cur.row + dir[d][0];
            int newC = cur.col + dir[d][1];

            if (newR <= 0 || newR > n || newC <= 0 || newC > n) continue;   // OOR
            if (visited[newR][newC]) continue;      // already visited
            if (blocked[newR][newC]) continue;      // blocked

            Q.push({newR, newC, cur.time + 1});
            visited[newR][newC] = 1;
        }
    }

    // if it reaches here, the two positions are unconnected
    return MAX_DIST;        
}

// 편의점 toR, toC에서 가장 가까운 베이스캠프의 위치 반환
pii find_closest_basecamp(int toR, int toC) {
    int min_dist = MAX_DIST;
    int minR, minC;
    for (int i = 1; i <= n; i++) {
        for (int j = 1; j <= n; j++) {
            if (basecamp[i][j] == 0) continue;      // not a basecamp
            if (blocked[i][j]) continue;            // blocked
            int dist = distance(i, j, toR, toC);
            if (dist < min_dist) {
                min_dist = dist;
                minR = i; minC = j;
            }
        }
    }
    pii ret = make_pair(minR, minC);
    return ret;
}

// 현재 위치 fromR, fromC에서 한 칸 이동해서 편의점 toR, toC에 가장 가까워질 수 있는 위치 반환
pii find_closest_step(int fromR, int fromC, int toR, int toC) {
    int min_dist = MAX_DIST;
    int minR, minC;
    for (int d = 0; d < 4; d++){
        int newR = fromR + dir[d][0];
        int newC = fromC + dir[d][1];

        if (newR <= 0 || newR > n || newC <= 0 || newC > n) continue;   // OOR
        if (blocked[newR][newC]) continue;                              // blocked

        int dist = distance(newR, newC, toR, toC);
        if (dist < min_dist) {
            min_dist = dist;
            minR = newR; minC = newC;
        }
    }
    pii ret = make_pair(minR, minC);
    return ret;
}

int main() {
    cin >> n >> m;

    for (int i = 1; i <= n; i++) {
        for (int j = 1; j <= n; j++) {
            cin >> basecamp[i][j];
        }
    }

    conv.push_back({-1, -1});            // dummy push for 1-base
    for (int i = 0; i < m; i++) {
        int row, col; cin >> row >> col;
        conv.push_back({row, col});
    }

    for (int i = 1; i <= n; i++) {
        for (int j = 1; j <= n; j++) {
            blocked[i][j] = 0;
        }
    }

    people.push_back({0, 0, 0});           // dummy push for 1-base

    bool all_arrived;
    int t = 1;
    while (1) {
        all_arrived = true;

        // 격자에 있는 사람들 모두가 본인이 가고 싶은 편의점을 향해 1칸 (최단거리 : bfs로 구해놓기)
        vector<pii> tmp;
        for (int id = 1; id < people.size(); id++) {
            if (people[id].arrived) continue;           // 이미 편의점에 도착했다면 무시

            all_arrived = false;

            pii pos = find_closest_step(people[id].row, people[id].col, conv[id].x, conv[id].y);    // 상하좌우 중 편의점에 가장 가까운 칸
            
            people[id].row = pos.x;                     // 위치 업데이트
            people[id].col = pos.y;

            if (pos.x == conv[id].x && pos.y == conv[id].y) {   // 편의점에 도착
                people[id].arrived = 1;
                tmp.push_back({pos.x, pos.y});
            }
        }
        // 모두 움직인 후에 이동 완료된 편의점은 block
        while (!tmp.empty()) {
            pii pos = tmp.back(); tmp.pop_back();
            blocked[pos.x][pos.y] = 1;
        }

        // 모든 사람이 격자에 등장 & 모두 편의점에 도착
        if (t > m && all_arrived) {
            cout << t - 1 << '\n';      // 직전 분에 도착한 것임
            return 0;
        }

        // t번 사람이 최단거리 배캠, lock
        if (t <= m) {
            pii bc = find_closest_basecamp(conv[t].x, conv[t].y);
            people.push_back({bc.x, bc.y, 0});      // t번 사람 베이스캠프에 등장
            blocked[bc.x][bc.y] = 1;                // 해당 베이스캠프를 더이상 지나갈 수 없음
        }

        t++;
    }
}

// 1-base
// m명의 사람들마다 본인 편의점으로 가는 최소 루트 정도는 가지고 있는 것이 유리할 듯
// board에 사람 id를 넣는 형태 -> 굳이? 
// 이동하면서 같은 칸에 두 명이 있을 수 있는가? -> 가능... 그냥 vector로 가지고 있는 게 편할듯
