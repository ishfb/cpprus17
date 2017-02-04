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

private:
	void LoadUsers(string db) { users_.resize(1); }
	void LoadTariffs(string db) { tariffs_.resize(1); }

	class Event {
	public:
		static Event Payment(int rubles) { return {}; }
		static Event PhoneCall(int minutes) { return {}; }

		bool IsPayment() const { return false; }
		int GetMoney() const { return 5; }
		int GetDuration() const { return 3; }
	};

	struct Tariff {
		int costPerMinute_;
	};

	class UserInfo {
	public:
		void ChargePhoneCall(int minutes, const Tariff& tariff) {
			money_ -= minutes * tariff.costPerMinute_;
			history_.push_back(Event::PhoneCall(minutes));

			if (money_ <= 0) {
				SendSms("Your balance is " + to_string(money_));
			}
		}

		void AddMoney(int rubles) {
			money_ += rubles;
			history_.push_back(Event::Payment(rubles));
			SendSms("Got payment " + to_string(rubles));
		}

		int GetTariff() const {
			return tariff_;
		}

	private:
		void SendSms(string /*message*/) {
		}

		int money_;
		int tariff_;
		vector<Event> history_;
	};

	vector<UserInfo> users_;
	vector<Tariff> tariffs_;
};

int main() {
	Billing b("users.sql", "tariffs.sql");
	b.ChargePhoneCall(0, 2);
	b.AddMoney(0, 10);

	return 0;
}
