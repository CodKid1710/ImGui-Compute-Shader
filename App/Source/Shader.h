#pragma once

#include <filesystem>

uint32_t CreateComputeShader(const std::filesystem::path& path);
uint32_t ReloadComputeShader(uint32_t shaderHandle, const std::filesystem::path& path);