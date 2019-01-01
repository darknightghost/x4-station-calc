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
    UI strings.
'''

__LOCALE = 44
__DEFRAULT = 44
__STRING_TABLE = {
    "MAIN_WINDOW_TITLE": {
        44: "X4 Station Calculator",
        86: "X4 空间站模块计算工具"
    }
}


def set_locale(locale):
    '''
        Set locale..
    '''
    __LOCALE = locale


def locale():
    '''
        Get locale.
    '''
    return __LOCALE


def get_string(key):
    '''
        Get String from string table.
    '''
    strings = __STRING_TABLE[key]
    try:
        return strings[__LOCALE]

    except KeyError:
        return strings[__DEFRAULT]
