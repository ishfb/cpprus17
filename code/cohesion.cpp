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

class Event {
public:
	bool IsPayment() const { return false; }
	int GetMoney() const { return 5; }
	int GetDuration() const { return 3; }
};

class UserHistory {
public:
	const vector<Event>& GetEvents() const {
		static const vector<Event> res;
		return res;
	}

	void AddPhoneCall(int duration, int cost) {}
	void AddPayment(int rubles) {}
};

struct UserStats {
	int callCount_ = 0;
	int totalDuration_ = 0;
	int moneySpent_ = 0;

	void AddPhoneCall(int minutes, int cost) {
		++callCount_;
		totalDuration_ += minutes;
		moneySpent_ += cost;
	}
};

struct UserAccount {
	int money_ = 0;

	void ApplyCharge(int rubles) {
		money_ -= rubles;
	}

	void ApplyPayment(int rubles) {
		money_ += rubles;
	}
};

struct UserInfo {
	int tariff_;
	UserAccount account_;
	UserHistory history_;
	UserStats stats_;
};

struct Tariff {
	int costPerMinute_;

	int GetCost(int minutes) const {
		return costPerMinute_ * minutes;
	}
};

class Billing {
public:
	Billing(vector<UserInfo> users, vector<Tariff> tariffs)
		: users_(std::move(users))
		, tariffs_(std::move(tariffs))
	{
	}

	void ChargePhoneCall(int userId, int minutes) {
		UserInfo& user = users_[userId];
		const Tariff& t = tariffs_[user.tariff_];

		int cost = t.GetCost(minutes);
		user.account_.ApplyCharge(cost);
		user.history_.AddPhoneCall(minutes, cost);
		user.stats_.AddPhoneCall(minutes, cost);

		if (user.account_.money_ <= 0) {
			SendSms(userId, "Your balance is " + to_string(user.account_.money_));
		}
	}

	void AddMoney(int userId, int rubles) {
		UserInfo& user = users_[userId];

		user.account_.ApplyPayment(rubles);
		user.history_.AddPayment(rubles);

		SendSms(userId, "Got payment " + to_string(rubles));
	}

private:
	void SendSms(int /*userId*/, string /*message*/) {
	}

	vector<UserInfo> users_;
	vector<Tariff> tariffs_;
};

void TestPaymentDoesntIncreaseMoney() {
	UserAccount ua{15};
	ua.ApplyCharge(10);
	assert(ua.money_ <= 15);
}

void TestNoChargeForZeroMinutes() {
	UserAccount ua{15};
	const Tariff tariff{10};

	ua.ApplyCharge(tariff.GetCost(0));
	assert(ua.money_ == 15);
}

vector<UserInfo> LoadUsers(string /*db*/) {
	UserInfo ui{0};
	return {ui};
}

vector<Tariff> LoadTariffs(string /*db*/) {
	return {Tariff{5}};
}

void MyTool() {
	const Tariff newTariff{2};

	auto users = LoadUsers("users.sql");
	int negativeBalanceCount = 0;
	for (const auto& u : users) {
		UserAccount ua{200};
		UserStats stats;
		for (const Event& e : u.history_.GetEvents()) {
			if (e.IsPayment()) {
				ua.ApplyPayment(e.GetMoney());
			} else {
				int cost = newTariff.GetCost(e.GetDuration());
				ua.ApplyCharge(cost);
				stats.AddPhoneCall(e.GetDuration(), cost);
			}
		}
		if (ua.money_ < 0) {
			++negativeBalanceCount;
		}
		cout << stats.moneySpent_ << endl;
	}

	cout << negativeBalanceCount << endl;
}

int main() {
	Billing b(LoadUsers("users.sql"), LoadTariffs("tariffs.sql"));
	b.ChargePhoneCall(0, 2);
	b.AddMoney(0, 50);

	TestPaymentDoesntIncreaseMoney();
	TestNoChargeForZeroMinutes();
	cout << "Test are OK\n";

	MyTool();
	return 0;
}
