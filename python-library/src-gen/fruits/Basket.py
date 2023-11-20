from dataclasses import dataclass, field
from dataclasses_json import dataclass_json, Undefined
from typing import Optional, List
import fruits.Apple

@dataclass_json(undefined=Undefined.RAISE)
@dataclass
class Basket:
    apples: List[fruits.Apple.Apple]

def createBasket():
    return Basket([])
