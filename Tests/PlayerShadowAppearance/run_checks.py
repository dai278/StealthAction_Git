#!/usr/bin/env python3
"""影解除とカメラ非表示の実関数を代用型で検証する。UEの描画・物理は対象外。"""
from pathlib import Path
import os
import subprocess
import tempfile

HERE = Path(__file__).resolve().parent
SOURCE = HERE.parents[1] / "Source/StealthAction"
methods = []
for filename, signatures in [
    ("PlayerCharacter/PlayerCharacter.cpp", ["void APlayerCharacter::UpdateMeshCameraVisibility()"]),
    ("Private/State/Player/PlayerShadowState.cpp", ["void UPlayerShadowState::Exit()", "void UPlayerShadowState::RestoreShadowState()"]),
]:
    data = (SOURCE / filename).read_bytes()
    try:
        text = data.decode("utf-8-sig")
    except UnicodeDecodeError:
        text = data.decode("cp932")
    for signature in signatures:
        start = text.index(signature)
        end = text.index("{", start) + 1
        depth = 1
        while depth:
            depth += (text[end] == "{") - (text[end] == "}")
            end += 1
        methods.append(text[start:end])

with tempfile.TemporaryDirectory(prefix="shadow-appearance-") as folder:
    path = Path(folder) / "check.cpp"
    path.write_text((HERE / "appearance.cpp").read_text().replace("// PRODUCTION_METHODS", "\n\n".join(methods)))
    binary = Path(folder) / "check"
    subprocess.run([os.environ.get("CXX", "g++"), "-std=c++17", "-Wall", "-Wextra", "-Werror",
                    str(path), "-o", str(binary)], check=True)
    subprocess.run([str(binary)], check=True)
    print("PASS: BP exit without parent, overlap during restore, material and visibility restore, camera transition, early exit, safe position, single notification")
