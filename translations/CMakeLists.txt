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

# set TS_FILES and EXTERNAL_QM_FILESj to let qtutilities handle translation processing
foreach (TS_FILE ${TRANSLATIONS_FILES})
    list(APPEND TS_FILES "../translations/${TS_FILE}")
endforeach ()
set(TS_FILES "${TS_FILES}" PARENT_SCOPE)
set(EXTERNAL_QM_FILES "${CMAKE_CURRENT_BINARY_DIR}/../langs.json" PARENT_SCOPE)

# allow to built-in translations (via qtutilities); otherwise install langs.json normally
option(BUILTIN_TRANSLATIONS "enables/disables built-in translations" OFF)
if (NOT BUILTIN_TRANSLATIONS)
    if ((UNIX OR MINGW) AND NOT APPLE)
        install(FILES langs.json DESTINATION ${DATA_DIR}/translations)
    elseif (WIN32)
        install(FILES langs.json DESTINATION .)
    endif ()
endif ()

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/langs.json ${CMAKE_CURRENT_BINARY_DIR}/.. COPYONLY)
