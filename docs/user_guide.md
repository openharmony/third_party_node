# C API用户指导

## 基础知识
了解C API基础知识，请参考《[Native API入门](https://gitee.com/openharmony/docs/blob/master/zh-cn/application-dev/reference/native-api-intro.md)》


## 接口开放策略

### 不开放原则

1. 应用生态提供的核心特性只开放ArkTS API，不开发CAPI。
2. 不开发ArkUI的CAPI。
3. 不开发硬件底层接口（HDI）CAPI。
4. 不开放命令行接口CAPI。
### 开放原则
5. 高性能、密集计算业务场景主动开放CAPI。例如：需要高性能的IO、CPU密集计算等场景；音视频编解码、图形计算等场景。
6. 应用生态业务依赖的场景，主动开放高阶CAPI。例如：对标竞品，媒体高阶CAPI。
7. 应用生态框架以来的场景，按需开放CAPI。例如：按需开放Unity/electron/CFE框架中依赖的CAPI。
8. 行业约定或者标准要求的场景按需开放CAPI。此类CAPI独立发布，不放入OpenHarmony CAPI中。例如：金融或者安全行业高密加密算法等，按需独立发布。

### 前向兼容性原则
9. CAPI的前向兼容性原则与ArkTS API策略保持一致。CI流程中会使用自动化工具进行看护。


## CAPI接口设计规范
### 设计规则
* 【规则】接口新增或者变更需要到APP_SIG会议进行评审，通过后才能修改。
* 【规则】接口要考虑OpenHarmony生态的一致性，保证在生态版本设备上的兼容，实现代码需同步发布。
* 【规则】CAPI相关的接口文件，包含接口列表文件ndk.json，头文件，编译构建脚本三部分；这些内容必须放置在interface_sdk_c仓中，不能对外产生依赖。三方库接口暴露规则同自研仓，接口是处理过的接口文件，不是三方库原生文件。
* 【规则】接口命名必须符合《[CAPI接口编码规范](./capi_naming.md)》。
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
* 【规则】CAPI中头文件中不允许包含不发布的接口声明，类型定义；三方库的接口文件，可
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

* 【规则】包含CAPI的实现动态库放到/system/{lib}/ndk目录下。
* 【规则】每个接口都需要与syscap相关联；一个头文件中的接口只能属于一个syscap，不能包含多个syscap能力；多个文件可以关联相同的syscap。
* 【规则】自研接口按照文档注释写作规范进行注释，三方库接口有修改的，注释规则按照注释规范进行注释。
* 【建议】对外提供的接口，采用对外封闭，依赖倒置原则；结构体的具体实现不对外暴露，方便后续扩展修改。

### 兼容性规则
* 【规则】接口发布后，需要保证版本上的兼容性；接口原型，语义，错误处理等都要保持兼容。
* 【规则】接口删除需提前五个版本进行预告；在使用这个接口的时候实现时进行告警提示；头文件接口注释标注@deprecated。 
* 【规则】不允许进行接口原型变更，可以通过新增接口，废弃老接口的方式进行。
* 【规则】接口语义不允许变更，语义通过xts用例保持兼容性。接口涉及的结构体，枚举值变更也会影响语义，建议在设计的时候保留一些字段供扩展使用。
* 【规则】结构体成员，枚举值可以标明废弃，不建议删除。

## 文档注释写作规范
CAPI中暴露的接口，分为自研接口与三方开源库接口。

自研接口注释书写规范步骤：
1. 每个头文件都需要书写注释。
2. 注释书写参考 [Nativ接口注释规范](https://gitee.com/openharmony/docs/blob/master/zh-cn/contribute/template/native-template.md)，注释命令必须按照@开头。
3. 提交注释头文件到 https://gitee.com/openharmony-sig/interface_native_header/tree/master/zh-cn/native_sdk 这个仓。
4. 等待资料审核，合入；同时翻译一份英文注释头文件到en目录。
5. 下载这个头文件，覆盖到对应头文件。
