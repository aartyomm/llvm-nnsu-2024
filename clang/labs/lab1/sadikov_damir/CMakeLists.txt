add_llvm_library(DeprecatedPlugin_SadikovD MODULE DeprecatedPlugin_SadikovD.cpp PLUGIN_TOOL clang)

if(WIN32 OR CYGWIN)
  set(LLVM_LINK_COMPONENTS
    Support
  )
  clang_target_link_libraries(DeprecatedPlugin_SadikovD PRIVATE
    clangAST
    clangBasic
    clangFrontend
    )
endif()

set(CLANG_TEST_DEPS "DeprecatedPlugin_SadikovD" ${CLANG_TEST_DEPS} PARENT_SCOPE)