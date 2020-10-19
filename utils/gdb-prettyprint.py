import gdb.printing

class PointPrinter(object):
    "Print a point"

    def __init__(self, val):
        self.val = val

    def to_string(self):
        return "(x={},y={})".format(self.val["m_data"][0], self.val["m_data"][1])

    def display_hint(self):
        return 'point'

class BoxPrinter(object):
    "Print a box"

    def __init__(self, val):
        self.val = val

    def to_string(self):
        return "(origin={},end={})".format(str(self.val["m_begin"]), str(self.val["m_end"]))


    def display_hint(self):
        return 'box'

def build_pretty_printer():
    pp = gdb.printing.RegexpCollectionPrettyPrinter("pylene")
    pp.add_printer('point', '^mln::_point<mln::impl::pcontainer<2, int> >$', PointPrinter)
    pp.add_printer('box', '^mln::_box<mln::impl::_bstorage<2, int> >$', BoxPrinter)
    #pp.add_printer('bar', '^bar$', barPrinter)
    return pp

gdb.printing.register_pretty_printer(
    gdb.current_objfile(),
    build_pretty_printer())
