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


main_function()
