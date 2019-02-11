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
'''
    Station modules.
'''

import pathlib
import json

import StringTable
import Faction
import Product

import Common
from Common import *

__STATION_MODULES = {}
__STATION_MODULE_TYPE = {}
'''
    "Venture": ["STATION_TYPE_VENTURE", None],
    "Build": ["STATION_TYPE_BUILD", None]
'''


def __initialize():
    stationModuleDir = pathlib.Path(__file__).parent / "station_modules"
    for t in __STATION_MODULE_TYPE.keys():
        moduleDir = stationModuleDir / t.lower()
        if not moduleDir.exists():
            continue

        for sname in moduleDir.glob("*.json"):
            print("Loading data file \"%s\"..." % (str(sname)))
            with open(str(sname)) as f:
                data = json.loads(f.read())
                m = __STATION_MODULE_TYPE[t][1](data)
                __STATION_MODULES[m.id()] = m


@TypeChecker(str)
def typeName(t):
    '''
        Get name of type.
    '''
    return StringTable.get_string(__STATION_MODULE_TYPE[t][0])


@TypeChecker(str, str, type)
def __registStationModule(typeStr, nameID, cls):
    '''
        Regist station info.
    '''
    global __STATION_MODULE_TYPE
    __STATION_MODULE_TYPE[typeStr] = (nameID, cls)


class StationProductInfo:
    '''
        Station product info.
    '''

    @TypeChecker(object, dict)
    def __init__(self, data):
        self.__id = data["id"]
        Product.product(self.__id)
        self.__amount = data["amount"]
        if not isinstance(self.__amount, int):
            raise TypeError("Amount should be an integer.")

    def product(self):
        '''
            Get product.
        '''
        return Product.product(self.__id)

    def amount(self):
        '''
            Get amount.
        '''
        return self.__amount

    def __str__(self):
        return "{\n" \
                "    product name = %s\n" \
                "    amount = %d\n" \
                "}" % (self.product().name(),
                        self.amount())


class __StationModule:
    '''
        Base class of all station modules.
    '''

    @TypeChecker(object, dict)
    def __init__(self, data):
        self.__id = data["id"]

        self.__type = data["type"]
        typeName(self.__type)

        self.__factions = data["factions"].copy()
        for f in self.__factions:
            Faction.faction(f)

        self.__turretNum = data["turret"]
        if not isinstance(self.__turretNum, int):
            raise TypeError("Number of turret should be a integer.")

        self.__name = data["name"].copy()

    def id(self):
        '''
            Get module id.
        '''
        return self.__id

    def type(self):
        '''
            Get module type.
        '''
        return self.__type

    def typeName(self):
        '''
            Get name of module type.
        '''
        return typeName(self.type())

    def factions(self):
        '''
            Get list of factions.
        '''
        ret = []
        for f in self.__factions:
            ret.append(Faction.faction(f))

        return ret

    def turretNum(self):
        '''
            Get number of turret.
        '''
        return self.__turretNum

    def name(self):
        '''
            Get module name.
        '''
        try:
            return self.__name[StringTable.locale()]

        except KeyError:
            return self.__name[StringTable.default_locale()]

    def __str__(self):
        factionsStr = "[\n"
        for f in self.factions():
            factionsStr += "    %s,\n" % (f.name())

        factionsStr = factionsStr[:-2] + "\n]"
        return "id = %s,\n" \
                "type = %s,\n" \
                "name = %s,\n" \
                "factions = %s,\n" \
                "number of turret = %d" % (
                        self.id(),
                        self.typeName(),
                        self.name(),
                        factionsStr,
                        self.turretNum()
                    )


class DefenceModule(__StationModule):
    '''
        Defence module.
    '''

    @TypeChecker(object, dict)
    def __init__(self, data):
        super().__init__(data)
        self.__sLaunchTubes = data["s_launch_tubes"]
        if not isinstance(self.__sLaunchTubes, int):
            raise TypeError("Value of s_launch_tubes should be an integer.")

        self.__mLaunchTubes = data["m_launch_tubes"]
        if not isinstance(self.__mLaunchTubes, int):
            raise TypeError("Value of m_launch_tubes should be an integer.")

    def sLaunchTubes(self):
        '''
            Get number of s launch tubes.
        '''
        return self.__sLaunchTubes

    def sLaunchTubes(self):
        '''
            Get number of m launch tubes.
        '''
        return self.__mLaunchTubes

    def __str__(self):
        commonStr = addIndent(super().__str__())
        return "{\n" \
                "%s,\n" \
                "    number of s launch tubes = %d,\n" \
                "    number of m launch tubes = %d\n" \
                "}" % (
                        commonStr,
                        self.__sLaunchTubes,
                        self.__mLaunchTubes)


#Regist type
__registStationModule("Defence", "STATION_TYPE_DEFENCE", DefenceModule)


class DockModule(__StationModule):
    '''
        Dock module.
    '''

    @TypeChecker(object, dict)
    def __init__(self, data):
        super().__init__(data)
        self.__shipStorage = data["ship_storage"]
        if not isinstance(self.__shipStorage, int):
            raise TypeError("Value of ship_storage should be an integer.")

        self.__sDocking = data["s_docking"]
        if not isinstance(self.__sDocking, int):
            raise TypeError("Value of s_docking should be an integer.")

        self.__mDocking = data["m_docking"]
        if not isinstance(self.__mDocking, int):
            raise TypeError("Value of m_docking should be an integer.")

        self.__lXLDocking = data["l_xl_docking"]
        if not isinstance(self.__lXLDocking, int):
            raise TypeError("Value of l_xl_docking should be an integer.")

    def shipStorage(self):
        '''
            Get ship storage.
        '''
        return self.__shipStorage

    def sDocking(self):
        '''
            Get number of s docking.
        '''
        return self.__sDocking

    def mDocking(self):
        '''
            Get number of m docking.
        '''
        return self.__mDocking

    def lXLDocking(self):
        '''
            Get number of L/XL docking.
        '''
        return self.__lXLDocking

    def __str__(self):
        commonStr = addIndent(super().__str__())
        return "{\n" \
                "%s,\n" \
                "    number of ship storage = %d,\n" \
                "    number of S docking = %d,\n" \
                "    number of M docking = %d,\n" \
                "    number of L/XL ldocking = %d\n" \
                "}" % (
                        commonStr,
                        self.__shipStorage,
                        self.__sDocking,
                        self.__mDocking,
                        self.__lXLDocking)


#Regist type
__registStationModule("Dock", "STATION_TYPE_DOCK", DockModule)


class HabitationModule(__StationModule):
    ''' 
        Habitation modules.
    '''

    @TypeChecker(object, dict)
    def __init__(self, data):
        super().__init__(data)

        self.__workforceCapacity = data["workforce_capacity"]
        if not isinstance(self.__workforceCapacity, int):
            raise TypeError(
                "Value of workforce_capacity should be an integer.")

        self.__foods = []
        for f in data["food"]:
            self.__foods.append(StationProductInfo(f))

    def workforceCapacity(self):
        '''
            Get workforce capacity.
        '''
        return self.__workforceCapacity

    def foods(self):
        '''
            Get foods.
        '''
        return self.__foods

    def __str__(self):
        commonStr = addIndent(super().__str__())
        foodsStr = "foods = [\n"
        for f in self.foods():
            foodsStr += "%s,\n" % (addIndent(str(f)))

        foodsStr = addIndent(foodsStr[:-2] + "\n]")
        return "{\n" \
                "%s,\n" \
                "    workforce capacity = %d,\n" \
                "%s\n" \
                "}" % (
                        commonStr,
                        self.workforceCapacity(),
                        foodsStr)


#Regist type
__registStationModule("Habitation", "STATION_TYPE_HABITATION",
                      HabitationModule)


class OtherModule(__StationModule):
    '''
        Other modules.
    '''

    @TypeChecker(object, dict)
    def __init__(self, data):
        super().__init__(data)

    def __str__(self):
        commonStr = addIndent(super().__str__())

        return "{\n" \
                "%s\n" \
                "}" % (
                        commonStr)


#Regist type
__registStationModule("Other", "STATION_TYPE_OTHER", OtherModule)


class ProductionModule(__StationModule):
    '''
        Production modules.
    '''

    @TypeChecker(object, dict)
    def __init__(self, data):
        super().__init__(data)
        self.__products = []
        for p in data["products"]:
            self.__products.append(StationProductInfo(p))

        self.__resources = []
        for r in data["resources"]:
            self.__resources.append(StationProductInfo(r))

    def products(self):
        '''
            Get products.
        '''
        return self.__products

    def resources(self):
        '''
            Get resources.
        '''
        return self.__resources

    def __str__(self):
        commonStr = addIndent(super().__str__())
        resourcesStr = "resources = [\n"
        for r in self.resources():
            resourcesStr += "%s,\n" % (addIndent(str(r)))

        resourcesStr = addIndent(resourcesStr[:-2] + "\n]")

        productsStr = "products = [\n"
        for p in self.products():
            productsStr += "%s,\n" % (addIndent(str(p)))

        productsStr = addIndent(productsStr[:-2] + "\n]")
        return "{\n" \
                "%s,\n" \
                "%s,\n" \
                "%s\n" \
                "}" % (
                        commonStr,
                        resourcesStr,
                        productsStr)


#Regist type
__registStationModule("Production", "STATION_TYPE_PRODUCTION",
                      ProductionModule)


class StorageModule(__StationModule):
    '''
        Storage modules.
    '''

    @TypeChecker(object, dict)
    def __init__(self, data):
        super().__init__(data)

        self.__storageType = data["storage_type"]

        if not isinstance(self.__storageType, str):
            raise TypeError("Type of storage_type must be string.")

        if self.__storageType not in ("Solid", "Container", "Liquid"):
            raise ValueError("Illegale storage type \"%s\"." \
                %(self.__storageType))

        self.__storageCapacity = data["storage_capacity"]
        if not isinstance(self.__storageCapacity, int):
            raise TypeError("Type of storage_capacity must be integer.")

    def storageType(self):
        '''
            Get storage type.
        '''
        return self.__storageType

    def storageCapacity(self):
        '''
            Get storage capacity.
        '''
        return self.__storageCapacity

    def __str__(self):
        commonStr = addIndent(super().__str__())

        return "{\n" \
                "%s,\n" \
                "    storage type = %s,\n" \
                "    storage capacity = %d\n" \
                "}" % (
                        commonStr,
                        self.storageType(),
                        self.storageCapacity())


#Regist type
__registStationModule("Storage", "STATION_TYPE_STORAGE", StorageModule)

__initialize()

if __name__ == '__main__':
    '''
        Test.
    '''
    testSetLocale()
    print("[")
    for s in __STATION_MODULES:
        print(addIndent(str(__STATION_MODULES[s]) + ","))

    print("]")
