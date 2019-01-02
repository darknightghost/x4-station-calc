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

__LOCALE = "en_US"
__DEFAULT = "en_US"
__LOCALES = {"en_US": "English(American)", "zh_CN": "简体中文"}
__STRING_TABLE = {
    #Titles
    "TITLE_MAIN_WINDOW": {
        "en_US": "X4 Station Calculator",
        "zh_CN": "X4 空间站模块计算工具"
    },
    "TITLE_MODULE_LIST": {
        "en_US": "Station Modules",
        "zh_CN": "空间站模块"
    },
    "TITLE_MODULE_INFO": {
        "en_US": "Module Info",
        "zh_CN": "模块信息"
    },
    "TITLE_INFO": {
        "en_US": "Info",
        "zh_CN": "提示"
    },

    #Menu
    "MENU_FILE": {
        "en_US": "&File",
        "zh_CN": "文件(&F)"
    },
    "MENU_FILE_NEW": {
        "en_US": "&New",
        "zh_CN": "新建(&N)"
    },
    "MENU_FILE_OPEN": {
        "en_US": "&Open",
        "zh_CN": "打开(&O)"
    },
    "MENU_FILE_SAVE": {
        "en_US": "&Save",
        "zh_CN": "保存(&S)"
    },
    "MENU_FILE_SAVE_AS": {
        "en_US": "&Save as",
        "zh_CN": "另存为(&A)"
    },
    "MENU_FILE_CLOSE": {
        "en_US": "&Close",
        "zh_CN": "关闭(&C)"
    },
    "MENU_FILE_EXIT": {
        "en_US": "&Exit",
        "zh_CN": "退出(&E)"
    },
    "MENU_SETTING": {
        "en_US": "&Settings",
        "zh_CN": "设置(&S)"
    },
    "MENU_SETTING_LANGUAGE": {
        "en_US": "&Language",
        "zh_CN": "语言(&L)"
    },
    "MENU_VIEW": {
        "en_US": "&View",
        "zh_CN": "视图(&V)"
    },
    "MENU_VIEW_MODULE_LIST": {
        "en_US": "&Station Modules",
        "zh_CN": "空间站模块(&S)"
    },
    "MENU_VIEW_MODULE_INFO": {
        "en_US": "&Module Info",
        "zh_CN": "模块信息(&M)"
    },

    #Informations
    "INFO_EFFECT_NEXT_LAUNCH": {
        "en_US": "The modification will take effect after the next launch.",
        "zh_CN": "修改将在下次启动后生效."
    }
}


def set_locale(locale):
    '''
        Set locale..
    '''
    __LOCALES[locale]
    global __LOCALE
    __LOCALE = locale


def locale():
    '''
        Get locale.
    '''
    return __LOCALE


def local_dict():
    '''
        Get locales and names.
    '''
    return __LOCALES.copy()


def get_string(key):
    '''
        Get String from string table.
    '''
    strings = __STRING_TABLE[key]
    try:
        return strings[__LOCALE]

    except KeyError:
        return strings[__DEFAULT]
