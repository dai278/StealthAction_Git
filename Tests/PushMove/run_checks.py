#!/usr/bin/env python3
"""押す方向と移動の実関数をUE型の代用品で検証する。実際の衝突はUEで確認する。"""
from pathlib import Path
import os
import subprocess
import tempfile

HERE = Path(__file__).resolve().parent
source = (HERE.parents[1] / "Source/StealthAction/Private/Interact/PushMoveActor.cpp").read_bytes().decode("cp932")
methods = []
for signature in ["void APushMoveActor::Interact(AActor* _interactOwner)", "void APushMoveActor::Tick("]:
    start = source.index(signature)
    end = source.index("{", start) + 1
    depth = 1
    while depth:
        depth += (source[end] == "{") - (source[end] == "}")
        end += 1
    methods.append(source[start:end])

with tempfile.TemporaryDirectory(prefix="push-move-") as folder:
    path = Path(folder) / "check.cpp"
    path.write_text((HERE / "movement.cpp").read_text().replace("// PRODUCTION_METHODS", "\n\n".join(methods)))
    binary = Path(folder) / "check"
    subprocess.run([os.environ.get("CXX", "g++"), "-std=c++17", "-Wall", "-Wextra", "-Werror",
                    str(path), "-o", str(binary)], check=True)
    subprocess.run([str(binary)], check=True)
    print("PASS: four sides, corner facing and position jitter, rotation, translation, rectangular bounds, scale, invalid input, overshoot, blocking")
