#ifndef TOPOLOGY_HPP
# define TOPOLOGY_HPP

# include <mln/core/image/image2d.hpp>


namespace mln
{

  /// \brief Display a boolean image in Khalimsky space
  ///
  template <typename I>
  void
  Kdisplay(const Image<I>& ima);


  struct K1_tag {};
  struct K2_tag {};


  namespace K1 {
    bool is_face_2(point2d p);
    bool is_face_1(point2d p);
    bool is_face_1v(point2d p);
    bool is_face_1h(point2d p);
  }

  namespace K2 {
    bool is_face_2(point2d p);
  }

  /***************************/
  /****** Implementation *****/
  /***************************/



  template <typename I>
  void
  Kdisplay(const Image<I>& ima_, const point2d strides = point2d{1,1})
  {
    static_assert( std::is_same<mln_value(I), bool>::value,
		   "Bool required for image type");

    const I& ima = exact(ima_);
    auto shp = ima.domain().shape();
    int nr = shp[0], nc = shp[1];

    for (int j = 0; j < nc+4; j++)
      std::cout << "#";

    std::cout << std::endl;
    for (int i = 0; i < nr; i++)
      {
	std::cout << "# ";
	for (int j = 0; j < nc; j++)
	  {
	    point2d p = point2d{i, j};
	    if (ima((point2d{i, j}).as_vec() * strides.as_vec()))
	      {
	      if (i % 2 == 0 and j % 2 == 0)
		std::cout << "x";
	      else if (i % 2 == 0)
		std::cout << "|";
	      else if (j % 2 == 0)
		std::cout << "-";
	      else
		std::cout << ".";
	      }
	    else
	      std::cout << " ";
	  }
	std::cout << " #" << std::endl;
      }
  }


  namespace K1 {
    bool is_face_2(point2d p)
    {
      return (p[0] % 2 == 0 and p[1] % 2 == 0);
    }

    bool is_face_1(point2d p)
    {
      return ((p[0] % 2 == 0) xor (p[1] % 2 == 0));
    }

    bool is_face_1v(point2d p)
    {
      return (p[0] % 2 == 0 and p[1] % 2 == 1);
    }

    bool is_face_1h(point2d p)
    {
      return (p[0] % 2 == 1 and p[1] % 2 == 0);
    }

  }

  namespace K2
  {
    bool is_face_2(point2d p)
    {
      return (p[0] % 4 == 0 and p[1] % 4 == 0);
    }
  }


}


#endif // ! TOPOLOGY_HPP