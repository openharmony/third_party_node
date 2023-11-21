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

import enum


class TAGS(enum.Enum):
    ADD_TO_GROUP = 'addtogroup'
    BRIEF = 'brief'
    DEPRECATED = 'deprecated'
    FILE = 'file'
    LIBRARY = 'library'
    PARAM = 'param'
    PERMISSION = 'permission'
    RETURN = 'return'
    SINCE = 'since'
    SYSCAP = 'syscap'
    LEFT_BRACE = '{'
    RIGHT_BRACE = '}'


class ErrorType(enum.Enum):
    DEFAULT = {
        'id': -1,
        'description': '',
    }
    EMPTY_TAG = {
        'id': 0,
        'description': "miss tag value",
    }
    ERROR_TAG = {
        'id': 1,
        'description': 'error tag name',
    }
    MISSPELL_WORDS = {
        'id': 1,
        'description': 'misspell words',
    }
    NAMING_ERRORS = {
        'id': 2,
        'description': 'naming errors',
    }
    SYNTAX_ERRORS = {
        'id': 3,
        'description': 'syntax errors',
    }
    UNKNOW_DEPRECATED = {
        'id': 5,
        'description': 'unknow deprecated',
    }
    WRONG_VALUE = {
        'id': 7,
        'description': 'wrong value',
    }
    WRONG_SCENE = {
        'id': 8,
        'description': 'wrong scene',
    }


class LogType(enum.Enum):
    DEFAULT = ''
    LOG_API = 'Api'
    LOG_JSDOC = 'JsDoc'
    LOG_FILE = 'File'


class ErrorLevel(enum.Enum):
    HIGH = 3
    MIDDLE = 2
    LOW = 1


class ErrorMessage(enum.Enum):
    EMPTY_TAG = 'the [$$] tag value is empty. Please supplement the default value'
    USE_UPPER_TAG = "the [$$] tag is upper. Should use lower: [$$]"
    REPEAT_FILE_TAG = 'the [$$] tag is repeat. Please check the tag in file'
    ERROR_INFO_VALUE_TAG = 'the [$$] tag value is incorrect. Please check the usage method'
    ERROR_INFO_VALUE_LIBRARY = 'the [library] tag value is incorrect. This tag must be end with .so or .a, \
        or is NA. Please check the usage method'
    ERROR_INFO_VALUE_PARAM = 'the value of the [$$] [param] tag is incorrect. Please check if it matches \
        the [$$] parameter name'
    ERROR_INFO_COUNT_PARAM = 'the count of the [param] tag is wrong. Please check the parameters and Doc'
    ERROR_INFO_VALUE_PERMISSION = 'the [permission] tag value is incorrect. Please check if the permission \
        field has been configured or update the configuration file'
    ERROR_INFO_VALUE_SINCE = 'the [since] tag value is incorrect. Please check if the tag value is a numerical value'
    ERROR_INFO_VALUE_SYSCAP = 'the [syscap] tag value is incorrect. Please check if the syscap field is configured'
    ERROR_USE_LEFT_BRACE = 'the validity verification of the Doc tag failed. The [{] tag is not allowed to used \
        in Doc which not has addtogroup tag, or used in the wrong place.'
    ERROR_REPEAT_LEFT_BRACE = 'the validity verification of the Doc tag failed. The [{] tag is not allowed to \
        reuse in Doc which has addtogroup tag.'
    ERROR_USE_RIGHT_BRACE = 'the validity verification of the JSDoc tag failed. The [}] tag is not allowed to \
        be reused please delete the extra tags.'
    ERROR_FILE_HAS_ONE_LOSE_OTHER = 'the file has the $$, but do not has the $$.'
    ERROR_FILE_LOSE_ONE = 'the file missing $$'
    FUNCTION_DECL = 'Function naming should use the big hump naming style or beginning with OH/OS,and \
        using "_" segmentation.'
    STRUCT_DECL = 'Structure type naming should use the big hump naming style.'
    ENUM_DECL = 'Enum type naming should use the big hump naming style.'
    UNION_DECL = 'Consortium type naming should use the big hump naming style.'
    VAR_DECL = 'Variable naming should use the small hump naming style.'
    PARM_DECL = 'Function parameters naming should use the small hump naming style.'
    MACRO_PARAMETERS_TYPE_NAMING_ERROR = 'Macro parameters naming should use the small hump naming style.'
    FIELD_DECL = 'Fields in the structure naming should use the small hump naming style.'
    MEMBERS_OF_THE_CONSORTIUM_TYPE_NAMING_ERROR = 'Members of the consortium naming should use the \
        small hump naming style.'
    MACRO_DEFINITION = 'Macro naming should use all uppercase, separated by underscores naming style.'
    ENUM_CONSTANT_DECL = 'Enum value naming should use all uppercase, separated by underscores naming style.'
    GOTO_LABEL_TYPE_NAMING_ERROR = 'Goto label value naming should use all uppercase, separated by \
        underscores naming style.'
    GLOBAL_VARIABLE_TYPE_NAMING_ERROR = 'Global variable should increase "g_" prefix.'
    TRANSLATION_UNIT = 'File naming should be all lowercase, separated by underscores.'


class OutputTxt:
    id = -1
    level = -1
    location = ''
    file_path = ''
    message = ''

    def __init__(self, id_param, level_param, location_param, file_path_param, message_param):
        self.id = id_param
        self.level = level_param
        self.location = location_param
        self.file_path = file_path_param
        self.message = message_param

    def get_id(self):
        return self.id

    def set_id(self, id_param):
        self.id = id_param

    def get_level(self):
        return self.level

    def set_level(self, level_param):
        self.level = level_param

    def get_location(self):
        return self.location

    def set_location(self, location_param):
        self.location = location_param

    def get_file_path(self):
        return self.file_path

    def set_file_path(self, file_path_param):
        self.file_path = file_path_param

    def get_message(self):
        return self.message

    def set_message(self, message_param):
        self.message = message_param


class ApiResultInfo:
    error_type: ErrorType = ErrorType.DEFAULT.value
    error_info = ''
    level: ErrorLevel = -1
    api_name = ''
    api_full_text = ''
    file_name = ''
    location = ''
    location_line = -1
    location_column = -1
    type: LogType = LogType.DEFAULT.value
    version = -1
    basename = ''

    def __init__(self, error_type_param=None, error_info_param='', api_name_param=''):
        if error_type_param is None:
            error_type_param = ErrorType.DEFAULT.value
        self.error_type = error_type_param
        self.error_info = error_info_param
        self.api_name = api_name_param

    def get_error_type(self):
        return self.error_type

    def set_error_type(self, error_type_param):
        self.error_type = error_type_param

    def get_file_name(self):
        return self.file_name

    def set_file_name(self, file_name_param):
        self.file_name = file_name_param

    def get_type(self):
        return self.type

    def set_type(self, type_param):
        self.type = type_param

    def get_error_info(self):
        return self.error_info

    def set_error_info(self, error_info_param):
        self.error_info = error_info_param

    def get_version(self):
        return self.version

    def set_version(self, version_param):
        self.version = version_param

    def get_basename(self):
        return self.basename

    def set_basename(self, basename_param):
        self.basename = basename_param

    def get_level(self):
        return self.level

    def set_level(self, level_param):
        self.level = level_param

    def get_api_name(self):
        return self.api_name

    def set_api_name(self, api_name_param):
        self.api_name = api_name_param

    def get_api_full_text(self):
        return self.api_full_text

    def set_api_full_text(self, api_full_text_param):
        self.api_full_text = api_full_text_param

    def get_location_line(self):
        return self.location_line

    def set_location_line(self, location_line_param):
        self.location_line = location_line_param

    def get_location_column(self):
        return self.location_column

    def set_location_column(self, location_column_param):
        self.location_column = location_column_param

    def get_location(self):
        return self.location

    def set_location(self, location_param):
        self.location = location_param


class DocInfo:
    group = ''
    brief = ''
    deprecated = ''
    file = ''
    permission = ''
    since = ''
    syscap = ''
    param_index = -1
    throw_index = -1


class FileDocInfo:
    is_in_group_tag = False
    group_name = None
    has_group_start = False
    has_group_end = False
    is_in_file_tag = False
    file_name = None
    file_brief = None
    file_library = None
    file_syscap = None
    curr_doc_info = DocInfo()
