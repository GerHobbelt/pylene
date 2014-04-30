#ifndef MLN_MORPHO_DATASTRUCT_IMAGE_PROPERTY_MAP_HPP
# define MLN_MORPHO_DATASTRUCT_IMAGE_PROPERTY_MAP_HPP

# include <mln/core/image_base.hpp>
# include <mln/morpho/datastruct/component_tree.hpp>
# include <mln/morpho/datastruct/attribute_map.hpp>
# include <mln/core/range/transform.hpp>

namespace mln
{

  namespace morpho
  {
    template <class P, class AMap, class ValueMap>
    struct image_tree_property_map;

    template <class P, class AMap, class ValueMap>
    image_tree_property_map<P, AMap, ValueMap>
    make_image(const component_tree<P, AMap >& tree,
               const ValueMap& vmap);
  }


  template <class P, class AMap, class ValueMap>
  struct image_traits< morpho::image_tree_property_map<P, AMap, ValueMap> >
  {
    typedef std::true_type              accessible;
    typedef random_access_image_tag     category;
    typedef std::true_type              concrete; // depend
    typedef std::true_type              indexable;
    typedef extension::none_extension_tag         extension;
    typedef std::false_type             shallow_copy;
  };


  namespace morpho
  {
    // Fwd
    template <class ImTreePropMap>
     struct image_tree_property_map_pixel;

    template <class P, class AMap, class ValueMap>
    struct image_tree_property_map :
      image_base< image_tree_property_map<P, AMap, ValueMap>,
                  typename component_tree<P, AMap>::node_type,
                  typename ValueMap::value_type >
    {
    private:
      typedef component_tree<P, AMap>           tree_t;
      typedef typename tree_t::node_type        node_t;
      typedef image_tree_property_map           self_t;

      struct fun_viter;
      struct fun_cviter;
      struct fun_pixter;
      struct fun_cpixter;


    public:
      typedef typename ValueMap::value_type                     value_type;
      typedef typename ValueMap::reference                      reference;
      typedef typename ValueMap::const_reference                const_reference;
      typedef typename tree_t::node_type                        point_type;
      typedef typename tree_t::vertex_id_t                      size_type;
      typedef void                                              difference_type;
      typedef image_tree_property_map_pixel<self_t>             pixel_type;
      typedef image_tree_property_map_pixel<const self_t>       const_pixel_type;

      template <class ImTreePropMap>
      friend struct image_tree_property_map_pixel;

      template <class ImTreePropMap>
      friend struct image_tree_property_map_pixel;


      // Ranges & iterators
      typedef typename tree_t::node_range                                       domain_type;

      typedef transformed_range<domain_type, fun_viter>      value_range;
      typedef transformed_range<domain_type, fun_cviter>     const_value_range;
      typedef transformed_range<domain_type, fun_pixter>     pixel_range;
      typedef transformed_range<domain_type, fun_cpixter>    const_pixel_range;

      image_tree_property_map(const tree_t& tree, const ValueMap& vmap);

      // Acces
      reference         operator() (const node_t& node);
      const_reference   operator() (const node_t& node) const;
      reference         at (const node_t& node);
      const_reference   at (const node_t& node) const;
      reference         operator[] (size_type index);
      const_reference   operator[] (size_type index) const;

      node_t          point_at_index(size_type index) const;
      size_type       index_of_point(const node_t& node) const;


      /// This function does not make sense on a Tree Property Map.
      /// It's here to fulfill the concept requirement.
      difference_type delta_index(const node_t& node) const;


      // Range & iterator
      domain_type               domain() const;
      value_range               values();
      const_value_range         values() const;
      pixel_range               pixels();
      const_pixel_range         pixels() const;


    private:
      tree_t           m_tree;
      ValueMap         m_vmap;
    };

    /******************************/
    /*** Implementation         ***/
    /******************************/

    template <class ImTreePropMap>
    struct image_tree_property_map_pixel
      : Pixel< image_tree_property_map_pixel<ImTreePropMap> >
    {
      typedef ImTreePropMap                             image_type;
      typedef mln_value(ImTreePropMap)                  value_type;
      typedef mln_reference(ImTreePropMap)              reference;
      typedef mln_point(ImTreePropMap)                  point_type;
      typedef mln_point(ImTreePropMap)                  site_type;
      typedef typename ImTreePropMap::size_type         size_type;

      image_tree_property_map_pixel() = default;
      image_tree_property_map_pixel(ImTreePropMap* ima, size_type node_id)
        : m_ima(ima), m_index(node_id)
      {
      }

      image_tree_property_map_pixel(const typename ImTreePropMap::pixel_type& other)
        : m_ima(other.m_ima), m_index(other.m_index)
      {
      }

      reference val() const
      {
        return m_ima->m_vmap[m_index];
      }

      point_type point() const
      {
        return m_ima->m_tree.get_node(m_index);
      }

      site_type site() const
      {
        return m_ima->m_tree.get_node(m_index);
      }

      image_type& image() const
      {
        return *m_ima;
      }

      size_type index() const
      {
        return m_index;
      }

    private:
      image_type*               m_ima;
      size_type                 m_index;
    };

    template <class P, class AMap, class ValueMap>
    struct image_tree_property_map<P, AMap, ValueMap>::fun_viter
    {
      reference
      operator() (const point_type& node) const
      {
        return m_this->m_vmap[node];
      }

      self_t* m_this;
    };

    template <class P, class AMap, class ValueMap>
    struct image_tree_property_map<P, AMap, ValueMap>::fun_cviter
    {
      const_reference
      operator() (const point_type& node) const
      {
        return m_this->m_vmap[node];
      }

      const self_t* m_this;
    };

    template <class P, class AMap, class ValueMap>
    struct image_tree_property_map<P, AMap, ValueMap>::fun_pixter
    {
      pixel_type
      operator() (const point_type& node) const
      {
        return pixel_type(m_this, node.id());
      }

      self_t* m_this;
    };

    template <class P, class AMap, class ValueMap>
    struct image_tree_property_map<P, AMap, ValueMap>::fun_cpixter
    {
      const_pixel_type
      operator() (const point_type& node) const
      {
        return pixel_type(m_this, node.id());
      }

      const self_t* m_this;
    };

    template <class P, class AMap, class ValueMap>
    image_tree_property_map<P, AMap, ValueMap>::image_tree_property_map(const tree_t& tree, const ValueMap& vmap)
      : m_tree(tree),
        m_vmap(vmap)
    {
    }


    template <class P, class AMap, class ValueMap>
    inline
    typename image_tree_property_map<P, AMap, ValueMap>::reference
    image_tree_property_map<P, AMap, ValueMap>::operator() (const node_t& node)
    {
      return m_vmap[node];
    }


    template <class P, class AMap, class ValueMap>
    typename image_tree_property_map<P, AMap, ValueMap>::const_reference
    image_tree_property_map<P, AMap, ValueMap>::operator() (const node_t& node) const
    {
      return m_vmap[node];
    }
    template <class P, class AMap, class ValueMap>
    typename image_tree_property_map<P, AMap, ValueMap>::reference
    image_tree_property_map<P, AMap, ValueMap>::at (const node_t& node)
    {
      return m_vmap[node];
    }

    template <class P, class AMap, class ValueMap>
    typename image_tree_property_map<P, AMap, ValueMap>::const_reference
    image_tree_property_map<P, AMap, ValueMap>::at (const node_t& node) const
    {
      return m_vmap[node];
    }

    template <class P, class AMap, class ValueMap>
    typename image_tree_property_map<P, AMap, ValueMap>::reference
    image_tree_property_map<P, AMap, ValueMap>::operator[] (size_type index)
    {
      return m_vmap[index];
    }

    template <class P, class AMap, class ValueMap>
    typename image_tree_property_map<P, AMap, ValueMap>::const_reference
    image_tree_property_map<P, AMap, ValueMap>::operator[] (size_type index) const
    {
      return m_vmap[index];
    }


    template <class P, class AMap, class ValueMap>
    typename image_tree_property_map<P, AMap, ValueMap>::point_type
    image_tree_property_map<P, AMap, ValueMap>::point_at_index(size_type index) const
    {
      return m_tree.get_node(index);
    }

    template <class P, class AMap, class ValueMap>
    typename image_tree_property_map<P, AMap, ValueMap>::size_type
    image_tree_property_map<P, AMap, ValueMap>::index_of_point(const node_t& node) const
    {
      return node.id();
    }

    template <class P, class AMap, class ValueMap>
    typename image_tree_property_map<P, AMap, ValueMap>::difference_type
    image_tree_property_map<P, AMap, ValueMap>::delta_index(const node_t& node) const
    {
      (void) node;
    }

    template <class P, class AMap, class ValueMap>
    typename image_tree_property_map<P, AMap, ValueMap>::domain_type
    image_tree_property_map<P, AMap, ValueMap>::domain() const
    {
      return m_tree.nodes();
    }

    template <class P, class AMap, class ValueMap>
    typename image_tree_property_map<P, AMap, ValueMap>::value_range
    image_tree_property_map<P, AMap, ValueMap>::values()
    {
      fun_viter fun = { this };
      return value_range(m_tree.nodes(), fun);
    }

    template <class P, class AMap, class ValueMap>
    typename image_tree_property_map<P, AMap, ValueMap>::const_value_range
    image_tree_property_map<P, AMap, ValueMap>::values() const
    {
      fun_cviter fun = { this };
      return const_value_range(m_tree.nodes(), fun);
    }

    template <class P, class AMap, class ValueMap>
    typename image_tree_property_map<P, AMap, ValueMap>::pixel_range
    image_tree_property_map<P, AMap, ValueMap>::pixels()
    {
      fun_pixter fun = { this };
      return pixel_range(m_tree.nodes(), fun);
    }

    template <class P, class AMap, class ValueMap>
    typename image_tree_property_map<P, AMap, ValueMap>::const_pixel_range
    image_tree_property_map<P, AMap, ValueMap>::pixels() const
    {
      fun_cpixter fun = { this };
      return const_pixel_range(m_tree.nodes(), fun);
    }


    template <class P, class AMap, class ValueMap>
    image_tree_property_map<P, AMap, ValueMap>
    make_image(const component_tree<P, AMap>& tree,
               const ValueMap& vmap)
    {
      return image_tree_property_map<P, AMap, ValueMap>(tree, vmap);
    }

  }

}

#endif // ! MLN_MORPHO_DATASTRUCT_IMAGE_PROPERTY_MAP_HPP
