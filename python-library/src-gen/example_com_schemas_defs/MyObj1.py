from dataclasses import dataclass, field
from dataclasses_json import dataclass_json, Undefined
from typing import Optional, List
@dataclass_json(undefined=Undefined.RAISE)
@dataclass
class MyObj1:
    s: str
    i: int
    f: float

def createMyObj1():
    return MyObj1(str(),int(),float())
