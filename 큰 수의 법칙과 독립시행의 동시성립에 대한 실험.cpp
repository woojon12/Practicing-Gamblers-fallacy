
/*
* 흔히 도박사의 오류, 겜블러의 오류라고 불리는 그것.
* 지금까지 동전의 앞면이 많이 나왔으니
* 앞으로도 많이 나올 것이다 or 이제는 뒷면이 나올 때이다
* 와 같이 생각하는 것은 잘못된 것이라고 한다.

하지만 이는 직관적으로 받아들이기 어렵다.
동전 1000번 던져서 앞면(or뒷면)이 300번 이하로 나올 확률은 너무너무너무 작다.
실제로 프로그램 돌려보면 430 아래로 내려가는 일이 잘 없다.

이 정도면 분명 한쪽 면이 많이 나왔으면 다른 한쪽 면이 보충되어줘야
그런 법칙이 성립한다고 할 수 있지 않은가?
도박사의 오류는 사실 오류가 아니게 되지 않은가?

하지만 수학자들은 동전 던지기는 독립시행이므로
확률은 여전히 2분의 1이라고 한다

큰 수의 법칙과 독립시행의 법칙이 둘 다 옳다는 것을 직접 관측하려면
'앞면이 많이 나왔을 때 다음에는 뒷면일 것으로 확신하고 베팅할 확률이 더 높은 프로그램'
...의 승률이 그럼에도 50% 에 수렴하는지를 보면 된다
*/


/* ------------TODO

[딜러]
딜러가 모든 사람의 순위를 집계한다.
그 클래스의 이름은 Dealer 이다.
하나의 동전을 가지고 여러 사람이 승부하는 방식으로 할지,
여러 사람이 각자의 동전을 가지고 승부하는 방식으로 할지 처음에 고를 수 있다.
이 역할 역시 딜러가 파라미터를 받아 결정한다.
딜러가 받는 파라미터는 다음과 같다.
(Done) 1. 게임에 참가하는 겜블러의 수는 몇명인가
(Done) 2. 각 겜블러는 어떤 겜블러인가
(Done) 3. 각 겜블러에게 한 게임 당 동전 던지기를 몇 회 시킬 것인가 (함수에서 받음)
(Done) 4. 그런 게임을 몇 번 할 것인가
만약 게임 횟수가 단 1회이면 각자의 승리 결과를 출력한다.


[겜블러 interface]
모든 Gambler는 반드시 자기만의 betting() 메서드를 가진다.
동전을 맞춘 횟수가 몇번인지 세는 int win; 도 가진다.
또한 그 win을 반환하는 get_win() 메서드도 가진다.
본인이 몇(인덱스) 위를 얼마만큼 했는가를 저장하는 벡터도 가질 것.

[각 겜블러의 베팅 성향]

ConsecutiveGambler는 처음엔 아무 쪽면만 베팅하다가
한 쪽면이 n번 이상 나오면 무조건 다음 베팅에서 도박사의 오류를 실천한다.
n은 우리가 설정할 수 있다.
도박사의 오류를 앞면이 많이 나왔으니 '다음에도 앞면이 나올 것이다'(0) or '이번에는 뒷면이 나올 때이다'(1)
중 어느 쪽으로 실천할지도 정할 수 있고, 기본값은 true(1)이다.
오류를 범할 순간이 아닌 때에 마지막 오류에서 했던 선택을 유지하거나, 그냥 랜덤으로 하거나 선택 가능.

ConsecutiveProbabilityGambler는 처음 한 번은 아무 쪽 면에 베팅하고
한 쪽면이 연속으로 나오면, 그 다음 베팅에 도박사의 오류를 실천할 확률이 점점 높아진다.
최대 n번 연속으로 나온다면, 그 확률은 100%가 된다.
역시 도박사의 오류를 어느 쪽으로 실천할지 결정 가능.

TotalGambler는 처음 한 번은 아무 쪽 면에 베팅하고
그 다음부턴 지금까지 나온 총 횟수가 적은 면에 베팅한다

TotalDifferenceGambler 는 두 면의 총 횟수가 n이상 차이가 난 후에 적은 쪽에 베팅한다.
그 이외 시점엔 모두 마지막으로 했던 선택을 유지한다.

공동 1,2,3...등한 횟수도 따로 출력해야할듯.
비등비등한 얘들끼리 싸울 때 인덱스 상 0에 가까운 얘들이 더 많이 이기는 것처럼 보인다.
*/



#include <iostream>
#include <random>
#include <chrono>
#include <vector>
#include "Dealer.h"
#include "Gambler.h"

using namespace std;

int main()
{
    auto start = std::chrono::high_resolution_clock::now();

    int num_of_games = 1000;
    int num_of_gambler_competetion = 3;

    TotalDifferenceGambler player1(1, 0);
    ConsecutiveGambler player2(1, 1, 1);
    BernoulliGambler player6(0);

    Dealer dealer(false, num_of_gambler_competetion, player1, player2, player6);
    dealer.game_start(10, 100);

    //cout << "normal gambler : " << coinResult.get_front() << endl;

    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
    std::cout << "Execution time: " << duration << " microseconds" << std::endl;
    return 0;
}
