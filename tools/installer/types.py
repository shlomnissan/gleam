from __future__ import annotations

from dataclasses import dataclass
from pathlib import Path

@dataclass
class ConfigurationOptions:
    os_name: str
    install_prefix: Path
    build_shared: bool
    build_imgui: bool