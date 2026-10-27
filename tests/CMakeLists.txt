find_package(Qt6 6.2 REQUIRED COMPONENTS Test)

# Crypto unit tests: pure lmcCrypto round-trips, no network, no GUI.
# Fixture dir is baked in at configure time (out-of-source safe).
add_executable(tst_crypto
    unit/tst_crypto.cpp
)

target_compile_definitions(tst_crypto PRIVATE
    LMC_CRYPTO_FIXTURES_DIR="${CMAKE_CURRENT_SOURCE_DIR}/data/crypto"
)

target_link_libraries(tst_crypto PRIVATE lmc_core Qt6::Test)

set_target_properties(tst_crypto PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/tests
    RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/tests
    RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/tests
    RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO ${CMAKE_BINARY_DIR}/tests
    RUNTIME_OUTPUT_DIRECTORY_MINSIZEREL ${CMAKE_BINARY_DIR}/tests
)

add_test(NAME crypto COMMAND tst_crypto)
# tst links Qt Widgets (via lmc_core) so QTest instantiates QApplication:
# no X server needed for headless crypto checks.
set_tests_properties(crypto PROPERTIES ENVIRONMENT "QT_QPA_PLATFORM=offscreen")

set(LMC_REGRESSION_TESTS clipboardimage chatimage filetransfer filereceive translations)
if(LMC_LITE_BUILD)
    list(APPEND LMC_REGRESSION_TESTS contactgroups)
endif()
foreach(test_name IN LISTS LMC_REGRESSION_TESTS)
    add_executable(tst_${test_name} unit/tst_${test_name}.cpp)
    target_link_libraries(tst_${test_name} PRIVATE lmc_core Qt6::Test)
    set_target_properties(tst_${test_name} PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/tests
        RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/tests
        RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/tests
        RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO ${CMAKE_BINARY_DIR}/tests
        RUNTIME_OUTPUT_DIRECTORY_MINSIZEREL ${CMAKE_BINARY_DIR}/tests)
    add_test(NAME ${test_name} COMMAND tst_${test_name})
    set_tests_properties(${test_name} PROPERTIES
        ENVIRONMENT "QT_QPA_PLATFORM=offscreen"
        TIMEOUT 90)
endforeach()

target_sources(tst_chatimage PRIVATE ${CMAKE_SOURCE_DIR}/lmc/src/resource.qrc)

target_sources(tst_translations PRIVATE ${CMAKE_SOURCE_DIR}/lmc/src/resource.qrc)
target_compile_definitions(tst_translations PRIVATE
    LMC_TRANSLATIONS_DIR="${CMAKE_BINARY_DIR}/lmc/runtime/lang"
    LMC_CHINESE_TS="${CMAKE_SOURCE_DIR}/translations/lmc_zh_CN.ts")
add_dependencies(tst_translations lmc_runtime_files)
if(APPLE)
    target_compile_definitions(tst_translations PRIVATE
        LMC_MACOS_INFO_PLIST="$<TARGET_BUNDLE_DIR:lmc>/Contents/Info.plist")
    add_dependencies(tst_translations lmc)
endif()
if(MSVC)
    target_compile_options(tst_translations PRIVATE /utf-8)
    target_compile_options(tst_chatimage PRIVATE /utf-8)
endif()
