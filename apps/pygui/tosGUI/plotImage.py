import matplotlib.pyplot as plt

def onclick(event):
    print 'button=%d, x=%d, y=%d, xdata=%f, ydata=%f'%(
                event.button, event.x, event.y, event.xdata, event.ydata)

def plotImage(arr, *args, **kwargs):
    fig=plt.figure()
    plt.imshow(arr)
    fig.canvas.mpl_connect("button_press_event", onclick)
    plt.show()
