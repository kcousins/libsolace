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
 * libSolace: MutableMemoryView
 *	@file		solace/mutableMemoryView.hpp
 *	@brief		Mutable version of MemoryView
 ******************************************************************************/
#pragma once
#ifndef SOLACE_MUTABLEMEMORYVIEW_HPP
#define SOLACE_MUTABLEMEMORYVIEW_HPP

#include "solace/memoryView.hpp"


namespace Solace {

/* View into a fixed-length raw memory buffer which allows mutation of the undelaying data.
 * A very thin abstruction on top of raw memory address - it remembers memory block address and size.
 *
 * View has a value semantic and gives a user random access to the undelying memory.
 *
 * For a mutable access please use @see MemoryView
 * For the a convenient adapter for stream semantic please @see WriteBuffer
 */
class MutableMemoryView
        : public MemoryView {
public:
    using MemoryView::size_type;
    using MemoryView::value_type;

    using MemoryView::const_iterator;
    using MemoryView::const_reference;

    using reference = value_type &;
    using iterator = value_type *;

public:

    /** Construct an empty memory view */
    MutableMemoryView() noexcept = default;

    /**
     * Construct a memory view from a row pointer and the size
     * @param data A pointer to the contigues memory region.
     * @param dataSize The size of the contigues memory region.
     *
     * @throws IllegalArgumentException if the `data` is nullptr while size is non-zero.
     */
    MutableMemoryView(void* data, size_type dataSize) :
        MemoryView(data, dataSize)
    {}

    MutableMemoryView(MutableMemoryView const&) noexcept = default;
    MutableMemoryView& operator= (MutableMemoryView const&) noexcept = default;

    MutableMemoryView(MutableMemoryView&& rhs) noexcept = default;

    MutableMemoryView& swap(MutableMemoryView& rhs) noexcept {
        MemoryView::swap(rhs);

        return (*this);
    }


    MutableMemoryView& operator= (MutableMemoryView&& rhs) noexcept {
        return swap(rhs);
    }

    using MemoryView::equals;

    bool equals(MutableMemoryView const& other) const noexcept {
        return MemoryView::equals(other);
    }


    /**
     * Return iterator to beginning of the collection
     * @return iterator to beginning of the collection
     */
    iterator begin() noexcept {
        return const_cast<value_type*>(dataAddress());
    }
    using MemoryView::begin;

    /**
     * Return iterator to end of the collection
     * @return iterator to end of the collection
     */
    iterator end() noexcept {
        return const_cast<value_type*>(dataAddress() + size());
    }
    using MemoryView::end;

    reference  operator[] (size_type index);
    using MemoryView::operator[];

    using MemoryView::dataAddress;

    value_type* dataAddress();
    value_type* dataAddress(size_type offset);


    using MemoryView::dataAs;
    template <typename T>
    T* dataAs(size_type offset = 0) {
        assertIndexInRange(offset, 0, this->size());
        assertIndexInRange(offset + sizeof(T), offset, this->size() + 1);

        return reinterpret_cast<T*>(dataAddress() + offset);
    }


    /**
     * Copy data from the given memory view into this one
     * @param source Source of data to be written into this location.
     * @param offset Offset location into this buffer to copy data to.
     */
    void write(MemoryView const& source, size_type offset = 0);

    /**
     * Copy data from this buffer into the given one.
     * @param data Data destinatio to transer data into.
     */
    void read(MutableMemoryView& dest);

    /**
     * Copy data from this buffer into the given one.
     * @param data Data destinatio to transer data into.
     * @param bytesToRead Number of bytes to copy from this bufer into the destanation.
     * @param offset Offset location into this buffer to start reading from.
     */
    void read(MutableMemoryView& dest, size_type bytesToRead, size_type offset = 0);

    /** Fill memory block with the given value.
     *
     * @param value Value to fill memory block with.
     *
     * @return A reference to this for fluent interface
     */
    MutableMemoryView& fill(byte value);

    /** Fill memory block with the given value.
     *
     * @param value Value to fill memory block with.
     * @param from Offset to start the fill from.
     * @param to Index to fill the view upto.
     *
     * @return A reference to this for fluent interface
     */
    MutableMemoryView& fill(byte value, size_type from, size_type to);

    /**
     * Lock this virtual address space memory into RAM, preventing that memory from being paged to the swap area.
     * @note: Memory locking and unlocking are performed in units of whole pages.
     * That is if when this memory view if locked - it will lock all memory that falls onto the same pages as this.
     */
    MutableMemoryView& lock();

    /**
     * Unlock virtual address space, so that pages in the specified virtual address range may once more to be
     * swapped out if required by the kernel memory manager.
     * @note: Memory locking and unlocking are performed in units of whole pages.
     * That is if when this memory view is unlocked - it will also unlock all memory that falls onto the same pages.
     */
    MutableMemoryView& unlock();

    using MemoryView::slice;
    MutableMemoryView slice(size_type from, size_type to);

    template<typename T, typename... Args>
    T* construct(Args&&... args) {
        assertIndexInRange(static_cast<size_type>(sizeof(T)), static_cast<size_type>(0), this->size() + 1);

        return new (dataAddress()) T(std::forward<Args>(args)...);
    }

    template<typename T>
    void destruct(T* t) {
        t->~T();
    }
};


/**
 * Wrap an memory pointer into a memory view object.
 *
 * @param data Pointer the memory to wrap
 * @param size Size in bytes of the allocated memory segment
 * @param freeFunc A funcion to call when the wrapper object is destroyed.
 * This can be used to free the memory pointed by the data pointed as the MemoryView does not assume the ownership.
 *
 * @return MemoryView object wrapping the memory address given
 */
inline MutableMemoryView wrapMemory(void* data, MutableMemoryView::size_type size) { return {data, size}; }

inline MutableMemoryView wrapMemory(byte* data, MutableMemoryView::size_type size) { return {data, size}; }

inline MutableMemoryView wrapMemory(char* data, MutableMemoryView::size_type size) { return {data, size}; }

template<typename PodType, size_t N>
inline MutableMemoryView wrapMemory(PodType (&data)[N]) {
    return wrapMemory(static_cast<void*>(data), N * sizeof(PodType));
}


inline void
swap(MutableMemoryView& a, MutableMemoryView& b) {
    a.swap(b);
}


inline
bool operator== (MutableMemoryView const& rhs, MutableMemoryView const& lhs) noexcept {
    return rhs.equals(lhs);
}

inline
bool operator!= (MutableMemoryView const& rhs, MutableMemoryView const& lhs) noexcept {
    return !rhs.equals(lhs);
}

inline
bool operator== (MutableMemoryView const& rhs, MemoryView const& lhs) noexcept {
    return rhs.equals(lhs);
}

inline
bool operator!= (MutableMemoryView const& rhs, MemoryView const& lhs) noexcept {
    return !rhs.equals(lhs);
}


/// Some data manipulication utilities:

/*
 * 32-bit integer manipulation (big endian)
 */
inline
void getUint32_BE(uint32& n, const byte* b, size_t i) {
    n =   static_cast<uint32>(b[i    ]) << 24
        | static_cast<uint32>(b[i + 1]) << 16
        | static_cast<uint32>(b[i + 2]) <<  8
        | static_cast<uint32>(b[i + 3]);
}

inline
void putUint32_BE(uint32& n, byte* b, size_t i) {
    b[i    ] = static_cast<byte>(n >> 24);
    b[i + 1] = static_cast<byte>(n >> 16);
    b[i + 2] = static_cast<byte>(n >>  8);
    b[i + 3] = static_cast<byte>(n);
}


/*
 * 32-bit integer manipulation macros (little endian)
 */
inline
void getInt32_LE(int32& n, const byte* b, size_t i) {
    n =   static_cast<int32>(b[i   ])
        | static_cast<int32>(b[i + 1]) <<  8
        | static_cast<int32>(b[i + 2]) << 16
        | static_cast<int32>(b[i + 3]) << 24;
}

inline
void getUint32_LE(uint32& n, const byte* b, size_t i) {
    n =   static_cast<uint32>(b[i   ])
        | static_cast<uint32>(b[i + 1]) <<  8
        | static_cast<uint32>(b[i + 2]) << 16
        | static_cast<uint32>(b[i + 3]) << 24;
}

inline
void putInt32_LE(int32& n, byte* b, size_t i) {
    b[i    ] = static_cast<byte>((n)       & 0xFF);
    b[i + 1] = static_cast<byte>((n >>  8) & 0xFF);
    b[i + 2] = static_cast<byte>((n >> 16) & 0xFF);
    b[i + 3] = static_cast<byte>((n >> 24) & 0xFF);
}

inline
void putUint32_LE(uint32& n, byte* b, size_t i) {
    b[i    ] = static_cast<byte>((n)       & 0xFF);
    b[i + 1] = static_cast<byte>((n >>  8) & 0xFF);
    b[i + 2] = static_cast<byte>((n >> 16) & 0xFF);
    b[i + 3] = static_cast<byte>((n >> 24) & 0xFF);
}


/*
 * 64-bit integer manipulation macros (little endian)
 */
inline
void getUint64_LE(uint64& n, const byte* b, size_t i) {
    n =   static_cast<uint64>(b[i   ])
        | static_cast<uint64>(b[i + 1]) <<  8
        | static_cast<uint64>(b[i + 2]) << 16
        | static_cast<uint64>(b[i + 3]) << 24
        | static_cast<uint64>(b[i + 4]) << 32
        | static_cast<uint64>(b[i + 5]) << 40
        | static_cast<uint64>(b[i + 6]) << 48
        | static_cast<uint64>(b[i + 7]) << 56;
}

inline
void putUint64_LE(uint64& n, byte* b, size_t i) {
    b[i    ] = static_cast<byte>((n)       & 0xFF);
    b[i + 1] = static_cast<byte>((n >>  8) & 0xFF);
    b[i + 2] = static_cast<byte>((n >> 16) & 0xFF);
    b[i + 3] = static_cast<byte>((n >> 24) & 0xFF);
    b[i + 4] = static_cast<byte>((n >> 32) & 0xFF);
    b[i + 5] = static_cast<byte>((n >> 40) & 0xFF);
    b[i + 6] = static_cast<byte>((n >> 48) & 0xFF);
    b[i + 7] = static_cast<byte>((n >> 56) & 0xFF);
}

}  // End of namespace Solace
#endif  // SOLACE_MUTABLEMEMORYVIEW_HPP