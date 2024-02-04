/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#ifndef ENVIRONMENT_H
#define ENVIRONMENT_H_H
/**
 * @addtogroup environment
 *
 * @brief This module provides the ability to access the environment directory and obtain the native interface
   for public root directory.
 * @since 12
 */

/**
 * @file environment.h
 *
 * @brief Provides environment APIS.
 * @library libenvironment_ndk.z.so
 * @syscap SystemCapability.FileManagement.File.Environment.FolderObtain
 * @since 12
 */
#ifdef __cplusplus
extern "C" {
#endif
/**
 * @brief Enumerates the file location.
 *
 * @since 12
 */
typedef enum OH_Environment_FileLocation {
    /**
     * @brief Indicates the file located on the local.
     */
    LOCAL = 1,

    /**
     * @brief Indicates the file located on the cloud.
     */
    CLOUD = 2
} OH_Environment_FileLocation;

/**
 * @brief Get the file location.
 *
 * @param uri Represents a pointer to a uri.
 * @param uriLength Indicates the length of the uri.
 * @return Returns the file location enum.
 * @see OH_Environment_FileLocation
 * @since 12
 */
OH_Environment_FileLocation OH_Environment_GetFileLocation(char *uri, int uriLength);

/**
 * @brief Get the user Download directory.
 *
 * @param result Represents a pointer to a string.
 * @param resultNum Indicates the length of the result string.
 * @return Returns the status code of the execution.
 * @since 12
 */
int OH_Environment_GetUserDownloadDir(char **result, int *resultLength);

/**
 * @brief Get the user Desktop directory.
 *
 * @param result Represents a pointer to a string.
 * @param resultNum Indicates the length of the result string.
 * @return Returns the status code of the execution.
 * @since 12
 */
int OH_Environment_GetUserDesktopDir(char **result, int *resultLength);

/**
 * @brief Get the user Document directory.
 *
 * @param result Represents a pointer to a string.
 * @param resultNum Indicates the length of the result string.
 * @return Returns the status code of the execution.
 * @since 12
 */
int OH_Environment_GetUserDocumentDir(char **result, int *resultLength);
#ifdef __cplusplus
};
#endif
#endif //FILE_SHARE_H