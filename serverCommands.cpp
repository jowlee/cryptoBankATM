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
