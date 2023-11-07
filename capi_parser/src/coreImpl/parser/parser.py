import os                       # 可用于操作目录文件
import glob                     # 可用于查找指定目录下指定后缀的文件
import re                       # 正则表达是模块--可用于操作文件里面的内容
import shutil                   # 拷贝文件
from coreImpl.parser import parse_include, generating_tables          # 引入解析文件 # 引入得到结果表格文件
import json
from utils.constants import StringConstant


def find_gn_file(directory):                                # 找指定目录下所有GN文件
    gn_files = []
    for root, dirs, files in os.walk(directory):            # dirpath, dirnames, filenames(对应信息)
        for file in files:
            if file.endswith(".gn"):
                gn_files.append(os.path.join(root, file))
    return gn_files


def find_function_file(file, function_name):                                          # 在GN文件中查找指定函数并在有函数名，获取对应sources的值
    with open(file, 'r') as f:
        content = f.read()                                                            # 获取文件内容
        pattern = r'\b' + re.escape(function_name) + r'\b'                            # '\b'确保函数名的完全匹配
        matches = re.findall(pattern, content)
        f.seek(0)                                                                     # 回到文件开始位置
        if len(matches):                                                              # 是否匹配成功
            sources = []                                                              # 转全部匹配的sources的.h(可能不止一个-headers函数)
            f.seek(0)
            end = 0                                                                   # 记录光标
            for i in range(len(matches)):
                # 匹配sources = \[[^\]]*\](匹配方括号内的内容，其中包括一个或多个非右括号字符),\s*：匹配0个或多个空白字符
                pattern = r'sources\s*=\s*\[[^\]]*\]'
                sources_match = re.search(pattern, content)
                if sources_match:
                    sources_value = sources_match.group(0)                            # 获取完整匹配的字符串
                    sources_value = re.sub(r'\s', '', sources_value)      # 去除源字符串的空白字符(换行符)和空格
                    pattern = r'"([^"]+h)"'                                           # 匹配引号中的内容，找对应的.h
                    source = re.findall(pattern, sources_value)
                    sources.extend(source)
                end += sources_match.end()                                            # 每次找完一个sources的.h路径，记录光标结束位置
                f.seek(end)                                                           # 移动光标在该结束位置
                content = f.read()                                                    # 从当前位置读取问价内容，防止重复
            return len(matches) > 0, sources
        else:
            return None, None                                                       # gn文件没有对应的函数


def get_dest_dir(file, function_name):                                                # 获取dest_dir
    with open(file, 'r') as f:
        content = f.read()                                                            # 获取文件内容
        pattern = r'\b' + re.escape(function_name) + r'\b'                            # '\b'确保函数名的完全匹配
        matches = re.findall(pattern, content)
        f.seek(0)
        if matches:
            sources_dir = []
            f.seek(0)
            end = 0
            for i in range(len(matches)):
                pattern = r'dest_dir\s*=\s*"([^"]*)"'
                source_match = re.search(pattern, content)
                if source_match:
                    con = source_match.group(1)
                    con_real = con[1:]
                    sources_dir.append(con)
                    end += source_match.end()  # 每次找完一个sources的.h路径，记录光标结束位置
                    f.seek(end)  # 移动光标在该结束位置
                    content = f.read()
            return sources_dir
        else:
            return None


def find_json_file(gn_file_match):                                               # 找gn文件同级目录下的.json文件
    match_json_file = []
    directory = os.path.dirname(gn_file_match)
    for file in glob.glob(os.path.join(directory, "*.json")):                    # 统计.json文件
        match_json_file.append(file)
    return match_json_file


# def get_

def dire_func(gn_file, func_name):                                               # 统计数据的
    matches_file_total = []                                                      # 统计有ohos_ndk_headers函数的gn文件
    json_file_total = []                                                         # 统计跟含有函数的gn文件同级的json文件
    source_include = []                                                          # 统计sources里面的.h
    length, source = find_function_file(gn_file, func_name)                      # 找到包含函数的gn文件和同级目录下的.json文件
    if length:                                                                   # 保证两个都不为空，source可能为空
        source_include = source                                                  # 获取头文件列表
        matches_file_total.append(gn_file)                                       # 调用匹配函数的函数(说明有对应的函数、source)
        json_file_total.extend(find_json_file(gn_file))                          # 找json

    return matches_file_total, json_file_total, source_include


def change_json_file(dict_data, name):                                                   # 生成json文件
    file_name = name + '_new' + '.json'                                                  # json文件名
    with open(file_name, 'w', encoding='UTF-8') as f:                                    # encoding='UTF-8'能显示中文
        # ensure_ascii=False确保能显示中文，indent=4(格式控制)使生成的json样式跟字典一样
        json.dump(dict_data, f, ensure_ascii=False, indent=4)

    return file_name


def change_abs(include_files, dire_path):                                           # 获取.h绝对路径
    abs_path = []
    for j in range(len(include_files)):                                             # 拼接路径，生成绝对路径
        # os.path.normpath(path):规范或者是格式化路径，它会把所有路径分割符按照操作系统进行替换
        # 把规范路径和gn文件对应的目录路径拼接
        if os.path.isabs(include_files[j]):                                         # 是否是绝对路径，是就拼接路径盘，不是就拼接gn目录路径
            head = os.path.splitdrive(dire_path)                                    # 获取windows盘路径
            include_file = os.path.normpath(include_files[j])
            include_file = include_file.replace('\\\\interface\\sdk_c', StringConstant.REPLACE_WAREHOUSE.value)   # 去掉绝对路径的双\\,替换为interface_sdk_c
            if head:
                include_file = os.path.join(head[0], include_file)                  # 拼接盘和路径
            abs_path.append(include_file)
        else:
            relative_path = os.path.abspath(os.path.join(dire_path, os.path.normpath(include_files[j])))  # ../ .解决
            abs_path.append(relative_path)
    print("头文件绝对路径：\n", abs_path)
    print("=" * 50)
    return abs_path


def get_result_table(json_files, abs_path, lib_path, link_path, gn_path):                    # 进行处理，生成表格
    if json_files:
        file_name = os.path.split(json_files[0])                                    # 取第一个json名，但我是用列表装的
        file_name = os.path.splitext(file_name[1])                                  # 取下标1对应的元素(元组)
        data = parse_include.get_include_file(lib_path, abs_path, link_path, gn_path)        # 获取解析返回的数据
        parse_json_name = change_json_file(data, file_name[0])                      # 生成json文件
        result_list, head_name, only_file1, only_file2 = generating_tables.get_json_file(parse_json_name, json_files)  # 解析完后，传两个json文件，对比两个json文件，最后生成数据表格
        return result_list, head_name, only_file1, only_file2
    else:
        return None, None


def create_dir(sources_dir, gn_file, function_name, link_include_file):
    for i in range(len(sources_dir)):
        directory = sources_dir[i]
        new_dire = os.path.join('sysroot', directory)
        new_dire = os.path.normpath(new_dire)
        if not os.path.exists(new_dire):
            os.makedirs(new_dire)

        else:
            print("目录已存在")
        if new_dire in link_include_file:
            pass
        else:
            link_include_file.append(new_dire)                                                  # 添加链接的头文件
        match_files, json_files, include_files = dire_func(gn_file, function_name)
        dire_path = os.path.dirname(gn_file)                                                 # 获取gn文件路径
        if match_files:
            abs_path = change_abs(include_files, dire_path)                                  # 接收.h绝对路径
            for j in range(len(abs_path)):
                shutil.copy(abs_path[j], new_dire)
        else:
            print("在create_dir函数中，原因：gn文件条件不满足")


def link_include(directory_path, function_names, link_include_file):
    gn_file_total = find_gn_file(directory_path)                                             # 查找gn文件
    for i in range(len(gn_file_total)):                                                      # 处理每个gn文件
        sources_dir = get_dest_dir(gn_file_total[i], function_names)
        create_dir(sources_dir, gn_file_total[i], function_names, link_include_file)


def main_entrance(directory_path, function_names, lib_path, link_path):                      # 主入口
    gn_file_total = find_gn_file(directory_path)                                             # 查找gn文件

    for i in range(len(gn_file_total)):                                                      # 处理每个gn文件
        match_files, json_files, include_files = dire_func(gn_file_total[i], function_names)
        dire_path = os.path.dirname(gn_file_total[i])                                        # 获取gn文件路径

        print("目录路径： {}".format(dire_path))
        print("同级json文件：\n", json_files)
        print("头文件：\n", include_files)

        if match_files:                                                                      # 符合条件的gn文件
            abs_path = change_abs(include_files, dire_path)                                  # 接收.h绝对路径
            result_list, head_name, only_file1, only_file2 = get_result_table(json_files, abs_path, lib_path, link_path, dire_path)    # 接收对比结果信息
            generating_tables.generate_excel(result_list, head_name, only_file1, only_file2)    # 转为表格
            if result_list:
                print("有匹配项，已生成表格")
            else:
                print("没有匹配项 or gn文件下无json文件")
        else:
            print("gn文件无header函数")


def copy_std_lib(link_include_file):
    std_include = r'sysroot\ndk_musl_include_files'
    if not os.path.exists(std_include):
        shutil.copytree(r'third_party\musl\ndk_musl_include', std_include)
    link_include_file.append(std_include)


def find_include(link_include_path):
    for dir_path, dir_name, file_name in os.walk('sysroot\\$ndk_headers_out_dir'):
        link_include_path.append(dir_path)


def parser(directory_path):                                                                  # 目录路径
    function_name = StringConstant.FUNK_NAME.value                                           # 匹配的函数名
    libclang_path = StringConstant.LIB_CLANG_PATH.value                                      # 共享库路径

    link_include_path = []                                                                   # 装链接头文件路径
    copy_std_lib(link_include_path)                                                          # ndk头文件移到sysroot中
    find_include(link_include_path)
    link_include(directory_path, function_name, link_include_path)

    main_entrance(directory_path, function_name, libclang_path, link_include_path)           # 调用入口函数

