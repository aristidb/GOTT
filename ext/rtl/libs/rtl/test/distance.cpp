#include "pch.hpp"

#include <boost/rtl/generic.hpp>
#include <math.h>
#include <exception>
#include "test.hpp"
#include <boost/rtl/utils.hpp>
#include <string>
#include <boost/rtl/selection.hpp>
#include <boost/rtl/projection.hpp>
#include <boost/rtl/rename.hpp>
#include <boost/rtl/utils.hpp>
#include "test_function.hpp"
#include <boost/rtl/table.hpp>
#include "table_defs.hpp"
#include <boost/rtl/join_xx.hpp>

using namespace boost;

namespace ds
{
	using namespace boost;
    using namespace rel;
	using namespace unit;

	static const double pi = 3.1415927;
	static const double radius = 6371.0; // in km.

	class EarthCoord
	{
	public:
		EarthCoord(double lat, double lng) : lat_((lat/180.)*pi), lng_((lng/180.)*pi)
		{}

		EarthCoord() : lat_(0.), lng_(0.)
		{}

		double distance(const EarthCoord& other) const
		{
			double s =	cos(lng_)*cos(lat_)*cos(other.lng_)*cos(other.lat_)+
						sin(lng_)*cos(lat_)*sin(other.lng_)*cos(other.lat_)+
						sin(lat_)*sin(other.lat_);
			return radius*acos(s);
		}

		double getLatitude() const {return lat_;}
		double getLongitude() const {return lng_;}
	private:
		double lat_;
		double lng_;
	};

	std::ostream& operator <<(std::ostream& s, const EarthCoord& c)
	{
		return s << c.getLatitude() << " " << c.getLongitude();
	}

	BOOST_RTL_DEFINE_COLUMN(std::string, name);
	BOOST_RTL_DEFINE_COLUMN(EarthCoord, coord);

	typedef boost::mpl::vector2<name,coord> fld_list;
	typedef boost::mpl::vector1<name> srt_list;
	BOOST_RTL_TABLE_2(cities_type,fld_list,srt_list);
	typedef cities_type::value_type tup;

	struct a;
	typedef rel::alias<coord,a> coord1;
	typedef rel::alias<name,a> name1;

	struct distance_less_than
	{
		distance_less_than(double d) : d_(d) {}
		template <class Tuple> bool operator()(const Tuple& tp) const
		{
			return BOOST_RTL_FIELD(tp,coord).distance(BOOST_RTL_FIELD(tp,coord1)) < d_;
		}		
		double d_;
	};


	void foo()
	{
		cities_type cities;
		cities.insert(tup("Miami        ",EarthCoord(25.7739,-80.194)));
		cities.insert(tup("Washington DC",EarthCoord(38.895,-77.0367)));
		cities.insert(tup("Denver       ",EarthCoord(39.7392,-104.984)));
		cities.insert(tup("San Francisco",EarthCoord(37.775,-122.418)));
		cities.insert(tup("New York     ",EarthCoord(40.7142,-74.006)));
		cities.insert(tup("Boston       ",EarthCoord(42.3583,-71.060)));

		print(
			projection<boost::mpl::vector2<name,name1> >
			(
				selection
				(
					join_gt<boost::mpl::vector1<name> >(cities,auto_rename<a>(cities)),
					distance_less_than(3000.)
				)
			)
		);
	}

	test_function _("distance", foo);
}
