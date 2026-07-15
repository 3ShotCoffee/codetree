#include <iostream>
#include <vector>
#include <list>
#include <cmath>
#include <cassert>
#include <algorithm>
using namespace std;

int q, n, m;

vector<list<int>> belts;
vector<int> fronts, backs;
// 벨트를 deque로 구현 XX 물건 모두 옮기기 때문에 list여야 함.
// 오름차순 -> sort하면 그만
// 5를 위해 빠르게 선물 위치 알 수 있어야, 선물의 개수가 변하지 않으므로 선물마다 어디 벨트에 있는지 메모
// 최악의 경우 모든 쿼리가 5번, 모든 선물이 한 벨트에 있는 경우 10^10 시간복잡도가 나게 됨.
// -> 내가 어디 벨트에 있는지 X 내 앞뒤에 뭐가 있는지 메모해둬야 함.

void print_belt_state() {
    cout << "belt state is : \n";
    for (int i = 1; i <= n; i++) {
        for (int p : belts[i]) cout << p << " ";
        cout << '\n';
    }
    cout << '\n';
}

void print_front_and_back(){
    for (int i = 1; i <= m; i++) {
        cout << "item " << i << " has item " << fronts[i] << " as front and item " << backs[i] << " as back\n";
    }
}

int main() {
    cin >> q;
    for (int query = 0; query < q; query++) {
        int src, dst, pNum, bNum; 
        int a, b, c;
        int qType; cin >> qType;
        //cout << "query " << qType << '\n';
        switch(qType) {
            case 100 : {        // 공장 설립
                cin >> n >> m;
                // belt init
                belts.resize(n + 1);                // 1-base the belt number
                for (int i = 1; i <= m; i++) {
                    cin >> bNum;
                    belts[bNum].push_back(i);
                }
                for (int i = 1; i <= n; i++) {
                    belts[i].sort();     // sort the presents on each belt
                }

                // front-and-back init
                fronts.resize(m + 1);                  // 1-base the front and backs
                backs.resize(m + 1);
                for (int i = 1; i <= n; i++) {
                    /*for (int j = 0; j < belts[i].size(); j++) {
                        fronts[belts[i][j]] = (j == 0) ? -1 : belts[i][j - 1];
                        backs[belts[i][j]] = (j == belts[i].size() - 1) ? -1 : belts[i][j + 1];
                    }*/
                    for (auto it = belts[i].begin(); it != belts[i].end(); it = next(it)) {
                        fronts[*it] = (it == belts[i].begin()) ? -1 : *prev(it);
                        backs[*it] = (it == prev(belts[i].end())) ? -1 : *next(it);
                    }
                }

                //print_belt_state();
                //print_front_and_back();
                break;
            }
            case 200 : {        // 물건 모두 옮기기
                cin >> src >> dst;
                // adjust the front and backs
                int src_back = belts[src].empty() ? -1 : belts[src].back();
                int dst_front = belts[dst].empty() ? -1 : belts[dst].front();
                if (src_back != -1) backs[src_back] = dst_front;
                if (dst_front != -1) fronts[dst_front] = src_back;

                // actual augmentation
                belts[dst].splice(belts[dst].begin(), belts[src]);              // O(1)!!
                cout << belts[dst].size() << '\n';

                //print_belt_state();                
                //print_front_and_back();
                break;
            }
            case 300 : {        // 앞 물건만 교체하기
                cin >> src >> dst;
                if (!belts[src].empty() && !belts[dst].empty()) {
                    // exchange the fronts
                    int tmp = belts[src].front(); 
                    belts[src].pop_front();
                    belts[src].push_front(belts[dst].front());
                    belts[dst].pop_front();
                    belts[dst].push_front(tmp);

                    int src_back = belts[src].size() == 1 ? -1 : *next(belts[src].begin());
                    int dst_back = belts[dst].size() == 1 ? -1 : *next(belts[dst].begin());
                    backs[belts[src].front()] = src_back;
                    backs[belts[dst].front()] = dst_back;
                    if (src_back != -1) fronts[src_back] = belts[src].front();
                    if (dst_back != -1) fronts[dst_back] = belts[dst].front();
                }
                else if (!belts[src].empty()) {         // src만 nonempty
                    belts[dst].push_front(belts[src].front());
                    belts[src].pop_front();

                    int src_back = belts[src].size() == 0 ? -1 : belts[src].front();
                    backs[belts[dst].front()] = -1;
                    if (src_back != -1) fronts[src_back] = -1;
                }
                else if (!belts[dst].empty()) {         // dst만 nonempty
                    belts[src].push_front(belts[dst].front());
                    belts[dst].pop_front();

                    int dst_back = belts[dst].size() == 0 ? -1 : belts[dst].front();
                    backs[belts[src].front()] = -1;
                    if (dst_back != -1) fronts[dst_back] = -1;
                }
                // 둘 다 empty인 경우 아무것도 하지 말라
                cout << belts[dst].size() << '\n';  

                //print_belt_state();   
                //print_front_and_back();         
                break;
            }
            case 400 : {        // 물건 나누기
                cin >> src >> dst;
                if (belts[src].size() <= 1) {
                    cout << belts[dst].size() << '\n';
                    break;              // src 벨트에 물건이 하나 이하인 경우 무시 
                }
                int offset = floor(belts[src].size() / 2);                  // 이동해야 하는 선물의 개수
                int tail = *next(belts[src].begin(), offset - 1);           // 이 번호의 선물까지만 이동
                belts[dst].insert(belts[dst].begin(), belts[src].begin(), next(belts[src].begin(), offset));
                belts[src].erase(belts[src].begin(), next(belts[src].begin(), offset));

                if (belts[dst].size() != offset) {
                    auto it = belts[dst].begin(); advance(it, offset);
                    backs[tail] = *it; fronts[*it] = tail;
                }
                else {
                    backs[tail] = -1;
                }
                assert(belts[src].size() >= 1);
                fronts[belts[src].front()] = -1;

                cout << belts[dst].size() << '\n';

                //print_belt_state();
                //print_front_and_back();
                break;
            }
            case 500 : {        // 선물 정보 얻기
                cin >> pNum;
                a = fronts[pNum];
                b = backs[pNum];
                cout << a + 2 * b << '\n';
                break;
            }
            case 600 : {        // 벨트 정보 얻기
                cin >> bNum; 
                a = belts[bNum].empty() ? -1 : belts[bNum].front();
                b = belts[bNum].empty() ? -1 : belts[bNum].back();
                c = belts[bNum].size();
                cout << a + 2 * b + 3 * c << '\n';
                break;
            }
        }
    }


    return 0;
}
