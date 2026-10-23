#!/usr/bin/env python3
"""UEを代用したC++型で、実際の切り替え関数をコンパイルして検証する。
UHT、UEのBT実行、Perception、アセット設定の検証にはならない。
"""
from pathlib import Path
import os
import subprocess
import tempfile

HERE = Path(__file__).resolve().parent
ROOT = HERE.parents[1]
SOURCE = ROOT / "Source/StealthAction/Private/Enemy/Modular"


def extract(filename, signature):
    text = (SOURCE / filename).read_text(encoding="utf-8")
    start = text.index(signature)
    start = text.rfind("\n", 0, start) + 1
    brace = text.index("{", start)
    depth, end = 1, brace + 1
    while depth:
        depth += (text[end] == "{") - (text[end] == "}")
        end += 1
    return text[start:end]


CASES = {
    "switch": ("ModularEnemyAIController.cpp", [
        "void AModularEnemyAIController::RequestBehaviorTree",
        "void AModularEnemyAIController::TryStartPendingTree",
        "void AModularEnemyAIController::StopEnemyLogic",
    ]),
    "attack": ("ModularEnemyCharacter.cpp", [
        "void AModularEnemyCharacter::Tick",
        "bool AModularEnemyCharacter::BeginAttack",
        "void AModularEnemyCharacter::CancelAttack",
    ]),
    "states": ("ModularEnemyStateMachine.cpp", [
        "bool UModularEnemyStateMachine::ChangeEnemyState",
        "bool UModularEnemyStateMachine::ReturnToAction",
        "void UModularEnemyStateMachine::Update",
        "void UModularEnemyStateMachine::NotifyDamage",
        "void UModularEnemyStateMachine::Stop",
    ]),
}

with tempfile.TemporaryDirectory(prefix="modular-enemy-") as output:
    for name, (filename, methods) in CASES.items():
        fixture = (HERE / f"{name}.cpp").read_text(encoding="utf-8")
        production = "\n\n".join(extract(filename, signature) for signature in methods)
        generated = Path(output) / f"{name}.cpp"
        generated.write_text(fixture.replace("// PRODUCTION_METHODS", production), encoding="utf-8")
        binary = Path(output) / name
        subprocess.run([os.environ.get("CXX", "g++"), "-std=c++17", "-Wall", "-Wextra", "-Werror",
                        str(generated), "-o", str(binary)], check=True)
        subprocess.run([str(binary)], check=True)
        print(f"PASS: {name}", flush=True)
