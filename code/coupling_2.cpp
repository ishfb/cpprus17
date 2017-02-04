#include <iostream>
#include <cassert>
#include <sstream>
#include <vector>
#include <string>
#include <map>

using namespace std;

/*
 * Хотим:
 * - снимать деньги за звонок
 * - отправлять SMS, если вышел в минус
 * - считать статистику:
 *   - суммарное время звонков
 *   - количество звонков
 * - пополнять счёт
 * - слать SMS о пополнении
 * - сохранять историю всех операций
 *
 * - написать инструмент, который будет воспроизводить историю пользователя
 * на экспериментальном тарифе
 *   - история поль-ля обновляться не должна
 *   - статистика должна считаться
 *
 * - хотим покрыть тестами применение тарифа
 * - хотим покрыть тестами подсчёт статистики
 * */

class Billing {
public:
	Billing(string usersDatabase, string tariffsDatabase) {
		LoadUsers(usersDatabase);
		LoadTariffs(tariffsDatabase);
	}

	void ChargePhoneCall(int userId, int minutes) {
		UserInfo& user = users_[userId];
		const Tariff& t = tariffs_[user.GetTariff()];
		user.ChargePhoneCall(minutes, t);
	}

	void AddMoney(int userId, int rubles) {
		users_[userId].AddMoney(rubles);
	}

	void PrintStatsAsJson() {
		cout << "[";
		for (size_t i = 0; i < users_.size(); ++i) {
			users_[i].PrintStatsAsJson();
			if (i + 1 < users_.size()) {
				cout << ",";
			}
		}
		cout << "]";
	}

private:
	void LoadUsers(string db);
	void LoadTariffs(string db);

	class Event {
	public:
		static Event Payment(int rubles);
		static Event PhoneCall(int minutes);

		bool IsPayment() const;
		int GetMoney() const;
		int GetDuration() const;
	};

	struct Tariff {
		int costPerMinute_;
	};

	class UserInfo {
	public:
		void ChargePhoneCall(int minutes, const Tariff& tariff) {
			int charge = minutes * tariff.costPerMinute_;
			money_ -= charge;
			history_.push_back(Event::PhoneCall(minutes));

			if (money_ <= 0) {
				SendSms("Your balance is " + to_string(money_));
			}

			moneySpent_ += charge;
			totalDuration_ += minutes;
			++callCount_;
		}

		void AddMoney(int rubles) {
			money_ += rubles;
			history_.push_back(Event::Payment(rubles));
			SendSms("Got payment " + to_string(rubles));
		}

		int GetTariff() const {
			return tariff_;
		}

		void PrintStatsAsJson() {
			cout << "{"
				<< R"("callCount":)" << callCount_ << ","
				<< R"("totalDuration":)" << totalDuration_ << ","
				<< R"("moneySpent":)" << moneySpent_
				<< "}";
		}

	private:
		void SendSms(string message);

		int money_;
		int tariff_;
		int callCount_;
		int totalDuration_;
		int moneySpent_;
		vector<Event> history_;
	};

	vector<UserInfo> users_;
	vector<Tariff> tariffs_;
};

int main() {
	Billing b("users.sql", "tariffs.sql");
	b.ChargePhoneCall(0, 2);
	b.AddMoney(0, 10);
	b.PrintStatsAsJson();

	return 0;
}

void Billing::UserInfo::SendSms(string message) {
}

Billing::Event Billing::Event::Payment(int rubles) { return {}; }
Billing::Event Billing::Event::PhoneCall(int minutes) { return {}; }
bool Billing::Event::IsPayment() const { return false; }
int Billing::Event::GetMoney() const { return 5; }
int Billing::Event::GetDuration() const { return 3; }

void Billing::LoadUsers(string db) { users_.resize(1); }
void Billing::LoadTariffs(string db) { tariffs_.resize(1); }

