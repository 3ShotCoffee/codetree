#include <iostream>
#include <vector>
#include <deque>
#include <algorithm>
using namespace std;

#define pii pair<int, int>

int n, m, k;
int board[20][20];
int board_id[20][20];
int dir[4][2] = {{0, 1}, {-1, 0}, {0, -1}, {1, 0}};     // 오른쪽 위쪽 왼쪽 아래쪽 순서

vector<deque<pii>> teams;

void dfs(int row, int col, deque<pii> &dq) {
    if (board[dq.back().first][dq.back().second] == 3) {    // 꼬리사람까지 넣었음
        teams.push_back(dq);
        return;
    }

    for (int d = 0; d < 4; d++) {
        int newR = row + dir[d][0];
        int newC = col + dir[d][1];

        if (newR < 0 || newR >= n || newC < 0 || newC >= n) continue;       // OOR
        if (dq.size() > 1 && dq[dq.size()-2].first == newR && dq[dq.size()-2].second == newC) continue;  // going backwards
        if (board[newR][newC] == 0) continue;   // empty
        if (board[newR][newC] == 4) continue;   // 이동선
        if (board[row][col] == 1 && board[newR][newC] == 3) continue;   // 머리에서 꼬리로 바로 갈 수 없음

        dq.push_back({newR, newC});
        dfs(newR, newC, dq);
        dq.pop_back();

        break;      // 어차피 한 방향만 보면 됨
    }
}

int main() {
    ios_base::sync_with_stdio(0);
    cin.tie(0); cout.tie(0);

    cin >> n >> m >> k;
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            cin >> board[i][j];
        }
    }

    // build teams as dq's
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            if (board[i][j] == 1) {     // 머리사람부터 시작
                deque<pii> dq; 
                dq.push_back({i, j});
                dfs(i, j, dq);
            }
        }
    }

    int sum = 0;
    for (int turn = 0; turn < k; turn++) {
        // board_id init
        for (int i = 0; i < n; i++) {
            for (int j = 0; j < n; j++) {
                board_id[i][j] = -1;
            }
        }

        // [각 팀이 한 칸씩 이동]
        for (int i = 0; i < teams.size(); i++) {
            bool full = true;           // 이동선에 사람이 꽉 차 있으면 4를 찾을 수 없음
            
            // 머리사람 이동
            for (int d = 0; d < 4; d++) {
                int newR = teams[i].front().first + dir[d][0];
                int newC = teams[i].front().second + dir[d][1];

                if (newR < 0 || newR >= n || newC < 0 || newC >= n) continue;       // OOR
                if (board[newR][newC] == 4) {
                    full = false;
                    board[teams[i].front().first][teams[i].front().second] = 2;     // 머리사람 -> 중간사람 대체
                    board[newR][newC] = 1;                                          // 머리사람 채우기
                    teams[i].push_front({newR, newC});
                    break;
                }
            }

            if (!full) {    // 꼬리사람 이동
                board[teams[i].back().first][teams[i].back().second] = 4;       // 꼬리사람 -> 이동선 대체
                teams[i].pop_back();
                board[teams[i].back().first][teams[i].back().second] = 3;       // 꼬리사람 채우기
            }
            else {          // 꼬리가 있던 자리에 머리가 옴
                pii old_back = teams[i].back(); teams[i].pop_back();
                board[old_back.first][old_back.second] = 1;                     // 꼬리는 머리가 됨
                board[teams[i].front().first][teams[i].front().second] = 2;     // 머리는 중간이 됨
                board[teams[i].back().first][teams[i].back().second] = 3;       // 마지막이 꼬리가 됨
                teams[i].push_front(old_back);
            }

            // 사람이 있는 자리에 team id 채우기
            for (pii p : teams[i]) {
                board_id[p.first][p.second] = i;
            }
        }

        /*for (int i = 0; i < n; i++) {
            for (int j = 0; j < n; j++) {
                cout << board[i][j] << " ";
            }
            cout << '\n';
        }
        cout << '\n';*/

        // [공이 던져짐 + 점수 획득]
        int d = (turn / n) % 4;         // 공이 나아가는 방향
        int idx = turn % n;             // 공이 나아가는 경로의 행/열번호
        bool is_hit = false;
        for (int i = 0; i < n; i++) {
            pair<int, int> ball;        // 공의 위치 찾기
            switch (d) {
                case 0 : {
                    ball = {idx, i};
                    break;
                }
                case 1 : {
                    ball = {n - i - 1, idx};
                    break;
                }
                case 2 : {
                    ball = {n - 1 - idx, n - i - 1};
                    break;
                }
                case 3 : {
                    ball = {i, n - 1 - idx};
                    break;
                }
            }
            int id = board_id[ball.first][ball.second];
            if (id != -1) {      // 누군가가 있음
                for (int j = 0; j < teams[id].size(); j++) {
                    if (ball == teams[id][j]) {
                        sum += (j + 1) * (j + 1);
                        is_hit = true;
                        reverse(teams[id].begin(), teams[id].end());
                        break;
                    }
                }
            }
            if (is_hit) break;
        }
    }

    cout << sum << '\n';

    return 0;
}
