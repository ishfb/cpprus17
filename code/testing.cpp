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

class Billing {
public:
    Billing(string usersDatabase, string tariffsDatabase);

    void ChargePhoneCall(int userId, int minutes);
    void AddMoney(int userId, int rubles);

private: // class Billing
    void LoadUsers(string db);
    void LoadTariffs(string db);

    class UserInfo {
    public:
        void ChargePhoneCall(int minutes, const Tariff& tariff);
        void AddMoney(int rubles);
        int GetTariff() const;
    private:
        class Event;
        void SendSms(string message);

        int tariff_;
        UserAccount account_;

        vector<Event> history_;
    };  // class Billing::UserInfo

    vector<UserInfo> users_;
    vector<Tariff> tariffs_;
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
}

void Billing::AddMoney(int userId, int rubles) {
    users_[userId].AddMoney(rubles);
}

void TestTalkMorePayMore() {
    const Tariff tariff{1};
    assert(tariff.GetCost(15) > tariff.GetCost(5));
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
    TestTalkMorePayMore();
    TestPayment();
    cout << "Tests OK\n";

    return 0;
}

void Billing::UserInfo::SendSms(string message) {
}

void Billing::UserInfo::ChargePhoneCall(int minutes, const Tariff& tariff) {
    account_.ApplyCharge(tariff.GetCost(minutes));
    history_.push_back(Event::PhoneCall(minutes));

    if (account_.money_ <= 0) {
        SendSms("Your balance is " + to_string(account_.money_));
    }
}

void Billing::UserInfo::AddMoney(int rubles) {
    account_.ApplyPayment(rubles);
    history_.push_back(Event::Payment(rubles));
    SendSms("Got payment " + to_string(rubles));
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
void Billing::LoadTariffs(string db) {
    tariffs_.resize(1);
}

