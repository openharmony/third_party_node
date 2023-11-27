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

class ParserGetResultTable:
    result_list = []
    head_name = ""
    only_file1 = []
    only_file2 = []
    data = []

    def __init__(self, result_list_need, head_name_need, only_file1_need, only_file2_need, data_need):
        self.result_list = result_list_need
        self.head_name = head_name_need
        self.only_file1 = only_file1_need
        self.only_file2 = only_file2_need
        self.data = data_need
