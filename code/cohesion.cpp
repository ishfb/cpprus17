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
	static Event Payment(int rubles);
	static Event PhoneCall(int minutes);

	bool IsPayment() const;
	int GetMoney() const;
	int GetDuration() const;
};

struct Tariff {
	int costPerMinute_;

	int GetCost(int minutes) const {
		return minutes * costPerMinute_;
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

struct UserStats {
	void AddPhoneCall(int minutes, int cost) {
		++callCount_;
		totalDuration_ += minutes;
		moneySpent_ += cost;
	}

	void PrintAsJson() const {
		cout << "{"
			<< R"("callCount":)" << callCount_ << ","
			<< R"("totalDuration":)" << totalDuration_ << ","
			<< R"("moneySpent":)" << moneySpent_
			<< "}";
	}

	int callCount_ = 0;
	int totalDuration_ = 0;
	int moneySpent_ = 0;
};

struct UserInfo {
	int tariff_ = 0;
	UserAccount account_;
	UserStats stats_;
	vector<Event> history_;
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

		int charge = t.GetCost(minutes);
		user.account_.ApplyCharge(charge);
		user.history_.push_back(Event::PhoneCall(minutes));

		if (user.account_.money_ <= 0) {
			SendSms(userId, "Your balance is " + to_string(user.account_.money_));
		}

		user.stats_.AddPhoneCall(minutes, charge);
	}

	void AddMoney(int userId, int rubles) {
		users_[userId].account_.ApplyCharge(rubles);
		users_[userId].history_.push_back(Event::Payment(rubles));
		SendSms(userId, "Got payment " + to_string(rubles));
	}

	void PrintStatsAsJson() {
		cout << "[";
		for (size_t i = 0; i < users_.size(); ++i) {
			users_[i].stats_.PrintAsJson();
			if (i + 1 < users_.size()) {
				cout << ",";
			}
		}
		cout << "]";
	}

private:
	void SendSms(int userId, string message);

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

vector<UserInfo> LoadUsers(string db);
vector<Tariff> LoadTariffs(string db);

void MyTool() {
	const Tariff newTariff{2};

	auto users = LoadUsers("users.sql");
	int negativeBalanceCount = 0;
	for (const auto& u : users) {
		UserAccount ua{200};
		UserStats stats;
		for (const Event& e : u.history_) {
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
	b.AddMoney(0, 10);
	b.PrintStatsAsJson();
	cout << "Billing is OK" << endl;

	TestPaymentDoesntIncreaseMoney();
	TestNoChargeForZeroMinutes();
	cout << "Test are OK" << endl;

	MyTool();

	return 0;
}

Event Event::Payment(int rubles) { return {}; }
Event Event::PhoneCall(int minutes) { return {}; }

bool Event::IsPayment() const { return false; }
int Event::GetMoney() const { return 5; }
int Event::GetDuration() const { return 3; }

void Billing::SendSms(int userId, string message) {
}

vector<UserInfo> LoadUsers(string) {
	UserInfo ui;
	return {ui};
}

vector<Tariff> LoadTariffs(string db) {
	return {Tariff{5}};
}

