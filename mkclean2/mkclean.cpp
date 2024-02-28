// Copyright © 2010-2024 Matroska (non-profit organisation).
// SPDX-License-Identifier: BSD-3-Clause

// TODO remove
#include "../mkclean2/porting.h"

#define EBML2_UGLY_HACKS_API // we force some values in the elements we read
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
#include <matroska/KaxVersion.h>
#include "mkclean2_project.h"

#include <cstdio>
#include <cinttypes>
#include <sstream>

// #ifndef CONFIG_EBML_WRITING
// #error libebml2 was not built with writing support!
// #endif

using namespace libmatroska;
using namespace libebml;

static bool QuickExit = false;

// some macros for code readability
#define EL_Pos(elt)         EBML_ElementPosition(elt)

bool_t EBML_MasterUseChecksum(ebml_master *Element, bool_t Use)
{
#if 1
    Element->EnableChecksum(Use);
    return true;
#else
    if (Use && Element->CheckSumStatus==0)
    {
        Element->Base.bNeedDataSizeUpdate = 1;
        Element->CheckSumStatus = 1;
        return 1;
    }
    if (!Use && Element->CheckSumStatus)
    {
        Element->Base.bNeedDataSizeUpdate = 1;
        Element->CheckSumStatus = 0;
        return 1;
    }
    return 0;
#endif
}

/*!
 * \todo write the CRC-32 on Clusters too (make it faster in libebml2)
 * \todo discards tracks that has the same UID
 * \todo remuxing: turn a BlockGroup into a SimpleBlock in v2 profiles and when it makes sense (duration = default track duration) (optimize mode)
 * \todo error when an unknown codec (for the profile) is found (option to turn into a warning) (loose mode)
 * \todo compute the segment duration based on audio (when it's not set)
 * \todo remuxing: repack audio frames using lacing (no longer than the matching video frame ?) (optimize mode)
 * \todo compute the track default duration (when it's not set or not optimal) (optimize mode)
 * \todo add a batch mode to treat more than one file at once
 * \todo get the file name/list to treat from stdin too
 * \todo add an option to remove the original file
 * \todo add an option to rename the output to the original file
 * \todo add an option to show the size gained/added compared to the original
 * \todo support the japanese translation
 *
 * less important:
 * \todo (optionally) change the Segment UID (when key parts are altered/added)
 * \todo force keeping some forbidden elements in a profile (chapters/tags in 'webm') (loose mode)
 * \todo add an option to cut short a file with a start/end
 * \todo allow creating/replacing Tags
 * \todo allow creating/replacing Chapters
 * \todo allow creating/replacing Attachments
 */

#ifdef TARGET_WIN
#include <windows.h>
void DebugMessage(const tchar_t* Msg,...)
{
#if !defined(NDEBUG) || defined(LOGFILE) || defined(LOGTIME)
	va_list Args;
	tchar_t Buffer[1024],*s=Buffer;

	va_start(Args,Msg);
	vstprintf_s(Buffer,TSIZEOF(Buffer), Msg, Args);
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

#define EXTRA_SEEK_SPACE       22
#define MARKER3D         (block_info*)1

typedef struct block_info
{
	matroska_block *Block;
	mkv_timestamp_t DecodeTime;
	size_t FrameStartIndex;

} block_info;

#define TABLE_MARKER (uint8_t*)1

typedef struct track_info
{
	bool_t IsLaced;
//    bool_t NeedsReading;

} track_info;

using array = std::vector<libebml::EbmlElement>; // TODO use reference or pointer of shared_ptr
using array_ebml_element_p = std::vector<ebml_element*>; // TODO use reference or pointer of shared_ptr
using array_size_t = std::vector<std::size_t>;
using array_block_info_p = std::vector<block_info*>;
using array_block_info = std::vector<block_info>;
using array_array_block_info = std::vector<array_block_info>;
using array_binary = std::vector<binary>;
// using array_array_binary = std::vector<array_binary>;
using array_array_binary = array_binary;
using array_track_info = std::vector<track_info>;
using array_matroska_cluster_p = std::vector<matroska_cluster*>;


static bool WriteWebM(const EbmlElement & Elt)
{
    const auto & profile = Elt.ElementSpec().GetVersions();
    if (profile.GetNameSpace() == MatroskaProfile::Namespace)
    {
        const auto & profiles = reinterpret_cast<const MatroskaProfile &>(profile);
        if (!profiles.InWebM)
            return false;
    }
    return EbmlElement::WriteSkipDefault(Elt);
}
static const EbmlElement::ShouldWrite ProfileWebM = WriteWebM;

static bool WriteDivX(const EbmlElement & Elt)
{
    const auto & profile = Elt.ElementSpec().GetVersions();
    if (profile.GetNameSpace() == MatroskaProfile::Namespace)
    {
        const auto & profiles = reinterpret_cast<const MatroskaProfile &>(profile);
        if (!profiles.InDivX)
            return false;
    }
    return EbmlElement::WriteSkipDefault(Elt);
}
static const EbmlElement::ShouldWrite ProfileDiVX = WriteDivX;

template<EbmlDocVersion::version_type N>
static bool WriteProfile(const EbmlElement & Elt)
{
    const auto & semcb = Elt.ElementSpec();
    const auto & profiles = semcb.GetVersions();
    if (!profiles.IsValidInVersion(N))
        return false;
    return EbmlElement::WriteSkipDefault(Elt);
}
static const EbmlElement::ShouldWrite Profile1 = WriteProfile<1>;
static const EbmlElement::ShouldWrite Profile2 = WriteProfile<2>;
static const EbmlElement::ShouldWrite Profile3 = WriteProfile<3>;
static const EbmlElement::ShouldWrite Profile4 = WriteProfile<4>;
static const EbmlElement::ShouldWrite Profile5 = WriteProfile<5>;

static int GetProfileId(const EbmlElement::ShouldWrite & ProfileNum)
{
    if (&ProfileNum != &ProfileWebM)
        return 4;
    if (&ProfileNum != &ProfileDiVX)
        return 5;
	if (&ProfileNum == &Profile1)
        return 1;
	if (&ProfileNum == &Profile2)
        return 2;
	if (&ProfileNum == &Profile3)
        return 3;
	if (&ProfileNum == &Profile4)
        return 6;
	if (&ProfileNum == &Profile5)
        return 7;
	return 0;
}

static const EbmlElement::ShouldWrite & ProfileFrom(int Profile)
{
	switch (Profile)
	{
    default:
	case PROFILE_MATROSKA_V1: return Profile1;
	case PROFILE_MATROSKA_V2: return Profile2;
	case PROFILE_MATROSKA_V3: return Profile3;
	case PROFILE_WEBM:        return ProfileWebM;
	case PROFILE_DIVX:        return ProfileDiVX;
	case PROFILE_MATROSKA_V4: return Profile4;
	case PROFILE_MATROSKA_V5: return Profile5;
	}
}

static int GetProfileId(int Profile)
{
	switch (Profile)
	{
	default:                  return 0;
	case PROFILE_MATROSKA_V1: return 1;
	case PROFILE_MATROSKA_V2: return 2;
	case PROFILE_MATROSKA_V3: return 3;
	case PROFILE_WEBM:        return 4;
	case PROFILE_DIVX:        return 5;
	case PROFILE_MATROSKA_V4: return 6;
	case PROFILE_MATROSKA_V5: return 7;
	}
}

static const tchar_t *GetProfileName(int ProfileNum)
{
static const tchar_t *Profile[8] = {T("unknown"), T("matroska v1"), T("matroska v2"), T("matroska v3"), T("webm"), T("matroska+DivX"), T("matroska v4"), T("matroska v5")};
	return Profile[ProfileNum];
}

static const tchar_t *GetProfileName(const EbmlElement::ShouldWrite & ProfileNum)
{
	return GetProfileName(GetProfileId(ProfileNum));
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

struct profile_check
{
    int *Result;
    const EbmlElement *Parent;
    const tchar_t *EltName;
    const EbmlElement::ShouldWrite & ProfileMask;
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

static int DocVersion = 1;
static int SrcProfile(0), DstProfile(0);
// static textwriter *StdErr = NULL;
static size_t ExtraSizeDiff = 0;
static bool_t Quiet = 0;
static bool_t Unsafe = 0;
static bool_t Live = 0;
static int TotalPhases = 2;
static int CurrentPhase = 1;

static bool_t MasterError(void *cookie, int type, const tchar_t *ClassName, const ebml_element *i)
{
	tchar_t IdString[MAXPATH];
    if (type==MASTER_CHECK_PROFILE_INVALID)
    {
    	EBML_ElementGetName(i,IdString,TSIZEOF(IdString));
        if (!Quiet)
            TextPrintf(StdErr,T("The %s element at %") TPRId64 T(" is not part of profile '%s', skipping\r\n"),IdString,EBML_ElementPosition(i),GetProfileName(DstProfile));
    }
    else if (type==MASTER_CHECK_MULTIPLE_UNIQUE)
    {
    	EBML_ElementGetName(i,IdString,TSIZEOF(IdString));
        if (!Quiet)
            TextPrintf(StdErr,T("The %s element at %") TPRId64 T(" has multiple versions of the unique element %s, skipping\r\n"),IdString,EBML_ElementPosition(i),ClassName);
    }
    else if (type==MASTER_CHECK_MISSING_MANDATORY)
    {
    	EBML_ElementGetName(static_cast<EbmlElement*>(cookie),IdString,TSIZEOF(IdString));
        if (!Quiet)
    	    TextPrintf(StdErr,T("The %s element at %") TPRId64 T(" is missing mandatory element %s\r\n"),IdString,EBML_ElementPosition(static_cast<EbmlElement*>(cookie)),ClassName);
    }
    return 1;
}

static void ReduceSize(ebml_element *Element)
{
    EBML_ElementSetSizeLength(Element, 0); // reset
    // if (Node_IsPartOf(Element,EBML_MASTER_CLASS))
    if (Element->IsMaster())
    {
        EBML_MASTER_ITERATOR i, j;

        if (Unsafe)
            EBML_MasterUseChecksum((ebml_master*)Element,0);

        if (EBML_ElementIsType(Element, KaxBlockGroup) && !EBML_MasterCheckMandatory((ebml_master*)Element, 0, ProfileFrom(DstProfile)))
        {
            NodeDelete(Element);
            return;
        }

        for (i=EBML_MasterChildren((ebml_master*)Element);EBML_MasterEnd(i,(ebml_master*)Element);i=j)
		{
			j = EBML_MasterNext(i);
            if (EBML_ElementIsType(*i,EbmlVoid))
			// if (Node_IsPartOf(i, EBML_VOID_CLASS))
			{
				NodeDelete(*i);
				continue;
			}
			// else if (Node_IsPartOf(i, EBML_DUMMY_ID))
			else if ((*i)->IsDummy())
			{
				NodeDelete(*i);
				continue;
			}
            if (EBML_ElementIsType(*i,EbmlCrc32))
			// else if (EBML_ElementIsType(i, EBML_getContextEbmlCrc32()))
			{
				NodeDelete(*i);
				continue;
			}
            ReduceSize(*i);
		}

        // if (!EBML_MasterChildren((ebml_master*)Element) && !EBML_MasterCheckMandatory((ebml_master*)Element, 0, DstProfile))
        if (EBML_MasterEmpty((ebml_master*)Element) && !EBML_MasterCheckMandatory((ebml_master*)Element, 0, ProfileFrom(DstProfile)))
        {
            NodeDelete(Element);
            return;
        }

        // EBML_MasterAddMandatory((ebml_master*)Element,1, DstProfile);
        ((ebml_master*)Element)->ProcessMandatory();

        int Result = 0 ;
    	tchar_t String[MAXPATH];
        struct profile_check Checker{&Result, Element, String, ProfileFrom(DstProfile)};
    // int *Result;
    // const EbmlElement *Parent;
    // const tchar_t *EltName;
    // const EbmlElement::ShouldWrite & ProfileMask;
    //     Checker.EltName = String;
    //     Checker.ProfileMask = ProfileMask;
    //     Checker.Parent = Elt;
    //     Checker.Result = &Result;
        EBML_MasterCheckContext((ebml_master*)Element, GetProfileId(DstProfile), ProfileCallback, &Checker);
        // EBML_MasterCheckContext((ebml_master*)Element, DstProfile, MasterError, Element);
	}
}

static bool_t ReadClusterData(matroska_cluster *Cluster, stream *Input)
{
    bool_t Changed = 0;
    filepos_t Result = ERR_NONE;
    EBML_MASTER_ITERATOR Block, GBlock, NextBlock;
    // read all the Block/SimpleBlock data
    for (Block = EBML_MasterChildren(Cluster);EBML_MasterEnd(Block,Cluster);Block=NextBlock)
    {
        NextBlock = EBML_MasterNext(Block);
        if (EBML_ElementIsType(*Block, KaxBlockGroup))
        {
            for (auto GBlock = EBML_MasterChildren((KaxBlockGroup*)*Block);EBML_MasterEnd(GBlock,(KaxBlockGroup*)*Block);GBlock=EBML_MasterNext(GBlock))
            {
                if (EBML_ElementIsType(*GBlock, KaxBlock))
                {
                    if ((Result = MATROSKA_BlockReadData((matroska_block*)*GBlock, Input, SrcProfile))!=ERR_NONE)
                    {
                        Changed = 1;
                        NodeDelete(*Block);
                    }
                    break;
                }
            }
        }
        else if (EBML_ElementIsType(*Block, KaxSimpleBlock))
        {
            if ((Result = MATROSKA_BlockReadData((matroska_block*)*Block, Input, SrcProfile))!=ERR_NONE)
            {
                Changed = 1;
                NodeDelete(*Block);
            }
        }
    }
    return Changed;
}

static filepos_t UnReadClusterData(ebml_master *Cluster, bool_t IncludingNotRead)
{
    filepos_t Result = ERR_NONE;
    EBML_MASTER_CONST_ITERATOR Block, GBlock;
    for (Block = EBML_MasterChildren(Cluster);Result==ERR_NONE && EBML_MasterEnd(Block,Cluster);Block=EBML_MasterNext(Block))
    {
        if (EBML_ElementIsType(*Block, KaxBlockGroup))
        {
            for (GBlock = EBML_MasterChildren((KaxBlockGroup*)*Block);EBML_MasterEnd(GBlock,(KaxBlockGroup*)*Block);GBlock=EBML_MasterNext(GBlock))
            {
                if (EBML_ElementIsType(*GBlock, KaxBlock))
                {
                    Result = MATROSKA_BlockReleaseData((matroska_block*)*GBlock,IncludingNotRead);
                    break;
                }
            }
        }
        else if (EBML_ElementIsType(*Block, KaxSimpleBlock))
            Result = MATROSKA_BlockReleaseData((matroska_block*)*Block,IncludingNotRead);
    }
    return Result;
}

static void SetClusterPrevSize(array_matroska_cluster_p Clusters, stream *Input)
{
    array::iterator Cluster;
    ebml_element *Elt, *Elt2;
    filepos_t ClusterSize = INVALID_FILEPOS_T;

    // Write the Cluster PrevSize
    for (auto Cluster = ARRAYBEGIN(Clusters,ebml_master*);Cluster != ARRAYEND(Clusters,ebml_master*); ++Cluster)
    {
        if (Input!=NULL)
            ReadClusterData(*Cluster,Input);

        if (ClusterSize != INVALID_FILEPOS_T)
        {
            Elt = EBML_MasterGetChild(&reinterpret_cast<ebml_master &>(*Cluster), KaxClusterPrevSize, DstProfile);
            if (Elt)
            {
                EBML_IntegerSetValue((ebml_integer*)Elt, ClusterSize);
                Elt2 = EBML_MasterFindChild(&reinterpret_cast<ebml_master &>(*Cluster), KaxClusterTimestamp);
                if (Elt2)
                    NodeTree_SetParent(Elt,&reinterpret_cast<ebml_master &>(*Cluster),NodeTree_Next(Elt2));
                ExtraSizeDiff += (size_t)EBML_ElementFullSize(Elt,0);
                EBML_ElementUpdateSize(&reinterpret_cast<ebml_master &>(*Cluster),0,1,ProfileFrom(DstProfile));
            }
        } else EBML_ElementUpdateSize(&reinterpret_cast<ebml_master &>(*Cluster),0,1, ProfileFrom(DstProfile));
        EBML_ElementSetInfiniteSize(*Cluster,Live);
        ClusterSize = EBML_ElementFullSize(*Cluster,0);

        if (Input!=NULL)
            UnReadClusterData(&reinterpret_cast<ebml_master &>(*Cluster), 0);
    }
}

static void UpdateCues(ebml_master *Cues, ebml_master *Segment)
{
    EBML_MASTER_ITERATOR Cue,NextCue;

    // reevaluate the size needed for the Cues
    for (Cue=EBML_MasterChildren(Cues);EBML_MasterEnd(Cue,Cues);Cue=NextCue)
    {
        NextCue =EBML_MasterNext(Cue);
        if (MATROSKA_CuePointUpdate((matroska_cuepoint*)*Cue, (ebml_element*)Segment, ProfileFrom(DstProfile))!=ERR_NONE)
        {
            EBML_MasterRemove(Cues,Cue); // make sure it doesn't remain in the list
            NodeDelete(*Cue);
        }
    }
}

static void SettleClustersWithCues(array_matroska_cluster_p Clusters, filepos_t ClusterStart, ebml_master *Cues, ebml_master *Segment, bool_t SafeClusters, stream *Input)
{
    ebml_element *Elt, *Elt2;
    // decltype(Clusters)::iterator Cluster;
    filepos_t OriginalSize = EBML_ElementDataSize((ebml_element*)Cues,0);
    filepos_t ClusterPos = ClusterStart + EBML_ElementFullSize((ebml_element*)Cues,0);
    filepos_t ClusterSize = INVALID_FILEPOS_T;

    // reposition all the Clusters
    for (auto Cluster=ARRAYBEGIN(Clusters,ebml_master*);Cluster!=ARRAYEND(Clusters,ebml_master*);++Cluster)
    {
        if (Input!=NULL)
            ReadClusterData(*Cluster,Input);

        EBML_ElementForcePosition((ebml_element*)&(*Cluster), ClusterPos);
        if (SafeClusters)
        {
            Elt = NULL;
            if (ClusterSize != INVALID_FILEPOS_T)
            {
                Elt = EBML_MasterGetChild(&(*Cluster), KaxClusterPrevSize, DstProfile);
                if (Elt)
                {
                    EBML_IntegerSetValue((ebml_integer*)Elt, ClusterSize);
                    Elt2 = EBML_MasterFindChild(&(*Cluster), KaxClusterTimestamp);
                    if (Elt2)
                        NodeTree_SetParent(Elt,*Cluster,NodeTree_Next(Elt2)); // make sure the PrevSize is just after the ClusterTimestamp
                    ExtraSizeDiff += (size_t)EBML_ElementFullSize(Elt,0);
                }
            }
        }
        EBML_ElementUpdateSize(*Cluster,0,0, ProfileFrom(DstProfile));
        ClusterSize = EBML_ElementFullSize((ebml_element*)&(*Cluster),0);
        ClusterPos += ClusterSize;

        if (Input!=NULL)
            UnReadClusterData(&reinterpret_cast<ebml_master &>(*Cluster), 0);
    }

    UpdateCues(Cues, Segment);

    ClusterPos = EBML_ElementUpdateSize(Cues,0,0, ProfileFrom(DstProfile));
    if (ClusterPos != OriginalSize)
        SettleClustersWithCues(Clusters,ClusterStart,Cues,Segment, SafeClusters, Input);

}

static void ShowProgress(const ebml_element *RCluster, filepos_t TotalSize)
{
    if (!Quiet)
        TextPrintf(StdErr,T("Progress %d/%d: %3d%%\r"), CurrentPhase, TotalPhases,Scale32(100,EBML_ElementPosition(RCluster),TotalSize)+1);
}

static void EndProgress()
{
    if (!Quiet)
        TextPrintf(StdErr,T("Progress %d/%d: 100%%\r\n"), CurrentPhase, TotalPhases);
}

static array_matroska_cluster_p::iterator LinkCueCluster(matroska_cuepoint *Cue, libmatroska::KaxInfo *SegmentInfo, array_matroska_cluster_p *Clusters, array_matroska_cluster_p::iterator StartCluster, filepos_t TotalSize)
{
    matroska_block *Block;
    int16_t CueTrack;
    mkv_timestamp_t CueTimestamp;
    size_t StartBoost = 7;

    CueTrack = MATROSKA_CueTrackNum(Cue);
    CueTimestamp = MATROSKA_CueTimestamp(Cue, SegmentInfo);
    ++CurrentPhase;
    if (StartCluster != ARRAYEND(*Clusters,matroska_cluster*))
    {
        for (auto Cluster=StartCluster;StartBoost && Cluster!=ARRAYEND(*Clusters,matroska_cluster*);++Cluster,--StartBoost)
        {
            Block = MATROSKA_GetBlockForTimestamp((matroska_cluster*)&(*Cluster), CueTimestamp, CueTrack);
            if (Block)
            {
                MATROSKA_LinkCuePointBlock(Cue,Block);
                ShowProgress((ebml_element*)&(*Cluster),TotalSize);
                return Cluster;
            }
        }
    }

    for (auto Cluster=ARRAYBEGIN(*Clusters,matroska_cluster*);Cluster!=ARRAYEND(*Clusters,matroska_cluster*);++Cluster)
    {
        Block = MATROSKA_GetBlockForTimestamp((matroska_cluster*)&(*Cluster), CueTimestamp, CueTrack);
        if (Block)
        {
            MATROSKA_LinkCuePointBlock(Cue,Block);
            ShowProgress((ebml_element*)&(*Cluster),TotalSize);
            return Cluster;
        }
    }

    TextPrintf(StdErr,T("Could not find the matching block for timestamp %0.3f s\r\n"),CueTimestamp/1000000000.0);
    return ARRAYEND(*Clusters,matroska_cluster*);
}

static int LinkClusters(array_matroska_cluster_p *Clusters, ebml_master *RSegmentInfo, ebml_master *Tracks, int dstProfile, array_track_info *WTracks, mkv_timestamp_t Offset)
{
	EBML_MASTER_CONST_ITERATOR Block;
    ebml_element *GBlock, *BlockTrack, *Type;
    ebml_integer *Time;
    int BlockNum;

	// find out if the Clusters use forbidden features for that dstProfile
	if (dstProfile == PROFILE_MATROSKA_V1 || dstProfile == PROFILE_DIVX)
	{
		for (auto Cluster=ARRAYBEGIN(*Clusters,matroska_cluster*);Cluster!=ARRAYEND(*Clusters,matroska_cluster*);++Cluster)
		{
			for (Block = EBML_MasterChildren(*Cluster);EBML_MasterEnd(Block,*Cluster);Block=EBML_MasterNext(Block))
			{
				if (EBML_ElementIsType(*Block, KaxSimpleBlock))
                {
					TextPrintf(StdErr,T("Using SimpleBlock in profile '%s' at %") TPRId64 T(" try \"--doctype %d\"\r\n"),GetProfileName(dstProfile),EBML_ElementPosition(*Block),GetProfileId(PROFILE_MATROSKA_V2));
					return -32;
				}
			}
		}
	}

	// link each Block/SimpleBlock with its Track and SegmentInfo
	for (auto Cluster=ARRAYBEGIN(*Clusters,matroska_cluster*);Cluster!=ARRAYEND(*Clusters,matroska_cluster*);++Cluster)
	{
        if (Offset != INVALID_TIMESTAMP_T)
        {
            Time = EBML_MasterGetChild(*Cluster, KaxClusterTimestamp, DstProfile);
            if (Time)
                EBML_IntegerSetValue(Time, Offset + EBML_IntegerValue(Time));
        }
		MATROSKA_LinkClusterBlocks(*Cluster, RSegmentInfo, Tracks, 0, GetProfileId(DstProfile));
		ReduceSize((ebml_element*)&(*Cluster));
	}

    // mark all the audio/subtitle tracks as keyframes
	for (auto Cluster=ARRAYBEGIN(*Clusters,matroska_cluster*);Cluster!=ARRAYEND(*Clusters,matroska_cluster*);++Cluster)
	{
		for (Block = EBML_MasterChildren(static_cast<EbmlMaster*>(*Cluster));EBML_MasterEnd(Block,static_cast<EbmlMaster*>(*Cluster));Block=EBML_MasterNext(Block))
		{
			if (EBML_ElementIsType(*Block, KaxBlockGroup))
			{
				GBlock = EBML_MasterFindChild((KaxBlockGroup*)(*Block), KaxBlock);
				if (GBlock)
				{
					BlockTrack = MATROSKA_BlockReadTrack((matroska_block*)&(*GBlock));
                    if (!BlockTrack) continue;
                    Type = EBML_MasterFindChild((ebml_master*)BlockTrack,KaxTrackType);
                    if (!Type) continue;
                    if (EBML_IntegerValue((ebml_integer*)Type)==MATROSKA_TRACK_TYPE_AUDIO || EBML_IntegerValue((ebml_integer*)Type)==MATROSKA_TRACK_TYPE_SUBTITLE)
                    {
                        MATROSKA_BlockSetKeyframe((matroska_block*)&(*GBlock),1);
						MATROSKA_BlockSetDiscardable((matroska_block*)&(*GBlock),0);
                    }
                    BlockNum = MATROSKA_BlockTrackNum((matroska_block*)&(*GBlock));
                    if (MATROSKA_BlockGetFrameCount((matroska_block*)&(*GBlock))>1)
                        ARRAYBEGIN(*WTracks,track_info)[BlockNum].IsLaced = 1;
				}
			}
			else if (EBML_ElementIsType(*Block, KaxSimpleBlock))
			{
				BlockTrack = MATROSKA_BlockReadTrack((matroska_block*)&(*Block));
                if (!BlockTrack) continue;
                Type = EBML_MasterFindChild((ebml_master*)BlockTrack,KaxTrackType);
                if (!Type) continue;
                if (EBML_IntegerValue((ebml_integer*)Type)==MATROSKA_TRACK_TYPE_AUDIO || EBML_IntegerValue((ebml_integer*)Type)==MATROSKA_TRACK_TYPE_SUBTITLE)
                {
                    MATROSKA_BlockSetKeyframe((matroska_block*)&(*Block),1);
                    MATROSKA_BlockSetDiscardable((matroska_block*)&(*Block),0);
                }
                BlockNum = MATROSKA_BlockTrackNum((matroska_block*)&(*Block));
                if (MATROSKA_BlockGetFrameCount((matroska_block*)&(*Block))>1)
                    ARRAYBEGIN(*WTracks,track_info)[BlockNum].IsLaced = 1;
			}
		}
    }
	return 0;
}

static void OptimizeCues(ebml_master *Cues, array_matroska_cluster_p *Clusters, KaxInfo *RSegmentInfo, filepos_t StartPos, ebml_master *WSegment, filepos_t TotalSize, bool_t ReLink, bool_t SafeClusters, stream *Input)
{
    array_matroska_cluster_p::iterator Cluster;
    EBML_MASTER_ITERATOR Cue;

    ReduceSize((ebml_element*)Cues);

	if (ReLink)
	{
		// link each Cue entry to the segment
		for (Cue = EBML_MasterChildren(Cues);EBML_MasterEnd(Cue,Cues);Cue=EBML_MasterNext(Cue))
			MATROSKA_LinkCueSegmentInfo(static_cast<matroska_cuepoint*>(*Cue),RSegmentInfo);

		// link each Cue entry to the corresponding Block/SimpleBlock in the Cluster
		Cluster = ARRAYEND(*Clusters,matroska_cluster*);
		for (Cue = EBML_MasterChildren(Cues);EBML_MasterEnd(Cue,Cues);Cue=EBML_MasterNext(Cue))
			Cluster = LinkCueCluster(static_cast<matroska_cuepoint*>(*Cue),RSegmentInfo,Clusters,Cluster,TotalSize);
		EndProgress();
	}

    // sort the Cues
    MATROSKA_CuesSort(Cues);

    SettleClustersWithCues(*Clusters,StartPos,Cues,WSegment,SafeClusters, Input);
}

// static ebml_element *CheckMatroskaHead(const ebml_element *Head, const ebml_parser_context *Parser, stream *Input)
static ebml_element *CheckMatroskaHead(const ebml_element *Head, EbmlStream *Input)
{
    // ebml_parser_context SubContext;
    ebml_element *SubElement;
    int UpperElement=0;
    tchar_t String[MAXLINE];

#if 1
    SubElement = (ebml_master*)EBML_FindNextElement(Input, EBML_CLASS_CONTEXT(EbmlHead), &UpperElement, 0);
#else
    SubContext.UpContext = Parser;
    SubContext.Context = EBML_ElementContext(Head);
    SubContext.EndPosition = EBML_ElementPositionEnd(Head);
    SubContext.Profile = Parser->Profile;
    SubElement = EBML_FindNextElement(Input, &SubContext, &UpperElement, 1);
#endif
    while (SubElement)
    {
        if (EBML_ElementIsType(SubElement, EBML_getContextReadVersion()))
        {
            if (EBML_ElementReadData(SubElement,Input,NULL,0,SCOPE_ALL_DATA,0)!=ERR_NONE)
            {
                TextPrintf(StdErr,T("Error reading\r\n"));
                break;
            }
            else if (EBML_IntegerValue((ebml_integer*)SubElement) > EBML_MAX_VERSION)
            {
                TextPrintf(StdErr,T("EBML Read version %") TPRId64 T(" not supported\r\n"),EBML_IntegerValue((ebml_integer*)SubElement));
                break;
            }
        }
        else if (EBML_ElementIsType(SubElement, EBML_getContextMaxIdLength()))
        {
            if (EBML_ElementReadData(SubElement,Input,NULL,0,SCOPE_ALL_DATA,0)!=ERR_NONE)
            {
                TextPrintf(StdErr,T("Error reading\r\n"));
                break;
            }
            else if (EBML_IntegerValue((ebml_integer*)SubElement) > EBML_MAX_ID)
            {
                TextPrintf(StdErr,T("EBML Max ID Length %") TPRId64 T(" not supported\r\n"),EBML_IntegerValue((ebml_integer*)SubElement));
                break;
            }
        }
        else if (EBML_ElementIsType(SubElement, EBML_getContextMaxSizeLength()))
        {
            if (EBML_ElementReadData(SubElement,Input,NULL,0,SCOPE_ALL_DATA,0)!=ERR_NONE)
            {
                TextPrintf(StdErr,T("Error reading\r\n"));
                break;
            }
            else if (EBML_IntegerValue((ebml_integer*)SubElement) > EBML_MAX_SIZE)
            {
                TextPrintf(StdErr,T("EBML Max Coded Size %") TPRId64 T(" not supported\r\n"),EBML_IntegerValue((ebml_integer*)SubElement));
                break;
            }
        }
        else if (EBML_ElementIsType(SubElement, EBML_getContextDocType()))
        {
            if (EBML_ElementReadData(SubElement,Input,NULL,0,SCOPE_ALL_DATA,0)!=ERR_NONE)
            {
                TextPrintf(StdErr,T("Error reading\r\n"));
                break;
            }
            else
            {
                EBML_StringGet((ebml_string*)SubElement,String,TSIZEOF(String));
                if (tcscmp(String,T("matroska"))==0)
                    SrcProfile = PROFILE_MATROSKA_V1;
                else if (tcscmp(String,T("webm"))==0)
                    SrcProfile = PROFILE_WEBM;
                else
                {
                    TextPrintf(StdErr,T("EBML DocType '%s' not supported\r\n"),String);
                    break;
                }
            }
        }
        else if (EBML_ElementIsType(SubElement, EBML_getContextDocTypeReadVersion()))
        {
            if (EBML_ElementReadData(SubElement,Input,NULL,0,SCOPE_ALL_DATA,0)!=ERR_NONE)
            {
                TextPrintf(StdErr,T("Error reading\r\n"));
                break;
            }
            else if (EBML_IntegerValue((ebml_integer*)SubElement) > MATROSKA_MAX_VERSION)
            {
                TextPrintf(StdErr,T("EBML Read version %") TPRId64 T(" not supported\r\n"),EBML_IntegerValue((ebml_integer*)SubElement));
                break;
            }
            else
                DocVersion = (int)EBML_IntegerValue((ebml_integer*)SubElement);
        }
        else if (EBML_ElementIsType(SubElement, KaxSegment))
            return SubElement;
        else
            EBML_ElementSkipData(SubElement,*Input,EBML_CLASS_CONTEXT(EbmlHead),NULL,0);
        NodeDelete(SubElement);
#if 1
        SubElement = EBML_FindNextElement(Input, EBML_CLASS_CONTEXT(EbmlHead), &UpperElement, 1);
#else
        SubElement = EBML_FindNextElement(Input, &SubContext, &UpperElement, 1);
#endif
    }

    return NULL;
}

static bool_t WriteCluster(matroska_cluster *Cluster, stream *Output, stream *Input, filepos_t PrevSize, mkv_timestamp_t *PrevTimestamp)
{
    filepos_t IntendedPosition = EBML_ElementPosition((ebml_element*)Cluster);
    ebml_element *Elt;
    bool_t CuesChanged = ReadClusterData(Cluster, Input);

    if (*PrevTimestamp != INVALID_TIMESTAMP_T)
    {
        mkv_timestamp_t OrigTimestamp = MATROSKA_ClusterTimestamp((matroska_cluster*)Cluster);
        if (*PrevTimestamp >= OrigTimestamp)
        {
            if (!Quiet)
                TextPrintf(StdErr,T("The Cluster at position %") TPRId64 T(" has the same timestamp %") TPRId64 T(" as the previous cluster %") TPRId64 T(", incrementing\r\n"), EBML_ElementPosition((ebml_element*)Cluster),*PrevTimestamp,OrigTimestamp);
            MATROSKA_ClusterSetTimestamp((matroska_cluster*)Cluster, *PrevTimestamp + MATROSKA_ClusterTimestampScale((matroska_cluster*)Cluster, 0));
            CuesChanged = 1;
        }
    }
    *PrevTimestamp = MATROSKA_ClusterTimestamp((matroska_cluster*)Cluster);

    EBML_ElementRender((ebml_element*)Cluster,Output,0,0,1,ProfileFrom(DstProfile),NULL);

    UnReadClusterData(Cluster, 1);

    if (!Live && EBML_ElementPosition((ebml_element*)Cluster) != IntendedPosition)
        TextPrintf(StdErr,T("Failed to write a Cluster at the required position %") TPRId64 T(" vs %") TPRId64 T("\r\n"), EBML_ElementPosition((ebml_element*)Cluster),IntendedPosition);
    if (!Live && PrevSize!=INVALID_FILEPOS_T)
    {
        Elt = EBML_MasterGetChild(Cluster, KaxClusterPrevSize, DstProfile);
        if (Elt && PrevSize!=EBML_IntegerValue((ebml_integer*)Elt))
            TextPrintf(StdErr,T("The PrevSize of the Cluster at the position %") TPRId64 T(" is wrong: %") TPRId64 T(" vs %") TPRId64 T("\r\n"), EBML_ElementPosition((ebml_element*)Cluster),EBML_IntegerValue((ebml_integer*)Elt),PrevSize);
    }
    return CuesChanged;
}

static void MetaSeekUpdate(ebml_master *SeekHead)
{
    EBML_MASTER_CONST_ITERATOR SeekPoint;
    for (SeekPoint=EBML_MasterChildren(SeekHead); EBML_MasterEnd(SeekPoint,SeekHead); SeekPoint=EBML_MasterNext(SeekPoint))
        MATROSKA_MetaSeekUpdate((matroska_seekpoint*)&(*SeekPoint));
    EBML_ElementUpdateSize(SeekHead,0,0, ProfileFrom(DstProfile));
}

static ebml_master *GetMainTrack(ebml_master *Tracks, array_size_t *TrackOrder)
{
	ebml_master *Track;
	ebml_element *Elt;
	int64_t TrackNum = -1;
	array_size_t::iterator order;

	// find the video (first) track
	for (Track = (ebml_master*)EBML_MasterFindChild(Tracks,KaxTrackEntry); Track; Track=EBML_MasterNextChild(Tracks,Track))
	{
		Elt = EBML_MasterFindChild(Track,KaxTrackType);
		if (EBML_IntegerValue((ebml_integer*)Elt) == MATROSKA_TRACK_TYPE_VIDEO)
		{
			TrackNum = EBML_IntegerValue((ebml_integer*)EBML_MasterFindChild(Track,KaxTrackNumber));
			break;
		}
	}

	if (!Track)
	{
		// no video track found, look for an audio track
		for (Track = (ebml_master*)EBML_MasterFindChild(Tracks,KaxTrackEntry); Track; Track=EBML_MasterNextChild(Tracks,Track))
		{
			Elt = EBML_MasterFindChild(Track,KaxTrackType);
			if (EBML_IntegerValue((ebml_integer*)Elt) == MATROSKA_TRACK_TYPE_AUDIO)
			{
				TrackNum = EBML_IntegerValue((ebml_integer*)EBML_MasterFindChild(Track,KaxTrackNumber));
				break;
			}
		}
	}

	if (TrackOrder)
	{
		order = ARRAYBEGIN(*TrackOrder,size_t);

		for (auto Elt = EBML_MasterFindChild(Tracks,KaxTrackEntry); Elt; Elt=EBML_MasterNextChild(Tracks,Elt))
		{
			if (Elt!=(ebml_element*)Track)
				*order++ = (size_t)EBML_IntegerValue((ebml_integer*)EBML_MasterFindChild((ebml_master*)Elt,KaxTrackNumber));
		}

		if (Track)
			*order++ = (size_t)EBML_IntegerValue((ebml_integer*)EBML_MasterFindChild(Track,KaxTrackNumber));
	}

	if (!Track)
		TextPrintf(StdErr,T("Could not find an audio or video track to use as main track"));

	return Track;
}

static bool_t GenerateCueEntries(ebml_master *Cues, array *Clusters, ebml_master *Tracks, ebml_master *WSegmentInfo, ebml_element *RSegment, filepos_t TotalSize)
{
	ebml_master *Track;
	EBML_MASTER_ITERATOR Elt;
	matroska_block *Block;
	array::iterator Cluster;
	matroska_cuepoint *CuePoint;
	int64_t TrackNum;
	mkv_timestamp_t PrevTimestamp = INVALID_TIMESTAMP_T, BlockTimestamp;

	Track = GetMainTrack(Tracks, NULL);
	if (!Track)
	{
		TextPrintf(StdErr,T("Could not generate the Cue entries"));
		return 0;
	}

	ebml_element *eTrackNum = EBML_MasterFindChild(Track,KaxTrackNumber);
	assert(eTrackNum!=NULL);
	if (eTrackNum)
		TrackNum = EBML_IntegerValue((ebml_integer*)eTrackNum);

	// find all the keyframes
    ++CurrentPhase;
	for (auto Cluster = ARRAYBEGIN(*Clusters,ebml_element*);Cluster != ARRAYEND(*Clusters,ebml_element*); ++Cluster)
	{
        ShowProgress((ebml_element*)&(*Cluster), TotalSize);
		MATROSKA_LinkClusterWriteSegmentInfo((matroska_cluster*)&(*Cluster),WSegmentInfo);
		for (auto Elt = EBML_MasterChildren(&static_cast<EbmlMaster &>(*Cluster)); EBML_MasterEnd(Elt,&static_cast<EbmlMaster &>(*Cluster)); Elt = EBML_MasterNext(Elt))
		{
			Block = NULL;
			if (EBML_ElementIsType(*Elt, KaxSimpleBlock))
			{
				if (MATROSKA_BlockKeyframe((KaxSimpleBlock*)*Elt))
					Block = (matroska_block*)*Elt;
			}
			else if (EBML_ElementIsType(*Elt, KaxBlockGroup))
			{
				EBML_MASTER_CONST_ITERATOR EltB;
                ebml_element *BlockRef = NULL;
				for (EltB = EBML_MasterChildren((KaxBlockGroup*)*Elt); EBML_MasterEnd(EltB,(KaxBlockGroup*)*Elt); EltB = EBML_MasterNext(EltB))
				{
					if (EBML_ElementIsType(*EltB, KaxBlock))
						Block = (matroska_block*)*EltB;
					else if (EBML_ElementIsType(*EltB, KaxReferenceBlock))
						BlockRef = *EltB;
				}
				if (BlockRef && Block)
					Block = NULL; // not a keyframe
			}

			if (Block && MATROSKA_BlockTrackNum(Block) == TrackNum)
			{
				BlockTimestamp = MATROSKA_BlockTimestamp((matroska_cluster*)&(*Cluster),Block);
				if ((BlockTimestamp - PrevTimestamp) < 800000000 && PrevTimestamp != INVALID_TIMESTAMP_T)
					break; // no more than 1 Cue per Cluster and per 800 ms

				CuePoint = (matroska_cuepoint*)EBML_MasterAddElt(Cues,KaxCuePoint,1,DstProfile);
				if (!CuePoint)
				{
					TextPrintf(StdErr,T("Failed to create a new CuePoint ! out of memory ?\r\n"));
					return 0;
				}
				MATROSKA_LinkCueSegmentInfo(CuePoint,WSegmentInfo);
				MATROSKA_LinkCuePointBlock(CuePoint,Block);
				MATROSKA_CuePointUpdate(CuePoint,RSegment, ProfileFrom(DstProfile));

				PrevTimestamp = BlockTimestamp;

				break; // one Cues per Cluster is enough
			}
		}
	}
    EndProgress();

	if (EBML_MasterEmpty(Cues))
	{
		TextPrintf(StdErr,T("Failed to create the Cue entries, no Block found\r\n"));
		return 0;
	}

	EBML_ElementUpdateSize(Cues,0,0, ProfileFrom(DstProfile));
	return 1;
}

static int TimcodeCmp(const void* Param, const mkv_timestamp_t *a, const mkv_timestamp_t *b)
{
	if (*a == *b)
		return 0;
	if (*a > *b)
		return 1;
	return -1;
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

static void CleanCropValues(ebml_master *Track, int64_t Width, int64_t Height)
{
    ebml_element *Left,*Right,*Top,*Bottom;
    Left = EBML_MasterGetChild(Track,KaxVideoPixelCropLeft, DstProfile);
    Right = EBML_MasterGetChild(Track,KaxVideoPixelCropRight, DstProfile);
    Top = EBML_MasterGetChild(Track,KaxVideoPixelCropTop, DstProfile);
    Bottom = EBML_MasterGetChild(Track,KaxVideoPixelCropBottom, DstProfile);
    if (EBML_IntegerValue((ebml_integer*)Top)+EBML_IntegerValue((ebml_integer*)Bottom) >= Height || EBML_IntegerValue((ebml_integer*)Left)+EBML_IntegerValue((ebml_integer*)Right) >= Width)
    {
        // invalid crop, remove the values
        NodeDelete(Left);
        NodeDelete(Right);
        NodeDelete(Top);
        NodeDelete(Bottom);
    }
    else
    {
        if (EBML_IntegerValue((ebml_integer*)Left)==0)   NodeDelete(Left);
        if (EBML_IntegerValue((ebml_integer*)Right)==0)  NodeDelete(Right);
        if (EBML_IntegerValue((ebml_integer*)Top)==0)    NodeDelete(Top);
        if (EBML_IntegerValue((ebml_integer*)Bottom)==0) NodeDelete(Bottom);
    }
}

#define HasTrackUID(m,v,t)  tHasTrackUID<t>(m,v)

template<class UIDType>
// static bool_t HasTrackUID(ebml_master *Tracks, int TrackUID, const ebml_context *UIDType)
static bool_t tHasTrackUID(ebml_master *Tracks, int TrackUID)
{
    ebml_master *Track;
    for (Track = (ebml_master*)EBML_MasterFindChild(Tracks,KaxTrackEntry);Track; Track = EBML_MasterNextChild(Tracks,Track))
    {
        ebml_element *Elt = EBML_MasterFindChild(Track,UIDType);
        if (Elt && EBML_IntegerValue((ebml_integer*)Elt)==TrackUID)
            return 1;
    }
    return 0;
}

static int CleanTracks(ebml_master *Tracks, int srcProfile, int *dstProfile, ebml_master *Attachments, array_block_info_p *Alternate3DTracks)
{
    ebml_master *Track, *CurTrack, *OtherTrack;
    ebml_element *Elt, *Elt2, *DisplayW, *DisplayH;
    int TrackNum, Width, Height;
    MatroskaTrackType TrackType;
    tchar_t CodecID[MAXPATH];

    for (Track = (ebml_master*)EBML_MasterFindChild(Tracks,KaxTrackEntry); Track;)
    {
		CurTrack = Track;
		Track = EBML_MasterNextChild(Tracks,Track);
		Elt = EBML_MasterFindChild(CurTrack,KaxTrackNumber);
		if (!Elt)
		{
			TextPrintf(StdErr,T("The track at %") TPRId64 T(" has no number set!\r\n"),EBML_ElementPosition((ebml_element*)CurTrack));
			NodeDelete(CurTrack);
			continue;
		}
		TrackNum = (int)EBML_IntegerValue((ebml_integer*)Elt);

		Elt = EBML_MasterFindChild(CurTrack,KaxCodecID);
		if (!Elt && !EBML_MasterFindChild(CurTrack,KaxTrackOperation))
		{
			TextPrintf(StdErr,T("The track %d at %") TPRId64 T(" has no CodecID set!\r\n"), TrackNum,EBML_ElementPosition((ebml_element*)CurTrack));
			NodeDelete(CurTrack);
			continue;
		}

		Elt = EBML_MasterFindChild(CurTrack,KaxTrackType);
		if (!Elt)
		{
			TextPrintf(StdErr,T("The track %d at %") TPRId64 T(" has no type set!\r\n"), TrackNum,EBML_ElementPosition((ebml_element*)CurTrack));
			NodeDelete(CurTrack);
			continue;
		}
        TrackType = (MatroskaTrackType)EBML_IntegerValue((ebml_integer*)Elt);

        if (ARRAYCOUNT(*Alternate3DTracks, block_info*) >= (size_t)TrackNum && TrackType!=MATROSKA_TRACK_TYPE_VIDEO)
            ARRAYBEGIN(*Alternate3DTracks, block_info*)[TrackNum] = NULL;

        // clean the aspect ratio
        Elt = EBML_MasterFindChild(CurTrack,KaxTrackVideo);
        if (Elt)
        {
            Width = (int)EBML_IntegerValue((ebml_integer*)EBML_MasterFindChild((ebml_master*)Elt,KaxVideoPixelWidth));
            Height = (int)EBML_IntegerValue((ebml_integer*)EBML_MasterFindChild((ebml_master*)Elt,KaxVideoPixelHeight));
	        if (Width==0 || Height==0)
	        {
		        TextPrintf(StdErr,T("The track %d at %") TPRId64 T(" has invalid pixel dimensions %dx%d!\r\n"), TrackNum,EBML_ElementPosition((ebml_element*)CurTrack),Width,Height);
		        NodeDelete(CurTrack);
		        continue;
	        }

            DisplayW = EBML_MasterFindChild((ebml_master*)Elt,KaxVideoDisplayWidth);
            DisplayH = EBML_MasterFindChild((ebml_master*)Elt,KaxVideoDisplayHeight);
            if (DisplayW || DisplayH)
            {
                Elt2 = EBML_MasterFindChild((ebml_master*)Elt,KaxVideoDisplayUnit);
                if (Elt2 && EBML_IntegerValue((ebml_integer*)Elt2)==MATROSKA_DISPLAY_UNIT_DISPLAYASPECTRATIO)
                {
                    // if the output size appears in pixel, fix it
                    if ((!DisplayW || Width==EBML_IntegerValue((ebml_integer*)DisplayW)) && (!DisplayH || Height==EBML_IntegerValue((ebml_integer*)DisplayH)))
                        EBML_IntegerSetValue((ebml_integer*)Elt2, MATROSKA_DISPLAY_UNIT_PIXELS);
                    else
                        CleanCropValues((ebml_master*)Elt, 0, 0);
                }

                if (!Elt2 || EBML_IntegerValue((ebml_integer*)Elt2)==MATROSKA_DISPLAY_UNIT_PIXELS) // pixel AR
                {
                    if (!DisplayW)
                    {
                        if (EBML_IntegerValue((ebml_integer*)DisplayH)==Height)
                        {
                            NodeDelete(DisplayH);
                            DisplayH = NULL; // we don't the display values, they are the same as the pixel ones
                        }
                        else
                        {
                            DisplayW = EBML_MasterFindFirstElt((ebml_master*)Elt,KaxVideoDisplayWidth,1,0,0);
                            EBML_IntegerSetValue((ebml_integer*)DisplayW,Width);
                        }
                    }
                    else if (EBML_IntegerValue((ebml_integer*)DisplayW)==0)
                    {
		                TextPrintf(StdErr,T("The track %d at %") TPRId64 T(" has invalid display width %") TPRId64 T("!\r\n"), TrackNum,EBML_ElementPosition((ebml_element*)CurTrack),EBML_IntegerValue((ebml_integer*)DisplayW));
		                NodeDelete(CurTrack);
		                continue;
                    }
                    else if (!DisplayH)
                    {
                        if (EBML_IntegerValue((ebml_integer*)DisplayW)==Width)
                        {
                            NodeDelete(DisplayW);
                            DisplayW = NULL; // we don't the display values, they are the same as the pixel ones
                        }
                        else
                        {
                            DisplayH = EBML_MasterFindFirstElt((ebml_master*)Elt,KaxVideoDisplayHeight,1,0,0);
                            EBML_IntegerSetValue((ebml_integer*)DisplayH,Height);
                        }
                    }
                    else if (EBML_IntegerValue((ebml_integer*)DisplayH)==0)
                    {
                        if (!Quiet)
		                    TextPrintf(StdErr,T("The track %d at %") TPRId64 T(" has invalid display height %") TPRId64 T(", deleting track!\r\n"), TrackNum,EBML_ElementPosition((ebml_element*)CurTrack),EBML_IntegerValue((ebml_integer*)DisplayH));
		                NodeDelete(CurTrack);
		                continue;
                    }

                    if (DisplayW && DisplayH)
                    {
                        if (EBML_IntegerValue((ebml_integer*)DisplayH) < Height && EBML_IntegerValue((ebml_integer*)DisplayW) < Width) // Haali's non-pixel shrinking
                        {
                            int64_t DW = EBML_IntegerValue((ebml_integer*)DisplayW);
							int64_t DH = EBML_IntegerValue((ebml_integer*)DisplayH);
                            int Serious = gcd(DW,DH)==1; // shrank as much as it was possible
                            if (8*DW <= Width && 8*DH <= Height)
                                ++Serious; // shrank too much compared to the original
                            else if (2*DW >= Width && 2*DH >= Height)
                                --Serious; // may be intentional

                            if (Serious) // doesn't seem correct as pixels
                            {
							    if (DW > DH)
							    {
								    EBML_IntegerSetValue((ebml_integer*)DisplayW,Scale64(Height,DW,DH));
								    EBML_IntegerSetValue((ebml_integer*)DisplayH,Height);
							    }
							    else
							    {
								    EBML_IntegerSetValue((ebml_integer*)DisplayH,Scale64(Width,DH,DW));
								    EBML_IntegerSetValue((ebml_integer*)DisplayW,Width);
							    }

							    // check if the AR is respected otherwise force it into a DAR
							    if (EBML_IntegerValue((ebml_integer*)DisplayW)*DH != EBML_IntegerValue((ebml_integer*)DisplayH)*DW)
							    {
								    Elt2 = EBML_MasterFindFirstElt((ebml_master*)Elt,KaxVideoDisplayUnit, 1, 0, 0);
								    if (Elt2)
								    {
									    EBML_IntegerSetValue((ebml_integer*)Elt2,MATROSKA_DISPLAY_UNIT_DISPLAYASPECTRATIO);
									    EBML_IntegerSetValue((ebml_integer*)DisplayW,DW);
									    EBML_IntegerSetValue((ebml_integer*)DisplayH,DH);
								    }
							    }
                            }
                        }
                        if (DisplayH && EBML_IntegerValue((ebml_integer*)DisplayH) == Height)
                        {
                            NodeDelete(DisplayH);
                            DisplayH = NULL;
                        }
                        if (DisplayW && EBML_IntegerValue((ebml_integer*)DisplayW) == Width)
                        {
                            NodeDelete(DisplayW);
                            DisplayW = NULL;
                        }
                    }
                }
                Elt2 = EBML_MasterGetChild((ebml_master*)Elt,KaxVideoDisplayUnit, DstProfile);
                if (EBML_IntegerValue((ebml_integer*)Elt2)!=MATROSKA_DISPLAY_UNIT_DISPLAYASPECTRATIO)
                    CleanCropValues((ebml_master*)Elt, DisplayW?EBML_IntegerValue((ebml_integer*)DisplayW):Width, DisplayH?EBML_IntegerValue((ebml_integer*)DisplayH):Height);
            }

            if (srcProfile==PROFILE_MATROSKA_V1 || srcProfile==PROFILE_MATROSKA_V2 || srcProfile==PROFILE_DIVX)
            {
                // clean the older StereoMode values
                Elt2 = EBML_MasterFindChild(Elt,KaxVideoStereoMode);
                if (!Elt2)
                    Elt2 = EBML_MasterFindChild(Elt,KaxOldStereoMode);
                if (Elt2)
                {
                    Width = (int)EBML_IntegerValue((ebml_integer*)Elt2);
                    if (Width!=MATROSKA_VIDEO_OLDSTEREOMODE_MONO && Width <= 3) // upper values are probably the new ones
                    {
                        *dstProfile = PROFILE_MATROSKA_V3;
                        if (!Quiet)
                            TextPrintf(StdErr,T("The track %d at %") TPRId64 T(" is using an old StereoMode value, converting to profile '%s'\r\n"), TrackNum,EBML_ElementPosition((ebml_element*)CurTrack),GetProfileName(*dstProfile));
                        if (EBML_ElementIsType(Elt2, KaxOldStereoMode))
                            // replace the old by a new
                            EBML_ElementForceContext(Elt2, KaxVideoStereoMode);

                        // replace the old values with the new ones
                        if (Width==MATROSKA_VIDEO_OLDSTEREOMODE_BOTH_EYES)
                        {
                            if (!Quiet)
                                TextPrintf(StdErr,T("  turning 'Both Eyes' into 'side by side (left first)\r\n"));
                            EBML_IntegerSetValue((ebml_integer*)Elt2,MATROSKA_VIDEO_STEREO_LEFT_RIGHT);
                        }
                        else
                        {
                            EBML_IntegerSetValue((ebml_integer*)Elt2,MATROSKA_VIDEO_STEREO_MONO);
                            if (!Quiet)
                                TextPrintf(StdErr,T("  turning %s eye to mono\r\n"), Width==MATROSKA_VIDEO_OLDSTEREOMODE_LEFT_EYE?T("left"):T("right"));
                            // look for the other track
                            for (OtherTrack = EBML_MasterNextChild(Tracks,CurTrack);OtherTrack; OtherTrack = EBML_MasterNextChild(Tracks,OtherTrack))
                            {
                                ebml_element *VidElt = EBML_MasterFindChild(OtherTrack,KaxTrackVideo);
                                if (VidElt)
                                {
                                    // TODO: only use a video track that has the same output pixels as the source
                                    ebml_element *OtherStereo = EBML_MasterFindChild(VidElt,KaxVideoStereoMode);
                                    if (!OtherStereo)
                                        OtherStereo = EBML_MasterFindChild(VidElt,KaxOldStereoMode);
                                    if (OtherStereo && ((Width==MATROSKA_VIDEO_OLDSTEREOMODE_LEFT_EYE && EBML_IntegerValue((ebml_integer*)OtherStereo)==MATROSKA_VIDEO_OLDSTEREOMODE_RIGHT_EYE) ||
                                        (Width==MATROSKA_VIDEO_OLDSTEREOMODE_RIGHT_EYE && EBML_IntegerValue((ebml_integer*)OtherStereo)==MATROSKA_VIDEO_OLDSTEREOMODE_LEFT_EYE)))
                                    {
                                        ebml_master *CombinedTrack;
                                        int NewTrackUID;

                                        if (!Quiet)
                                            TextPrintf(StdErr,T("  turning matching %s eye to mono and creating a new combined track\r\n"), Width==MATROSKA_VIDEO_OLDSTEREOMODE_RIGHT_EYE?T("left"):T("right"));
                                        EBML_IntegerSetValue((ebml_integer*)OtherStereo,MATROSKA_VIDEO_STEREO_MONO);

                                        // create another track that is this one combined
                                        CombinedTrack = (ebml_master*)EBML_ElementCopy(CurTrack, NULL);
                                        EBML_MasterAppend(Tracks, (ebml_element*)CombinedTrack);

                                        // set a new TrackNumber
                                        NewTrackUID = TrackNum+1;
                                        while (HasTrackUID(Tracks, NewTrackUID, KaxTrackNumber))
                                            ++NewTrackUID;
                                        Elt = EBML_MasterFindChild(CombinedTrack,KaxTrackNumber);
                                        EBML_IntegerSetValue((ebml_integer*)Elt,NewTrackUID);

                                        // set a new TrackUID
                                        NewTrackUID = 1;
                                        while (HasTrackUID(Tracks, NewTrackUID, KaxTrackUID))
                                            ++NewTrackUID;
                                        Elt = EBML_MasterFindChild(CombinedTrack,KaxTrackUID);
                                        EBML_IntegerSetValue((ebml_integer*)Elt,NewTrackUID);

                                        // cleaning
                                        Elt = EBML_MasterFindChild(CombinedTrack,KaxTrackFlagEnabled);
                                        if (Elt)
                                            EBML_IntegerSetValue((ebml_integer*)Elt,1);

                                        Elt = EBML_MasterFindChild(CombinedTrack,KaxTrackFlagLacing);
                                        NodeDelete(Elt);
                                        Elt = EBML_MasterFindChild(CombinedTrack,KaxTrackMinCache);
                                        NodeDelete(Elt);
                                        Elt = EBML_MasterFindChild(CombinedTrack,KaxTrackMaxCache);
                                        NodeDelete(Elt);
                                        Elt = EBML_MasterFindChild(CombinedTrack,KaxTrackDefaultDuration);
                                        NodeDelete(Elt);
                                        Elt = EBML_MasterFindChild(CombinedTrack,KaxTrackTimestampScale);
                                        NodeDelete(Elt);
                                        Elt = EBML_MasterFindChild(CombinedTrack,KaxMaxBlockAdditionID);
                                        NodeDelete(Elt);
                                        Elt = EBML_MasterGetChild(CombinedTrack,KaxCodecID, DstProfile);
                                        EBML_StringSetValue((ebml_string*)Elt,"V_COMBINED");
                                        Elt = EBML_MasterFindChild(CombinedTrack,KaxCodecPrivate);
                                        NodeDelete(Elt);
                                        Elt = EBML_MasterFindChild(CombinedTrack,KaxTrackAttachmentLink);
                                        NodeDelete(Elt);
                                        Elt = EBML_MasterFindChild(CombinedTrack,KaxTrackTranslate);
                                        NodeDelete(Elt);

                                        // add the TrackCombine info
                                        Elt = EBML_MasterAddElt(CombinedTrack, KaxTrackOperation, 1, DstProfile);
                                        if (Elt)
                                            Elt = EBML_MasterAddElt((ebml_master*)Elt, KaxTrackCombinePlanes, 1, DstProfile);
                                        if (Elt)
                                        {
                                            // track 1
                                            CombinedTrack = (ebml_master*)EBML_MasterAddElt((ebml_master*)Elt, KaxTrackPlane, 1, DstProfile);
                                            OtherStereo = EBML_MasterFindChild(CurTrack, KaxTrackUID);
                                            if (!OtherStereo)
                                            {
                                                OtherStereo = EBML_MasterAddElt((ebml_master*)CurTrack, KaxTrackUID, 1, DstProfile);
                                                while (HasTrackUID(Tracks, NewTrackUID, KaxTrackUID))
                                                    ++NewTrackUID;
                                                EBML_IntegerSetValue((ebml_integer*)OtherStereo, NewTrackUID);
                                            }
                                            Elt2 = EBML_MasterGetChild(CombinedTrack, KaxTrackPlaneUID, DstProfile);
                                            EBML_IntegerSetValue((ebml_integer*)Elt2, EBML_IntegerValue((ebml_integer*)OtherStereo));
                                            Elt2 = EBML_MasterGetChild(CombinedTrack, KaxTrackPlaneType, DstProfile);
                                            EBML_IntegerSetValue((ebml_integer*)Elt2, Width==MATROSKA_VIDEO_OLDSTEREOMODE_LEFT_EYE? MATROSKA_TRACKPLANETYPE_LEFT_EYE : MATROSKA_TRACKPLANETYPE_RIGHT_EYE);

                                            // track 2
                                            CombinedTrack = (ebml_master*)EBML_MasterAddElt((ebml_master*)Elt, KaxTrackPlane, 1, DstProfile);
                                            OtherStereo = EBML_MasterFindChild(OtherTrack, KaxTrackUID);
                                            if (!OtherStereo)
                                            {
                                                OtherStereo = EBML_MasterAddElt((ebml_master*)OtherTrack, KaxTrackUID, 1, DstProfile);
                                                while (HasTrackUID(Tracks, NewTrackUID, KaxTrackUID))
                                                    ++NewTrackUID;
                                                EBML_IntegerSetValue((ebml_integer*)OtherStereo, NewTrackUID);
                                            }
                                            Elt2 = EBML_MasterGetChild(CombinedTrack, KaxTrackPlaneUID, DstProfile);
                                            EBML_IntegerSetValue((ebml_integer*)Elt2, EBML_IntegerValue((ebml_integer*)OtherStereo));
                                            Elt2 = EBML_MasterGetChild(CombinedTrack, KaxTrackPlaneType, DstProfile);
                                            EBML_IntegerSetValue((ebml_integer*)Elt2, Width==MATROSKA_VIDEO_OLDSTEREOMODE_RIGHT_EYE? MATROSKA_TRACKPLANETYPE_LEFT_EYE : MATROSKA_TRACKPLANETYPE_RIGHT_EYE);
                                        }

                                        return CleanTracks(Tracks, srcProfile, dstProfile, Attachments, Alternate3DTracks);
                                    }
                                }
                            }

                            if (!Quiet)
                                TextPrintf(StdErr,T("  could not find the matching %s track!\r\n"), Width==MATROSKA_VIDEO_OLDSTEREOMODE_RIGHT_EYE?T("left"):T("right"));
                        }
                    }
                }
            }
        }

        if (ARRAYCOUNT(*Alternate3DTracks, block_info*) >= (size_t)TrackNum && ARRAYBEGIN(*Alternate3DTracks, block_info*)[TrackNum])
        {
            // force the proper StereoMode value
            Elt2 = EBML_MasterGetChild((ebml_master*)Elt,KaxVideoStereoMode, DstProfile);
            EBML_IntegerSetValue((ebml_integer*)Elt2, MATROSKA_VIDEO_STEREO_BOTH_EYES_BLOCK_LR);
        }

        if (*dstProfile==PROFILE_WEBM)
        {
            // verify that we have only VP8, VP9, Opus and Vorbis tracks
            Elt = EBML_MasterFindChild(CurTrack,KaxCodecID);
            EBML_StringGet((ebml_string*)Elt,CodecID,TSIZEOF(CodecID));
            if (!((TrackType==MATROSKA_TRACK_TYPE_VIDEO && tcsisame_ascii(CodecID,T("V_VP8"))) ||
                  (TrackType==MATROSKA_TRACK_TYPE_VIDEO && tcsisame_ascii(CodecID,T("V_VP9"))) ||
                  (TrackType==MATROSKA_TRACK_TYPE_AUDIO && tcsisame_ascii(CodecID,T("A_VORBIS"))) ||
                  (TrackType==MATROSKA_TRACK_TYPE_AUDIO && tcsisame_ascii(CodecID,T("A_OPUS")))
                ))
            {
                TextPrintf(StdErr,T("Wrong codec '%s' for profile '%s' removing track %d\r\n"),CodecID,GetProfileName(*dstProfile),TrackNum);
                NodeDelete(CurTrack);
                continue;
            }
        }

        // clean the output sampling freq
        Elt = EBML_MasterFindChild(CurTrack,KaxTrackAudio);
        if (Elt)
        {
            Elt2 = EBML_MasterFindChild((ebml_master*)Elt,KaxAudioOutputSamplingFreq);
            if (Elt2)
            {
                DisplayH = EBML_MasterFindChild((ebml_master*)Elt,KaxAudioSamplingFreq);
                assert(DisplayH!=NULL);
                if (EBML_FloatValue((ebml_float*)Elt2) == EBML_FloatValue((ebml_float*)DisplayH))
                    NodeDelete(Elt2);
            }
        }

        // clean the attachment links
        Elt = EBML_MasterFindChild(CurTrack,KaxTrackAttachmentLink);
        while (Elt)
        {
            Elt2 = NULL;
            if (!Attachments)
                Elt2 = Elt;
            else
            {
                Elt2 = EBML_MasterFindChild(Attachments,KaxAttached);
                while (Elt2)
                {
                    DisplayH = EBML_MasterFindChild((ebml_master*)Elt2,KaxFileUID);
                    if (DisplayH && EBML_IntegerValue((ebml_integer*)DisplayH)==EBML_IntegerValue((ebml_integer*)Elt))
                        break;
                    Elt2 = EBML_MasterNextChild(Attachments, Elt2);
                }
                if (!Elt2) // the attachment wasn't found, delete Elt
                    Elt2 = Elt;
                else
                    Elt2 = NULL;
            }

            Elt = EBML_MasterNextChild(CurTrack, Elt);
            if (Elt2)
                NodeDelete(Elt2);
        }
    }

    // disable Alternate3DTracks handling if there is no matching track
    TrackNum = -1;
    for (size_t TrackType=0; TrackType<ARRAYCOUNT(*Alternate3DTracks, block_info*); ++TrackType)
        if (ARRAYBEGIN(*Alternate3DTracks, block_info*)[TrackType])
        {
            TrackNum = TrackType;
            break;
        }
    if (TrackNum==-1)
        ArrayClear(Alternate3DTracks);

    if (EBML_MasterFindChild(Tracks,KaxTrackEntry)==NULL)
        return -19;

    // put the TrackNumber, TrackType and CodecId at the front of the Track elements
    for (Track = (ebml_master*)EBML_MasterFindChild(Tracks,KaxTrackEntry); Track; Track = EBML_MasterNextChild(Tracks,Track))
    {
        Elt = EBML_MasterFindChild(Track,KaxTrackNumber);
        if (Elt)
            NodeTree_SetParent(Elt,Track,EBML_MasterChildren(Track));

        Elt2 = EBML_MasterFindChild(Track,KaxTrackType);
        assert(Elt2 != NULL);
        if (Elt2)
        {
            if (Elt)
                NodeTree_SetParent(Elt2, Track, EBML_MasterNext(Elt));

            DisplayW = EBML_MasterFindChild(Track, KaxCodecID);
            if (DisplayW)
                NodeTree_SetParent(DisplayW, Track, EBML_MasterNext(Elt2));
        }
    }

    return 0;
}

static void InitCommonHeader(binary *TrackHeader)
{
    // special mark to tell the header has not been used yet
#if 1
    // *TrackHeader = TABLE_MARKER;
#else
    TrackHeader->_Begin = TABLE_MARKER;
#endif
}

static bool_t BlockIsCompressed(const matroska_block *Block)
{
    ebml_master *Track = (ebml_master*)MATROSKA_BlockReadTrack(Block);
    if (Track)
    {
        ebml_master *Elt = (ebml_master*)EBML_MasterFindChild(Track, KaxContentEncodings);
        if (Elt)
        {
            Elt = (ebml_master*)EBML_MasterFindChild(Elt, KaxContentEncoding);
            if (!EBML_MasterEmpty(Elt))
            {
                if (EBML_MasterNext(Elt))
                    return 1; // we don't support cascaded encryption/compression

                Elt = (ebml_master*)EBML_MasterFindChild(Elt, KaxContentCompression);
                if (!Elt)
                    return 1; // we don't support encryption

                auto Algo = EBML_MasterGetChild(Elt, KaxContentCompAlgo, DstProfile);
                if (Algo!=NULL && EBML_IntegerValue(Algo)==MATROSKA_TRACK_ENCODING_COMP_ZLIB)
                    return 1;
            }
        }
    }
    return 0;
}

static void ShrinkCommonHeader(array_binary *TrackHeader, matroska_block *Block, stream *Input)
{
    size_t Frame,FrameCount,EqualData;

    if (/*TrackHeader->_Begin != TABLE_MARKER &&*/ ARRAYCOUNT(*TrackHeader,uint8_t)==0)
        return;
    if (MATROSKA_BlockReadData(Block,Input,SrcProfile)!=ERR_NONE)
        return;

    if (BlockIsCompressed(Block))
        return;

    FrameCount = MATROSKA_BlockGetFrameCount(Block);
    Frame = 0;
    if (FrameCount /*&& TrackHeader->_Begin == TABLE_MARKER*/)
    {
        matroska_frame & FrameData =
        // use the first frame as the reference
        MATROSKA_BlockGetFrame(Block,Frame,&FrameData,1);
        /*TrackHeader->_Begin = NULL;*/
        for (std::uint32_t i=0;i<FrameData.Size();i++)
            ArrayAppend(TrackHeader,FrameData.Buffer()[i],1,0);
        // ArrayAppend(TrackHeader,FrameData.Buffer(),FrameData.Size(),0);
        Frame = 1;
    }
    for (;Frame<FrameCount;++Frame)
    {
        matroska_frame & FrameData =
        MATROSKA_BlockGetFrame(Block,Frame,&FrameData,1);
        EqualData = 0;
        while (EqualData < FrameData.Size() && EqualData < ARRAYCOUNT(*TrackHeader,uint8_t))
        {
            if (ARRAYBEGIN(*TrackHeader,uint8_t)[EqualData] == FrameData.Buffer()[EqualData])
                ++EqualData;
            else
                break;
        }
        if (EqualData != ARRAYCOUNT(*TrackHeader,uint8_t))
            ArrayShrink(TrackHeader,ARRAYCOUNT(*TrackHeader,uint8_t)-EqualData);
        if (ARRAYCOUNT(*TrackHeader,uint8_t)==0)
            break;
    }
    MATROSKA_BlockReleaseData(Block,1);
}

static void ClearCommonHeader(array_binary *TrackHeader)
{
    // if (TrackHeader->_Begin == TABLE_MARKER)
    //     TrackHeader->_Begin = NULL;
}

static void WriteJunk(stream *Output, size_t Amount)
{
	char Val = 0x0A;
	while (Amount--)
		Stream_Write(Output,&Val,1,NULL);
}

#if defined(_WIN32) && defined(UNICODE)
int wmain(int argc, const wchar_t *argv[])
#else
int main(int argc, const char *argv[])
#endif
{
    int i,Result = 0;
    int ShowUsage = 0;
    int ShowVersion = 0;
    // parsercontext p;
    // textwriter _StdErr;
    stream *Input = NULL,*Output = NULL;
    std::unique_ptr<EbmlStream> inputStream;
    tchar_t Path[MAXPATHFULL];
    tchar_t String[MAXLINE],Original[MAXLINE],*s;
    ebml_master *EbmlHead = NULL, *RSegment = NULL, *RLevel1 = NULL, **Cluster;
    ebml_master *RSegmentInfo = NULL, *RTrackInfo = NULL, *RChapters = NULL, *RTags = NULL, *RCues = NULL, *RAttachments = NULL;
    ebml_master *WSegment = NULL, *WMetaSeek = NULL, *WSegmentInfo = NULL, *WTrackInfo = NULL;
    ebml_element *Elt, *Elt2;
    matroska_seekpoint *WSeekPoint = NULL, *W1stClusterSeek = NULL;
    ebml_unistring *LibName, *AppName;
    array_matroska_cluster_p RClusters, *Clusters;
    array WClusters;
    array_track_info WTracks;
    // ebml_parser_context RContext;
    // ebml_parser_context RSegmentContext;
    int UpperElement;
    filepos_t MetaSeekBefore, MetaSeekAfter;
    filepos_t NextPos = 0, SegmentSize = 0, ClusterSize, CuesSize;
    size_t ExtraVoidSize = 0;
    mkv_timestamp_t PrevTimestamp;
    bool_t CuesChanged;
	bool_t KeepCues = 0, Remux = 0, CuesCreated = 0, Optimize = 0, OptimizeVideo = 1, UnOptimize = 0, ClustersNeedRead = 0, Regression = 0;
    int InputPathIndex = 1;
	int64_t TimestampScale = 0, OldTimestampScale;
    size_t MaxTrackNum = 0;
    array_array_binary TrackMaxHeader; // array of uint8_t (max common header)
    filepos_t TotalSize;
    array_block_info_p Alternate3DTracks;
    const KaxTimestampScale *TimeScale;

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
    ArrayInit(&WClusters);
    ArrayInit(&WTracks);
	ArrayInit(&TrackMaxHeader);
    ArrayInit(&Alternate3DTracks);
	Clusters = &RClusters;

    // StdErr = &_StdErr;
    // memset(StdErr,0,sizeof(_StdErr));
    // StdErr->Stream = (stream*)NodeSingleton(&p,STDERR_ID);

#if defined(_WIN32) && defined(UNICODE)
    Node_FromWcs(&p,Path,TSIZEOF(Path),argv[0]);
#else
    Node_FromStr(&p,Path,TSIZEOF(Path),argv[0]);
#endif
    std::string_view cmd = Path;
    UnOptimize = cmd.find("mkWDclean") != std::string_view::npos;
    if (UnOptimize)
        TextPrintf(StdErr,T("Running special mkWDclean mode, please fix your player instead of valid Matroska files\r\n"));
	Path[0] = 0;

	for (i=1;i<argc;++i)
	{
#if defined(_WIN32) && defined(UNICODE)
	    Node_FromWcs(&p,Path,TSIZEOF(Path),argv[i]);
#else
	    Node_FromStr(&p,Path,TSIZEOF(Path),argv[i]);
#endif
		if (tcsisame_ascii(Path,T("--keep-cues"))) { KeepCues = 1; InputPathIndex = i+1; }
		else if (tcsisame_ascii(Path,T("--remux"))) { Remux = 1; InputPathIndex = i+1; }
		else if (tcsisame_ascii(Path,T("--live"))) { Live = 1; InputPathIndex = i+1; }
		else if (tcsisame_ascii(Path,T("--doctype")) && i+1<argc-1)
		{
#if defined(_WIN32) && defined(UNICODE)
		    Node_FromWcs(&p,Path,TSIZEOF(Path),argv[++i]);
#else
		    Node_FromStr(&p,Path,TSIZEOF(Path),argv[++i]);
#endif
			if (tcsisame_ascii(Path,T("1")))
				DstProfile = PROFILE_MATROSKA_V1;
			else if (tcsisame_ascii(Path,T("2")))
				DstProfile = PROFILE_MATROSKA_V2;
			else if (tcsisame_ascii(Path,T("3")))
				DstProfile = PROFILE_MATROSKA_V3;
			else if (tcsisame_ascii(Path,T("4")))
				DstProfile = PROFILE_WEBM;
			else if (tcsisame_ascii(Path,T("5")))
				DstProfile = PROFILE_DIVX;
			else if (tcsisame_ascii(Path,T("6")))
				DstProfile = PROFILE_MATROSKA_V4;
			else if (tcsisame_ascii(Path,T("7")))
				DstProfile = PROFILE_MATROSKA_V5;
			else
			{
		        TextPrintf(StdErr,T("Unknown doctype %s\r\n"),Path);
                Path[0] = 0;
				Result = -8;
				goto exit;
			}
			InputPathIndex = i+1;
		}
		else if (tcsisame_ascii(Path,T("--timecodescale")) && i+1<argc-1)
		{
#if defined(_WIN32) && defined(UNICODE)
		    Node_FromWcs(&p,Path,TSIZEOF(Path),argv[++i]);
#else
		    Node_FromStr(&p,Path,TSIZEOF(Path),argv[++i]);
#endif
			TimestampScale = StringToInt(Path,0);
			InputPathIndex = i+1;
		}
		else if (tcsisame_ascii(Path,T("--alt-3d")) && i+1<argc-1)
		{
            size_t TrackId;
#if defined(_WIN32) && defined(UNICODE)
		    Node_FromWcs(&p,Path,TSIZEOF(Path),argv[++i]);
#else
		    Node_FromStr(&p,Path,TSIZEOF(Path),argv[++i]);
#endif
            TrackId = StringToInt(Path,0);
            if (ARRAYCOUNT(Alternate3DTracks, block_info*) < (TrackId+1))
            {
                size_t OldSize = ARRAYCOUNT(Alternate3DTracks, block_info*);
			    ArrayResize(&Alternate3DTracks, (TrackId+1), block_info*,64);
                memset(&ARRAYBEGIN(Alternate3DTracks, block_info*)[OldSize], 0, (TrackId-OldSize+1)*sizeof(block_info*));
            }
            ARRAYBEGIN(Alternate3DTracks, block_info*)[TrackId] = MARKER3D;
			InputPathIndex = i+1;
		}
		else if (tcsisame_ascii(Path,T("--unsafe"))) { Unsafe = 1; InputPathIndex = i+1; }
		else if (tcsisame_ascii(Path,T("--optimize"))) { Optimize = 1; OptimizeVideo = 1; InputPathIndex = i+1; }
		else if (tcsisame_ascii(Path,T("--optimize_nv"))) { Optimize = 1; OptimizeVideo = 0; InputPathIndex = i+1; }
		else if (tcsisame_ascii(Path,T("--regression"))) { Regression = 1; InputPathIndex = i+1; }
		else if (tcsisame_ascii(Path,T("--no-optimize"))) { UnOptimize = 1; InputPathIndex = i+1; }
		else if (tcsisame_ascii(Path,T("--quiet"))) { Quiet = 1; InputPathIndex = i+1; }
		else if (tcsisame_ascii(Path,T("--version"))) { ShowVersion = 1; InputPathIndex = i+1; }
        else if (tcsisame_ascii(Path,T("--help"))) {ShowVersion = 1; ShowUsage = 1; InputPathIndex = i+1; }
		else if (i<argc-2) TextPrintf(StdErr,T("Unknown parameter '%s'\r\n"),Path);
	}

    if (argc < (1+InputPathIndex) || ShowVersion)
    {
        TextWrite(StdErr,PROJECT_NAME T(" v") PROJECT_VERSION T(", Copyright (c) 2010-2020 Matroska Foundation\r\n"));
        if (argc < 2 || ShowUsage)
        {
            TextWrite(StdErr,T("Usage: ") PROJECT_NAME T(" [options] <matroska_src> [matroska_dst]\r\n"));
		    TextWrite(StdErr,T("Options:\r\n"));
		    TextWrite(StdErr,T("  --keep-cues   keep the original Cues content and move it to the front\r\n"));
		    TextWrite(StdErr,T("  --remux       redo the Clusters layout\r\n"));
		    TextWrite(StdErr,T("  --doctype <v> force the doctype version\r\n"));
		    TextWrite(StdErr,T("    1: 'matroska' v1\r\n"));
		    TextWrite(StdErr,T("    2: 'matroska' v2\r\n"));
		    TextWrite(StdErr,T("    3: 'matroska' v3\r\n"));
		    TextWrite(StdErr,T("    4: 'webm'\r\n"));
		    TextWrite(StdErr,T("    5: 'matroska' v1 with DivX extensions\r\n"));
		    TextWrite(StdErr,T("    6: 'matroska' v4\r\n"));
		    TextWrite(StdErr,T("  --live        the output file resembles a live stream\r\n"));
		    TextWrite(StdErr,T("  --timecodescale <v> force the global TimestampScale to <v> (1000000 is a good value)\r\n"));
		    TextWrite(StdErr,T("  --unsafe      don't output elements that are used for file recovery (saves more space)\r\n"));
		    TextWrite(StdErr,T("  --optimize    use all possible optimization for the output file\r\n"));
		    TextWrite(StdErr,T("  --optimize_nv use all possible optimization for the output file, except video tracks\r\n"));
		    TextWrite(StdErr,T("  --no-optimize disable some optimization for the output file\r\n"));
		    TextWrite(StdErr,T("  --regression  the output file is suitable for regression tests\r\n"));
            TextWrite(StdErr,T("  --alt-3d <t>  the track with ID <v> has alternate 3D fields (left first)\r\n"));
		    TextWrite(StdErr,T("  --quiet       only output errors\r\n"));
            TextWrite(StdErr,T("  --version     show the version of ") PROJECT_NAME T("\r\n"));
            TextWrite(StdErr,T("  --help        show this screen\r\n"));
        }
        Path[0] = 0;
        Result = -1;
        goto exit;
    }

#if defined(_WIN32) && defined(UNICODE)
    Node_FromWcs(&p,Path,TSIZEOF(Path),argv[InputPathIndex]);
#else
    Node_FromUTF8(&p,Path,TSIZEOF(Path),argv[InputPathIndex]);
#endif
    Input = StreamOpen(&p,Path,SFLAG_RDONLY/*|SFLAG_BUFFERED*/);
    if (!Input)
    {
        TextPrintf(StdErr,T("Could not open file \"%s\" for reading\r\n"),Path);
        Path[0] = 0;
        Result = -2;
        goto exit;
    }

    TotalSize = Stream_Seek(Input, 0, SEEK_END);
    Stream_Seek(Input, 0, SEEK_SET);

    if (InputPathIndex==argc-1)
    {
        tchar_t Ext[MAXDATA];
#if 1
        std::string_view sPath = Path;
#if defined(_WIN32)
        auto slash = sPath.rfind("\\");
#else
        auto slash = sPath.rfind("/");
#endif
        std::string OutPath;
        std::string_view sFileName;
        if (slash == std::string_view::npos) // no path
        {
            sFileName = sPath;
        }
        else
        {
            OutPath = sPath.substr(0, slash + 1);
            sFileName = sPath.substr(slash + 1);
        }
        OutPath.append("clean.");
        OutPath.append(sFileName);
#else
        SplitPath(Path,Original,TSIZEOF(Original),String,TSIZEOF(String),Ext,TSIZEOF(Ext));
        if (!Original[0])
            Path[0] = 0;
        else
        {
            tcscpy_s(Path,TSIZEOF(Path),Original);
            AddPathDelimiter(Path,TSIZEOF(Path));
        }
        if (Ext[0])
            stcatprintf_s(Path,TSIZEOF(Path),T("clean.%s.%s"),String,Ext);
        else
            stcatprintf_s(Path,TSIZEOF(Path),T("clean.%s"),String);
#endif
    }
    else
#if defined(_WIN32) && defined(UNICODE)
        Node_FromWcs(&p,Path,TSIZEOF(Path),argv[argc-1]);
#else
        Node_FromStr(&p,Path,TSIZEOF(Path),argv[argc-1]);
#endif
    Output = StreamOpen(&p,Path,SFLAG_WRONLY|SFLAG_CREATE);
    if (!Output)
    {
        TextPrintf(StdErr,T("Could not open file \"%s\" for writing\r\n"),Path);
        Result = -3;
        goto exit;
    }

    // parse the source file to determine if it's a Matroska file and determine the location of the key parts
#if 1
    inputStream = std::make_unique<EbmlStream>(*Input);
    EbmlHead = (ebml_master*)EBML_FindNextElement(inputStream.get(), Context_KaxMatroska, &UpperElement, 0);
#else
    RContext.Context = KaxStream;
    RContext.EndPosition = INVALID_FILEPOS_T;
    RContext.UpContext = NULL;
    RContext.Profile = EBML_ANY_PROFILE;
    EbmlHead = (ebml_master*)EBML_FindNextElement(Input, &RContext, &UpperElement, 0);
#endif
    if (!EbmlHead || !EBML_ElementIsType((ebml_element*)EbmlHead, EBML_getContextHead()))
    {
        TextWrite(StdErr,T("EBML head not found! Are you sure it's a matroska/webm file?\r\n"));
        Result = -4;
        goto exit;
    }

#if 1
    RSegment = (ebml_master*)CheckMatroskaHead((ebml_element*)EbmlHead,inputStream.get());
#else
    RSegment = (ebml_master*)CheckMatroskaHead((ebml_element*)EbmlHead,&RContext,Input);
#endif
    if (SrcProfile==PROFILE_MATROSKA_V1 && DocVersion==2)
        SrcProfile = PROFILE_MATROSKA_V2;
    else if (SrcProfile==PROFILE_MATROSKA_V1 && DocVersion==3)
        SrcProfile = PROFILE_MATROSKA_V3;
    else if (SrcProfile==PROFILE_MATROSKA_V1 && DocVersion==4)
        SrcProfile = PROFILE_MATROSKA_V4;
    else if (SrcProfile==PROFILE_MATROSKA_V1 && DocVersion==5)
        SrcProfile = PROFILE_MATROSKA_V5;

	if (!DstProfile)
		DstProfile = SrcProfile;

    if (DstProfile==PROFILE_WEBM)
    {
        UnOptimize = 1;
        Optimize = 0;
    }

    if (!RSegment)
    {
        Result = -5;
        goto exit;
    }
    NodeDelete(EbmlHead);
    EbmlHead = NULL;

    if (Unsafe)
        ++TotalPhases;
    if (!Live)
        ++TotalPhases;

    if (EBML_ElementPositionEnd((ebml_element*)RSegment) != INVALID_FILEPOS_T)
        TotalSize = EBML_ElementPositionEnd((ebml_element*)RSegment);


    // locate the Segment Info, Track Info, Chapters, Tags, Attachments, Cues Clusters*
#if 1
    RLevel1 = (ebml_master*)EBML_FindNextElement(inputStream, EBML_CLASS_CONTEXT(KaxSegment), &UpperElement, 0);
#else
    RContext.EndPosition = TotalSize; // avoid reading too far some dummy/void elements for this segment
    RSegmentContext.Context = KaxSegment;
    RSegmentContext.EndPosition = EBML_ElementPositionEnd((ebml_element*)RSegment);
    RSegmentContext.UpContext = &RContext;
    RSegmentContext.Profile = SrcProfile;
	UpperElement = 0;
//TextPrintf(StdErr,T("Loading the level1 elements in memory\r\n"));
    RLevel1 = (ebml_master*)EBML_FindNextElement(Input, &RSegmentContext, &UpperElement, 1);
#endif
    while (RLevel1)
    {
        ShowProgress((ebml_element*)RLevel1, TotalSize);
        if (EBML_ElementIsType((ebml_element*)RLevel1, KaxInfo))
        {
            if (EBML_ElementReadData(RLevel1,Input,&RSegmentContext,1,SCOPE_ALL_DATA,0)==ERR_NONE)
                RSegmentInfo = RLevel1;
        }
        else if (EBML_ElementIsType((ebml_element*)RLevel1, KaxTracks))
        {
            if (EBML_ElementReadData(RLevel1,Input,&RSegmentContext,1,SCOPE_ALL_DATA,0)==ERR_NONE)
                RTrackInfo = RLevel1;
        }
        else if (!Live && EBML_ElementIsType((ebml_element*)RLevel1, KaxChapters))
        {
            if (EBML_ElementReadData(RLevel1,Input,&RSegmentContext,1,SCOPE_ALL_DATA,0)==ERR_NONE)
                RChapters = RLevel1;
        }
        else if (!Live && EBML_ElementIsType((ebml_element*)RLevel1, KaxTags))
        {
            if (EBML_ElementReadData(RLevel1,Input,&RSegmentContext,1,SCOPE_ALL_DATA,0)==ERR_NONE)
                RTags = RLevel1;
        }
        else if (!Live && EBML_ElementIsType((ebml_element*)RLevel1, KaxCues) && KeepCues)
        {
            if (EBML_ElementReadData(RLevel1,Input,&RSegmentContext,1,SCOPE_ALL_DATA,0)==ERR_NONE)
                RCues = RLevel1;
        }
        else if (!Live && EBML_ElementIsType((ebml_element*)RLevel1, KaxAttachments))
        {
            if (EBML_ElementReadData(RLevel1,Input,&RSegmentContext,1,SCOPE_ALL_DATA,0)==ERR_NONE)
                RAttachments = RLevel1;
        }
        else if (EBML_ElementIsType((ebml_element*)RLevel1, KaxCluster))
        {
			// only partially read the Cluster data (not the data inside the blocks)
            if (EBML_ElementReadData((ebml_element*)RLevel1,Input,&RSegmentContext,!Remux,SCOPE_PARTIAL_DATA,0)==ERR_NONE)
			{
                ArrayAppend(&RClusters,(matroska_cluster*)RLevel1,ebml_element*,256);
				// remove MATROSKA_ContextPosition and MATROSKA_ContextPrevSize until supported
				EbmlHead = (ebml_master*)EBML_MasterFindChild(RLevel1, KaxClusterPosition);
				if (EbmlHead)
					NodeDelete(EbmlHead);
				EbmlHead = (ebml_master*)EBML_MasterFindChild(RLevel1, KaxClusterPrevSize);
				if (EbmlHead)
					NodeDelete(EbmlHead);
				EbmlHead = NULL;
                RLevel1 = (ebml_master*)EBML_ElementSkipData((ebml_element*)RLevel1, *inputStream.get(), EBML_CLASS_CONTEXT(KaxSegment), NULL, 1);
                if (RLevel1 != NULL)
                    continue;
			}
        }
        else
		{
			EbmlHead = (ebml_master*)EBML_ElementSkipData((ebml_element*)RLevel1, *inputStream.get(), EBML_CLASS_CONTEXT(KaxSegment), NULL, 1);
			assert(EbmlHead==NULL);
            NodeDelete(RLevel1);
		}
#if 1
        RLevel1 = (ebml_master*)EBML_FindNextElement(inputStream, EBML_CLASS_CONTEXT(KaxSegment), &UpperElement, 0);
#else
        RLevel1 = (ebml_master*)EBML_FindNextElement(Input, &RSegmentContext, &UpperElement, 1);
#endif
    }
    EndProgress();

    if (!RSegmentInfo)
    {
        TextWrite(StdErr,T("The source Segment has no Segment Info section\r\n"));
        Result = -6;
        goto exit;
    }
    WSegmentInfo = (ebml_master*)EBML_ElementCopy(RSegmentInfo, NULL);
    EBML_MasterUseChecksum(WSegmentInfo,!Unsafe);

	TimeScale = EBML_MasterGetChild(WSegmentInfo,KaxTimestampScale, SrcProfile);
	if (!TimeScale)
	{
		TextWrite(StdErr,T("Failed to get the TimestampScale handle\r\n"));
		Result = -10;
		goto exit;
	}
    OldTimestampScale = EBML_IntegerValue((ebml_integer*)TimeScale);
    if (TimestampScale==0)
    {
        // avoid using a TimestampScale too small
        TimestampScale = OldTimestampScale;
        while (TimestampScale < 100000)
            TimestampScale <<= 1;
    }
    EBML_IntegerSetValue((ebml_integer*)TimeScale,TimestampScale);
	// RLevel1 = NULL;

    if (Live)
    {
	    // remove MATROSKA_ContextDuration from Live streams
	    ebml_float *Duration = (ebml_float*)EBML_MasterFindChild(WSegmentInfo, KaxDuration);
	    if (Duration)
		    NodeDelete(Duration);
    }
    else if (TimestampScale && TimestampScale != OldTimestampScale)
    {
	    ebml_float *Duration = (ebml_float*)EBML_MasterFindChild(WSegmentInfo, KaxDuration);
        if (Duration)
        {
            double duration = EBML_FloatValue(Duration);
            EBML_FloatSetValue(Duration, (duration * OldTimestampScale) / TimestampScale);
        }
    }

    // reorder elements in WSegmentInfo
    Elt2 = EBML_MasterFindChild(WSegmentInfo, KaxTimestampScale);
    if (Elt2)
        NodeTree_SetParent(Elt2,WSegmentInfo,EBML_MasterChildren(WSegmentInfo));
    if (!Elt2)
        Elt2 = *EBML_MasterChildren(WSegmentInfo);
    Elt = EBML_MasterFindChild(WSegmentInfo, KaxDuration);
    if (Elt)
        NodeTree_SetParent(Elt,WSegmentInfo,Elt2);

    if (!RTrackInfo && ARRAYCOUNT(RClusters,ebml_element*))
    {
        TextWrite(StdErr,T("The source Segment has no Track Info section\r\n"));
        Result = -7;
        goto exit;
    }

	if (RTrackInfo)
	{
		Result = CleanTracks(RTrackInfo, SrcProfile, &DstProfile, RAttachments, &Alternate3DTracks);
		if (Result!=0)
		{
			TextWrite(StdErr,T("No Tracks left to use!\r\n"));
			goto exit;
		}
		WTrackInfo = (ebml_master*)EBML_ElementCopy(RTrackInfo, NULL);
		if (WTrackInfo==NULL)
		{
			TextWrite(StdErr,T("Failed to copy the track info!\r\n"));
			goto exit;
		}
        EBML_MasterUseChecksum(WTrackInfo,!Unsafe);

		// count the max track number
		for (auto Elt=EBML_MasterChildren(WTrackInfo); EBML_MasterEnd(Elt,WTrackInfo); Elt=EBML_MasterNext(Elt))
		{
			if (EBML_ElementIsType(*Elt, KaxTrackEntry))
			{
                EBML_MasterUseChecksum((ebml_master*)&(*Elt),!Unsafe);
				auto Elt2 = EBML_MasterFindChild((ebml_master*)&(*Elt),KaxTrackNumber);
				if (Elt2 && (size_t)EBML_IntegerValue((ebml_integer*)&(*Elt2)) > MaxTrackNum)
					MaxTrackNum = (size_t)EBML_IntegerValue((ebml_integer*)&(*Elt2));
			}
		}

		// make sure the lacing flag is set on tracks that use it
		i = -1;
		for (auto Elt = EBML_MasterChildren(WTrackInfo);EBML_MasterEnd(Elt,WTrackInfo);Elt=EBML_MasterNext(Elt))
		{
			auto Elt2 = EBML_MasterFindChild((ebml_master*)&*Elt,KaxTrackNumber);
            if (Elt2)
			    i = std::max<int>(i,(int)EBML_IntegerValue((ebml_integer*)&*Elt2));
		}
		ArrayResize(&WTracks,i+1,track_info,0);
		ArrayZero(&WTracks);
	}

    // Write the EBMLHead
    EbmlHead = (ebml_master*)EBML_ElementCreate(&p,EBML_getContextHead(),0,EBML_ANY_PROFILE,NULL);
    if (!EbmlHead)
        goto exit;
    EBML_MasterUseChecksum(EbmlHead,!Unsafe);
    NodeTree_Clear((nodetree*)EbmlHead); // remove the default values
    // DocType
    ebml_string *DocType = EBML_MasterGetChild(EbmlHead,EBML_getContextDocType(), EBML_ANY_PROFILE);
    if (!DocType)
        goto exit;
    // assert(Node_IsPartOf(RLevel1,EBML_STRING_CLASS));
    if (DstProfile == PROFILE_WEBM)
    {
        if (EBML_StringSetValue(DocType,"webm") != ERR_NONE)
            goto exit;
    }
    else
    {
        if (EBML_StringSetValue(DocType,"matroska") != ERR_NONE)
            goto exit;
    }

    // Doctype version
	if (DstProfile==PROFILE_MATROSKA_V2 || DstProfile==PROFILE_WEBM)
		DocVersion=2;
	if (DstProfile==PROFILE_MATROSKA_V3)
		DocVersion=3;
	if (DstProfile==PROFILE_MATROSKA_V4)
		DocVersion=4;
	if (DstProfile==PROFILE_MATROSKA_V5)
		DocVersion=5;

    if (ARRAYCOUNT(Alternate3DTracks, block_info*) && DstProfile!=PROFILE_MATROSKA_V3 && DstProfile!=PROFILE_MATROSKA_V4 && DstProfile!=PROFILE_MATROSKA_V5)
    {
        TextPrintf(StdErr,T("Using --alt-3d in profile '%s' try \"--doctype %d\"\r\n"),GetProfileName(DstProfile),GetProfileId(PROFILE_MATROSKA_V3));
        goto exit;
    }

    ebml_integer *DocVer = EBML_MasterGetChild(EbmlHead,EBML_getContextDocTypeVersion(), EBML_ANY_PROFILE);
    if (!DocVer)
        goto exit;
    // assert(Node_IsPartOf(RLevel1,EBML_INTEGER_CLASS));
    EBML_IntegerSetValue((ebml_integer*)DocVer, DocVersion);

    // Doctype readable version
    DocVer = EBML_MasterGetChild(EbmlHead,EBML_getContextDocTypeReadVersion(), EBML_ANY_PROFILE);
    if (!DocVer)
        goto exit;
    // assert(Node_IsPartOf(RLevel1,EBML_INTEGER_CLASS));
    EBML_IntegerSetValue((ebml_integer*)DocVer, DocVersion);

    if (EBML_ElementRender((ebml_element*)EbmlHead,Output,1,0,1,ProfileFrom(DstProfile),NULL)!=ERR_NONE)
        goto exit;
    NodeDelete(EbmlHead);
    EbmlHead = NULL;
    RLevel1 = NULL;

    // Write the Matroska Segment Head
    WSegment = (ebml_master*)EBML_ElementCreate(&p,KaxSegment,0,DstProfile,NULL);
	if (Live)
		EBML_ElementSetInfiniteSize((ebml_element*)WSegment,1);
	else
    {
        // temporary value
        if (EBML_ElementIsFiniteSize((ebml_element*)RSegment))
		    EBML_ElementForceDataSize((ebml_element*)WSegment, EBML_ElementDataSize((ebml_element*)RSegment,0));
        else
            EBML_ElementSetSizeLength((ebml_element*)WSegment, EBML_MAX_SIZE);
    }
    if (EBML_ElementRenderHead(WSegment,Output,0,NULL,ProfileFrom(DstProfile))!=ERR_NONE)
    {
        TextWrite(StdErr,T("Failed to write the (temporary) Segment head\r\n"));
        Result = -10;
        goto exit;
    }

    //  Compute the Segment Info size
    ReduceSize((ebml_element*)WSegmentInfo);
    LibName = (ebml_unistring*)EBML_MasterFindFirstElt(WSegmentInfo, KaxMuxingApp, 1, 0, 0);
    EBML_StringGetUnicode(LibName,Original,TSIZEOF(Original));
    if (Regression)
        EBML_UniStringSetValue(LibName,T("libebml2 + libmatroska2"));
    else
    {
        // change the library names & app name
        std::stringstream ss;
        ss << EbmlCodeVersion << " + " << KaxCodeVersion;
        // stprintf_s(String,TSIZEOF(String),T("%s + %s"),Node_GetDataStr(&p,CONTEXT_LIBEBML_VERSION),Node_GetDataStr(&p,CONTEXT_LIBMATROSKA_VERSION));
        EBML_UniStringSetValue(LibName,String);
    }

    AppName = (ebml_unistring*)EBML_MasterFindFirstElt(WSegmentInfo, KaxWritingApp, 1, 0, 0);
    EBML_StringGetUnicode(AppName,String,TSIZEOF(String));
	ExtraSizeDiff = tcslen(String);
    if (!tcsisame_ascii(String,Original)) // libavformat writes the same twice, we only need one
    {
		if (Original[0])
			tcscat_s(Original,TSIZEOF(Original),T(" + "));
        tcscat_s(Original,TSIZEOF(Original),String);
    }
    s = Original;
    if (tcsnicmp_ascii(Original,T("mkclean "),8)==0)
        s += 14;
    if (Regression)
    {
        if (s[0])
            stprintf_s(String,TSIZEOF(String),PROJECT_NAME T(" regression from %s"),s);
        else
            stprintf_s(String,TSIZEOF(String),PROJECT_NAME T(" regression"));
    }
    else
    {
	    stprintf_s(String,TSIZEOF(String),PROJECT_NAME T(" ") PROJECT_VERSION);
	    if (Remux || Optimize || Live || UnOptimize || ARRAYCOUNT(Alternate3DTracks, block_info*))
		    tcscat_s(String,TSIZEOF(String),T(" "));
	    if (Remux)
		    tcscat_s(String,TSIZEOF(String),T("r"));
	    if (Optimize)
		    tcscat_s(String,TSIZEOF(String),T("o"));
	    if (Live)
		    tcscat_s(String,TSIZEOF(String),T("l"));
	    if (UnOptimize)
		    tcscat_s(String,TSIZEOF(String),T("u"));
	    if (ARRAYCOUNT(Alternate3DTracks, block_info*))
        {
		    tcscat_s(String,TSIZEOF(String),T("3"));
            Remux = 1;
        }
	    if (s[0])
		    stcatprintf_s(String,TSIZEOF(String),T(" from %s"),s);
    }
    EBML_UniStringSetValue(AppName,String);
	ExtraSizeDiff = tcslen(String) - ExtraSizeDiff + 2;

	if (Regression || Remux || !EBML_MasterFindChild(WSegmentInfo, KaxDateUTC))
	{
		auto RLevel1 = EBML_MasterGetChild(WSegmentInfo, KaxDateUTC, DstProfile);
        if (Regression)
            EBML_DateSetDateTime((ebml_date*)RLevel1, 1);
        else
		    EBML_DateSetDateTime((ebml_date*)RLevel1, GetTimeDate());
		RLevel1 = NULL;
	}

	if (!Live)
	{
		//  Prepare the Meta Seek with average values
		WMetaSeek = (ebml_master*)EBML_MasterAddElt(WSegment,KaxSeekHead,0, DstProfile);
        EBML_MasterUseChecksum(WMetaSeek,!Unsafe);
		EBML_ElementForcePosition((ebml_element*)WMetaSeek, Stream_Seek(Output,0,SEEK_CUR)); // keep the position for when we need to write it
        NextPos = 38 + 4* (Unsafe ? 17 : 23); // raw estimation of the SeekHead size
        if (RAttachments)
            NextPos += Unsafe ? 18 : 24;
        if (RChapters)
            NextPos += Unsafe ? 17 : 23;

		// segment info
		WSeekPoint = (matroska_seekpoint*)EBML_MasterAddElt(WMetaSeek,KaxSeek,0, DstProfile);
        EBML_MasterUseChecksum((ebml_master*)WSeekPoint,!Unsafe);
		EBML_ElementForcePosition((ebml_element*)WSegmentInfo, NextPos);
		NextPos += EBML_ElementFullSize((ebml_element*)WSegmentInfo,0) + 60; // 60 for the extra string we add
		MATROSKA_LinkMetaSeekElement(WSeekPoint,(ebml_element*)WSegmentInfo);

		// track info
		if (WTrackInfo)
		{
			WSeekPoint = (matroska_seekpoint*)EBML_MasterAddElt(WMetaSeek,KaxSeek,0, DstProfile);
            EBML_MasterUseChecksum((ebml_master*)WSeekPoint,!Unsafe);
			EBML_ElementForcePosition((ebml_element*)WTrackInfo, NextPos);
            EBML_ElementUpdateSize(WTrackInfo, 0, 0, ProfileFrom(DstProfile));
			NextPos += EBML_ElementFullSize((ebml_element*)WTrackInfo,0);
			MATROSKA_LinkMetaSeekElement(WSeekPoint,(ebml_element*)WTrackInfo);
		}
		else
		{
			TextWrite(StdErr,T("Warning: the source Segment has no Track Info section (can be a chapter file)\r\n"));
		}

		// chapters
		if (RChapters)
		{
			ReduceSize((ebml_element*)RChapters);
            if (EBML_MasterUseChecksum(RChapters,!Unsafe))
                EBML_ElementUpdateSize(RChapters, 0, 0, ProfileFrom(DstProfile));
			if (!EBML_MasterCheckMandatory(RChapters,0, ProfileFrom(DstProfile)))
			{
				TextWrite(StdErr,T("The Chapters section is missing mandatory elements, skipping\r\n"));
				NodeDelete(RChapters);
				RChapters = NULL;
			}
			else
			{
				WSeekPoint = (matroska_seekpoint*)EBML_MasterAddElt(WMetaSeek,KaxSeek,0, DstProfile);
                EBML_MasterUseChecksum((ebml_master*)WSeekPoint,!Unsafe);
				EBML_ElementForcePosition((ebml_element*)RChapters, NextPos);
				NextPos += EBML_ElementFullSize((ebml_element*)RChapters,0);
				MATROSKA_LinkMetaSeekElement(WSeekPoint,(ebml_element*)RChapters);
			}
		}

		// attachments
		if (RAttachments)
		{
			MATROSKA_AttachmentSort(RAttachments);
			ReduceSize((ebml_element*)RAttachments);
            if (EBML_MasterUseChecksum(RAttachments,!Unsafe))
                EBML_ElementUpdateSize(RAttachments, 0, 0, ProfileFrom(DstProfile));
			if (!EBML_MasterCheckMandatory(RAttachments,0, ProfileFrom(DstProfile)))
			{
				TextWrite(StdErr,T("The Attachments section is missing mandatory elements, skipping\r\n"));
				NodeDelete(RAttachments);
				RAttachments = NULL;
			}
			else
			{
				WSeekPoint = (matroska_seekpoint*)EBML_MasterAddElt(WMetaSeek,KaxSeek,0, DstProfile);
                EBML_MasterUseChecksum((ebml_master*)WSeekPoint,!Unsafe);
				EBML_ElementForcePosition((ebml_element*)RAttachments, NextPos);
				NextPos += EBML_ElementFullSize((ebml_element*)RAttachments,0);
				MATROSKA_LinkMetaSeekElement(WSeekPoint,(ebml_element*)RAttachments);
			}
		}

		// tags
		if (RTags)
		{
			ReduceSize((ebml_element*)RTags);
            if (EBML_MasterUseChecksum(RTags,!Unsafe))
                EBML_ElementUpdateSize(RTags, 0, 0, ProfileFrom(DstProfile));
			if (!EBML_MasterCheckMandatory(RTags,0, ProfileFrom(DstProfile)))
			{
				TextWrite(StdErr,T("The Tags section is missing mandatory elements, skipping\r\n"));
				NodeDelete(RTags);
				RTags = NULL;
			}
			else
			{
				WSeekPoint = (matroska_seekpoint*)EBML_MasterAddElt(WMetaSeek,KaxSeek,0, DstProfile);
                EBML_MasterUseChecksum((ebml_master*)WSeekPoint,!Unsafe);
				EBML_ElementForcePosition((ebml_element*)RTags, NextPos);
				NextPos += EBML_ElementFullSize((ebml_element*)RTags,0);
				MATROSKA_LinkMetaSeekElement(WSeekPoint,(ebml_element*)RTags);
			}
		}
	}
	else
	{
		WriteJunk(Output,134);
		NextPos += 134;
	}

    Result = LinkClusters(&RClusters,RSegmentInfo,RTrackInfo,DstProfile, &WTracks, Live?12345:INVALID_TIMESTAMP_T);
	if (Result!=0)
		goto exit;

    // use the output track settings for each block
    for (auto Cluster = ARRAYBEGIN(*Clusters,ebml_master*);Cluster != ARRAYEND(*Clusters,ebml_master*); ++Cluster)
    {
        array_ebml_element_p::iterator RLevel1;
        //EBML_MasterUseChecksum((ebml_master*)*Cluster,!Unsafe);
        for (auto Elt = EBML_MasterChildren(*Cluster);EBML_MasterEnd(Elt,*Cluster);Elt=RLevel1)
        {
            RLevel1 = EBML_MasterNext(Elt);
            if (EBML_ElementIsType(*Elt, KaxBlockGroup))
            {
                for (auto Elt2 = EBML_MasterChildren((KaxBlockGroup*)*Elt);EBML_MasterEnd(Elt2,(KaxBlockGroup*)*Elt);Elt2=EBML_MasterNext(Elt2))
                {
                    if (EBML_ElementIsType(*Elt2, KaxBlock))
                    {
                        if (MATROSKA_LinkBlockWithWriteTracks((KaxBlock*)*Elt2,WTrackInfo,DstProfile)!=ERR_NONE)
                            NodeDelete(*Elt);
                        else if (MATROSKA_LinkBlockWriteSegmentInfo((KaxBlock*)*Elt2,WSegmentInfo)!=ERR_NONE)
                            NodeDelete(*Elt);
                        break;
                    }
                }
            }
            else if (EBML_ElementIsType(*Elt, KaxSimpleBlock))
            {
                if (MATROSKA_LinkBlockWithWriteTracks((KaxSimpleBlock*)*Elt,WTrackInfo,DstProfile)!=ERR_NONE)
                    NodeDelete(*Elt);
                else if (MATROSKA_LinkBlockWriteSegmentInfo((KaxSimpleBlock*)*Elt,WSegmentInfo)!=ERR_NONE)
                    NodeDelete(*Elt);
            }
        }
	    //EBML_ElementUpdateSize(*Cluster, 0, 0);
    }

    if (Optimize && !UnOptimize)
    {
        int16_t BlockTrack;
        array_ebml_element_p::iterator Block;
        KaxBlock *GBlock;
        array_matroska_cluster_p::iterator ClusterR;

	    if (!Quiet) TextWrite(StdErr,T("Optimizing...\r\n"));

		ArrayResize(&TrackMaxHeader, MaxTrackNum+1, binary, 0);
		ArrayZero(&TrackMaxHeader);
        for (i=0;(size_t)i<=MaxTrackNum;++i) {
            bool_t OptimizeTrack = 1;
            if (!OptimizeVideo)
            {
                ebml_master *Track;
	            for (Track = (ebml_master*)EBML_MasterFindChild(WTrackInfo,KaxTrackEntry); Track; Track=EBML_MasterNextChild(WTrackInfo,Track))
	            {
		            Elt = EBML_MasterFindChild(Track,KaxTrackNumber);
		            if (EBML_IntegerValue((ebml_integer*)Elt) != i)
                        continue;
		            Elt = EBML_MasterFindChild(Track,KaxTrackType);
		            if (EBML_IntegerValue((ebml_integer*)Elt) == MATROSKA_TRACK_TYPE_VIDEO)
                    {
                        Elt = EBML_MasterFindChild(Track,KaxContentEncodings);
                        NodeDelete(Elt);
			            OptimizeTrack = 0;
                    }
		            break;
	            }
            }
            if (OptimizeTrack)
                InitCommonHeader(&ARRAYBEGIN(TrackMaxHeader,binary)[i]);
        }

	    for (ClusterR=ARRAYBEGIN(RClusters,matroska_cluster*);ClusterR!=ARRAYEND(RClusters,matroska_cluster*);++ClusterR)
	    {
		    for (Block = EBML_MasterChildren(*ClusterR);EBML_MasterEnd(Block,*ClusterR);Block=EBML_MasterNext(Block))
		    {
			    if (EBML_ElementIsType(*Block, KaxBlockGroup))
			    {
				    GBlock = EBML_MasterFindChild((KaxBlockGroup*)*Block, KaxBlock);
				    if (GBlock)
				    {
					    BlockTrack = MATROSKA_BlockTrackNum((matroska_block*)&(*GBlock));
                        ShrinkCommonHeader(&TrackMaxHeader+BlockTrack/*ARRAYBEGIN(TrackMaxHeader,binary) FIXME [BlockTrack]*/, GBlock, Input);
				    }
			    }
			    else if (EBML_ElementIsType(*Block, KaxSimpleBlock))
			    {
				    BlockTrack = MATROSKA_BlockTrackNum((KaxSimpleBlock*)*Block);
                    ShrinkCommonHeader(&TrackMaxHeader+BlockTrack/*ARRAYBEGIN(TrackMaxHeader,binary) FIXME [BlockTrack]*/, (KaxSimpleBlock*)*Block, Input);
			    }
		    }
	    }

        for (i=0;(size_t)i<=MaxTrackNum;++i)
            ClearCommonHeader(&TrackMaxHeader+i/*ARRAYBEGIN(TrackMaxHeader,binary) FIXME [i]*/);
    }

	if (Remux && WTrackInfo)
	{
		// create WClusters
		array_matroska_cluster_p::iterator ClusterR;
		matroska_cluster *ClusterW;
	    EBML_MASTER_CONST_ITERATOR Block;
        ebml_element *GBlock;
		ebml_master *Track;
        matroska_block *Block1;
		mkv_timestamp_t Prev = INVALID_TIMESTAMP_T, *Tst, BlockTime, BlockDuration, MasterEndTimestamp, BlockEnd, MainBlockEnd;
		size_t MainTrack, BlockTrack;
        size_t Frame, *pTrackOrder;
		bool_t Deleted;
		array KeyFrameTimestamps, *pTrackBlock;
		array_size_t TrackBlockCurrIdx, TrackOrder;
        array_array_block_info TrackBlocks;
        // matroska_frame FrameData;
		block_info BlockInfo;
        array_block_info::iterator pBlockInfo;
		err_t Result;

		if (!Quiet) TextWrite(StdErr,T("Remuxing...\r\n"));
		// count the number of useful tracks
		Frame = 0;
		for (auto Track=EBML_MasterChildren(WTrackInfo); EBML_MasterEnd(Track,WTrackInfo); Track=EBML_MasterNext(Track))
		{
			if (EBML_ElementIsType((ebml_element*)&*Track, KaxTrackEntry))
				++Frame;
		}

		ArrayInit(&TrackBlocks);
		ArrayResize(&TrackBlocks, MaxTrackNum+1, array_block_info, 0);
		ArrayZero(&TrackBlocks);

		ArrayInit(&TrackBlockCurrIdx);
		ArrayResize(&TrackBlockCurrIdx, MaxTrackNum+1, size_t, 0);
		ArrayZero(&TrackBlockCurrIdx);

		ArrayInit(&TrackOrder);

		// fill TrackBlocks with all the Blocks per track
		BlockInfo.DecodeTime = INVALID_TIMESTAMP_T;
		BlockInfo.FrameStartIndex = 0;

		for (ClusterR=ARRAYBEGIN(RClusters,matroska_cluster*);ClusterR!=ARRAYEND(RClusters,matroska_cluster*);++ClusterR)
		{
			for (Block = EBML_MasterChildren(*ClusterR);EBML_MasterEnd(Block,*ClusterR);Block=EBML_MasterNext(Block))
			{
				if (EBML_ElementIsType(*Block, KaxBlockGroup))
				{
					GBlock = EBML_MasterFindChild((KaxBlockGroup*)&*Block, KaxBlock);
					if (GBlock)
					{
						BlockTrack = MATROSKA_BlockTrackNum((matroska_block*)GBlock);
						BlockInfo.Block = (KaxBlock*)GBlock;
						ArrayAppend(&ARRAYAT(TrackBlocks,array_block_info,BlockTrack),BlockInfo,array_block_info,1024);
					}
				}
				else if (EBML_ElementIsType(*Block, KaxSimpleBlock))
				{
					BlockTrack = MATROSKA_BlockTrackNum((matroska_block*)&(*Block));
					BlockInfo.Block = (KaxSimpleBlock*)*Block;
					ArrayAppend(&ARRAYAT(TrackBlocks,array_block_info,BlockTrack),BlockInfo,array_block_info,1024);
				}
			}
		}

		// determine what is the main track num (video track)
		for (;;)
		{
			bool_t Exit = 1;
			ArrayResize(&TrackOrder, Frame, size_t, 0);
			ArrayZero(&TrackOrder);
			Track = GetMainTrack(WTrackInfo,&TrackOrder);
			if (!Track)
			{
				TextWrite(StdErr,T("Impossible to remux without a proper track to use\r\n"));
				goto exit;
			}
			Elt = EBML_MasterFindChild(Track,KaxTrackNumber);
			assert(Elt!=NULL);
			MainTrack = (int16_t)EBML_IntegerValue((ebml_integer*)Elt);

			for (auto pTrackOrder=ARRAYBEGIN(TrackOrder,size_t);pTrackOrder!=ARRAYEND(TrackOrder,size_t);++pTrackOrder)
			{
				if (!ARRAYCOUNT(ARRAYBEGIN(TrackBlocks,array_block_info)[*pTrackOrder],block_info))
				{
					if (!Quiet) TextPrintf(StdErr,T("Track %d has no blocks! Deleting...\r\n"),(int)*pTrackOrder);
					for (auto Elt = EBML_MasterFindChild(WTrackInfo,KaxTrackEntry); Elt; Elt=EBML_MasterNextChild(WTrackInfo,Elt))
					{
						if (EBML_IntegerValue((ebml_integer*)EBML_MasterFindChild((ebml_master*)Elt,KaxTrackNumber))==*pTrackOrder)
						{
							NodeDelete(Elt);
							--Frame;
        					Exit = 0;
							break;
						}
					}
				}
			}
			if (Exit)
				break;
		}

		// process the decoding time for all blocks
		for (auto pTrackBlock=ARRAYBEGIN(TrackBlocks,array_block_info);pTrackBlock!=ARRAYEND(TrackBlocks,array_block_info);++pTrackBlock)
		{
			BlockEnd = INVALID_TIMESTAMP_T;
			for (pBlockInfo=ARRAYBEGIN(*pTrackBlock,block_info);pBlockInfo!=ARRAYEND(*pTrackBlock,block_info);++pBlockInfo)
			{
				BlockTime = MATROSKA_BlockTimestamp((*pBlockInfo).Block);
				pBlockInfo->DecodeTime = BlockTime;
				if ((pBlockInfo+1)!=ARRAYEND(*pTrackBlock,block_info) && BlockEnd!=INVALID_TIMESTAMP_T)
				{
					BlockDuration = MATROSKA_BlockTimestamp((*(pBlockInfo+1)).Block);
					if (BlockTime > BlockDuration)
					{
						//assert(BlockDuration > BlockEnd);
						pBlockInfo->DecodeTime = BlockEnd + ((BlockTime - BlockDuration) >> 2);
					}
				}
				BlockEnd = pBlockInfo->DecodeTime;
			}
		}

		// get all the keyframe timestamps for our main track
		ArrayInit(&KeyFrameTimestamps);
		pTrackBlock=ARRAYAT(TrackBlocks,array_block_info,MainTrack);
		for (pBlockInfo=ARRAYBEGIN(*pTrackBlock,block_info);pBlockInfo!=ARRAYEND(*pTrackBlock,block_info);++pBlockInfo)
		{
			if (MATROSKA_BlockKeyframe(pBlockInfo->Block))
			{
				assert(pBlockInfo->DecodeTime == MATROSKA_BlockTimestamp(pBlockInfo->Block));
				ArrayAppend(&KeyFrameTimestamps,&pBlockInfo->DecodeTime,sizeof(pBlockInfo->DecodeTime),256);
			}
		}
		if (!ARRAYCOUNT(KeyFrameTimestamps,mkv_timestamp_t))
		{
			TextPrintf(StdErr,T("Impossible to remux, no keyframe found for track %d\r\n"),(int)MainTrack);
			goto exit;
		}

		// \todo sort Blocks of all tracks (according to the ref frame when available)
		// sort the timestamps, just in case the file wasn't properly muxed
		//ArraySort(&KeyFrameTimestamps, mkv_timestamp_t, (arraycmp)TimcodeCmp, NULL, 1);

		// discrimate the timestamps we want to use as cluster boundaries
		//   create a new Cluster no shorter than 1s (unless the next one is too distant like 2s)
		Prev = INVALID_TIMESTAMP_T;
		for (Tst = ARRAYBEGIN(KeyFrameTimestamps, mkv_timestamp_t); Tst!=ARRAYEND(KeyFrameTimestamps, mkv_timestamp_t);)
		{
			Deleted = 0;
			if (Prev!=INVALID_TIMESTAMP_T && *Tst < Prev + 1000000000)
			{
				// too close
				if (Tst+1 != ARRAYEND(KeyFrameTimestamps, mkv_timestamp_t) && *(Tst+1) < Prev + 2000000000)
				{
					ArrayRemove(&KeyFrameTimestamps, mkv_timestamp_t, Tst, (arraycmp)TimcodeCmp, NULL);
					Deleted = 1;
				}
			}
			if (!Deleted)
				Prev = *Tst++;
		}

		// create each Cluster
		if (!Quiet) TextWrite(StdErr,T("Reclustering...\r\n"));
		for (Tst = ARRAYBEGIN(KeyFrameTimestamps, mkv_timestamp_t); Tst!=ARRAYEND(KeyFrameTimestamps, mkv_timestamp_t); ++Tst)
		{
			bool_t ReachedClusterEnd = 0;
			ClusterW = (matroska_cluster*)EBML_ElementCreate(Track, KaxCluster, 0, DstProfile, NULL);
			ArrayAppend(&WClusters,&ClusterW,sizeof(ClusterW),256);
			MATROSKA_LinkClusterReadSegmentInfo(ClusterW, RSegmentInfo, 1);
			MATROSKA_LinkClusterWriteSegmentInfo(ClusterW, WSegmentInfo);
			MATROSKA_ClusterSetTimestamp(ClusterW,*Tst); // \todo avoid having negative timestamps in the Cluster ?

			if ((Tst+1)==ARRAYEND(KeyFrameTimestamps, mkv_timestamp_t))
				MasterEndTimestamp = INVALID_TIMESTAMP_T;
			else
				MasterEndTimestamp = *(Tst+1);

			while (!ReachedClusterEnd && ARRAYBEGIN(TrackBlockCurrIdx,size_t)[MainTrack] != ARRAYCOUNT(ARRAYBEGIN(TrackBlocks,array_block_info)[MainTrack],block_info))
			{
				// next Block end in the master track
				if (ARRAYBEGIN(TrackBlockCurrIdx,size_t)[MainTrack]+1 == ARRAYCOUNT(ARRAYBEGIN(TrackBlocks,array_block_info)[MainTrack],block_info))
					MainBlockEnd = INVALID_TIMESTAMP_T;
				else
				{
					pBlockInfo = ARRAYBEGIN(ARRAYBEGIN(TrackBlocks,array_block_info)[MainTrack],block_info) + ARRAYBEGIN(TrackBlockCurrIdx,size_t)[MainTrack] + 1;
					MainBlockEnd = pBlockInfo->DecodeTime;
				}

                if (EBML_ElementPosition((ebml_element*)ClusterW) == INVALID_FILEPOS_T)
                    EBML_ElementForcePosition((ebml_element*)ClusterW, EBML_ElementPosition((ebml_element*)pBlockInfo->Block)); // fake average value

                // loop on all tracks in their specified order
				for (auto pTrackOrder=ARRAYBEGIN(TrackOrder,size_t);pTrackOrder!=ARRAYEND(TrackOrder,size_t);++pTrackOrder)
				{
					// output all the blocks until MainBlockEnd (included) for this track
					while (ARRAYBEGIN(TrackBlockCurrIdx,size_t)[*pTrackOrder] < ARRAYCOUNT(ARRAYBEGIN(TrackBlocks,array_block_info)[*pTrackOrder],block_info))
					{
						// End of the current Block
						if (ARRAYBEGIN(TrackBlockCurrIdx,size_t)[*pTrackOrder]+1 == ARRAYCOUNT(ARRAYBEGIN(TrackBlocks,array_block_info)[*pTrackOrder],block_info))
							BlockEnd = INVALID_TIMESTAMP_T;
						else
						{
							pBlockInfo = ARRAYBEGIN(ARRAYBEGIN(TrackBlocks,array_block_info)[*pTrackOrder],block_info) + ARRAYBEGIN(TrackBlockCurrIdx,size_t)[*pTrackOrder] + 1;
							BlockEnd = pBlockInfo->DecodeTime;
						}

						pBlockInfo = ARRAYBEGIN(ARRAYBEGIN(TrackBlocks,array_block_info)[*pTrackOrder],block_info) + ARRAYBEGIN(TrackBlockCurrIdx,size_t)[*pTrackOrder];
						if (pBlockInfo->DecodeTime > MainBlockEnd && *pTrackOrder!=MainTrack)
                        {
                            if (ARRAYCOUNT(Alternate3DTracks, block_info*) && ARRAYBEGIN(Alternate3DTracks, block_info*)[*pTrackOrder])
                                ARRAYBEGIN(Alternate3DTracks, block_info*)[*pTrackOrder] = NULL;
							break; // next track around this timestamp
                        }

						if (pBlockInfo->FrameStartIndex!=0)
						{
                            // use the frames left from the previous block
							if (EBML_ElementIsType((ebml_element*)pBlockInfo->Block, KaxSimpleBlock))
							{
                                Block1 = (matroska_block*)EBML_ElementCopy(pBlockInfo->Block, NULL);
								MATROSKA_LinkBlockWriteSegmentInfo(Block1,WSegmentInfo);

								for (; pBlockInfo->FrameStartIndex < MATROSKA_BlockGetFrameCount(pBlockInfo->Block); ++pBlockInfo->FrameStartIndex)
								{
									if (MATROSKA_BlockGetFrameEnd(pBlockInfo->Block,pBlockInfo->FrameStartIndex) >= MasterEndTimestamp)
										break;
									MATROSKA_BlockGetFrame(pBlockInfo->Block, pBlockInfo->FrameStartIndex, &FrameData, 1);
									MATROSKA_BlockAppendFrame(Block1, &FrameData, *Tst);
								}

								if (MATROSKA_BlockGetFrameCount(Block1))
									EBML_MasterAppend((ebml_master*)ClusterW,(ebml_element*)Block1);
								else
									NodeDelete(Block1);

								if (pBlockInfo->FrameStartIndex!=MATROSKA_BlockGetFrameCount(pBlockInfo->Block))
									break; // next track
								else
								{
									ARRAYBEGIN(TrackBlockCurrIdx,size_t)[*pTrackOrder]++;
									continue;
								}
							}
							else if (EBML_ElementIsType((ebml_element*)pBlockInfo->Block, KaxBlock))
							{
                                Elt = EBML_ElementCopy(NodeTree_Parent(pBlockInfo->Block), NULL);
								Block1 = (matroska_block*)EBML_MasterFindChild((ebml_master*)Elt, KaxBlock);
								MATROSKA_LinkBlockWriteSegmentInfo(Block1,WSegmentInfo);

								for (; pBlockInfo->FrameStartIndex < MATROSKA_BlockGetFrameCount(pBlockInfo->Block); ++pBlockInfo->FrameStartIndex)
								{
									if (MATROSKA_BlockGetFrameEnd(pBlockInfo->Block,pBlockInfo->FrameStartIndex) >= MasterEndTimestamp)
										break;
									MATROSKA_BlockGetFrame(pBlockInfo->Block, pBlockInfo->FrameStartIndex, &FrameData, 1);
									MATROSKA_BlockAppendFrame(Block1, &FrameData, *Tst);
								}

								if (MATROSKA_BlockGetFrameCount(Block1))
									EBML_MasterAppend((ebml_master*)ClusterW,(ebml_element*)Elt);
								else
									NodeDelete(Elt);

								if (pBlockInfo->FrameStartIndex!=MATROSKA_BlockGetFrameCount(pBlockInfo->Block))
									break; // next track
								else
								{
									ARRAYBEGIN(TrackBlockCurrIdx,size_t)[*pTrackOrder]++;
									continue;
								}
							}
						}

						if (MainBlockEnd!=INVALID_TIMESTAMP_T && BlockEnd>MasterEndTimestamp && *pTrackOrder!=MainTrack && MATROSKA_BlockLaced(pBlockInfo->Block))
						{
							// relacing
                            //TextPrintf(StdErr,T("\rRelacing block track %d at %") TPRId64 T(" ends %") TPRId64 T(" next cluster at %") TPRId64 T("\r\n"),*pTrackOrder,pBlockInfo->DecodeTime,BlockEnd,MasterEndTimestamp);
                            if (MATROSKA_BlockReadData(pBlockInfo->Block,Input,SrcProfile)==ERR_NONE)
                            {
						        bool_t HasDuration = MATROSKA_BlockProcessFrameDurations(pBlockInfo->Block,Input,SrcProfile)==ERR_NONE;
							    Result = ERR_NONE;
							    if (EBML_ElementIsType((ebml_element*)pBlockInfo->Block, KaxSimpleBlock))
							    {
								    // This block needs to be split
                                    Block1 = (matroska_block*)EBML_ElementCopy(pBlockInfo->Block, NULL);
							        MATROSKA_LinkBlockWriteSegmentInfo(Block1,WSegmentInfo);

							        for (; pBlockInfo->FrameStartIndex < MATROSKA_BlockGetFrameCount(pBlockInfo->Block); ++pBlockInfo->FrameStartIndex)
							        {
								        if (HasDuration && MATROSKA_BlockGetFrameEnd(pBlockInfo->Block,pBlockInfo->FrameStartIndex) >= MasterEndTimestamp)
									        break;
								        MATROSKA_BlockGetFrame(pBlockInfo->Block, pBlockInfo->FrameStartIndex, &FrameData, 1);
								        MATROSKA_BlockAppendFrame(Block1, &FrameData, *Tst);
							        }

							        if (MATROSKA_BlockGetFrameCount(Block1)==MATROSKA_BlockGetFrameCount(pBlockInfo->Block))
							        {
								        pBlockInfo->FrameStartIndex = 0; // all the frames are for the next Cluster
								        NodeDelete(Block1);
							        }
							        else
							        {
								        if (MATROSKA_BlockGetFrameCount(Block1))
									        Result = EBML_MasterAppend((ebml_master*)ClusterW,(ebml_element*)Block1);
								        else
									        NodeDelete(Block1);
							        }
							        break; // next track
							    }
							    else
							    {
								    assert(EBML_ElementIsType((ebml_element*)pBlockInfo->Block, KaxBlock));
								    // This block needs to be split
                                    Elt = EBML_ElementCopy(NodeTree_Parent(pBlockInfo->Block), NULL);
								    Block1 = (matroska_block*)EBML_MasterFindChild((ebml_master*)Elt, KaxBlock);
							        MATROSKA_LinkBlockWriteSegmentInfo(Block1,WSegmentInfo);

							        for (; pBlockInfo->FrameStartIndex < MATROSKA_BlockGetFrameCount(pBlockInfo->Block); ++pBlockInfo->FrameStartIndex)
							        {
								        if (HasDuration && MATROSKA_BlockGetFrameEnd(pBlockInfo->Block,pBlockInfo->FrameStartIndex) >= MasterEndTimestamp)
									        break;
								        MATROSKA_BlockGetFrame(pBlockInfo->Block, pBlockInfo->FrameStartIndex, &FrameData, 1);
								        MATROSKA_BlockAppendFrame(Block1, &FrameData, *Tst);
							        }

							        if (MATROSKA_BlockGetFrameCount(Block1)==MATROSKA_BlockGetFrameCount(pBlockInfo->Block))
							        {
								        pBlockInfo->FrameStartIndex = 0; // all the frames are for the next Cluster
								        NodeDelete(Elt);
							        }
							        else
							        {
								        if (MATROSKA_BlockGetFrameCount(Block1))
									        Result = EBML_MasterAppend((ebml_master*)ClusterW,Elt);
								        else
									        NodeDelete(Elt);
							        }
							        break; // next track
							    }

							    if (Result != ERR_NONE)
							    {
								    if (Result==ERR_INVALID_DATA)
									    TextPrintf(StdErr,T("Impossible to remux, the TimestampScale may be too low, try --timecodescale 1000000\r\n"));
								    else
									    TextPrintf(StdErr,T("Impossible to remux, error appending a block\r\n"));
								    Result = -46;
								    goto exit;
							    }
                                MATROSKA_BlockReleaseData(pBlockInfo->Block,0);
                            }
						}

						if (MATROSKA_BlockGetFrameCount(pBlockInfo->Block))
						{
                            block_info *prevBlock = NULL;
                            if (ARRAYCOUNT(Alternate3DTracks, block_info*) && ARRAYBEGIN(Alternate3DTracks, block_info*)[*pTrackOrder])
                            {
                                prevBlock = ARRAYBEGIN(Alternate3DTracks, block_info*)[*pTrackOrder];
                                if (prevBlock==MARKER3D)
                                {
                                    ARRAYBEGIN(Alternate3DTracks, block_info*)[*pTrackOrder] = pBlockInfo;
                                    prevBlock = NULL;
                                }
                                else if (MATROSKA_BlockGetFrameCount(prevBlock->Block) >= 2)
                                {
                                    ARRAYBEGIN(Alternate3DTracks, block_info*)[*pTrackOrder] = pBlockInfo;
                                    prevBlock = NULL;
                                }
                                else
                                {
                                    Result = MATROSKA_BlockReadData(prevBlock->Block,Input,SrcProfile);
                                }
                            }

                            if (prevBlock)
                            {
                                // add the first frame into the previous Block
                                if (Result==ERR_NONE)
                                {
                                    Result = MATROSKA_BlockReadData(pBlockInfo->Block,Input,SrcProfile);
                                    if (EBML_ElementIsType((ebml_element*)pBlockInfo->Block, KaxSimpleBlock))
                                    {
                                        Block1 = (matroska_block*)pBlockInfo->Block;
                                    }
                                    else
                                    {
                                        Elt = EBML_ElementCopy(NodeTree_Parent(pBlockInfo->Block), NULL);
								        Block1 = (matroska_block*)EBML_MasterFindChild((ebml_master*)Elt, KaxBlock);
                                    }
                                    assert(!MATROSKA_BlockIsKeyframe(Block1));
						            MATROSKA_BlockGetFrame(Block1, pBlockInfo->FrameStartIndex, &FrameData, 1);
                                    MATROSKA_BlockAppendFrame(prevBlock->Block, &FrameData, *Tst);
                                    MATROSKA_BlockReleaseData(pBlockInfo->Block,0);
                                }
                                MATROSKA_BlockReleaseData(prevBlock->Block,0);
                            }
                            else
                            {
							    if (EBML_ElementIsType((ebml_element*)pBlockInfo->Block, KaxSimpleBlock))
                                {
						            Result = MATROSKA_LinkBlockWriteSegmentInfo(pBlockInfo->Block,WSegmentInfo);
                                    if (Result == ERR_NONE)
							            Result = EBML_MasterAppend((ebml_master*)ClusterW,(ebml_element*)pBlockInfo->Block);
                                }
							    else
							    {
                                    assert(EBML_ElementIsType((ebml_element*)pBlockInfo->Block, KaxBlock));
                                    Result = MATROSKA_LinkBlockWriteSegmentInfo(pBlockInfo->Block,WSegmentInfo);
                                    if (Result == ERR_NONE)
                                        Result = EBML_MasterAppend((ebml_master*)ClusterW,EBML_ElementParent((ebml_element*)pBlockInfo->Block));
							    }
                            }

							if (Result != ERR_NONE)
							{
								if (Result==ERR_INVALID_DATA)
									TextPrintf(StdErr,T("Impossible to remux, the TimestampScale may be too low, try --timecodescale 1000000\r\n"));
								else
									TextPrintf(StdErr,T("Impossible to remux, error appending a block\r\n"));
								Result = -46;
								goto exit;
							}
							ARRAYBEGIN(TrackBlockCurrIdx,size_t)[*pTrackOrder]++;
						}

						if (*pTrackOrder==MainTrack)
						{
							if (MainBlockEnd == INVALID_TIMESTAMP_T || BlockEnd == MasterEndTimestamp)
								ReachedClusterEnd = 1;
							break;
						}
					}
				}
			}
		}

		ArrayClear(&KeyFrameTimestamps);
		ArrayClear(&TrackBlocks);
		ArrayClear(&TrackBlockCurrIdx);
		ArrayClear(&TrackOrder);

		Clusters = &WClusters;
		NodeDelete(RCues);
		RCues = NULL;
	}

	if (WTrackInfo)
	{
        array *HeaderData = NULL;
        ebml_binary *CodecPrivate;
        ebml_master *Encodings;
        size_t TrackNum;
        tchar_t CodecID[MAXDATA];
		// fix/clean the Lacing flag for each track
		//assert(KaxTrackFlagLacing->DefaultValue==1);
		for (RLevel1 = EBML_MasterChildren(WTrackInfo); EBML_MasterEnd(RLevel1,WTrackInfo); RLevel1=EBML_MasterNext(RLevel1))
		{
            if (EBML_ElementIsType((ebml_element*)RLevel1, KaxTrackEntry))
            {
                MatroskaTrackEncodingCompAlgo encoding = MATROSKA_TRACK_ENCODING_COMP_NONE;
                MatroskaContentEncodingScope zlib_scope = MATROSKA_CONTENTENCODINGSCOPE_BLOCK;

                Elt2 = EBML_MasterFindChild(RLevel1,KaxTrackNumber);
			    if (!Elt2) continue;
                TrackNum = (size_t)EBML_IntegerValue((ebml_integer*)Elt2);
			    if (ARRAYBEGIN(WTracks,track_info)[TrackNum].IsLaced)
			    {
				    // has lacing
				    Elt2 = EBML_MasterFindChild(RLevel1,KaxTrackFlagLacing);
				    if (Elt2)
					    NodeDelete(Elt2);
			    }
			    else
			    {
				    // doesn't have lacing
				    Elt2 = EBML_MasterFindFirstElt(RLevel1,KaxTrackFlagLacing,1,0,0);
				    EBML_IntegerSetValue((ebml_integer*)Elt2,0);
			    }

                CodecPrivate = (ebml_binary*) EBML_MasterFindChild(RLevel1,KaxCodecPrivate);
                if (CodecPrivate && EBML_ElementDataSize((ebml_element*)CodecPrivate, 1)==0)
                {
                    NodeDelete(CodecPrivate);
                    CodecPrivate = NULL;
                }

                Encodings = (ebml_master*)EBML_MasterFindChild(RLevel1,KaxContentEncodings);
                if (UnOptimize)
                    // remove the previous track compression
                    encoding = MATROSKA_TRACK_ENCODING_COMP_NONE;
                else if (!Optimize)
                    // keep the same kind of encoding as before
                    encoding = MATROSKA_TrackGetBlockCompression((matroska_trackentry*)RLevel1, DstProfile);
                else
                {
                    Elt = EBML_MasterFindFirstElt(RLevel1,KaxCodecID,1,0,0);
                    EBML_StringGet((ebml_string*)Elt,CodecID,TSIZEOF(CodecID));
                    if (tcsisame_ascii(CodecID,T("S_USF")) || tcsisame_ascii(CodecID,T("S_VOBSUB")) || tcsisame_ascii(CodecID,T("S_HDMV/PGS")) || tcsisame_ascii(CodecID,T("B_VOBBTN"))
                        || tcsisame_ascii(CodecID,T("V_UNCOMPRESSED"))|| tcsstr(CodecID,T("A_PCM"))==CodecID)
                        encoding = MATROSKA_TRACK_ENCODING_COMP_ZLIB;
                    else
                    {
                        // don't keep the zlib compression on compressed codecs
                        //encoding = MATROSKA_TrackGetBlockCompression((matroska_trackentry*)RLevel1);
                        //if (encoding == MATROSKA_TRACK_ENCODING_COMP_NONE || encoding == MATROSKA_TRACK_ENCODING_COMP_HEADERSTRIP)
                        {
                            HeaderData = ARRAYBEGIN(TrackMaxHeader,binary)+TrackNum;
                            if (ARRAYCOUNT(*HeaderData,uint8_t))
                                encoding = MATROSKA_TRACK_ENCODING_COMP_HEADERSTRIP;
                        }
                    }
                }
                if (encoding == MATROSKA_TRACK_ENCODING_COMP_NONE)
                    zlib_scope = 0;
                else
                {
                    if (Encodings)
                    {
                        Elt = EBML_MasterFindChild((ebml_master*)Encodings,KaxContentEncoding);
                        if (Elt)
                        {
                            Elt2 =  EBML_MasterGetChild((ebml_master*)Elt,KaxContentEncodingScope, DstProfile);
                            if (Elt2)
                                zlib_scope = EBML_IntegerValue((ebml_integer*)Elt2);
                        }
                    }
                }

                // see if we can add CodecPrivate too
                if ((Optimize || encoding != MATROSKA_TRACK_ENCODING_COMP_NONE) && encoding != MATROSKA_TRACK_ENCODING_COMP_HEADERSTRIP && zlib_scope != MATROSKA_CONTENTENCODINGSCOPE_PRIVATE)
                {
                    if (CodecPrivate!=NULL)
                    {
                        size_t ExtraCompHeaderBytes = (encoding == MATROSKA_TRACK_ENCODING_COMP_NONE) ? 13 : 3; // extra bytes needed to add the comp header to the track
                        size_t CompressedSize = (size_t)EBML_ElementDataSize((ebml_element*)CodecPrivate, 1);
                        size_t origCompressedSize = CompressedSize;
                        uint8_t *Compressed = malloc(CompressedSize);
                        if (CompressFrameZLib(EBML_BinaryGetData(CodecPrivate), origCompressedSize, &Compressed, &CompressedSize)==ERR_NONE
                            && (CompressedSize + ExtraCompHeaderBytes) < origCompressedSize)
                        {
                            encoding = MATROSKA_TRACK_ENCODING_COMP_ZLIB;
                            zlib_scope |= MATROSKA_CONTENTENCODINGSCOPE_PRIVATE;
                        }
                        free(Compressed);
                    }
                }

                switch (encoding)
                {
                case MATROSKA_TRACK_ENCODING_COMP_ZLIB:
                case MATROSKA_TRACK_ENCODING_COMP_BZLIB: // transform bzlib into zlib
                case MATROSKA_TRACK_ENCODING_COMP_LZO1X: // transform lzo1x into zlib
                    if (MATROSKA_TrackSetCompressionZlib((matroska_trackentry*)RLevel1, zlib_scope,DstProfile))
						ClustersNeedRead = 1;
                    break;
                case MATROSKA_TRACK_ENCODING_COMP_HEADERSTRIP:
                    if (!HeaderData || MATROSKA_TrackSetCompressionHeader((matroska_trackentry*)RLevel1, ARRAYBEGIN(*HeaderData,uint8_t), ARRAYCOUNT(*HeaderData,uint8_t), DstProfile))
					    ClustersNeedRead = 1;
                    break;
                default:
                    if (MATROSKA_TrackSetCompressionNone((matroska_trackentry*)RLevel1))
					    ClustersNeedRead = 1;
                    break;
                }

                Encodings = (ebml_master*)EBML_MasterFindChild(RLevel1,KaxContentEncodings);
                if (Encodings)
                {
                    if ((Elt2 = EBML_MasterFindChild(Encodings,KaxContentEncoding)) != NULL)
                    {
                        Elt =  EBML_MasterGetChild((ebml_master*)Elt2,KaxContentEncodingScope, DstProfile);
                        if (EBML_IntegerValue((ebml_integer*)Elt) & MATROSKA_CONTENTENCODINGSCOPE_BLOCK)
                        {
                            Elt = EBML_MasterGetChild((ebml_master*)Elt2,KaxContentCompression, DstProfile);
                            Elt = EBML_MasterGetChild((ebml_master*)Elt,KaxContentCompAlgo, DstProfile);
                            if (EBML_IntegerValue((ebml_integer*)Elt)!=MATROSKA_TRACK_ENCODING_COMP_HEADERSTRIP)
                                ClustersNeedRead = 1;
                        }
                    }
                }
            }
		}
	}

	if (!Live)
	{
        // cues
        if (ARRAYCOUNT(*Clusters,ebml_element*) < 2)
        {
            NodeDelete(RCues);
            RCues = NULL;
        }
		else if (RCues)
		{
			ReduceSize((ebml_element*)RCues);
			if (!EBML_MasterCheckMandatory(RCues,0, ProfileFrom(DstProfile)))
			{
				TextWrite(StdErr,T("The original Cues are missing mandatory elements, creating from scratch\r\n"));
				NodeDelete(RCues);
				RCues = NULL;
			}
            else if (EBML_MasterUseChecksum(RCues,!Unsafe))
                EBML_ElementUpdateSize(RCues,0,0, ProfileFrom(DstProfile));
		}

		if (!RCues && WTrackInfo && ARRAYCOUNT(*Clusters,ebml_element*) > 1)
		{
			// generate the cues
			RCues = (ebml_master*)EBML_ElementCreate(&p,KaxCues,0, DstProfile,NULL);
            EBML_MasterUseChecksum(RCues,!Unsafe);
			if (!Quiet) TextWrite(StdErr,T("Generating Cues from scratch\r\n"));
			CuesCreated = GenerateCueEntries(RCues,Clusters,WTrackInfo,WSegmentInfo,(ebml_element*)RSegment, TotalSize);
			if (!CuesCreated)
			{
				NodeDelete(RCues);
				RCues = NULL;
			}
		}

		if (RCues)
		{
			WSeekPoint = (matroska_seekpoint*)EBML_MasterAddElt(WMetaSeek,KaxSeek,0, DstProfile);
            EBML_MasterUseChecksum((ebml_master*)WSeekPoint,!Unsafe);
			EBML_ElementForcePosition((ebml_element*)RCues, NextPos);
			NextPos += EBML_ElementFullSize((ebml_element*)RCues,0);
			MATROSKA_LinkMetaSeekElement(WSeekPoint,(ebml_element*)RCues);
		}

        ExtraVoidSize = 2 * EXTRA_SEEK_SPACE; // leave room for 2 unknown level1 elements
        if (!RTags)
            ExtraVoidSize += EXTRA_SEEK_SPACE;

        if (!RAttachments)
            ExtraVoidSize += EXTRA_SEEK_SPACE;

        if (!RChapters)
            ExtraVoidSize += EXTRA_SEEK_SPACE;

        // first cluster
        if (ARRAYCOUNT(RClusters,matroska_cluster*))
        {
			W1stClusterSeek = (matroska_seekpoint*)EBML_MasterAddElt(WMetaSeek,KaxSeek,0, DstProfile);
            EBML_MasterUseChecksum((ebml_master*)W1stClusterSeek,!Unsafe);
			EBML_ElementForcePosition(ARRAYBEGIN(*Clusters,ebml_element*)[0], NextPos + ExtraVoidSize);
			NextPos += EBML_ElementFullSize(ARRAYBEGIN(*Clusters,ebml_element*)[0],0);
			MATROSKA_LinkMetaSeekElement(W1stClusterSeek,ARRAYBEGIN(*Clusters,ebml_element*)[0]);
        }

		// first estimation of the MetaSeek size
		MetaSeekUpdate(WMetaSeek);
		MetaSeekBefore = EBML_ElementFullSize((ebml_element*)WMetaSeek,0);
        NextPos = EBML_ElementPositionData((ebml_element*)WSegment) + EBML_ElementFullSize((ebml_element*)WMetaSeek,0);

        NextPos += ExtraVoidSize;
	}

    EBML_ElementUpdateSize(WSegmentInfo,0,0, ProfileFrom(DstProfile));
    EBML_ElementForcePosition((ebml_element*)WSegmentInfo, NextPos);
    NextPos += EBML_ElementFullSize((ebml_element*)WSegmentInfo,0);

    //  Compute the Track Info size
    if (WTrackInfo)
    {
        ReduceSize((ebml_element*)WTrackInfo);
        EBML_ElementUpdateSize(WTrackInfo,0,0, ProfileFrom(DstProfile));
        EBML_ElementForcePosition((ebml_element*)WTrackInfo, NextPos);
        NextPos += EBML_ElementFullSize((ebml_element*)WTrackInfo,0);
    }

	if (!Live)
	{
        ebml_element *Void;
		//  Compute the Chapters size
		if (RChapters)
		{
			ReduceSize((ebml_element*)RChapters);
			EBML_ElementUpdateSize(RChapters,0,0, ProfileFrom(DstProfile));
			EBML_ElementForcePosition((ebml_element*)RChapters, NextPos);
			NextPos += EBML_ElementFullSize((ebml_element*)RChapters,0);
		}

		//  Compute the Attachments size
		if (RAttachments)
		{
			ReduceSize((ebml_element*)RAttachments);
			EBML_ElementUpdateSize(RAttachments,0,0, ProfileFrom(DstProfile));
			EBML_ElementForcePosition((ebml_element*)RAttachments, NextPos);
			NextPos += EBML_ElementFullSize((ebml_element*)RAttachments,0);
		}

		//  Compute the Tags size
		if (RTags)
		{
			EBML_ElementUpdateSize(RTags,0,0, ProfileFrom(DstProfile));
			EBML_ElementForcePosition((ebml_element*)RTags, NextPos);
			NextPos += EBML_ElementFullSize((ebml_element*)RTags,0);
		}

		MetaSeekUpdate(WMetaSeek);
		NextPos += EBML_ElementFullSize((ebml_element*)WMetaSeek,0) - MetaSeekBefore;

		//  Compute the Cues size
		if (WTrackInfo && RCues)
		{
            OptimizeCues(RCues,Clusters,WSegmentInfo,NextPos, WSegment, TotalSize, !CuesCreated, !Unsafe, ClustersNeedRead?Input:NULL);
			EBML_ElementForcePosition((ebml_element*)RCues, NextPos);
			NextPos += EBML_ElementFullSize((ebml_element*)RCues,0);
		}
        else if (ARRAYCOUNT(*Clusters,ebml_element*)==1)
            EBML_ElementForcePosition(ARRAYBEGIN(*Clusters,ebml_element*)[0], NextPos);

		// update and write the MetaSeek and the elements following
        // write without the fake Tags pointer
		Stream_Seek(Output,EBML_ElementPosition((ebml_element*)WMetaSeek),SEEK_SET);
        MetaSeekUpdate(WMetaSeek);

		EBML_ElementFullSize((ebml_element*)WMetaSeek,0);
		if (EBML_ElementRender((ebml_element*)WMetaSeek,Output,0,0,1,ProfileFrom(DstProfile),&MetaSeekBefore)!=ERR_NONE)
		{
			TextWrite(StdErr,T("Failed to write the final Meta Seek\r\n"));
			Result = -22;
			goto exit;
		}
		SegmentSize += MetaSeekBefore;

        // create a fake placeholder elements to have its position prepared in the SeekHead
        Void = EBML_ElementCreate(WMetaSeek,EBML_getContextEbmlVoid(),1,DstProfile,NULL);
        EBML_VoidSetFullSize(Void, ExtraVoidSize);
		if (EBML_ElementRender((ebml_element*)Void,Output,0,0,1,ProfileFrom(DstProfile),&ClusterSize)!=ERR_NONE)
		{
			TextWrite(StdErr,T("Failed to write the Void after Meta Seek\r\n"));
			Result = -24;
			goto exit;
		}
        NodeDelete(Void);
		SegmentSize += ClusterSize;
    }
    else if (!Unsafe)
        SetClusterPrevSize(*Clusters, ClustersNeedRead?Input:NULL);

    if (EBML_ElementRender((ebml_element*)WSegmentInfo,Output,0,0,1,ProfileFrom(DstProfile),&ClusterSize)!=ERR_NONE)
    {
        TextWrite(StdErr,T("Failed to write the Segment Info\r\n"));
        Result = -11;
        goto exit;
    }
	SegmentSize += ClusterSize;
    if (WTrackInfo)
    {
        if (EBML_ElementRender((ebml_element*)WTrackInfo,Output,0,0,1,ProfileFrom(DstProfile),&ClusterSize)!=ERR_NONE)
        {
            TextWrite(StdErr,T("Failed to write the Track Info\r\n"));
            Result = -12;
            goto exit;
        }
        SegmentSize += ClusterSize;
    }
    if (!Live && RChapters)
    {
        if (EBML_ElementRender((ebml_element*)RChapters,Output,0,0,1,ProfileFrom(DstProfile),&ClusterSize)!=ERR_NONE)
        {
            TextWrite(StdErr,T("Failed to write the Chapters\r\n"));
            Result = -13;
            goto exit;
        }
        SegmentSize += ClusterSize;
    }

    //  Write the Attachments
    if (!Live && RAttachments)
    {
        ReduceSize((ebml_element*)RAttachments);
        if (EBML_ElementRender((ebml_element*)RAttachments,Output,0,0,1,ProfileFrom(DstProfile),&ClusterSize)!=ERR_NONE)
        {
            TextWrite(StdErr,T("Failed to write the Attachments\r\n"));
            Result = -17;
            goto exit;
        }
        SegmentSize += ClusterSize;
    }

    if (!Live && RTags)
    {
        if (EBML_ElementRender((ebml_element*)RTags,Output,0,0,1,ProfileFrom(DstProfile),&ClusterSize)!=ERR_NONE)
        {
            TextWrite(StdErr,T("Failed to write the Tags\r\n"));
            Result = -14;
            goto exit;
        }
        SegmentSize += ClusterSize;
    }

    if (!Live && RCues)
    {
        if (EBML_ElementRender((ebml_element*)RCues,Output,0,0,1,ProfileFrom(DstProfile),&CuesSize)!=ERR_NONE)
        {
            TextWrite(StdErr,T("Failed to write the Cues\r\n"));
            Result = -15;
            goto exit;
        }
        SegmentSize += CuesSize;
    }

    //  Write the Clusters
    ClusterSize = INVALID_FILEPOS_T;
    PrevTimestamp = INVALID_TIMESTAMP_T;
    CuesChanged = 0;
    CurrentPhase = TotalPhases;
    for (auto Cluster = ARRAYBEGIN(*Clusters,ebml_master*);Cluster != ARRAYEND(*Clusters,ebml_master*); ++Cluster)
    {
        ShowProgress((ebml_element*)*Cluster, TotalSize);
        CuesChanged = WriteCluster(*Cluster,Output,Input, ClusterSize, &PrevTimestamp) || CuesChanged;
        if (!Unsafe)
            ClusterSize = EBML_ElementFullSize((ebml_element*)*Cluster,0);
        SegmentSize += EBML_ElementFullSize((ebml_element*)*Cluster,0);
    }
    EndProgress();

    if (CuesChanged && !Live && RCues)
    {
        filepos_t PosBefore = Stream_Seek(Output,0,SEEK_CUR);
        Stream_Seek(Output,EBML_ElementPosition((ebml_element*)RCues),SEEK_SET);

        UpdateCues(RCues, WSegment);

        if (EBML_ElementRender((ebml_element*)RCues,Output,0,0,1,ProfileFrom(DstProfile),&ClusterSize)!=ERR_NONE)
        {
            TextWrite(StdErr,T("Failed to write the Cues\r\n"));
            Result = -16;
            goto exit;
        }
        if (CuesSize >= ClusterSize+2)
        {
            // the cues were shrinked, write a void element
            ebml_element *Void = EBML_ElementCreate(RCues,EBML_getContextEbmlVoid(),1, DstProfile,NULL);
            EBML_VoidSetFullSize(Void, CuesSize - ClusterSize);
            EBML_ElementRender(Void,Output,0,0,1,ProfileFrom(DstProfile),NULL);
        }
        else
        {
            assert(ClusterSize == CuesSize);
            if (ClusterSize != CuesSize)
            {
                TextWrite(StdErr,T("The Cues size changed after a Cluster timestamp was altered!\r\n"));
                Result = -18;
                goto exit;
            }
        }
        Stream_Seek(Output,PosBefore,SEEK_SET);
    }

    // update the WSegment size
	if (!Live)
	{
		if (EBML_ElementDataSize((ebml_element*)WSegment,0)!=INVALID_FILEPOS_T && SegmentSize - ExtraSizeDiff - ExtraVoidSize > EBML_ElementDataSize((ebml_element*)WSegment,0))
		{
			if (EBML_CodedSizeLength(SegmentSize,EBML_ElementSizeLength((ebml_element*)WSegment),0) != EBML_CodedSizeLength(SegmentSize,EBML_ElementSizeLength((ebml_element*)WSegment),0))
			{
                // TODO: this check is always false
				TextPrintf(StdErr,T("The segment written is much bigger than the original %") TPRId64 T(" vs %") TPRId64 T(" !\r\n"),SegmentSize,EBML_ElementDataSize((ebml_element*)WSegment,0));
				Result = -20;
				goto exit;
			}
			if (!Quiet) TextPrintf(StdErr,T("The segment written is bigger than the original %") TPRId64 T(" vs %") TPRId64 T(" !\r\n"),SegmentSize,EBML_ElementDataSize((ebml_element*)WSegment,0));
		}
		if (EBML_CodedSizeLength(EBML_ElementDataSize((ebml_element*)WSegment,0),0,!Live) > EBML_CodedSizeLength(SegmentSize,0,!Live))
			EBML_ElementSetSizeLength((ebml_element*)WSegment, EBML_CodedSizeLength(EBML_ElementDataSize((ebml_element*)WSegment,0),0,!Live));
		EBML_ElementForceDataSize((ebml_element*)WSegment, SegmentSize);
		Stream_Seek(Output,EBML_ElementPosition((ebml_element*)WSegment),SEEK_SET);
		if (EBML_ElementRenderHead(WSegment, Output, 0, NULL,ProfileFrom(DstProfile))!=ERR_NONE)
		{
			TextWrite(StdErr,T("Failed to write the final Segment size !\r\n"));
			Result = -21;
			goto exit;
		}

		// update the Meta Seek
		MetaSeekUpdate(WMetaSeek);
		//Stream_Seek(Output,EBML_ElementPosition(WMetaSeek,SEEK_SET);
		if (EBML_ElementRender((ebml_element*)WMetaSeek,Output,0,0,1,ProfileFrom(DstProfile),&MetaSeekAfter)!=ERR_NONE)
		{
			TextWrite(StdErr,T("Failed to write the final Meta Seek\r\n"));
			Result = -22;
			goto exit;
		}
		if (MetaSeekBefore != MetaSeekAfter)
		{
			TextPrintf(StdErr,T("The final Meta Seek size has changed %") TPRId64 T(" vs %") TPRId64 T(" !\r\n"),MetaSeekBefore,MetaSeekAfter);
			Result = -23;
			goto exit;
		}
	}

    if (!Quiet) TextPrintf(StdErr,T("Finished cleaning & optimizing \"%s\"\r\n"),Path);

exit:
    NodeDelete(WSegment);

    for (auto Cluster = ARRAYBEGIN(RClusters,ebml_master*);Cluster != ARRAYEND(RClusters,ebml_master*); ++Cluster)
        NodeDelete(*Cluster);
    ArrayClear(&RClusters);
    for (auto Cluster = ARRAYBEGIN(WClusters,ebml_master*);Cluster != ARRAYEND(WClusters,ebml_master*); ++Cluster)
        NodeDelete(*Cluster);
    for (MaxTrackNum=0;MaxTrackNum<ARRAYCOUNT(TrackMaxHeader,binary);++MaxTrackNum)
        ArrayClear(ARRAYBEGIN(TrackMaxHeader,binary)+MaxTrackNum);
    ArrayClear(&Alternate3DTracks);
    ArrayClear(&TrackMaxHeader);
    ArrayClear(&WClusters);
    ArrayClear(&WTracks);
    NodeDelete(RAttachments);
    NodeDelete(RTags);
    NodeDelete(RCues);
    NodeDelete(RChapters);
    NodeDelete(RTrackInfo);
    NodeDelete(WTrackInfo);
    NodeDelete(RSegmentInfo);
    NodeDelete(WSegmentInfo);
    NodeDelete(RLevel1);
    NodeDelete(RSegment);
    NodeDelete(EbmlHead);
    if (Input)
        StreamClose(Input);
    if (Output)
        StreamClose(Output);

    if (Result<0 && Path[0])
        FileErase((nodecontext*)&p,Path,1,0);

    // EBML & Matroska ending
    MATROSKA_Done(&p);

    // Core-C ending
    if (!Regression) // until all the memory leaks are fixed
    ParserContext_Done(&p);

    return Result;
}
