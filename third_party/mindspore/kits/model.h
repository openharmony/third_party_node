/**
 * Copyright 2021 Huawei Technologies Co., Ltd
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/**
 * @addtogroup MindSpore
 * @{
 *
 * @brief 提供MindSpore Lite的模型推理相关接口。
 *
 * @Syscap SystemCapability.Ai.MindSpore
 * @since 9
 */

/**
 * @file model.h
 *
 * @brief 提供了模型相关接口，可以用于模型创建、模型推理等。
 *
 * @library libmindspore_lite_ndk.so
 * @since 9
 */
#ifndef MINDSPORE_INCLUDE_C_API_MODEL_C_H
#define MINDSPORE_INCLUDE_C_API_MODEL_C_H

#include "mindspore/tensor.h"
#include "mindspore/context.h"
#include "mindspore/status.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef void *OH_AI_ModelHandle;

typedef struct OH_AI_TensorHandleArray {
  size_t handle_num;
  OH_AI_TensorHandle *handle_list;
} OH_AI_TensorHandleArray;

#define OH_AI_MAX_SHAPE_NUM 32
typedef struct OH_AI_ShapeInfo {
  size_t shape_num;
  int64_t shape[OH_AI_MAX_SHAPE_NUM];
} OH_AI_ShapeInfo;

typedef struct OH_AI_CallBackParam {
  char *node_name;
  char *node_type;
} OH_AI_CallBackParam;

typedef bool (*OH_AI_KernelCallBack)(const OH_AI_TensorHandleArray inputs, const OH_AI_TensorHandleArray outputs,
                                     const OH_AI_CallBackParam kernel_Info);

/**
 * @brief Create a model object.
 * @return Model object handle.
 * @since 9
 */
OH_AI_API OH_AI_ModelHandle OH_AI_ModelCreate();

/**
 * @brief Destroy the model object.
 * @param model Model object handle address.
 * @since 9
 */
OH_AI_API void OH_AI_ModelDestroy(OH_AI_ModelHandle *model);

/**
 * @brief Build the model from model file buffer so that it can run on a device.
 * @param model Model object handle.
 * @param model_data Define the buffer read from a model file.
 * @param data_size Define bytes number of model file buffer.
 * @param model_type Define The type of model file.
 * @param model_context Define the context used to store options during execution.
 * @return OH_AI_Status.
 * @since 9
 */
OH_AI_API OH_AI_Status OH_AI_ModelBuild(OH_AI_ModelHandle model, const void *model_data, size_t data_size,
                                        OH_AI_ModelType model_type, const OH_AI_ContextHandle model_context);

/**
 * @brief Load and build the model from model path so that it can run on a device.
 * @param model Model object handle.
 * @param model_path Define the model file path.
 * @param model_type Define The type of model file.
 * @param model_context Define the context used to store options during execution.
 * @return OH_AI_Status.
 * @since 9
 */
OH_AI_API OH_AI_Status OH_AI_ModelBuildFromFile(OH_AI_ModelHandle model, const char *model_path,
                                                OH_AI_ModelType model_type, const OH_AI_ContextHandle model_context);

/**
 * @brief Resizes the shapes of inputs.
 * @param model Model object handle.
 * @param inputs The array that includes all input tensor handles.
 * @param shape_infos Defines the new shapes of inputs, should be consistent with inputs.
 * @param shape_info_num The num of shape_infos.
 * @return OH_AI_Status.
 * @since 9
 */
OH_AI_API OH_AI_Status OH_AI_ModelResize(OH_AI_ModelHandle model, const OH_AI_TensorHandleArray inputs,
                                         OH_AI_ShapeInfo *shape_infos, size_t shape_info_num);

/**
 * @brief Inference model.
 * @param model Model object handle.
 * @param inputs The array that includes all input tensor handles.
 * @param outputs The array that includes all output tensor handles.
 * @param before CallBack before predict.
 * @param after CallBack after predict.
 * @return OH_AI_Status.
 * @since 9
 */
OH_AI_API OH_AI_Status OH_AI_ModelPredict(OH_AI_ModelHandle model, const OH_AI_TensorHandleArray inputs,
                                          OH_AI_TensorHandleArray *outputs, const OH_AI_KernelCallBack before,
                                          const OH_AI_KernelCallBack after);

/**
 * @brief Obtains all input tensor handles of the model.
 * @param model Model object handle.
 * @return The array that includes all input tensor handles.
 * @since 9
 */
OH_AI_API OH_AI_TensorHandleArray OH_AI_ModelGetInputs(const OH_AI_ModelHandle model);

/**
 * @brief Obtains all output tensor handles of the model.
 * @param model Model object handle.
 * @return The array that includes all output tensor handles.
 * @since 9
 */
OH_AI_API OH_AI_TensorHandleArray OH_AI_ModelGetOutputs(const OH_AI_ModelHandle model);

/**
 * @brief Obtains the input tensor handle of the model by name.
 * @param model Model object handle.
 * @param tensor_name The name of tensor.
 * @return The input tensor handle with the given name, if the name is not found, an NULL is returned.
 * @since 9
 */
OH_AI_API OH_AI_TensorHandle OH_AI_ModelGetInputByTensorName(const OH_AI_ModelHandle model, const char *tensor_name);

/**
 * @brief Obtains the output tensor handle of the model by name.
 * @param model Model object handle.
 * @param tensor_name The name of tensor.
 * @return The output tensor handle with the given name, if the name is not found, an NULL is returned.
 * @since 9
 */
OH_AI_API OH_AI_TensorHandle OH_AI_ModelGetOutputByTensorName(const OH_AI_ModelHandle model, const char *tensor_name);

#ifdef __cplusplus
}
#endif
#endif  // MINDSPORE_INCLUDE_C_API_MODEL_C_H
