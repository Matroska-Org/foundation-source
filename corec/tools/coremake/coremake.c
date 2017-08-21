/*
  $Id$

  Copyright (c) 2006-2016, CoreCodec, Inc.
  All rights reserved.

  Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions are met:
  * Redistributions of source code must retain the above copyright notice,
    this list of conditions and the following disclaimer.
  * Redistributions in binary form must reproduce the above copyright notice,
    this list of conditions and the following disclaimer in the documentation
    and/or other materials provided with the distribution.
  * Neither the name of the CoreCodec, Inc. nor the names of its contributors
    may be used to endorse or promote products derived from this software
    without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
  THE POSSIBILITY OF SUCH DAMAGE.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>

#define MAX_LINE		8192

#include "directory.h"

#ifndef MAX_PATH
#define MAX_PATH		1024
#endif

#if !defined(_MSC_VER) && !defined(__MINGW32__)
# ifndef strcmpi
#  define strcmpi strcasecmp
# endif
# ifndef stricmp
#  define stricmp strcasecmp
# endif
# ifndef strncmpi
#  define strncmpi strncasecmp
# endif
# ifndef strnicmp
#  define strnicmp strncasecmp
# endif
# define make_dir(x) mkdir(x,S_IRWXU)
#else
# define make_dir(x) mkdir(x)
#endif

static int verbose = 0;

#define MAX_PUSHED_PATH  8
char buildpath[MAX_PUSHED_PATH][MAX_PATH];
int buildflags[MAX_PUSHED_PATH];
int curr_build = 0;

#define ROOT_NAME      "ROOT"
#define UNIVERSE_NAME  "UNIVERSE"

enum build_step
{
    PREBUILD_PRE_CONFIG,    /* up to the CONFIG instruction */
    PREBUILD_BEFORE_CONFIG, /* second parsing before the CONFIG instruction */
    PREBUILD_AFTER_CONFIG,  /* second parsing after the CONFIG instruction */
};

typedef struct reader_static
{
	FILE* f;
	int comment;
    int ext_variable;
	int no;
	int flags;
    int filename_kind;
	int filepos;

} reader_static;

typedef struct reader
{
	reader_static r;
	char *pos;
	char *line;
	char *token;
	char *filename;
	char *project_root;
	char *src_root;
	char *coremake_root;

} reader;

typedef struct item item;
typedef struct itemcond itemcond;

typedef struct build_pos build_pos;
struct build_pos
{
    build_pos* prev;
    item* p;
};

#define FLAG_PATH_NOT_PATH            0
#define FLAG_PATH_SOURCE              1
#define FLAG_PATH_GENERATED           2
#define FLAG_PATH_COREMAKE            3
#define FLAG_PATH_SYSTEM              4
#define FLAG_PATH_MASK             0x0F
#define FLAG_PATH_SET_ABSOLUTE     0x10
#define FLAG_DEFINED               0x20
#define FLAG_REMOVED               0x40
#define FLAG_PROCESSED             0x80
#define FLAG_ATTRIB                0x100
#define FLAG_TOKEN_READY           0x200
#define FLAG_CONFIG_FILE_SET       0x400
#define FLAG_NO_INCLUDE            0x800

#define CMD_COREMAKE                1
#define CMD_AUTOMAKE                2

enum
{
	COND_GET,
	COND_OR,
	COND_AND,
	COND_NOT,
    COND_1,
    COND_0,
};

struct itemcond
{
    int refcount;
	int func;
	item* value;
	itemcond* a;
	itemcond* b;
};

struct item
{
	char* value;
	itemcond* cond;
	item* parent;
	item** child;
	item** childend;
	item** childalloc;
    int flags;
    int stamp;
};

int stamp=1;

void upr(char* s)
{
	for (;*s;++s)
		*s = toupper(*s);
}

void lwr(char* s)
{
	for (;*s;++s)
		*s = tolower(*s);
}

void syntax(reader* p)
{
	printf("syntax error %s:%d\n%s\r\n",p->filename,p->r.no,p->line);
	exit(1);
}

void* zalloc(int size)
{
	void* p = malloc(size);
	if (!p)
	{
		printf("out of memory!\r\n");
		exit(1);
	}
	memset(p,0,size);
	return p;
}

void set_path_type(item *i, int type)
{
    assert((type & FLAG_PATH_MASK)==type);
    i->flags &= ~FLAG_PATH_MASK;
    i->flags |= type;
}

size_t item_childcount(const item* p)
{
	if (!p) return 0;
	return (size_t)(p->childend - p->child);
}

void item_remove(item* i)
{
	item** child;
	item* p = i->parent;
	for (child = p->child;child != p->childend;++child)
		if (*child == i)
		{
			memmove(child,child+1,(p->childend-child-1)*sizeof(item*));
			--p->childend;
			break;
		}

	i->parent = NULL;
}

static void item_add(item* p,item* child)
{
	if (!child)
		return;

	if (child->parent)
		item_remove(child);

	if (p->childalloc == p->childend)
	{
		intptr_t count = p->childend - p->child;
		intptr_t alloc = count+16;
		p->child = (item**)realloc(p->child,sizeof(item*)*alloc);
		if (!p->child)
		{
			printf("out of memory!\r\n");
			exit(1);
		}
		p->childend = p->child+count;
		p->childalloc = p->child+alloc;
	}
	*(p->childend++) = child;
	child->parent = p;
}

item* item_find(const item* parent,const char* value)
{
	if (parent)
	{
		item** child;
		for (child = parent->child;child != parent->childend;++child)
			if (stricmp((*child)->value,value)==0)
				return *child;
	}
	return NULL;
}

int compare_name(const item* a, const item* b)
{
	return stricmp(a->value,b->value)>0;
}

static item* item_find_add(item* parent,const char* value,int defined);

static int item_is_root(const item *p)
{
	return strcmp(p->parent->value, ROOT_NAME) == 0;
}

static int item_is_universe(const item *p)
{
	return strcmp(p->parent->value, UNIVERSE_NAME) == 0;
}

static const item* item_root(const item* p, int full)
{
	while (p->parent && (full || !item_is_root(p)))
		p = p->parent;
	return p;
}

static const item* item_universe(const item* p)
{
	while (p->parent && !item_is_universe(p))
		p = p->parent;
	return p;
}

static item* item_find_add_in_root(item* p,const char* name)
{
	return item_find_add((item*) item_root(p, 0),name,0);
}

static item* item_find_in_root(const item* p, const char* name, int full)
{
	return item_find(item_root(p, full), name);
}

static item* getconfig(const item* p)
{
	return item_find(item_root(p, 0),"config");
}

/* get the first value value of type item */
static item* getvalue(item* p)
{
	if (p)
	{
		item** child;
		for (child=p->child;child!=p->childend;++child)
			if (!((*child)->flags & FLAG_REMOVED))
				return *child;
	}
	return NULL;
}

void setvalue(item* p,const char* value)
{
    item* v = getvalue(p);
    if (v)
    {
        if (strcmp(v->value,value)==0)
            return;
        v->flags |= FLAG_REMOVED;
    }
    item_find_add(p,value,1);
}

struct target_def {
    const char *name;
    const char *output_name;
    int is_lib;
};

static const struct target_def all_targets[] = {
    { "group",       NULL,         0 },
    { "exe",         "output_exe", 0 },
    { "con",         "output_con", 0 },
    { "dll",         "output_dll", 0 },
    { "lib",         "output_lib", 1 },
    { "exe_csharp",  "output_exe", 0 },
    { "con_csharp",  "output_con", 0 },
    { "dll_csharp",  "output_dll", 0 },
    { "lib_csharp",  "output_lib", 1 },
    { "exe_android", "output_android", 0 },
    { "dll_android", "output_android_lib", 1 },
    { "generate",    NULL,         0 },
    { NULL, NULL, 0 }
};

static item* findref(const item* p)
{
	if (p->parent && (stricmp(p->parent->value,"project")==0 ||
		              stricmp(p->parent->value,"dep")==0 ||
		              stricmp(p->parent->value,"use")==0 ||
					  stricmp(p->parent->value,"useinclude")==0 ||
					  stricmp(p->parent->value,"usemerge")==0 ||
					  stricmp(p->parent->value,"usebuilt")==0))
	{
        size_t target;
        item* v = NULL;
		const item* root = item_root(p, 0);
        for (target = 0; !v && all_targets[target].name; target++)
            v = item_find(item_find(root, all_targets[target].name), p->value);
        if (!v)
        {
            /* try other roots */
            const item *all_roots = item_universe(root);
            if (all_roots)
            {
                item** child_root;
                for (child_root = all_roots->child; !v && child_root != all_roots->childend; ++child_root)
                {
                    if (*child_root == root)
                        continue;
                    for (target = 0; !v && all_targets[target].name; target++)
                        v = item_find(item_find(*child_root, all_targets[target].name), p->value);
                }
            }
        }
		return v;
	}
	return NULL;
}

static int compare_ref_use(const item* refa, const item* refb)
{
	size_t i;
	const item* use;

	use = item_find(refb,"use");
	for (i=0;use && i<item_childcount(use);++i)
		if (refa == findref(use->child[i]))
		{
			use = item_find(refa,"use");
			for (i=0;use && i<item_childcount(use);++i)
				if (refb == findref(use->child[i]))
				{
					// circular reference
					return compare_name(refa,refb);
				}

			return 1;
		}

	return 0;
}

static int compare_use(const item* a, const item* b)
{
	const item* refa = findref(a);
	const item* refb = findref(b);
	if (refa && refb)
		return compare_ref_use(refa, refb);
	return 0;
}

void item_sort(item* p,int(*compare)(const item*, const item*))
{
	if (item_childcount(p)>=2)
	{
		int changed;
		do
		{
			size_t i,j,n=item_childcount(p);
            changed=0;

			for (i=0;i<n;++i)
                for (j=i+1;j<n;++j)
                    if (compare(p->child[i],p->child[j]))
				    {
					    item* tmp = p->child[i];
					    p->child[i] = p->child[j];
					    p->child[j] = tmp;
    				    changed=1;
                    }

		} while (changed);
	}
}

static item* item_find_add(item* parent,const char* value,int set_defined)
{
	item* p = item_find(parent,value);
	if (!p)
	{
		p = (item*)zalloc(sizeof(item));
		if (value)
			p->value = strdup(value);
		if (parent)
			item_add(parent,p);
	}
	if (set_defined)
		p->flags |= FLAG_DEFINED;
	return p;
}

void itemcond_delete(itemcond* p)
{
	if (p && --p->refcount==0)
	{
		itemcond_delete(p->a);
		itemcond_delete(p->b);
		free(p);
	}
}

void item_delete(item* p)
{
	item** child;
	if (!p)
		return;
	if (p->parent)
		item_remove(p);
	for (child = p->child;child != p->childend;++child)
	{
		(*child)->parent = NULL;
		item_delete(*child);
	}
	itemcond_delete(p->cond);
	free(p->value);
	free(p);
}

itemcond* itemcond_new(int func, item* value)
{
	itemcond* p = (itemcond*)zalloc(sizeof(itemcond));
    p->refcount = 1;
	p->func = func;
	p->value = value;
	return p;
}

int itemcond_same(itemcond* a,itemcond* b)
{
	if (!a && !b)
		return 1;
	if (!a || !b)
		return 0;
	if (a->func != b->func)
		return 0;
	switch (a->func)
	{
	case COND_GET:
		return a->value == b->value;
	case COND_AND:
	case COND_OR:
		return (itemcond_same(a->a,b->a) && itemcond_same(a->b,b->b)) ||
               (itemcond_same(a->b,b->a) && itemcond_same(a->a,b->b));
	case COND_NOT:
		return itemcond_same(a->a,b->a);
	default:
		return 1;
	}
}

int itemcond_contains(itemcond* cond,item* has)
{
    if (cond->func == COND_AND && has)
    {
        if (cond->a->value == has)
            return 1;
        if (cond->b->value == has)
            return 1;
    }
    return 0;
}

void itemcond_reduce(itemcond* cond)
{
    if (cond->func == COND_OR)
    {
        itemcond_reduce(cond->a);
        itemcond_reduce(cond->b);
        if (cond->a->value)
        {
            if (itemcond_same(cond->a,cond->b))
            {
                cond->func = COND_GET;
                cond->value = cond->a->value;
            }
        }
        if (cond->a->func == COND_GET && itemcond_contains(cond->b,cond->a->value))
        {
            cond->func = COND_GET;
            cond->value = cond->a->value;
        }
        if (cond->b->func == COND_GET && itemcond_contains(cond->a,cond->b->value))
        {
            cond->func = COND_GET;
            cond->value = cond->b->value;
        }
    }
    else
    if (cond->func == COND_AND)
    {
        itemcond_reduce(cond->a);
        itemcond_reduce(cond->b);
        if (cond->a->value)
        {
            if (itemcond_same(cond->a,cond->b))
            {
                cond->func = COND_GET;
                cond->value = cond->a->value;
            }
        }
    }
}

int itemcond_partof(itemcond* a,itemcond* b)
{
    if (itemcond_same(a,b))
        return 1;

    if (a && a->func == COND_OR)
        return itemcond_partof(a->a,b) || itemcond_partof(a->b,b);

    return 0;
}

itemcond* itemcond_dup(itemcond* p)
{
	if (p)
        ++p->refcount;
    return p;
}

void preprocess_condeval(item* p);
int itemcond_eval(itemcond* p)
{
	switch (p->func)
	{
	case COND_GET:
		if (!(p->value->flags & FLAG_PROCESSED))
			preprocess_condeval(p->value);
		return (p->value->flags & FLAG_DEFINED)==FLAG_DEFINED;
	case COND_AND:
		return itemcond_eval(p->a) && itemcond_eval(p->b);
	case COND_OR:
		return itemcond_eval(p->a) || itemcond_eval(p->b);
	case COND_NOT:
		return !itemcond_eval(p->a);
	case COND_0:
        return 0;
	case COND_1:
        return 1;
	default:
		return 1;
	}
}

itemcond* itemcond_and(itemcond* p,itemcond* cond)
{
	if (cond)
	{
		if (p)
		{
			itemcond* v = itemcond_new(COND_AND,NULL);
			v->a = itemcond_dup(cond);
			v->b = p;
			p = v;
		}
		else
			p = itemcond_dup(cond);
	}
	return p;
}

itemcond* itemcond_or(itemcond* p,itemcond* cond)
{
	if (cond && p)
	{
		itemcond* v = itemcond_new(COND_OR,NULL);
		v->a = itemcond_dup(cond);
		v->b = p;
		p = v;
	}
	else
	{
		itemcond_delete(p);
		p = NULL;
	}
	return p;
}

static item* item_getmerge(item* into,const item* child,int removed,int *existed)
{
    item* dup;
    dup = item_find(into,child->value);
	*existed = dup != NULL;
	if (!dup)
		dup = item_find_add(into,child->value,(child->flags & FLAG_DEFINED)==FLAG_DEFINED);
    if (((child->flags & FLAG_REMOVED) || removed) && (!*existed || (dup->flags & FLAG_REMOVED)))
        dup->flags |= FLAG_REMOVED;
    else
        dup->flags &= ~FLAG_REMOVED;
    dup->flags |= (child->flags & ~FLAG_REMOVED);
	if (dup->flags & FLAG_PATH_SOURCE)
	{
		const item *src_root = item_root(child, 0);
		const item *dst_root = item_root(into, 0);
		if (src_root != dst_root)
		{
			/* turn into an absolute path with the source root */
			if (!(dup->flags & FLAG_PATH_SET_ABSOLUTE))
			{
				char new_path[MAX_PATH];
				strcpy(new_path, src_root->value);
				strcat(new_path, dup->value);
				free(dup->value);
				dup->value = strdup(new_path);
				dup->flags |= FLAG_PATH_SET_ABSOLUTE;
			}
		}
	}
    return dup;
}

static void item_merge2(item* into,const item* group,itemcond* cond0,int removed,int append_cond)
{
	item** child;
	itemcond* cond = itemcond_and(itemcond_dup(group->cond),cond0);
	for (child = group->child;child != group->childend;++child)
	{
		int child_exists;
		item* dup = item_getmerge(into,*child,removed,&child_exists);
		item_merge2(dup,*child,cond,removed,child_exists);
	}

	if (!item_childcount(group) || (into->flags & FLAG_ATTRIB))
	{
		if (append_cond)
			into->cond = itemcond_or(into->cond,cond);
		else
		{
			itemcond_delete(into->cond);
			into->cond = itemcond_dup(cond);
		}
	}
	itemcond_delete(cond);
}

static void item_merge(item* into,const item* group,item* filter)
{
	if (!group) return;

	int removed=0;
	item* i;
	itemcond* cond = NULL;
	for (i=group->parent;i && !item_is_root(i);i=i->parent)
		cond = itemcond_and(cond,i->cond);
	if (filter)
	{
		removed = (filter->flags & FLAG_REMOVED)==FLAG_REMOVED;
		cond = itemcond_and(cond,filter->cond);
	}
	item_merge2(into,group,cond,removed,1);
	itemcond_delete(cond);
}

static void item_merge_name(item* dst, const item* src, const char* value, item* cond)
{
	src = item_find(src, value);
	if (src)
	{
		int exists;
		item_merge(item_getmerge(dst, src, 0, &exists), src, cond);
	}
}

void reader_init(reader *p)
{
	memset(p,0,sizeof(reader));
    p->filename = (char*)malloc(MAX_PATH);
	p->project_root = (char*)malloc(MAX_PATH);
	p->src_root = (char*)malloc(MAX_PATH);
	p->coremake_root = (char*)malloc(MAX_PATH);
	p->line = (char*)malloc(MAX_LINE);
    p->token = (char*)malloc(MAX_LINE);
    memset(p->filename,0,MAX_PATH);
    memset(p->line,0,MAX_LINE);
    memset(p->token,0,MAX_LINE);
}

void reader_free(reader *p)
{
	free(p->project_root);
	free(p->src_root);
	free(p->coremake_root);
	free(p->filename);
    free(p->line);
    free(p->token);
}

void reader_save(reader *p,reader* save)
{
	p->r.filepos = ftell(p->r.f);
	memcpy(save,p,sizeof(reader_static));
    save->filename = strdup(p->filename);
	save->project_root = strdup(p->project_root);
	save->src_root = strdup(p->src_root);
	save->coremake_root = strdup(p->coremake_root);
	save->line = strdup(p->line);
    save->token = strdup(p->token);
    save->pos = save->line + (p->pos - p->line);
}

void reader_restore(reader *p,reader* save)
{
	memcpy(p,save,sizeof(reader_static));
    strcpy(p->filename,save->filename);
	strcpy(p->project_root, save->project_root);
	strcpy(p->src_root, save->src_root);
	strcpy(p->coremake_root, save->coremake_root);
	strcpy(p->line,save->line);
    strcpy(p->token,save->token);
    p->pos = p->line + (save->pos - save->line);
	fseek(p->r.f,p->r.filepos,SEEK_SET);
}

static int reader_line(reader* p)
{
	size_t i;
	long before_pos = ftell(p->r.f);
	p->line[0] = 0;
	fgets(p->line,MAX_LINE,p->r.f);
	if (feof(p->r.f) && p->line[0]==0)
		return 0;
	i=strlen(p->line);
	fseek(p->r.f, before_pos + i, SEEK_SET);
	while (i && isspace(p->line[--i]))
		p->line[i]=0;
	p->pos = p->line;
	++p->r.no;
	return 1;
}

static char* strins(char* s,const char* begin,const char* end)
{
	size_t n = end?end-begin:strlen(begin);
	memmove(s+n,s,strlen(s)+1);
	memcpy(s,begin,n);
	return s;
}

char* strdel(char* s,const char* end)
{
	memmove(s,end,strlen(end)+1);
	return s;
}

void reader_comment(reader* p)
{
	char* s = p->line;
	while (*s)
	{
		if (p->r.comment)
		{
			char* s0 = s;
			for (;*s;++s)
				if (s[0]=='*' && s[1]=='/')
				{
					s = strdel(s0,s);
					p->r.comment = 0;
					break;
				}

			if (p->r.comment)
			{
				s = s0;
				*s = 0;
			}
		}
		else
		{
			for (;*s;++s)
			{
				if (s[0]=='/' && s[1]=='*')
				{
					s = strdel(s,s+2);
					p->r.comment = 1;
					break;
				}
				if (s[0]=='/' && s[1]=='/')
				{
					*s = 0;
					break;
				}
			}
		}
	}
}

int iscond(int ch)
{
	return strchr("<>[]&|=+-!",ch) != NULL;
}

int isname(int ch)
{
	return isalpha(ch) || isdigit(ch) || ch=='_' || ch=='-' || ch=='#' || ch=='/' || ch=='\\' || ch=='.' || ch=='%' || ch=='\'';
}

static int reader_read(reader* p)
{
	char* s = p->pos;
	for (;;)
	{
		if (!*s)
		{
			if (!reader_line(p))
			{
				p->token[0] = 0;
				return 0;
			}
			s = p->pos;
		}

		if (p->r.comment)
		{
			char* s0 = s;
			for (;*s;++s)
				if (s[0]=='*' && s[1]=='/')
				{
					s = strdel(s0,s+2);
					p->r.comment = 0;
					break;
				}
		}
        else if (p->r.ext_variable)
        {
            for (; *s; ++s)
            {
                p->token[p->r.ext_variable++] = *s;
                if (!isname(*s) && *s != ')')
                {
                    s++;
                    break;
                }
            }
            p->token[p->r.ext_variable] = 0;
            p->r.ext_variable = 0;
            break;
        }
		else
		{
			while (*s && isspace(*s))
				++s;

			if (s[0]=='/' && s[1]=='*')
			{
				s = strdel(s,s+2);
				p->r.comment = 1;
			}
			else
			if (s[0]=='/' && s[1]=='/')
				*s = 0;
			else
			if (isname(*s))
			{
				int i;
				for (i=0;*s && isname(*s);++s,++i)
					p->token[i] = *s;
				p->token[i] = 0;
				break;
			}
			else
			if (*s=='"')
			{
				int i;
				++s;
				for (i=0;*s && (s[0]!='"' || s[1]=='"');++s,++i)
                {
                    if (s[0]=='"')
                        ++s;
					p->token[i] = *s;
                }
				p->token[i] = 0;
				if (*s=='"')
					++s;
				break;
            }
            else
            if (s[0] == '$' && s[1] == '(')
            {
                p->token[0] = *(s++);
                p->token[1] = *(s++);
                p->r.ext_variable = 2;
            }
            else
            if (*s == '<')
            {
                int i;
                ++s;
                for (i = 0; *s && s[0] != '>'; ++s, ++i)
                {
                    if (s[0] == '>')
                        ++s;
                    p->token[i] = *s;
                }
                p->token[i] = 0;
                if (*s == '>')
                    ++s;
                break;
            }
            else
			if (*s)
			{
				p->token[0] = *(s++);
				p->token[1] = 0;

				if (*s && iscond(s[-1]) && iscond(*s))
				{
					p->token[1] = *(s++);
					p->token[2] = 0;
				}
				break;
			}
		}
	}

	p->pos = s;
	p->r.flags |= FLAG_TOKEN_READY;
	return 1;
}

int reader_eof(reader* p)
{
	if (p->r.flags & FLAG_TOKEN_READY)
		return 0;
	return !reader_read(p);
}

static int reader_istoken(reader* p,const char* token)
{
	if (!(p->r.flags & FLAG_TOKEN_READY) && !reader_read(p))
		return 0;

	if (stricmp(p->token,token)!=0)
		return 0;

	p->r.flags &= ~FLAG_TOKEN_READY;
	return 1;
}

int reader_istoken_n(reader* p,const char* token, int n)
{
	if (!(p->r.flags & FLAG_TOKEN_READY) && !reader_read(p))
		return 0;

	if (strnicmp(p->token,token,n)!=0)
		return 0;

	return 1;
}

void reader_token_skip(reader* p, int n)
{
    strdel(p->token,p->token+n);
    if (!p->token[0])
    	p->r.flags &= ~FLAG_TOKEN_READY;
}

int reader_tokenline(reader* p,int onespace)
{
	char* s = p->pos;

	assert(!(p->r.flags & FLAG_TOKEN_READY));

    if (onespace >= 0)
    {
	    while (*s && isspace(*s))
	    {
		    ++s;
		    if (onespace) break;
	    }
    }

	strcpy(p->token,s);
	p->pos = s + strlen(s);

	return p->token[0]!=0;
}

void reader_token(reader* p)
{
	if (((p->r.flags & FLAG_TOKEN_READY) != FLAG_TOKEN_READY) && !reader_read(p))
	//if (!(p->r.flags & FLAG_TOKEN_READY) && !reader_read(p))
		syntax(p);
	p->r.flags &= ~FLAG_TOKEN_READY;
}

void reader_name(reader* p)
{
	reader_token(p);
	if (!isalpha(p->token[0]) && p->token[0]!='_' && p->token[0]!='.' && p->token[0]!='\\')
		syntax(p);
}

static char* getfilename(const char* path)
{
	char* i = strrchr(path,'/');
	if (i) return i+1;
	return (char*)path;
}

void removedrive(char *path)
{
    if (((path[0]>='a' && path[0]<='z') || (path[0]>='A' && path[0]<='Z')) && path[1]==':')
	    memmove(path,path+2,strlen(path));
}

void truncfilename(char* path)
{
	char* i = getfilename(path);
	memmove(path,i,strlen(i)+1);
	i = strrchr(path,'.');
	if (i) *i=0;
}

void truncfilefourcc(char* path)
{
	char* i = getfilename(path);
	memmove(path,i,strlen(i)+1);
    path[4] = 0;
    while (strlen(path)<4)
        strcat(path,"_");
}

void truncfileext(char* path)
{
	char* i = getfilename(path);
	char* ext = strrchr(i,'.');
	if (ext)
		memmove(path,ext+1,strlen(ext+1)+1);
	else
		path[0]=0;
}

void truncfourcc(char* s)
{
	size_t len = strlen(s);
	if (len>2 && s[0]=='\'' && s[len-1]=='\'')
	{
		strdel(s+len-1,s+len);
		strdel(s,s+1);
	}
}

void trunchex(char* s)
{
	if (s[0]=='0' && s[1]=='x')
		strdel(s,s+2);
}

void unstring(char *s)
{
    size_t len;
    if (s[0] != '"')
        return;
    len = strlen(s);
    if (len > 1 && s[len - 1] == '"')
    {
        memmove(s, s + 1, len - 2);
        s[len - 2] = '\0';
    }
}

char* nextlevel(const char* path)
{
	char* i = strchr(path,'/');
	if (i) return i+1;
	return NULL;
}

static void addendpath(char* path)
{
	if (path[0] && path[strlen(path)-1]!='/')
		strcat(path,"/");
}

void delendpath(char* path)
{
	if (path[0] && path[strlen(path)-1]=='/')
		path[strlen(path)-1] = 0;
}

void truncfilepath(char* path,int delend)
{
	char* i = getfilename(path);
	if (i) *i=0;
	if (delend)
	{
		delendpath(path);
		if (!path[0])
			strcpy(path,".");
	}
	else
		addendpath(path);
}

void truncfileupper(char* path)
{
    delendpath(path);
    truncfilepath(path,0);
    if (!path[0])
        strcpy(path,"./");
}

static void pathunix(char* path)
{
	int i;
	for (i=0;path[i];++i)
		if (path[i]=='\\')
			path[i]='/';
}

void pathdos(char* path)
{
	int i;
	for (i=0;path[i];++i)
		if (path[i]=='/')
			path[i]='\\';
}

void reversestr(char *value)
{
    char tmpstr[MAX_LINE];
    size_t i,len=strlen(value);
    for (i=0;i<len;++i)
        tmpstr[i]=value[len-i-1];
    memcpy(value,tmpstr,len);
    value[len]=0;
}

void escapestr(char *value)
{
    char *s;
    s = strchr(value,'\\');
    while (s)
    {
        memmove(s+1,s,strlen(s)+1);
        value[s-value] = '\\';
        s = strchr(s+2,'\\');
    }
    s = strchr(value,'"');
    while (s)
    {
        memmove(s+1,s,strlen(s)+1);
        value[s-value] = '\\';
        s = strchr(s+2,'"');
    }
}

void escapepath(char *value, int mode)
{
    char *s;
    s = strchr(value,'/');
    while (s)
    {
        if (mode==2)
            *s = '_';
        else if (mode==3)
            *s = '.';
        s = strchr(s+1,'/');
    }
}

int ispathabs(const char *path)
{
    return (path && (path[0] == '$' || path[0] == '/' || path[0] == '\\' || (isalpha(path[0]) && path[1] == ':' && (path[2] == '/' || path[2] == '\\'))));
}

void simplifypath(char* path, int head);
void getrelpath(char* path, int path_flags, const char* __curr, int curr_flags, int delend, int levelup,
	            const char *project_root, const char *src_root, const char *coremake_root)
{
    char *__path,_path[MAX_PATH];
    char *curr,_curr[MAX_PATH];

    if ((path_flags & FLAG_PATH_MASK) == FLAG_PATH_NOT_PATH)
        return;

    __path = path;
    strcpy(_path,path);
    pathunix(_path);
    path = _path;
    strcpy(_curr,__curr);
    pathunix(_curr);
    curr = _curr;

	if (path_flags & FLAG_PATH_SET_ABSOLUTE)
	{
        assert(ispathabs(curr));
        size_t same = 0;
        while (curr[same] == path[same])
            same++;
        while (same && path[same] != '/')
            same--;
        path += same;
        curr += same;
	}
    else
    {
        const char *abspath = NULL;
        // ensure that we deal with absolute pathes to compare
        assert(ispathabs(curr));

        switch (path_flags & FLAG_PATH_MASK)
        {
        case FLAG_PATH_SOURCE:
            abspath = src_root;
            break;
        case FLAG_PATH_GENERATED:
            abspath = project_root;
            break;
        case FLAG_PATH_COREMAKE:
            abspath = coremake_root;
        }

        if (abspath)
        {
            size_t abspath_len = strlen(abspath);
            if (strnicmp(path, abspath, abspath_len) != 0)
                strins(path, abspath, NULL);
        }

        simplifypath(path, 1);
        simplifypath(curr, 1);
    }

    for (;;)
	{
		char* i = nextlevel(path);
		char* j = nextlevel(curr);

		if (i && j && (i-path)==(j-curr) && strnicmp(path,curr,i-path)==0)
		{
			strdel(path,i);
			curr = j;
		}
		else
			break;
	}

	while ((curr = nextlevel(curr)) != NULL)
    {
        if (--levelup < 0)
		    strins(path,"../",NULL);
    }

	if (!path[0])
		strcpy(path,"./");

    if (delend)
		delendpath(path);
    strcpy(__path,path);
}

int strip_path_abs(char *path, int flags, const char *projc_root, const char *src_root, const char *coremake_root)
{
	const char *abspath = NULL;
    assert((flags & FLAG_PATH_MASK) != FLAG_PATH_NOT_PATH);
    switch (flags & FLAG_PATH_MASK)
    {
    case FLAG_PATH_GENERATED:
		abspath = projc_root;
		break;
    case FLAG_PATH_COREMAKE:
		abspath = coremake_root;
        break;
    case FLAG_PATH_SOURCE:
		abspath = src_root;
        break;
    case FLAG_PATH_SYSTEM: // do nothing
        break;
    default: assert(0);
    }

	if (abspath)
	{
		size_t abspath_len = strlen(abspath);
		if (abspath_len && strstr(path, abspath) == path)
			memmove(path, path + abspath_len, strlen(path + abspath_len) + 1);
		else
			return !(flags & FLAG_PATH_SET_ABSOLUTE);
	}
	return 1;
}

static void reader_strip_abs(reader* p)
{
    int relpath = strip_path_abs(p->token,p->r.flags,p->project_root, p->src_root, p->coremake_root);
    if (relpath)
        p->r.flags &= ~FLAG_PATH_SET_ABSOLUTE;
}

void reader_filename(reader* p, int dst_flags)
{
    assert((dst_flags & FLAG_PATH_MASK) != FLAG_PATH_NOT_PATH);
    if (stricmp(p->token,".")==0)
        p->token[0]='\0';

    if ((dst_flags & FLAG_PATH_SET_ABSOLUTE)==0 && ((p->token[0] != '$' || p->token[1] != '(') && p->token[0] != '/'))
    {
        char* end = getfilename(p->filename);
        while (p->token[0]=='.' && p->token[1]=='.' && end != p->filename)
        {
            char* i;
            for (i=end-1;i>p->filename;--i)
                if (i[-1]=='/')
                    break;

            strdel(p->token,p->token+2);
            if (p->token[0]=='/')
                strdel(p->token,p->token+1);

            end = i;
        }
        strins(p->token,p->filename,end);
        p->r.flags &= ~FLAG_PATH_MASK;
        p->r.flags |= p->r.filename_kind & FLAG_PATH_MASK;
        // adjust the pathes according to their modifier
        if ((dst_flags & FLAG_PATH_MASK) != (p->r.filename_kind & FLAG_PATH_MASK))
        {
            if (p->coremake_root[0] || (dst_flags & FLAG_PATH_MASK) != FLAG_PATH_COREMAKE) // keep the full path to get the coremake_root
                reader_strip_abs(p);

            switch (dst_flags & FLAG_PATH_MASK)
            {
            case FLAG_PATH_GENERATED:
                strins(p->token,p->project_root,NULL);
                break;
            case FLAG_PATH_COREMAKE:
                strins(p->token,p->coremake_root,NULL);
                break;
            case FLAG_PATH_SOURCE:
                strins(p->token,p->src_root,NULL);
                break;
            }
            p->r.flags &= ~FLAG_PATH_MASK;
            p->r.flags |= (dst_flags & FLAG_PATH_MASK);
        }
    }
    pathunix(p->token);
}

itemcond* load_cond0(reader* file,item* config);
itemcond* load_cond2(reader* file,item* config)
{
	itemcond* p;
	if (reader_istoken(file,"("))
	{
		p = load_cond0(file,config);
		if (!reader_istoken(file,")"))
			syntax(file);
	}
	else
	{
		if (reader_istoken(file,")"))
			syntax(file);
		reader_name(file);
		p = itemcond_new(COND_GET,item_find_add(config,file->token,0));
	}
	return p;
}

itemcond* load_cond1(reader* file,item* config)
{
	if (reader_istoken(file,"!"))
	{
		itemcond* p = itemcond_new(COND_NOT,NULL);
		p->a = load_cond1(file,config);
		return p;
	}
	else
		return load_cond2(file,config);
}

itemcond* load_cond0(reader* file,item* config)
{
	itemcond* a = load_cond1(file,config);
	if (reader_istoken(file,"||"))
	{
		itemcond* p = itemcond_new(COND_OR,NULL);
		p->a = a;
		p->b = load_cond0(file,config);
		return p;
	}
	if (reader_istoken(file,"&&"))
	{
		itemcond* p = itemcond_new(COND_AND,NULL);
		p->a = a;
		p->b = load_cond0(file,config);
		return p;
	}
	return a;
}

itemcond* load_cond(item* item, reader* file, int force)
{
	itemcond* p = NULL;
	if (reader_istoken(file,"("))
	{
		p = load_cond0(file,getconfig(item));
		if (!reader_istoken(file,")"))
			syntax(file);
	}
	else
	if (force)
		p = load_cond0(file,getconfig(item));

	return p;
}

static void settle_root(item *root, const char *src_root, const char *proj_root, const char *coremake_root)
{
	item* i;

	item_find_add_in_root(root, "config");

	i = item_find_add(root, "rootpath", 1);
	i = item_find_add(i, src_root, 1);
	set_path_type(i, FLAG_PATH_SOURCE);
    if (ispathabs(src_root))
        i->flags |= FLAG_PATH_SET_ABSOLUTE;

	i = item_find_add(root, "builddir", 1);
	i = item_find_add(i, proj_root, 1);
	set_path_type(i, FLAG_PATH_GENERATED);
    if (ispathabs(proj_root))
        i->flags |= FLAG_PATH_SET_ABSOLUTE;

	i = item_find_add(root, "platform_files", 1);
	i = item_find_add(i, coremake_root, 1);
	set_path_type(i, FLAG_PATH_COREMAKE);
    if (ispathabs(coremake_root))
        i->flags |= FLAG_PATH_SET_ABSOLUTE;
}

int load_file(item* root,const char* filename, itemcond* cond0, const char *root_path, const char *src_path, const char *coremake_path);
int load_item(item* root,reader* file,int sub,itemcond* cond0)
{
	int result=0;
	int has_statement = 0;
	item *i,*j=NULL;
    size_t target;
	reader old_reader;
	int new_root = 0;

    while (!reader_eof(file))
	{
		if (reader_istoken(file,"#include"))
		{
			has_statement = 1;
			if (reader_istoken(file,"*/*.proj"))
			{
				char path[MAX_PATH];
				DIR* dir;
				strcpy(path, file->project_root);
				strcat(path,file->filename);
				getfilename(path)[0]='\0';
				delendpath(path);
				dir = opendir(path[0]?path:".");
				if (dir)
				{
					struct dirent* entry;
                    struct stat entrystats;
					addendpath(path);
					while ((entry = readdir(dir)) != NULL)
					{
						sprintf(file->token,"%s%s",path,entry->d_name);
                        stat(file->token,&entrystats);
						if ((entrystats.st_mode & S_IFDIR) && entry->d_name[0]!='.')
						{
    						sprintf(file->token,"%s%s/%s.proj",path,entry->d_name,entry->d_name);
							strip_path_abs(file->token, FLAG_PATH_GENERATED, file->project_root, file->src_root, file->coremake_root);
                            if (!load_file(root,file->token,cond0, file->project_root, file->src_root, file->coremake_root) && strrchr(entry->d_name,'-'))
                            {
                                // if it's a UNIX library name, try without the version
						        char *empty = strrchr(file->token,'-');
                                *empty = 0;
                                strcat(file->token,".proj");
                                load_file(root,file->token,cond0, file->project_root, file->src_root, file->coremake_root);
                            }
						}
					}
					closedir(dir);
				}
			}
			else
			{
                reader_token(file);
				reader_filename(file,0);
				load_file(root,file->token,cond0, file->project_root, file->src_root, file->coremake_root);
			}
		}
		else
		if (reader_istoken(file,"if"))
		{
			has_statement = 1;
            itemcond* condextra = NULL;
			int mode;
			do
			{
				itemcond* cond = load_cond(root,file,1);
                itemcond* cond1 = itemcond_and(itemcond_and(itemcond_dup(cond),cond0),condextra);
				mode = load_item(root,file,2,cond1);
				itemcond_delete(cond1);
				if (mode==1)
				{
					itemcond* not1 = (itemcond*)itemcond_new(COND_NOT,NULL);
					not1->a = itemcond_dup(cond);
                    not1 = itemcond_and(itemcond_and(not1,cond0),condextra);
					load_item(root,file,3,not1);
	    			itemcond_delete(not1);
				}
                else if (mode==2)
                {
					itemcond* not1 = itemcond_new(COND_NOT,NULL);
					not1->a = itemcond_dup(cond);
                    condextra = itemcond_and(condextra,not1);
	    			itemcond_delete(not1);
                }
				itemcond_delete(cond);
			}
			while (mode==2);
            itemcond_delete(condextra);
		}
		else
		if (reader_istoken(file,"else"))
		{
			has_statement = 1;
			if (sub != 2)
				syntax(file);
			result = 1;
			break;
		}
		else
		if (reader_istoken(file,";"))
        {
            //nothing to do
        }
        else
		if (reader_istoken(file,"elif"))
		{
			has_statement = 1;
			if (sub != 2)
				syntax(file);
			result = 2;
			break;
		}
		else
		if (reader_istoken(file,"endif"))
		{
			has_statement = 1;
			if (sub != 2 && sub != 3)
				syntax(file);
			break;
		}
		else
		if (reader_istoken(file,"}"))
		{
			if (sub != 1)
				syntax(file);
			break;
		}
		else
		{
			int config = 0;
			int need_path = 0;
			int filename;
            int uselib;
            int deepercond;
            int attrib;
            int generated_dir;
            int coremake_dir;
            int system_dir;
			itemcond* cond;
			reader_name(file);

            deepercond = !root->parent && stricmp(file->token,"project")==0;

            if (item_is_root(root))
            {
                for (target = 0; all_targets[target].name; target++)
                    if (stricmp(file->token, all_targets[target].name) == 0)
                    {
                        need_path = 1;
                        break;
                    }
                if (stricmp(file->token, "workspace") == 0)
                    need_path = 1;
            }

            uselib = stricmp(file->token,"uselib")==0 || stricmp(file->token,"builtlib")==0;

			filename = stricmp(file->token,"config_file")==0 ||
					   stricmp(file->token,"config_include")==0 ||
					   stricmp(file->token,"config_cleaner")==0 ||
					   stricmp(file->token,"platform_files")==0 ||
					   stricmp(file->token,"expinclude")==0 ||
					   stricmp(file->token,"subinclude")==0 ||
					   stricmp(file->token,"sysinclude")==0 ||
					   stricmp(file->token,"include")==0 ||
					   stricmp(file->token,"include_debug")==0 ||
					   stricmp(file->token,"include_release")==0 ||
					   stricmp(file->token,"os_include")==0 ||
					   stricmp(file->token,"libinclude")==0 ||
					   stricmp(file->token,"libinclude_debug")==0 ||
					   stricmp(file->token,"libinclude_release")==0 ||
					   stricmp(file->token,"def")==0 ||
					   stricmp(file->token,"icon")==0 ||
					   stricmp(file->token,"header")==0 ||
					   stricmp(file->token,"header_force")==0 ||
					   stricmp(file->token,"header_qt4")==0 ||
					   stricmp(file->token,"ui_form_qt4")==0 ||
					   stricmp(file->token,"resource_qt4")==0 ||
					   stricmp(file->token,"install_zip")==0 ||
					   stricmp(file->token,"install_cab")==0 ||
					   stricmp(file->token,"install")==0 ||
					   stricmp(file->token,"android_value")==0 ||
					   stricmp(file->token,"nsi")==0 ||
					   stricmp(file->token,"pmdoc")==0 ||
					   stricmp(file->token,"osx_icon")==0 ||
					   stricmp(file->token,"source_m68k")==0 ||
					   stricmp(file->token,"export_svn")==0 ||
					   stricmp(file->token,"source")==0 ||
					   stricmp(file->token,"sourcedir") == 0 ||
					   stricmp(file->token,"compile")==0 ||
                       stricmp(file->token,"sourceam") == 0 ||
                       stricmp(file->token,"linkfile")==0 ||
					   stricmp(file->token,"crt0")==0 ||
					   stricmp(file->token,"symbian_cert")==0 ||
					   stricmp(file->token,"symbian_key")==0 ||
					   stricmp(file->token,"doxygen")==0 ||
                       stricmp(file->token,"project_svn_revision")==0 || /* TODO really ? */
                       stricmp(file->token,"project_help")==0 ||
                       stricmp(file->token,"config_android_ndk")==0;

            generated_dir = stricmp(file->token,"config_file")==0 ||
                            stricmp(file->token,"config_include")==0;

			new_root = stricmp(file->token, "config_file") == 0;

            coremake_dir = stricmp(file->token,"platform_files")==0;

            system_dir = stricmp(file->token,"config_android_ndk")==0 ||
                         stricmp(file->token,"os_include")==0;

            attrib = filename ||
					   stricmp(file->token,"register_cab")==0 ||
                       stricmp(file->token,"reg")==0 ||
					   stricmp(file->token,"class")==0 ||
                       stricmp(file->token,"no_include") == 0; // we need this because priority would mess up conditions

			if (new_root)
			{
				char root_path[MAX_PATH];
				if (ispathabs(file->filename))
					strcpy(root_path, file->filename);
				else
				{
					strcpy(root_path, file->project_root);
					strcat(root_path, file->filename);
				}
				truncfilepath(root_path,0);
				if (strcmp(root_path, file->src_root) == 0)
					new_root = 0;
				if (new_root)
				{
					if (has_statement)
					{
						printf("CONFIG_FILE needs to be first in the .proj file %s:%d\r\n", file->filename, file->r.no);
						exit(1);
					}
					root = item_find_add(root->parent, root_path, 0);
					/* switch the local reader to a new root */
					reader_save(file, &old_reader);
					strcpy(file->project_root, root_path); /* TODO fix out of tree build */
					strcpy(file->src_root, root_path);
					if (++curr_build > MAX_PUSHED_PATH)
					{
						printf("can't push directory %s, limit reached\r\n", file->project_root);
						exit(1);
					}
					strcpy(buildpath[curr_build], file->project_root);
					settle_root(root, file->src_root, file->project_root, file->coremake_root);
					memmove(file->filename, getfilename(file->filename), strlen(file->filename) + 1);
					//chdir(file->project_root);
				}
			}
			has_statement = 1;

			if (stricmp(file->token,"config")==0)
			{
				config = 1;
				j = getconfig(root);
			}
			else
				j = item_find_add(root,file->token,0);

			if (new_root)
			{
				/* TODO copy "outputpath", "rootpath", etc from the old root or use the global values */
			}

			cond = load_cond(root,file,0);
			cond = itemcond_and(cond,cond0);

			if (cond && config)
			{
				item* v;
				for (v=root;v;v=v->parent)
					cond = itemcond_and(cond,v->cond);
			}

			if (reader_istoken(file,"}"))
			{
				if (sub != 1)
					syntax(file);
				item_find_add(j,"1",1);
				break;
			}

			if (reader_istoken(file,",") || reader_istoken(file,";"))
			{
				item_find_add(j,"1",1);
			}
			else
			if (reader_istoken(file,"{"))
			{
				if (config)
					syntax(file);
				load_item(j,file,1,cond);
			}
			else
			{
				do
				{
                    int exists;

					reader_token(file);
                    if (filename)
                    {
                        int file_flags = generated_dir?FLAG_PATH_GENERATED:(coremake_dir?FLAG_PATH_COREMAKE:(system_dir?FLAG_PATH_SYSTEM:FLAG_PATH_SOURCE));
                        if (ispathabs(file->token))
                        {
                            file_flags |= FLAG_PATH_SET_ABSOLUTE;
                        }
                        reader_filename(file,file_flags);
                    }

                    exists = item_find(j,file->token)!=NULL;
					i = item_find_add(j,file->token,0);
                    if (attrib)
                        i->flags |= FLAG_ATTRIB;

                    if (coremake_dir)
                        set_path_type(i,FLAG_PATH_COREMAKE);
                    else if (generated_dir)
                        set_path_type(i,FLAG_PATH_GENERATED);
                    else if (filename || need_path)
                        set_path_type(i,FLAG_PATH_SOURCE);
                    else
                        set_path_type(i,FLAG_PATH_NOT_PATH);

					if (need_path)
					{
						char path[MAX_PATH];
						item *v = item_find_add(i,"PATH",0);
                        item *j;

                        if (exists)
                        {
                            item* existing = getvalue(v);
                            printf("project already defined at %s!\n%s:%d\n%s\r\n",existing?existing->value:"?",file->filename,file->r.no,file->line);
	                        exit(1);
                        }

                        path[0]=0;
                        strins(path,file->filename,getfilename(file->filename));
                        strip_path_abs(path,file->r.filename_kind, file->project_root, file->src_root, file->coremake_root);
                        if (generated_dir)
                            strins(path,file->project_root,NULL);
                        else if (coremake_dir)
                            strins(path,file->coremake_root,NULL);
                        else
                            strins(path,file->src_root,NULL);

						j = item_find_add(v,path,1);
                        if (generated_dir)
                            set_path_type(j,FLAG_PATH_GENERATED);
                        else if (coremake_dir)
                            set_path_type(j,FLAG_PATH_COREMAKE);
                        else
                            set_path_type(j,FLAG_PATH_SOURCE);

                        // pre-define some items
						item_find_add(i,"OUTPUT",0);
						item_find_add(i,"EXPINCLUDE",0);
						item_find_add(i,"SUBINCLUDE",0);
						item_find_add(i,"SYSINCLUDE",0);
						item_find_add(i,"LIBINCLUDE",0);
						item_find_add(i,"LIBINCLUDE_DEBUG",0);
						item_find_add(i,"LIBINCLUDE_RELEASE",0);
						item_find_add(i,"INCLUDE",0);
						item_find_add(i,"INCLUDE_DEBUG",0);
						item_find_add(i,"INCLUDE_RELEASE",0);
						item_find_add(i,"EXPDEFINE",0);
						item_find_add(i,"DEFINE",0);
						item_find_add(i,"LIBS",0);
						item_find_add(i,"LIBS_DEBUG",0);
						item_find_add(i,"LIBS_RELEASE",0);
						item_find_add(i,"SYSLIBS",0);
					}

                    if (!deepercond)
                    {
                        if (exists)
                            i->cond = itemcond_or(i->cond,cond);
                        else
                            i->cond = itemcond_dup(cond);
                    }

                    if (uselib)
                    {
                        int exists;
                        item *incs;
                        item* i;
                        char path[MAX_PATH];
                        path[0]=0;
                        strins(path,file->filename,getfilename(file->filename));
                        incs = item_find_add(root,"libinclude",0);
                        exists = item_find(incs,path)!=NULL;
                        i = item_find_add(incs,path,0);
                        i->flags = file->r.filename_kind;
                        if (exists)
                            i->cond = itemcond_or(i->cond,cond);
                        else
                            i->cond = itemcond_dup(cond);
                    }

					if (reader_istoken(file,":"))
					{
                        int pri;
                        reader_token(file);
                        if (sscanf(file->token,"%d",&pri)!=1)
                            syntax(file);

                        item_find_add(item_find_add(i,"priority",0),file->token,1);
                    }

					if (reader_istoken(file,"{"))
					{
						if (config)
							syntax(file);
                        load_item(i,file,1,deepercond?cond:NULL);
					}
				}
				while (reader_istoken(file,","));
			}

			itemcond_delete(cond);
		}
	}
	if (new_root)
	{
		--curr_build;
		reader_restore(file, &old_reader);
		//chdir(file->project_root);
	}
	return result;
}

int load_file(item* root,const char* filename, itemcond* cond, const char *projt_root, const char *src_path, const char *coremake_path)
{
	reader r;
	assert(strcmp(root->parent->value,ROOT_NAME)==0);
    reader_init(&r);
	strcpy(r.filename,filename);
	strcpy(r.project_root, projt_root);
	strcpy(r.src_root, src_path);
	strcpy(r.coremake_root, coremake_path);
	pathunix(r.filename);
	r.r.f = fopen(filename,"r");
	r.pos = r.line;
	if (r.r.f)
	{
        r.r.filename_kind = FLAG_PATH_SOURCE;
		load_item(root,&r,0,cond);
		fclose(r.r.f);
        reader_free(&r);
		return 1;
	}
    reader_free(&r);
	return 0;
}

void dumpcond(FILE* f,itemcond* p,int first,int prev,int* startpos,int nobreak)
{
    int pos = ftell(f);
    if (startpos && !nobreak && pos>*startpos+200)
    {
        fprintf(f,"\\\n    ");
        *startpos = pos;
    }

    itemcond_reduce(p);

	if (!first && (p->func==COND_OR || p->func==COND_AND) && p->func!=prev)
		fprintf(f,"(");

	switch (p->func)
	{
	case COND_GET:
		fprintf(f,"defined(%s)",p->value->value);
		break;
	case COND_NOT:
        if (p->a->func == COND_NOT)
		    dumpcond(f,p->a->a,0,COND_NOT,startpos,0);
        else
        {
		    fprintf(f,"!");
		    dumpcond(f,p->a,0,p->func,startpos,1);
        }
		break;
	case COND_OR:
		dumpcond(f,p->a,0,p->func,startpos,0);
		fprintf(f," || ");
		dumpcond(f,p->b,0,p->func,startpos,0);
		break;
	case COND_AND:
		dumpcond(f,p->a,0,p->func,startpos,0);
		fprintf(f," && ");
		dumpcond(f,p->b,0,p->func,startpos,0);
		break;
	case COND_0:
		fprintf(f,"0");
		break;
	case COND_1:
		fprintf(f,"1");
		break;
	}

	if (!first && (p->func==COND_OR || p->func==COND_AND) && p->func!=prev)
		fprintf(f,")");
}

void dumpitem(item* p,int deep)
{
	item** child;
	printf("%*c%s",deep*2,' ',p->value);
	if (p->cond)
	{
		printf(" (");
		dumpcond(stdout,p->cond,1,COND_GET,NULL,0);
		printf(")");
	}
	printf("\r\n");
	for (child=p->child;child!=p->childend;++child)
		dumpitem(*child,deep+1);
}

void usage()
{
	printf("usage: coremake [<options>] <platform>\r\n\r\n");
	printf(" -h : help\r\n");
	printf(" -v : verbose\r\n");
	printf(" -r : root folder (default is current dir)\r\n");
	printf(" -f <file> : specify project file (default is root.proj)\r\n");
	printf(" -d : dump project\r\n");
	printf(" -p : dump project at the end\r\n");
    // TODO: add support for using another config file than config.h
}

void preprocess_setremoved(item* p)
{
	item** child;
	p->flags |= FLAG_REMOVED;
	p->flags &= ~FLAG_DEFINED;
	for (child=p->child;child!=p->childend;++child)
		preprocess_setremoved(*child);
}

void preprocess_condeval(item* p)
{
	item** child;

	p->flags |= FLAG_PROCESSED;

	if (p->cond)
	{
		if (!itemcond_eval(p->cond))
			preprocess_setremoved(p);
	}

	if (!(p->flags & FLAG_REMOVED))
		for (child=p->child;child!=p->childend;++child)
			preprocess_condeval(*child);
}

void preprocess_condstart(item* p)
{
	item** child;
	p->flags &= ~FLAG_PROCESSED;
	p->flags &= ~FLAG_REMOVED;
	for (child=p->child;child!=p->childend;++child)
		preprocess_condstart(*child);
}

void preprocess_config(item* p)
{
	item** child;
	for (child=p->child;child!=p->childend;++child)
		if (!((*child)->flags & FLAG_DEFINED))
			(*child)->flags |= FLAG_REMOVED;
}

void preprocess_condend(item* p)
{
	item** child;

	if (p->cond)
	{
		itemcond_delete(p->cond);
		p->cond = NULL;
	}

	for (child=p->child;child!=p->childend;++child)
	{
		if ((*child)->flags & FLAG_REMOVED)
		{
			item_delete(*child);
			--child;
		}
		else
			preprocess_condend(*child);
	}
}

void preprocess_dependency_project(item* p)
{
	if (!(p->flags & FLAG_PROCESSED))
	{
		// remove not exising dependencies
		size_t i;
		item* use = item_find(p,"use");

		p->flags |= FLAG_PROCESSED;

		for (i=0;i<item_childcount(use);++i)
		{
            item* ref = findref(use->child[i]);
			if (!ref || (ref->flags & FLAG_REMOVED) || ref==p)
			{
				preprocess_setremoved(use->child[i]);
			}
			else
			if (stricmp(ref->parent->value,"lib")==0)
			{
				preprocess_dependency_project(ref);
				// copy use of lib's
				item_merge(use,item_find(ref,"use"),use->child[i]);

                item_merge_name(p,ref,"uselib",use->child[i]);
                item_merge_name(p,ref,"framework",use->child[i]);
                item_merge_name(p,ref,"framework_lib",use->child[i]);
                item_merge_name(p,ref,"privateframework",use->child[i]);
                item_merge_name(p,ref,"libinclude",use->child[i]);
                item_merge_name(p,ref,"libinclude_debug",use->child[i]);
                item_merge_name(p,ref,"libinclude_release",use->child[i]);
                item_merge_name(p,ref,"rpath",use->child[i]);
                item_merge_name(p,ref,"android_permission",use->child[i]);
			}
		}
	}
}

void preprocess_dependency(item* p)
{
	item** child;
	if (!p) return;
	for (child=p->child;child!=p->childend;++child)
		preprocess_dependency_project(*child);
}

void file_finalize(char* name)
{
	char tmp[MAX_PATH];
	char* i = strrchr(name,'.');
	if (i && stricmp(i,".tmp")==0)
	{
		FILE* a;
		FILE* b;
		int same=0;

		strcpy(tmp,name);
		*i = 0;

		a = fopen(tmp,"rb");
		b = fopen(name,"rb");
		if (a && b)
		{
			char bufa[1024];
			char bufb[1024];
			for (;;)
			{
				size_t na = fread(bufa,1,sizeof(bufa),a);
				size_t nb = fread(bufb,1,sizeof(bufb),b);
				if (na != nb || memcmp(bufa,bufb,na)!=0)
					break;
				if (feof(a) || feof(b))
				{
					same=1;
					break;
				}
			}
		}
		if (a) fclose(a);
		if (b) fclose(b);
		if (!same)
		{
			if (verbose)
				printf("updating %s\r\n",name);
			remove(name);
			rename(tmp,name);
		}
		else
			remove(tmp);
	}
}

itemcond* itemcond_print(FILE* f,itemcond* p,itemcond* cond)
{
	if (!itemcond_same(p,cond))
	{
		if (p)
		{
			itemcond_delete(p);
			fprintf(f,"#endif\n");
		}

		p = itemcond_dup(cond);
		if (p)
		{
            int pos = ftell(f);
			fprintf(f,"#if ");
			dumpcond(f,p,1,COND_GET,&pos,0);
			fprintf(f,"\n");
		}
	}
	return p;
}

int is_project_data(item* p)
{
	char name[MAX_LINE];
	strcpy(name,p->value);
	name[8]=0;
	return stricmp(name,"PROJECT_")==0;
}

int is_host_data(item* p)
{
	char name[MAX_LINE];
	strcpy(name,p->value);
	name[5]=0;
	return stricmp(name,"HOST_")==0;
}

#define MAX_PRI     (1<<20)

int getpri(item* p)
{
    int i;
    item* v = item_find(p,"priority");
    if (v && item_childcount(v) && sscanf(v->child[0]->value,"%d",&i)==1)
        return i;
    return MAX_PRI;
}

static int tokeneval(char* s,int skip,build_pos* pos,reader* error, int extra_cmd);
static void create_missing_dirs(const char *path);
static void getabspath(char* path, int path_flags, const char *rel_path, int rel_flags, const char *prj_root, const char *src_root, const char *coremake_root);
static void compile_file(item* p, const char *src, const char *dst, int flags, build_pos *pos, int automake, const char *pjr_root, const char *src_root, const char *coremake_root);
static int build_parse(item* p, reader* file, int sub, int skip, build_pos* pos0, enum build_step prebuild);

void preprocess_stdafx_includes(item* p,int lib, const char *p_root, const char *src_root, const char *coremake_root)
{
	item** child;
	if (!p) return;
	for (child=p->child;child!=p->childend;++child)
	{
		size_t i;
		int prj = 0, host = 0;
        char gen_path[MAX_PATH];
		item* plugin = getvalue(item_find(*child,"plugin"));
		item* no_stdafx = getvalue(item_find(*child,"no_stdafx"));
		item* no_project = getvalue(item_find(*child,"no_project"));
		item* cls = item_find(*child,"class");
		item* reg = item_find(*child,"reg");
		item* path = getvalue(item_find(*child,"path"));
        item *include = item_find_add(*child,"include",0);

        if (!path)
            continue;

        strcpy(gen_path,path->value);
        strip_path_abs(gen_path,path->flags, p_root, src_root, coremake_root);
        getabspath(gen_path,FLAG_PATH_GENERATED,"",FLAG_PATH_GENERATED, p_root, src_root, coremake_root);

        if (!plugin)
		    for (i=0;i<item_childcount(*child);++i)
			    if (item_childcount((*child)->child[i]) && is_project_data((*child)->child[i]))
			    {
				    prj = 1;
                    break;
                }

		for (i=0;i<item_childcount(*child);++i)
			if (item_childcount((*child)->child[i]) && is_host_data((*child)->child[i]))
			{
				host = 1;
				break;
			}

		if (prj && !no_project)
		{
            /* add _project.h */
            item *src;
			char file[MAX_PATH];
            item *add_inc;

            sprintf(file,"%s%s_project.h",gen_path,(*child)->value);
			src = item_find_add(item_find_add(*child,"header",0),file,1);
            set_path_type(src,FLAG_PATH_GENERATED);

            add_inc = item_find_add(include,gen_path,0);
            set_path_type(add_inc,FLAG_PATH_GENERATED);
            add_inc->flags |= FLAG_ATTRIB;
		}

		if (host && !lib)
		{
            /* add _host.h */
            item *src;
			char file[MAX_PATH];
            item *add_inc;

            sprintf(file,"%s%s_host.h",gen_path,(*child)->value);
			src = item_find_add(item_find_add(*child,"header",0),file,1);
            set_path_type(src,FLAG_PATH_GENERATED);

            add_inc = item_find_add(include,gen_path,0);
            set_path_type(add_inc,FLAG_PATH_GENERATED);
            add_inc->flags |= FLAG_ATTRIB;
        }

		if ((item_childcount(reg) || item_childcount(cls) || prj) && !lib && !no_stdafx)
		{
			/* add _stdafx.c */
            item *src;
			char file[MAX_PATH];
            item *add_inc;

            src = item_find_add(item_find_add(*child,"expinclude",0),gen_path,1);
            set_path_type(src,FLAG_PATH_GENERATED);
			sprintf(file,"%s%s_stdafx.c",gen_path,(*child)->value);
			src = item_find_add(item_find_add(*child,"source",0),file,1);
            set_path_type(src,FLAG_PATH_GENERATED);
            src = item_find_add(src,"NO_PCH",1);
            item_find_add(src,".",1);

            add_inc = item_find_add(include,gen_path,0);
            set_path_type(add_inc,FLAG_PATH_GENERATED);
            add_inc->flags |= FLAG_ATTRIB;

			/* add _stdafx.h */
			sprintf(file,"%s%s_stdafx.h",gen_path,(*child)->value);
			src = item_find_add(item_find_add(*child,"header",0),file,1);
            set_path_type(src,FLAG_PATH_GENERATED);
        }
	}
}

/* add the output generated directory in EXPINCLUDE */
void preprocess_generate(item* p)
{
    item **child, *i;
    char config_path[MAX_PATH];
    if (!p) return;
    for (child = p->child; child != p->childend; ++child)
    {
        item* outputpath = getvalue(item_find_in_root(*child, "outputpath",1));
        item* outputdir = item_find(*child, "output_dir");
        strcpy(config_path, outputpath->value);
        if (outputdir)
            strcat(config_path, getvalue(outputdir)->value);
        i = item_find_add(item_find_add(*p->child, "expinclude", 0), config_path, 1);
        set_path_type(i, FLAG_PATH_GENERATED);
    }
}

void preprocess_automake(item* p, const char *pj_root, const char *src_root, const char *coremake_root)
{
    item** child;
    if (!p) return;
    for (child = p->child; child != p->childend; ++child)
    {
        size_t i;
        char gen_path[MAX_PATH];
        item* path = getvalue(item_find(*child, "path"));
        item* src_am = item_find(*child, "sourceam");

        if (!path)
            continue;

        strcpy(gen_path, path->value);
        strip_path_abs(gen_path, path->flags, pj_root, src_root, coremake_root);
        getabspath(gen_path, FLAG_PATH_GENERATED, "", FLAG_PATH_GENERATED, pj_root, src_root, coremake_root);

        for (i = 0; i<item_childcount(src_am); ++i)
        {
            /* add automake compiled files */
            item *src;
            build_pos compile_pos;
            compile_pos.p = src_am;
            compile_pos.prev = NULL;

            char file[MAX_PATH], *s = strdup((src_am->child[i])->value);
            truncfilename(s);
            sprintf(file, "%s%s", gen_path, s);
            free(s);
            src = item_find_add(item_find_add(*child, "source", 0), file, 1);
            set_path_type(src, FLAG_PATH_GENERATED);

            compile_file(src, (src_am->child[i])->value, file, FLAG_PATH_GENERATED, &compile_pos, 1, pj_root, src_root, coremake_root);
        }
    }
}

static void preprocess_stdafx(item* p,int lib, const char *pro_root, const char *src_root, const char *coremake_root)
{
	item** child;
	if (!p) return;
	for (child=p->child;child!=p->childend;++child)
	{
		size_t i,j;
		int prj = 0, host = 0;
        char gen_path[MAX_PATH];
		item* plugin = getvalue(item_find(*child,"plugin"));
		item* no_stdafx = getvalue(item_find(*child,"no_stdafx"));
		item* cls = item_find_add(*child,"class",0);
		item* reg = item_find_add(*child,"reg",0);
		item* use = item_find(*child,"use");
		item* usebuilt = item_find(*child,"usebuilt");
		const item* path = getvalue(item_find_add(*child,"path",0));
		item* src = item_find(*child,"source");
		item* libs = item_find_add(*child,"libs",0);
		item* syslibs = item_find_add(*child,"syslibs",0);
		item* install = item_find_add(*child,"install",0);
		item* uselib = item_find(*child,"uselib");

        if (path)
        {
            strcpy(gen_path,path->value);
            strip_path_abs(gen_path,path->flags, pro_root, src_root, coremake_root);
            getabspath(gen_path,FLAG_PATH_GENERATED,"",FLAG_PATH_GENERATED, pro_root, src_root, coremake_root);
        }

        if (!plugin)
		    for (i=0;i<item_childcount(*child);++i)
			    if (item_childcount((*child)->child[i]) && is_project_data((*child)->child[i]))
			    {
				    prj = 1;
                    if (stricmp((*child)->child[i]->value,"project_svn_revision")==0)
                    {
                        item** k;
		                for (k=((*child)->child[i])->child;k!=((*child)->child[i])->childend;++k)
                        {
                            // open the <project_svn_revision>/.svn/entries file
                            reader r;
                            reader_init(&r);
	                        r.pos = r.line;
                            strcpy(r.filename,(*k)->value);
	                        pathunix(r.filename);
                            addendpath(r.filename);
                            strcat(r.filename,".svn/entries");
	                        r.r.f = fopen(r.filename,"r");

                            if (r.r.f)
                            {
								strcpy(r.project_root, pro_root);
								strcpy(r.src_root, src_root);
								strcpy(r.coremake_root, coremake_root);
								if (reader_line(&r))
                                {
                                    if (stricmp(r.line,"8")!=0 && stricmp(r.line,"9")!=0 && stricmp(r.line,"10")!=0)
				                        printf("unknown .svn/entries version '%s'\r\n",r.line);
                                    else
                                    {
                                        if (reader_line(&r) && reader_line(&r) && reader_line(&r))
                                        {
                                            item* build=item_find_add(*child,"PROJECT_BUILD",1);
                                            item** j;
		                                    for (j=build->child;j!=build->childend;++j)
                                                if (itemcond_partof((*k)->cond,(*j)->cond))
                                                {
                                                    itemcond_delete((*j)->cond);
                                                    (*j)->cond = itemcond_new(COND_0,NULL);
                                                    (*j)->flags |= FLAG_REMOVED;
                                                }

                                            if (j==build->childend)
                                            {
                                                item* line = item_find_add(build,r.line,1);
                                                line->cond = itemcond_dup((*k)->cond);
                                                preprocess_condeval(line);
                                            }
                                        }
                                    }
                                }
                                fclose(r.r.f);
                            }
                            reader_free(&r);
                        }
                    }
			    }

		for (i=0;i<item_childcount(*child);++i)
			if (item_childcount((*child)->child[i]) && is_host_data((*child)->child[i]))
			{
				host = 1;
				break;
			}

		// copy register from source
		for (i=0;i<item_childcount(src);++i)
		{
			item_merge(reg,item_find(src->child[i],"reg"),NULL);
			item_merge(cls,item_find(src->child[i],"class"),NULL);
		}

		if (uselib)
		    for (i=0;i<item_childcount(uselib);++i)
		    {
			    item_merge(reg,item_find(uselib->child[i],"reg"),NULL);
			    item_merge(cls,item_find(uselib->child[i],"class"),NULL);
		    }

		// register libraries as well from use
		for (i=0;i<item_childcount(use);++i)
		{
			item* ref = findref(use->child[i]);
			if (ref && stricmp(ref->parent->value,"lib")==0)
			{
				item* src = item_find(ref,"source");
        		item* uselib = item_find(*child,"uselib");

				for (j=0;j<item_childcount(src);++j)
				{
					item_merge(reg,item_find(src->child[j],"reg"),use->child[i]);
					item_merge(cls,item_find(src->child[j],"class"),use->child[i]);
				}

                if (uselib)
        		    for (j=0;j<item_childcount(uselib);++j)
        		    {
					    item_merge(reg,item_find(uselib->child[j],"reg"),use->child[i]);
					    item_merge(cls,item_find(uselib->child[j],"class"),use->child[i]);
				    }

				item_merge(reg,item_find(ref,"reg"),use->child[i]);
				item_merge(cls,item_find(ref,"class"),use->child[i]);
				item_merge(libs,item_find(ref,"libs"),use->child[i]);
				item_merge(syslibs,item_find(ref,"syslibs"),use->child[i]);
				item_merge(install,item_find(ref,"install"),use->child[i]);
			}
		}

		// register libraries as well from usebuilt
		for (i=0;i<item_childcount(usebuilt);++i)
		{
			item* ref = findref(usebuilt->child[i]);
			if (ref && stricmp(ref->parent->value,"lib")==0)
			{
				item* src = item_find(ref,"source");
        		item* uselib = item_find(*child,"uselib");

				for (j=0;j<item_childcount(src);++j)
				{
					item_merge(reg,item_find(src->child[j],"reg"),usebuilt->child[i]);
					item_merge(cls,item_find(src->child[j],"class"),usebuilt->child[i]);
				}

                if (uselib)
        		    for (j=0;j<item_childcount(uselib);++j)
        		    {
					    item_merge(reg,item_find(uselib->child[j],"reg"),usebuilt->child[i]);
					    item_merge(cls,item_find(uselib->child[j],"class"),usebuilt->child[i]);
				    }

				item_merge(reg,item_find(ref,"reg"),usebuilt->child[i]);
				item_merge(cls,item_find(ref,"class"),usebuilt->child[i]);
				item_merge(libs,item_find(ref,"libs"),usebuilt->child[i]);
				item_merge(syslibs,item_find(ref,"syslibs"),usebuilt->child[i]);
				item_merge(install,item_find(ref,"install"),usebuilt->child[i]);
			}
		}

        if (item_find(getconfig(p),"RESOURCE_COREC"))
        {
        	if (item_find_add(getconfig(p),"COREMAKE_STATIC",0)->flags & FLAG_DEFINED)
            {
                // make resource out of plugin/driver dll files
		        for (i=0;i<item_childcount(use);++i)
		        {
        			item* ref = findref(use->child[i]);
                    if (ref && stricmp(ref->parent->value,"dll")==0 && getvalue(item_find(ref,"nolib")))
                    {
                        item* outputpath = getvalue(item_find_in_root(ref,"outputpath",1));
                        item* output = getvalue(item_find(ref,"output"));
                        if (outputpath && outputpath->value && output && output->value)
                        {
                            item* tmp, *out;
                            char path[MAX_PATH];
                    	    strcpy(path,outputpath->value);
                            addendpath(path);
                            strcat(path,output->value);
                            tmp = item_find_add(ref,"__tmp",1);
                            out = item_find_add(tmp,path,1);
                            set_path_type(out,outputpath->flags & FLAG_PATH_MASK);
                            item_merge(install,tmp,use->child[i]);
                        }
                    }
                }
            }

            if (item_childcount(install))
            {
                itemcond* cond = itemcond_new(COND_GET,item_find(getconfig(p),"RESOURCE_COREC"));
                item* v = item_find_add(*child,"install_resource",0);
    		    item_merge(v,install,NULL);
    		    for (i=0;i<item_childcount(v);++i)
                {
                    char name[MAX_PATH];
                    strcpy(name,v->child[i]->value);
                    truncfilename(name);
                    strins(name,"Resource_",NULL);
                    strcat(name,"_Class");
                    free(v->child[i]->value);
    			    v->child[i]->value = strdup(name);
                    v->child[i]->flags |= FLAG_ATTRIB;
                    item_find_add(item_find_add(v->child[i],"priority",1),"1",1);
                    v->child[i]->cond = itemcond_and(v->child[i]->cond,cond);
                }
                item_merge(cls,v,NULL);
                itemcond_delete(cond);
            }
        }

		if (path)
		{
            preprocess_stdafx_includes(p,lib, pro_root, src_root, coremake_root);

            if (prj)
            {
			    FILE* f;
			    char file[MAX_PATH];
			    sprintf(file,"%s%s_project.h",gen_path,(*child)->value);

			    strcat(file,".tmp");
                create_missing_dirs(file);
			    f = fopen(file,"w");
			    if (f)
			    {
				    itemcond* cond = NULL;
				    fprintf(f,"/* DO NOT EDIT, FILE GENERATED BY COREMAKE */\n\n");
				    fprintf(f,"#ifndef T\n");
				    fprintf(f,"#define T(x) x\n");
				    fprintf(f,"#define HAS_DEFINED_T\n");
				    fprintf(f,"#endif /* T */\n\n");

				    for (i=0;i<item_childcount(*child);++i)
					    if (is_project_data((*child)->child[i]))
					    {
						    for (j=0;j<item_childcount((*child)->child[i]);++j)
						    {
                                build_pos pos;
                                reader r;

							    item* value = (*child)->child[i]->child[j];
							    cond = itemcond_print(f,cond,value->cond);

                                reader_init(&r);
                                strcpy(r.filename,file);
                                r.r.filename_kind = FLAG_PATH_GENERATED;
                                r.r.no = 0;
                                strcpy(r.line,value->value);
                                strcpy(r.token,value->value);
                                pos.p = value;
                                pos.prev = NULL;
                                tokeneval(r.token,0,&pos,&r,0);

                                upr((*child)->child[i]->value);

                                if (stricmp((*child)->child[i]->value,"project_svn_revision")!=0)
                                {
							        if (r.token[0]=='\'' || (isdigit(r.token[0]) && !strchr(r.token,'.') && !strchr(r.token,' ')))
								        fprintf(f,"#define %s %s\n",(*child)->child[i]->value,r.token);
							        else
								        fprintf(f,"#define %s T(\"%s\")\n",(*child)->child[i]->value,r.token);
                                }

                                if (stricmp((*child)->child[i]->value,"project_version")==0 && ((*child)->child[i])->child)
                                {
                                    char proj_name[256];
                                    int major=1, minor=0, revision=0;
                                    sscanf(r.token,"%d.%d.%d",&major,&minor,&revision);
                                    fprintf(f,"#define PROJECT_VERSION_MAJOR %d\n",major);
                                    fprintf(f,"#define PROJECT_VERSION_MINOR %d\n",minor);
                                    fprintf(f,"#define PROJECT_VERSION_REVISION %d\n",revision);

                                    strcpy(proj_name,(*child)->value);
                                    upr(proj_name);
                                    fprintf(f,"#define %s_PROJECT_VERSION 0x%02x%02x%02x\n",proj_name,major,minor,revision);
                                }
                                reader_free(&r);
                            }
					    }

				    cond = itemcond_print(f,cond,NULL);
				    fprintf(f,"\n");
				    fprintf(f,"#ifdef HAS_DEFINED_T\n");
				    fprintf(f,"#undef T\n");
				    fprintf(f,"#undef HAS_DEFINED_T\n");
				    fprintf(f,"#endif /* HAS_DEFINED_T */\n\n");
				    fclose(f);
				    file_finalize(file);
                }
            }

		    if (host && !lib)
		    {
			    FILE* f;
			    char file[MAX_PATH];
			    sprintf(file,"%s%s_host.h",gen_path,(*child)->value);

			    strcat(file,".tmp");
                create_missing_dirs(file);
			    f = fopen(file,"w");
			    if (f)
			    {
				    itemcond* cond = NULL;
				    fprintf(f,"/* DO NOT EDIT, FILE GENERATED BY COREMAKE */\n\n");
				    fprintf(f,"#include \"config.h\"\n\n");

				    for (i=0;i<item_childcount(*child);++i)
					    if (is_host_data((*child)->child[i]))
					    {
						    for (j=0;j<item_childcount((*child)->child[i]);++j)
						    {
							    item* value = (*child)->child[i]->child[j];
							    cond = itemcond_print(f,cond,value->cond);
							    if (value->value[0]=='\'' || (isdigit(value->value[0]) && !strchr(value->value,'.')))
								    fprintf(f,"#define %s %s\n",(*child)->child[i]->value,value->value);
							    else
								    fprintf(f,"#define %s T(\"%s\")\n",(*child)->child[i]->value,value->value);
						    }
					    }

				    cond = itemcond_print(f,cond,NULL);
                    fprintf(f,"\n");
                    fclose(f);
                    file_finalize(file);
			    }
            }

		    if ((item_childcount(reg) || item_childcount(cls) || prj) && !lib && !no_stdafx)
		    {
			    /* generate stdafx.c */
			    FILE* f;
			    char file[MAX_PATH];
			    sprintf(file,"%s%s_stdafx.c",gen_path,(*child)->value);

			    strcat(file,".tmp");
                create_missing_dirs(file);
			    f = fopen(file,"w");
			    if (f)
			    {
				    itemcond* cond = NULL;
				    fprintf(f,"/* DO NOT EDIT, FILE GENERATED BY COREMAKE */\n\n");
				    fprintf(f,"#include \"%s_stdafx.h\"\n",(*child)->value);
				    if (prj)
					    fprintf(f,"#include \"%s_project.h\"\n",(*child)->value);
				    fprintf(f,"\n\n");

				    if (item_childcount(reg) || item_childcount(cls))
				    {
                        item* found;
                        int found_reg = 0;
                        int found_pri;

					    for (i=0;i<item_childcount(cls);++i)
                        {
                            cls->child[i]->flags &= ~FLAG_PROCESSED;
						    fprintf(f,"extern const nodemeta %s[];\n",cls->child[i]->value);
                        }

					    for (i=0;i<item_childcount(reg);++i)
					    {
                            reg->child[i]->flags &= ~FLAG_PROCESSED;
						    fprintf(f,"extern void %s_Init(nodemodule* Module);\n",reg->child[i]->value);
						    fprintf(f,"extern void %s_Done(nodemodule* Module);\n",reg->child[i]->value);
					    }

					    fprintf(f,"\nerr_t %s(nodemodule* Module)\n",plugin?"DLLRegister":"StdAfx_Init");
					    fprintf(f,"{\n");

                        do
                        {
                            found = NULL;
                            found_pri = MAX_PRI+1;

					        for (i=0;i<item_childcount(reg);++i)
					        {
                                if (!(reg->child[i]->flags & FLAG_PROCESSED))
                                {
                                    int pri = getpri(reg->child[i]);
                                    if (found_pri > pri)
                                    {
                                        found_pri = pri;
                                        found_reg = 1;
                                        found = reg->child[i];
                                    }
                                }
					        }
					        for (i=0;i<item_childcount(cls);++i)
					        {
                                if (!(cls->child[i]->flags & FLAG_PROCESSED))
                                {
                                    int pri = getpri(cls->child[i]);
                                    if (found_pri > pri)
                                    {
                                        found_pri = pri;
                                        found_reg = 0;
                                        found = cls->child[i];
                                    }
                                }
					        }

                            if (found)
                            {
                                found->flags |= FLAG_PROCESSED;
						        cond = itemcond_print(f,cond,found->cond);
                                if (found_reg)
						            fprintf(f,"\t%s_Init(Module);\n",found->value);
                                else
    						        fprintf(f,"\tNodeRegisterClassEx(Module,%s);\n",found->value);
					        }

                        } while (found);

					    cond = itemcond_print(f,cond,NULL);

					    fprintf(f,"\treturn ERR_NONE;\n");
					    fprintf(f,"}\n\n");

					    fprintf(f,"void %s(nodemodule* Module)\n",plugin?"DLLUnRegister":"StdAfx_Done");
					    fprintf(f,"{\n");
					    for (i=0;i<item_childcount(reg);++i)
					    {
						    cond = itemcond_print(f,cond,reg->child[i]->cond);
						    fprintf(f,"\t%s_Done(Module);\n",reg->child[i]->value);
					    }
					    cond = itemcond_print(f,cond,NULL);
					    fprintf(f,"}\n\n");
				    }

				    if (prj)
				    {
					    fprintf(f,"\nvoid ProjectSettings(nodecontext* p)\n");
					    fprintf(f,"{\n");
					    fprintf(f,"#ifdef PROJECT_FOURCC\n");
					    fprintf(f,"    fourcc_t FourCC = PROJECT_FOURCC;\n");
					    fprintf(f,"    Node_Set(p,NODECONTEXT_PROJECT_FOURCC,&FourCC,sizeof(FourCC));\n");
					    fprintf(f,"#endif\n");
					    fprintf(f,"#ifdef PROJECT_NAME\n");
					    fprintf(f,"    Node_SetData((node*)p,NODECONTEXT_PROJECT_NAME,TYPE_STRING,PROJECT_NAME);\n");
					    fprintf(f,"#endif\n");
					    fprintf(f,"#ifdef PROJECT_VENDOR\n");
					    fprintf(f,"    Node_SetData((node*)p,NODECONTEXT_PROJECT_VENDOR,TYPE_STRING,PROJECT_VENDOR);\n");
					    fprintf(f,"#endif\n");
					    fprintf(f,"#ifdef PROJECT_HELP\n");
					    fprintf(f,"    Node_SetData((node*)p,NODECONTEXT_PROJECT_HELP,TYPE_STRING,PROJECT_HELP);\n");
					    fprintf(f,"#endif\n");
					    fprintf(f,"#ifdef PROJECT_VERSION\n");
					    fprintf(f,"    Node_SetData((node*)p,NODECONTEXT_PROJECT_VERSION,TYPE_STRING,PROJECT_VERSION);\n");
					    fprintf(f,"#endif\n");
					    fprintf(f,"#ifdef PROJECT_BUILD\n");
					    fprintf(f,"    {\n");
					    fprintf(f,"        int BuildVersion = PROJECT_BUILD;\n");
					    fprintf(f,"        Node_Set(p,NODECONTEXT_PROJECT_BUILD,&BuildVersion,sizeof(BuildVersion));\n");
					    fprintf(f,"    }\n");
					    fprintf(f,"#endif\n");
					    fprintf(f,"#ifdef PROJECT_MIME\n");
					    fprintf(f,"    Node_SetData((node*)p,NODECONTEXT_PROJECT_MIME,TYPE_STRING,PROJECT_MIME);\n");
					    fprintf(f,"#endif\n");
					    fprintf(f,"#ifdef PROJECT_APPID\n");
					    fprintf(f,"    {\n");
					    fprintf(f,"        uint16_t AppId = PROJECT_APPID;\n");
					    fprintf(f,"        Node_Set(p,NODECONTEXT_PROJECT_APPID,&AppId,sizeof(AppId));\n");
					    fprintf(f,"    }\n");
					    fprintf(f,"#endif\n");
					    fprintf(f,"#ifdef PROJECT_PATH\n");
					    fprintf(f,"    Node_SetData((node*)p,NODECONTEXT_PROJECT_PATH,TYPE_STRING,PROJECT_PATH);\n");
					    fprintf(f,"#endif\n");
					    fprintf(f,"}\n");
				    }

				    fclose(f);
				    file_finalize(file);
			    }

			    /* generate _stdafx.h */
			    sprintf(file,"%s%s_stdafx.h",gen_path,(*child)->value);
			    strcat(file,".tmp");
			    f = fopen(file,"w");
			    if (f)
			    {
				    fprintf(f,"/* DO NOT EDIT, FILE GENERATED BY COREMAKE */\n\n");
				    fprintf(f,"#include \"corec/node/node.h\"\n");
				    fprintf(f,"\n");

				    fprintf(f,"#ifdef __cplusplus\n");
				    fprintf(f,"extern \"C\" {\n");
				    fprintf(f,"#endif\n");
				    fprintf(f,"\n");

				    if (item_childcount(reg) || item_childcount(cls))
				    {
					    fprintf(f,"extern err_t %s(nodemodule* Module);\n",plugin?"DLLRegister":"StdAfx_Init");
					    fprintf(f,"extern void %s(nodemodule* Module);\n",plugin?"DLLUnRegister":"StdAfx_Done");
				    }

				    if (prj)
					    fprintf(f,"extern void ProjectSettings(nodecontext* p);\n");

				    fprintf(f,"\n#ifdef __cplusplus\n");
				    fprintf(f,"}\n");
				    fprintf(f,"#endif\n");

				    fclose(f);
				    file_finalize(file);
			    }
            }
		}
	}
}

static void preprocess_dependency_include(item* base, item* list, int keep_exp_inc, int discard_exp_def)
{
	size_t i;
	for (i=0;i<item_childcount(list);++i)
	{
		item* ref = findref(list->child[i]);
		if (ref && ref->stamp != stamp)
		{
            ref->stamp = stamp;

			// add to "expinclude" to "include" or "expinclude"
            item_merge(item_find_add(base,keep_exp_inc?"expinclude":"include",0),item_find(ref,"expinclude"),list->child[i]);
			// also merge "subinclude"
			item_merge(item_find_add(base,"subinclude",0),item_find(ref,"subinclude"),list->child[i]);
			// also merge "sysinclude"
			item_merge(item_find_add(base,"sysinclude",0),item_find(ref,"sysinclude"),list->child[i]);
			// also merge "linkfile"
			item_merge(item_find_add(base,"linkfile",0),item_find(ref,"linkfile"),list->child[i]);
			// also merge "crt0"
			item_merge(item_find_add(base,"crt0",0),item_find(ref,"crt0"),list->child[i]);
			// also merge "expdefine" to "define" or "expdefine"
            if (!discard_exp_def)
			    item_merge(item_find_add(base,"define",0),item_find(ref,"expdefine"),list->child[i]);

            preprocess_dependency_include(base,item_find(ref,"useinclude"),keep_exp_inc,1);
            preprocess_dependency_include(base,item_find(ref,"use"),keep_exp_inc,0);
		}
	}
}

static void preprocess_dependency_init(item* p,int onlysource)
{
	item** child;
	if (!p) return;
	for (child=p->child;child!=p->childend;++child)
	{
		size_t i;
		item* list;
		int empty = 1;

		(*child)->flags &= ~FLAG_PROCESSED;

        ++stamp;
        preprocess_dependency_include(*child,item_find(*child,"useinclude"),0,1);
        preprocess_dependency_include(*child,item_find(*child,"use"),0,0);

		list = item_find(*child,"source");
		for (i=0;i<item_childcount(list);++i)
			if (!(list->child[i]->flags & FLAG_REMOVED) && !item_find(list->child[i],"sys"))
			{
				empty = 0;
				break;
			}

		if (empty) {
			list = item_find(*child, "sourcedir");
			for (i = 0; i < item_childcount(list); ++i)
			if (!(list->child[i]->flags & FLAG_REMOVED) && !item_find(list->child[i], "sys"))
			{
				empty = 0;
				break;
			}
		}

		if (empty && !onlysource)
		{
			list = item_find(*child,"use");
			for (i=0;i<item_childcount(list);++i)
				if (!(list->child[i]->flags & FLAG_REMOVED))
				{
					item* ref = findref(list->child[i]);
					if (ref && !(ref->flags & FLAG_REMOVED) && stricmp(ref->parent->value,"lib")==0)
					{
						empty = 0;
						break;
					}
				}
		}

		if (empty)
			preprocess_setremoved(*child); // empty project
	}
}

static void preprocess_workspace_adddep(item* workspace_use,item* p)
{
	item* ref = findref(p);
	p->flags |= FLAG_PROCESSED;
	if (ref && !(ref->flags & FLAG_REMOVED))
	{
		size_t i;
		item* use = item_find(ref,"use");
		for (i=0;i<item_childcount(use);++i)
			if (!(use->child[i]->flags & FLAG_REMOVED))
			{
				item* dep = item_find_add(workspace_use,use->child[i]->value,1);
				if (!(dep->flags & FLAG_PROCESSED))
					preprocess_workspace_adddep(workspace_use,dep);
			}

		use = item_find(ref,"dep");
		for (i=0;i<item_childcount(use);++i)
			if (!(use->child[i]->flags & FLAG_REMOVED))
			{
				item* dep = item_find_add(workspace_use,use->child[i]->value,1);
				if (!(dep->flags & FLAG_PROCESSED))
					preprocess_workspace_adddep(workspace_use,dep);
			}
	}
}

void preprocess_workspace_init(item* p)
{
	item** child;
	if (!p) return;
	for (child=p->child;child!=p->childend;++child)
		(*child)->flags &= ~FLAG_PROCESSED;
}

void preprocess_workspace(item* p)
{
	item** child;
	if (!p) return;
	for (child=p->child;child!=p->childend;++child)
	{
		item* use = item_find(*child,"use");
		size_t i;
        for (i = 0; i < item_childcount(use); ++i)
        {
            use->child[i]->flags &= ~FLAG_PROCESSED;
            preprocess_workspace_adddep(use, use->child[i]);
        }

		for (i=0;i<item_childcount(use);++i)
		{
			item* ref = findref(use->child[i]);
			if (!ref || (ref->flags & FLAG_REMOVED))
			{
				item_delete(use->child[i]);
				--i;
			}
		}

		if (!item_childcount(use))
		{
			item_delete(*child);
			--child;
		}
#if 0
		else
		{
			// copy all the FRAMEWORK of each (use) into the WORKSPACE (*child)
			item *frameworks = item_find_add(*child,"framework",0);
			for (i=0;i<item_childcount(use);++i)
			{
				item* ref = findref(use->child[i]);
				if (ref)
				{
					item_merge(frameworks,item_find(ref,"framework"),NULL);
				}
			}
		}
#endif
	}
}

static void merge_project(item* target,item* source,item* filter)
{
	size_t j;

	/* copy source into tmp conditionally */
    item* tmp = item_find_add(NULL,"tmp",0);

	item_add(tmp,item_find(source,"path"));

	item_merge(target,source,filter);

	for (j=0;j<item_childcount(tmp);++j)
		item_add(source,tmp->child[j]);

	item_delete(tmp);

}

void replace_use(item* p,const char* remove,item* set)
{
	item** child;
	if (!p) return;
	for (child=p->child;child!=p->childend;++child)
	{
		item* use = item_find(*child,"use");
		item* a = item_find(use,remove);
		if (a && !(a->flags & FLAG_REMOVED))
		{
            // remove old use
			item_delete(a);

			if (stricmp((*child)->value,set->value)!=0)
			{
				item* setuse = item_find(item_find(set,"use"),(*child)->value);
				if (setuse)
				{
                    if ((*child)->parent && stricmp((*child)->parent->value,"lib")==0)
                        continue;

					item_find_add(item_find_add(set,"dep",0),(*child)->value,1);
					item_delete(setuse);
				}

                // replace use
				item_find_add(use,set->value,1);
			}
		}
	}
}

void preprocess_usemerge(item* p)
{
	item** child;
	const item* root;
	if (!p) return;
	root = item_root(p, 0);
	for (child=p->child;child!=p->childend;++child)
	{
		size_t i;
		item* merge = item_find(*child,"usemerge");
		for (i=0; merge && i<item_childcount(merge); ++i)
		{
			item* dll = findref(merge->child[i]);
			if (dll && stricmp(dll->parent->value,"dll")==0 && !(merge->child[i]->flags & FLAG_REMOVED))
			{
                item* use;
				item* lib = item_find_add(item_find_add((item*) root,"lib",0),dll->value,0);

                item_delete(item_find(dll,"nolib"));
                item_delete(item_find(dll,"output"));

		        item_merge(lib,dll,merge->child[i]);
                item_delete(dll);

                use = item_find(item_find(lib,"use"),(*child)->value);
                if (use)
                    item_delete(use);

                use = item_find(item_find(lib,"usebuilt"),(*child)->value);
                if (use)
                    item_delete(use);

				replace_use(item_find(root,"dll"),lib->value,*child);
				replace_use(item_find(root,"lib"),lib->value,*child);
				replace_use(item_find(root,"con"),lib->value,*child);
				replace_use(item_find(root,"exe"),lib->value,*child);

                item_find_add(item_find_add(*child,"use",0),lib->value,1)->flags &= ~FLAG_REMOVED;
			}
            else
			if (dll && stricmp(dll->parent->value,"dll_csharp")==0 && !(merge->child[i]->flags & FLAG_REMOVED))
			{
                item* use;
				item* lib = item_find_add(item_find_add((item*) root,"lib_csharp", 0),dll->value,0);

                item_delete(item_find(dll,"nolib"));
                item_delete(item_find(dll,"output"));

		        item_merge(lib,dll,merge->child[i]);
                item_delete(dll);

                use = item_find(item_find(lib,"use"),(*child)->value);
                if (use)
                    item_delete(use);

                use = item_find(item_find(lib,"usebuilt"),(*child)->value);
                if (use)
                    item_delete(use);

				replace_use(item_find(root,"dll_csharp"),lib->value,*child);
				replace_use(item_find(root,"lib_csharp"),lib->value,*child);
				replace_use(item_find(root,"con_csharp"),lib->value,*child);
				replace_use(item_find(root,"exe_csharp"),lib->value,*child);

                item_find_add(item_find_add(*child,"use",0),lib->value,1)->flags &= ~FLAG_REMOVED;
			}
		}
	}
}

void preprocess_outputname(item* p,const char* outputname)
{
	item* output;
	item** child;
	const item* root;
	if (!p) return;
	root = item_root(p, 0);
	output = getvalue(item_find(getconfig(root),outputname));
	for (child=p->child;child!=p->childend;++child)
	{
		if (!output)
			preprocess_setremoved(*child);
		else
		if (!getvalue(item_find(*child,"output")))
			item_find_add(item_find_add(*child,"output",0),output->value,1);
	}
}

void preprocess_project(item* root)
{
	size_t i;
	item* p = item_find(root, "project");
	if (!p) return;
    for (i=0;i<item_childcount(p);++i)
	{
        item* ref = findref(p->child[i]);
        if (ref)
        {
			merge_project(ref,p->child[i],p->child[i]);
			item_delete(p->child[i]);
			--i;
        }
	}
}

static item *find_group_from_root(item *root, const char *name)
{
	if (!root) return NULL;
	item* base_groups = item_find(root, "group");
	return item_find(base_groups, name);
}

static void preprocess_use_group_root(item *root, item *targets, item *target)
{
	size_t i;
	item* use = item_find(target, "use");
	for (i = 0; i<item_childcount(use); ++i)
	{
		item *child = use->child[i];
        item* group = find_group_from_root(root, child->value);
		if (!group)
		{
			const item *all_roots = item_universe(root);
			if (all_roots)
			{
				item** child_root;
				for (child_root = all_roots->child; !group && child_root != all_roots->childend; ++child_root)
				{
					if (*child_root == root)
						continue;
					if (!item_is_root(*child_root))
						continue;
					group = find_group_from_root(*child_root, child->value);
				}
			}
		}
		if (group)
		{
			merge_project(target, group, child);
			item_delete(child);
			--i; // process the same item again until there is no more 'group'
		}
	}
}

/* replace the "use" of a "group" by the content of the "group" */
static void preprocess_use_group(item *root, const char *target_type)
{
	item** child;
	item* targets = item_find(root, target_type);
	item* sub_root = item_find(root, ROOT_NAME);
	if (sub_root)
		preprocess_use_group(sub_root, target_type);
	if (!targets) return;
	for (child=targets->child;child!=targets->childend;++child)
	{
		preprocess_use_group_root(root, targets, *child);
	}
}

static void preprocess_uselib(item* p,item* ref,item* uselib)
{
	item** child;
	if (!p) return;
	for (child=p->child;child!=p->childend;++child)
        if (!((*child)->flags & FLAG_REMOVED))
	    {
		    size_t i;
		    item* use = item_find(*child,"use");
		    for (i=0;i<item_childcount(use);++i)
		    {
			    if (stricmp(use->child[i]->value,ref->value)==0)
			    {
                    size_t j;
		            item* src = item_find(ref,"source");
		            for (j=0;j<item_childcount(src);++j)
		            {
                        char path[MAX_PATH];
                        strcpy(path,src->child[j]->value);
                        truncfileext(path);
                        if (stricmp(path,"rc")==0)
    			            item_merge(item_find_add(item_find_add(*child,"source",0),src->child[j]->value,1),src->child[j],use->child[i]);

			            item_merge(item_find_add(*child,"reg",0),item_find(src->child[j],"reg"),use->child[i]);
			            item_merge(item_find_add(*child,"class",0),item_find(src->child[j],"class"),use->child[i]);
		            }

	                item_merge(item_find_add(*child,"reg",0),item_find(ref,"reg"),use->child[i]);
	                item_merge(item_find_add(*child,"class",0),item_find(ref,"class"),use->child[i]);
	                item_merge(item_find_add(*child,"libs",0),item_find(ref,"libs"),use->child[i]);
	                item_merge(item_find_add(*child,"syslibs",0),item_find(ref,"syslibs"),use->child[i]);
	                item_merge(item_find_add(*child,"install",0),item_find(ref,"install"),use->child[i]);

	                // add to "include"
	                item_merge(item_find_add(*child,"include",0),item_find(ref,"expinclude"),use->child[i]);
	                // also merge "subinclude"
	                item_merge(item_find_add(*child,"subinclude",0),item_find(ref,"subinclude"),use->child[i]);
	                // also merge "sysinclude"
	                item_merge(item_find_add(*child,"sysinclude",0),item_find(ref,"sysinclude"),use->child[i]);
			        // also merge "libinclude"
			        item_merge(item_find_add(*child,"libinclude",0),item_find(ref,"libinclude"),use->child[i]);
			        // also merge "uselib"
			        item_merge(item_find_add(*child,"uselib",0),item_find(ref,"uselib"),use->child[i]);
			        // also merge "rpath"
			        item_merge(item_find_add(*child,"rpath",0),item_find(ref,"rpath"),use->child[i]);
	                // add to "define"
	                item_merge(item_find_add(*child,"define",0),item_find(ref,"expdefine"),use->child[i]);

				    item_find_add(item_find_add(*child,"uselib",1),uselib->value,1);
				    item_delete(use->child[i]);
				    --i;
			    }
		    }
	    }
}

static void preprocess_builtlib(item* p)
{
	size_t i, target;
	const item* root;
	if (!p) return;
	root = item_root(p, 0);
	for (i=0;i<item_childcount(p);++i)
	{
		item* builtlib = item_find(p->child[i],"builtlib");
        item* value = getvalue(builtlib);
        if (value)
        {
            ++stamp;
            preprocess_dependency_include(p->child[i],item_find(p->child[i],"useinclude"),1,1);
            preprocess_dependency_include(p->child[i],item_find(p->child[i],"use"),1,0);

            for (target = 0; all_targets[target].name; target++)
                preprocess_uselib(item_find(root, all_targets[target].name),p->child[i],value);
			item_delete(p->child[i]);
			--i;
        }
	}
}

void generate_xcodeuid(char *xcodeuid,const char *srcstr)
{
	int i;
	union
	{
		unsigned int d[3];
		unsigned char b[12];
	} code;

    for (i=0;i<3;++i)
        code.d[i] = 1;

	for (i=0;srcstr[i];++i)
	{
		unsigned char ch = (unsigned char)srcstr[i];
		code.d[0] = (code.d[0] + 0x33299112 + code.d[0] * ch) ^ code.d[2];
		code.d[1] = (code.d[1] + 0x1345AF22 + code.d[2] * ch) ^ code.d[1];
		code.d[2] = (code.d[2] + 0x73A92332 + code.d[1] * ch) ^ code.d[0];
	}

	sprintf(xcodeuid,"%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X",
		code.b[0],code.b[1],code.b[2],code.b[3],
		code.b[4],code.b[5],code.b[6],code.b[7],
		code.b[8],code.b[9],code.b[10],code.b[11]);
}

void generate_msmuid(char *msmuid,const char *srcstr)
{
	int i;
    union
    {
        unsigned int d[4];
        unsigned char b[16];
    } code;

    for (i=0;i<4;++i)
        code.d[i] = 1;

    for (i=0;srcstr[i];++i)
    {
        unsigned char ch = (unsigned char)srcstr[i];
        code.d[0] = (code.d[0] + 0x33299112 + code.d[2] * ch) ^ code.d[3];
        code.d[1] = (code.d[1] + 0x1345AF22 + code.d[3] * ch) ^ code.d[2];
        code.d[2] = (code.d[2] + 0x73A92332 + code.d[0] * ch) ^ code.d[1];
        code.d[3] = (code.d[3] + 0x5123ABC1 + code.d[1] * ch) ^ code.d[0];
    }

	sprintf(msmuid,"%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X",
		code.b[0],code.b[1],code.b[2],code.b[3],
		code.b[4],code.b[5],code.b[6],code.b[7],
		code.b[8],code.b[9],code.b[10],code.b[11],
        code.b[12],code.b[13],code.b[14],code.b[15]);
}

void generate_msuid(char *msuid,const char *srcstr)
{
	int i;
    union
    {
        unsigned int d[4];
        unsigned char b[16];
    } code;

    for (i=0;i<4;++i)
        code.d[i] = 1;

    for (i=0;srcstr[i];++i)
    {
        unsigned char ch = (unsigned char)srcstr[i];
        code.d[0] = (code.d[0] + 0x33299112 + code.d[2] * ch) ^ code.d[3];
        code.d[1] = (code.d[1] + 0x1345AF22 + code.d[3] * ch) ^ code.d[2];
        code.d[2] = (code.d[2] + 0x73A92332 + code.d[0] * ch) ^ code.d[1];
        code.d[3] = (code.d[3] + 0x5123ABC1 + code.d[1] * ch) ^ code.d[0];
    }

    sprintf(msuid,"{%02X%02X%02X%02X-%02X%02X-%02X%02X-%02X%02X-%02X%02X%02X%02X%02X%02X}",
        code.b[0],code.b[1],code.b[2],code.b[3],
        code.b[4],code.b[5],code.b[6],code.b[7],
        code.b[8],code.b[9],code.b[10],code.b[11],
        code.b[12],code.b[13],code.b[14],code.b[15]);
}

void preprocess_sort_workspace(item* p)
{
	item** child;
	if (!p) return;
	for (child=p->child;child!=p->childend;++child)
	{
#if 0
		size_t i;
		item *frameworks = item_find(*child,"framework");
		for (i=0;i<item_childcount(frameworks);++i)
		{
	        char xcodeuid[25];
			generate_xcodeuid(xcodeuid,frameworks->child[i]->value);
			item_find_add(item_find_add(frameworks->child[i],"xcodegrpuid",0),xcodeuid,1);

			generate_xcodeuid(xcodeuid, xcodeuid);
			item_find_add(item_find_add(frameworks->child[i],"xcodegrpuid2",0),xcodeuid,1);
		}
#endif
        item_sort(item_find(*child,"use"), compare_use); // for automake "subdirs"
    }
}

void preprocess_presort(item* p)
{
    // before builtlib removes projects
	if (!p) return;
	item_sort(p,compare_ref_use); // symbian libary linking madness...
}

static void generate_uids(item *into, int framework)
{
	char xcodeuid[25];
	char msmuid[40];
	char projfile[MAX_PATH];

	strcpy(projfile, into->parent->parent->value);
	strcat(projfile, into->parent->value);
	strcat(projfile, into->value);

	generate_xcodeuid(xcodeuid, projfile);
	item_find_add(item_find_add(into, framework ? "xcodefrwfile" : "xcodeuid", 0), xcodeuid, 1);

	generate_msmuid(msmuid, projfile);
	item_find_add(item_find_add(into, "msmuid", 0), msmuid, 1);
}

static void preprocess_sort(item* p)
{
	item** child;
	if (!p) return;
	for (child=p->child;child!=p->childend;++child)
	{
		item* src;
        int major,minor,revision;
        size_t i;
        char ver[64];
        char msuid[64];
        char xcodeuid[25];
        char msmuid[40];
		char projfile[MAX_PATH];

		generate_msuid(msuid,(*child)->value);
        item_find_add(item_find_add(*child,"guid",0),msuid,1);

		generate_msuid(msuid,msuid);
        item_find_add(item_find_add(*child,"guidbis",0),msuid,1);

		generate_msuid(msuid,msuid);
        item_find_add(item_find_add(*child,"guid2",0),msuid,1);

		generate_msuid(msuid,msuid);
        item_find_add(item_find_add(*child,"guid3",0),msuid,1);

		generate_msmuid(msmuid,msuid);
        item_find_add(item_find_add(*child,"msmuid",0),msmuid,1);

		generate_msmuid(msmuid,msmuid);
        item_find_add(item_find_add(*child,"msmuid2",0),msmuid,1);

        generate_xcodeuid(xcodeuid,(*child)->value);
        item_find_add(item_find_add(*child,"xcodegrpuid",0),xcodeuid,1);

        generate_xcodeuid(xcodeuid, xcodeuid);
        item_find_add(item_find_add(*child,"xcodegrpuid2",0),xcodeuid,1);

        generate_xcodeuid(xcodeuid, xcodeuid);
        item_find_add(item_find_add(*child,"xcodegrpuid3",0),xcodeuid,1);

        generate_xcodeuid(xcodeuid, xcodeuid);
        item_find_add(item_find_add(*child,"xcodegrpuid4",0),xcodeuid,1);

        generate_xcodeuid(xcodeuid, xcodeuid);
        item_find_add(item_find_add(*child,"xcodegrpuid5",0),xcodeuid,1);

        generate_xcodeuid(xcodeuid, xcodeuid);
        item_find_add(item_find_add(*child,"xcodegrpuid6",0),xcodeuid,1);

        generate_xcodeuid(xcodeuid, xcodeuid);
        item_find_add(item_find_add(*child,"xcodegrpuid7",0),xcodeuid,1);

        generate_xcodeuid(xcodeuid, xcodeuid);
        item_find_add(item_find_add(*child,"xcodegrpuid8",0),xcodeuid,1);

        generate_xcodeuid(xcodeuid, xcodeuid);
        item_find_add(item_find_add(*child,"xcodegrpuid9",0),xcodeuid,1);

        generate_xcodeuid(xcodeuid, xcodeuid);
        item_find_add(item_find_add(*child,"xcodegrpuid10",0),xcodeuid,1);

		item_sort(item_find(*child,"source"), compare_name);
		item_sort(item_find(*child, "sourcedir"), compare_name);
		item_sort(item_find(*child,"use"), compare_use); // symbian libary linking madness...

	    src = item_find(*child,"source");
        for (i=0;i<item_childcount(src);++i)
			generate_uids(src->child[i], 0);

	    src = item_find(*child,"header");
        for (i=0;i<item_childcount(src);++i)
			generate_uids(src->child[i], 0);
		
	    src = item_find(*child,"osx_strings");
        for (i=0;i<item_childcount(src);++i)
			generate_uids(src->child[i], 0);

	    src = item_find(*child,"osx_icon");
        for (i=0;i<item_childcount(src);++i)
			generate_uids(src->child[i], 0);

	    src = item_find(*child,"icon");
        for (i=0;i<item_childcount(src);++i)
			generate_uids(src->child[i], 0);

	    src = item_find(*child,"install");
        for (i=0;i<item_childcount(src);++i)
			generate_uids(src->child[i], 0);

        src = item_find(*child,"framework");
        for (i=0;i<item_childcount(src);++i)
			generate_uids(src->child[i], 1);

        src = item_find(*child,"framework_lib");
        for (i=0;i<item_childcount(src);++i)
			generate_uids(src->child[i], 1);

        src = item_find(*child,"privateframework");
        for (i=0;i<item_childcount(src);++i)
			generate_uids(src->child[i], 1);

		src = item_find(*child,"use");
        for (i=0;i<item_childcount(src);++i)
        {
			item* ref = findref(src->child[i]);
			if (ref)
			{
				strcpy(projfile,(*child)->value);
				strcat(projfile,"lib");
				strcat(projfile,ref->value);

                generate_xcodeuid(xcodeuid, projfile);
				item_find_add(item_find_add(src->child[i],"xcodefrwfile",0),xcodeuid,1);

                generate_msmuid(msmuid, projfile);
			    item_find_add(item_find_add(src->child[i],"msmuid",0),msmuid,1);
			}
        }

		src = item_find(*child,"install_cab");
        for (i=0;i<item_childcount(src);++i)
        {
			strcpy(projfile,(*child)->value);
			strcat(projfile,"install_cab");
			strcat(projfile,src->child[i]->value);

            generate_msmuid(msmuid, projfile);
			item_find_add(item_find_add(src->child[i],"msmuid",0),msmuid,1);

            generate_msmuid(msmuid, msmuid);
			item_find_add(item_find_add(src->child[i],"msmuid2",0),msmuid,1);
        }

		src = item_find(*child,"register_cab");
        for (i=0;i<item_childcount(src);++i)
        {
			strcpy(projfile,(*child)->value);
			strcat(projfile,"register_cab");
			strcat(projfile,src->child[i]->value);

            generate_msmuid(msmuid, projfile);
			item_find_add(item_find_add(src->child[i],"msmuid",0),msmuid,1);

            generate_msmuid(msmuid,msmuid);
            item_find_add(item_find_add(src->child[i],"msmuid2",0),msmuid,1);

		    generate_msmuid(msmuid,msmuid);
            item_find_add(item_find_add(src->child[i],"msmuid3",0),msmuid,1);

		    generate_msmuid(msmuid,msmuid);
            item_find_add(item_find_add(src->child[i],"msmuid4",0),msmuid,1);

		    generate_msmuid(msmuid,msmuid);
            item_find_add(item_find_add(src->child[i],"msmuid5",0),msmuid,1);

		    generate_msmuid(msmuid,msmuid);
            item_find_add(item_find_add(src->child[i],"msmuid6",0),msmuid,1);

		    generate_msmuid(msmuid,msmuid);
            item_find_add(item_find_add(src->child[i],"msmuid7",0),msmuid,1);

		    generate_msmuid(msmuid,msmuid);
            item_find_add(item_find_add(src->child[i],"msmuid8",0),msmuid,1);

		    generate_msmuid(msmuid,msmuid);
            item_find_add(item_find_add(src->child[i],"msmuid9",0),msmuid,1);
        }

		src = item_find_add(*child, "project_name", 0);
        if (!getvalue(src))
            item_find_add(src,(*child)->value,1);

		src = item_find_add(*child, "project_path", 0);
        if (!getvalue(src))
            item_find_add(src,(*child)->value,1);

		src = item_find_add(*child, "project_version", 0);
        if (!getvalue(src))
            item_find_add(src,"1.0.0",1);

        if ((item_find_add(getconfig(*child),"TARGET_PALMOS",0)->flags & FLAG_DEFINED) && !getvalue(item_find_add(*child,"project_fourcc",0)))
            item_find_add(item_find_add(*child,"project_fourcc",0),"'CMAK'",1);

        major=1;
        minor=0;
        revision=0;
        sscanf(getvalue(item_find_add(*child,"project_version",0))->value,"%d.%d.%d",&major,&minor,&revision);

        sprintf(ver,"%d",major);
        item_find_add(item_find_add(*child,"project_version_major",0),ver,1);
        sprintf(ver,"%d",minor);
        item_find_add(item_find_add(*child,"project_version_minor",0),ver,1);
        sprintf(ver,"%d",revision);
        item_find_add(item_find_add(*child,"project_version_revision",0),ver,1);
    }
}

void preprocess_part1(item* root, const char *pr_root, const char *src_root, const char *coremake_root)
{
    item* i;
    item* con_to_exe;
    char config_path[MAX_PATH];
    item *config_file;
    size_t target;
    assert(item_is_root(root));

    i = getvalue(item_find(root, "platformlib"));
    if (i)
    {
        char path[MAX_PATH];
        sprintf(path, "lib/%s/", i->value);
        item_find_add(item_find_add(root, "libpath", 1), path, 1); /* TODO remove, it's never used */
    }
    else
        printf("PLATFORMLIB not defined for this target, USELIB will not work\r\n");

    preprocess_condstart(root);
    preprocess_config(getconfig(root));
    preprocess_condeval(root);

    // add the path with config.h to CONFIG_INCLUDE
    i = item_find_add(root, "config_include", 0);
    config_file = getvalue(item_find(root, "config_file"));
    if (config_file)
        strcpy(config_path, config_file->value);
    else
        config_path[0] = 0;
    truncfilepath(config_path, 0);
    i = item_find_add(i, config_path, 0);
    set_path_type(i, FLAG_PATH_GENERATED);
    i->flags |= FLAG_ATTRIB;
    i->flags |= FLAG_PATH_GENERATED;

    // add the path of the CONFIG_CLEANER file to CONFIG_INCLUDE
    i = item_find(root, "config_cleaner");
    if (i && getvalue(i))
    {
        strcpy(config_path, getvalue(i)->value);
        truncfilepath(config_path, 0);
        i = item_find_add(item_find_add(root, "config_include", 0), config_path, 1);
        set_path_type(i, FLAG_PATH_SOURCE);
    }

    // add the path of PLATFORM_FILES to CONFIG_INCLUDE if COREMAKE_CONFIG_HELPER is set
    if (item_find_add(getconfig(root), "COREMAKE_CONFIG_HELPER", 0)->flags & FLAG_DEFINED)
    {
        i = item_find_add(item_find_add(root, "config_include", 0), coremake_root, 1);
        set_path_type(i, FLAG_PATH_SOURCE);
    }

    // "GROUP con_to_exe": replaces all "con" by "exe" and add "USE con_to_exe"
    con_to_exe = getvalue(item_find(item_find(item_find(root, "group"), "con_to_exe"), "source"));
    if (con_to_exe)
    {
        i = item_find(root, "con");
        if (i)
        {
            item** child;
            for (child = i->child; child != i->childend; ++child)
                if (!((*child)->flags & FLAG_REMOVED))
                {
                    item* j = item_find_add(item_find_add(root, "exe", 0), (*child)->value, 1);
                    item_merge(j, *child, NULL);
                    item_find_add(item_find_add(j, "use", 0), "con_to_exe", 1);
                    item_delete(*child);
                    --child;
                }
        }
    }
    else if (item_find_add(getconfig(root), "COREMAKE_CONSOLE", 0)->flags & FLAG_DEFINED)
    {
        // repleace all "exe" by "con"
        i = item_find(root, "exe");
        if (i)
        {
            item** child;
            for (child = i->child; child != i->childend; ++child)
                if (!((*child)->flags & FLAG_REMOVED))
                {
                    item* j = item_find_add(item_find_add(root, "con", 0), (*child)->value, 1);
                    item_merge(j, *child, NULL);
                    item_delete(*child);
                    --child;
                }
        }
    }

    /* copy all groups into the actual target */
    for (target = 0; all_targets[target].name; target++)
        if (all_targets[target].output_name)
            preprocess_use_group(root, all_targets[target].name);
    preprocess_use_group(root, "workspace");
}

void preprocess_part2(item* root, const char *pr_root, const char *src_root, const char *coremake_root)
{
    item* i;
    size_t target;

    // COREMAKE_STATIC and TARGET_ALWAYS_STATIC: replaces all "dll" by "lib"
	if ((item_find_add(getconfig(root),"COREMAKE_STATIC",0)->flags & FLAG_DEFINED) || (item_find_add(getconfig(root),"TARGET_ALWAYS_STATIC",0)->flags & FLAG_DEFINED))
	{
		i = item_find(root,"dll");
        if (i)
        {
        	item** child;
        	for (child=i->child;child!=i->childend;++child)
                if (!((*child)->flags & FLAG_REMOVED) && ((item_find_add(getconfig(*child),"TARGET_ALWAYS_STATIC",0)->flags & FLAG_DEFINED) || !getvalue(item_find(*child,"never_static"))))
	            {
		            item_merge(item_find_add(item_find_add(root,"lib",0),(*child)->value,1),*child,NULL);
		            item_delete(*child);
                    --child;
        	    }
        }
		i = item_find(root,"dll_csharp");
        if (i)
        {
        	item** child;
        	for (child=i->child;child!=i->childend;++child)
                if (!((*child)->flags & FLAG_REMOVED) && ((item_find_add(getconfig(*child),"TARGET_ALWAYS_STATIC",0)->flags & FLAG_DEFINED) || !getvalue(item_find(*child,"never_static"))))
	            {
		            item_merge(item_find_add(item_find_add(root,"lib_csharp",0),(*child)->value,1),*child,NULL);
		            item_delete(*child);
                    --child;
        	    }
        }
    }

    for (target = 0; all_targets[target].name; target++)
        if (all_targets[target].output_name)
	        preprocess_presort(item_find(root, all_targets[target].name));

    preprocess_builtlib(item_find(root,"project"));
    preprocess_builtlib(item_find(root,"lib"));
    preprocess_builtlib(item_find(root,"lib_csharp"));

    for (target = 0; all_targets[target].name; target++)
        if (all_targets[target].output_name && !all_targets[target].is_lib)
    	    preprocess_usemerge(item_find(root, all_targets[target].name));

    // the .build (or .inc) file needs to define these
    for (target = 0; all_targets[target].name; target++)
        if (all_targets[target].output_name)
	        preprocess_outputname(item_find(root, all_targets[target].name), all_targets[target].output_name);

    for (target = 0; all_targets[target].name; target++)
        if (all_targets[target].output_name)
            preprocess_stdafx_includes(item_find(root, all_targets[target].name), all_targets[target].is_lib, pr_root, src_root, coremake_root);
//	preprocess_stdafx_includes(item_find(p,"dll_android"),0);

    preprocess_generate(item_find(root, "generate"));

    for (target = 0; all_targets[target].name; target++)
        if (all_targets[target].output_name)
            preprocess_dependency_init(item_find(root, all_targets[target].name), all_targets[target].is_lib);

    for (target = 0; all_targets[target].name; target++)
        if (all_targets[target].output_name)
            preprocess_dependency(item_find(root, all_targets[target].name));

    for (target = 0; all_targets[target].name; target++)
        if (all_targets[target].output_name)
	        preprocess_stdafx(item_find(root, all_targets[target].name), all_targets[target].is_lib, pr_root, src_root, coremake_root);
//	preprocess_stdafx(item_find(p,"dll_android"),1);

    for (target = 0; all_targets[target].name; target++)
        if (all_targets[target].output_name)
            preprocess_workspace_init(item_find(root, all_targets[target].name));
	preprocess_workspace(item_find(root,"workspace"));

	preprocess_condend(root);

    for (target = 0; all_targets[target].name; target++)
        if (all_targets[target].output_name)
            preprocess_sort(item_find(root, all_targets[target].name));
    preprocess_sort(item_find(root, "generate"));
	preprocess_sort_workspace(item_find(root,"workspace"));

    for (target = 0; all_targets[target].name; target++)
        if (all_targets[target].output_name)
            preprocess_automake(item_find(root, all_targets[target].name), pr_root, src_root, coremake_root);
}

FILE* file_built;
void simplifypath(char* path, int head)
{
    char* s;
    for (s=path;*s;++s)
    {
        if (strncmp(s,"/../",4)==0 && s!=path)
        {
            char* end = s+4;
            do
            {
				if (s[-1] == '/' || s[-1] == '\\')
					break;
			} while (--s!=path);
            s = strdel(s,end);
            if (s != path)
                --s;
            --s;
        }
        if (strncmp(s,"/./",3)==0 && s!=path)
            memmove(s+1,s+3,strlen(s+3)+1);
        if (strncmp(s,"/.",3)==0 && s!=path)
            s[1]=0;
    }
    if (head)
    {
        while (path[0] == '.' && path[1] == '/')
            memmove(path,path+2,strlen(path+2));
    }
}

static void getabspath(char* path, int path_flags, const char *rel_path, int rel_flags, const char *prj_root, const char *src_root, const char *coremake_root)
{
    assert((path_flags & FLAG_PATH_MASK) != FLAG_PATH_NOT_PATH);
    if (!(path_flags & FLAG_PATH_SET_ABSOLUTE))
    {
        char base[3*MAX_PATH];
        if (rel_path[0])
        {
            base[0] = '\0';
            strins(base,rel_path,getfilename(rel_path));
        }
        else
            switch (rel_flags & FLAG_PATH_MASK)
            {
            case FLAG_PATH_SOURCE:    strcpy(base, src_root); break;
            case FLAG_PATH_GENERATED: strcpy(base, prj_root); break;
            case FLAG_PATH_COREMAKE:  strcpy(base, coremake_root); break;
            default: base[0] = '\0'; // safety
            }
        addendpath(base);
        strcat(base,path);
        simplifypath(base,0);
        strcpy(path,base);
    }
}

char* getname(char* s,char* name)
{
	for (;*s && isspace(*s);++s);
	for (;*s && isname(*s);++s)
		*(name++) = *s;
	*name = 0;
	return s;
}

void urlquote(char *name,int no_backslash)
{
    char *q = strrchr(name,'\"');
    while (q)
    {
        strdel(q,q+1);
        strins(q,"&quot;",NULL);
        if (!no_backslash)
            strins(q,"\\",NULL);
        q = strrchr(name,'\"');
    }
}

static int tokeneval(char* s,int skip,build_pos* pos,reader* error, int extra_cmd)
{
	size_t maskpos,maskend;
	char mask[MAX_LINE];
	size_t outpos;
	char out[MAX_LINE];
	char name[MAX_LINE];
	char value[MAX_LINE];
    int result = FLAG_PATH_NOT_PATH;
    int has_forced_generated = 0;

	for (;*s;++s)
	{
		char* s0 = s;
        if (s[0] == '%' && s[1] == '%')
        {
            if (extra_cmd != CMD_AUTOMAKE)
            {
                if (s[2] == '(')
                {
                    strdel(s, s + 1);
                    s+=2;
                    continue;
                }
                else
                {
                    s += 2;
                    s = getname(s, name);
                    if (stricmp(name, "DIRPUSH") == 0)
                    {
                        int flags;
                        s = strdel(s0, s);
                        s = strins(s, "cd ", NULL);
                        s += 2;
                        while (isspace(*s)) ++s;
                        s0 = s;
                        flags = tokeneval(s, skip, pos, error, extra_cmd);
                        if (++curr_build > MAX_PUSHED_PATH)
                        {
                            printf("can't push directory %s, limit reached\r\n", s);
                            exit(1);
                        }
                        strcpy(name, s);
                        getabspath(name, flags, buildpath[curr_build - 1], buildflags[curr_build - 1], error->project_root, error->src_root, error->coremake_root);
                        addendpath(name);
                        strcpy(buildpath[curr_build], name);
                        buildflags[curr_build] = flags;

                        getrelpath(name, flags, buildpath[curr_build - 1], buildflags[curr_build - 1], 0, 0, error->project_root, error->src_root, error->coremake_root);
                        truncfilepath(name, 1);

                        s = strcpy(s0, name);
                        continue;
                    }
                    else
                        if (stricmp(name, "DIRPOP") == 0)
                        {
                            int flags;
                            s = strdel(s0, s);
                            s = strins(s, "cd ", NULL);
                            s += 3;
                            while (isspace(*s)) ++s;
                            s0 = s;
                            flags = tokeneval(s, skip, pos, error, extra_cmd);
                            if (--curr_build < 0)
                            {
                                printf("can't pop directory limit reached\r\n");
                                exit(1);
                            }
                            strcpy(name, buildpath[curr_build]);
                            getrelpath(name, buildflags[curr_build], buildpath[curr_build + 1], buildflags[curr_build + 1], 0, 0, error->project_root, error->src_root, error->coremake_root);
                            truncfilepath(name, 1);

                            s = strcpy(s0, name);
                            continue;
                        }
                }
            }
        }
		if ((extra_cmd != CMD_AUTOMAKE) && s[0]=='%' && s[1]=='(')
		{
            int in_generated;
            int count;
			int upper;
            int delhex;
			int relpath;
			int filepath;
			int filefourcc;
			int filename;
			int fileext;
            int fileupper;
			int fourcc;
            int levelup;
			int delend;
            int deltrail;
            int addend;
			int dos;
			int reverse;
            int abspath;
            int getused;
            int escape;
            int nodrive;
            int findfile;
            int quote;
            int no_backslash;
            int only_abspath;
            int only_relpath;
			item** child;
			size_t n;
            const char* iname;
            build_pos* ipos;
            item* i;

			s += 2;
            in_generated = *s==(char)0xAF; // ¯
            if (in_generated) ++s;
            if (*s==(char)0xAF && ++in_generated) ++s;
            nodrive = *s==':';
            if (nodrive) ++s;
            count = *s=='=';
            if (count) ++s;
            dos = *s==(char)0x7E; // ~
			if (dos)
                ++s;
			fourcc = *s=='\'';
			if (fourcc) ++s;
			fileupper = *s=='?';
			if (fileupper) ++s;
			filename = *s=='<';
			if (filename) ++s;
			filefourcc = *s==',';
			if (filefourcc) ++s;
			fileext = *s=='>';
			if (fileext) ++s;
            only_abspath = *s==(char)0xBA; // º
			if (only_abspath) ++s;
            only_relpath = *s==(char)0xBA; // º
            if (only_relpath) { ++s; only_abspath=0; }
            relpath = *s=='!';
			if (relpath)
                ++s;
            escape = *s==(char)0x60; // `
			if (escape) ++s;
			while (*s==(char)0x60 && ++escape) ++s;
			upper = *s=='^';
			if (upper) ++s;
			filepath = *s=='/';
			if (filepath) { ++s; if (*s=='!') ++s; else relpath=1; }
			abspath = *s=='|';
            if (abspath) { ++s; relpath=1; }
            levelup = 0;
            while (*s==';' && ++levelup) s++;
			delend = *s=='@';
			if (delend) { ++s; relpath=1; }
            deltrail = *s==(char)0xA7; // §
			if (deltrail) ++s;
			addend = *s=='+';
			if (addend) { ++s; }
			delhex = *s=='#';
			if (delhex) ++s;
            reverse = *s=='_';
            if (reverse) ++s;
            getused = *s=='*';
            if (getused) ++s;
            findfile = *s==(char)0xF7; // ÷
            if (findfile) ++s;
            quote = *s=='&';
            if (quote) ++s;
            no_backslash = *s=='&';
            if (no_backslash) ++s;

            if (!filename)
            {
                if (in_generated==1)
                {
                    has_forced_generated = 1;
                    result |= FLAG_PATH_GENERATED;
                }
                else if (in_generated)
                {
                    has_forced_generated = 2;
                    result |= FLAG_PATH_SYSTEM;
                }
            }

            if (relpath && (buildflags[curr_build] & FLAG_PATH_SET_ABSOLUTE))
                abspath = 1;

			s = getname(s,name);

            ipos = pos;
            iname = name;
            while (iname[0]=='.' && iname[1]=='.' && iname[2]=='/')
            {
                if (ipos->prev)
                    ipos = ipos->prev;
                iname += 3;
            }
            i = ipos->p;

			if (stricmp(iname,"type")==0)
			{
				if (!skip)
				{
					if ((!i->parent || relpath))
						syntax(error);
					strcpy(name,i->parent->value);
					if (upper)
						upr(name);
				}
			}
			else
			{
				item* i2;
				item* j;
				int usename=0;
				if (stricmp(iname,"name")==0)
				{
					j = i;
					usename = 1;
				}
				else
				{
					item *ii=i;
					if (getused && ipos->prev)
					{
						size_t k;
						ii = ipos->prev->p;
						j = item_find(ii,"use");
						for (k=0;k<item_childcount(j);++k)
						{
							if (strcmpi(j->child[k]->value,i->value)==0)
							{
								ii = j->child[k];
								break;
							}
						}
					}

                    j = item_find(ii,iname);
					for (i2=ii;!skip && !j && i2->parent;)
					{
						// search in parents
						i2=i2->parent;
						j = item_find(i2,iname);
					}

					if (!skip && !j)
						j = item_find(getconfig(i),iname);
				}

                if (count)
                {
                    sprintf(name,"%d",j?(int)item_childcount(j):0);
                }
                else
				if (s[0]==':')
				{
                    int checkquote;
                    int deep;
					++s;
					maskpos=0;
					for (n=0;*s && *s!='=';++s)
						if (*s=='%')
							maskpos=n;
						else
							mask[n++]=*s;
					mask[n]=0;
					maskend=n-maskpos;
					if (*s != '=')
						syntax(error);
					++s;
					outpos=0;
                    deep=0;
					for (n=0;*s;++s)
                    {
                        if (*s==')' && --deep<0)
                            break;
                        if (*s=='(')
                            ++deep;
                        if (*s=='%')
                        {
                            if (s[1]=='(')
                            {
                                for (;*s!=')';++s)
                                    out[n++]=*s;
                                out[n++]=*s;
                            }
                            else
							    outpos=n;
                        }
						else
							out[n++]=*s;
                    }
					out[n]=0;

                    checkquote = outpos>0 && out[outpos-1]=='\'' && out[outpos]=='\'';
                    if (checkquote)
                    {
                        out[outpos-1]='"';
                        out[outpos]='"';
                    }

					if (*s != ')')
						syntax(error);

					name[0]=0;
					if (j)
					{
						for (child=j->child;usename || child!=j->childend;++child)
						{
                            item *Item = usename?j:(*child);
                            int value_flags = Item->flags;
                            relpath = 1;

                            if (only_abspath || only_relpath)
                            {
                                int isabs = (Item->flags & FLAG_PATH_SET_ABSOLUTE) == FLAG_PATH_SET_ABSOLUTE;
                                if (isabs && !only_abspath)
                                    continue;
                                if (!isabs && !only_relpath)
                                    continue;
                            }

                            strcpy(value,Item->value);

                            // force a relative path by default
                            if ((value_flags & FLAG_PATH_MASK) != FLAG_PATH_NOT_PATH)
                            {
                                relpath = strip_path_abs(value, value_flags, error->project_root, error->src_root, error->coremake_root);
                                if (relpath)
                                    value_flags &= ~FLAG_PATH_SET_ABSOLUTE;
                            }

                            if (in_generated==1)
                            {
                                value_flags &= ~FLAG_PATH_MASK;
                                value_flags |= FLAG_PATH_GENERATED;
                            }
                            else if (in_generated)
                            {
                                value_flags &= ~FLAG_PATH_MASK;
                                value_flags |= FLAG_PATH_SYSTEM;
                            }

                            if (relpath)
								getrelpath(value,value_flags,buildpath[curr_build],buildflags[curr_build],delend,levelup, error->project_root, error->src_root, error->coremake_root);
                            if (abspath)
                                getabspath(value,value_flags,buildpath[curr_build],buildflags[curr_build], error->project_root, error->src_root, error->coremake_root);
							if (filepath) truncfilepath(value,delend);
							if (fileupper) truncfileupper(value);
							if (filename) truncfilename(value);
							if (filefourcc) truncfilefourcc(value);
							if (fileext) truncfileext(value);
                            if (addend) addendpath(value);
                            if (delhex) trunchex(value);
							if (fourcc) truncfourcc(value);
                            if (deltrail) delendpath(value);
							if (dos)
								pathdos(value);
							if (upper)
								upr(value);
							if (reverse) reversestr(value);
                            if (nodrive)
                                removedrive(value);
                            if (escape==1)
                                escapestr(value);
                            else if (escape)
                                escapepath(value,escape);
                            if (quote) urlquote(value,no_backslash);
							n = strlen(value);
							if (n>strlen(mask) && memcmp(value,mask,maskpos)==0 &&
								strcmp(value+n-maskend,mask+maskpos)==0)
							{
                                int skipquote=0;
                                char* name0 = name+strlen(name);
								if (name[0])
									strcat(name," ");
                                if (checkquote)
                                {
                                    char* i=value+maskpos;
                                    char* ie=value+n-maskend;
                                    skipquote=1;
                                    for (;i!=ie;++i)
                                    {
                                        char ch=*i;
                                        if (!isalpha(ch) && !isdigit(ch) && ch!='_' && ch!='-' && ch!='.' && ch!='/')
                                        {
                                            skipquote=0;
                                            break;
                                        }
                                    }
                                }
								strins(name+strlen(name),out,out+outpos-skipquote);
								strins(name+strlen(name),value+maskpos,value+n-maskend);
								strins(name+strlen(name),out+outpos+skipquote,out+strlen(out));

                                if (findfile && !skip)
                                {
                                    char path[MAX_PATH];
                                    struct stat s;
                                    char* filename = name0;
                                    while (isspace(*filename)) ++filename;
                                    strcpy(path,filename);
                                    tokeneval(path,0,pos,error,extra_cmd);
                                    pathunix(path);
                                    getabspath(path,FLAG_PATH_SOURCE,buildpath[curr_build],buildflags[curr_build], error->project_root, error->src_root, error->coremake_root);
                                    if (stat(path,&s)!=0)
                                        name0[0]=0;
                                }
							}
							if (usename)
								break;
						}
					}
				}
				else
				{
                    int name_flags = 0;
                    int is_relpath = 1;
					name[0]=0;
					if (usename)
                    {
						strcpy(name,j->value);
                        name_flags = j->flags;
                    }
					else
					if (j)
					{
                        name_flags |= j->flags;
						for (child=j->child;child!=j->childend;++child)
						{
                            if (findfile && !skip)
                            {
                                struct stat s;
                                char path[MAX_PATH];
                                strcpy(path,(*child)->value);
                                tokeneval(path,0,pos,error,extra_cmd);
                                pathunix(path);
                                getabspath(path,FLAG_PATH_SOURCE,buildpath[curr_build],buildflags[curr_build], error->project_root, error->src_root, error->coremake_root);
                                if (stat(path,&s)!=0)
                                    continue;
                            }
							if (name[0])
								strcat(name," ");
							strcat(name,(*child)->value);
                            name_flags |= (*child)->flags;
						}
					}

                    // force a relative path by default
                    if (!filename && (name_flags & FLAG_PATH_MASK) != FLAG_PATH_NOT_PATH)
                    {
                        if (!escape && (!has_forced_generated || (name_flags & FLAG_PATH_MASK)==FLAG_PATH_SYSTEM))
                        {
                            result &= ~FLAG_PATH_MASK;
                            result |= name_flags & FLAG_PATH_MASK;
                        }
                        if (skip || !abspath)
                        {
                            is_relpath = strip_path_abs(name, name_flags, error->project_root, error->src_root, error->coremake_root);
                            if (is_relpath)
                                name_flags &= ~FLAG_PATH_SET_ABSOLUTE;
                        }
                        else
                            is_relpath = 0;
                    }

                    if (in_generated==1)
                    {
                        name_flags &= ~FLAG_PATH_MASK;
                        name_flags |= FLAG_PATH_GENERATED;
                    }
                    else if (in_generated)
                    {
                        name_flags &= ~FLAG_PATH_MASK;
                        name_flags |= FLAG_PATH_SYSTEM;
                    }

                    if (is_relpath && relpath)
						getrelpath(name,name_flags,buildpath[curr_build],buildflags[curr_build],delend,levelup, error->project_root, error->src_root, error->coremake_root);
                    else if (!is_relpath && relpath)
                        getrelpath(name, name_flags, buildpath[curr_build], buildflags[curr_build], delend, levelup, error->project_root, error->src_root, error->coremake_root);
                    if (!skip && abspath && is_relpath)
                        getabspath(name,name_flags,buildpath[curr_build],buildflags[curr_build], error->project_root, error->src_root, error->coremake_root);
					if (filepath) truncfilepath(name,delend);
					if (fileupper) truncfileupper(name);
					if (filename) truncfilename(name);
    				if (filefourcc) truncfilefourcc(name);
					if (fileext) truncfileext(name);
                    if (addend) addendpath(name);
                    if (delhex) trunchex(name);
					if (fourcc) truncfourcc(name);
                    if (deltrail) delendpath(name);
					if (dos)
						pathdos(name);
					if (upper)
						upr(name);
					if (reverse) reversestr(name);
                    if (nodrive)
                        removedrive(name);
                    if (escape==1)
                        escapestr(name);
                    else if (escape)
                        escapepath(name,escape);
                    if (quote) urlquote(name,no_backslash);
                }
			}

			for (;*s && isspace(*s);++s);
			if (*s != ')')
				syntax(error);
			++s;

			s = strdel(s0,s);
			s = strins(s,name,NULL);
			--s;
		}
        if ((extra_cmd & CMD_AUTOMAKE) && s[0] == '@')
        {
            item* i = pos->p, *ii = i, *j, *i2;

            /* parse the name */
            s += 1;
            s = getname(s, name);
            /* translate automake version numbers to coremake version numbers */
            if (strcmp(name, "VERSION_MAJOR") == 0)
                strcpy(name, "PACKAGE_VERSION_MAJOR");
            else if (strcmp(name, "VERSION_MINOR") == 0)
                strcpy(name, "PACKAGE_VERSION_MINOR");
            else if (strcmp(name, "VERSION_REVISION") == 0)
                strcpy(name, "PACKAGE_VERSION_REVISION");
            else if (strcmp(name, "VERSION_EXTRA") == 0)
                strcpy(name, "PACKAGE_VERSION_EXTRA");

            j = item_find(ii, name);
            for (i2 = ii; !skip && !j && i2->parent;)
            {
                // search in parents
                i2 = i2->parent;
                j = item_find(i2, name);
            }
            if (!j)
                j = item_find(getconfig(pos->p), name);

            if (!j)
                printf("unknown automake field %s\r\n", name);

            if (j)
                strcpy(name, (*j->child)->value);
            unstring(name);

            if (*s != '@')
                syntax(error);
            ++s;

            s = strdel(s0, s);
            s = strins(s, name, NULL);
            --s;
        }
	}
    return result;
}

int reader_tokeneval(reader* p,int skip,int onespace,build_pos* pos,int extra_cmd)
{
	reader_tokenline(p,onespace);
    return tokeneval(p->token,skip,pos,p,extra_cmd);
}

int str2bool(const char* s)
{
	return s[0] && strcmp(s,"0")!=0;
}

char* bool2str(int v)
{
	return strdup(v?"1":"0");
}

void evalspace(const char** s)
{
	for (;(*s)[0] && isspace((*s)[0]);++(*s));
}

char* eval0(const char** s,item* config,reader* file);
char* eval3(const char** s,item* config,reader* file)
{
	int n;
	char* a;
	const char* s0;
	evalspace(s);
	if ((*s)[0] == '(')
	{
		char* a;
		*s += 1;
		a = eval0(s,config,file);

		evalspace(s);
		if ((*s)[0] != ')')
			syntax(file);
		*s += 1;
		return a;
	}
	else
	if ((*s)[0] == '"')
	{
		*s += 1;
		s0 = *s;
		for (n=0;(*s)[0] && (*s)[0]!='"';(*s)++,++n);
		if ((*s)[0]!='"')
			syntax(file);
		*s += 1;
		a = (char*)zalloc(n+1);
		memcpy(a,s0,n);
	}
	else
	if (isname((*s)[0]))
	{
		s0 = *s;
		for (n=0;(*s)[0] && isname((*s)[0]);(*s)++,++n);
		a = (char*)zalloc(n+1);
		memcpy(a,s0,n);

		if (stricmp(a,"defined")==0 && (*s)[0]=='(')
		{
			item* i;
			free(a);
			*s += 1;

			s0 = *s;
			for (n=0;(*s)[0] && (*s)[0]!=')';(*s)++,++n);
			a = (char*)zalloc(n+1);
			memcpy(a,s0,n);

			i = item_find(config,a);
			free(a);

			if ((*s)[0] != ')')
				syntax(file);
			*s += 1;

			a = bool2str(i && (i->flags & FLAG_DEFINED));
		}
	}
	else
		a = strdup("");

	return a;
}

char* eval_not_equal(const char** s,item* config,reader* file)
{
	evalspace(s);
	if ((*s)[0] == '!' && (*s)[1] != '=')
	{
		int value;
		char* a;
		*s += 1;
		a = eval_not_equal(s,config,file);
		value = !str2bool(a);
		free(a);
		return bool2str(value);
	}
	else
		return eval3(s,config,file);
}

char* eval1(const char** s,item* config,reader* file)
{
	int value;
	char* b;
	char* a = eval_not_equal(s,config,file);

	evalspace(s);
	if ((*s)[0] == '=' && (*s)[1] == '=')
	{
		*s += 2;
		b = eval1(s,config,file);
		value = stricmp(a,b)==0;
		free(a);
		free(b);
		a = bool2str(value);
	}
	else
	if ((*s)[0] == '!' && (*s)[1] == '=')
	{
		*s += 2;
		b = eval1(s,config,file);
		value = stricmp(a,b)!=0;
		free(a);
		free(b);
		a = bool2str(value);
	}
	return a;
}

char* eval0(const char** s,item* config,reader* file)
{
	int value;
	char* b;
	char* a = eval1(s,config,file);

	evalspace(s);
	if ((*s)[0] == '&' && (*s)[1] == '&')
	{
		*s += 2;
		b = eval0(s,config,file);
		value = str2bool(a) && str2bool(b);
		free(a);
		free(b);
		a = bool2str(value);
	}
	else
	if ((*s)[0] == '|' && (*s)[1] == '|')
	{
		*s += 2;
		b = eval0(s,config,file);
		value = str2bool(a) || str2bool(b);
		free(a);
		free(b);
		a = bool2str(value);
	}
	return a;
}

int eval(const char* s,item* config,reader* file)
{
	int result;
	char* a = eval0(&s,config,file);
	result = str2bool(a);
	free(a);
	return result;
}

int same_ch(int a, int b)
{
#ifdef _WIN32
    return toupper(a) == toupper(b);
#else
    return a==b;
#endif
}

int match(const char* name, const char* mask)
{
    if (*mask=='*')
    {
        ++mask;
        for (;!match(name,mask);++name)
        {
            if (*name==0)
                return 0;
        }
        return 1;
    }
    else
    {
        if (*mask==0)
            return *name==0;

        if (*name==0)
            return 0;

        return (mask[0]=='?' || same_ch(name[0],mask[0])) && match(name+1,mask+1);
    }
}

void remove_file(const char* name)
{
	if (strchr(name,'*') == NULL && strchr(name,'?') == NULL)
		remove(name);
	else
	{
		DIR* dir;
        const char* mask = getfilename(name);
        char path[MAX_PATH];
		strncpy(path,name,MAX_PATH);
		truncfilepath(path,1);
		dir = opendir(path[0]?path:".");
		if (dir)
		{
			struct dirent* entry;
			addendpath(path);
			while ((entry = readdir(dir)) != NULL)
			{
				if (entry->d_name[0]!='.' && match(entry->d_name,mask))
				{
                    char filepath[MAX_PATH];
                    strcpy(filepath,path);
                    strcat(filepath,entry->d_name);
					remove(filepath);
				}
			}
			closedir(dir);
		}
	}
}

void remove_dir(const char* name)
{
	DIR* dir = opendir(name);
	if (dir)
	{
		struct dirent* entry;
		while ((entry = readdir(dir)) != NULL)
		{
			if (entry->d_name[0]!='.')
			{
				char item[MAX_PATH];
				sprintf(item,"%s/%s",name,entry->d_name);
				remove_dir(item);
			}
		}
		closedir(dir);
        remove_dir(".DS_Store");
        remove_dir(".Trashes");
		rmdir(name);
	}
	else
		remove(name);
}

item* reader_item(reader* file, int skip, build_pos* pos)
{
    item* i = NULL;

	reader_name(file);
    if (!skip)
    {
        const char* token = file->token;
        build_pos* parent = pos;
        while (token[0]=='.' && token[1]=='.' && token[2]=='/')
        {
            if (parent)
                parent = parent->prev;
            token += 3;
        }
        if (parent)
    		i = item_find_add(parent->p,token,1);
    }

    return i;
}

static void create_missing_dirs(const char *path)
{
	size_t strpos = 0, strpos_i;
	char new_dir[MAX_PATH];
	char new_path[MAX_PATH];
	int ret;
	struct stat dir_stats;

    assert(ispathabs(path)); // internally all pathes should be absolute

    strcpy(new_path,path);
    pathunix(new_path);
    simplifypath(new_path,1);

	for (;;)
	{
		strpos_i = strcspn(&new_path[strpos], "/");
		if (strpos_i == strlen(&new_path[strpos]))
			break;
		strpos += strpos_i+1;

		strncpy(new_dir, new_path, strpos-1);
        new_dir[strpos-1] = '\0';

		if (stat(new_dir, &dir_stats) == 0)
		{
			if ((dir_stats.st_mode & S_IFDIR) != S_IFDIR)
			{
				printf("%s is not a directory\r\n",new_dir);
				exit(1);
			}
		}
        else if (strpos>1 && new_dir[strpos-2] != ':')
		{
			ret = make_dir(new_dir);

			if (ret != 0 && ret != EEXIST)
			{
				printf("can't create directory %s\r\n",new_dir);
				exit(1);
			}
		}
	}
}

void getarg(char* s, const char** in)
{
    int quote = 0;

	while (**in && isspace(**in))
        ++(*in);

	for (;**in && (quote || !isspace(**in));++(*in))
    {
        if (**in == '"')
        {
            quote = !quote;
            continue;
        }
        *(s++) = **in;
    }

    *s = 0;
}

static void compile_file(item* p, const char *src, const char *dst, int flags, build_pos *pos, int automake, const char *pjr_root, const char *src_root, const char *coremake_root)
{
    char tmpstr[MAX_LINE];
    reader r;

    reader_init(&r);
    getarg(r.filename,&src);
    pathunix(r.filename);
	strcpy(r.project_root, pjr_root);
	strcpy(r.src_root, src_root);
	strcpy(r.coremake_root, coremake_root);
	getabspath(r.filename,flags,"",FLAG_PATH_SOURCE, pjr_root, src_root, coremake_root);
    r.r.f = fopen(r.filename,"r");
    r.r.flags = flags;
    r.pos = r.line;
    r.r.filename_kind = FLAG_PATH_SOURCE;

    // TODO: in verbose mode we should issue a warning if the file is not found

    if (r.r.f)
    {
        item *src;
        char backup[MAX_PATH];
        FILE* backupfile = file_built;
        int backupflags = buildflags[curr_build];
        strcpy(backup,buildpath[curr_build]);
        strcpy(buildpath[curr_build], dst);
        getabspath(buildpath[curr_build],FLAG_PATH_GENERATED|(ispathabs(buildpath[curr_build])?FLAG_PATH_SET_ABSOLUTE:0),"",FLAG_PATH_GENERATED, r.project_root, r.src_root, r.coremake_root);
        simplifypath(buildpath[curr_build],0);
        buildflags[curr_build] = FLAG_PATH_GENERATED;

        // check that the src and dst files are different
        if (stricmp(r.filename,buildpath[curr_build])==0)
        {
            char *ext = strrchr(buildpath[curr_build],'.');
            if (!ext)
                strcat(buildpath[curr_build],".compiled");
            else
                strins(ext,ext,NULL);
            printf("*warning: COMPILE '%s' already exists, using '%s'\r\n",r.filename,buildpath[curr_build]);
        }

        create_missing_dirs(buildpath[curr_build]);
        file_built = fopen(buildpath[curr_build],"w+");

        if (file_built)
        {
            while (reader_line(&r))
            {
                const char* eval = r.line;
                evalspace(&eval);
                if (!automake && stricmp(eval, "%%BEGIN") == 0)
                {
                    reader_line(&r);
                    build_parse(p,&r,16,0,pos, 0);
                }
                else
                {
                    reader_tokeneval(&r, 0, -1, pos, automake ? CMD_AUTOMAKE : CMD_COREMAKE);
                    fputs(r.token,file_built);
                    fputc(10,file_built);
                }
            }

            fclose(file_built);
        }

        fclose(r.r.f);

        tmpstr[0] = 0;
        strins(tmpstr, r.filename, getfilename(r.filename));
        item_delete(item_find(pos->p, "base"));
        src = item_find_add(item_find_add(pos->p, "base", 0), tmpstr, 1);
        set_path_type(src, FLAG_PATH_SOURCE);

        file_built = backupfile;
        strcpy(buildpath[curr_build],backup);
        buildflags[curr_build] = backupflags;
    }
    reader_free(&r);
}

static void build_file(item* p,const char* filename, int filename_kind, const char *pjr_root, const char *src_root, const char *coremake_root, enum build_step prebuild);
static int build_parse(item* p,reader* file,int sub,int skip,build_pos* pos0, enum build_step prebuild)
{
	int bin;
	int result=0;
	item* i;
    build_pos pos;
    int is_sharped;
    char tmpstr[MAX_LINE];

    if (pos0 && pos0->p == p)
        pos.prev = pos0->prev;
    else
        pos.prev = pos0;
    pos.p = p;

	while (!reader_eof(file))
	{
        is_sharped =0;
		if (reader_istoken_n(file,"#",1))
        {
            is_sharped = 1;
            reader_token_skip(file,1);
        }

		if (is_sharped && reader_istoken(file,"include"))
		{
            reader_token(file);
			reader_filename(file,FLAG_PATH_COREMAKE);
            if (!skip && !(file->r.flags & FLAG_NO_INCLUDE))
                build_file(p, file->token, FLAG_PATH_COREMAKE, file->project_root, file->src_root, file->coremake_root, prebuild);
		}
		else
		if (is_sharped && reader_istoken(file,"undef"))
		{
			reader_name(file);
			if (!skip)
			{
				i=item_find(getconfig(p),file->token);
				if (i) i->flags &= ~FLAG_DEFINED;
			}
		}
		else
		if (is_sharped && reader_istoken(file,"define"))
		{
			reader_name(file);
			i = skip?NULL:item_find_add(getconfig(p),file->token,1);
            if (!skip && strnicmp(file->token,"CONFIG_CUSTOMER_",16)==0)
            {
                char *lower = file->token+16;
                i = item_find_add(getconfig(p),"CONFIG_CUSTOMER",1);
                lwr(lower);
                item_find_add(i,lower,1);
            }
            else
			if (reader_tokenline(file,0) && !skip)
			{
				char* s = file->token;
				size_t len = strlen(s);

				if (len>=2 && s[0]=='\'' && s[len-1]=='\'')
				{
					strdel(s+len-1,s+len);
					strdel(s,s+1);
				}
				if (len>=5 && s[0]=='T' && s[1]=='(' && s[2]=='"' &&
					s[len-1]==')' && s[len-2]=='"')
				{
					strdel(s+len-2,s+len);
					strdel(s,s+3);
				}
				i = item_find_add(i,file->token,1);
                if (stricmp(i->parent->value,"CONFIG_ANDROID_NDK")==0)
                {
                    pathunix(i->value);
                    set_path_type(i,FLAG_PATH_SYSTEM);
                }
			}
		}
		else
		if (!is_sharped && reader_istoken(file,"config"))
		{
            if (prebuild == PREBUILD_PRE_CONFIG)
            {
                if (!skip)
                {
                    const item* root = item_root(p, 0);
                    item* config = item_find(root, "config_file");
                    if (config && getvalue(config))
                    {
                        item* no_include = item_find(*config->child, "no_include");
                        set_path_type(config, FLAG_PATH_GENERATED);
                        build_file(p, getvalue(config)->value, FLAG_PATH_GENERATED | (no_include ? FLAG_NO_INCLUDE : 0), file->project_root, file->src_root, file->coremake_root, prebuild);
                    }
                    config = item_find(getconfig(p), "COREMAKE_CONFIG_HELPER");
                    if (config && config->flags & FLAG_DEFINED)
                    {
                        strcpy(tmpstr, file->coremake_root);
                        strcat(tmpstr, "/config_helper.h");
                        build_file(p, tmpstr, FLAG_PATH_COREMAKE, file->project_root, file->src_root, file->coremake_root, prebuild);
                    }
                    config = item_find(root, "config_cleaner");
                    if (config && getvalue(config))
                    {
                        set_path_type(config, FLAG_PATH_SOURCE);
                        build_file(p, getvalue(config)->value, FLAG_PATH_SOURCE, file->project_root, file->src_root, file->coremake_root, prebuild);
                    }
                    //preprocess(p, file->project_root, file->src_root, file->coremake_root);
                }
                return result;
            }
            if (prebuild != PREBUILD_BEFORE_CONFIG)
                printf("More than one CONFIG %s:%d\r\n", file->line, file->r.no);
            prebuild = PREBUILD_AFTER_CONFIG; /* allow all prebuild stuff */
		}
		else
		if (!is_sharped && reader_istoken(file,"while"))
		{
			int v;
            reader whilepos;
    		reader_save(file,&whilepos);
			do
			{
				reader_restore(file,&whilepos);
				reader_tokeneval(file,skip,0,&pos,0);
				v = eval(file->token,getconfig(p),file);
				build_parse(p,file,21,!v || skip,&pos, prebuild);
                if (p->flags & FLAG_REMOVED) skip = 1;
			}
			while (v && !skip);
            reader_free(&whilepos);
        }
		else
		if (reader_istoken(file,"if"))
		{
            int done=0;
			int m;
			do
			{
				int v;
				reader_tokeneval(file,skip,0,&pos,0);
				v = eval(file->token,getconfig(p),file);
				m = build_parse(p,file,2,!v || skip || done,&pos, prebuild);
                if (p->flags & FLAG_REMOVED) skip = 1;
				if (m==1)
                {
					build_parse(p,file,3,v || skip || done,&pos, prebuild);
                    if (p->flags & FLAG_REMOVED) skip = 1;
                }
                else if (v)
                    done = 1;
			}
			while (m==2);
		}
		else
		if (reader_istoken(file,"ifdef"))
		{
            int done=0;
			int m;
			do
			{
				int v;
				reader_tokeneval(file,skip,0,&pos,0);
				i = item_find(getconfig(p),file->token);
				v = i && (i->flags & FLAG_DEFINED);
				m = build_parse(p,file,2,!v || skip || done,&pos, prebuild);
                if (p->flags & FLAG_REMOVED) skip = 1;
				if (m==1)
                {
					build_parse(p,file,3,v || skip || done,&pos, prebuild);
                    if (p->flags & FLAG_REMOVED) skip = 1;
                }
                else if (v)
                    done = 1;
			}
			while (m==2);
		}
		else
		if (reader_istoken(file,"ifndef"))
		{
            int done=0;
			int m;
			do
			{
				int v;
				reader_tokeneval(file,skip,0,&pos,0);
				i = item_find(getconfig(p),file->token);
				v = !i || !(i->flags & FLAG_DEFINED);
				m = build_parse(p,file,2,!v || skip || done,&pos, prebuild);
                if (p->flags & FLAG_REMOVED) skip = 1;
				if (m==1)
                {
					build_parse(p,file,3,v || skip || done,&pos, prebuild);
                    if (p->flags & FLAG_REMOVED) skip = 1;
                }
                else if (v)
                    done = 1;
			}
			while (m==2);
		}
		else
		if (!is_sharped && reader_istoken(file,"%%END"))
		{
			if (sub!=16)
				syntax(file);
			break;
		}
		else
		if (reader_istoken(file,"else"))
		{
			if (sub!=2)
				syntax(file);
			result = 1;
			break;
		}
		else
		if (reader_istoken(file,"elif"))
		{
			if (sub!=2)
				syntax(file);
			result = 2;
			break;
		}
		else
		if (reader_istoken(file,"endif"))
		{
			if (sub!=2 && sub!=3)
				syntax(file);
			break;
		}
		else
		if (!is_sharped && reader_istoken(file,"endadd"))
		{
			if (sub!=9)
				syntax(file);
			break;
		}
		else
		if (!is_sharped && reader_istoken(file,"endwhile"))
		{
			if (sub!=21)
				syntax(file);
			break;
		}
		else
		if (!is_sharped && reader_istoken(file,"print"))
		{
			reader_tokeneval(file,skip,1,&pos,0);
			if (!skip)
			{
				if (!file_built)
				{
					printf("not file opened!\r\n");
					exit(1);
				}
				fprintf(file_built,"%s",file->token);
			}
		}
		else
		if (!is_sharped && reader_istoken(file,"echo"))
		{
			reader_tokeneval(file,skip,1,&pos,0);
			if (!skip)
				printf("%s\r\n",file->token);
        }
		else
		if (!is_sharped && reader_istoken(file,"printnl"))
		{
			reader_tokeneval(file,skip,1,&pos,0);
			if (!skip)
			{
				if (!file_built)
				{
					printf("no file opened!\r\n");
					exit(1);
				}
				fprintf(file_built,"%s\n",file->token);
			}
		}
		else
		if (!is_sharped && reader_istoken(file,"printnldos"))
		{
			reader_tokeneval(file,skip,1,&pos,0);
			if (!skip)
			{
				if (!file_built)
				{
					printf("no file opened!\r\n");
					exit(1);
				}
#ifdef _WIN32
				fprintf(file_built,"%s\n",file->token);
#else
				fprintf(file_built,"%s\r\n",file->token);
#endif
			}
		}
		else
		if (!is_sharped && reader_istoken(file,"compile"))
        {
			int flags = reader_tokeneval(file,skip,1,&pos,0);
			if (!skip)
			{
                char src[MAX_LINE];
                const char* s = file->token;
                getarg(src, &s);
                getarg(tmpstr, &s);
                compile_file(p, src, tmpstr, flags, &pos, 0, file->project_root, file->src_root, file->coremake_root);
            }
        }
		else
		if (!is_sharped && reader_istoken(file,"inc"))
		{
            i = reader_item(file,skip,&pos);
            i = getvalue(i);
            if (!skip && i && i->value)
            {
                char v[16];
                sprintf(v,"%d",atoi(i->value)+1);
                free(i->value);
    			i->value = strdup(v);
            }
        }
		else
		if (!is_sharped && reader_istoken(file,"for"))
		{
            int reverse = reader_istoken(file,"reverse");
			if (reader_istoken(file,"each"))
			{
				char* s;
				reader forpos;
				int first=1;

				reader_tokeneval(file,skip,0,&pos,0);

				reader_save(file,&forpos);

				s = forpos.token;
				for (;;)
				{
                    build_pos* ipos = &pos;
                    const char* iname = tmpstr;

					s = getname(s,tmpstr);

                    while (iname[0]=='.' && iname[1]=='.' && iname[2]=='/')
                    {
                        if (ipos->prev)
                            ipos = ipos->prev;
                        iname += 3;
                    }

					i = item_find(ipos->p,iname);
                    if (i && !skip)
                    {
        				item** child;
        				item** childend;

                        if (reverse)
                        {
                            child = i->childend-1;
                            childend = i->child-1;
                        }
                        else
                        {
                            child = i->child;
                            childend = i->childend;
                        }

						while (child!=childend)
						{
                            item* w = findref(*child);
							if (!w) w = *child;
							if (!first)
								reader_restore(file,&forpos);
                            setvalue(item_find_add(w,"for_last",0),(child+(reverse?-1:1)==childend)?"1":"0");

							build_parse(w,file,1,0,&pos, prebuild);
                            if (w->flags & FLAG_REMOVED)
                            {
                                item_delete(w);
                                if (!reverse)
                                {
                                    --child;
                                    childend = i->childend;
                                }
                            }

                            if (reverse)
                                --child;
                            else
                                ++child;

							first = 0;
						}
                    }

					for (;*s && isspace(*s);++s);
					if (*s==0)
						break;
					if (*s!=',')
						syntax(&forpos);
					++s;
				}
                reader_free(&forpos);

				if (first)
					build_parse(p,file,1,1,&pos, prebuild);
			}
			else
				syntax(file);
		}
		else
		if (!is_sharped && reader_istoken(file,"endfor"))
		{
			if (sub!=1)
				syntax(file);
			break;
		}
		else
		if (!is_sharped && reader_istoken(file,"remove"))
		{
			reader_tokeneval(file,skip,0,&pos,0);
			if (!skip)
				remove_file(file->token);
		}
		else
		if (!is_sharped && reader_istoken(file,"delete"))
		{
			if (!skip)
            {
                p->flags |= FLAG_REMOVED;
                skip = 1;
            }
		}
		else
		if (!is_sharped && reader_istoken(file,"removedir"))
		{
			reader_tokeneval(file,skip,0,&pos,0);
			if (!skip)
				remove_dir(file->token);
		}
		else
        if (!is_sharped && reader_istoken(file,"script"))
        {
            chmod(buildpath[curr_build],S_IEXEC|S_IREAD|S_IWRITE);
        }
        else
		if (!is_sharped && reader_istoken(file,"fileabs"))
        {
            if (!skip)
                buildflags[curr_build] |= FLAG_PATH_SET_ABSOLUTE;
        }
        else
		if (!is_sharped && ((bin=reader_istoken(file,"filebin"))!=0 || reader_istoken(file,"file")))
		{
            if (!skip)
            {
				if (file_built)
				{
					fclose(file_built);
					file_finalize(buildpath[curr_build]);
				}
            }

			buildflags[curr_build] = reader_tokeneval(file,skip,0,&pos,0);

			if (!skip)
			{
				strcat(file->token,".tmp");
				strcpy(buildpath[curr_build],file->token);
                simplifypath(buildpath[curr_build],0);
                getabspath(buildpath[curr_build],buildflags[curr_build],"",buildflags[curr_build], file->project_root, file->src_root, file->coremake_root);
				create_missing_dirs(buildpath[curr_build]);
				file_built = fopen(buildpath[curr_build],bin?"wb":"w");
				if (!file_built)
				{
					printf("can't create %s\r\n",file->token);
					exit(1);
				}
			}
		}
		else
		if (!is_sharped && reader_istoken(file,"add"))
		{
            int flags;
            i = reader_item(file,skip,&pos);

			flags = reader_tokeneval(file,skip,0,&pos,0);
			if (i)
            {
                item *j = item_find_add(i,file->token,1);
                set_path_type(j,flags & FLAG_PATH_MASK);
                build_parse(j,file,9,skip,&pos, prebuild);
            }
            else
                build_parse(p,file,9,1,&pos, prebuild);
		}
		else
		if (!is_sharped && reader_istoken(file,"dirpush"))
		{
            int flags = reader_tokeneval(file,skip,0,&pos,0);
            if (!skip)
            {
                if (++curr_build > MAX_PUSHED_PATH)
                {
					printf("can't push directory %s, limit reached\r\n",file->token);
					exit(1);
                }
                getabspath(file->token,flags,buildpath[curr_build-1],buildflags[curr_build-1], file->project_root, file->src_root, file->coremake_root);
                addendpath(file->token);
                strcpy(buildpath[curr_build],file->token);
                buildflags[curr_build] = flags;
            }
        }
		else
		if (!is_sharped && reader_istoken(file,"dirpop"))
		{
            if (!skip)
            {
                if (--curr_build < 0)
                {
				    printf("can't pop directory limit reached\r\n");
				    exit(1);
                }
            }
        }
        else
		if (!is_sharped && reader_istoken(file,"mkdir"))
		{
			int flags = reader_tokeneval(file,skip,0,&pos,0);
			if (!skip)
            {
                getabspath(file->token,flags,"",FLAG_PATH_GENERATED, file->project_root, file->src_root, file->coremake_root);
				create_missing_dirs(file->token);
            }
		}
        else
		if (!is_sharped && reader_istoken(file,"copy"))
		{
			int flags = reader_tokeneval(file,skip,0,&pos,0);
			if (!skip)
            {
                FILE *src,*dst;
                const char* s = file->token;
                getarg(tmpstr,&s);
	            pathunix(tmpstr);
                //getabspath(tmpstr,flags,"",buildflags[curr_build]);
                src = fopen(tmpstr,"rb");
                if (!src)
                    printf("can't open file %s for copy reading\r\n",tmpstr);
                else
                {
                    getarg(tmpstr,&s);
	                pathunix(tmpstr);
                    getabspath(tmpstr,flags,"",buildflags[curr_build], file->project_root, file->src_root, file->coremake_root);
                    create_missing_dirs(tmpstr);
                    dst = fopen(tmpstr, "wb");
                    if (!dst)
                        printf("can't open file %s for copy writing\r\n",tmpstr);
                    else
                    {
                        size_t read_size;
                        read_size = fread(tmpstr,1,sizeof(tmpstr),src);
                        while (read_size)
                        {
                            if (fwrite(tmpstr,1,read_size,dst)!=read_size)
                            {
                                printf("error copying !");
                                exit(1);
                            }
                            if (feof(src))
                                break;
                            read_size = fread(tmpstr,1,sizeof(tmpstr),src);
                        }
                        fclose(dst);
                    }
                    fclose(src);
                }
            }
		}
        else
        if (!is_sharped)
		{
            i = reader_item(file,skip,&pos);
			if (reader_istoken(file,"="))
			{
				int flags = reader_tokeneval(file,skip,0,&pos,0);
				while (i && i->child != i->childend)
					item_delete(i->child[0]);
				if (i)
                {
					item_find_add(i,file->token,1);
                    set_path_type(i,flags & FLAG_PATH_MASK);
                }
			}
			else
			if (reader_istoken(file,"+="))
			{
				int flags;
				flags = reader_tokeneval(file,skip,0,&pos,0);
				if (i)
                {
					item *j = item_find_add(i,file->token,1);
                    set_path_type(j,flags & FLAG_PATH_MASK);
                }
			}
			else
				syntax(file);
		}
		else
			syntax(file);
	}
	return result;
}

static void build_file(item* p,const char* filename, int filename_kind, const char *pjr_root, const char *src_root, const char *coremake_root, enum build_step prebuild)
{
	reader r;
    reader_init(&r);
	strcpy(r.filename,filename);
	pathunix(r.filename);
	strcpy(r.project_root, pjr_root);
	strcpy(r.src_root, src_root);
	strcpy(r.coremake_root, coremake_root);
	r.r.f = fopen(filename,"r");
	r.pos = r.line;
	if (!r.r.f)
	{
		printf("'%s' build file not found!\r\n",filename);
		exit(1);
	}
    r.r.flags = filename_kind;
    r.r.filename_kind = filename_kind;
	build_parse(p,&r,0,0,NULL, prebuild);
	fclose(r.r.f);
    reader_free(&r);
}

/* generates a workspace for the proj type */
static item* default_workspace(item* workspace,item* i,item* p, const char *proj)
{
	item** child;
	if (!p) return i;
	for (child=p->child;child!=p->childend;++child)
    {
        if (!i)
        {
            i = item_find_add(workspace,proj,1);
        }
        item_find_add(item_find_add(i,"use",1),(*child)->value,1);
    }
    return i;
}

int main(int argc, char** argv)
{
	int n;
	int dumppost = 0;
	int dump = 0;
	item* i;
	char path[MAX_PATH];
	char proj_root[MAX_PATH];
	char src_root[MAX_PATH] = "";
	char coremake_root[MAX_PATH] = "";
	char root_forced = 0;
	char platform[MAX_PATH] = "";
	char proj[MAX_PATH] = "root.proj";
	
    getcwd(proj_root,sizeof(proj_root));
    pathunix(proj_root);
    addendpath(proj_root);

	for (n=1;n<argc;++n)
	{
		if (argv[n][0]=='-')
		{
			switch (argv[n][1])
			{
			case 'v':
				verbose = 1;
				break;
			case 'h':
				usage();
				return 0;
			case 'd':
				dump = 1;
				break;
			case 'p':
				dumppost = 1;
				break;
			case 'f':
				strcpy(proj,argv[++n]);
				root_forced = 1;
				break;
            case 'r':
                strcpy(src_root,argv[++n]);
                pathunix(src_root);
                if (!ispathabs(src_root))
                {
                    strins(src_root,proj_root,NULL);
                    simplifypath(src_root,0);
                }
                addendpath(src_root);
                break;
			}
		}
		else
		if (platform[0]==0)
			strcpy(platform,argv[n]);
	}

	strcpy(path,src_root);
    addendpath(path);
	strcat(path,proj);

    if (!src_root[0])
        strcpy(src_root, proj_root);

	item *universe = item_find_add(NULL, UNIVERSE_NAME, 0);

	item *all_roots = item_find_add(universe, ROOT_NAME, 0);

	item* root = item_find_add(all_roots, proj_root, 0);
	item_find_add_in_root(root, "config");
	/* TODO use settle_root settle_root(root, src_root, proj_root, coremake_root);*/

    if (!load_file(root,path,NULL, proj_root, src_root, coremake_root) && !root_forced)
	{
        // try <directory>.proj
		strcpy(proj,src_root);
        delendpath(proj);
        truncfilename(proj);
		strcat(proj,".proj");
        strins(proj,src_root,NULL);
		if (!load_file(root,proj,NULL, proj_root, src_root, coremake_root))
        {
            // search for *.proj
			DIR* dir = opendir(src_root);
			if (dir)
			{
                int found = 0;
				struct dirent* entry;
				while ((entry = readdir(dir)) != NULL)
					if (entry->d_name[0]!='.' && strrchr(entry->d_name,'.') &&
                        stricmp(strrchr(entry->d_name,'.'),".proj")==0 && load_file(root,entry->d_name,NULL, proj_root, src_root, coremake_root))
                    {
                        found = 1;
                        strcpy(proj,entry->d_name);
                        pathunix(proj);
                        truncfilename(proj);
                        break;
                    }

				closedir(dir);
                if (!found)
                {
                    printf("No root .proj file found !");
                    exit(1);
                }
			}
        }
	}

    if (!getvalue(item_find(root,"workspace")))
    {
        // add a default workspace as none were defined
        item* w = item_find_add(root,"workspace",0);
        item* i = NULL;
		size_t target;
		truncfilename(proj);
		for (target = 0; all_targets[target].name; target++)
			if (all_targets[target].output_name)
				i=default_workspace(w,i,item_find(root, all_targets[target].name), proj);
    }

	preprocess_project(root);

	if (dump)
		dumpitem(root,1);

	if (!platform[0])
	{
		if (!dump)
			usage();
		return 1;
	}

	item_find_add(item_find_add(universe,"platformname",1),platform,1);

	sprintf(path,"%srelease/%s/",proj_root,platform);
    i = item_find_add(universe,"outputpath",1);
	i = item_find_add(i,path,1);
    set_path_type(i,FLAG_PATH_GENERATED);
    if (ispathabs(path))
        i->flags |= FLAG_PATH_SET_ABSOLUTE;

	sprintf(path,"%sbuild/%s/",proj_root,platform);
    i = item_find_add(universe,"buildpath",1);
	i = item_find_add(i,path,1);
    set_path_type(i,FLAG_PATH_GENERATED);
    if (ispathabs(path))
        i->flags |= FLAG_PATH_SET_ABSOLUTE;

	i = getvalue(item_find_add(root,"platform_files",0));
	if (i)
	{
		if (ispathabs(i->value) || !ispathabs(src_root))
			strcpy(coremake_root,i->value);
		else
		{
			strcpy(coremake_root,src_root);
			strcat(coremake_root,i->value);
		}
		simplifypath(coremake_root,1);
	}
	else
    {
#if defined(_WIN32) && !defined(__CYGWIN__)
        HMODULE this1 = GetModuleHandleA("coremake.exe");
        if (this1)
        {
            GetModuleFileNameA(this1,coremake_root,MAX_PATH);
            FreeLibrary(this1);
            pathunix(coremake_root);
            getfilename(coremake_root)[0] = 0;
            strcat(coremake_root,"coremake");
        }
#else
        /// \todo this shouldn't work on UNIX platforms
        ///       since the platform files should be in a dir like /usr/share
        ///       and the argv most likely doesn't contain the path to the exe
        ///       We probably need a --prefix during compilation to hardcode that /usr/share path
        strcpy(coremake_root,"/usr/local/share/coremake");
#endif
    }
	settle_root(root, src_root, proj_root, coremake_root);

	addendpath(coremake_root);

    strcpy(path,coremake_root);
	strcat(path,platform);
	strcat(path,".build");

	strcpy(buildpath[0],proj_root); //safety
    buildflags[0] = FLAG_PATH_GENERATED;
	file_built = NULL;

    /* pre-build files up to the CONFIG instruction */
	item** child_root;
	for (child_root = all_roots->child; child_root != all_roots->childend; ++child_root)
	{
		/* call this for each root in the universe */
		if (!item_is_root(*child_root))
			continue;
		item *proj_path     = item_find(*child_root, "builddir");
		item *src_path      = item_find(*child_root, "rootpath");
		item *coremake_path = item_find(*child_root, "platform_files");
		build_file(*child_root, path, FLAG_PATH_COREMAKE, getvalue(proj_path)->value, getvalue(src_path)->value, getvalue(coremake_path)->value, PREBUILD_PRE_CONFIG);
		if (file_built)
		{
			fclose(file_built);
			file_finalize(buildpath[curr_build]);
		}
	}

    for (child_root = all_roots->child; child_root != all_roots->childend; ++child_root)
    {
        /* call this for each root in the universe */
        if (!item_is_root(*child_root))
            continue;
        item *proj_path = item_find(*child_root, "builddir");
        item *src_path = item_find(*child_root, "rootpath");
        item *coremake_path = item_find(*child_root, "platform_files");
        preprocess_part1(*child_root, getvalue(proj_path)->value, getvalue(src_path)->value, getvalue(coremake_path)->value);
    }

    for (child_root = all_roots->child; child_root != all_roots->childend; ++child_root)
    {
        /* call this for each root in the universe */
        if (!item_is_root(*child_root))
            continue;
        item *proj_path = item_find(*child_root, "builddir");
        item *src_path = item_find(*child_root, "rootpath");
        item *coremake_path = item_find(*child_root, "platform_files");
        preprocess_part2(*child_root, getvalue(proj_path)->value, getvalue(src_path)->value, getvalue(coremake_path)->value);
    }

    /* build output files from the CONFIG instruction */
    for (child_root = all_roots->child; child_root != all_roots->childend; ++child_root)
    {
        /* call this for each root in the universe */
        if (!item_is_root(*child_root))
            continue;
        item *proj_path = item_find(*child_root, "builddir");
        item *src_path = item_find(*child_root, "rootpath");
        item *coremake_path = item_find(*child_root, "platform_files");
        build_file(*child_root, path, FLAG_PATH_COREMAKE, getvalue(proj_path)->value, getvalue(src_path)->value, getvalue(coremake_path)->value, PREBUILD_BEFORE_CONFIG);
        if (file_built)
        {
            fclose(file_built);
            file_finalize(buildpath[curr_build]);
        }
    }

	if (dumppost)
		dumpitem(universe,1);

	item_delete(universe);
	return 0;
}
