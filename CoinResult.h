#pragma once

class CoinResult {
public:
    bool coin; //가장 최근에 나온 동전의 결과. 앞면과 뒷면은 추상적이므로 여기선 0(false)과 1(true)로 부른다.
    int consecutive_count = 0; //한 쪽면이 몇번이나 연속으로 나왔는지 저장, 0은 음수, 1은 양수로 쌓인다.
    //bool previous_coin_side = 0;
    //이전에 나왔던 동전 저장...이었으나 위 변수가 음수나 양수냐에 따라 이전에 나왔던 동전을 알 수 있으므로 필요가 없는 변수
    int num_front = 0, num_back = 0; //앞(0), 뒷면(1)이 총 몇 번 나왔는가 저장

public:
    bool is_first_execution()
    {
        if (num_front + num_back == 0) return true;
        return false;
    }

    void change_consecutive_count()
    {
        if (coin == 1) ++consecutive_count;
        else --consecutive_count;
    }

    void set_result(bool coin) {
        this->coin = coin;

        if ((consecutive_count > 0 && !coin)
            || (consecutive_count < 0 && coin)) consecutive_count = 0;

        change_consecutive_count();

        if (coin == 0)
            ++num_front;
        else if (coin == 1)
            ++num_back;
    }

    int get_front() {
        return num_front;
    }

    void initialize_data()
    {
        num_front = num_back = consecutive_count = 0;
    }
};