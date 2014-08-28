
#include <mln/core/image/image2d.hpp>
#include <mln/io/imread.hpp>
#include <mln/core/colors.hpp>
#include <mln/colors/literal.hpp>

#include <mln/morpho/component_tree/component_tree.hpp>
#include <mln/morpho/component_tree/io.hpp>
#include <mln/morpho/component_tree/reconstruction.hpp>
#include <mln/morpho/component_tree/accumulate.hpp>
#include <mln/accu/accumulators/mean.hpp>

#include <QApplication>
#include <QtGui>
#include <mln/qt/imageviewer.hpp>
#include <apps/tos/Kinterpolate.hpp>
#include "brush.hpp"
#include "myheap.hpp"


using namespace mln;

typedef morpho::component_tree<unsigned, image2d<unsigned> > tree_t;

property_map<tree_t, float> run_djikstra(const tree_t& tree,
                                         property_map<tree_t, uint8>& colormap,
                                         const property_map<tree_t, rgb<int> >& vmap,
                                         int color)
{
  property_map<tree_t, float> distancemap(tree, value_traits<float>::max());
  property_map<tree_t, int>   posmap(tree, -1);

  myheap heap(tree, distancemap, posmap);
  mln_foreach(auto node, tree.nodes())
    if (colormap[node] == (uint8)-1)
      colormap[node] = 0;
    else if (colormap[node] == color) {
      distancemap[node] = 0;
      heap.push(node);
    }


  {
    mln_entering("Running djikstra");
    tree_t::node_type p;
    morpho::tree_neighb_t nbh;
    mln_iter(q, nbh(p));
    while (not heap.empty())
      {
        p = heap.pop();
        assert(posmap[p] == -1);
        mln_forall(q)
        {
          float d = l2norm(vmap[p] - vmap[*q]) + distancemap[p];
          if (d < distancemap[*q])
            {
              distancemap[*q] = d;
              if (posmap[*q] == -1) { // not in queue, insert
                heap.push(*q);
              } else {
                heap.update(*q);
              }
            }
        }
      }
    mln_exiting();
  }

  return distancemap;
}


image2d<rgb8>
segmentation_(const tree_t& tree,
              const image2d<rgb8>& ima_,
              const image2d<rgb8>& markers__)
{
  image2d<uint8> markers_ = transform(markers__,
                                      [](const rgb8& v) -> uint8 {
                                        if (v == colors::literal::red) return 1;
                                        else if (v == colors::literal::blue) return 2;
                                        else return 0;
                                      });

  image2d<rgb8>  ima = Kadjust_to(ima_, tree._get_data()->m_pmap.domain());
  image2d<uint8> marker = Kadjust_to(markers_, tree._get_data()->m_pmap.domain(), "zero");

  auto vmap = morpho::vaccumulate_proper(tree, ima, accu::features::mean<>());

  property_map<tree_t, uint8> colormap(tree, 0);


  property_map<tree_t, float> distancemap(tree, value_traits<float>::max());
  property_map<tree_t, int>   posmap(tree, -1);

  mln_foreach(auto px, marker.pixels())
    {
      tree_t::node_type node = tree.get_node_at(px.index());
      if (colormap[node] == 0)
        colormap[node] = px.val();
      else if (colormap[node] != px.val())
        colormap[node] = -1;
    }


  myheap heap(tree, distancemap, posmap);
  mln_foreach(auto node, tree.nodes())
    if (colormap[node] == (uint8)-1)
      colormap[node] = 0;
    else if (colormap[node] != 0) {
      distancemap[node] = 0;
      heap.push(node);
    }

  // Run djisktra
  {
    mln_entering("Running djikstra");
    tree_t::node_type p;
    morpho::tree_neighb_t nbh;
    mln_iter(q, nbh(p));
    while (not heap.empty())
      {
        p = heap.pop();
        assert(posmap[p] == -1);
        mln_forall(q)
        {
          float d = l2norm(vmap[p] - vmap[*q]) + distancemap[p];
          if (d < distancemap[*q])
            {
              colormap[*q] = colormap[p];
              distancemap[*q] = d;
              if (posmap[*q] == -1) { // not in queue, insert
                heap.push(*q);
              } else {
                heap.update(*q);
              }
          }
        }
      }
    mln_exiting();
  }

  image2d<float> dist;
  image2d<uint8> color;
  resize(dist, ima);
  resize(color, ima);

  morpho::reconstruction(tree, colormap, color);
  morpho::reconstruction(tree, distancemap, dist);
  io::imsave(color, "/tmp/colormap.tiff");
  io::imsave(dist, "/tmp/distancemap.tiff");

  auto rec = transform(imzip(color, ima), [](const std::tuple<uint8, rgb8>& v) {
      return std::get<0>(v) == 2 ? std::get<1>(v) : rgb8(0);
    });

  return Kadjust_to(rec, ima_.domain());
}


image2d<rgb8>
segmentation(const tree_t& tree,
             const image2d<rgb8>& ima_,
             const image2d<rgb8>& markers__)
{
  image2d<uint8> markers_ = transform(markers__,
                                      [](const rgb8& v) -> uint8 {
                                        if (v == colors::literal::red) return 1;
                                        else if (v == colors::literal::blue) return 2;
                                        else return 0;
                                      });

  image2d<rgb8>  ima = Kadjust_to(ima_, tree._get_data()->m_pmap.domain());
  image2d<uint8> marker = Kadjust_to(markers_, tree._get_data()->m_pmap.domain(), "zero");

  auto vmap = morpho::vaccumulate_proper(tree, ima, accu::features::mean<>());

  property_map<tree_t, uint8> colormap(tree, 0);
  mln_foreach(auto px, marker.pixels())
    {
      tree_t::node_type node = tree.get_node_at(px.index());
      if (colormap[node] == 0)
        colormap[node] = px.val();
      else if (colormap[node] != px.val())
        colormap[node] = -1;
    }

  property_map<tree_t, float> dist1 = run_djikstra(tree, colormap, vmap, 1);
  property_map<tree_t, float> dist2 = run_djikstra(tree, colormap, vmap, 2);

  auto proba_map = make_functional_property_map<tree_t::node_type>
    ([&dist1,&dist2](const tree_t::node_type& x) {
      return dist1[x] / (dist1[x] + dist2[x]);
    });

  auto color_map = make_functional_property_map<tree_t::node_type>
    ([&dist1,&dist2](const tree_t::node_type& x) -> uint8{
      return dist1[x] < dist2[x] ? 1 : 2;
    });


  image2d<float> dist;
  image2d<uint8> color;
  resize(dist, ima);
  resize(color, ima);

  morpho::reconstruction(tree, color_map, color);
  morpho::reconstruction(tree, proba_map, dist);
  io::imsave(color, "/tmp/colormap.tiff");
  io::imsave(dist, "/tmp/distancemap.tiff");

  auto rec = transform(imzip(color, ima), [](const std::tuple<uint8, rgb8>& v) {
      return std::get<0>(v) == 2 ? std::get<1>(v) : rgb8(0);
    });

  return Kadjust_to(rec, ima_.domain());
}



int main(int argc, char** argv)
{
  QApplication a(argc, argv);

  tree_t tree;
  {
    std::ifstream fs(argv[1]);
    morpho::load(fs, tree);
  }

  image2d<rgb8> ima;
  io::imread(argv[2], ima);


  QMainWindow win;
  qt::ImageViewer* viewer = new qt::ImageViewer(ima);

  auto callback = std::bind(segmentation, tree, ima, std::placeholders::_1);

  QGraphicsScene* scene = viewer->getScene();
  MyBrush brush(viewer, callback);
  scene->installEventFilter(&brush);

  QToolBar* toolbar = new QToolBar(&win);
  QAction* action1 = toolbar->addAction("Bg");
  QAction* action2 = toolbar->addAction("Fg");
  QAction* action3 = toolbar->addAction("Run");
  QAction* action4 = toolbar->addAction("Revert");
  QAction* action5 = toolbar->addAction("Reload");
  QObject::connect(action1, SIGNAL(triggered()),
                   &brush, SLOT(setColor1()));
  QObject::connect(action2, SIGNAL(triggered()),
                   &brush, SLOT(setColor2()));
  QObject::connect(action3, SIGNAL(triggered()),
                   &brush, SLOT(run()));
  QObject::connect(action4, SIGNAL(triggered()),
                   &brush, SLOT(revert()));
  QObject::connect(action5, SIGNAL(triggered()),
                   &brush, SLOT(reload()));


  QActionGroup agroup(&win);
  agroup.addAction(action1);
  agroup.addAction(action2);
  action1->setCheckable(true);
  action2->setCheckable(true);


  win.setCentralWidget(viewer);
  win.addToolBar(toolbar);
  win.show();

  // QImage image("../../../img/small.pgm");
  // QGraphicsPixmapItem item(QPixmap::fromImage(image));
  // QGraphicsScene* m_scene = new QGraphicsScene;
  // m_scene->addItem(&item);

  // QGraphicsView* view = new QGraphicsView(m_scene);
  // QMainWindow win; 
  // win.setCentralWidget(view);
  // win.show();

  a.exec();
}
