/*
		Project:		WINEDIT (Windows MDI-Editor)
		Module: 		WINEDIT.CPP
		Description:	The Mainprogram
		Author:			Martin Gäckler
		Address:		HoFmannsthalweg 14, A-4030 Linz
		Web:			https://www.gaeckler.at/

		Copyright:		(c) 1988-2024 Martin Gäckler

		This program is free software: you can redistribute it and/or modify  
		it under the terms of the GNU General Public License as published by  
		the Free Software Foundation, version 3.

		You should have received a copy of the GNU General Public License 
		along with this program. If not, see <http://www.gnu.org/licenses/>.

		THIS SOFTWARE IS PROVIDED BY Martin Gäckler, Austria, Linz ``AS IS''
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

#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include <io.h>

#include <gak/xml.h>
#include <gak/directory.h>

#include <winlib/winapp.h>
#include <winlib/mdiwin.h>
#include <winlib/editchld.h>
#include <winlib/dialogwi.h>
#include <winlib/stddlg.h>
#include <winlib/f_type.h>

#include "winedit.h"

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

// --------------------------------------------------------------------- //
// ----- constants ----------------------------------------------------- //
// --------------------------------------------------------------------- //

#define MAX_FILE_LIST	10
#define MIN_FILE_ID		1000
#define WM_LOAD_FILE	WM_USER

// --------------------------------------------------------------------- //
// ----- class definitions --------------------------------------------- //
// --------------------------------------------------------------------- //

class PreferencesDlg : public ModalDialog
{
	EditControl	tabwidth;
	CheckBox	backup;
	CheckBox	autoIndent;

	private:
	virtual ProcessStatus handleCreate( void );
	virtual ProcessStatus handleOk( void );
};

class AboutDlg : public ModalDialog
{
	Label	m_addressWidthLabel;

	private:
	virtual ProcessStatus handleCreate( void );
};

class WinEditChild : public EditChild
{
	virtual void handleFocus( void );
	virtual SuccessCode handleClose( void );

	public:
	void htmlEntities( void );
};

class WinEditMain : public MDIwindow
{
	bool				lastWholeWord;
	bool				lastMatchCase;
	bool				lastDownSearch;
	STRING				lastSearchFor;
	STRING				lastReplace;
	char				*lastFileList[MAX_FILE_LIST];
	EDIT::editCharset	lastCharsets[MAX_FILE_LIST];
	size_t				numFiles;
	WORD				lastLineMenuItemChecked;
	Font				theFont;

	private:
	void createBackup( const STRING &fileName );
	void openFile( void );
	void saveFile( void );
	void saveAllFiles( void );
	void saveFileAs( void );
	void changeCharset( EDIT::editCharset newCharset );
	void htmlEntities( void );
	void closeAll( void );

	void changeLineEnd( RLINE_ENDS newLineEnd );

	void preferences( void );
	void selectFont( void );
	void print( void );

	void undo( void );
	void cut( void );
	void copy( void );
	void paste( void );
	void clear( void );
	void selectAll( void );
	void deleteLine( void );
	void indent( void );
	void unindent( void );
	void lineBreak( void );

	virtual void handleFind( const char *, bool, bool, bool );
	virtual void handleReplace( const char *, const char *, bool, bool, bool );
	void searchText( void );
	void replaceText( void );
	void searchNext( void );
	void compareFiles( void );

	private:
	void loadFileList( void );
	void saveFileList( void );
	void addFileEntry( size_t id );
	void removeFileEntry( size_t id );
	void addFile( const char *fileName, EDIT::editCharset charset );

	virtual SuccessCode handleClose( void );
	virtual ProcessStatus handleCommand( int command );
	virtual void handleFile( const char *, size_t, size_t );
	virtual void handleFocus( void );
	virtual void handleTimer( void );
	virtual ProcessStatus handleMessage( UINT, WPARAM, LPARAM );

	virtual ProcessStatus ddeInitiate( const char *app, const char *topic );
	virtual ProcessStatus ddeExecute( const char *command );


	public:
	WinEditMain();
	~WinEditMain();

	SuccessCode create( int nCmdShow );
	void openFile( const char *fileName, EDIT::editCharset charSet );
	void checkLineEnd( RLINE_ENDS lineEnd );
};

class WinEdit : public Application
{
	WinEditMain		*mainWindow;

	virtual bool 			startApplication( HINSTANCE hInstance, const char *cmdLine );
	virtual CallbackWindow	*createMainWindow( const char *cmdLine, int nCmdShow );
	virtual void deleteMainWindow( BasicWindow  *mainWindow );

	public:
	WinEdit() : Application( "EDIT_ICON" ) {}
};

// --------------------------------------------------------------------- //
// ----- module statics ------------------------------------------------ //
// --------------------------------------------------------------------- //

static WinEdit	application;

static char appName[] = "WinEdit";


struct
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

static STRING getDir( STRING pathName )
{
	size_t	i, slashPos;
	bool  	inString = false;

	// find the first blank that delimits params from command
	for( i=0; i<pathName.strlen(); i++ )
		if( pathName[i] == '\"' )
			inString = !inString;
		else if( !inString && pathName[i] == ' ' )
/*v*/		break;

	pathName.cut( i-1 );	// remove parameters

	// find the last slash
	slashPos = pathName.searchRChar('\\');

	if( slashPos != (size_t)-1 )
		pathName.cut(slashPos);

	if( pathName[0U] == '\"' )
		pathName.delStr( 0, 1 );

	return pathName.upperCase();
}

// --------------------------------------------------------------------- //
// ----- class constructors/destructors -------------------------------- //
// --------------------------------------------------------------------- //

WinEditMain::WinEditMain() : MDIwindow( NULL ), theFont( this )
{
	int	i;

	lastWholeWord = lastMatchCase = false;
	lastDownSearch = true;
	lastLineMenuItemChecked = 0;

	for( i=0; i<MAX_FILE_LIST; i++ )
	{
		lastFileList[i] = NULL;
		lastCharsets[i] = EDIT::CS_ANSI;
	}

	numFiles = 0;
}

WinEditMain::~WinEditMain()
{
	int i;

	for( i=0; i<MAX_FILE_LIST; i++ )
		if( lastFileList[i] )
			free( lastFileList[i] );
}

// --------------------------------------------------------------------- //
// ----- class inlines ------------------------------------------------- //
// --------------------------------------------------------------------- //

inline SuccessCode WinEditMain::create( int nCmdShow )
{
	setText( appName );
	if( MDIwindow::create( MAIN_MENU, WINDOW_MENU ) == scERROR )
		return scERROR;

	setAccelerators( MAIN_MENU );

	acceptFiles();

	addStatusBar();
	setStatusText( "Welcome" );
	setTimer( 500 );

	restoreWindowRect( "mainWindow", nCmdShow );

//	hideVertScrollBar();
//	hideHorizScrollBar();

	loadFileList();

	return scSUCCESS;
}

// --------------------------------------------------------------------- //
// ----- class privates ------------------------------------------------ //
// --------------------------------------------------------------------- //

void WinEditMain::loadFileList( void )
{
	doEnterFunction("WinEditMain::loadFileList");

	long 				i, numFiles;
	char				profileEntry[32];
	STRING				fileName;
	EDIT::editCharset	charset;

	for( i=0; i<MAX_FILE_LIST; i++ )
	{
		sprintf( profileEntry, "lastFile%d", i );
		fileName = application.GetProfile( "", profileEntry, (const char *)0 );
		if( !fileName[0U] )
			break;

		sprintf( profileEntry, "lastCharset%d", i );
		charset = (EDIT::editCharset)application.GetProfile( "", profileEntry, EDIT::CS_ANSI );

		if( !access( fileName, 0 ) )
			addFile( fileName, charset );
	}

	numFiles = application.GetProfile( "", "OpenFiles", 0 );
	for( i=0; i<numFiles; i++ )
	{
		sprintf( profileEntry, "openFile%d", i );

		fileName = application.GetProfile( "", profileEntry, (const char *)0 );
		if( !fileName[0U] )
			break;

		sprintf( profileEntry, "charset%d", i );
		charset = (EDIT::editCharset)application.GetProfile( "", profileEntry, (long)EDIT::CS_ANSI );
		openFile( fileName, charset );
	}
}

void WinEditMain::saveFileList( void )
{
	size_t 			i;
	char			profileEntry[32];
	STRING			fileName;

	for( i=0; i<numFiles; i++ )
	{
		sprintf( profileEntry, "lastFile%d", (int)i );
		application.WriteProfile( false, "", profileEntry, lastFileList[i] );

		sprintf( profileEntry, "lastCharset%d", (int)i );
		application.WriteProfile( false, "", profileEntry, lastCharsets[i] );
	}

	i=0;
	WinEditChild	*editor = static_cast<WinEditChild *>(getFirstChild());
	while( editor )
	{
		fileName = editor->getFileName();
		if( fileName[0U] )
		{
			sprintf( profileEntry, "openFile%d", (int)i );
			application.WriteProfile( false, "", profileEntry, fileName );
			sprintf( profileEntry, "charset%d", (int)i );
			application.WriteProfile( false, "", profileEntry, editor->getCharset() );
			i++;
		}

		editor = static_cast<WinEditChild *>(getNextChild());
	}

	application.WriteProfile( false, "", "OpenFiles", long(i) );
}

void WinEditMain::addFileEntry( size_t id )
{
	char	menuEntry[1024];
	size_t	menuMnemonic = id+1;

	if( menuMnemonic >= 10 )
		sprintf( menuEntry, "%d&%d %s", int(menuMnemonic/10), int(menuMnemonic%10), lastFileList[id] );
	else
		sprintf( menuEntry, "&%d %s", int(menuMnemonic), lastFileList[id] );

	getMenu()->addMenuItem( 0, int(MIN_FILE_ID+id), menuEntry );
}

void WinEditMain::removeFileEntry( size_t id )
{
	size_t	i;

	getMenu()->removeMenuItem( 0, int(MIN_FILE_ID+id) );
	free( lastFileList[id] );

	for( i=id+1; i<numFiles; i++ )
	{
		getMenu()->removeMenuItem( 0, int(MIN_FILE_ID+i) );
		lastFileList[i-1] = lastFileList[i];
		lastCharsets[i-1] = lastCharsets[i];
		addFileEntry( i-1 );
	}

	numFiles--;
}

void WinEditMain::addFile( const char *fileName, EDIT::editCharset charset )
{
	doEnterFunction("WinEditMain::addFile");
	size_t	i;
	char	*newFile = strdup( fileName );

	for( i=0; i<numFiles; i++ )
		if( !strcmp( lastFileList[i], newFile ) )
		{
			removeFileEntry( i );
			break;
		}

	if( numFiles >= MAX_FILE_LIST )
		removeFileEntry( 0 );

	lastFileList[numFiles] = newFile;
	lastCharsets[numFiles] = charset;
	addFileEntry( numFiles );
	numFiles++;
}

void WinEditMain::createBackup( const STRING &fileName )
{
	bool	backupFlag;
	STRING	backupFile;
	size_t	slash;
	size_t	dot;

	backupFlag = application.GetProfile( "", "backup", true );
	if( backupFlag )
	{
		backupFile = fileName;

		dot = backupFile.searchRChar( '.' );
		slash = backupFile.searchRChar( '\\' );

		if( dot!=(size_t)-1 && dot > slash )
			backupFile.cut( dot );

		backupFile += ".bak";

		if( !access( backupFile, 0 ) )
			::remove( backupFile );
		rename( fileName, backupFile );
	}
}

void WinEditMain::openFile( void )
{
	OpenFileDialog	fileDlg;
	char			fileName[1024];

	STRING			directory;

	// find default directory
	getcwd( fileName, sizeof( fileName ) );
	directory = fileName;
	directory = application.GetProfile( NULL,"directory", directory );
	setcwd( directory );

	// restore last filter used
	fileDlg.setFilterIndex( application.GetProfile( NULL, "filterIndex", 1 ) );
	fileDlg.setFilename( fileName );
	if( fileDlg.create( this,
						0,
						C_SOURCE_FILES, ALL_FILES - C_SOURCE_FILES +1 )
	)
	{
		application.WriteProfile( false, NULL, "filterIndex", fileDlg.getFilterIndex() );
		openFile( fileDlg.getFilename(), EDIT::CS_ANSI );
	}

	// save default directory
	getcwd( fileName, sizeof( fileName ) );
	directory = fileName;
	application.WriteProfile( false, NULL, "directory", directory );
}

void WinEditMain::saveFile( void )
{
	STRING			fileName;

	WinEditChild	*editor = static_cast<WinEditChild *>(getActiveChild());
	if( editor && editor->isChanged() )
	{
		fileName = editor->getFileName();
		if( editor->isEditable() && fileName[(size_t)0] )
		{
			createBackup( fileName );
			editor->saveToFile();
		}
		else
			saveFileAs();
	}
}

void WinEditMain::saveAllFiles( void )
{
	STRING			fileName;

	WinEditChild	*editor = static_cast<WinEditChild *>(getFirstChild());
	while( editor )
	{
		fileName = editor->getFileName();
		if( editor->isEditable() && editor->isChanged() )
		{
			if( fileName[(size_t)0] )
			{
				createBackup( fileName );
				editor->saveToFile();
			}
			else
				saveFileAs();
		}

		editor = static_cast<WinEditChild *>(getNextChild());
	}
}

void WinEditMain::saveFileAs( void )
{
	SaveFileAsDialog	saveDialog;
	char				fileName[1024];

	WinEditChild	*editor = static_cast<WinEditChild *>(getActiveChild());
	if( editor )
	{
		STRING tmpName = editor->getFileName();
		tmpName = getDir( tmpName );
		if( !tmpName.isEmpty() )
			setcwd( tmpName );

		tmpName = editor->getFileName();
		if( !tmpName.isEmpty() )
			strcpy( fileName, tmpName );
		else
			*fileName = 0;

		saveDialog.setFilterIndex( application.GetProfile( NULL, "filterIndex", 1 ) );

		saveDialog.setFilename( fileName );
		if( saveDialog.create( this,
								0,
								C_SOURCE_FILES, ALL_FILES - C_SOURCE_FILES +1 ) )
		{
			editor->saveToFile( saveDialog.getFilename() );
			addFile( saveDialog.getFilename(), editor->getCharset() );
			application.WriteProfile( false, NULL, "filterIndex", saveDialog.getFilterIndex() );
		}
	}
}

void WinEditMain::changeCharset( EDIT::editCharset newCharset )
{
	doEnterFunction("WinEditMain::changeCharset");

	EDIT::editCharset	oldCharset;

	WinEditChild	*editor = static_cast<WinEditChild *>(getActiveChild());
	if( editor )
	{
		oldCharset = editor->getCharset();
		if( oldCharset != newCharset )
		{
			SHORT	kState = GetKeyState( VK_SHIFT );
			kState >>= 8;
			if( kState )
				editor->convertCharset( newCharset );
			else
				editor->setCharset( newCharset );

			if( oldCharset == EDIT::CS_OEM )
				getMenu()->uncheckMenuItem( EDIT_OEM_CONVERT );
			else if( oldCharset == EDIT::CS_ANSI )
				getMenu()->uncheckMenuItem( EDIT_CS_ANSI );
			else if( oldCharset == EDIT::CS_UTF8 )
				getMenu()->uncheckMenuItem( EDIT_CS_UTF8 );

			if( newCharset == EDIT::CS_OEM )
				getMenu()->checkMenuItem( EDIT_OEM_CONVERT );
			else if( newCharset == EDIT::CS_ANSI )
				getMenu()->checkMenuItem( EDIT_CS_ANSI );
			else if( newCharset == EDIT::CS_UTF8 )
				getMenu()->checkMenuItem( EDIT_CS_UTF8 );

			STRING theFileName = editor->getFileName();
			for( int i=0; i<MAX_FILE_LIST; i++ )
			{
				if( !strcmpi( theFileName, lastFileList[i] ) )
				{
					lastCharsets[i] = newCharset;
					break;
				}
			}

		}
	}
}

void WinEditMain::htmlEntities( void )
{
	WinEditChild	*editor = static_cast<WinEditChild *>(getActiveChild());
	if( editor && editor->isEditable() )
		editor->htmlEntities();
}


void WinEditMain::changeLineEnd( RLINE_ENDS newLineEnd )
{
	WinEditChild	*editor = static_cast<WinEditChild *>(getActiveChild());
	if( editor && editor->isEditable() )
	{
		editor->setLineEnd( newLineEnd );
		checkLineEnd( newLineEnd );
	}
}

void WinEditMain::print( void )
{
	PrinterDialog	dlg;
	PrintDevice		*printer;

	WinEditChild	*editor = static_cast<WinEditChild *>(getActiveChild());
	if( editor )
	{
		if( dlg.create( this ) )
		{
			printer = dlg.getPrinter();
			editor->print( printer );
			delete printer;
		}
	}
}

void WinEditMain::closeAll( void )
{
	STRING			fileName;

	WinEditChild	*editor = static_cast<WinEditChild *>(getFirstChild());
	while( editor )
	{
		editor->message( WM_CLOSE );
		editor = static_cast<WinEditChild *>(getNextChild());
	}
}


void WinEditMain::preferences( void )
{
	PreferencesDlg	dialog;
	unsigned int	tabWidth;

	if( dialog.create( FILE_PREFERENCES, this  ) )
	{
		bool autoIndentFlag = application.GetProfile( "", "autoIndent", true );
		tabWidth = (unsigned int)application.GetProfile( "", "tabWidth", 4 );
		WinEditChild	*editor = static_cast<WinEditChild *>(getFirstChild());
		if( editor ) do
		{
			editor->setTabWidth( tabWidth );
			if( autoIndentFlag )
				editor->setAutoIndent();
			else
				editor->clrAutoIndent();
			editor = static_cast<WinEditChild *>(getNextChild());
		} while( editor );
	}
}

void WinEditMain::selectFont( void )
{
	if( theFont.selectFont( this, true, CF_FIXEDPITCHONLY	) )
	{
		application.WriteProfile( false, "", "Font", theFont.getFontName() );
		application.WriteProfile( false, "", "FontSize", theFont.getFontSize() );
		application.WriteProfile( false, "", "Style", theFont.getStyle() );
		WinEditChild	*editor = static_cast<WinEditChild *>(getFirstChild());
		if( editor ) do
		{
			editor->setFont( theFont );
			editor = static_cast<WinEditChild *>(getNextChild());
		} while( editor );
	}
}

void WinEditMain::undo( void )
{
	WinEditChild	*editor = static_cast<WinEditChild *>(getActiveChild());
	if( editor )
	{
		editor->undo();
	}
}

void WinEditMain::cut( void )
{
	WinEditChild	*editor = static_cast<WinEditChild *>(getActiveChild());
	if( editor )
	{
		editor->cut();
	}
}

void WinEditMain::copy( void )
{
	WinEditChild	*editor = static_cast<WinEditChild *>(getActiveChild());
	if( editor )
	{
		editor->copy();
	}
}

void WinEditMain::paste( void )
{
	WinEditChild	*editor = static_cast<WinEditChild *>(getActiveChild());
	if( editor )
	{
		editor->paste();
	}
}

void WinEditMain::clear( void )
{
	WinEditChild	*editor = static_cast<WinEditChild *>(getActiveChild());
	if( editor )
	{
		editor->clear();
	}
}

void WinEditMain::selectAll( void )
{
	WinEditChild	*editor = static_cast<WinEditChild *>(getActiveChild());
	if( editor )
	{
		editor->selectAll();
	}
}

void WinEditMain::deleteLine( void )
{
	WinEditChild	*editor = static_cast<WinEditChild *>(getActiveChild());
	if( editor )
	{
		editor->deleteLine();
	}
}

void WinEditMain::indent( void )
{
	WinEditChild	*editor = static_cast<WinEditChild *>(getActiveChild());
	if( editor && editor->isEditable() )
	{
		editor->indent();
	}
}

void WinEditMain::unindent( void )
{
	WinEditChild	*editor = static_cast<WinEditChild *>(getActiveChild());
	if( editor )
	{
		editor->unindent();
	}
}

void WinEditMain::lineBreak( void )
{
	WinEditChild	*editor = static_cast<WinEditChild *>(getActiveChild());
	if( editor )
	{
		if( editor->isLineBreak() )
		{
			editor->clrLineBreak();
			getMenu()->uncheckMenuItem( EDIT_BREAK );
		}
		else
		{
			editor->setLineBreak();
			getMenu()->checkMenuItem( EDIT_BREAK );
		}
	}
}

void WinEditMain::searchText( void )
{
	SearchDialog	*searchDialog;
	STRING			selection;

	WinEditChild	*editor = static_cast<WinEditChild *>(getActiveChild());
	if( editor )
	{
		selection = editor->getSelection();
	}

	if( selection.isEmpty() )
	{
		selection = lastSearchFor;
	}
	else
	{
		lastSearchFor = selection;
	}

	searchDialog = SearchDialog::create();
	if( searchDialog )
	{
		searchDialog->create( this, selection, lastWholeWord, lastMatchCase, lastDownSearch );
	}
}

void WinEditMain::replaceText( void )
{
	ReplaceDialog	*replaceDialog;
	STRING			selection;

	WinEditChild	*editor = static_cast<WinEditChild *>(getActiveChild());
	if( editor )
		selection = editor->getSelection();

	if( !strlen( selection ) )
		selection = lastSearchFor;
	else
		lastSearchFor = selection;

	replaceDialog = ReplaceDialog::create();
	if( replaceDialog )
		replaceDialog->create( this, selection, lastReplace, lastWholeWord, lastMatchCase );
}

void WinEditMain::searchNext( void )
{
	if( !lastReplace.isEmpty() )
		handleReplace( lastSearchFor, lastReplace,
						false, lastWholeWord, lastMatchCase );
	else if( !lastSearchFor.isEmpty() )
		handleFind( lastSearchFor, lastWholeWord, lastMatchCase, lastDownSearch );
}

void WinEditMain::compareFiles( void )
{
	STRING			fileName;
	STRING			cmdLine = "TDIFF ";

	WinEditChild	*editor = static_cast<WinEditChild *>(getActiveChild());
	if( editor )
	{
		fileName = editor->getFileName();
		cmdLine += fileName;
		cmdLine += " ";
	}

	editor = static_cast<WinEditChild *>(getNextChild());
	if( editor )
	{
		fileName = editor->getFileName();
		cmdLine += fileName;
		cmdLine += " ";
	}

	STRING tmp = getTempPath();
	cmdLine += tmp;

	cmdLine += DIRECTORY_DELIMITER_STRING "diff.txt";

	if( editor )
		WinExec( cmdLine, SW_NORMAL );
}

// --------------------------------------------------------------------- //
// ----- class virtuals ------------------------------------------------ //
// --------------------------------------------------------------------- //

ProcessStatus AboutDlg::handleCreate( void )
{
	m_addressWidthLabel.initHandle(this, INFO_ADRESS_WIDTH);
	m_addressWidthLabel.setText( sizeof(void*)==4 ? "32-Bit" : "64 Bit");
	return psPROCESSED;
}

ProcessStatus PreferencesDlg::handleCreate( void )
{
	bool	backupFlag, autoIndentFlag;

	tabwidth.initHandle( this, TABWIDTH );
	backup.initHandle( this, CREATE_BACKUP );
	autoIndent.initHandle( this, IDC_AUTO_INDENT );

	tabwidth = application.GetProfile( "", "tabWidth", "4" );

	backupFlag = application.GetProfile( "", "backup", true );
	if( backupFlag )
		backup.setActive();
	else
		backup.clrActive();

	autoIndentFlag = application.GetProfile( "", "autoIndent", true );
	if( autoIndentFlag )
		autoIndent.setActive();
	else
		autoIndent.clrActive();

	return psPROCESSED;
}

ProcessStatus PreferencesDlg::handleOk( void )
{
	bool	backupFlag, autoIndentFlag;
	STRING	tabWidthStr = tabwidth;

	application.WriteProfile( false, "", "tabWidth", tabWidthStr );

	backupFlag = backup.isActive();
	application.WriteProfile( false, "", "backup", backupFlag );

	autoIndentFlag = autoIndent.isActive();
	application.WriteProfile( false, "", "autoIndent", autoIndentFlag );

	return psDO_DEFAULT;
}

void WinEditChild::handleFocus( void )
{
	WinEditMain	*mainWindow = (WinEditMain *)getMDIparent();

	enum EDIT::editCharset charSet = getCharset();

	if( charSet == EDIT::CS_OEM )
		mainWindow->getMenu()->checkMenuItem( EDIT_OEM_CONVERT );
	else
		mainWindow->getMenu()->uncheckMenuItem( EDIT_OEM_CONVERT );

	if( charSet == EDIT::CS_ANSI )
		mainWindow->getMenu()->checkMenuItem( EDIT_CS_ANSI );
	else
		mainWindow->getMenu()->uncheckMenuItem( EDIT_CS_ANSI );

	if( charSet == EDIT::CS_UTF8 )
		mainWindow->getMenu()->checkMenuItem( EDIT_CS_UTF8 );
	else
		mainWindow->getMenu()->uncheckMenuItem( EDIT_CS_UTF8 );

	if( isLineBreak() )
		mainWindow->getMenu()->checkMenuItem( EDIT_BREAK );
	else
		mainWindow->getMenu()->uncheckMenuItem( EDIT_BREAK );

	mainWindow->checkLineEnd( getLineEnd() );

	ManagerChild::handleFocus();

	if( checkModifyTime() )
	{
		storeModifyTime();
		if(
			messageBox(
				"File Changed? Do you want to reload?",
				"Win Edit",
				MB_YESNO
			) == IDYES )
		{
			reloadFile();
		}
	}
}

SuccessCode WinEditChild::handleClose( void )
{
	int			button;
	STRING		theText;
	STRING		theFile;
	char		text_buff[1024];

	if( isChanged() )
	{
		theText = application.loadString( CLOSE_ANYWAY );
		theFile = getFileName();
		sprintf( text_buff, theText, (const char *)theFile ? (const char *)theFile : "untitled" );
		button = messageBox( text_buff, WARNING,
					MB_APPLMODAL|MB_ICONSTOP|MB_YESNOCANCEL );

		if( button == IDCANCEL )
			return scERROR;

		if( button == IDYES )
		{
			saveToFile();
			if( isChanged() )
				return scERROR;
		}
	}

	close();
	return scSUCCESS;
}

void WinEditMain::handleFocus( void )
{
	WinEditChild	*editor = static_cast<WinEditChild *>(getActiveChild());
	if( editor )
		editor->focus();
}

SuccessCode WinEditMain::handleClose( void )
{
	saveFileList();
	saveWindowRect( "mainWindow" );
	return MDIwindow::handleClose();
}

ProcessStatus WinEditMain::handleCommand( int command )
{
	switch( command )
	{
	case FILE_NEW:
		openFile( NULL, EDIT::CS_ANSI );	break;
	case FILE_OPEN:
		openFile();							break;
	case FILE_SAVE:
		saveFile();							break;
	case FILE_SAVE_ALL:
		saveAllFiles();						break;

	case FILE_SAVE_AS:
		saveFileAs();						break;

	case FILE_PRINT:
		print();							break;

	case FILE_PREFERENCES:
		preferences();						break;

	case FILE_FONT:
		selectFont();						break;

	case FILE_QUIT:
		handleClose();						break;

	case EDIT_UNDO:
		undo();								break;
	case EDIT_CUT:
		cut();								break;
	case EDIT_COPY:
		copy();								break;
	case EDIT_PASTE:
		paste();							break;
	case EDIT_CLEAR:
		clear();							break;
	case EDIT_SELECT_ALL:
		selectAll();						break;
	case EDIT_DELETE_LINE:
		deleteLine();						break;
	case EDIT_INDENT:
		indent();							break;
	case EDIT_UNINDENT:
		unindent();							break;
	case EDIT_BREAK:
		lineBreak();						break;
	case EDIT_OEM_CONVERT:
		changeCharset( EDIT::CS_OEM );		break;
	case EDIT_CS_ANSI:
		changeCharset( EDIT::CS_ANSI );		break;
	case EDIT_CS_UTF8:
		changeCharset( EDIT::CS_UTF8 );		break;
	case EDIT_HTML_ENTITIES:
		htmlEntities();						break;

	case EDIT_LINE_CR:
		changeLineEnd(RL_END_CR);			break;
	case EDIT_LINE_CRLF:
		changeLineEnd(RL_END_CRLF);			break;
	case EDIT_LINE_LF:
		changeLineEnd(RL_END_LF);			break;
	case EDIT_LINE_LFCR:
		changeLineEnd(RL_END_LFCR);			break;

	case SEARCH_SEARCH:
		searchText();						break;
	case SEARCH_REPLACE:
		replaceText();						break;
	case SEARCH_NEXT:
		searchNext();						break;
	case COMPARE_FILES:
		compareFiles();						break;

	case WIN_TILE_HORIZ:
		tileChildren();						break;
	case WIN_TILE_VERT:
		tileChildren( false );				break;
	case WIN_CASCADE:
		cascadeChildren();					break;
	case WIN_ARRANGE:
		arrangeIcons();						break;
	case WIN_CLOSE_ALL:
		closeAll();							break;

	case INFO_ABOUT:
		{
			AboutDlg	dlg;
			dlg.create(command, this );
		}

	default:
		if( command >= MIN_FILE_ID
		&&  command < MIN_FILE_ID+MAX_FILE_LIST
		&& lastFileList[command-MIN_FILE_ID] )
		{
			openFile( lastFileList[command-MIN_FILE_ID], lastCharsets[command-MIN_FILE_ID] );
			break;
		}

		return psDO_DEFAULT;
	}
	return psPROCESSED;
}

void WinEditMain::handleFile( const char *fileName, size_t, size_t )
{
	openFile( fileName, EDIT::CS_ANSI );
}

void WinEditMain::handleFind( const char *text, bool wholeWord, bool matchCase, bool downSearch )
{
	lastSearchFor = text;
	lastWholeWord = wholeWord;
	lastMatchCase = matchCase;
	lastDownSearch = downSearch;
	lastReplace   = (char *)NULL;

	WinEditChild	*editor = static_cast<WinEditChild *>(getActiveChild());
	if( editor )
	{
		editor->searchText( 
			lastSearchFor, 
			downSearch ? END_SELECTION 
					   : BEGIN_SELECTION, 
			wholeWord, matchCase, downSearch 
		);
	}
}

void WinEditMain::handleReplace( const char *text, const char *replace,
									bool all, bool wholeWord, bool matchCase )
{
	lastSearchFor = text;
	lastReplace   = replace;
	lastWholeWord = wholeWord;
	lastMatchCase = matchCase;

	WinEditChild	*editor = static_cast<WinEditChild *>(getActiveChild());
	if( editor && editor->isEditable() )
	{
		editor->searchText(
			lastSearchFor,
			BEGIN_SELECTION,
			wholeWord, matchCase, lastDownSearch,
			lastReplace,
			all
		);
	}
}

ProcessStatus WinEditMain::ddeInitiate( const char *application, const char *topic )
{
	if( !strcmpi( application, appName )
	&&  !strcmpi( topic, "SYSTEM" ) )
		return psPROCESSED;
	return psDO_DEFAULT;
}

ProcessStatus WinEditMain::ddeExecute( const char *commandPtr )
{
	if( !strncmpi( commandPtr, "open ", sizeof( "open " ) -1 ) )
	{
		openFile( commandPtr + sizeof( "open" ), EDIT::CS_ANSI );
		return psPROCESSED;
	}

	return psDO_DEFAULT;
}

void WinEditMain::handleTimer( void )
{
	static size_t	lastLine = -1;
	static size_t	lastColumn = -1;
	static bool		lastModified = false;

	size_t			line, column;
	bool			modified;
	char			newStatus[128];

	WinEditChild	*editor = static_cast<WinEditChild *>(getActiveChild());
	if( editor )
	{
		editor->getCursorPosition( &line, &column );
		modified = editor->isChanged();

		if( lastLine != line
		|| lastColumn != column
		|| lastModified != modified )
		{
			sprintf(
				newStatus,
				"%ld:%ld %s",
				(long)line, (long)column,
				modified ? "modified" : ""
			);
			setStatusText( newStatus );

			if( modified )
				getMenu()->enableMenuItem( FILE_SAVE );
			else
				getMenu()->disableMenuItem( FILE_SAVE );

			lastLine = line;
			lastColumn = column;
			lastModified = modified;
		}
	}
	else
	{
		setStatusText( "Welcome" );
		lastLine = lastColumn = -1;
		lastModified = false;
	}
}

ProcessStatus WinEditMain::handleMessage( UINT msg, WPARAM, LPARAM atom)
{
	char	fileName[512];

	if( msg == WM_LOAD_FILE )
	{
		GlobalGetAtomName( (ATOM)atom, fileName, sizeof( fileName ) );
		handleFile( (const char *)fileName, 0, 0 );
		GlobalDeleteAtom((ATOM)atom);
		return psPROCESSED;
	}
	return psDO_DEFAULT;
}

bool WinEdit::startApplication( HINSTANCE, const char *cmdLine )
{
	doEnableLogEx(gakLogging::llInfo);

	FileTypeRegistry	fType;
	size_t				i;
	STRING				applicationPath = application.getFileName();
	STRING				appFile = getFile( applicationPath );
	int					regType;

	for( i=0; i<applicationPath.strlen(); i++ )
	{
		if( isspace(applicationPath[i] ) )
		{
			applicationPath.insStr( 0, "\"" );
			applicationPath += '\"';
/*v*/		break;
		}
	}

	for( i=0; i<sizeof(fTypes)/sizeof(fTypes[0]); i++ )
	{
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

			if( !fType.type_description[0U] )
				fType.type_description = fTypes[i].description;

			fType.cmd = "open",
			fType.cmd_description = "&Open";

			fType.commandLine = applicationPath;

			fType.ddeCommand = "open %1",
			fType.ddeIfExec = "",
			fType.application = appName,
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
			fType.application = appName,
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
			fType.application = appName,
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

	setApplication( appName );

	ForeignWindow window = ForeignWindow::findWindow("WinEdit_MDI_WIN");

	if( window.isValid() )
	{
		if( *cmdLine )
		{
			ATOM	theAtom = GlobalAddAtom( cmdLine );
			window.message( WM_LOAD_FILE, 0, (LPARAM)theAtom );
		}

		window.focus();
/***/	return true;	// terminate application
	}

	return false;
}

CallbackWindow *WinEdit::createMainWindow( const char *cmdLine, int nCmdShow )
{
	STRING fileName = cmdLine;

	mainWindow = new WinEditMain;
	if( mainWindow->create( nCmdShow ) == scERROR )
	{
		MessageBox( NULL, "Cannot Create Window", "Error", MB_ICONSTOP|MB_OK );
/*@*/	return NULL;
	}

	if( !fileName.isEmpty() )
		mainWindow->openFile( fileName, EDIT::CS_ANSI );

	return mainWindow;
}

void WinEdit::deleteMainWindow( BasicWindow  *mainWindow )
{
	delete mainWindow;
}

// --------------------------------------------------------------------- //
// ----- class publics ------------------------------------------------- //
// --------------------------------------------------------------------- //
void WinEditChild::htmlEntities( void )
{
	unsigned char	c;
	const char		*cp;
	bool			inTag = false;
	bool			inScript = false;
	bool			inSgml = false;
	bool			changedFlag = isChanged();
	STRING			entity, newText = "";
	STRING			text = getText();

	for( cp = text; (c=*cp) != 0; cp++ )
	{
		if( c== '<' )
		{
			newText += (char)c;

			if( !strncmp( cp, "<?", 2 ) )
				inSgml = true;
			else if( !strncmpi( cp, "<script", 7 ) )
			{
				inScript = true;
				inTag = true;
			}
			else if( !strncmpi( cp, "</script", 8 ) )
			{
				inScript = false;
				inTag = true;
			}
			else if( !inScript && !inSgml )
				inTag = true;
		}
		else if( inSgml && c=='?' )
		{
			newText += (char)c;
			c = cp[1];
			if( c == '>' )
			{
				newText += (char)c;
				cp++;
				inSgml = false;
			}
		}
		else if( !inSgml && c=='>' )
		{
			inTag = false;
			newText += (char)c;
		}
		else if( inTag || inScript || inSgml )
		{
			newText += (char)c;
		}
		else if( c == '&' )
		{
			const char *cp2=cp+1;

			if( *cp2 == '#' )
			{
				for( cp2++; *cp2 && isdigit( *cp2 ); cp2++ )
					;
			}
			else
			{
				for( ; *cp2 && isalpha( *cp2 ); cp2++ )
					;
			}
			if( *cp2 != ';' )
			{
				newText += "&amp;";
				changedFlag = true;
			}
			else
				newText += '&';
		}
		else
		{
			entity = xml::Element::ascii2HTML( c );
			newText += entity;
			if( strlen(entity) > 1 )
				changedFlag = true;
		}
	}

	if( changedFlag )
	{
		setText( newText );
		setChanged();
	}
}

void WinEditMain::openFile( const char *iFileName, EDIT::editCharset charSet )
{
	doEnterFunction("WinEditMain::openFile");

	WinEditChild	*editor=NULL;
	STRING			curFileName;
	unsigned int	tabWidth;
	char			*theFileName = iFileName ? strdup( iFileName ) : NULL;
	bool			newWindow = false;

	focus();

	if( !theFont.isAssigned() )
	{
		theFont.setFixedFont();
		theFont.setFontName( application.GetProfile( "", "Font", "Fixedsys" ) );
		theFont.setFontSize( application.GetProfile( "", "FontSize", 9 ) );
		theFont.setStyle( application.GetProfile( "", "Style", 0 ) );
		theFont.create();
	}

	if( theFileName )
	{
		size_t	fileNameLen;

		if( theFileName[0] == '\"' )
			strcpy( theFileName, theFileName+1 );

		fileNameLen = strlen( theFileName );
		if( fileNameLen > 0 && theFileName[fileNameLen-1] == '\"' )
			theFileName[fileNameLen-1] = 0;
	}

	if( theFileName )
	{
		for( int i=0; i<MAX_FILE_LIST; i++ )
		{
			if( lastFileList[i] && !strcmpi( theFileName, lastFileList[i] ) )
			{
				charSet = lastCharsets[i];
				break;
			}
		}
		editor = static_cast<WinEditChild *>(getFirstChild());
		while( editor )
		{
			curFileName = editor->getFileName();
			if( curFileName == (STRING)theFileName )
				break;
			editor = static_cast<WinEditChild *>(getNextChild());
		}
	}

	if( !editor )
	{
		editor = new WinEditChild;
		editor->create( this, theFileName, 0, 0 );

		if( theFont.isAssigned() )
			editor->setFont( theFont );

		tabWidth = (unsigned int)application.GetProfile( "", "tabWidth", 4 );

		editor->setTabWidth( tabWidth );
		bool autoIndentFlag = application.GetProfile( "", "autoIndent", true );
		if( autoIndentFlag )
			editor->setAutoIndent();
		else
			editor->clrAutoIndent();

		newWindow = true;
	}
	else
		editor->focus();

	if( theFileName )
	{
		if( strAccess( theFileName, 02 ) )
			editor->setReadonly();

		if( newWindow || editor->isChanged() || editor->checkModifyTime() )
		{
			doLogPosition();
			editor->readFromFile( theFileName );
		}
		addFile( theFileName, charSet );
	}
	else
	{
		editor->setText( UNTITLED );
	}

	doLogValue( theFileName ? theFileName : "NULL" );
	doLogValue( charSet );
	editor->setCharset( charSet );

	doLogValue( sizeof(RLINE_ENDS) );
	checkLineEnd( editor->getLineEnd() );

	if( theFileName )
	{
		free( theFileName );
	}

	handleFocus();
}

void WinEditMain::checkLineEnd( RLINE_ENDS newLineEnd )
{
	WORD menuItem = 0;

	if( lastLineMenuItemChecked > 0 )
		getMenu()->uncheckMenuItem( lastLineMenuItemChecked );

	switch( newLineEnd )
	{
		case RL_END_CR:		menuItem = EDIT_LINE_CR;	break;
		case RL_END_LF:		menuItem = EDIT_LINE_LF;	break;
		case RL_END_CRLF:	menuItem = EDIT_LINE_CRLF;	break;
		case RL_END_LFCR:	menuItem = EDIT_LINE_LFCR;	break;
	}

	if( menuItem > 0 )
		getMenu()->checkMenuItem( menuItem );

	lastLineMenuItemChecked = menuItem;
}


