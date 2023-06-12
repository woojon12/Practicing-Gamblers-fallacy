#pragma once
#include "Gambler.h"
#include <random>
#include <vector>
#include <cstdarg>
#include <functional>

class Dealer {
	int num_flip; //한 게임 당 동전 던지는 횟수
	int num_gamblers; //몇 명이서
	int num_games; //몇 게임을 할 것인가
	bool multi_coin; //동전 던지기를 각자 할지(true) 하나의 동전으로 같이 할지(false).
	std::vector<std::reference_wrapper<Gambler>> Gamblers;
	CoinResult coinResult;

public:
	Dealer(bool multi_coin, int num_gamblers, ...)
		: multi_coin(multi_coin), num_gamblers(num_gamblers) {

		//Gamblers.resize(num_gamblers);
		//어째선지 이걸 하면 C2512라고 뭐 생성자 없다는 오류가 발생한다. 아마 이 안에서 생성자를 뭐 어떻게 쓰는 듯?
		//그래서 그냥 이거없이 push_back 만 해야함.

		std::va_list Gambler_args;
		va_start(Gambler_args, num_gamblers);

		for (int i = 0; i < num_gamblers; ++i) { 
			auto v = std::vector<int>(num_gamblers, 0);
			Gamblers.push_back(std::ref(va_arg(Gambler_args, Gambler))); 
			Gamblers[i].get().rank_score = v;
			Gamblers[i].get().tie_rank_score = v;
		}

		va_end(Gambler_args);
		//std::cout << "Debug 이 함수를 나가면 name이 죽는다. : " << Gamblers[0].get().name << std::endl;
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
			ref_wrp.get().win = 0; //debug? 이걸 없애도 의미있는 결과기는 함.
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

			//#한 게임 결과 정산

			std::vector<std::pair<int, int>> index_and_winscore;
			//vector의 인덱스는 순위,
			//pair의 first는 각 겜블러를 가리키는 숫자, second는 겜블러의 득점수
			for (int i = 0; i < num_gamblers; ++i) {
				index_and_winscore.push_back(std::pair<int, int>(i, Gamblers[i].get().win));
			}
			for (int l = 0; l < num_gamblers - 1; ++l) { //버블 정렬
				for (int n = 0; n < num_gamblers - l - 1; ++n) {
					if (index_and_winscore[n].second < index_and_winscore[n + 1].second) {
						auto temp = index_and_winscore[n + 1];
						index_and_winscore[n + 1] = index_and_winscore[n];
						index_and_winscore[n] = temp;
					}
				}
			}
			//TODO(Done) : 공동 1위, 공동 2위도 제작할 것.

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
					//std::cout << Gamblers[index_and_winscore[human_index].first].get().name << "가 " << current_handle_rank + 1 << "위를 " << Gamblers[human_index].get().rank_score[current_handle_rank] << "번 했다" << std::endl;
					++human_index;
				}
				else {
					for (int i = human_index; i < j + 1; ++i) {
						++Gamblers[index_and_winscore[i].first].get().tie_rank_score[current_handle_rank];
						/*
						단독 1등보다는 공동 1등의 가치가 더 낮고, 그런 차이가 있으니 그냥 분리만 해놓는거임.
						더 정밀하게 하려면 누구랑 비겨서 공동 n위가 됐는지까지 다 정리해야겠지만 그건 보는 입장에서도 머리 아프니
						일단 공동과 단독만 구분되도록 만듦.
						*/
					}

					//debug
					/*std::cout << "공동 " << current_handle_rank + 1 << "등 : " << Gamblers[index_and_winscore[human_index].first].get().name;
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

		//#모든 게임 결과 출력

		if (num_games != 1) {
			for (int i = 0; i < num_gamblers; ++i) {
				std::cout << Gamblers[i].get().name << std::endl;

				for (int j = 0; j < num_gamblers; ++j) {
					std::cout << "\t" << "단독 " << j + 1 << "위 횟수 : " << Gamblers[i].get().rank_score[j] <<
						" (공동 "<< j + 1 << "등 횟수 : " << Gamblers[i].get().tie_rank_score[j] << ")" << std::endl;
				}
			}
		}
		else {
			winscore_print();
		}
	}

	void multi_coin_game() {
		std::random_device rd; //만들기만 해도 rd() 로 난수 생성 가능. 근데 흔히 쓰는 rand()보다 속도가 엄청 느리기에 시드용으로 한 번만 생성
		std::mt19937_64 gen(rd()); //srand()처럼 시드를 받기도 하는데, 메르센 트위스터 알고리즘을 이용하는 난수 생성기라서 mt 라는 이름 붙음
		std::uniform_int_distribution<int> distribution(0, 1); //어디서부터 어디까지 범위 내에서 숫자 뽑을건지 분포를 정하는 클래스
		//이 분포 클래스 객체에 mt19937 이라는 난수 생성기(여기서는 gen이란 변수)를 전달해주면 해당 범위 내 숫자가 나온다.

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
		std::random_device rd; //만들기만 해도 rd() 로 난수 생성 가능. 근데 흔히 쓰는 rand()보다 속도가 엄청 느리기에 시드용으로 한 번만 생성
		std::mt19937_64 gen(rd()); //srand()처럼 시드를 받기도 하는데, 메르센 트위스터 알고리즘을 이용하는 난수 생성기라서 mt 라는 이름 붙음
		std::uniform_int_distribution<int> distribution(0, 1); //어디서부터 어디까지 범위 내에서 숫자 뽑을건지 분포를 정하는 클래스
		//이 분포 클래스 객체에 mt19937 이라는 난수 생성기(여기서는 gen이란 변수)를 전달해주면 해당 범위 내 숫자가 나온다.

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
    //이거 왜 오류뜨나 했더니. set의 반복자가 리턴하는 건 const라서 그걸로 쓸 함수가 const 함수여야만 함.
}
*/