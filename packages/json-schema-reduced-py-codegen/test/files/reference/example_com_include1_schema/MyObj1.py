from dataclasses import dataclass, field
from dataclasses_json import dataclass_json, Undefined
from typing import Optional, List
import example_com_include2_schema.MyObj2

@dataclass_json(undefined=Undefined.RAISE)
@dataclass
class MyObj1:
    s: str
    i: int
    o: example_com_include2_schema.MyObj2.MyObj2

def createMyObj1():
    return MyObj1(str(),int(),example_com_include2_schema.MyObj2.MyObj2())
