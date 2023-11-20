from dataclasses import dataclass, field
from dataclasses_json import dataclass_json, Undefined
from typing import Optional, List
import example_com_schemas_defs.MyEnum1
import example_com_schemas_defs.MyObj1

@dataclass_json(undefined=Undefined.RAISE)
@dataclass
class MyObj2:
    b: bool
    s: str
    i8: int
    i16a: int
    i16b: int
    i16: int
    u16: int
    u32: int
    f: float
    o: example_com_schemas_defs.MyObj1.MyObj1
    e: example_com_schemas_defs.MyEnum1.MyEnum1
    a1: List[int]
    a2: List[str]
    a3: List[example_com_schemas_defs.MyEnum1.MyEnum1]
    a4: List[example_com_schemas_defs.MyObj1.MyObj1]

def createMyObj2():
    return MyObj2(bool(),str(),int(),int(),int(),int(),int(),int(),float(),example_com_schemas_defs.MyObj1.MyObj1(),list(example_com_schemas_defs.MyEnum1.MyEnum1)[0],[],[],[],[])
