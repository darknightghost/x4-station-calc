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
    Common decorators.
'''

import inspect
import functools
from distutils.version import StrictVersion

VERSION = StrictVersion("0.0.6")
RECORD_VERSION = StrictVersion("0.0.2")

import Common

from Common.QSquareButton import *


def TypeChecker(*type_args, **type_kwargs):
    '''
        Check types of arguments.

        @TypeChecker(type_of_arg0, type_of_arg1, ...)
        def func(arg0, arg1):
            ...
    '''

    def decorator(func):
        sig = inspect.signature(func)

        #Get arguments types
        types = sig.bind_partial(*type_args, **type_kwargs).arguments

        @functools.wraps(func)
        def wrapper(*args, **kwargs):
            #Get arguments values
            values = sig.bind(*args, **kwargs)

            #Check types
            for name, value in values.arguments.items():
                if name in types:
                    if not isinstance(value, types[name]):
                        raise TypeError('Argument {} must be {}'.format(
                            name, types[name]))

            return func(*args, **kwargs)

        return wrapper

    return decorator


@TypeChecker(str, str)
def addIndent(s, indent=" " * 4):
    '''
        Add indent.
    '''
    lines = s.split("\n")
    ret = ""
    for l in lines:
        ret += "%s%s\n" % (indent, l)

    return ret[:-1]


import StringTable


def testSetLocale():
    import sys
    if len(sys.argv) > 1:
        StringTable.setLocale(sys.argv[1])
