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

import StationModule
import StringTable
import Common
from Common import *


class QTreeWidgetStationModuleItem(QTreeWidgetItem):
    '''
        Item with station module.
    '''

    @TypeChecker(QTreeWidgetItem, StationModule.StationModule, QTreeWidgetItem)
    def __init__(self, stationModule, parent):
        super().__init__(parent)
        self.__stationModule = stationModule
        self.setText(0, self.__stationModule.name())
        self.setFlags(Qt.ItemIsEnabled | Qt.ItemIsSelectable)

    def stationModule(self):
        '''
            Get station module.
        '''
        return self.__stationModule


class ModuleTreeWidget(QTreeWidget):
    '''
        List of station modules.
    '''

    @TypeChecker(QTreeWidget, object, QWidget)
    def __init__(self, filter, parent):
        '''
            ModuleTreeWidget(filter_function, parent)   -> widget
            
            @TypeChecker(StationModule.StationModule)
            def filter_function(m):
                return True
        '''
        #INitialize widget
        super().__init__(parent)
        if not callable(filter):
            raise TypeError("\"filter\" must be callable.")

        self.__filter = filter
        self.header().setVisible(False)
        self.setSelectionMode(QAbstractItemView.ExtendedSelection)

        #Load data
        self.__loadStationModules()

    def __loadStationModules(self):
        d = {}
        for t in StationModule.stationTypes():
            #Category
            categoryNode = QTreeWidgetItem(self)
            categoryNode.setText(0, StationModule.typeName(t))
            categoryNode.setFlags(Qt.ItemIsEnabled)
            self.addTopLevelItem(categoryNode)
            d[t] = categoryNode

        #Station modules
        for m in StationModule.stationModules():
            QTreeWidgetStationModuleItem(m, d[m.type()])
