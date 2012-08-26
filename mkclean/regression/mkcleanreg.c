/*
 * $Id$
 * Copyright (c) 2010, Matroska (non-profit organisation)
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of the Matroska assocation nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY the Matroska association ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL The Matroska Foundation BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "mkcleanreg_stdafx.h"
#include "mkcleanreg_project.h"
#include "corec/helpers/parser/parser.h"
#include "corec/helpers/md5/md5.h"
#include "corec/helpers/system/ccsystem.h"

/*!
 * \todo compare the extracted raw tracks MD5 to the original tracks
 */

static textwriter *StdErr = NULL;
static tchar_t MkPath[MAXPATHFULL];
static bool_t KeepOutput = 0;
static bool_t Quiet = 0;
static bool_t Generate = 0;

#define MD5_BLOCK_SIZE  (8*1024)

static void testFile(nodecontext *p, int LineNum,const tchar_t *File, tchar_t *MkParams, filepos_t FileSize, const tchar_t *md5sum)
{
    tchar_t Command[MAXLINE],OutFile[MAXPATHFULL];
    int result;
    err_t Err;
    streamdir FileInfo;
    stream *sFile;
    size_t ReadSize;
    md5_ctx MD5proc;
    uint8_t *DataBuffer;
    uint8_t MD5sum[16];

    if (!FileExists(p,File)) {
        TextPrintf(StdErr,T("Fail:2:%d: %s doesn't exist\r\n"),LineNum,File);
        return;
    }

    tcscpy_s(Command, TSIZEOF(Command), T("_"));
    if (tcsstr(MkParams,T("--remux")))
        tcscat_s(Command, TSIZEOF(Command), T("m"));
    if (tcsstr(MkParams,T("--optimize")))
        tcscat_s(Command, TSIZEOF(Command), T("o"));
    if (tcsstr(MkParams,T("--no-optimize")))
        tcscat_s(Command, TSIZEOF(Command), T("n"));
    if (tcsstr(MkParams,T("--unsafe")))
        tcscat_s(Command, TSIZEOF(Command), T("u"));
    if (tcsstr(MkParams,T("--live")))
        tcscat_s(Command, TSIZEOF(Command), T("l"));
    stprintf_s(OutFile,TSIZEOF(OutFile),T("%s%s.mkv"),File, Command);

    stprintf_s(Command,TSIZEOF(Command),T("%s --regression --quiet \"%s\" \"%s\""),MkParams, File, OutFile);
    result = RunCommand(p, MkPath, Command, 1);

    if (result!=0) {
        TextPrintf(StdErr,T("Fail:3:%d: mkclean returned %d for %s\r\n"),LineNum,result,File);
        return;
    }

    if (FileStat(p,OutFile,&FileInfo)!=ERR_NONE) {
        TextPrintf(StdErr,T("Fail:4:%d: failed to stat file %s\r\n"),LineNum,File);
        return;
    }

    if (!Generate && FileInfo.Size != FileSize) {
        TextPrintf(StdErr,T("Fail:5:%d: wanted %") TPRId64 T(" got %") TPRId64 T(" size in %s\r\n"),LineNum,FileSize,FileInfo.Size,File);
        return;
    }

    if (!Generate)
    {
        // test with mkvalidator
        tcscpy_s(Command,TSIZEOF(Command),T("--quiet "));
        if (tcsstr(MkParams,T("--live")))
            tcscat_s(Command,TSIZEOF(Command),T("--live "));
        if (!tcsstr(MkParams,T("--remux")))
            tcscat_s(Command,TSIZEOF(Command),T("--no-warn "));
        stcatprintf_s(Command,TSIZEOF(Command),T("\"%s\""),OutFile);
        result = RunCommand(p, T("mkvalidator"), Command, Quiet);

        if (result!=0) {
            TextPrintf(StdErr,T("Fail:6:%d: mkvalidator returned %d for %s\r\n"),LineNum,result,OutFile);
            return;
        }
    }

    // check the MD5sum
    sFile = StreamOpen(p, OutFile, SFLAG_RDONLY);
    if (sFile==NULL) {
        TextPrintf(StdErr,T("Fail:7:%d: could not open %s for MD5\r\n"),LineNum,OutFile);
        return;
    }

    DataBuffer = malloc(MD5_BLOCK_SIZE);

    MD5Init(&MD5proc);
    while ((Err=Stream_Read(sFile,DataBuffer,MD5_BLOCK_SIZE,&ReadSize))==ERR_NONE)
        MD5Update(&MD5proc, (uint8_t*)DataBuffer, ReadSize);
    if (Err==ERR_END_OF_FILE)
        MD5Update(&MD5proc, (uint8_t*)DataBuffer, ReadSize);
    MD5Final(&MD5proc, MD5sum);
    StreamClose(sFile);

    free(DataBuffer);

    stprintf_s(Command,TSIZEOF(Command),T("%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x"),MD5sum[0], 
        MD5sum[1],MD5sum[2],MD5sum[3],MD5sum[4],MD5sum[5],MD5sum[6],MD5sum[7],MD5sum[8],MD5sum[9],MD5sum[10],MD5sum[11],MD5sum[12],MD5sum[13],MD5sum[14],MD5sum[15]);
    if (Generate) {
        TextPrintf(StdErr,T("\"%s\" \"%-18s\" %11") TPRId64 T(" %s\n"),File,MkParams,FileInfo.Size,Command);
    } else if (!tcsisame_ascii(md5sum,Command)) {
        TextPrintf(StdErr,T("Fail:8:%d: bad MD5 %s for %s\r\n"),LineNum,Command,OutFile);
        return;
    }

    if (!KeepOutput)
        FileErase(p, OutFile, 1, 0); // delete correct files
    if (!Generate)
        TextPrintf(StdErr,T("Success:0:%d: %s %s\r\n"),LineNum,File,MkParams);
}

#if defined(TARGET_WIN) && defined(UNICODE)
int wmain(int argc, const wchar_t *argv[])
#else
int main(int argc, const char *argv[])
#endif
{
    parsercontext p;
    textwriter _StdErr;
    tchar_t Path[MAXPATHFULL], FileRoot[MAXPATHFULL], String[MAXDATA], Line[MAXLINE];
    int i;
    int ShowVersion = 0, ShowUsage = 0;
    int Result = 0;
    stream *RegList = NULL;
    parser RegParser;
    tchar_t MD5[34];
    int64_t Size;
    int LineNum;

    // Core-C init phase
    ParserContext_Init(&p,NULL,NULL,NULL);
	StdAfx_Init((nodemodule*)&p);
    ProjectSettings((nodecontext*)&p);

    StdErr = &_StdErr;
    memset(StdErr,0,sizeof(_StdErr));
    StdErr->Stream = (stream*)NodeSingleton(&p,STDERR_ID);

    memset(&RegParser,0,sizeof(RegParser));

#if 1
    tcscpy_s(MkPath,TSIZEOF(MkPath),T("mkclean"));
#else
    Node_FromStr(&p,Path,TSIZEOF(Path),argv[0]);
    SplitPath(Path,MkPath,TSIZEOF(MkPath),NULL,0,NULL,0);
    if (MkPath[0])
        AddPathDelimiter(MkPath,TSIZEOF(MkPath));
    tcscat_s(MkPath,TSIZEOF(MkPath),T("mkclean"));
#endif

    if (argc<2)
    {
        TextWrite(StdErr,T("No list of regression files provided!\r\n"));
        Result = -1;
        goto exit;
    }

    for (i=1;i<argc;++i)
	{
#if defined(TARGET_WIN) && defined(UNICODE)
	    Node_FromWcs(&p,Path,TSIZEOF(Path),argv[i]);
#else
	    Node_FromStr(&p,Path,TSIZEOF(Path),argv[i]);
#endif
		if (tcsisame_ascii(Path,T("--version"))) { ShowVersion = 1; }
        else if (tcsisame_ascii(Path,T("--help"))) {ShowVersion = 1; ShowUsage = 1; }
#if defined(TARGET_WIN) && defined(UNICODE)
        else if (tcsisame_ascii(Path,T("--mkclean"))) Node_FromWcs(&p,MkPath,TSIZEOF(MkPath),argv[++i]);
#else
        else if (tcsisame_ascii(Path,T("--mkclean"))) Node_FromStr(&p,MkPath,TSIZEOF(MkPath),argv[++i]);
#endif
        else if (tcsisame_ascii(Path,T("--keep"))) KeepOutput = 1;
        else if (tcsisame_ascii(Path,T("--quiet"))) {Quiet = 1; }
        else if (tcsisame_ascii(Path,T("--generate"))) {Generate = 1; }
		else if (i!=argc-1) TextPrintf(StdErr,T("Unknown parameter '%s'\r\n"),Path);
    }

    if (Result!=0 || ShowVersion)
    {
        TextWrite(StdErr,T("mkcleanreg v") PROJECT_VERSION T(", Copyright (c) 2010 Matroska Foundation\r\n"));
        if (argc < 1 || ShowUsage)
        {
            TextWrite(StdErr,T("Usage: mkcleanreg [options] <regression_list>\r\n"));
		    TextWrite(StdErr,T("Options:\r\n"));
		    TextWrite(StdErr,T("  --mkclean <path> path to mkclean to test (default is current path)\r\n"));
            TextWrite(StdErr,T("  --generate       output is usable as a regression list file\r\n"));
            TextWrite(StdErr,T("  --quiet          don't display messages from mkvalidator\r\n"));
            TextWrite(StdErr,T("  --keep           keep the output files\r\n"));
            TextWrite(StdErr,T("  --version        show the version of mkvalidator\r\n"));
            TextWrite(StdErr,T("  --help           show this screen\r\n"));
            TextWrite(StdErr,T("regression file format:\r\n"));
            TextWrite(StdErr,T("\"<file_path>\" \"<mkclean_options>\" <expected_size> <expected_md5>\r\n"));
        }
        Result = -2;
        goto exit;
    }

#if defined(TARGET_WIN) && defined(UNICODE)
    Node_FromWcs(&p,Path,TSIZEOF(Path),argv[argc-1]);
#else
    Node_FromStr(&p,Path,TSIZEOF(Path),argv[argc-1]);
#endif
    if (!FileExists((nodecontext*)&p, Path))
    {
        TextPrintf(StdErr,T("The file with the list of regression files '%s' could not be found!\r\n"),Path);
        Result = -3;
        goto exit;
    }

    RegList = StreamOpen(&p, Path, SFLAG_RDONLY);
    if (!RegList)
    {
        TextPrintf(StdErr,T("Could not read '%s'\r\n"),Path);
        Result = -4;
        goto exit;
    }

    if (ParserStream(&RegParser,RegList,&p)!=ERR_NONE)
    {
        TextPrintf(StdErr,T("Could not parse '%s'\r\n"),Path);
        Result = -5;
        goto exit;
    }

    SplitPath(Path,FileRoot,TSIZEOF(FileRoot),NULL,0,NULL,0);
    if (!FileRoot[0]) {
        FileRoot[0] = '.';
        FileRoot[1] = 0;
    }
    AddPathDelimiter(FileRoot,TSIZEOF(FileRoot));

    LineNum = 1;
    while (ParserLine(&RegParser, Line, TSIZEOF(Line)))
    {
        const tchar_t *s = Line;
        // parse the line and if it's OK, launch the process
        if (!ExprIsTokenEx(&s,T("\"%s\" \"%s\" %") TPRId64 T(" %s"),Path,TSIZEOF(Path),String,TSIZEOF(String),&Size,MD5,TSIZEOF(MD5))) {
            if (!Generate)
                TextPrintf(StdErr,T("Fail:100:%d: Invalid Line %s\r\n"),LineNum,Line);
        } else {
            // transform the path relative to the regression file to an absolute file
            tcscpy_s(Line,TSIZEOF(Line),FileRoot);
            tcscat_s(Line,TSIZEOF(Line),Path);
            testFile((nodecontext*)&p, LineNum, Line, String, Size, MD5);
        }
        ++LineNum;
    }

    // TODO: compare the extracted raw tracks MD5 to the original tracks

exit:
    ParserStream(&RegParser,NULL,NULL);
    StreamClose(RegList);

    // Core-C ending
	StdAfx_Done((nodemodule*)&p);
    ParserContext_Done(&p);

    return Result;
}
