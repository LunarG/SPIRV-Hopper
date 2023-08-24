# ~~~
# Copyright (c) 2023 LunarG, Inc.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
# ~~~
cmake_minimum_required(VERSION 3.17.2)

project(HOPPER LANGUAGES CXX)

set(HOPPER_CPP_STANDARD 17 CACHE STRING "Set the C++ standard to build against.")
set(CMAKE_CXX_STANDARD ${HOPPER_CPP_STANDARD})
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CMAKE_CXX_VISIBILITY_PRESET "hidden")
set(CMAKE_VISIBILITY_INLINES_HIDDEN "YES")

if (GLSLANG_INSTALL_DIR)
    list(APPEND CMAKE_PREFIX_PATH ${GLSLANG_INSTALL_DIR})
endif()
if (SPIRV_HEADERS_INSTALL_DIR)
    list(APPEND CMAKE_PREFIX_PATH ${SPIRV_HEADERS_INSTALL_DIR})
endif()
if (SPIRV_REFLECT_INSTALL_DIR)
    list(APPEND CMAKE_PREFIX_PATH ${SPIRV_REFLECT_INSTALL_DIR})
endif()
if (VULKAN_HEADERS_INSTALL_DIR)
    list(APPEND CMAKE_PREFIX_PATH ${VULKAN_HEADERS_INSTALL_DIR})
endif()

find_package(VulkanHeaders REQUIRED CONFIG QUIET)
target_compile_definitions(Vulkan::Headers INTERFACE)

add_library(VVL-SPIRV-LIBS INTERFACE)
find_package(SPIRV-Headers REQUIRED CONFIG QUIET)
target_link_libraries(VVL-SPIRV-LIBS INTERFACE SPIRV-Headers::SPIRV-Headers)

find_package(glslang REQUIRED CONFIG QUIET)

add_library(VkLayer_utils STATIC)
target_sources(VkLayer_utils PRIVATE
    src/generated/vk_function_pointers.h
    src/generated/vk_function_pointers.cpp
    src/generated/vk_typemap_helper.h
)

target_link_libraries(VkLayer_utils PUBLIC
    Vulkan::Headers
)
target_include_directories(VkLayer_utils SYSTEM PRIVATE external)
target_include_directories(VkLayer_utils PUBLIC src)

add_library(spirv-reflect STATIC)
target_sources(spirv-reflect PRIVATE
    ${SPIRV_REFLECT_INSTALL_DIR}/spirv_reflect.h
    ${SPIRV_REFLECT_INSTALL_DIR}/spirv_reflect.cpp
)
target_include_directories(spirv-reflect SYSTEM PUBLIC ${SPIRV_REFLECT_INSTALL_DIR})

# Used so SPIRV-Reflect can use the SPIRV-Headers as SPIRV_HEADERS_INSTALL_DIR
target_compile_definitions(spirv-reflect PUBLIC SPIRV_REFLECT_USE_SYSTEM_SPIRV_H)

target_link_libraries(spirv-reflect PUBLIC
    VVL-SPIRV-LIBS
)

add_executable(spirv-hopper)

target_sources(spirv-hopper PRIVATE
    src/main.cpp
    src/spirv_hopper.h
    src/spirv_hopper.cpp
    src/pass_through_shaders.cpp
    src/vulkan_object.h
    src/vulkan_object.cpp
)
target_include_directories(spirv-hopper SYSTEM PRIVATE src)
target_link_libraries(spirv-hopper PRIVATE
    Vulkan::Headers
    glslang::SPIRV
    VkLayer_utils
    spirv-reflect
)