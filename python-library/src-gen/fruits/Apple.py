from dataclasses import dataclass, field
from dataclasses_json import dataclass_json, Undefined
from typing import Optional, List
import fruits.Color

@dataclass_json(undefined=Undefined.RAISE)
@dataclass
class Apple:
    color: fruits.Color.Color
    size_cm: float

def createApple():
    return Apple(list(fruits.Color.Color)[0],float())
