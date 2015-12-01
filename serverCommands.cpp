#include <vector>


class userInfo {
  private:
    bool loggedIn;
    std::string name;
    std::string pin;
    int balance;
  public:
    userInfo(std::string n, std::string p, int b) {
      name = n;
      pin = p;
      balance = b;
      loggedIn = false;
    }
    std::string getName() {return name;}
    bool hasName(std::string name) {
      if (this->name.compare(name) == 0) {
        return true;
      }
      return false;
    }
    bool hasPin(std::string pin) {
      if (this->pin.compare(pin) == 0) {
        return true;
      }
      return false;
    }
    bool isLoggedIn() {
      return loggedIn;
    }
    bool Login(std::string pin) {
      if (this->pin.compare(pin) == 0) {
        loggedIn = true;
        return true;
      }
      return false;
    }
};

class userDB {
  private:
    std::vector<userInfo> users;
  public:
    userDB() {}
    void addUser(userInfo user) {
      users.push_back(user);
    }
    bool loginUser(std::string name, std::string pin) {
      for (int i = 0; i ++; i < users.size()) {
        userInfo user = users[i];
        if (user.hasName(name)) {
          return user.Login(pin);
        }
      }
      return false;
    }
};
void init_bank(userDB* users) {
  userInfo a = userInfo("alice", "abc123", 50);
  userInfo b = userInfo("bob", "def456", 100);
  userInfo e = userInfo("eve", "ghi789", 0);
  users->addUser(a);
  users->addUser(b);
  users->addUser(e);
}

void deposit(const std::string* user, unsigned long amount);
void balance(const std::string* user);

void login(const std::string* user, int pin);
void balance(const std::string* user);
void amount(const std::string* user, int amount);
void transfer(const std::string* user, int amount, const std::string* otherUser);
void logout(std::string user);

void deposit(const std::string* user, unsigned long amount) {
  std::cout << "dtest" << std::endl;
}

void balance(const std::string* user) {
  std::cout << "btest" << std::endl;
}

void login(const std::string* user, int pin) {

}
