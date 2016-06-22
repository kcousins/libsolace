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
 *	@file		memoryView.cpp
 *	@author		$LastChangedBy: soultaker $
 *	@date		$LastChangedDate$
 *	@brief		Implementation of MemoryView
 *	ID:			$Id$
 ******************************************************************************/
#include "solace/byteBuffer.hpp"

#include <cstring>  // memcpy

using Solace::MemoryView;


MemoryView::MemoryView(size_type newSize):
    _size(newSize), _ownsData(true), _dataAddress(new value_type[newSize])
{
}


MemoryView::MemoryView(const MemoryView& rhs):
            _size(rhs._size),
            _ownsData(rhs._ownsData),
            _dataAddress(rhs._dataAddress)
{
    if (rhs._dataAddress && rhs._ownsData) {
        _dataAddress = new value_type[_size];

        memcpy(_dataAddress, rhs._dataAddress, _size * sizeof(value_type));
    }
}


MemoryView::MemoryView(size_type newSize, void* bytes, bool copyData):
    _size(newSize),
    _ownsData(copyData),
    _dataAddress(reinterpret_cast<value_type*>(bytes))
{
    if (!bytes) {
        // FIXME(abbyssoul): Review nullptr handling policy
//        raise<IllegalArgumentException>("bytes");
        _size = 0;
    }

    if (bytes && copyData) {
        _dataAddress = new value_type[_size];

        memcpy(_dataAddress, bytes, _size * sizeof(value_type));
    }
}


MemoryView::~MemoryView() {
    if (_ownsData && _dataAddress) {
        _ownsData = false;

        delete [] _dataAddress;

        _dataAddress = 0;
        _size = 0;
    }
}


MemoryView MemoryView::slice(size_type from, size_type to, bool copyData) const {
    return MemoryView {to - from, _dataAddress + from, copyData};
}