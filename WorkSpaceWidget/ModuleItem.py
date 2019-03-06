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

import WorkSpaceWidget


class ModuleItemWidget(QWidget):
    #Signals
    @TypeChecker(QWidget, Station.StationModules, QTreeWidget, QTreeWidgetItem)
    def __init__(self, item, treeWidget, treeItem):
        super().__init__()
        self.__treeWidget = treeWidget
        self.__treeItem = treeItem
        self.__item = item
        self.__layout = QHBoxLayout(self)
        self.setLayout(self.__layout)
        margins = self.__layout.contentsMargins()
        margins.setLeft(0)
        margins.setRight(0)
        margins.setTop(0)
        margins.setBottom(0)
        self.__layout.setContentsMargins(margins)
        self.setMaximumHeight(28)

        self.__layout.addStretch()

        self.__spinboxAmount = QSpinBox(self)
        self.__spinboxAmount.setMinimum(1)
        self.__spinboxAmount.setValue(self.__item.amount())
        self.__spinboxAmount.valueChanged.connect(self.__onSpinAmountChanged)
        self.__layout.addWidget(self.__spinboxAmount)

        self.__btnUp = QSquareButton("↑", self)
        self.__layout.addWidget(self.__btnUp)
        self.__btnUp.clicked.connect(self.__onMoveUp)

        self.__btnDown = QSquareButton("↓", self)
        self.__layout.addWidget(self.__btnDown)
        self.__btnDown.clicked.connect(self.__onMoveDown)

        self.__btnRemove = QSquareButton("×", self)
        self.__layout.addWidget(self.__btnRemove)
        self.__btnRemove.clicked.connect(self.__onRemove)

        self.__layout.addStretch()
        self.__item.amountChanged.connect(self.__onItemAmountChanged)

        self.__onUpdateBtn()
        self.__treeWidget.updateItemButtons.connect(self.__onUpdateBtn)

    def __onUpdateBtn(self):
        if self.__item == self.__item.parent()[0]:
            self.__btnUp.setEnabled(False)

        else:
            self.__btnUp.setEnabled(True)

        if self.__item == self.__item.parent()[-1]:
            self.__btnDown.setEnabled(False)

        else:
            self.__btnDown.setEnabled(True)

    @TypeChecker(QWidget, int)
    def __onSpinAmountChanged(self, n):
        if n != self.__item.amount():
            from WorkSpaceWidget.Operations import ChangeModuleAmountOperation as ChangeModuleAmountOperation
            op = ChangeModuleAmountOperation(self.__item, n)
            self.__treeWidget.doOperation(op)
            self.__loadAmount

    def __onItemAmountChanged(self, stationModules, oldNum, newNum):
        self.__spinboxAmount.setValue(newNum)

    def __loadAmount(self):
        self.__spinboxAmount.setValue(self.__item.amount())

    def __onMoveUp(self):
        from WorkSpaceWidget.Operations import SwapModuleOperation as SwapModuleOperation
        index = self.__treeItem.parent().indexOfChild(self.__treeItem)
        op = SwapModuleOperation(self.__treeItem.parent(), index, index - 1)
        self.__treeWidget.doOperation(op)

    def __onMoveDown(self):
        from WorkSpaceWidget.Operations import SwapModuleOperation as SwapModuleOperation
        index = self.__treeItem.parent().indexOfChild(self.__treeItem)
        op = SwapModuleOperation(self.__treeItem.parent(), index, index + 1)
        self.__treeWidget.doOperation(op)

    def __onRemove(self):
        from WorkSpaceWidget.Operations import RemoveOperation as RemoveOperation
        op = RemoveOperation([self.__treeItem])
        self.__treeWidget.doOperation(op)
        
    def closeEvent(self, event):
        self.__treeWidget.updateItemButtons.disconnect(self.__onUpdateBtn)


class ModuleItem(QTreeWidgetItem):
    '''
        Station module group.
    '''
    #Signals
    updateData = pyqtSignal()

    @TypeChecker(QTreeWidgetItem, Station.StationModules,
                 (QTreeWidgetItem, type(None)))
    def __init__(self, item, parent=None):
        super().__init__(parent)
        self.__item = item
        self.setText(0, item.stationModule().name())
        self.setFlags(Qt.ItemIsEnabled | Qt.ItemIsSelectable)

    def item(self):
        '''
            Get Item.
        '''
        return self.__item

    def onAdd(self):
        self.__itemWidget = ModuleItemWidget(self.__item, self.treeWidget(),
                                             self)
        self.treeWidget().setItemWidget(self, 1, self.__itemWidget)
        self.__itemWidget.show()
        
    def onRemove(self):
        self.__itemWidget.close()

    @TypeChecker(QTreeWidgetItem, int)
    def onClicked(self, column):
        self.treeWidget().moduleClicked.emit(
            self.__item.stationModule().info())
