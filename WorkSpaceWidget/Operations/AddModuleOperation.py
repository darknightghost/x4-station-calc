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


class AddModuleOperation(Operation):
    '''
        Add station module.
    '''

    @TypeChecker(Operation, list)
    def __init__(self, modules):
        super().__init__()
        self.__modules = modules

        self.__groupItem = None
        self.__moduleItems = None

        for m in self.__modules:
            if not isinstance(m, StationModule.StationModule):
                raise TypeError(
                    "Type of module should be \"StationModule.StationModule\"."
                )

    def onSetWorkspace(self):
        '''
            Set workspace widget.
        '''
        #Check status
        seleceted = self._workSpace.selectedItems()
        if len(seleceted) != 1:
            return False

        seleceted = seleceted[0]
        if isinstance(seleceted, ModuleGroupItem):
            self.__groupItem = seleceted
            self.__moduleItems = None
            self.__index = 0

        elif isinstance(seleceted, ModuleItem):
            self.__groupItem = seleceted.parent()
            self.__moduleItems = None
            self.__index = self.__groupItem.indexOfChild(seleceted) + 1

        else:
            return False

        return True

    def onDo(self):
        '''
            Do operation, return True if success.
        '''
        if self.__moduleItems == None:
            self.__moduleItems = []
            i = 0
            for m in self.__modules:
                self.__moduleItems.append(
                    self.__groupItem.addStationModule(m, self.__index + i))
                i += 1

            return True

        else:
            i = 0
            for m in self.__moduleItems:
                self.__groupItem.addStationModule(m, self.__index + i)
                i += 1

    def onUndo(self):
        '''
            Undo operation.
        '''
        if None in (self.__groupItem, self.__moduleItems):
            return False

        for m in self.__moduleItems:
            self.__groupItem.removeStationModule(m)
