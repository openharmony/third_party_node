## 1.修改环境变量配置

使用该工具前需要修改[constants.py](./src/utils/constants.py)
文件下的StringConstant.LIB_CLG_PATH、StringConstant.REPLACE_WAREHOUSE、StringConstant.INCLUDE_LIB；
StringConstant.LIB_CLG_PATH:共享库(本地的)
REPLACE_WAREHOUSE：拉下来的interface_sdk_c仓的目录(本地的路径) --例如：(去掉磁盘的路径)\\interface_sdk_c
StringConstant.INCLUDE_LIB：# 拉到本地仓的三方库路径

## 2.环境：

1)python-3.11.4-amd64

2)PyCharm Community Edition 2023.2

3)下载[requirements.txt](./requirements.txt)下面的库 ---下载指令：pip install -r txt文件路径

4)需要把src目录设置为sources root(找到src目录，点击右键，将目标标记为里面)

5)在interface_sdk_c目录下运行的是src目录下的mian.py文件

## 3.终端指令

options:
  -h, --help            show this help message and exit
  -N {collect, check, diff, collect_h, collect_file}, --tool-name {collect, check, diff, collect_h, collect_file}
                        工具名称
  -P PARSER_PATH, --parser-path PARSER_PATH
                        解析路径

例如：用的统计工具：终端指令：py -N collect -P 目录文件路径   ---就是调用CAPI统计工具的使用

## 4.自动化测试

1）在test/ut下添加对应工具的测试用例

2）在test/expect下添加对应工具的期望结果

3）在test/testCase/run_main.py添加对应的方法执行测试用例的解析，将结果输出到test/output下，与expect对比期望结果

添加的方法需要以test_开头

## 5.统计工具使用指令

注意：工作路都在interface_sdk_c下(也就是C仓下)
1）统计工具(C仓-含gn文件)
在工作路径下执行：py main.py路径(相对工作路径) -N collect -P 目录文件路径
生成文件名：result_total.xlsx--扫描结果api数据

2）统计工具(直接获取)
在工作路径下执行：py main.py路径(相对工作路径) -N collect_h -P 目录文件路径/头文件路径
生成文件名：parser_direct_data.xlsx--扫描结果api数据

3）统计工具(获取到文件级的kit、子系统和文件当前接口数)
在工作路径下执行：py main.py路径(相对工作路径) -N collect_h -P 参数随意 -O 输出json文件路径名或者不加-O
生成文件名(不加-O)：file_api_json.json--文件级接口数据；
生成文件名(加-O)：传入json文件路径名
