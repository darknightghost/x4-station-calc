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

import json

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
from WorkSpaceWidget.ModulesItem import *
from WorkSpaceWidget.Operations import *


class PasteOperation(Operation):
    '''
        Add new group.
    '''

    def onSetWorkspace(self):
        '''
            On set workspace widget.
        '''
        clipboard = QApplication.clipboard()
        mimeData = clipboard.mimeData()

        if mimeData.hasFormat(Station.StationModulesGroupsMimeData.mimeType()):
            self.__pasteGroup = True

            #Get index
            if isinstance(self._workSpace.currentItem(), ModuleGroupItem):
                self.__index = self._workSpace.currentItem().parent(
                ).indexOfChild(self._workSpace.currentItem()) + 1

            elif isinstance(self._workSpace.currentItem(), ModuleItem):
                self.__index = self._workSpace.currentItem().parent().parent(
                ).indexOfChild(self._workSpace.currentItem().parent()) + 1

            elif isinstance(self._workSpace.currentItem(),
                            WorkSpaceWidget.ModulesItem):
                self.__index = 0

            else:
                return False

            #Get items
            s = bytes(
                mimeData.data(
                    Station.StationModulesGroupsMimeData.mimeType())).decode(
                        encoding="utf-8")
            dicts = json.loads(s, encoding="utf-8")
            self.__groups = []
            for d in dicts:
                self.__groups.append(Station.StationModulesGroup(d))

        elif mimeData.hasFormat(Station.StationModulesMimeData.mimeType()):
            self.__pasteGroup = False
            #Index and group
            if isinstance(self._workSpace.currentItem(), ModuleItem):
                self.__index = self._workSpace.currentItem().parent(
                ).indexOfChild(self._workSpace.currentItem()) + 1
                self.__groupItem = self._workSpace.currentItem().parent()

            elif isinstance(self._workSpace.currentItem(), ModuleGroupItem):
                self.__index = 0
                self.__groupItem = self._workSpace.currentItem()

            else:
                return False

            #Get items
            s = bytes(
                mimeData.data(
                    Station.StationModulesMimeData.mimeType())).decode(
                        encoding="utf-8")
            dicts = json.loads(s, encoding="utf-8")
            self.__items = []
            for d in dicts:
                self.__items.append(Station.StationModules(d))

        else:
            return False

        return True

    def onDo(self):
        '''
            Do operation, return True if success.
        '''
        if self.__pasteGroup:
            #Groups
            for i in range(0, len(self.__groups)):
                self.__groups[i] = self._workSpace.operationAddGroup(
                    self.__index + i, self.__groups[i])

        else:
            #Items
            for i in range(0, len(self.__items)):
                self.__items[i] = self.__groupItem.addStationModule(
                    self.__items[i], self.__index + i)

        return True

    def onUndo(self):
        '''
            Undo operation.
        '''
        if self.__pasteGroup:
            #Groups
            for g in self.__groups:
                self._workSpace.operationRemoveGroup(g)

        else:
            #Items
            for i in self.__items:
                self.__groupItem.removeStationModule(i)

        return True
