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

import Common
from Common import *
import StringTable

import WorkSpaceWidget
from WorkSpaceWidget.ModuleGroupItem import *


class ModulesItem(QTreeWidgetItem):
    updateData = pyqtSignal()

    @TypeChecker(QTreeWidgetItem, QTreeWidget)
    def __init__(self, parent):
        super().__init__(parent)
        self.setText(0, StringTable.getString("STR_STATION_MODULES"))
        self.setFlags(Qt.ItemIsEnabled)
        self.__loadData(parent)
        self.setExpanded(True)

    def __loadData(self, parent):
        for g in parent.station():
            self.addChild(ModuleGroupItem(g, self))
