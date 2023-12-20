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

import enum


class StringConstant(enum.Enum):
    LIB_CLG_PATH = r'D:\Environment\LLVM\bin\libclang.dll'  # 共享库
    FUNK_NAME = "ohos_ndk_headers"
    REPLACE_WAREHOUSE = '\\interface_sdk_c\\interface_sdk_c'  # 拉到本地仓路径(去掉磁盘)
    # 拉到本地仓的三方库绝对路径
    INCLUDE_LIB = r'.\third_party\musl\ndk_musl_include'
    STD_INCLUDE = r'.\sysroot\ndk_musl_include_files'
    SELF_INCLUDE = r'.\sysroot\self_include_files'
    RESULT_HEAD_NAME = "result_total.xlsx"


class RegularExpressions(enum.Enum):
    START_COMMENT = r'/\*\*(.*?)\*/'
    END_COMMENT = r'/\*\* @} \*/'
    SOURCES = r'sources\s*=\s*\[[^\]]*\]'
    DEST_DIR = r'dest_dir\s*=\s*"([^"]*)"'
    INCLUDE_H = r'"([^"]+h)"'
    CREATE_LIB_PATH = r'sysroot\$ndk_headers_out_dir'
