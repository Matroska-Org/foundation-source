// Copyright © 2010-2024 Matroska (non-profit organisation).
// SPDX-License-Identifier: BSD-3-Clause

#include <ebml/EbmlMaster.h>
#include <ebml/EbmlString.h>
#include <ebml/EbmlSInteger.h>
#include <ebml/EbmlUInteger.h>
#include <ebml/EbmlFloat.h>
#include <ebml/EbmlDummy.h>
#include <ebml/EbmlVoid.h>
#include <ebml/EbmlHead.h>
#include <ebml/EbmlStream.h>
#include <ebml/StdIOCallback.h>

#include <matroska/KaxCluster.h>
#include <matroska/KaxSemantic.h>
#include <matroska/KaxTracks.h>
#include <matroska/KaxSeekHead.h>
#include <matroska/KaxCuesData.h>
#include <matroska/KaxContexts.h>
#include <matroska/KaxSegment.h>
#include <matroska/KaxBlockData.h>
#include "mkvalidator_project.h"

#include <cstdio>
#include <cassert>
#include <cstdint>
#include <cinttypes>
#include <vector>
#include <limits>
#include <string.h>
#include <algorithm>
#include <memory>

using namespace libmatroska;
using namespace libebml;

/*!
 * \todo verify the track timestamp scale is not null
 * \todo verify that the size of frames inside a lace is legit (ie the remaining size for the last must be > 0)
 * \todo verify that items with a limited set of values don't use other values
 * \todo verify that timestamps for each track are increasing (for keyframes and p frames)
 * \todo optionally show the use of deprecated elements
 * \todo support concatenated segments
 */

// TODO move in libebml
#define EBML_MAX_VERSION    1
#define EBML_MAX_ID         4
#define EBML_MAX_SIZE       8


// FIXME remove ?
using tchar_t = char;
#define T(x)  x
#define TSIZEOF(x) sizeof(x)
#define TPRId64 PRId64
#define TPRIx64 PRIx64
#define tcslen(s)  strlen(s)
#define tcscpy_s(d,dn,s) strcpy(d,s)
#define tcscat_s(d,dn,s) strcat(d,s)
#define tcscmp(a,b)  strcmp(a,b)

#define Node_FromStr(c,d,dn,s)  strcpy(d,s)

#define MAXPATH 1024
#define MAXPATHFULL 1024
#define MAXLINE 2048
#define MAXDATA 2048

using bool_t = bool;

using array = std::vector<EbmlElement>;
#define ARRAYBEGIN(v,t)  (v).begin()
#define ARRAYEND(v,t)    (v).end()
#define ARRAYCOUNT(v,t)  (v).size()
#define ArrayInit(v)     (v)->clear()
#define ArrayClear(v)    (v)->clear()
#define ArrayResize(v,s,t,a) (v)->resize(s)
#define ArrayZero(v) // nothing, it's always used after resize
#define ArrayAppend(v,e,s,a)  (v)->push_back(e)

#define NodeDelete(o)    delete o

using ebml_element = EbmlElement;
using ebml_master = EbmlMaster;
using ebml_integer = EbmlSInteger;
using matroska_cluster = KaxCluster;
using matroska_block = KaxInternalBlock;
using matroska_seekpoint = KaxSeek;
using matroska_cuepoint = KaxCuePoint;

using mkv_timestamp_t = std::uint64_t;
constexpr const mkv_timestamp_t INVALID_TIMESTAMP_T = std::numeric_limits<mkv_timestamp_t>::max();

#define EBML_MasterChildren(m)  (m)->begin()
#define EBML_MasterEnd(i,m)     (i) != (m)->end()
#define EBML_MasterNext(i)      (i)++

#define EBML_ElementGetName(e,d,dn)   strcpy(d,EBML_NAME(e))
#define EBML_ElementClassID(e)        (*e)->GetClassId()
#define EBML_ElementPosition(e)       (e)->GetElementPosition()
#define EBML_ElementDataSize(e,u)     (e)->GetSize()
#define EBML_ElementFullSize(e,u)     (e)->ElementSize((u != 0) ? EbmlElement::WriteAll : EbmlElement::WriteSkipDefault)
#define EBML_ElementPositionEnd(e)    (e)->GetEndPosition()
#define EBML_ElementPositionData(e)   ((e)->GetEndPosition() - (e)->GetSize())
#define EBML_ElementIsType(e,t)       ((e)->GetClassId() == EBML_ID(t))
#define EBML_ElementReadData(e,i,b,c,s,f) (e)->ReadData(i,s)
#define EBML_ElementSkipData(e,s,c,p,d)   (e)->SkipData(s,c,p,d)
#define EBML_FindNextElement(stream, sem, level, dummy) (stream)->FindNextElement(sem, *level, UINT64_MAX, dummy)

#define ERR_NONE  (!INVALID_FILEPOS_T)

#define EBML_MasterFindChild(m,c)     FindChild<c>(*m)
#define EBML_MasterGetChild(m,c,u)    &GetChild<c>(*m)
#define EBML_MasterNextChild(m,p)     FindNextChild(*m, *p)
#define EBML_MasterIsChecksumValid(m) (m)->VerifyChecksum()
#define NodeTree_SetParent(e,p,u)     (p)->PushElement(*(e))

#define EBML_IntegerValue(e)          (reinterpret_cast<const EbmlUInteger*>(e))->GetValue()

#define EBML_StringGet(e,d,dn)         strcpy(d,(e)->GetValue().c_str())

#define EBML_FloatValue(e)             (reinterpret_cast<EbmlFloat*>(e))->GetValue()

#define MATROSKA_MetaSeekIsClass(s,c)  (s)->IsEbmlId(EBML_ID(c))

#define MATROSKA_BlockTrackNum(b)      (b)->TrackNum()
#define MATROSKA_BlockGetFrameCount(b) (b)->NumberFrames()
#define MATROSKA_BlockGetLength(b,i)   (b)->GetFrameSize(i)

#define MATROSKA_CueTrackNum(c)        (c)->GetSeekPosition()->TrackNumber()


#define PROFILE_MATROSKA_V1  1
#define PROFILE_MATROSKA_V2  2
#define PROFILE_MATROSKA_V3  3
#define PROFILE_MATROSKA_V4  4
#define PROFILE_MATROSKA_V5  5
#define PROFILE_DIVX 0
#define PROFILE_WEBM 10


size_t EBML_IdToString(tchar_t *Out, size_t OutLen, const EbmlId & Id)
{
    size_t i,FinalHeadSize = Id.GetLength();
	if (OutLen < FinalHeadSize*4+1)
		return 0;
	Out[0] = 0;
    for (i=0;i<4;++i)
	{
		if (Out[0] || (Id.GetValue() >> 8*(3-i)) & 0xFF)
		{
			tchar_t tmp[5];
			snprintf(tmp,TSIZEOF(tmp), T("[%02X]"),(Id.GetValue() >> 8*(3-i)) & 0xFF);
			strcat(Out,tmp);
		}
	}
	return FinalHeadSize*4;
}

static inline int64_t Scale64(int64_t v,int64_t Num,int64_t Den)
{
	if (Den)
		return (v * Num) / Den;
	return 0;
}

tchar_t* tcsupr(tchar_t* p)
{
	tchar_t* i=p;
	for (;*i;++i)
		*i = (tchar_t)toupper(*i);
	return p;
}

#define isame_ascii(a,b,i) \
    ca=a[i]; \
    cb=b[i]; \
    cb ^= ca; \
    if (cb) /* not equal? */\
    { \
        cb &= ~0x20; /* only allow the 0x20 bit to be different */ \
        ca &= ~0x20; /* and only in the 'A'..'Z' range */ \
        if (cb || ca<'A' || ca>'Z') \
            return 0; \
    } \
    if (!ca) /* both zero? */ \
        break;

bool_t tcsisame_ascii(const tchar_t* a,const tchar_t* b)
{
    int ca,cb;
    for (;;a+=4,b+=4)
    {
        isame_ascii(a,b,0)
        isame_ascii(a,b,1)
        isame_ascii(a,b,2)
        isame_ascii(a,b,3)
    }
    return 1;
}

#define MASTER_CHECK_PROFILE_INVALID    0
#define MASTER_CHECK_MISSING_MANDATORY  1
#define MASTER_CHECK_MULTIPLE_UNIQUE    2

static bool AllowedInProfile(int ProfileMask, const MatroskaProfile &profile)
{
    return true;
}

void EBML_MasterCheckContext(EbmlMaster *Element, int ProfileMask, bool_t (*ErrCallback)(void *cookie, int type, const tchar_t *ClassName, const EbmlElement*), void *cookie)
{
	tchar_t ClassString[MAXPATH];
	EBML_MASTER_ITERATOR i;
    EbmlElement *SubElt;
    const EbmlSemantic *s;
#if 1 // TODO
	for (i=EBML_MasterChildren(Element);EBML_MasterEnd(i,Element);EBML_MasterNext(i))
	{
		if ((*i)->GetClassId() == EBML_ID(EbmlDummy))
        {
            for (size_t si=0; si < EBML_CTX_SIZE(EBML_CONTEXT(Element)); si++)
		    //for (s=Element->Base.Context->Semantic; s->eClass; ++s)
		    {
                s = &EBML_CTX_IDX(EBML_CONTEXT(Element), si);
			    //if (s->eClass->Id == i->Context->Id)
			    if (EBML_INFO_ID(s->GetCallbacks()) == (*i)->GetClassId())
			    {
#if 1
                    const auto & cb = s->GetCallbacks();
                    auto profiles = reinterpret_cast<const MatroskaProfile &>(cb.GetVersions());
                    // if (s->DisabledProfile & ProfileMask)
                    if (!AllowedInProfile(ProfileMask, profiles))
                    {
				        Node_FromStr(Element,ClassString,TSIZEOF(ClassString),cb.GetName());
                        if (ErrCallback && ErrCallback(cookie,MASTER_CHECK_PROFILE_INVALID,ClassString,*i))
                        {
                            // TODO EBML_MasterRemove(Element,*i); // make sure it doesn't remain in the list
					        // TODO NodeDelete(i);
					        i=EBML_MasterChildren(Element);
                            break;
                        }
                    }
                    if (s->IsUnique() && (SubElt=Element->FindFirstElt(cb, false)) && (SubElt=EBML_MasterNextChild(Element,SubElt)))
                    // if (s->IsUnique() && (SubElt=EBML_MasterFindChild(Element,s->eClass)) && (SubElt=EBML_MasterNextChild(Element,SubElt)))
                    {
		                Node_FromStr(Element,ClassString,TSIZEOF(ClassString),cb.GetName());
                        if (ErrCallback && ErrCallback(cookie,MASTER_CHECK_MULTIPLE_UNIQUE,ClassString,SubElt))
                        {
                            // TODO EBML_MasterRemove(Element,*i); // make sure it doesn't remain in the list
			                // TODO NodeDelete(i);
			                i=EBML_MasterChildren(Element);
                            break;
                        }
                    }
				    break;
#endif
			    }
		    }
        }
	}

    for (size_t si=0; si < EBML_CTX_SIZE(EBML_CONTEXT(Element)); si++)
	//for (s=Element->Base.Context->Semantic; s->eClass; ++s)
	{
        s = &EBML_CTX_IDX(EBML_CONTEXT(Element), si);
        const auto & cb = s->GetCallbacks();
	    if (s->IsMandatory() && !cb.HasDefault() && !Element->FindFirstElt(cb, false))
	    {
		    // Node_FromStr(Element,ClassString,TSIZEOF(ClassString),s->eClass->ElementName);
		    Node_FromStr(Element,ClassString,TSIZEOF(ClassString),cb.GetName());
            if (ErrCallback)
                ErrCallback(cookie,MASTER_CHECK_MISSING_MANDATORY,ClassString,NULL);
	    }
	}
#endif
}

void MATROSKA_LinkCueSegmentInfo(matroska_cuepoint *Cue, ebml_master *SegmentInfo)
{
#if 0 // TODO
    assert(EBML_ElementIsType((ebml_element*)Cue, KaxCuePoint));
    assert(EBML_ElementIsType((ebml_element*)SegmentInfo, KaxInfo));
    Node_SET(Cue,MATROSKA_CUE_SEGMENTINFO,&SegmentInfo);
#endif
}

mkv_timestamp_t MATROSKA_CueTimestamp(const matroska_cuepoint *Cue)
{
#if 0 // TODO
    const ebml_integer *Timestamp;
    assert(EBML_ElementIsType((ebml_element*)Cue, KaxCuePoint));
	mkv_timestamp_t timestamp;
	if (!Cue->Timestamp(timestamp, ))
        return INVALID_TIMESTAMP_T;
    // return EBML_IntegerValue(Timestamp) * MATROSKA_SegmentInfoTimestampScale(Cue->SegInfo);
    return timestamp;
#endif
	return INVALID_TIMESTAMP_T;
}

void MATROSKA_LinkClusterBlocks(matroska_cluster *Cluster, EbmlMaster *RSegmentInfo, EbmlMaster *Tracks, bool_t KeepUnmatched, int ForProfile)
{
	// TODO
    auto timestamp = GetChild<KaxClusterTimestamp>(*Cluster);
    auto timestampscale = GetChild<KaxTimestampScale>(*RSegmentInfo);
    Cluster->InitTimestamp(static_cast<std::uint64_t>(timestamp), static_cast<std::uint64_t>(timestampscale));
}

mkv_timestamp_t MATROSKA_ClusterTimestamp(matroska_cluster *Cluster)
{
	return Cluster->GetBlockGlobalTimestamp(0);
}

bool MATROSKA_BlockKeyframe(KaxBlockGroup *BlockGroup, const KaxInternalBlock *Block)
{
    if (!BlockGroup)
        return false;

	if (EBML_MasterFindChild(BlockGroup,KaxReferenceBlock))
        return false;

    const auto *Duration = EBML_MasterFindChild(BlockGroup,KaxBlockDuration);
	if (Duration!=NULL && EBML_IntegerValue(Duration)==0)
        return false;

	return true;
}

bool MATROSKA_BlockLaced(const KaxInternalBlock *Block)
{
	// TODO move in libmatroska
    assert(Block->GetBestLacingType() != LacingType::LACING_AUTO);
	return Block->GetBestLacingType() != LacingType::LACING_NONE;
}

mkv_timestamp_t MATROSKA_BlockTimestamp(const KaxInternalBlock *Block)
{
#if 0 //TODO
    ebml_element *Cluster;
    assert(Node_IsPartOf(Block,MATROSKA_BLOCK_CLASS));
	if (Block->GlobalTimestamp!=INVALID_TIMESTAMP_T)
		return Block->GlobalTimestamp;
    if (Block->ReadTrack==NULL)
        return INVALID_TIMESTAMP_T;
    assert(Block->LocalTimestampUsed);
    Cluster = EBML_ElementParent(Block);
    while (Cluster && !EBML_ElementIsType(Cluster, KaxCluster))
        Cluster = EBML_ElementParent(Cluster);
    if (!Cluster)
        return INVALID_TIMESTAMP_T;
    Block->GlobalTimestamp = MATROSKA_ClusterTimestamp((matroska_cluster*)Cluster) + (mkv_timestamp_t)(Block->LocalTimestamp * MATROSKA_SegmentInfoTimestampScale(Block->ReadSegInfo) * MATROSKA_TrackTimestampScale(Block->ReadTrack));
    MATROSKA_BlockSetTimestamp(Block, Block->GlobalTimestamp, MATROSKA_ClusterTimestamp((matroska_cluster*)Cluster));
    return Block->GlobalTimestamp;
#endif
	return Block->GlobalTimestamp();
}

matroska_block *MATROSKA_GetBlockForTimestamp(matroska_cluster *Cluster, mkv_timestamp_t Timestamp, int16_t Track)
{
    EBML_MASTER_CONST_ITERATOR Block, GBlock;
    for (Block = EBML_MasterChildren(Cluster);EBML_MasterEnd(Block,Cluster);Block=EBML_MasterNext(Block))
    {
        if (EBML_ElementIsType(*Block, KaxBlockGroup))
        {
            for (GBlock = EBML_MasterChildren((KaxBlockGroup*)*Block);EBML_MasterEnd(GBlock,(KaxBlockGroup*)*Block);GBlock=EBML_MasterNext(GBlock))
            {
                if (EBML_ElementIsType(*GBlock, KaxBlock))
                {
                    if (MATROSKA_BlockTrackNum((matroska_block*)*GBlock) == Track &&
                        MATROSKA_BlockTimestamp((matroska_block*)*GBlock) == Timestamp)
                    {
                        return (matroska_block*)*GBlock;
                    }
                }
            }
        }
        else if (EBML_ElementIsType(*Block, KaxSimpleBlock))
        {
            if (MATROSKA_BlockTrackNum((matroska_block*)*Block) == Track &&
                MATROSKA_BlockTimestamp((matroska_block*)*Block) == Timestamp)
            {
                return (matroska_block*)*Block;
            }
        }
    }
    return NULL;
}


typedef struct track_info
{
    int Num;
    int Kind;
    filepos_t DataLength;
    EbmlString *CodecID;

} track_info;

static EbmlMaster *RSegmentInfo = NULL, *RTrackInfo = NULL, *RChapters = NULL, *RTags = NULL, *RCues = NULL, *RAttachments = NULL, *RSeekHead = NULL, *RSeekHead2 = NULL;
static std::vector<KaxCluster*> RClusters;
static std::vector<track_info> Tracks;
static size_t TrackMax=0;
static bool Warnings = true;
static bool Live = false;
static bool Details = false;
static bool DivX = false;
static bool Quiet = false;
static bool QuickExit = false;
static mkv_timestamp_t MinTime = INVALID_TIMESTAMP_T, MaxTime = INVALID_TIMESTAMP_T;
static mkv_timestamp_t ClusterTime = INVALID_TIMESTAMP_T;

// some macros for code readability
#define EL_Pos(elt)         EBML_ElementPosition(elt)
#define EL_Int(elt)         EBML_IntegerValue(elt)
#define EL_Type(elt, type)  EBML_ElementIsType(*elt, type)
#define EL_DataSize(elt)    EBML_ElementDataSize(*elt, 1)

#ifdef TARGET_WIN
#include <windows.h>
void DebugMessage(const tchar_t* Msg,...)
{
#if !defined(NDEBUG) || defined(LOGFILE) || defined(LOGTIME)
	va_list Args;
	tchar_t Buffer[1024],*s=Buffer;

	va_start(Args,Msg);
	vsnprintf(Buffer,TSIZEOF(Buffer), Msg, Args);
	va_end(Args);
	tcscat_s(Buffer,TSIZEOF(Buffer),T("\r\n"));
#endif

#ifdef LOGTIME
    {
        tchar_t timed[1024];
        SysTickToString(timed,TSIZEOF(timed),GetTimeTick(),1,1,0);
        stcatprintf_s(timed,TSIZEOF(timed),T(" %s"),s);
        s = timed;
    }
#endif

#if !defined(NDEBUG)
	OutputDebugString(s);
#endif

#if defined(LOGFILE)
{
    static FILE* f=NULL;
    static char s8[1024];
    size_t i;
    if (!f)
        f=fopen("\\corelog.txt","a+b");
    for (i=0;s[i];++i)
        s8[i]=(char)s[i];
    s8[i]=0;
    fputs(s8,f);
    fflush(f);
}
#endif
}
#endif

static const tchar_t *GetProfileName(size_t ProfileNum)
{
static const tchar_t *Profile[8] = {T("unknown"), T("matroska v1"), T("matroska v2"), T("matroska v3"), T("webm"), T("matroska+DivX"), T("matroska v4"), T("matroska v5")};
	switch (ProfileNum)
	{
	default:                  return Profile[0];
	case PROFILE_MATROSKA_V1: return Profile[1];
	case PROFILE_MATROSKA_V2: return Profile[2];
	case PROFILE_MATROSKA_V3: return Profile[3];
	case PROFILE_WEBM:        return Profile[4];
	case PROFILE_DIVX:        return Profile[5];
	case PROFILE_MATROSKA_V4: return Profile[6];
	case PROFILE_MATROSKA_V5: return Profile[7];
	}
}

static int OutputError(int ErrCode, const tchar_t *ErrString, ...)
{
	tchar_t Buffer[MAXLINE];
	va_list Args;
	va_start(Args,ErrString);
	vsnprintf(Buffer,TSIZEOF(Buffer), ErrString, Args);
	va_end(Args);
	fprintf(stderr,T("\rERR%03X: %s\r\n"),ErrCode,Buffer);
    if (QuickExit)
        exit(-ErrCode);
	return -ErrCode;
}

static void OutputWarning(int ErrCode, const tchar_t *ErrString, ...)
{
    if (Warnings)
    {
	    tchar_t Buffer[MAXLINE];
	    va_list Args;
	    va_start(Args,ErrString);
	    vsnprintf(Buffer,TSIZEOF(Buffer), ErrString, Args);
	    va_end(Args);
	    fprintf(stderr,T("\rWRN%03X: %s\r\n"),ErrCode,Buffer);
        if (QuickExit)
            exit(-ErrCode);
    }
}

static filepos_t CheckUnknownElements(EbmlMaster *Elt)
{
	tchar_t IdStr[32], String[MAXPATH];
	EBML_MASTER_CONST_ITERATOR SubElt;
	filepos_t VoidAmount = 0;
	for (SubElt = EBML_MasterChildren(Elt); EBML_MasterEnd(SubElt,Elt); EBML_MasterNext(SubElt))
	{
		if ((*SubElt)->GetClassId() == EBML_ID(EbmlDummy))
		{
            EBML_ElementGetName(Elt,String,TSIZEOF(String));
			EBML_IdToString(IdStr,TSIZEOF(IdStr),EBML_ElementClassID(SubElt));
			OutputWarning(12,T("Unknown element in %s %s at %") TPRId64 T(" (size %") TPRId64 T(" total %") TPRId64 T(")"),String,IdStr,EL_Pos(*SubElt),EL_DataSize(SubElt), EBML_ElementFullSize(*SubElt, 0));
		}
		else if ((*SubElt)->GetClassId() == EBML_ID(EbmlVoid))
		{
			VoidAmount = EBML_ElementFullSize(*SubElt,0);
		}
		else if ((*SubElt)->IsMaster())
		{
			VoidAmount += CheckUnknownElements(reinterpret_cast<EbmlMaster*>(*SubElt));
		}
	}
	return VoidAmount;
}

static int64_t gcd(int64_t a, int64_t b)
{
    for (;;)
    {
        int64_t c = a % b;
        if(!c) return b;
        a = b;
        b = c;
    }
}

static int CheckVideoTrack(EbmlMaster *Track, int TrackNum, int ProfileNum)
{
	int Result = 0;
	EbmlElement *Elt, *PixelW, *PixelH;
	EbmlSInteger *Unit;
	EbmlMaster *Video;
	Video = EBML_MasterFindChild(Track,KaxTrackVideo);
	if (!Video)
		Result = OutputError(0xE0,T("Video track at %") TPRId64 T(" is missing a Video element"),EL_Pos(Track));
	// check the DisplayWidth and DisplayHeight are correct
	else
	{
		int64_t DisplayW = 0,DisplayH = 0;
		PixelW = EBML_MasterGetChild(Video,KaxVideoPixelWidth, ProfileNum);
		if (!PixelW)
			Result |= OutputError(0xE1,T("Video track #%d at %") TPRId64 T(" has no pixel width"),TrackNum,EL_Pos(Track));
		PixelH = EBML_MasterGetChild(Video,KaxVideoPixelHeight, ProfileNum);
		if (!PixelH)
			Result |= OutputError(0xE2,T("Video track #%d at %") TPRId64 T(" has no pixel height"),TrackNum,EL_Pos(Track));

        Unit = (EbmlSInteger*)EBML_MasterGetChild(Video,KaxVideoDisplayUnit,ProfileNum);
		assert(Unit!=NULL);

		Elt = EBML_MasterFindChild(Video,KaxVideoDisplayWidth);
		if (Elt)
			DisplayW = EL_Int(Elt);
		else if (EL_Int(Unit)!=MATROSKA_DISPLAY_UNIT_PIXELS)
			Result |= OutputError(0xE2,T("Video track #%d at %") TPRId64 T(" has an implied non pixel width"),TrackNum,EL_Pos(Track));
        else if (PixelW)
			DisplayW = EL_Int(PixelW);

		Elt = EBML_MasterFindChild(Video,KaxVideoDisplayHeight);
		if (Elt)
			DisplayH = EL_Int(Elt);
		else if (EL_Int(Unit)!=MATROSKA_DISPLAY_UNIT_PIXELS)
			Result |= OutputError(0xE2,T("Video track #%d at %") TPRId64 T(" has an implied non pixel height"),TrackNum,EL_Pos(Track));
		else if (PixelH)
			DisplayH = EL_Int(PixelH);

		if (DisplayH==0)
			Result |= OutputError(0xE7,T("Video track #%d at %") TPRId64 T(" has a null display height"),TrackNum,EL_Pos(Track));
		if (DisplayW==0)
			Result |= OutputError(0xE7,T("Video track #%d at %") TPRId64 T(" has a null display width"),TrackNum,EL_Pos(Track));

		if (EL_Int(Unit)==MATROSKA_DISPLAY_UNIT_PIXELS && PixelW && PixelH)
		{
			// check if the pixel sizes appear valid
			if (DisplayW < EL_Int(PixelW) && DisplayH < EL_Int(PixelH))
			{
                int Serious = gcd(DisplayW,DisplayH)==1; // the DAR values were reduced as much as possible
                if (DisplayW*EL_Int(PixelH) == DisplayH*EL_Int(PixelW))
                    Serious++; // same aspect ratio as the source
                if (8*DisplayW <= EL_Int(PixelW) && 8*DisplayH <= EL_Int(PixelH))
                    Serious+=2; // too much shrinking compared to the original pixels
                if (ProfileNum!=PROFILE_WEBM)
                    --Serious; // in Matroska it's tolerated as it's been operating like that for a while

				if (Serious>2)
					Result |= OutputError(0xE3,T("The output pixels for Video track #%d seem wrong %") TPRId64 T("x%") TPRId64 T("px from %") TPRId64 T("x%") TPRId64,TrackNum,DisplayW,DisplayH,EL_Int(PixelW),EL_Int(PixelH));
				else if (Serious)
					OutputWarning(0xE3,T("The output pixels for Video track #%d seem wrong %") TPRId64 T("x%") TPRId64 T("px from %") TPRId64 T("x%") TPRId64,TrackNum,DisplayW,DisplayH,EL_Int(PixelW),EL_Int(PixelH));
			}
		}

        if (EL_Int(Unit)==MATROSKA_DISPLAY_UNIT_DISPLAYASPECTRATIO)
        {
            // crop values should never exist
            Elt = EBML_MasterFindChild(Video,KaxVideoPixelCropTop);
            if (Elt)
                Result |= OutputError(0xE4,T("Video track #%d is using unconstrained aspect ratio and has top crop at %") TPRId64,TrackNum,EL_Pos(Elt));
            Elt = EBML_MasterFindChild(Video,KaxVideoPixelCropBottom);
            if (Elt)
                Result |= OutputError(0xE4,T("Video track #%d is using unconstrained aspect ratio and has bottom crop at %") TPRId64,TrackNum,EL_Pos(Elt));
            Elt = EBML_MasterFindChild(Video,KaxVideoPixelCropLeft);
            if (Elt)
                Result |= OutputError(0xE4,T("Video track #%d is using unconstrained aspect ratio and has left crop at %") TPRId64,TrackNum,EL_Pos(Elt));
            Elt = EBML_MasterFindChild(Video,KaxVideoPixelCropRight);
            if (Elt)
                Result |= OutputError(0xE4,T("Video track #%d is using unconstrained aspect ratio and has right crop at %") TPRId64,TrackNum,EL_Pos(Elt));

			if (PixelW && DisplayW == EL_Int(PixelW))
				OutputWarning(0xE7,T("DisplayUnit seems to be pixels not aspect-ratio for Video track #%d %") TPRId64 T("px width from %") TPRId64,TrackNum,DisplayW,EL_Int(PixelW));
			if (PixelH && DisplayH == EL_Int(PixelH))
				OutputWarning(0xE7,T("DisplayUnit seems to be pixels not aspect-ratio for Video track #%d %") TPRId64 T("px height from %") TPRId64,TrackNum,DisplayH,EL_Int(PixelH));
        }
        else
        {
            // crop values should be less than the extended value
            PixelW = EBML_MasterGetChild(Video,KaxVideoPixelCropTop, ProfileNum);
            PixelH = EBML_MasterGetChild(Video,KaxVideoPixelCropBottom, ProfileNum);
            if (EL_Int(PixelW) + EL_Int(PixelH) >= DisplayH)
                Result |= OutputError(0xE5,T("Video track #%d is cropping too many vertical pixels %") TPRId64 T(" vs %") TPRId64 T(" + %") TPRId64,TrackNum, DisplayH, EL_Int(PixelW), EL_Int(PixelH));

            PixelW = EBML_MasterGetChild(Video,KaxVideoPixelCropLeft, ProfileNum);
            PixelH = EBML_MasterGetChild(Video,KaxVideoPixelCropRight, ProfileNum);
            if (EL_Int(PixelW) + EL_Int(PixelH) >= DisplayW)
                Result |= OutputError(0xE6,T("Video track #%d is cropping too many horizontal pixels %") TPRId64 T(" vs %") TPRId64 T(" + %") TPRId64,TrackNum, DisplayW, EL_Int(PixelW), EL_Int(PixelH));
        }
	}
	return Result;
}

static int CheckTracks(EbmlMaster *TrackEntries, int ProfileNum)
{
	EbmlMaster *Track;
	EbmlElement *TrackType, *TrackNum, *Elt, *Elt2;
	EbmlString *CodecID;
	tchar_t CodecName[MAXPATH],String[MAXPATH];
	int Result = 0;
	Track = EBML_MasterFindChild(TrackEntries, KaxTrackEntry);
	while (Track)
	{
        // check if the codec is valid for the profile
		TrackNum = EBML_MasterGetChild(Track, KaxTrackNumber, ProfileNum);
		if (TrackNum)
		{
			TrackType = EBML_MasterGetChild(Track, KaxTrackType, ProfileNum);
			CodecID = (EbmlString*)EBML_MasterGetChild(Track, KaxCodecID, ProfileNum);
			if (!CodecID)
				Result |= OutputError(0x300,T("Track #%d has no CodecID defined"),(int)EL_Int(TrackNum));
			else if (!TrackType)
				Result |= OutputError(0x301,T("Track #%d has no type defined"),(int)EL_Int(TrackNum));
			else
			{
				EBML_StringGet(CodecID,CodecName,TSIZEOF(CodecName));
				tcscpy_s(String,TSIZEOF(String),CodecName);
				if (tcscmp(tcsupr(String),CodecName)!=0)
					OutputWarning(0x307,T("Track #%d codec '%s' should be uppercase"),(int)EL_Int(TrackNum),CodecName);
				if (tcslen(String)<3 || String[1]!='_' || (String[0]!='A' && String[0]!='V' && String[0]!='S' && String[0]!='B'))
					OutputWarning(0x308,T("Track #%d codec '%s' doesn't appear to be valid"),(int)EL_Int(TrackNum),String);

                // check that the audio frequencies are not 0
                if (EL_Int(TrackType) == MATROSKA_TRACK_TYPE_AUDIO)
                {
                    Elt = EBML_MasterGetChild(Track, KaxTrackAudio, ProfileNum);
                    if (Elt==NULL)
                        Result |= OutputError(0x309,T("Audio Track #%d has no audio settings"),(int)EL_Int(TrackNum));
                    else
                    {
                        Elt2 = EBML_MasterFindChild((EbmlMaster*)Elt, KaxAudioOutputSamplingFreq);
                        if (Elt2 && EBML_FloatValue((EbmlFloat*)Elt2)==0)
                            Result |= OutputError(0x30A,T("Audio Track #%d has a null output sampling frequency"),(int)EL_Int(TrackNum));
                        Elt2 = EBML_MasterFindChild((EbmlMaster*)Elt, KaxAudioSamplingFreq);
                        if (Elt2 && EBML_FloatValue((EbmlFloat*)Elt2)==0)
                            Result |= OutputError(0x30A,T("Audio Track #%d has a null sampling frequency"),(int)EL_Int(TrackNum));
                    }
                }

				if (ProfileNum==PROFILE_WEBM)
				{
					if (EL_Int(TrackType) != MATROSKA_TRACK_TYPE_AUDIO && EL_Int(TrackType) != MATROSKA_TRACK_TYPE_VIDEO)
						Result |= OutputError(0x302,T("Track #%d type %d not supported for profile '%s'"),(int)EL_Int(TrackNum),(int)EL_Int(TrackType),GetProfileName(ProfileNum));
					if (CodecID)
					{
						if (EL_Int(TrackType) == MATROSKA_TRACK_TYPE_AUDIO)
						{
							if (!tcsisame_ascii(CodecName,T("A_VORBIS")) && !tcsisame_ascii(CodecName,T("A_OPUS")))
								Result |= OutputError(0x303,T("Track #%d codec %s not supported for profile '%s'"),(int)EL_Int(TrackNum),CodecName,GetProfileName(ProfileNum));
						}
						else if (EL_Int(TrackType) == MATROSKA_TRACK_TYPE_VIDEO)
						{
							if (!tcsisame_ascii(CodecName,T("V_VP8")) && !tcsisame_ascii(CodecName,T("V_VP9")))
								Result |= OutputError(0x304,T("Track #%d codec %s not supported for profile '%s'"),(int)EL_Int(TrackNum),CodecName,GetProfileName(ProfileNum));
						}
					}
				}
			}
		}

        // check if the AttachmentLink values match existing attachments
		TrackType = EBML_MasterFindChild(Track, KaxTrackAttachmentLink);
        while (TrackType)
        {
            if (!RAttachments)
            {
                if (TrackNum)
				    Result |= OutputError(0x305,T("Track #%d has attachment links but not attachments in the file"),(int)EL_Int(TrackNum));
                else
                    Result |= OutputError(0x305,T("Track at %") TPRId64 T(" has attachment links but not attachments in the file"),EL_Pos(Track));
                break;
            }

            for (EBML_MASTER_CONST_ITERATOR Elt=EBML_MasterChildren(RAttachments);EBML_MasterEnd(Elt,RAttachments);EBML_MasterNext(Elt))
            {
                if (EL_Type(Elt, KaxAttached))
                {
                    Elt2 = EBML_MasterFindChild((EbmlMaster*)*Elt, KaxFileUID);
                    if (Elt2 && EL_Int(Elt2) == EL_Int(TrackType))
                        break;
                }
            }
            if (!Elt)
            {
                if (TrackNum)
				    Result |= OutputError(0x306,T("Track #%d attachment link UID 0x%") TPRIx64 T(" not found in attachments"),(int)EL_Int(TrackNum),EL_Int(TrackType));
                else
                    Result |= OutputError(0x306,T("Track at %") TPRId64 T(" attachment link UID 0x%") TPRIx64 T(" not found in attachments"),EL_Pos(Track),EL_Int(TrackType));
            }

            TrackType = EBML_MasterNextChild(Track, TrackType);
        }

		Track = EBML_MasterNextChild(TrackEntries, Track);
	}
	return Result;
}

struct profile_check
{
    int *Result;
    const EbmlElement *Parent;
    const tchar_t *EltName;
    int ProfileMask;
};

static bool ProfileCallback(void *opaque, int type, const tchar_t *ClassName, const EbmlElement* Elt)
{
	struct profile_check *check = static_cast<profile_check*>(opaque);
    if (type==MASTER_CHECK_PROFILE_INVALID)
		*check->Result |= OutputError(0x201,T("Invalid '%s' for profile '%s' in %s at %") TPRId64,ClassName,GetProfileName(check->ProfileMask),check->EltName,EL_Pos(check->Parent));
    else if (type==MASTER_CHECK_MISSING_MANDATORY)
        *check->Result |= OutputError(0x200,T("Missing element '%s' in %s at %") TPRId64, ClassName,check->EltName,EL_Pos(check->Parent));
    else if (type==MASTER_CHECK_MULTIPLE_UNIQUE)
		*check->Result |= OutputError(0x202,T("Unique element '%s' in %s at %") TPRId64 T(" found more than once at %") TPRId64, ClassName,check->EltName,EL_Pos(check->Parent),EL_Pos(Elt));
    return 0; // don't remove anything
}

static int CheckProfileViolation(EbmlElement *Elt, int ProfileMask)
{
	int Result = 0;
	tchar_t String[MAXPATH];
    struct profile_check Checker;

	if (Elt->IsMaster())
	{
		EbmlMaster *Master = (EbmlMaster*)Elt;
	    EBML_ElementGetName(Elt,String,TSIZEOF(String));
        if (!EBML_MasterIsChecksumValid(Master))
            Result |= OutputError(0x203,T("Invalid checksum for element '%s' at %") TPRId64,String,EL_Pos(Elt));

        Checker.EltName = String;
        Checker.ProfileMask = ProfileMask;
        Checker.Parent = Elt;
        Checker.Result = &Result;
        EBML_MasterCheckContext(Master, ProfileMask, ProfileCallback, &Checker);

		for (auto SubElt = EBML_MasterChildren(Master); EBML_MasterEnd(SubElt,Master); EBML_MasterNext(SubElt))
			if ((*SubElt)->IsMaster())
    		    Result |= CheckProfileViolation(*SubElt,ProfileMask);
	}

	return Result;
}

static int CheckSeekHead(const KaxSegment& RSegment, EbmlMaster *SeekHead)
{
	int Result = 0;
	matroska_seekpoint *RLevel1 = (matroska_seekpoint*)EBML_MasterFindChild(SeekHead, KaxSeek);
    bool BSegmentInfo = 0, BTrackInfo = 0, BCues = 0, BTags = 0, BChapters = 0, BAttachments = 0, BSecondSeek = 0;
	while (RLevel1)
	{
		filepos_t Pos = RSegment.GetGlobalPosition(RLevel1->Location());
		auto aSeekId = GetChild<KaxSeekID>(*RLevel1);
		auto SeekId = EbmlId(aSeekId.GetBuffer(), aSeekId.GetSize());
		tchar_t IdString[32];

		EBML_IdToString(IdString,TSIZEOF(IdString),SeekId);
		if (Pos == INVALID_FILEPOS_T)
			Result |= OutputError(0x60,T("The SeekPoint at %") TPRId64 T(" has an unknown position (ID %s)"),EL_Pos(RLevel1),IdString);
		else if (SeekId.GetValue()==0)
			Result |= OutputError(0x61,T("The SeekPoint at %") TPRId64 T(" has no ID defined (position %") TPRId64 T(")"),EL_Pos(RLevel1),Pos);
		else if (MATROSKA_MetaSeekIsClass(RLevel1, KaxInfo))
		{
			if (!RSegmentInfo)
				Result |= OutputError(0x62,T("The SeekPoint at %") TPRId64 T(" references an unknown SegmentInfo at %") TPRId64,EL_Pos(RLevel1),Pos);
			else if (EL_Pos(RSegmentInfo) != Pos)
				Result |= OutputError(0x63,T("The SeekPoint at %") TPRId64 T(" references a SegmentInfo at wrong position %") TPRId64 T(" (real %") TPRId64 T(")"),EL_Pos(RLevel1),Pos,EL_Pos(RSegmentInfo));
            BSegmentInfo = 1;
		}
		else if (MATROSKA_MetaSeekIsClass(RLevel1, KaxTracks))
		{
			if (!RTrackInfo)
				Result |= OutputError(0x64,T("The SeekPoint at %") TPRId64 T(" references an unknown TrackInfo at %") TPRId64,EL_Pos(RLevel1),Pos);
			else if (EL_Pos(RTrackInfo) != Pos)
				Result |= OutputError(0x65,T("The SeekPoint at %") TPRId64 T(" references a TrackInfo at wrong position %") TPRId64 T(" (real %") TPRId64 T(")"),EL_Pos(RLevel1),Pos,EL_Pos(RTrackInfo));
            BTrackInfo = 1;
		}
		else if (MATROSKA_MetaSeekIsClass(RLevel1, KaxCues))
		{
			if (!RCues)
				Result |= OutputError(0x66,T("The SeekPoint at %") TPRId64 T(" references an unknown Cues at %") TPRId64,EL_Pos(RLevel1),Pos);
			else if (EL_Pos(RCues) != Pos)
				Result |= OutputError(0x67,T("The SeekPoint at %") TPRId64 T(" references a Cues at wrong position %") TPRId64 T(" (real %") TPRId64 T(")"),EL_Pos(RLevel1),Pos,EL_Pos(RCues));
            BCues = 1;
		}
		else if (MATROSKA_MetaSeekIsClass(RLevel1, KaxTags))
		{
			if (!RTags)
				Result |= OutputError(0x68,T("The SeekPoint at %") TPRId64 T(" references an unknown Tags at %") TPRId64,EL_Pos(RLevel1),Pos);
			else if (EL_Pos(RTags) != Pos)
				Result |= OutputError(0x69,T("The SeekPoint at %") TPRId64 T(" references a Tags at wrong position %") TPRId64 T(" (real %") TPRId64 T(")"),EL_Pos(RLevel1),Pos,EL_Pos(RTags));
            BTags = 1;
		}
		else if (MATROSKA_MetaSeekIsClass(RLevel1, KaxChapters))
		{
			if (!RChapters)
				Result |= OutputError(0x6A,T("The SeekPoint at %") TPRId64 T(" references an unknown Chapters at %") TPRId64,EL_Pos(RLevel1),Pos);
			else if (EL_Pos(RChapters) != Pos)
				Result |= OutputError(0x6B,T("The SeekPoint at %") TPRId64 T(" references a Chapters at wrong position %") TPRId64 T(" (real %") TPRId64 T(")"),EL_Pos(RLevel1),Pos,EL_Pos(RChapters));
            BChapters = 1;
		}
		else if (MATROSKA_MetaSeekIsClass(RLevel1, KaxAttachments))
		{
			if (!RAttachments)
				Result |= OutputError(0x6C,T("The SeekPoint at %") TPRId64 T(" references an unknown Attachments at %") TPRId64,EL_Pos(RLevel1),Pos);
			else if (EL_Pos(RAttachments) != Pos)
				Result |= OutputError(0x6D,T("The SeekPoint at %") TPRId64 T(" references a Attachments at wrong position %") TPRId64 T(" (real %") TPRId64 T(")"),EL_Pos(RLevel1),Pos,EL_Pos(RAttachments));
            BAttachments = 1;
		}
		else if (MATROSKA_MetaSeekIsClass(RLevel1, KaxSeekHead))
		{
			if (EL_Pos(SeekHead) == Pos)
				Result |= OutputError(0x6E,T("The SeekPoint at %") TPRId64 T(" references references its own SeekHead"),EL_Pos(RLevel1));
			else if (SeekHead == RSeekHead)
            {
                if (!RSeekHead2)
				    Result |= OutputError(0x6F,T("The SeekPoint at %") TPRId64 T(" references an unknown secondary SeekHead at %") TPRId64,EL_Pos(RLevel1),Pos);
                BSecondSeek = 1;
            }
			else if (SeekHead == RSeekHead2 && Pos!=EL_Pos(RSeekHead))
			    Result |= OutputError(0x70,T("The SeekPoint at %") TPRId64 T(" references an unknown extra SeekHead at %") TPRId64,EL_Pos(RLevel1),Pos);
		}
		else if (MATROSKA_MetaSeekIsClass(RLevel1, KaxCluster))
		{
			decltype(RClusters)::iterator Cluster;
			for (Cluster = ARRAYBEGIN(RClusters,EbmlElement*);Cluster != ARRAYEND(RClusters,EbmlElement*); ++Cluster)
			{
				if (EL_Pos(*Cluster) == Pos)
					break;
			}
			if (Cluster == ARRAYEND(RClusters,EbmlElement*) && Cluster != ARRAYBEGIN(RClusters,EbmlElement*))
				Result |= OutputError(0x71,T("The SeekPoint at %") TPRId64 T(" references a Cluster not found at %") TPRId64,EL_Pos(RLevel1),Pos);
		}
		else
			OutputWarning(0x860,T("The SeekPoint at %") TPRId64 T(" references an element that is not a known level 1 ID %s at %") TPRId64 T(")"),EL_Pos(RLevel1),IdString,Pos);
		RLevel1 = (matroska_seekpoint*)EBML_MasterNextChild(SeekHead, RLevel1);
	}
    if (SeekHead == RSeekHead)
    {
        if (!BSegmentInfo && RSegmentInfo)
            OutputWarning(0x861,T("The SegmentInfo is not referenced in the main SeekHead"));
        if (!BTrackInfo && RTrackInfo)
            OutputWarning(0x861,T("The TrackInfo is not referenced in the main SeekHead"));
        if (!BCues && RCues)
            OutputWarning(0x861,T("The Cues is not referenced in the main SeekHead"));
        if (!BTags && RTags)
            OutputWarning(0x861,T("The Tags is not referenced in the main SeekHead"));
        if (!BChapters && RChapters)
            OutputWarning(0x861,T("The Chapters is not referenced in the main SeekHead"));
        if (!BAttachments && RAttachments)
            OutputWarning(0x861,T("The Attachments is not referenced in the main SeekHead"));
        if (!BSecondSeek && RSeekHead2)
            OutputWarning(0x861,T("The secondary SeekHead is not referenced in the main SeekHead"));
    }
	return Result;
}

static void LinkClusterBlocks(int ProfileNum)
{
	decltype(RClusters)::iterator Cluster;
	for (Cluster=ARRAYBEGIN(RClusters,matroska_cluster*);Cluster!=ARRAYEND(RClusters,matroska_cluster*);++Cluster)
		MATROSKA_LinkClusterBlocks(*Cluster, RSegmentInfo, RTrackInfo, 1, ProfileNum);
}

static bool TrackIsLaced(int16_t TrackNum, int ProfileNum)
{
    EbmlElement *TrackData;
    EbmlMaster *Track = EBML_MasterFindChild(RTrackInfo, KaxTrackEntry);
    while (Track)
    {
        TrackData = EBML_MasterGetChild(Track, KaxTrackNumber, ProfileNum);
        if (EL_Int(TrackData) == TrackNum)
        {
            TrackData = EBML_MasterGetChild(Track, KaxTrackFlagLacing, ProfileNum);
            return EL_Int(TrackData) != 0;
        }
        Track = (EbmlMaster*)EBML_MasterNextChild(RTrackInfo, Track);
    }
    return 1;
}

static bool TrackIsVideo(int16_t TrackNum, int ProfileNum)
{
    EbmlElement *TrackData;
    EbmlMaster *Track = EBML_MasterFindChild(RTrackInfo, KaxTrackEntry);
    while (Track)
    {
        TrackData = EBML_MasterGetChild(Track, KaxTrackNumber, ProfileNum);
        if (EL_Int(TrackData) == TrackNum)
        {
            TrackData = EBML_MasterGetChild(Track, KaxTrackType, ProfileNum);
            return EL_Int(TrackData) == MATROSKA_TRACK_TYPE_VIDEO;
        }
        // look for TrackNum in the next Track
        Track = (EbmlMaster*)EBML_MasterNextChild(RTrackInfo, Track);
    }
    return 0;
}

static bool TrackNeedsKeyframe(int16_t TrackNum, int ProfileNum)
{
    EbmlElement *TrackData;
    EbmlMaster *Track = EBML_MasterFindChild(RTrackInfo, KaxTrackEntry);
    while (Track)
    {
        TrackData = EBML_MasterGetChild(Track, KaxTrackNumber, ProfileNum);
        if (EL_Int(TrackData) == TrackNum)
        {
            TrackData = EBML_MasterGetChild(Track, KaxTrackType, ProfileNum);
            switch (EL_Int(TrackData))
            {
            case MATROSKA_TRACK_TYPE_VIDEO:
                return 0;
            case MATROSKA_TRACK_TYPE_AUDIO:
                {
                    tchar_t CodecName[MAXDATA];
                    EbmlString *CodecID = (EbmlString*)EBML_MasterGetChild(Track, KaxCodecID, ProfileNum);
                    EBML_StringGet(CodecID,CodecName,TSIZEOF(CodecName));
                    return !tcsisame_ascii(CodecName,T("A_TRUEHD"));
                }
            default: return 1;
            }
        }
        // look for TrackNum in the next Track
        Track = (EbmlMaster*)EBML_MasterNextChild(RTrackInfo, Track);
    }
    return 0;
}

static int CheckVideoStart(int ProfileNum)
{
	int Result = 0;
	decltype(RClusters)::iterator Cluster;
    EBML_MASTER_ITERATOR Block, GBlock;
    int16_t BlockNum;
    mkv_timestamp_t ClusterTimestamp;
    std::vector<bool> TrackKeyframe;
	std::vector<filepos_t> TrackFirstKeyframePos;

	for (Cluster=ARRAYBEGIN(RClusters,EbmlMaster*);Cluster!=ARRAYEND(RClusters,EbmlMaster*);++Cluster)
    {
        ArrayInit(&TrackKeyframe);
        ArrayResize(&TrackKeyframe,TrackMax+1,bool,256);
        ArrayZero(&TrackKeyframe);
        ArrayInit(&TrackFirstKeyframePos);
        ArrayResize(&TrackFirstKeyframePos,TrackMax+1,filepos_t,256);
		ArrayZero(&TrackFirstKeyframePos);

        ClusterTimestamp = MATROSKA_ClusterTimestamp(*Cluster);
        if (ClusterTimestamp==INVALID_TIMESTAMP_T)
            Result |= OutputError(0xC1,T("The Cluster at %") TPRId64 T(" has no timestamp"),EL_Pos(*Cluster));
        else if (ClusterTime!=INVALID_TIMESTAMP_T && ClusterTime >= ClusterTimestamp)
			OutputWarning(0xC2,T("The timestamp of the Cluster at %") TPRId64 T(" is not incrementing (may be intentional)"),EL_Pos(*Cluster));
        ClusterTime = ClusterTimestamp;

	    for (Block = EBML_MasterChildren(*Cluster);EBML_MasterEnd(Block,*Cluster);EBML_MasterNext(Block))
	    {
		    if (EL_Type(Block, KaxBlockGroup))
		    {
			    for (GBlock = EBML_MasterChildren((KaxBlockGroup*)*Block);EBML_MasterEnd(GBlock,(KaxBlockGroup*)*Block);EBML_MasterNext(GBlock))
			    {
				    if (EL_Type(GBlock, KaxBlock))
				    {
                        BlockNum = MATROSKA_BlockTrackNum((KaxBlock*)*GBlock);
						if (BlockNum > ARRAYCOUNT(TrackKeyframe,bool))
							OutputError(0xC3,T("Unknown track #%d in Cluster at %") TPRId64 T(" in Block at %") TPRId64,(int)BlockNum,EL_Pos(*Cluster),EL_Pos((KaxBlock*)*GBlock));
                        else if (TrackIsVideo(BlockNum, ProfileNum))
						{
							if (!ARRAYBEGIN(TrackKeyframe,bool)[BlockNum] && MATROSKA_BlockKeyframe((KaxBlockGroup*)*Block, (KaxBlock*)*GBlock))
								ARRAYBEGIN(TrackKeyframe,bool)[BlockNum] = 1;
							if (!ARRAYBEGIN(TrackKeyframe,bool)[BlockNum] && ARRAYBEGIN(TrackFirstKeyframePos,filepos_t)[BlockNum]==0)
								ARRAYBEGIN(TrackFirstKeyframePos,filepos_t)[BlockNum] = EL_Pos(*Cluster);
                        }
					    break;
				    }
			    }
		    }
		    else if (EL_Type(Block, KaxSimpleBlock))
		    {
                BlockNum = MATROSKA_BlockTrackNum((KaxSimpleBlock*)*Block);
				if (BlockNum > ARRAYCOUNT(TrackKeyframe,bool))
                    OutputError(0xC3,T("Unknown track #%d in Cluster at %") TPRId64 T(" in SimpleBlock at %") TPRId64,(int)BlockNum,EL_Pos(*Cluster),EL_Pos((KaxSimpleBlock*)*Block));
                else if (TrackIsVideo(BlockNum, ProfileNum))
				{
					if (!ARRAYBEGIN(TrackKeyframe,bool)[BlockNum] && reinterpret_cast<const KaxSimpleBlock*>(*Block)->IsKeyframe())
						ARRAYBEGIN(TrackKeyframe,bool)[BlockNum] = 1;
					if (!ARRAYBEGIN(TrackKeyframe,bool)[BlockNum] && ARRAYBEGIN(TrackFirstKeyframePos,filepos_t)[BlockNum]==0)
						ARRAYBEGIN(TrackFirstKeyframePos,filepos_t)[BlockNum] = EL_Pos(*Cluster);
                }
		    }
	    }
		for (BlockNum=0;BlockNum<ARRAYCOUNT(TrackKeyframe,bool);++BlockNum)
		{
			if (ARRAYBEGIN(TrackKeyframe,bool)[BlockNum] && ARRAYBEGIN(TrackFirstKeyframePos,filepos_t)[BlockNum]!=0)
				OutputWarning(0xC0,T("First Block for video track #%d in Cluster at %") TPRId64 T(" is not a keyframe"),(int)BlockNum,ARRAYBEGIN(TrackFirstKeyframePos,filepos_t)[BlockNum]);
		}
        ArrayClear(&TrackKeyframe);
        ArrayClear(&TrackFirstKeyframePos);
    }
	return Result;
}

static int CheckPosSize(const EbmlElement *RSegment)
{
	int Result = 0;
	decltype(RClusters)::iterator Cluster;
	EbmlElement *PrevCluster=NULL;
    EbmlElement *Elt;

	for (Cluster=ARRAYBEGIN(RClusters,EbmlElement*);Cluster!=ARRAYEND(RClusters,EbmlElement*);++Cluster)
    {
        Elt = EBML_MasterFindChild((EbmlMaster*)*Cluster,KaxClusterPrevSize);
        if (Elt)
        {
            if (PrevCluster==NULL)
                Result |= OutputError(0xA0,T("The PrevSize %") TPRId64 T(" was set on the first Cluster at %") TPRId64,EL_Int(Elt),EL_Pos(Elt));
            else if (EL_Int(Elt) != EL_Pos(*Cluster) - EL_Pos(PrevCluster))
                Result |= OutputError(0xA1,T("The Cluster PrevSize %") TPRId64 T(" at %") TPRId64 T(" should be %") TPRId64,EL_Int(Elt),EL_Pos(Elt),EL_Pos(*Cluster) - EL_Pos(PrevCluster));
        }
        Elt = EBML_MasterFindChild((EbmlMaster*)*Cluster,KaxClusterPosition);
        if (Elt)
        {
            if (EL_Int(Elt) != EL_Pos(*Cluster) - EBML_ElementPositionData(RSegment))
                Result |= OutputError(0xA2,T("The Cluster position %") TPRId64 T(" at %") TPRId64 T(" should be %") TPRId64,EL_Int(Elt),EL_Pos(Elt),EL_Pos(*Cluster) - EBML_ElementPositionData(RSegment));
        }
        PrevCluster = *Cluster;
    }
	return Result;
}

static int CheckLacingKeyframe(int ProfileNum)
{
	int Result = 0;
	decltype(RClusters)::iterator Cluster;
    EBML_MASTER_CONST_ITERATOR Block, GBlock;
    int16_t BlockNum;
    mkv_timestamp_t BlockTime;
    size_t Frame,TrackIdx;

	for (Cluster=ARRAYBEGIN(RClusters,matroska_cluster*);Cluster!=ARRAYEND(RClusters,matroska_cluster*);++Cluster)
    {
	    for (Block = EBML_MasterChildren(*Cluster);EBML_MasterEnd(Block,*Cluster);EBML_MasterNext(Block))
	    {
		    if (EL_Type(Block, KaxBlockGroup))
		    {
			    for (GBlock = EBML_MasterChildren((KaxBlockGroup*)*Block);EBML_MasterEnd(GBlock,(KaxBlockGroup*)*Block);EBML_MasterNext(GBlock))
			    {
				    if (EL_Type(GBlock, KaxBlock))
				    {
                        //MATROSKA_ContextFlagLacing
                        BlockNum = MATROSKA_BlockTrackNum((KaxBlock*)*GBlock);
                        for (TrackIdx=0; TrackIdx<ARRAYCOUNT(Tracks,track_info); ++TrackIdx)
                            if (ARRAYBEGIN(Tracks,track_info)[TrackIdx].Num == BlockNum)
                                break;

                        if (TrackIdx==ARRAYCOUNT(Tracks,track_info))
                            Result |= OutputError(0xB2,T("Block at %") TPRId64 T(" is using an unknown track #%d"),EL_Pos(*GBlock),(int)BlockNum);
                        else
                        {
                            if (MATROSKA_BlockLaced((KaxBlock*)*GBlock) && !TrackIsLaced(BlockNum, ProfileNum))
                                Result |= OutputError(0xB0,T("Block at %") TPRId64 T(" track #%d is laced but the track is not"),EL_Pos(*GBlock),(int)BlockNum);
                            if (!MATROSKA_BlockKeyframe((KaxBlockGroup*)*Block, (KaxBlock*)*GBlock) && TrackNeedsKeyframe(BlockNum, ProfileNum))
                                Result |= OutputError(0xB1,T("Block at %") TPRId64 T(" track #%d is not a keyframe"),EL_Pos(*GBlock),(int)BlockNum);

                            for (Frame=0; Frame<MATROSKA_BlockGetFrameCount((KaxBlock*)*GBlock); ++Frame)
                                ARRAYBEGIN(Tracks,track_info)[TrackIdx].DataLength += MATROSKA_BlockGetLength((KaxBlock*)*GBlock,Frame);
                            if (Details)
                            {
                                BlockTime = MATROSKA_BlockTimestamp((KaxBlock*)*GBlock);
                                if (MinTime==INVALID_TIMESTAMP_T || MinTime>BlockTime)
                                    MinTime = BlockTime;
                                if (MaxTime==INVALID_TIMESTAMP_T || MaxTime<BlockTime)
                                    MaxTime = BlockTime;
                            }
                        }
					    break;
				    }
			    }
		    }
		    else if (EL_Type(Block, KaxSimpleBlock))
		    {
                BlockNum = MATROSKA_BlockTrackNum((KaxSimpleBlock*)*Block);
                for (TrackIdx=0; TrackIdx<ARRAYCOUNT(Tracks,track_info); ++TrackIdx)
                    if (ARRAYBEGIN(Tracks,track_info)[TrackIdx].Num == BlockNum)
                        break;

                if (TrackIdx==ARRAYCOUNT(Tracks,track_info))
                    Result |= OutputError(0xB2,T("Block at %") TPRId64 T(" is using an unknown track #%d"),EL_Pos(*Block),(int)BlockNum);
                else
                {
                    if (MATROSKA_BlockLaced((KaxSimpleBlock*)*Block) && !TrackIsLaced(BlockNum, ProfileNum))
                        Result |= OutputError(0xB0,T("SimpleBlock at %") TPRId64 T(" track #%d is laced but the track is not"),EL_Pos(*Block),(int)BlockNum);
                    if (!reinterpret_cast<const KaxSimpleBlock*>(*Block)->IsKeyframe() && TrackNeedsKeyframe(BlockNum, ProfileNum))
                        Result |= OutputError(0xB1,T("SimpleBlock at %") TPRId64 T(" track #%d is not a keyframe"),EL_Pos(*Block),(int)BlockNum);
                    for (Frame=0; Frame<MATROSKA_BlockGetFrameCount((KaxSimpleBlock*)*Block); ++Frame)
                        ARRAYBEGIN(Tracks,track_info)[TrackIdx].DataLength += MATROSKA_BlockGetLength((KaxSimpleBlock*)*Block,Frame);
                    if (Details)
                    {
                        BlockTime = MATROSKA_BlockTimestamp((KaxSimpleBlock*)*Block);
                        if (MinTime==INVALID_TIMESTAMP_T || MinTime>BlockTime)
                            MinTime = BlockTime;
                        if (MaxTime==INVALID_TIMESTAMP_T || MaxTime<BlockTime)
                            MaxTime = BlockTime;
                    }
                }
		    }
	    }
    }
	return Result;
}

static int CheckCueEntries(EbmlMaster *Cues)
{
	int Result = 0;
	mkv_timestamp_t TimestampEntry, PrevTimestamp = INVALID_TIMESTAMP_T;
	int16_t TrackNumEntry;
	decltype(RClusters)::iterator Cluster;
    int ClustNum = 0;

	if (!RSegmentInfo)
		Result |= OutputError(0x310,T("A Cues (index) is defined but no SegmentInfo was found"));
	else if (ARRAYCOUNT(RClusters,matroska_cluster*))
	{
		matroska_cuepoint *CuePoint = (matroska_cuepoint*)EBML_MasterFindChild(Cues, KaxCuePoint);
		while (CuePoint)
		{
            if (!Quiet && ClustNum++ % 24 == 0)
                fprintf(stderr,T("."));
			MATROSKA_LinkCueSegmentInfo(CuePoint,RSegmentInfo);
			TimestampEntry = MATROSKA_CueTimestamp(CuePoint);
			TrackNumEntry = MATROSKA_CueTrackNum(CuePoint);

			if (TimestampEntry < PrevTimestamp && PrevTimestamp != INVALID_TIMESTAMP_T)
				OutputWarning(0x311,T("The Cues entry for timestamp %") TPRId64 T(" ms is listed after entry %") TPRId64 T(" ms"),Scale64(TimestampEntry,1,1000000),Scale64(PrevTimestamp,1,1000000));

			// find a matching Block
			for (Cluster = ARRAYBEGIN(RClusters,matroska_cluster*);Cluster != ARRAYEND(RClusters,matroska_cluster*); ++Cluster)
			{
				auto Block = MATROSKA_GetBlockForTimestamp(*Cluster, TimestampEntry, TrackNumEntry);
				if (Block)
					break;
			}
			if (Cluster == ARRAYEND(RClusters,matroska_cluster*))
				Result |= OutputError(0x312,T("CueEntry Track #%d and timestamp %") TPRId64 T(" ms not found"),(int)TrackNumEntry,Scale64(TimestampEntry,1,1000000));
			PrevTimestamp = TimestampEntry;
			CuePoint = (matroska_cuepoint*)EBML_MasterNextChild(Cues, CuePoint);
		}
	}
	return Result;
}

#if defined(_WIN32) && defined(UNICODE)
int wmain(int argc, const wchar_t *argv[])
#else
int main(int argc, const char *argv[])
#endif
{
    int Result = 0;
    int ShowUsage = 0;
    int ShowVersion = 0;
    // parsercontext p;
	void *p;
    // textwriter _StdErr;
    StdIOCallback *Input = NULL;
	std::unique_ptr<EbmlStream> stream;
    tchar_t Path[MAXPATHFULL];
    tchar_t String[MAXLINE];
    EbmlMaster *EbmlHead = NULL, *RSegment = NULL, *RLevel1 = NULL, *Prev, *RLevelX;
	decltype(RClusters)::iterator Cluster;
	EbmlElement *EbmlDocVer, *EbmlReadDocVer;
    EbmlString *LibName, *AppName;
    // ebml_parser_context RContext;
    // ebml_parser_context RSegmentContext;
    int i,UpperElement;
	int MatroskaProfile = 0;
    bool HasVideo = 0;
	int DotCount;
    std::vector<track_info>::iterator TI;
	filepos_t VoidAmount = 0;

#if 0
    // Core-C init phase
    ParserContext_Init(&p,NULL,NULL,NULL);
	Node_SetData(&p.Base.Base.Base,NODECONTEXT_PROJECT_VENDOR,TYPE_STRING,"Matroska");
	Node_SetData(&p.Base.Base.Base,NODECONTEXT_PROJECT_VERSION,TYPE_STRING,PROJECT_VERSION);
	Node_SetData(&p.Base.Base.Base,NODECONTEXT_PROJECT_NAME,TYPE_STRING,PROJECT_NAME);

    // EBML & Matroska Init
    MATROSKA_Init(&p);
#endif
    ArrayInit(&RClusters);
    ArrayInit(&Tracks);

	for (i=1;i<argc;++i)
	{
#if defined(_WIN32) && defined(UNICODE)
	    Node_FromWcs(&p,Path,TSIZEOF(Path),argv[i]);
#else
		Node_FromStr(&p,Path,TSIZEOF(Path),argv[i]);
#endif
		if (tcsisame_ascii(Path,T("--no-warn"))) Warnings = 0;
		else if (tcsisame_ascii(Path,T("--live"))) Live = 1;
		else if (tcsisame_ascii(Path,T("--details"))) Details = 1;
		else if (tcsisame_ascii(Path,T("--divx"))) DivX = 1;
		else if (tcsisame_ascii(Path,T("--version"))) ShowVersion = 1;
		else if (tcsisame_ascii(Path,T("--quiet"))) Quiet = 1;
        else if (tcsisame_ascii(Path,T("--quick"))) QuickExit = 1;
        else if (tcsisame_ascii(Path,T("--help"))) {ShowVersion = 1; ShowUsage = 1;}
		else if (i<argc-1) fprintf(stderr,T("Unknown parameter '%s'\r\n"),Path);
	}

    if (argc < 2 || ShowVersion)
    {
        fprintf(stderr,PROJECT_NAME T(" v") PROJECT_VERSION T(", Copyright (c) 2010-2024 Matroska Foundation\r\n"));
        if (argc < 2 || ShowUsage)
        {
            Result = OutputError(1,T("Usage: ") PROJECT_NAME T(" [options] <matroska_src>"));
		    fprintf(stderr,T("Options:\r\n"));
		    fprintf(stderr,T("  --no-warn   only output errors, no warnings\r\n"));
            fprintf(stderr,T("  --live      only output errors/warnings relevant to live streams\r\n"));
            fprintf(stderr,T("  --details   show details for valid files\r\n"));
            fprintf(stderr,T("  --divx      assume the file is using DivX specific extensions\r\n"));
            fprintf(stderr,T("  --quick     exit after the first error or warning\r\n"));
            fprintf(stderr,T("  --quiet     don't ouput progress and file info\r\n"));
            fprintf(stderr,T("  --version   show the version of ") PROJECT_NAME T("\r\n"));
            fprintf(stderr,T("  --help      show this screen\r\n"));
        }
        goto exit;
    }

#if defined(_WIN32) && defined(UNICODE)
    Node_FromWcs(&p,Path,TSIZEOF(Path),argv[argc-1]);
#else
	Node_FromStr(&p,Path,TSIZEOF(Path),argv[argc-1]);
#endif
    // Input = StreamOpen(&p,Path,SFLAG_RDONLY/*|SFLAG_BUFFERED*/);
	Input = new StdIOCallback(Path, MODE_READ);
    if (!Input)
    {
        fprintf(stderr,T("Could not open file \"%s\" for reading\r\n"),Path);
        Result = -2;
        goto exit;
    }

    // parse the source file to determine if it's a Matroska file and determine the location of the key parts
	stream = std::make_unique<EbmlStream>(*Input);
#if 0
    RContext.Context = KaxStream;
    RContext.EndPosition = INVALID_FILEPOS_T;
    RContext.UpContext = NULL;
    RContext.Profile = EBML_ANY_PROFILE;
#endif
    EbmlHead = (EbmlMaster*)EBML_FindNextElement(stream.get(), Context_KaxMatroska, &UpperElement, 0);
	if (!EbmlHead || !EL_Type(&EbmlHead, libebml::EbmlHead))
    {
        Result = OutputError(3,T("EBML head not found! Are you sure it's a matroska/webm file?"));
        goto exit;
    }

    if (!Quiet) fprintf(stderr,T("."));

	if (EBML_ElementReadData(EbmlHead,*Input,&RContext,0,SCOPE_ALL_DATA, 1)==INVALID_FILEPOS_T)
    {
        Result = OutputError(4,T("Could not read the EBML head"));
        goto exit;
    }
    if (!EBML_MasterIsChecksumValid(EbmlHead))
    {
        Result = OutputError(12,T("The EBML header is damaged (invalid CheckSum)"));
        goto exit;
    }

	VoidAmount += CheckUnknownElements(EbmlHead);

	RLevel1 = (EbmlMaster*)EBML_MasterGetChild(EbmlHead,EReadVersion, EBML_ANY_PROFILE);
	if (EL_Int(RLevel1) > EBML_MAX_VERSION)
		OutputError(5,T("The EBML read version is not supported: %d"),(int)EL_Int(RLevel1));

	RLevel1 = (EbmlMaster*)EBML_MasterGetChild(EbmlHead,EMaxIdLength, EBML_ANY_PROFILE);
	if (EL_Int(RLevel1) > EBML_MAX_ID)
		OutputError(6,T("The EBML max ID length is not supported: %d"),(int)EL_Int(RLevel1));

	RLevel1 = (EbmlMaster*)EBML_MasterGetChild(EbmlHead,EMaxSizeLength, EBML_ANY_PROFILE);
	if (EL_Int(RLevel1) > EBML_MAX_SIZE)
		OutputError(7,T("The EBML max size length is not supported: %d"),(int)EL_Int(RLevel1));

	RLevel1 = (EbmlMaster*)EBML_MasterGetChild(EbmlHead,EDocType, EBML_ANY_PROFILE);
    EBML_StringGet((EbmlString*)RLevel1,String,TSIZEOF(String));
    if (tcscmp(String,T("matroska"))!=0 && tcscmp(String,T("webm"))!=0)
	{
		Result = OutputError(8,T("The EBML doctype is not supported: %s"),String);
		goto exit;
	}

	EbmlDocVer = EBML_MasterGetChild(EbmlHead,EDocTypeVersion, EBML_ANY_PROFILE);
	EbmlReadDocVer = EBML_MasterGetChild(EbmlHead,EDocTypeReadVersion, EBML_ANY_PROFILE);

	if (EL_Int(EbmlReadDocVer) > EL_Int(EbmlDocVer))
		OutputError(9,T("The DocType version %d is higher than the read Doctype version %d"),(int)EL_Int(EbmlDocVer),(int)EL_Int(EbmlReadDocVer));

	if (tcscmp(String,T("matroska"))==0)
	{
        if (DivX)
			MatroskaProfile = PROFILE_DIVX;
        else if (EL_Int(EbmlDocVer)==5)
		    MatroskaProfile = PROFILE_MATROSKA_V5;
        else if (EL_Int(EbmlDocVer)==4)
		    MatroskaProfile = PROFILE_MATROSKA_V4;
        else if (EL_Int(EbmlDocVer)==3)
		    MatroskaProfile = PROFILE_MATROSKA_V3;
        else if (EL_Int(EbmlDocVer)==2)
		    MatroskaProfile = PROFILE_MATROSKA_V2;
		else if (EL_Int(EbmlDocVer)==1)
	    	MatroskaProfile = PROFILE_MATROSKA_V1;
		else
			OutputError(10,T("Unknown Matroska profile %d/%d"),(int)EL_Int(EbmlDocVer),(int)EL_Int(EbmlReadDocVer));
	}
	else if (tcscmp(String,T("webm"))==0)
		MatroskaProfile = PROFILE_WEBM;

    if (!Quiet) fprintf(stderr,T("."));

	// find the segment
	RSegment = (EbmlMaster*)EBML_FindNextElement(stream.get(), Context_KaxMatroska, &UpperElement, 1);
    if (RSegment == NULL)
    {
        Result = OutputError(0x20, T("No Segment found"));
        goto exit;
    }
#if 0
    RSegmentContext.Context = KaxSegment;
    RSegmentContext.EndPosition = EBML_ElementPositionEnd((EbmlElement*)RSegment);
    RSegmentContext.UpContext = &RContext;
    RSegmentContext.Profile = MatroskaProfile;

    RContext.EndPosition = EBML_ElementPositionEnd((EbmlElement*)RSegment);
#endif

	UpperElement = 0;
	DotCount = 0;
	Prev = NULL;
    RLevel1 = (EbmlMaster*)EBML_FindNextElement(stream.get(), Context_KaxSegment, &UpperElement, 1);
    while (RLevel1)
	{
        RLevelX = NULL;
        if (EL_Type(&RLevel1, KaxCluster))
        {
            if (EBML_ElementReadData(RLevel1,*Input,Context_KaxSegment,0,SCOPE_PARTIAL_DATA,4)!=INVALID_FILEPOS_T)
			{
                ArrayAppend(&RClusters,(KaxCluster*)RLevel1,sizeof(RLevel1),256);
				((KaxCluster*)RLevel1)->SetParent(*(KaxSegment*)RSegment);
				// NodeTree_SetParent(RLevel1, RSegment, NULL);
				VoidAmount += CheckUnknownElements(RLevel1);
				Result |= CheckProfileViolation(RLevel1, MatroskaProfile);
                RLevelX = (EbmlMaster*)EBML_ElementSkipData(RLevel1, *stream.get(), Context_KaxSegment, NULL, 1);
			}
			else
			{
				Result = OutputError(0x180,T("Failed to read the Cluster at %") TPRId64 T(" size %") TPRId64,EL_Pos(RLevel1),EL_DataSize(&RLevel1));
				goto exit;
			}
        }
        else if (EL_Type(&RLevel1, KaxSeekHead))
        {
            if (Live)
            {
                OutputWarning(0x170,T("The live stream has a SeekHead at %") TPRId64,EL_Pos(RLevel1));
			    RLevelX = (EbmlMaster*)EBML_ElementSkipData(RLevel1, *stream.get(), Context_KaxSegment, NULL, 1);
                NodeDelete(RLevel1);
                RLevel1 = NULL;
            }
            else if (EBML_ElementReadData(RLevel1,*Input,Context_KaxSegment,1,SCOPE_ALL_DATA,2)!=INVALID_FILEPOS_T)
			{
				if (!RSeekHead)
					RSeekHead = RLevel1;
				else if (!RSeekHead2)
                {
					OutputWarning(0x103,T("Unnecessary secondary SeekHead was found at %") TPRId64,EL_Pos(RLevel1));
					RSeekHead2 = RLevel1;
                }
				else
					OutputWarning(0x101,T("Extra SeekHead found at %") TPRId64 T(" (size %") TPRId64 T(")"),EL_Pos(RLevel1),EL_DataSize(&RLevel1));
				NodeTree_SetParent(RLevel1, RSegment, NULL);
				VoidAmount += CheckUnknownElements(RLevel1);
				Result |= CheckProfileViolation(RLevel1, MatroskaProfile);
			}
			else
			{
				Result = OutputError(0x100,T("Failed to read the SeekHead at %") TPRId64 T(" size %") TPRId64,EL_Pos(RLevel1),EL_DataSize(&RLevel1));
				goto exit;
			}
		}
        else if (EL_Type(&RLevel1, KaxInfo))
        {
            if (EBML_ElementReadData(RLevel1,*Input,Context_KaxSegment,1,SCOPE_ALL_DATA,1)!=INVALID_FILEPOS_T)
			{
				if (RSegmentInfo != NULL)
					OutputWarning(0x110,T("Extra SegmentInfo found at %") TPRId64 T(" (size %") TPRId64 T(")"),EL_Pos(RLevel1),EL_DataSize(&RLevel1));
				else
				{
					RSegmentInfo = RLevel1;
					NodeTree_SetParent(RLevel1, RSegment, NULL);
					VoidAmount += CheckUnknownElements(RLevel1);
					Result |= CheckProfileViolation(RLevel1, MatroskaProfile);

                    if (Live)
                    {
                        KaxDuration *Elt = EBML_MasterFindChild(RLevel1,KaxDuration);
                        if (Elt)
                            OutputWarning(0x112,T("The live Segment has a duration set at %") TPRId64,EL_Pos(Elt));
                    }
                }
			}
			else
			{
				Result = OutputError(0x111,T("Failed to read the SegmentInfo at %") TPRId64 T(" size %") TPRId64,EL_Pos(RLevel1),EL_DataSize(&RLevel1));
				goto exit;
			}
		}
        else if (EL_Type(&RLevel1, KaxTracks))
        {
            if (EBML_ElementReadData(RLevel1,*Input,Context_KaxSegment,1,SCOPE_ALL_DATA,4)!=INVALID_FILEPOS_T)
			{
				if (RTrackInfo != NULL)
					OutputWarning(0x120,T("Extra TrackInfo found at %") TPRId64 T(" (size %") TPRId64 T(")"),EL_Pos(RLevel1),EL_DataSize(&RLevel1));
				else
				{
                    size_t TrackCount;
					EbmlMaster *Elt;

                    RTrackInfo = RLevel1;
					NodeTree_SetParent(RLevel1, RSegment, NULL);
					VoidAmount += CheckUnknownElements(RLevel1);
					Result |= CheckProfileViolation(RLevel1, MatroskaProfile);

                    Elt = EBML_MasterFindChild(RTrackInfo,KaxTrackEntry);
                    TrackCount = 0;
                    while (Elt)
                    {
						Elt = (EbmlMaster*)EBML_MasterNextChild(RTrackInfo,Elt);
                        ++TrackCount;
                    }

                    ArrayResize(&Tracks,TrackCount,track_info,256);
                    ArrayZero(&Tracks);

                    Elt = EBML_MasterFindChild(RTrackInfo,KaxTrackEntry);
                    TrackCount = 0;
                    while (Elt)
                    {
                        EbmlDocVer = EBML_MasterFindChild(Elt,KaxTrackNumber);
                        assert(EbmlDocVer!=NULL);
                        if (EbmlDocVer)
                        {
                            TrackMax = std::max(TrackMax,(size_t)EL_Int(EbmlDocVer));
                            ARRAYBEGIN(Tracks,track_info)[TrackCount].Num = (int)EL_Int(EbmlDocVer);
                        }
                        EbmlDocVer = EBML_MasterFindChild(Elt,KaxTrackType);
                        assert(EbmlDocVer!=NULL);
                        if (EbmlDocVer)
                        {
                            if (EL_Int(EbmlDocVer)==MATROSKA_TRACK_TYPE_VIDEO)
							{
								Result |= CheckVideoTrack(Elt, ARRAYBEGIN(Tracks,track_info)[TrackCount].Num, MatroskaProfile);
                                HasVideo = 1;
							}
                            ARRAYBEGIN(Tracks,track_info)[TrackCount].Kind = (int)EL_Int(EbmlDocVer);
                        }
                        ARRAYBEGIN(Tracks,track_info)[TrackCount].CodecID = (EbmlString*)EBML_MasterFindChild(Elt,KaxCodecID);
                        Elt = (EbmlMaster*)EBML_MasterNextChild(RTrackInfo,Elt);
                        ++TrackCount;
                    }
                    EbmlDocVer = NULL;
                    Elt = NULL;
                }
			}
			else
			{
				Result = OutputError(0x121,T("Failed to read the TrackInfo at %") TPRId64 T(" size %") TPRId64,EL_Pos(RLevel1),EL_DataSize(&RLevel1));
				goto exit;
			}
		}
        else if (EL_Type(&RLevel1, KaxCues))
        {
            if (Live)
            {
                OutputWarning(0x171,T("The live stream has Cues at %") TPRId64,EL_Pos(RLevel1));
			    RLevelX = (EbmlMaster*)EBML_ElementSkipData(RLevel1, *stream.get(), Context_KaxSegment, NULL, 1);
                NodeDelete(RLevel1);
                RLevel1 = NULL;
            }
            else if (EBML_ElementReadData(RLevel1,*Input,Context_KaxSegment,1,SCOPE_ALL_DATA,3)!=INVALID_FILEPOS_T)
			{
				if (RCues != NULL)
					OutputWarning(0x130,T("Extra Cues found at %") TPRId64 T(" (size %") TPRId64 T(")"),EL_Pos(RLevel1),EL_DataSize(&RLevel1));
				else
				{
					RCues = RLevel1;
					NodeTree_SetParent(RLevel1, RSegment, NULL);
					VoidAmount += CheckUnknownElements(RLevel1);
					Result |= CheckProfileViolation(RLevel1, MatroskaProfile);
				}
			}
			else
			{
				Result = OutputError(0x131,T("Failed to read the Cues at %") TPRId64 T(" size %") TPRId64,EL_Pos(RLevel1),EL_DataSize(&RLevel1));
				goto exit;
			}
		}
        else if (EL_Type(&RLevel1, KaxChapters))
        {
            if (Live)
            {
                Result |= OutputError(0x172,T("The live stream has Chapters at %") TPRId64,EL_Pos(RLevel1));
			    RLevelX = (EbmlMaster*)EBML_ElementSkipData(RLevel1, *stream.get(), Context_KaxSegment, NULL, 1);
                NodeDelete(RLevel1);
                RLevel1 = NULL;
            }
            else if (EBML_ElementReadData(RLevel1,*Input,Context_KaxSegment,1,SCOPE_ALL_DATA,16)!=INVALID_FILEPOS_T)
			{
				if (RChapters != NULL)
					OutputWarning(0x140,T("Extra Chapters found at %") TPRId64 T(" (size %") TPRId64 T(")"),EL_Pos(RLevel1),EL_DataSize(&RLevel1));
				else
				{
					RChapters = RLevel1;
					NodeTree_SetParent(RLevel1, RSegment, NULL);
					VoidAmount += CheckUnknownElements(RLevel1);
					Result |= CheckProfileViolation(RLevel1, MatroskaProfile);
				}
			}
			else
			{
				Result = OutputError(0x141,T("Failed to read the Chapters at %") TPRId64 T(" size %") TPRId64,EL_Pos(RLevel1),EL_DataSize(&RLevel1));
				goto exit;
			}
		}
        else if (EL_Type(&RLevel1, KaxTags))
        {
            if (EBML_ElementReadData(RLevel1,*Input,Context_KaxSegment,1,SCOPE_ALL_DATA,4)!=INVALID_FILEPOS_T)
			{
				if (RTags != NULL)
					Result |= OutputError(0x150,T("Extra Tags found at %") TPRId64 T(" (size %") TPRId64 T(")"),EL_Pos(RLevel1),EL_DataSize(&RLevel1));
				else
				{
					RTags = RLevel1;
					NodeTree_SetParent(RLevel1, RSegment, NULL);
					VoidAmount += CheckUnknownElements(RLevel1);
					Result |= CheckProfileViolation(RLevel1, MatroskaProfile);
				}
			}
			else
			{
				Result = OutputError(0x151,T("Failed to read the Tags at %") TPRId64 T(" size %") TPRId64,EL_Pos(RLevel1),EL_DataSize(&RLevel1));
				goto exit;
			}
		}
        else if (EL_Type(&RLevel1, KaxAttachments))
        {
            if (Live)
            {
                Result |= OutputError(0x173,T("The live stream has a Attachments at %") TPRId64,EL_Pos(RLevel1));
			    RLevelX = (EbmlMaster*)EBML_ElementSkipData(RLevel1, *stream.get(), Context_KaxSegment, NULL, 1);
                NodeDelete(RLevel1);
                RLevel1 = NULL;
            }
            else if (EBML_ElementReadData(RLevel1,*Input,Context_KaxSegment,1,SCOPE_ALL_DATA,3)!=INVALID_FILEPOS_T)
			{
				if (RAttachments != NULL)
					Result |= OutputError(0x160,T("Extra Attachments found at %") TPRId64 T(" (size %") TPRId64 T(")"),EL_Pos(RLevel1),EL_DataSize(&RLevel1));
				else
				{
					RAttachments = RLevel1;
					NodeTree_SetParent(RLevel1, RSegment, NULL);
					VoidAmount += CheckUnknownElements(RLevel1);
					Result |= CheckProfileViolation(RLevel1, MatroskaProfile);
				}
			}
			else
			{
				Result = OutputError(0x161,T("Failed to read the Attachments at %") TPRId64 T(" size %") TPRId64,EL_Pos(RLevel1),EL_DataSize(&RLevel1));
				goto exit;
			}
		}
		else
		{
			if ((RLevel1)->GetClassId() == EBML_ID(EbmlDummy))
			{
				tchar_t Id[32];
				EBML_IdToString(Id,TSIZEOF(Id),EBML_ElementClassID(&RLevel1));
				OutputWarning(0x80,T("Unknown element %s at %") TPRId64 T(" size %") TPRId64,Id,EL_Pos(RLevel1),EL_DataSize(&RLevel1));
			}
			if ((RLevel1)->GetClassId() == EBML_ID(EbmlVoid))
			{
				VoidAmount += EBML_ElementFullSize(RLevel1,0);
			}
			RLevelX = (EbmlMaster*)EBML_ElementSkipData(RLevel1, *stream.get(), Context_KaxSegment, NULL, 1);
            NodeDelete(RLevel1);
            RLevel1 = NULL;
		}
        if (!Quiet) {
            fprintf(stderr,T(".")); ++DotCount;
		    if (!(DotCount % 60))
			    fprintf(stderr,T("\r                                                              \r"));
        }

		Prev = RLevel1;
        if (RLevelX)
            RLevel1 = RLevelX;
        else
		    RLevel1 = (EbmlMaster*)EBML_FindNextElement(stream.get(), Context_KaxSegment, &UpperElement, 1);
	}

	if (!RSegmentInfo)
	{
		Result = OutputError(0x40,T("The segment is missing a SegmentInfo"));
		goto exit;
	}

	if (Prev)
	{
		if (EBML_ElementPositionEnd((EbmlElement*)RSegment)!=INVALID_FILEPOS_T && EBML_ElementPositionEnd((EbmlElement*)RSegment)!=EBML_ElementPositionEnd((EbmlElement*)Prev))
			Result |= OutputError(0x42,T("The segment's size %") TPRId64 T(" doesn't match the position where it ends %") TPRId64,EBML_ElementPositionEnd((EbmlElement*)RSegment),EBML_ElementPositionEnd((EbmlElement*)Prev));
	}

	if (!RSeekHead)
    {
        if (!Live)
		    OutputWarning(0x801,T("The segment has no SeekHead section"));
    }
	else
		Result |= CheckSeekHead(*reinterpret_cast<KaxSegment*>(RSegment), RSeekHead);
	if (RSeekHead2)
		Result |= CheckSeekHead(*reinterpret_cast<KaxSegment*>(RSegment), RSeekHead2);

	if (ARRAYCOUNT(RClusters,EbmlElement*))
	{
        if (!Quiet) fprintf(stderr,T("."));
		LinkClusterBlocks(MatroskaProfile);

        if (HasVideo)
            Result |= CheckVideoStart(MatroskaProfile);
        Result |= CheckLacingKeyframe(MatroskaProfile);
        Result |= CheckPosSize((EbmlElement*)RSegment);
		if (!RCues)
        {
            if (!Live && ARRAYCOUNT(RClusters,EbmlElement*)>1)
			    OutputWarning(0x800,T("The segment has Clusters but no Cues section (bad for seeking)"));
        }
		else
			Result |= CheckCueEntries(RCues);
		if (!RTrackInfo)
		{
			Result = OutputError(0x41,T("The segment has Clusters but no TrackInfo section"));
			goto exit;
		}
	}

    if (!Quiet) fprintf(stderr,T("."));
	if (RTrackInfo)
		CheckTracks(RTrackInfo, MatroskaProfile);

    for (TI=ARRAYBEGIN(Tracks,track_info); TI!=ARRAYEND(Tracks,track_info); ++TI)
    {
        if (TI->DataLength==0)
            OutputWarning(0xB8,T("Track #%d is defined but has no frame"),TI->Num);
    }

	if (VoidAmount > 4*1024)
		OutputWarning(0xD0,T("There are %") TPRId64 T(" bytes of void data\r\n"),VoidAmount);

	if (!Quiet && Result==0)
    {
        fprintf(stderr,T("\r") PROJECT_NAME T(" ") PROJECT_VERSION T(": the file appears to be valid\r\n"));
        if (Details)
        {
            for (TI=ARRAYBEGIN(Tracks,track_info); TI!=ARRAYEND(Tracks,track_info); ++TI)
            {
                EBML_StringGet(TI->CodecID,String,TSIZEOF(String));
                fprintf(stderr,T("Track #%d %18s %") TPRId64 T(" bits/s\r\n"),TI->Num,String,Scale64(TI->DataLength,8000000, (MaxTime-MinTime)/1000));
            }
        }
    }

exit:
	if (!Quiet)
	{
        fprintf(stderr, T("\r\tfile \"%s\"\r\n"), Path);
        if (RSegmentInfo)
        {
            tchar_t App[MAXPATH];
            App[0] = 0;
            LibName = (EbmlString*)EBML_MasterFindChild(RSegmentInfo, KaxMuxingApp);
            AppName = (EbmlString*)EBML_MasterFindChild(RSegmentInfo, KaxWritingApp);
            if (LibName)
            {
                EBML_StringGet(LibName, String, TSIZEOF(String));
                tcscat_s(App, TSIZEOF(App), String);
            }
            if (AppName)
            {
                EBML_StringGet(AppName, String, TSIZEOF(String));
                if (App[0])
                    tcscat_s(App, TSIZEOF(App), T(" / "));
                tcscat_s(App, TSIZEOF(App), String);
            }
            if (App[0] == 0)
                tcscat_s(App, TSIZEOF(App), T("<unknown>"));
            fprintf(stderr, T("\r\tcreated with %s\r\n"), App);
        }
	}

    for (Cluster = ARRAYBEGIN(RClusters,EbmlMaster*);Cluster != ARRAYEND(RClusters,EbmlMaster*); ++Cluster)
        NodeDelete(*Cluster);
    ArrayClear(&RClusters);
    if (RAttachments)
        NodeDelete(RAttachments);
    if (RTags)
        NodeDelete(RTags);
    if (RCues)
        NodeDelete(RCues);
    if (RChapters)
        NodeDelete(RChapters);
    if (RTrackInfo)
        NodeDelete(RTrackInfo);
    if (RSegmentInfo)
        NodeDelete(RSegmentInfo);
    if (RLevel1)
        NodeDelete(RLevel1);
    if (RSegment)
        NodeDelete(RSegment);
    if (EbmlHead)
        NodeDelete(EbmlHead);
    ArrayClear(&Tracks);
	stream.reset();
	delete Input;

#if 0
    // EBML & Matroska ending
    MATROSKA_Done(&p);

    // Core-C ending
    ParserContext_Done(&p);
#endif

    return Result;
}
