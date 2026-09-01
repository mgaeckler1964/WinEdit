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
	const int AboutLABEL_id=136;
	const int autoIndent_id=131;
	const int autoReload_id=133;
	const int backup_id=132;
	const int blockBtn_id=141;
	const int centerBtn_id=142;
	const int CPUWidthLABEL_id=137;
	const int FormatBlockDlg_id=130;
	const int leftBtn_id=140;
	const int linewidth_id=139;
	const int lwLABEL_id=138;
	const int PreferencesDlg_id=128;
	const int rightBtn_id=143;
	const int stripLineEnds_id=134;
	const int tabwidth_id=135;

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
}	// namespace winlibGUI

#endif // C__CRESD_SOURCE_WINEDIT_WINEDIT_GUI_H
