/*
		Project:		WINEDIT (Windows MDI-Editor)
		Module:			WinEditMain.h
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

#ifndef WIN_EDIT_MAIN_H
#define WIN_EDIT_MAIN_H

// --------------------------------------------------------------------- //
// ----- switches ------------------------------------------------------ //
// --------------------------------------------------------------------- //

// --------------------------------------------------------------------- //
// ----- includes ------------------------------------------------------ //
// --------------------------------------------------------------------- //

#include <gak/fixedArray.h>

#include <winlib/mdiwin.h>

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

// --------------------------------------------------------------------- //
// ----- constants ----------------------------------------------------- //
// --------------------------------------------------------------------- //

const size_t	MAX_FILE_LIST	= 10;
const int		MIN_FILE_ID		= 1000;
const int		LOAD_FILE		= 666;

// --------------------------------------------------------------------- //
// ----- macros -------------------------------------------------------- //
// --------------------------------------------------------------------- //

// --------------------------------------------------------------------- //
// ----- type definitions ---------------------------------------------- //
// --------------------------------------------------------------------- //

// --------------------------------------------------------------------- //
// ----- class definitions --------------------------------------------- //
// --------------------------------------------------------------------- //
class WinEditChild;

class WinEditMain : public winlib::MDIwindow
{
	bool				m_lastWholeWord;
	bool				m_lastMatchCase;
	bool				m_lastDownSearch;
	gak::STRING			m_lastSearchFor;
	gak::STRING			m_lastReplace;

	struct FileSpec
	{
		gak::F_STRING				filename;
		winlib::EDIT::EditCharset	charset;
		size_t						selStart, selEnd;
	};

	gak::FixedArray<FileSpec,MAX_FILE_LIST>		m_lastFiles;
	size_t				m_numFiles;
	WORD				m_lastLineMenuItemChecked;
	winlib::Font		m_theFont;

	private:
	void checkAllFiles();
	void createBackup( const gak::STRING &fileName );
	void openFile();
	void saveFile();
	public:
	void saveFile(WinEditChild	*editor);
	private:
	void saveAllFiles();
	void saveFileAs();
	void saveFileAs(WinEditChild*child);
	void changeCharset( winlib::EDIT::EditCharset newCharset, bool flipBOM=false );
	void htmlEntities();
	void closeAll();

	void changeLineEnd( RLINE_ENDS newLineEnd );
	void changeReadOnly();

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
	void formatBlock();
	void convertIndent( int comand );

	void handleFind( const char *, bool, bool, bool ) override;
	void handleReplace( const char *, const char *, bool, bool, bool ) override;
	void searchText();
	void replaceText();
	void searchNext();
	void compareFiles();

	private:
	void loadAllRegistryFileLists();
	static void loadRegistryFileEntry(const char *baseName, const char *legacyCharset, unsigned u, FileSpec *entry, bool *foundLegacy);
	static void saveRegistryFileEntry(const char *baseName, unsigned u, const FileSpec &entry);
	void add2OpenFileList(WinEditChild	*editor);
	void saveOpenFileList();
	void addMenuFileEntryFromList( size_t id );
	void removeMenuFileEntry( size_t id );
	void addMenuFile( const FileSpec &fs );

	winlib::SuccessCode handleClose() override;
	winlib::ProcessStatus handleCommand( int command ) override;
	void handleFile( const char *, size_t, size_t ) override;
	void handleFocus() override;
	void handleTimer() override;
	winlib::ProcessStatus handleData(const ForeignWindow &src, gak::uint64 dataType, const void *data, size_t size) override;

	winlib::ProcessStatus ddeInitiate( const char *app, const char *topic ) override;
	winlib::ProcessStatus ddeExecute( const char *command ) override;


	public:
	WinEditMain();
	~WinEditMain();

	winlib::SuccessCode create( int nCmdShow );
	void openFile( const gak::STRING &fileName, winlib::EDIT::EditCharset charSet=winlib::EDIT::csANSI, size_t selStart=0, size_t selEnd=0 );
	void checkLineEnd( RLINE_ENDS lineEnd );
	void updateFileMenu( const gak::STRING &fileName, size_t selStart, size_t selEnd );
};

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

// --------------------------------------------------------------------- //
// ----- class inlines ------------------------------------------------- //
// --------------------------------------------------------------------- //

// --------------------------------------------------------------------- //
// ----- class constructors/destructors -------------------------------- //
// --------------------------------------------------------------------- //

// --------------------------------------------------------------------- //
// ----- class static functions ---------------------------------------- //
// --------------------------------------------------------------------- //

// --------------------------------------------------------------------- //
// ----- class privates ------------------------------------------------ //
// --------------------------------------------------------------------- //

// --------------------------------------------------------------------- //
// ----- class protected ----------------------------------------------- //
// --------------------------------------------------------------------- //

// --------------------------------------------------------------------- //
// ----- class virtuals ------------------------------------------------ //
// --------------------------------------------------------------------- //
   
// --------------------------------------------------------------------- //
// ----- class publics ------------------------------------------------- //
// --------------------------------------------------------------------- //

// --------------------------------------------------------------------- //
// ----- entry points -------------------------------------------------- //
// --------------------------------------------------------------------- //

#ifdef __BORLANDC__
#	pragma option -RT.
#	pragma option -b.
#	pragma option -a.
#	pragma option -p.
#endif

#endif	// WIN_EDIT_MAIN_H
