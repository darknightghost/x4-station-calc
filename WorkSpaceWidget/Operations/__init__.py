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

import Station
import StationModule

import WorkSpaceWidget

import WorkSpaceWidget.Operations


class Operation:
    '''
        Base class of station operations.
    '''

    def __init__(self):
        self._workSpace = None

    def setWorkspace(self, workspace):
        '''
            Set workspace widget.
        '''
        self._workSpace = workspace
        return self.onSetWorkspace()

    def onSetWorkspace(self):
        '''
            Set workspace widget.
        '''
        return True

    def do(self):
        '''
            Do operation, return True if success.
        '''
        if self._workSpace == None:
            return False

        else:
            if self.onDo():
                return True

            else:
                return False

    def onDo(self):
        '''
            Do operation, return True if success.
        '''
        raise NotImplementedError()

    def undo(self):
        '''
            Undo operation.
        '''
        if self._workSpace == None:
            return False

        else:
            if self.onUndo():
                return True

            else:
                return False

    def onUndo(self):
        '''
            Undo operation.
        '''
        raise NotImplementedError()


from WorkSpaceWidget.Operations.NewGroupOperation import *
from WorkSpaceWidget.Operations.ChangeGroupNameOperation import *
from WorkSpaceWidget.Operations.AddModuleOperation import *
from WorkSpaceWidget.Operations.ChangeModuleAmountOperation import *
from WorkSpaceWidget.Operations.SwapModuleOperation import *
from WorkSpaceWidget.Operations.RemoveOperation import *
from WorkSpaceWidget.Operations.PasteOperation import *
