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

import WorkSpaceWidget


class Operation:
    '''
        Base class of station operations.
    '''

    def __init__(self):
        self.__widget = None

    @TypeChecker(object, WorkSpaceWidget.WorkSpaceWidget)
    def setWorkspace(self, workspace):
        '''
            Set workspace widget.
        '''
        self.__workSpace = workspace
        self.onSetWorkspace()

    def onSetWorkspace(self):
        '''
            Called when workspace changed.
        '''
        raise NotImplementedError()

    def do(self):
        '''
            Do operation, return True if success.
        '''
        raise NotImplementedError()

    def undo(self):
        '''
            Undo operation.
        '''
        raise NotImplementedError()


class AddModuleOperation(Operation):
    pass
