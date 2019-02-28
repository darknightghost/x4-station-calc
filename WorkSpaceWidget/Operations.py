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
from WorkSpaceWidget.SummaryItem import *
from WorkSpaceWidget.Operations import *


class Operation:
    '''
        Base class of station operations.
    '''

    def __init__(self):
        self._workSpace = None

    def setWorkspace(self, workspace):
        '''
            Set workspace widget.
        '''
        self._workSpace = workspace

    def do(self):
        '''
            Do operation, return True if success.
        '''
        if self._workSpace == None:
            return False

        else:
            return self.onDo() == True

    def onDo(self):
        '''
            Do operation, return True if success.
        '''
        raise NotImplementedError()

    def undo(self):
        '''
            Undo operation.
        '''
        if self._workSpace == None:
            return False

        else:
            return self.onUndo() == True

    def onUndo(self):
        '''
            Undo operation.
        '''
        raise NotImplementedError()


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

    def onDo(self):
        '''
            Do operation, return True if success.
        '''
        #Check status
        seleceted = self._workSpace.selectedItems()
        if len(seleceted) != 1:
            return False

        seleceted = seleceted[0]
        if not isinstance(seleceted, ModuleGroupItem):
            return False

        #Do operation
        self.__groupItem = seleceted
        self.__moduleItems = []
        for m in self.__modules:
            self.__moduleItems.append(self.__groupItem.addStationModule(m))

        return True

    def onUndo(self):
        '''
            Undo operation.
        '''
        if None in (self.__groupItem, self.__moduleItems):
            return False

        for m in self.__moduleItems:
            self.__groupItem.removeStationModule(m)
