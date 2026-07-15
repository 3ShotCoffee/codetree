#include <iostream>
#include <queue>
#include <algorithm>
using namespace std;

int n, m, k;
int dir[4][2] = {{-1, 0}, {0, 1}, {1, 0}, {0, -1}};     // 위쪽 오른쪽 아래쪽 왼쪽

struct Player {
    int row, col;
    int dir;
    int power, gun;
    int points;
    const bool operator==(const Player &p) const {
        if (row == p.row && col == p.col) return true;
        return false;
    }
};
vector<Player> players;

priority_queue<int, vector<int>> guns[20][20];   // 한 칸에 여러 총이 있을 수 있음.

int main() {
    cin >> n >> m >> k;

    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            int gun_power; cin >> gun_power;
            if (gun_power != 0) guns[i][j].push(gun_power);
        }
    } 

    for (int i = 0; i < m; i++) {
        int x, y, d, s; cin >> x >> y >> d >> s;
        players.push_back({x - 1, y - 1, d, s, 0, 0});
    }

    for (int round = 0; round < k; round++) {
        // 순차적으로 플레이어 한 칸씩 이동 
        for (int id = 0; id < players.size(); id++) {
            int newR = players[id].row + dir[players[id].dir][0];
            int newC = players[id].col + dir[players[id].dir][1];

            if (newR < 0 || newR >= n || newC < 0 || newC >= n) {
                players[id].dir = (players[id].dir + 2) % 4;
                newR = players[id].row + dir[players[id].dir][0];
                newC = players[id].col + dir[players[id].dir][1];
            }

            Player dummy = {newR, newC, 0, 0, 0, 0};
            auto it = find(players.begin(), players.end(), dummy);
            players[id].row = newR; players[id].col = newC;     // 반드시 다른 플레이어 있는지 확인 후 이동

            if (it == players.end()) {          // no player
                if (!guns[newR][newC].empty() && guns[newR][newC].top() > players[id].gun) {      // yes (effective) gun
                    int new_gun = guns[newR][newC].top(); 
                    guns[newR][newC].pop();
                    guns[newR][newC].push(players[id].gun);
                    players[id].gun = new_gun;
                }
            }
            else {                              // yes player
                int otherId = it - players.begin();
                int winId;
                if (players[id].power + players[id].gun == players[otherId].power + players[otherId].gun) {
                    winId = players[id].power > players[otherId].power ? id : otherId;
                }
                else {
                    winId = (players[id].power + players[id].gun > players[otherId].power + players[otherId].gun) ? id : otherId;
                }
                int loseId = winId == id ? otherId : id;

                // winner earns points
                players[winId].points += (players[winId].power + players[winId].gun - players[loseId].power - players[loseId].gun);
                //cout << players[winId].power << " " << players[winId].gun << " " << players[loseId].power << " " << players[loseId].gun << '\n';
                //cout << "winner (id " << winId << ") now has " << players[winId].points << " points\n";

                // loser puts down its gun
                if (players[loseId].gun != 0) guns[newR][newC].push(players[loseId].gun);
                players[loseId].gun = 0;

                // winner picks up a gun if can
                if (!guns[newR][newC].empty() && guns[newR][newC].top() > players[winId].gun) {      // yes (effective) gun
                    int new_gun = guns[newR][newC].top(); 
                    guns[newR][newC].pop();
                    guns[newR][newC].push(players[winId].gun);
                    players[winId].gun = new_gun;
                }

                // loser moves
                for (int offset = 0; offset < 4; offset++) {
                    newR = players[loseId].row + dir[(players[loseId].dir + offset) % 4][0];
                    newC = players[loseId].col + dir[(players[loseId].dir + offset) % 4][1];

                    if (newR < 0 || newR >= n || newC < 0 || newC >= n) continue;                   // OOR
                    Player dummy = {newR, newC, 0, 0, 0, 0};
                    if (find(players.begin(), players.end(), dummy) != players.end()) continue;     // 다른 플레이어 있음

                    players[loseId].row = newR; players[loseId].col = newC;     // 이동

                    if (!guns[newR][newC].empty() && guns[newR][newC].top() > players[loseId].gun) {      // yes (effective) gun
                        int new_gun = guns[newR][newC].top(); 
                        guns[newR][newC].pop();
                        guns[newR][newC].push(players[loseId].gun);
                        players[loseId].gun = new_gun;
                    } 
                    players[loseId].dir = (players[loseId].dir + offset) % 4;
                    break;  
                }
            }
        }
    }

    for (Player p : players) {
        cout << p.points << " ";
    }
    cout << '\n';

    return 0;
}


// 1500
// 플레이어 찾기? 30
