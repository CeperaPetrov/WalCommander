/*
	Copyright (c) by Valery Goryachev (Wal)
*/


#include "swl.h"

namespace wal {

int uiClassVListWin = GetUiID("VListWin");
int VListWin::UiGetClassId(){	return uiClassVListWin; }

VListWin::~VListWin()
{
	if (IsCaptured()) this->ReleaseCapture(&captureSD);
}

VListWin::VListWin(WTYPE wt, unsigned hints, int nId, Win *parent, SelectType st, BorderType bt, crect *rect)
:Win(wt, hints, parent, rect, nId),
	selectType(st),
	borderType(bt),
	itemHeight(1),
	itemWidth(1),
	xOffset(0),
	count(0),
	first(0),
	pageSize(1),
	current(-1),
	captureDelta(0),
//	borderColor(0),
//	bgColor(0x808080),
	vScroll(0, this,true,true),
	hScroll(0, this,false),
	layout(4,4)

{
	vScroll.Show(SHOW_INACTIVE);   //!!! ��������� � �������� ???
	vScroll.Enable();
	hScroll.Show(SHOW_INACTIVE);
	hScroll.Enable();

	vScroll.SetManagedWin(this);
	hScroll.SetManagedWin(this);

	layout.AddWin(&vScroll,1,2);
	layout.AddWin(&hScroll,2,1);
	layout.AddRect(&listRect,1,1);
	layout.AddRect(&scrollRect,2,2);
	int borderWidth = (bt==SINGLE_BORDER)? 1 :(bt == BORDER_3D ? 2 : 0);
	layout.LineSet(0,borderWidth);
	layout.LineSet(3,borderWidth);
	layout.ColSet(0,borderWidth);
	layout.ColSet(3,borderWidth);
	layout.SetColGrowth(1);
	layout.SetLineGrowth(1);
	this->SetLayout(&layout);

	RecalcLayouts();
	CheckPage(true);
}

bool VListWin::CheckPage(bool mustVisible)
{
	
	int p = listRect.Height() / itemHeight;

	if (p <= 0) p = 1;

	int f = first;

	if (current >= 0 && mustVisible)
	{
		if (current >= f + p) f = current - p + 1;
		if (current < f) f = current;
	}

	if (f + p > count) f = count - p;

	if (f < 0) f = 0;

	bool ret = pageSize != p || f != first ;

	pageSize = p;
	first = f;

	return ret;
}

void VListWin::CalcScroll()
{
	if (itemHeight<=0) itemHeight=1;
	if (itemWidth<=0) itemWidth=1;

//	int n = listRect.Height()/itemHeight;
//	pageSize = n;
	ScrollInfo vsi, hsi;
	vsi.pageSize = pageSize;
	vsi.size = count;
	vsi.pos = first;

	hsi.pageSize = listRect.Width();
	hsi.size = itemWidth;
	hsi.pos = xOffset;

	bool vVisible = vScroll.IsVisible();
	vScroll.Command(CMD_SCROLL_INFO, SCMD_SCROLL_VCHANGE, this, &vsi);

	bool hVisible = hScroll.IsVisible();
	hScroll.Command(CMD_SCROLL_INFO, SCMD_SCROLL_HCHANGE, this, &hsi);

	if (vVisible != vScroll.IsVisible() || hVisible != hScroll.IsVisible()) 
		this->RecalcLayouts();

//MB(L"%i,%i,%i", hsi.size, hsi.pageSize, hsi.pos);
}

void VListWin::MoveFirst(int n)
{
	if (n + pageSize >= count) 
		n = count - pageSize;
		
	if (n < 0) n = 0;
	
	if (first != n) 
	{
		first = n;
		CalcScroll();
		Invalidate();
	}
}

void VListWin::MoveXOffset(int n)
{
	if (n + listRect.Width()>= itemWidth) n = itemWidth - listRect.Width();
	if (n<0) n = 0;
	if (xOffset !=n) {
		xOffset = n;
		CalcScroll();
		Invalidate();
	}
}

bool VListWin::Command(int id, int subId, Win *win, void *data)
{
	if (id == CMD_SCROLL_INFO) {
		switch (subId) {
		case SCMD_SCROLL_LINE_UP: MoveFirst(first-1);break;
		case SCMD_SCROLL_LINE_DOWN:MoveFirst(first+1);break;
		case SCMD_SCROLL_PAGE_UP:MoveFirst(first-pageSize); break;
		case SCMD_SCROLL_PAGE_DOWN:MoveFirst(first+pageSize); break;
		case SCMD_SCROLL_LINE_LEFT: MoveXOffset(xOffset-10);break;
		case SCMD_SCROLL_LINE_RIGHT:MoveXOffset(xOffset+10);break;
		case SCMD_SCROLL_PAGE_LEFT: MoveXOffset(xOffset - listRect.Width());break;
		case SCMD_SCROLL_PAGE_RIGHT:MoveXOffset(xOffset+listRect.Width());break;

		case SCMD_SCROLL_TRACK:(win == &vScroll) ? 
					MoveFirst(((int*)data)[0]) :
					MoveXOffset(((int*)data)[0]);break;
		}
		return true;
	}
	return Win::Command(id, subId, win, data);
}

void VListWin::Paint(GC &gc, const crect &paintRect)
{
	unsigned bgColor = UiGetColor(uiBackground,0,0,0x808080);
	crect rect = ClientRect();
	switch (borderType) {
	case SINGLE_BORDER: DrawBorder(gc,rect, UiGetColor(uiFrameColor, 0, 0, 0x0)); break;
	case BORDER_3D: 
		if (UiGetBool(ui3d, 0, 0, true)) 
			Draw3DButtonW2(gc,rect, bgColor, false); 
		else {
			DrawBorder(gc,rect, ColorTone(bgColor,-100));
			rect.Dec();
			DrawBorder(gc,rect, bgColor);
		}
	default: ;
	}

	if (!scrollRect.IsEmpty()) 
	{
		gc.SetFillColor(0xD0D0D0);
		gc.FillRect(scrollRect); //CCC
	}

	if (itemHeight > 0) 
	{
		int n = (rect.Height() + (itemHeight - 1)) / itemHeight;
		crect r = this->listRect;
		int bottom = r.bottom;
		r.bottom = r.top + itemHeight;
		
		for (int i = 0; i < n; i++) 
		{
			gc.SetClipRgn(&r);
			crect r1(r); 
			r1.left -= xOffset;
			this->DrawItem(gc, i+first,r1);
			r.top += itemHeight;
			r.bottom += itemHeight;
			if (r.bottom>bottom) r.bottom = bottom;
		}
	} else {
		//�� ������ ������
		rect.Dec();
		gc.SetFillColor(bgColor);
		gc.FillRect(rect);
	}
}

bool VListWin::EventFocus(bool recv)
{
	Invalidate();
	return true;
}

void VListWin::EventSize(cevent_size *pEvent)
{
	RecalcLayouts();
	CheckPage(true);
	CalcScroll();
	if (CheckPage(true))
		Invalidate();
	//MoveFirst(first);
}

bool VListWin::EventMouse(cevent_mouse* pEvent)
{
	if (!IsEnabled()) return false;
			
	if (!IsCaptured() && !listRect.In(pEvent->Point()))
	{
		//��������� ������ ����� ������������� � �������� �� ������� ���� (�������� �� ComboBox)
		crect rect = ScreenRect();
		cpoint point = pEvent->Point();

		point.x += rect.left;
		point.y += rect.top;

		if (vScroll.IsVisible())
		{
			rect = vScroll.ScreenRect();
			if (rect.In(point)){
				cevent_mouse ev(pEvent->Type(), cpoint(point.x - rect.left, point.y - rect.top), pEvent->Button(), pEvent->ButtonFlag(), pEvent->Mod());
				return vScroll.EventMouse(&ev);
			}
		}

		if (hScroll.IsVisible())
		{
			rect = hScroll.ScreenRect();
			if (rect.In(point)){
				cevent_mouse ev(pEvent->Type(), cpoint(point.x - rect.left, point.y - rect.top), pEvent->Button(), pEvent->ButtonFlag(), pEvent->Mod());
				return hScroll.EventMouse(&ev);
			}
		}


		return true;
	}

	int n = (pEvent->Point().y - listRect.top)/this->itemHeight + first;

		
	if (pEvent->Type() == EV_MOUSE_WHEEL) 
	{
		if (pEvent->Delta() > 0) 
		{
			int n = pageSize / 3;
			if (n < 1) n = 1;
			MoveFirst(first - n); 
			return true;
		}

		if (pEvent->Delta() < 0 ) 
		{
			int n = pageSize / 3;
			if (n < 1) n = 1;
			MoveFirst(first + n); 
			return true;
		}

		return true;
	}

	if (pEvent->Type() == EV_MOUSE_PRESS && pEvent->Button() == MB_L && listRect.In(pEvent->Point()))
	{
		captureDelta=0;
		MoveCurrent(n);
		if (this->SetCapture(&captureSD))
		{
			this->SetTimer(0,100);
		}
		return true;
	}

	if (pEvent->Type() == EV_MOUSE_DOUBLE)
	{
		MoveCurrent(n);
		Command(CMD_ITEM_DOUBLECLICK, GetCurrent(), this, 0);
		return true;
	}

	if (pEvent->Type() == EV_MOUSE_MOVE && IsCaptured())
	{
		if (pEvent->Point().y>=listRect.top && pEvent->Point().y<=listRect.bottom) {
			MoveCurrent(n);
			captureDelta=0;
		} else {
			captureDelta = (pEvent->Point().y > listRect.bottom) ? +1 : (pEvent->Point().y < listRect.top)? -1:0;
		}
		return true;
	}

	if (pEvent->Type() == EV_MOUSE_RELEASE && pEvent->Button()==MB_L)
	{
		if (IsCaptured()) this->ReleaseCapture(&captureSD);
		this->DelTimer(0);

		Command(CMD_ITEM_CLICK, GetCurrent(), this, 0);
		return true;
	}

	return false;
}

void VListWin::EventTimer(int tid)
{
	if (tid == 0 && captureDelta) 
	{
		int n = current + captureDelta;
		if (n>=0 && n<this->count) 
			MoveCurrent(n);
	}
}

bool VListWin::EventKey(cevent_key* pEvent)
{
	if (pEvent->Type() == EV_KEYDOWN)
	{
		switch (pEvent->Key()) {
		case VK_DOWN: MoveCurrent(current+1); break;
		case VK_UP: MoveCurrent(current-1); break;
		case VK_END: MoveCurrent(count-1); break;
		case VK_HOME: MoveCurrent(0); break;
		case VK_PRIOR: MoveCurrent(current - (pageSize<=1 ? pageSize:pageSize-1)); break;
		case VK_NEXT: MoveCurrent(current + (pageSize<=1 ? pageSize:pageSize-1)); break;

		case VK_LEFT: MoveXOffset(xOffset-10); break;
		case VK_RIGHT: MoveXOffset(xOffset+10); break;

		case VK_RETURN: Command(CMD_ITEM_ENTER, GetCurrent(), this, 0); break; 

		default: return false;
		}
		Invalidate();
		return true;
	}
	return false;
}


void VListWin::SetCount(int c){ count=c; }

void VListWin::SetCurrent(int n)
{
	current = n;
}

void VListWin::MoveCurrent(int n, bool mustVisible)
{
	if (selectType == NO_SELECT) 
		return;
		
	int f = first;
	
	if (n < 0) n = 0; //count-1;
	
	if (n >= count) 
		n = count-1; //n = 0;
		
	if (mustVisible) 
	{
		if (n - f >= pageSize) 
		{
			f = n - pageSize + 1;
		}
		if (f > n) f = n;
		if (f < 0) f = 0;
	}
	
	bool redraw = (f != first || n != current);
	first = f;
	
	bool curChanged = (n != current);
	
	current = n;
	
	if (redraw) 
	{ 
		CalcScroll();
		Invalidate();
	}
	
	if (curChanged) 
		Command(CMD_ITEM_CHANGED, GetCurrent(), this, 0);
}

void VListWin::SetNoCurrent()
{
	if (current == -1) return;
	current = -1;
	Command(CMD_ITEM_CHANGED, GetCurrent(), this, 0);
	if (IsVisible()) Invalidate();
}

void VListWin::SetItemSize(int h, int w)
{
	if (itemHeight !=h || itemWidth !=w) 
	{
		itemHeight = h;
		itemWidth = w;
		CalcScroll();
		Invalidate(); 
	}
}



void VListWin::DrawItem(GC &gc, int n, crect rect)
{	
	gc.SetFillColor((n%2)?0xFFFFFF:0xE0FFE0);
	gc.FillRect(rect);
}

void VListWin::ClearSelection(){ selectList.clear(); }
bool VListWin::IsSelected(int n){ return selectList.exist(n); }

void VListWin::ClearSelected(int n1, int n2)
{
	if (selectType != MULTIPLE_SELECT) return;
	for (int i = n1; i<n2; i++)
		selectList.del(i);
}

void VListWin::SetSelected(int n1, int n2)
{
	if (selectType != MULTIPLE_SELECT) return;
	for (int i = n1; i<n2; i++)
		selectList.put(i);
}


}; //namespace wal