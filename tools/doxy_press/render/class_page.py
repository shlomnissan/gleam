from __future__ import annotations
from ..model import ClassDoc
from ..render.render_pieces import (
    render_enum,
    render_function,
    render_inner_class,
    render_typedef,
    render_var_group,
    render_variable,
)
from ..resolver import Resolver
from typing import List

def _render_base_class(c: ClassDoc, resolver: Resolver):
    if not c.base_class_id: return ""
    t = resolver.class_id_to_url(c.base_class_id)
    return f"::: info\nDerives from {t} and inherits all public properties and methods.\n:::"

def _render_class_header(c: ClassDoc, resolver: Resolver):
    return (
        f"# {c.display}\n"
        f"<div class=\"docblock docblock-class\">"
        f"<div class=\"description\">"
        f"{c.brief}\n\n"
        f"{c.details}\n"
        f"{_render_base_class(c, resolver)}\n"
        f"</div>"
        f"</div>\n"
    )

def render_class(c: ClassDoc, resolver: Resolver):
    lines: List[str] = []

    lines += [_render_class_header(c, resolver)]

    if c.constructors or c.factories:
        lines += ["## Construction\n"]
        if c.constructors:
            lines += ["### Constructors", "<br/>"]
            for cs in c.constructors:
                lines += [render_function(cs, resolver), "\n"]
        if c.factories:
            lines += ["### Factories <Badge type='warning' text='preferred' />", "<br/>"]
            for f in c.factories:
                lines += [render_function(f, resolver), "\n"]

    if c.enums or c.inner_classes or c.typedefs:
        lines += ["## Types\n"]
        for enum_doc in c.enums:
            lines += [render_enum(enum_doc, resolver), "\n"]
        for class_doc in c.inner_classes:
            lines += [render_inner_class(class_doc, resolver), "\n"]
        for typedef_doc in c.typedefs:
            lines += [render_typedef(typedef_doc, resolver), "\n"]

    if c.variables or c.var_groups:
        lines += ["## Properties\n"]
        for group_doc in c.var_groups:
            lines += [render_var_group(group_doc, resolver), "\n"]
        for var_doc in c.variables:
            lines += [render_variable(var_doc, resolver), "\n"]

    if c.functions:
        lines += ["## Functions\n"]
        for func_doc in c.functions:
            lines += [render_function(func_doc, resolver), "\n"]

    return "\n".join(lines)
