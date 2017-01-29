#include <iostream>
#include <vector>
#include <string>
#include <map>
using namespace std;

using UserId = int;
using RegionId = int;

struct Tariff {
  int costPerMinute;
};

class History {
public:
  void AddCall(int duration) {}
  void AddPayment(int money) {}
  void AddBlock() {}
  void AddUnblock() {}
};

struct UserAccount {
  int money;
  int tariff;
  History history;
};

class UserAccounts {
public:
  UserAccounts(string userAccountsDatabase, string tariffsDatabase, RegionId region) {}

  void ChargePhoneCall(UserId user, int duration) {
    auto& u = users[user];

    u.money -= tariffs[u.tariff].costPerMinute * duration;
    u.history.AddCall(duration);
    if (users[user].money <= 0) {
      SendSms(user, "Account is blocked");
      u.history.AddBlock();
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

int main() {
  UserAccounts ua("", "", 1);
  return 0;
}
