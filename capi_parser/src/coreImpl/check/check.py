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
from typedef.check.check import ApiResultInfo, FileDocInfo, OutputTxt
from coreImpl.check.check_doc import process_comment, process_file_doc_info
from coreImpl.check.check_name import check_file_name, check_ndk_name


def process_api_json(api_info, file_doc_info: FileDocInfo, api_result_info_list):
    api_result_info_list.extend(check_ndk_name(api_info))
    if 'comment' in api_info.keys():
        comment = api_info['comment']
        api_result_info_list.extend(
            process_comment(comment, file_doc_info, api_info))
    child_node_list = get_api_info_child(api_info)
    for child_node in child_node_list:
        process_api_json(child_node, file_doc_info, api_result_info_list)


def get_api_info_child(api_info):
    if 'children' in api_info.keys():
        return api_info['children']
    if 'members' in api_info.keys():
        return api_info['members']
    if 'parm' in api_info.keys():
        return api_info['parm']
    return []


def process_file_json(file_info, api_result_info_list):
    api_result_info_list.extend(check_file_name(file_info['name']))
    apis = file_info['children']
    file_doc_info = FileDocInfo()
    for api in apis:
        process_api_json(api, file_doc_info, api_result_info_list)
    api_result_info_list.extend(process_file_doc_info(file_doc_info, file_info))


def process_all_json(python_obj) -> list[ApiResultInfo]:
    api_result_info_list = []
    for file_info in python_obj:
        process_file_json(file_info, api_result_info_list)
    return api_result_info_list


def write_in_txt(check_result):
    txtResul = []
    for result in check_result:
        location = '{}(line:{}, col:{})'.format(result.location, result.locationLine, result.locationColumn)
        message = 'API check error of [{}]:{}'.format(result.errorType['description'], result.errorInfo)
        txtResul.append(OutputTxt(result.errorType['id'], result.level, location, result.fileName, message))
    txtResul.append('api_check: false')
    result_json = json.dumps(txtResul, default=lambda obj: obj.__dict__, indent=4)
    fs = open(r'./Error.txt', 'w', encoding='utf-8')
    fs.write(result_json)
    fs.close()


def curr_entry(file_path):
    with open('./src/coreImpl/check/data.json') as json_file:
        python_obj = json.load(json_file)
    check_result = process_all_json(python_obj)
    if len(check_result) == 0:
        return
    write_in_txt(check_result)


def get_md_files(url) -> list[str]:
    file = open(url, "r")
    file_list = []
    line = file.readline()
    while line:
        file_list.append(line)
        line = file.readline()
    file.close()
    return file_list
