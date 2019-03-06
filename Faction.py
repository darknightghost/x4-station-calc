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

import StringTable
import Common
from Common import *

__FACTIONS = {}


def __initialize():
    factionsDir = pathlib.Path(__file__).parent / "factions"
    for fname in factionsDir.glob("*.json"):
        with open(str(fname)) as f:
            data = json.loads(f.read(), encoding="utf-8")
            faction = Faction(data)
            __FACTIONS[faction.id()] = faction


@TypeChecker(str)
def faction(id):
    '''
        Get faction object.
    '''
    return __FACTIONS[id]


def factions():
    '''
        Get all factions.
    '''
    ret = []
    for f in __FACTIONS:
        ret.append(__FACTIONS[f])

    return ret


class Faction:
    '''
        Faction.
    '''

    @TypeChecker(object, dict)
    def __init__(self, data):
        self.__id = data["id"]
        self.__name = data["name"].copy()

    def id(self):
        '''
            Get Faction id
        '''
        return self.__id

    def name(self):
        '''
            Get faction name.
        '''
        try:
            return self.__name[StringTable.locale()]

        except KeyError:
            return self.__name[StringTable.defaultLocale()]

    def __str__(self):
        return "{\n" \
                "    id = %s,\n" \
                "    name = %s\n" \
                "}"%(self.id(), self.name())

    def __hash__(self):
        return self.id().__hash__()

    def __eq__(self, f):
        return self.id() == f.id()


__initialize()

if __name__ == '__main__':
    '''
        Test.
    '''
    testSetLocale()
    print("[")
    for f in __FACTIONS:
        print(addIndent(str(__FACTIONS[f]) + ","))

    print("]")
