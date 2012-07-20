#ifndef MLN_IO_READER_HH
# define MLN_IO_READER_HH

# include <typeindex>

namespace mln
{
  namespace io
  {

    class Reader
    {
    public:

      /// \brief Initialize the backend
      virtual void initialize() {};

      /// \brief Load an image from its filename
      /// \param[in] path Path of the file
      virtual void load(const char* path) = 0;

      /// \brief Close the file
      virtual void close() = 0;

      /// \brief Read the next pixel and move on internal pointer.
      /// \param[out] out Adress pointer where data will be stored to.
      virtual void read_next_pixel(void* out) = 0;

      /// \brief Read the next line and move on internal pointer.
      /// \param[out] out Adress pointer where data will be stored to.
      virtual void read_next_line(void* out) = 0;

      /// \brief Get the number of dimension of the image
      virtual int get_ndim() const = 0;

      /// \brief Get value type id of data
      virtual std::type_index get_value_type_id() const = 0;

      // Concept
      // void read_pixel_();
      // void read_line_();

    };

  }

}





#endif /* !MLN_IO_READER_HH */
