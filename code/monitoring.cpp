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

class Billing {
public:
    Billing(string usersDatabase, string tariffsDatabase);

    void ChargePhoneCall(int userId, int minutes);
    void AddMoney(int userId, int rubles);
    void PrintStatsAsXml();

private: // class Billing
    void LoadUsers(string db);
    void LoadTariffs(string db);
    void SendSms(int userId, string message);

    class UserInfo {
    public:
        void ChargePhoneCall(int minutes, const Tariff& tariff);
        void AddMoney(int rubles);
        int GetTariff() const;
        const UserAccount& GetAccount() const { return account_; }
        const UserStats& GetStats() const { return stats_; }
    private:
        class Event;

        int tariff_;
        UserStats stats_;
        UserAccount account_;
        vector<Event> history_;
    };  // class Billing::UserInfo

    vector<UserInfo> users_;
    vector<Tariff> tariffs_;
    int smsSent_ = 0;
};  // class Billing

class Billing::UserInfo::Event {
public:
    static Event Payment(int rubles);
    static Event PhoneCall(int minutes);

    bool IsPayment() const;
    int GetMoney() const;
    int GetDuration() const;
};

Billing::Billing(string usersDatabase, string tariffsDatabase) {
    LoadUsers(usersDatabase);
    LoadTariffs(tariffsDatabase);
}

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

    return 0;
}

void Billing::UserInfo::ChargePhoneCall(int minutes, const Tariff& tariff) {
    int cost = tariff.GetCost(minutes);
    account_.ApplyCharge(cost);
    history_.push_back(Event::PhoneCall(minutes));

    stats_.AddExpense(cost);
}

void Billing::UserInfo::AddMoney(int rubles) {
    account_.ApplyPayment(rubles);
    history_.push_back(Event::Payment(rubles));
}

int Billing::UserInfo::GetTariff() const {
    return tariff_;
}

Billing::UserInfo::Event Billing::UserInfo::Event::Payment(int rubles) {
    return {};
}
Billing::UserInfo::Event Billing::UserInfo::Event::PhoneCall(int minutes) {
    return {};
}
bool Billing::UserInfo::Event::IsPayment() const {
    return false;
}
int Billing::UserInfo::Event::GetMoney() const {
    return 5;
}
int Billing::UserInfo::Event::GetDuration() const {
    return 3;
}

void Billing::LoadUsers(string db) {
    users_.resize(1);
}

void Billing::PrintStatsAsXml() {
    cout << "<sms>" << smsSent_ << "</sms>";
    for (const UserInfo& u : users_) {
        cout << "<spent>" << u.GetStats().moneySpent_ << "</spent>";
    }
}

void Billing::LoadTariffs(string db) {
    tariffs_.resize(1);
}

void Billing::SendSms(int userId, string message) {
    ++smsSent_;
    // ...
}
