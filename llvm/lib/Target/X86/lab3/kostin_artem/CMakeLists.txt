set(PluginName X86KostinPass)
file(GLOB_RECURSE ALL_SOURCE_FILES *.cpp *.h)
add_llvm_pass_plugin(${PluginName}
        ${ALL_SOURCE_FILES}
        DEPENDS
        intrinsics_gen
        X86
        BUILDTREE_ONLY)

target_include_directories(${PluginName} PUBLIC ${PATH_TO_X86})
