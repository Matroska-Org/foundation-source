# zlib library API

option(CONFIG_ZLIB "Enable zlib (de)compression" ON)

if (CONFIG_ZLIB)
    add_library("zlib" STATIC)
    set(zlib_group_PUBLIC_HEADERS
        crc32.h
        inffast.h
        inffixed.h
        inflate.h
        inftrees.h
        zconf.h
        zlib.h
        zutil.h
        deflate.h
        trees.h
    )
    set(zlib_group_SOURCES
        adler32.c
        crc32.c
        inffast.c
        inflate.c
        inftrees.c
        uncompr.c
        zutil.c
        compress.c
        deflate.c
        trees.c
    )
    set(zlib_group_msvc_x64_SOURCES
        contrib/masmx64/gvmat64.asm
        contrib/masmx64/inffas8664.c
        contrib/masmx64/inffasx64.asm
    )
    set(zlib_group_msvc_x86_SOURCES
        contrib/masmx64/masmx86/inffas32.asm
        contrib/masmx64/masmx86/match686.asm
    )
    set(zlib_group_gcc_x64_SOURCES
        contrib/masmx64/masmx86/inffas32.asm
        contrib/masmx64/masmx86/match686.asm
    )
    set(zlib_group_gcc_x86_SOURCES
        contrib/masmx64/masmx86/inffas32.asm
        contrib/masmx64/masmx86/match686.asm
    )

    if(0)
    if(MSVC)
        if (${CMAKE_HOST_SYSTEM_PROCESSOR} MATCHES AMD64)
            target_compile_definitions("zlib" PRIVATE ASMINF)
            set(ZLIB_ASM_SOURCES ${zlib_group_msvc_x64_SOURCES})
        elseif (${CMAKE_HOST_SYSTEM_PROCESSOR} MATCHES X86)
            target_compile_definitions("zlib" PRIVATE ASMINF)
            set(ZLIB_ASM_SOURCES ${zlib_group_msvc_x86_SOURCES})
        endif()
      endif(MSVC)

    if(GCC AND NOT APPLE)
        if (${CMAKE_HOST_SYSTEM_PROCESSOR} MATCHES x86_64)
            target_compile_definitions("zlib" PRIVATE ASMV)
            set(ZLIB_ASM_SOURCES ${zlib_group_gcc_x86_SOURCES})
        elseif (${CMAKE_HOST_SYSTEM_PROCESSOR} MATCHES i686)
            target_compile_definitions("zlib" PRIVATE ASMV)
            target_compile_definitions("zlib" PRIVATE ASMINF)
            set(ZLIB_ASM_SOURCES ${zlib_group_gcc_x86_SOURCES})
        endif()
    endif()
    endif()
    
    target_compile_definitions("zlib" PRIVATE FASTEST NO_ERRNO_H)
    # if(WIN32 && DYNAMIC BUILD)
    #     target_compile_definitions("zlib" ZLIB_DLL)
    # endif(WIN32)
    target_sources("zlib" PRIVATE ${zlib_group_SOURCES} ${ZLIB_ASM_SOURCES} ${zlib_group_PUBLIC_HEADERS})
    target_include_directories("zlib" PUBLIC ".")
    set_target_properties("zlib" PROPERTIES 
        PUBLIC_HEADER "${zlib_group_PUBLIC_HEADERS}"
    )
endif(CONFIG_ZLIB)
