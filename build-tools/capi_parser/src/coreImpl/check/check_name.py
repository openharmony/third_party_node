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
import os.path
import re
from typedef.check.check import ApiResultInfo, ErrorMessage, ErrorType, LogType, ErrorLevel


def check_large_hump(api_info):
    return processing_check_data('LARGE_HUMP', api_info)


def check_function_name(api_info):
    api_result_info_list = []
    name = api_info['name']
    self_developed_function_result = re.match(r'^[OH|OS]+([\_]([A-Z]+[a-z0-9]*)+)*$', name)
    ordinary_function_result = re.match(r'^([A-Z][a-z0-9]*)*$', name)
    if self_developed_function_result or ordinary_function_result is not None:
        return api_result_info_list
    else:
        api_result_info = ApiResultInfo(ErrorType.NAMING_ERRORS.value,
                                        ErrorMessage[api_info['kind']].value.replace('$$', name), name)
        api_result_info.set_location_line(api_info['location']['location_line'])
        api_result_info.set_location_column(api_info['location']['location_column'])
        api_result_info.set_location(api_info['location']['location_path'])
        api_result_info.set_type(LogType.LOG_API.value)
        api_result_info.set_level(ErrorLevel.LOW.value)
        api_result_info.set_file_name(api_info['location']['location_path'])
        api_result_info_list.append(api_result_info)
        return api_result_info_list


def check_variable_name(api_info):
    is_const = api_info['is_const']
    if is_const:
        return processing_check_data('ALL_UPPERCASE_HUMP', api_info)
    else:
        return processing_check_data('SMALL_HUMP', api_info)


def check_small_hump(api_info):
    return processing_check_data('SMALL_HUMP', api_info)


def check_all_uppercase_hump(api_info):
    return processing_check_data('ALL_UPPERCASE_HUMP', api_info)


def check_global_variable(api_info):
    return processing_check_data('GLOBAL_VARIABLE', api_info)


def check_file_name(file_path):
    api_result_info_list = []
    file_name = os.path.basename(file_path)
    result = re.match(CheckName['FILE_NAME'].value, file_name)
    if result is None:
        error_info = ErrorMessage.TRANSLATION_UNIT.value.replace('$$', file_name)
        api_result_info = ApiResultInfo(ErrorType.NAMING_ERRORS.value, error_info, '')
        api_result_info.set_type(LogType.LOG_FILE.value)
        api_result_info.set_file_name(file_path)
        api_result_info.set_location(file_path)
        api_result_info.set_level(ErrorLevel.LOW.value)
        api_result_info_list.append(api_result_info)
    return api_result_info_list


def processing_check_data(function_type, api_info):
    api_result_info_list = []
    name = api_info['name']
    result = re.match(CheckName[function_type].value, name)
    if result is None:
        api_result_info = ApiResultInfo(ErrorType.NAMING_ERRORS.value,
                                        ErrorMessage[api_info['kind']].value.replace('$$', name), name)
        api_result_info.set_location_line(api_info['location']['location_line'])
        api_result_info.set_location_column(api_info['location']['location_column'])
        api_result_info.set_location(api_info['location']['location_path'])
        api_result_info.set_type(LogType.LOG_API.value)
        api_result_info.set_level(ErrorLevel.LOW.value)
        api_result_info.set_file_name(api_info['location']['location_path'])
        api_result_info_list.append(api_result_info)
    return api_result_info_list


class CheckName(enum.Enum):
    LARGE_HUMP = r'^([A-Z][a-z0-9]*)*([\_]([A-Z][a-z0-9]*)*)*$'
    SMALL_HUMP = r'^([a-z][A-Z0-9]*)*([\_]([a-z][A-Z0-9]*)*)*$'
    ALL_UPPERCASE_HUMP = r'^[A-Z]+[0-9]*([\_][A-Z0-9]*)*$'
    GLOBAL_VARIABLE = r'^g_([a-z][A-Z0-9]*)*$'
    FILE_NAME = r'^[a-z]+[a-z0-9]+([\_][a-z0-9]+)*\.h$'


process_tag_function = {
    'FUNCTION_DECL': check_function_name,
    'STRUCT_DECL': check_large_hump,
    'ENUM_DECL': check_large_hump,
    'UNION_DECL': check_large_hump,
    'VAR_DECL': check_variable_name,
    'PARM_DECL': check_small_hump,
    'FIELD_DECL': check_small_hump,
    'MACRO_DEFINITION': check_all_uppercase_hump,
    'ENUM_CONSTANT_DECL': check_all_uppercase_hump,
}


def check_ndk_name(api_info):
    api_result_info_list = []
    kind = api_info['kind']
    if kind not in process_tag_function.keys():
        return api_result_info_list
    name_process = process_tag_function[kind]
    return name_process(api_info)
