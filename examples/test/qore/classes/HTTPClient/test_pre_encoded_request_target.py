#!/usr/bin/env python3
"""Check pre-encoded request targets against an independent HTTP byte peer.

Copyright (C) 2026 Qore Technologies, s.r.o.
RFC 3986 sections 2.2, 2.3 and 3.3 define the URI characters used here.
"""
from contextlib import contextmanager
import os
from pathlib import Path
import socket
import subprocess
import sys
import threading
import unittest

ROOT = Path(__file__).resolve().parent


@contextmanager
def peer(count):
    listener = socket.socket()
    listener.settimeout(180 if os.getenv("QORE_TEST_VALGRIND_DIR") else 30)
    listener.bind(("127.0.0.1", 0))
    listener.listen()
    requests, errors = [], []

    def serve():
        try:
            while len(requests) < count:
                with listener.accept()[0] as connection:
                    connection.settimeout(20)
                    request = bytearray()
                    while b"\r\n\r\n" not in request:
                        part = connection.recv(1024)
                        if not part or len(request) + len(part) > 16384:
                            raise AssertionError("incomplete or oversized request")
                        request.extend(part)
                    requests.append(bytes(request).split(b"\r\n", 1)[0])
                    connection.sendall(b"HTTP/1.0 200 OK\r\nContent-Length: 2\r\n\r\nOK")
        except BaseException as error:
            errors.append(error)

    thread = threading.Thread(target=serve, daemon=True)
    thread.start()
    try:
        yield f"http://127.0.0.1:{listener.getsockname()[1]}", requests
    finally:
        failed = sys.exc_info()[0] is not None
        try:
            listener.shutdown(socket.SHUT_RDWR)
        except OSError:
            pass
        listener.close()
        thread.join(timeout=25)
        if thread.is_alive():
            raise AssertionError("HTTP peer did not terminate")
        if errors and not failed:
            raise errors[0]


class PreEncodedRequestTargetTest(unittest.TestCase):
    def exchange(self, paths, expected, mode):
        command = ["qore", "-b", "--enable-debug", "--exec-mode=" + os.getenv("QORE_EXEC_MODE", "jit")]
        if log_directory := os.getenv("QORE_TEST_VALGRIND_DIR"):
            command = ["valgrind", "--error-exitcode=90", "--leak-check=full",
                       "--show-leak-kinds=definite,indirect,possible",
                       "--errors-for-leak-kinds=definite,indirect,possible",
                       "--log-file=" + str(Path(log_directory) / (self._testMethodName + ".log")), *command]
        with peer(expected.count("OK")) as (url, requests):
            process = subprocess.run([*command, str(ROOT / "pre-encoded-request-target.qr"), url, mode, *paths],
                                     capture_output=True, text=True, timeout=180)
            self.assertEqual(0, process.returncode, process.stdout + process.stderr)
            self.assertEqual("", process.stderr)
            self.assertEqual(expected, process.stdout.splitlines())
        self.assertEqual([f"GET {path} HTTP/1.1".encode("ascii")
                          for path, result in zip(paths, expected, strict=True) if result == "OK"], requests)

    def test_constructor_preserves_unreserved_and_reserved_characters(self):
        paths = ["/~user/ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-._~",
                 "/a:b@c!$&'()*+,;=d", "/a%2Fb%3Fc%23d%25e%7E", "/%c3%a9/%E4%B8%AD",
                 "/query?a=b%2Fc&d=~", "/"]
        self.exchange(paths, ["OK"] * len(paths), "constructor")

    def test_setter_preserves_literal_and_encoded_tildes(self):
        paths = ["/~user/catalog.xsd", "/%7euser/catalog.xsd", "/~~?name=~%7E"]
        self.exchange(paths, ["OK"] * len(paths), "setter")

    def test_rejected_unencoded_characters_leave_the_client_usable(self):
        paths = ["/bad" + character + "path" for character in "{}|\\^[]`" + "".join(map(chr, range(1, 32)))]
        self.exchange([*paths, "/~recovered"], ["ERROR:URL-ENCODING-ERROR"] * len(paths) + ["OK"], "setter")


if __name__ == "__main__":
    unittest.main()
