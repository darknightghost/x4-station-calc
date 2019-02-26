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


class ModuleItem(QTreeWidgetItem):
    '''
        Station module group.
    '''

    class ModuleItemWidget(QWidget):
        #Signals
        moveUp = pyqtSignal()
        moveDown = pyqtSignal()
        remove = pyqtSignal()

        @TypeChecker(QWidget, Station.StationModules, QTreeWidgetItem)
        def __init__(self, item, parent):
            super().__init__(parent.treeWidget())
            self.__item = item
            self.__layout = QHBoxLayout(self)
            self.setLayout(self.__layout)

            self.__spinboxAmount = QSpinBox(self)
            self.__spinboxAmount.setMinimum(1)
            self.__spinboxAmount.valueChanged.connect(self.__onAmountChanged)
            self.__layout.addWidget(self.__spinboxAmount)

            def btnResizeEvent(event):
                width = event.size().height()
                self.__btnPrev.setMaximumWidth(width)
                self.__btnPrev.setMinimumWidth(width)

            self.__btnUp = QPushButton("↑", self)
            setattr(self.__btnUp, "resizeEvent", btnResizeEvent)
            self.__layout.addWidget(self.__btnUp)
            self.__btnUp.clicked.connect(self.__onMoveUp)

            self.__btnDown = QPushButton("↓", self)
            setattr(self.__btnDown, "resizeEvent", btnResizeEvent)
            self.__layout.addWidget(self.__btnDown)
            self.__btnDown.clicked.connect(self.__onMoveDown)

            self.__btnRemove = QPushButton("×", self)
            setattr(self.__btnRemove, "resizeEvent", btnResizeEvent)
            self.__layout.addWidget(self.__btnRemove)
            self.__btnRemove.clicked.connect(self.__onRemove)

            self.__layout.addStretch()

        @TypeChecker(QWidget, int)
        def __onAmountChanged(self, n):
            self.__item.setAmount(n)
            self.__loadAmount

        def __loadAmount(self):
            self.__spinboxAmount.setValue(self.__item.amount())

        def __onMoveUp(self):
            self.moveUp.emit()

        def __onMoveDown(self):
            self.moveDown.emit()

        def __onRemove(self):
            self.remove.emit()

    #Signals
    updateData = pyqtSignal()

    @TypeChecker(QTreeWidgetItem, Station.StationModules, QTreeWidgetItem)
    def __init__(self, item, parent):
        super().__init__(parent)
        self.__item = item
        self.setText(0, item.name())
        self.setFlags(Qt.ItemIsEnabled | Qt.ItemIsSelectable)
        self.__itemWidget = self.ModuleItemWidget(item, self)
        self.treeWidget().setItemWidget(self, 1, self.__itemWidget)
