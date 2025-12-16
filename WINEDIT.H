/*
		Project:		WINEDIT (Windows MDI-Editor)
		Module: 		WINEDIT.H
		Description:	The IDs for the ressource objects
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

/*
	Dialogs, menus
*/

#define MAIN_MENU			100

/*
	menu items
*/

#define FILE_NEW			101
#define FILE_OPEN			102
#define FILE_SAVE			103
#define FILE_SAVE_AS		104
#define FILE_SAVE_ALL		105
#define FILE_QUIT			109
#define FILE_PREFERENCES	110
#define FILE_FONT			111
#define FILE_PRINT			112

#define EDIT_UNDO			201
#define EDIT_CUT			202
#define EDIT_COPY			203
#define EDIT_PASTE			204
#define EDIT_CLEAR			205
#define EDIT_LINE_CRLF		206
#define EDIT_LINE_LF		207
#define EDIT_LINE_CR		208
#define EDIT_LINE_LFCR		209
#define EDIT_OEM_CONVERT	210
#define EDIT_INDENT			211
#define EDIT_UNINDENT		212
#define EDIT_BREAK			213
#define EDIT_SELECT_ALL		214
#define EDIT_DELETE_LINE	215
#define EDIT_HTML_ENTITIES	216
#define EDIT_CS_ANSI		217
#define EDIT_CS_UTF8		218
#define EDIT_CS_UTF8_BOM	219
#define EDIT_READ_ONLY		220

#define SEARCH_SEARCH		301
#define SEARCH_REPLACE		302
#define SEARCH_NEXT			303
#define COMPARE_FILES		304

#define WINDOW_MENU			3
#define WIN_TILE_HORIZ		401
#define WIN_TILE_VERT		402
#define WIN_CASCADE			403
#define WIN_ARRANGE			404
#define WIN_CLOSE_ALL		405

#define INFO_ABOUT			501

/*
	dialog items
*/
#define CREATE_BACKUP		101
#define TABWIDTH			102
#define IDC_AUTO_INDENT		103
#define AUTO_RELOAD			104

#define INFO_WINDOWS_WIDTH	502

/*
	strings
*/
#define NOREGISTER			1
#define NOWINDOW			2
#define UNTITLED			3
#define WARNING				4
#define CLOSE_ANYWAY		5

#define STYLE_SHEETS		10
#define C_SOURCE_FILES		11
#define CG_CONFIG			12
#define CG_LOG				13
#define CONFIG_FILES		14
#define HEADER_FILES		15
#define HELP_PROJECTS		16
#define HTML_FILES			17
#define JAVA_SCRIPTS		18
#define JAVA_FILES			19
#define LIVELINK_FILES		20
#define MAKE_FILES			21
#define DEF_FILES			22
#define BATCH_FILES			23
#define OPL_FILES			24
#define PASCAL_FILES		25
#define PHP_FILES			26
#define PROC_FILES			27
#define PROT_FILES			28
#define REGISTRATION_FILES	29
#define RESSOURCE_FILES		30
#define SQL_SCRIPTS			31
#define SYMBOL_TABLES		32
#define TEXT_FILES			33
#define XML_FILES			34
#define XSD_FILES			35
#define XSL_FILES			36
#define DTD_FILES			37
#define ALL_FILES			38

