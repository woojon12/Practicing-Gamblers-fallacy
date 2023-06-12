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
	std::vector<int> rank_score; //��(�ε���) ���� ��(����)��ŭ �ߴ°��� ������. ó���� ��� ���Ҹ� 0���� �ʱ�ȭ�� ��.
    std::vector<int> tie_rank_score; //���� 1,2,3...���� �� ���� ���� ����.
	int win = 0; //�� ���� �� ������
    std::string name; //���� �Ӽ��� �������� �� �����ڰ� �˾Ƽ� �ʱ�ȭ���־� ������ ��� ��� �� ������ �� ���� ����.

    std::mt19937_64 base_gen; //���� ���� rd()�� �õ带 �޴� ������ �׳� �ϳ��� seed�θ� �� ��.
    std::uniform_int_distribution<> base_dis;
protected:
    virtual bool betting(CoinResult& coinResult) { return 0; };//����
    virtual bool DoIGamblersFallacy(CoinResult& coinResult) { return 0; } //������ ���� ���ڻ��� ������ �������� �ϴ°�? �� �Ǵ��ϰ�, �´ٸ� true ��ȯ

protected:
    Gambler() : base_dis(0, 1)
    {
        std::random_device rd;
        auto debug = rd();
        //std::cout << debug << std::endl;
        base_gen.seed(debug);
    }

    std::mt19937_64& SetSeedAndReturn(std::mt19937_64& gen) //���ſ� ������ �� dis(SetSeedAndReturn(gen)) �̷��� ����� ����.
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
    int max_assure_threshold; //���� ���� �̸�ŭ �������� ������ ���� ������ 100% Ȯ���Ѵ�.
    int current_betting = 0;
    bool other_side_propensity = true; /*
        �̰� true�̸� ���� ���� ���� ���´� ������ �ٸ� �� ���� �����Ѵ�.
        �ݴ�� false�̸� ���� ���� ���� ������ �����ε� ���� ���̶� ����ϰ� �� ���� �����Ѵ� */
    bool keep_pre_error_betting = true; /*
        �̰� true�̸� ���������� ���ڻ��� ������ ��õ���� ���� ������ �����Ѵ�.
        false�̸� ���ڻ��� ������ ��õ���� �ƴ��� ���� ������ ��� �����̴� */

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
                //debug //cout << "{" << max_assure_threshold << "}"<< "�ٲ�" << endl;
            }
            else if (coinResult.consecutive_count < 0)
                current_betting = other_side_propensity ? 1 : 0;
        }
        else if(!keep_pre_error_betting){
            current_betting = base_dis(base_gen); //���⼭ ���� rd()�� �ϸ� ����ð� ���� �ɷ��� �׳� ���� �õ�.
        }

        return current_betting;
    }

    ConsecutiveGambler(int mat, bool osp, bool kpe, int) //name�� �� �� �ʱ�ȭ�Ǵ� �� ���� ���� �����ڸ� protected���� �����ε�, �׸� ���� �ƹ� �ǹ̾��� int �� ����
        : max_assure_threshold(mat), other_side_propensity(osp), keep_pre_error_betting(kpe)
    {
        if (max_assure_threshold < 1) this->max_assure_threshold = 1; //�� ������ �ּ� 1 �̻��̾�� ��
        else this->max_assure_threshold = max_assure_threshold;
    }

public:
    ConsecutiveGambler(int max_assure_threshold = 3, bool other_side_propensity = true, bool keep_pre_error_betting = true)
        : max_assure_threshold(max_assure_threshold), other_side_propensity(other_side_propensity), keep_pre_error_betting(keep_pre_error_betting)
    {
        if (max_assure_threshold < 1) this->max_assure_threshold = 1; //�� ������ �ּ� 1 �̻��̾�� ��
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
                //debug //cout << "{" << max_assure_threshold << "}"<< "�ٲ�" << endl;
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
    int assure_difference; //�� ���� �̻� Ƚ�����̰� ���� �������� �ݴ���� ���� ������ Ȯ���Ѵ�.
    bool no_random; //�� ���� Ƚ���� ���� ���� ���������� ����(false), ���� 0���θ� �ܼ���������(true)�� ����.

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