#include <iostream>
#include <vector>
#include <queue>
using namespace std;

#define pii pair<int, int>
#define MAX_POWER 5001
#define x first
#define y second

int N, M, K;

struct Node {
    int pow;        // 공격력
    int rec;        // 공격한 가장 최근의 턴
    int rel;        // 이번 턴 공격과 유관 (1)
};
Node board[12][12];     // 1-base

int dir4[4][2] = {{0, 1}, {1, 0}, {0, -1}, {-1, 0}};    // 우 하 좌 상
int dir8[8][2] = {{-1, 0}, {-1, -1}, {0, -1}, {1, -1}, {1, 0}, {1, 1}, {0, 1}, {-1, 1}};

// 공격자 포탑을 선정.
pii find_attacker() {
    pii ret;
    int power = MAX_POWER;
    int recent;
    int row, col;
    for (int i = 1; i <= N; i++) {
        for (int j = 1; j <= M; j++) {
            if (board[i][j].pow <= 0) continue;   // 부서진 포탑 제외
            if (board[i][j].pow < power ||
                board[i][j].pow == power && board[i][j].rec > recent ||
                board[i][j].pow == power && board[i][j].rec == recent && i + j > row + col ||
                board[i][j].pow == power && board[i][j].rec == recent && i + j == row + col && j > col) {
                    row = i, col = j;
                    power = board[i][j].pow; 
                    recent = board[i][j].rec;
                }
        }
    }
    ret = {row, col};
    return ret;
}

// 피공격자 포탑을 선정. 
pii find_attacked(int attackerR, int attackerC) {
    pii ret;
    int power = 0;
    int recent;
    int row, col;
    for (int i = 1; i <= N; i++) {
        for (int j = 1; j <= M; j++) {
            if (attackerR == i && attackerC == j) continue;
            if (board[i][j].pow <= 0) continue;   // 부서진 포탑 제외
            if (board[i][j].pow > power ||
                board[i][j].pow == power && board[i][j].rec < recent ||
                board[i][j].pow == power && board[i][j].rec == recent && i + j < row + col ||
                board[i][j].pow == power && board[i][j].rec == recent && i + j == row + col && j < col) {
                    row = i, col = j;
                    power = board[i][j].pow; 
                    recent = board[i][j].rec;
                }
        }
    }
    ret = {row, col};
    return ret;
}

int visited[12][12];
int parent[12][12];
queue<pii> Q;
// find the shortest path from attacker to attacked
bool find_shortest_path(int fromR, int fromC, int toR, int toC) {
    bool found = false;

    // init visited & parent
    for (int i = 1; i <= N; i++) {
        for (int j = 1; j <= M; j++) {
            visited[i][j] = 0;
            parent[i][j] = -1;
        }
    }

    Q.push({fromR, fromC});
    visited[fromR][fromC] = 1;
    while(!Q.empty()) {
        pii cur = Q.front(); Q.pop();

        if (cur.x == toR && cur.y == toC) {
            found = true;
            while(!Q.empty()) Q.pop();
            break;
        }

        for (int d = 0; d < 4; d++) {
            int newR = cur.x + dir4[d][0];
            int newC = cur.y + dir4[d][1];

            // 반대 방향으로 나오기
            if (newR == 0) newR = N;
            if (newR == N + 1) newR = 1;
            if (newC == 0) newC = M;
            if (newC == M + 1) newC = 1;

            if (visited[newR][newC]) continue;          // 이미 방문
            if (board[newR][newC].pow <= 0) continue;   // 부서진 포탑

            Q.push({newR, newC});
            visited[newR][newC] = 1;
            parent[newR][newC] = d;                     // 왔던 방향을 기록
        }
    }

    return found;
}

void razor_attack(int fromR, int fromC, int toR, int toC) {
    int power = board[fromR][fromC].pow;                // 공격자 공격력
    int row = toR;
    int col = toC;

    board[toR][toC].pow -= power;                       
    while (row != fromR || col != fromC) {
        if (row != toR || col != toC) {                 // 피공격자 포탑이 아니면
            board[row][col].rel = 1;                    // 공격과 유관
            board[row][col].pow -= power / 2;           // 공격력 절반만큼 타격
        }

        int d = parent[row][col];                   // d 방향을 통해 row, col에 도달
        row += dir4[(d < 2) ? d + 2 : d - 2][0];
        col += dir4[(d < 2) ? d + 2 : d - 2][1];

        if (row == 0) row = N;
        if (row == N + 1) row = 1;
        if (col == 0) col = M;
        if (col == M + 1) col = 1;
    }
}

void potan_attack(int fromR, int fromC, int toR, int toC) {
    int power = board[fromR][fromC].pow;                // 공격자 공격력
    
    board[toR][toC].pow -= power;
    for (int d = 0; d < 8; d++) {
        int newR = toR + dir8[d][0];
        int newC = toC + dir8[d][1];

        // 반대 방향으로 나오기
        if (newR == 0) newR = N;
        if (newR == N + 1) newR = 1;
        if (newC == 0) newC = M;
        if (newC == M + 1) newC = 1;

        if (newR == fromR && newC == fromC) continue;   // 공격자는 피해 없음
        if (board[newR][newC].pow <= 0) continue;       // 부서진거 굳이 공격하지 않는다

        board[newR][newC].rel = 1;                      // 공격과 유관
        board[newR][newC].pow -= power / 2;
    }
}

void print_power() {
    for (int i = 1; i <= N; i++) {
        for (int j = 1; j <= M; j++) {
            cout << board[i][j].pow << " ";
        }
        cout << '\n';
    }
}

int main() {
    cin >> N >> M >> K;

    for (int i = 1; i <= N; i++) {
        for (int j = 1; j <= M; j++) {
            int power; cin >> power;
            board[i][j] = {power, 0, 0};        // 모두 0번 턴에 공격 경험 있다고 가정
        }
    }

    for (int turn = 1; turn <= K; turn++) {
        // 공격자 선정
        pii er = find_attacker();
        board[er.x][er.y].pow += N + M;     // handicap
        board[er.x][er.y].rec = turn;       // 이번 턴에 공격했음을 기록
        board[er.x][er.y].rel = 1;          // 공격과 유관

        // 피공격자 선정
        pii ed = find_attacked(er.x, er.y);
        board[ed.x][ed.y].rel = 1;          // 공격과 유관

        // 공격자 -> 피공격자 최단경로 찾기 (경로저장)
        bool found = find_shortest_path(er.x, er.y, ed.x, ed.y);
        
        if (found) {        // 레이저 공격
            razor_attack(er.x, er.y, ed.x, ed.y);
        }
        else {              // 그런 경로 없으면 포탄 공격
            potan_attack(er.x, er.y, ed.x, ed.y);
        }

        //print_power();

        // 부서지지 않은 포탑 1개 이하면 중지
        int aliveCnt = 0;
        for (int i = 1; i <= N; i++) {
            for (int j = 1; j <= M; j++) {
                if (board[i][j].pow > 0)     
                    aliveCnt++;
            }
        }
        if (aliveCnt <= 1) break;

        for (int i = 1; i <= N; i++) {
            for (int j = 1; j <= M; j++) {
                if (board[i][j].pow > 0 && board[i][j].rel == 0)   // 공격과 무관 시 정비
                    board[i][j].pow++;
                board[i][j].rel = 0;            // 모두 공격 무관처리
            }
        }
    }

    int maxPower = 0;
    for (int i = 1; i <= N; i++) {
        for (int j = 1; j <= M; j++) {
            maxPower = max(maxPower, board[i][j].pow);
        }
    }
    cout << maxPower << '\n';

    return 0;
}