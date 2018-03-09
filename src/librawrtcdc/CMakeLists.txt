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
    # Add the relevant source files
    list(APPEND rawrtcdc_SOURCES
            sctp_redirect_transport.c)
endif ()

# Setup library (link & install)
add_library(rawrtcdc SHARED ${rawrtcdc_SOURCES})
target_link_libraries(rawrtcdc
        ${rawrtcdc_DEP_LIBRARIES})
add_library(rawrtcdc-static STATIC ${rawrtcdc_SOURCES})
target_link_libraries(rawrtcdc-static
        ${rawrtcdc_DEP_LIBRARIES})
install(TARGETS rawrtcdc rawrtcdc-static
        DESTINATION lib)

# Optional: SCTP redirect transport tool definitions
if (SCTP_REDIRECT_TRANSPORT)
    # And set the preprocessor variable
    target_compile_definitions(rawrtcdc PRIVATE SCTP_REDIRECT_TRANSPORT)
    target_compile_definitions(rawrtcdc-static PRIVATE SCTP_REDIRECT_TRANSPORT)
endif ()

# ARM: Disable hardware CRC32-C (which requires SSE 4.2)
if (${CMAKE_SYSTEM_PROCESSOR} MATCHES "arm")
    target_compile_definitions(rawrtcdc PRIVATE DISABLE_HW_CRC32C)
    target_compile_definitions(rawrtcdc-static PRIVATE DISABLE_HW_CRC32C)
endif ()

# Generate internal headers & install them
set(internal_header_file "${CMAKE_SOURCE_DIR}/src/include/rawrtcdc_internal.h")
set(rawrtcdc_HEADERS_INCLUDE "")
foreach (header IN LISTS rawrtcdc_HEADERS)
    string(CONCAT rawrtcdc_HEADERS_INCLUDE
            ${rawrtcdc_HEADERS_INCLUDE}
            "#include <rawrtcdc/internal/"
            ${header} ">\n")
endforeach ()
configure_file(${CMAKE_SOURCE_DIR}/src/include/rawrtcdc_internal.h.cmakein
        ${internal_header_file} @ONLY)
install(FILES ${rawrtcdc_HEADERS} DESTINATION include/rawrtcdc/internal)
install(FILES ${internal_header_file} DESTINATION include)

# Generate pkg-config file & install it
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/pkg-config.pc.cmakein
        ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.pc @ONLY)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.pc
        DESTINATION lib/pkgconfig)
