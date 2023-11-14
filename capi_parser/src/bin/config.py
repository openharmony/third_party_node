import enum
from coreImpl.parser import parser


class ToolNameType(enum.Enum):
    COLLECT = 'collect'
    DIFF = 'diff'


tool_name_type_set = [
    member.value for name_tool,
    member in ToolNameType.__members__.items()
]


class FormatType(enum.Enum):
    JSON = 'json'
    EXCEL = 'excel'


format_set = [
    member.value for name_format,
    member in FormatType.__members__.items()
]


def run_tools(options):
    tool_name = options.tool_name
    if tool_name == ToolNameType["COLLECT"].value:
        parser.parser(options.parser_path)
    elif tool_name == ToolNameType["DIFF"].value:
        print("开发中。。。")
    else:
        print("工具名称错误")


class Config(object):
    name = 'parser'
    version = '0.1.0'
    description = 'Compare the parser the NDKS'
    commands = [
        {
            "name": "--tool-name",
            "abbr": "-N",
            "required": True,
            "choices": tool_name_type_set,
            "type": str,
            "default": ToolNameType["COLLECT"],
            "help": "工具名称"
        },
        {
            "name": "--parser-path",
            "abbr": "-P",
            "required": True,
            "type": str,
            "help": "解析路径"
        }
    ]
