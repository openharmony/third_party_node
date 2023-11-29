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

import re
import os
import clang.cindex
from clang.cindex import Config
from clang.cindex import Index
from clang.cindex import CursorKind
from clang.cindex import TypeKind
from utils.constants import StringConstant
from utils.constants import RegularExpressions


def find_parent(cursor):  # 获取父节点
    cursor_parent = cursor.semantic_parent
    if cursor_parent is not None:
        if cursor_parent.kind == CursorKind.VAR_DECL:  # 父节点为VAR_DECL 用于整型变量节点
            return cursor_parent.kind

        # 用于判断里面成员属于那类
        elif cursor_parent.kind == CursorKind.STRUCT_DECL or cursor_parent.kind == CursorKind.UNION_DECL:
            return cursor_parent.kind
        else:
            parent_kind = processing_root_parent(cursor_parent)
            return parent_kind
    return None


def processing_root_parent(cursor_parent):
    parent = cursor_parent.semantic_parent
    if parent is not None:
        if parent.type.kind == TypeKind.INVALID:
            parent_kind = CursorKind.TRANSLATION_UNIT
            return parent_kind
        else:
            return parent.kind
    return None


def processing_no_child(cursor, data):  # 处理没有子节点的节点
    if cursor.kind == CursorKind.INTEGER_LITERAL:  # 整型字面量类型节点，没有子节点
        parent_kind = find_parent(cursor)  # 判断是属于那类的
        if parent_kind:
            if parent_kind == CursorKind.STRUCT_DECL:
                data["name"] = 'struct_int_no_spelling'
            elif parent_kind == CursorKind.UNION_DECL:
                data["name"] = 'union_int_no_spelling'
            elif parent_kind == CursorKind.ENUM_DECL:
                data["name"] = 'enum_int_no_spelling'
            elif parent_kind == CursorKind.VAR_DECL:
                data["name"] = 'var_int_no_spelling'
            else:
                data["name"] = "integer_no_spelling"
            tokens = cursor.get_tokens()
            for token in tokens:
                data["integer_value"] = token.spelling  # 获取整型变量值


def get_complex_def(tokens_new, count_token, tokens, data):
    count = 1
    logo = 0
    logo_com = 0
    count_com = 0
    for token_2 in tokens_new:
        if token_2.spelling == ')':
            logo = 1
            break
        else:
            count += 1
    if count_token == count:
        pass
    elif logo == 1:  # 获取复合型宏定义宏名
        logo_com = logo
        count_com = count + 1
        tokens_name = tokens[:count + 1]
        data["name"] = ''.join([token.spelling for token in tokens_name])
    return logo_com, count_com


def processing_complex_def(tokens, data):  # 处理复合型宏
    tokens_new = tokens[1:]  # 跳过正常宏名
    logo_com = 0  # 记录复合型，复合型文本也得根据这个
    count_com = 0
    count_token = len(tokens_new)  # value ()
    for token in tokens_new:
        if token.kind.name == 'KEYWORD':
            break
        if token.kind.name == 'IDENTIFIER':
            logo_com, count_com = get_complex_def(tokens_new, count_token, tokens, data)
    get_def_text(tokens, data, logo_com, count_com)  # 获取宏文本


def get_def_text(tokens, data, logo_compose, count_compose):  # 获取宏文本
    if logo_compose == 1:
        marco_expansion = ''.join([token.spelling for token in tokens[count_compose:]])  # 获取宏文本,有就记录，没有不管
        if marco_expansion:
            data["text"] = marco_expansion
        else:
            pass
    else:
        marco_expansion = ''.join([token.spelling for token in tokens[1:]])  # 获取宏文本,有就记录，没有不管
        if marco_expansion:
            data["text"] = marco_expansion
        else:
            pass


def get_token(cursor):
    tokens = []
    for token in cursor.get_tokens():
        tokens.append(token.spelling)

    return tokens


def judgment_extern(cursor, data):  # 判断是否带有extern
    tokens = get_token(cursor)
    if cursor.kind == CursorKind.FUNCTION_DECL:
        if 'static' in tokens or 'deprecated' in tokens:
            is_extern = False
        else:
            is_extern = True
    elif cursor.kind == CursorKind.VAR_DECL:
        if 'extern' in tokens:
            is_extern = True
        else:
            is_extern = False
        if 'const' in tokens:
            data["is_const"] = True
        else:
            data["is_const"] = False
    else:
        is_extern = True

    data["is_extern"] = is_extern


def binary_operator(cursor, data):  # 二元操作符处理
    data["name"] = "binary_ope_no_spelling"
    tokens = cursor.get_tokens()
    spelling_arr = ['<<', '>>', '+', '-', '*', '/']
    for token in tokens:
        if token.spelling in spelling_arr:
            data["operator"] = token.spelling


def distinction_member(cursor, data):  # 区别结构体和联合体成员
    parent_kind = find_parent(cursor)  # 查找父节点类型
    if parent_kind:
        if parent_kind == CursorKind.UNION_DECL:
            data["member"] = "union_member"
        elif parent_kind == CursorKind.STRUCT_DECL:
            data["member"] = "struct_member"


def processing_parm(cursor, data):  # 函数参数节点处理
    if cursor.spelling:  # 函数参数是否带参数名
        data["name"] = cursor.spelling
    else:
        data["name"] = "arg_no_spelling"

    if cursor.type.get_pointee().kind == TypeKind.FUNCTIONPROTO:  # 参数为函数指针，获取对应的返回类型
        data["func_pointer_result_type"] = cursor.type.get_pointee().get_result().spelling


def processing_enum(cursor, data):  # 获取枚举值
    data["value"] = cursor.enum_value


def processing_def(cursor, data):  # 处理宏定义
    marco_ext = cursor.extent
    tokens = cursor.translation_unit.get_tokens(extent=marco_ext)  # 找到对应的宏定义位置
    tokens = list(tokens)  # Generator转为list
    processing_complex_def(tokens, data)  # 获取宏名和宏文本
    data["type"] = "def_no_type"


def processing_func(cursor, data):  # 处理函数
    data["return_type"] = cursor.result_type.spelling  # 增加返回类型键值对
    judgment_extern(cursor, data)


def processing_type(cursor, data):  # 没有类型的节点处理
    if cursor.kind == CursorKind.MACRO_INSTANTIATION:  # 也属于宏定义 --宏引用
        data["type"] = "insta_no_type"

    elif cursor.kind == CursorKind.INCLUSION_DIRECTIVE:  # 头文件也没type，规定
        data["type"] = "inclusion_no_type"


def processing_name(cursor, data):  # 没有名的节点处理
    if cursor.kind == CursorKind.PAREN_EXPR:  # 括号表达式()
        data["paren"] = "()"
        data["name"] = "paren_expr_no_spelling"

    elif cursor.kind == CursorKind.UNEXPOSED_EXPR:  # 未公开表达式，用于表示未明确定义的表达式
        data["name"] = "unexposed_expr_no_spelling"


def processing_char(cursor, data):  # 字符节点处理
    tokens = list(cursor.get_tokens())
    char_value = (tokens[0].spelling.strip("'"))
    data["name"] = char_value


special_node_process = {
    CursorKind.ENUM_CONSTANT_DECL.name: processing_enum,
    CursorKind.MACRO_DEFINITION.name: processing_def,
    CursorKind.FUNCTION_DECL.name: processing_func,
    CursorKind.VAR_DECL.name: judgment_extern,
    CursorKind.PARM_DECL.name: processing_parm,
    CursorKind.FIELD_DECL.name: distinction_member,
    CursorKind.MACRO_INSTANTIATION.name: processing_type,
    CursorKind.INCLUSION_DIRECTIVE.name: processing_type,
    CursorKind.BINARY_OPERATOR.name: binary_operator,
    CursorKind.PAREN_EXPR.name: processing_name,
    CursorKind.UNEXPOSED_EXPR.name: processing_name,
    CursorKind.CHARACTER_LITERAL.name: processing_char
}


def processing_special_node(cursor, data, key, gn_path=None):  # 处理需要特殊处理的节点
    if key == 0:
        location_path = cursor.spelling
        kind_name = CursorKind.TRANSLATION_UNIT.name
    else:
        location_path = cursor.location.file.name
        kind_name = cursor.kind.name

    loc = {
        "location_path": '{}'.format(location_path),
        "location_line": cursor.location.line,
        "location_column": cursor.location.column
    }
    if gn_path:
        relative_path = os.path.relpath(location_path, gn_path)  # 获取头文件相对路
        loc["location_path"] = relative_path
    data["location"] = loc
    if kind_name in special_node_process.keys():
        node_process = special_node_process[kind_name]
        node_process(cursor, data)  # 调用对应节点处理函数


def node_extent(cursor, current_file):
    start_offset = cursor.extent.start.offset
    end_offset = cursor.extent.end.offset
    with open(current_file, 'r', encoding='utf=8') as f:
        f.seek(start_offset)
        content = f.read(end_offset - start_offset)

    extent = {
        "start_offset": start_offset,
        "end_offset": end_offset,
        "content": content
    }
    f.close()
    return extent


def ast_to_dict(cursor, current_file, gn_path=None, comment=None, key=0):  # 解析数据的整理
    # 通用
    data = {
        "name": cursor.spelling,
        "kind": '',
        "type": cursor.type.spelling,
        "gn_path": gn_path,
        "node_content": {},
        "comment": ''
    }
    if cursor.raw_comment:  # 是否有注释信息，有就取，没有过
        data["comment"] = cursor.raw_comment
    else:
        data["comment"] = 'none_comment'

    if key == 0:
        data["kind"] = CursorKind.TRANSLATION_UNIT.name
        if comment:
            data["comment"] = comment
        if gn_path:
            relative_path = os.path.relpath(cursor.spelling, gn_path)
            data["name"] = relative_path
    else:
        content = node_extent(cursor, current_file)
        data["node_content"] = content
        data["kind"] = cursor.kind.name

    processing_special_node(cursor, data, key, gn_path)  # 节点处理
    children = list(cursor.get_children())  # 判断是否有子节点，有就追加children，没有根据情况来
    if len(children) > 0:
        if key != 0:
            if cursor.kind == CursorKind.FUNCTION_DECL:  # 函数参数
                name = "parm"
            elif (cursor.kind == CursorKind.ENUM_DECL
                  or cursor.kind == CursorKind.STRUCT_DECL
                  or cursor.kind == CursorKind.UNION_DECL):
                name = "members"
            else:
                name = "children"
        else:
            name = "children"
        data[name] = []

        for child in children:
            # 剔除多余宏定义和跳过UNEXPOSED_ATTR节点
            if child.location.file is not None and child.kind != CursorKind.UNEXPOSED_ATTR \
                    and child.location.file.name == current_file:
                processing_ast_node(child, current_file, data, name, gn_path)
    else:
        processing_no_child(cursor, data)  # 处理没有子节点的节点
    return data


def processing_ast_node(child, current_file, data, name, gn_path):
    child_data = ast_to_dict(child, current_file, gn_path, key=1)
    if child.kind == CursorKind.TYPE_REF:
        data["type_ref"] = child_data
    else:
        data[name].append(child_data)


def preorder_travers_ast(cursor, total, comment, current_file, gn_path=None):  # 获取属性
    ast_dict = ast_to_dict(cursor, current_file, gn_path, comment)  # 获取节点属性
    total.append(ast_dict)  # 追加到数据统计列表里面


def get_start_comments(include_path):  # 获取每个头文件的最开始注释
    file_comment = []
    content = open_file(include_path)
    if content:
        pattern = RegularExpressions.START_COMMENT.value
        matches = re.finditer(pattern, content, re.DOTALL | re.MULTILINE)
        for mat in matches:
            file_comment.append(mat.group())

    with open(include_path, 'r', encoding='utf-8') as f:
        f.seek(0)
        content = f.read()
        pattern_high = RegularExpressions.END_COMMENT.value
        matches_high = re.findall(pattern_high, content, re.DOTALL | re.MULTILINE)
        if matches_high:
            file_comment.extend(matches_high)
        f.close()
    str_file_comment = '\n'.join(file_comment)
    return str_file_comment


def open_file(include_path):
    with open(include_path, 'r', encoding='utf-8') as f:
        content = ''
        loge = 0
        for line in f:
            if line.startswith('#ifdef __cplusplus'):
                loge = 1
                break
            else:
                inside_ifdef = True

            if inside_ifdef:
                content += line
        if loge == 0:
            content = ''
        f.close()
        return content


def api_entrance(share_lib, include_path, gn_path=None, link_path=None):  # 统计入口
    # clang.cindex需要用到libclang.dll共享库   所以配置共享库
    if Config.loaded:
        print("config.loaded == true")
    else:
        Config.set_library_file(share_lib)
        print("lib.dll: install path")
    # 创建AST索引
    index = Index.create()
    print('=' * 50)
    # options赋值为如下，代表宏定义解析数据也要
    args = ['-I{}'.format(path) for path in link_path]
    args.append('-std=c99')
    options = clang.cindex.TranslationUnit.PARSE_DETAILED_PROCESSING_RECORD
    print(args)

    data_total = []  # 列表对象-用于统计
    for item in include_path:  # 对每个头文件做处理
        tu = index.parse(item, args=args, options=options)
        print(tu)
        print('=' * 50)
        ast_root_node = tu.cursor  # 获取根节点
        print(ast_root_node)
        matches = get_start_comments(item)  # 接收文件最开始的注释
        # 前序遍历AST
        preorder_travers_ast(ast_root_node, data_total, matches, item, gn_path)  # 调用处理函数
        print('=' * 50)

    return data_total


def get_include_file(include_file_path, link_path, gn_path=None):  # 库路径、.h文件路径、链接头文件路径
    # libclang.dll库路径
    libclang_path = StringConstant.LIB_CLG_PATH.value
    # c头文件的路径
    file_path = include_file_path
    print(file_path)
    # 头文件链接路径
    link_include_path = link_path  # 可以通过列表传入
    data = api_entrance(libclang_path, file_path, gn_path, link_include_path)  # 调用接口

    return data
