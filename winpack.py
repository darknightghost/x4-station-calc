#! /usr/bin/env python3
# -*- coding: utf-8 -*-
'''
      Copyright 2019,王思远 <darknightghost.cn@gmail.com>
      This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.
    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.
      You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
'''
'''
    Pack windows package.
'''

import pathlib
import re
import os
import shutil

SCRIPT_DIR = pathlib.Path(__file__).parent
MAIN_FILE = SCRIPT_DIR / "main.pyw"


def scanDir(beginDir):
    ret = []
    exp = re.compile(".*\\.py|.*\\.pyw")
    for f in beginDir.glob("*"):
        if f.is_dir():
            ret += scanDir(f)

        elif exp.fullmatch(f.name) and f.absolute() != MAIN_FILE.absolute():
            ret.append(f.relative_to(SCRIPT_DIR))

    return ret


def main():
    os.chdir(SCRIPT_DIR)

    distpath = pathlib.Path(__file__).parent / "pack" / "dist"
    try:
        shutil.rmtree(distpath)

    except FileNotFoundError:
        pass

    workpath = pathlib.Path(__file__).parent / "pack" / "build"

    try:
        shutil.rmtree(workpath)

    except FileNotFoundError:
        pass

    #Scan ".py"
    fileList = scanDir(SCRIPT_DIR)

    #Make command
    cmd = "pyinstaller.exe -w --distpath \"%s\" --workpath \"%s\"" % (
        str(distpath), str(workpath))

    for f in fileList:
        cmd += " -p \"%s\"" % (str(f))

    cmd += " \"%s\"" % (str(MAIN_FILE.relative_to(SCRIPT_DIR)))

    print(cmd)

    #Pack
    ret = os.system(cmd)

    if ret != 0:
        return ret

    #Copy data
    shutil.copytree(
        str(SCRIPT_DIR / "factions"), str(distpath / "main" / "factions"))
    shutil.copytree(
        str(SCRIPT_DIR / "products"), str(distpath / "main" / "products"))
    shutil.copytree(
        str(SCRIPT_DIR / "station_modules"),
        str(distpath / "main" / "station_modules"))
    shutil.copyfile(
        str(SCRIPT_DIR / "LICENSE"), str(distpath / "main" / "LICENSE"))
    shutil.copyfile(
        str(SCRIPT_DIR / "README.md"), str(distpath / "main" / "README.md"))

    print("The output directory is \"%s\"." % str(distpath / "main"))
    return 0


if __name__ == "__main__":
    ret = main()
    exit(ret)
