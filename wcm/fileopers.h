/*
   Copyright (c) by Valery Goryachev (Wal)
*/


#ifndef FILEOPERS_H
#define FILEOPERS_H

#include "operthread.h"
#include "vfs.h"
#include "mfile.h"

class OF_FSCInfo: public FSCInfo
{
	OperThreadNode* volatile _node;
public:
	OF_FSCInfo( OperThreadNode* n ): _node( n ) {}
	virtual bool SmbLogon( FSSmbParam* a );
	virtual bool FtpLogon( FSFtpParam* a );
	virtual bool Prompt( const unicode_t* header, const unicode_t* message, FSPromptData* p, int count );
	virtual bool Stopped();
	virtual ~OF_FSCInfo();
};


class OperFileThread: public OperThread
{
	OF_FSCInfo _info;
public:
	OperFileThread( const char* opName, NCDialogParent* par, OperThreadNode* n )
		: OperThread( opName, par, n ), _info( n ) {}
	OF_FSCInfo* Info() { return &_info; }
	virtual ~OperFileThread();
};


////////////////////////////////   read DIR

class OperRDData: public OperData
{
public:
	bool volatile executed;
	clPtr<FS> fs; //??volatile
	FSPath path; //??volatile
	FSString errorString; //??volatile
	FSString nonFatalErrorString;
	clPtr<FSList> list; //??volatile

	OperRDData( NCDialogParent* p ): OperData( p ), executed( false ) {}

	void SetNewParams( clPtr<FS> f, FSPath& p )
	{
		executed = false;
		fs = f;
		path = p;
		list.clear();
		errorString = "";
		nonFatalErrorString = "";
	}

	virtual ~OperRDData();
};

void ReadDirThreadFunc( OperThreadNode* node );


class OperFileNameWin: public Win
{
	std::vector<unicode_t> text;
	int _ccount;
public:
	OperFileNameWin( Win* parent, int ccount = 80 )
		:  Win( Win::WT_CHILD, 0, parent, 0 ), _ccount( ccount )
	{
		wal::GC gc( this );
		cpoint size = GetStaticTextExtent( gc, ABCString, GetFont() );
		size.x = size.x / ABCStringLen * ccount;

		if ( size.x > 500 ) { size.x = 550; }

		SetLSize( LSize( size ) );
	}
	void SetText( const unicode_t* s )
	{
		text = new_unicode_str( s );
		Invalidate();
	}

	void Paint( wal::GC& gc, const crect& paintRect );
	virtual ~OperFileNameWin();
};


///////////////////////////////// common file opers

bool MkDir( clPtr<FS> f, FSPath& p, NCDialogParent* parent );
bool DeleteList( clPtr<FS> f, FSPath& p, clPtr<FSList> list, NCDialogParent* parent );
clPtr<cstrhash<bool, unicode_t> > CopyFiles( clPtr<FS> srcFs, FSPath& srcPath, clPtr<FSList> list, clPtr<FS> destFs, FSPath& destPath, NCDialogParent* parent );
bool MoveFiles( clPtr<FS> srcFs, FSPath& srcPath, clPtr<FSList> list, clPtr<FS> destFs, FSPath& destPath, NCDialogParent* parent );

#endif
