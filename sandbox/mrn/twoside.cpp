// sandbox: does NOT care about leaks
#include <iostream>
#include <algorithm>
#include <vector>

using namespace std;

double const special = 77777;

struct node {
  node() : value(special) {}

  void add_child(node &child) {
    children.push_back(&child);
    child.parent = this;
  }

  vector<node*> children;
  node *parent;
  char op;
  double value;

  void print() {
    cout << op;
    if (value != special)
      cout << value;
    if (children.empty())
      return;
    cout << '(';
    for (vector<node*>::iterator it = children.begin(); it != children.end(); ++it) {
      if (it != children.begin())
        cout << ',';
      (*it)->print();
    }
    cout << ')';
  }
};

int main() {
  node container;
  container.op = '|';
  node eq;
  eq.op = '=';
  container.add_child(eq);
  node lhs;
  lhs.op = '+';
  eq.add_child(lhs);
  node v1;
  v1.op = ' ';
  v1.value = 1;
  lhs.add_child(v1);
  node x;
  x.op = ' ';
  x.value = 0.6;
  lhs.add_child(x);
  node rhs;
  rhs.op = '$';
  rhs.value = 2;
  eq.add_child(rhs);
  container.print();
  cout << endl;
}
