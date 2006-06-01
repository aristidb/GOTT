// sandbox: does NOT care about leaks
#include <iostream>
#include <algorithm>

using namespace std;

class variable;

class expression {
public:
  virtual ~expression() {}
  virtual void print(ostream &) const = 0;
  virtual bool find_var(variable const &) const = 0;
  virtual bool var_eq(variable const &) const { return false; }
};

ostream &operator<<(ostream &s, expression const &x) {
  x.print(s);
  return s;
}

class unary_expression : public expression {
public:
};

class number : public unary_expression {
public:
  number(double value) : value(value) {}

  void print(ostream &s) const { s << value; }

  bool find_var(variable const &) const { return false; }

private:
  double value;
};

class variable : public unary_expression {
public:
  variable(int id) : id(id) {}

  void print(ostream &s) const { s << '$' << id; }

  bool find_var(variable const &x) const {
    return id == x.id;
  }

  bool var_eq(variable const &x) const {
    return id == x.id;
  }

private:
  int id;
};

class binary_expression : public expression {
public:
  binary_expression(expression *left_p, expression *right_p)
    : left_p(left_p), right_p(right_p) {}

  void print(ostream &s) const {
    s << '(';
    left().print(s);
    print_op(s);
    right().print(s);
    s << ')';
  }

  bool find_var(variable const &x) const {
    return left().find_var(x) || right().find_var(x);
  }

  void make_left(variable const &x) {
    if (!left().find_var(x)) {
      if (!right().find_var(x))
        throw "expected variable not in expression";
      rotate();
    }
  }

  void make_leftest(variable const &x) {
    make_left(x);
    binary_expression *rec = dynamic_cast<binary_expression *>(left_p);
    if (rec)
      rec->make_leftest(x);
  }

  expression &left() { return *left_p; }
  expression const &left() const { return *left_p; }
  expression &right(){ return *right_p; }
  expression const &right() const { return *right_p; }

protected:
  void rotate() {
    swap(left_p, right_p);
  }

  virtual void print_op(ostream &) const = 0;

private:
  expression *left_p;
  expression *right_p;
};

class sum : public binary_expression {
public:
  sum(expression *left_p, expression *right_p)
    : binary_expression(left_p, right_p) {}

  void print_op(ostream &s) const { s << " + "; }
};

class product : public binary_expression {
public:
  product(expression *left_p, expression *right_p)
    : binary_expression(left_p, right_p) {}

  void print_op(ostream &s) const { s << " * "; }
};

class equation : public binary_expression {
public:
  equation(expression *left_p, expression *right_p)
    : binary_expression(left_p, right_p) {}

  bool is_solved(variable const &var) const {
    if (right().find_var(var))
      return false;
    return left().var_eq(var);
  }

  bool solve(variable const &var) {
    make_leftest(var);
    if (is_solved(var))
      return true;
    return false;
  }

  void print_op(ostream &s) const { s << " == "; }
};

void helper(equation *e, variable const &v) {
  if (e->solve(v))
    cout << "succeeded for ";
  else
    cout << "failed for ";
  cout << v << ": " << *e << endl;
}

int main() {
  try {
    equation *s = new equation(new variable(2), new sum(new number(7.1), new variable(1)));
    cout << *s << endl;
    helper(s, 1);
    helper(s, 2);
  } catch(char const *msg) {
    cout << "failed: " << msg << endl;
  }
}
