add_llvm_library(ClassListPlugin MODULE classList.cpp PLUGIN_TOOL clang)

if(WIN32 OR CYGWIN)
  set(LLVM_LINK_COMPONENTS
    Support
  )
  clang_target_link_libraries(ClassListPlugin PRIVATE
    clangAST
    clangBasic
    clangFrontend
    )
endif()

set(CLANG_TEST_DEPS "ClassListPlugin" ${CLANG_TEST_DEPS} PARENT_SCOPE)