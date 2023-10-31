import enum
from coreImpl.parser import parser


class ToolNameType(enum.Enum):
    COLLECT = 'collect'
    DIFF = 'diff'


toolNameTypeSet = [member.value for name,
                   member in ToolNameType.__members__.items()]


class FormatType(enum.Enum):
    JSON = 'json'
    EXCEL = 'excel'


formatSet = [member.value for name,
             member in FormatType.__members__.items()]


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
    commands = [{"name": "--tool-name", "abbr": "-N", "required": True, "choices": toolNameTypeSet, "type": str, "default": ToolNameType["COLLECT"], "help":"工具名称"},
                {"name": "--parser-path", "abbr": "-P", "required": True, "type": str,  "help": "解析路径"}]
