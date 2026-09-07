/*
		Project:		WINEDIT (Windows MDI-Editor)
		Module: 		WINEDIT.CPP
		Description:	An MDI text editor based on the windows edit control
		Author:			Martin Gäckler
		Address:		Hofmannsthalweg 14, A-4030 Linz
		Web:			https://www.gaeckler.at/

		Copyright:		(c) 1995-2026 Martin Gäckler

		This program is free software: you can redistribute it and/or modify  
		it under the terms of the GNU General Public License as published by  
		the Free Software Foundation, version 3.

		You should have received a copy of the GNU General Public License 
		along with this program. If not, see <http://www.gnu.org/licenses/>.

		THIS SOFTWARE IS PROVIDED BY Martin Gäckler, Linz, Austria ``AS IS''
		AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
		TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
		PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR
		CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
		SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
		LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF
		USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
		ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
		OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
		OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
		SUCH DAMAGE.
*/

// --------------------------------------------------------------------- //
// ----- switches ------------------------------------------------------ //
// --------------------------------------------------------------------- //

#define STRICT

// --------------------------------------------------------------------- //
// ----- includes ------------------------------------------------------ //
// --------------------------------------------------------------------- //

#include <memory>

#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include <io.h>

#include <gak/xml.h>
#include <gak/directory.h>
#include <gak/ensemble.h>

#include <winlib/Atom.h>
#include <winlib/WINLIB.H>
#include <winlib/winapp.h>
#include <winlib/mdiwin.h>
#include <winlib/DialogWindow.h>
#include <winlib/StandardDialogs.h>
#include <winlib/FileTypeRegistry.h>
#include <winlib/colors.h>
#include <winlib/version.h>

#include "Dialogs.h"
#include "winedit.h"
#include "winedit.gui.h"
#include "WinEditMain.h"

// --------------------------------------------------------------------- //
// ----- module switches ----------------------------------------------- //
// --------------------------------------------------------------------- //

#ifdef __BORLANDC__
#	pragma option -RT-
#	pragma option -b
#	ifdef __WIN32__
#		pragma option -a4
#		pragma option -pc
#	else
#		pragma option -po
#		pragma option -a2
#	endif
#endif

using namespace gak;
using namespace winlib;
using namespace winlibGUI;

// --------------------------------------------------------------------- //
// ----- constants ----------------------------------------------------- //
// --------------------------------------------------------------------- //

namespace myProfile
{
	const char autoIndent[] = "autoIndent";
	const char autoReload[] = "autoReload";
	const char backup[] = "backup";
	const char charset[] = "charset";
	const char directory[] = "directory";
	const char filterIndex[] = "filterIndex";
	const char Font[] = "Font";
	const char FontSize[] = "FontSize";
	const char lastCharset[] = "lastCharset";
	const char lastFile[] = "lastFile";
	const char filename[] = "filename";
	const char openFile[] = "openFile";
	const char OpenFiles[] = "OpenFiles";
	const char selEnd[] = "selEnd";
	const char selStart[] = "selStart";
	const char stripLineEnds[] = "stripLineEnds";
	const char Style[] = "Style";
	const char tabWidth[] = "tabWidth";
	const char lineWidth[] = "lineWidth";
	const char blockFormat[] = "blockFormat";
};

const char APP_NAME[] = "WinEdit";

// --------------------------------------------------------------------- //
// ----- class definitions --------------------------------------------- //
// --------------------------------------------------------------------- //

class WinEdit : public GuiApplication
{
	bool 			startApplication( HINSTANCE hInstance, const char *cmdLine ) override;
	CallbackWindow	*createMainWindow( const char *cmdLine, int nCmdShow ) override;
	void deleteMainWindow( BasicWindow  *mainWindow ) override;

	public:
	WinEdit() : GuiApplication( EDIT_ICON ) {}
};

// --------------------------------------------------------------------- //
// ----- module statics ------------------------------------------------ //
// --------------------------------------------------------------------- //

static WinEdit	application;

struct FileType
{
	const char	*extension;
	const char	*description;
} fTypes[] =
{
	{ "css", "Cascading Style Sheet" },
	{ "c", "C Source" },
	{ "h", "C Header" },
	{ "cpp", "C++ Source" },
	{ "hpp", "C++ Header" },
	{ "cgi", "Code Guard Configuration" },
	{ "cgl", "Code Guard Log" },
	{ "ini", "Configuration file" },
	{ "cfg", "Configuration file" },
	{ "inf", "Configuration file" },
	{ "hpj", "Help Project" },
	{ "htm", "HTML document" },
	{ "html", "HTML document" },
	{ "js", "Java Script" },
	{ "jav", "Java Source" },
	{ "java", "Java Source" },
	{ "map", "Linker Symbol Table" },
	{ "md", "GIT Readme" },
	{ "out", "Livelink Protocol" },
	{ "mak", "Makefile" },
	{ "def", "Module definition file" },
	{ "bat", "MS-DOS Batch" },
	{ "opl", "OpenUI Source" },
	{ "pas", "Pascal Source" },
	{ "php", "PHP Source" },
	{ "pov", "POV-Ray Source" },
	{ "pc", "Pro*C/C++ Source" },
	{ "log", "Protocol" },
	{ "reg", "Registration file" },
	{ "rc", "Ressource file" },
	{ "rh", "Ressource header" },
	{ "sql", "SQL Script" },
	{ "txt", "Text file" },
	{ "cmd", "Windows NT Batch" },
	{ "dtd", "XML Document Type Definition" },
	{ "xsd", "XML Schema" },
	{ "xsl", "XSL Stylesheet" },
	{ "xml", "XML document" },
};

static STRING getFile( STRING pathName )
{
	size_t	i, dotPos, slashPos;
	bool  	inString = false;

	// find the first blank that delimits params from command
	for( i=0; i<pathName.strlen(); i++ )
		if( pathName[i] == '\"' )
			inString = !inString;
		else if( !inString && pathName[i] == ' ' )
/*v*/		break;

	pathName.cut( i-1 );	// remove parameters

	// find the last slash
	slashPos = 0;
	for( i=0; i<pathName.strlen(); i++ )
		if( pathName[i] == '\\' )
			slashPos = i+1;

	// find the first dot
	for( i=slashPos; i<pathName.strlen(); i++ )
		if( pathName[i] == '.' )
/*v*/		break;

	dotPos = i;

	return pathName.subString( slashPos, dotPos-slashPos ).upperCase();
}

static void registerFileType( const STRING &applicationPath, const STRING &appFile, size_t i )
{
	int					regType;
	FileTypeRegistry	fType;

	fType.type = "",
	fType.type_description = "";

	fType.cmd = "",
	fType.cmd_description = "";

	fType.commandLine = "";

	fType.ddeCommand = "",
	fType.ddeIfExec = "",
	fType.application = "",
	fType.topic = "";

	if( getFileType( fTypes[i].extension, "open", &fType )
	||  !getFile( fType.commandLine ).compareI( appFile )
	||  !getFile( fType.commandLine ).compareI( "winedit" )
	||  !getFile( fType.commandLine ).compareI( "winedt32" )
	||  !getFile( fType.commandLine ).compareI( "notepad" ) )
	{
		// not registered yet
		if( !fType.type[0U] )
		{
			fType.type = fTypes[i].extension;
			fType.type += "_file";
		}

		doLogValueEx( gakLogging::llInfo, fType.type_description );
		doLogValueEx( gakLogging::llInfo, fTypes[i].description );
		if( !fType.type_description[0U] )
			fType.type_description = fTypes[i].description;

		fType.cmd = "open",
		fType.cmd_description = "&Open";

		fType.commandLine = applicationPath;

		fType.ddeCommand = "open %1",
		fType.ddeIfExec = "",
		fType.application = APP_NAME,
		fType.topic = "";

		addFileType( &fType );
		regType = 1;
	}
	else if( getFileType( fTypes[i].extension, "edit", &fType )
	||  !getFile( fType.commandLine ).compareI( appFile )
	||  !getFile( fType.commandLine ).compareI( "winedit" )
	||  !getFile( fType.commandLine ).compareI( "winedt32" )
	||  !getFile( fType.commandLine ).compareI( "notepad" ) )
	{
		// register as edit command
		if( !fType.type[0U] )
		{
			fType.type = fTypes[i].extension;
			fType.type += "_file";
		}

		if( !fType.type_description[0U] )
			fType.type_description = fTypes[i].description;

		fType.cmd = "edit",
		fType.cmd_description = "&Edit",

		fType.commandLine = applicationPath;

		fType.ddeCommand = "open %1",
		fType.ddeIfExec = "",
		fType.application = APP_NAME,
		fType.topic = "";

		addFileType( &fType );
		regType = 2;
	}
	else
	{
		// register as win edit command
		if( !fType.type[0U] )
		{
			fType.type = fTypes[i].extension;
			fType.type += "_file";
		}

		if( !fType.type_description[0U] )
			fType.type_description = fTypes[i].description;

		fType.cmd = "WinEdit",
		fType.cmd_description = "&WinEdit",

		fType.commandLine = applicationPath;

		fType.ddeCommand = "open %1",
		fType.ddeIfExec = "",
		fType.application = APP_NAME,
		fType.topic = "";

		addFileType( &fType );
		regType = 3;
	}

	if( regType < 2
	&&	!getFileType( fTypes[i].extension, "edit", &fType )
	&&  (  !fType.commandLine[0U]
		|| !getFile( fType.commandLine ).compareI( appFile )
		|| !getFile( fType.commandLine ).compareI( "winedit" )
		|| !getFile( fType.commandLine ).compareI( "winedt32" ) ))
	{
		removeFileType( &fType );
	}
	if( regType < 3
	&&	!getFileType( fTypes[i].extension, "WinEdit", &fType )
	&&  (  !fType.commandLine[0U]
		|| !getFile( fType.commandLine ).compareI( appFile )
		|| !getFile( fType.commandLine ).compareI( "winedit" )
		|| !getFile( fType.commandLine ).compareI( "winedt32" ) ))
	{
		removeFileType( &fType );
	}
}

// --------------------------------------------------------------------- //
// ----- class constructors/destructors -------------------------------- //
// --------------------------------------------------------------------- //

// --------------------------------------------------------------------- //
// ----- class inlines ------------------------------------------------- //
// --------------------------------------------------------------------- //

// --------------------------------------------------------------------- //
// ----- class static functions ---------------------------------------- //
// --------------------------------------------------------------------- //

// --------------------------------------------------------------------- //
// ----- class privates ------------------------------------------------ //
// --------------------------------------------------------------------- //

// --------------------------------------------------------------------- //
// ----- class virtuals ------------------------------------------------ //
// --------------------------------------------------------------------- //

bool WinEdit::startApplication( HINSTANCE, const char *cmdLine )
{
	doEnableLogEx(gakLogging::llInfo);
	doEnterFunctionEx(gakLogging::llInfo, "WinEdit::startApplication" );
	doLogValueEx( gakLogging::llInfo, cmdLine );

	STRING				applicationPath = application.getFileName();
	STRING				appFile = getFile( applicationPath );

	for( size_t i=0; i<applicationPath.strlen(); i++ )
	{
		if( isspace(applicationPath[i] ) )
		{
			applicationPath.insStr( 0, "\"" );
			applicationPath += '\"';
/*v*/		break;
		}
	}
	FOR_EACH( i, fTypes, 
	{
		registerFileType( applicationPath, appFile, i );
	});

	setApplication( APP_NAME );

	ForeignWindow window = ForeignWindow::findWindow("WinEdit_MDI_WIN");

	if( window.isValid() )
	{
		if( *cmdLine )
		{
			window.copyData( makeFullPath(cmdLine), LOAD_FILE );
		}

		window.focus();
/***/	return true;	// terminate application
	}

	return false;
}

CallbackWindow *WinEdit::createMainWindow( const char *cmdLine, int nCmdShow )
{
	doEnterFunctionEx( gakLogging::llInfo, "WinEdit::createMainWindow" );
	doLogValueEx( gakLogging::llInfo, cmdLine );
	STRING fileName = makeFullPath(cmdLine);
	doLogValueEx( gakLogging::llInfo, fileName );

	std::unique_ptr<WinEditMain> mainWindow(new WinEditMain);
	if( mainWindow->create( nCmdShow ) == scERROR )
	{
		MessageBox( nullptr, "Cannot Create Window", "Error", MB_ICONSTOP|MB_OK );
/*@*/	return nullptr;
	}

	if( *cmdLine )
		mainWindow->openFile( fileName );

	return mainWindow.release();
}

void WinEdit::deleteMainWindow( BasicWindow  *mainWindow )
{
	delete mainWindow;
}

// --------------------------------------------------------------------- //
// ----- class publics ------------------------------------------------- //
// --------------------------------------------------------------------- //

// --------------------------------------------------------------------- //
// ----- entry points -------------------------------------------------- //
// --------------------------------------------------------------------- //
