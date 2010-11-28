/*
 * $Id: ebmlbinary.c 1319 2008-09-19 16:08:57Z robux4 $
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

#ifndef _EBML2_EBML_MASTER_H
#define _EBML2_EBML_MASTER_H

#include "ebml/EbmlElement.h"

#define EBML_MASTER_ITERATOR  EbmlMaster_itr
#define EBML_MASTER_CONST_ITERATOR  EbmlMaster_itr
#define EBML_MASTER_RITERATOR EbmlMaster_rev_itr
#define EBML_MASTER_CONST_RITERATOR EbmlMaster_rev_itr

namespace LIBEBML_NAMESPACE {

    class EbmlMaster;

    class EbmlMaster_itr {
    public:
        EbmlElement* operator*() const;
        EbmlMaster_itr& operator++();
        bool operator!=(const EbmlMaster_itr &) const;
        bool operator==(const EbmlMaster_itr &) const;
    };

    class EbmlMaster_rev_itr {
    public:
        EbmlElement* operator*() const;
        EbmlMaster_rev_itr& operator--();
        bool operator!=(const EbmlMaster_rev_itr &) const;
        bool operator==(const EbmlMaster_rev_itr &) const;
    };

    class EbmlMaster : public EbmlElement {
    public:
        EbmlMaster(const ebml_context &, ebml_element *WithNode = NULL);
        EbmlElement *FindElt(const ebml_context & Kind) const;
		EbmlElement *FindFirstElt(const ebml_context & Kind) const;
		EbmlElement *FindFirstElt(const ebml_context & Kind, const bool bCreateIfNull) const;
		EbmlElement *FindNextElt(const EbmlElement & Past) const;
		EbmlElement *FindNextElt(const EbmlElement & Past, const bool bCreateIfNull);
		EbmlElement *AddNewElt(const ebml_context & Kind);
		bool InsertElement(EbmlElement & element, size_t position = 0);
		bool PushElement(EbmlElement & element);

        size_t ListSize() const;
		EbmlElement * operator[](size_t position);
		const EbmlElement * operator[](size_t position) const;
		/*!
			\brief remove all elements, even the mandatory ones
		*/
		void RemoveAll();
		/*!
			\brief Remove an element from the list of the master
		*/
        void Remove(const EBML_MASTER_ITERATOR & Itr);
        void Remove(const EBML_MASTER_RITERATOR & Itr);

        virtual EbmlElement * Clone() const;

        void Sort();
        bool CheckMandatory() const;
		/*!
			\brief Remove an element from the list of the master
		*/
		void Remove(size_t Index);
        filepos_t WriteHead(IOCallback & output, size_t SizeLength, bool bWithDefault = false);

        EBML_MASTER_ITERATOR begin() const;
        EBML_MASTER_ITERATOR end() const;
        EBML_MASTER_RITERATOR rbegin() const;
        EBML_MASTER_RITERATOR rend() const;

        // virtual methods needed for the Core-C counterpart
        virtual filepos_t ReadData(IOCallback & input, ScopeMode ReadFully = SCOPE_ALL_DATA);
        virtual filepos_t RenderData(IOCallback & output, bool bForceRender, bool bSaveDefault = false);
        virtual filepos_t UpdateSize(bool bWithDefault = false, bool bForceRender = false);
    };

    template <typename Type>
    Type & GetChild(EbmlMaster & Master)
    {
        return *(static_cast<Type *>(Master.FindFirstElt(Type::GetContext(), true)));
    }

    template <typename Type>
    Type & GetNextChild(EbmlMaster & Master, const Type & PastElt)
    {
	    return *(static_cast<Type *>(Master.FindNextElt(PastElt, true)));
    }

    template <typename Type>
    Type * FindChild(EbmlMaster & Master)
    {
	    return static_cast<Type *>(Master.FindFirstElt(Type::GetContext(), false));
    }

    template <typename Type>
    Type & AddNewChild(EbmlMaster & Master)
    {
	    return *(static_cast<Type *>(Master.AddNewElt(Type::GetContext())));
    }
};

#endif // _EBML2_EBML_MASTER_H
