#!/usr/bin/env python3
"""Caught TLS handshake failures preserve diagnostics without writing to stderr.

Copyright (C) 2026 Qore Technologies, s.r.o.
Run with a Debug Qore in PATH and LD_LIBRARY_PATH pointing to its build directory.
QORE_TEST_VALGRIND_DIR enables Valgrind with separate, unfiltered diagnostic logs.
"""
from contextlib import contextmanager
import os
from pathlib import Path
import socket
import ssl
import subprocess
import sys
import tempfile
import threading
import unittest

ROOT = Path(__file__).resolve().parent


@contextmanager
def peer(context):
    listener = socket.socket()
    # Instrumented Qore startup can exceed the network operation's own deadline.
    listener.settimeout(180 if os.getenv("QORE_TEST_VALGRIND_DIR") else 30)
    listener.bind(("127.0.0.1", 0))
    listener.listen()
    errors = []
    requests = []

    def serve():
        try:
            with listener.accept()[0] as connection:
                connection.settimeout(30)
                try:
                    with context.wrap_socket(connection, server_side=True) as tls:
                        request = bytearray()
                        while b"\r\n\r\n" not in request:
                            part = tls.recv(1024)
                            if not part or len(request) + len(part) > 16384:
                                raise AssertionError("incomplete or oversized HTTP request")
                            request.extend(part)
                        requests.append(bytes(request).split(b"\r\n", 1)[0])
                        tls.sendall(b"HTTP/1.0 200 OK\r\nContent-Length: 7\r\n\r\ntrusted")
                except ssl.SSLError as error:
                    # The client rejects an untrusted certificate or hostname before HTTP.
                    if error.reason not in ("TLSV1_ALERT_UNKNOWN_CA", "SSLV3_ALERT_BAD_CERTIFICATE"):
                        raise
        except BaseException as error:
            errors.append(error)

    thread = threading.Thread(target=serve, daemon=True)
    thread.start()  # Bound/listening before the client starts; no readiness polling.
    try:
        yield f"https://127.0.0.1:{listener.getsockname()[1]}/", requests
    finally:
        failed = sys.exc_info()[0] is not None
        try:
            listener.shutdown(socket.SHUT_RDWR)
        except OSError:
            pass  # A completed accept may have already released the listener.
        listener.close()
        thread.join(timeout=35)
        if thread.is_alive():
            raise AssertionError("TLS peer did not terminate")
        if errors and not failed:
            raise errors[0]


class TlsErrorOutputTest(unittest.TestCase):
    def exchange(self, trusted, matching_name):
        with tempfile.TemporaryDirectory(prefix="qore-tls-output-") as directory:
            directory = Path(directory)
            key = directory / "key.pem"
            certificate = directory / "certificate.pem"
            subject = "IP:127.0.0.1" if matching_name else "DNS:other.invalid"
            subprocess.run(["openssl", "req", "-x509", "-newkey", "rsa:2048", "-nodes",
                            "-keyout", str(key), "-out", str(certificate), "-days", "1",
                            "-subj", "/CN=localhost", "-addext", "subjectAltName=" + subject],
                           check=True, capture_output=True, timeout=30)
            context = ssl.SSLContext(ssl.PROTOCOL_TLS_SERVER)
            context.load_cert_chain(certificate, key)
            environment = dict(os.environ)
            # Isolate trust from the host and from the caller's OpenSSL settings.
            environment["SSL_CERT_DIR"] = str(directory)
            environment["SSL_CERT_FILE"] = str(certificate if trusted else directory / "absent.pem")
            command = ["qore", "-b", "--enable-debug", "--exec-mode=" + os.getenv("QORE_EXEC_MODE", "jit")]
            if log_directory := os.getenv("QORE_TEST_VALGRIND_DIR"):
                log = Path(log_directory) / (self._testMethodName + ".log")
                command = ["valgrind", "--error-exitcode=90", "--leak-check=full",
                           "--show-leak-kinds=definite,indirect,possible",
                           "--errors-for-leak-kinds=definite,indirect,possible",
                           "--log-file=" + str(log), *command]
            with peer(context) as (url, requests):
                process = subprocess.run([*command, str(ROOT / "tls-error-output.qr"), url],
                                         capture_output=True, text=True, timeout=180, env=environment)
                self.assertEqual(0, process.returncode, process.stdout + process.stderr)
                self.assertEqual("", process.stderr)
            if trusted and matching_name:
                self.assertEqual("BODY:trusted\n", process.stdout)
                self.assertEqual([b"GET / HTTP/1.1"], requests)
            else:
                self.assertTrue(process.stdout.startswith("ERROR:SOCKET-SSL-ERROR\n"), process.stdout)
                self.assertIn("certificate verify failed", process.stdout)
                self.assertEqual([], requests)

    def test_trusted_certificate(self):
        self.exchange(True, True)

    def test_untrusted_certificate(self):
        self.exchange(False, True)

    def test_wrong_host(self):
        self.exchange(True, False)


if __name__ == "__main__":
    unittest.main()
