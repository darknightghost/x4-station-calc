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
from PyQt5.QtWidgets import QApplication, QWidget, QMainWindow

import StringTable


class MainWindow(QMainWindow):
    def __init__(self, parent=None):
        super().__init__(parent)
        self.setWindowTitle(StringTable.get_string("MAIN_WINDOW_TITLE"))
        desktop = QApplication.desktop()
        self.resize(desktop.width() * 3 / 4, desktop.height() * 3 / 4)
        self.move(desktop.width() / 8, desktop.height() / 8)

    #Slots

    #Methods
    def open_station(self, path):
        pass
