#ifndef GOTT_MATH_COLOR_HPP_INCLUDED
#define GOTT_MATH_COLOR_HPP_INCLUDED
#include <limits>

// TODO: signed types?
namespace gott{ namespace math {

template<typename ChannelType>
struct colour_channel_traits
{
	static ChannelType max()  { return std::numeric_limits<ChannelType>::max(); }
};

template<>
struct colour_channel_traits<long double>
{
	static long double max()  { return 1.0; }
};

template<>
struct colour_channel_traits<float>
{
	static float max()  { return 1.0; }
};


template<>
struct colour_channel_traits<double>
{
	static double max()  { return 1.0; }
};

}}
#endif

