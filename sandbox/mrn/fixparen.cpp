#include <iostream>
using namespace std;

string fix_paren(string const &input) {
  int balance = 0, pre_paren = 0;
  for (string::const_iterator it = input.begin(); it != input.end(); ++it) {
    switch (*it) {
    case '(':
      ++balance;
      break;
    case ')':
      --balance;
      break;
    }
    if (balance < 0) {
      pre_paren += -balance;
      balance = 0;
    }
  }
  return string(pre_paren, '(') + input + string(balance, ')');
}

int main() {
  string s;
  while (getline(cin, s))
    cout << fix_paren(s) << endl;
}
