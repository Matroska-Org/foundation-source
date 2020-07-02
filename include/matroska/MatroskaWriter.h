#ifdef _MSC_VER
#pragma once
#endif
#ifndef MKV_WRITER_H
#define MKV_WRITER_H

#include "MatroskaInfo.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct OutputStream {
	int (*write)(struct OutputStream* cc, filepos_t pos, void* buffer, size_t count);
	filepos_t(*scan)(struct OutputStream* cc, filepos_t start, unsigned signature);
	size_t(*getcachesize)(struct OutputStream* cc);
	filepos_t(*getfilesize)(struct OutputStream* cc);
	const char* (*geterror)(struct OutputStream* cc);
	int (*progress)(struct OutputStream* cc, filepos_t cur, filepos_t max);

	bool_t  (*iowritech)(struct OutputStream* inf, int ch);
	int (*iowrite)(struct OutputStream* inf, void* buffer, int count);
	void (*ioseek)(struct OutputStream* inf, longlong where, int how);
	filepos_t(*iotell)(struct OutputStream* inf);

	void* (*memalloc)(struct OutputStream* cc, size_t size);
	void* (*memrealloc)(struct OutputStream* cc, void* mem, size_t newsize);
	void (*memfree)(struct OutputStream* cc, void* mem);
//#if defined(NO_MATROSKA2_GLOBAL)
	anynode* AnyNode;
	void* ptr;
//#endif
} OutputStream;

#define MATROSKA_OUTPUT_DEFAULT     0
//write cue first and do not add seek in the head
#define MATROSKA_OUTPUT_NOSEEK 1
MATROSKA_DLL MatroskaFile* mkv_OpenOutput(OutputStream* io, int mode, char* err_msg, size_t err_msgSize);

MATROSKA_DLL int mkv_AddTrack(MatroskaFile* File, TrackInfo* info);

MATROSKA_DLL int mkv_WriteHeader(MatroskaFile* File, char* err_msg, size_t err_msgSize);
MATROSKA_DLL int mkv_TempHeaders(MatroskaFile* File);

MATROSKA_DLL int mkv_WriteFrame(MatroskaFile* File, int track, ulonglong StartTime, ulonglong EndTime, unsigned int size, void* data, bool_t key_frame, bool_t can_discard);

MATROSKA_DLL void mkv_WriteTail(MatroskaFile* File);

MATROSKA_DLL void mkv_CloseOutput(MatroskaFile* File);

#ifdef __cplusplus
}
#endif

#endif
