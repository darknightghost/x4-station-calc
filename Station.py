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
import os
from distutils.version import StrictVersion

import StationModule
import Station
import Common
from Common import *


class StationFileException(Exception):
    '''
        Base exception class.
    '''
    pass


class StationParseException(StationFileException):
    '''
        Exception when failed to parse station file.
    '''

    @TypeChecker(Exception, str)
    def __init__(self, path, reason):
        super().__init__(
            StringTable.getString("FAILED_PARSE_STATION_FILE") % (path,
                                                                  reason))


class StationFileReadException(StationFileException):
    '''
        Exception when failed to read station file.
    '''

    @TypeChecker(Exception, str)
    def __init__(self, path):
        super().__init__(
            StringTable.getString("FAILED_READ_STATION_FILE") % (path))


class StationFileWriteException(StationFileException):
    '''
        Exception when failed to write station file.
    '''

    @TypeChecker(Exception, str)
    def __init__(self, path):
        super().__init__(
            StringTable.getString("FAILED_WRITE_STATION_FILE") % (path))


class StationModules:
    '''
        Station modules in group.
    '''

    def __init__(self, data, amount=1, parent=None):
        '''
            StationModules(data)       -> stationModules
            StationModules(id, amount) -> stationModules
        '''
        self.__parent = parent
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

    def setParent(self, parent):
        '''
            Set parent.
        '''
        self.__parent = parent
        self.__setParentDirty()

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
        self.__setParentDirty()

    def decrease(self):
        '''
            Decrease amount.
        '''
        if self.__amount > 1:
            self.__amount -= 1
            self.__setParentDirty()

    @TypeChecker(object, int)
    def setAmount(self, amount):
        '''
            Set amount.
        '''
        if amount > 0:
            self.__amount = amount
            self.__setParentDirty()

        else:
            raise ValueError("Amount must bigger than 0.")

    def toDict(self):
        '''
            Get json dict.
        '''
        return {"id": self.__stationModule.id(), "amount": self.__amount}

    def __setParentDirty(self):
        '''
            Make parent dirty.
        '''
        if self.__parent != None:
            self.__parent.setDirty()

    def __hash__(self):
        return self.stationModule().__hash__()

    def __iadd__(self, item):
        if isinstance(item, int):
            if item < 0:
                raise ValueError("\"item\" must bigger than 0.")
            self.__amount += item
            self.__setParentDirty()

        elif isinstance(self, StationModules):
            self.__amount += item.amount()
            self.__setParentDirty()

        else:
            raise TypeError("\"item\" must be StationModules or int.")


class StationModulesGroup:
    '''
        Station module group.
    '''

    def __init__(self, data, parent=None):
        '''
            StationModulesGroup(data)       -> stationModulesGroup
            StationModulesGroup()           -> stationModulesGroup
        '''
        self.__parent = parent
        if isinstance(data, dict):
            self.__initByData(data)

        else:
            self.__initWithoutData()

    @TypeChecker(object, dict)
    def __initByData(self, data):
        self.__name = data["name"]
        self.__stationModules = []
        self.__stationModuleIndex = {}
        for m in data["modules"]:
            m = StationModules(m, parent=self)
            try:
                self.__stationModuleIndex[m.stationModule] += m

            except KeyError:
                self.__stationModules.append(m)
                self.__stationModuleIndex[m.stationModule()] = m

    def __initWithoutData(self):
        self.__name = StringTable.getString("NEW_GROUP_NAME")
        self.__stationModules = []
        self.__stationModuleIndex = {}

    def setParent(self, parent):
        '''
            Set parent.
        '''
        self.__parent = parent
        self.__setParentDirty()

    def name(self):
        '''
            Get name.
        '''
        return self.__name

    @TypeChecker(object, str)
    def setName(self, name):
        '''
            Set name.
        '''
        self.__name = name
        self.__setParentDirty()

    def setDirty(self):
        '''
            Make file dirty.
        '''
        self.__setParentDirty()

    def toDict(self):
        '''
            To dictionary.
        '''
        modules = []
        for m in self.__stationModules:
            modules.append(m.toDict())

        return {"name": self.name(), "modules": modules}

    def __setParentDirty(self):
        '''
            Make parent dirty.
        '''
        if self.__parent != None:
            self.__parent.setDirty()

    @TypeChecker(object, StationModules)
    def append(self, item):
        '''
            Append item.
        '''
        item.setParent(self)
        try:
            self.__stationModuleIndex[item.stationModule()] += item

        except KeyError:
            self.__stationModules.append(item)
            self.__setParentDirty()

    @TypeChecker(object, StationModules)
    def remove(self, item):
        '''
            Remove item.
        '''
        item.setParent(None)
        self.__stationModules.remove(item)
        self.__setParentDirty()

    def __iter__(self):
        return self.__stationModules.__iter__()

    @TypeChecker(object, int)
    def __getitem__(self, index):
        return self.__stationModules[index]

    @TypeChecker(object, int, StationModules)
    def __setitem__(self, index, value):
        value.setParent(self)
        self.__stationModules[index] = value

    def __delitem__(self, index):
        self.__stationModules[index].setParent(None)
        del self.__stationModules[index]
        self.__setParentDirty()

    def __iadd__(self, l):
        for m in l:
            if not isinstance(m, StationModules):
                raise TypeError(
                    "Type of item should be %s." % (str(StationModules)))
            m.setParent(self)
            self.__stationModules.append(m)


class Station:
    '''
        Station file.

        File format:
        {
            "groups" : [
                {
                    "name" : "group1",
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
                },
                {
                    "name" : "group2",
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
                },
                {
                    "name" : "group3",
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
            self.__stationModulesGroups = []
            self.__dirty = True

        else:
            #Load file
            self.__dirty = False
            try:
                f = open(path)
                s = f.read()

            except Exception:
                raise StationFileReadException(path)

            f.close()

            #Parse json
            try:
                data = json.loads(s)

            except json.decoder.JSONDecodeError:
                raise StationParseException(
                    path, StringTable.getString("ILLEGAL_JSON_FORMAT"))

            #Check version
            try:
                version = StrictVersion(data["version"])

            except Exception:
                raise StationParseException(path, traceback.format_exc())

            if version > VERSION:
                raise StationParseException(
                    path,
                    StringTable.getString("VERSION_TOO_LOW") % (str(version)))

            #Load data
            self.__stationModulesGroups = []
            try:
                self.__stationModulesGroups = []
                for d in data["groups"]:
                    self.__stationModulesGroups.append(StationModules(d))

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
        if self.__path == None:
            return StringTable.getString("NEW_FILE_NAME")

        else:
            return os.path.splitext(os.path.basename(self.__path))[0]

    def isDirty(self):
        '''
            Check if the file is dirty.
        '''
        return self.__dirty

    def setDirty(self):
        '''
            Make file dirty.
        '''
        self.__dirty = True

    def save(self):
        '''
            Save file.
        '''
        #Make json
        modules = []
        for m in self.__stationModulesGroups:
            modules.append(m.toDict())

        stationDict = {"version": str(RECORD_VERSION), "groups": modules}
        s = json.dumps(stationDict, ensure_ascii=False)

        #Write json
        try:
            f = open(self.__path, "w")
            f.write(s)
            f.close()
        except Exception:
            raise StationFileWriteException(self.__path)

        self.__dirty = False

    @TypeChecker(object, StationModulesGroup)
    def append(self, item):
        '''
            Append item.
        '''
        item.setParent(self)
        self.__stationModulesGroups.append(item)

    @TypeChecker(object, StationModulesGroup)
    def remove(self, item):
        '''
            Remove item.
        '''
        item.setParent(None)
        self.__stationModulesGroups.remove(item)

    def __iter__(self):
        return self.__stationModulesGroups.__iter__()

    @TypeChecker(object, int)
    def __getitem__(self, index):
        return self.__stationModulesGroups[index]

    @TypeChecker(object, int, StationModulesGroup)
    def __setitem__(self, index, value):
        value.setParent(self)
        self.__stationModulesGroups[index] = value

    def __delitem__(self, index):
        self.__stationModulesGroups[index].setParent(None)
        del self.__stationModulesGroups[index]
        self.setDirty()

    def __iadd__(self, l):
        for m in l:
            if not isinstance(m, StationModulesGroup):
                raise TypeError(
                    "Type of item should be %s." % (str(StationModulesGroup)))
            m.setParent(self)
            self.__stationModulesGroups.append(m)
            self.setDirty()
