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


class ButtonBarWidget(QWidget):
    #Signals
    changeFilterVisible = pyqtSignal(bool)
    addToStation = pyqtSignal()

    def __init__(self, parent):
        super().__init__(parent)
        self.__layout = QHBoxLayout(self)
        self.setLayout(self.__layout)
        margins = self.__layout.contentsMargins()
        margins.setLeft(0)
        margins.setRight(0)
        self.__layout.setContentsMargins(margins)

        #Show/Hide filter
        self.__btnShowFilter = QPushButton(StringTable.getString("BTN_SHOW_FILTER"),self)
        self.__btnShowFilter.clicked.connect(self.__onShowHideFilterClicked)
        self.__layout.addWidget(self.__btnShowFilter)
        self.__showFilter = False

        #Space
        self.__layout.addStretch()

        #Add to station
        self.__btnAddToStation = QPushButton(StringTable.getString("BTN_ADD_TO_STATION"),self)
        self.__btnAddToStation.clicked.connect(self.__onAddToStationClicked)
        self.__layout.addWidget(self.__btnAddToStation)

    def __onAddToStationClicked(self):
        self.addToStation.emit()


    def __onShowHideFilterClicked(self):
        self.__showFilter = not self.__showFilter
        self.changeFilterVisible.emit(self.__showFilter)
        self.__btnShowFilter.setText(
                StringTable.getString("BTN_HIDE_FILTER") \
                        if self.__showFilter \
                        else StringTable.getString("BTN_SHOW_FILTER"))
