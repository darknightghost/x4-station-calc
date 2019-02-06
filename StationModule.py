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

import Common
from Common import *

__STATION_MODULES = {}
__TYPE_NAME_ID = {}


class StationModule:
    '''
        Base class of all station modules.
    '''

    @TypeChecker(object, dict)
    def __init__(self, data):
        self.__id = data["id"]
        self.__type = data["type"]

        if self.__type not in __TYPE_NAME_ID.keys():
            raise KeyError(
                "Unknow station-module-type \"%s\"." % (self.__type))

        self.__factions = data["factions"].copy()

        for f in self.__factions:
            Faction.faction(f)

        self.__turretNum = int(data["turret"])
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

    def faction(self):
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
