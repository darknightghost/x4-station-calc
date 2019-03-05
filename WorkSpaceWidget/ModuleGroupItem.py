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

import Station
import StationModule
import WorkSpaceWidget

from WorkSpaceWidget.ModuleItem import *


class ModuleGroupItem(QTreeWidgetItem):
    '''
        Station module group.
    '''
    updateData = pyqtSignal()

    @TypeChecker(QTreeWidgetItem, Station.StationModulesGroup,
                 (QTreeWidgetItem, type(None)))
    def __init__(self, item, parent):
        super().__init__(parent)
        self.__item = item
        self.setText(0, item.name())
        self.setFlags(Qt.ItemIsEnabled | Qt.ItemIsSelectable
                      | Qt.ItemIsEditable)
        self.__index = {}

        for m in item:
            item = ModuleItem(m, self)
            self.__index[m.id()] = item
            item.onAdd()

        self.setExpanded(True)

    @TypeChecker(QTreeWidgetItem, (StationModule.StationModule, ModuleItem),
                 int)
    def addStationModule(self, m, index=-1):
        '''
            Add station module.
        '''
        if index < 0:
            index = self.childCount() + 1 + index

        if isinstance(m, StationModule.StationModule):
            m = Station.StationModules(m.id())

            if m in self.__item:
                self.__item.insert(index, m)
                self.treeWidget().setCurrentItem(self.__index[m.id()])
                return self.__index[m.id()]

            else:
                self.__item.insert(index, m)
                item = ModuleItem(m, None)
                self.__index[m.id()] = item
                self.insertChild(index, item)
                item.onAdd()
                self.treeWidget().setCurrentItem(item)
                return self.__index[m.id()]

        else:
            if m.item() in self.__item:
                m.item().increase()

            else:
                self.__item.insert(index, m.item())
                self.__index[m.item().id()] = m
                self.insertChild(index, m)
                m.onAdd()
                self.treeWidget().setCurrentItem(m)
            return m

    @TypeChecker(QTreeWidgetItem, ModuleItem)
    def removeStationModule(self, m):
        '''
            Remove station module.
        '''
        if m.item().amount() > 1:
            m.item().decrease()

        else:
            item = m.item()
            del self.__index[m.item().id()]
            self.removeChild(m)
            self.__item.remove(item)

    @TypeChecker(QTreeWidgetItem, int, int)
    def swapStationModule(self, index1, index2):
        '''
            Swap
        '''
        if index1 < index2:
            index1, index2 = index2, index1

        self.item().swap(index1, index2)

        item2 = self.takeChild(index2)
        self.insertChild(index1, item2)
        item1 = self.takeChild(index1 - 1)
        self.insertChild(index2, item1)
        item1.onAdd()
        item2.onAdd()

    def item(self):
        '''
            Get item.
        '''
        return self.__item

    def setName(self, name):
        '''
            Set group name.
        '''
        self.__item.setName(name)
        self.setText(0, name)

    @TypeChecker(QTreeWidgetItem, QTreeWidgetItem, int)
    def onChanged(self, item, column):
        if self.text(0) != self.__item.name():
            op = ChangeGroupNameOperation(self, self.__item.name(),
                                          self.text(0))
            self.treeWidget().doOperation(op)


from WorkSpaceWidget.Operations import *
