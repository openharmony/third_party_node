import json
import os
import pandas as pd  # 用于生成表格


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
        for item1 in it["children"]:  # 抛开根节点
            if (item1["kind"] == 'FUNCTION_DECL' or item1["kind"] == 'VAR_DECL') and item1["is_extern"]:
                item = filter_func(item1)
                result_api.append(item)
    return result_api


def get_parm(item, parm):
    if item["parm"]:
        for i in range(len(item["parm"])):
            if item["parm"][i]["kind"] != 'PARM_DECL':
                continue
            else:
                str_parm = item["parm"][i]["type"] + ' ' + item["parm"][i]["name"]
                parm.append(str_parm)
        item["parm"] = parm


def filter_func(item):
    del item["is_extern"]  # 剔除is_extern键值对，过滤后都是extern
    del item["comment"]
    item["location_path"] = item["location"]["location_path"]
    item["location"] = item["location"]["location_line"]
    if item["kind"] == 'FUNCTION_DECL':
        item["kind"] = '函数类型'
        parm = []  # 装函数参数
        if "parm" in item:
            get_parm(item, parm)
    else:
        item["kind"] = '变量类型'
    return item


def generate_excel(array, name, only_file1, only_file2):
    # 将列表转成DataFrame，并且按列的方式读取数据(orient='columns')
    pf = pd.DataFrame.from_dict(array, orient='columns')
    pf1 = pd.DataFrame(only_file1)
    pf2 = pd.DataFrame(only_file2)

    # 将列名换为中文名
    columns_map = {
        'name': '名称',
        'kind': '节点类型',
        'type': '类型',
        'gn_path': 'gn文件路径',
        'location_path': '文件相对路径',
        'location': '位置行',
        'return_type': '返回类型',
        'parm': '参数'
    }

    pf.rename(columns=columns_map, inplace=True)
    with pd.ExcelWriter(name) as writer:  # 生成该表格
        pf.to_excel(writer, sheet_name='对比结果', index=False)
        pf1.to_excel(writer, sheet_name='生成json独有', index=False)
        pf2.to_excel(writer, sheet_name='原json独有', index=False)


def increase_sheet(array, name, sheet):
    pf = pd.DataFrame(array)
    writer = pd.ExcelWriter(name, mode='a', engine='openpyxl', if_sheet_exists='new')
    pf.to_excel(writer, sheet_name=sheet, index=False)


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
        result_list, only_file1, only_file2 = compare_json_file(json_file1, item)

    return result_list, head_name, only_file1, only_file2  # 返回对比数据，和所需表格名
