#!/usr/bin/env python3
"""通常攻撃の実際の関数を、UE型の代用品でコンパイルして検証する。"""
from pathlib import Path
import os
import subprocess
import tempfile

HERE = Path(__file__).resolve().parent
SOURCE = HERE.parents[1] / "Source/StealthAction/Private/State/Player"


def extract(filename, signature):
    data = (SOURCE / filename).read_bytes()
    try:
        text = data.decode("utf-8-sig")
    except UnicodeDecodeError:
        text = data.decode("cp932")
    start = text.index(signature)
    start = text.rfind("\n", 0, start) + 1
    end = text.index("{", start) + 1
    depth = 1
    while depth:
        depth += (text[end] == "{") - (text[end] == "}")
        end += 1
    return text[start:end]


CASES = {
    "legacy_damage": ("../../Enemy/EnemyBase.cpp", ["void AEnemyBase::OnDamage"]),
    "selection": ("PlayerStateMachine.cpp", ["void UPlayerStateMachine::TryAttack"]),
    "timing": ("PlayerNormalAttackState.cpp", [
        "void UPlayerNormalAttackState::OnUpdate_Implementation",
        "void UPlayerNormalAttackState::BackgroundUpdate",
        "void UPlayerNormalAttackState::OnAttackMontageEnded",
        "void UPlayerNormalAttackState::OnAttackMontageBlendingOut",
    ]),
    "hits": ("PlayerNormalAttackState.cpp", ["void UPlayerNormalAttackState::ApplyAttackHits"]),
}
with tempfile.TemporaryDirectory(prefix="player-normal-attack-") as output:
    for name, (filename, signatures) in CASES.items():
        source = (HERE / f"{name}.cpp").read_text()
        production = "\n\n".join(extract(filename, signature) for signature in signatures)
        path = Path(output) / f"{name}.cpp"
        path.write_text(source.replace("// PRODUCTION_METHODS", production))
        binary = Path(output) / name
        subprocess.run([os.environ.get("CXX", "g++"), "-std=c++17", "-Wall", "-Wextra", "-Werror",
                        "-Wno-unused-parameter", str(path), "-o", str(binary)], check=True)
        subprocess.run([str(binary)], check=True)
        print(f"PASS: {name}", flush=True)
