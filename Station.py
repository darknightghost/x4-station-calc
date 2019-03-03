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

import PyQt5
from PyQt5.QtCore import *
from PyQt5.QtGui import *
from PyQt5.QtWidgets import *

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


class StationModules(QObject):
    '''
        Station modules in group.
    '''
    #amountChanged(stationModules, oldNum, newNum)
    amountChanged = pyqtSignal(QObject, int, int)

    def __init__(self, data, amount=1, parent=None):
        '''
            StationModules(data)       -> stationModules
            StationModules(id, amount) -> stationModules
        '''
        super().__init__(parent)
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
        super().setParent(parent)
        self.__setParentDirty()

    def id(self):
        '''
            Station module id.
        '''
        return self.__stationModule.id()

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
        self.setAmount(self.__amount + 1)

    def decrease(self):
        '''
            Decrease amount.
        '''
        self.setAmount(self.__amount - 1)

    @TypeChecker(object, int)
    def setAmount(self, amount):
        '''
            Set amount.
        '''
        if amount > 0 and amount != self.__amount:
            oldAmount = self.__amount
            self.__amount = amount
            self.__setParentDirty()
            self.amountChanged.emit(self, oldAmount, amount)

        else:
            raise ValueError("Amount must bigger than 0.")

    def toDict(self):
        '''
            Get json dict.
        '''
        return {"id": self.__stationModule.id(), "amount": self.__amount}

    def json(self):
        '''
            Pack to json.
        '''
        return json.dumps(self.toDict(), ensure_ascii=False)

    def copy(self):
        '''
            Copy a new instance.
        '''
        return type(self)(self.toDict())

    def __setParentDirty(self):
        '''
            Make parent dirty.
        '''
        if self.parent() != None:
            self.parent().setDirty()

    def __hash__(self):
        return self.stationModule().__hash__()

    def __iadd__(self, item):
        if isinstance(item, int):
            if item < 0:
                raise ValueError("\"item\" must bigger than 0.")
            self.setAmount(self.__amount + item)

        elif isinstance(self, StationModules):
            self.setAmount(self.__amount + item.amount())

        else:
            raise TypeError("\"item\" must be StationModules or int.")

        return self


class StationModulesGroup(QObject):
    '''
        Station module group.
    '''
    #addModules(group, modules)
    addModules = pyqtSignal(QObject, StationModules)
    #removeModules(group, modules)
    removeModules = pyqtSignal(QObject, StationModules)
    #nameChanged(group)
    nameChanged = pyqtSignal(QObject)

    def __init__(self, data=None, parent=None):
        '''
            StationModulesGroup(data)       -> stationModulesGroup
            StationModulesGroup()           -> stationModulesGroup
        '''
        super().__init__(parent)
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
                self.__stationModuleIndex[m.stationModule().id()] += m

            except KeyError:
                self.__stationModules.append(m)
                self.__stationModuleIndex[m.stationModule().id()] = m

    def __initWithoutData(self):
        self.__name = StringTable.getString("NEW_GROUP_NAME")
        self.__stationModules = []
        self.__stationModuleIndex = {}

    def setParent(self, parent):
        '''
            Set parent.
        '''
        super().setParent(parent)
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
        self.nameChanged.emit(self)

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

    def json(self):
        '''
            Pack to json.
        '''
        return json.dumps(self.toDict(), ensure_ascii=False)

    def __setParentDirty(self):
        '''
            Make parent dirty.
        '''
        if self.parent() != None:
            self.parent().setDirty()

    @TypeChecker(object, StationModules)
    def append(self, item):
        '''
            Append item.
        '''
        try:
            self.__stationModuleIndex[item.stationModule().id()] += item

        except KeyError:
            self.__stationModules.append(item)
            self.__stationModuleIndex[item.stationModule().id()] = item
            item.setParent(self)
            self.__setParentDirty()
            self.addModules.emit(self, item)

    @TypeChecker(object, StationModules)
    def remove(self, item):
        '''
            Remove item.
        '''
        self.__stationModules.remove(item)
        del self.__stationModuleIndex[item.stationModule().id()]
        item.setParent(None)
        self.__setParentDirty()
        self.removeModules.emit(self, item)

    def copy(self):
        '''
            Copy a new instance.
        '''
        return type(self)(self.toDict())

    @TypeChecker(object, (StationModules, StationModule.StationModule))
    def __contains__(self, station):
        return station.id() in self.__stationModuleIndex

    def __iter__(self):
        return self.__stationModules.__iter__()

    @TypeChecker(object, (int, StationModule.StationModule))
    def __getitem__(self, index):
        if isinstance(index, int):
            return self.__stationModules[index]

        else:
            return self.__stationModuleIndex[index.id()]

    def __delitem__(self, index):
        self.remove(self.__stationModules[index])

    def __iadd__(self, l):
        for m in l:
            if not isinstance(m, StationModules):
                raise TypeError(
                    "Type of item should be %s." % (str(StationModules)))
            m.setParent(self)
            self.append(m)


class Station(QObject):
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
    EXT_NAME = ".x4station"
    #addGroup(station, group)
    addGroup = pyqtSignal(QObject, StationModulesGroup)
    #removeGroup(station, group)
    removeGroup = pyqtSignal(QObject, StationModulesGroup)

    @TypeChecker(object, (str, type(None)))
    def __init__(self, path=None):
        super().__init__()
        #Path
        self.__path = path

        if path == None:
            #New file
            self.__stationModulesGroups = [StationModulesGroup()]
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
                for g in data["groups"]:
                    self.__stationModulesGroups.append(
                        StationModulesGroup(g, parent=self))

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
        if len(path) < len(
                self.EXT_NAME) or path[-len(self.EXT_NAME):] != self.EXT_NAME:
            path += self.EXT_NAME
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
        self.setDirty()
        self.addGroup.emit(self, item)

    @TypeChecker(object, StationModulesGroup)
    def remove(self, item):
        '''
            Remove item.
        '''
        self.__stationModulesGroups.remove(item)
        item.setParent(None)
        self.setDirty()
        self.removeGroup.emit(self, item)

    def __iter__(self):
        return self.__stationModulesGroups.__iter__()

    @TypeChecker(object, int)
    def __getitem__(self, index):
        return self.__stationModulesGroups[index]

    def __delitem__(self, index):
        self.remove(self.__stationModulesGroups[index])

    def __iadd__(self, l):
        for m in l:
            if not isinstance(m, StationModulesGroup):
                raise TypeError(
                    "Type of item should be %s." % (str(StationModulesGroup)))
            self.append(m)
