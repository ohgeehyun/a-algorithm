#include <iostream>
#include <vector>
#include <windows.h>
#include "vector2int.h"
#include <queue>

using namespace std;

struct Pos
{
    Pos(int y, int x) { Y = y, X = x; };
    Pos() : Y(0), X(0) {};
    int Y;
    int X;
};

struct PQNode
{
    int F;  //총 비용(total cost) G+H 가 된다~
    int G;  //출발지점에서 현재 노드까지의 비용 cost from start
    int Y;  //현재 노드에서 도착지점까지의 비용cost to end
    int X;  //???

    bool operator<(const PQNode& other) const {
        return F > other.F; //c++은 과 c#의 우선순위큐 우선순위 기준이 반대이다. c++은 값이 큰것이 우선순위가 높기 때문에 연산자 오버라이딩을 통해 설정
    }
};

int _MaxY = 5;
int _MinY = -5;
int _MaxX = 5;
int _MinX = -5;

Pos Cell2Pos(const Vector2Int& cell) 
{
    return Pos(_MaxY - cell.posy, cell.posx - _MinX);
}


Vector2Int Pos2Cell(const Pos& pos) 
{
    return Vector2Int(pos.X + _MinX, _MaxY - pos.Y);
}

vector<Vector2Int> CalcCellPathFromParent(const vector<vector<Pos>>& parent, const Pos& dest) {

    vector<Vector2Int> cells;
    int y = dest.Y;
    int x = dest.X;
    while (parent[y][x].Y != y || parent[y][x].X != x) {
        cells.push_back(Pos2Cell({ y,x }));
        Pos p = parent[y][x];
        y = p.Y;
        x = p.X;
    }
    cells.push_back(Pos2Cell({ y,x }));
    reverse(cells.begin(), cells.end());

    int i = 0;
    for (auto& pair : cells)
    {
        cout << i << " 번쨰 좌표: " << pair.posy << ',' << pair.posx << endl;
        i++;
    }

    return cells;
}

//checkObject는 장애물 탐색여부이다. 현재는 없게 진행되므로 신경안써도되게끔 진행하였다.
vector<Vector2Int> FindPath(Vector2Int startCellPos, Vector2Int destCellPos, bool checkObjects)
{
    const int _deltaY[] = { 1, -1, 0, 0 }; // Up Down
    const int _deltaX[] = { 0, 0, -1, 1 }; // Left Right
    // 어디로 가든 비용은  10 but 대각선은 안된다 원래대로라면 벡터에서 대각선은 피타고라스정리에의해 14.14 배 여야하지만 바람의 나라 형식의 게임에서라면 대각선으로 움직이지않는다.  
    const int _cost[] = { 10, 10, 10, 10 };
    
    //열린 목록 : 현재노드가 갈수 있는 영역
    vector<vector<int>> open(20, vector <int>(20, INT_MAX));
    //닫힌 목록 : 현재 노드에서 가장 작은 비용을 가지고있는 노드가 선택되고 열린목록에서 빼고 닫힌 목록으로 넣어 '부모 노드'가 될 준비가 되었음을 의미
    vector<vector<bool>> closed(20, vector <bool>(20, false));
    //부모 노드는 지금까지 진행되온 노드이며 경로를 전부 찾고 거슬러 올라갈 떄 사용된다.
    vector<vector<Pos>> parent(20, vector <Pos>(20, Pos()));

    std::priority_queue<PQNode> pq;

    //배열은 0부터 시작하기때문에 음수가없다. 벡터로 인한 좌표는 배열에맞게 수정해주어야한다.
    Pos pos = Cell2Pos(startCellPos);
    Pos dest = Cell2Pos(destCellPos);

    //초기 값 세팅 첫스타트 지역에 대한 정보를 설정
    open[pos.Y][pos.X] = 10 * (std::abs(dest.Y - pos.Y) + std::abs(dest.X - pos.X)); //스타트지역에서  도착지점까지의 값(비용) 을 계산.
    pq.push({open[pos.Y][pos.X],0, pos.Y,pos.X}); //우선순위 큐에 (F(N)= G+H의 값,G(n),y 좌표,x 좌표 )
    parent[pos.Y][pos.X] = pos; //부모좌표에 시작 지점좌표 값 을 넣어준다.

    while (!pq.empty())
    {
        //우선순위큐에 값을 하나 뺸다.
        PQNode node = pq.top();
        pq.pop();

        //우선순위에서 가저 온 값이 이미 열린노드에서 닫힌노르도 온적이있는 노드라면 이미 방문한 적 이있는 노드
        if (closed[node.Y][node.X])
            continue;

        //방문한적이 없는 노드라면 방문체크
        closed[node.Y][node.X] = true;

        //도착지점이라면 break;
        if (node.Y == dest.Y && node.X == dest.X)
            break;
        
        //현재 노드에서 갈  수 있는 장소 탐색
        for (int i = 0; i < 4; i++)
        {
            Pos next(node.Y + _deltaY[i], node.X + _deltaX[i]);

            //장애물이없다는 가정하에 소스코드 작성하여 장애물 부분은스킵 장애물이 추가된다면 장애물있을경우 제외하는 코드 추가 여기서는 맵 밖 또는 배열 인덱스 범위 초과
            if (next.X < 0 || next.Y < 0 || next.X > 11 || next.Y > 11)
                continue;

            //이미 방문한적있는 타일이라면 스킵
            if (closed[next.Y][next.X])
                continue;

            int g = node.G + _cost[i]; //G는 현재까지든 이동 비용 거기다 탐색한 곳으로 가게 되는 비용을 더 해준다.
            int h = 10 * (abs(dest.Y - next.Y) + abs(dest.X - next.X)); //다음 노드에서 도착지점까지거리 계산

            //다른 경로에서 다음 노드를 거처서 가는길이 지금 상태에서  다음 타일을 거처서가는 거보다 빠르다는게 이미 나온 상황
            if (open[next.Y][next.X] <= g + h)
                continue;
          
            open[next.Y][next.X] = g + h;  //열린 노드에 다음 타일의 F(n) = G(n)+h(n)을 기록
            pq.push({ g + h,g,next.Y,next.X }); //우선순위 큐에 pos의 정보 기록
            parent[next.Y][next.X] = { node.Y,node.X };  //다음 노드의 부모가될 현재노드를 기록

        }
    }
    return CalcCellPathFromParent(parent, dest);
}


int main()
{   
    SetConsoleOutputCP(CP_UTF8);

    FindPath(Vector2Int(0,0),Vector2Int(5,5),false);

    system("pause");
}
