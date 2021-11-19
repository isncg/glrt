// header.h: 标准系统包含文件的包含文件，
// 或特定于项目的包含文件
//

#pragma once

#include "targetver.h"
#define WIN32_LEAN_AND_MEAN             // 从 Windows 头文件中排除极少使用的内容
// Windows 头文件
#include <windows.h>
// C 运行时头文件
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include <iostream>
#include <sstream>

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <vector>
#include <map>

typedef glm::vec3 Vector3;
typedef glm::vec2 Vector2;
typedef glm::vec4 Vector4;
typedef glm::mat4 Matrix4x4;
typedef glm::vec4 Color;
typedef glm::lowp_i8vec4 Color32;
typedef glm::ivec2 Vector2i;

template<typename T>
GLint GetComponentCount(const T& tag);
GLint GetComponentCount(const Color32& tag);

GLenum GetComponentType(const Vector3& tag);
GLenum GetComponentType(const Vector2& tag);
GLenum GetComponentType(const Vector4& tag);
GLenum GetComponentType(const Color32& tag);