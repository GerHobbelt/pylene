#pragma once

#include <FreeImage.h>

#include <iosfwd>


namespace mln
{
  namespace io
  {
    namespace internal
    {

      struct istream_wrapper
      {
        istream_wrapper(std::istream& is)
        {
          m_cs     = &is;
          m_offset = is.tellg();
        }

        ~istream_wrapper() {}

        static unsigned read(void* buffer, unsigned size, unsigned count, fi_handle isw_)
        {
          istream_wrapper* isw = (istream_wrapper*)isw_;
          isw->m_cs->read((char*)buffer, size * count);
          if (*isw->m_cs)
            return count;
          else
          {
            isw->m_cs->clear();
            return (unsigned)(isw->m_cs->gcount() / size);
          }
        }

        static int seek(fi_handle isw_, long offset, int origin)
        {
          istream_wrapper*       isw = (istream_wrapper*)isw_;
          std::istream::off_type off = offset;
          if (origin == SEEK_SET)
            off += isw->m_offset;
          isw->m_cs->seekg(off, (std::ios_base::seekdir)origin);
          return (*isw->m_cs) ? 0 : -1;
        }

        static long tell(fi_handle isw_)
        {
          istream_wrapper* isw = (istream_wrapper*)isw_;
          return (long)(isw->m_cs->tellg() - isw->m_offset);
        }

      private:
        std::istream*          m_cs;
        std::istream::off_type m_offset;
      };

      struct ostream_wrapper
      {

        ostream_wrapper(std::ostream& os)
        {
          m_cs     = &os;
          m_offset = os.tellp();
        }

        ~ostream_wrapper() {}

        static unsigned write(void* buffer, unsigned size, unsigned count, fi_handle osw_)
        {
          ostream_wrapper* osw = (ostream_wrapper*)osw_;
          osw->m_cs->write((char*)buffer, size * count);
          return (osw->m_cs) ? count : 0;
        }

        static int seek(fi_handle osw_, long offset, int origin)
        {
          ostream_wrapper*       osw = (ostream_wrapper*)osw_;
          std::ostream::off_type off = offset;
          if (origin == SEEK_SET)
            off += osw->m_offset;
          osw->m_cs->seekp(off, (std::ios_base::seekdir)origin);
          return (osw->m_cs) ? 0 : -1;
        }

        static long tell(fi_handle osw_)
        {
          ostream_wrapper* osw = (ostream_wrapper*)osw_;
          return (long)(osw->m_cs->tellp() - osw->m_offset);
        }

      private:
        std::ostream*          m_cs;
        std::ostream::off_type m_offset;
      };

    } // namespace internal
  }   // namespace io
} // namespace mln