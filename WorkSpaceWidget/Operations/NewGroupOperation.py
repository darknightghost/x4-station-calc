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

from WorkSpaceWidget.ModuleGroupItem import *
from WorkSpaceWidget.ModuleItem import *
from WorkSpaceWidget.Operations import *


class NewGroupOperation(Operation):
    '''
        Add new group.
    '''

    @TypeChecker(Operation)
    def __init__(self):
        super().__init__()
        self.__group = None

    def onDo(self):
        '''
            Do operation, return True if success.
        '''
        if self.__group == None:
            selected = self._workSpace.currentItem()
            if isinstance(selected, ModuleGroupItem):
                self.__index = selected.parent().indexOfChild(selected) + 1

            elif isinstance(selected, ModuleItem):
                self.__index = selected.parent().parent().indexOfChild(
                    selected.parent()) + 1

            else:
                self.__index = 0

            self.__group = self._workSpace.operationAddGroup(self.__index)

        else:
            self._workSpace.operationAddGroup(self.__index, self.__group)

        return True

    def onUndo(self):
        '''
            Undo operation.
        '''
        self._workSpace.operationRemoveGroup(self.__group)
        return True
