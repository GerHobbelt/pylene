from ValueSetExample.ExampleStruct import MyStruct
from ValueSetExample.BaseValueSet import AbstractValueSet

from typing import Any


class MyValueSet(AbstractValueSet):

    def __init__(self): pass

    def get_MyStruct__(self, v: Any):
        return v.getV() if isinstance(v, MyStruct) else v

    def cast(self, value: Any, type_):
        v = self.get_MyStruct__(value)
        return super().cast(v, type_)

    def max(self):
        return 255

    def min(self):
        return 0

    def sup(self):
        return 255

    def inf(self):
        return 0

    def plus(self, value: Any) -> Any:
        v = self.get_MyStruct__(value)
        return MyStruct(super().plus(v))

    def minus(self, value: Any) -> Any:
        v = self.get_MyStruct__(value)
        return MyStruct(super().minus(v))

    def add(self, lhs: Any, rhs: Any) -> Any:
        l = self.get_MyStruct__(lhs)
        r = self.get_MyStruct__(rhs)
        return MyStruct(super().add(l, r))

    def sub(self, lhs: Any, rhs: Any) -> Any:
        l = self.get_MyStruct__(lhs)
        r = self.get_MyStruct__(rhs)
        return MyStruct(super().sub(l, r))

    def mult(self, lhs: Any, rhs: Any) -> Any:
        l = self.get_MyStruct__(lhs)
        r = self.get_MyStruct__(rhs)
        return MyStruct(super().mult(l, r))

    def div(self, lhs: Any, rhs: Any) -> Any:
        l = self.get_MyStruct__(lhs)
        r = self.get_MyStruct__(rhs)
        return MyStruct(super().div(l, r))

    def mod(self, lhs: Any, rhs: Any) -> Any:
        l = self.get_MyStruct__(lhs)
        r = self.get_MyStruct__(rhs)
        return MyStruct(super().mod(l, r))

    def pow(self, lhs: Any, rhs: Any) -> Any:
        l = self.get_MyStruct__(lhs)
        r = self.get_MyStruct__(rhs)
        return MyStruct(super().pow(l, r))
