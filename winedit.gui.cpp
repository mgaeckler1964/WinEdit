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
"<child type=\"CheckBox\" name=\"autoIndent\" caption=\"Auto &amp;Indent\" x=\"8\" y=\"8\" width=\"80\" height=\"24\" style=\"1342242819\" id=\"173\" />\n"
"<child type=\"CheckBox\" name=\"backup\" caption=\"Create &amp;Backup\" x=\"8\" y=\"32\" width=\"104\" height=\"24\" style=\"1342242819\" id=\"174\" />\n"
"<child type=\"CheckBox\" name=\"autoReload\" caption=\"Auto &amp;Reload\" x=\"8\" y=\"56\" width=\"80\" height=\"24\" style=\"1342242819\" id=\"175\" />\n"
"<child type=\"CheckBox\" name=\"stripLineEnds\" caption=\"StripLineEnds\" x=\"8\" y=\"80\" width=\"80\" height=\"24\" style=\"1342242819\" id=\"176\" />\n"
"<child type=\"Label\" name=\"\" caption=\"Tab &amp;Width\" x=\"112\" y=\"8\" width=\"80\" height=\"24\" style=\"1342242827\" />\n"
"<child type=\"EditControl\" name=\"tabwidth\" caption=\"\" x=\"176\" y=\"8\" width=\"80\" height=\"24\" style=\"1350631808\" id=\"177\" />\n"
"<child type=\"PushButton\" name=\"IDOK\" caption=\"&amp;OK\" x=\"8\" y=\"112\" width=\"80\" height=\"24\" style=\"1342242817\" id=\"1\" />\n"
"<child type=\"PushButton\" name=\"IDCANCEL\" caption=\"&amp;Cancel\" x=\"96\" y=\"112\" width=\"80\" height=\"24\" style=\"1342242816\" id=\"2\" />\n"
"</form>\n"
"<form name=\"AboutDlg\" style=\"281542656\" width=\"288\" height=\"207\" id=\"129\" baseClass=\"ModalPopup\" caption=\"About  WinEdit\">\n"
"<child type=\"Label\" name=\"AboutLABEL\" caption=\"Label\" x=\"8\" y=\"8\" width=\"164\" height=\"161\" style=\"1342242816\" id=\"178\" />\n"
"<child type=\"Label\" name=\"CPUWidthLABEL\" caption=\"Label\" x=\"176\" y=\"8\" width=\"80\" height=\"25\" style=\"1342242827\" id=\"179\" />\n"
"<child type=\"PushButton\" name=\"IDOK\" caption=\"&amp;OK\" x=\"8\" y=\"176\" width=\"80\" height=\"25\" style=\"1342242817\" id=\"1\" />\n"
"<child type=\"Label\" name=\"\" caption=\"#102\" x=\"176\" y=\"32\" width=\"32\" height=\"32\" style=\"1342242819\" />\n"
"</form>\n"
"<form name=\"FormatBlockDlg\" style=\"281018368\" width=\"251\" height=\"128\" id=\"130\" baseClass=\"ModalPopup\">\n"
"<child type=\"Label\" name=\"lwLABEL\" caption=\"Line &amp;Width\" x=\"8\" y=\"8\" width=\"0\" height=\"0\" style=\"1342373899\" id=\"180\" />\n"
"<child type=\"EditControl\" name=\"linewidth\" caption=\"\" x=\"80\" y=\"8\" width=\"80\" height=\"24\" style=\"1350631808\" id=\"181\" />\n"
"<child type=\"RadioButton\" name=\"leftBtn\" caption=\"&amp;Left\" x=\"8\" y=\"48\" width=\"60\" height=\"24\" style=\"1342242825\" id=\"182\" />\n"
"<child type=\"RadioButton\" name=\"blockBtn\" caption=\"&amp;Block\" x=\"64\" y=\"48\" width=\"64\" height=\"24\" style=\"1342242825\" id=\"183\" />\n"
"<child type=\"RadioButton\" name=\"centerBtn\" caption=\"&amp;Center\" x=\"128\" y=\"48\" width=\"64\" height=\"24\" style=\"1342242825\" id=\"184\" />\n"
"<child type=\"RadioButton\" name=\"rightBtn\" caption=\"&amp;Right\" x=\"192\" y=\"48\" width=\"80\" height=\"24\" style=\"1342242825\" id=\"185\" />\n"
"<child type=\"PushButton\" name=\"IDOK\" caption=\"&amp;OK\" x=\"8\" y=\"88\" width=\"80\" height=\"24\" style=\"1342242817\" id=\"1\" />\n"
"<child type=\"PushButton\" name=\"IDCANCEL\" caption=\"C&amp;ancel\" x=\"96\" y=\"88\" width=\"80\" height=\"24\" style=\"1342242816\" id=\"2\" />\n"
"</form>\n"
"</forms>\n"
"<menus>\n"
"<menu name=\"MENU\" id=\"172\">\n"
"<menu caption=\"&amp;File\" name=\"\">\n"
"<menu caption=\"&amp;New\" name=\"FILE_NEW\" id=\"131\" />\n"
"<menu caption=\"&amp;Open...\" name=\"FILE_OPEN\" id=\"132\" />\n"
"<menu caption=\"&amp;Save\" name=\"FILE_SAVE\" id=\"133\" />\n"
"<menu caption=\"Save &amp;as...\" name=\"FILE_SAVE_AS\" id=\"134\" />\n"
"<menu caption=\"Save a&amp;ll\" name=\"FILE_SAVE_ALL\" id=\"135\" />\n"
"<menu caption=\"-\" />\n"
"<menu caption=\"&amp;Print..\" name=\"FILE_PRINT\" id=\"136\" />\n"
"<menu caption=\"-\" />\n"
"<menu caption=\"&amp;Preferences...\" name=\"FILE_PREFERENCES\" id=\"137\" />\n"
"<menu caption=\"&amp;Font...\" name=\"FILE_FONT\" id=\"138\" />\n"
"<menu caption=\"-\" />\n"
"<menu caption=\"&amp;Quit\" name=\"FILE_QUIT\" id=\"139\" />\n"
"</menu>\n"
"<menu caption=\"&amp;Edit\" name=\"\">\n"
"<menu caption=\"&amp;Undo\" name=\"EDIT_UNDO\" id=\"140\" />\n"
"<menu caption=\"-\" />\n"
"<menu caption=\"&amp;Cut\" name=\"EDIT_CUT\" id=\"141\" />\n"
"<menu caption=\"C&amp;opy\" name=\"EDIT_COPY\" id=\"142\" />\n"
"<menu caption=\"&amp;Paste\" name=\"EDIT_PASTE\" id=\"143\" />\n"
"<menu caption=\"C&amp;lear\" name=\"EDIT_CLEAR\" id=\"144\" />\n"
"<menu caption=\"-\" />\n"
"<menu caption=\"Select &amp;All\" name=\"EDIT_SELECT_ALL\" id=\"145\" />\n"
"<menu caption=\"&amp;Delete Line\" name=\"EDIT_DELETE_LINE\" id=\"146\" />\n"
"<menu caption=\"-\" />\n"
"<menu caption=\"Line CRLF\" name=\"EDIT_LINE_CRLF\" id=\"147\" />\n"
"<menu caption=\"Line LF\" name=\"EDIT_LINE_LF\" id=\"148\" />\n"
"<menu caption=\"Line CR\" name=\"EDIT_LINE_CR\" id=\"149\" />\n"
"<menu caption=\"Line LFCR\" name=\"EDIT_LINE_LFCR\" id=\"150\" />\n"
"<menu caption=\"-\" />\n"
"<menu caption=\"ANSI\" name=\"EDIT_CS_ANSI\" id=\"151\" />\n"
"<menu caption=\"UTF-8\" name=\"EDIT_CS_UTF8\" id=\"152\" />\n"
"<menu caption=\"    with BOM\" name=\"EDIT_CS_UTF8_BOM\" id=\"153\" />\n"
"<menu caption=\"OE&amp;M (Dos)\" name=\"EDIT_OEM_CONVERT\" id=\"154\" />\n"
"<menu caption=\"-\" />\n"
"<menu caption=\"&amp;HTML-Entities\" name=\"EDIT_HTML_ENTITIES\" id=\"155\" />\n"
"<menu caption=\"-\" />\n"
"<menu caption=\"&amp;Indent\" name=\"EDIT_INDENT\" id=\"156\" />\n"
"<menu caption=\"U&amp;nindent\" name=\"EDIT_UNINDENT\" id=\"157\" />\n"
"<menu caption=\"&amp;Tab Indent\" name=\"EDIT_TAB_INDENT\" id=\"158\" />\n"
"<menu caption=\"&amp;Space Indent\" name=\"EDIT_SPACE_INDENT\" id=\"159\" />\n"
"<menu caption=\"&amp;Format...\" name=\"EDIT_FORMAT\" id=\"160\" />\n"
"<menu caption=\"-\" />\n"
"<menu caption=\"&amp;Read-Only\" name=\"EDIT_READ_ONLY\" id=\"161\" />\n"
"</menu>\n"
"<menu caption=\"&amp;Search\">\n"
"<menu caption=\"&amp;Search...\" name=\"SEARCH_SEARCH\" id=\"162\" />\n"
"<menu caption=\"&amp;Replace...\" name=\"SEARCH_REPLACE\" id=\"163\" />\n"
"<menu caption=\"&amp;Next\" name=\"SEARCH_NEXT\" id=\"164\" />\n"
"<menu caption=\"-\" />\n"
"<menu caption=\"Compare\" name=\"COMPARE_FILES\" id=\"165\" />\n"
"</menu>\n"
"<menu caption=\"&amp;Window\">\n"
"<menu caption=\"Tile &amp;horizontal\" name=\"WIN_TILE_HORIZ\" id=\"166\" />\n"
"<menu caption=\"Tile &amp;vertical\" name=\"WIN_TILE_VERT\" id=\"167\" />\n"
"<menu caption=\"&amp;Cascade\" name=\"WIN_CASCADE\" id=\"168\" />\n"
"<menu caption=\"&amp;Arange icons\" name=\"WIN_ARRANGE\" id=\"169\" />\n"
"<menu caption=\"-\" />\n"
"<menu caption=\"Close A&amp;ll\" name=\"WIN_CLOSE_ALL\" id=\"170\" />\n"
"</menu>\n"
"<menu caption=\"&amp;Info\">\n"
"<menu caption=\"&amp;About Editor...\" name=\"INFO_ABOUT\" id=\"171\" />\n"
"</menu>\n"
"</menu>\n"
"</menus>\n"
"<strings>\n"
"<list name=\"STRINGLIST\">\n"
"<string name=\"NOREGISTER\" caption=\"Unable to register window class\" id=\"186\" />\n"
"<string name=\"NOWINDOW\" caption=\"Unable to open window\" id=\"187\" />\n"
"<string name=\"UNTITLED\" caption=\"Untitled\" id=\"188\" />\n"
"<string name=\"WARNING\" caption=\"Warning\" id=\"189\" />\n"
"<string name=\"CLOSE_ANYWAY\" caption=\"File %s not saved. Save now?\" id=\"190\" />\n"
"</list>\n"
"<list name=\"FileTypeList\">\n"
"<string name=\"STYLE_SHEETS\" caption=\"Cascading Styles(*.css)|*.css|\" id=\"191\" />\n"
"<string name=\"C_SOURCE_FILES\" caption=\"C(++)-Source(*.cpp;*.c)|*.cpp;*.c|\" id=\"192\" />\n"
"<string name=\"CG_CONFIG\" caption=\"Code Guard Config(*.cgi)|*.cgi|\" id=\"193\" />\n"
"<string name=\"CG_LOG\" caption=\"Code Guard Log(*.cgl)|*.cgl|\" id=\"194\" />\n"
"<string name=\"CONFIG_FILES\" caption=\"Configurationfiles(*.ini;*.cfg;*.inf)|*.ini;*.cfg;*.inf|\" id=\"195\" />\n"
"<string name=\"HEADER_FILES\" caption=\"Headerfiles(*.hpp;*.h;*.rh)|*.hpp;*.h;*.rh|\" id=\"196\" />\n"
"<string name=\"HELP_PROJECTS\" caption=\"Help Projects(*.hpj)|*.hpj|\" id=\"197\" />\n"
"<string name=\"HTML_FILES\" caption=\"HTML Files(*.html;*.htm)|*.html;*.htm|\" id=\"198\" />\n"
"<string name=\"JAVA_SCRIPTS\" caption=\"Java-Scripts(*.js)|*.js|\" id=\"199\" />\n"
"<string name=\"JAVA_FILES\" caption=\"Java-Source(*.java)|*.java|\" id=\"200\" />\n"
"<string name=\"LIVELINK_FILES\" caption=\"Livelink(*.out)|*.out|\" id=\"201\" />\n"
"<string name=\"MAKE_FILES\" caption=\"Makefiles(*.mak)|*.mak|\" id=\"202\" />\n"
"<string name=\"DEF_FILES\" caption=\"Module Definitionfiles(*.def)|*.def|\" id=\"203\" />\n"
"<string name=\"BATCH_FILES\" caption=\"MSDOS-Batch files(*.bat;*.cmd)|*.bat;*.cmd|\" id=\"204\" />\n"
"<string name=\"OPL_FILES\" caption=\"OpenUI Source(*.opl)|*.opl|\" id=\"205\" />\n"
"<string name=\"PASCAL_FILES\" caption=\"Pascal-Source(*.pas)|*.pas|\" id=\"206\" />\n"
"<string name=\"PHP_FILES\" caption=\"PHP-Source(*.php)|*.php|\" id=\"207\" />\n"
"<string name=\"PROC_FILES\" caption=\"Pro*C/C++ spource(*.pc)|*.pc|\" id=\"208\" />\n"
"<string name=\"PROT_FILES\" caption=\"Protocols(*.log)|*.log|\" id=\"209\" />\n"
"<string name=\"REGISTRATION_FILES\" caption=\"Registrations (*.reg)|*.reg|\" id=\"210\" />\n"
"<string name=\"RESSOURCE_FILES\" caption=\"Ressource(*.rc)|*.rc|\" id=\"211\" />\n"
"<string name=\"SQL_SCRIPTS\" caption=\"SQL Scripts(*.sql)|*.sql|\" id=\"212\" />\n"
"<string name=\"SYMBOL_TABLES\" caption=\"Symbol Tables(*.map)|*.map|\" id=\"213\" />\n"
"<string name=\"TEXT_FILES\" caption=\"Text files(*.txt)|*.txt|\" id=\"214\" />\n"
"<string name=\"XML_FILES\" caption=\"XML files(*.xml)|*.xml|\" id=\"215\" />\n"
"<string name=\"XSD_FILES\" caption=\"XML Schema(*.xsd)|*.xsd|\" id=\"216\" />\n"
"<string name=\"XSL_FILES\" caption=\"XSL Stylesheets(*.xsl)|*.xsl|\" id=\"217\" />\n"
"<string name=\"DTD_FILES\" caption=\"XML Document Type Definitions(*.dtd)|*.dtd|\" id=\"218\" />\n"
"<string name=\"ALL_FILES\" caption=\"All files|*.*|\" id=\"219\" />\n"
"</list>\n"
"</strings>\n"
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
	const int STRINGLIST_ids[] = {
		NOREGISTER_id,
		NOWINDOW_id,
		UNTITLED_id,
		WARNING_id,
		CLOSE_ANYWAY_id,
	};
	const int FileTypeList_ids[] = {
		STYLE_SHEETS_id,
		C_SOURCE_FILES_id,
		CG_CONFIG_id,
		CG_LOG_id,
		CONFIG_FILES_id,
		HEADER_FILES_id,
		HELP_PROJECTS_id,
		HTML_FILES_id,
		JAVA_SCRIPTS_id,
		JAVA_FILES_id,
		LIVELINK_FILES_id,
		MAKE_FILES_id,
		DEF_FILES_id,
		BATCH_FILES_id,
		OPL_FILES_id,
		PASCAL_FILES_id,
		PHP_FILES_id,
		PROC_FILES_id,
		PROT_FILES_id,
		REGISTRATION_FILES_id,
		RESSOURCE_FILES_id,
		SQL_SCRIPTS_id,
		SYMBOL_TABLES_id,
		TEXT_FILES_id,
		XML_FILES_id,
		XSD_FILES_id,
		XSL_FILES_id,
		DTD_FILES_id,
		ALL_FILES_id,
	};
}	// namespace winlibGUI
