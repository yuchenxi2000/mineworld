#!/usr/bin/env python3
"""Validate a macOS release ZIP and optionally run its matching C++ tests."""

import argparse
from pathlib import Path
import platform
import subprocess
import tempfile


def run(command, **kwargs):
    return subprocess.run(command, check=True, timeout=30, **kwargs)


def stop(process):
    process.terminate()
    try:
        return process.communicate(timeout=5)
    except subprocess.TimeoutExpired:
        process.kill()
        return process.communicate(timeout=5)


def check_startup(binary, architecture, directory, bundled_glew):
    # arch is a system binary: pass DYLD diagnostics via -e so they reach the app.
    process = subprocess.Popen(
        ["/usr/bin/arch", "-" + architecture, "-e", "DYLD_PRINT_LIBRARIES=1", str(binary)],
        cwd=directory,
        stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True,
    )
    try:
        try:
            stdout, stderr = process.communicate(timeout=5)
        except subprocess.TimeoutExpired:
            stdout, stderr = stop(process)
        else:
            raise RuntimeError(
                f"{architecture}: app exited early ({process.returncode})\n{stdout}\n{stderr}"
            )
    finally:
        if process.poll() is None:
            stop(process)

    print(stdout, end="", flush=True)
    if "[texture] current atlas size" not in stdout:
        raise RuntimeError(f"{architecture}: texture initialization did not complete\n{stderr}")
    glew_lines = [line for line in stderr.splitlines() if "libglew" in line.lower()]
    for library in bundled_glew:
        if not any(str(library) in line for line in glew_lines):
            raise RuntimeError(f"{architecture}: bundled GLEW was not loaded\n{stderr}")
    if any(not any(str(library) in line for library in bundled_glew) for line in glew_lines):
        raise RuntimeError(f"{architecture}: unexpected external GLEW loaded\n{stderr}")
    print(f"PASS: {architecture} extracted app ran for 5 seconds; terminated by test.", flush=True)


def check_package(archive, architectures, test_build):
    resources = Path(__file__).resolve().parents[1] / "res"
    with tempfile.TemporaryDirectory(prefix="mineworld-package-") as directory:
        run(["/usr/bin/ditto", "-x", "-k", str(archive), directory])
        app = Path(directory) / "MineWorld.app"
        binary = app / "Contents/MacOS/mineworld"
        run(["/usr/bin/plutil", "-lint", str(app / "Contents/Info.plist")])
        run(["/usr/bin/codesign", "--verify", "--deep", "--strict", str(app)])
        run(["/usr/bin/lipo", str(binary), "-verify_arch", *architectures])
        if test_build:
            for name in ("mineworld_tests", "mineworld_gl_smoke"):
                run(["/usr/bin/lipo", str(test_build / name), "-verify_arch", *architectures])

        for architecture in architectures:
            dependencies = run(
                ["/usr/bin/otool", "-arch", architecture, "-L", str(binary)],
                capture_output=True, text=True,
            ).stdout.splitlines()[1:]
            bundled_glew = []
            for line in dependencies:
                dependency = line.strip().split(" (", 1)[0]
                if "libglew" not in dependency.lower():
                    continue
                prefix = "@executable_path/"
                if not dependency.startswith(prefix):
                    raise RuntimeError(f"{architecture}: external GLEW dependency: {dependency}")
                library = binary.parent / dependency[len(prefix):]
                if not library.is_file():
                    raise RuntimeError(f"{architecture}: bundled GLEW is missing: {library}")
                run(["/usr/bin/lipo", str(library), "-verify_arch", architecture])
                bundled_glew.append(library)
            print(
                f"{architecture}: " + ("bundled dynamic GLEW" if bundled_glew else "static GLEW"),
                flush=True,
            )
            if test_build:
                for name in ("mineworld_tests", "mineworld_gl_smoke"):
                    print(f"Testing {architecture}: {name}", flush=True)
                    run(["/usr/bin/arch", "-" + architecture, str(test_build / name)], cwd=resources)
            check_startup(binary, architecture, directory, bundled_glew)


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("archive", type=Path, help="ZIP containing MineWorld.app")
    parser.add_argument(
        "--architectures", nargs="+", choices=("arm64", "x86_64"),
        default=[platform.machine()], help="architectures to run (default: native)",
    )
    parser.add_argument(
        "--test-build", type=Path,
        help="optional CMake build directory containing regression and OpenGL test binaries",
    )
    args = parser.parse_args()
    if platform.system() != "Darwin":
        parser.error("this test requires macOS with a desktop graphics session")
    archive = args.archive.resolve()
    if not archive.is_file():
        parser.error(f"archive does not exist: {archive}")
    try:
        check_package(
            archive, args.architectures,
            args.test_build.resolve() if args.test_build else None,
        )
    except (OSError, RuntimeError, subprocess.SubprocessError) as error:
        parser.exit(1, f"FAIL: {error}\n")
    print("All macOS package checks passed.")


if __name__ == "__main__":
    main()
