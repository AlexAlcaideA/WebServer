#pragma once

#include <string>
#include <vector>
#include "Configuration.hpp"

template <typename Context>
bool SetRoot(Context* ctx, const std::vector<std::string>& args);
template <typename Context>
bool AddIndex(Context* ctx, const std::vector<std::string>& args);
template <typename Context>
bool SetListen(Context* ctx, const std::string& ip, const unsigned int& port);

#include "../template/Parser.tpp"