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
from PyQt5.QtWidgets import QMessageBox
from PyQt5.QtWidgets import QApplication, QWidget, QMainWindow
from PyQt5.QtWidgets import QMenu, QAction, QActionGroup
from PyQt5.QtWidgets import QFileDialog

import StringTable
import pathlib
import json
import locale


class MainWindow(QMainWindow):
    CONFIG_PATH = pathlib.Path(__file__).parent / ".config"

    def __init__(self, parent=None):
        super().__init__(parent)
        self.__load_window()
        self.__opened = False
        self.__path = None

        #Language
        try:
            StringTable.set_locale(self.__config["locale"])

        except KeyError:
            sys_locale = locale.getdefaultlocale()[0]
            try:
                StringTable.set_locale(sys_locale)
            except KeyError:
                pass

            self.__config["locale"] = StringTable.locale()

        self.setWindowTitle(StringTable.get_string("TITLE_MAIN_WINDOW"))
        desktop = QApplication.desktop()
        try:
            self.resize(self.__config["width"], self.__config["height"])

        except KeyError:
            self.resize(desktop.width() * 3 / 4, desktop.height() * 3 / 4)

        try:
            self.move(self.__config["x"], self.__config["y"])

        except KeyError:
            self.move(desktop.width() / 8, desktop.height() / 8)

        self.__init_file_menu()
        self.__init_setting_menu()
        self.__init_view_menu()

    #Initialize menus
    def __init_file_menu(self):
        self.__file_menu = QMenu(StringTable.get_string("MENU_FILE"))
        self.menuBar().addMenu(self.__file_menu)
        self.__file_new_action = QAction(
            StringTable.get_string("MENU_FILE_NEW"))
        self.__file_new_action.triggered.connect(self.on_menu_file_new)
        self.__file_menu.addAction(self.__file_new_action)

        self.__file_open_action = QAction(
            StringTable.get_string("MENU_FILE_OPEN"))
        self.__file_open_action.triggered.connect(self.on_menu_file_open)
        self.__file_menu.addAction(self.__file_open_action)

        self.__file_menu.addSeparator()

        self.__file_save_action = QAction(
            StringTable.get_string("MENU_FILE_SAVE"))
        self.__file_save_action.triggered.connect(self.on_menu_file_save)
        self.__file_save_action.setEnabled(False)
        self.__file_menu.addAction(self.__file_save_action)

        self.__file_save_as_action = QAction(
            StringTable.get_string("MENU_FILE_SAVE_AS"))
        self.__file_save_as_action.triggered.connect(self.on_menu_file_saveas)
        self.__file_save_as_action.setEnabled(False)
        self.__file_menu.addAction(self.__file_save_as_action)

        self.__file_menu.addSeparator()

        self.__file_close_action = QAction(
            StringTable.get_string("MENU_FILE_CLOSE"))
        self.__file_close_action.triggered.connect(self.on_menu_file_close)
        self.__file_menu.addAction(self.__file_close_action)

        self.__file_menu.addSeparator()

        self.__file_exit_action = QAction(
            StringTable.get_string("MENU_FILE_EXIT"))
        self.__file_exit_action.triggered.connect(self.on_menu_file_exit)
        self.__file_close_action.setEnabled(False)
        self.__file_menu.addAction(self.__file_exit_action)

    def __init_setting_menu(self):
        self.__setting_menu = QMenu(StringTable.get_string("MENU_SETTING"))
        self.menuBar().addMenu(self.__setting_menu)
        self.__setting_language_menu = QMenu(
            StringTable.get_string("MENU_SETTING_LANGUAGE"))
        self.__setting_menu.addMenu(self.__setting_language_menu)

        #Languages
        self.__language_action_group = QActionGroup(self)
        languages = StringTable.local_dict()
        for l in languages.keys():
            action = QAction(languages[l], self.__language_action_group)
            setattr(action, "locale", l)
            action.setCheckable(True)
            action.setChecked(l == StringTable.locale())
            self.__setting_language_menu.addAction(action)

        self.__language_action_group.triggered[QAction].connect(
            self.on_change_locale)

    def __init_view_menu(self):
        self.__view_menu = QMenu(StringTable.get_string("MENU_VIEW"))
        self.menuBar().addMenu(self.__view_menu)

        self.__view_module_list_menu = QAction(
            StringTable.get_string("MENU_VIEW_MODULE_LIST"))
        self.__view_module_list_menu.setCheckable(True)
        self.__view_module_list_menu.setChecked(False)
        self.__view_module_list_menu.setEnabled(False)
        self.__view_menu.addAction(self.__view_module_list_menu)

        self.__view_module_info_menu = QAction(
            StringTable.get_string("MENU_VIEW_MODULE_INFO"))
        self.__view_module_info_menu.setCheckable(True)
        self.__view_module_info_menu.setChecked(False)
        self.__view_module_info_menu.setEnabled(False)
        self.__view_menu.addAction(self.__view_module_info_menu)

    def __save_window(self):
        self.__config["width"] = self.width()
        self.__config["height"] = self.height()
        self.__config["x"] = self.x()
        self.__config["y"] = self.y()
        s = json.dumps(self.__config)
        with open(str(self.CONFIG_PATH), "w") as f:
            f.write(s)

    def __load_window(self):
        if self.CONFIG_PATH.exists():
            with open(str(self.CONFIG_PATH)) as f:
                self.__config = json.loads(f.read())

        else:
            self.__config = {}

    #Slots
    def on_change_locale(self, action):
        self.__config["locale"] = action.locale
        QMessageBox.information(
            self, StringTable.get_string("TITLE_INFO"),
            StringTable.get_string("INFO_EFFECT_NEXT_LAUNCH"))

    def on_menu_file_new(self, state):
        self.open_station(None)

    def on_menu_file_open(self, state):
        try:
            open_path = self.__config["openPath"]

        except KeyError:
            open_path = "."

        filename, file_type = QFileDialog.getOpenFileName(
            self, StringTable.get_string("TITLE_OPEN_FILE"), open_path,
            StringTable.get_string("TYPE_FILE"))

        if file_type != "":
            self.__config["openPath"] = str(
                pathlib.Path(filename).absolute().parent)
            self.open_station(filename)

    def on_menu_file_save(self, state):
        if self.__path == None:
            self.on_menu_file_saveas(state)

        else:
            self.save_station(self.__path)

    def on_menu_file_saveas(self, state):
        try:
            savepath = self.__config["openPath"]

        except KeyError:
            save_path = "."

        filename, file_type = QFileDialog.getSaveFileName(
            self, StringTable.get_string("TITLE_SAVE_FILE"), save_path,
            StringTable.get_string("TYPE_FILE"))

        if file_type != "":
            self.__config["openPath"] = str(
                pathlib.Path(filename).absolute().parent)
            self.save_station(filename)

    def on_menu_file_close(self, state):
        self.close_station()

    def on_menu_file_exit(self, state):
        self.close_station()
        self.close()

    #Events
    def closeEvent(self, event):
        self.__save_window()
        super().closeEvent(event)

    #Methods
    def open_station(self, path):
        pass

    def save_station(self, path):
        pass

    def close_station(self):
        pass


class ActionAttachWidget:
    pass
