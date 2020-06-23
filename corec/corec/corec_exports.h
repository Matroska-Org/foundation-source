#pragma once

#include "portab.h"
#include "corec/corec_exports.h"

#ifndef EBML_DLL
#if defined(EBML2_EXPORTS)
#define EBML_DLL DLLEXPORT
#elif defined(EBML2_IMPORTS)
#define EBML_DLL DLLIMPORT
#else
#define EBML_DLL
#endif
#endif
/*
#ifndef NODEMETA_DEFINED
#define NODEMETA_DEFINED
typedef struct nodemeta {
    uint32_t Meta : 8;
    uint32_t Id : 24;
    uintptr_t Data;

} nodemeta;
#endif 
*/
EBML_DLL extern const nodemeta BufStream_Class[];
EBML_DLL extern const nodemeta MemStream_Class[];
EBML_DLL extern const nodemeta Streams_Class[];
EBML_DLL extern const nodemeta File_Class[];
//EBML_DLL extern const nodemeta FileDb_Class[];
//EBML_DLL extern const nodemeta VFS_Class[];
EBML_DLL extern const nodemeta Stdio_Class[];
EBML_DLL extern const nodemeta LangStr_Class[];
EBML_DLL extern const nodemeta UrlPart_Class[];
