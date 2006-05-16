#ifndef BOOST_RTL_PERFMETER_HPP_INCLUDED
#define BOOST_RTL_PERFMETER_HPP_INCLUDED

class Perfmeter
{
public:
	Perfmeter() : v_(0) {default_counter++;}
	Perfmeter(int v) : v_(v) {}
	Perfmeter(const Perfmeter& other) : v_(other.v_) 
	{
		copy_counter++;
	}
	bool operator < (const Perfmeter& other) const 
	{
		comp_counter++;
		return (v_ < other.v_);
	}
	Perfmeter& operator=(const Perfmeter& other) 
	{
		assign_counter++;
		v_ = other.v_;
		return *this;
	}
	operator int() const {return v_;}
	Perfmeter& operator+=(const Perfmeter& other)
	{
		v_ += other.v_;
		return *this;
	}
	static int getDeftCount() {return default_counter;}
	static int getCopyCount() {return copy_counter;}
	static int getCompCount() {return comp_counter;}
	static int getAsgnCount() {return assign_counter;}
public:
	int v_;
private:
	static int default_counter;
	static int copy_counter;
	static int comp_counter;
	static int assign_counter;
};

#endif//BOOST_RTL_PERFMETER_HPP_INCLUDED
