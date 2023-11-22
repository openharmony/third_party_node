#!/usr/bin/env python
# -*- coding: utf-8 -*-

import sys
import os
import json
import unittest
sys.path.append(os.path.abspath(os.path.join(os.path.dirname(__file__), "../../src")))
from coreImpl.check.check import get_check_result, write_in_txt


class TestMethods(unittest.TestCase):
    def test_check(self):
        test_path = os.path.abspath(os.path.join(os.path.dirname(__file__), "..\\"))
        test_case_path = os.path.join(test_path, "ut\\check")
        output_path = os.path.join(test_path, "output\\check")
        expect_path = os.path.join(test_path, "expect\\check")
        for dirpath, dirnames, filenames in os.walk(test_case_path):
            for item in filenames:
                file_name = item.split('.')[0]
                check_result = get_check_result([os.path.join(dirpath, item)])
                write_in_txt(check_result, os.path.join(output_path, "{}.txt".format(file_name)))
                with open(os.path.join(expect_path, "{}.txt".format(file_name))) as json_file:
                    permission_file_content = json.load(json_file)
                result_json = json.dumps(permission_file_content, default=lambda obj: obj.__dict__, indent=4)
                self.assertEqual(result_json, "result_json", "{} case is error".format(os.path.join(dirpath, item)))


if __name__ == '__main__':
    unittest.main()
