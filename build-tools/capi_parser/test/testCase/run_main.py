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


import sys
import os
import json
import unittest
sys.path.append(os.path.abspath(os.path.join(os.path.dirname(__file__), "../../src")))
from coreImpl.check.check import get_check_result_list, write_in_txt


class TestMethods(unittest.TestCase):
    def test_check(self):
        test_path = os.path.abspath(os.path.join(os.path.dirname(__file__), "..\\"))
        test_case_path = os.path.join(test_path, "ut\\check")
        output_path = os.path.join(test_path, "output\\check")
        expect_path = os.path.join(test_path, "expect\\check")
        for dirpath, dirnames, filenames in os.walk(test_case_path):
            for item in filenames:
                file_name = item.split('.')[0]
                check_result = get_check_result_list([os.path.join(dirpath, item)])
                write_in_txt(check_result, os.path.join(output_path, "{}.txt".format(file_name)))
                with open(os.path.join(expect_path, "{}.txt".format(file_name))) as json_file:
                    permission_file_content = json.load(json_file)
                result_json = json.dumps(permission_file_content, default=lambda obj: obj.__dict__, indent=4)
                self.assertEqual(result_json, "result_json", "{} case is error".format(os.path.join(dirpath, item)))


if __name__ == '__main__':
    unittest.main()
