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
 * libSolace
 *	@file		stringBuilder.cpp
 *	@author		$LastChangedBy$
 *	@date		$LastChangedDate$
 *	@brief		Implementation of StringBuilder
 *	ID:			$Id$
 ******************************************************************************/
#include "solace/stringBuilder.hpp"
#include "solace/exception.hpp"

#include "solace/readBuffer.hpp"

using namespace Solace;



StringBuilder::StringBuilder(MemoryView&& buffer, StringView str)
    : StringBuilder(std::move(buffer))
{
    _buffer.write(str.view());
}

StringBuilder::StringBuilder(MemoryBuffer&& buffer, StringView str)
    : StringBuilder(std::move(buffer))
{
    _buffer.write(str.view());
}

StringBuilder& StringBuilder::append(char c) {
    _buffer.write(c);

	return *this;
}

StringBuilder& StringBuilder::append(Char c) {
    return append(StringView(c.c_str(), c.getBytesCount()));
}

StringBuilder& StringBuilder::append(StringView cstr) {
    _buffer.write(cstr.view());

	return *this;
}

StringView
StringBuilder::view() const noexcept {
    return StringView(_buffer.viewWritten().dataAs<const char>(), _buffer.position());
}


String
StringBuilder::toString() const {
    if (_buffer.position() == 0)
        return String::Empty;

    const auto written = _buffer.viewWritten();
    return StringView{written.dataAs<char>(), static_cast<size_type>(written.size())};
}

bool StringBuilder::empty() const {
    return (_buffer.position() == 0);
}

StringBuilder::size_type
StringBuilder::length() const noexcept {
    return _buffer.position();
}


String
StringBuilder::substring(size_type from, size_type to) const {
    // TODO(abbyssoul): Check for index out of range

    auto data = _buffer.viewWritten();
    return String(std::string(
            reinterpret_cast<const char*>(data.dataAddress() + from),
            to - from));
}


Optional<StringBuilder::size_type>
StringBuilder::indexOf(const Char& ch, size_type fromIndex) const {
    // TODO(abbyssoul): Check for index out of range

    MemoryView::value_type buffer[Char::max_bytes];
    auto b = wrapMemory(buffer);


    ReadBuffer reader(_buffer.viewWritten());
    for (size_type i = fromIndex; i + ch.getBytesCount() < reader.limit(); ++i) {
        reader.read(i, b, ch.getBytesCount());

        if (ch.equals(b)) {
            return Optional<size_type>(i);
        }
    }

    return none;
}
