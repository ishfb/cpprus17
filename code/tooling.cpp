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
    int moneySpent_ = 0;

    void AddExpense(int rubles) {
        moneySpent_ += rubles;
    }
};

class Event {
public:
    static Event Payment(int rubles);
    static Event PhoneCall(int minutes);

    bool IsPayment() const;
    int GetMoney() const;
    int GetDuration() const;
};

class UserInfo {
public:
    void ChargePhoneCall(int minutes, const Tariff& tariff);
    void AddMoney(int rubles);
    int GetTariff() const;
    const UserAccount& GetAccount() const { return account_; }
    const UserStats& GetStats() const { return stats_; }
    const vector<Event>& GetHistory() const { return history_; }

private:
    int tariff_;
    UserStats stats_;
    UserAccount account_;
    vector<Event> history_ = {Event{}, Event{}};
};  // class Billing::UserInfo

vector<UserInfo> LoadUsers(string db);
vector<Tariff> LoadTariffs(string db);

class Billing {
public:
    Billing(string usersDatabase, string tariffsDatabase)
        : users_(LoadUsers(usersDatabase))
        , tariffs_(LoadTariffs(tariffsDatabase))
    {}

    void ChargePhoneCall(int userId, int minutes);
    void AddMoney(int userId, int rubles);
    void PrintStatsAsXml();

private: // class Billing
    void SendSms(int userId, string message);

    vector<UserInfo> users_;
    vector<Tariff> tariffs_;
    int smsSent_ = 0;
};  // class Billing

void Billing::ChargePhoneCall(int userId, int minutes) {
    UserInfo& user = users_[userId];
    const Tariff& t = tariffs_[user.GetTariff()];

    user.ChargePhoneCall(minutes, t);

    const int money = user.GetAccount().money_;
    if (money <= 0) {
        SendSms(userId, "Your balance is " + to_string(money));
    }
}

void Billing::AddMoney(int userId, int rubles) {
    users_[userId].AddMoney(rubles);
    SendSms(userId, "Got payment " + to_string(rubles));
}

void TestChargeDoesntIncreaseMoney() {
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

void TestPayment() {
    UserAccount ua{15};

    ua.ApplyPayment(10);
    assert(ua.money_ == 25);
}

void InvestigateNewTariff() {
    const Tariff newTariff{2};
    vector<UserInfo> users = LoadUsers("users.sql");
    int negativeBalanceCount = 0;
    for (const UserInfo& realUser : users) {
        UserAccount ua{500};
        UserStats stats;
        for (const Event& e : realUser.GetHistory()) {
            if (e.IsPayment()) {
                ua.ApplyPayment(e.GetMoney());
            } else {
                int cost = newTariff.GetCost(e.GetDuration());
                ua.ApplyCharge(cost);
                stats.AddExpense(cost);
            }
            if (ua.money_ < 0) {
                ++negativeBalanceCount;
            }
        }
        cout << stats.moneySpent_ << endl;
    }
    cout << negativeBalanceCount << endl;
}



int main() {
    Billing b("users.sql", "tariffs.sql");
    b.ChargePhoneCall(0, 2);
    b.AddMoney(0, 10);
    cout << "Production OK\n";

    TestNoChargeForZeroMinutes();
    TestChargeDoesntIncreaseMoney();
    TestPayment();
    cout << "Tests OK\n";

    b.PrintStatsAsXml();

    InvestigateNewTariff();

    return 0;
}

void UserInfo::ChargePhoneCall(int minutes, const Tariff& tariff) {
    int cost = tariff.GetCost(minutes);
    account_.ApplyCharge(cost);
    history_.push_back(Event::PhoneCall(minutes));

    stats_.AddExpense(cost);
}

void UserInfo::AddMoney(int rubles) {
    account_.ApplyPayment(rubles);
    history_.push_back(Event::Payment(rubles));
}

int UserInfo::GetTariff() const {
    return tariff_;
}

Event Event::Payment(int rubles) {
    return {};
}
Event Event::PhoneCall(int minutes) {
    return {};
}
bool Event::IsPayment() const {
    return false;
}
int Event::GetMoney() const {
    return 5;
}
int Event::GetDuration() const {
    return 3;
}

vector<UserInfo> LoadUsers(string db) {
    return vector<UserInfo>(1);
}

void Billing::PrintStatsAsXml() {
    cout << "<sms>" << smsSent_ << "</sms>";
    for (const UserInfo& u : users_) {
        cout << "<spent>" << u.GetStats().moneySpent_ << "</spent>";
    }
}

vector<Tariff> LoadTariffs(string db) {
    return vector<Tariff>(1);
}

void Billing::SendSms(int userId, string message) {
    ++smsSent_;
    // ...
}
