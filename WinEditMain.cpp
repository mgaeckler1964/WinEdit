/*
		Project:		WINEDIT (Windows MDI-Editor)
		Module:			WinEditMain.cpp
		Description:	The MDI Main Window
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

// --------------------------------------------------------------------- //
// ----- includes ------------------------------------------------------ //
// --------------------------------------------------------------------- //

#include "WinEditMain.h"

#include <gak/fmtNumber.h>

#include <WINLIB/colors.h>
#include <WINLIB/StandardDialogs.h>

#include "WINEDIT.H"
#include "WinEditChild.h"
#include "Dialogs.h"

#include "winedit.gui.h"

// --------------------------------------------------------------------- //
// ----- imported datas ------------------------------------------------ //
// --------------------------------------------------------------------- //

// --------------------------------------------------------------------- //
// ----- module switches ----------------------------------------------- //
// --------------------------------------------------------------------- //

#ifdef __BORLANDC__
#	pragma option -RT-
#	pragma option -b
#	pragma option -a4
#	pragma option -pc
#endif

using namespace gak;
using namespace winlib;
using namespace winlibGUI;

// --------------------------------------------------------------------- //
// ----- constants ----------------------------------------------------- //
// --------------------------------------------------------------------- //

// --------------------------------------------------------------------- //
// ----- macros -------------------------------------------------------- //
// --------------------------------------------------------------------- //

// --------------------------------------------------------------------- //
// ----- type definitions ---------------------------------------------- //
// --------------------------------------------------------------------- //

// --------------------------------------------------------------------- //
// ----- class definitions --------------------------------------------- //
// --------------------------------------------------------------------- //

// --------------------------------------------------------------------- //
// ----- exported datas ------------------------------------------------ //
// --------------------------------------------------------------------- //

// --------------------------------------------------------------------- //
// ----- module static data -------------------------------------------- //
// --------------------------------------------------------------------- //

// --------------------------------------------------------------------- //
// ----- class static data --------------------------------------------- //
// --------------------------------------------------------------------- //

// --------------------------------------------------------------------- //
// ----- prototypes ---------------------------------------------------- //
// --------------------------------------------------------------------- //

// --------------------------------------------------------------------- //
// ----- module functions ---------------------------------------------- //
// --------------------------------------------------------------------- //

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
// ----- class inlines ------------------------------------------------- //
// --------------------------------------------------------------------- //

// --------------------------------------------------------------------- //
// ----- class constructors/destructors -------------------------------- //
// --------------------------------------------------------------------- //

WinEditMain::WinEditMain() : MDIwindow( nullptr ), m_theFont( this )
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
// ----- class static functions ---------------------------------------- //
// --------------------------------------------------------------------- //

void WinEditMain::loadRegistryFileEntry(const char *baseName, const char *legacyCharset, unsigned u, FileSpec *entry, bool *foundLegacy)
{
	STRING fileName;
	STRING uStr = formatNumber(u);
	STRING regName = STRING(baseName) + uStr;

	entry->filename = appObject->GetProfile( nullptr, regName, fileName );
	if( !entry->filename.isEmpty() )
	{
		STRING charsetName = STRING(legacyCharset) + uStr;
		entry->charset = EDIT::EditCharset(appObject->GetProfile( nullptr, charsetName, EDIT::csANSI ));
		entry->selStart = entry->selEnd = 0;

		appObject->DeleteProfile(false, "", regName );
		appObject->DeleteProfile(false, "", charsetName );

		appObject->WriteProfile( false, regName, myProfile::filename, entry->filename );
		appObject->WriteProfile( false, regName, myProfile::charset, entry->charset );

		*foundLegacy = true;
	}
	else
	{
		entry->filename = appObject->GetProfile( regName, myProfile::filename, entry->filename );
		entry->charset = EDIT::EditCharset(appObject->GetProfile( regName, myProfile::charset, EDIT::csANSI ));
		entry->selStart = appObject->GetProfile( regName, myProfile::selStart, 0 );
		entry->selEnd = appObject->GetProfile( regName, myProfile::selEnd, 0 );
	}
}

void WinEditMain::saveRegistryFileEntry(const char *baseName, unsigned u, const FileSpec &entry)
{
	STRING regName = STRING(baseName) + formatNumber(u);

	appObject->WriteProfile( false, regName, myProfile::filename, entry.filename );
	appObject->WriteProfile( false, regName, myProfile::charset, entry.charset );
	appObject->WriteProfile( false, regName, myProfile::selStart, long(entry.selStart) );
	appObject->WriteProfile( false, regName, myProfile::selEnd, long(entry.selEnd) );
}

// --------------------------------------------------------------------- //
// ----- class privates ------------------------------------------------ //
// --------------------------------------------------------------------- //

void WinEditMain::loadAllRegistryFileLists()
{
	doEnterFunctionEx(gakLogging::llInfo, "WinEditMain::loadFileList");

	for( unsigned u=0; u<MAX_FILE_LIST; ++u )
	{
		FileSpec	fs;
		bool		dummy;
		loadRegistryFileEntry(myProfile::lastFile, myProfile::lastCharset, u, &fs, &dummy);

		if( !strAccess( fs.filename, 0 ) )
		{
			addMenuFile( fs );
		}
	}

	bool		foundLegacy = false;
	unsigned	numFiles = appObject->GetProfile( nullptr, myProfile::OpenFiles, 0 );
	for( unsigned u=0; u<numFiles; ++u )
	{
		FileSpec fs;
		loadRegistryFileEntry(myProfile::openFile, myProfile::charset, u, &fs, &foundLegacy);

		doLogValueEx( gakLogging::llInfo, u );
		openFile( fs.filename, fs.charset, fs.selStart, fs.selEnd );
	}

	if( foundLegacy )
	{
		unsigned u=numFiles;
		while( 1 )
		{
			STRING uStr = formatNumber(u++);
			STRING openFileEntry =  STRING(myProfile::openFile) + uStr;
			STRING charsetEntry =  STRING(myProfile::charset) + uStr;

			STRING fileName = appObject->GetProfile( nullptr, openFileEntry, (const char *)0 );
			EDIT::EditCharset charset = EDIT::EditCharset(appObject->GetProfile( nullptr, charsetEntry, (long)-1 ));

			appObject->DeleteProfile( false, "", openFileEntry );
			appObject->DeleteProfile( false, "", charsetEntry );
			if( fileName.isEmpty() && charset == -1 )
				break;
		}
	}
}

void WinEditMain::add2OpenFileList(WinEditChild	*editor)
{
	unsigned u = appObject->GetProfile( "", myProfile::OpenFiles, long(-1) )+1;
	STRING fileName = editor->getFileName();
	if( !fileName.isEmpty() )
	{
		FileSpec openFile = {
			fileName, 
			editor->getCharset()
		};
		editor->getSelection(&openFile.selStart, &openFile.selEnd );
		saveRegistryFileEntry(myProfile::openFile, u, openFile);
		appObject->WriteProfile( false, "", myProfile::OpenFiles, long(u) );
	}
}

void WinEditMain::saveOpenFileList()
{
	STRING			fileName;

	for( unsigned u=0; u<m_numFiles; u++ )
	{
		saveRegistryFileEntry(myProfile::lastFile, u, m_lastFiles[u]);
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
			saveRegistryFileEntry(myProfile::openFile, u++, openFile);
		}

		editor = static_cast<WinEditChild *>(getNextChild());
	}

	appObject->WriteProfile( false, "", myProfile::OpenFiles, long(u) );
}

void WinEditMain::addMenuFileEntryFromList( size_t id )
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

void WinEditMain::removeMenuFileEntry( size_t id )
{
	getMenu()->removeMenuItem( 0, int(MIN_FILE_ID+id) );

	for( size_t i=id+1; i<m_numFiles; i++ )
	{
		getMenu()->removeMenuItem( 0, int(MIN_FILE_ID+i) );
		m_lastFiles[i-1] = m_lastFiles[i];
		addMenuFileEntryFromList( i-1 );
	}

	--m_numFiles;
}

void WinEditMain::addMenuFile( const FileSpec &fs )
{
	// const STRING &fileName, EDIT::EditCharset charset, size_t selStart, size_t selEnd

	doEnterFunctionEx(gakLogging::llInfo, "WinEditMain::addFile");
	for( size_t i=0; i<m_numFiles; i++ )
	{
		if( m_lastFiles[i].filename == fs.filename )
		{
			removeMenuFileEntry( i );
			break;
		}
	}

	if( m_numFiles >= MAX_FILE_LIST )
		removeMenuFileEntry( 0 );

	m_lastFiles[m_numFiles] = fs;
	addMenuFileEntryFromList( m_numFiles );
	m_numFiles++;
}

void WinEditMain::createBackup( const STRING &fileName )
{
	bool	backupFlag;
	STRING	backupFile;
	size_t	slash;
	size_t	dot;

	backupFlag = appObject->GetProfile( nullptr, myProfile::backup, true );
	if( backupFlag )
	{
		backupFile = fileName;

		dot = backupFile.searchRChar( '.' );
		slash = backupFile.searchRChar( '\\' );

		if( dot!=(size_t)-1 && dot > slash )
			backupFile.cut( dot );

		backupFile += ".bak";

		if( !strAccess( backupFile, 0 ) )
			::strRemove( backupFile );
		rename( fileName, backupFile );
	}
}

void WinEditMain::checkAllFiles()
{
	WinEditChild	*editor = static_cast<WinEditChild *>(getFirstChild());
	while( editor )
	{
		STRING	fileName = editor->getFileName();

		if( !fileName.isEmpty() )
		{
			int errCode = strAccess( fileName, 02 );
			if( errCode && editor->isEditable() )
			{
				editor->setReadonly();
				editor->setBackgroundColorByRef(colors::LIGHT_SALMON);
			}
			else if( !errCode && !editor->isEditable() )
			{
				editor->setEditable();
				editor->setBackgroundColorByRef(colors::WHITE);
			}
		}

		editor = static_cast<WinEditChild *>(getNextChild());
	}
}

void WinEditMain::openFile()
{
	OpenFileDialog	fileDlg;

	// find default directory
	STRING curDir = getcwd();
	STRING	directory = appObject->GetProfile( nullptr, myProfile::directory, curDir );
	setcwd( directory );

	// restore last filter used
	fileDlg.setFilterIndex( appObject->GetProfile( nullptr, myProfile::filterIndex, 1 ) );
	fileDlg.setDirectory( directory );
	if( fileDlg.create( this,
						0,
						FileTypeList_ids, FileTypeList_count )
	)
	{
		appObject->WriteProfile( false, nullptr, myProfile::filterIndex, fileDlg.getFilterIndex() );
		openFile( fileDlg.getFilename() );
	}

	// save default directory
	curDir = getcwd();
	appObject->WriteProfile( false, nullptr, myProfile::directory, curDir );
}

void WinEditMain::saveFile()
{
	WinEditChild	*editor = static_cast<WinEditChild *>(getActiveChild());
	if( editor && editor->isChanged() )
	{
		saveFile(editor);
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
	WinEditChild	*editor = static_cast<WinEditChild *>(getActiveChild());
	if( editor )
	{
		saveFileAs(editor);
	}
}

void WinEditMain::saveFileAs(WinEditChild	*editor)
{
	SaveFileAsDialog	saveDialog;
	STRING				fileName;
	STRING				tmpName = editor->getFileName();
	tmpName = getDir( tmpName );
	if( !tmpName.isEmpty() )
		setcwd( tmpName );

	tmpName = editor->getFileName();
	if( !tmpName.isEmpty() )
		fileName = tmpName;

	saveDialog.setFilterIndex( appObject->GetProfile( nullptr, myProfile::filterIndex, 1 ) );

	saveDialog.setFilename( fileName );
	if( saveDialog.create( this,
							0,
							FileTypeList_ids, FileTypeList_count ) )
	{
		editor->saveToFile( saveDialog.getFilename() );
		FileSpec fs = {
			saveDialog.getFilename(), editor->getCharset()
		};
		editor->getSelection(&fs.selStart, &fs.selEnd);
		addMenuFile( fs );
		appObject->WriteProfile( false, nullptr, myProfile::filterIndex, saveDialog.getFilterIndex() );
		add2OpenFileList(editor);
	}
}

void WinEditMain::changeCharset( EDIT::EditCharset newCharset, bool flipBOM )
{
	doEnterFunctionEx(gakLogging::llInfo, "WinEditMain::changeCharset");

	EDIT::EditCharset	oldCharset;

	WinEditChild	*editor = static_cast<WinEditChild *>(getActiveChild());
	if( editor )
	{
		oldCharset = editor->getCharset();
		if( newCharset != EDIT::csUTF8 )
		{
			editor->withBOM() = false;
			getMenu()->uncheckMenuItem( EDIT_CS_UTF8_BOM_id );
		}
		else if(flipBOM)
		{
			if( oldCharset != EDIT::csUTF8 )
			{
				editor->withBOM() = true;
				getMenu()->checkMenuItem( EDIT_CS_UTF8_BOM_id );
			}
			else
			{
				if( editor->withBOM() )
				{
					editor->withBOM() = false;
					getMenu()->uncheckMenuItem( EDIT_CS_UTF8_BOM_id );
				}
				else
				{
					editor->withBOM() = true;
					getMenu()->checkMenuItem( EDIT_CS_UTF8_BOM_id );
				}
			}
		}
		if( oldCharset != newCharset )
		{
			SHORT	kState = GetKeyState( VK_SHIFT );
			kState >>= 8;
			if( kState )
				editor->convertCharset( newCharset );
			else
				editor->setCharset( newCharset );

			if( oldCharset == EDIT::csOEM )
				getMenu()->uncheckMenuItem( EDIT_OEM_CONVERT_id );
			else if( oldCharset == EDIT::csANSI )
				getMenu()->uncheckMenuItem( EDIT_CS_ANSI_id );
			else if( oldCharset == EDIT::csUTF8 )
				getMenu()->uncheckMenuItem( EDIT_CS_UTF8_id );

			if( newCharset == EDIT::csOEM )
				getMenu()->checkMenuItem( EDIT_OEM_CONVERT_id );
			else if( newCharset == EDIT::csANSI )
				getMenu()->checkMenuItem( EDIT_CS_ANSI_id );
			else if( newCharset == EDIT::csUTF8 )
				getMenu()->checkMenuItem( EDIT_CS_UTF8_id );

			STRING theFileName = editor->getFileName();
			for( size_t i=0; i<MAX_FILE_LIST; i++ )
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

void WinEditMain::changeReadOnly()
{
	WinEditChild	*editor = static_cast<WinEditChild *>(getActiveChild());
	if( editor )
	{
		editor->changeReadOnly();
	}
}

void WinEditMain::print()
{
	WinEditChild	*editor = static_cast<WinEditChild *>(getActiveChild());
	if( editor )
	{
		PrinterDialog	dlg;

		if( dlg.create( this ) )
		{
			std::unique_ptr<PrintDevice> printer( dlg.getPrinter() );
			editor->print( printer.get() );
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

	if( dialog.create( this  ) == scSUCCESS && dialog.getModalResult() == IDOK )
	{
		bool			autoIndentFlag = appObject->GetProfile( nullptr, myProfile::autoIndent, true );
		bool			autoReloadFlag = appObject->GetProfile( nullptr, myProfile::autoReload, false );
		bool			stripLineEndsFlag = appObject->GetProfile( nullptr, myProfile::stripLineEnds, false );
		unsigned		tabWidth = unsigned(appObject->GetProfile( nullptr, myProfile::tabWidth, 4 ));
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

			editor->stripLineEnds() = stripLineEndsFlag;

			editor = static_cast<WinEditChild *>(getNextChild());
		} while( editor );
	}
}

void WinEditMain::selectFont()
{
	if( m_theFont.selectFont( this, true, CF_FIXEDPITCHONLY	) )
	{
		appObject->WriteProfile( false, "", myProfile::Font, m_theFont.getFontName() );
		appObject->WriteProfile( false, "", myProfile::FontSize, m_theFont.getFontSize() );
		appObject->WriteProfile( false, "", myProfile::Style, m_theFont.getStyle() );
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
			getMenu()->uncheckMenuItem( EDIT_BREAK_id );
		}
		else
		{
			editor->setLineBreak();
			getMenu()->checkMenuItem( EDIT_BREAK_id );
		}
	}
}

void WinEditMain::formatBlock()
{

	WinEditChild	*editor = static_cast<WinEditChild *>(getActiveChild());
	if( editor )
	{
		FormatBlockDlg	dialog;

		if( dialog.create( this  ) == scSUCCESS && dialog.getModalResult() == IDOK )
		{
			WinEditChild::BlockFormat	fmt = dialog.getFormat();
			size_t lineLen = dialog.getLineLen();
			if( lineLen )
				editor->formatBlock(fmt, lineLen);
		}
	}
}

void WinEditMain::convertIndent( int command )
{
	WinEditChild	*editor = static_cast<WinEditChild *>(getActiveChild());
	if( editor )
	{
		editor->convertIndent(command);
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
// ----- class protected ----------------------------------------------- //
// --------------------------------------------------------------------- //

// --------------------------------------------------------------------- //
// ----- class virtuals ------------------------------------------------ //
// --------------------------------------------------------------------- //
   
void WinEditMain::handleFocus()
{
	WinEditChild	*editor = static_cast<WinEditChild *>(getActiveChild());
	if( editor )
		editor->focus();
}

SuccessCode WinEditMain::handleClose()
{
	saveOpenFileList();
	saveWindowRect( "mainWindow" );
	return MDIwindow::handleClose();
}

ProcessStatus WinEditMain::handleCommand( int command )
{
	switch( command )
	{
	case FILE_NEW_id:
		openFile( nullptr );				break;
	case FILE_OPEN_id:
		openFile();							break;
	case FILE_SAVE_id:
		saveFile();							break;
	case FILE_SAVE_ALL_id:
		saveAllFiles();						break;

	case FILE_SAVE_AS_id:
		saveFileAs();						break;

	case FILE_PRINT_id:
		print();							break;

	case FILE_PREFERENCES_id:
		preferences();						break;

	case FILE_FONT_id:
		selectFont();						break;

	case FILE_QUIT_id:
		handleClose();						break;

	case EDIT_UNDO_id:
		undo();								break;
	case EDIT_CUT_id:
		cut();								break;
	case EDIT_COPY_id:
		copy();								break;
	case EDIT_PASTE_id:
		paste();							break;
	case EDIT_CLEAR_id:
		clear();							break;
	case EDIT_SELECT_ALL_id:
		selectAll();						break;
	case EDIT_DELETE_LINE_id:
		deleteLine();						break;
	case EDIT_INDENT_id:
		indent();							break;
	case EDIT_UNINDENT_id:
		unindent();							break;
	case EDIT_BREAK_id:
		lineBreak();						break;
	case EDIT_OEM_CONVERT_id:
		changeCharset( EDIT::csOEM );		break;
	case EDIT_CS_ANSI_id:
		changeCharset( EDIT::csANSI );		break;
	case EDIT_CS_UTF8_id:
		changeCharset( EDIT::csUTF8 );		break;
	case EDIT_CS_UTF8_BOM_id:
		changeCharset(EDIT::csUTF8,true);	break;
	case EDIT_HTML_ENTITIES_id:
		htmlEntities();						break;

	case EDIT_LINE_CR_id:
		changeLineEnd(RL_END_CR);			break;
	case EDIT_LINE_CRLF_id:
		changeLineEnd(RL_END_CRLF);			break;
	case EDIT_LINE_LF_id:
		changeLineEnd(RL_END_LF);			break;
	case EDIT_LINE_LFCR_id:
		changeLineEnd(RL_END_LFCR);			break;
	case EDIT_READ_ONLY_id:
		changeReadOnly();					break;

	case EDIT_FORMAT_id:
		formatBlock();						break;
	case EDIT_TAB_INDENT_id:
	case EDIT_SPACE_INDENT_id:
		convertIndent( command );			break;

	case SEARCH_SEARCH_id:
		searchText();						break;
	case SEARCH_REPLACE_id:
		replaceText();						break;
	case SEARCH_NEXT_id:
		searchNext();						break;
	case COMPARE_FILES_id:
		compareFiles();						break;

	case WIN_TILE_HORIZ_id:
		tileChildren();						break;
	case WIN_TILE_VERT_id:
		tileChildren( false );				break;
	case WIN_CASCADE_id:
		cascadeChildren();					break;
	case WIN_ARRANGE_id:
		arrangeIcons();						break;
	case WIN_CLOSE_ALL_id:
		closeAll();							break;

	case INFO_ABOUT_id:
		{
			AboutDlg	dlg;
			dlg.create( this );
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
	m_lastReplace   = nullptr;

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
	doEnterFunctionEx(gakLogging::llInfo, "WinEdit::ddeInitiate" );
	doLogValueEx( gakLogging::llInfo, application );
	doLogValueEx( gakLogging::llInfo, topic );

	if( !strcmpi( application, APP_NAME )
	&&  !strcmpi( topic, "SYSTEM" ) )
		return psPROCESSED;
	return psDO_DEFAULT;
}

ProcessStatus WinEditMain::ddeExecute( const char *commandPtr )
{
	doEnterFunctionEx(gakLogging::llInfo, "WinEdit::startApplication" );
	doLogValueEx( gakLogging::llInfo, commandPtr );

	if( !strncmpi( commandPtr, "open ", sizeof( "open " ) -1 ) )
	{
		openFile( commandPtr + sizeof( "open" ) );
		return psPROCESSED;
	}

	return psDO_DEFAULT;
}

void WinEditMain::handleTimer()
{
	static size_t	s_lastLine = size_t(-1);
	static size_t	s_lastColumn = size_t(-1);
	static bool		s_lastModified = false;

	size_t			line, column;
	bool			modified;
	STRING			newStatus;

	WinEditChild	*editor = static_cast<WinEditChild *>(getActiveChild());
	if( editor )
	{
		checkAllFiles();

		editor->getCursorPosition( &line, &column );
		modified = editor->isChanged();

		if( s_lastLine != line
		|| s_lastColumn != column
		|| s_lastModified != modified )
		{
			newStatus = formatNumber(line) + ':' + formatNumber(column);
			if(modified)
			{
				newStatus += " modified";
				getMenu()->enableMenuItem( FILE_SAVE_id );
			}
			else
			{
				getMenu()->disableMenuItem( FILE_SAVE_id );
			}
			setStatusText( newStatus );

			s_lastLine = line;
			s_lastColumn = column;
			s_lastModified = modified;
		}
	}
	else
	{
		setStatusText( "Welcome" );
		s_lastLine = s_lastColumn = size_t(-1);
		s_lastModified = false;
	}
}

ProcessStatus WinEditMain::handleData(const ForeignWindow &, gak::uint64 dataType, const void *data, size_t size)
{
	dataType; size;
	assert( dataType == LOAD_FILE );
	assert(strlen((const char*)data)+1==size);

	handleFile( (const char *)data, 0, 0 );
	return psPROCESSED;
}

// --------------------------------------------------------------------- //
// ----- class publics ------------------------------------------------- //
// --------------------------------------------------------------------- //

SuccessCode WinEditMain::create( int nCmdShow )
{
	setText( APP_NAME );

	static ACCEL accelerators[] =
	{
		{ FVIRTKEY|FCONTROL,	'A', EDIT_SELECT_ALL_id },
		{ FVIRTKEY|FCONTROL,	'B', EDIT_FORMAT_id },
		{ FVIRTKEY|FCONTROL,	'C', EDIT_COPY_id },
		{ FVIRTKEY|FCONTROL,	'F', SEARCH_SEARCH_id },
		{ FVIRTKEY|FCONTROL,	'I', EDIT_INDENT_id },
		{ FVIRTKEY|FCONTROL,	'L', SEARCH_NEXT_id },
		{ FVIRTKEY|FCONTROL,	'N', FILE_NEW_id },
		{ FVIRTKEY|FCONTROL,	'O', FILE_OPEN_id },
		{ FVIRTKEY|FCONTROL,	'R', SEARCH_REPLACE_id },
		{ FVIRTKEY|FCONTROL,	'S', FILE_SAVE_id },
		{ FVIRTKEY|FCONTROL,	'U', EDIT_UNINDENT_id },
		{ FVIRTKEY|FCONTROL,	'V', EDIT_PASTE_id },
		{ FVIRTKEY|FCONTROL,	'X', EDIT_CUT_id },
		{ FVIRTKEY|FALT,		'X', FILE_QUIT_id },
		{ FVIRTKEY|FCONTROL,	'Y', EDIT_DELETE_LINE_id },
		{ FVIRTKEY|FCONTROL,	'Z', EDIT_UNDO_id },
	};
	setAccelerators( accelerators, int(arraySize( accelerators )) );

	if( MDIwindow::create( MENU_id, WINDOW_MENU ) == scERROR )
		return scERROR;

	acceptFiles();

	addStatusBar();
	setStatusText( "Welcome" );
	setTimer( 500 );

	restoreWindowRect( "mainWindow", nCmdShow );

//	hideVertScrollBar();
//	hideHorizScrollBar();

	loadAllRegistryFileLists();

	return scSUCCESS;
}

void WinEditMain::saveFile(WinEditChild	*editor)
{
	STRING	fileName = editor->getFileName();
	if( editor->isEditable() && !fileName.isEmpty() )
	{
		createBackup( fileName );
		editor->saveToFile();
	}
	else
		saveFileAs();
}

void WinEditMain::openFile( const STRING &iFileName, EDIT::EditCharset charSet, size_t selStart, size_t selEnd )
{
	doEnterFunctionEx(gakLogging::llInfo, "WinEditMain::openFile");

	WinEditChild	*editor=nullptr;
	F_STRING		curFileName;
	unsigned int	tabWidth;
	bool			newWindow = false;

	focus();

	if( !m_theFont.isAssigned() )
	{
		m_theFont.setFixedFont();
		m_theFont.setFontName( appObject->GetProfile( nullptr, myProfile::Font, "Fixedsys" ) );
		m_theFont.setFontSize( appObject->GetProfile( nullptr, myProfile::FontSize, 9 ) );
		m_theFont.setStyle( appObject->GetProfile( nullptr, myProfile::Style, 0 ) );
		m_theFont.create();
	}

	F_STRING	theFileName = ( iFileName.beginsWith('\"') && iFileName.endsWith('\"') ) 
		? iFileName.subString( 1, iFileName.strlen()-2 ) 
		: iFileName;

	if( !theFileName.isEmpty() )
	{
		for( size_t i=0; i<MAX_FILE_LIST; i++ )
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
		editor = new WinEditChild(this);
		editor->create( this, theFileName, 0, 0 );

		if( m_theFont.isAssigned() )
			editor->setFont( m_theFont );

		tabWidth = (unsigned int)appObject->GetProfile( nullptr, myProfile::tabWidth, 4 );

		editor->setTabWidth( tabWidth );
		bool autoIndentFlag = appObject->GetProfile( nullptr, myProfile::autoIndent, true );
		if( autoIndentFlag )
			editor->setAutoIndent();
		else
			editor->clrAutoIndent();

		bool autoReloadFlag = appObject->GetProfile( nullptr, myProfile::autoReload, false );
		if( autoReloadFlag )
			editor->enableAutoReload();
		else
			editor->disableAutoReload();

		bool stripLineEndsFlag = appObject->GetProfile( nullptr, myProfile::stripLineEnds, false );
		editor->stripLineEnds() = stripLineEndsFlag;

		newWindow = true;
	}
	else
		editor->focus();

	if( !theFileName.isEmpty() )
	{
		if( strAccess( theFileName, 02 ) )
		{
			editor->setReadonly();
			editor->setBackgroundColorByRef(colors::LIGHT_SALMON);
		}

		if( newWindow || editor->isChanged() || editor->checkModifyTime() )
		{
			doLogPosition();
			editor->readFromFile( theFileName );
			if( editor->getCharset() == EDIT::csUTF8 )
			{
				charSet = EDIT::csUTF8;
			}
		}
		FileSpec fs = {
			theFileName, charSet, selStart, selEnd
		};
		addMenuFile( fs );
	}
	else
	{
		editor->setTitle( UNTITLED_id );
	}

	editor->setCharset( charSet );
	editor->setSelection( selStart, selEnd );
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
		case RL_END_CR:		menuItem = EDIT_LINE_CR_id;		break;
		case RL_END_LF:		menuItem = EDIT_LINE_LF_id;		break;
		case RL_END_CRLF:	menuItem = EDIT_LINE_CRLF_id;	break;
		case RL_END_LFCR:	menuItem = EDIT_LINE_LFCR_id;	break;
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

// --------------------------------------------------------------------- //
// ----- entry points -------------------------------------------------- //
// --------------------------------------------------------------------- //

#ifdef __BORLANDC__
#	pragma option -RT.
#	pragma option -b.
#	pragma option -a.
#	pragma option -p.
#endif
