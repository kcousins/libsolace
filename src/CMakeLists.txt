
set(SOURCE_FILES
        assert.cpp
        exception.cpp
        errorDomain.cpp
        error.cpp
        atom.cpp
        char.cpp

        memoryView.cpp
        mutableMemoryView.cpp
        memoryResource.cpp
        memoryManager.cpp
        byteReader.cpp
        byteWriter.cpp

        array.cpp
        base16.cpp
#        base32.cpp
        base64.cpp
        string.cpp
        stringBuilder.cpp
        stringView.cpp

        version.cpp
        path.cpp
        encoder.cpp
        env.cpp
        uuid.cpp

        hashing/messageDigest.cpp
        hashing/md5.cpp
        hashing/murmur3.cpp
        hashing/sha1.cpp
        hashing/sha2.cpp
        hashing/sha3.cpp
        )

add_library(${PROJECT_NAME} ${SOURCE_FILES})

install(TARGETS ${PROJECT_NAME}
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib)
