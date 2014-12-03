/*
	Copyright (c) by Valery Goryachev (Wal)
*/


#ifndef PANEL_H
#define PANEL_H

#include "swl.h"
#include "vfs.h"
#include "vfs-uri.h"
#include "operwin.h"
#include "fileopers.h"
#include "panel_list.h"

using namespace wal;

extern int uiClassPanel;

class PanelWin;


class PanelSearchWin: public Win {
	PanelWin *_parent;
	EditLine _edit;
	StaticLine _static;
	Layout _lo;
	carray<unicode_t> oldMask;
public:
	cptr<cevent_key> ret_key;
	PanelSearchWin(PanelWin *parent, cevent_key *key);
	virtual void Paint(wal::GC &gc, const crect &paintRect);
	virtual bool EventShow(bool show);
	virtual bool EventKey(cevent_key* pEvent);
	virtual bool EventChildKey(Win* child, cevent_key* pEvent);
	virtual cfont* GetChildFont(Win *w, int fontId);
	virtual bool Command(int id, int subId, Win *win, void *data);
	
	virtual bool EventMouse(cevent_mouse* pEvent);
	
	void EndSearch(cevent_key* pEvent);
	
	virtual ~PanelSearchWin();
};

class PanelPlace {
	struct Node {
		FSPtr fsPtr;
		FSPath path;
		Node *next;
	};
	Node *stack;
	void Clear(bool toSys = false)
	{
		while (stack) { 
			if (toSys && !stack->fsPtr.IsNull() && stack->fsPtr->Type() == FS::SYSTEM && !stack->next) 
				break;
			Node *p=stack; stack = stack->next; delete p;
		}
	}
	FSPath emptyPath;
public:
	PanelPlace():stack(0){}
	
	bool Pop(){ 
		if (stack && stack->next) { Node *p=stack; stack = stack->next; delete p; return true; }
		return false;
	}
	
	bool Set(FSPtr fsPtr, FSPath &path, bool push){ 
		if (fsPtr.IsNull()) return false; 
		cptr<Node> node = new Node;
		node->fsPtr = fsPtr;
		node->path = path;
		if (!push) 
			Clear(fsPtr->Type() != FS::SYSTEM);
		node->next = stack;
		stack = node.ptr();
		node.drop();
		return true;
	}
	
	int Count(){ Node *p = stack; int n = 0; while (p) { p=p->next; n++; } return n; }
	
	void Reset(FSPtr fsPtr, FSPath &path)
	{
		if (fsPtr.IsNull()) return; 
		if (stack) {
			stack->path = path;
			stack->fsPtr = fsPtr;
		} else
			Set(fsPtr, path, false);
	}

	bool IsEmpty() const { return stack == 0; }
	
	FS* GetFS(){ return stack ? stack->fsPtr.Ptr() : 0; }
	FSPtr GetFSPtr(){ return stack ? stack->fsPtr : FSPtr(); }
	FSPath* GetPathPtr(){ return stack ? &stack->path : 0; }
	FSPath& GetPath(){ return stack ? stack->path : emptyPath; }
	~PanelPlace(){ Clear(); }
};
 
class PanelWin: public NCDialogParent {
public:
	enum VIEW_MODE {
		MEDIUM = 0,
		BRIEF = 1,
		FULL = 2,
		FULL_ST = 3,
		FULL_ACCESS = 4,
		TWO_COLUMNS = 5
	};
	
	enum LOAD_TYPE { SET = 0, PUSH, RESET };

private:
	static unicode_t dirPrefix[];
	static unicode_t exePrefix[];

	int dirPrefixW;
	int exePrefixW;

	Layout _lo;
	crect _centerRect;
	crect _headRect;
	crect _footRect;

	ScrollBar _scroll;
	PanelList _list;
	cptr<PanelSearchWin> _search;
		
	cpoint _letterSize[2];
	int _itemHeight;
	int _rows;
	int _cols;
	int _first;
	int _current;
	int *_viewMode;
	FSStatVfs _vst;
	
	ccollect<crect, 0x100> _rectList;
	ccollect<crect, 0x100> _emptyRectList;
	ccollect<crect, 0x100> _vLineRectList;

	crect* GetItemRect(int n)
	{ 
		if ( n < _first || n >= _rectList.count()) 
			return 0;
		return & _rectList[n - _first];
	}

	int GetItemNumber(int r, int c)
	{
		int n = c*_rows + r;
		return _first + n;
	}
	
	void SetScroll();
	
	void Check();
	void DrawItem(wal::GC &gc,  int n);
	void DrawFooter(wal::GC &gc);
	void RedrawList(wal::GC &gc);

	PanelPlace _place;
		
	unicode_t userNameBuf[64];
	unicode_t groupNameBuf[64];
	
	const unicode_t* GetUserName(int id){ FS *fs = GetFS(); if (fs) return fs->GetUserName(id, userNameBuf); userNameBuf[0]=0; return userNameBuf; }
	const unicode_t* GetGroupName(int id){ FS *fs = GetFS(); if (fs) return fs->GetGroupName(id, groupNameBuf); groupNameBuf[0]=0; return groupNameBuf; }
	
	bool _inOperState;
	LOAD_TYPE _operType;
	OperRDData _operData;
	cptr<cstrhash<bool,unicode_t> > _operSelected;
	FSString _operCurrent;
	
public:
	PanelWin(Win *parent, int *mode);
	bool IsSelectedPanel();
	void SelectPanel();
	
	FSPath& GetPath(){ return _place.GetPath(); }
	FS* GetFS(){ return _place.GetFS(); }
	FSPtr GetFSPtr(){ return _place.GetFSPtr(); }

	FSStatVfs StatVfs(){ return _vst; }
	
	FSString UriOfDir();
	FSString UriOfCurrent();
	
	void SortByName();
	void SortByExt();
	void SortBySize();
	void SortByMTime();
	void DisableSort();
	
	cptr<cevent_key> QuickSearch(cevent_key *key);
	bool Search(unicode_t *s, bool nextSearch);
	
	void SetCurrent(int n);
	bool SetCurrent(FSString &a);
	void SetCurrent(int n, bool shift, int *selectType); //selectType 0-disable selection, 1-enable, -1 - not defined
	
	int ViewMode(){ return *_viewMode; }
	void SetViewMode(int m){ *_viewMode = m; Check(); SetScroll(); Invalidate(); }

	void LoadPath(FSPtr fs, FSPath &path, FSString *current, cptr<cstrhash<bool,unicode_t> > selected, LOAD_TYPE lType);
	
	void Reread(FSString *current);
	void Reread(bool resetCurrent = false);

	
//	void KeyUp(){ SetCurrent(_current-1); };
//	void KeyDown(){ SetCurrent(_current+1); };
//	void KeyEnd() { SetCurrent(_list.Count()-1);}
//	void KeyHome() { SetCurrent(0); }
	void KeyPrior() { SetCurrent(_current-_rows*_cols+1); }
	void KeyNext() { SetCurrent(_current+_rows*_cols-1); }
//	void KeyLeft() { SetCurrent(_current-_rows);  }
//	void KeyRight() { SetCurrent(_current+_rows); }
	
	void KeyUp	(bool shift, int *selectType) { SetCurrent(_current-1,	shift, selectType); };
	void KeyDown	(bool shift, int *selectType) { SetCurrent(_current+1,	shift, selectType); };
	void KeyEnd	(bool shift, int *selectType) { SetCurrent(_list.Count()-1,	shift, selectType);}
	void KeyHome	(bool shift, int *selectType) { SetCurrent(0,	shift, selectType); }
	void KeyPrior	(bool shift, int *selectType) { SetCurrent(_current-_rows*_cols+1,	shift, selectType); }
	void KeyNext	(bool shift, int *selectType) { SetCurrent(_current+_rows*_cols-1,	shift, selectType); }
	void KeyLeft	(bool shift, int *selectType) { SetCurrent(_current-_rows,	shift, selectType);  }
	void KeyRight	(bool shift, int *selectType) { SetCurrent(_current+_rows,	shift, selectType); }
	
	void KeyIns(){ _list.InvertSelection(_current); SetCurrent(_current+1); }

	int Current(){ return _current; }
	FSNode* GetCurrent(){ return _list.Get(_current); }
	const unicode_t* GetCurrentFileName(){ return _list.GetFileName(_current); }
	
	PanelCounter GetSelectedCounter(){ return _list.SelectedCounter(); }
	
	int Count(){ return _list.Count(); }
	const FSNode* Get(int n){ return _list.Get(n); }
	
	//dir movies
	void DirUp();
	void DirEnter();
	void DirRoot();
	
	virtual void OperThreadSignal(int info);
	virtual void OperThreadStopped();
	
	cptr<FSList> GetSelectedList() 
	{
		if (_list.SelectedCounter().count>0) return _list.GetSelectedList();
		cptr<FSList> p = new FSList;
		FSNode *node = GetCurrent();
		if (node) p->CopyOne(node);
		return p;
	}
	
	void Invert(){ _list.InvertSelection(); Invalidate(); }
	void ClearSelection(cstrhash<bool,unicode_t> *resList){ _list.ClearSelection(resList); }
	void Mark(const unicode_t *mask, bool enable){ _list.Mark(mask, enable); Invalidate(); }
	
	virtual int UiGetClassId();
	virtual void Paint(wal::GC &gc, const crect &paintRect);
	virtual void EventSize(cevent_size *pEvent);
	virtual bool EventMouse(cevent_mouse* pEvent);
	virtual bool Command(int id, int subId, Win *win, void *data);
	virtual bool Broadcast(int id, int subId, Win *win, void *data); 
	virtual void OnChangeStyles();
	virtual ~PanelWin();
};

#endif
