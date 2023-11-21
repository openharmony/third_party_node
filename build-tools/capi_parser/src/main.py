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

import argparse
from bin import config


def main_function():
    parser = argparse.ArgumentParser(
        prog=config.Config.name, description=config.Config.description)
    for command in config.Config.commands:
        arg_abbr = command.get("abbr")
        arg_name = command.get("name")
        arg_choices = command.get("choices")
        arg_required = (True if command.get("required") else False)
        arg_type = command.get("type")
        default = command.get("default")
        arg_help = command.get("help")
        parser.add_argument(arg_abbr, arg_name, choices=arg_choices,
                            required=arg_required, type=arg_type, default=default, help=arg_help)

    config.run_tools(parser.parse_args())


if __name__ == '__main__':
    main_function()
