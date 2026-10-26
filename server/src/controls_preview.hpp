// Native component preview; uses the same server controls as Ghost applications.
#ifndef FENSTER_CONTROLS_PREVIEW_HPP
#define FENSTER_CONTROLS_PREVIEW_HPP
#ifndef _GHOST_
#include "components/button.hpp"
#include "components/image.hpp"
#include "components/panel.hpp"
#include "components/text/text_box.hpp"
#include "components/window.hpp"
#include "server.hpp"
#include <libfenster/icons.hpp>

namespace fensterserver
{
inline void createControlsPreview()
{
	using namespace fenster;
	auto window = new Window(); window->setTitle("Navigator — component preview");
	window->setBounds({110, 100, 1060, 760});
	auto root = window->getPanel(); root->setBackground(_RGB(255, 255, 255));
	auto text = [](Panel* parent, const std::string& title, Rectangle box, int size = 14, ColorArgb color = _RGB(48, 57, 74)) {
		auto label = new Label(); label->setTitle(title); label->setBounds(box); label->setColor(color);
		label->setNumericProperty(ComponentProperty::FontSize, size); parent->addChild(label); return label;
	};
	auto control = [](Panel* parent, const std::string& title, Icon icon, Rectangle box) {
		auto button = new Button(); button->setTitle(title); button->setBounds(box);
		button->setNumericProperty(ComponentProperty::Icon, static_cast<uint32_t>(icon));
		button->setNumericProperty(ComponentProperty::Variant, static_cast<uint32_t>(ButtonVariant::Ghost));
		parent->addChild(button); return button;
	};
	auto chrome = new Panel(); chrome->setBackground(_RGB(250, 251, 253)); chrome->setBounds({0, 0, 1040, 104}); root->addChild(chrome);
	control(chrome, "File", Icon::None, {16, 1, 52, 28});
	control(chrome, "View", Icon::None, {70, 1, 52, 28});
	control(chrome, "Go", Icon::None, {124, 1, 52, 28});
	auto back = control(chrome, "", Icon::Back, {18, 49, 34, 36}); back->setStringProperty(ComponentProperty::Tooltip, "Back (Alt+Left)");
	auto next = control(chrome, "", Icon::Forward, {56, 49, 34, 36}); next->setEnabled(false);
	auto path = new TextBox(); path->setTitle("/user/Documents"); path->setBounds({106, 47, 566, 40});
	path->setNumericProperty(ComponentProperty::Icon, static_cast<uint32_t>(Icon::Folder)); chrome->addChild(path);
	control(chrome, "", Icon::Up, {684, 49, 34, 36});
	auto search = new TextBox(); search->setBounds({730, 47, 290, 40});
	search->setStringProperty(ComponentProperty::Placeholder, "Filter this folder");
	search->setNumericProperty(ComponentProperty::Icon, static_cast<uint32_t>(Icon::Search)); chrome->addChild(search);
	auto side = new Panel(); side->setBounds({0, 104, 190, 562}); side->setBackground(_RGB(247, 249, 252)); root->addChild(side);
	text(side, "FAVORITES", {24, 23, 142, 24}, 11, _RGB(145, 155, 173));
	const char* places[] = {"Home", "Desktop", "Applications", "Computer", "System"};
	Icon placeIcons[] = {Icon::Home, Icon::Desktop, Icon::Applications, Icon::Computer, Icon::Folder};
	for(int i = 0; i < 5; ++i) {
		auto item = control(side, places[i], placeIcons[i], {12, 61 + i * 46, 166, 40});
		item->setNumericProperty(ComponentProperty::Alignment, static_cast<uint32_t>(TextAlignment::LEFT));
		item->setNumericProperty(ComponentProperty::Selected, i == 0);
	}
	text(side, "QUICK ACCESS", {24, 340, 142, 24}, 11, _RGB(145, 155, 173));
	text(side, "Ctrl+L   Go to a folder", {24, 375, 150, 25}, 12, _RGB(127, 139, 158));
	text(side, "Ctrl+F   Find a file", {24, 402, 150, 25}, 12, _RGB(127, 139, 158));
	text(root, "Documents", {218, 119, 450, 34}, 22, _RGB(35, 44, 65));
	text(root, "/user/Documents", {218, 154, 450, 20}, 12, _RGB(139, 150, 168));
	auto grid = control(root, "", Icon::Grid, {932, 128, 36, 34}); grid->setNumericProperty(ComponentProperty::Selected, true);
	control(root, "", Icon::List, {972, 128, 36, 34});
	const char* names[] = {"Projects", "Pictures", "Music", "Archive", "Readme.txt", "Notes.txt", "Navigator.bin", "Todo.txt"};
	for(int i = 0; i < 8; ++i) {
		auto tile = new Panel(); tile->setBounds({206 + (i % 5) * 140, 198 + (i / 5) * 142, 128, 132});
		tile->setNumericProperty(ComponentProperty::CornerRadius, 10);
		tile->setNumericProperty(ComponentProperty::HoverBackground, _RGB(244, 246, 251));
		tile->setNumericProperty(ComponentProperty::SelectedBackground, _RGB(233, 237, 255));
		tile->setNumericProperty(ComponentProperty::Selected, i == 4);
		auto image = new Image(); image->setBounds({35, 15, 58, 58});
		image->setNumericProperty(ComponentProperty::Icon, static_cast<uint32_t>(i < 4 ? Icon::Folder : i == 6 ? Icon::Applications : Icon::Document));
		tile->addChild(image);
		auto name = text(tile, names[i], {6, 79, 116, 27}); name->setAlignment(TextAlignment::CENTER);
		auto kind = text(tile, i < 4 ? "Folder" : i == 6 ? "Application" : "Text file", {6, 106, 116, 18}, 12, _RGB(137, 148, 167)); kind->setAlignment(TextAlignment::CENTER);
		root->addChild(tile);
	}
	text(root, "8 items   ·   Readme.txt selected", {20, 666, 500, 34}, 12, _RGB(137, 148, 167));
	Server::instance()->screen->addChild(window); window->setVisible(true);
}
}
#endif
#endif
