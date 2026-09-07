/*
		Project:		WINEDIT (Windows MDI-Editor)
		Module:			WinEditChild.cpp
		Description:	The MDI Document Window
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

#include "WinEditChild.h"

#include <gak/string.h>

#include <WINLIB/colors.h>

#include "winedit.gui.h"
#include "WinEditMain.h"

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

static const char BLOCK_MARKER[] = "\r\n\r\n";
static const char EOL_MARKER[] = "\r\n";

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

static void concatenate( STRING *block, const char *sep, const STRING &part )
{
	if( block->size() )
	{
		*block += sep;
		*block += part;
	}
	else
		*block = part;
}

static void concatenate( STRING *line, STRING *word, size_t lineLen, ArrayOfStrings *lines )
{
	size_t lineSize = line->size();
	size_t wordSize = word->size();

	if( lineSize && lineSize + 1 + wordSize > lineLen )
	{
		lines->addElement( *line );
		lineSize = 0;
		*line = "";
	}

	if( wordSize )
	{
		concatenate( line, " ", *word );
		lineSize = line->size();
		wordSize = 0;
		*word = "";
	}

	if( lineSize ) 
		lines->addElement( *line );
}

static void extractLines( const STRING &block, size_t lineLen, ArrayOfStrings *lines )
{
	lines->clear();
	STRING oneLine, oneWord;
	oneLine.setMinSize( lineLen );
	bool crlfDropped = false;
	for( const char *cp = block; *cp; ++cp )
	{
		char c = *cp;
		if( c == '\r' || c=='\n' )
			crlfDropped = true;
		else if( (isSpace( c ) || crlfDropped) && oneWord.size() )
		{
			if( oneLine.size() + 1 + oneWord.size() > lineLen )
			{
				lines->addElement( oneLine );
				oneLine = oneWord;
				oneWord = "";
			}
			else
			{
				concatenate( &oneLine, " ", oneWord );
				oneWord = "";
			}
			crlfDropped = false;
		}
		if( !isSpace( c ) )
		{
			crlfDropped = false;
			oneWord += c;
		}
	}
	concatenate( &oneLine, &oneWord, lineLen, lines );
}

static void rightJustify( ArrayOfStrings &lines, size_t lineLen )
{
	for(
		ArrayOfStrings::iterator it = lines.begin(), endIT = lines.end();
		it != endIT;
		++it
	)
	{
		if( it->size() < lineLen )
			it->insChar(0, ' ', lineLen - it->size() );
	}
}

static void centerJustify( ArrayOfStrings &lines, size_t lineLen )
{
	for(
		ArrayOfStrings::iterator it = lines.begin(), endIT = lines.end();
		it != endIT;
		++it
	)
	{
		if( it->size() < lineLen )
			it->insChar(0, ' ', (lineLen - it->size())/2 );
	}
}

static void blockJustify( ArrayOfStrings &lines, size_t lineLen )
{
	if( !lines.size() )
		return;

	for(
		ArrayOfStrings::iterator it = lines.begin(), endIT = lines.end()-1;
		it != endIT;
		++it
	)
	{
		if( it->size() < lineLen )
		{
			size_t spaceLeft = lineLen - it->size();

			size_t spaceCount = 0;
			for( const char *cp = it->c_str(); *cp; ++cp )
			{
				if( *cp == ' ' )
				{
					++spaceCount;
				}
			}
			spaceLeft += spaceCount;

			double oneSpace = double(spaceLeft)/double(spaceCount);
			ArrayOfStrings	words;
			words.createElements( *it, " ", true );
			size_t spaceAdded = 0;
			double spaceRequired = 0;
			STRING newLine;
			for(
				ArrayOfStrings::iterator it2 = words.begin(), endIT2 = words.end();
				it2 != endIT2;
				++it2
			)
			{
				if( newLine.size() )
				{
					spaceRequired += oneSpace;
					size_t spaceRequiredI = size_t(spaceRequired+0.5);
					size_t thisNewSpace = spaceRequiredI-spaceAdded;
					newLine.add( ' ', thisNewSpace );
					spaceAdded += thisNewSpace;
				}
				newLine += *it2;
			}
			*it = newLine;
		}
	}
}

static STRING buildBlock( const ArrayOfStrings &lines )
{
	STRING	block;
	for(
		ArrayOfStrings::const_iterator it = lines.cbegin(), endIT = lines.cend();
		it != endIT;
		++it
	)
	{
		if( block.size() )
			block += EOL_MARKER;
		block += *it;
	}
	return block;
}

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
   
void WinEditChild::handleFocus()
{
	WinEditMain	*mainWindow = (WinEditMain *)getMDIparent();

	enum EDIT::EditCharset charSet = getCharset();

	if( charSet == EDIT::csOEM )
		mainWindow->getMenu()->checkMenuItem( EDIT_OEM_CONVERT_id );
	else
		mainWindow->getMenu()->uncheckMenuItem( EDIT_OEM_CONVERT_id );

	if( charSet == EDIT::csANSI )
		mainWindow->getMenu()->checkMenuItem( EDIT_CS_ANSI_id );
	else
		mainWindow->getMenu()->uncheckMenuItem( EDIT_CS_ANSI_id );

	if( charSet == EDIT::csUTF8 )
	{
		mainWindow->getMenu()->checkMenuItem( EDIT_CS_UTF8_id );
		if( withBOM() )
		{
			mainWindow->getMenu()->checkMenuItem( EDIT_CS_UTF8_BOM_id );
		}
		else
		{
			mainWindow->getMenu()->uncheckMenuItem( EDIT_CS_UTF8_BOM_id );
		}
	}
	else
	{
		mainWindow->getMenu()->uncheckMenuItem( EDIT_CS_UTF8_id );
		mainWindow->getMenu()->uncheckMenuItem( EDIT_CS_UTF8_BOM_id );
	}
	if( isLineBreak() )
		mainWindow->getMenu()->checkMenuItem( EDIT_BREAK_id );
	else
		mainWindow->getMenu()->uncheckMenuItem( EDIT_BREAK_id );
	if( !isEditable() )
		mainWindow->getMenu()->checkMenuItem( EDIT_READ_ONLY_id );
	else
		mainWindow->getMenu()->uncheckMenuItem( EDIT_READ_ONLY_id );

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
	WinEditMain *mainWindow = (WinEditMain *)getMDIparent();

	if( isChanged() )
	{
		STRING	theMessage = appObject->loadString( CLOSE_ANYWAY_id );
		STRING	theFile = getFileName();
		if( theMessage.size() + theFile.size() > 1024 )
		{
			theFile = STRING("...") + theFile.rightString( 1021 - theMessage.strlen() );
		}
		theMessage.searchNreplace( "%s", !theFile.isEmpty() ? theFile : appObject->loadString( UNTITLED_id ) );
		int button = messageBox( theMessage, WARNING_id, MB_APPLMODAL|MB_ICONSTOP|MB_YESNOCANCEL );

		if( button == IDCANCEL )
/*@*/		return scERROR;

		if( button == IDYES )
		{
			mainWindow->saveFile(this);
			if( isChanged() )
/*@*/			return scERROR;
		}
	}

	size_t selStart, selEnd;
	getSelection(&selStart, &selEnd);
	mainWindow->updateFileMenu( getFileName(), selStart, selEnd );

	close();
	return scSUCCESS;
}

// --------------------------------------------------------------------- //
// ----- class publics ------------------------------------------------- //
// --------------------------------------------------------------------- //

void WinEditChild::changeReadOnly()
{
	STRING fileName = getFileName();
	if( !fileName.isEmpty() )
	{
		unsigned long attr = GetFileAttributes( fileName );
		if( attr & FILE_ATTRIBUTE_READONLY )
		{
			attr &= ~FILE_ATTRIBUTE_READONLY;
			setEditable();
			setBackgroundColorByRef(colors::WHITE);
		}
		else if( !isChanged() )
		{
			attr |= FILE_ATTRIBUTE_READONLY;
			setReadonly();
			setBackgroundColorByRef(colors::LIGHT_SALMON);
		}
		SetFileAttributes( fileName, attr );
		handleFocus();
	}
}

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

	size_t			start, end;
	size_t			line = getControl()->getFirstVisible();
	getSelection(&start, &end);

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
			if( start > newText.strlen() )
			{
				start += entity.strlen()-1;
			}
			if( end > newText.strlen() )
			{
				end += entity.strlen()-1;
			}
			newText += entity;
			if( strlen(entity) > 1 )
				changedFlag = true;
		}
	}

	if( changedFlag )
	{
		setText( newText );
		setChanged();
		getControl()->setFirstVisible( line );
		setSelection(start, end);
	}
}

void WinEditChild::formatBlock( BlockFormat format, size_t lineLen )
{
	bool			found = false;
	size_t			start, end;
	size_t			line = getControl()->getFirstVisible();
	getSelection(&start, &end);
	STRING			text = getText();

	if( start )
	{
		--start;
		for( const char *cp = text+start; ; --start, --cp )
		{
			if( !strncmp(cp, BLOCK_MARKER, sizeof(BLOCK_MARKER)-1 ) )
			{
				found = true;
/*v*/			break;
			}
			if( !start )
			{
				found = false;
/*v*/			break;
			}
		}
		if( found )
			start += sizeof(BLOCK_MARKER)-1;
	}
	if( end < start )
	{
		end = start;
	}
	for( const char *cp = text+end; end < text.size(); ++end, ++cp )
	{
		if( !strncmp(cp, BLOCK_MARKER, sizeof(BLOCK_MARKER)-1 ) )
		{
/*v*/		break;
		}
	}

	if( start < end )
	{
		STRING			oldText = text.subString( start, end-start );
		ArrayOfStrings	lines;
		extractLines( oldText, lineLen, &lines );
		if( format == bfRIGHT )
			rightJustify(  lines, lineLen );
		else if( format == bfCENTER )
			centerJustify(  lines, lineLen );
		else if( format == bfBLOCK )
			blockJustify(  lines, lineLen );

		STRING newText = buildBlock( lines );

		text.replaceText(start, end-start, newText );

		setText( text );
		setChanged();
		getControl()->setFirstVisible( line );
		end = start + newText.size();
		setSelection(start, end);
	}
}

void WinEditChild::convertIndent( int command )
{
	int				tabWidth = getTabWidth();
	bool			toSpaces = command == EDIT_SPACE_INDENT_id;
	size_t			start, end;
	size_t			line = getControl()->getFirstVisible();
	getSelection(&start, &end);
	STRING			text = getText();
	STRING			block = (start==end) ? text : text.subString( start, end-start );
	if( stripLineEnds() )
	{
		block.stripLineEnds();
	}
	STRING			newBlock;
	newBlock.setMinSize( block.size() );
	bool			startFlag = true;
	size_t			curIndent = 0;
	for( const char *cp = block.c_str(); *cp; ++cp )
	{
		if( startFlag && (*cp=='\t' || *cp==' ') )
		{
			if( toSpaces )
			{
				if( *cp=='\t' )
				{
					newBlock.add( ' ', tabWidth-curIndent%tabWidth );
					curIndent = 0;
				}
				else
				{
					newBlock.add( ' ' );
					++curIndent;
				}
			}
			else
			{
				if( *cp=='\t' )
				{
					curIndent += tabWidth;
					curIndent -= curIndent%tabWidth;
				}
				else
				{
					++curIndent;
				}
			}
		}
		else
		{
			if( startFlag && !toSpaces && curIndent )
			{
				newBlock.add( '\t', curIndent/tabWidth );
				newBlock.add( ' ', curIndent%tabWidth );
			}
			newBlock += *cp;
			startFlag = (*cp == '\r' || *cp == '\n');
			curIndent = 0;	// now we are within the line and we must reset the current indent
		}
	}

	if( start == end )
	{
		setText( newBlock );
	}
	else
	{
		text.replaceText(start, end-start, newBlock );
		setText( text );
		end = start + newBlock.size();
	}
	setChanged();
	getControl()->setFirstVisible( line );
	setSelection(start, end);
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
