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

import logging
import pathlib
import time


class LogHandler(logging.StreamHandler):
    def __init__(self, maxRecord=8192, stream=None):
        super().__init__(stream)
        self.__maxRecord = 8192
        self.__buffer = []

    def writeFile(self, path):
        with open(path, "w") as f:
            for l in self.__buffer:
                f.write(l)
                f.write("\n")

        print("Log has been written to \"%s\"." % (path))

    def emit(self, record):
        self.__buffer.append(self.format(record))
        if len(self.__buffer) > self.__maxRecord:
            self.__buffer.pop(0)
        super().emit(record)


__HANDLER = LogHandler()
__HANDLER.setFormatter(
    logging.Formatter('%(asctime)s %(levelname)s : %(message)s'))


def writeLog():
    logPath = pathlib.Path(__file__).parent / ("err.log")
    __HANDLER.writeFile(str(logPath))
    return


def __initLogger():
    logger = logging.getLogger()
    logger.setLevel(logging.DEBUG)
    logger.addHandler(__HANDLER)


__initLogger()
