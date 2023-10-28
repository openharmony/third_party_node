
import argparse
from bin import config


parser = argparse.ArgumentParser(
    prog=config.Config.name, description=config.Config.description)
for command in config.Config.commands:
    abbr = command.get("abbr")
    name = command.get("name")
    choices = command.get("choices")
    required = (True if command.get("required") else False)
    type = command.get("type")
    default = command.get("default")
    help = command.get("help")
    parser.add_argument(abbr, name, choices=choices,
                        required=required, type=type, default=default, help=help)


# args = parser.parse_args()
config.run_tools(parser.parse_args())
