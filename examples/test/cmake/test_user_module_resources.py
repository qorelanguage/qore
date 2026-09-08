#!/usr/bin/env python3
"""Verify source-only module resource installs with CMAKE_EXECUTABLE (or cmake).

Copyright (C) 2026 Qore Technologies, s.r.o.
"""

import os
from pathlib import Path
import subprocess
import tempfile
import unittest

ROOT = Path(__file__).resolve().parents[3]
CMAKE = os.environ.get("CMAKE_EXECUTABLE", "cmake")


class UserModuleResourcesTest(unittest.TestCase):
    def test_directory_resources(self):
        for registration in ('qore_user_module("qlib/Fixture")',
                             'qore_external_user_module("qlib/Fixture" "")',
                             'qore_user_modules("qlib/Fixture")'):
            with self.subTest(registration=registration), tempfile.TemporaryDirectory(
                    prefix="qore-module-resources-") as directory:
                root = Path(directory)
                source = root / "source with spaces"
                module = source / "qlib/Fixture"
                module.mkdir(parents=True)
                resources = {"Fixture.qm": "%modern\n", "Part.qc": "# fixture\n",
                             "schema.json": '{"paths": {}}\n', "schema.yaml": "paths: {}\n",
                             "logo.svg": "<svg/>\n", "wire.proto": 'syntax = "proto3";\n'}
                for name, content in resources.items():
                    (module / name).write_text(content)
                (module / "unpackaged.txt").write_text("not a module resource")
                (source / "CMakeLists.txt").write_text(f'''cmake_minimum_required(VERSION 3.14...3.31)
project(ModuleResources NONE)
include("{ROOT.as_posix()}/cmake/QoreMacros.cmake")
set(QORE_BUILD_AOT_MODULES OFF)
set(DOXYGEN_FOUND FALSE)
set(CMAKE_DISABLE_FIND_PACKAGE_Doxygen TRUE)
set(QORE_USER_MODULES_DIR share/qore-modules)
set(QORE_QM_SOURCE_INSTALL_COMPONENT source-modules)
{registration}
''')
                build = root / "build-debug"
                install = root / "install"
                for command in ([CMAKE, "-S", str(source), "-B", str(build),
                                 f"-DCMAKE_INSTALL_PREFIX={install}"],
                                [CMAKE, "--install", str(build), "--component", "source-modules"]):
                    result = subprocess.run(command, capture_output=True, text=True, timeout=30)
                    self.assertEqual(0, result.returncode, result.stdout + result.stderr)
                    self.assertNotIn("Warning", result.stderr)
                installed = install / "share/qore-modules/Fixture"
                self.assertEqual(set(resources), {path.name for path in installed.iterdir()})
                for name, content in resources.items():
                    self.assertEqual(content, (installed / name).read_text())


if __name__ == "__main__":
    unittest.main()
