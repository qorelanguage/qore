#!/usr/bin/env python3
"""Exercise test-runner exit classification without waiting for a real timeout.

Copyright (C) 2026 Qore Technologies, s.r.o.
"""

import os
from pathlib import Path
import subprocess
import tempfile
import unittest


ROOT = Path(__file__).resolve().parents[3]


class TimeoutDiagnosticsTest(unittest.TestCase):
    def run_runner(self, exit_code):
        with tempfile.TemporaryDirectory(prefix="qore-runner-diagnostics-") as directory:
            root = Path(directory)
            binaries = root / "bin"
            binaries.mkdir()
            tests = root / "examples/test"
            tests.mkdir(parents=True)
            (tests / "one.qtest").touch()
            (tests / "two.qtest").touch()
            (binaries / "libqore.so").touch()
            scripts = {
                "qore": '#!/bin/sh\ncase "$*" in *one.qtest*) exit "$TEST_EXIT";; esac\nexit 0\n',
                "timeout": '#!/bin/sh\nshift\nexec "$@"\n',
                "gdb": '#!/bin/sh\necho invoked >> "$GDB_MARKER"\n',
                "sysctl": '#!/bin/sh\nexit 1\n',
            }
            for name, source in scripts.items():
                path = binaries / name
                path.write_text(source)
                path.chmod(0o755)
            cores = root / "cores"
            cores.mkdir()
            # A local dummy core keeps the crash case independent of host crash artifacts.
            (cores / "core.fixture").touch()
            marker = root / "gdb-called"
            env = {
                "PATH": str(binaries) + os.pathsep + os.defpath,
                "QORE_BINARY": str(binaries / "qore"),
                "LIBQORE_BINARY": str(binaries / "libqore.so"),
                "CORE_DIR": str(cores),
                "TEST_EXIT": str(exit_code),
                "GDB_MARKER": str(marker),
            }
            result = subprocess.run(["sh", str(ROOT / "run_tests.sh")], cwd=root,
                                    env=env, capture_output=True, text=True, timeout=15)
            return result, marker.exists()

    def test_timeouts_remain_failures_without_debugger_rerun(self):
        for code in (124, 143):
            with self.subTest(code=code):
                result, debugger = self.run_runner(code)
                self.assertEqual(1, result.returncode, result.stdout + result.stderr)
                self.assertIn("TIMEOUT: test exceeded", result.stdout)
                self.assertNotIn("CRASH:", result.stdout)
                self.assertFalse(debugger, result.stdout)
                self.assertIn("Running test (2/2)", result.stdout)

    def test_unexpected_signal_keeps_crash_diagnostics(self):
        result, debugger = self.run_runner(139)
        self.assertEqual(1, result.returncode, result.stdout + result.stderr)
        self.assertIn("CRASH: test killed by signal 11", result.stdout)
        self.assertNotIn("TIMEOUT:", result.stdout)
        self.assertTrue(debugger, result.stdout)

    def test_ordinary_failure_does_not_invoke_debugger(self):
        result, debugger = self.run_runner(1)
        self.assertEqual(1, result.returncode, result.stdout + result.stderr)
        self.assertNotIn("TIMEOUT:", result.stdout)
        self.assertNotIn("CRASH:", result.stdout)
        self.assertFalse(debugger, result.stdout)

    def test_success(self):
        result, debugger = self.run_runner(0)
        self.assertEqual(0, result.returncode, result.stdout + result.stderr)
        self.assertFalse(debugger, result.stdout)


if __name__ == "__main__":
    unittest.main()
