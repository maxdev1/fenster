// fenster — lightweight window server and UI toolkit
// Copyright (c) 2025 Max Schlüssel
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "test.hpp"

#include <components/radio_button.hpp>
#include <components/radio_group.hpp>
#include <components/text/text_box.hpp>
#include <components/tree.hpp>
#include <components/tree_node.hpp>
#include <layout/flex_layout.hpp>
#include <layout/stack_layout.hpp>
#include <server.hpp>
#include <sstream>
#include <components/menu_bar.hpp>
#include <components/menu_item.hpp>

#include "components/button.hpp"
#include "components/checkbox.hpp"
#include "components/cursor.hpp"
#include "components/scrollpane.hpp"
#include "components/window.hpp"
#include "layout/flow_layout.hpp"
#include "layout/grid_layout.hpp"
#include <libfenster/font/font_loader.hpp>

#include "layout/fill_layout.hpp"

using namespace fenster;

namespace fensterserver {
	void createTestWindow() {
		auto window = new Window;
		window->setTitle("Components");
		window->setBounds(fenster::Rectangle(100, 30, 320, 530));
		window->setLayout(new GridLayout());

		auto scroller = new ScrollPane;
		scroller->setBounds(fenster::Rectangle(0, 0, 300, 200));
		scroller->setFixedWidth(true);

		auto content = new Panel();
		auto contentGrid = new StackLayout(20);
		contentGrid->setPadding(Insets(10, 10, 10, 10));
		content->setLayout(contentGrid);
		scroller->setContent(content);
		window->addChild(scroller);

		auto info = new Label();
		info->setTitle("Buttons:");
		content->addChild(info);

		{
			auto gridPanel = new Panel();
			auto panelLayout = new GridLayout(2, 0, 10, 10);
			panelLayout->debug = true;
			gridPanel->setLayout(panelLayout);

			auto button1 = new Button();
			button1->setMinimumSize(Dimension(0, 20));
			button1->setTitle("Button, enabled");
			gridPanel->addChild(button1);

			auto button2 = new Button();
			button2->setMinimumSize(Dimension(0, 20));
			button2->setTitle("Button, disabled");
			button2->setEnabled(false);
			gridPanel->addChild(button2);

			auto button3 = new Button();
			button3->setTitle("Button with height from text");
			gridPanel->addChild(button3);

			{
				auto b = new Button();
				b->setTitle("Another button");
				gridPanel->addChild(b);
			}
			{
				auto b = new Button();
				b->setTitle("Another button");
				gridPanel->addChild(b);
			}

			content->addChild(gridPanel);
		}

		{
			auto panel = new Panel();
			panel->setLayout(new GridLayout(1, 0, 10, 10));

			auto info = new Label();
			info->setTitle("Text fields:");
			panel->addChild(info);

			auto text = new TextBox();
			text->setPreferredSize(Dimension(0, 30));
			text->setTitle("Text value");
			panel->addChild(text);

			auto pass = new TextBox();
			pass->setSecure(true);
			pass->setTitle("Password");
			pass->setPreferredSize(Dimension(0, 30));
			panel->addChild(pass);

			content->addChild(panel);
		}

		{
			auto panel = new Panel();
			panel->setLayout(new StackLayout(10));

			auto label = new Label();
			label->setTitle("Check boxes and radios (in a flow layout)");
			panel->addChild(label);

			auto flowPanel = new Panel();
			flowPanel->setLayout(new FlowLayout(10, 10));
			panel->addChild(flowPanel);

			auto checkGroup = new Panel();
			checkGroup->setLayout(new StackLayout(5));

			auto check1 = new Checkbox();
			check1->getLabel().setTitle("Show radio group");
			check1->setChecked(true);
			checkGroup->addChild(check1);

			auto check2 = new Checkbox();
			check2->getLabel().setTitle("Show more labels");
			checkGroup->addChild(check2);

			flowPanel->addChild(checkGroup);

			auto radioGroup = new RadioGroup();
			auto subRadio1 = new RadioButton();
			subRadio1->getLabel().setTitle("First option");
			radioGroup->addChild(subRadio1);
			auto subRadio2 = new RadioButton();
			subRadio2->getLabel().setTitle("Second option");
			radioGroup->addChild(subRadio2);
			flowPanel->addChild(radioGroup);

			check1->addInternalCheckedHandler([radioGroup](bool checked, bool) {
				radioGroup->setVisible(checked);
			});

			auto labelPanel = new Panel();
			labelPanel->setLayout(new StackLayout(Orientation::Horizontal));

			auto label1 = new Label();
			label1->setTitle("This is a label");
			labelPanel->addChild(label1);

			auto label2 = new Label();
			label2->setTitle("This is another label");
			labelPanel->addChild(label2);

			flowPanel->addChild(labelPanel);

			content->addChild(panel);
		}

		Server::instance()->screen->addChild(window);
		window->setVisible(true);
	}

	void createGridOfButtons(Point at) {
		auto window = new Window;
		window->setTitle("Grid layout");
		window->setBounds(fenster::Rectangle(at.x, at.y, 300, 300));

		auto grid = new GridLayout(3, 3, 10, 10);
		grid->setPadding(Insets(10, 10, 10, 10));
		window->setLayout(grid);

		for (int i = 0; i < 9; i++) {
			auto button1 = new Button();
			std::stringstream s;
			s << "Button " << i;
			button1->setTitle(s.str().c_str());
			window->addChild(button1);
		}

		Server::instance()->screen->addChild(window);
		window->setVisible(true);
	}

	void createWindowWithTree() {
		std::string defaultTreeContent = R"({
	"rootNodes": [
		{
			"title": "Fruits",
			"children": [
				{"title": "Citrus", "children": [
					{"title": "Orange"},
					{"title": "Lemon"},
					{"title": "Lime"}
				]},
				{"title": "Berries", "children": [
					{"title": "Strawberry"},
					{"title": "Blueberry"}
				]},
				{"title": "Tropical", "children": [
					{"title": "Mango"}
				]}
			]
		},
		{
			"title": "Vegetables",
			"children": [
				{"title": "Root vegetables", "children": [
					{"title": "Carrot"},
					{"title": "Beetroot"}
				]},
				{"title": "Leafy Greens", "children": [
					{"title": "Spinach"},
					{"title": "Kale"}
				]}
			]
		}
	]
})";
		auto window = new Window;

		auto menuBar = new MenuBar();
		{
			auto fileMenu = new MenuItem();
			fileMenu->setTitle("File");
			menuBar->addChild(fileMenu);

			auto menuSubItemNewFile = new MenuItem();
			menuSubItemNewFile->setTitle("New...");
			fileMenu->addChild(menuSubItemNewFile);

			auto menuSubItemNewFileSub1 = new MenuItem();
			menuSubItemNewFileSub1->setTitle("Text");
			menuSubItemNewFile->addChild(menuSubItemNewFileSub1);

			auto menuSubItemNewFileSub2 = new MenuItem();
			menuSubItemNewFileSub2->setTitle("Assembly");
			menuSubItemNewFile->addChild(menuSubItemNewFileSub2);

			auto menuSubItemOpen = new MenuItem();
			menuSubItemOpen->setTitle("Open...");
			fileMenu->addChild(menuSubItemOpen);

			auto menuSubItemOpenSub1 = new MenuItem();
			menuSubItemOpenSub1->setTitle("More specific");
			menuSubItemOpen->addChild(menuSubItemOpenSub1);

			auto menuSubItemOpenSub11 = new MenuItem();
			menuSubItemOpenSub11->setTitle("Very specific");
			menuSubItemOpenSub1->addChild(menuSubItemOpenSub11);

			auto menuSubItemOpenSub12 = new MenuItem();
			menuSubItemOpenSub12->setTitle("Even more specific");
			menuSubItemOpenSub1->addChild(menuSubItemOpenSub12);

			auto menuSubItemOpenSub2 = new MenuItem();
			menuSubItemOpenSub2->setTitle("Test 2");
			menuSubItemOpen->addChild(menuSubItemOpenSub2);

			auto menuSubItemClose = new MenuItem();
			menuSubItemClose->setTitle("Close");
			fileMenu->addChild(menuSubItemClose);

			auto openMenu = new MenuItem();
			openMenu->setTitle("Open");
			menuBar->addChild(openMenu);

			auto viewMenu = new MenuItem();
			viewMenu->setTitle("View");
			menuBar->addChild(viewMenu);
		}
		window->setMenuBar(menuBar);

		auto panel = new Panel();
		auto panelLayout = new StackLayout();
		panelLayout->setPadding(Insets(10, 10, 10, 10));
		panelLayout->setSpace(10);
		panel->setLayout(panelLayout);

		auto colorSection = new Panel();
		{
			colorSection->setLayout(new StackLayout(10, fenster::Insets(10, 10, 10, 10)));

			auto area1label = new Label();
			area1label->setTitle("Choose color:");
			colorSection->addChild(area1label);

			auto radiogroup = new RadioGroup();
			auto radiogroupLayout = new StackLayout(5);
			radiogroupLayout->setPadding(Insets(5, 5, 5, 5));
			radiogroup->setLayout(radiogroupLayout);

			auto radioGreen = new RadioButton();
			radioGreen->setTitle("Green");
			radiogroup->addChild(radioGreen);
			radioGreen->addInternalCheckedHandler([colorSection](bool checked, bool triggeredByGroup) {
				if (checked) {
					colorSection->setBackground(_RGB(200, 255, 200));
				}
			});

			auto radioBlue = new RadioButton();
			radioBlue->setTitle("Blue");
			radiogroup->addChild(radioBlue);
			radioBlue->addInternalCheckedHandler([colorSection](bool checked, bool triggeredByGroup) {
				if (checked) {
					colorSection->setBackground(_RGB(200, 200, 255));
				}
			});

			auto radioRed = new RadioButton();
			radioRed->setTitle("Red");
			radiogroup->addChild(radioRed);
			radioRed->addInternalCheckedHandler([colorSection](bool checked, bool triggeredByGroup) {
				if (checked) {
					colorSection->setBackground(_RGB(255, 200, 200));
				}
			});

			colorSection->addChild(radiogroup);
		}
		panel->addChild(colorSection);

		auto secondArea = new Panel();
		{
			auto secondAreaLayout = new StackLayout(10, fenster::Insets(10, 10, 10, 10));
			secondArea->setLayout(secondAreaLayout);

			auto jsonTreeAreaTitle = new Label();
			jsonTreeAreaTitle->setTitle("Json to tree:");
			secondArea->addChild(jsonTreeAreaTitle);

			auto jsonTreeSection = new Panel();
			auto jsonTreeSectionLayout = new GridLayout(2, 0, 20, 20);
			jsonTreeSection->setLayout(jsonTreeSectionLayout);
			{
				auto tree = new Tree();
				auto treeInputSection = new Panel();
				{
					treeInputSection->setLayout(new StackLayout(5));

					auto jsonInput = new TextBox();
					jsonInput->setMultiLine(true);
					jsonInput->setFont(fenster::FontLoader::get("monaco"));
					jsonInput->setMinimumSize(Dimension(0, 100));
					jsonInput->setMaximumSize(Dimension(999999, 400));
					jsonInput->setText(defaultTreeContent);
					treeInputSection->addChild(jsonInput);

					auto button = new Button();
					button->setTitle("To tree");
					treeInputSection->addChild(button);
					button->setInternalActionHandler([tree, jsonInput]() {
						auto text = jsonInput->getText();
						platformLog(("setting model from JSON: " + text).c_str());
						tree->setModelFromJson(text);
					});
				}
				jsonTreeSection->addChild(treeInputSection);

				tree->setModelFromJson(defaultTreeContent);
				jsonTreeSection->addChild(tree);
			}
			secondArea->addChild(jsonTreeSection);
		}
		panel->addChild(secondArea);

		window->setTitle("Settings");
		window->setBounds(Rectangle(530, 30, 800, 600));
		window->setLayout(new GridLayout(1));

		auto scroller = new ScrollPane;
		scroller->setFixedWidth(true);
		scroller->setContent(panel);
		window->addChild(scroller);

		Server::instance()->screen->addChild(window);
		window->setVisible(true);
	}

	void createWindowWithTreeAndFlex() {
		std::string defaultTreeContent = R"({
	"rootNodes": [
		{
			"title": "Fruits",
			"children": [
				{"title": "Citrus", "children": [
					{"title": "Orange"},
					{"title": "Lemon"},
					{"title": "Lime"}
				]},
				{"title": "Berries", "children": [
					{"title": "Strawberry"},
					{"title": "Blueberry"}
				]},
				{"title": "Tropical", "children": [
					{"title": "Mango"}
				]}
			]
		},
		{
			"title": "Vegetables",
			"children": [
				{"title": "Root vegetables", "children": [
					{"title": "Carrot"},
					{"title": "Beetroot"}
				]},
				{"title": "Leafy Greens", "children": [
					{"title": "Spinach"},
					{"title": "Kale"}
				]}
			]
		}
	]
})";
		auto window = new Window;

		auto menuBar = new MenuBar();
		{
			auto fileMenu = new MenuItem();
			fileMenu->setTitle("File");
			menuBar->addChild(fileMenu);

			auto menuSubItemNewFile = new MenuItem();
			menuSubItemNewFile->setTitle("New...");
			fileMenu->addChild(menuSubItemNewFile);

			auto menuSubItemNewFileSub1 = new MenuItem();
			menuSubItemNewFileSub1->setTitle("Text");
			menuSubItemNewFile->addChild(menuSubItemNewFileSub1);

			auto menuSubItemNewFileSub2 = new MenuItem();
			menuSubItemNewFileSub2->setTitle("Assembly");
			menuSubItemNewFile->addChild(menuSubItemNewFileSub2);

			auto menuSubItemOpen = new MenuItem();
			menuSubItemOpen->setTitle("Open...");
			fileMenu->addChild(menuSubItemOpen);

			auto menuSubItemOpenSub1 = new MenuItem();
			menuSubItemOpenSub1->setTitle("More specific");
			menuSubItemOpen->addChild(menuSubItemOpenSub1);

			auto menuSubItemOpenSub11 = new MenuItem();
			menuSubItemOpenSub11->setTitle("Very specific");
			menuSubItemOpenSub1->addChild(menuSubItemOpenSub11);

			auto menuSubItemOpenSub12 = new MenuItem();
			menuSubItemOpenSub12->setTitle("Even more specific");
			menuSubItemOpenSub1->addChild(menuSubItemOpenSub12);

			auto menuSubItemOpenSub2 = new MenuItem();
			menuSubItemOpenSub2->setTitle("Test 2");
			menuSubItemOpen->addChild(menuSubItemOpenSub2);

			auto menuSubItemClose = new MenuItem();
			menuSubItemClose->setTitle("Close");
			fileMenu->addChild(menuSubItemClose);

			auto openMenu = new MenuItem();
			openMenu->setTitle("Open");
			menuBar->addChild(openMenu);

			auto viewMenu = new MenuItem();
			viewMenu->setTitle("View");
			menuBar->addChild(viewMenu);
		}
		window->setMenuBar(menuBar);

		auto panel = new Panel();
		auto panelLayout = new FlexLayout();
		panel->setLayout(panelLayout);

		auto colorSection = new Panel();
		panelLayout->setComponentInfo(colorSection, 0, 1, 0);
		{
			colorSection->setLayout(new StackLayout(10));

			auto area1label = new Label();
			area1label->setTitle("Choose color:");
			colorSection->addChild(area1label);

			auto radiogroup = new RadioGroup();
			auto radiogroupLayout = new StackLayout(5);
			radiogroupLayout->setPadding(Insets(5, 5, 5, 5));
			radiogroup->setLayout(radiogroupLayout);

			auto radioGreen = new RadioButton();
			radioGreen->setTitle("Green");
			radiogroup->addChild(radioGreen);
			radioGreen->addInternalCheckedHandler([colorSection](bool checked, bool triggeredByGroup) {
				if (checked) {
					colorSection->setBackground(_RGB(200, 255, 200));
				}
			});

			auto radioBlue = new RadioButton();
			radioBlue->setTitle("Blue");
			radiogroup->addChild(radioBlue);
			radioBlue->addInternalCheckedHandler([colorSection](bool checked, bool triggeredByGroup) {
				if (checked) {
					colorSection->setBackground(_RGB(200, 200, 255));
				}
			});

			auto radioRed = new RadioButton();
			radioRed->setTitle("Red");
			radiogroup->addChild(radioRed);
			radioRed->addInternalCheckedHandler([colorSection](bool checked, bool triggeredByGroup) {
				if (checked) {
					colorSection->setBackground(_RGB(255, 200, 200));
				}
			});

			colorSection->addChild(radiogroup);
		}
		panel->addChild(colorSection);

		auto secondArea = new Panel();
		panelLayout->setComponentInfo(secondArea, 1, 0, 100);
		{
			auto secondAreaLayout = new StackLayout(10);
			secondArea->setLayout(secondAreaLayout);

			auto jsonTreeAreaTitle = new Label();
			jsonTreeAreaTitle->setTitle("Json to tree:");
			secondArea->addChild(jsonTreeAreaTitle);

			auto jsonTreeSection = new Panel();
			auto jsonTreeSectionLayout = new GridLayout(2, 0, 0, 20);
			jsonTreeSection->setLayout(jsonTreeSectionLayout);
			{
				auto tree = new Tree();
				auto treeInputSection = new Panel();
				{
					treeInputSection->setLayout(new StackLayout(5));

					auto jsonInput = new TextBox();
					jsonInput->setMultiLine(true);
					jsonInput->setFont(fenster::FontLoader::get("monaco"));
					jsonInput->setMinimumSize(Dimension(0, 100));
					jsonInput->setMaximumSize(Dimension(999999, 200));
					jsonInput->setText(defaultTreeContent);
					treeInputSection->addChild(jsonInput);

					auto button = new Button();
					button->setTitle("To tree");
					treeInputSection->addChild(button);
					button->setInternalActionHandler([tree, jsonInput]() {
						auto text = jsonInput->getText();
						platformLog(("setting model from JSON: " + text).c_str());
						tree->setModelFromJson(text);
					});
				}
				jsonTreeSection->addChild(treeInputSection);

				tree->setModelFromJson(defaultTreeContent);
				jsonTreeSection->addChild(tree);
			}
			secondArea->addChild(jsonTreeSection);
		}
		panel->addChild(secondArea);

		auto thirdArea = new Panel();
		thirdArea->setBackground(_RGB(200, 255, 200));
		panelLayout->setComponentInfo(thirdArea, 1, 0, 100);
		panel->addChild(thirdArea);

		window->setTitle("Settings");
		window->setBounds(Rectangle(530, 30, 800, 600));
		window->setLayout(new GridLayout(1));

		// window->addChild(panel);

		auto scroller = new ScrollPane;
		scroller->setFixedWidth(true);
		scroller->setContent(panel);
		window->addChild(scroller);

		Server::instance()->screen->addChild(window);
		window->setVisible(true);
	}

	void createTestScrollBoth(Point at) {
		auto window = new Window();
		window->setTitle("Scroll pane with both directions (size from preferred panel size)");
		window->setBounds(Rectangle(at.x, at.y, 400, 400));
		window->setLayout(new GridLayout(1));

		auto scroller = new ScrollPane();

		auto panel = new Panel();
		panel->setPreferredSize(Dimension(800, 800));
		panel->setLayout(new GridLayout(5, 20));

		for (int i = 0; i < 100; i++) {
			auto label = new Label();
			std::stringstream ss;
			ss << "This is cell " << i / 5 << "/" << i % 5;
			label->setTitle(ss.str());
			label->setAlignment(TextAlignment::CENTER);
			panel->addChild(label);
		}
		scroller->setContent(panel);

		window->addChild(scroller);

		Server::instance()->screen->addChild(window);
		window->setVisible(true);
	}


	void createTestScrollBothSelfSizing() {
		auto window = new Window();
		window->setTitle("Scroll pane with both directions (take size from child)");
		window->setBounds(Rectangle(300, 150, 400, 400));
		window->setLayout(new GridLayout(1));

		auto scroller = new ScrollPane();

		auto panel = new Panel();
		panel->layoutPolicyHorizontal = LayoutingPolicy::Preferred;
		panel->setLayout(new GridLayout(5, 20));

		for (int i = 0; i < 100; i++) {
			auto label = new Label();
			std::stringstream ss;
			ss << "This is cell " << i / 5 << "/" << i % 5;
			label->setTitle(ss.str());
			label->setAlignment(TextAlignment::CENTER);
			panel->addChild(label);
		}
		scroller->setContent(panel);

		window->addChild(scroller);

		Server::instance()->screen->addChild(window);
		window->setVisible(true);
	}

	void createTestMenu() {
		auto window = new Window();
		window->setTitle("Open tests");
		window->setBounds(Rectangle(1000, 700, 300, 200));
		window->setLayout(new GridLayout(1));

		auto content = window->getPanel();
		auto layout = new GridLayout(2, 0, 20, 20);
		layout->setPadding(Insets(10, 10, 10, 10));
		content->setLayout(layout);

		{
			auto buttonGrid = new Button();
			buttonGrid->setTitle("Grid of buttons");
			content->addChild(buttonGrid);
			buttonGrid->setInternalActionHandler([window]() {
				createGridOfButtons(window->getBounds().getStart() + Point(20, 20));
			});
		}
		{
			auto scrollerTest = new Button();
			scrollerTest->setTitle("Scroller");
			content->addChild(scrollerTest);
			scrollerTest->setInternalActionHandler([window]() {
				createTestScrollBoth(window->getBounds().getStart() + Point(20, 20));
			});
		}

		Server::instance()->screen->addChild(window);
		window->setVisible(true);
	}

	void createSingleLabelWindow() {
		auto window = new Window();
		window->setTitle("Window with just a label");
		window->setBounds(Rectangle(150, 800, 300, 200));
		window->setLayout(new GridLayout(1));

		auto content = window->getPanel();
		content->setLayout(new StackLayout());
		content->setBackground(_RGB(100, 255, 200));

		auto panel = new Panel();
		panel->setBackground(_RGB(0, 255, 0));
		panel->setLayout(new StackLayout());

		auto label = new Label();
		label->setAlignment(TextAlignment::CENTER);
		label->setTitle("Agdliom");
		panel->addChild(label);

		content->addChild(panel);

		Server::instance()->screen->addChild(window);
		window->setVisible(true);
	}

	void Test::createTestComponents() {
#ifndef _GHOST_
		createSingleLabelWindow();
		createTestMenu();
		createTestWindow();
		createWindowWithTree();
		// createWindowWithTreeAndFlex();
		createTestScrollBothSelfSizing();
#endif
	}
}
