# Library sources
set(rawrtcdc_SOURCES
        crc32c.c
        data_channel.c
        data_channel_parameters.c
        data_transport.c
        main.c
        sctp_capabilities.c
        sctp_transport.c)

# Library internal headers
set(rawrtcdc_HEADERS
        crc32c.h
        data_channel.h
        data_channel_parameters.h
        data_transport.h
        main.h
        sctp_capabilities.h
        sctp_transport.h)

# Optional: SCTP redirect transport tool sources
if (SCTP_REDIRECT_TRANSPORT)
    list(APPEND rawrtcdc_SOURCES
            sctp_redirect_transport.c)
    list(APPEND rawrtcdc_HEADERS
            sctp_redirect_transport.h)
endif ()

# Setup library (link & install)
add_library(rawrtcdc ${rawrtcdc_SOURCES})
set_property(TARGET rawrtcdc
        PROPERTY POSITION_INDEPENDENT_CODE ON)
target_link_libraries(rawrtcdc
        ${rawrtcdc_DEP_LIBRARIES})
install(TARGETS rawrtcdc
        DESTINATION lib)

# Optional: SCTP redirect transport tool definitions
if (SCTP_REDIRECT_TRANSPORT)
    # And set the preprocessor variable
    target_compile_definitions(rawrtcdc PRIVATE SCTP_REDIRECT_TRANSPORT)
endif ()

# ARM: Disable hardware CRC32-C (which requires SSE 4.2)
if (${CMAKE_SYSTEM_PROCESSOR} MATCHES "arm")
    target_compile_definitions(rawrtcdc PRIVATE DISABLE_HW_CRC32C)
endif ()

# Install internal headers
install(FILES ${rawrtcdc_HEADERS} DESTINATION include/rawrtcdc/internal)

# Generate pkg-config file & install it
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/pkg-config.pc.cmakein
        ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.pc @ONLY)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.pc
        DESTINATION lib/pkgconfig)
