set(TRANSLATIONS_FILES
    pianobooster_ca.ts
    pianobooster_cs.ts
    pianobooster_da.ts
    pianobooster_de.ts
    pianobooster_es.ts
    pianobooster_fr.ts
    pianobooster_gl.ts
    pianobooster_is.ts
    pianobooster_it.ts
    pianobooster_ja.ts
    pianobooster_nb.ts
    pianobooster_nl.ts
    pianobooster_pl.ts
    pianobooster_ru.ts
    pianobooster_sk.ts
    pianobooster_sv.ts
    pianobooster_uk.ts
    pianobooster_zh.ts)

if (INSTALL_ALL_LANGS)
    file(GLOB TRANSLATIONS_FILES "*.ts")
endif ()

get_filename_component(full_path_blank_ts ${CMAKE_CURRENT_SOURCE_DIR}/pianobooster_blank.ts ABSOLUTE)
list(REMOVE_ITEM TRANSLATIONS_FILES ${full_path_blank_ts})

find_package("${QT_PACKAGE_PREFIX}LinguistTools" REQUIRED COMPONENTS LinguistTools)

if (COMMAND QT_ADD_TRANSLATION)
    qt_add_translation(QM_FILES ${TRANSLATIONS_FILES})
else ()
    qt5_add_translation(QM_FILES ${TRANSLATIONS_FILES})
endif ()
add_custom_target(translations ALL DEPENDS ${QM_FILES})

if (UNIX AND NOT APPLE)
    install(FILES ${QM_FILES} DESTINATION ${DATA_DIR}/translations)
    install(FILES langs.json DESTINATION ${DATA_DIR}/translations)
endif (UNIX AND NOT APPLE)

if (WIN32)
    install(FILES ${QM_FILES} DESTINATION .)
    install(FILES langs.json DESTINATION .)
endif (WIN32)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/langs.json ${CMAKE_CURRENT_BINARY_DIR}/ COPYONLY)
