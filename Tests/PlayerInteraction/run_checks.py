#!/usr/bin/env python3
"""実際の接触確認・インタラクト関数をUE型の代用品で検証する。UEの物理やUHTは対象外。"""
from pathlib import Path
import os
import subprocess
import tempfile

HERE = Path(__file__).resolve().parent
SOURCE = HERE.parents[1] / "Source/StealthAction"


def extract(path, name):
    data = (SOURCE / path).read_bytes()
    try:
        text = data.decode("utf-8-sig")
    except UnicodeDecodeError:
        text = data.decode("cp932")
    start = text.index(name)
    start = text.rfind("\n", 0, start) + 1
    end = text.index("{", start) + 1
    depth = 1
    while depth:
        depth += (text[end] == "{") - (text[end] == "}")
        end += 1
    return text[start:end]


CASES = {
    "detection": [("PlayerCharacter/PlayerCharacter.cpp", name) for name in [
        "void APlayerCharacter::GetNearbyInteractTargets",
        "AInteract* APlayerCharacter::FindInteractTarget",
        "bool APlayerCharacter::IsInInteractRange",
        "void APlayerCharacter::RefreshInteractTarget",
    ]],
    "interaction": [
        ("Private/State/Player/PlayerInteractState.cpp", "bool UPlayerInteractState::CanEnter()"),
        ("Private/State/Player/PlayerInteractState.cpp", "void UPlayerInteractState::OnEnter_Implementation"),
        ("Private/State/Player/PlayerInteractState.cpp", "void UPlayerInteractState::OnUpdate_Implementation"),
    ] + [("Private/State/Player/PlayerInteractAnimationState.cpp", name) for name in [
        "bool UPlayerInteractAnimationState::BeginInteraction",
        "bool UPlayerInteractAnimationState::CanEnter()",
        "bool UPlayerInteractAnimationState::IsTargetAvailable",
        "void UPlayerInteractAnimationState::OnUpdate_Implementation",
        "void UPlayerInteractAnimationState::CompleteInteraction",
    ]],
}

with tempfile.TemporaryDirectory(prefix="player-interaction-") as folder:
    for name, methods in CASES.items():
        fixture = (HERE / f"{name}.cpp").read_text()
        code = "\n\n".join(extract(path, method) for path, method in methods)
        path = Path(folder) / f"{name}.cpp"
        path.write_text(fixture.replace("// PRODUCTION_METHODS", code))
        binary = Path(folder) / name
        subprocess.run([os.environ.get("CXX", "g++"), "-std=c++17", "-Wall", "-Wextra", "-Werror",
                        "-Wno-unused-parameter", str(path), "-o", str(binary)], check=True)
        subprocess.run([str(binary)], check=True)
        print(f"PASS: {name}", flush=True)
