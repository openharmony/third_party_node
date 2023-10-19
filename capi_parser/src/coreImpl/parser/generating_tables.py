import json
import pandas as pd                   # 用于生成表格
import os


def compare_json_file(js_file1, js_file2):                                      # 获取对比结果
    with open(js_file1, 'r', encoding='utf-8') as js1:
        data1 = json.load(js1)
    with open(js_file2, 'r') as js2:
        data2 = json.load(js2)
    compare_result = []
    result_api = filter_compare(data1)
    for i in range(len(result_api)):
        name1 = result_api[i]["name"]
        for item in data2:
            if item["name"]:
                name2 = item["name"]
                if name1 == name2:
                    compare_result.append(result_api[i])
    return compare_result


def filter_compare(data1):                                                      # 获取函数和变量
    result_api = []
    for i in range(len(data1)):
        for item1 in data1[i]["children"]:  # 抛开根节点
            if (item1["kind"] == 'FUNCTION_DECL' or item1["kind"] == 'VAR_DECL') and item1["is_extern"]:
                if item1["kind"] == 'FUNCTION_DECL':
                    del item1["parm"]                                           # 剔除参数键值对，可以不需要
                del item1["is_extern"]                                          # 剔除is_extern键值对，过滤后都是extern
                result_api.append(item1)

    return result_api


def generate_excel(array, name):
    pf = pd.DataFrame.from_dict(array, orient='columns')                # 将列表转成DataFrame，并且按列的方式读取数据(orient='columns')

    order = ['name', 'kind', 'type', 'return_type']                     # 指定字段顺序
    pf = pf[order]
    columns_map = {                                                     # 将列名换为中文名
        'name': '名称',
        'kind': '节点类型',
        'type': '类型',
        'return_type': '返回类型',
    }

    pf.rename(columns=columns_map, inplace=True)

    pf.to_excel(name, index=False)                                      # 生成该表格


def get_json_file(json_file_new, json_file):                                              # 获取生成的json文件
    json_file1 = r'{}'.format(json_file_new)                                              # 获取要对比的json文件
    json_file2 = json_file
    head_name = os.path.splitext(json_file1)                                              # 去掉文件名后缀
    head_name = head_name[0] + '.xlsx'                                                    # 加后缀
    result_list = []
    for i in range(len(json_file2)):                                                      # 对比每一个json(目录下的)
        result_list = compare_json_file(json_file1, json_file2[i])                        # 对比两个json文件

    return result_list, head_name                                                         # 返回对比数据，和所需表格名


