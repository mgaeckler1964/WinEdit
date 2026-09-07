/*
		Project:		WINEDIT (Windows MDI-Editor)
		Module:			Dialogs.cpp
		Description:	The dialogs for the editor
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

#include "Dialogs.h"
#include "WINEDIT.H"

#include <gak/fmtNumber.h>

#include <WINLIB/version.h>
#include <WINLIB/WINAPP.H>

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

using namespace winlib;
using namespace gak;

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
   
ProcessStatus AboutDlg::handleCreate()
{
	ProgramVersionInfo	vi(appObject->getFileName());

	STRING versionInfo = STRING(vi.getProductName()).add('\n')
		.add('\n')
		.add("From ").add( vi.getCompanyName() ).add('\n')
		.add("Version ").add( vi.getVersionString() ) .add('\n')
		.add( vi.getLegalCopyRight() ).add('\n')
		.add( vi.getFileDescription() ).add('\n')
		.add('\n')
		.add("Hofmannsthalweg 14\n")
		.add("A-4030 Linz\n")
		.add("Phone: +43 - 664 - 54 57 612\n")
		.add("E-Mail: martin@gaeckler.at\n")
		.add("https://www.gaeckler.at/")
	;

	CPUWidthLABEL->setText( getWindowsWidth() );
	AboutLABEL->setText(versionInfo);
	return psPROCESSED;
}

ProcessStatus PreferencesDlg::handleCreate()
{
	bool	backupFlag, autoIndentFlag, autoReloadFlag, stripLineEndsFlag;

	tabwidth->setText(formatNumber(appObject->GetProfile( nullptr, myProfile::tabWidth, 4)));

	backupFlag = appObject->GetProfile( nullptr, myProfile::backup, true );
	if( backupFlag )
		backup->setActive();
	else
		backup->clrActive();

	autoIndentFlag = appObject->GetProfile( nullptr, myProfile::autoIndent, true );
	if( autoIndentFlag )
		autoIndent->setActive();
	else
		autoIndent->clrActive();

	autoReloadFlag = appObject->GetProfile( nullptr, myProfile::autoReload, false );
	if( autoReloadFlag )
		autoReload->setActive();
	else
		autoReload->clrActive();

	stripLineEndsFlag = appObject->GetProfile( nullptr, myProfile::stripLineEnds, false );
	if( stripLineEndsFlag )
		stripLineEnds->setActive();
	else
		stripLineEnds->clrActive();

	return psPROCESSED;
}

ProcessStatus PreferencesDlg::handleOk()
{
	bool	backupFlag, autoIndentFlag, autoReloadFlag, stripLineEndsFlag;

	appObject->WriteProfile( false, "", myProfile::tabWidth, tabwidth->getText().getValueN<unsigned>() );

	backupFlag = backup->isActive();
	appObject->WriteProfile( false, "", myProfile::backup, backupFlag );

	autoIndentFlag = autoIndent->isActive();
	appObject->WriteProfile( false, "", myProfile::autoIndent, autoIndentFlag );

	autoReloadFlag = autoReload->isActive();
	appObject->WriteProfile( false, "", myProfile::autoReload, autoReloadFlag );

	stripLineEndsFlag = stripLineEnds->isActive();
	appObject->WriteProfile( false, "", myProfile::stripLineEnds, stripLineEndsFlag );

	return PreferencesDlg_form::handleOk();
}

ProcessStatus FormatBlockDlg::handleCreate()
{
	m_lineLen = appObject->GetProfile( nullptr, myProfile::lineWidth, 0);
	m_format =  (WinEditChild::BlockFormat)appObject->GetProfile( nullptr, myProfile::blockFormat, WinEditChild::bfLEFT);

	linewidth->setText(formatNumber(m_lineLen));

	if( m_format == WinEditChild::bfBLOCK )
		blockBtn->setActive();
	else if( m_format == WinEditChild::bfCENTER )
		centerBtn->setActive();
	else if( m_format == WinEditChild::bfRIGHT )
		rightBtn->setActive();
	else
		leftBtn->setActive();

	lwLABEL->move( size_t(0) );
	linewidth->move( size_t(1) );

	return psPROCESSED;
}

ProcessStatus FormatBlockDlg::handleOk()
{
	m_lineLen = linewidth->getText().getValueN<unsigned>();
	appObject-> WriteProfile( false, "", myProfile::lineWidth, m_lineLen );

	if( leftBtn->isActive() )
		m_format = WinEditChild::bfLEFT;
	else if( blockBtn->isActive() )
		m_format = WinEditChild::bfBLOCK;
	else if( centerBtn->isActive() )
		m_format = WinEditChild::bfCENTER;
	else if( rightBtn->isActive() )
		m_format = WinEditChild::bfRIGHT;
	else
		m_format = WinEditChild::bfUNKNOWN;
	appObject->WriteProfile( false, "", myProfile::blockFormat, m_format );

	return FormatBlockDlg_form::handleOk();
}

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
