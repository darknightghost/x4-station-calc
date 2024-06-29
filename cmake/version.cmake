file (STRINGS
    "${CMAKE_CURRENT_SOURCE_DIR}/VERSION"
    VERSION
    LIMIT_COUNT     1
)

string (STRIP
    "${VERSION}"
    VERSION
)

string (REPLACE
    "."
    ";"
    __version_replaced
    "${VERSION}"
)

list (GET
    __version_replaced
    0
    MAJOR_VERSION
)

list (GET
    __version_replaced
    1
    MINOR_VERSION
)

list (GET
    __version_replaced
    2
    PATCH_VERSION
)

unset (__version_replaced)

message (STATUS "Target Version - ${VERSION}")
