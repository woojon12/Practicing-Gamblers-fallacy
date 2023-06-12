#pragma once

class CoinResult {
public:
    bool coin; //���� �ֱٿ� ���� ������ ���. �ո�� �޸��� �߻����̹Ƿ� ���⼱ 0(false)�� 1(true)�� �θ���.
    int consecutive_count = 0; //�� �ʸ��� ����̳� �������� ���Դ��� ����, 0�� ����, 1�� ����� ���δ�.
    //bool previous_coin_side = 0;
    //������ ���Դ� ���� ����...�̾����� �� ������ ������ ����Ŀ� ���� ������ ���Դ� ������ �� �� �����Ƿ� �ʿ䰡 ���� ����
    int num_front = 0, num_back = 0; //��(0), �޸�(1)�� �� �� �� ���Դ°� ����

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