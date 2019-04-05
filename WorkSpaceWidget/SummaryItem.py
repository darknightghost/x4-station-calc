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
import Product

import WorkSpaceWidget


class SummaryProductItem(QTreeWidgetItem):
    @TypeChecker(QTreeWidgetItem, Product.Product, str,
                 (QTreeWidgetItem, type(None)))
    def __init__(self, product, amountStr, parent=None):
        super().__init__(parent, [product.name(), amountStr])
        self.setFlags(Qt.ItemIsEnabled | Qt.ItemIsSelectable)
        self.__product = product

        self.__btnWidget = QWidget()
        self.__btnLayout = QHBoxLayout(self.__btnWidget)
        margins = self.__btnLayout.contentsMargins()
        margins.setLeft(0)
        margins.setRight(0)
        margins.setTop(0)
        margins.setBottom(0)
        self.__btnLayout.setContentsMargins(margins)
        self.__btnFilterModules = QPushButton(
            StringTable.getString("BTN_FILTER_MODULES"))
        self.__btnLayout.addWidget(self.__btnFilterModules)
        self.__btnLayout.addStretch()
        self.treeWidget().setItemWidget(self, 2, self.__btnWidget)
        self.__btnFilterModules.clicked.connect(self.onBtnFilterClicked)

    def onBtnFilterClicked(self):
        self.treeWidget().setProductFilter(self.__product.id())

    @TypeChecker(QTreeWidgetItem, int)
    def onClicked(self, column):
        self.treeWidget().moduleClicked.emit(self.__product.info())


class SummaryItem(QTreeWidgetItem):
    '''
        Summary.
    '''

    @TypeChecker(QTreeWidgetItem, Station.StationModulesGroup,
                 (type(None), QTreeWidgetItem))
    def __init__(self, group, parent=None):
        super().__init__(parent, [group.name()])

        self.setFlags(Qt.ItemIsEnabled)
        self.setExpanded(False)

        self.__group = group
        group.addModules.connect(self.__stationModuleAddRemove)
        group.removeModules.connect(self.__stationModuleAddRemove)
        group.nameChanged.connect(self.update)

        #Station Attributes
        #[["attribute name", item]]
        self.__attributes = [
            [
                "turretNum",
                QTreeWidgetItem(self,
                                [StringTable.getString("STR_TURRET_NUM"), "0"])
            ],
            [
                "sLaunchTubes",
                QTreeWidgetItem(
                    self, [StringTable.getString("STR_S_LAUNCE_TUBE"), "0"])
            ],
            [
                "mLaunchTubes",
                QTreeWidgetItem(
                    self, [StringTable.getString("STR_M_LAUNCE_TUBE"), "0"])
            ],
            [
                "shipStorage",
                QTreeWidgetItem(
                    self, [StringTable.getString("STR_SHIP_STORAGE"), "0"])
            ],
            [
                "sDock",
                QTreeWidgetItem(self,
                                [StringTable.getString("STR_S_DOCK"), "0"])
            ],
            [
                "mDock",
                QTreeWidgetItem(self,
                                [StringTable.getString("STR_M_DOCK"), "0"])
            ],
            [
                "lXLDock",
                QTreeWidgetItem(self,
                                [StringTable.getString("STR_L_XL_DOCK"), "0"])
            ],
            [
                "lFabricationBay",
                QTreeWidgetItem(
                    self,
                    [StringTable.getString("STR_L_SHIP_FABRICATION_BAY"), "0"])
            ],
            [
                "xlFabricationBay",
                QTreeWidgetItem(self, [
                    StringTable.getString("STR_XL_SHIP_FABRICATION_BAY"), "0"
                ])
            ],
            [
                "lMaintenanceBay",
                QTreeWidgetItem(
                    self,
                    [StringTable.getString("STR_L_SHIP_MAINTENANCE_BAY"), "0"])
            ],
            [
                "xlMaintenanceBay",
                QTreeWidgetItem(self, [
                    StringTable.getString("STR_XL_SHIP_MAINTENANCE_BAY"), "0"
                ])
            ],
        ]

        #Workforce
        self.__workforce = 0
        self.__workforceItem = QTreeWidgetItem(
            self, [StringTable.getString("STR_WORKFORCE_CAPACITY")])
        self.__workforceItem.setFlags(Qt.ItemIsEnabled)

        #Foods
        #{Product : amount}
        self.__foods = {}
        self.__foodsItem = QTreeWidgetItem(
            self, [StringTable.getString("STR_SUPPLY_REQUIRED")])
        self.__foodsItem.setFlags(Qt.ItemIsEnabled)
        self.__foodsItem.setExpanded(True)

        #Resources
        #{Product : amount}
        self.__resources = {}
        self.__resourcesItem = QTreeWidgetItem(
            self, [StringTable.getString("STR_RESOURCES")])
        self.__resourcesItem.setFlags(Qt.ItemIsEnabled)
        self.__resourcesItem.setExpanded(True)

        #Intermediates
        self.__intermediatesItem = QTreeWidgetItem(
            self, [StringTable.getString("STR_INTERMEDIATES")])
        self.__intermediatesItem.setFlags(Qt.ItemIsEnabled)
        self.__intermediatesItem.setExpanded(True)

        #Products
        #{Product : amount}
        self.__products = {}
        self.__maxProducts = {}
        self.__productsItem = QTreeWidgetItem(
            self, [StringTable.getString("STR_PRODUCTS")])
        self.__productsItem.setFlags(Qt.ItemIsEnabled)
        self.__productsItem.setExpanded(True)

        self.update()

    def attributes(self):
        '''
            Attrributes.

            return {"attr" : num}
        '''
        ret = {}
        for a in self.__attributes:
            ret[a[0]] = int(a[1].text(1))

        return ret

    def workforce(self):
        '''
            Get workforce.
        '''
        return self.__workforce

    def foods(self):
        '''
            Get foods.
        '''
        return self.__foods

    def products(self):
        '''
            Get products.
        '''
        return self.__products, self.__maxProducts

    def resources(self):
        '''
            Get resources.
        '''
        return self.__resources

    def update(self):
        '''
            Update data.
        '''
        self.setText(0, self.__group.name())
        for a in self.__attributes:
            a[1].setText(1, "0")

        self.__products = {}
        self.__maxProducts = {}
        self.__resources = {}
        self.__foods = {}
        self.__workforce = 0

        self.__foodsItem.takeChildren()
        self.__resourcesItem.takeChildren()
        self.__intermediatesItem.takeChildren()
        self.__productsItem.takeChildren()

        for m in self.__group:
            #Attributes
            try:
                m.amountChanged.disconnect(self.__stationModuleNumChanged)

            except TypeError:
                pass

            m.amountChanged.connect(self.__stationModuleNumChanged)
            for a in self.__attributes:
                amount = int(a[1].text(1))
                try:
                    amount += getattr(m.stationModule(), a[0])() * m.amount()
                except AttributeError:
                    continue

                a[1].setText(1, str(amount))
                a[1].setFlags(Qt.ItemIsEnabled)
                a[1].setExpanded(True)

            #Workforce
            try:
                self.__workforce += m.stationModule().workforceCapacity(
                ) * m.amount()

            except AttributeError:
                pass

            try:
                self.__workforce -= m.stationModule().maxEmployeeNum(
                ) * m.amount()

            except AttributeError:
                pass

            #Products
            try:
                products = m.stationModule().products()
                for p in products:
                    try:
                        self.__products[p.product()] += p.amount() * m.amount()
                        self.__maxProducts[p.product()] += p.amount(
                        ) * m.amount() * m.stationModule().maxEfficiency()

                    except KeyError:
                        self.__products[p.product()] = p.amount() * m.amount()
                        self.__maxProducts[p.product()] = p.amount(
                        ) * m.amount() * m.stationModule().maxEfficiency()

            except AttributeError:
                pass

            #Resources
            try:
                resources = m.stationModule().resources()
                for r in resources:
                    try:
                        self.__resources[
                            r.product()] += r.amount() * m.amount()

                    except KeyError:
                        self.__resources[r.product()] = r.amount() * m.amount()

            except AttributeError:
                pass

            #Foods
            try:
                foods = m.stationModule().foods()
                for f in foods:
                    try:
                        self.__foods[f.product()] += f.amount() * m.amount()

                    except KeyError:
                        self.__foods[f.product()] = f.amount() * m.amount()

            except AttributeError:
                pass

        #Workforce
        self.__workforceItem.setText(1, "%+d" % (self.__workforce))

        #Foods, resources, intermediates & products
        for p in self.__products:
            if p in self.__resources:
                #p is an intermediate
                amount = self.__products[p] - self.__resources[p]
                maxAmount = self.__maxProducts[p] - self.__resources[p]
                if p in self.__foods:
                    amount -= self.__foods[p]
                    maxAmount -= self.__foods[p]
                SummaryProductItem(p, "%+d/h -> %+d/h" % (amount, maxAmount),
                                   self.__intermediatesItem)

            else:
                #p is a product
                amount = self.__products[p]
                maxAmount = self.__maxProducts[p]
                if p in self.__foods:
                    amount -= self.__foods[p]
                    maxAmount -= self.__foods[p]

                if amount > 0 or maxAmount > 0:
                    SummaryProductItem(p, "%d/h -> %d/h" % (amount, maxAmount),
                                       self.__productsItem)

        for p in self.__resources:
            if p in self.__products:
                continue

            amount = self.__resources[p]
            SummaryProductItem(p, "%d/h" % (amount), self.__resourcesItem)

        for p in self.__foods:
            amount = self.__foods[p]
            maxAmount = self.__foods[p]
            if p in self.__products:
                if amount > self.__products[p] or maxAmount > self.__products[
                        p]:
                    amount -= self.__products[p]
                    amount = amount if amount > 0 else 0
                    maxAmount -= self.__maxProducts[p]
                    maxAmount = maxAmount if maxAmount > 0 else 0

                else:
                    continue

            SummaryProductItem(p, "%d/h -> %d/h" % (amount, maxAmount),
                               self.__foodsItem)

        #Hide empty items
        for a in self.__attributes:
            if a[1].text(1) == "0":
                a[1].setHidden(True)

            else:
                a[1].setHidden(False)

        for item in (self.__foodsItem, self.__resourcesItem,
                     self.__intermediatesItem, self.__productsItem):
            if item.childCount() == 0:
                item.setHidden(True)

            else:
                item.setHidden(False)

        self.treeWidget().updateTotal.emit()

    def __stationModuleNumChanged(self, modules, oldNUm, newNum):
        self.update()

    def __stationModuleAddRemove(self, group, modules):
        self.update()
