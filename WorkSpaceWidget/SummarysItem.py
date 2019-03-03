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

import WorkSpaceWidget

from WorkSpaceWidget.SummaryItem import *


class SummarysItem(QTreeWidgetItem):
    @TypeChecker(QTreeWidgetItem, QTreeWidget)
    def __init__(self, parent):
        super().__init__(parent)
        self.setText(0, StringTable.getString("STR_SUMMARY"))
        self.setFlags(Qt.ItemIsEnabled)
        self.setExpanded(True)

        #Groups
        self.__groupsSummaryItem = QTreeWidgetItem(
            self, [StringTable.getString("STR_SUMMARY_EACH_GROUP")])
        self.__groupsSummaryItem.setFlags(Qt.ItemIsEnabled)
        self.__groupsSummaryItem.setExpanded(False)

        self.__groups = {}
        station = self.treeWidget().station()
        for g in station:
            self.__groups[g] = SummaryItem(g, self.__groupsSummaryItem)

        #Total
        self.__totalSummaryItem = QTreeWidgetItem(
            self, [StringTable.getString("STR_SUMMARY_TOTAL")])
        self.__totalSummaryItem.setFlags(Qt.ItemIsEnabled)
        self.__totalSummaryItem.setExpanded(True)
        self.__initTotal()
        self.__onUpdateTotal()

        station.addGroup.connect(self.__onAddGroup)
        station.removeGroup.connect(self.__onRemoveGroup)

        self.treeWidget().updateTotal.connect(self.__onUpdateTotal)

    #Slots
    def __onAddGroup(self, station, group):
        self.__groups[group] = SummaryItem(group, self.__groupsSummaryItem)
        self.treeWidget().updateTotal.emit()

    def __onRemoveGroup(self, station, group):
        self.__groupsSummaryItem.removeChild(self.__groups[group])
        del self.__groups[group]
        self.treeWidget().updateTotal.emit()

    def __initTotal(self):
        #Station Attributes
        #[["attribute name", item]]
        self.__attributes = {
            "turretNum":
            QTreeWidgetItem(self.__totalSummaryItem,
                            [StringTable.getString("STR_TURRET_NUM"), "0"]),
            "sLaunchTubes":
            QTreeWidgetItem(self.__totalSummaryItem,
                            [StringTable.getString("STR_S_LAUNCE_TUBE"), "0"]),
            "mLaunchTubes":
            QTreeWidgetItem(self.__totalSummaryItem,
                            [StringTable.getString("STR_M_LAUNCE_TUBE"), "0"]),
            "shipStorage":
            QTreeWidgetItem(self.__totalSummaryItem,
                            [StringTable.getString("STR_SHIP_STORAGE"), "0"]),
            "sDock":
            QTreeWidgetItem(self.__totalSummaryItem,
                            [StringTable.getString("STR_S_DOCK"), "0"]),
            "mDock":
            QTreeWidgetItem(self.__totalSummaryItem,
                            [StringTable.getString("STR_M_DOCK"), "0"]),
            "lXLDock":
            QTreeWidgetItem(self.__totalSummaryItem,
                            [StringTable.getString("STR_L_XL_DOCK"), "0"]),
            "workforceCapacity":
            QTreeWidgetItem(
                self.__totalSummaryItem,
                [StringTable.getString("STR_WORKFORCE_CAPACITY"), "0"]),
        }

        #Foods
        #{Product : amount}
        self.__foods = {}
        self.__foodsItem = QTreeWidgetItem(
            self.__totalSummaryItem,
            [StringTable.getString("STR_SUPPLY_REQUIRED")])
        self.__foodsItem.setFlags(Qt.ItemIsEnabled)
        self.__foodsItem.setExpanded(True)

        #Resources
        #{Product : amount}
        self.__resources = {}
        self.__resourcesItem = QTreeWidgetItem(
            self.__totalSummaryItem, [StringTable.getString("STR_RESOURCES")])
        self.__resourcesItem.setFlags(Qt.ItemIsEnabled)
        self.__resourcesItem.setExpanded(True)

        #Intermediates
        self.__intermediatesItem = QTreeWidgetItem(
            self.__totalSummaryItem,
            [StringTable.getString("STR_INTERMEDIATES")])
        self.__intermediatesItem.setFlags(Qt.ItemIsEnabled)
        self.__intermediatesItem.setExpanded(True)

        #Products
        #{Product : amount}
        self.__products = {}
        self.__productsItem = QTreeWidgetItem(
            self.__totalSummaryItem, [StringTable.getString("STR_PRODUCTS")])
        self.__productsItem.setFlags(Qt.ItemIsEnabled)
        self.__productsItem.setExpanded(True)

    def __onUpdateTotal(self):
        #Clear
        for a in self.__attributes:
            self.__attributes[a].setText(1, "0")

        self.__foodsItem.takeChildren()
        self.__resourcesItem.takeChildren()
        self.__intermediatesItem.takeChildren()
        self.__productsItem.takeChildren()

        allFoods = {}
        allProducts = {}
        allResources = {}

        def merge(dest, src):
            for p in src:
                try:
                    dest[p] += src[p]

                except KeyError:
                    dest[p] = src[p]

        #Update data
        for k in self.__groups:
            g = self.__groups[k]
            attrs = g.attributes()
            for a in attrs:
                self.__attributes[a].setText(
                    1, str(int(self.__attributes[a].text(1)) + attrs[a]))

            merge(allFoods, g.foods())
            merge(allProducts, g.products())
            merge(allResources, g.resources())

        #Foods, resources, intermediates & products
        intermediates = []
        foods = []
        products = []
        resources = []

        for p in allProducts:
            if p in allResources:
                #p is an intermediate
                amount = allProducts[p] - allResources[p]
                SummaryProductItem(p, "%+d/h" % (amount),
                                   self.__intermediatesItem)

            else:
                #p is a product
                amount = allProducts[p]
                if p in allFoods:
                    amount -= allFoods[p]

                if amount > 0:
                    SummaryProductItem(p, "%d/h" % (amount),
                                       self.__productsItem)

        for p in allResources:
            if p in allProducts:
                continue

            amount = allResources[p]
            SummaryProductItem(p, "%d/h" % (amount), self.__resourcesItem)

        for p in allFoods:
            amount = allFoods[p]
            if p in allProducts:
                if amount < allProducts[p]:
                    continue

                else:
                    amount -= allProducts[p]

            SummaryProductItem(p, "%d/h" % (amount), self.__foodsItem)

        #Hide empty items
        for a in self.__attributes:
            item = self.__attributes[a]
            item.setFlags(Qt.ItemIsEnabled)
            if item.text(1) == "0":
                item.setHidden(True)

            else:
                item.setHidden(False)

        for item in (self.__foodsItem, self.__resourcesItem,
                     self.__intermediatesItem, self.__productsItem):
            if item.childCount() == 0:
                item.setHidden(True)

            else:
                item.setHidden(False)
