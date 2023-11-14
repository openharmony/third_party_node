import enum


class StringConstant(enum.Enum):
    LIB_CLG_PATH = r'D:\Environment\LLVM\bin\libclang.dll'  # 共享库
    FUNK_NAME = "ohos_ndk_headers"
    REPLACE_WAREHOUSE = '\\interface_sdk_c\\interface_sdk_c'  # 拉到本地仓路径(去掉磁盘)
    # 拉到本地仓的三方库绝对路径
    INCLUDE_LIB = r'third_party\musl\ndk_musl_include'
