from __future__ import annotations
from dataclasses import dataclass, field
from typing import Dict, Optional, List

@dataclass
class Class:
    id: str
    name: str
    display: str
    slug: str
    group_id: Optional[str]

@dataclass
class Group:
    id: str
    name: str
    slug: str
    class_ids: List[str] = field(default_factory=list)

@dataclass
class Member:
    id: str
    parent_id: str
    name: str
    kind: str
    slug: str

@dataclass
class Inventory:
    groups: Dict[str, Group] = field(default_factory=dict)
    classes: Dict[str, Class] = field(default_factory=dict)
    members: Dict[str, Member] = field(default_factory=dict)
    class_to_group: Dict[str, str] = field(default_factory=dict)

@dataclass
class Description:
    brief: str
    details: str

@dataclass
class TypePart:
    text: str
    id: Optional[str] = None

@dataclass
class Type:
    parts: List[TypePart] = field(default_factory=list)

@dataclass
class VarDoc:
    id: str
    name: str
    type: Type
    init_value: Optional[str]
    brief: str
    details: str
    line: int

@dataclass
class EnumDoc:
    id: str
    name: str
    scoped: bool
    brief: str
    details: str
    values: Dict[str, str] = field(default_factory=dict)

@dataclass
class TypedefDoc:
    id: str
    name: str
    definition: str
    type: Type
    brief: str
    details: str

@dataclass
class ParamDoc:
    name: str
    type: Type
    brief: str
    init_value: str

@dataclass
class FunctionDoc:
    id: str
    name: str
    definition: str
    type: Type
    virtual: bool
    brief: str
    details: str
    params: List[ParamDoc] = field(default_factory=list)

@dataclass
class VarGroupDoc:
    header: str
    description: str
    variables: List[VarDoc] = field(default_factory=list)

@dataclass
class ClassDoc:
    id: str
    name: str
    display: str
    base_class_id: Optional[str]
    brief: str
    details: str
    inner_classes: List[ClassDoc] = field(default_factory=list)
    constructors: List[FunctionDoc] = field(default_factory=list)
    factories: List[FunctionDoc] = field(default_factory=list)
    functions: List[FunctionDoc] = field(default_factory=list)
    variables: List[VarDoc] = field(default_factory=list)
    var_groups: List[VarGroupDoc] = field(default_factory=list)
    enums: List[EnumDoc] = field(default_factory=list)
    typedefs: List[TypedefDoc] = field(default_factory=list)