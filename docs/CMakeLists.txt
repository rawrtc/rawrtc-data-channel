# Generate Doxyfile
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.cmakein
        ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile @ONLY)

# Generate conf.py for Sphinx
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/conf.py.cmakein
        ${CMAKE_CURRENT_SOURCE_DIR}/conf.py @ONLY)

# Add custom target to generate the docs
add_custom_target(docs
        "sphinx-build ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/_build"
        COMMENT "Generate documentary for ${PROJECT_NAME}")
