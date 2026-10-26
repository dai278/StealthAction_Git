#!/usr/bin/env python3
"""しゃがみの実関数と影への縮小処理を代用型で検証する。UEの物理は対象外。"""
from pathlib import Path
import os
import subprocess
import tempfile

HERE = Path(__file__).resolve().parent
SOURCE = HERE.parents[1] / "Source/StealthAction/Private/State/Player"
source = (SOURCE / "PlayerCrouchState.cpp").read_text(encoding="utf-8-sig")
methods = []
for signature in ["void UPlayerCrouchState::OnEnter_Implementation()", "void UPlayerCrouchState::OnExit_Implementation()",
                  "void UPlayerCrouchState::ResizeCapsuleAtFeet("]:
    start = source.index(signature)
    end = source.index("{", start) + 1
    depth = 1
    while depth:
        depth += (source[end] == "{") - (source[end] == "}")
        end += 1
    methods.append(source[start:end])
shadow = (SOURCE / "PlayerShadowState.cpp").read_text(encoding="utf-8-sig")
start = shadow.index("\tOriginalCollisionProfile = Capsule->GetCollisionProfileName();")
end = shadow.index("\tSinkStartMeshLocation = Mesh->GetRelativeLocation();", start)
end = shadow.index(";", end) + 1
geometry = shadow[start:end]
with tempfile.TemporaryDirectory(prefix="player-crouch-") as folder:
    path = Path(folder) / "check.cpp"
    fixture = (HERE / "crouch.cpp").read_text()
    path.write_text(fixture.replace("// PRODUCTION_METHODS", "\n\n".join(methods)).replace("// PRODUCTION_SHADOW_GEOMETRY", geometry))
    binary = Path(folder) / "check"
    subprocess.run([os.environ.get("CXX", "g++"), "-std=c++17", "-Wall", "-Wextra", "-Werror",
                    str(path), "-o", str(binary)], check=True)
    subprocess.run([str(binary)], check=True)
    print("PASS: stable feet and mesh, scaled capsule, radius clamp, crouch to shadow under ceiling, safe standing restore")
