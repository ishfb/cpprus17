#include <iostream>
#include <cassert>
#include <sstream>
#include <vector>
#include <string>
#include <map>

using namespace std;

using UserId = int;
using RegionId = int;

struct Tariff {
	int costPerMinute;
	int useCount = 0;

	int GetCharge(int duration) const {
		return costPerMinute * duration;
	}
};

struct UserAccount {
	int money;
	int tariff;
	int failCount = 0;
	int totalDuration = 0;
};

void ApplyCharge(UserAccount& ua, int charge) {
	ua.money -= charge;
}

vector<UserAccount> LoadUserAccounts(string db) {
	return vector<UserAccount>(1);
}

vector<Tariff> LoadTariffs(string db) {
	return vector<Tariff>(1);
}

class Billing {
public:
	Billing(vector<UserAccount> users, vector<Tariff> tariffs);

	void ChargePhoneCall(UserId user, int duration);
	void AddMoney(UserId user, int money);
	void PrintStatsAsXml(ostream& os);

private:
	void SendSms(UserId user, string text);

	vector<UserAccount> users;
	vector<Tariff> tariffs;
	int smsSent = 0;
};

Billing::Billing(vector<UserAccount> users, vector<Tariff> tariffs)
	: users(std::move(users))
	, tariffs(std::move(tariffs))
{
}

void Billing::ChargePhoneCall(UserId user, int duration) {
	auto& u = users[user];
	if (u.money <= 0) {
		u.failCount++;
		return;
	}

	ApplyCharge(u, tariffs[u.tariff].GetCharge(duration));
	if (u.money <= 0) {
		SendSms(user, "Account is blocked");
	}

	tariffs[u.tariff].useCount++;
	u.totalDuration += duration;
}

void Billing::AddMoney(UserId user, int money) {
	auto& u = users[user];

	u.money += money;
	SendSms(user, "Got " + to_string(money) + " tugrics");
}

void Billing::SendSms(int user, string text) {
	smsSent++;
}

void Billing::PrintStatsAsXml(ostream& os) {
	os << "<sms>" << smsSent << "</sms>";
	os << "<users>";
	for (const auto& u : users) {
		os << "<total_duration>" << u.totalDuration << "</total_duration>";
	}
	os << "</users>";
}

void TestPhoneCallDoesntIncreaseMoney() {
	const Tariff mostPopular { 1 };
	UserAccount ua;
	ua.money = 10;

	ApplyCharge(ua, mostPopular.GetCharge(5));
	assert(ua.money <= 10);
}

void TestNoCallNoCharge() {
	const Tariff mostPopular { 1 };
	UserAccount ua;
	ua.money = 10;

	ApplyCharge(ua, mostPopular.GetCharge(0));
	assert(ua.money == 10);
}

void MyTool() {
	const Tariff experimental{12};

	for (auto& u : LoadUserAccounts("users.sql")) {
		u.tariff = 0;
		Billing billing({u}, {experimental});

		vector<int> history;
		for (auto duration : history) {
			billing.ChargePhoneCall(0, duration); // Sends SMS
		}

		billing.PrintStatsAsXml(cout); // Have to parse XML
	}
}

int main() {
	Billing billing(LoadUserAccounts("users.sql"), LoadTariffs("tariffs.sql"));
	billing.ChargePhoneCall(0, 15);
	billing.AddMoney(0, 150);

	TestPhoneCallDoesntIncreaseMoney();
	TestNoCallNoCharge();
	cout << "Tests are OK";

	MyTool();
	return 0;
}
