from typing import Any


class MyStruct:
    v_: Any

    def __init__(self, v: Any):
        self.v_ = v

    def getV(self) -> Any:
        return self.v_

    def setV(self, v: Any):
        self.v_ = v
