#pragma once
#include "Gambler.h"
#include <random>
#include <vector>
#include <cstdarg>
#include <functional>

class Dealer {
	int num_flip; //�� ���� �� ���� ������ Ƚ��
	int num_gamblers; //�� ���̼�
	int num_games; //�� ������ �� ���ΰ�
	bool multi_coin; //���� �����⸦ ���� ����(true) �ϳ��� �������� ���� ����(false).
	std::vector<std::reference_wrapper<Gambler>> Gamblers;
	CoinResult coinResult;

public:
	Dealer(bool multi_coin, int num_gamblers, ...)
		: multi_coin(multi_coin), num_gamblers(num_gamblers) {

		//Gamblers.resize(num_gamblers);
		//��°���� �̰� �ϸ� C2512��� �� ������ ���ٴ� ������ �߻��Ѵ�. �Ƹ� �� �ȿ��� �����ڸ� �� ��� ���� ��?
		//�׷��� �׳� �̰ž��� push_back �� �ؾ���.

		std::va_list Gambler_args;
		va_start(Gambler_args, num_gamblers);

		for (int i = 0; i < num_gamblers; ++i) { 
			auto v = std::vector<int>(num_gamblers, 0);
			Gamblers.push_back(std::ref(va_arg(Gambler_args, Gambler))); 
			Gamblers[i].get().rank_score = v;
			Gamblers[i].get().tie_rank_score = v;
		}

		va_end(Gambler_args);
		//std::cout << "Debug �� �Լ��� ������ name�� �״´�. : " << Gamblers[0].get().name << std::endl;
	}


	void game_start(int num_games, int num_flip)
	{
		this->num_games = num_games;
		this->num_flip = num_flip;

		private_game_start();
	}

private:
	void initialize_all_gamblers_winscore()
	{
		for(auto ref_wrp : Gamblers)
			ref_wrp.get().win = 0; //debug? �̰� ���ֵ� �ǹ��ִ� ������ ��.
	}

	void winscore_print()
	{
		for (int i = 0; i < num_gamblers; ++i) {
			std::cout << Gamblers[i].get().win << " : " << Gamblers[i].get().name << std::endl;
		}
	}

	void private_game_start() {
		for (int i = 0; i < num_games; ++i) {
			initialize_all_gamblers_winscore();
			coinResult.initialize_data();

			if (multi_coin == false) {
				one_coin_game();
			}
			else {
				multi_coin_game();
			}

			//#�� ���� ��� ����

			std::vector<std::pair<int, int>> index_and_winscore;
			//vector�� �ε����� ����,
			//pair�� first�� �� �׺��� ����Ű�� ����, second�� �׺��� ������
			for (int i = 0; i < num_gamblers; ++i) {
				index_and_winscore.push_back(std::pair<int, int>(i, Gamblers[i].get().win));
			}
			for (int l = 0; l < num_gamblers - 1; ++l) { //���� ����
				for (int n = 0; n < num_gamblers - l - 1; ++n) {
					if (index_and_winscore[n].second < index_and_winscore[n + 1].second) {
						auto temp = index_and_winscore[n + 1];
						index_and_winscore[n + 1] = index_and_winscore[n];
						index_and_winscore[n] = temp;
					}
				}
			}
			//TODO(Done) : ���� 1��, ���� 2���� ������ ��.

			//debug
			/*for (auto e : index_and_winscore)
				std::cout << "[" << e.second << ":" << Gamblers[e.first].get().name << "] ";
			std::cout << std::endl;*/

			int current_handle_rank = 0;
			int human_index = 0;

			while (human_index < num_gamblers) {
				int j = human_index;
				for (j = human_index; j < num_gamblers - 1; ++j) {
					if (index_and_winscore[j].second == index_and_winscore[j + 1].second) {
						continue;
					}

					break;
				}

				if (j == human_index) {
					++Gamblers[index_and_winscore[human_index].first].get().rank_score[current_handle_rank];
					//debug
					//std::cout << Gamblers[index_and_winscore[human_index].first].get().name << "�� " << current_handle_rank + 1 << "���� " << Gamblers[human_index].get().rank_score[current_handle_rank] << "�� �ߴ�" << std::endl;
					++human_index;
				}
				else {
					for (int i = human_index; i < j + 1; ++i) {
						++Gamblers[index_and_winscore[i].first].get().tie_rank_score[current_handle_rank];
						/*
						�ܵ� 1��ٴ� ���� 1���� ��ġ�� �� ����, �׷� ���̰� ������ �׳� �и��� �س��°���.
						�� �����ϰ� �Ϸ��� ������ ��ܼ� ���� n���� �ƴ������� �� �����ؾ߰����� �װ� ���� ���忡���� �Ӹ� ������
						�ϴ� ������ �ܵ��� ���еǵ��� ����.
						*/
					}

					//debug
					/*std::cout << "���� " << current_handle_rank + 1 << "�� : " << Gamblers[index_and_winscore[human_index].first].get().name;
					for (int i = human_index + 1; i < j + 1; ++i) {
						std::cout << ", " << Gamblers[i].get().name;
					}
					std::cout << std::endl;*/

					human_index = j + 1;
				}

				++current_handle_rank;
			}

			//debug
			//winscore_print(); std::cout << std::endl;
		}

		//#��� ���� ��� ���

		if (num_games != 1) {
			for (int i = 0; i < num_gamblers; ++i) {
				std::cout << Gamblers[i].get().name << std::endl;

				for (int j = 0; j < num_gamblers; ++j) {
					std::cout << "\t" << "�ܵ� " << j + 1 << "�� Ƚ�� : " << Gamblers[i].get().rank_score[j] <<
						" (���� "<< j + 1 << "�� Ƚ�� : " << Gamblers[i].get().tie_rank_score[j] << ")" << std::endl;
				}
			}
		}
		else {
			winscore_print();
		}
	}

	void multi_coin_game() {
		std::random_device rd; //����⸸ �ص� rd() �� ���� ���� ����. �ٵ� ���� ���� rand()���� �ӵ��� ��û �����⿡ �õ������ �� ���� ����
		std::mt19937_64 gen(rd()); //srand()ó�� �õ带 �ޱ⵵ �ϴµ�, �޸��� Ʈ������ �˰����� �̿��ϴ� ���� ������� mt ��� �̸� ����
		std::uniform_int_distribution<int> distribution(0, 1); //��𼭺��� ������ ���� ������ ���� �������� ������ ���ϴ� Ŭ����
		//�� ���� Ŭ���� ��ü�� mt19937 �̶�� ���� ������(���⼭�� gen�̶� ����)�� �������ָ� �ش� ���� �� ���ڰ� ���´�.

		for (int l = 0; l < num_gamblers; ++l) {
			coinResult.initialize_data();
			for (int j = 0; j < num_flip; ++j) {
				bool coin = distribution(gen);
				bool bet = Gamblers[l].get().betting(coinResult);
				//debug
				//std::cout << bet << ":" << coin << ":" << coinResult.num_front << " & " << coinResult.num_back << " | " << Gamblers[l].get().name << std::endl;
				if (bet == coin) {
					++Gamblers[l].get().win;
				}

				coinResult.set_result(coin);
			}
		}
	}

	void one_coin_game() {
		std::random_device rd; //����⸸ �ص� rd() �� ���� ���� ����. �ٵ� ���� ���� rand()���� �ӵ��� ��û �����⿡ �õ������ �� ���� ����
		std::mt19937_64 gen(rd()); //srand()ó�� �õ带 �ޱ⵵ �ϴµ�, �޸��� Ʈ������ �˰����� �̿��ϴ� ���� ������� mt ��� �̸� ����
		std::uniform_int_distribution<int> distribution(0, 1); //��𼭺��� ������ ���� ������ ���� �������� ������ ���ϴ� Ŭ����
		//�� ���� Ŭ���� ��ü�� mt19937 �̶�� ���� ������(���⼭�� gen�̶� ����)�� �������ָ� �ش� ���� �� ���ڰ� ���´�.

		coinResult.initialize_data();
		for (int j = 0; j < num_flip; ++j) {
			bool coin = distribution(gen);

			for (int l = 0; l < num_gamblers; ++l) {
				bool bet = Gamblers[l].get().betting(coinResult);
				//debug
				//std::cout << bet << ":" << coin << ":" << coinResult.num_front << " & " << coinResult.num_back <<" | " << Gamblers[l].get().name << std::endl;
				if (bet == coin) {
					++Gamblers[l].get().win;
				}
			}
			coinResult.set_result(coin);
		}
	}

	void rating_rank() {
		
	}
};

/*template <typename T>
T determine_ranking(int grade, set<T>& s)
{
    auto itr =s.begin();

    std::advance(itr, grade - 1);

    ++(itr->rank.at(grade));

    //return itr->get_win();
    //�̰� �� �����߳� �ߴ���. set�� �ݺ��ڰ� �����ϴ� �� const�� �װɷ� �� �Լ��� const �Լ����߸� ��.
}
*/