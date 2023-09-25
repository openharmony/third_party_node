# C API用户指导

## 基础知识
了解C API基础知识，请参考《[Native API入门](https://gitee.com/openharmony/docs/blob/master/zh-cn/application-dev/reference/native-api-intro.md)》。


## 接口开放策略

### 不开放原则

1. 应用生态提供的核心特性只开放ArkTS API，不开发C API。
2. 不开发ArkUI的C API。
3. 不开发硬件底层接口（HDI）C API。
4. 不开放命令行接口C API。
### 开放原则
5. 高性能、密集计算业务场景主动开放C API，例如：需要高性能的IO、CPU密集计算等场景；音视频编解码、图形计算等场景。
6. 应用生态业务依赖的场景，主动开放高阶C API。例如：对标竞品，媒体高阶C API。
7. 应用生态框架以来的场景，按需开放C API。例如：按需开放Unity/electron/CFE框架中依赖的C API。
8. 行业约定或者标准要求的场景按需开放C API。此类C API独立发布，不放入OpenHarmony C API中。例如：金融或者安全行业高密加密算法等，按需独立发布。

### 前向兼容性原则
9. C API的前向兼容性原则与ArkTS API策略保持一致。CI流程中会使用自动化工具进行看护。


## C API接口设计规范
本规范是在《[OpenHarmony API治理章程](https://gitee.com/openharmony/docs/blob/master/zh-cn/design/OpenHarmony-API-governance.md)》《[OpenHarmony API社区规范](https://gitee.com/openharmony/docs/blob/master/zh-cn/design/OpenHarmony-API-quality.md)》的基础上，对C API设计进行的一些补充。在C API接口设计上同时满足此三份规范。

### 设计规则
* 【规则】C API相关的接口文件，包含接口列表文件ndk.json，头文件，编译构建脚本三部分；这些内容必须放置在interface_sdk_c仓中，不能对外产生依赖。三方库接口暴露规则同自研仓，接口是处理过的接口文件，不是三方库原生文件。
* 【规则】接口命名必须符合《[C API接口编码规范](./capi_naming.md)》。
* 【规则】需要发布的接口需要明确在ndk.json文件中声明列出，不允许直接发布镜像中的动态库，都采用stub动态库的方式发布。
  * json文件是一个符号描述数组，举例如下：
  ```
  [
      {
          "name": "__progname",
          "type": "variable"
      },
      {
          "name": "pthread_cond_clockwait"
      }
  ]
  ```
  name表示符号的名字，type表示符号类型，不写type，默认认为是函数；varible表示这个符号是变量。
* 【规则】C API中头文件中不允许包含不发布的接口声明，类型定义；三方库的接口文件，可
以备案保留。
* 【规则】接口必须是C语言接口，不允许出现C++元素，保证引用方是C代码也能使用；提供头文件中的接口需要用C declare声明。
  
  * C declare声明
  ```
  #ifdef __cplusplus
  extern "C" {
  #endif
  ...
  #ifdef __cplusplus
  }
  #endif
  ```
  * __反例__  
  struct/enum声明的类型，如果没有声明typedef类型，在使用时必须加上struct、enum关键字。

  * __例外__  直接提供实现库，嵌入到应用包中的接口可以使用C++元素。

* 【规则】包含C API的实现动态库放到/system/{lib}/ndk目录下。
* 【规则】每个接口都需要与syscap相关联；一个头文件中的接口只能属于一个syscap，不能包含多个syscap能力；多个文件可以关联相同的syscap。
* 【规则】自研接口按照如下文档注释写作规范进行注释，三方库接口有修改的，注释规则按照注释规范进行注释。
* 【规则】对外提供的接口，采用对外封闭，依赖倒置原则；结构体的具体实现不对外暴露，方便后续扩展修改

> 此条是《[OpenHarmony API社区规范](https://gitee.com/openharmony/docs/blob/master/zh-cn/design/OpenHarmony-API-quality.md)》规则16：注意封装：不暴露实现细节的C API补充解释。

### 兼容性规则
* 【规则】不允许进行接口原型变更，可以通过新增接口，废弃老接口的方式进行。
* 【规则】接口语义不允许变更，语义通过xts用例保持兼容性。接口涉及的结构体，枚举值变更也会影响语义，建议在设计的时候保留一些字段供扩展使用。
* 【规则】结构体成员，枚举值可以标明废弃，不建议删除。

## 文档注释写作规范
本章在OpenHarmony API社区规范[API设计概述](https://gitee.com/openharmony/docs/blob/master/zh-cn/design/OpenHarmony-API-quality.md#api设计概述)基础上，对C API的注释流程做了补充。

自研接口注释书写规范步骤：
1. 每个头文件都需要书写注释。
2. 注释书写参考 [Native接口注释规范](https://gitee.com/openharmony/docs/blob/master/zh-cn/contribute/template/native-template.md)，注释命令必须按照@开头。
3. 提交注释头文件到 https://gitee.com/openharmony-sig/interface_native_header/tree/master/zh-cn/native_sdk 这个仓。
4. 等待资料审核，合入；同时翻译一份英文注释头文件到en目录。
5. 下载这个头文件，覆盖到对应头文件。
