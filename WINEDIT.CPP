/*
		Project:		WINEDIT (Windows MDI-Editor)
		Module: 		WINEDIT.CPP
		Description:	The Mainprogram
		Author:			Martin Gäckler
		Address:		HoFmannsthalweg 14, A-4030 Linz
		Web:			https://www.gaeckler.at/

		Copyright:		(c) 1988-2025 Martin Gäckler

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

#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include <io.h>

#include <gak/xml.h>
#include <gak/directory.h>
#include <gak/ensemble.h>

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

namespace myProfile
{
	static const char autoIndent[] = "autoIndent";
	static const char autoReload[] = "autoReload";
	static const char backup[] = "backup";
	static const char charset[] = "charset";
	static const char directory[] = "directory";
	static const char filterIndex[] = "filterIndex";
	static const char Font[] = "Font";
	static const char FontSize[] = "FontSize";
	static const char lastCharset[] = "lastCharset";
	static const char lastFile[] = "lastFile";
	static const char filename[] = "filename";
	static const char openFile[] = "openFile";
	static const char OpenFiles[] = "OpenFiles";
	static const char selEnd[] = "selEnd";
	static const char selStart[] = "selStart";
	static const char Style[] = "Style";
	static const char tabWidth[] = "tabWidth";
};

// --------------------------------------------------------------------- //
// ----- class definitions --------------------------------------------- //
// --------------------------------------------------------------------- //

class PreferencesDlg : public ModalDialog
{
	EditControl	tabwidth;
	CheckBox	backup;
	CheckBox	autoIndent;
	CheckBox	autoReload;

	private:
	virtual ProcessStatus handleCreate( void );
	virtual ProcessStatus handleOk( void );
};

class AboutDlg : public ModalDialog
{
	Label	m_addressWidthLabel;

	private:
	virtual ProcessStatus handleCreate();
};

class WinEditChild : public EditChild
{
	virtual void handleFocus();
	virtual SuccessCode handleClose();

	public:
	void htmlEntities();
};

class WinEditMain : public MDIwindow
{
	bool				m_lastWholeWord;
	bool				m_lastMatchCase;
	bool				m_lastDownSearch;
	STRING				m_lastSearchFor;
	STRING				m_lastReplace;

	struct FileSpec
	{
		F_STRING			filename;
		EDIT::EditCharset	charset;
		size_t				selStart, selEnd;
	};

	FixedArray<FileSpec,MAX_FILE_LIST>		m_lastFiles;
	size_t				m_numFiles;
	WORD				m_lastLineMenuItemChecked;
	Font				m_theFont;

	private:
	void createBackup( const STRING &fileName );
	void openFile();
	void saveFile();
	void saveAllFiles();
	void saveFileAs();
	void changeCharset( EDIT::EditCharset newCharset );
	void htmlEntities();
	void closeAll();

	void changeLineEnd( RLINE_ENDS newLineEnd );

	void preferences();
	void selectFont();
	void print();

	void undo();
	void cut();
	void copy();
	void paste();
	void clear();
	void selectAll();
	void deleteLine();
	void indent();
	void unindent();
	void lineBreak();

	virtual void handleFind( const char *, bool, bool, bool );
	virtual void handleReplace( const char *, const char *, bool, bool, bool );
	void searchText();
	void replaceText();
	void searchNext();
	void compareFiles();

	private:
	void loadFileList();
	static void saveFileEntry(const char *baseName, unsigned u, const FileSpec &entry);
	void saveFileList();
	void addFileEntry( size_t id );
	void removeFileEntry( size_t id );
	void addFile( const STRING &fileName, EDIT::EditCharset charset, size_t selStart, size_t selEnd );

	virtual SuccessCode handleClose();
	virtual ProcessStatus handleCommand( int command );
	virtual void handleFile( const char *, size_t, size_t );
	virtual void handleFocus();
	virtual void handleTimer();
	virtual ProcessStatus handleMessage( UINT, WPARAM, LPARAM );

	virtual ProcessStatus ddeInitiate( const char *app, const char *topic );
	virtual ProcessStatus ddeExecute( const char *command );


	public:
	WinEditMain();
	~WinEditMain();

	SuccessCode create( int nCmdShow );
	void openFile( const STRING &fileName, EDIT::EditCharset charSet=EDIT::csANSI, size_t selStart=0, size_t selEnd=0 );
	void checkLineEnd( RLINE_ENDS lineEnd );
	void updateFileMenu( const STRING &fileName, size_t selStart, size_t selEnd );
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

WinEditMain::WinEditMain() : MDIwindow( NULL ), m_theFont( this )
{
	m_lastWholeWord = m_lastMatchCase = false;
	m_lastDownSearch = true;
	m_lastLineMenuItemChecked = 0;

	for( size_t i=0; i<MAX_FILE_LIST; i++ )
	{
		m_lastFiles[i].charset = EDIT::csANSI;
	}

	m_numFiles = 0;
}

WinEditMain::~WinEditMain()
{
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

void WinEditMain::loadFileList()
{
	doEnterFunction("WinEditMain::loadFileList");

	unsigned			numFiles;
	STRING				fileName;

	for( unsigned u=0; u<MAX_FILE_LIST; ++u )
	{
		STRING uStr = formatNumber(u);
		STRING lastFile = STRING(myProfile::lastFile) + uStr;

		fileName = application.GetProfile( lastFile, myProfile::filename, (const char *)0 );
		if( fileName.isEmpty() )
		{
			fileName = application.GetProfile( "", lastFile, (const char *)0 );
			if( fileName.isEmpty() )
			{
				break;
			}
			application.DeleteProfile(false, "", lastFile );
			application.WriteProfile(false, lastFile, myProfile::filename, fileName);
		}

		EDIT::EditCharset	charset = EDIT::EditCharset(application.GetProfile( lastFile, myProfile::charset, -1 ));
		if( charset == -1 )
		{
			STRING lastCharset =  STRING(myProfile::lastCharset) + uStr;
			charset = EDIT::EditCharset(application.GetProfile( "", lastCharset, -1 ));
			if( charset != -1 )
			{
				application.DeleteProfile(false, "", lastCharset );
				application.WriteProfile(false, lastFile, myProfile::charset, charset);
			}
			else
			{
				charset = EDIT::csANSI;
			}
		}


		if( !access( fileName, 0 ) )
		{
			size_t selStart = application.GetProfile( lastFile, myProfile::selStart, 0 );
			size_t selEnd = application.GetProfile( lastFile, myProfile::selEnd, 0 );
			addFile( fileName, charset, selStart, selEnd );
		}
	}

	bool foundLegacy = false;
	numFiles = application.GetProfile( "", myProfile::OpenFiles, 0 );
	for( unsigned u=0; u<numFiles; ++u )
	{
		STRING uStr = formatNumber(u);
		STRING openFileEntry =  STRING(myProfile::openFile) + uStr;

		fileName = application.GetProfile( openFileEntry, myProfile::filename, (const char *)0 );
		if( fileName.isEmpty() )
		{
			fileName = application.GetProfile( "", openFileEntry, (const char *)0 );
			if( fileName.isEmpty() )
				break;
			foundLegacy = true;
		}

		EDIT::EditCharset charset = EDIT::EditCharset(application.GetProfile( openFileEntry, myProfile::charset, (long)-1 ));
		if( charset == -1 )
		{
			STRING charsetEntry =  STRING(myProfile::charset) + uStr;
			charset = EDIT::EditCharset(application.GetProfile( "", charsetEntry, (long)-1 ));
			if( charset != -1 )
				foundLegacy = true;
			else
				charset = EDIT::csANSI;
		}
		size_t selStart = EDIT::EditCharset(application.GetProfile( openFileEntry, myProfile::selStart, 0 ));
		size_t selEnd = EDIT::EditCharset(application.GetProfile( openFileEntry, myProfile::selEnd, 0 ));
		openFile( fileName, charset, selStart, selEnd );
	}

	if( foundLegacy )
	{
		unsigned u=0;
		while( 1 )
		{
			STRING uStr = formatNumber(u++);
			STRING openFileEntry =  STRING(myProfile::openFile) + uStr;
			STRING charsetEntry =  STRING(myProfile::charset) + uStr;

			STRING fileName = application.GetProfile( "", openFileEntry, (const char *)0 );
			EDIT::EditCharset charset = EDIT::EditCharset(application.GetProfile( "", charsetEntry, (long)-1 ));

			application.DeleteProfile( false, "", openFileEntry );
			application.DeleteProfile( false, "", charsetEntry );
			if( fileName.isEmpty() && charset == -1 )
				break;
		}
	}
}

void WinEditMain::saveFileEntry(const char *baseName, unsigned u, const FileSpec &entry)
{
	STRING regName = STRING(baseName) + formatNumber(u);

	application.WriteProfile( false, regName, myProfile::filename, entry.filename );
	application.WriteProfile( false, regName, myProfile::charset, entry.charset );
	application.WriteProfile( false, regName, myProfile::selStart, entry.selStart );
	application.WriteProfile( false, regName, myProfile::selEnd, entry.selEnd );
}

void WinEditMain::saveFileList()
{
	STRING			fileName;

	for( unsigned u=0; u<m_numFiles; u++ )
	{
		saveFileEntry(myProfile::lastFile, u, m_lastFiles[u]);
	}

	unsigned u=0;
	WinEditChild	*editor = static_cast<WinEditChild *>(getFirstChild());
	while( editor )
	{
		fileName = editor->getFileName();
		if( !fileName.isEmpty() )
		{
			FileSpec openFile = {
				fileName, 
				editor->getCharset()
			};
			editor->getSelection(&openFile.selStart, &openFile.selEnd );
			saveFileEntry(myProfile::openFile, u++, openFile);
		}

		editor = static_cast<WinEditChild *>(getNextChild());
	}

	application.WriteProfile( false, "", myProfile::OpenFiles, long(u) );
}

void WinEditMain::addFileEntry( size_t id )
{
	STRING	menuEntry;
	size_t	menuMnemonic = id+1;

	if( menuMnemonic >= 10 )
		menuEntry = STRING('0' + char(menuMnemonic/10)) + '&' + ('0'+int(menuMnemonic%10));
	else
		menuEntry = STRING('&')+ ('0'+int(menuMnemonic));
	menuEntry += ' ';
	menuEntry += m_lastFiles[id].filename;

	getMenu()->addMenuItem( 0, int(MIN_FILE_ID+id), menuEntry );
}

void WinEditMain::removeFileEntry( size_t id )
{
	getMenu()->removeMenuItem( 0, int(MIN_FILE_ID+id) );

	for( size_t i=id+1; i<m_numFiles; i++ )
	{
		getMenu()->removeMenuItem( 0, int(MIN_FILE_ID+i) );
		m_lastFiles[i-1] = m_lastFiles[i];
		addFileEntry( i-1 );
	}

	--m_numFiles;
}

void WinEditMain::addFile( const STRING &fileName, EDIT::EditCharset charset, size_t selStart, size_t selEnd )
{
	doEnterFunction("WinEditMain::addFile");
	for( size_t i=0; i<m_numFiles; i++ )
	{
		if( m_lastFiles[i].filename == fileName )
		{
			removeFileEntry( i );
			break;
		}
	}

	if( m_numFiles >= MAX_FILE_LIST )
		removeFileEntry( 0 );

	m_lastFiles[m_numFiles].filename = fileName;
	m_lastFiles[m_numFiles].charset = charset;
	m_lastFiles[m_numFiles].selStart = selStart;
	m_lastFiles[m_numFiles].selEnd = selEnd;
	addFileEntry( m_numFiles );
	m_numFiles++;
}

void WinEditMain::createBackup( const STRING &fileName )
{
	bool	backupFlag;
	STRING	backupFile;
	size_t	slash;
	size_t	dot;

	backupFlag = application.GetProfile( "", myProfile::backup, true );
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

void WinEditMain::openFile()
{
	OpenFileDialog	fileDlg;
	char			fileName[1024];

	STRING			directory;

	// find default directory
	getcwd( fileName, sizeof( fileName ) );
	directory = fileName;
	directory = application.GetProfile( NULL, myProfile::directory, directory );
	setcwd( directory );

	// restore last filter used
	fileDlg.setFilterIndex( application.GetProfile( NULL, myProfile::filterIndex, 1 ) );
	fileDlg.setFilename( fileName );
	if( fileDlg.create( this,
						0,
						C_SOURCE_FILES, ALL_FILES - C_SOURCE_FILES +1 )
	)
	{
		application.WriteProfile( false, NULL, myProfile::filterIndex, fileDlg.getFilterIndex() );
		openFile( fileDlg.getFilename() );
	}

	// save default directory
	getcwd( fileName, sizeof( fileName ) );
	directory = fileName;
	application.WriteProfile( false, NULL, myProfile::directory, directory );
}

void WinEditMain::saveFile()
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

void WinEditMain::saveAllFiles()
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

void WinEditMain::saveFileAs()
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

		saveDialog.setFilterIndex( application.GetProfile( NULL, myProfile::filterIndex, 1 ) );

		saveDialog.setFilename( fileName );
		if( saveDialog.create( this,
								0,
								C_SOURCE_FILES, ALL_FILES - C_SOURCE_FILES +1 ) )
		{
			editor->saveToFile( saveDialog.getFilename() );
			size_t selStart, selEnd;
			editor->getSelection(&selStart, &selEnd);
			addFile( saveDialog.getFilename(), editor->getCharset(), selStart, selEnd );
			application.WriteProfile( false, NULL, myProfile::filterIndex, saveDialog.getFilterIndex() );
		}
	}
}

void WinEditMain::changeCharset( EDIT::EditCharset newCharset )
{
	doEnterFunction("WinEditMain::changeCharset");

	EDIT::EditCharset	oldCharset;

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

			if( oldCharset == EDIT::csOEM )
				getMenu()->uncheckMenuItem( EDIT_OEM_CONVERT );
			else if( oldCharset == EDIT::csANSI )
				getMenu()->uncheckMenuItem( EDIT_CS_ANSI );
			else if( oldCharset == EDIT::csUTF8 )
				getMenu()->uncheckMenuItem( EDIT_CS_UTF8 );

			if( newCharset == EDIT::csOEM )
				getMenu()->checkMenuItem( EDIT_OEM_CONVERT );
			else if( newCharset == EDIT::csANSI )
				getMenu()->checkMenuItem( EDIT_CS_ANSI );
			else if( newCharset == EDIT::csUTF8 )
				getMenu()->checkMenuItem( EDIT_CS_UTF8 );

			STRING theFileName = editor->getFileName();
			for( int i=0; i<MAX_FILE_LIST; i++ )
			{
				if( !strcmpi( theFileName, m_lastFiles[i].filename ) )
				{
					m_lastFiles[i].charset = newCharset;
					break;
				}
			}

		}
	}
}

void WinEditMain::htmlEntities()
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

void WinEditMain::print()
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

void WinEditMain::closeAll()
{
	STRING			fileName;

	WinEditChild	*editor = static_cast<WinEditChild *>(getFirstChild());
	while( editor )
	{
		editor->message( WM_CLOSE );
		editor = static_cast<WinEditChild *>(getNextChild());
	}
}


void WinEditMain::preferences()
{
	PreferencesDlg	dialog;
	unsigned int	tabWidth;

	if( dialog.create( FILE_PREFERENCES, this  ) )
	{
		bool autoIndentFlag = application.GetProfile( "", myProfile::autoIndent, true );
		bool autoReloadFlag = application.GetProfile( "", myProfile::autoReload, false );
		tabWidth = (unsigned int)application.GetProfile( "", myProfile::tabWidth, 4 );
		WinEditChild	*editor = static_cast<WinEditChild *>(getFirstChild());
		if( editor ) do
		{
			editor->setTabWidth( tabWidth );
			if( autoIndentFlag )
				editor->setAutoIndent();
			else
				editor->clrAutoIndent();
			if( autoReloadFlag )
				editor->enableAutoReload();
			else
				editor->disableAutoReload();

			editor = static_cast<WinEditChild *>(getNextChild());
		} while( editor );
	}
}

void WinEditMain::selectFont()
{
	if( m_theFont.selectFont( this, true, CF_FIXEDPITCHONLY	) )
	{
		application.WriteProfile( false, "", myProfile::Font, m_theFont.getFontName() );
		application.WriteProfile( false, "", myProfile::FontSize, m_theFont.getFontSize() );
		application.WriteProfile( false, "", myProfile::Style, m_theFont.getStyle() );
		WinEditChild	*editor = static_cast<WinEditChild *>(getFirstChild());
		if( editor ) do
		{
			editor->setFont( m_theFont );
			editor = static_cast<WinEditChild *>(getNextChild());
		} while( editor );
	}
}

void WinEditMain::undo()
{
	WinEditChild	*editor = static_cast<WinEditChild *>(getActiveChild());
	if( editor )
	{
		editor->undo();
	}
}

void WinEditMain::cut()
{
	WinEditChild	*editor = static_cast<WinEditChild *>(getActiveChild());
	if( editor )
	{
		editor->cut();
	}
}

void WinEditMain::copy()
{
	WinEditChild	*editor = static_cast<WinEditChild *>(getActiveChild());
	if( editor )
	{
		editor->copy();
	}
}

void WinEditMain::paste()
{
	WinEditChild	*editor = static_cast<WinEditChild *>(getActiveChild());
	if( editor )
	{
		editor->paste();
	}
}

void WinEditMain::clear()
{
	WinEditChild	*editor = static_cast<WinEditChild *>(getActiveChild());
	if( editor )
	{
		editor->clear();
	}
}

void WinEditMain::selectAll()
{
	WinEditChild	*editor = static_cast<WinEditChild *>(getActiveChild());
	if( editor )
	{
		editor->selectAll();
	}
}

void WinEditMain::deleteLine()
{
	WinEditChild	*editor = static_cast<WinEditChild *>(getActiveChild());
	if( editor )
	{
		editor->deleteLine();
	}
}

void WinEditMain::indent()
{
	WinEditChild	*editor = static_cast<WinEditChild *>(getActiveChild());
	if( editor && editor->isEditable() )
	{
		editor->indent();
	}
}

void WinEditMain::unindent()
{
	WinEditChild	*editor = static_cast<WinEditChild *>(getActiveChild());
	if( editor )
	{
		editor->unindent();
	}
}

void WinEditMain::lineBreak()
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

void WinEditMain::searchText()
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
		selection = m_lastSearchFor;
	}
	else
	{
		m_lastSearchFor = selection;
	}

	searchDialog = SearchDialog::create();
	if( searchDialog )
	{
		searchDialog->create( this, selection, m_lastWholeWord, m_lastMatchCase, m_lastDownSearch );
	}
}

void WinEditMain::replaceText()
{
	ReplaceDialog	*replaceDialog;
	STRING			selection;

	WinEditChild	*editor = static_cast<WinEditChild *>(getActiveChild());
	if( editor )
		selection = editor->getSelection();

	if( !strlen( selection ) )
		selection = m_lastSearchFor;
	else
		m_lastSearchFor = selection;

	replaceDialog = ReplaceDialog::create();
	if( replaceDialog )
		replaceDialog->create( this, selection, m_lastReplace, m_lastWholeWord, m_lastMatchCase );
}

void WinEditMain::searchNext()
{
	if( !m_lastReplace.isEmpty() )
		handleReplace( m_lastSearchFor, m_lastReplace,
						false, m_lastWholeWord, m_lastMatchCase );
	else if( !m_lastSearchFor.isEmpty() )
		handleFind( m_lastSearchFor, m_lastWholeWord, m_lastMatchCase, m_lastDownSearch );
}

void WinEditMain::compareFiles()
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

ProcessStatus AboutDlg::handleCreate()
{
	m_addressWidthLabel.initHandle(this, INFO_ADRESS_WIDTH);
	m_addressWidthLabel.setText( sizeof(void*)==4 ? "32-Bit" : "64 Bit");
	return psPROCESSED;
}

ProcessStatus PreferencesDlg::handleCreate()
{
	bool	backupFlag, autoIndentFlag, autoReloadFlag;

	tabwidth.initHandle( this, TABWIDTH );
	backup.initHandle( this, CREATE_BACKUP );
	autoIndent.initHandle( this, IDC_AUTO_INDENT );
	autoReload.initHandle( this, AUTO_RELOAD );

	tabwidth.setText(formatNumber(application.GetProfile( "", myProfile::tabWidth, 4)));

	backupFlag = application.GetProfile( "", myProfile::backup, true );
	if( backupFlag )
		backup.setActive();
	else
		backup.clrActive();

	autoIndentFlag = application.GetProfile( "", myProfile::autoIndent, true );
	if( autoIndentFlag )
		autoIndent.setActive();
	else
		autoIndent.clrActive();

	autoReloadFlag = application.GetProfile( "", myProfile::autoReload, false );
	if( autoReloadFlag )
		autoReload.setActive();
	else
		autoReload.clrActive();

	return psPROCESSED;
}

ProcessStatus PreferencesDlg::handleOk()
{
	bool	backupFlag, autoIndentFlag, autoReloadFlag;

	application.WriteProfile( false, "", myProfile::tabWidth, tabwidth.getText().getValueN<unsigned>() );

	backupFlag = backup.isActive();
	application.WriteProfile( false, "", myProfile::backup, backupFlag );

	autoIndentFlag = autoIndent.isActive();
	application.WriteProfile( false, "", myProfile::autoIndent, autoIndentFlag );

	autoReloadFlag = autoReload.isActive();
	application.WriteProfile( false, "", myProfile::autoReload, autoReloadFlag );

	return psDO_DEFAULT;
}

void WinEditChild::handleFocus()
{
	WinEditMain	*mainWindow = (WinEditMain *)getMDIparent();

	enum EDIT::EditCharset charSet = getCharset();

	if( charSet == EDIT::csOEM )
		mainWindow->getMenu()->checkMenuItem( EDIT_OEM_CONVERT );
	else
		mainWindow->getMenu()->uncheckMenuItem( EDIT_OEM_CONVERT );

	if( charSet == EDIT::csANSI )
		mainWindow->getMenu()->checkMenuItem( EDIT_CS_ANSI );
	else
		mainWindow->getMenu()->uncheckMenuItem( EDIT_CS_ANSI );

	if( charSet == EDIT::csUTF8 )
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
			(autoReload() && !isChanged()) ||
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

SuccessCode WinEditChild::handleClose()
{
	if( isChanged() )
	{
		char	text_buff[1024];
		STRING	theMessage = application.loadString( CLOSE_ANYWAY );
		STRING	theFile = getFileName();
		if( theMessage.size() + theFile.size() >= 1024 )
		{
			theFile = STRING("...") + theFile.rightString( 1016 - theMessage.strlen() );
		}
		sprintf( text_buff, theMessage, !theFile.isEmpty() ? (const char *)theFile : "untitled" );
		int button = messageBox( text_buff, WARNING, MB_APPLMODAL|MB_ICONSTOP|MB_YESNOCANCEL );

		if( button == IDCANCEL )
/*@*/		return scERROR;

		if( button == IDYES )
		{
			saveToFile();
			if( isChanged() )
/*@*/			return scERROR;
		}
	}

	WinEditMain *mainWindows = (WinEditMain *)getMDIparent();
	size_t selStart, selEnd;
	getSelection(&selStart, &selEnd);
	mainWindows->updateFileMenu( getFileName(), selStart, selEnd );

	close();
	return scSUCCESS;
}

void WinEditMain::handleFocus()
{
	WinEditChild	*editor = static_cast<WinEditChild *>(getActiveChild());
	if( editor )
		editor->focus();
}

SuccessCode WinEditMain::handleClose()
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
		openFile( nullptr );				break;
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
		changeCharset( EDIT::csOEM );		break;
	case EDIT_CS_ANSI:
		changeCharset( EDIT::csANSI );		break;
	case EDIT_CS_UTF8:
		changeCharset( EDIT::csUTF8 );		break;
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
		if( command >= MIN_FILE_ID && command < MIN_FILE_ID+MAX_FILE_LIST )
		{
			size_t id = command-MIN_FILE_ID;
			FileSpec *fs = m_lastFiles.getDataBuffer() + id;
			if( !fs->filename.isEmpty() )
			{
				openFile( fs->filename, fs->charset, fs->selStart,fs->selEnd );
				break;
			}
		}

		return psDO_DEFAULT;
	}
	return psPROCESSED;
}

void WinEditMain::handleFile( const char *fileName, size_t, size_t )
{
	openFile( fileName );
}

void WinEditMain::handleFind( const char *text, bool wholeWord, bool matchCase, bool downSearch )
{
	m_lastSearchFor = text;
	m_lastWholeWord = wholeWord;
	m_lastMatchCase = matchCase;
	m_lastDownSearch = downSearch;
	m_lastReplace   = (char *)NULL;

	WinEditChild	*editor = static_cast<WinEditChild *>(getActiveChild());
	if( editor )
	{
		editor->searchText( 
			m_lastSearchFor, 
			downSearch ? END_SELECTION 
					   : BEGIN_SELECTION, 
			wholeWord, matchCase, downSearch 
		);
	}
}

void WinEditMain::handleReplace( const char *text, const char *replace,
									bool all, bool wholeWord, bool matchCase )
{
	m_lastSearchFor = text;
	m_lastReplace   = replace;
	m_lastWholeWord = wholeWord;
	m_lastMatchCase = matchCase;

	WinEditChild	*editor = static_cast<WinEditChild *>(getActiveChild());
	if( editor && editor->isEditable() )
	{
		editor->searchText(
			m_lastSearchFor,
			BEGIN_SELECTION,
			wholeWord, matchCase, m_lastDownSearch,
			m_lastReplace,
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
		openFile( commandPtr + sizeof( "open" ) );
		return psPROCESSED;
	}

	return psDO_DEFAULT;
}

void WinEditMain::handleTimer()
{
	static size_t	lastLine = size_t(-1);
	static size_t	lastColumn = size_t(-1);
	static bool		lastModified = false;

	size_t			line, column;
	bool			modified;
	STRING			newStatus;

	WinEditChild	*editor = static_cast<WinEditChild *>(getActiveChild());
	if( editor )
	{
		editor->getCursorPosition( &line, &column );
		modified = editor->isChanged();

		if( lastLine != line
		|| lastColumn != column
		|| lastModified != modified )
		{
			newStatus = formatNumber(line) + ':' + formatNumber(column);
			if(modified)
			{
				newStatus += " modified";
				getMenu()->enableMenuItem( FILE_SAVE );
			}
			else
			{
				getMenu()->disableMenuItem( FILE_SAVE );
			}
			setStatusText( newStatus );

			lastLine = line;
			lastColumn = column;
			lastModified = modified;
		}
	}
	else
	{
		setStatusText( "Welcome" );
		lastLine = lastColumn = size_t(-1);
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
		mainWindow->openFile( fileName );

	return mainWindow;
}

void WinEdit::deleteMainWindow( BasicWindow  *mainWindow )
{
	delete mainWindow;
}

// --------------------------------------------------------------------- //
// ----- class publics ------------------------------------------------- //
// --------------------------------------------------------------------- //
void WinEditChild::htmlEntities()
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

void WinEditMain::openFile( const STRING &iFileName, EDIT::EditCharset charSet, size_t selStart, size_t selEnd )
{
	doEnterFunction("WinEditMain::openFile");

	WinEditChild	*editor=NULL;
	F_STRING		curFileName;
	unsigned int	tabWidth;
	bool			newWindow = false;

	focus();

	if( !m_theFont.isAssigned() )
	{
		m_theFont.setFixedFont();
		m_theFont.setFontName( application.GetProfile( "", myProfile::Font, "Fixedsys" ) );
		m_theFont.setFontSize( application.GetProfile( "", myProfile::FontSize, 9 ) );
		m_theFont.setStyle( application.GetProfile( "", myProfile::Style, 0 ) );
		m_theFont.create();
	}

	F_STRING	theFileName = ( iFileName.beginsWith('\"') && iFileName.endsWith('\"') ) 
		? iFileName.subString( 1, iFileName.strlen()-2 ) 
		: iFileName;

	if( !theFileName.isEmpty() )
	{
		for( int i=0; i<MAX_FILE_LIST; i++ )
		{
			if( m_lastFiles[i].filename == theFileName )
			{
				charSet = m_lastFiles[i].charset;
				break;
			}
		}
		editor = static_cast<WinEditChild *>(getFirstChild());
		while( editor )
		{
			curFileName = editor->getFileName();
			if( curFileName == theFileName )
				break;
			editor = static_cast<WinEditChild *>(getNextChild());
		}
	}

	if( !editor )
	{
		editor = new WinEditChild;
		editor->create( this, theFileName, 0, 0 );

		if( m_theFont.isAssigned() )
			editor->setFont( m_theFont );

		tabWidth = (unsigned int)application.GetProfile( "", myProfile::tabWidth, 4 );

		editor->setTabWidth( tabWidth );
		bool autoIndentFlag = application.GetProfile( "", myProfile::autoIndent, true );
		if( autoIndentFlag )
			editor->setAutoIndent();
		else
			editor->clrAutoIndent();

		bool autoReloadFlag = application.GetProfile( "", myProfile::autoReload, false );
		if( autoReloadFlag )
			editor->enableAutoReload();
		else
			editor->disableAutoReload();

		newWindow = true;
	}
	else
		editor->focus();

	if( !theFileName.isEmpty() )
	{
		if( strAccess( theFileName, 02 ) )
			editor->setReadonly();

		if( newWindow || editor->isChanged() || editor->checkModifyTime() )
		{
			doLogPosition();
			editor->readFromFile( theFileName );
		}
		addFile( theFileName, charSet, selStart, selEnd );
	}
	else
	{
		editor->setTitle( UNTITLED );
	}

	doLogValue( !theFileName.isEmpty() ? theFileName : F_STRING("NULL") );
	doLogValue( charSet );
	editor->setCharset( charSet );
	editor->setSelection( selStart, selEnd );
	doLogValue( sizeof(RLINE_ENDS) );
	checkLineEnd( editor->getLineEnd() );


	handleFocus();
}

void WinEditMain::checkLineEnd( RLINE_ENDS newLineEnd )
{
	WORD menuItem = 0;

	if( m_lastLineMenuItemChecked > 0 )
		getMenu()->uncheckMenuItem( m_lastLineMenuItemChecked );

	switch( newLineEnd )
	{
		case RL_END_CR:		menuItem = EDIT_LINE_CR;	break;
		case RL_END_LF:		menuItem = EDIT_LINE_LF;	break;
		case RL_END_CRLF:	menuItem = EDIT_LINE_CRLF;	break;
		case RL_END_LFCR:	menuItem = EDIT_LINE_LFCR;	break;
	}

	if( menuItem > 0 )
		getMenu()->checkMenuItem( menuItem );

	m_lastLineMenuItemChecked = menuItem;
}

void WinEditMain::updateFileMenu( const STRING &fileName, size_t selStart, size_t selEnd )
{
	for( size_t i=0; i<m_numFiles; ++i )
	{
		FileSpec &entry = m_lastFiles[i];
		if( entry.filename == fileName )
		{
			entry.selStart = selStart;
			entry.selEnd = selEnd;
			break;
		}
	}
}

