extern "C" {
	#include "../libmatroska2/matroska/MatroskaParser.h"
	#include "../libebml2/ebml2_stdafx.h"
	#include "../libmatroska2/matroska2_stdafx.h"
	#include "../libmatroska2/matroska/matroska2.h"
}

#include <vpx/vpx_codec.h>
#include <vpx/vpx_decoder.h>
#include <vpx/vp8dx.h>

#include <cstdio>
#include <vector>

struct file_reading {
	FILE* file;
	std::vector<std::vector<char>> buffers;
};

InputStream istream;

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

int ioread(InputStream* inf, void* buffer, int count)
{
	return fread(buffer, 1, count, ((file_reading*)inf->ptr)->file);
}

int ioreadch(InputStream* inf)
{
	char ch;
	fread(&ch, 1, 1, ((file_reading*)inf->ptr)->file);
	return ch;
}

void ioseek(InputStream* inf, longlong wher, int how)
{
	_fseeki64(((file_reading*)inf->ptr)->file, wher, how);
}

filepos_t iotell(InputStream* inf)
{
	return _ftelli64(((file_reading*)inf->ptr)->file);
}

void* makeref(InputStream* inf, int count)
{
	file_reading& reading = *(file_reading*)inf->ptr;
	std::vector<char> temp(count);
	fread(&temp[0], 1, count, reading.file);
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
	return realloc(mem, count);
}

int progress(InputStream* inf, filepos_t cur, filepos_t max)
{
	return _fseeki64(((file_reading*)inf->ptr)->file, cur, SEEK_SET);
}

int read(InputStream* inf, filepos_t pos, void* buffer, size_t count)
{
	__int64 cur = _ftelli64(((file_reading*)inf->ptr)->file);
	fseek(((file_reading*)inf->ptr)->file, pos, SEEK_SET);
	uint64_t read = fread(buffer, 1, count, ((file_reading*)inf->ptr)->file);
	_fseeki64(((file_reading*)inf->ptr)->file, cur, SEEK_SET);
	return read;
}

void releaseref(InputStream* inf, void* ref)
{
	file_reading& reading = *(file_reading*)inf->ptr;
	for (uint64_t i = 0; i < reading.buffers.size(); ++i) {
		if (&reading.buffers[i][0] == ref) {
			for (uint64_t j = i; j < reading.buffers.size() - 1; ++j) {
				reading.buffers[j] = std::move(reading.buffers[j + 1]);
			}
			//need this for valid buffer list.
			reading.buffers.pop_back();
			break;
		}
	}
}

MatroskaFile* InitReader(nodecontext& ctx, file_reading& reading, const char* filename)
{
	FILE* file = fopen(filename, "rb");
	if (!file) {
		printf("Cannot open file\n");
		return 0;
	}
	reading.file = file;
	istream.ptr = &reading;
	NodeContext_Init(&ctx, NULL, NULL, NULL);
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
	char err[2048]{};
	EBML_RegisterAll((nodemodule*)&ctx);
	MATROSKA_RegisterAll((nodemodule*)&ctx);
	MATROSKA_Init(&ctx);
	NodeRegisterClassEx((nodemodule*)&ctx, HaaliStream_Class);
	NodeRegisterClassEx((nodemodule*)&ctx, WriteStream_Class);
	istream.progress(&istream, 0, 0);
	MatroskaFile* rtn =  mkv_OpenInput(&istream, err, 2048);
	istream.progress(&istream, rtn->pFirstCluster, 0);
	return rtn;
}

void DeinitReader(nodecontext& ctx, MatroskaFile* File)
{	
	file_reading* reading = (file_reading*)istream.ptr;
	fclose(reading->file);
	mkv_CloseInput(File);
	MATROSKA_Done(&ctx);
	MATROSKA_UnRegisterAll((nodemodule*)&ctx);
	EBML_UnRegisterAll((nodemodule*)&ctx);
	NodeContext_Done(&ctx);
}

int _main()
{
	nodecontext ctx;
	file_reading reading;
	MatroskaFile* mkv_file = InitReader(ctx,reading, "D:\\GamePlatforms\\Steam\\steamapps\\common\\Neptunia Rebirth1\\data\\MOVIE00000\\movie\\direct_remux.mkv");
	int num = mkv_GetNumTracks(mkv_file);
	vpx_codec_dec_cfg cfg;
	int video_track = -1;
	for (int i = 0; i < num; ++i) {
		TrackInfo* info = mkv_GetTrackInfo(mkv_file,i);
		if (info->Type == TRACK_TYPE_VIDEO) {
			video_track=i;
			cfg.w = info->AV.Video.PixelWidth;
			cfg.h = info->AV.Video.PixelHeight;
			cfg.threads = 8;
		}
	}
	vpx_codec_iface_t* iface = vpx_codec_vp9_dx();
	vpx_codec_ctx codec_ctx;
	int err = vpx_codec_dec_init(&codec_ctx, iface, &cfg, 0);
	int res = 0;
	unsigned int track = 0, frame_size = 0, frame_flags = 0;
	ulonglong start = 0, end = 0, file_pos = 0;
	uint8_t* frame_ref;
	while (!res) {
		res = mkv_ReadFrame(mkv_file, 0, &track, &start, &end, &file_pos, &frame_size, (void**)&frame_ref, &frame_flags);
		if (res) break;
		if (track == video_track) {
			vpx_codec_iter_t iter = NULL;
			vpx_image_t* img = NULL;
			err = vpx_codec_decode(&codec_ctx, frame_ref, frame_size, 0, 0);
			if(err)
				printf(vpx_codec_error(&codec_ctx));
			while ((img = vpx_codec_get_frame(&codec_ctx, &iter)) != NULL) {
				//render frame
			}
		}
		istream.releaseref(&istream, frame_ref);
	}
	vpx_codec_iter_t iter = NULL;
	vpx_image_t* img = NULL;
	err = vpx_codec_decode(&codec_ctx, 0, 0, 0, 0);
	if (err)
		printf(vpx_codec_error(&codec_ctx));
	while ((img = vpx_codec_get_frame(&codec_ctx, &iter)) != NULL) {
		//render frame
	}
	vpx_codec_destroy(&codec_ctx);
	DeinitReader(ctx,mkv_file);
	return 0;
}

int main()
{
	while(true) _main();
	return 0;
}
