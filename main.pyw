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

import platform
import time
import sys
import traceback
import logging

if int(platform.python_version().split(".")[0]) != 3:
    print("Python3 is required!")
    exit(-1)

import sys

try:
    import PyQt5
    from PyQt5.QtCore import *
    from PyQt5.QtGui import *
    from PyQt5.QtWidgets import *
except ImportError:
    print("PyQt5 is required!")
    exit(-1)

import InitLogger

import Common
from Common import *

logger = logging.getLogger()


def main():
    #App
    app = QApplication(sys.argv)
    QApplication.setApplicationName("X4 Station Caculator")
    QApplication.setApplicationVersion(str(VERSION))
    QApplication.setStyle(QStyleFactory.create("fusion"))

    #Arguments
    parser = QCommandLineParser()
    parser.setApplicationDescription("X4:Foundations station caculator.")
    parser.addHelpOption()
    parser.addVersionOption()
    parser.addPositionalArgument("station-file", "Station file to open.")
    parser.process(app)

    pargs = parser.positionalArguments()

    if len(pargs) == 1:
        stationPath = pargs[0]

    else:

        stationPath = None

    #Main window

    import MainWindow
    MainWindow = MainWindow.MainWindow
    w = MainWindow(None, stationPath)
    w.show()

    return app.exec_()


if __name__ == '__main__':
    try:
        sys.exit(main())

    except Exception as e:
        logger.error(traceback.format_exc())
        InitLogger.writeLog()
        raise e
