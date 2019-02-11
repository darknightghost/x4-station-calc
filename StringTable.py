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

import Common
from Common import *

__LOCALE = "en_US"
__DEFAULT = "en_US"
__LOCALES = {"en_US": "English(American)", "zh_CN": "简体中文"}
__STRING_TABLE = {
    #Record File
    "NEW_FILE_NAME": {
        "en_US": "New station",
        "zh_CN": "新空间站"
    },
    "FAILED_READ_STATION_FILE": {
        "en_US": "Failed to read \"%s\".",
        "zh_CN": "读取文件\"%s\"失败."
    },
    "FAILED_WRITE_STATION_FILE": {
        "en_US": "Failed to write \"%s\".",
        "zh_CN": "写入文件\"%s\"失败."
    },
    "FAILED_PARSE_STATION_FILE": {
        "en_US": "Failed to parse \"%s\" :\n%s",
        "zh_CN": "解析文件\"%s\"失败 :\n%s"
    },
    "ILLEGAL_JSON_FORMAT":{
        "en_US": "Illegal json format.",
        "zh_CN": "Json格式非法."
    }

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

    #File dialog
    "TITLE_OPEN_FILE": {
        "en_US": "Load save",
        "zh_CN": "加载存档"
    },
    "TITLE_SAVE_FILE": {
        "en_US": "Save As",
        "zh_CN": "另存为"
    },
    "TYPE_FILE": {
        "en_US": "X4 stations (*.x4station)",
        "zh_CN": "x4空间站存档 (*.x4station)"
    },

    #Informations
    "INFO_EFFECT_NEXT_LAUNCH": {
        "en_US": "The modification will take effect after the next launch.",
        "zh_CN": "修改将在下次启动后生效."
    },

    #Station types
    "STATION_TYPE_PRODUCTION": {
        "en_US": "Production Modules",
        "zh_CN": "生产模块"
    },
    "STATION_TYPE_STORAGE": {
        "en_US": "Storage Modules",
        "zh_CN": "仓储模块"
    },
    "STATION_TYPE_DEFENCE": {
        "en_US": "Defence Modules",
        "zh_CN": "防御模块"
    },
    "STATION_TYPE_HABITATION": {
        "en_US": "Habitation Modules",
        "zh_CN": "居住模块",
    },
    "STATION_TYPE_DOCK": {
        "en_US": "Dock Modules",
        "zh_CN": "停靠模块",
    },
    "STATION_TYPE_OTHER": {
        "en_US": "Other Modules",
        "zh_CN": "其他模块",
    },
    "STATION_TYPE_VENTURE": {
        "en_US": "Venture Modules",
        "zh_CN": "远征模块",
    },
    "STATION_TYPE_BUILD": {
        "en_US": "Build Modules",
        "zh_CN": "建造模块",
    }
}


@TypeChecker(str)
def setLocale(locale):
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


def defaultLocale():
    '''
        Get default locale.
    '''
    return __DEFAULT


def localeDict():
    '''
        Get locales and names.
    '''
    return __LOCALES.copy()


@TypeChecker(str)
def getString(key):
    '''
        Get String from string table.
    '''
    strings = __STRING_TABLE[key]
    try:
        return strings[__LOCALE]

    except KeyError:
        return strings[__DEFAULT]
