from __future__ import annotations
from ..resolver import Resolver
from pathlib import Path
from typing import Optional, List

import xml.etree.ElementTree as ET

def _read_program_listing(listing: ET.Element, lang: str = "cpp"):
    lines: List[str] = []
    for line in listing.findall("codeline"):
        lines.append(read_pieces(line, None, False))
    return f"\n```{lang}\n{("\n".join(lines).rstrip("\n"))}\n```\n"

def element_text(el: ET.Element, default: str = "") -> str:
    if el is None or el.text is None:
        return default
    return el.text.strip()

def bool_attr(el: ET.Element, name: str):
    return (el.get(name) or "").lower() in ("yes", "true", "1")

def write_if_changed(path: Path, content: str) -> bool:
    path.parent.mkdir(parents=True, exist_ok=True)
    if path.exists() and path.read_text(encoding="utf-8") == content:
        return False
    path.write_text(content, encoding="utf-8")
    return True

def read_pieces(el: ET.Element, resolver: Optional[Resolver], strip: bool = True):
    output: List[str] = []

    def walk(node: ET.Element):
        if el is None: return
        if node.text: output.append(node.text)

        for child in node:
            tag = child.tag
            if tag == "ref":
                label = read_pieces(child, resolver) or element_text(child)
                if resolver:
                    child_id = child.get("refid")
                    label = resolver.id_to_url_with_label(child_id, label)
                output.append(label)
            elif tag == "itemizedlist":
                walk(child)
            elif tag == "listitem":
                label = read_pieces(child, resolver) or element_text(child)
                output.append(f"- {label}\n")
            elif tag == "linebreak":
                output.append("  \n")
            elif tag == "sp":
                output.append(" ")
            elif tag == "programlisting":
                output.append(_read_program_listing(child))
            elif tag == "simplesect" or tag == "parameteritem":
                continue
            else:
                output.append(read_pieces(child, resolver, strip))

            if child.tail:
                output.append(child.tail)
                if child.tag == "para": output.append("\n")

    walk(el)
    return "".join(output).strip() if strip else "".join(output)