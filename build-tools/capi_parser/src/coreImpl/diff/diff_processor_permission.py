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
import enum


class RangeChange(enum.Enum):
    DOWN = 'down'
    UP = 'up'
    NO = 'no'
    CHANGE = 'change'


class CalculateValue:
    pass_data = []
    fail_data = []

    def __init__(self):
        self.pass_data = []
        self.fail_data = []


class DiffProcessorPermission:
    # 转义规则
    splitchar = {
        'and': {'splitchar': 'and', 'transferchar': 'and'},
        'or': {'splitchar': 'or', 'transferchar': 'or'},
        'eq': {'splitchar': '=', 'transferchar': '=='},
        'LE': {'splitchar': '->', 'transferchar': '<='},
        'RE': {'splitchar': '<-', 'transferchar': '>='},
        'not': {'splitchar': '-', 'transferchar': '!'},
        'lcurve': {'splitchar': '\\(', 'transferchar': '('},
        'rcurve': {'splitchar': '\\)', 'transferchar': ')'},
    }
    variable_list = []  # 命题集合

    def find_variable_list(self, string):  # return all variable_list in the string.
        for char in self.splitchar:
            string = re.sub(re.compile(self.splitchar[char]['splitchar']), '', string)
        str_set = set(string.split(' '))
        str_set.remove('')
        variable_list = list(str_set)
        variable_list.sort()
        return tuple(variable_list)

    def formatten(self, string):
        for char in self.splitchar:
            string = re.sub(re.compile(self.splitchar[char]['splitchar']), self.splitchar[char]['transferchar'], string)
        return string

    def get_bool_in_list(self, number_list, bin_len):
        state_list = [bin(i) for i in number_list]
        state_list = [x[2:] for x in state_list]
        state_list = ['0' * (bin_len - len(x)) + x for x in state_list]
        state_list = [tuple([bool(eval(y)) for y in x]) for x in state_list]
        return tuple(state_list)

    def calculate(self, string: str, variable_length: int, state_list):
        state_value = {i: None for i in range(2 ** variable_length)}
        for state_index in range(2 ** variable_length):
            modify_string = string
            for variable_index in range(variable_length):
                modify_string = modify_string.replace(
                    self.variable_list[variable_index], str(state_list[state_index][variable_index]))
            state_value[state_index] = eval(modify_string)
        return state_value

    def process_value(self, state_value):
        calculate = CalculateValue()
        for state in state_value:
            if state_value[state]:
                calculate.pass_data.append(state)
            else:
                calculate.fail_data.append(state)
        return calculate

    def calculate_paradigm(self, string):
        self.variable_list = self.find_variable_list(string)
        string = self.formatten(string)
        variable_length = len(self.variable_list)
        state_list = self.get_bool_in_list(range(2 ** variable_length), variable_length)
        state_value = self.calculate(string, variable_length, state_list)
        calculate = self.process_value(state_value)
        return calculate

    def calculate_paradigm_up(self, old_str, new_str, falg_bool=True):
        char = ''
        if falg_bool:
            char = self.splitchar['LE']['splitchar']
        else:
            char = self.splitchar['RE']['splitchar']
        merge_str = f'({old_str}) {char} ({new_str})'
        return self.calculate_paradigm(merge_str)


class CalculateRsesult:
    variable_list = []
    state_down = []
    state_up = []
    state_range = ''

    def __init__(self):
        self.variable_list = []
        self.state_down = []
        self.state_up = []
        self.state_range = ''


def compare_permission(old_str, new_str):
    permissor_tool = DiffProcessorPermission()
    up_state = permissor_tool.calculate_paradigm_up(old_str, new_str, True)
    down_state = permissor_tool.calculate_paradigm_up(old_str, new_str, False)
    variable_list = permissor_tool.variable_list
    calculate_rsesult = CalculateRsesult()
    calculate_rsesult.variable_list = variable_list
    if len(down_state.fail_data) > 0:
        calculate_rsesult.state_up = permissor_tool.get_bool_in_list(down_state.fail_data, len(variable_list))
        calculate_rsesult.state_range = RangeChange.UP.value
    if len(up_state.fail_data) > 0:
        calculate_rsesult.state_down = permissor_tool.get_bool_in_list(up_state.fail_data, len(variable_list))
        if calculate_rsesult.state_range == '':
            calculate_rsesult.state_range = RangeChange.DOWN.value
        else:
            calculate_rsesult.state_range = RangeChange.CHANGE.value
    return calculate_rsesult
