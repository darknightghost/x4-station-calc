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

import pathlib
import json
import locale
import sys

import StringTable
import Station
import Common
from Common import *

import WorkSpaceWidget
import ModuleListWidget
import InfoWidget
import DockWidget
import LicenseDialog


class MainWindow(QMainWindow):
    CONFIG_PATH = pathlib.Path(__file__).parent / ".config"

    @TypeChecker(QMainWindow, (QWidget, type(None)), (str, type(None)))
    def __init__(self, parent=None, stationPath=None):
        super().__init__(parent)
        self.__loadWindow()
        self.__station = None
        self.setCentralWidget(QWidget(self))
        self.centralWidget().setEnabled(False)

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
        self.__initEditMenu()
        self.__initSettingMenu()
        self.__initViewMenu()
        self.__initWidgets()

        #Open file
        if stationPath != None:
            self.openStation(stationPath)

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

    def __initEditMenu(self):
        self.__editMenu = QMenu(StringTable.getString("MENU_EDIT"))
        self.menuBar().addMenu(self.__editMenu)

        self.__editNewGroupAction = QAction(
            StringTable.getString("MENU_EDIT_NEW_GROUP"))
        self.__editMenu.addAction(self.__editNewGroupAction)
        self.__editNewGroupAction.setShortcut(QKeySequence("Ctrl+G"))
        self.__editNewGroupAction.setEnabled(False)

        self.__editMenu.addSeparator()

        self.__editUndoAction = QAction(
            StringTable.getString("MENU_EDIT_UNDO"))
        self.__editMenu.addAction(self.__editUndoAction)
        self.__editUndoAction.setShortcut(QKeySequence.Undo)
        self.__editUndoAction.setEnabled(False)

        self.__editRedoAction = QAction(
            StringTable.getString("MENU_EDIT_REDO"))
        self.__editMenu.addAction(self.__editRedoAction)
        self.__editRedoAction.setShortcut(QKeySequence.Redo)
        self.__editRedoAction.setEnabled(False)

        self.__editMenu.addSeparator()

        self.__editCutAction = QAction(StringTable.getString("MENU_EDIT_CUT"))
        self.__editMenu.addAction(self.__editCutAction)
        self.__editCutAction.setShortcut(QKeySequence.Cut)
        self.__editCutAction.setEnabled(False)

        self.__editCopyAction = QAction(
            StringTable.getString("MENU_EDIT_COPY"))
        self.__editMenu.addAction(self.__editCopyAction)
        self.__editCopyAction.setShortcut(QKeySequence.Copy)
        self.__editCopyAction.setEnabled(False)

        self.__editPasteAction = QAction(
            StringTable.getString("MENU_EDIT_PASTE"))
        self.__editMenu.addAction(self.__editPasteAction)
        self.__editPasteAction.setShortcut(QKeySequence.Paste)
        self.__editPasteAction.setEnabled(False)

        self.__editMenu.addSeparator()

        self.__editRemoveAction = QAction(
            StringTable.getString("MENU_EDIT_REMOVE"))
        self.__editMenu.addAction(self.__editRemoveAction)
        self.__editRemoveAction.setShortcut(QKeySequence("Ctrl+R"))
        self.__editRemoveAction.setEnabled(False)

    def editMenu(self):
        '''
            Get edit menu.
        '''
        return self.__editMenu

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

        self.__viewModuleListMenu = DockWidget.QActionAttachWidget(
            StringTable.getString("MENU_VIEW_MODULE_LIST"), False)
        self.__viewModuleListMenu.setEnabled(True)
        self.__viewMenu.addAction(self.__viewModuleListMenu)

        self.__viewInfoMenu = DockWidget.QActionAttachWidget(
            StringTable.getString("MENU_VIEW_INFO"), False)
        self.__viewInfoMenu.setEnabled(True)
        self.__viewMenu.addAction(self.__viewInfoMenu)

    def __initWidgets(self):
        self.__infoWidget = self.__initSingleDockWidget(
            InfoWidget.InfoWidget, self.__viewInfoMenu,
            Qt.BottomDockWidgetArea, "info_widget")
        self.__moduleListWidget = self.__initSingleDockWidget(
            ModuleListWidget.ModuleListWidget, self.__viewModuleListMenu,
            Qt.LeftDockWidgetArea, "module_list_widget")

        self.__moduleListWidget.moduleClicked.connect(
            self.__infoWidget.setData)

    @TypeChecker(QMainWindow, type(DockWidget.QDockWidgetAttachAction),
                 DockWidget.QActionAttachWidget, int, str)
    def __initSingleDockWidget(self, cls, action, defaultArea, node):
        #Load config
        try:
            dockArea = self.__config[node]["dock_area"]

        except KeyError:
            dockArea = defaultArea

        try:
            visible = self.__config[node]["visible"]

        except KeyError:
            visible = True

        try:
            floating = self.__config[node]["floating"]
        except KeyError:
            floating = False

        try:
            x = self.__config[node]["x"]
            y = self.__config[node]["y"]
            w = self.__config[node]["w"]
            h = self.__config[node]["h"]
            setModuleListSize = True

        except KeyError:
            setModuleListSize = False

        if dockArea == Qt.NoDockWidgetArea:
            dockArea = defaultArea

        #Create widget
        widget = cls(self)
        self.addDockWidget(dockArea, widget)
        widget.setFloating(floating)
        if setModuleListSize:
            if floating:
                widget.setGeometry(x, y, w, h)

            elif dockArea in (Qt.LeftDockWidgetArea, Qt.RightDockWidgetArea):
                self.resizeDocks((widget, ), (w, ), Qt.Horizontal)

            elif dockArea in (Qt.TopDockWidgetArea, Qt.BottomDockWidgetArea):
                self.resizeDocks((widget, ), (h, ), Qt.Horizontal)

        action.attach(widget)
        widget.setVisible(visible)

        return widget

    def __saveSigleDockWidget(self, widget, node):
        self.__config[node] = {}
        self.__config[node]["dock_area"] = self.dockWidgetArea(widget)
        self.__config[node]["floating"] = widget.isFloating()
        self.__config[node]["visible"] = widget.isVisible()
        geo = widget.geometry()
        self.__config[node]["x"] = geo.x()
        self.__config[node]["y"] = geo.y()
        self.__config[node]["w"] = geo.width()
        self.__config[node]["h"] = geo.height()

    def __saveWindow(self):
        #Main widow
        self.__config["width"] = self.width()
        self.__config["height"] = self.height()
        self.__config["x"] = self.x()
        self.__config["y"] = self.y()

        #ModuleListWidget
        self.__saveSigleDockWidget(self.__moduleListWidget,
                                   "module_list_widget")

        #InfoWidget
        self.__saveSigleDockWidget(self.__infoWidget, "info_widget")

        #Save
        s = json.dumps(self.__config)
        with open(str(self.CONFIG_PATH), "w") as f:
            f.write(s)

    def __loadWindow(self):
        if self.CONFIG_PATH.exists():
            with open(str(self.CONFIG_PATH)) as f:
                self.__config = json.loads(f.read(), encoding="utf-8")

        else:
            licenseDlg = LicenseDialog.LicenseDialog()
            if licenseDlg.exec() != QDialog.Accepted:
                sys.exit(0)

            self.__config = {}

    #Slots
    @TypeChecker(QMainWindow, QAction)
    def onChangeLocale(self, action):
        self.__config["locale"] = action.locale
        QMessageBox.information(
            self, StringTable.getString("TITLE_MSG_INFO"),
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
    @TypeChecker(QMainWindow, QCloseEvent)
    def closeEvent(self, event):
        if self.__station != None:
            if not self.closeStation():
                event.ignore()
                return

        self.__saveWindow()
        self.__moduleListWidget.die()
        super().closeEvent(event)

    #Methods
    @TypeChecker(QMainWindow, (str, type(None)))
    def openStation(self, path):
        #Open new station
        try:
            self.__station = Station.Station(path)
        except Station.StationFileException as e:
            QMessageBox.critical(self,
                                 StringTable.getString("TITLE_MSG_ERROR"),
                                 str(e))
            return

        #Enable menus
        self.__fileSaveAction.setEnabled(True)
        self.__fileSaveAsAction.setEnabled(True)
        self.__fileCloseAction.setEnabled(True)

        #Create central widget
        s = self.centralWidget()
        self.setCentralWidget(
            WorkSpaceWidget.WorkSpaceWidget(self, self.__station))
        s.close()
        del s
        self.centralWidget().show()

        self.centralWidget().moduleClicked.connect(self.__infoWidget.setData)

        self.centralWidget().changeEnableAddState.connect(
            self.__moduleListWidget.setAddButtonEnabled)

        self.centralWidget().changeAddGroupState.connect(
            self.__editNewGroupAction.setEnabled)
        self.__editNewGroupAction.triggered.connect(
            self.centralWidget().addGroup)

        self.centralWidget().changeUndoState.connect(
            self.__editUndoAction.setEnabled)
        self.__editUndoAction.triggered.connect(self.centralWidget().undo)

        self.centralWidget().changeRedoState.connect(
            self.__editRedoAction.setEnabled)
        self.__editRedoAction.triggered.connect(self.centralWidget().redo)

        self.centralWidget().changeCopyState.connect(
            self.__editCutAction.setEnabled)
        self.__editCutAction.triggered.connect(self.centralWidget().cut)

        self.centralWidget().changeCopyState.connect(
            self.__editCopyAction.setEnabled)
        self.__editCopyAction.triggered.connect(self.centralWidget().copy)

        self.centralWidget().changePasteState.connect(
            self.__editPasteAction.setEnabled)
        self.__editPasteAction.triggered.connect(self.centralWidget().paste)

        self.centralWidget().changeRemoveState.connect(
            self.__editRemoveAction.setEnabled)
        self.__editRemoveAction.triggered.connect(self.centralWidget().remove)

        self.__moduleListWidget.operation.connect(
            self.centralWidget().doOperation)

        self.centralWidget().initMenuState()

        #Set title
        self.setWindowTitle(
            StringTable.getString("TITLE_MAIN_WINDOW_OPENED") %
            (self.__station.name()))

    def saveStation(self):
        #Save
        try:
            self.__station.save()

        except Station.StationFileException as e:
            QMessageBox.critical(self,
                                 StringTable.getString("TITLE_MSG_ERROR"),
                                 str(e))

        #Set title
        self.setWindowTitle(
            StringTable.getString("TITLE_MAIN_WINDOW_OPENED") %
            (self.__station.name()))

    def closeStation(self):
        if self.__station == None:
            return True

        if self.__station.isDirty():
            #Ask to save file.
            result = QMessageBox.question(
                self, StringTable.getString("TITLE_MSG_INFO"),
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
        s = self.centralWidget()
        sz = s.size()
        self.setCentralWidget(QWidget(self))
        self.centralWidget().setEnabled(False)
        s.close()
        del s
        del self.__station
        self.centralWidget().show()
        self.centralWidget().resize(sz)
        self.__station = None

        #Disable menu
        self.__fileSaveAction.setEnabled(False)
        self.__fileSaveAsAction.setEnabled(False)
        self.__fileCloseAction.setEnabled(False)
        self.__moduleListWidget.setAddButtonEnabled(False)

        #Set title
        self.setWindowTitle(StringTable.getString("TITLE_MAIN_WINDOW"))

        return True
