#include <cassert>
#include <iostream>
#include <sstream>
#include <vector>
#include <string>
#include <map>

using namespace std;

template<class T>
string to_string(T x) {
    ostringstream os;
    os << x;
    return os.str();
}

using UserId = int;
using RegionId = int;

class History {
public:
    void AddCall(int duration) {
    }

    void AddPayment(int money) {
    }

    void AddBlock() {
    }

    void AddUnblock() {
    }

    vector<int> GetCalls() const { return {}; }
};

struct UserAccount {
    int money;
    int tariff;
    History history;
    int totalDuration;
    int blockCount;
};

struct Tariff {
    int costPerMinute;
    int useCount = 0;
    int totalDuration = 0;

    void ApplyTo(UserAccount& ua, int duration) const {
        ua.money -= duration * costPerMinute;
    }
};

vector<UserAccount> LoadUserDatabase(string db) {
    return {};
}

vector<Tariff> LoadTariffDatabase(string db) {
    return {};
}

class UserAccounts {
public:
    UserAccounts(vector<UserAccount> u, vector<Tariff> t) : users(u), tariffs(t) {
    }

    void ChargePhoneCall(UserId user, int duration) {
        auto& u = users[user];

        tariffs[u.tariff].ApplyTo(u, duration);

        u.history.AddCall(duration);
        ++tariffs[u.tariff].useCount;
        tariffs[u.tariff].totalDuration += duration;
        u.totalDuration += duration;
        if (users[user].money <= 0) {
            SendSms(user, "Account is blocked");
            u.history.AddBlock();
            ++u.blockCount;
        }
    }

    void AddMoney(UserId user, int money) {
        auto& u = users[user];

        u.money += money;
        SendSms(user, "Got " + to_string(money) + " tugrics");
        u.history.AddPayment(money);
    }

    void SendSms(UserId user, string text) {
    }

private:
    vector<UserAccount> users;
    vector<Tariff> tariffs;
};

void TestMoneyDoesntGrowAfterCall() {
    UserAccount ua;
    ua.money = 50;

    Tariff superCheap;
    superCheap.costPerMinute = 1;

    superCheap.ApplyTo(ua, 10);
    assert(ua.money <= 50);
}

void TestNoCallNoCharge() {
    Tariff t;
    t.costPerMinute = 5;
    UserAccount ua;
    ua.money = 1;

    t.ApplyTo(ua, 0);
    assert(ua.money == 1);
}

void InvestigateNewTariff(const Tariff& t) {
    auto users = LoadUserDatabase("users.sql");

    for (const auto& user : users) {
        UserAccount ua;
        ua.money = 500;

        for (auto duration : user.history.GetCalls()) {
            t.ApplyTo(ua, duration);
        }

        cout << ua.money;
    }
}

int main() {
    TestMoneyDoesntGrowAfterCall();
    TestNoCallNoCharge();

    UserAccounts uas{LoadUserDatabase("users.sql"), LoadTariffDatabase("tariffs.sql")};
    return 0;
}
