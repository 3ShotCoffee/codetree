#include <iostream>
#include <vector>
using namespace std;

#define MAX_DIST 5000
#define DIST(aR, aC, bR, bC) (aR - bR) * (aR - bR) + (aC - bC) * (aC - bC)

int N, M, P, C, D;
int santa_id[52][52];       // keep santa id

int rooR, rooC;         // 루돌프의 위치
int rooD[8][2] = {{-1, 0}, {-1, -1}, {0, -1}, {1, -1}, {1, 0}, {1, 1}, {0, 1}, {-1, 1}};    // 상 부터 반시계방향
int sanD[4][2] = {{-1, 0}, {0, 1}, {1, 0}, {0, -1}};    // 상 우 하 좌

struct Node {
    int row, col, score;
    int sleep;              // 깨기까지 남은 시간. 0이면 깬 상태
    int dead;               // 0 생존 1 탈락
};
vector<Node> santas;        // 1-base

// 루돌프와 가장 가까운 산타의 id 반환
int find_closest_santa(){
    int dist = MAX_DIST;
    int id;
    for (int i = 1; i <= N; i++) {
        for (int j = 1; j <= N; j++) {
            int who = santa_id[i][j];
            if (who == 0) continue;             // 산타가 없으면 패스
            if (santas[who].dead) continue;
            int new_dist = DIST(rooR, rooC, i, j);
            if (new_dist <= dist) {
                dist = new_dist;
                id = santa_id[i][j];
            }
        }
    }
    return id;
}

// 허공의 newS 산타를 row, col로 옮기기
void push_santa(int row, int col, int newS, int offR, int offC) {
    if (row < 1 || row > N || col < 1 || col > N) {     // OOR, 탈락
        santas[newS].dead = 1;
        return;
    }

    int oldS = santa_id[row][col];
    santas[newS].row = row; santas[newS].col = col;     // 탈락이 아니면 row, col로 newS가 이동
    santa_id[row][col] = newS;

    if (oldS != 0) {                                    // 밀려난 칸에 산타가 있으면 그것도 처리
        push_santa(row + offR, col + offC, oldS, offR, offC);
    }
}

// 충돌의 구현 (산타와 루돌프가 함수 호출 시점에 특정 칸에 함께 있음)
// 루돌프가 움직여서 충돌이 일어난 경우
void collision_roodolf(int d) {
    int id = santa_id[rooR][rooC];
    int newR = santas[id].row + C * rooD[d][0];
    int newC = santas[id].col + C * rooD[d][1];

    santas[id].sleep = 2;                                   // 기절
    santas[id].score += C;                                  // 점수 얻기
    santa_id[rooR][rooC] = 0;                               // 현재 자리에서 떼어내기
    push_santa(newR, newC, id, rooD[d][0], rooD[d][1]);     // 새로운 자리로 밀기
}

// 충돌의 구현 (산타와 루돌프가 현재 특정 칸에 함께 있음)
// 산타가 움직여서 충돌이 일어난 경우
void collision_santa(int d) {
    int id = santa_id[rooR][rooC];
    int newR = santas[id].row + D * sanD[d][0];
    int newC = santas[id].col + D * sanD[d][1];

    santas[id].sleep = 2;                                   // 기절
    santas[id].score += D;                                  // 점수 얻기
    santa_id[rooR][rooC] = 0;                               // 현재 자리에서 떼어내기
    push_santa(newR, newC, id, sanD[d][0], sanD[d][1]);     // 새로운 자리로 밀기
}

// debug
void print_santa_id() {
    for (int i = 1; i <= N; i++) {
        for (int j = 1; j <= N; j++) {
            cout << santa_id[i][j] << " ";
        }
        cout << '\n';
    }
    cout << "roodolf is at " << rooR << " " <<rooC << '\n';
}

int main() {
    cin >> N >> M >> P >> C >> D >> rooR >> rooC;

    // init santa_id
    for (int i = 1; i <= N; i++) {
        for (int j = 1; j <= N; j++) {
            santa_id[i][j] = 0;
        }
    }

    santas.resize(P + 1);
    for (int i = 0; i < P; i++) {
        int n, r, c; cin >> n >> r >> c;
        santas[n] = {r, c, 0, 0, 0};
        santa_id[r][c] = n;
    }

    for (int turn = 1; turn <= M; turn++) {
        //cout << "turn " << turn << " : \n";
        //print_santa_id();

        // 루돌프 한 번 움직임, 충돌
        int id = find_closest_santa();
        int dist = MAX_DIST;
        int direction;
        for (int d = 0; d < 8; d++) {
            int newR = rooR + rooD[d][0];
            int newC = rooC + rooD[d][1];

            if (newR < 1 || newR > N || newC < 1 || newC > N) continue;     // OOR

            int new_dist = DIST(newR, newC, santas[id].row, santas[id].col);
            if (new_dist >= dist) continue;                                 // 더 짧은 거리가 있음

            dist = new_dist;
            direction = d;
        }
        rooR += rooD[direction][0];          // 루돌프 돌진
        rooC += rooD[direction][1];

        if (santa_id[rooR][rooC] != 0) collision_roodolf(direction);        // 돌진한 칸에 산타가 있으면 충돌 구현

        //cout << "after roodolf :\n";
        //print_santa_id();

        // 산타 순서대로 움직임, 충돌 (기절 or 탈락 제외)
        // 움직일 수 있는 칸이 있더라도 루돌프로부터 가까워지지 않는다면 움직임 x
        for (int id = 1; id <= P; id++) {
            if (santas[id].sleep > 0) continue;     // 기절한 산타 패스
            if (santas[id].dead != 0) continue;     // 탈락한 산타 패스

            bool can_move = false;
            int dist = DIST(rooR, rooC, santas[id].row, santas[id].col);
            int direction;
            for (int d = 0; d < 4; d++) {
                int newR = santas[id].row + sanD[d][0];
                int newC = santas[id].col + sanD[d][1];

                if (newR < 1 || newR > N || newC < 1 || newC > N) continue;     // OOR
                if (santa_id[newR][newC] > 0) continue;                         // 다른 산타 있음

                int new_dist = DIST(rooR, rooC, newR, newC);
                if (new_dist >= dist) continue;                                 // 가까워지지 않음

                can_move = true;
                dist = new_dist;
                direction = d;
            }
            if (can_move) {     // 움직일 수 있다면 움직이기
                santa_id[santas[id].row][santas[id].col] = 0;
                santas[id].row += sanD[direction][0];
                santas[id].col += sanD[direction][1];
                santa_id[santas[id].row][santas[id].col] = id;

                if (santa_id[rooR][rooC] != 0) collision_santa((direction + 2) % 4);      // 움직인 칸에 루돌프가 있으면 충돌 구현
            }
        }

        //cout << "after santa :\n";
        //print_santa_id();

        int all_dead = true;
        for (int i = 1; i <= P; i++) {
            if (!santas[i].dead) {
                all_dead = false;
                santas[i].score++;          // 생존한 산타들에게 1점씩 부여
            }
            if (santas[i].sleep > 0) {
                santas[i].sleep--;          // 기절 -1
            }
        }
        if (all_dead) break;                // 모두 탈락이면 게임 종료
    }

    // 산타마다 최종 점수 출력
    for (int i = 1; i <= P; i++) {
        cout << santas[i].score << " ";
    }
    cout << '\n';

    return 0;
}

// 루돌프 움직임마다 가까운 산타 찾아야함 - 최대 30명이면 vector 유지도 괜찮은 선택
// 산타마다 id, score, sleep, 
// 
