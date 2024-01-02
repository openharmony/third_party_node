#!/usr/bin/env python
# -*- coding: utf-8 -*-
# Copyright (c) 2023 Huawei Device Co., Ltd.
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

import enum


class Scene(enum.Enum):
    FUNCTION_DECL = 'FUNCTION_DECL'
    MACRO_DEFINITION = 'MACRO_DEFINITION'
    STRUCT_DECL = 'STRUCT_DECL'
    UNION_DECL = 'UNION_DECL'
    ENUM_DECL = 'ENUM_DECL'
    VAR_DECL = 'VAR_DECL'
    TYPEDEF_DECL = 'TYPEDEF_DECL'


class TAGS(enum.Enum):
    ADD_TO_GROUP = 'addtogroup'
    BRIEF = 'brief'
    DEPRECATED = 'deprecated'
    FILE = 'file'
    LIBRARY = 'library'
    PARAM = 'param'
    PERMISSION = 'permission'
    RETURN = 'return'
    SINCE = 'since'
    SYSCAP = 'syscap'
    LEFT_BRACE = '{'
    RIGHT_BRACE = '}'


class DiffType(enum.Enum):
    DEFAULT = ''
    ADD_API = 'add api'
    REDUCE_API = 'delete api'
    ADD_DOC = 'add doc'
    REDUCE_DOC = 'delete doc'
    ADD_DOC_TAG = 'add doc tag'
    REDUCE_DOC_TAG = 'delete doc tag'
    FUNCTION_PARAM_POS_CHANGE = 'change param site in function'    # 我觉得可以理解为函数参数类型改变

    DEFINE_NAME_CHANGE = 'change define name'
    DEFINE_TEXT_CHANGE = 'change define text'

    FUNCTION_NAME_CHANGE = 'change function name'
    FUNCTION_RETURN_CHANGE = 'change function return value'
    FUNCTION_PARAM_NAME_CHANGE = 'change param name in function'    # 这个我觉得不考虑
    FUNCTION_PARAM_TYPE_CHANGE = 'change param type in function'
    FUNCTION_PARAM_ADD = 'add param in function'
    FUNCTION_PARAM_REDUCE = 'delete param in function'

    STRUCT_NAME_CHANGE = 'change struct name'
    STRUCT_MEMBER_NAME_CHANGE = 'change member name in struct'
    STRUCT_MEMBER_TYPE_CHANGE = 'change member type in struct'
    STRUCT_MEMBER_ADD = 'add member in struct'
    STRUCT_MEMBER_REDUCE = 'delete member in struct'

    UNION_NAME_CHANGE = 'change union name'
    UNION_MEMBER_NAME_CHANGE = 'change member name in union'
    UNION_MEMBER_TYPE_CHANGE = 'change member type in union'
    UNION_MEMBER_ADD = 'add member in union'
    UNION_MEMBER_REDUCE = 'delete member in union'

    ENUM_NAME_CHANGE = 'change enum name'
    ENUM_MEMBER_NAME_CHANGE = 'change member name in enum'
    ENUM_MEMBER_VALUE_CHANGE = 'change member value in enum'
    ENUM_MEMBER_ADD = 'add member in enum'
    ENUM_MEMBER_REDUCE = 'delete member in enum'

    VARIABLE_NAME_CHANGE = 'change variable name'
    VARIABLE_TYPE_CHANGE = 'change variable type'
    VARIABLE_VALUE_CHANGE = 'change variable value'

    CONSTANT_NAME_CHANGE = 'change constant name'
    CONSTANT_TYPE_CHANGE = 'change constant type'
    CONSTANT_VALUE_CHANGE = 'change constant value'

    TYPEDEF_NAME_TYPE_CHANGE = 'change typedef name type'

    DOC_TAG_ADDTOGROUP_NA_TO_HAVE = 'add addtogroup tag'
    DOC_TAG_ADDTOGROUP_HAVE_TO_NA = 'delete addtogroup tag'
    DOC_TAG_ADDTOGROUP_A_TO_B = 'change addtogroup tag'
    DOC_TAG_BRIEF_NA_TO_HAVE = 'add brief tag'
    DOC_TAG_BRIEF_HAVE_TO_NA = 'delete brief tag'
    DOC_TAG_BRIEF_A_TO_B = 'change brief tag'
    DOC_TAG_DEPRECATED_NA_TO_HAVE = 'add deprecated tag'
    DOC_TAG_DEPRECATED_HAVE_TO_NA = 'delete deprecated tag'
    DOC_TAG_DEPRECATED_A_TO_B = 'change deprecated tag'
    DOC_TAG_FILE_NA_TO_HAVE = 'add file tag'
    DOC_TAG_FILE_HAVE_TO_NA = 'delete file tag'
    DOC_TAG_FILE_A_TO_B = 'change file tag'
    DOC_TAG_LIBRARY_NA_TO_HAVE = 'add library tag'
    DOC_TAG_LIBRARY_HAVE_TO_NA = 'delete library tag'
    DOC_TAG_LIBRARY_A_TO_B = 'change library tag'
    DOC_TAG_PARAM_NA_TO_HAVE = 'add param tag'
    DOC_TAG_PARAM_HAVE_TO_NA = 'delete param tag'
    DOC_TAG_PARAM_NAME_A_TO_B = 'change param tag name'
    DOC_TAG_PARAM_A_TO_B = 'change param tag value'
    DOC_TAG_PERMISSION_NA_TO_HAVE = 'add permission tag'
    DOC_TAG_PERMISSION_HAVE_TO_NA = 'delete permission tag'
    DOC_TAG_PERMISSION_RANGE_BIGGER = 'increased permissions, ex: add "or" or remove "and"'
    DOC_TAG_PERMISSION_RANGE_SMALLER = 'reduced permissions, ex: add "and" or remove "or"'
    DOC_TAG_PERMISSION_RANGE_CHANGE = 'change permissions, before not included after and after not included before'
    DOC_TAG_SINCE_NA_TO_HAVE = 'add since tag'
    DOC_TAG_SINCE_HAVE_TO_NA = 'delete since tag'
    DOC_TAG_SINCE_A_TO_B = 'change since tag'
    DOC_TAG_SYSCAP_NA_TO_HAVE = 'add syscap tag'
    DOC_TAG_SYSCAP_HAVE_TO_NA = 'delete syscap tag'
    DOC_TAG_SYSCAP_A_TO_B = 'change syscap tag'
    DOC_TAG_LEFT_BRACE_NA_TO_HAVE = 'add { tag'
    DOC_TAG_LEFT_BRACE_HAVE_TO_NA = 'delete { tag'
    DOC_TAG_RIGHT_BRACE_NA_TO_HAVE = 'add } tag'
    DOC_TAG_RIGHT_BRACE_HAVE_TO_NA = 'delete } tag'


compatible_list = [
    DiffType.ADD_API,
    DiffType.ADD_DOC,
    DiffType.ADD_DOC_TAG,
    DiffType.FUNCTION_PARAM_NAME_CHANGE,
    DiffType.DOC_TAG_ADDTOGROUP_NA_TO_HAVE,
    DiffType.DOC_TAG_ADDTOGROUP_HAVE_TO_NA,
    DiffType.DOC_TAG_ADDTOGROUP_A_TO_B,
    DiffType.DOC_TAG_BRIEF_NA_TO_HAVE,
    DiffType.DOC_TAG_BRIEF_HAVE_TO_NA,
    DiffType.DOC_TAG_BRIEF_A_TO_B,
    DiffType.DOC_TAG_DEPRECATED_NA_TO_HAVE,
    DiffType.DOC_TAG_DEPRECATED_HAVE_TO_NA,
    DiffType.DOC_TAG_DEPRECATED_A_TO_B,
    DiffType.DOC_TAG_FILE_NA_TO_HAVE,
    DiffType.DOC_TAG_FILE_HAVE_TO_NA,
    DiffType.DOC_TAG_FILE_A_TO_B,
    DiffType.DOC_TAG_PARAM_A_TO_B,
    DiffType.DOC_TAG_PERMISSION_HAVE_TO_NA,
    DiffType.DOC_TAG_PERMISSION_RANGE_BIGGER,
    DiffType.DOC_TAG_SINCE_NA_TO_HAVE,
    DiffType.DOC_TAG_SINCE_A_TO_B,
    DiffType.DOC_TAG_LEFT_BRACE_NA_TO_HAVE,
    DiffType.DOC_TAG_LEFT_BRACE_HAVE_TO_NA,
    DiffType.DOC_TAG_RIGHT_BRACE_NA_TO_HAVE,
    DiffType.DOC_TAG_RIGHT_BRACE_HAVE_TO_NA,
]


class DiffInfo:
    api_name: str = ''
    api_type: str = ''
    diff_type: DiffType = DiffType.DEFAULT
    diff_message: str = ''
    old_api_full_text: str = ''
    new_api_full_text: str = ''
    api_line: int = 0
    api_column: int = 0
    api_file_path: str = ''
    is_compatible = False

    def __init__(self, diff_type: DiffType):
        self.diff_type = diff_type
        self.diff_message = diff_type.value
        self.set_diff_type(diff_type)

    def set_api_line(self, api_line):
        self.api_line = api_line

    def get_api_line(self):
        return self.api_line

    def set_api_column(self, api_column):
        self.api_column = api_column

    def get_api_column(self):
        return self.api_column

    def set_api_file_path(self, api_file_path):
        self.api_file_path = api_file_path

    def get_api_file_path(self):
        return self.api_file_path

    def set_api_name(self, api_name):
        self.api_name = api_name

    def get_api_name(self):
        return self.api_name

    def set_api_type(self, api_type):
        self.api_type = api_type

    def get_api_type(self):
        return self.api_type

    def set_diff_type(self, diff_type):
        if diff_type in compatible_list:
            self.is_compatible = True
        self.diff_type = diff_type

    def get_diff_type(self):
        return self.diff_type

    def set_diff_message(self, diff_message):
        self.diff_message = diff_message

    def get_diff_message(self):
        return self.diff_message

    def set_old_api_full_text(self, old_api_full_text):
        self.old_api_full_text = old_api_full_text

    def get_old_api_full_text(self):
        return self.old_api_full_text

    def set_new_api_full_text(self, new_api_full_text):
        self.new_api_full_text = new_api_full_text

    def get_new_api_full_text(self):
        return self.new_api_full_text

    def set_is_compatible(self, is_compatible):
        self.is_compatible = is_compatible

    def get_is_compatible(self):
        return self.is_compatible


class OutputJson:
    api_name: str = ''
    api_type: str = ''
    diff_type: str = ''
    diff_message: str = ''
    old_api_full_text: str = ''
    new_api_full_text: str = ''
    api_line: int = 0
    api_column: int = 0
    api_file_path: str = ''
    is_compatible = False

    def __init__(self, diff_info):
        self.api_name = diff_info.api_name
        self.api_type = diff_info.api_type
        self.diff_type = diff_info.diff_type.name
        self.diff_message = diff_info.diff_message
        self.old_api_full_text = diff_info.old_api_full_text
        self.new_api_full_text = diff_info.new_api_full_text
        self.api_line = diff_info.api_line
        self.api_column = diff_info.api_column
        self.api_file_path = diff_info.api_file_path
        self.is_compatible = diff_info.is_compatible
