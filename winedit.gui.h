/*
	this file was created from C:\CRESD\Source\WinEdit\winedit.gui with MG Gui Builder
	+++++++++++++++++++++++
	!!!!! Do not Edit !!!!!
	+++++++++++++++++++++++
*/
#ifndef C__CRESD_SOURCE_WINEDIT_WINEDIT_GUI_H
#define C__CRESD_SOURCE_WINEDIT_WINEDIT_GUI_H
#include <winlib/popup.h>
#include <winlib/frame.h>
#include <winlib/scrollFrame.h>
#include <winlib/ControlW.h>
#include <winlib/xmlEditorChild.h>
#include <winlib/chartWin.h>
#include <winlib/gridView.h>

#include <winlib/winApp.h>

namespace winlibGUI {

	const int AboutDlg_id=129;
	const int AboutLABEL_id=178;
	const int ALL_FILES_id=220;
	const int autoIndent_id=173;
	const int autoReload_id=175;
	const int backup_id=174;
	const int BATCH_FILES_id=205;
	const int blockBtn_id=183;
	const int centerBtn_id=184;
	const int CG_CONFIG_id=193;
	const int CG_LOG_id=194;
	const int CLOSE_ANYWAY_id=190;
	const int COMPARE_FILES_id=165;
	const int CONFIG_FILES_id=195;
	const int CPUWidthLABEL_id=179;
	const int C_SOURCE_FILES_id=191;
	const int DEF_FILES_id=204;
	const int DTD_FILES_id=216;
	const int EDIT_CLEAR_id=144;
	const int EDIT_COPY_id=142;
	const int EDIT_CS_ANSI_id=151;
	const int EDIT_CS_UTF8_id=152;
	const int EDIT_CS_UTF8_BOM_id=153;
	const int EDIT_CUT_id=141;
	const int EDIT_DELETE_LINE_id=146;
	const int EDIT_FORMAT_id=160;
	const int EDIT_HTML_ENTITIES_id=155;
	const int EDIT_INDENT_id=156;
	const int EDIT_LINE_CR_id=149;
	const int EDIT_LINE_CRLF_id=147;
	const int EDIT_LINE_LF_id=148;
	const int EDIT_LINE_LFCR_id=150;
	const int EDIT_OEM_CONVERT_id=154;
	const int EDIT_PASTE_id=143;
	const int EDIT_READ_ONLY_id=161;
	const int EDIT_SELECT_ALL_id=145;
	const int EDIT_SPACE_INDENT_id=159;
	const int EDIT_TAB_INDENT_id=158;
	const int EDIT_UNDO_id=140;
	const int EDIT_UNINDENT_id=157;
	const int FILE_FONT_id=138;
	const int FILE_NEW_id=131;
	const int FILE_OPEN_id=132;
	const int FILE_PREFERENCES_id=137;
	const int FILE_PRINT_id=136;
	const int FILE_QUIT_id=139;
	const int FILE_SAVE_id=133;
	const int FILE_SAVE_ALL_id=135;
	const int FILE_SAVE_AS_id=134;
	const int FormatBlockDlg_id=130;
	const int GITHUB_FILES_id=196;
	const int HEADER_FILES_id=197;
	const int HELP_PROJECTS_id=198;
	const int HTML_FILES_id=199;
	const int INFO_ABOUT_id=171;
	const int JAVA_FILES_id=201;
	const int JAVA_SCRIPTS_id=200;
	const int leftBtn_id=182;
	const int linewidth_id=181;
	const int LIVELINK_FILES_id=202;
	const int lwLABEL_id=180;
	const int MAKE_FILES_id=203;
	const int MENU_id=172;
	const int NOREGISTER_id=186;
	const int NOWINDOW_id=187;
	const int OPL_FILES_id=206;
	const int PASCAL_FILES_id=207;
	const int PHP_FILES_id=208;
	const int PreferencesDlg_id=128;
	const int PROC_FILES_id=209;
	const int PROT_FILES_id=210;
	const int REGISTRATION_FILES_id=211;
	const int RESSOURCE_FILES_id=212;
	const int rightBtn_id=185;
	const int SEARCH_NEXT_id=164;
	const int SEARCH_REPLACE_id=163;
	const int SEARCH_SEARCH_id=162;
	const int SQL_SCRIPTS_id=213;
	const int stripLineEnds_id=176;
	const int STYLE_SHEETS_id=192;
	const int SYMBOL_TABLES_id=214;
	const int tabwidth_id=177;
	const int TEXT_FILES_id=215;
	const int UNTITLED_id=188;
	const int WARNING_id=189;
	const int WIN_ARRANGE_id=169;
	const int WIN_CASCADE_id=168;
	const int WIN_CLOSE_ALL_id=170;
	const int WIN_TILE_HORIZ_id=166;
	const int WIN_TILE_VERT_id=167;
	const int XML_FILES_id=217;
	const int XSD_FILES_id=218;
	const int XSL_FILES_id=219;

	class GuiApplication : public winlib::Application {
		public:
		virtual gak::xml::Document *getGuiDoc();
		GuiApplication(int iconID=-1) : winlib::Application(iconID) {}
	};

	class PreferencesDlg_form : public winlib::ModalPopup {
		public:
		PreferencesDlg_form(winlib::BasicWindow *owner) : ModalPopup(owner) {}
		winlib::SuccessCode create(winlib::BasicWindow*parent) {
			return ModalPopup::create(parent,"PreferencesDlg");
		}

		winlib::CheckBox *autoIndent;
		winlib::CheckBox *backup;
		winlib::CheckBox *autoReload;
		winlib::CheckBox *stripLineEnds;
		winlib::EditControl *tabwidth;
		winlib::PushButton *IDOK_PushButton;
		winlib::PushButton *IDCANCEL_PushButton;
		private:
		virtual void getControls();
	};	// end of PreferencesDlg

	class AboutDlg_form : public winlib::ModalPopup {
		public:
		AboutDlg_form(winlib::BasicWindow *owner) : ModalPopup(owner) {}
		winlib::SuccessCode create(winlib::BasicWindow*parent) {
			return ModalPopup::create(parent,"AboutDlg");
		}

		winlib::Label *AboutLABEL;
		winlib::Label *CPUWidthLABEL;
		winlib::PushButton *IDOK_PushButton;
		private:
		virtual void getControls();
	};	// end of AboutDlg

	class FormatBlockDlg_form : public winlib::ModalPopup {
		public:
		FormatBlockDlg_form(winlib::BasicWindow *owner) : ModalPopup(owner) {}
		winlib::SuccessCode create(winlib::BasicWindow*parent) {
			return ModalPopup::create(parent,"FormatBlockDlg");
		}

		winlib::Label *lwLABEL;
		winlib::EditControl *linewidth;
		winlib::RadioButton *leftBtn;
		winlib::RadioButton *blockBtn;
		winlib::RadioButton *centerBtn;
		winlib::RadioButton *rightBtn;
		winlib::PushButton *IDOK_PushButton;
		winlib::PushButton *IDCANCEL_PushButton;
		private:
		virtual void getControls();
	};	// end of FormatBlockDlg
	extern const int STRINGLIST_ids[];
	static const int STRINGLIST_count = 5;
	extern const int FileTypeList_ids[];
	static const int FileTypeList_count = 30;
}	// namespace winlibGUI

#endif // C__CRESD_SOURCE_WINEDIT_WINEDIT_GUI_H
