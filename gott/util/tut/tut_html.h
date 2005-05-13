#ifndef TUT_REPORTER_HTML
#define TUT_REPORTER_HTML

#include <gott/util/tut/tut.h>
#include <boost/format.hpp>

namespace {
  void escape(std::ostream &s, char const *in) {
    s << "<span class=\"code\">\n";
    for (;*in;++in)
      switch (*in) {
      case ' ': s << "&nbsp;"; break;
      case '\n': s << "<br>\n"; break;
      default: s << *in;
      }
    s << "</div>\n";
  }
}

namespace tut {
  class html_reporter : public callback {
    std::string current_group;
    typedef std::vector<test_result> not_passed_list;
    not_passed_list not_passed;
    std::ostream& os;

  public:
    int ok_count;
    int exceptions_count;
    int failures_count;
    int terminations_count;
    int warnings_count;

    html_reporter(std::ostream& out = std::cout) 
      : os(out),
      ok_count(0), exceptions_count(0), failures_count(0), 
      terminations_count(0), warnings_count(0),
      group_started(false)
    { start_html(); }

    ~html_reporter() { end_html(); }

    void run_started() {
      ok_count = 0;
      exceptions_count = 0;  
      failures_count = 0;
      terminations_count = 0;
      warnings_count = 0;

      not_passed.clear();

      os << "<table>\n";
    }

    void test_completed(const test_result& tr) {
      if (tr.group != current_group) {
        current_group = tr.group;
        if (group_started)
          os << "</td>\n</tr>\n";
        os << "<tr>\n";
        group_started = true;
        os << (boost::format("<td class=\"group\">%1%</td>\n<td>\n") % current_group);
      }

      char const *res = "<span class=\"test_ok\">.</span>\n";
      if (tr.result == test_result::ok) {
        ok_count++;
      } else if (tr.result == test_result::ex) {
        exceptions_count++;
      } else if (tr.result == test_result::fail) {
        failures_count++;
      } else if (tr.result == test_result::warn) {
        warnings_count++;
      } else {
        terminations_count++;
      }

      if (tr.result != test_result::ok) {
        res = "<span class=\"test_fail\">!</span>\n";
        not_passed.push_back(tr);
      }
      
      os << res;
    }

    void run_completed() {
      os << "</table>";
      if (not_passed.size() > 0) {
        os << "<h2>Failure summary</h2>\n<ul>\n";
        not_passed_list::const_iterator i = not_passed.begin();
        while (i != not_passed.end()) {
           test_result tr = *i;

           os << "<li>\n";
           os << (boost::format("%1%<%2%>:&nbsp;") % tr.group % tr.test);

           //problem:
           switch (tr.result) {
             case test_result::fail:
               os << "fail";
               os << '\n';
               break;
             case test_result::ex: 
               os << "exception";
               if (tr.exception_typeid != "")
                 os << " " << tr.exception_typeid;
               os << '\n';
               break;
             case test_result::term: 
               os << "terminated\n";
               break;
             case test_result::warn: 
               os << "warn\n";
               break;
             default: break;
           }

           if (tr.message != "") {
             os << " : ";
             escape(os, tr.message.c_str());
           }

           os << "</li>\n";

           ++i;
        }
        os << "</ul>\n";
       }

      os << "<ul>\n";
      if (terminations_count > 0) 
        os << "<li>terminations: " << terminations_count << "</li>\n";
      if (exceptions_count > 0)
        os << "<li>exceptions: " << exceptions_count << "</li>\n";
      if (failures_count > 0)
        os << "<li>failures: " << failures_count << "</li>\n";
      if (warnings_count > 0)
        os << "<li>warnings: " << warnings_count << "</li>\n";
      os << "<li>tests passed: " << ok_count << "</li>\n</ul>\n";

      os << "</hr>";
    }

    bool all_ok() const {
      return not_passed.size() == 0;
    }

  private:
    void start_html() {
      os << "<html><head><title>Test results</title></head>\n";
      os << "<body><h1>Test results</h1>\n";
    }

    void end_html() {
      os << "</body></html>\n";
    }

    bool group_started;
  };

}

#endif
