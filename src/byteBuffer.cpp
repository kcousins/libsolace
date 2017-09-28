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
 *	@file		byteBuffer.cpp
 *	@author		$LastChangedBy: soultaker $
 *	@date		$LastChangedDate$
 *	@brief		Implementation of ByteBuffer
 *	ID:			$Id$
 ******************************************************************************/
#include "solace/byteBuffer.hpp"
#include "solace/exception.hpp"


#include <cstring>  // memcpy


using Solace::MemoryView;
using Solace::byte;
using Solace::ByteBuffer;


ByteBuffer& ByteBuffer::limit(size_type newLimit) {
    if (capacity() < newLimit) {
        raise<IllegalArgumentException>("newLimit");
    }

    _limit = newLimit;

    return *this;
}


ByteBuffer& ByteBuffer::position(size_type newPosition) {
    if (limit() < newPosition) {
        raise<IllegalArgumentException>("newPosition");
    }

    _position = newPosition;

    return *this;
}

ByteBuffer& ByteBuffer::advance(size_type increment) {
    if (remaining() < increment) {
        raise<IllegalArgumentException>("positionIncrement");
    }

    _position += increment;

    return *this;
}


byte ByteBuffer::get() {
    if (remaining() < 1) {
        raise<OverflowException>(_position + 1, _position, _limit);
    }

    return _storage[_position++];
}

byte ByteBuffer::get(size_type pos) const {
    if (limit() <= pos) {
        raise<IllegalArgumentException>("pos");
    }

    return _storage[pos];
}


ByteBuffer& ByteBuffer::read(MemoryView& from, size_type bytesToRead) {
    if (from.size() < bytesToRead) {
        raise<IllegalArgumentException>("bytesToRead");
    }

    return read(from.dataAddress(), bytesToRead);
}


ByteBuffer& ByteBuffer::read(void* bytes, size_type count) {
    if (remaining() < count) {
        raise<OverflowException>(_position + count, _position, _limit);
    }

    memcpy(bytes, _storage.dataAddress(_position), count);
    _position += count;

    return (*this);
}


const ByteBuffer& ByteBuffer::read(size_type offset, byte* bytes, size_type count) const {
    if (_limit < (offset + count)) {
        raise<OverflowException>(offset + count, 0, _limit);
    }

    memcpy(bytes, _storage.dataAddress(offset), count);

    return (*this);
}


const ByteBuffer& ByteBuffer::read(size_type offset, MemoryView& bytes, size_type count) const {
    if (_limit < (offset + count)) {
        raise<OverflowException>(offset + count, 0, _limit);
    }

    if (count < bytes.size()) {
        raise<OverflowException>(count, 0, bytes.size());
    }

    memcpy(bytes.dataAddress(), _storage.dataAddress(offset), count);

    return (*this);
}


ByteBuffer& ByteBuffer::write(const MemoryView& dest, size_type bytesToWrite) {
    if (dest.size() < bytesToWrite) {
         raise<OverflowException>("bytesToWrite", bytesToWrite, 0, dest.size());
    }

    return write(dest.dataAddress(), bytesToWrite);
}

ByteBuffer& ByteBuffer::write(const void* bytes, size_type count) {
    if (remaining() < count) {
         raise<OverflowException>(_position + count, _position, remaining());
    }

    memcpy(_storage.dataAddress(_position), bytes, count);

    return advance(count);
}