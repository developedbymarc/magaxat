add_executable(Magaxat)

target_sources(Magaxat
    PRIVATE
        main.c
)

target_link_libraries(Magaxat
    PRIVATE
        protocol
)
