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

from coreImpl.parser.parser import parser_include_ast
from coreImpl.diff.diff_file import start_diff_file


def process_pr(pr_id):
    print("开发中...")


def process_dir(old_dir, new_dir):
    start_diff_file(old_dir, new_dir)


def get_dir_by_pr(pr: str):
    return {"old": "", "new": ""}


def process_dir_diff(old_dir, new_dir):
    diff_info_list = []
    return diff_info_list


def process_file_diff(old_file, new_file):
    diff_info_list = []
    return diff_info_list


def get_parser_data(file_path):
    root_start = file_path.split('sdk_c')[0]
    root_path = f'{root_start}sdk_c'
    parser_data = parser_include_ast(root_path, [file_path])
    return parser_data
