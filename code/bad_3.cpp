#include <iostream>
#include <sstream>
#include <vector>
#include <string>
#include <map>

using namespace std;

using UserId = int;
using RegionId = int;

class Billing {
public:
    Billing(string userAccountDatabase, string tariffsDatabase);

    void ChargePhoneCall(UserId user, int duration);
    void AddMoney(UserId user, int money);
    void PrintStatsAsXml(ostream& os);

private:
    struct Tariff {
        int costPerMinute;
        int useCount = 0;
    };

    struct UserAccount {
        int money;
        int tariff;
        int totalDuration = 0;
        int failCount = 0;
    };

    void LoadUserAccounts(string db) { users.resize(1); }
    void LoadTariffs(string db) { tariffs.resize(1); }
    void SendSms(UserId user, string text);

    vector<UserAccount> users;
    vector<Tariff> tariffs;
    int smsSent = 0;
};

Billing::Billing(string userAccountDatabase, string tariffsDatabase) {
    LoadUserAccounts(userAccountDatabase);
    LoadTariffs(tariffsDatabase);
}

void Billing::ChargePhoneCall(UserId user, int duration) {
    if (u.money <= 0) {
        u.failCount++;
        return;
    }

    auto& u = users[user];
    u.money -= tariffs[u.tariff].costPerMinute * duration;
    if (users[user].money <= 0) {
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

int main() {
    Billing billing("", "");
    billing.ChargePhoneCall(0, 15);
    billing.AddMoney(0, 150);
    return 0;
}
