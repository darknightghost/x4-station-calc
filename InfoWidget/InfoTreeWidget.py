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


class InfoItem:
    '''
        Infomation name value pair to show.
    '''

    @TypeChecker(object, str, (str, list), (object, type(None)))
    def __init__(self, name, value, onDblClick=None):
        '''
            InfoItem(name, value, onDblClick)   -> infoItem
            InfoItem(name, value)               -> infoItem

        '''
        if onDblClick != None and not callable(onDblClick):
            raise TypeError("\"onDblClick\" must be callable.")

        self.__name = name
        self.__value = value
        self.__onDblClick = onDblClick

    def name(self):
        '''
            Get name.
        '''
        return self.__name

    def value(self):
        '''
            Get value.
        '''
        return self.__value

    def canBeDblClick(self):
        '''
            Can be double clicked or not.
        '''
        return self.__onDblClick != None

    def __call__(self):
        if self.__onDblClick == None:
            return None

        else:
            return self.__onDblClick()


class InfoTreeWidgetItem(QTreeWidgetItem):
    @TypeChecker(QTreeWidgetItem, InfoItem, (QTreeWidget, QTreeWidgetItem))
    def __init__(self, item, parent):
        if isinstance(item.value(), list):
            super().__init__(parent, [item.name()])
            #Load children
            for c in item.value():
                InfoTreeWidgetItem(c, self)
            self.setExpanded(True)

        else:
            super().__init__(parent, [item.name(), item.value()])

        flags = Qt.ItemIsEnabled
        if item.canBeDblClick():
            flags |= Qt.ItemIsSelectable

        self.setFlags(flags)

        self.__item = item

    def __call__(self):
        return self.__item()


class InfoTreeWidget(QTreeWidget):
    '''
        Information view.
    '''
    nextItem = pyqtSignal(list)

    @TypeChecker(QTreeWidget, QDockWidget)
    def __init__(self, parent):
        #Init
        super().__init__(parent)
        self.addChild = self.addTopLevelItem
        self.setHeaderHidden(True)
        self.setColumnCount(2)
        self.header().setSectionResizeMode(QHeaderView.ResizeToContents)
        self.itemDoubleClicked.connect(self.__onItemDoubleClicked)

    @TypeChecker(QTreeWidget, list)
    def update(self, data):
        '''
            Update data.
        '''
        self.clear()
        for i in data:
            InfoTreeWidgetItem(i, self)

    @TypeChecker(QTreeWidget, InfoTreeWidgetItem, int)
    def __onItemDoubleClicked(self, item, column):
        ret = item()
        if ret != None:
            self.nextItem.emit(ret)
