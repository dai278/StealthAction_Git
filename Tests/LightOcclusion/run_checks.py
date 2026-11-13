#!/usr/bin/env python3
"""ライト判定の実関数を代用型で検証する。UEのメッシュ衝突は対象外。"""
from pathlib import Path
import os
import subprocess
import tempfile

HERE = Path(__file__).resolve().parent
source = (HERE.parents[1] / "Source/StealthAction/Private/Light/ExtendedSpotLight.cpp").read_bytes().decode("cp932")
start = source.index("bool AExtendedSpotLight::IsHit(")
end = source.index("{", start) + 1
depth = 1
while depth:
    depth += (source[end] == "{") - (source[end] == "}")
    end += 1
with tempfile.TemporaryDirectory(prefix="light-occlusion-") as folder:
    path = Path(folder) / "check.cpp"
    path.write_text((HERE / "occlusion.cpp").read_text().replace("// PRODUCTION_METHOD", source[start:end]))
    binary = Path(folder) / "check"
    subprocess.run([os.environ.get("CXX", "g++"), "-std=c++17", "-Wall", "-Wextra", "-Werror",
                    str(path), "-o", str(binary)], check=True)
    subprocess.run([str(binary)], check=True)
    print("PASS: skip shapes, mesh behind capsule, static and skeletal meshes, endpoint, range, cone, complex flag")
