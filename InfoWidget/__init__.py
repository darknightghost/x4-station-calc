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

import StringTable
import Common
from Common import *

import DockWidget

import InfoWidget
from InfoWidget.InfoTreeWidget import *


class InfoWidget(DockWidget.QDockWidgetAttachAction):
    def __init__(self, parent=None):
        super().__init__(QWidget(), parent)
        self.setWidget(QWidget(self))
        self.__layout = QGridLayout(self.widget())
        self.widget().setLayout(self.__layout)

        #Button
        self.__btnPrev = QSquareButton("←")
        self.__layout.addWidget(self.__btnPrev, 0, 0)

        #Space
        self.__layout.setColumnStretch(1, 0)

        #Tree widget
        self.__treeView = InfoTreeWidget(self)
        self.__layout.addWidget(self.__treeView, 1, 0, 2, 0)
        self.setWindowTitle(StringTable.getString("TITLE_INFO_WIDGET"))

        self.__history = []
        self.__treeView.nextItem.connect(self.onNextData)
        self.__btnPrev.clicked.connect(self.prevData)
        self.__btnPrev.setEnabled(False)

    @TypeChecker(DockWidget.QDockWidgetAttachAction, list)
    def setData(self, data):
        '''
            Update to new data and clean history.
        '''
        self.__history = [data]
        self.__btnPrev.setEnabled(False)
        self.__updateData(data)

    @TypeChecker(DockWidget.QDockWidgetAttachAction, list)
    def onNextData(self, data):
        '''
            Update to next data.
        '''
        self.__history.append(data)
        self.__updateData(data)
        self.__btnPrev.setEnabled(True)

    def prevData(self):
        '''
            Update to prev data.
        '''
        if len(self.__history) > 1:
            self.__history.pop()
            self.__updateData(self.__history[-1])

        if len(self.__history) <= 1:
            self.__btnPrev.setEnabled(False)

    @TypeChecker(DockWidget.QDockWidgetAttachAction, list)
    def __updateData(self, data):
        '''
            Update data.
        '''
        self.__treeView.update(data)
