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
        return self.onSetWorkspace()

    def onSetWorkspace(self):
        '''
            Set workspace widget.
        '''
        return True

    def do(self):
        '''
            Do operation, return True if success.
        '''
        if self._workSpace == None:
            return False

        else:
            if self.onDo():
                return True

            else:
                return False

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
            if self.onUndo():
                return True

            else:
                return False

    def onUndo(self):
        '''
            Undo operation.
        '''
        raise NotImplementedError()


class ChangeGroupNameOperation(Operation):
    '''
        Change group name.
    '''

    @TypeChecker(Operation, ModuleGroupItem, str, str)
    def __init__(self, groupItem, oldName, newName):
        self.__groupItem = groupItem
        self.__oldName = oldName
        self.__newName = newName

    def onDo(self):
        '''
            Do operation, return True if success.
        '''
        self.__groupItem.setName(self.__newName)
        return True

    def onUndo(self):
        '''
            Undo operation.
        '''
        self.__groupItem.setName(self.__oldName)
        return True


class NewGroupOperation(Operation):
    '''
        Add new group.
    '''

    @TypeChecker(Operation)
    def __init__(self):
        self.__group = None

    def onDo(self):
        '''
            Do operation, return True if success.
        '''
        if self.__group == None:
            self.__group = self._workSpace.operationAddGroup()

        else:
            self._workSpace.operationAddGroup(self.__group)

        return True

    def onUndo(self):
        '''
            Undo operation.
        '''
        self._workSpace.operationRemoveGroup(self.__group)
        return True


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
        if not isinstance(seleceted, ModuleGroupItem):
            return False

        #Do operation
        self.__groupItem = seleceted
        self.__moduleItems = None

        return True

    def onDo(self):
        '''
            Do operation, return True if success.
        '''
        if self.__moduleItems == None:
            self.__moduleItems = []
            for m in self.__modules:
                self.__moduleItems.append(self.__groupItem.addStationModule(m))

            return True

        else:
            for m in self.__moduleItems:
                self.__groupItem.addStationModule(m)

    def onUndo(self):
        '''
            Undo operation.
        '''
        if None in (self.__groupItem, self.__moduleItems):
            return False

        for m in self.__moduleItems:
            self.__groupItem.removeStationModule(m)
