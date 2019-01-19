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

__FACTIONS = {}


def __initialize():
    factions_dir = pathlib.Path(__file__).parent / "factions"
    for fname in factions_dir.glob("*.json"):
        with open(str(fname)) as f:
            data = json.loads(f.read())
            faction = Faction(data)
            __FACTIONS[faction.id()] = faction


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
            return self.__name[StringTable.default_locale()]

    def __str__(self):
        return "{\n" \
                "    id = %s,\n" \
                "    name = %s\n" \
                "}"%(self.id(), self.name())


__initialize()

if __name__ == '__main__':
    print("[")
    for f in __FACTIONS:
        for l in (str(__FACTIONS[f]) + ",").split("\n"):
            print("    %s" % (l.replace("\n", "")))

    print("]")