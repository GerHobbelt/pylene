Writing parallel pointwise algorithms
-------------------------------------

TODO



Writing parallel local algorithms
---------------------------------

Parallel algorithms should be implemented in the ``parallel`` namespace.
Parallel algorithms in Pylene work with tiles (see :ref:doc:`Tiling </tiling>`), and as such have a code structure
fit for tile operations. They are split up in three classes:

* ``TileLoader``, tasked with loading a tile into the class
* ``TileExecutor`` which executes a chosen algorithm on the loaded tile
* ``TileWriter`` which writes the tile back into the image, along with optional tile postprocessing. 

In the following paragraphs, you will see code blocks detailing how to implement a parallel algorithm
followed by explanations of the code blocks.

::

    template <class InputImage>
    class TileLoader_MyAlgo : public TileLoaderBase
    {
    public:

      /// \param width the extended width of the tile
      /// \param height the extended height of the tile
      TileLoader_MyAlgo(InputImage input, int tile_width, int tile_height)
        : _in{input}
        , m_tile{tile_width, tile_height}
      {
      }

      TileLoader_MyAlgo(const TileLoader_MyAlgo& other)
        : _in{other._in}
        , m_tile{other.m_tile.width(), other.m_tile.height()}
      {
      }

      TileLoader_MyAlgo& operator=(const TileLoader_MyAlgo&) = delete;

      /// \param roi The tile region
      /// \param input_roi The extended roi required to compute the tile region
      void load_tile(mln::box2d input_roi) const final
      {
        assert(m_tile.width() >= input_roi.width() && "Tile width mismatches");
        assert(m_tile.height() >= input_roi.height() && "Tile height mismatches");

        m_tile.set_domain_topleft(input_roi.tl());
        assert(m_tile.domain().includes(input_roi));
        assert(m_tile.domain().tl() == input_roi.tl());

        image_value_t<InputImage> padding_value = 0;
        auto padding_method = mln::PAD_ZERO;
        auto dst = m_tile.clip(input_roi);
        copy_pad(_in, dst, padding_method, padding_value);
      }

      mln::ndbuffer_image get_tile() const final
      {
        return m_tile;
      }

    private:
      InputImage _in;
      mutable mln::image2d<image_value_t<InputImage>> m_tile;
    };

Regarding ``TileLoader``
* The arguments of the constructor are the entire image we're processing as well as tile size parameters
* The inheritance from ``TileLoaderBase``: using class inheritance allows us to keep
  the design generic. As such, this is mandatory.
* The ``load_tile`` function creates tiles based on a region of interest ``roi`` being the
  part of the image we're currently processing. We use ``clip`` and ``copy_pad`` (see
  :ref:doc:`Padding </core/pad>`) to first clip the ``roi`` in the input image and then
  pad the sides of the clipped image as needed (either by taking neighboring values in the
  input image when available, or by padding with zeroes if necessary).

::

    template <class Image, class SE>
    class TileExecutor_MyAlgo : public TileExecutorBase
    {
    public:
      TileExecutor_MyAlgo(const SE& se)
        : _se{se}
      {
      }

      void execute(mln::ndbuffer_image in, mln::ndbuffer_image out) const final
      {
        assert(in.domain().includes(out.domain()));

        auto in_image2d  = *(in.cast_to<Image, 2>());
        auto out_image2d = *(out.cast_to<Image, 2>());
        MyAlgo(in_image2d, _se, out_image2d);
      }

    private:
      const SE&         _se;
    };

Regarding the ``TileExecutor``:

* The only argument for this class is the structuring element (see :ref:doc:`Structuring elements </core/se>`) used for pixel neighborhood.    
  Other arguments, if any, can be created by the arguments given to ``execute``
* The inheritance from the base class ``TileExecutorBase`` is once again mandatory.
* The ``execute`` function executes a chosen function on an input given by the ``TileLoader`` and also needs an output tile given by the ``TileWriter``
::

    template <class OutputImage>
    class TileWriter_MyAlgo : public TileWriterBase
    {
    public:
      TileWriter_MyAlgo(OutputImage out)
        : _out{out}
      {
      }

      void write_tile(mln::box2d roi) const final { (void)roi; }

      mln::ndbuffer_image get_tile(mln::box2d roi) const final { return _out.clip(roi); }

    private:
      OutputImage _out;
    };

Most of the time, the ``TileWriter`` isn't going to do much (or anything at all), so having a quasi-empty class
like this is going to be enough. When you need to do some postprocessing, the ``write_tile`` function should be adapted
accordingly.
As can be expected the only parameter than needs to be passed as argument here is the output image.
::

    template <class InputImage, class SE, class OutputImage>
    struct MyAlgoParallel : ParallelLocalCanvas2D
    {
      static_assert(std::is_same_v<image_value_t<InputImage>, image_value_t<OutputImage>>);

    private:
      MyAlgoParallel(InputImage& in, OutputImage& out, SE& se, mln::box2d roi, mln::box2d tile_dims)
        : m_se{se}
        , m_output_roi{roi}
        , m_tile_l{in, tile_dims.width(), tile_dims.height()}
        , m_tile_w{out}
        , m_tile_e{se}
      {}

    public:
      MyAlgoParallel(InputImage& in, OutputImage& out, SE& se, mln::box2d roi)
        : MyAlgoParallel(in, out, se, roi, se.compute_input_region({TILE_WIDTH, TILE_HEIGHT}))
      {
      }

      MyAlgoParallel(InputImage& in, OutputImage& out, SE& se, mln::box2d roi, int tile_width, int tile_height)
        : MyAlgoParallel(in, out, se, roi, se.compute_input_region({tile_width, tile_height}))
      {
        ParallelLocalCanvas2D::TILE_WIDTH  = tile_width;
        ParallelLocalCanvas2D::TILE_HEIGHT = tile_height;
      }

      std::unique_ptr<ParallelLocalCanvas2D> clone()  const final { return std::make_unique<MyAlgoParallel>(*this); }
      mln::box2d GetOutputRegion() const noexcept final { return m_output_roi; }
      mln::box2d ComputeInputRegion(mln::box2d roi) const noexcept final { return m_se.compute_input_region(roi); }

      const TileLoaderBase*   GetTileLoader() const noexcept   final { return &m_tile_l; };
      const TileWriterBase*   GetTileWriter() const noexcept   final { return &m_tile_w; };
      const TileExecutorBase* GetTileExecutor() const noexcept final { return &m_tile_e; };

    private:
      using tile_loader_t   = TileLoader_MyAlgo<InputImage>;
      using tile_executor_t = TileExecutor_MyAlgo<image_value_t<InputImage>, SE>;
      using tile_writer_t   = TileWriter_MyAlgo<OutputImage>;

      SE               m_se;
      mln::box2d       m_output_roi;
      tile_loader_t    m_tile_l;
      tile_writer_t    m_tile_w;
      tile_executor_t  m_tile_e;
    };

This is simply a wrapper around the writer, loader and executor classes for a given algorithm.
The resulting class is used as argument in the function ``sequential_execute_local2D`` (see below)
::

    MyAlgo_returntype MyAlgo(args)
    {
      MyAlgoParallel caller(image, out, se, output_roi, tile_width, tile_height);
      sequential_execute_local2D(caller);
    }

Finally, the function call executes the algorithm by using the wrapper ``MyAlgoParallel`` just previously defined.

* Function definition is identical to non-parallel function signature, allowing for ease of use
  when going from non-parallel to parallel with the only difference being the prefix of the function when
  using it in parallel: ``parallel::``
* ``sequential_execute_local2D`` is a wrapper around ``tbb::parallel_for`` using the different classes defined previously.    
  The code executed in parallel through ``tbb::parallel_for`` is similar to the following
::

  void ParallelLocalCanvas2D::ExecuteTile(mln::box2d roi) const
  {
    auto m_tile_l = this->GetTileLoader();
    auto m_tile_w = this->GetTileWriter();
    auto m_tile_e = this->GetTileExecutor();

    mln::box2d input_roi = this->ComputeInputRegion(roi);
    m_tile_l->load_tile(input_roi);
    m_tile_e->execute(m_tile_l->get_tile(), m_tile_w->get_tile(roi));
    m_tile_w->write_tile(roi);
  }