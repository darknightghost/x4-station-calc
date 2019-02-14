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
        self.__comboFaction = QComboBox(self)
        self.__layout.addWidget(self.__comboFaction, 0, 1)
        self.__loadFactions()

        #By product
        self.__chkByProduct = QCheckBox(
            StringTable.getString("CHK_BY_PRODUCT"), self)
        self.__layout.addWidget(self.__chkByProduct, 1, 0)
        self.__comboProduct = QComboBox(self)
        self.__layout.addWidget(self.__comboProduct, 1, 1)
        self.__loadProducts()

        #By keywords
        self.__chkByKeywords = QCheckBox(
            StringTable.getString("CHK_BY_KEYWORDS"), self)
        self.__layout.addWidget(self.__chkByKeywords, 2, 0)
        self.__txtKeywords = QLineEdit(self)
        self.__layout.addWidget(self.__txtKeywords, 2, 1)

    @TypeChecker(QWidget, StationModule.StationModule)
    def filter(self, m):
        return True

    def __loadFactions(self):
        for f in Faction.factions():
            self.__comboFaction.addItem(f.name(), f)

    def __loadProducts(self):
        for p in Product.products():
            self.__comboProduct.addItem(p.name(), p)
