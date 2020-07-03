#include <cstdint>
#include <cstdlib>
#include <cstdio>
#include <iostream>
#include <vector>
#include <string>
typedef int_fast64_t filepos_t;
extern "C" {
	#define _USE_DLL
	#define MATROSKA2_IMPORTS
	#include <matroska/matroska2.h>
	#include <matroska/MatroskaParser.h>
	#include <matroska/MatroskaWriter.h>
	#include <ebml/corec/parser.h>
	#include <ebml/corec/node.h>
	#include <ebml/corec/streams.h>

	#ifdef inline
	#undef inline
	#endif
}

struct file_reading {
	FILE* file;
	std::vector<std::vector<char>> buffers;
};

InputStream istream;
OutputStream ostream;

const char* geterror(InputStream* cc)
{
	return "dummy error";
}

filepos_t getfilesize(InputStream* cc)
{
	__int64 cur = _ftelli64(((file_reading*)cc->ptr)->file);
	fseek(((file_reading*)cc->ptr)->file, 0L, SEEK_END);
	__int64 size = _ftelli64(((file_reading*)cc->ptr)->file);
	_fseeki64(((file_reading*)cc->ptr)->file, cur, SEEK_SET);
	return size;
}

const char* geterror(OutputStream* cc)
{
	return "dummy error";
}

filepos_t getfilesize(OutputStream* cc)
{
	__int64 cur = _ftelli64(((file_reading*)cc->ptr)->file);
	fseek(((file_reading*)cc->ptr)->file, 0L, SEEK_END);
	__int64 size = _ftelli64(((file_reading*)cc->ptr)->file);
	_fseeki64(((file_reading*)cc->ptr)->file, cur, SEEK_SET);
	return size;
}

int ioread(InputStream* inf, void* buffer, int count)
{
	return fread(buffer,1,count, ((file_reading*)inf->ptr)->file);
}

int ioreadch(InputStream* inf)
{
	char ch;
	fread(&ch,1,1, ((file_reading*)inf->ptr)->file);
	return ch;
}

int iowrite(OutputStream* inf, void* buffer, int count)
{
	return fwrite(buffer, 1, count, ((file_reading*)inf->ptr)->file);
}

bool_t iowritech(OutputStream* inf, int ch)
{
	char temp = ch;
	return fwrite(&temp, 1, 1, ((file_reading*)inf->ptr)->file);
}

void ioseek(InputStream* inf, longlong wher, int how)
{
	_fseeki64(((file_reading*)inf->ptr)->file,wher,how);
}

filepos_t iotell(InputStream* inf)
{
	return _ftelli64(((file_reading*)inf->ptr)->file);
}

void ioseek(OutputStream* inf, longlong wher, int how)
{
	_fseeki64(((file_reading*)inf->ptr)->file, wher, how);
}

filepos_t iotell(OutputStream* inf)
{
	return _ftelli64(((file_reading*)inf->ptr)->file);
}

void*  makeref(InputStream* inf, int count)
{
	file_reading& reading = *(file_reading*)inf->ptr;
	std::vector<char> temp(count);
	fread(&temp[0],1,count,reading.file);
	reading.buffers.emplace_back(std::move(temp));
	return &reading.buffers.back()[0];
}

void* memalloc(InputStream* inf, size_t count)
{
	return malloc(count);
}

void memfree(InputStream* inf, void* mem)
{
	free(mem);
}

void* memrealloc(InputStream* inf, void* mem, size_t count)
{
	return realloc(mem,count);
}

void* memalloc(OutputStream* inf, size_t count)
{
	return malloc(count);
}

void memfree(OutputStream* inf, void* mem)
{
	free(mem);
}

void* memrealloc(OutputStream* inf, void* mem, size_t count)
{
	return realloc(mem, count);
}

int progress(InputStream* inf, filepos_t cur, filepos_t max)
{
	return _fseeki64(((file_reading*)inf->ptr)->file, cur, SEEK_SET);
}

int progress(OutputStream* inf, filepos_t cur, filepos_t max)
{
	return 0;
}

int read(InputStream* inf, filepos_t pos, void* buffer, size_t count)
{
	__int64 cur = _ftelli64(((file_reading*)inf->ptr)->file);
	fseek(((file_reading*)inf->ptr)->file, pos, SEEK_SET);
	uint64_t read = fread(buffer,1,count, ((file_reading*)inf->ptr)->file);
	_fseeki64(((file_reading*)inf->ptr)->file, cur, SEEK_SET);
	return read;
}

int write(OutputStream* inf, filepos_t pos, void* buffer, size_t count)
{
	__int64 cur = _ftelli64(((file_reading*)inf->ptr)->file);
	fseek(((file_reading*)inf->ptr)->file, pos, SEEK_SET);
	uint64_t write = fwrite(buffer, 1, count, ((file_reading*)inf->ptr)->file);
	_fseeki64(((file_reading*)inf->ptr)->file, cur, SEEK_SET);
	return write;
}

void releaseref(InputStream* inf, void* ref)
{
	file_reading& reading = *(file_reading*)inf->ptr;
	for (uint64_t i = 0; i < reading.buffers.size(); ++i) {
		if (&reading.buffers[i][0] == ref) {
			for (uint64_t j = i; j < reading.buffers.size() - 1; ++j) {
				reading.buffers[j]=std::move(reading.buffers[j+1]);
			}
			//need this for valid buffer list.
			reading.buffers.pop_back();
			break;
		}
	}
}

void free_module(nodecontext* ctx, nodemodule* mod){}



int main()
{
	while (true) {
		FILE* file = fopen("D:\\GamePlatforms\\Steam\\steamapps\\common\\Neptunia Rebirth1\\data\\MOVIE00000\\movie\\direct.webm", "rb");
		if (!file) {
			printf("Cannot open file\n");
			exit(1);
		}
		FILE* writefile = fopen("D:\\GamePlatforms\\Steam\\steamapps\\common\\Neptunia Rebirth1\\data\\MOVIE00000\\movie\\direct_remux.mkv", "wb");
		if (!file) {
			printf("Cannot open file\n");
			exit(1);
		}
		file_reading reading, writing;
		reading.file = file;
		writing.file=writefile;
		istream.ptr = &reading;
		ostream.ptr = &writing;
		nodecontext ctx;
		NodeContext_Init(&ctx, NULL, NULL, NULL);
		ctx.FreeModule = free_module;
		istream.AnyNode = &ctx;
		istream.geterror = geterror;
		istream.getfilesize = getfilesize;
		istream.ioread = ioread;
		istream.ioreadch = ioreadch;
		istream.ioseek = ioseek;
		istream.iotell = iotell;
		istream.makeref = makeref;
		istream.memalloc = memalloc;
		istream.memfree = memfree;
		istream.memrealloc = memrealloc;
		istream.progress = progress;
		istream.read = read;
		istream.releaseref = releaseref;
		istream.scan = nullptr;
		ostream.AnyNode = &ctx;
		ostream.geterror = geterror;
		ostream.getfilesize = getfilesize;
		ostream.iowrite = iowrite;
		ostream.iowritech = iowritech;
		ostream.ioseek = ioseek;
		ostream.iotell = iotell;
		ostream.memalloc = memalloc;
		ostream.memfree = memfree;
		ostream.memrealloc = memrealloc;
		ostream.progress = progress;
		ostream.write = write;
		char err[2048]{};
//		NodeContext_Init(&ctx, NULL, NULL, NULL);
		EBML_RegisterAll((nodemodule*)&ctx);
		MATROSKA_RegisterAll((nodemodule*)&ctx);
		MATROSKA_Init(&ctx);
		NodeRegisterClassEx((nodemodule*)&ctx, HaaliStream_Class);
		NodeRegisterClassEx((nodemodule*)&ctx, WriteStream_Class);
		istream.progress(&istream, 0, 0);
		ostream.progress(&ostream, 0, 0);
		MatroskaFile* mkv_file = mkv_OpenInput(&istream, err, 2048);
		MatroskaFile* mkv_ofile = mkv_OpenOutput(&ostream, MATROSKA_OUTPUT_NOSEEK, err, 2048);
		{
			mkv_ofile->Seg.DateUTC = mkv_file->Seg.DateUTC;
			memcpy(mkv_ofile->Seg.UID,mkv_file->Seg.UID,16);
			for (size_t i = 0; i < mkv_GetNumTracks(mkv_file); ++i) {
				mkv_AddTrack(mkv_ofile, mkv_GetTrackInfo(mkv_file, i));
			}
		}
		unsigned int track = 0, frame_size = 0, frame_flags = 0;
		ulonglong start = 0, end = 0, file_pos = 0;
		void* frame_ref;
		istream.progress(&istream, mkv_file->pFirstCluster, 0);

		mkv_WriteHeader(mkv_ofile,err,2048);
		mkv_TempHeaders(mkv_ofile);
		int res = 0;
		while (!res) {
			res = mkv_ReadFrame(mkv_file, 0, &track, &start, &end, &file_pos, &frame_size, &frame_ref, &frame_flags);
	//		printf("Track: %d, Size: %d, Read Type: %d, Write Type: %d\n",track, frame_size, mkv_GetTrackInfo(mkv_file,track)->Type, mkv_GetTrackInfo(mkv_ofile, track)->Type);
			if(res) break;
			mkv_WriteFrame(mkv_ofile,track,start,end,frame_size,frame_ref,frame_flags & FRAME_KF,0);
			istream.releaseref(&istream, frame_ref);
		}
		mkv_WriteTail(mkv_ofile);
		std::cout << file;
		mkv_CloseInput(mkv_file);
		mkv_CloseOutput(mkv_ofile);
		fclose(file);
		fflush(writefile);
		fclose(writefile);
		MATROSKA_Done(&ctx);
		MATROSKA_UnRegisterAll((nodemodule*)&ctx);
		EBML_UnRegisterAll((nodemodule*)&ctx);
		NodeContext_Done(&ctx);
	}
}



