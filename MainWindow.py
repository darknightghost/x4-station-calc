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
import PyQt5.QtCore
import PyQt5.QtGui
from PyQt5.QtCore import QEvent
from PyQt5.QtGui import QKeySequence
from PyQt5.QtWidgets import QShortcut
from PyQt5.QtWidgets import QMessageBox
from PyQt5.QtWidgets import QApplication, QWidget, QMainWindow, QDockWidget
from PyQt5.QtWidgets import QMenu, QAction, QActionGroup
from PyQt5.QtWidgets import QFileDialog

import pathlib
import json
import locale

import StringTable
import Station
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
        self.changed.connect(self.onChanged)
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

    @TypeChecker(QAction, bool)
    def onToggle(self, checked):
        if self.__attachedWidget != None:
            if self.__attachedWidget.isVisible() != checked:
                self.__attachedWidget.setVisible(checked)

    def onChanged(self):
        '''
            Set enable status.
        '''
        if self.__attachedWidget != None:
            if self.__attachedWidget.isEnabled() != self.isEnabled():
                self.__attachedWidget.setEnabled(self.isEnabled())


class QDockWidgetAttachAction(QDockWidget):
    '''
        QDockWidget whose visibility is controled by a QActionAttachWidget.
    '''

    def __init__(self, parent):
        self.__attachedAction = None

    @TypeChecker(QDockWidget, bool)
    def visibilityChanged(self, visible):
        if self.__attachedAction != None:
            if visibility != self.__attachedAction.isChecked():
                self.__attachedAction.setChecked(visibility)

    @TypeChecker(QDockWidget, QEvent)
    def changeEvent(self, event):
        if event.type() == QEvent.EnabledChange:
            if self.__attachedAction != None:
                if self.__attachedAction.isEnabled() != self.isEnabled():
                    self.__attachedAction.setEnabled(self.isEnabled())

        super().changeEvent(event)


class MainWindow(QMainWindow):
    CONFIG_PATH = pathlib.Path(__file__).parent / ".config"

    @TypeChecker(QMainWindow, QWidget)
    def __init__(self, parent=None):
        super().__init__(parent)
        self.__loadWindow()
        self.__station = None

        #Language
        try:
            StringTable.setLocale(self.__config["locale"])

        except KeyError:
            sys_locale = locale.getdefaultlocale()[0]
            try:
                StringTable.setLocale(sys_locale)
            except KeyError:
                pass

            self.__config["locale"] = StringTable.locale()

        self.setWindowTitle(StringTable.getString("TITLE_MAIN_WINDOW"))
        desktop = QApplication.desktop()
        try:
            self.resize(self.__config["width"], self.__config["height"])

        except KeyError:
            self.resize(desktop.width() * 3 / 4, desktop.height() * 3 / 4)

        try:
            self.move(self.__config["x"], self.__config["y"])

        except KeyError:
            self.move(desktop.width() / 8, desktop.height() / 8)

        self.__initFileMenu()
        self.__initSettingMenu()
        self.__initViewMenu()

    #Initialize menus
    def __initFileMenu(self):
        self.__fileMenu = QMenu(StringTable.getString("MENU_FILE"))
        self.menuBar().addMenu(self.__fileMenu)
        self.__fileNewAction = QAction(StringTable.getString("MENU_FILE_NEW"))
        self.__fileNewAction.setShortcut(QKeySequence.New)
        self.__fileNewAction.triggered.connect(self.onMenuFileNew)
        self.__fileMenu.addAction(self.__fileNewAction)

        self.__fileOpenAction = QAction(
            StringTable.getString("MENU_FILE_OPEN"))
        self.__fileOpenAction.setShortcut(QKeySequence.Open)
        self.__fileOpenAction.triggered.connect(self.onMenuFileOpen)
        self.__fileMenu.addAction(self.__fileOpenAction)

        self.__fileMenu.addSeparator()

        self.__fileSaveAction = QAction(
            StringTable.getString("MENU_FILE_SAVE"))
        self.__fileSaveAction.setShortcut(QKeySequence.Save)
        self.__fileSaveAction.triggered.connect(self.onMenuFileSave)
        self.__fileSaveAction.setEnabled(False)
        self.__fileMenu.addAction(self.__fileSaveAction)

        self.__fileSaveAsAction = QAction(
            StringTable.getString("MENU_FILE_SAVE_AS"))
        self.__fileSaveAsAction.setShortcut(QKeySequence.SaveAs)
        self.__fileSaveAsAction.triggered.connect(self.onMenuFileSaveAs)
        self.__fileSaveAsAction.setEnabled(False)
        self.__fileMenu.addAction(self.__fileSaveAsAction)

        self.__fileMenu.addSeparator()

        self.__fileCloseAction = QAction(
            StringTable.getString("MENU_FILE_CLOSE"))
        self.__fileCloseAction.setShortcut(QKeySequence.Close)
        self.__fileCloseAction.triggered.connect(self.onMenuFileClose)
        self.__fileMenu.addAction(self.__fileCloseAction)

        self.__fileMenu.addSeparator()

        self.__fileExitAction = QAction(
            StringTable.getString("MENU_FILE_EXIT"))
        self.__fileExitAction.setShortcut(QKeySequence.Quit)
        self.__fileExitAction.triggered.connect(self.onMenuFileExit)
        self.__fileCloseAction.setEnabled(False)
        self.__fileMenu.addAction(self.__fileExitAction)

    def __initSettingMenu(self):
        self.__settingMenu = QMenu(StringTable.getString("MENU_SETTING"))
        self.menuBar().addMenu(self.__settingMenu)
        self.__settingLanguageMenu = QMenu(
            StringTable.getString("MENU_SETTING_LANGUAGE"))
        self.__settingMenu.addMenu(self.__settingLanguageMenu)

        #Languages
        self.__languageActionGroup = QActionGroup(self)
        languages = StringTable.localeDict()
        for l in languages.keys():
            action = QAction(languages[l], self.__languageActionGroup)
            setattr(action, "locale", l)
            action.setCheckable(True)
            action.setChecked(l == StringTable.locale())
            self.__settingLanguageMenu.addAction(action)

        self.__languageActionGroup.triggered[QAction].connect(
            self.onChangeLocale)

    def __initViewMenu(self):
        self.__viewMenu = QMenu(StringTable.getString("MENU_VIEW"))
        self.menuBar().addMenu(self.__viewMenu)

        self.__viewModuleListMenu = QActionAttachWidget(
            StringTable.getString("MENU_VIEW_MODULE_LIST"), False)
        self.__viewModuleListMenu.setEnabled(False)
        self.__viewMenu.addAction(self.__viewModuleListMenu)

        self.__viewModuleInfoMenu = QActionAttachWidget(
            StringTable.getString("MENU_VIEW_MODULE_INFO"), False)
        self.__viewModuleInfoMenu.setEnabled(False)
        self.__viewMenu.addAction(self.__viewModuleInfoMenu)

    def __saveWindow(self):
        self.__config["width"] = self.width()
        self.__config["height"] = self.height()
        self.__config["x"] = self.x()
        self.__config["y"] = self.y()
        s = json.dumps(self.__config)
        with open(str(self.CONFIG_PATH), "w") as f:
            f.write(s)

    def __loadWindow(self):
        if self.CONFIG_PATH.exists():
            with open(str(self.CONFIG_PATH)) as f:
                self.__config = json.loads(f.read())

        else:
            self.__config = {}

    #Slots
    @TypeChecker(QMainWindow, QAction)
    def onChangeLocale(self, action):
        self.__config["locale"] = action.locale
        QMessageBox.information(
            self, StringTable.getString("TITLE_INFO"),
            StringTable.getString("INFO_EFFECT_NEXT_LAUNCH"))

    def onMenuFileNew(self, state):
        #Close opened station
        if self.__station != None:
            if not self.closeStation():
                return

        self.openStation(None)

    def onMenuFileOpen(self, state):
        #Close opened station
        if self.__station != None:
            if not self.closeStation():
                return

        #Open
        try:
            open_path = self.__config["openPath"]

        except KeyError:
            open_path = "."

        filename, file_type = QFileDialog.getOpenFileName(
            self, StringTable.getString("TITLE_OPEN_FILE"), open_path,
            StringTable.getString("TYPE_FILE"))

        if file_type != "":
            self.__config["openPath"] = str(
                pathlib.Path(filename).absolute().parent)
            self.openStation(filename)

    def onMenuFileSave(self, state):
        if self.__station.path() == None:
            self.onMenuFileSaveAs(state)

        else:
            self.saveStation()

    def onMenuFileSaveAs(self, state):
        try:
            save_path = self.__config["openPath"]

        except KeyError:
            save_path = "."

        filename, file_type = QFileDialog.getSaveFileName(
            self, StringTable.getString("TITLE_SAVE_FILE"), save_path,
            StringTable.getString("TYPE_FILE"))

        if file_type != "":
            self.__config["openPath"] = str(
                pathlib.Path(filename).absolute().parent)
            self.__station.setPath(filename)
            self.saveStation()

    def onMenuFileClose(self, state):
        self.closeStation()

    def onMenuFileExit(self, state):
        self.close()

    #Events
    def closeEvent(self, event):
        if self.__station != None:
            if not self.closeStation():
                event.ignore()
                return

        self.__saveWindow()
        super().closeEvent(event)

    #Methods
    @TypeChecker(QMainWindow, (str, type(None)))
    def openStation(self, path):
        #Open new station
        try:
            self.__station = Station.Station(path)
        except Station.StationFileException as e:
            QMessageBox.critical(self, StringTable.getString("TITLE_ERROR"),
                                 str(e))

        #Enable menus
        self.__fileSaveAction.setEnabled(True)
        self.__fileSaveAsAction.setEnabled(True)
        self.__fileCloseAction.setEnabled(True)

        #Set title
        self.setWindowTitle(
            StringTable.getString("TITLE_MAIN_WINDOW_OPENED") %
            (self.__station.name()))

    def saveStation(self):
        try:
            self.__station.save()

        except Station.StationFileException as e:
            QMessageBox.critical(self, StringTable.getString("TITLE_ERROR"),
                                 str(e))

    def closeStation(self):
        if self.__station == None:
            return True

        if self.__station.isDirty():
            #Ask to save file.
            result = QMessageBox.question(
                self, StringTable.getString("TITLE_INFO"),
                StringTable.getString("FILE_NOT_SAVE"),
                QMessageBox.Yes | QMessageBox.No | QMessageBox.Cancel)

            if result == QMessageBox.Yes:
                #Save file
                self.onMenuFileSave(None)
                if self.__station.isDirty():
                    return False

            elif result != QMessageBox.No:
                return False

        #Close
        del self.__station
        self.__station = None

        #Disable menu
        self.__fileSaveAction.setEnabled(False)
        self.__fileSaveAsAction.setEnabled(False)
        self.__fileCloseAction.setEnabled(False)

        #Set title
        self.setWindowTitle(StringTable.getString("TITLE_MAIN_WINDOW"))

        return True
