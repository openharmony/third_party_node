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

#ifndef NET_WEBSOCKET_TYPE_H
#define NET_WEBSOCKET_TYPE_H

/**
 * @addtogroup netstack
 * @{
 *
 * @brief Provides C APIs for the websocket client module.
 *
 * @since 11
 * @version 1.0
 */

/**
 * @file net_websocket_type.h
 * @brief Defines the data structure for the C APIs of the websocket client module.
 *
 * @library libnet_websocket.so
 * @syscap SystemCapability.Communication.NetStack
 * @since 11
 * @version 1.0
 */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Defines the parameters for connection closing by the server.
 *
 * @since 11
 * @version 1.0
 */
struct Websocket_CloseResult {
    /** Error code */
    uint32_t code;
    /** Error cause */
    const char *reason;
};

/**
 * @brief Defines the parameters for proactive connection closing by the client.
 *
 * @since 11
 * @version 1.0
 */
struct Websocket_CloseOption {
    /** Error code */
    uint32_t code;
    /** Error cause */
    const char *reason;
};

/**
 * @brief Defines the parameters for the connection error reported by the server.
 *
 * @since 11
 * @version 1.0
 */
struct Websocket_ErrorResult {
    /** Error code */
    uint32_t errorCode;
    /** Error message */
    const char *errorMessage;
};

/**
 * @brief Defines the parameters for the connection success reported by the server.
 *
 * @since 11
 * @version 1.0
 */
struct Websocket_OpenResult {
    /** Connection success code */
    uint32_t code;
    /** Connection success reason */
    const char *reason;
};

/**
 * @brief Defines the callback function invoked when an <b>open</b> message is received.
 *
 * @param client websocket client.
 * @param openResult Content of the <b>open</b> message received by the websocket client.
 * @since 11
 * @version 1.0
 */
typedef void (*Websocket_OnOpenCallback)(struct Websocket *client, Websocket_OpenResult openResult);

/**
 * @brief Defines the callback function invoked when data is received.
 *
 * @param client websocket client.
 * @param data Data received by the websocket client.
 * @param length Length of the data received by the websocket client.
 * @since 11
 * @version 1.0
 */
typedef void (*Websocket_OnMessageCallback)(struct Websocket *client, char *data, uint32_t length);

/**
 * @brief Defines the callback function invoked when an error message is received.
 *
 * @param client websocket client.
 * @param errorResult Content of the connection error message received by the websocket client.
 * @since 11
 * @version 1.0
 */
typedef void (*Websocket_OnErrorCallback)(struct Websocket *client, Websocket_ErrorResult errorResult);

/**
 * @brief Defines the callback function invoked when a <b>close</b> message is received.
 *
 * @param client webSocket client.
 * @param closeResult Content of the <b>close</b> message received by the webSocket client.
 * @since 11
 * @version 1.0
 */
typedef void (*Websocket_OnCloseCallback)(struct Websocket *client, Websocket_CloseResult closeResult);

/**
 * @brief Adds the header linked list to the websocket client.
 *
 * @since 11
 * @version 1.0
 */
struct Websocket_HeaderNode {
    /** Header field name */
    const char *fieldName;
    /** Header field content */
    const char *fieldValue;
    /** Next pointer of the header linked list */
    struct Websocket_HeaderNode *next;
};

/**
 * @brief Defines the parameters for the connection between the websocket client and server.
 *
 * @param headers Header information.
 * @since 11
 * @version 1.0
 */
struct Websocket_RequestOptions {
    struct Websocket_HeaderNode *headers;
};

/**
 * @brief Defines the websocket client structu re.
 *
 * @since 11
 * @version 1.0
 */
struct Websocket {
    /** Pointer to the callback invoked when a connection message is received */
    Websocket_OnOpenCallback onOpen;
    /** Pointer to the callback invoked when a message is received */
    Websocket_OnMessageCallback onMessage;
    /** Pointer to the callback invoked when an error message is received */
    Websocket_OnErrorCallback onError;
    /** Pointer to the callback invoked when a close message is received */
    Websocket_OnCloseCallback onClose;
    /** Content of the request for establishing a connection on the client */
    Websocket_RequestOptions requestOptions;
};

typedef enum OH_Websocket_ErrCode {
    /**
     * Operation success.
     */
    WEBSOCKET_OK = 0,

    /**
     * @brief Error code base.
     */
    E_BASE = 1000,

    /**
     * @brief The websocket client is null.
     */
    WEBSOCKET_CLIENT_IS_NULL = (E_BASE + 1),

    /**
     * @brief A webSocket client is not created.
     */
    WEBSOCKET_CLIENT_IS_NOT_CREAT = (E_BASE + 2),

    /**
     * @brief An error occurs while setting up a websocket connection.
     */
    WEBSOCKET_CONNECTION_ERROR = (E_BASE + 3),

    /**
     * @brief An error occurs while parsing websocket connection parameters.
     */
    WEBSOCKET_CONNECTION_PARSEURL_ERROR = (E_BASE + 5),

    /**
     * @brief The memory is insufficient for creating a context during websocket connection setup.
     */
    WEBSOCKET_CONNECTION_NO_MEMOERY = (E_BASE + 6),

    /**
     * @brief The websocket connection is closed by the peer.
     */
    WEBSOCKET_PEER_INITIATED_CLOSE = (E_BASE + 7),

    /**
     * @brief The websocket connection is destroyed.
     */
    WEBSOCKET_DESTROY = (E_BASE + 8),

    /**
     * @brief An incorrect protocol is used for websocket connection.
     */
    WEBSOCKET_PROTOCOL_ERROR = (E_BASE + 9),

    /**
     * @brief The memory for the websocket client to send data is insufficient.
     */
    WEBSOCKET_SEND_NO_MEMOERY_ERROR = (E_BASE + 10),

    /**
     * @brief The data sent by the websocket client is null.
     */
    WEBSOCKET_SEND_DATA_NULL = (E_BASE + 11),

    /**
     * @brief The length of the data sent by the websocket client exceeds the limit.
     */
    WEBSOCKET_DATA_LENGTH_EXCEEDS = (E_BASE + 12),

    /**
     * @brief The queue length of the data sent by the websocket client exceeds the limit.
     */
    WEBSOCKET_QUEUE_LENGTH_EXCEEDS = (E_BASE + 13),

    /**
     * @brief The context of the websocket client is null.
     */
    WEBSOCKET_ERROR_NO_CLIENTCONTEX = (E_BASE + 14),

    /**
     * @brief The header of the webSocket client is null.
     */
    WEBSOCKET_ERROR_NO_HEADR_CONTEXT = (E_BASE + 15),

    /**
     * @brief The header of the websocket client exceeds the limit.
     */
    WEBSOCKET_ERROR_NO_HEADR_EXCEEDS = (E_BASE + 16),

    /**
     * @brief The websocket client is not connected.
     */
    WEBSOCKET_ERROR_HAVE_NO_CONNECT = (E_BASE + 17),

    /**
     * @brief The websocket client does not have the connection context.
     */
    WEBSOCKET_ERROR_HAVE_NO_CONNECT_CONTEXT = (E_BASE + 18),
} OH_Websocket_ErrCode;

#ifdef __cplusplus
}
#endif

#endif // NET_WEBSOCKET_TYPE_H