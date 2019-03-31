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
    UI strings.
'''

__VERSION_URL = "https://raw.githubusercontent.com/darknightghost/x4-station-calc/master/Common/__init__.py"
__MAIN_URL = "https://github.com/darknightghost/x4-station-calc/"
__WIKI_URL = "https://github.com/darknightghost/x4-station-calc/wiki"

import PyQt5
from PyQt5.QtCore import *
from PyQt5.QtGui import *
from PyQt5.QtWidgets import *

import urllib
import urllib.request

from distutils.version import StrictVersion

import sys
import re
import os

import logging

import Common
from Common import *

import StringTable

logger = logging.getLogger()


def __url():
    '''
        Get download page url.
    '''
    if os.path.exists(os.path.abspath(__file__)):
        return __MAIN_URL

    else:
        return __WIKI_URL


@TypeChecker((QMainWindow, type(None)))
def checkUpdate(parent):
    '''
        Check update.
    '''
    #Download url
    logger.info("Getting url \"%s\"." % (__VERSION_URL))
    try:
        request = urllib.request.urlopen(__VERSION_URL, timeout=10)
        data = request.read().decode(encoding="utf-8")

    except Exception as e:
        logger.error(str(e))
        QMessageBox.critical(
            parent, StringTable.getString("TITLE_MSG_ERROR"),
            StringTable.getString("STR_CHECKUPDATE_NETWORK_ERROR"))
        return

    #Check version
    try:
        exp = "^VERSION\\s*=\\s*StrictVersion\\(\"\\d+\\.\\d+\\.\\d+\"\\)"
        match = re.search(exp, data, re.MULTILINE)
        data = str(match.group(0))
        exp = "\\d+\\.\\d+\\.\\d+"
        match = re.search(exp, data)
        latestVersion = StrictVersion(str(match.group(0)))
    except AttributeError:
        QMessageBox.critical(
            parent, StringTable.getString("TITLE_MSG_ERROR"),
            StringTable.getString("STR_CHECKUPDATE_NETWORK_ERROR"))
        return

    #Show message
    logger.info("Current version is %s, the latest version is %s." %
                (str(VERSION), str(latestVersion)))
    if VERSION < latestVersion:
        if QMessageBox.information(
                parent,
                StringTable.getString("TITLE_CHECKUPDATE"),
                StringTable.getString("STR_HAS_NEW_VERSION"),
                buttons=QMessageBox.Yes | QMessageBox.No,
                defaultButton=QMessageBox.Yes) == QMessageBox.Yes:
            QDesktopServices.openUrl(QUrl(__url()))

    else:
        QMessageBox.information(
            parent, StringTable.getString("TITLE_CHECKUPDATE"),
            StringTable.getString("STR_HAS_NOT_NEW_VERSION"))

    return


if __name__ == '__main__':
    '''
        Test.
    '''
    testSetLocale()
    app = QApplication(sys.argv)
    checkUpdate(None)
