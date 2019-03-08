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

import PyQt5
from PyQt5.QtCore import *
from PyQt5.QtGui import *
from PyQt5.QtWidgets import *

import pathlib
import json
import locale

import StringTable
import Common
from Common import *


class AboutDialog(QDialog):
    def __init__(self, parent=None):
        super().__init__(parent)
        self.setModal(True)
        self.setWindowTitle(StringTable.getString("STR_ABOUT_TITLE"))

        #Text
        self.__vbox = QVBoxLayout()
        self.setLayout(self.__vbox)

        self.__lblAbout = QLabel(
            StringTable.getString("STR_ABOUT") %
            (StringTable.getString("TITLE_MAIN_WINDOW"), str(VERSION)))
        self.__vbox.addWidget(self.__lblAbout)
        rect = self.__lblAbout.fontMetrics().boundingRect(
            self.__lblAbout.text())
        self.__lblAbout.setMinimumSize(rect.size())
