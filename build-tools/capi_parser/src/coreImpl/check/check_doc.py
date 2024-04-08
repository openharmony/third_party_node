#!/usr/bin/env python
# coding=utf-8
##############################################
# Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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
##############################################
import json
import re
import subprocess
import os
from clang.cindex import CursorKind
from typedef.check.check import ApiResultInfo, DocInfo, ErrorType, ErrorMessage, FileDocInfo, LogType, TAGS, ErrorLevel

current_file = os.path.dirname(__file__)
# permission数据来源于https://gitee.com/openharmony/utils_system_resources/raw/master/systemres/main/config.json
permission_tag_rules = ['ohos.permission.HEALTH_DATA', 'ohos.permission.HEART_RATE', 'ohos.permission.ACCELERATION']
with open(os.path.abspath(os.path.join(current_file, "rules/perssion_rule.json"))) as json_file:
    permission_file_content = json.load(json_file)
    permission_tag_rules.extend([item['name'] for item in permission_file_content['module']['definePermissions']])
syscap_tag_rules: list = []
with open(os.path.abspath(os.path.join(current_file, "rules/syscap_rule.json"))) as json_file:
    syscap_tag_rules = json.load(json_file)


def create_api_result_info_by_doc(error_type: ErrorType, error: ErrorMessage, params: list, api_info: object):
    error_info = str(error.value)
    for param in params:
        error_info = error_info.replace('$$', str(param), 1)
    api_result_info = ApiResultInfo(error_type.value, error_info, api_info['name'])
    api_result_info.set_api_since(api_info['since'])
    api_result_info.set_type(LogType.LOG_JSDOC.value)
    api_result_info.set_level(ErrorLevel.LOW.value)
    if 'location' in api_info.keys():
        api_result_info.set_location(api_info['location']['location_path'])
        api_result_info.set_location_line(api_info['location']['location_line'])
        api_result_info.set_location_column(api_info['location']['location_column'])
        api_result_info.set_file_name(api_info['location']['location_path'])
    else:
        api_result_info.set_file_name(api_info['name'])
        api_result_info.set_location(api_info['name'])
    return api_result_info


def process_tag_addtogroup(tag_info, file_doc_info: FileDocInfo, api_info) -> list:
    api_result_info_list = []
    group_name = tag_info['name']
    if group_name == "":
        api_result_info = create_api_result_info_by_doc(
            ErrorType.EMPTY_TAG, ErrorMessage.EMPTY_TAG, [tag_info['tag']], api_info)
        api_result_info_list.append(api_result_info)
    return api_result_info_list


def process_tag_brief(tag_info, file_doc_info: FileDocInfo, api_info) -> list:
    doc_info = file_doc_info.curr_doc_info
    api_result_info_list = []
    brief = tag_info['name'] + tag_info['description']
    doc_info.brief = brief
    return api_result_info_list


def process_tag_deprecated(tag_info, file_doc_info: FileDocInfo, api_info) -> list:
    doc_info = file_doc_info.curr_doc_info
    api_result_info_list = []
    name = tag_info['name']
    version: str = tag_info['description']
    if name != "since" or not version.isdigit():
        api_result_info = create_api_result_info_by_doc(
            ErrorType.UNKNOW_DEPRECATED, ErrorMessage.ERROR_INFO_VALUE_TAG, [tag_info['tag']], api_info)
        api_result_info_list.append(api_result_info)
    doc_info.deprecated = version
    return api_result_info_list


def process_tag_file(tag_info, file_doc_info: FileDocInfo, api_info) -> list:
    doc_info = file_doc_info.curr_doc_info
    api_result_info_list = []
    file_name = tag_info['name']
    if file_name == "":
        api_result_info = create_api_result_info_by_doc(
            ErrorType.EMPTY_TAG, ErrorMessage.EMPTY_TAG, [tag_info['tag']], api_info)
        api_result_info_list.append(api_result_info)
    doc_info.file = file_name
    return api_result_info_list


def process_tag_library(tag_info, file_doc_info: FileDocInfo, api_info) -> list:
    api_result_info_list = []
    library: str = tag_info['name'] + tag_info['description']
    if not library.endswith(('.so', '.a')) and library != "NA":
        api_result_info = create_api_result_info_by_doc(
            ErrorType.WRONG_VALUE, ErrorMessage.ERROR_INFO_VALUE_LIBRARY, [], api_info)
        api_result_info_list.append(api_result_info)
    return api_result_info_list


def process_tag_param(tag_info, file_doc_info: FileDocInfo, api_info) -> list:
    file_doc_info.curr_doc_info.param_index += 1
    api_result_info_list = []
    if api_info['kind'] != CursorKind.FUNCTION_DECL.name:
        return api_result_info_list

    if 'parm' not in api_info.keys():
        api_result_info = create_api_result_info_by_doc(
            ErrorType.WRONG_VALUE, ErrorMessage.ERROR_INFO_COUNT_PARAM, [], api_info)
        api_result_info_list.append(api_result_info)
        return api_result_info_list
    index = file_doc_info.curr_doc_info.param_index
    params = api_info['parm']
    if (len(params) < index + 1):
        return api_result_info_list
    param = api_info['parm'][index]
    if tag_info['name'] != param['name']:
        api_result_info = create_api_result_info_by_doc(
            ErrorType.WRONG_VALUE, ErrorMessage.ERROR_INFO_VALUE_PARAM, [index + 1, index + 1], api_info)
        api_result_info_list.append(api_result_info)

    return api_result_info_list


def process_tag_permission(tag_info, file_doc_info: FileDocInfo, api_info) -> list:
    doc_info = file_doc_info.curr_doc_info
    api_result_info_list = []
    permission: str = ""
    if tag_info['description'] == "":
        permission: str = tag_info['name']
    else:
        permission: str = tag_info['name'] + ' ' + tag_info['description']
    permission_blank = re.sub(re.compile(r'\(|\)'), '', permission)
    permission_join = re.sub(re.compile(r'( or | and )'), '$', permission_blank)
    permission_list = permission_join.split('$')
    error_permission_list = list(filter(lambda item: item not in permission_tag_rules, permission_list))
    if error_permission_list:
        api_result_info = create_api_result_info_by_doc(
            ErrorType.WRONG_VALUE, ErrorMessage.ERROR_INFO_VALUE_PERMISSION, [], api_info)
        api_result_info_list.append(api_result_info)
    doc_info.permission = permission
    return api_result_info_list


def process_tag_return(tag_info, file_doc_info: FileDocInfo, api_info) -> list:
    api_result_info_list = []
    return api_result_info_list


def process_tag_since(tag_info, file_doc_info: FileDocInfo, api_info) -> list:
    doc_info = file_doc_info.curr_doc_info
    api_result_info_list = []
    value: str = tag_info['name'] + tag_info['description']
    if value == "":
        api_result_info = create_api_result_info_by_doc(
            ErrorType.EMPTY_TAG, ErrorMessage.EMPTY_TAG, [tag_info['tag']], api_info)
        api_result_info_list.append(api_result_info)
    elif not value.isdigit():
        api_result_info = create_api_result_info_by_doc(
            ErrorType.WRONG_VALUE, ErrorMessage.ERROR_INFO_VALUE_SINCE, [], api_info)
        api_result_info_list.append(api_result_info)
    doc_info.since = tag_info['name']
    return api_result_info_list


def process_tag_syscap(tag_info, file_doc_info: FileDocInfo, api_info) -> list:
    doc_info = file_doc_info.curr_doc_info
    api_result_info_list = []
    syscap = tag_info['name'] + tag_info['description']
    if syscap == "":
        api_result_info = create_api_result_info_by_doc(
            ErrorType.EMPTY_TAG, ErrorMessage.EMPTY_TAG, [tag_info['tag']], api_info)
        api_result_info_list.append(api_result_info)
    if syscap not in syscap_tag_rules:
        api_result_info = create_api_result_info_by_doc(
            ErrorType.WRONG_VALUE, ErrorMessage.ERROR_INFO_VALUE_SYSCAP, [], api_info)
        api_result_info_list.append(api_result_info)
    doc_info.syscap = syscap
    return api_result_info_list


def process_tag_left_brace(tag_info, file_doc_info: FileDocInfo, api_info) -> list:
    api_result_info_list = []
    if not file_doc_info.is_in_group_tag:
        api_result_info = create_api_result_info_by_doc(
            ErrorType.WRONG_SCENE, ErrorMessage.ERROR_USE_LEFT_BRACE, [], api_info)
        api_result_info_list.append(api_result_info)
    return api_result_info_list


def process_tag_right_brace(tag_info, file_doc_info: FileDocInfo, api_info) -> list:
    api_result_info_list = []
    if not file_doc_info.has_group_end:
        file_doc_info.has_group_end = True
        return api_result_info_list
    api_result_info = create_api_result_info_by_doc(
        ErrorType.WRONG_SCENE, ErrorMessage.ERROR_USE_RIGHT_BRACE, [], api_info)
    api_result_info_list.append(api_result_info)
    return api_result_info_list


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


def process_each_tags(tag_info, file_doc_info: FileDocInfo, api_info) -> list:
    '''
    处理解析出来的每个tag标签
    '''
    api_result_info_list = []
    doc_info = file_doc_info.curr_doc_info
    tag: str = tag_info['tag']
    if tag.lower() != tag:
        tag = tag.lower()
        api_result_info = create_api_result_info_by_doc(
            ErrorType.ERROR_TAG, ErrorMessage.USE_UPPER_TAG, [tag_info['tag'], tag], api_info)
        api_result_info_list.append(api_result_info)
    if tag not in process_tag_function.keys():
        return []
    tag_process = process_tag_function[tag]
    api_result_info_list.extend(tag_process(tag_info, file_doc_info, api_info))
    return api_result_info_list


def process_file_doc_group(file_doc_info: FileDocInfo, item, api_info) -> list:
    '''
    处理每个文件中头文件中的addtogroup
    '''
    api_result_info_list = []
    if item['tag'] == 'addtogroup':
        if file_doc_info.group_name is None:
            file_doc_info.group_name = item['name']
            file_doc_info.is_in_group_tag = True
        else:
            api_result_info = create_api_result_info_by_doc(
                ErrorType.WRONG_SCENE, ErrorMessage.REPEAT_FILE_TAG, [item['tag']], api_info)
            api_result_info_list.append(api_result_info)
    if item['tag'] == '{':
        if not file_doc_info.has_group_start:
            file_doc_info.has_group_start = True
        else:
            api_result_info = create_api_result_info_by_doc(
                ErrorType.WRONG_SCENE, ErrorMessage.ERROR_REPEAT_LEFT_BRACE, [item['tag']], api_info)
            api_result_info_list.append(api_result_info)

    return api_result_info_list


def process_file_doc_file(file_doc_info: FileDocInfo, item, api_info) -> list:
    '''
    处理每个文件中头文件中的file
    '''
    api_result_info_list = []
    if item['tag'] == 'file':
        if file_doc_info.file_name is None:
            file_doc_info.file_name = item['name']
            file_doc_info.is_in_file_tag = True
        else:
            api_result_info = create_api_result_info_by_doc(
                ErrorType.WRONG_SCENE, ErrorMessage.REPEAT_FILE_TAG, [item['tag']], api_info)
            api_result_info_list.append(api_result_info)
    elif item['tag'] == 'brief':
        file_doc_info.file_brief = item['name'] + item['description']
    elif item['tag'] == 'library':
        file_doc_info.file_library = item['name'] + item['description']
    elif item['tag'] == 'syscap':
        file_doc_info.file_syscap = item['name'] + item['description']
    return api_result_info_list


def process_each_comment(comment_object, file_doc_info: FileDocInfo, api_info) -> list:
    '''
    检查单个comment对应的报错信息，不处理文件级别的校验
    '''
    api_result_info_list = []
    tags = comment_object['tags']
    file_doc_info.curr_doc_info = DocInfo()  # 每段doc返回的数据格式
    for item in tags:
        # 处理文件组数据，一个文件只有一个，是否需要判断多个存在？
        if item['tag'] == 'addtogroup' or file_doc_info.is_in_group_tag:
            api_result_info_list.extend(process_file_doc_group(file_doc_info, item, api_info))
        # 处理文件中file说明，一个文件只有一个，是否需要判断多个存在？
        if item['tag'] == 'file' or file_doc_info.is_in_file_tag:
            api_result_info_list.extend(process_file_doc_file(file_doc_info, item, api_info))
        api_result_info_list.extend(process_each_tags(item, file_doc_info, api_info))
    # 判断param标签的数量和方法参数的数量是否对应
    param_tag_count = file_doc_info.curr_doc_info.param_index + 1
    if api_info['kind'] == CursorKind.FUNCTION_DECL.name and \
            'parm' in api_info.keys() and len(api_info['parm']) != param_tag_count:
        api_result_info = create_api_result_info_by_doc(
            ErrorType.WRONG_SCENE, ErrorMessage.ERROR_INFO_COUNT_PARAM, [], api_info)
        api_result_info_list.append(api_result_info)
    # 判断group标签的开头
    if file_doc_info.is_in_group_tag and not file_doc_info.has_group_start:
        api_result_info = create_api_result_info_by_doc(
            ErrorType.WRONG_SCENE, ErrorMessage.ERROR_FILE_HAS_ONE_LOSE_OTHER, ['group tag', 'start tag {'], api_info)
        api_result_info_list.append(api_result_info)
    # 处理file标签的值
    if file_doc_info.is_in_file_tag:
        if file_doc_info.file_brief is None:
            api_result_info = create_api_result_info_by_doc(
                ErrorType.WRONG_SCENE, ErrorMessage.ERROR_FILE_HAS_ONE_LOSE_OTHER, ['file tag', 'brief tag'], api_info)
            api_result_info_list.append(api_result_info)
        if file_doc_info.file_library is None:
            api_result_info = create_api_result_info_by_doc(ErrorType.WRONG_SCENE,
                                                            ErrorMessage.ERROR_FILE_HAS_ONE_LOSE_OTHER,
                                                            ['file tag', 'library tag'], api_info)
            api_result_info_list.append(api_result_info)
        if file_doc_info.file_syscap is None:
            api_result_info = create_api_result_info_by_doc(
                ErrorType.WRONG_SCENE, ErrorMessage.ERROR_FILE_HAS_ONE_LOSE_OTHER, ['file tag', 'syscap tag'], api_info)
            api_result_info_list.append(api_result_info)

    return api_result_info_list


def process_tag_missing(comment_object, file_doc_info: FileDocInfo, api_info):
    api_result_info_list = []
    if api_info['kind'] not in process_tag_missing_function.keys():
        return []
    has_since_tag = False
    tags = comment_object['tags']
    for item in tags:
        if item['tag'] == 'since':
            has_since_tag = True
    if not has_since_tag:
        api_result_info = create_api_result_info_by_doc(
            ErrorType.WRONG_SCENE, ErrorMessage.ERROR_LOST_LABEL, ['since'], api_info)
        api_result_info_list.append(api_result_info)
    return api_result_info_list


process_tag_missing_function = {
    CursorKind.FUNCTION_DECL.name: [],
    CursorKind.UNION_DECL.name: [],
    CursorKind.STRUCT_DECL.name: [],
    CursorKind.ENUM_DECL.name: [],
    CursorKind.TYPEDEF_DECL.name: [],
    CursorKind.FUNCTION_DECL.name: [],
    CursorKind.VAR_DECL.name: [],
    CursorKind.MACRO_DEFINITION.name: [],
}


def process_comment(comment: str, file_doc_info: FileDocInfo, api_info) -> list:
    '''
    处理comment数据，通过node调用comment-parser解析doc注释
    '''
    api_result_info_list = []
    if comment == "none_comment":
        return api_result_info_list
    result = subprocess.check_output(
        ['node', os.path.abspath(os.path.join(current_file, "comment_parser.js")), comment])   # 解析comment
    result_json = json.loads(result.decode('utf-8'))
    for index, item in enumerate(result_json):
        api_result_info_list.extend(process_each_comment(item, file_doc_info, api_info))
        if index == len(result_json) - 1:
            api_result_info_list.extend(process_tag_missing(item, file_doc_info, api_info))
        file_doc_info.is_in_group_tag = False      # 初始化group标签判断
        file_doc_info.is_in_file_tag = False       # 初始化file标签判断
    return api_result_info_list


def process_file_doc_info(file_doc_info: FileDocInfo, file_info) -> list:
    '''
    根据文件信息校验文件级错误信息
    '''
    api_result_info_list = []
    # 处理group说明
    if file_doc_info.group_name is None:
        api_result_info = create_api_result_info_by_doc(
            ErrorType.WRONG_SCENE, ErrorMessage.ERROR_FILE_LOSE_ONE, ['group doc'], file_info)
        api_result_info_list.append(api_result_info)
    else:
        # 判断group标签的结尾
        if not file_doc_info.has_group_end:
            api_result_info = create_api_result_info_by_doc(
                ErrorType.WRONG_SCENE, ErrorMessage.ERROR_FILE_HAS_ONE_LOSE_OTHER,
                ['group tag', 'end tag }'], file_info)
            api_result_info_list.append(api_result_info)
    # 处理file说明
    if file_doc_info.file_name is None:
        api_result_info = create_api_result_info_by_doc(
            ErrorType.WRONG_SCENE, ErrorMessage.ERROR_FILE_LOSE_ONE, ['file doc'], file_info)
        api_result_info_list.append(api_result_info)

    return api_result_info_list
