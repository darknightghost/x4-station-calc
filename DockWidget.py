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


class QActionAttachWidget(QAction):
    '''
        QAction used to control show or hide of a QDockWidgetAttachAction.
    '''

    @TypeChecker(QAction, str, bool)
    def __init__(self, title, checked):
        super().__init__(title)
        self.__attachedWidget = None
        self.toggled.connect(self.onToggle)
        self.setCheckable(True)
        self.setChecked(checked)

    @TypeChecker(QAction, QDockWidget)
    def attach(self, widget):
        '''
            Attach to a widget.
        '''
        self.__attachedWidget = widget
        self.__attachedWidget.onAttach(self)
        self.__attachedWidget.setVisible(self.isChecked())
        self.__attachedWidget.setEnabled(self.isEnabled())

    def deattach(self):
        '''
            Deattach.
        '''
        if self.__attachedWidget != None:
            self.__attachedWidget.onDeattach(self)
            self.__attachedWidget = None

    @TypeChecker(QAction, bool)
    def onToggle(self, checked):
        if self.__attachedWidget != None:
            if self.__attachedWidget.isVisible() != checked:
                self.__attachedWidget.setVisible(checked)


class QDockWidgetAttachAction(QDockWidget):
    '''
        QDockWidget whose visibility is controled by a QActionAttachWidget.
    '''

    @TypeChecker(QDockWidget, QWidget, (QWidget, type(None)))
    def __init__(self, widget, parent=None):
        super().__init__(parent)
        self.__attachedAction = None
        self.__alive = True
        self.setWidget(widget)

    @TypeChecker(QDockWidget, QActionAttachWidget)
    def onAttach(self, action):
        self.__attachedAction = action

    def onDeattach(self, action):
        self.__attachedAction = None

    @TypeChecker(QDockWidget)
    def __visibilityChanged(self):
        if self.__attachedAction != None:
            if self.isVisible() != self.__attachedAction.isChecked():
                self.__attachedAction.setChecked(self.isVisible())

    def die(self):
        self.__alive = False

    @TypeChecker(QDockWidget, QShowEvent)
    def showEvent(self, event):
        super().showEvent(event)
        self.__visibilityChanged()

    @TypeChecker(QDockWidget, QHideEvent)
    def hideEvent(self, event):
        super().hideEvent(event)
        self.__visibilityChanged()

    @TypeChecker(QDockWidget, QCloseEvent)
    def closeEvent(self, event):
        if self.__alive:
            event.ignore()
            self.setVisible(False)

        else:
            if self.__attachedAction != None:
                self.__attachedAction.deattach()
            super().closeEvent(event)
