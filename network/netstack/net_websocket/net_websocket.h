/*
 * Copyright (C) 2023 Huawei Device Co., Ltd.
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

#ifndef NET_WEBSOCKET_H
#define NET_WEBSOCKET_H

#include <signal.h>
#include <stdint.h>
#include <string.h>

/**
 * @addtogroup netstack
 * @{
 *
 * @brief Provides C APIs for the websocket client module.

 * @since 11
 * @version 1.0
 */

/**
 * @file net_websocket.h
 *
 * @brief Defines the APIs for the websocket client module.
 *
 * @library libnet_websocket.so
 * @syscap SystemCapability.Communication.NetStack
 * @since 11
 * @version 1.0
 */

#include "net_websocket_type.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Constructor of websocket.
 *
 * @param onMessage Callback function invoked when a message is received.
 * @param onClose Callback function invoked when a connection closing message is closed.
 * @param onError Callback function invoked when a connection error message is received.
 * @param onOpen Callback function invoked when a connection setup message is received.
 * @return Pointer to the websocket client if success; NULL otherwise.
 * @syscap SystemCapability.Communication.NetStack
 * @since 11
 * @version 1.0
 */
struct Websocket *OH_NetStack_WebsocketClient_Construct(Websocket_OnOpenCallback onOpen,
                                                        Websocket_OnMessageCallback onMessage,
                                                        Websocket_OnErrorCallback onError,
                                                        Websocket_OnCloseCallback onclose);

/**
 * @brief Adds the header information to the client request.
 *
 * @param client Pointer to the websocket client.
 * @param header Header information
 * @return 0 if success; non-0 otherwise. For details about error codes, see {@link OH_Websocket_ErrCode}.
 * @syscap SystemCapability.Communication.NetStack
 * @since 11
 * @version 1.0
 */
int OH_NetStack_WebsocketClient_AddHeader(struct Websocket *client, struct Websocket_HeaderNode header);

/**
 * @brief Connects the client to the server.
 *
 * @param client Pointer to the websocket client.
 * @param url URL for the client to connect to the server.
 * @param options Optional parameters.
 * @return 0 if success; non-0 otherwise. For details about error codes, see {@link OH_Websocket_ErrCode}.
 * @permission ohos.permission.INTERNET
 * @syscap SystemCapability.Communication.NetStack
 * @since 11
 * @version 1.0
 */
int OH_NetStack_WebsocketClient_Connect(struct Websocket *client, const char *url,
                                        struct Websocket_RequestOptions options);

/**
 * @brief Sends data from the client to the server.
 *
 * @param client Pointer to the websocket client.
 * @param data Data sent by the client.
 * @param length Length of the data sent by the client.
 * @return 0 if success; non-0 otherwise. For details about error codes, see {@link OH_Websocket_ErrCode}.
 * @permission ohos.permission.INTERNET
 * @syscap SystemCapability.Communication.NetStack
 * @since 11
 * @version 1.0
 */
int OH_NetStack_WebsocketClient_Send(struct Websocket *client, char *data, size_t length);

/**
 * @brief Closes a webSocket connection.
 *
 * @param client Pointer to the websocket client.
 * @param url URL for the client to connect to the server.
 * @param options Optional parameters.
 * @return 0 if success; non-0 otherwise. For details about error codes, see {@link OH_Websocket_ErrCode}.
 * @permission ohos.permission.INTERNET
 * @syscap SystemCapability.Communication.NetStack
 * @since 11
 * @version 1.0
 */
int OH_NetStack_WebsocketClient_Close(struct Websocket *client, struct Websocket_CloseOption options);

/**
 * @brief Releases the context and resources of the websocket connection.
 *
 * @param client Pointer to the websocket client.
 * @return 0 if success; non-0 otherwise. For details about error codes, see {@link OH_Websocket_ErrCode}.
 * @permission ohos.permission.INTERNET
 * @syscap SystemCapability.Communication.NetStack
 * @since 11
 * @version 1.0
 */
int OH_NetStack_WebsocketClient_Destroy(struct Websocket *client);

#ifdef __cplusplus
}
#endif

#endif // NET_WEBSOCKET_H
