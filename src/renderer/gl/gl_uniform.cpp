/*
===========================================================================
  VGLX https://vglx.org
  Copyright © 2024 - Present, Shlomi Nissan
===========================================================================
*/

#include "renderer/gl/gl_uniform.hpp"

#include "utilities/logger.hpp"

namespace vglx {

namespace {

UniformType ToUniformType(GLenum type) {
    switch (type) {
        case GL_BOOL: return UniformType::Bool;
        case GL_FLOAT: return UniformType::Float;
        case GL_FLOAT_MAT3: return UniformType::Matrix3;
        case GL_FLOAT_MAT4: return UniformType::Matrix4;
        case GL_FLOAT_VEC2: return UniformType::Vector2;
        case GL_FLOAT_VEC3: return UniformType::Vector3;
        case GL_FLOAT_VEC4: return UniformType::Vector4;
        case GL_INT: return UniformType::Int;
        case GL_SAMPLER_2D: return UniformType::Sampler2D;
        default: return UniformType::Unsupported;
    }
}

}

GLUniform::GLUniform(std::string_view name, GLint location, GLenum type)
  : name_(name),
    location_(location),
    type_(ToUniformType(type))
{
    if (type_ == UniformType::Unsupported) {
        Logger::Log(LogLevel::Error, "Unsupported GL uniform type {}:{}", type, name);
    }
}

auto GLUniform::SetValue(const void* value) -> void {
    switch(type_) {
        case UniformType::Bool:
            if (data_.b != *reinterpret_cast<const float*>(value)) {
                data_.b = *reinterpret_cast<const float*>(value);
                needs_upload_ = true;
            }
            break;
        case UniformType::Float:
            if (data_.f != *reinterpret_cast<const float*>(value)) {
                data_.f = *reinterpret_cast<const float*>(value);
                needs_upload_ = true;
            }
            break;
        case UniformType::Int:
            if (data_.i != *reinterpret_cast<const int*>(value)) {
                data_.i = *reinterpret_cast<const int*>(value);
                needs_upload_ = true;
            }
            break;
         case UniformType::Matrix3:
            if (data_.m3 != *reinterpret_cast<const Matrix3*>(value)) {
                data_.m3 = *reinterpret_cast<const Matrix3*>(value);
                needs_upload_ = true;
            }
            break;
        case UniformType::Matrix4:
            if (data_.m4 != *reinterpret_cast<const Matrix4*>(value)) {
                data_.m4 = *reinterpret_cast<const Matrix4*>(value);
                needs_upload_ = true;
            }
            break;
        case UniformType::Sampler2D:
            if (data_.i != *reinterpret_cast<const int*>(value)) {
                data_.i = *reinterpret_cast<const int*>(value);
                needs_upload_ = true;
            }
            break;
        case UniformType::Vector2:
            if (data_.v2 != *reinterpret_cast<const Vector2*>(value)) {
                data_.v2 = *reinterpret_cast<const Vector2*>(value);
                needs_upload_ = true;
            }
            break;
        case UniformType::Vector3:
            if (data_.v3 != *reinterpret_cast<const Vector3*>(value)) {
                data_.v3 = *reinterpret_cast<const Vector3*>(value);
                needs_upload_ = true;
            }
            break;
        case UniformType::Vector4:
            if (data_.v4 != *reinterpret_cast<const Vector4*>(value)) {
                data_.v4 = *reinterpret_cast<const Vector4*>(value);
                needs_upload_ = true;
            }
            break;
        case UniformType::Unsupported: break;
    }
}

auto GLUniform::UploadIfNeeded() -> void {
    if (!needs_upload_) return;
    switch(type_) {
        case UniformType::Bool: glUniform1i(location_, data_.b ? 1 : 0); break;
        case UniformType::Float: glUniform1f(location_, data_.f); break;
        case UniformType::Int: glUniform1i(location_, data_.i); break;
        case UniformType::Matrix3: glUniformMatrix3fv(location_, 1, GL_FALSE, &data_.m3[0][0]); break;
        case UniformType::Matrix4: glUniformMatrix4fv(location_, 1, GL_FALSE, &data_.m4[0][0]); break;
        case UniformType::Sampler2D: glUniform1i(location_, data_.i); break;
        case UniformType::Vector2: glUniform2fv(location_, 1, &data_.v2[0]); break;
        case UniformType::Vector3: glUniform3fv(location_, 1, &data_.v3[0]); break;
        case UniformType::Vector4: glUniform4fv(location_, 1, &data_.v4[0]); break;
        case UniformType::Unsupported: break;
    }

    needs_upload_ = false;
}

}