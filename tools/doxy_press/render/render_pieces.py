from __future__ import annotations
from ..model import (
    ClassDoc,
    EnumDoc,
    FunctionDoc,
    Type,
    TypedefDoc,
    VarDoc,
)
from ..resolver import Resolver
from ..strings import (
    escape_angle_brackets,
    remove_first_qualification,
    tighten_template_spaces,
)
from html import escape

import re

def _t_clean(s: str):
    s = re.sub(r'\boverride\b', '', s)
    s = re.sub(r'\s*=\s*0\b', '', s)
    s = tighten_template_spaces(s)
    return re.sub(r'\s+', ' ', s).strip()

def _t_resolved(t: Type, resolver: Resolver):
    output = ""
    for p in t.parts:
        output += resolver.id_to_url_with_label(p.id, p.text) if p.id else p.text
    return escape_angle_brackets(_t_clean(output))

def _t_str(t: Type):
    return _t_clean("".join(p.text.strip() for p in t.parts))

def _badge(s: str, t: str):
    return f"<Badge type=\"{t}\" text=\"{s}\" />"

def render_variable(v: VarDoc, resolver: Resolver):
    init_value = "{}"
    if v.init_value:
        init_value = f"{{{v.init_value}}}"

    return (
        f"<div class=\"docblock\">"
        f"<div class=\"definition\">\n\n"
        f"### <span class=\"name\">{escape(v.name)}</span> "
        f"<span class=\"type\">{_t_resolved(v.type, resolver)}</span>"
        f"{resolver.member_id_to_anchor(v.id)}\n"
        f"</div>"
        f"<div class=\"description\">\n\n"
        f"{v.brief}\n\n {v.details}\n"
        f"```cpp\n{_t_str(v.type)} {v.name} {init_value};\n```\n"
        f"</div>"
        f"</div>"
    )

def render_function(f: FunctionDoc, resolver: Resolver):
    table = ""
    if f.params:
        table = "|Parameter|Description|\n|---|---|\n"
        for p in f.params:
            table += f"<span class=\"type\">{p.name}</span> "
            table += f"| {p.brief}\n"

    badge = ""
    if f.virtual in ("virtual", "pure-virtual"):
        badge = _badge("pure virtual" if f.virtual == "pure-virtual" else "virtual", "info")

    return (
        f"<div class=\"docblock\">"
        f"<div class=\"definition\">\n\n"
        f"### <span class=\"name\">{escape(f.name)}()</span> "
        f"<span class=\"type\">{_t_resolved(f.type, resolver)}</span> "
        f"{badge}"
        f"{resolver.member_id_to_anchor(f.id)}\n"
        f"</div>"
        f"<div class=\"description\">\n\n"
        f"{f.brief}\n\n {f.details}\n"
        f"```cpp\n{f.definition};\n```\n"
        f"{table}"
        f"</div>"
        f"</div>"
    )

def render_enum(e: EnumDoc, resolver: Resolver):
    badge = "scoped enum" if e.scoped else "enum"
    qualified_name = remove_first_qualification(e.name)
    table = "|Value|Description|\n|---|---|\n"
    for val, desc in e.values.items():
        table += f"| <span class=\"type\">{val}</span> | {desc}\n"

    return (
        f"<div class=\"docblock\">"
        f"<div class=\"definition\">\n\n"
        f"### <span class=\"name\">{qualified_name}</span> "
        f"{_badge(badge, "info")}"
        f"{resolver.member_id_to_anchor(e.id)}\n"
        f"</div>"
        f"<div class=\"description\">\n\n"
        f"{e.brief}\n\n {e.details}\n"
        f"{table}"
        f"</div>"
        f"</div>"
    )

def render_inner_class(c: ClassDoc, resolver: Resolver):
    qualified_name = remove_first_qualification(c.name)
    table = "|Parameter|Description|\n|---|---|\n"
    for v in c.variables:
        table += f"<span class=\"type\">{v.name}</span> "
        table += f"<span class=\"inner-type\">{_t_resolved(v.type, resolver)}</span> "
        table += f"| {v.brief}\n"

    return (
        f"<div class=\"docblock\">"
        f"<div class=\"definition\">\n\n"
        f"### <span class=\"name\">{qualified_name}</span> "
        f"{_badge("struct", "info")}"
        f"{resolver.member_id_to_anchor(c.id)}\n"
        f"</div>"
        f"<div class=\"description\">\n\n"
        f"{c.brief}\n\n {c.details}\n"
        f"{table}"
        f"</div>"
        f"</div>"
    )

def render_typedef(t: TypedefDoc, resolver: Resolver):
    return (
        f"<div class=\"docblock\">"
        f"<div class=\"definition\">\n\n"
        f"### <span class=\"name\">{escape(t.name)}</span> "
        f"{_badge("typedef", "info")}"
        f"{resolver.member_id_to_anchor(t.id)}\n"
        f"</div>"
        f"<div class=\"description\">\n\n"
        f"{t.brief}\n\n {t.details}\n"
        f"```cpp\n{t.definition}\n```\n"
        f"</div>"
        f"</div>"
    )