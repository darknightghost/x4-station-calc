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

from WorkSpaceWidget.ModulesItem import *
from WorkSpaceWidget.ModuleItem import *
from WorkSpaceWidget.ModuleGroupItem import *
from WorkSpaceWidget.SummarysItem import *
from WorkSpaceWidget.Operations import *


class RemoveOperation(Operation):
    '''
        Add station module.
    '''

    @TypeChecker(Operation, (list, type(None)))
    def __init__(self, items=None):
        super().__init__()
        self.__items = items

    def onSetWorkspace(self):
        '''
            Set workspace widget.
        '''
        #Get items
        if self.__items == None:
            self.__items = self._workSpace.selectedItems()

        if len(self.__items) == 0:
            return False

        self.__groups = []
        self.__modules = []
        for item in self.__items:
            if isinstance(item, ModuleGroupItem):
                self.__groups.append((item, item.parent().indexOfChild(item)))

            elif isinstance(item, ModuleItem):
                self.__modules.append((item, item.parent().indexOfChild(item),
                                       item.parent()))

        #Sort
        self.__groups.sort(key=lambda item: item[1])
        self.__modules.sort(key=lambda item: item[1])

        return True

    def onDo(self):
        '''
            Do operation, return True if success.
        '''
        #Remove modules
        for m in self.__modules:
            m[0].parent().removeStationModule(m[0], removeAll=True)

        #Remove groups
        for g in self.__groups:
            self._workSpace.operationRemoveGroup(g[0])

        return True

    def onUndo(self):
        '''
            Undo operation.
        '''
        #Add groups
        for g in self.__groups:
            self._workSpace.operationAddGroup(index=g[1], g=g[0])

        #Add modules
        for m in self.__modules:
            m[2].addStationModule(m[0], m[1])

        return True
