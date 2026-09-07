/*
		Project:		WINEDIT (Windows MDI-Editor)
		Module: 		WINEDIT.H
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
// ----- constants ----------------------------------------------------- //
// --------------------------------------------------------------------- //

namespace myProfile
{
	extern const char autoIndent[];
	extern const char autoReload[];
	extern const char backup[];
	extern const char charset[];
	extern const char directory[];
	extern const char filterIndex[];
	extern const char Font[];
	extern const char FontSize[];
	extern const char lastCharset[];
	extern const char lastFile[];
	extern const char filename[];
	extern const char openFile[];
	extern const char OpenFiles[];
	extern const char selEnd[];
	extern const char selStart[];
	extern const char stripLineEnds[];
	extern const char Style[];
	extern const char tabWidth[];
	extern const char lineWidth[];
	extern const char blockFormat[];
};

extern const char APP_NAME[];

#define EDIT_ICON			101
#define EDIT_ICON_DLG		102
#define WINDOW_MENU			3		/// TODO auto generate with GUI Builder if possible
