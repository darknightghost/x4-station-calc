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


class SummaryItem(QTreeWidgetItem):
    '''
        Summary.
    '''

    @TypeChecker(QTreeWidgetItem, Station.StationModulesGroup,
                 (type(None), QTreeWidgetItem))
    def __init__(self, group, parent=None):
        super().__init__(parent, [group.name()])

        self.setFlags(Qt.ItemIsEnabled)
        self.setExpanded(True)

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
                "workforceCapacity",
                QTreeWidgetItem(
                    self,
                    [StringTable.getString("STR_WORKFORCE_CAPACITY"), "0"])
            ],
        ]

        #Products
        #{ID : amount}
        self.__products = {}

        self.update()

    def update(self):
        '''
            Update data.
        '''
        self.setText(0, self.__group.name())
        for a in self.__attributes:
            a[1].setText(1, "0")

        for m in self.__group:
            #Attributes
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

        for a in self.__attributes:
            if a[1].text(1) == "0":
                a[1].setHidden(True)

            else:
                a[1].setHidden(False)

        self.treeWidget().updateTotal.emit()

    def __stationModuleNumChanged(self, modules, oldNUm, newNum):
        self.update()

    def __stationModuleAddRemove(self, group, modules):
        self.update()
