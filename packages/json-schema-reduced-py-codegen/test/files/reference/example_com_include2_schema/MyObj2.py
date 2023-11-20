from dataclasses import dataclass, field
from dataclasses_json import dataclass_json, Undefined
from typing import Optional, List
@dataclass_json(undefined=Undefined.RAISE)
@dataclass
class MyObj2:
    s: str
    i: int

def createMyObj2():
    return MyObj2(str(),int())
