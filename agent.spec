# -*- mode: python ; coding: utf-8 -*-

import os

project_path = os.path.abspath(".")
script_path = os.path.join(project_path, "agent.py")

a = Analysis(
    [script_path],
    pathex=[project_path],
    binaries=[],
    datas=[],
    hiddenimports=['pydantic.deprecated.decorator'],
    hookspath=[],
    hooksconfig={},
    runtime_hooks=[],
    excludes=['pysqlite2', 'MySQLdb'],
    noarchive=False,
)

pyz = PYZ(a.pure, a.zipped_data,
          cipher=None)

exe = EXE(
    pyz,
    a.scripts,
    [],
    exclude_binaries=True,
    name='agent',
    debug=False,
    bootloader_ignore_signals=False,
    strip=False,
    upx=True,
    console=True,
    disable_windowed_traceback=False,
    target_arch=None,
    codesign_identity=None,
    entitlements_file=None,
)

app = BUNDLE(
    exe,
    name='agent',
    icon=None
)

# Enable OneFile here
onefile = EXE(
    pyz,
    a.scripts,
    a.binaries,
    a.zipfiles,
    a.datas,
    [],
    name='agent',
    debug=False,
    bootloader_ignore_signals=False,
    strip=False,
    upx=True,
    console=True,
    disable_windowed_traceback=False,
    target_arch=None,
    codesign_identity=None,
    entitlements_file=None,
    singlefile=True  # <== This makes it one .exe file!
)
