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
import os
import openpyxl


def compare_json_file(js_file1, js_file2):  # 获取对比结果
    with open(js_file1, 'r', encoding='utf-8') as js1:
        data1 = json.load(js1)
    with open(js_file2, 'r') as js2:
        data2 = json.load(js2)
    compare_result = []
    only_file1 = []  # 装file1独有的
    result_api = filter_compare(data1)
    for it in result_api:
        name1 = it["name"]
        key = 0
        for item in data2:
            if item["name"] == name1:
                key = 1
                compare_result.append(it)
                break
        if key == 0:
            only_file1.append(it)
    only_file2 = get_difference_data(compare_result, data2)  # 获取file2独有的
    js1.close()
    js2.close()
    return compare_result, only_file1, only_file2


def get_difference_data(compare_result, data2):
    only_file2 = []
    for item in data2:
        name2 = item["name"]
        key = 0
        for it in compare_result:
            name1 = it["name"]
            if name2 == name1:
                key = 1
                break
        if key == 0:
            only_file2.append(item)
    return only_file2


def filter_compare(data1):  # 获取函数和变量
    result_api = []
    for it in data1:
        get_result_api(it, result_api)
    return result_api


def get_result_api(file_data, result_api):
    if 'children' in file_data:
        for item1 in file_data["children"]:  # 抛开根节点
            if (item1["kind"] == 'FUNCTION_DECL' or item1["kind"] == 'VAR_DECL') and item1["is_extern"]:
                item = filter_func(item1)
                result_api.append(item)


def get_parm(item, parm):
    if item["parm"]:
        for i in range(len(item["parm"])):
            if item["parm"][i]["kind"] != 'PARM_DECL':
                continue
            else:
                str_parm = '{} {}'.format(item["parm"][i]["type"], item["parm"][i]["name"])
                parm.append(str_parm)
        item["parm"] = parm


def filter_func(item):
    del item["is_extern"]  # 剔除is_extern键值对，过滤后都是extern
    del item["comment"]
    if "type_ref" in list(item.keys()):
        del item["type_ref"]
    if "children" in list(item.keys()):
        del item["children"]

    item["location_path"] = item["location"]["location_path"]
    item["location"] = item["location"]["location_line"]
    if item["kind"] == 'FUNCTION_DECL':
        item["kind"] = '函数类型'
        parm = []  # 装函数参数
        if "parm" in item:
            get_parm(item, parm)
    else:
        item["kind"] = '变量类型'
        del item["is_const"]
    return item


def generate_excel(array, name, only_file1, only_file2):
    # 将列名换为中文名
    columns_map = {
        'name': '名称',
        'kind': '节点类型',
        'type': '类型',
        'gn_path': 'gn文件路径',
        "node_content": '节点内容',
        'location': '位置行',
        'return_type': '返回类型',
        'parm': '参数',
        'location_path': '文件相对路径',
    }

    workbook = openpyxl.Workbook()
    work_sheet1 = workbook.active
    work_sheet1.title = '对比结果'
    write_dict_to_worksheet(work_sheet1, array, header=columns_map)

    work_sheet2 = workbook.create_sheet('生成json独有')
    write_dict_to_worksheet(work_sheet2, only_file1, header=columns_map)

    work_sheet3 = workbook.create_sheet('原有json独有')
    write_dict_to_worksheet(work_sheet3, only_file2)
    workbook.save(name)


def write_dict_to_worksheet(work_sheet, result_data, header=None):
    if header is None:
        header = {
            'name': '名称'
        }
    row_num = 1
    for col_num, col_value in enumerate(header.values()):
        work_sheet.cell(row_num, col_num + 1, col_value)

    row_num = 2
    for data in result_data:
        for col_num, col_value in enumerate(data.values()):
            processing_worksheet_data(work_sheet, col_value, row_num, col_num)
        row_num += 1


def processing_worksheet_data(work_sheet, col_value, row_num, col_num):
    if isinstance(col_value, dict):
        param_data = []
        for dict_value in col_value.values():
            if isinstance(dict_value, int):
                dict_value = str(dict_value)
            param_data.append(dict_value)
        param_str = ','.join(param_data)
        work_sheet.cell(row_num, col_num + 1, param_str)
    elif isinstance(col_value, list):
        list_data = ','.join(col_value)
        work_sheet.cell(row_num, col_num + 1, list_data)
    else:
        work_sheet.cell(row_num, col_num + 1, col_value)


def del_repetition_value(only_file_list, compare_list):
    data = []
    for item in only_file_list:
        if item not in compare_list:
            data.append(item)
    return data


def get_json_file(json_file_new, json_file):  # 获取生成的json文件
    json_file1 = r'{}'.format(json_file_new)  # 获取要对比的json文件
    json_file2 = json_file
    head_name = os.path.splitext(json_file1)  # 去掉文件名后缀
    head_name = head_name[0] + '.xlsx'  # 加后缀
    result_list = []
    only_file1 = []
    only_file2 = []
    for item in json_file2:  # 对比每一个json(目录下的)
        # 对比两个json文件
        result_list_part, only_file1_part, only_file2_part = compare_json_file(json_file1, item)
        result_list.extend(result_list_part)
        only_file1.extend(only_file1_part)
        only_file2.extend(only_file2_part)
    only_file1_new = del_repetition_value(only_file1, result_list)
    return result_list, head_name, only_file1_new, only_file2  # 返回对比数据，和所需表格名
