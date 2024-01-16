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

import filecmp
import json
import os
from collections import OrderedDict
import openpyxl as op
from coreImpl.parser.parser import parser_include_ast
from coreImpl.diff.diff_processor_node import judgment_entrance
from typedef.diff.diff import OutputJson

global_old_dir = ''
global_new_dir = ''
diff_info_list = []


def start_diff_file(old_dir, new_dir):
    result_info_list = global_assignment(old_dir, new_dir)
    generate_excel(result_info_list)
    result_json = result_to_json(result_info_list)
    write_in_txt(result_json, r'./ndk_diff.txt')
    print(result_json)


def generate_excel(result_info_list):
    data = []
    for diff_info in result_info_list:
        info_data = []
        info_data.append(diff_info.api_name)
        info_data.append(diff_info.api_line)
        info_data.append(diff_info.api_column)
        info_data.append(diff_info.api_file_path)
        info_data.append(diff_info.api_type)
        info_data.append(diff_info.diff_type.name)
        info_data.append(diff_info.diff_message)
        info_data.append(diff_info.old_api_full_text)
        info_data.append(diff_info.new_api_full_text)
        result = '是' if diff_info.is_compatible else '否'
        info_data.append(result)
        data.append(info_data)
    wb = op.Workbook()
    ws = wb['Sheet']
    ws.append(['api名称', '所在行', '所在列', '所在文件', '节点类型',
               '变更类型', '变更信息', '旧版节点内容', '新版节点内容', '兼容'])
    for i in range(len(data)):
        d = data[i][0], data[i][1], data[i][2], data[i][3], data[i][4],\
            data[i][5], data[i][6], data[i][7], data[i][8], data[i][9]
        ws.append(d)
    wb.save('diff.xlsx')


def global_assignment(old_dir, new_dir):
    global diff_info_list
    diff_info_list = []
    global global_old_dir
    global_old_dir = old_dir
    global global_new_dir
    global_new_dir = new_dir
    do_diff(old_dir, new_dir)
    return diff_info_list


def result_to_json(result_info_list):
    result_json = []
    for diff_info in result_info_list:
        result_json.append(OutputJson(diff_info))
    return json.dumps(result_json, default=lambda obj: obj.__dict__, indent=4)


def write_in_txt(check_result, output_path):
    with open(output_path, 'w', encoding='utf-8') as fs:
        fs.write(check_result)
        fs.close()


def do_diff(old_dir, new_dir):
    old_file_list = os.listdir(old_dir)
    new_file_list = os.listdir(new_dir)
    diff_list(old_file_list, new_file_list, old_dir, new_dir)


def get_file_ext(file_name):
    return os.path.splitext(file_name)[1]


def diff_list(old_file_list, new_file_list, old_dir, new_dir):
    all_list = set(old_file_list + new_file_list)
    if len(all_list) == 0:
        return
    for target_file in all_list:
        if (get_file_ext(target_file) != '.h'
                and get_file_ext(target_file) != ''):
            continue
        if (target_file in old_file_list
                and target_file not in new_file_list):
            diff_file_path = os.path.join(old_dir, target_file)
            del_old_file(diff_file_path)
        if (target_file in new_file_list
                and target_file not in old_file_list):
            diff_file_path = os.path.join(new_dir, target_file)
            add_new_file(diff_file_path)
        get_same_file_diff(target_file, old_file_list, new_file_list, old_dir, new_dir)


def add_new_file(diff_file_path):
    if os.path.isdir(diff_file_path):
        add_file(diff_file_path)
    else:
        result_map = parse_file_result(parser_include_ast(global_new_dir, [diff_file_path], flag=1))
        for new_info in result_map.values():
            diff_info_list.extend(judgment_entrance(None, new_info))


def del_old_file(diff_file_path):
    if os.path.isdir(diff_file_path):
        del_file(diff_file_path)
    else:
        result_map = parse_file_result(parser_include_ast(global_old_dir, [diff_file_path], flag=0))
        for old_info in result_map.values():
            diff_info_list.extend(judgment_entrance(old_info, None))


def get_same_file_diff(target_file, old_file_list, new_file_list, old_dir, new_dir):
    if (target_file in old_file_list
            and target_file in new_file_list):
        if (os.path.isdir(os.path.join(old_dir, target_file))
                and os.path.isdir(os.path.join(new_dir, target_file))):
            old_child_dir = os.path.join(old_dir, target_file)
            new_child_dir = os.path.join(new_dir, target_file)
            do_diff(old_child_dir, new_child_dir)
        if (os.path.isfile(os.path.join(old_dir, target_file))
                and os.path.isfile(os.path.join(new_dir, target_file))):
            old_target_file = os.path.join(old_dir, target_file)
            new_target_file = os.path.join(new_dir, target_file)
            if not filecmp.cmp(old_target_file, new_target_file):
                get_file_result_diff(old_target_file, new_target_file)


def get_file_result_diff(old_target_file, new_target_file):
    old_file_result_map = parse_file_result(parser_include_ast(global_old_dir, [old_target_file], flag=0))
    new_file_result_map = parse_file_result(parser_include_ast(global_new_dir, [new_target_file], flag=1))
    merged_dict = OrderedDict(list(old_file_result_map.items()) + list(new_file_result_map.items()))
    all_key_list = merged_dict.keys()
    for key in all_key_list:
        diff_info_list.extend(judgment_entrance(old_file_result_map.get(key), new_file_result_map.get(key)))


def del_file(dir_path):
    file_list = os.listdir(dir_path)
    for i in file_list:
        if get_file_ext(i) != '.h' and get_file_ext(i) != '':
            continue
        file_path = os.path.join(dir_path, i)
        if os.path.isdir(file_path):
            del_file(file_path)
        if get_file_ext(i) == '.h':
            result_map = parse_file_result(parser_include_ast(global_old_dir, [file_path], flag=0))
            for old_info in result_map.values():
                diff_info_list.extend(judgment_entrance(old_info, None))


def add_file(dir_path):
    file_list = os.listdir(dir_path)
    for i in file_list:
        if get_file_ext(i) != '.h' and get_file_ext(i) != '':
            continue
        file_path = os.path.join(dir_path, i)
        if os.path.isdir(file_path):
            add_file(file_path)
        if get_file_ext(i) == '.h':
            result_map = parse_file_result(parser_include_ast(global_new_dir, [file_path], flag=1))
            for new_info in result_map.values():
                diff_info_list.extend(judgment_entrance(None, new_info))


def parse_file_result(result):
    result_map = {}
    for root_node in result:
        children_list = root_node['children']
        for children in children_list:
            if children["name"] == '':
                continue
            result_map.setdefault(f'{children["name"]}-{children["kind"]}', children)
        del root_node['children']
        result_map.setdefault(f'{root_node["name"]}-{root_node["kind"]}', root_node)
    return result_map
