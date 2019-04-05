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

import StationModule
import StringTable
import Faction
import Product
import Common
from Common import *


class FilterWidget(QWidget):
    refreshTreeView = pyqtSignal()

    def __init__(self, parent):
        super().__init__(parent)
        self.__layout = QGridLayout(self)
        self.setLayout(self.__layout)
        margins = self.__layout.contentsMargins()
        margins.setLeft(0)
        margins.setRight(0)
        self.__layout.setContentsMargins(margins)

        #By factions
        self.__chkByFaction = QCheckBox(
            StringTable.getString("CHK_BY_FACTION"), self)
        self.__layout.addWidget(self.__chkByFaction, 0, 0)
        self.__chkByFaction.stateChanged.connect(self.__checkStateChanged)
        self.__comboFaction = QComboBox(self)
        self.__layout.addWidget(self.__comboFaction, 0, 1)
        self.__loadFactions()
        self.__comboFaction.currentIndexChanged.connect(self.__factionChanged)

        #By product
        self.__chkByProduct = QCheckBox(
            StringTable.getString("CHK_BY_PRODUCT"), self)
        self.__layout.addWidget(self.__chkByProduct, 1, 0)
        self.__chkByProduct.stateChanged.connect(self.__checkStateChanged)
        self.__comboProduct = QComboBox(self)
        self.__layout.addWidget(self.__comboProduct, 1, 1)
        self.__loadProducts()
        self.__comboProduct.currentIndexChanged.connect(self.__productChanged)

        #By keywords
        self.__chkByKeywords = QCheckBox(
            StringTable.getString("CHK_BY_KEYWORDS"), self)
        self.__chkByKeywords.stateChanged.connect(self.__checkStateChanged)
        self.__layout.addWidget(self.__chkByKeywords, 2, 0)
        self.__txtKeywords = QLineEdit(self)
        self.__layout.addWidget(self.__txtKeywords, 2, 1)
        self.__txtKeywords.textChanged.connect(self.__keywordChanged)

    @TypeChecker(QWidget, StationModule.StationModule)
    def filter(self, m):
        #By faction
        if self.__chkByFaction.checkState() > 0:
            if self.__comboFaction.itemData(
                    self.__comboFaction.currentIndex()) not in m.factions():
                return False

        #By product
        if self.__chkByProduct.checkState() > 0:
            if isinstance(m, StationModule.ProductionModule):
                products = []
                for p in m.products():
                    products.append(p.product())

                if self.__comboProduct.itemData(
                        self.__comboProduct.currentIndex()) not in (
                            products):
                    return False

        #By keywords
        if self.__chkByKeywords.checkState() > 0:
            if self.__txtKeywords.text() not in m.name():
                return False

        return True

    @TypeChecker(QWidget, str)
    def setProductFilter(self, pid):
        self.__chkByProduct.setChecked(True)
        for i in range(0, self.__comboProduct.count()):
            if self.__comboProduct.itemData(i).id() == pid:
                self.__comboProduct.setCurrentIndex(i)
                return

        return

    def __loadFactions(self):
        for f in Faction.factions():
            self.__comboFaction.addItem(f.name(), f)

    def __loadProducts(self):
        for p in Product.products():
            self.__comboProduct.addItem(p.name(), p)

    #Slots
    @TypeChecker(QWidget, int)
    def __checkStateChanged(self, state):
        self.refreshTreeView.emit()

    @TypeChecker(QWidget, int)
    def __factionChanged(self, index):
        if self.__chkByFaction.checkState() > 0:
            self.refreshTreeView.emit()

    @TypeChecker(QWidget, int)
    def __productChanged(self, index):
        if self.__chkByProduct.checkState() > 0:
            self.refreshTreeView.emit()

    @TypeChecker(QWidget, str)
    def __keywordChanged(self, s):
        if self.__chkByKeywords.checkState() > 0:
            self.refreshTreeView.emit()
