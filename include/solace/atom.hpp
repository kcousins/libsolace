/*
*  Copyright 2016 Ivan Ryabov
*
*  Licensed under the Apache License, Version 2.0 (the "License");
*  you may not use this file except in compliance with the License.
*  You may obtain a copy of the License at
*
*      http://www.apache.org/licenses/LICENSE-2.0
*
*  Unless required by applicable law or agreed to in writing, software
*  distributed under the License is distributed on an "AS IS" BASIS,
*  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
*  See the License for the specific language governing permissions and
*  limitations under the License.
*/
/*******************************************************************************
 * libSolace: atom
 *	@file		solace/atom.hpp
 ******************************************************************************/
#pragma once
#ifndef SOLACE_ATOM_HPP
#define SOLACE_ATOM_HPP

#include "solace/types.hpp"

#include <type_traits>
#include <climits>
#include <cstdint>


namespace Solace {


/**
 * Atoms value type
 */
enum class AtomValue : uint64_t {
    /// @cond PRIVATE
    _dirty_little_hack = 1337
    /// @endcond
};


namespace detail {

template <typename T = std::uintmax_t>
constexpr std::enable_if_t<std::is_integral<T>::value, T>
wrap(const char *const str) noexcept {
    constexpr auto N = sizeof(T);
    T n {};
    std::size_t i {};
    while (i < N && str[i]) {
        n = (n << CHAR_BIT) | str[i++];
    }

    return (n << (N - i) * CHAR_BIT);
}

template <typename T>
std::enable_if_t<std::is_integral<T>::value>
unwrap(const T n, char *const buffer) noexcept {
    constexpr auto N = sizeof(T);
    constexpr auto lastbyte = static_cast<char>(~0);
    for (std::size_t i = 0UL; i < N; ++i) {
        buffer[i] = ((n >> (N - i - 1) * CHAR_BIT) & lastbyte);
    }

    buffer[N] = '\0';
}

}  // namespace detail


/// Creates an atom value from a given short string literal.
template <size_t Size>
[[nodiscard]]
AtomValue atom(char const (&str)[Size]) {
    // last character is the NULL terminator
    constexpr auto kMaxLiteralSize = sizeof(std::uintmax_t);
    static_assert(Size <= kMaxLiteralSize, "String literal too long");

    return static_cast<AtomValue>(detail::wrap(str));
}

}  // End of namespace Solace
#endif  // SOLACE_ATOM_HPP
