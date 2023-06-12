#pragma once
//#include "Dealer.h"
#include "CoinResult.h"
#include <vector>
#include <string>
#include <random>

//class Dealer;

class Gambler {
    friend class Dealer;

protected:
	std::vector<int> rank_score; //몇(인덱스) 위를 얼마(원소)만큼 했는가를 저장함. 처음에 모든 원소를 0으로 초기화할 것.
    std::vector<int> tie_rank_score; //공동 1,2,3...등을 한 경우는 따로 저장.
	int win = 0; //한 게임 당 득점수
    std::string name; //무슨 속성을 가졌는지 각 생성자가 알아서 초기화해주어 마지막 결과 출력 때 구분이 잘 가게 해줌.

    std::mt19937_64 base_gen; //워낙 자주 rd()로 시드를 받는 곳에선 그냥 하나의 seed로만 쓸 거.
    std::uniform_int_distribution<> base_dis;
protected:
    virtual bool betting(CoinResult& coinResult) { return 0; };//베팅
    virtual bool DoIGamblersFallacy(CoinResult& coinResult) { return 0; } //본인이 지금 도박사의 오류를 저질러야 하는가? 를 판단하고, 맞다면 true 반환

protected:
    Gambler() : base_dis(0, 1)
    {
        std::random_device rd;
        auto debug = rd();
        //std::cout << debug << std::endl;
        base_gen.seed(debug);
    }

    std::mt19937_64& SetSeedAndReturn(std::mt19937_64& gen) //무거운 랜덤할 때 dis(SetSeedAndReturn(gen)) 이렇게 쓰라고 만듦.
    {
        std::random_device rd;
        gen.seed(rd());
        return gen;
    }
};

class BernoulliGambler : public Gambler {
    bool current_betting;
    bool use_random;

protected:
    bool betting(CoinResult& coinResult) override {
        if (use_random) {
            return base_dis(base_gen);
        }

        return current_betting;
    }

public:
    BernoulliGambler(bool use_random, bool which_side_no_random = 0) 
        : use_random(use_random), current_betting(which_side_no_random)
    {
        name = "Bernoulli";
        if (use_random) name.append(" (random)");
    }
};

/////////////////////////////////////////////////////////////////////////////////

class ConsecutiveGambler : public Gambler {
protected:
    int max_assure_threshold; //한쪽 면이 이만큼 연속으로 나오면 다음 동전을 100% 확신한다.
    int current_betting = 0;
    bool other_side_propensity = true; /*
        이게 true이면 한쪽 면이 많이 나온다 싶으면 다른 쪽 면을 선택한다.
        반대로 false이면 한쪽 면이 많이 나오면 앞으로도 나올 것이라 기대하고 그 쪽을 선택한다 */
    bool keep_pre_error_betting = true; /*
        이게 true이면 마지막으로 도박사의 오류를 실천했을 때의 선택을 유지한다.
        false이면 도박사의 오류를 실천하지 아니한 때의 선택은 모두 랜덤이다 */

protected:
    bool DoIGamblersFallacy(CoinResult& coinResult) override
    {
        if (abs(coinResult.consecutive_count) == max_assure_threshold)
            return true;

        return false;
    }

    bool betting(CoinResult& coinResult) override
    {
        if (DoIGamblersFallacy(coinResult)) {
            if (coinResult.consecutive_count > 0)
            {
                current_betting = other_side_propensity ? 0 : 1;
                //debug //cout << "{" << max_assure_threshold << "}"<< "바뀜" << endl;
            }
            else if (coinResult.consecutive_count < 0)
                current_betting = other_side_propensity ? 1 : 0;
        }
        else if(!keep_pre_error_betting){
            current_betting = base_dis(base_gen); //여기서 새로 rd()를 하면 수행시간 오래 걸려서 그냥 고정 시드.
        }

        return current_betting;
    }

    ConsecutiveGambler(int mat, bool osp, bool kpe, int) //name이 두 번 초기화되는 걸 막기 위해 생성자를 protected에서 오버로드, 그를 위해 아무 의미없이 int 걍 받음
        : max_assure_threshold(mat), other_side_propensity(osp), keep_pre_error_betting(kpe)
    {
        if (max_assure_threshold < 1) this->max_assure_threshold = 1; //이 변수는 최소 1 이상이어야 함
        else this->max_assure_threshold = max_assure_threshold;
    }

public:
    ConsecutiveGambler(int max_assure_threshold = 3, bool other_side_propensity = true, bool keep_pre_error_betting = true)
        : max_assure_threshold(max_assure_threshold), other_side_propensity(other_side_propensity), keep_pre_error_betting(keep_pre_error_betting)
    {
        if (max_assure_threshold < 1) this->max_assure_threshold = 1; //이 변수는 최소 1 이상이어야 함
        else this->max_assure_threshold = max_assure_threshold;

        this->name = std::to_string(max_assure_threshold);
        this->name.append(" Consecutive");
        if (other_side_propensity) this->name.append(" (otherSide)");
        if (keep_pre_error_betting) this->name.append(" (keepPreError)");
    }
};


class ConsecutiveProbabilityGambler : public ConsecutiveGambler {
protected:
    bool DoIGamblersFallacy(CoinResult& coinResult) override
    {
        int dis_param = abs(coinResult.consecutive_count);

        std::uniform_int_distribution<> base_dis(
            dis_param > max_assure_threshold ? max_assure_threshold : dis_param
            , max_assure_threshold);
        

        if (base_dis(base_gen) == max_assure_threshold)
            return true;

        return false;
    }

    bool betting(CoinResult& coinResult) override
    {
        if (DoIGamblersFallacy(coinResult)) {
            if (coinResult.consecutive_count > 0)
            {
                current_betting = other_side_propensity ? 0 : 1;
                //debug //cout << "{" << max_assure_threshold << "}"<< "바뀜" << endl;
            }
            else if (coinResult.consecutive_count < 0)
                current_betting = other_side_propensity ? 1 : 0;
        }
        else if (!keep_pre_error_betting) {
            current_betting = base_dis(base_gen);
        }

        return current_betting;
    }

public:
    ConsecutiveProbabilityGambler(int max_assure_threshold = 3, bool other_side_propensity = true, bool keep_pre_error_betting = true)
        : ConsecutiveGambler(max_assure_threshold, other_side_propensity, keep_pre_error_betting, 1)
    {
        this->name = std::to_string(max_assure_threshold);
        this->name.append(" Consecutive Probability");
        if (other_side_propensity) this->name.append(" (otherSide)");
        if (keep_pre_error_betting) this->name.append(" (keepPreError)");
    }
};

/////////////////////////////////////////////////////////////////////////////////////

class TotalDifferenceGambler : public Gambler
{
    int assure_difference; //이 숫자 이상 횟수차이가 나면 다음에는 반대면이 나올 것으로 확신한다.
    bool no_random; //두 면의 횟수가 같을 때는 랜덤베팅을 할지(false), 오직 0으로만 단순베팅할지(true)를 고른다.

protected:
    bool DoIGamblersFallacy(CoinResult& coinResult) override
    {
        if (abs(coinResult.num_front - coinResult.num_back) >= assure_difference)
            return true;
        return false;
    }

    bool betting(CoinResult& coinResult) override
    {
        if (DoIGamblersFallacy(coinResult))
        {
            if (coinResult.num_front > coinResult.num_back) return 1;
            else if (coinResult.num_back > coinResult.num_front) return 0;
        }

        if (no_random)
            return 0;
        else
            return base_dis(base_gen);
    }

public:
    TotalDifferenceGambler(int assure_difference = 3, bool no_random = true)
        : assure_difference(assure_difference), no_random(no_random)
    {
        if (assure_difference < 1) this->assure_difference = 1;

        this->name = std::to_string(assure_difference);
        this->name.append(" Total Difference");
        if (no_random) this->name.append(" (No Random when no Error)");
    }
};