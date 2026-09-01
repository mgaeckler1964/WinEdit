/*
	this file was created from C:\CRESD\Source\WinEdit\winedit.gui with MG Gui Builder
	+++++++++++++++++++++++
	!!!!! Do not Edit !!!!!
	+++++++++++++++++++++++
*/

#include "C:\CRESD\Source\WinEdit\winedit.gui.h"

namespace winlibGUI {

	gak::xml::Document *GuiApplication::getGuiDoc() {
		gak::xml::Document *doc = winlib::Application::getGuiDoc();
		if(!doc) {
			gak::STRING xmlSrc = "<?xml version=\"1.0\" encoding=\"iso-8859-1\" ?>\n"
"<gui>\n"
"<forms>\n"
"<form name=\"PreferencesDlg\" style=\"281542656\" width=\"265\" height=\"145\" baseClass=\"ModalPopup\" id=\"128\" caption=\"Preferences\">\n"
"<child type=\"CheckBox\" name=\"autoIndent\" caption=\"Auto &amp;Indent\" x=\"8\" y=\"8\" width=\"80\" height=\"24\" style=\"1342242819\" id=\"131\" />\n"
"<child type=\"CheckBox\" name=\"backup\" caption=\"Create &amp;Backup\" x=\"8\" y=\"32\" width=\"104\" height=\"24\" style=\"1342242819\" id=\"132\" />\n"
"<child type=\"CheckBox\" name=\"autoReload\" caption=\"Auto &amp;Reload\" x=\"8\" y=\"56\" width=\"80\" height=\"24\" style=\"1342242819\" id=\"133\" />\n"
"<child type=\"CheckBox\" name=\"stripLineEnds\" caption=\"StripLineEnds\" x=\"8\" y=\"80\" width=\"80\" height=\"24\" style=\"1342242819\" id=\"134\" />\n"
"<child type=\"Label\" name=\"\" caption=\"Tab &amp;Width\" x=\"112\" y=\"8\" width=\"80\" height=\"24\" style=\"1342242827\" />\n"
"<child type=\"EditControl\" name=\"tabwidth\" caption=\"\" x=\"176\" y=\"8\" width=\"80\" height=\"24\" style=\"1350631808\" id=\"135\" />\n"
"<child type=\"PushButton\" name=\"IDOK\" caption=\"&amp;OK\" x=\"8\" y=\"112\" width=\"80\" height=\"24\" style=\"1342242817\" id=\"1\" />\n"
"<child type=\"PushButton\" name=\"IDCANCEL\" caption=\"&amp;Cancel\" x=\"96\" y=\"112\" width=\"80\" height=\"24\" style=\"1342242816\" id=\"2\" />\n"
"</form>\n"
"<form name=\"AboutDlg\" style=\"281542656\" width=\"288\" height=\"207\" id=\"129\" baseClass=\"ModalPopup\" caption=\"About  WinEdit\">\n"
"<child type=\"Label\" name=\"AboutLABEL\" caption=\"Label\" x=\"8\" y=\"8\" width=\"164\" height=\"161\" style=\"1342242816\" id=\"136\" />\n"
"<child type=\"Label\" name=\"CPUWidthLABEL\" caption=\"Label\" x=\"176\" y=\"8\" width=\"80\" height=\"25\" style=\"1342242827\" id=\"137\" />\n"
"<child type=\"PushButton\" name=\"IDOK\" caption=\"&amp;OK\" x=\"8\" y=\"176\" width=\"80\" height=\"25\" style=\"1342242817\" id=\"1\" />\n"
"<child type=\"Label\" name=\"\" caption=\"#102\" x=\"176\" y=\"32\" width=\"32\" height=\"32\" style=\"1342242819\" />\n"
"</form>\n"
"<form name=\"FormatBlockDlg\" style=\"281018368\" width=\"251\" height=\"128\" id=\"130\" baseClass=\"ModalPopup\">\n"
"<child type=\"Label\" name=\"lwLABEL\" caption=\"Line &amp;Width\" x=\"8\" y=\"8\" width=\"0\" height=\"0\" style=\"1342373899\" id=\"138\" />\n"
"<child type=\"EditControl\" name=\"linewidth\" caption=\"\" x=\"80\" y=\"8\" width=\"80\" height=\"24\" style=\"1350631808\" id=\"139\" />\n"
"<child type=\"RadioButton\" name=\"leftBtn\" caption=\"&amp;Left\" x=\"8\" y=\"48\" width=\"60\" height=\"24\" style=\"1342242825\" id=\"140\" />\n"
"<child type=\"RadioButton\" name=\"blockBtn\" caption=\"&amp;Block\" x=\"64\" y=\"48\" width=\"64\" height=\"24\" style=\"1342242825\" id=\"141\" />\n"
"<child type=\"RadioButton\" name=\"centerBtn\" caption=\"&amp;Center\" x=\"128\" y=\"48\" width=\"64\" height=\"24\" style=\"1342242825\" id=\"142\" />\n"
"<child type=\"RadioButton\" name=\"rightBtn\" caption=\"&amp;Right\" x=\"192\" y=\"48\" width=\"80\" height=\"24\" style=\"1342242825\" id=\"143\" />\n"
"<child type=\"PushButton\" name=\"IDOK\" caption=\"&amp;OK\" x=\"8\" y=\"88\" width=\"80\" height=\"24\" style=\"1342242817\" id=\"1\" />\n"
"<child type=\"PushButton\" name=\"IDCANCEL\" caption=\"C&amp;ancel\" x=\"96\" y=\"88\" width=\"80\" height=\"24\" style=\"1342242816\" id=\"2\" />\n"
"</form>\n"
"</forms>\n"
"</gui>";
			return Application::getGuiDoc( xmlSrc );
		}
		return doc;
	}


	void PreferencesDlg_form::getControls() {
		autoIndent=static_cast<winlib::CheckBox*>(findChild(autoIndent_id));
		backup=static_cast<winlib::CheckBox*>(findChild(backup_id));
		autoReload=static_cast<winlib::CheckBox*>(findChild(autoReload_id));
		stripLineEnds=static_cast<winlib::CheckBox*>(findChild(stripLineEnds_id));
		tabwidth=static_cast<winlib::EditControl*>(findChild(tabwidth_id));
		IDOK_PushButton=static_cast<winlib::PushButton*>(findChild(IDOK));
		IDCANCEL_PushButton=static_cast<winlib::PushButton*>(findChild(IDCANCEL));
	}	// PreferencesDlg

	void AboutDlg_form::getControls() {
		AboutLABEL=static_cast<winlib::Label*>(findChild(AboutLABEL_id));
		CPUWidthLABEL=static_cast<winlib::Label*>(findChild(CPUWidthLABEL_id));
		IDOK_PushButton=static_cast<winlib::PushButton*>(findChild(IDOK));
	}	// AboutDlg

	void FormatBlockDlg_form::getControls() {
		lwLABEL=static_cast<winlib::Label*>(findChild(lwLABEL_id));
		linewidth=static_cast<winlib::EditControl*>(findChild(linewidth_id));
		leftBtn=static_cast<winlib::RadioButton*>(findChild(leftBtn_id));
		blockBtn=static_cast<winlib::RadioButton*>(findChild(blockBtn_id));
		centerBtn=static_cast<winlib::RadioButton*>(findChild(centerBtn_id));
		rightBtn=static_cast<winlib::RadioButton*>(findChild(rightBtn_id));
		IDOK_PushButton=static_cast<winlib::PushButton*>(findChild(IDOK));
		IDCANCEL_PushButton=static_cast<winlib::PushButton*>(findChild(IDCANCEL));
	}	// FormatBlockDlg
}	// namespace winlibGUI
