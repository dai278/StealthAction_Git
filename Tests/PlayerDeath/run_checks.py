#!/usr/bin/env python3
"""実際の死亡ステート関数をUE型の代用品で検証する。モンタージュ再生自体はUEで確認する。"""
from pathlib import Path
import os
import subprocess
import tempfile

HERE = Path(__file__).resolve().parent
text = (HERE.parents[1] / "Source/StealthAction/Private/State/Player/PlayerDeadState.cpp").read_text()
methods = []
for signature in [
    "void UPlayerDeadState::OnEnter_Implementation",
    "void UPlayerDeadState::OnUpdate_Implementation",
    "void UPlayerDeadState::BeginGameOverTransition",
    "void UPlayerDeadState::OnExit_Implementation",
]:
    start = text.index(signature)
    start = text.rfind("\n", 0, start) + 1
    end = text.index("{", start) + 1
    depth = 1
    while depth:
        depth += (text[end] == "{") - (text[end] == "}")
        end += 1
    methods.append(text[start:end])
with tempfile.TemporaryDirectory(prefix="player-death-") as folder:
    path = Path(folder) / "check.cpp"
    path.write_text((HERE / "death.cpp").read_text().replace("// PRODUCTION_METHODS", "\n\n".join(methods)))
    binary = Path(folder) / "check"
    subprocess.run([os.environ.get("CXX", "g++"), "-std=c++17", "-Wall", "-Wextra", "-Werror",
                    "-Wno-unused-parameter", str(path), "-o", str(binary)], check=True)
    subprocess.run([str(binary)], check=True)
    print("PASS: montage duration, fade delay, failure fallback, single transition, cleanup")
