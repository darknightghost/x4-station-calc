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

import Station
import Common
from Common import *

import WorkSpaceWidget

from WorkSpaceWidget.ModulesItem import *
from WorkSpaceWidget.ModuleItem import *
from WorkSpaceWidget.ModuleGroupItem import *
from WorkSpaceWidget.SummarysItem import *
from WorkSpaceWidget.Operations import *


class WorkSpaceWidget(QTreeWidget):
    '''
        Workspace.
    '''
    moduleClicked = pyqtSignal(list)
    updateTotal = pyqtSignal()
    changeEnableAddState = pyqtSignal(bool)
    changeAddGroupState = pyqtSignal(bool)
    changeUndoState = pyqtSignal(bool)
    changeRedoState = pyqtSignal(bool)
    changeCopyState = pyqtSignal(bool)
    changePasteState = pyqtSignal(bool)
    changeRemovePasteState = pyqtSignal(bool)
    updateItemButtons = pyqtSignal()

    @TypeChecker(QTreeWidget, QMainWindow, Station.Station)
    def __init__(self, parent, station):
        super().__init__(parent)
        self.__station = station
        self.header().setVisible(False)
        self.setColumnCount(2)
        self.setSelectionMode(QAbstractItemView.ExtendedSelection)
        self.header().setSectionResizeMode(QHeaderView.ResizeToContents)

        self.itemChanged.connect(self.__onItemChanged)
        self.itemSelectionChanged.connect(self.__onItemSelectionChanged)
        self.itemClicked.connect(self.__onItemClicked)

        self.setStyleSheet("QTreeWidget::item{height:28px}")

        self.__modulesItem = ModulesItem(self)
        self.addTopLevelItem(self.__modulesItem)

        self.__summarysItem = SummarysItem(self)
        self.addTopLevelItem(self.__summarysItem)

        self.__operationDone = []
        self.__operationUndone = []

    def initMenuState(self):
        self.changeEnableAddState.emit(False)
        self.changeAddGroupState.emit(True)
        self.changeUndoState.emit(False)
        self.changeRedoState.emit(False)
        self.changeCopyState.emit(False)
        self.changePasteState.emit(False)
        self.changeRemovePasteState.emit(False)

    def station(self):
        '''
            Get station file.
        '''
        return self.__station

    @TypeChecker(QTreeWidget, Operation)
    def doOperation(self, op):
        '''
            Do operation.
        '''
        pos = self.horizontalScrollBar().value()
        if not op.setWorkspace(self):
            return

        if op.do():
            self.__operationDone.append(op)
            if len(self.__operationDone) > 1024:
                self.__operationDone = self.__operationDone[-1024:]
            self.__operationUndone = []
            self.__updateUndoRedo()

        self.horizontalScrollBar().setValue(pos)

    def redo(self):
        '''
            Redo undone operation.
        '''
        if len(self.__operationUndone) > 0:
            op = self.__operationUndone.pop()
            op.do()
            self.__operationDone.append(op)
            self.__updateUndoRedo()

    def undo(self):
        '''
            Undo operation.
        '''
        if len(self.__operationDone) > 0:
            op = self.__operationDone.pop()
            op.undo()
            self.__operationUndone.append(op)
            self.__updateUndoRedo()

    def addGroup(self):
        '''
            Add new Group.
        '''
        op = NewGroupOperation()
        self.doOperation(op)

    def remove(self):
        '''
            Remove selected.
        '''
        op = RemoveOperation()
        self.doOperation(op)

    @TypeChecker(QTreeWidget, int, (type(None), ModuleGroupItem))
    def operationAddGroup(self, index=-1, g=None):
        '''
            Add new group.
        '''
        if index < 0:
            index += self.__modulesItem.childCount() + 1

        if g == None:
            g = Station.StationModulesGroup()
            self.__station.insert(index, g)
            ret = ModuleGroupItem(g, None)
            self.__modulesItem.insertChild(index, ret)
            return ret

        else:
            self.__station.insert(index, g.item())
            self.__modulesItem.insertChild(index, g)
            return g

    @TypeChecker(QTreeWidget, ModuleGroupItem)
    def operationRemoveGroup(self, g):
        self.__station.remove(g.item())
        self.__modulesItem.removeChild(g)

    def __updateUndoRedo(self):
        self.changeUndoState.emit(False if len(self.__operationDone) ==
                                  0 else True)
        self.changeRedoState.emit(False if len(self.__operationUndone) ==
                                  0 else True)

    @TypeChecker(QTreeWidget, QTreeWidgetItem, int)
    def __onItemChanged(self, item, column):
        if hasattr(item, "onChanged"):
            item.onChanged(item, column)

    @TypeChecker(QTreeWidget, QTreeWidgetItem, int)
    def __onItemClicked(self, item, column):
        if hasattr(item, "onClicked"):
            item.onClicked(column)

    def __onItemSelectionChanged(self):
        selected = self.selectedItems()

        if len(selected) == 1:
            #Add module & paste
            if isinstance(selected[0], ModuleGroupItem) or isinstance(
                    selected[0], ModuleItem):
                self.changePasteState.emit(True)
                self.changeEnableAddState.emit(True)

            else:
                self.changePasteState.emit(False)
                self.changeEnableAddState.emit(False)

        else:
            self.changeEnableAddState.emit(False)
            self.changePasteState.emit(False)

        #Copy/cut/remove
        copyable = len(selected) > 0
        removeable = len(selected) > 0
        for s in selected:
            if isinstance(s, ModuleGroupItem):
                try:
                    if mark != 0:
                        copyable = False

                except NameError:
                    mark = 0

            elif isinstance(s, ModuleItem):
                try:
                    if mark != 1:
                        copyable = False

                except NameError:
                    mark = 1

            else:
                copyable = False
                removeable = False
                break

        self.changeCopyState.emit(copyable)
        self.changeRemovePasteState.emit(removeable)

    @TypeChecker(QTreeWidget, QCloseEvent)
    def closeEvent(self, event):
        #Disable all actions
        self.changeEnableAddState.emit(False)
        self.changeAddGroupState.emit(False)
        self.changeUndoState.emit(False)
        self.changeRedoState.emit(False)
        self.changeCopyState.emit(False)
        self.changePasteState.emit(False)
        self.changeRemovePasteState.emit(False)
        super().closeEvent(event)

    @TypeChecker(QTreeWidget, QContextMenuEvent)
    def contextMenuEvent(self, event):
        #Popup menu.
        self.parent().editMenu().exec(event.globalPos())
        super().contextMenuEvent(event)
