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
    Station file.
'''

import pathlib
import json
import traceback

import StationModule
import Station
import Common
from Common import *


class StationParseException(Exception):
    '''
        Exception when failed to parse station file.
    '''

    @TypeChecker(Exception, str)
    def __init__(self, path, reason):
        super().__init__(
            StringTable.getString("FAILED_PARSE_STATION_FILE") % (path,
                                                                  reason))


class StationFileReadException(Exception):
    '''
        Exception when failed to read station file.
    '''

    @TypeChecker(Exception, str)
    def __init__(self, path):
        super().__init__(
            StringTable.getString("FAILED_READ_STATION_FILE") % (path))


class StationFileWriteException(Exception):
    '''
        Exception when failed to write station file.
    '''

    @TypeChecker(Exception, str)
    def __init__(self, path):
        super().__init__(
            StringTable.getString("FAILED_WRITE_STATION_FILE") % (path))


class StationModules:
    '''
        Modules in station.
    '''

    def __init__(self, data, amount=1):
        '''
            StationModules(data)       -> stationModules
            StationModules(id, amount) -> stationModules
        '''
        if isinstance(data, dict):
            self.__initByData(data)

        else:
            self.__initById(data, amount)

    @TypeChecker(object, dict)
    def __initByData(self, data):
        self.__stationModule = StationModule.stationModule(data["id"])
        self.__amount = data["amount"]
        if self.__amount < 0:
            raise ValueError("Amount must bigger than 0.")

    @TypeChecker(object, str, int)
    def __initById(self, sid, amount=1):
        if amount < 0:
            raise ValueError("Amount must bigger than 0.")

        self.__stationModule = StationModule.stationModule(sid)
        self.__amount = amount

    def stationModule(self):
        '''
            Get station module.
        '''
        return self.__stationModule

    def amount(self):
        '''
            Get amount.
        '''
        return self.__amount

    def increase(self):
        '''
            Increase amount.
        '''
        self.__amount += 1

    def decrease(self):
        '''
            Decrease amount.
        '''
        if self.__amount > 1:
            self.__amount -= 1

    @TypeChecker(object, int)
    def setAmount(self, amount):
        '''
            Set amount.
        '''
        if amount > 0:
            self.__amount = amount

        else:
            raise ValueError("Amount must bigger than 0.")

    def toDict(self):
        '''
            Get json dict.
        '''
        return {"id": self.__stationModule.id(), "amount": self.__amount}


class Station:
    '''
        Station file.

        File format:
        {
            "name" : "station name",
            "modules" : [
                {
                    "id": "MODULE_ID_1",
                    "amount" : 4
                },
                {
                    "id": "MODULE_ID_2",
                    "amount" : 2
                },
                {
                    "id": "MODULE_ID_3",
                    "amount" : 7
                },
                ...
            ]
        }
    '''

    @TypeChecker(object, (str, type(None)))
    def __init__(self, path=None):
        #Path
        self.__path = path

        if path == None:
            #New file
            self.__name = StringTable.getString("NEW_FILE_NAME")
            self.__stationModules = []

        else:
            #Load file
            try:
                f = open(path)
                s = f.read()

            except Exception:
                raise StationFileReadException(path)

            f.close()

            try:
                data = json.loads(s)

            except json.decoder.JSONDecodeError:
                raise StationParseException(
                    path, StringTable.getString("ILLEGAL_JSON_FORMAT"))

            self.__stationModules = []
            try:
                self.__name = data["name"]
                self.__stationModules = []
                for d in data["modules"]:
                    self.__stationModules.append(StationModules(d))

            except Exception:
                raise StationParseException(path, traceback.format_exc())

        def path(self):
            '''
                Get path.
            '''
            return self.__path

        @TypeChecker(object, str)
        def setPath(self, path):
            '''
                Set path.
            '''
            self.__path = path

        def name(self):
            '''
                Get station name.
            '''
            return self.__name

        @TypeChecker(object, str)
        def setName(self, name):
            '''
                Set station name.
            '''
            self.__name = name

        def stationModules(self):
            '''
                Get station modules.
            '''
            return self.__stationModules

        def save(self):
            '''
                Save file.
            '''
            #Make json
            modules = []
            for m in self.__stationModules:
                modules.append(m.toDict())

            s = json.dumps({
                "name": self.__name,
                "modules": modules
            },
                           ensure_ascii=False)

            #Write json
            try:
                f = open(self.__path, "w")
                f.write(s)
                f.close()
            except Exception:
                raise StationFileWriteException(self.__path)

        def __iter__(self):
            return self.__stationModules.__iter__()

        @TypeChecker(object, int)
        def __getitem__(self, index):
            return self.__stationModules.__getitem__(index)

        @TypeChecker(object, int, StationModules)
        def __setitem__(self, index, value):
            self.__stationModules.__setitem__(index, value)

        def __delitem__(self, index):
            self.__stationModules.__delitem__(index)
