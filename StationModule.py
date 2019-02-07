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
    "Storage":  ["STATION_TYPE_STORAGE", None],
    "Defence": ["STATION_TYPE_DEFENCE", None],
    "Habitation": ["STATION_TYPE_HABITATION", None],
    "Dock": ["STATION_TYPE_DOCK", None],
    "Other": ["STATION_TYPE_OTHER", None],
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


class ProductionModule(__StationModule):
    '''
        Production modules.
    '''

    class ProductInfo:
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

    @TypeChecker(object, dict)
    def __init__(self, data):
        super().__init__(data)
        self.__products = []
        for p in data["products"]:
            self.__products.append(self.ProductInfo(p))

        self.__resources = []
        for r in data["resources"]:
            self.__resources.append(self.ProductInfo(r))

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
                "%s,\n" \
                "}" % (
                        commonStr,
                        resourcesStr,
                        productsStr)


#Regist type
__registStationModule("Production", "STATION_TYPE_PRODUCTION",
                      ProductionModule)

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
