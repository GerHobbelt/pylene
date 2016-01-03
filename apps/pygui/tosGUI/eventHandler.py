from tosGUI_ import loadtree, filtertree
import matplotlib.pyplot as plt
import numpy as np
import functools
import pandas as pd
from matplotlib.patches import Rectangle
IGNORE_ZERO = True


def getbin(arr, n):
    X, XI = np.unique(np.sort(arr), return_index=True)
    nv = len(X)
    xbins = [ X[0] ]
    k = 0
    xs = len(arr) / n
    while xbins[-1] < X[-1]:
        i = k+1
        while i < nv-1 and XI[i] < XI[k]+xs:
            i += 1
        xbins.append(X[i])
        k = i
    return np.array(xbins)


class Annotate(object):
    def __init__(self, axis, params):
        self.ax = axis
        self.params = params

        self.started = False
        self.ax.figure.canvas.mpl_connect('button_press_event', self.on_press)
        self.ax.figure.canvas.mpl_connect('button_release_event', self.on_release)
        self.ax.figure.canvas.mpl_connect('motion_notify_event', self.on_motion)

    def on_press(self, event):
        if event.xdata and event.ydata:
            self.x0 = event.xdata
            self.y0 = event.ydata
            self.rect = Rectangle((0,0), 1, 1, fill=False, color='r')
            self.rect.set_xy( (event.xdata, event.ydata) )
            self.ax.add_patch(self.rect)
            self.started = True

    def on_motion(self, event):
        if self.started and event.xdata and event.ydata:
            self.x1 = event.xdata
            self.y1 = event.ydata
            self.rect.set_width(self.x1 - self.x0)
            self.rect.set_height(self.y1 - self.y0)
            self.ax.figure.canvas.draw()

    def on_release(self, event):
        if self.started and event.xdata and event.ydata:
            self.x1 = event.xdata
            self.y1 = event.ydata
        if self.started:
            del self.ax.patches[0]
            self.ax.figure.canvas.draw()
            self.started = False
            args=self.params[1:]
            x0 = min(self.x0, self.x1)
            x1 = max(self.x0, self.x1)
            y0 = min(self.y0, self.y1)
            y1 = max(self.y0, self.y1)
            self.params[0]._on_select_rect(x0, y0, x1, y1, *args)


def plotspectra(df, x, y, z):
    n,m = 200,200
    if IGNORE_ZERO:
        df = df[df[y] > 0]

    x,y = y,x
    xbins = getbin(df[x], n)
    ybins = getbin(df[y], m)
    H, xedges, yedges = np.histogram2d(np.asarray(df[x]), np.asarray(df[y]), bins=[xbins, ybins]) #, weights=D[:,2])


    fig = plt.figure(figsize=(10,5))
    plt.imshow(H.T, interpolation="none", origin="lower", picker=True)

    nticks = 50.0
    xs = np.linspace(0, len(xbins)-1, nticks)
    xs = np.array(np.round(xs), dtype=int)
    xlabels = xbins[xs]
    plt.xticks(xs, xlabels, rotation=-90)

    ys = np.linspace(0, len(ybins)-1, nticks)
    ys = np.array(np.round(ys), dtype=int)
    ylabels = ybins[ys]
    plt.xlabel(x)
    plt.ylabel(y)
    plt.yticks(ys, ylabels)
    plt.show()
    return fig, xedges, yedges


class tosGUI:
    def __init__(self, parent, df, nodemap=None, leaves=None):
        '''
        Create a new event handler to synchronize images / attributes plot / spectrum
        '''
        if type(parent) == str:
            parent, nodemap, leaves = loadtree(parent)

        self.parent = parent
        self.nodemap = nodemap
        self.leaves = leaves
        self._dataframe = df
        self._image_set = [] # a set of (figure, image, args, kwargs)
        self._attribute_set = {}
        self._attribute_text_set = {}
        self._spectrum_set = []
        self._current_leaf = None
        self._current_leaf_depth = None
        self._current_nodes = [] # a list of nodes to be displayed on images

    def _on_select_point(self, event):
        if not event.xdata or not event.ydata:
            return
        pts = np.tile([[event.ydata, event.xdata]], (self.leaves.shape[0], 1))
        dist = ((self.leaves - pts) ** 2).sum(axis=1)
        imin = dist.argmin()
        self._current_leaf = tuple(self.leaves[imin])
        self._update_attributes()


    def _on_select_attrval(self, event):
        if not event.xdata or not event.ydata or not self._current_leaf_depth:
            return

        depth = int(event.xdata)
        d = self._current_leaf_depth
        print depth, d
        node = self.nodemap[self._current_leaf]
        while d > depth:
            node = self.parent[node]
            d -= 1
        self._current_nodes = [ node ]
        self._update_images()

    def _on_select_rect(self, x0, y0, x1, y1, df, xname, yname, xedges, yedges):
        # selection des noeuds
        x0 += .5
        y0 += .5
        x1 += .5
        y1 += .5
        x0 = xedges[int(x0)]
        x1 = xedges[int(x1)+1]
        y0 = yedges[int(y0)]
        y1 = yedges[int(y1)+1]
        S = df[ (x0 <= df[xname]) & (df[xname] < x1) & (y0 <= df[yname]) & (df[yname] < y1) ]
        self._current_nodes = np.asarray(S.index, dtype=np.uint32)
        self._update_images()

    def plotAttribute(self, arr, name = ""):
        fig = plt.figure()
        if not name and type(arr).__name__ == "Series":
            name = arr.name
        fig.canvas.mpl_connect('button_press_event', self._on_select_attrval)
        self._attribute_set[fig] = (arr, name)

    def plotAttributeText(self, arr, name = ""):
        if not name and type(arr).__name__ == "Series":
            name = arr.name
        self._attribute_text_set[name] = arr

    def plotImage(self, image, *args, **kwargs):
        if image.ndim == 2:
            image = image * (0.7 / np.amax(image))
            image = np.array(np.tile(image[:,:,np.newaxis], (1,1,3)), dtype=np.float32)
        elif image.ndim == 3:
            shp = image.shape
            m = 0.7 / image.reshape( (shp[0] * shp[1], 3) ).max(0)
            image = image * (np.tile(m[np.newaxis, np.newaxis,:], (shp[0], shp[1], 1)))
            image = np.array(image, dtype=np.float32)
        assert image.ndim == 3
        assert image.dtype == np.float32
        fig = plt.figure()
        ima = filtertree(image, self.parent, self.nodemap, self._current_nodes)
        plt.imshow(ima, interpolation='nearest', *args, **kwargs)
        fig.canvas.mpl_connect('button_press_event', self._on_select_point)
        plt.show()
        self._image_set.append( (fig, image, args, kwargs) )

    def plotSpectra(self, df, x, y, z):
        fig, xe, ye = plotspectra(df, x, y, z)
        params = (self, df, y, x, xe, ye)
        a = Annotate(fig.gca(), params)
        #fig.canvas.mpl_connect('pick_event', self._on_select_rect)
        self._spectrum_set.append( (fig, a) )

    def plotStat(self):
        print self._dataframe.describe()

    def _update_attributes(self):
        mask = np.zeros( self.parent.size, dtype=bool)
        x = self.nodemap[self._current_leaf]
        while self.parent[x] != x:
            mask[x] = True
            x = self.parent[x]
        mask[x] = True
        self._current_leaf_depth = int(mask.sum())

        for fig in self._attribute_set.keys():
            if hasattr(fig.canvas, "isVisible") and not fig.canvas.isVisible():
                del self._attribute_set[fig]
                fig.canvas.close()

        for fig, (attr, name) in self._attribute_set.iteritems():
            ax = fig.gca()
            ax.cla()
            ax.plot(attr[mask])
            ax.set_title(name)
            fig.canvas.draw()

    def _update_images(self):
        torem = []
        for i in xrange(len(self._image_set)):
            fig = self._image_set[i][0]
            if hasattr(fig.canvas, "isVisible") and not fig.canvas.isVisible():
                torem.append(i)
        for i in torem[::-1]:
            fig = self._image_set[i][0]
            fig.canvas.close()
            del self._image_set[i]

        for fig, ima, args, kwargs in self._image_set:
            ax = fig.gca()
            image = filtertree(ima, self.parent, self.nodemap, self._current_nodes)
            ax.images[0].set_data(image)
            fig.canvas.draw()
            #fig.canvas.draw()

        for name, arr in self._attribute_text_set.iteritems():
            print "=== ", name, " ==="
            for x in self._current_nodes:
                print arr[x]
