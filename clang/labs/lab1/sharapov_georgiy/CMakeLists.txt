add_llvm_library(DepWarningPlugin MODULE DepWarningPlugin.cpp PLUGIN_TOOL clang)

if(WIN32 OR CYGWIN)
  set(LLVM_LINK_COMPONENTS
    Support
  )
  clang_target_link_libraries(DepWarningPlugin PRIVATE
    clangAST
    clangBasic
    clangFrontend
    )
endif()

set(CLANG_TEST_DEPS "DepWarningPlugin" ${CLANG_TEST_DEPS} PARENT_SCOPE)
