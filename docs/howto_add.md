# C API构建添加指南
## 范围
本文旨在说明跟C API相关的如下问题：
* 编译方法
* 编译结果
* 编译GN模板
* 文档

## 需要了解的基础知识：

GN语法，https://zhuanlan.zhihu.com/p/136954435

对json描述文件有一定了解

## 编译方法
 使能ndk编译，默认编译linux/windows版本
```
./build.sh --product-name ohos-sdk
```
更加详细的SDK编译指导，请参考[如何编译full-SDK](http://https://gitee.com/openharmony/docs/blob/master/zh-cn/application-dev/faqs/full-sdk-compile-guide.md)。

## 编译结果

### 存放位置
跟os相关的内容（比如工具链，cmake等）会被拷贝到 root_out_dir/sdk-native/os-specific/目录下，其他内容放在root_out_dir/sdk-native/os-irrelevant目录下。归档的C API zip包放在root_out_dir/packages/ohos-sdk-native/[darwin|windows|linux]目录下，命名为native-[darwin|windows|linux]-x64-[x.x.x].zip。

### 压缩包目录结构
```
|--build
|--build-tools
|--docs
|--llvm
|--sysroot
|--nativeapi_syscap_config.json
|--NOTICE.txt
|--oh-uni-package.json
```
* build目录：存放cmake的配置文件
* build-tools目录：存放cmake等工具
* docs目录：存放sysroot对应的文档
* llvm目录：存放不同操作系统下的交叉工具链
* sysroot目录：存放头文件和库
* nativeapi_syscap_config.json: 存放头文件与syscap的映射关系
* NOTICE.txt：存放licence声明

## 编译模板说明
如前文所示，OpenHarmony native包由三部分组成；对应地，编译系统提供了一些GN模板，模板定义位于//build/ohos/ndk/ndk.gni。
* ohos_ndk_library : 对应包里的库文件。
* ohos_ndk_headers : 对应包里的头文件。
* ohos_ndk_toolchains ：对应包里的工具链。

ohos_ndk_headers和ohos_ndk_toolchains目标本质上只做copy

### ohos_ndk_library（目标名）
输入参数：
output_name: 最终输出的库的名字；选填，默认为目标名。

NOTE：库名字按照《C API接口编码规范》

下面的例子中，ndk_library的目标名为"libsensor"，但是输出的so名字为libsensor.z.so
```
ohos_ndk_library("libsensor_ndk") {
    ndk_description_file = "./libsensor.ndk.json"
    min_compact_version = "10"
    output_name = "sensor"  # 编译系统会自动将名字变成libsensor，如果不设置，就采用libsensor_ndk.so
    output_extension = "so" # 设置动态库的后缀名，默认为z.so
    system_capability = "SystemCapability.Xxx.Aaa" # 设置这个库关联的SysCap，当前一个库只支持一个SysCap
    system_capability_headers = [ # 与这个SysCap相关联的头文件
        "Xxx/Aaa.h",
        "Xxx/Bbb.h"
    ]
}
```
 **min_compact_version** : API level，当前library从哪个版本开始支持？如果没有设置，默认设置为从当前版本开始支持。

为什么需要变量min_compact_version？
一般地，操作系统的后期版本会提供比前期版本更多的库，比如某个提供23个NDK库，但是前一个版本仅提供13个NDK库。所以需要提供一个变量来告知编译系统当前库是从哪个版本开始支持的。

 **ndk_description_file** : 当前library的符号描述文件json格式. 这个文件类似于镜像中的version script文件格式。Harmonyos上采用json格式。为统一起见，ndk_description_file的命名统一为libxxx.ndk.json。Json文件可以通过添加key来提供更丰富的属性，为未来的扩展留下空间。

json描述文件的例子如下：
```
# NOTE：json文件本身不支持注释，这里作者便宜行事。读者使用的时候，请自行删除注释
[
  {
       "first_introduced": "2", # 描述当前接口引入的版本号
       "name": "GetSensorNdkVersion" # 当前接口的名字
  },
  {
       "name": "GetSensorNdkObject"，
       "type": "variable" # 描述当前符号是变量，而不是函数
  }
]
```

### ohos_ndk_headers(目标名)
输入参数：
 **sources** ：想拷贝的文件或目录列表
 **dest_dir** ：目标目录，默认值为$ndk_headers_out_dir，$ndk_headers_out_dir为$root_out_dir/sdk-native/sysroot/usr/include。

例子：例子中的sensor_ndk_interface.h将会被拷贝到$ndk_headers_out_dir/sensor目录下
```
ohos_ndk_headers("sensor_header") {
dest_dir = "$ndk_headers_out_dir/sensor"
sources = [
  "./include/sensor_ndk_interface.h",
]
}
```

### ohos_ndk_toolchains

这个模板只针对mac/win/linux平台的几个工具链，一般开发同学使用不到。
输入参数含义同ohos_ndk_headers。ohos_ndk_toolchains目标的默认dest_dir为$root_out_dir/sdk-native/sysroot/usr/lib


## 文档
C API的文档生成在docs目录，需要安装doxygen工具，在ubuntu上使用`sudo apt-get install doxygen`即可安装。如果编译机上没有安装doxygen，文档将无法生成。


