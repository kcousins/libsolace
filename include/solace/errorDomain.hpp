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
 * libSolace: Domain for error codes
 *	@file		solace/errorDomain.hpp
 ******************************************************************************/
#pragma once
#ifndef SOLACE_ERRORDOMAIN_HPP
#define SOLACE_ERRORDOMAIN_HPP

#include "solace/types.hpp"
#include "solace/atom.hpp"
#include "solace/optional.hpp"
#include "solace/stringView.hpp"


namespace Solace {


/**
 * Base class for error domains.
 * This is an interface that all ErrorDomain are expected to implement.
 */
class ErrorDomain {
public:
    virtual ~ErrorDomain() noexcept = default;

    ErrorDomain() = default;
    ErrorDomain(ErrorDomain const&) = delete;
    ErrorDomain(ErrorDomain&&) = default;
    ErrorDomain& operator= (ErrorDomain const&) = delete;
    ErrorDomain& operator= (ErrorDomain&&) = default;

    virtual StringView getName() const noexcept = 0;
    virtual StringView getMessage(int code) const noexcept = 0;
};


/**
 * Lookup error domain/category by atom value
 * @param categoryId Atom identifing error domain
 * @return An error domain if one was found.
 */
Optional<ErrorDomain*> getErrorDomain(AtomValue categoryId) noexcept;


}  // End of namespace Solace
#endif  // SOLACE_ERRORDOMAIN_HPP
