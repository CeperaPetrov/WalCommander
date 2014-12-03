
#ifdef _WIN32
#include <winsock2.h>
#endif

#include "ncdialogs.h"
#include "sftpdlg.h"

#include "operthread.h" //для carray_cat
#include "charsetdlg.h"
#include "ltext.h"
#include "nceditline.h"


class SftpLogonDialog: public NCVertDialog {
	Layout iL;
public:	
	StaticLine serverText; 
	StaticLine userText; 
	StaticLine portText;
	int charset;
	StaticLine charsetText, charsetIdText;
	
	NCEditLine serverEdit;
	NCEditLine userEdit;
	EditLine portEdit;
	Button charsetButton;
	
	SftpLogonDialog(NCDialogParent *parent, FSSftpParam &params);
	virtual bool EventChildKey(Win* child, cevent_key* pEvent);
	virtual bool Command(int id, int subId, Win *win, void *data);
	virtual ~SftpLogonDialog();
};

SftpLogonDialog::~SftpLogonDialog(){}

SftpLogonDialog::SftpLogonDialog(NCDialogParent *parent, FSSftpParam &params)
:	NCVertDialog(::createDialogAsChild, 0, parent, utf8_to_unicode( _LT("SFTP logon") ).ptr(), bListOkCancel),
	iL(16, 3),
	serverText(0, this, utf8_to_unicode( _LT("Server:") ).ptr()),
	userText(0, this, utf8_to_unicode( _LT("Login:") ).ptr()),

	portText(0, this, utf8_to_unicode( _LT("Port:") ).ptr()),
	charsetText(0, this, utf8_to_unicode( _LT("Charset:") ).ptr()),
	
	charset(params.charset),
	charsetIdText(0, this, utf8_to_unicode("***************").ptr()), //чтоб место забить
		
	serverEdit	("sftp-server", 0, this, 0, 16, 5, false, true, false), //(0, this, 0, 0, 16),
	userEdit	("sftp-user", 0, this, 0, 16, 5, false, true, false), //(0, this, 0, 0, 16),
	portEdit	(0, this, 0, 0, 7),
	
	charsetButton(0, this, utf8_to_unicode(">").ptr() , 1000)
{
	serverEdit.SetText(params.server.Data(), true);
	userEdit.SetText(params.user.Data(), true);
	
	char buf[0x100];
	snprintf(buf, sizeof(buf), "%i", params.port);
	portEdit.SetText(utf8_to_unicode(buf).ptr(), true);

	bool focus = false;

	iL.AddWin(&serverText,	0, 0, 0, 0); serverText.Enable(); serverText.Show(); 
	iL.AddWin(&serverEdit,	0, 1, 0, 1); serverEdit.Enable(); serverEdit.Show(); 
	if (!focus && !params.server.Data()[0]) { serverEdit.SetFocus(); focus = true; }

	iL.AddWin(&userText,	2, 0, 2, 0);	userText.Enable(); userText.Show(); 
	iL.AddWin(&userEdit,	2, 1, 2 ,1);	userEdit.Enable(); userEdit.Show(); 
	if (!focus && !params.user.Data()[0]) { userEdit.SetFocus(); focus = true; }

	iL.AddWin(&portText,	4, 0, 4, 0); portText.Enable(); portText.Show(); 
	iL.AddWin(&portEdit,	4, 1, 4, 1); portEdit.Enable(); portEdit.Show(); 
	
	iL.AddWin(&charsetText,	5, 0, 5, 0); charsetText.Enable(); charsetText.Show(); 
	charsetIdText.SetText(utf8_to_unicode(charset_table[params.charset]->name).ptr());
	iL.AddWin(&charsetIdText,5, 1, 5, 1); charsetIdText.Enable(); charsetIdText.Show();
	iL.AddWin(&charsetButton,5, 2); charsetButton.Enable(); charsetButton.Show();
	
	AddLayout(&iL);
	SetEnterCmd(CMD_OK);
	
	order.append(&serverEdit);
	order.append(&userEdit);
	order.append(&portEdit);
	order.append(&charsetButton);
	SetPosition();
}

bool SftpLogonDialog::Command(int id, int subId, Win *win, void *data)
{
	if (id == 1000) {
		int ret;
		if (SelectCharset((NCDialogParent*)Parent(), &ret, charset))
		{
			charset = ret;
			charsetIdText.SetText(utf8_to_unicode(charset_table[ret]->name).ptr());
		}
		return true;
	}
	return NCVertDialog::Command(id, subId, win, data);
}

bool SftpLogonDialog::EventChildKey(Win* child, cevent_key* pEvent)
{
	if (pEvent->Type() == EV_KEYDOWN) 
	{
		if (pEvent->Key() == VK_RETURN && charsetButton.InFocus()) //prevent autoenter
			return false;
		
	}; 
 	return NCVertDialog::EventChildKey(child, pEvent);
}

bool GetSftpLogon(NCDialogParent *parent, FSSftpParam &params)
{
	SftpLogonDialog dlg(parent, params);
	if (dlg.DoModal() == CMD_OK)
	{
		params.server	= dlg.serverEdit.GetText().ptr();
		dlg.serverEdit.Commit();

		params.user	= dlg.userEdit.GetText().ptr();
		dlg.userEdit.Commit();

		params.port	= atoi(unicode_to_utf8(dlg.portEdit.GetText().ptr()).ptr());
		params.isSet	= true;
		params.charset = dlg.charset;
		return true;
	}
	return false;
}




//////////////////////////  Prompt

class FSPromptDialog: public NCVertDialog {
	Layout iL;
public:	
	struct Node {
		cptr<StaticLine> prompt; 
		cptr<EditLine> ansver;
	};
	StaticLine message;
	ccollect<Node > list;

	FSPromptDialog(PromptCBData *data);
	virtual ~FSPromptDialog();
};

FSPromptDialog::~FSPromptDialog(){}

FSPromptDialog::FSPromptDialog(PromptCBData *data)
:	NCVertDialog(::createDialogAsChild, 0, data->parent, data->header, bListOkCancel),
	iL(16, 3),
	message(0, this, data->message)
{

	if (data->message[0]) 
	{
		iL.AddWin(&message,	0, 0, 0, 1);
		message.Enable(); message.Show();
	}

	int n = data->count;
	if (n > 16) n = 16;
	
	for (int i = 0; i<data->count; i++)
	{
		Node node;
		node.prompt = new StaticLine(0, this, data->prompts[i].prompt.Data()); 
		node.ansver = new EditLine(0, this, 0, 0, 16); 
		if (!data->prompts[i].visible)	node.ansver->SetPasswordMode();
		
		
		iL.AddWin(node.prompt.ptr(),	i+1, 0, i+1, 0);
		iL.AddWin(node.ansver.ptr(),	i+1, 1, i+1, 1);
		
		node.prompt->Enable(); node.prompt->Show();
		node.ansver->Enable(); node.ansver->Show();
		
		if (i==0) node.ansver->SetFocus();
		
		order.append(node.ansver.ptr()); //������� � list.append - �����
		
		list.append(node);
		
	}
	AddLayout(&iL);
	SetEnterCmd(CMD_OK);

}

bool GetPromptAnswer(PromptCBData *data)
{
	FSPromptDialog dlg(data);
	if (dlg.DoModal() != CMD_OK) return false;
	
	for (int i = 0; i<data->count; i++)
		data->prompts[i].prompt = dlg.list[i].ansver->GetText().ptr();
	
	return true;
}

