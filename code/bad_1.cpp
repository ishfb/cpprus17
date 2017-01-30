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

private:
    struct Tariff {
        int costPerMinute;
    };

    struct UserAccount {
        int money;
        int tariff;
    };

    void LoadUserAccounts(string db) { users.resize(1); }
    void LoadTariffs(string db) { tariffs.resize(1); }
    void SendSms(UserId user, string text) {}

    vector<UserAccount> users;
    vector<Tariff> tariffs;
};

Billing::Billing(string userAccountDatabase, string tariffsDatabase) {
    LoadUserAccounts(userAccountDatabase);
    LoadTariffs(tariffsDatabase);
}

void Billing::ChargePhoneCall(UserId user, int duration) {
    auto& u = users[user];
    u.money -= tariffs[u.tariff].costPerMinute * duration;
    if (users[user].money <= 0) {
        SendSms(user, "Account is blocked");
    }
}

void Billing::AddMoney(UserId user, int money) {
    auto& u = users[user];

    u.money += money;
    SendSms(user, "Got " + to_string(money) + " tugrics");
}

int main() {
    Billing billing("", "");
    billing.ChargePhoneCall(0, 15);
    billing.AddMoney(0, 150);
    return 0;
}
