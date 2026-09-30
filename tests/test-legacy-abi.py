#!/usr/bin/env python3
# SPDX-FileCopyrightText: 2026 Jolla Mobile Ltd
# SPDX-License-Identifier: MPL-2.0
"""Run a frozen v1-header client with current WebView/QtMoz context and settings.

Usage: test-legacy-abi.py /path/to/qtmozembed [build-directory]
Requires host Qt5 development tools, a C++ compiler, and the pinned Git history.
The v1 DSOs are link-only symbol fixtures, never executed. The replacement DSOs
compile the real context/settings and WebView implementations; only Gecko,
window draining and Silica are controlled test backends. This checks ELF loading,
old symbol imports, inherited calls, shared state and Qt signals, not rendering
or target-device compatibility. The production qmake .so.1 target is built and
staged to check that it cannot overwrite the unversioned v2 linker name.
"""
from pathlib import Path
import hashlib
import os
import re
import shlex
import subprocess
import sys
import tempfile

root = Path(__file__).resolve().parents[1]
qtmoz = Path(sys.argv[1]).resolve()
build = Path(sys.argv[2]).resolve() if len(sys.argv) > 2 else Path(tempfile.mkdtemp(prefix='webview-abi-'))
build.mkdir(parents=True, exist_ok=True)
fixture = root / 'tests/abi'
qt_flags = shlex.split(subprocess.check_output(['pkg-config', '--cflags', 'Qt5Core', 'Qt5Gui', 'Qt5Qml'], text=True))
qt_libs = shlex.split(subprocess.check_output(['pkg-config', '--libs', 'Qt5Core', 'Qt5Gui', 'Qt5Qml'], text=True))
base = ['g++', '-std=c++11', '-fPIC', '-g', *qt_flags]

def run(args, **kwargs):
    subprocess.run([str(arg) for arg in args], check=True, **kwargs)

def capture(args):
    return subprocess.check_output([str(arg) for arg in args], text=True)

def write(path, text):
    path.parent.mkdir(parents=True, exist_ok=True)
    path.write_text(text)

old = build / 'old'
old.mkdir(exist_ok=True)
# Immutable ESR115 references, independent of the current branch or remote HEAD.
for repo, revision, names in [
    (root, 'd54d7d72b0215188bf828bea3416b14eb04c9a44', ['lib/webengine.h', 'lib/webenginesettings.h']),
    (qtmoz, 'b9d613bc241f40fb37e5288389294e87162d26cd', ['src/qmozcontext.h', 'src/qmozenginesettings.h'])
]:
    for name in names:
        write(old / Path(name).name, capture(['git', '-C', repo, 'show', revision + ':' + name]))

client_obj = build / 'legacy-client.o'
run([*base, '-I' + str(old), '-c', fixture / 'legacy-client.cpp', '-o', client_obj])
undefined = [line.split()[-1] for line in capture(['nm', '-u', client_obj]).splitlines()]
# Link against only the exact old-header imports. No new headers or symbols are
# visible when compiling or linking this executable. Stub functions must not run.
for name, marker in [('libqt5embedwidget.so.1', 'QMoz'), ('libsailfishwebengine.so.1', 'SailfishOS')]:
    symbols = [symbol for symbol in undefined if marker in symbol]
    assert symbols, name
    declarations = []
    for symbol in symbols:
        assert re.fullmatch(r'[A-Za-z_][A-Za-z_0-9]*', symbol)
        if symbol.endswith('staticMetaObjectE'):
            declarations.append('char ' + symbol + '[256] = {};')
        else:
            declarations.append('void ' + symbol + '() { __builtin_trap(); }')
    source = old / (name + '.cpp')
    write(source, 'extern "C" {\n' + '\n'.join(declarations) + '\n}\n')
    run([*base, '-shared', source, '-Wl,-soname,' + name, '-o', old / name])
client = build / 'legacy-client'
run([*base, client_obj, '-L' + str(old), '-Wl,--no-as-needed',
     '-l:libsailfishwebengine.so.1', '-l:libqt5embedwidget.so.1', *qt_libs, '-ldl', '-o', client])
needed = capture(['readelf', '-d', client])
assert 'libqt5embedwidget.so.1' in needed and 'libsailfishwebengine.so.1' in needed
frozen_hash = hashlib.sha256(client.read_bytes()).hexdigest()

new = build / 'src'
new.mkdir(exist_ok=True)
stubs = build / 'stubs'
write(stubs / 'mozilla/embedlite/EmbedLiteApp.h', '#include "backend.h"\n')
includes = ['-I' + str(stubs), '-I' + str(fixture), '-I' + str(qtmoz / 'src'), '-I' + str(root / 'lib')]

def moc(header):
    output = new / ('moc_' + header.stem + '.cpp')
    run(['moc', *includes, *qt_flags, header, '-o', output])
    return output

context_mocs = [moc(qtmoz / 'src' / name) for name in
                ['qmozcontext.h', 'qmozcontext_p.h', 'qmozenginesettings.h', 'qmozenginesettings_p.h']]
run([*base, *includes, '-DBUILD_GRE_HOME="/test/gecko"', '-shared',
     qtmoz / 'src/qmozcontext.cpp', qtmoz / 'src/qmozenginesettings.cpp',
     fixture / 'backend.cpp', *context_mocs, *qt_libs, '-ldl',
     '-Wl,-z,defs', '-Wl,-soname,libqt5embedwidget.so.2', '-o', new / 'libqt5embedwidget.so.2'])
webview_mocs = [moc(root / 'lib' / name) for name in
               ['webengine.h', 'webenginesettings.h', 'webenginesettings_p.h']]
write(new / 'layout.cpp', '''#include "webengine.h"
#include "webenginesettings.h"
extern "C" size_t abiEngineSize() { return sizeof(SailfishOS::WebEngine); }
extern "C" size_t abiSettingsSize() { return sizeof(SailfishOS::WebEngineSettings); }
''')
run([*base, *includes, '-DSAILFISHOS_WEBVIEW_MOZILLA_COMPONENTS_PATH="/test/gecko"', '-shared',
     root / 'lib/webengine.cpp', root / 'lib/webenginesettings.cpp', root / 'lib/logging.cpp',
     *webview_mocs, new / 'layout.cpp', moc(fixture / 'silicatheme.h'), '-L' + str(new),
     '-l:libqt5embedwidget.so.2', *qt_libs, '-Wl,-z,defs',
     '-Wl,-soname,libsailfishwebengine.so.1', '-o', new / 'libsailfishwebengine.so.1'])
compat = build / 'compat'
compat.mkdir(exist_ok=True)
run(['qmake', qtmoz / 'compat/compat.pro', 'VERSION=2.0.0'], cwd=compat)
run(['make', '-j2'], cwd=compat)
run(['make', 'install', 'INSTALL_ROOT=' + str(build / 'stage')], cwd=compat)
soname = capture(['readelf', '-d', compat / 'libqt5embedwidget.so.1'])
assert re.search(r'\(SONAME\).*\[libqt5embedwidget.so.1\]', soname)
assert re.search(r'\(NEEDED\).*\[libqt5embedwidget.so.2\]', soname)
staged = list((build / 'stage').rglob('libqt5embedwidget.so*'))
assert staged and not any(path.name == 'libqt5embedwidget.so' for path in staged)
installed = next(path.parent for path in staged if path.name == 'libqt5embedwidget.so.1')
# Run the identical executable twice, swapping only its library search path.
env = dict(os.environ, LD_LIBRARY_PATH=str(new) + ':' + str(installed),
           LD_BIND_NOW='1', QT_QPA_PLATFORM='offscreen', DISABLE_PLAT_EGL_FIX='1')
for arguments in [[], ['deferred']]:
    run([client, *arguments], env=env)
assert hashlib.sha256(client.read_bytes()).hexdigest() == frozen_hash
print('PASS: unchanged v1-header ELF client, supported calls/signals/shared state, deferred startup, .so.1 packaging')
print('Client SHA256:', frozen_hash)
print('Artifacts:', build)
