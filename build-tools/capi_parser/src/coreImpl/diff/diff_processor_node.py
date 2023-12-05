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

import json
import subprocess
import os
from clang.cindex import CursorKind
from coreImpl.diff.diff_processor_permission import compare_permission, RangeChange
from typedef.diff.diff import TAGS, DiffType, DiffInfo, Scene

current_file = os.path.dirname(__file__)


def wrap_diff_info(old_info, new_info, diff_info: DiffInfo):
    if old_info is not None:
        diff_info.set_api_name(old_info['name'])
        diff_info.set_api_type(old_info['kind'])
        if 'content' in old_info['node_content']:
            diff_info.set_old_api_full_text(old_info['node_content']['content'])
    if new_info is not None:
        diff_info.set_api_name(new_info['name'])
        diff_info.set_api_type(new_info['kind'])
        if 'content' in new_info['node_content']:
            diff_info.set_new_api_full_text(new_info['node_content']['content'])

    return diff_info


def process_function(old, new):
    diff_info_list = []
    process_func_name(old, new, diff_info_list)  # 处理name
    process_func_return(old, new, diff_info_list)   # 处理返回值
    process_func_param(old, new, diff_info_list)    # 处理参数
    return diff_info_list


def process_func_name(old, new, diff_info_list):
    if old['name'] != new['name']:
        diff_info = wrap_diff_info(old, new, DiffInfo(DiffType.FUNCTION_NAME_CHANGE))
        diff_info_list.append(diff_info)


def process_func_return(old, new, diff_info_list):
    if old['return_type'] != new['return_type']:
        diff_info = wrap_diff_info(old, new, DiffInfo(DiffType.FUNCTION_RETURN_CHANGE))
        diff_info_list.append(diff_info)


def process_func_param(old, new, diff_info_list):
    if 'parm' in old and 'parm' in new:
        old_len = len(old['parm'])
        new_len = len(new['parm'])
        for i in range(max(old_len, new_len)):
            if (i + 1) > new_len:  # 减少参数
                diff_info = wrap_diff_info(old['parm'][i], new, DiffInfo(DiffType.FUNCTION_PARAM_REDUCE))
                diff_info_list.append(diff_info)

            elif (i + 1) > old_len:  # 增加参数
                diff_info = wrap_diff_info(old, new['parm'][i], DiffInfo(DiffType.FUNCTION_PARAM_ADD))
                diff_info_list.append(diff_info)

            else:
                process_param_scene(old['parm'], new['parm'], diff_info_list, i)

    elif 'parm' not in old and 'parm' in new:   # 旧无新有
        diff_info = wrap_diff_info(old, new, DiffInfo(DiffType.FUNCTION_PARAM_ADD))
        diff_info_list.append(diff_info)

    elif 'parm' in old and 'parm' not in new:   # 旧有新无
        diff_info = wrap_diff_info(old, new, DiffInfo(DiffType.FUNCTION_PARAM_REDUCE))
        diff_info_list.append(diff_info)


def process_param_scene(old_param, new_param, diff_info_list, index):
    if old_param[index]['type'] != new_param[index]['type']:
        diff_info = wrap_diff_info(old_param[index], new_param[index],
                                   DiffInfo(DiffType.FUNCTION_PARAM_TYPE_CHANGE))
        diff_info_list.append(diff_info)

    if old_param[index]['name'] != new_param[index]['name']:
        diff_info = wrap_diff_info(old_param[index], new_param[index],
                                   DiffInfo(DiffType.FUNCTION_PARAM_NAME_CHANGE))
        diff_info_list.append(diff_info)


def process_define(old, new):
    diff_define_list = []
    process_define_name(old, new, diff_define_list)  # 处理宏名
    process_define_text(old, new, diff_define_list)  # 处理宏文本
    return diff_define_list


def process_define_name(old, new, diff_define_list):
    if old['name'] != new['name']:
        diff_info = wrap_diff_info(old, new, DiffInfo(DiffType.DEFINE_NAME_CHANGE))
        diff_define_list.append(diff_info)


def process_define_text(old, new, diff_define_list):
    if 'text' not in old and 'text' in new:     # 旧无新有
        diff_info = wrap_diff_info(old, new, DiffInfo(DiffType.DEFINE_TEXT_CHANGE))
        diff_define_list.append(diff_info)
    elif 'text' in old and 'text' not in new:   # 旧有新无
        diff_info = wrap_diff_info(old, new, DiffInfo(DiffType.DEFINE_TEXT_CHANGE))
        diff_define_list.append(diff_info)
    elif 'text' in old and 'text' in new:
        if old['text'] != new['text']:
            diff_info = wrap_diff_info(old, new, DiffInfo(DiffType.DEFINE_TEXT_CHANGE))
            diff_define_list.append(diff_info)


def process_struct(old, new):
    diff_struct_list = []
    process_struct_name(old, new, diff_struct_list)     # 处理结构体名
    process_struct_member(old, new, diff_struct_list)   # 处理结构体成员
    return diff_struct_list


def process_struct_name(old, new, diff_struct_list):
    if old['name'] != new['name']:
        diff_info = wrap_diff_info(old, new, DiffInfo(DiffType.STRUCT_NAME_CHANGE))
        diff_struct_list.append(diff_info)


def process_struct_member(old, new, diff_struct_list):
    if 'members' in old and 'members' in new:   # 都有
        old_len = len(old['members'])
        new_len = len(new['members'])
        for i in range(max(old_len, new_len)):  # 处理type和name
            if (i + 1) > new_len:  # 减少成员
                diff_info = wrap_diff_info(old['members'][i], new,
                                           DiffInfo(DiffType.STRUCT_MEMBER_REDUCE))
                diff_struct_list.append(diff_info)
            elif (i + 1) > old_len:  # 增加成员
                diff_info = wrap_diff_info(old, new['members'][i],
                                           DiffInfo(DiffType.STRUCT_MEMBER_ADD))
                diff_struct_list.append(diff_info)
            else:
                process_struct_member_scene(old['members'], new['members'], i, diff_struct_list)

    elif 'members' not in old and 'members' in new:    # 旧无新有
        diff_info = wrap_diff_info(old, new, DiffInfo(DiffType.STRUCT_MEMBER_ADD))
        diff_struct_list.append(diff_info)

    elif 'members' in old and 'members' not in new:    # 旧有新无
        diff_info = wrap_diff_info(old, new, DiffInfo(DiffType.STRUCT_MEMBER_REDUCE))
        diff_struct_list.append(diff_info)


def process_struct_member_scene(old_member, new_member, index, diff_struct_list):
    special_data = []   # 存储嵌套的体系
    if (old_member[index]['kind'] == Scene.STRUCT_DECL.value) and \
            (new_member[index]['kind'] == Scene.STRUCT_DECL.value):  # 结构体套结构体
        special_data = process_struct(old_member[index], new_member[index])

    elif (old_member[index]['kind'] == Scene.UNION_DECL.value) and \
            (new_member[index]['kind'] == Scene.UNION_DECL.value):  # 结构体套联合体
        special_data = process_union(old_member[index], new_member[index])

    elif (old_member[index]['kind'] == Scene.ENUM_DECL.value) and \
            (new_member[index]['kind'] == Scene.ENUM_DECL.value):  # 结构体套枚举
        special_data = process_enum(old_member[index], new_member[index])
    diff_struct_list.extend(special_data)

    if old_member[index]['type'] != new_member[index]['type']:
        diff_info = wrap_diff_info(old_member[index], new_member[index],
                                   DiffInfo(DiffType.STRUCT_MEMBER_TYPE_CHANGE))
        diff_struct_list.append(diff_info)

    if old_member[index]['name'] != new_member[index]['name']:
        diff_info = wrap_diff_info(old_member[index], new_member[index],
                                   DiffInfo(DiffType.STRUCT_MEMBER_NAME_CHANGE))
        diff_struct_list.append(diff_info)


def process_union(old, new):
    diff_union_list = []
    process_union_name(old, new, diff_union_list)       # 处理联合体名
    process_union_member(old, new, diff_union_list)     # 处理联合体成员
    return diff_union_list


def process_union_name(old, new, diff_union_list):
    if old['name'] != new['name']:
        diff_info = wrap_diff_info(old, new, DiffInfo(DiffType.UNION_NAME_CHANGE))
        diff_union_list.append(diff_info)


def process_union_member(old, new, diff_union_list):
    if 'members' in old and 'members' in new:   # 都有
        old_len = len(old['members'])
        new_len = len(new['members'])
        for i in range(max(old_len, new_len)):  # 处理type和name
            if (i + 1) > new_len:  # 减少成员
                diff_info = wrap_diff_info(old['members'][i], new,
                                           DiffInfo(DiffType.UNION_MEMBER_REDUCE))
                diff_union_list.append(diff_info)
            elif (i + 1) > old_len:  # 增加成员
                diff_info = wrap_diff_info(old, new['members'][i],
                                           DiffInfo(DiffType.UNION_MEMBER_ADD))
                diff_union_list.append(diff_info)
            else:
                process_union_member_scene(old['members'], new['members'], i, diff_union_list)

    elif 'members' not in old and 'members' in new:    # 旧无新有
        diff_info = wrap_diff_info(old, new, DiffInfo(DiffType.UNION_MEMBER_ADD))
        diff_union_list.append(diff_info)

    elif 'members' in old and 'members' not in new:    # 旧有新无
        diff_info = wrap_diff_info(old, new, DiffInfo(DiffType.UNION_MEMBER_REDUCE))
        diff_union_list.append(diff_info)


def process_union_member_scene(old_member, new_member, i, diff_union_list):
    special_data = []   # 存储嵌套的体系
    if (old_member[i]['kind'] == Scene.STRUCT_DECL.value) and \
            (new_member[i]['kind'] == Scene.STRUCT_DECL.value):     # 联合体套结构体
        special_data = process_struct(old_member[i], new_member[i])

    elif (old_member[i]['kind'] == Scene.UNION_DECL.value) and \
            (new_member[i]['kind'] == Scene.UNION_DECL.value):     # 联合体套联合体
        special_data = process_union(old_member[i], new_member[i])

    elif (old_member[i]['kind'] == Scene.ENUM_DECL.value) and \
            (new_member[i]['kind'] == Scene.ENUM_DECL.value):     # 联合体套枚举
        special_data = process_enum(old_member[i], new_member[i])
    diff_union_list.extend(special_data)

    if old_member[i]['type'] != new_member[i]['type']:
        diff_info = wrap_diff_info(old_member[i], new_member[i],
                                   DiffInfo(DiffType.UNION_MEMBER_TYPE_CHANGE))
        diff_union_list.append(diff_info)

    if old_member[i]['name'] != new_member[i]['name']:
        diff_info = wrap_diff_info(old_member[i], new_member[i],
                                   DiffInfo(DiffType.UNION_MEMBER_NAME_CHANGE))
        diff_union_list.append(diff_info)


def process_enum(old, new):
    diff_enum_list = []
    process_enum_name(old, new, diff_enum_list)     # 处理枚举名
    process_enum_member(old, new, diff_enum_list)   # 处理枚举成员
    return diff_enum_list


def process_enum_name(old, new, diff_enum_list):
    if old['name'] != new['name']:
        diff_info = wrap_diff_info(old, new, DiffInfo(DiffType.ENUM_NAME_CHANGE))
        diff_enum_list.append(diff_info)


def process_enum_member(old, new, diff_enum_list):
    if 'members' in old and 'members' in new:   # 都有
        old_len = len(old['members'])
        new_len = len(new['members'])
        for i in range(max(old_len, new_len)):   # 处理value和name
            if (i + 1) > new_len:  # 减少成员
                diff_info = wrap_diff_info(old['members'][i], new,
                                           DiffInfo(DiffType.ENUM_MEMBER_REDUCE))
                diff_enum_list.append(diff_info)
            elif (i + 1) > old_len:  # 增加成员
                diff_info = wrap_diff_info(old, new['members'][i],
                                           DiffInfo(DiffType.ENUM_MEMBER_ADD))
                diff_enum_list.append(diff_info)
            else:
                process_enum_member_scene(old['members'], new['members'], i, diff_enum_list)

    elif 'members' not in old and 'members' in new:    # 旧无新有
        diff_info = wrap_diff_info(old, new, DiffInfo(DiffType.ENUM_MEMBER_ADD))
        diff_enum_list.append(diff_info)

    elif 'members' in old and 'members' not in new:    # 旧有新无
        diff_info = wrap_diff_info(old, new, DiffInfo(DiffType.ENUM_MEMBER_REDUCE))
        diff_enum_list.append(diff_info)


def process_enum_member_scene(old_member, new_member, i, diff_union_list):
    if old_member[i]['value'] != new_member[i]['value']:
        diff_info = wrap_diff_info(old_member[i], new_member[i],
                                   DiffInfo(DiffType.ENUM_MEMBER_VALUE_CHANGE))
        diff_union_list.append(diff_info)

    if old_member[i]['name'] != new_member[i]['name']:
        diff_info = wrap_diff_info(old_member[i], new_member[i],
                                   DiffInfo(DiffType.ENUM_MEMBER_NAME_CHANGE))
        diff_union_list.append(diff_info)


def process_variable_const(old, new):
    diff_var_or_con = []

    if old['is_const']:     # 处理常量
        process_constant_name(old, new, diff_var_or_con)  # 处理常量名
        process_constant_type(old, new, diff_var_or_con)  # 处理常量类型
        process_constant_value(old, new, diff_var_or_con)  # 处理常量值

    else:   # 处理变量
        process_variable_name(old, new, diff_var_or_con)     # 处理变量名
        process_variable_type(old, new, diff_var_or_con)     # 处理变量类型
        process_variable_value(old, new, diff_var_or_con)    # 处理变量值

    return diff_var_or_con


def process_variable_name(old, new, diff_variable_list):
    if old['name'] != new['name']:
        diff_info = wrap_diff_info(old, new, DiffInfo(DiffType.VARIABLE_NAME_CHANGE))
        diff_variable_list.append(diff_info)


def process_variable_type(old, new, diff_variable_list):
    if old['type'] != new['type']:
        diff_info = wrap_diff_info(old, new, DiffInfo(DiffType.VARIABLE_TYPE_CHANGE))
        diff_variable_list.append(diff_info)


def process_variable_value(old, new, diff_variable_list):
    if 'children' in old and 'children' in new:
        if old['children'][0]['node_content']['content'] != new['children'][0]['node_content']['content']:
            diff_info = wrap_diff_info(old['children'][0], new['children'][0],
                                       DiffInfo(DiffType.VARIABLE_VALUE_CHANGE))
            diff_variable_list.append(diff_info)

    elif 'children' not in old and 'children' in new:
        diff_info = wrap_diff_info(old, new['children'][0],
                                   DiffInfo(DiffType.VARIABLE_VALUE_CHANGE))
        diff_variable_list.append(diff_info)

    elif 'children' in old and 'children' not in new:
        diff_info = wrap_diff_info(old['children'][0], new,
                                   DiffInfo(DiffType.VARIABLE_VALUE_CHANGE))
        diff_variable_list.append(diff_info)


def process_constant_name(old, new, diff_constant_list):
    if old['name'] != new['name']:
        diff_info = wrap_diff_info(old, new, DiffInfo(DiffType.CONSTANT_NAME_CHANGE))
        diff_constant_list.append(diff_info)


def process_constant_type(old, new, diff_constant_list):
    if old['type'] != new['type']:
        diff_info = wrap_diff_info(old, new, DiffInfo(DiffType.CONSTANT_TYPE_CHANGE))
        diff_constant_list.append(diff_info)


def process_constant_value(old, new, diff_constant_list):
    if 'children' in old and 'children' in new:
        if old['children'][0]['node_content']['content'] != new['children'][0]['node_content']['content']:
            diff_info = wrap_diff_info(old['children'][0], new['children'][0],
                                       DiffInfo(DiffType.CONSTANT_VALUE_CHANGE))
            diff_constant_list.append(diff_info)

    elif 'children' not in old and 'children' in new:
        diff_info = wrap_diff_info(old, new['children'][0],
                                   DiffInfo(DiffType.CONSTANT_VALUE_CHANGE))
        diff_constant_list.append(diff_info)

    elif 'children' in old and 'children' not in new:
        diff_info = wrap_diff_info(old['children'][0], new,
                                   DiffInfo(DiffType.CONSTANT_VALUE_CHANGE))
        diff_constant_list.append(diff_info)


def process_typedef(old, new):
    diff_typedef_list = []
    process_typedef_name(old, new, diff_typedef_list)       # 处理命名

    if 'children' in old and 'children' in old:             # 处理子节点
        process_typedef_child(old['children'], new['children'], diff_typedef_list)

    return diff_typedef_list


def process_typedef_name(old, new, diff_typedef_list):
    if old['name'] != new['name']:
        diff_info = wrap_diff_info(old, new, DiffInfo(DiffType.TYPEDEF_NAME_TYPE_CHANGE))
        diff_typedef_list.append(diff_info)


def process_typedef_child(old_child, new_child, diff_typedef_list):
    special_data = []  # 存储嵌套的体系
    for i, _ in enumerate(old_child):
        if old_child[i]['name'] == '' and new_child[i]['name'] == '':
            if old_child[i]['kind'] == Scene.STRUCT_DECL.value and \
                    new_child[i]['kind'] == Scene.STRUCT_DECL.value:
                special_data = process_struct(old_child[i], new_child[i])

            elif old_child[i]['kind'] == Scene.UNION_DECL.value and \
                    new_child[i]['kind'] == Scene.UNION_DECL.value:
                special_data = process_union(old_child[i], new_child[i])

            elif old_child[i]['kind'] == Scene.ENUM_DECL.value and \
                    new_child[i]['kind'] == Scene.ENUM_DECL.value:
                special_data = process_enum(old_child[i], new_child[i])

            diff_typedef_list.extend(special_data)


process_data = {
    Scene.FUNCTION_DECL.value: process_function,
    Scene.MACRO_DEFINITION.value: process_define,
    Scene.STRUCT_DECL.value: process_struct,
    Scene.UNION_DECL.value: process_union,
    Scene.ENUM_DECL.value: process_enum,
    Scene.VAR_DECL.value: process_variable_const,
    Scene.TYPEDEF_DECL.value: process_typedef
}


def judgment_entrance(old, new):
    diff_info_list = []
    if old is None and new is None:
        return diff_info_list
    if old is None:
        diff_info_list.append(wrap_diff_info(old, new, DiffInfo(DiffType.ADD_API)))
        return diff_info_list
    if new is None:
        diff_info_list.append(wrap_diff_info(old, new, DiffInfo(DiffType.REDUCE_API)))
        return diff_info_list
    kind = new['kind']
    diff_info_list.extend(process_comment_str(old, new))
    if kind in process_data:
        diff_info_list.extend(process_data[kind](old, new))
    return diff_info_list


def process_tag_addtogroup(old_tag, new_tag, old_info, new_info):
    diff_info_list = []
    if old_tag is None:
        diff_info_list.append(wrap_diff_info(old_info, new_info, DiffInfo(DiffType.DOC_TAG_ADDTOGROUP_NA_TO_HAVE)))
        return diff_info_list
    if new_tag is None:
        diff_info_list.append(wrap_diff_info(old_info, new_info, DiffInfo(DiffType.DOC_TAG_ADDTOGROUP_HAVE_TO_NA)))
        return diff_info_list
    if old_tag['name'] != new_tag['name']:
        diff_info_list.append(wrap_diff_info(old_info, new_info, DiffInfo(DiffType.DOC_TAG_ADDTOGROUP_A_TO_B)))
    return diff_info_list


def process_tag_brief(old_tag, new_tag, old_info, new_info):
    diff_info_list = []
    if old_tag is None:
        diff_info_list.append(wrap_diff_info(old_info, new_info, DiffInfo(DiffType.DOC_TAG_BRIEF_NA_TO_HAVE)))
        return diff_info_list
    if new_tag is None:
        diff_info_list.append(wrap_diff_info(old_info, new_info, DiffInfo(DiffType.DOC_TAG_BRIEF_HAVE_TO_NA)))
        return diff_info_list
    old_brief = f'{old_tag["name"]} {old_tag["description"]}'
    new_brief = f'{new_tag["name"]} {new_tag["description"]}'
    if old_brief != new_brief:
        diff_info_list.append(wrap_diff_info(old_info, new_info, DiffInfo(DiffType.DOC_TAG_BRIEF_A_TO_B)))
    return diff_info_list


def process_tag_deprecated(old_tag, new_tag, old_info, new_info):
    diff_info_list = []
    if old_tag is None:
        diff_info_list.append(wrap_diff_info(old_info, new_info, DiffInfo(DiffType.DOC_TAG_DEPRECATED_NA_TO_HAVE)))
        return diff_info_list
    if new_tag is None:
        diff_info_list.append(wrap_diff_info(old_info, new_info, DiffInfo(DiffType.DOC_TAG_DEPRECATED_HAVE_TO_NA)))
        return diff_info_list
    old_deprecated = f'{old_tag["name"]} {old_tag["description"]}'
    new_deprecated = f'{new_tag["name"]} {new_tag["description"]}'
    if old_deprecated != new_deprecated:
        diff_info_list.append(wrap_diff_info(old_info, new_info, DiffInfo(DiffType.DOC_TAG_DEPRECATED_A_TO_B)))
    return diff_info_list


def process_tag_file(old_tag, new_tag, old_info, new_info):
    diff_info_list = []
    if old_tag is None:
        diff_info_list.append(wrap_diff_info(old_info, new_info, DiffInfo(DiffType.DOC_TAG_FILE_NA_TO_HAVE)))
        return diff_info_list
    if new_tag is None:
        diff_info_list.append(wrap_diff_info(old_info, new_info, DiffInfo(DiffType.DOC_TAG_FILE_HAVE_TO_NA)))
        return diff_info_list
    if old_tag['name'] != new_tag['name']:
        diff_info_list.append(wrap_diff_info(old_info, new_info, DiffInfo(DiffType.DOC_TAG_FILE_A_TO_B)))
    return diff_info_list


def process_tag_library(old_tag, new_tag, old_info, new_info):
    diff_info_list = []
    if old_tag is None:
        diff_info_list.append(wrap_diff_info(old_info, new_info, DiffInfo(DiffType.DOC_TAG_LIBRARY_NA_TO_HAVE)))
        return diff_info_list
    if new_tag is None:
        diff_info_list.append(wrap_diff_info(old_info, new_info, DiffInfo(DiffType.DOC_TAG_LIBRARY_HAVE_TO_NA)))
        return diff_info_list
    if old_tag['name'] != new_tag['name']:
        diff_info_list.append(wrap_diff_info(old_info, new_info, DiffInfo(DiffType.DOC_TAG_LIBRARY_A_TO_B)))
    return diff_info_list


def process_tag_param(old_tag, new_tag, old_info, new_info):
    diff_info_list = []
    if old_tag is None:
        diff_info_list.append(wrap_diff_info(old_info, new_info, DiffInfo(DiffType.DOC_TAG_PARAM_NA_TO_HAVE)))
        return diff_info_list
    if new_tag is None:
        diff_info_list.append(wrap_diff_info(old_info, new_info, DiffInfo(DiffType.DOC_TAG_PARAM_HAVE_TO_NA)))
        return diff_info_list
    if old_tag["name"] != new_tag['name']:
        diff_info_list.append(wrap_diff_info(old_info, new_info, DiffInfo(DiffType.DOC_TAG_PARAM_NAME_A_TO_B)))
    if old_tag["description"] != new_tag['description']:
        diff_info_list.append(wrap_diff_info(old_info, new_info, DiffInfo(DiffType.DOC_TAG_PARAM_A_TO_B)))
    return diff_info_list


def process_tag_permission(old_tag, new_tag, old_info, new_info):
    diff_info_list = []
    if old_tag is None:
        diff_info_list.append(wrap_diff_info(old_info, new_info, DiffInfo(DiffType.DOC_TAG_PERMISSION_NA_TO_HAVE)))
        return diff_info_list
    if new_tag is None:
        diff_info_list.append(wrap_diff_info(old_info, new_info, DiffInfo(DiffType.DOC_TAG_PERMISSION_HAVE_TO_NA)))
        return diff_info_list
    old_permission = f'{old_tag["name"]} {old_tag["description"]}'
    new_permission = f'{new_tag["name"]} {new_tag["description"]}'
    if old_permission != new_permission:
        compare_value = compare_permission(old_permission, new_permission)
        if compare_value.state_range == RangeChange.DOWN.value:
            diff_info_list.append(wrap_diff_info(old_info, new_info,
                                  DiffInfo(DiffType.DOC_TAG_PERMISSION_RANGE_SMALLER)))
        elif compare_value.state_range == RangeChange.UP.value:
            diff_info_list.append(wrap_diff_info(old_info, new_info, DiffInfo(
                DiffType.DOC_TAG_PERMISSION_RANGE_BIGGER)))
        elif compare_value.state_range == RangeChange.CHANGE.value:
            diff_info_list.append(wrap_diff_info(old_info, new_info,
                                  DiffInfo(DiffType.DOC_TAG_PERMISSION_RANGE_CHANGE)))
    return diff_info_list


def process_tag_return(old_tag, new_tag, old_info, new_info):
    diff_info_list = []
    return diff_info_list


def process_tag_since(old_tag, new_tag, old_info, new_info):
    diff_info_list = []
    if old_tag is None:
        diff_info_list.append(wrap_diff_info(old_info, new_info, DiffInfo(DiffType.DOC_TAG_SINCE_NA_TO_HAVE)))
        return diff_info_list
    if new_tag is None:
        diff_info_list.append(wrap_diff_info(old_info, new_info, DiffInfo(DiffType.DOC_TAG_SINCE_HAVE_TO_NA)))
        return diff_info_list
    old_since = f'{old_tag["name"]} {old_tag["description"]}'
    new_since = f'{new_tag["name"]} {new_tag["description"]}'
    if old_since != new_since:
        diff_info_list.append(wrap_diff_info(old_info, new_info, DiffInfo(DiffType.DOC_TAG_SINCE_A_TO_B)))
    return diff_info_list


def process_tag_syscap(old_tag, new_tag, old_info, new_info):
    diff_info_list = []
    if old_tag is None:
        diff_info_list.append(wrap_diff_info(old_info, new_info, DiffInfo(DiffType.DOC_TAG_SYSCAP_NA_TO_HAVE)))
        return diff_info_list
    if new_tag is None:
        diff_info_list.append(wrap_diff_info(old_info, new_info, DiffInfo(DiffType.DOC_TAG_SYSCAP_HAVE_TO_NA)))
        return diff_info_list
    old_syscap = f'{old_tag["name"]} {old_tag["description"]}'
    new_syscap = f'{new_tag["name"]} {new_tag["description"]}'
    if old_syscap != new_syscap:
        diff_info_list.append(wrap_diff_info(old_info, new_info, DiffInfo(DiffType.DOC_TAG_SYSCAP_A_TO_B)))
    return diff_info_list


def process_tag_left_brace(old_tag, new_tag, old_info, new_info):
    diff_info_list = []
    if old_tag is None:
        diff_info_list.append(wrap_diff_info(old_info, new_info, DiffInfo(DiffType.DOC_TAG_LEFT_BRACE_NA_TO_HAVE)))
        return diff_info_list
    if new_tag is None:
        diff_info_list.append(wrap_diff_info(old_info, new_info, DiffInfo(DiffType.DOC_TAG_LEFT_BRACE_HAVE_TO_NA)))
        return diff_info_list
    return diff_info_list


def process_tag_right_brace(old_tag, new_tag, old_info, new_info):
    diff_info_list = []
    if old_tag is None:
        diff_info_list.append(wrap_diff_info(old_info, new_info, DiffInfo(DiffType.DOC_TAG_RIGHT_BRACE_NA_TO_HAVE)))
        return diff_info_list
    if new_tag is None:
        diff_info_list.append(wrap_diff_info(old_info, new_info, DiffInfo(DiffType.DOC_TAG_RIGHT_BRACE_HAVE_TO_NA)))
        return diff_info_list
    return diff_info_list


process_tag_function = {
    TAGS['ADD_TO_GROUP'].value: process_tag_addtogroup,
    TAGS['BRIEF'].value: process_tag_brief,
    TAGS['DEPRECATED'].value: process_tag_deprecated,
    TAGS['FILE'].value: process_tag_file,
    TAGS['LIBRARY'].value: process_tag_library,
    TAGS['PARAM'].value: process_tag_param,
    TAGS['PERMISSION'].value: process_tag_permission,
    TAGS['RETURN'].value: process_tag_return,
    TAGS['SINCE'].value: process_tag_since,
    TAGS['SYSCAP'].value: process_tag_syscap,
    TAGS['LEFT_BRACE'].value: process_tag_left_brace,
    TAGS['RIGHT_BRACE'].value: process_tag_right_brace,
}


def process_comment(comment: str):
    '''
    处理comment数据，通过node调用comment-parser解析doc注释
    '''
    result_json = []
    if comment == "none_comment":
        return result_json
    result = subprocess.check_output(
        ['node', os.path.abspath(os.path.join(current_file, "../comment_parser.js")), comment])   # 解析comment
    result_json: list = json.loads(result.decode('utf-8'))
    return result_json


def get_tag_dict_from_list(tag_list):
    tag_dict = dict()
    for tag in tag_list:
        if tag['tag'] in tag_dict.keys():
            tag_dict[tag['tag']].append(tag)
        else:
            tag_dict[tag['tag']] = [tag]
    return tag_dict


def process_doc(old_doc, new_doc, old_info, new_info):
    diff_info_list = []
    old_tag_list = old_doc['tags']
    new_tag_list = new_doc['tags']
    old_tag_dict = get_tag_dict_from_list(old_tag_list)
    new_tag_dict = get_tag_dict_from_list(new_tag_list)
    for item in old_tag_dict.keys() | new_tag_dict.keys():
        if item not in process_tag_function.keys():
            continue
        old_tag = []
        new_tag = []
        if item in old_tag_dict.keys():
            old_tag = old_tag_dict.get(item)
        if item in new_tag_dict.keys():
            new_tag = new_tag_dict.get(item)
        max_length = max(len(old_tag), len(new_tag))
        while len(old_tag) < max_length:
            old_tag += [None] * (max_length - len(old_tag))
        while len(new_tag) < max_length:
            new_tag += [None] * (max_length - len(new_tag))
        tag_process = process_tag_function[item]
        index = 0
        while index < max_length:
            diff_info_list.extend(tag_process(old_tag[index], new_tag[index], old_info, new_info))
            index += 1

    return diff_info_list


def process_doc_list(old_doc_list: list, new_doc_list: list, old_info, new_info):
    diff_info_list = []
    old_length = len(old_doc_list)
    new_length = len(new_doc_list)
    index = 0
    while index < max(old_length, new_length):
        if index > old_length:
            diff_info_list.append(wrap_diff_info(old_info, new_info, DiffInfo(DiffType.ADD_DOC)))
            break
        if index > new_length:
            diff_info_list.append(wrap_diff_info(old_info, new_info, DiffInfo(DiffType.REDUCE_DOC)))
            break
        diff_info_list.extend(process_doc(old_doc_list[index], new_doc_list[index], old_info, new_info))
        index += 1

    return diff_info_list


def process_comment_str(old_info, new_info):
    diff_info_list = []
    if old_info['comment'] == new_info['comment']:
        return diff_info_list
    if old_info['comment'] == 'none_comment':
        diff_info_list.append(wrap_diff_info(old_info, new_info, DiffInfo(DiffType.ADD_DOC)))
        return diff_info_list
    if new_info['comment'] == 'none_comment':
        diff_info_list.append(wrap_diff_info(old_info, new_info, DiffInfo(DiffType.REDUCE_DOC)))
        return diff_info_list
    old_doc_list = process_comment(old_info['comment'])
    new_doc_list = process_comment(new_info['comment'])
    if new_info['kind'] == CursorKind.TRANSLATION_UNIT.name:
        diff_info_list.extend(process_doc_list(old_doc_list, new_doc_list, old_info, new_info))
    else:
        if len(old_doc_list) > len(new_doc_list):
            diff_info_list.append(wrap_diff_info(old_info, new_info, DiffInfo(DiffType.REDUCE_DOC)))
        elif len(old_doc_list) < len(new_doc_list):
            diff_info_list.append(wrap_diff_info(old_info, new_info, DiffInfo(DiffType.ADD_DOC)))
        else:
            diff_info_list.extend(process_doc_list(
                [old_doc_list[len(old_doc_list) - 1]], [new_doc_list[len(new_doc_list) - 1]], old_info, new_info))
    return diff_info_list
