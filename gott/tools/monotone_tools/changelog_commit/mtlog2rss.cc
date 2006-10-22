#include <boost/format.hpp>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/date_time/gregorian/formatters.hpp>      
#include "boost/date_time/posix_time/posix_time.hpp"

char const * rss_body = 
"<?xml version=\"1.0\"?>\n"
"<?xml-stylesheet type=\"text/xsl\" href=\"rss.xsl\"?>\n"
"<rss version=\"2.0\">\n"
"   <channel>\n"
"      <title>%1%</title>\n"
"      <link>%2%</link>\n"
"      <description> VCS Repository - ChangeLog </description>\n"
"      <language>en-us</language>\n"
"      <pubDate>%3%</pubDate>\n"
"      <lastBuildDate>%4%</lastBuildDate>\n"
"      <generator>mtlog2rss</generator>\n"
"      <webMaster>%5%</webMaster>\n"
"%6%\n"
"   </channel>\n"
"</rss>\n";

char const * rss_item = 
"      <item>\n"
"        <title>%1%</title>\n"
"	 <pubDate>%2%</pubDate>\n"
"	 <author>%3%</author>\n"
"	 <description>%4%</description>\n"
"      </item>\n";

std::string mt_log_sep = "-----------------------------------------------------------------";

struct rss_item_data
{
  std::string author;
  std::string revision;
  std::string branch;
  std::string date;
  std::vector<std::string> changelog_lines;
};

void parse_stream( std::istream & clis , std::vector<rss_item_data> & item_data_list )
{
  std::string line;
  std::getline(clis,line); // removing first line ( It's a mt_log_sep )
  rss_item_data tmp;
  
  while(std::getline(clis,line))
  {
    if(line == mt_log_sep)
    {
      item_data_list.push_back(tmp);
      tmp = rss_item_data();
    }
    else if(line.substr(0,10) == "Revision: ")
      tmp.revision = line.substr(10);
    else if(line.substr(0,8) == "Author: ") 
      tmp.author = line.substr(8);
    else if(line.substr(0,6) == "Date: ") 
      tmp.date = line.substr(6);
    else if(line.substr(0,8) == "Branch: ") 
      tmp.branch = line.substr(8);
    else if(line.substr(0,10) == "Ancestor: ") 
      continue;
    else if(line.substr(0,10) == "ChangeLog:")
      tmp.changelog_lines.push_back("Changes:"); 
    else if(line.find_first_not_of("\t\n\r ") == std::string::npos)
       continue; 
    else
      tmp.changelog_lines.push_back(line);
  }
}

std::string data_to_string( rss_item_data const & data )
{
  std::string c_lines;
  for(unsigned i = 0; i < data.changelog_lines.size(); ++i)
     c_lines += data.changelog_lines[i] + '\n';
  std::ostringstream os;
  os << boost::format(rss_item)  % (std::string("Revision: ") + data.revision) 
	                          % data.date
				  % data.author
				  % c_lines ;
  return os.str();
}

std::string get_date()
{
  using namespace boost::gregorian;
  using namespace boost::posix_time;
  ptime now = second_clock::local_time();
  now -= hours(1);
  return to_simple_string(now) + " GMT ";
}

std::string gen_output(std::vector<rss_item_data> const & data , char const * config_file)
{
  std::string items;
  for(unsigned i = 0; i < data.size(); ++i)
    items += data_to_string(data[i]);

  std::map<std::string,std::string> config;
  std::ifstream ifs(config_file);
  std::string line;
  while( std::getline(ifs,line))
  {
    std::string::size_type pos = line.find_first_of("=");
    config[line.substr(0,pos)] = line.substr(pos+1);
  }
  std::ostringstream os;
  os << boost::format(rss_body) % config["rsstitle"]
	                        % config["projectlink"]
	                        % get_date() 
	                        % get_date()
				% config["webmasteremail"] 
				% items;
  return  os.str();
}

int main(int argc, char**argv)
{
  if(argc < 4 ) return 0;
  std::ifstream clis(argv[1]);
  std::ofstream clos(argv[2]);
  std::vector<rss_item_data> item_data_list;
  parse_stream(clis,item_data_list);
  clos << gen_output(item_data_list,argv[3]);
}
