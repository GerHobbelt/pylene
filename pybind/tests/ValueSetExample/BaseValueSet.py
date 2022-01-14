from abc import ABC, abstractmethod
from typing import Any


class AbstractValueSet(ABC):

    def __init__(self): pass

    @abstractmethod
    def cast(self, value: Any, type_):
        if type_ in ["int", "float", "bool", "str"]:
            import importlib
            module = importlib.import_module('builtins')
            cls = getattr(module, type_)
            return cls(value)
        raise ValueError()
        # int
        # float
        # bool
        # str
        # object
        #  cls = getattr(module, type_)
        # try:
        #  # Check if it's a builtin type
        #  module = importlib.import_module('builtins')
        #  cls = getattr(module, type_)
        # except AttributeError:
        #  # if not, separate module and class
        #  module, type_ = type_.rsplit(".", 1)
        #  module = importlib.import_module(module)
        #  cls = getattr(module, type_)
        # return cls(value)

    @abstractmethod
    def max(self):
        import math
        return math.inf

    @abstractmethod
    def min(self):
        import math
        return -math.inf

    @abstractmethod
    def sup(self):
        import math
        return math.inf

    @abstractmethod
    def inf(self):
        import math
        return -math.inf

    @abstractmethod
    def plus(self, value: Any) -> Any:
        return + value

    @abstractmethod
    def minus(self, value: Any) -> Any:
        return - value

    @abstractmethod
    def add(self, lhs: Any, rhs: Any) -> Any:
        return lhs + rhs

    @abstractmethod
    def sub(self, lhs: Any, rhs: Any) -> Any:
        return lhs - rhs

    @abstractmethod
    def mult(self, lhs: Any, rhs: Any) -> Any:
        return lhs * rhs

    @abstractmethod
    def div(self, lhs: Any, rhs: Any) -> Any:
        return lhs / rhs

    @abstractmethod
    def mod(self, lhs: Any, rhs: Any) -> Any:
        return lhs % rhs

    @abstractmethod
    def pow(self, lhs: Any, rhs: Any) -> Any:
        return lhs ** rhs
