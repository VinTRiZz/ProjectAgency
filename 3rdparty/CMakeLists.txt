set( BUILD_EXAMPLES  OFF CACHE BOOL "Extra 3rdparty function" FORCE)
add_subdirectory(websocketpp)

if (PAG_BUILD_MANAGER)
    set( BUILD_BROTLI           OFF CACHE BOOL "Extra 3rdparty function" FORCE)
    set( BUILD_C-ARES           OFF CACHE BOOL "Extra 3rdparty function" FORCE)
    set( BUILD_CTL              OFF CACHE BOOL "Extra 3rdparty function" FORCE)
    set( BUILD_DOC              OFF CACHE BOOL "Extra 3rdparty function" FORCE)
    set( BUILD_EXAMPLES         OFF CACHE BOOL "Extra 3rdparty function" FORCE)
    set( BUILD_GMOCK            OFF CACHE BOOL "Extra 3rdparty function" FORCE)
    set( BUILD_MYSQL            OFF CACHE BOOL "Extra 3rdparty function" FORCE)
    set( BUILD_ORM              ON  CACHE BOOL "Extra 3rdparty function" FORCE)
    set( BUILD_POSTGRESQL       ON  CACHE BOOL "Extra 3rdparty function" FORCE)
    set( BUILD_REDIS            OFF CACHE BOOL "Extra 3rdparty function" FORCE)
    set( BUILD_SQLITE           OFF CACHE BOOL "Extra 3rdparty function" FORCE)
    set( BUILD_TESTING          OFF CACHE BOOL "Extra 3rdparty function" FORCE)
    set( BUILD_TESTS            OFF CACHE BOOL "Extra 3rdparty function" FORCE)
    add_subdirectory(drogon)

    # Disable annoying warnings from a library
    target_compile_options(drogon PRIVATE
        # MSVC: Disables 'deprecated' warning (C4996)
        $<$<CXX_COMPILER_ID:MSVC>:/wd4996>

        # GCC, Clang, AppleClang: Disables deprecation warnings
        $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:AppleClang>,$<CXX_COMPILER_ID:GNU>>:-Wno-deprecated-declarations>
    )
endif()

