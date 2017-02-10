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
    Billing(string usersDatabase, string tariffsDatabase);

    void ChargePhoneCall(int userId, int minutes);
    void AddMoney(int userId, int rubles);

private: // class Billing
    void LoadUsers(string db);
    void LoadTariffs(string db);

    struct Tariff {
        int costPerMinute_;
    };

    class UserInfo {
    public:
        void ChargePhoneCall(int minutes, const Tariff& tariff);
        void AddMoney(int rubles);
        int GetTariff() const;
    private:
        class Event;
        void SendSms(string message);
        int money_;
        int tariff_;
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

int main() {
    Billing b("users.sql", "tariffs.sql");
    b.ChargePhoneCall(0, 2);
    b.AddMoney(0, 10);

    return 0;
}

void Billing::UserInfo::SendSms(string message) {
}

void Billing::UserInfo::ChargePhoneCall(int minutes, const Billing::Tariff& tariff) {
    money_ -= minutes * tariff.costPerMinute_;
    history_.push_back(Event::PhoneCall(minutes));

    if (money_ <= 0) {
        SendSms("Your balance is " + to_string(money_));
    }
}

void Billing::UserInfo::AddMoney(int rubles) {
    money_ += rubles;
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

