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
import Station
import Common
from Common import *


class LicenseDialog(QDialog):
    LICENSE_PATH = pathlib.Path(__file__).parent / "LICENSE"

    def __init__(self, parent=None):
        super().__init__(parent)
        self.setModal(True)
        self.setWindowTitle("License")

        #Text
        self.__vbox = QVBoxLayout()
        self.setLayout(self.__vbox)

        self.__txtLicense = QTextEdit()
        self.__txtLicense.setReadOnly(True)
        with open(str(self.LICENSE_PATH)) as f:
            self.__txtLicense.setPlainText(f.read())
        self.__vbox.addWidget(self.__txtLicense)
        rect = self.__txtLicense.fontMetrics().boundingRect("a" * 71)
        self.__txtLicense.setMinimumWidth(rect.width())
        self.__txtLicense.setMinimumHeight(rect.height() * 24)

        #Buttons
        self.__btnAgree = QPushButton("I &Agree")
        self.__btnDisagree = QPushButton("I &Disagree")
        self.__hbox = QHBoxLayout()
        self.__vbox.addLayout(self.__hbox)
        self.__hbox.addStretch()
        self.__hbox.addWidget(self.__btnAgree)
        self.__btnAgree.clicked.connect(self.accept)
        self.__hbox.addStretch()
        self.__hbox.addWidget(self.__btnDisagree)
        self.__btnDisagree.clicked.connect(self.reject)
        self.__hbox.addStretch()
