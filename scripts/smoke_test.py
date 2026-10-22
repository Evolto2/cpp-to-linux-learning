"""Standard-library smoke checks; test data stays in temporary directories."""
import argparse
import hashlib
import json
import os
from pathlib import Path
import platform
import socket
import subprocess
import tempfile
import time

ROOT = Path(__file__).resolve().parents[1]
RESULTS = []
FLAGS = subprocess.CREATE_NO_WINDOW if os.name == 'nt' else 0

def check(name, action):
    try:
        detail = action() or 'OK'
        RESULTS.append({'name': name, 'passed': True, 'detail': detail})
        print(f'PASS {name}', flush=True)
    except Exception as exc:
        RESULTS.append({'name': name, 'passed': False, 'detail': str(exc)})
        print(f'FAIL {name}: {exc}', flush=True)

def require(condition, detail):
    if not condition:
        raise AssertionError(detail)

def verify_sources():
    count = 0
    for manifest in sorted((ROOT / 'projects').glob('*/SOURCE.json')):
        record = json.loads(manifest.read_text(encoding='utf-8'))
        upstream = manifest.parent / 'upstream'
        actual = {p.relative_to(upstream).as_posix() for p in upstream.rglob('*') if p.is_file()}
        require(actual == set(record['files_sha256']), f'Source file list differs: {manifest}')
        for relative, expected in record['files_sha256'].items():
            require(hashlib.sha256((upstream / relative).read_bytes()).hexdigest() == expected,
                    f'Source changed: {relative}')
            count += 1
    return f'{count} source files match recorded SHA-256 values'

def run(executable, text, cwd, args=()):
    result = subprocess.run([str(executable), *map(str, args)], input=text, text=True,
                            encoding='utf-8', errors='replace', capture_output=True,
                            cwd=cwd, timeout=10, creationflags=FLAGS)
    require(result.returncode == 0, f'exit={result.returncode}: {result.stderr}')
    return result.stdout + result.stderr

def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('--build-dir', required=True)
    parser.add_argument('--report', required=True)
    options = parser.parse_args()
    build = (ROOT / options.build_dir).resolve()
    suffix = '.exe' if os.name == 'nt' else ''
    exe = lambda name: build / (name + suffix)
    check('original-source-integrity', verify_sources)
    with tempfile.TemporaryDirectory(prefix='cpp-learning-tests-') as scratch:
        cwd = Path(scratch)
        cases = [('+', '3', '2', 'result: 5'), ('-', '3', '5', 'result: -2'),
                 ('*', '1.5', '4', 'result: 6'), ('/', '9', '2', 'result: 4.5'),
                 ('/', '9', '0', 'Division by zero'), ('?', '3', '2', "wasn't a valid operator")]
        for index, (operator, a, b, expected) in enumerate(cases, 1):
            def calculator_case(operator=operator, a=a, b=b, expected=expected):
                output = run(exe('calculator'), f'{operator}\n{a}\n{b}\n', cwd)
                require(expected in output, output)
            check(f'calculator-{index}', calculator_case)

        student = exe('student_manager')
        def student_case(text, expected):
            output = run(student, text, cwd)
            require(expected in output, output)

        check('student-add', lambda: student_case('1\n1001\nAlice Zhang\n22\nControl\n3.6\n6\n', 'record added successfully'))
        check('student-restart-persistence', lambda: student_case('2\n6\n', 'Alice Zhang'))
        check('student-search-id', lambda: student_case('3\n1\n1001\n6\n', 'Alice Zhang'))
        check('student-search-name', lambda: student_case('3\n2\nalice\n6\n', 'Matching Records Found'))
        check('student-duplicate-id', lambda: student_case('1\n1001\n6\n', 'already exists'))
        check('student-invalid-id', lambda: student_case('1\n-1\n6\n', 'Invalid roll number'))
        check('student-update', lambda: student_case('4\n1001\nAlice Updated\n23\nAutomation\n3.8\n6\n', 'record updated successfully'))
        check('student-updated-persistence', lambda: student_case('2\n6\n', 'Alice Updated'))
        check('student-cancel-delete', lambda: student_case('5\n1001\nn\n6\n', 'Deletion cancelled'))
        check('student-delete', lambda: student_case('5\n1001\ny\n6\n', 'record deleted successfully'))
        check('student-empty-after-restart', lambda: student_case('2\n6\n', 'No student records found'))

        with socket.socket() as reservation:
            reservation.bind(('127.0.0.1', 0))
            port = reservation.getsockname()[1]
        process = subprocess.Popen([str(exe('tcp_echo_server')), str(port)], cwd=cwd,
                                   stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL,
                                   creationflags=FLAGS)
        try:
            def ready():
                deadline = time.monotonic() + 10
                while time.monotonic() < deadline:
                    require(process.poll() is None, 'server exited before readiness')
                    try:
                        with socket.create_connection(('127.0.0.1', port), timeout=.3):
                            return 'server accepted a loopback connection'
                    except OSError:
                        time.sleep(.05)
                raise AssertionError('server readiness timeout')
            check('tcp-server-start', ready)
            def official_client():
                output = run(exe('tcp_echo_client'), 'hello-cpp-linux\n', cwd, ('127.0.0.1', port))
                require('Reply is: hello-cpp-linux' in output, output)
            check('tcp-official-client-echo', official_client)
            def raw_echo(payload):
                with socket.create_connection(('127.0.0.1', port), timeout=3) as client:
                    client.sendall(payload)
                    reply = b''
                    while len(reply) < len(payload):
                        chunk = client.recv(min(4096, len(payload)-len(reply)))
                        require(bool(chunk), 'server closed before full reply')
                        reply += chunk
                    require(reply == payload, 'echo payload mismatch')
            check('tcp-utf8-echo', lambda: raw_echo('你好，Linux'.encode('utf-8')))
            check('tcp-multiple-buffer-echo', lambda: raw_echo(bytes(range(256))*16))
            check('tcp-client-disconnect-and-reconnect', lambda: raw_echo(b'after-disconnect'))
        finally:
            process.terminate()
            try:
                process.wait(timeout=5)
            except subprocess.TimeoutExpired:
                process.kill()
                process.wait(timeout=5)

    report = {'platform': platform.platform(), 'python': platform.python_version(),
              'passed': sum(r['passed'] for r in RESULTS), 'total': len(RESULTS), 'results': RESULTS}
    report_path = ROOT / options.report
    report_path.parent.mkdir(parents=True, exist_ok=True)
    report_path.write_text(json.dumps(report, ensure_ascii=False, indent=2)+'\n', encoding='utf-8')
    print(f"{report['passed']}/{report['total']} passed", flush=True)
    return 0 if report['passed'] == report['total'] else 1

if __name__ == '__main__':
    raise SystemExit(main())
