#ifndef EXT_APP_H
#define EXT_APP_H

#include <swl.h>
using namespace wal;


inline void InitExtensionApp() {};

struct AppList: public iIntrusiveCounter
{
	struct Node
	{
		std::vector<unicode_t> name;

		bool terminal;
		std::vector<unicode_t> cmd;
		//or
		clPtr<AppList> sub;

		Node(): terminal( 0 ) {}
	};
	ccollect<Node> list;
	int Count() const { return list.count(); }
//	void Print(int lev=0);
};


std::vector<unicode_t> GetOpenCommand( const unicode_t* uri, bool* needTerminal, const unicode_t** pAppName );

clPtr<AppList> GetAppList( const unicode_t* uri );


#endif
