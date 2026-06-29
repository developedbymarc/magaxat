add_library(protocol INTERFACE)

target_sources(protocol
    INTERFACE
        FILE_SET HEADERS
        FILES
            protocol.h
)
