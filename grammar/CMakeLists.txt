function(antlr_grammar GRAMMAR_DIR GRAMMAR_NAME OUT_DIR)
    set(${GRAMMAR_NAME}_GRAMMAR_SOURCES
            ${OUT_DIR}/${GRAMMAR_NAME}BaseVisitor.cpp
            ${OUT_DIR}/${GRAMMAR_NAME}BaseVisitor.h
            ${OUT_DIR}/${GRAMMAR_NAME}Lexer.cpp
            ${OUT_DIR}/${GRAMMAR_NAME}Lexer.h
            ${OUT_DIR}/${GRAMMAR_NAME}Parser.cpp
            ${OUT_DIR}/${GRAMMAR_NAME}Parser.h
            ${OUT_DIR}/${GRAMMAR_NAME}Visitor.cpp
            ${OUT_DIR}/${GRAMMAR_NAME}Visitor.h
            CACHE INTERNAL "" FORCE)

    if (WIN32)
        message(FATAL_ERROR "Windows grammar generation is not done yet")
        add_custom_command(
                OUTPUT ${${GRAMMAR_NAME}_GRAMMAR_SOURCES}
                COMMAND bash ${CMAKE_SOURCE_DIR}/grammar/build.bat ${OUT_DIR} ${GRAMMAR_DIR}/${GRAMMAR_NAME}.g4
                DEPENDS ${GRAMMAR_DIR}/${GRAMMAR_NAME}.g4
                WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/grammar
                COMMENT "Generating grammar")
    endif (WIN32)

    if (UNIX)
        add_custom_command(
                OUTPUT ${${GRAMMAR_NAME}_GRAMMAR_SOURCES}
                COMMAND bash ${CMAKE_SOURCE_DIR}/grammar/build.sh ${OUT_DIR} ${GRAMMAR_DIR}/${GRAMMAR_NAME}.g4
                DEPENDS ${GRAMMAR_DIR}/${GRAMMAR_NAME}.g4
                WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/grammar
                COMMENT "Generating grammar")
    endif (UNIX)
endfunction()

