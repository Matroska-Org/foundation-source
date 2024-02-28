#include <ebml/EbmlMaster.h>
#include <ebml/EbmlUInteger.h>
#include <ebml/EbmlVoid.h>
#include <matroska/KaxSemantic.h>
#include <matroska/KaxCluster.h>
#include <matroska/KaxBlock.h>
#include <matroska/KaxSeekHead.h>
#include <matroska/KaxCuesData.h>
#include <matroska/KaxSegment.h>

#include <limits>
#include <cstdint>

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
#define tcsnicmp_ascii(a,b,n)  strncmp(a,b,n)
#define stprintf_s   std::snprintf
#define stcatprintf_s(d,dn,f,a1) std::snprintf(d+tcslen(d),dn-tcslen(d),f,a1)
#define stcatprintf_s2(d,dn,f,a1,a2) std::snprintf(d+tcslen(d),dn-tcslen(d),f,a1,a2)
#define StringToInt(s,r) std::atoi(s)

#define TextPrintf fprintf
#define TextWrite fprintf
using textwriter = FILE;
#define StdErr stderr

#define SFLAG_RDONLY  MODE_READ
#define SFLAG_WRONLY  MODE_WRITE
#define SFLAG_CREATE  MODE_CREATE
#define StreamOpen(c,p,f)  new StdIOCallback(p, (open_mode)(f))
static inline libebml::filepos_t Stream_Seek(libebml::IOCallback *s, std::uint64_t l, int w)
{
    (s)->setFilePointer(l,(libebml::seek_mode)w);
    return (s)->getFilePointer();
}
#define Stream_Write(s,b,l,p)  (s)->writeFully(b,l)

#define Node_FromStr(c,d,dn,s)  strcpy(d,s)
#define Node_FromUTF8(c,d,dn,s) strcpy(d,s)

#define MAXPATH 1024
#define MAXPATHFULL 1024
#define MAXLINE 2048
#define MAXDATA 2048

using bool_t = bool;

#define ARRAYBEGIN(v,t)  (v).begin()
#define ARRAYEND(v,t)    (v).end()
#define ARRAYCOUNT(v,t)  (v).size()
#define ARRAYAT(v,t,i)   (v).at(i)
#define ArrayInit(v)     (v)->clear()
#define ArrayClear(v)    (v)->clear()
#define ArrayResize(v,s,t,a) (v)->resize(s)
#define ArrayZero(v) // nothing, it's always used after resize
#define ArrayAppend(v,e,s,a)  (v)->push_back(e)
#define ArrayShrink(v,l)  (v)->resize((v)->size() - l); (v)->shrink_to_fit()

#define NodeDelete(o)    delete o

using ebml_element = libebml::EbmlElement;
using ebml_master = libebml::EbmlMaster;
using ebml_integer = libebml::EbmlUInteger;
using ebml_float = libebml::EbmlFloat;
using ebml_string = libebml::EbmlString;
using ebml_unistring = libebml::EbmlUnicodeString;
using ebml_date = libebml::EbmlDate;
using ebml_binary = libebml::EbmlBinary;
using matroska_cluster = libmatroska::KaxCluster;
using matroska_block = libmatroska::KaxInternalBlock;
using matroska_seekpoint = libmatroska::KaxSeek;
using matroska_cuepoint = libmatroska::KaxCuePoint;
using matroska_frame = libmatroska::DataBuffer;
using nodetree = libebml::EbmlMaster;

using stream = libebml::IOCallback;

using mkv_timestamp_t = std::uint64_t;
constexpr const mkv_timestamp_t INVALID_TIMESTAMP_T = std::numeric_limits<mkv_timestamp_t>::max();

#define ERR_NONE          (!INVALID_FILEPOS_T)
#define ERR_INVALID_DATA  INVALID_FILEPOS_T
using err_t = libebml::filepos_t;

#define EBML_getContextHead()                libebml::EbmlHead
#define EBML_getContextReadVersion()         libebml::EReadVersion
#define EBML_getContextMaxIdLength()         libebml::EMaxIdLength
#define EBML_getContextMaxSizeLength()       libebml::EMaxSizeLength
#define EBML_getContextDocType()             libebml::EDocType
#define EBML_getContextDocTypeVersion()      libebml::EDocTypeVersion
#define EBML_getContextDocTypeReadVersion()  libebml::EDocTypeReadVersion
#define EBML_getContextEbmlVoid()            libebml::EbmlVoid

#define MATROSKA_getContextInfo()           libmatroska::KaxInfo
#define MATROSKA_getContextTimestampScale() libmatroska::KaxTimestampScale
#define MATROSKA_getContextSeek()           libmatroska::KaxSeek
#define MATROSKA_getContextSegment()        libmatroska::KaxSegment
#define MATROSKA_getContextSeekID()         libmatroska::KaxSeekID
#define MATROSKA_getContextSeekPosition()   libmatroska::KaxSeekPosition

#define EBML_CodedSizeLength  CodedSizeLength

#define EBML_ElementCreate(n,c,d,p,o)  new (c)()

#define EBML_ElementGetName(e,d,dn)   strcpy(d,EBML_NAME(e))
#define EBML_ElementClassID(e)        (*e)->GetClassId()
#define EBML_ElementPosition(e)       (e)->GetElementPosition()
#define EBML_ElementDataSize(e,u)     (e)->GetSize()
#define EBML_ElementFullSize(e,u)     (e)->ElementSize((u != 0) ? libebml::EbmlElement::WriteAll : libebml::EbmlElement::WriteSkipDefault)
#define EBML_ElementPositionEnd(e)    (e)->GetEndPosition()
#define EBML_ElementPositionData(e)   ((e)->GetEndPosition() - (e)->GetSize())
#define EBML_ElementIsType(e,t)       ((e)->GetClassId() == EBML_ID(t))
#define EBML_ElementReadData(e,i,b,c,s,f) (e)->ReadData(*i,s)
#define EBML_ElementSkipData(e,s,c,p,d)   (e)->SkipData(s,c,p,d)
#define EBML_FindNextElement(stream, sem, level, dummy) (stream)->FindNextElement(sem, *level, UINT64_MAX, dummy)
#define EBML_ElementSizeLength(e)      (e)->GetSizeLength()
#define EBML_ElementSetSizeLength(e,s) (e)->SetSizeLength(s)
#define EBML_ElementIsFiniteSize(e)       (e)->IsFiniteSize()
#define EBML_ElementSetInfiniteSize(e,i)  (e)->SetSizeInfinite(i)
#define EBML_ElementUpdateSize(e,d,f,p) (e)->UpdateSize(p,f)
#define EBML_ElementForceDataSize(e,s)  (e)->ForceSize(s)
#define EBML_ElementForcePosition(e,p)  (e)->ForcePosition(p)
#define EBML_ElementForceContext(e,c) // FIXME turn OldStereo to new Stereo
static inline err_t EBML_ElementRenderHead(libebml::EbmlMaster *e, libebml::IOCallback *o, bool_t bKeepPosition, libebml::filepos_t *Rendered, const libebml::EbmlElement::ShouldWrite &p)
{
    return (e)->WriteHead(*o, 0, p);
}

// #define EBML_MasterChildren(m)  ((libebml::EbmlMaster *)(m))->begin()
// #define EBML_MasterEnd(i,m)     (i) != ((libebml::EbmlMaster *)(m))->end()
#define EBML_MasterChildren(m)  (m)->begin()
#define EBML_MasterEnd(i,m)     (i) != (m)->end()
#define EBML_MasterNext(i)      (i)++
#define EBML_MasterEmpty(m)     ((static_cast<const EbmlMaster *>(m))->begin() == (static_cast<const EbmlMaster *>(m))->end())
#define EBML_MasterRemove(m,e)  (m)->Remove(e)
#define EBML_MasterAppend(m,e)  (m)->PushElement(*(e))

#define EBML_MasterFindChild(m,c)     FindChild<c>(*(libebml::EbmlMaster *)(m))
#define EBML_MasterGetChild(m,c,u)    &GetChild<c>(*(libebml::EbmlMaster *)(m))
#define EBML_MasterNextChild(m,p)     FindNextChild(*(libebml::EbmlMaster *)(m), *p)
#define EBML_MasterIsChecksumValid(m) (m)->VerifyChecksum()
#define EBML_MasterFindFirstElt(m,c,n,d,p)  &GetChild<c>(*(libebml::EbmlMaster *)(m))
#define EBML_MasterAddElt(m,c,d,p)    (m)->AddNewElt(EBML_INFO(c))

#define NodeTree_Clear(m)             (m)->RemoveAll()
#define NodeTree_SetParent(e,p,u)     (static_cast<libebml::EbmlMaster *>(p))->PushElement(*(e))

#define EBML_IntegerValue(e)          (reinterpret_cast<const libebml::EbmlUInteger*>(e))->GetValue()
#define EBML_IntegerSetValue(e,v)     (reinterpret_cast<libebml::EbmlUInteger*>(e))->SetValue(v)

#define EBML_StringGet(e,d,dn)         strcpy(d,(e)->GetValue().c_str())
#define EBML_StringGetUnicode(e,d,dn)  strcpy(d,(e)->GetValueUTF8().c_str())
static inline err_t EBML_StringSetValue(libebml::EbmlString *e, const char *v)
{
    (e)->SetValue(v);
    return ERR_NONE;
}
static inline err_t EBML_UniStringSetValue(libebml::EbmlUnicodeString *e, const char *v)
{
    (e)->SetValueUTF8(v);
    return ERR_NONE;
}

#define EBML_FloatValue(e)             (reinterpret_cast<libebml::EbmlFloat*>(e))->GetValue()
#define EBML_FloatSetValue(e,v)        (e)->SetValue(v)

#define MATROSKA_MetaSeekIsClass(s,c)  (s)->IsEbmlId(EBML_ID(c))

#define MATROSKA_BlockTrackNum(b)      (b)->TrackNum()
#define MATROSKA_BlockGetFrameCount(b) (b)->NumberFrames()
#define MATROSKA_BlockGetFrame(b,i,f,d) (b)->GetBuffer(i)
#define MATROSKA_BlockGetLength(b,i)   (b)->GetFrameSize(i)

#define MATROSKA_CueTrackNum(c)        (c)->GetSeekPosition()->TrackNumber()
#define MATROSKA_CuesSort(c)           (c)->Sort()

#define MATROSKA_AttachmentSort(c)     (c)->Sort()


#define PROFILE_MATROSKA_V1  1
#define PROFILE_MATROSKA_V2  2
#define PROFILE_MATROSKA_V3  3
#define PROFILE_MATROSKA_V4  4
#define PROFILE_MATROSKA_V5  5
#define PROFILE_DIVX 0
#define PROFILE_WEBM 10

#define EBML_MAX_VERSION    1
#define EBML_MAX_ID         4
#define EBML_MAX_SIZE       8
#define MATROSKA_MAX_VERSION 5

#define MATROSKA_BlockReadData(b,s,p)  (b)->ReadData((*s))
#define MATROSKA_BlockReleaseData(b,r) ERR_NONE, (b)->ReleaseFrames()


static inline err_t EBML_ElementRender(libebml::EbmlElement *e, libebml::IOCallback *o, bool_t bWithDefault, bool_t bKeepPosition, bool_t bForceWithoutMandatory, const libebml::EbmlElement::ShouldWrite &p, libebml::filepos_t *r)
{
    auto res = (e)->Render(*(o),p,bKeepPosition,bForceWithoutMandatory);
    if (r != nullptr && res != INVALID_FILEPOS_T)
        *(r) = res;
    return res;
}

static inline ebml_element *EBML_ElementCopy(const void*, const void *Cookie)
{
    return nullptr; // TODO
}


static inline size_t EBML_IdToString(tchar_t *Out, size_t OutLen, const libebml::EbmlId & Id)
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

static inline int Scale32(int64_t v,int64_t Num,int64_t Den)
{
    return (int)Scale64(v, Num, Den);
}

static inline tchar_t* tcsupr(tchar_t* p)
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

static inline bool AllowedInProfile(int Profile, const libmatroska::MatroskaProfile &profile)
{
    if (profile.IsAlwaysDeprecated())
        return false;
    if (Profile == PROFILE_WEBM)
        return profile.InWebM;
    if (Profile == PROFILE_DIVX)
        return profile.InDivX;
    return profile.IsValidInVersion(Profile);
}

static inline void EBML_MasterCheckContext(libebml::EbmlMaster *Element, int ProfileMask, bool_t (*ErrCallback)(void *cookie, int type, const tchar_t *ClassName, const libebml::EbmlElement*), void *cookie)
{
	tchar_t ClassString[MAXPATH];
	EBML_MASTER_ITERATOR i;
    libebml::EbmlElement *SubElt;
    const libebml::EbmlSemantic *s;
#if 1 // TODO
	for (i=EBML_MasterChildren(Element);EBML_MasterEnd(i,Element);EBML_MasterNext(i))
	{
		if ((*i)->IsDummy())
        {
            for (size_t si=0; si < EBML_CTX_SIZE(EBML_CONTEXT(Element)); si++)
		    //for (s=Element->Base.Context->Semantic; s->eClass; ++s)
		    {
                s = &EBML_CTX_IDX(EBML_CONTEXT(Element), si);
			    //if (s->eClass->Id == i->Context->Id)
			    if (EBML_INFO_ID(s->GetCallbacks()) == (*i)->GetClassId())
			    {
                    const auto & cb = s->GetCallbacks();
#if 1
//                     const auto & semcb = Element->ElementSpec();
//                     const auto & profiles = reinterpret_cast<const libmatroska::MatroskaProfile &>(cb.GetVersions());
//                     // if (s->DisabledProfile & ProfileMask)
//                     if (!AllowedInProfile(ProfileMask, profiles))
//                     {
// AllowedInProfile(ProfileMask, profiles);
// 				        Node_FromStr(Element,ClassString,TSIZEOF(ClassString),EBML_INFO_NAME(cb));
//                         if (ErrCallback && ErrCallback(cookie,MASTER_CHECK_PROFILE_INVALID,ClassString,*i))
//                         {
//                             // TODO EBML_MasterRemove(Element,i); // make sure it doesn't remain in the list
// 					        // TODO NodeDelete(i);
// 					        i=EBML_MasterChildren(Element);
//                             break;
//                         }
//                     }
                    if (s->IsUnique() && (SubElt=Element->FindFirstElt(cb, false)) && (SubElt=EBML_MasterNextChild(Element,SubElt)))
                    // if (s->IsUnique() && (SubElt=EBML_MasterFindChild(Element,s->eClass)) && (SubElt=EBML_MasterNextChild(Element,SubElt)))
                    {
		                Node_FromStr(Element,ClassString,TSIZEOF(ClassString),EBML_INFO_NAME(cb));
                        if (ErrCallback && ErrCallback(cookie,MASTER_CHECK_MULTIPLE_UNIQUE,ClassString,SubElt))
                        {
                            EBML_MasterRemove(Element,i); // make sure it doesn't remain in the list
			                NodeDelete(*i);
			                i=EBML_MasterChildren(Element);
                            break;
                        }
                    }
				    break;
#endif
			    }
		    }
        }

        bool allowed = false; // unknown or libebml
        const auto & profile = (*i)->ElementSpec().GetVersions();
        if (profile.GetNameSpace() == libmatroska::MatroskaProfile::Namespace)
        {
            const auto & profiles = reinterpret_cast<const libmatroska::MatroskaProfile &>(profile);
            allowed = AllowedInProfile(ProfileMask, profiles);
        }
        // if (s->DisabledProfile & ProfileMask)
        if (!allowed)
        {
            Node_FromStr(*i,ClassString,TSIZEOF(ClassString),EBML_INFO_NAME((*i)->ElementSpec()));
            if (ErrCallback && ErrCallback(cookie,MASTER_CHECK_PROFILE_INVALID,ClassString,*i))
            {
                // TODO EBML_MasterRemove(Element,*i); // make sure it doesn't remain in the list
                // TODO NodeDelete(i);
                i=EBML_MasterChildren(Element);
                break;
            }
        }
	}

    for (size_t si=0; si < EBML_CTX_SIZE(EBML_CONTEXT(Element)); si++)
	//for (s=Element->Base.Context->Semantic; s->eClass; ++s)
	{
        s = &EBML_CTX_IDX(EBML_CONTEXT(Element), si);
        const auto & cb = s->GetCallbacks();

        // auto profiles = reinterpret_cast<const libmatroska::MatroskaProfile &>(cb.GetVersions());
        // // if (s->DisabledProfile & ProfileMask)
        // if (!AllowedInProfile(ProfileMask, profiles))
        // {
        //     Node_FromStr(Element,ClassString,TSIZEOF(ClassString),EBML_INFO_NAME(cb));
        //     if (ErrCallback && ErrCallback(cookie,MASTER_CHECK_PROFILE_INVALID,ClassString,Element))
        //     {
        //         // TODO EBML_MasterRemove(Element,*i); // make sure it doesn't remain in the list
        //         // TODO NodeDelete(i);
        //         i=EBML_MasterChildren(Element);
        //         break;
        //     }
        // }

	    if (s->IsMandatory() && !cb.HasDefault() && !Element->FindFirstElt(cb, false))
	    {
		    // Node_FromStr(Element,ClassString,TSIZEOF(ClassString),s->eClass->ElementName);
		    Node_FromStr(Element,ClassString,TSIZEOF(ClassString),EBML_INFO_NAME(cb));
            if (ErrCallback)
                ErrCallback(cookie,MASTER_CHECK_MISSING_MANDATORY,ClassString,NULL);
	    }
	}
#endif
}

static inline mkv_timestamp_t MATROSKA_CueTimestamp(const matroska_cuepoint *Cue, libmatroska::KaxInfo *SegmentInfo)
{
    const ebml_integer *Timestamp;
    assert(EBML_ElementIsType((ebml_element*)Cue, libmatroska::KaxCuePoint));
    const auto & SegmentScale = GetChild<libmatroska::KaxTimestampScale>(*SegmentInfo);
	mkv_timestamp_t timestamp;
	if (!Cue->Timestamp(timestamp, static_cast<std::uint64_t>(SegmentScale)))
        return INVALID_TIMESTAMP_T;
    // return EBML_IntegerValue(Timestamp) * MATROSKA_SegmentInfoTimestampScale(Cue->SegInfo);
    return timestamp;
}

static inline void MATROSKA_LinkClusterBlocks(matroska_cluster *Cluster, libebml::EbmlMaster *RSegmentInfo, libebml::EbmlMaster *Tracks, bool_t KeepUnmatched, int ForProfile)
{
	// TODO
    auto timestamp = GetChild<libmatroska::KaxClusterTimestamp>(*Cluster);
    auto timestampscale = GetChild<libmatroska::KaxTimestampScale>(*RSegmentInfo);
    Cluster->InitTimestamp(static_cast<std::uint64_t>(timestamp), static_cast<std::uint64_t>(timestampscale));
}

static inline void MATROSKA_LinkCueSegmentInfo(matroska_cuepoint *Cue, ebml_master *SegmentInfo)
{
    // TODO
}

static inline void MATROSKA_LinkCuePointBlock(matroska_cuepoint *CuePoint, matroska_block *Block)
{
    // TODO on KaxCues
    // CuePoint->PositionSet(*Block);
}

static inline void MATROSKA_LinkClusterWriteSegmentInfo(matroska_cluster *Cluster, ebml_master *SegmentInfo)
{
    // TODO
}

static inline err_t MATROSKA_LinkBlockWithWriteTracks(matroska_block *Block, ebml_master *Tracks, int ForProfile)
{
    // TODO
}

static inline err_t MATROSKA_LinkBlockWriteSegmentInfo(matroska_block *Block, ebml_master *SegmentInfo)
{
    // TODO
}

static inline mkv_timestamp_t MATROSKA_ClusterTimestamp(matroska_cluster *Cluster)
{
	return Cluster->GetBlockGlobalTimestamp(0);
}

static inline void MATROSKA_ClusterSetTimestamp(matroska_cluster *Cluster, mkv_timestamp_t Timestamp)
{
    Cluster->InitTimestamp(Timestamp, Cluster->GlobalTimestampScale());
}

static inline mkv_timestamp_t MATROSKA_SegmentInfoTimestampScale(const ebml_master *SegmentInfo)
{
    ebml_integer *TimestampScale = NULL;
    if (SegmentInfo)
    {
        assert(EBML_ElementIsType((ebml_element*)SegmentInfo, MATROSKA_getContextInfo()));
        TimestampScale = (ebml_integer*)EBML_MasterFindChild(SegmentInfo,MATROSKA_getContextTimestampScale());
    }
    if (!TimestampScale)
#if 1 // turn into a macro
        return libmatroska::KaxTimestampScale::GetElementSpec().DefaultValue();
#else
        return MATROSKA_getContextTimestampScale()->DefaultValue;
#endif
    return EBML_IntegerValue(TimestampScale);
}

static inline mkv_timestamp_t MATROSKA_ClusterTimestampScale(matroska_cluster *Cluster, bool_t Read)
{
    // TODO tie the KaxCluster to a KaxInfo
    // return MATROSKA_SegmentInfoTimestampScale(Cluster->ReadSegInfo);
    return Cluster->GlobalTimestampScale();
}

static inline bool MATROSKA_BlockKeyframe(libmatroska::KaxSimpleBlock *Block)
{
    return Block->IsKeyframe();
}

static inline bool MATROSKA_BlockKeyframe(libmatroska::KaxBlockGroup *BlockGroup, const matroska_block *Block)
{
    if (!BlockGroup)
        return false;

	if (FindChild<libmatroska::KaxReferenceBlock>(*BlockGroup))
        return false;

    const auto *Duration = FindChild<libmatroska::KaxBlockDuration>(*BlockGroup);
	if (Duration!=NULL && EBML_IntegerValue(Duration)==0)
        return false;

	return true;
}

static inline void MATROSKA_BlockSetKeyframe(matroska_block *Block, bool_t Set)
{
    if (EBML_ElementIsType(Block, libmatroska::KaxSimpleBlock))
        static_cast<libmatroska::KaxSimpleBlock*>(Block)->SetKeyframe(Set);
    // TODO handle BlockGroup for KaxBlock
}

static inline bool MATROSKA_BlockLaced(const matroska_block *Block)
{
	// TODO move in libmatroska
    assert(Block->GetCurrentLacing() != libmatroska::LacingType::LACING_AUTO);
	return Block->GetCurrentLacing() != libmatroska::LacingType::LACING_NONE;
}

void MATROSKA_BlockSetDiscardable(matroska_block *Block, bool_t Set)
{
    if (EBML_ElementIsType(Block, libmatroska::KaxSimpleBlock))
        static_cast<libmatroska::KaxSimpleBlock*>(Block)->SetDiscardable(Set);
}

static inline mkv_timestamp_t MATROSKA_BlockTimestamp(matroska_cluster *Cluster, const matroska_block *Block)
{
#if 0 //TODO
    assert(Node_IsPartOf(Block,MATROSKA_BLOCK_CLASS));
	if (Block->GlobalTimestamp!=INVALID_TIMESTAMP_T)
		return Block->GlobalTimestamp;
    if (Block->ReadTrack==NULL)
        return INVALID_TIMESTAMP_T;
    assert(Block->LocalTimestampUsed);
    Block->GlobalTimestamp = MATROSKA_ClusterTimestamp((matroska_cluster*)Cluster) + (mkv_timestamp_t)(Block->LocalTimestamp * MATROSKA_SegmentInfoTimestampScale(Block->ReadSegInfo) * MATROSKA_TrackTimestampScale(Block->ReadTrack));
    MATROSKA_BlockSetTimestamp(Block, Block->GlobalTimestamp, MATROSKA_ClusterTimestamp((matroska_cluster*)Cluster));
    return Block->GlobalTimestamp;
#endif
    return Cluster->GetBlockGlobalTimestamp(Block->GlobalTimestamp());
}

ebml_element *MATROSKA_BlockReadTrack(const matroska_block *Block)
{
    // TODO Block->GetParentCluster();
    // ebml_element *Track;
    // if (Node_GET((node*)Block,MATROSKA_BLOCK_READ_TRACK,&Track)!=ERR_NONE)
    //     return NULL;
    // return Track;
    return nullptr;
}


static inline matroska_block *MATROSKA_GetBlockForTimestamp(matroska_cluster *Cluster, mkv_timestamp_t Timestamp, int16_t Track)
{
    EBML_MASTER_CONST_ITERATOR Block, GBlock;
    for (Block = EBML_MasterChildren(Cluster);EBML_MasterEnd(Block,Cluster);EBML_MasterNext(Block))
    {
        if (EBML_ElementIsType(*Block, libmatroska::KaxBlockGroup))
        {
            for (GBlock = EBML_MasterChildren((libmatroska::KaxBlockGroup*)*Block);EBML_MasterEnd(GBlock,(libmatroska::KaxBlockGroup*)*Block);EBML_MasterNext(GBlock))
            {
                if (EBML_ElementIsType(*GBlock, libmatroska::KaxBlock))
                {
                    if (MATROSKA_BlockTrackNum((matroska_block*)*GBlock) == Track &&
                        MATROSKA_BlockTimestamp(Cluster, (matroska_block*)*GBlock) == Timestamp)
                    {
                        return (matroska_block*)*GBlock;
                    }
                }
            }
        }
        else if (EBML_ElementIsType(*Block, libmatroska::KaxSimpleBlock))
        {
            if (MATROSKA_BlockTrackNum((matroska_block*)*Block) == Track &&
                MATROSKA_BlockTimestamp(Cluster, (matroska_block*)*Block) == Timestamp)
            {
                return (matroska_block*)*Block;
            }
        }
    }
    return NULL;
}


static inline bool_t CheckMandatory(const ebml_master *Element, const libebml::EbmlElement::ShouldWrite &ForProfile)
{
  const auto & MasterContext = EBML_CONTEXT(Element);
  assert(MasterContext.GetSize() != 0);

  unsigned int EltIdx;
  for (EltIdx = 0; EltIdx < EBML_CTX_SIZE(MasterContext); EltIdx++) {
    if (EBML_CTX_IDX(MasterContext,EltIdx).IsMandatory()) {
      libebml::EbmlElement *found = Element->FindElt(EBML_CTX_IDX_INFO(MasterContext,EltIdx));
      if (found == nullptr) {
        const auto & semcb = EBML_CTX_IDX(MasterContext,EltIdx).GetCallbacks();
        const bool hasDefaultValue = semcb.HasDefault();

#if !defined(NDEBUG)
        // you are missing this Mandatory element
//         const char * MissingName = EBML_INFO_NAME(semcb);
#endif // !NDEBUG
        if (!hasDefaultValue)
          return false;
      } else {
        if (found->IsMaster() && !reinterpret_cast<libebml::EbmlMaster *>(found)->CheckMandatory())
          return false;
      }
    }
  }

  return true;
#if 0
    const ebml_semantic *i;
    for (i=Element->Base.Context->Semantic;i->eClass;++i)
    {
        if (i->Mandatory) {
            if (i->DisabledProfile & ForProfile) == 0 &&
                !i->eClass->HasDefault &&
                !EBML_MasterFindChild(Element,i->eClass))
                return 0;
        }
    }
    return 1;
#endif
}

static inline bool_t EBML_MasterCheckMandatory(const ebml_master *Element, bool_t bWithDefault, const libebml::EbmlElement::ShouldWrite & ForProfile)
{
	EBML_MASTER_CONST_ITERATOR Child;
	if (!CheckMandatory(Element, ForProfile))
		return 0;

    for (Child = EBML_MasterChildren(Element);EBML_MasterEnd(Child,Element); EBML_MasterNext(Child))
	// for (Child = EBML_MasterChildren(Element); Child; Child = EBML_MasterNext(Child))
	{
		if ((*Child)->IsMaster() && !EBML_MasterCheckMandatory((ebml_master*)*Child, false, ForProfile))
		// if (Node_IsPartOf(Child,EBML_MASTER_CLASS) && !EBML_MasterCheckMandatory((ebml_master*)Child, bWithDefault, ForProfile))
			return 0;
	}
    return 1;
}

err_t MATROSKA_CuePointUpdate(matroska_cuepoint *Cue, ebml_element *Segment, const libebml::EbmlElement::ShouldWrite & ForProfile);


//////  matroska_seekpoint  ///////

static inline void MATROSKA_LinkMetaSeekElement(matroska_seekpoint *MetaSeek, ebml_element *Link)
{
    // TODO MetaSeek->IndexThis();
}

err_t MATROSKA_MetaSeekUpdate(matroska_seekpoint *MetaSeek)
{
    ebml_element *WSeekID, *WSeekPosSegmentInfo, *RSegment, *Link = NULL;
    size_t IdSize;
    err_t Err;
    uint8_t IdBuffer[4];

    if (EBML_ElementIsType(MetaSeek,libebml::EbmlVoid))
    // if (Node_IsPartOf(MetaSeek,EBML_VOID_CLASS))
        return ERR_NONE;

    assert(EBML_ElementIsType((ebml_element*)MetaSeek, MATROSKA_getContextSeek()));
#if 0 // TODO
    RSegment = EBML_ElementParent(MetaSeek);
    while (RSegment && !EBML_ElementIsType(RSegment, MATROSKA_getContextSegment()))
        RSegment = EBML_ElementParent(RSegment);
    if (!RSegment)
        return ERR_INVALID_DATA;

    Err = Node_GET(MetaSeek,MATROSKA_SEEKPOINT_ELEMENT,&Link);
    if (Err != ERR_NONE)
        return Err;
    if (Link==NULL)
        return ERR_INVALID_DATA;

    WSeekID = EBML_MasterFindFirstElt((ebml_master*)MetaSeek,MATROSKA_getContextSeekID(),1,0,0);
    IdSize = EBML_FillBufferID(IdBuffer,sizeof(IdBuffer),EBML_ElementClassID(Link));
    EBML_BinarySetData((ebml_binary*)WSeekID,IdBuffer,IdSize);
#endif
    WSeekPosSegmentInfo = EBML_MasterFindFirstElt((ebml_master*)MetaSeek,MATROSKA_getContextSeekPosition(),1,0,0);
    EBML_IntegerSetValue((ebml_integer*)WSeekPosSegmentInfo, EBML_ElementPosition(Link) - EBML_ElementPositionData(RSegment));

    return Err;
}

//////  ebml_date  ///////

#include <ctime>

using datetime_t = time_t;

static inline datetime_t GetTimeDate()
{
    return time(NULL);
}

static inline void EBML_DateSetDateTime(libebml::EbmlDate *e, datetime_t d)
{
    e->SetEpochDate(d);
}
