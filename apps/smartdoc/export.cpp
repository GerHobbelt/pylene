#include "export.hpp"
#include <iostream>
#include <fstream>
#include <chrono>
#include <boost/date_time/posix_time/posix_time.hpp>

void export_xml(const char* filename,
                const char* document_id,
                const SmartDoc_t* p_array,
                int p_array_size)
{
  using namespace boost::posix_time;

  std::ofstream f(filename);
  ptime now = second_clock::local_time();

  f << "<?xml version='1.0' encoding='utf-8'?>\n"
    << "<seg_result version=\"0.2\" generated=\"" << to_iso_string(now) << "\">\n"
    << "  <software_used name=\"" << APP_NAME << "\" version=\"" << APP_VERSION << "\"/>\n"
    << "  <source_sample_file>" << document_id << "</source_sample_file>\n"
    << "  <segmentation_results>\n"
    ;

  // K0 + border
  auto p_cvt = [](int i) -> float { return i / 2.0 - 1; };

  for (int i = 0; i < p_array_size; ++i)
    {
      SmartDoc_t x = p_array[i];
      f << "    <frame index=\"" << (i+1) << "\" rejected=\"" << std::boolalpha << (not x.good) << "\">\n";

      if (x.good)
        {
          f <<
            "        <point name=\"tl\" x=\"" << p_cvt(x.quad[0][1]) << "\" y=\"" << p_cvt(x.quad[0][0]) << "\"/>\n"
            "        <point name=\"tr\" x=\"" << p_cvt(x.quad[1][1]) << "\" y=\"" << p_cvt(x.quad[1][0]) << "\"/>\n"
            "        <point name=\"br\" x=\"" << p_cvt(x.quad[2][1]) << "\" y=\"" << p_cvt(x.quad[2][0]) << "\"/>\n"
            "        <point name=\"bl\" x=\"" << p_cvt(x.quad[3][1]) << "\" y=\"" << p_cvt(x.quad[3][0]) << "\"/>\n"
            ;
        }
      f << "    </frame>\n";
    }
  f << "  </segmentation_results>\n"
    << "</seg_result>\n";
}
