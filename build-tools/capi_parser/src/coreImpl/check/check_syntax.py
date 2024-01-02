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

import re
import os
import subprocess
from typedef.check.check import ApiResultInfo, ErrorMessage, ErrorType, LogType, ErrorLevel


def check_syntax(file_path):
    cmd_list = ['clang']
    if os.path.exists(r'.\sysroot'):
        args = ['-I{}'.format(dir_path) for dir_path, _, _ in os.walk(r'.\sysroot')]
        cmd_list.extend(args)
    cmd_list.append('-std=c99')
    result_list = []
    command = cmd_list + [file_path]
    run_result = subprocess.run(command, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    result_list.extend(processing_data(run_result, file_path))
    return result_list


def processing_data(run_result, result_file):
    api_result_info_list = []
    for run_result_child in run_result.stderr.decode().split('^'):
        result_child = run_result_child.replace('~', '')
        ret = re.findall('\\d+:\\d+', result_child)
        if len(ret) != 0:
            error_message = get_specified_string(result_child)
            if len(error_message) == 1:
                continue
            position = ret[0]
            api_result_info = ApiResultInfo(ErrorType.SYNTAX_ERRORS.value,
                                            error_message[1], result_file)
            line_column = get_line_and_column(position)
            api_result_info.set_location_line(line_column[0])
            api_result_info.set_location_column(line_column[1])
            api_result_info.set_location(result_file)
            api_result_info.set_type(LogType.LOG_API.value)
            api_result_info.set_level(ErrorLevel.LOW.value)
            api_result_info.set_file_name(result_file)
            api_result_info_list.append(api_result_info)
    return api_result_info_list


def get_line_and_column(location):
    if location is not None:
        return location.split(':')
    return ['', '']


def get_original(result_child: str):
    if len(result_child) == 0:
        return result_child
    original = result_child.lstrip().split("\r\n")
    if len(original) == 2:
        return ''
    if len(original) == 3:
        return original[1]
    if len(original) == 4:
        return original[2]
    return ''


def get_specified_string(target_string):
    message_type = 'error'
    function_result = []
    pattern = r'error: (.*?)\r\n'
    matches = re.findall(pattern, target_string, re.DOTALL)
    if len(matches) == 0:
        pattern = r'warning: (.*?)\r\n'
        matches = re.findall(pattern, target_string, re.DOTALL)
        message_type = 'warning'
    if len(matches) == 0:
        pattern = r'note: (.*?)\r\n'
        matches = re.findall(pattern, target_string, re.DOTALL)
        message_type = 'note'
    function_result.append(message_type)
    for match in matches:
        function_result.append(match)
    if len(function_result[1]) == 0:
        function_result.append('')
    return function_result


def get_file_path(file_path: str):
    if len(file_path) == 0:
        return file_path
    path_split_len = len(file_path.split('\r\n'))
    path_list = file_path.split('\r\n')
    if path_split_len == 1:
        return file_path
    if path_split_len == 2:
        return path_list[1]
    if path_split_len == 3:
        return path_list[2]
    return ''
