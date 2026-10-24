#!/usr/bin/env python3
"""画面遷移の実際の関数をUE/Slateの代用型で検証する。UE描画やロード自体は対象外。"""
from pathlib import Path
import os
import subprocess
import tempfile

HERE = Path(__file__).resolve().parent
SOURCE = HERE.parents[1] / "Source/StealthAction/Private/UI/Transition/LevelTransitionSubsystem.cpp"
text = SOURCE.read_text()
methods = []
for signature in [
    "bool ULevelTransitionSubsystem::OpenLevelWithFade",
    "bool ULevelTransitionSubsystem::IsTransitionActive",
    "UWorld* ULevelTransitionSubsystem::GetTickableGameObjectWorld",
    "void ULevelTransitionSubsystem::Tick",
    "void ULevelTransitionSubsystem::AdvanceFade",
    "void ULevelTransitionSubsystem::OnMapLoaded",
    "void ULevelTransitionSubsystem::OnTravelFailed",
    "void ULevelTransitionSubsystem::SetFadeOpacity",
    "void ULevelTransitionSubsystem::FinishTransition",
]:
    start = text.index(signature)
    start = text.rfind("\n", 0, start) + 1
    end = text.index("{", start) + 1
    depth = 1
    while depth:
        depth += (text[end] == "{") - (text[end] == "}")
        end += 1
    methods.append(text[start:end])
with tempfile.TemporaryDirectory(prefix="level-fade-") as folder:
    fixture = (HERE / "transition.cpp").read_text()
    path = Path(folder) / "transition.cpp"
    path.write_text(fixture.replace("// PRODUCTION_METHODS", "\n\n".join(methods)))
    binary = Path(folder) / "check"
    subprocess.run([os.environ.get("CXX", "g++"), "-std=c++17", "-Wall", "-Wextra", "-Werror",
                    "-Wno-unused-parameter", str(path), "-o", str(binary)], check=True)
    subprocess.run([str(binary)], check=True)
    print("PASS: fade-out, travel, fade-in, duplicates, failure cleanup, zero durations")
