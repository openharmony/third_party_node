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
    ADD_API = '新增api'
    REDUCE_API = '删除api'
    ADD_DOC = '新增doc'
    REDUCE_DOC = '删除doc'
    ADD_DOC_TAG = '添加doc标签'
    REDUCE_DOC_TAG = '删除doc标签'
    FUNCTION_PARAM_POS_CHANGE = '修改函数参数位置'    # 我觉得可以理解为函数参数类型改变

    DEFINE_NAME_CHANGE = '宏名改变'
    DEFINE_TEXT_CHANGE = '宏文本改变'

    FUNCTION_NAME_CHANGE = '函数名改变'
    FUNCTION_RETURN_CHANGE = '函数返回类型改变'
    FUNCTION_PARAM_NAME_CHANGE = '函数参数名改变'    # 这个我觉得不考虑
    FUNCTION_PARAM_TYPE_CHANGE = '函数参数类型改变'
    FUNCTION_PARAM_ADD = '添加函数参数'
    FUNCTION_PARAM_REDUCE = '删除函数参数'

    STRUCT_NAME_CHANGE = '结构体名改变'
    STRUCT_MEMBER_NAME_CHANGE = '结构体成员名改变'
    STRUCT_MEMBER_TYPE_CHANGE = '结构体成员类型改变'
    STRUCT_MEMBER_ADD = '添加结构体成员'
    STRUCT_MEMBER_REDUCE = '删除结构体成员'

    UNION_NAME_CHANGE = '联合体名改变'
    UNION_MEMBER_NAME_CHANGE = '联合体成员名改变'
    UNION_MEMBER_TYPE_CHANGE = '联合体成员类型改变'
    UNION_MEMBER_ADD = '添加联合体成员'
    UNION_MEMBER_REDUCE = '删除联合体成员'

    ENUM_NAME_CHANGE = '枚举名改变'
    ENUM_MEMBER_NAME_CHANGE = '枚举成员名改变'
    ENUM_MEMBER_VALUE_CHANGE = '枚举成员值改变'
    ENUM_MEMBER_ADD = '添加枚举成员'
    ENUM_MEMBER_REDUCE = '删除枚举成员'

    VARIABLE_NAME_CHANGE = '变量名改变'
    VARIABLE_TYPE_CHANGE = '变量类型改变'
    VARIABLE_VALUE_CHANGE = '变量值的改变'

    CONSTANT_NAME_CHANGE = '常量名改变'
    CONSTANT_TYPE_CHANGE = '常量类型改变'
    CONSTANT_VALUE_CHANGE = '常量值的改变'

    TYPEDEF_NAME_TYPE_CHANGE = '重命名类型和命名改变'

    DOC_TAG_ADDTOGROUP_NA_TO_HAVE = '新增addtogroup标签'
    DOC_TAG_ADDTOGROUP_HAVE_TO_NA = '删除addtogroup标签'
    DOC_TAG_ADDTOGROUP_A_TO_B = '修改addtogroup标签'
    DOC_TAG_BRIEF_NA_TO_HAVE = '新增brief标签'
    DOC_TAG_BRIEF_HAVE_TO_NA = '删除brief标签'
    DOC_TAG_BRIEF_A_TO_B = '修改brief标签'
    DOC_TAG_DEPRECATED_NA_TO_HAVE = '接口变更为废弃'
    DOC_TAG_DEPRECATED_HAVE_TO_NA = '废弃接口变更为不废弃'
    DOC_TAG_DEPRECATED_A_TO_B = '接口废弃版本发生变化'
    DOC_TAG_FILE_NA_TO_HAVE = '新增file标签'
    DOC_TAG_FILE_HAVE_TO_NA = '删除file标签'
    DOC_TAG_FILE_A_TO_B = '修改file标签'
    DOC_TAG_LIBRARY_NA_TO_HAVE = '新增library'
    DOC_TAG_LIBRARY_HAVE_TO_NA = '删除library'
    DOC_TAG_LIBRARY_A_TO_B = '变更library'
    DOC_TAG_PARAM_NA_TO_HAVE = '新增param标签'
    DOC_TAG_PARAM_HAVE_TO_NA = '删除param标签'
    DOC_TAG_PARAM_NAME_A_TO_B = '修改param标签描述信息'
    DOC_TAG_PARAM_A_TO_B = '修改param标签描述信息'
    DOC_TAG_PERMISSION_NA_TO_HAVE = '权限从无到有'
    DOC_TAG_PERMISSION_HAVE_TO_NA = '权限从有到无'
    DOC_TAG_PERMISSION_RANGE_BIGGER = '增加or或减少and权限'
    DOC_TAG_PERMISSION_RANGE_SMALLER = '减少or或增加and权限'
    DOC_TAG_PERMISSION_RANGE_CHANGE = '权限发送改变无法判断范围变化'
    DOC_TAG_SINCE_NA_TO_HAVE = '新增since标签'
    DOC_TAG_SINCE_HAVE_TO_NA = '删除since标签'
    DOC_TAG_SINCE_A_TO_B = '修改since标签'
    DOC_TAG_SYSCAP_NA_TO_HAVE = '从没有syscap到有syscap'
    DOC_TAG_SYSCAP_HAVE_TO_NA = '从有syscap到没有syscap'
    DOC_TAG_SYSCAP_A_TO_B = 'syscap发生改变'
    DOC_TAG_LEFT_BRACE_NA_TO_HAVE = '新增左括号'
    DOC_TAG_LEFT_BRACE_HAVE_TO_NA = '删除左括号'
    DOC_TAG_RIGHT_BRACE_NA_TO_HAVE = '新增右括号'
    DOC_TAG_RIGHT_BRACE_HAVE_TO_NA = '删除右括号'


compatible_list = [DiffType.FUNCTION_PARAM_NAME_CHANGE]


class DiffInfo:
    api_name: str = ''
    api_type: str = ''
    diff_type: DiffType = DiffType.DEFAULT
    diff_message: str = ''
    old_api_full_text: str = ''
    new_api_full_text: str = ''
    is_compatible = False

    def __init__(self, diff_type: DiffType):
        self.diff_type = diff_type
        self.diff_message = diff_type.value
        self.set_diff_type(diff_type)

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
    is_compatible = False

    def __init__(self, diff_info):
        self.api_name = diff_info.api_name
        self.api_type = diff_info.api_type
        self.diff_type = diff_info.diff_type.name
        self.diff_message = diff_info.diff_message
        self.old_api_full_text = diff_info.old_api_full_text
        self.new_api_full_text = diff_info.new_api_full_text
        self.is_compatible = diff_info.is_compatible
