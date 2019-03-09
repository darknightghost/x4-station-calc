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
    Factions.
'''

import pathlib
import json
import logging

import StringTable
import InfoWidget
import Common
from Common import *

__PRODUCTS = {}

logger = logging.getLogger()


def __initialize():
    productsDir = pathlib.Path(__file__).parent / "products"
    for pname in productsDir.glob("*.json"):
        logger.debug("Loading data file \"%s\"..." % (str(pname)))
        with open(str(pname), encoding="utf-8") as f:
            data = json.loads(f.read(), encoding="utf-8")
            product = Product(data)
            __PRODUCTS[product.id()] = product


@TypeChecker(str)
def product(id):
    '''
        Get product by id.
    '''
    return __PRODUCTS[id]


def products():
    '''
        Get all products.
    '''
    ret = []
    for id in __PRODUCTS.keys():
        ret.append(__PRODUCTS[id])

    return ret


class Product:
    '''
        Product.
    '''

    @TypeChecker(object, dict)
    def __init__(self, data):
        self.__id = data["id"]
        self.__storage = data["storage"]
        self.__volume = int(data["volume"])
        self.__name = data["name"].copy()

        if self.__storage not in ("Solid", "Container", "Liquid"):
            raise ValueError("Illegale cargo type \"%s\" in product \"%s\"." \
                %(self.__storage, self.name()))

    def id(self):
        '''
            Get Product id
        '''
        return self.__id

    def name(self):
        '''
            Get product name.
        '''
        try:
            return self.__name[StringTable.locale()]

        except KeyError:
            return self.__name[StringTable.defaultLocale()]

    def storage(self):
        '''
            Get storage type.
        '''
        return self.__storage

    def volume(self):
        '''
            Get volume.
        '''
        return self.__volume

    def info(self):
        return [
            InfoWidget.InfoItem(
                StringTable.getString("STR_NAME"), self.name()),
            InfoWidget.InfoItem(
                StringTable.getString("STR_STORAGE_TYPE"), {
                    "Solid": StringTable.getString("STR_SOLID"),
                    "Container": StringTable.getString("STR_CONTAINER"),
                    "Liquid": StringTable.getString("STR_LIQUID")
                }[self.storage()]),
            InfoWidget.InfoItem(
                StringTable.getString("STR_VOLUME"),
                "%d m³" % (self.volume())),
        ]

    def __str__(self):
        return "{\n" \
                "    id = %s,\n" \
                "    name = %s,\n" \
                "    storage = %s,\n" \
                "    volume = %s\n" \
                "}"%(self.id(), self.name(), self.storage(), self.volume())

    def __hash__(self):
        return self.id().__hash__()

    def __eq__(self, p):
        return self.id() == p.id()


__initialize()

if __name__ == '__main__':
    '''
        Test.
    '''
    testSetLocale()
    print("[")
    for p in __PRODUCTS:
        print(addIndent(str(__PRODUCTS[p]) + ","))

    print("]")
