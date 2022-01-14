from ValueSetExample import MyValueSet, MyStruct

if __name__ == '__main__':
    import numpy as np

    vs = MyValueSet()

    arr = np.array([MyStruct(1), MyStruct(
        2), MyStruct(6.5), MyStruct(3.14)], ndmin=1)

    print(vs.max())

    mys = MyStruct(6.15)
    print(mys.getV())

    sint = vs.cast(mys, "int")
    print(sint)

    ret = vs.add(arr[0], arr[1])

    print(vs.cast(ret, "float"))
