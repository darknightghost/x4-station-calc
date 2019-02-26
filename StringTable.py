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
    "NEW_GROUP_NAME": {
        "en_US": "New group",
        "zh_CN": "新分组"
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
    "ILLEGAL_JSON_FORMAT": {
        "en_US": "Illegal json format.",
        "zh_CN": "Json格式非法."
    },
    "VERSION_TOO_LOW": {
        "en_US": "The file requires version over %s.",
        "zh_CN": "该文件需要版本大于%s."
    },
    "FILE_NOT_SAVE": {
        "en_US": "Last change has not been saved, save it now?.",
        "zh_CN": "有尚未保存的修改, 是否保存?"
    },

    #Titles
    "TITLE_MAIN_WINDOW": {
        "en_US": "X4 Station Calculator",
        "zh_CN": "X4空间站模块计算工具"
    },
    "TITLE_MAIN_WINDOW_OPENED": {
        "en_US": "X4 Station Calculator - %s",
        "zh_CN": "X4 空间站模块计算工具 - %s"
    },
    "TITLE_INFO": {
        "en_US": "Info",
        "zh_CN": "信息"
    },
    "TITLE_MSG_INFO": {
        "en_US": "Info",
        "zh_CN": "提示"
    },
    "TITLE_MSG_ERROR": {
        "en_US": "Error",
        "zh_CN": "错误"
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
    "MENU_EDIT": {
        "en_US": "&Edit",
        "zh_CN": "编辑(&E)"
    },
    "MENU_EDIT_NEW_GROUP": {
        "en_US": "&New Group",
        "zh_CN": "新建分组(&N)"
    },
    "MENU_EDIT_UNDO": {
        "en_US": "&Undo",
        "zh_CN": "撤销(&U)"
    },
    "MENU_EDIT_REDO": {
        "en_US": "&Redo",
        "zh_CN": "恢复(&R)"
    },
    "MENU_EDIT_CUT": {
        "en_US": "&Cut",
        "zh_CN": "剪切(&C)"
    },
    "MENU_EDIT_COPY": {
        "en_US": "C&opy",
        "zh_CN": "复制(&O)"
    },
    "MENU_EDIT_PASTE": {
        "en_US": "&Paste",
        "zh_CN": "粘贴(&P)"
    },
    "MENU_EDIT_REMOVE": {
        "en_US": "R&emove",
        "zh_CN": "删除(&E)"
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
    "MENU_VIEW_INFO": {
        "en_US": "&Info",
        "zh_CN": "信息(&I)"
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
    },

    #ModuleListWidget
    "TITLE_MODULE_LIST": {
        "en_US": "Station Modules",
        "zh_CN": "空间站模块"
    },
    "BTN_SHOW_FILTER": {
        "en_US": "Sh&ow Filter",
        "zh_CN": "显示过滤选项(&O)"
    },
    "BTN_HIDE_FILTER": {
        "en_US": "&Hide Filter",
        "zh_CN": "隐藏过滤选项(&H)"
    },
    "BTN_ADD_TO_STATION": {
        "en_US": "A&dd to Station",
        "zh_CN": "添加到空间站(&D)"
    },
    "CHK_BY_FACTION": {
        "en_US": "By Fa&ction",
        "zh_CN": "按种族(&C)"
    },
    "CHK_BY_PRODUCT": {
        "en_US": "By &Product",
        "zh_CN": "按产品(&P)"
    },
    "CHK_BY_KEYWORDS": {
        "en_US": "By &Keywords",
        "zh_CN": "按关键字(&K)"
    },

    #InfoWidget
    "TITLE_INFO_WIDGET": {
        "en_US": "Info",
        "zh_CN": "信息"
    },
    "STR_NAME": {
        "en_US": "Name",
        "zh_CN": "名称"
    },
    "STR_STORAGE_TYPE": {
        "en_US": "Storage Type",
        "zh_CN": "仓储类型"
    },
    "STR_VOLUME": {
        "en_US": "Volume",
        "zh_CN": "体积"
    },
    "STR_SOLID": {
        "en_US": "Solid",
        "zh_CN": "固体"
    },
    "STR_LIQUID": {
        "en_US": "Liquid",
        "zh_CN": "液体"
    },
    "STR_CONTAINER": {
        "en_US": "Container",
        "zh_CN": "集装"
    },
    "STR_STATION_MODULE_TYPE": {
        "en_US": "Station Module Type",
        "zh_CN": "空间站模块类型"
    },
    "STR_FACTION": {
        "en_US": "Faction",
        "zh_CN": "种族"
    },
    "STR_TURRET_NUM": {
        "en_US": "Number of turret",
        "zh_CN": "炮塔数量"
    },
    "STR_S_LAUNCE_TUBE": {
        "en_US": "S Launch Tube",
        "zh_CN": "S级发射管"
    },
    "STR_M_LAUNCE_TUBE": {
        "en_US": "M Launch Tube",
        "zh_CN": "M级发射管"
    },
    "STR_SHIP_STORAGE": {
        "en_US": "Ship Storage",
        "zh_CN": "飞船仓储"
    },
    "STR_S_DOCK": {
        "en_US": "S Docks",
        "zh_CN": "S级港口"
    },
    "STR_M_DOCK": {
        "en_US": "M Docks",
        "zh_CN": "M级港口"
    },
    "STR_L_XL_DOCK": {
        "en_US": "L/XL Docks",
        "zh_CN": "L/XL级泊位"
    },
    "STR_WORKFORCE_CAPACITY": {
        "en_US": "Workforce Capacity",
        "zh_CN": "劳动力数量"
    },
    "STR_SUPPLY_REQUIRED": {
        "en_US": "Maxium Supply Required",
        "zh_CN": "最大所需补给"
    },
    "STR_PRODUCTS": {
        "en_US": "Products",
        "zh_CN": "产品"
    },
    "STR_RESOURCES": {
        "en_US": "Resources",
        "zh_CN": "资源"
    },
    "STR_STORAGE_TYPE": {
        "en_US": "Storage Type",
        "zh_CN": "仓储类型"
    },
    "STR_STORAGE_CAPACITY": {
        "en_US": "Storage Capacity",
        "zh_CN": "仓储容量"
    },

    #WorkSpace
    "STR_STATION_MODULES": {
        "en_US": "Station Modules",
        "zh_CN": "空间站模块"
    },
    "STR_SUMMARY": {
        "en_US": "Summary",
        "zh_CN": "总计"
    },
    #"": {
    #    "en_US": "",
    #    "zh_CN": ""
    #},
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
