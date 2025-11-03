// fenster — lightweight window server and UI toolkit
// Copyright (c) 2025 Max Schlüssel
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "test.hpp"

#include <server.hpp>
#include <sstream>
#include <components/radio_button.hpp>
#include <components/radio_group.hpp>
#include <components/tree.hpp>
#include <components/tree_node.hpp>
#include <components/text/text_area.hpp>
#include <layout/flex_layout_manager.hpp>
#include <layout/stack_layout_manager.hpp>

#include "components/button.hpp"
#include "components/checkbox.hpp"
#include "components/cursor.hpp"
#include "components/scrollpane.hpp"
#include "components/text/text_field.hpp"
#include "components/window.hpp"
#include "layout/flow_layout_manager.hpp"
#include "layout/grid_layout_manager.hpp"

using namespace fenster;

namespace fensterserver
{
	class open_exe_data_t;
	void openExecutableSpawn(open_exe_data_t* data);

	class open_exe_data_t
	{
	public:
		std::string exe;
		std::string args;
	};

	void openExecutableSpawn(open_exe_data_t* data)
	{
		platformSpawn(data->exe.c_str(), data->args.c_str(), "/");
	}

	static int nextButtonPos = 70;

	void addExecutableButton(Window* window, std::string name, std::string exe, std::string args)
	{

		Button* openCalculatorButton = new Button();
		openCalculatorButton->setBounds(fenster::Rectangle(10, nextButtonPos, 270, 30));
		openCalculatorButton->getLabel().setTitle(name);
		openCalculatorButton->setInternalActionHandler([exe, args]()
		{
			auto data = new open_exe_data_t();
			data->exe = exe;
			data->args = args;
			platformCreateThreadWithData((void*) &openExecutableSpawn, data);
		});
		window->addChild(openCalculatorButton);
		nextButtonPos += 35;
	}

	void createTestWindow()
	{
		Window* window = new Window;
		window->setTitle("Components");
		window->setBounds(fenster::Rectangle(100, 30, 320, 530));
		window->setLayoutManager(new GridLayoutManager(1, 1));

		ScrollPane* scroller = new ScrollPane;
		scroller->setBounds(fenster::Rectangle(0, 0, 300, 200));
		scroller->setFixedWidth(true);

		Panel* content = new Panel();
		auto contentGrid = new GridLayoutManager(1);
		contentGrid->setRowSpace(20);
		contentGrid->setPadding(Insets(10, 10, 10, 10));
		content->setLayoutManager(contentGrid);
		scroller->setContent(content);
		window->addChild(scroller);

		{
			Panel* panel = new Panel();
			panel->setLayoutManager(new GridLayoutManager(2, 0, 10, 10));

			Label* info = new Label();
			info->setTitle("Buttons:");
			panel->addChild(info);

			Button* button1 = new Button();
			button1->setMinimumSize(Dimension(0, 20));
			button1->setTitle("Button, enabled");
			panel->addChild(button1);

			Button* button2 = new Button();
			button2->setMinimumSize(Dimension(0, 20));
			button2->setTitle("Button, disabled");
			button2->setEnabled(false);
			panel->addChild(button2);

			Button* button3 = new Button();
			button3->setTitle("Button with height from text");
			panel->addChild(button3);

			{
				Button* b = new Button();
				b->setTitle("Another button");
				panel->addChild(b);
			}
			{
				Button* b = new Button();
				b->setTitle("Another button");
				panel->addChild(b);
			}

			content->addChild(panel);
		}

		{
			Panel* panel = new Panel();
			panel->setLayoutManager(new GridLayoutManager(1, 0, 10, 10));

			Label* info = new Label();
			info->setTitle("Text fields:");
			panel->addChild(info);

			TextField* text = new TextField();
			text->setPreferredSize(Dimension(0, 30));
			panel->addChild(text);

			TextField* pass = new TextField();
			pass->setSecure(true);
			pass->setPreferredSize(Dimension(0, 30));
			panel->addChild(pass);

			content->addChild(panel);
		}

		{
			Panel* panel = new Panel();
			panel->setLayoutManager(new FlowLayoutManager());

			Checkbox* check = new Checkbox();
			check->getLabel().setTitle("Check me");
			panel->addChild(check);

			RadioGroup* radioGroup = new RadioGroup();
			auto subRadio1 = new RadioButton();
			subRadio1->getLabel().setTitle("First option");
			radioGroup->addChild(subRadio1);
			auto subRadio2 = new RadioButton();
			subRadio2->getLabel().setTitle("Second option");
			radioGroup->addChild(subRadio2);
			panel->addChild(radioGroup);

			content->addChild(panel);
		}

		Server::instance()->screen->addChild(window);
		window->setVisible(true);
	}

	void createTestWindow2()
	{
		Window* window = new Window;
		window->setTitle("Grid layout");
		window->setBounds(fenster::Rectangle(700, 10, 300, 300));

		auto grid = new GridLayoutManager(3, 3, 10, 10);
		grid->setPadding(Insets(10, 10, 10, 10));
		window->setLayoutManager(grid);

		for(int i = 0; i < 9; i++)
		{
			Button* button1 = new Button();
			std::stringstream s;
			s << "Button " << i;
			button1->setTitle(s.str().c_str());
			window->addChild(button1);
		}

		Server::instance()->screen->addChild(window);
		window->setVisible(true);
	}

	void createTestWindow3()
	{
		auto window = new Window;
		window->setTitle("Components");
		window->setBounds(fenster::Rectangle(530, 30, 320, 530));
		window->setLayoutManager(new GridLayoutManager(1, 1));

		auto scroller = new ScrollPane;
		scroller->setBounds(fenster::Rectangle(0, 0, 300, 200));

		auto panel = new Panel();
		auto stackLayout = new StackLayoutManager();
		stackLayout->setPadding(Insets(10, 10, 10, 10));
		stackLayout->setSpace(10);
		panel->setLayoutManager(stackLayout);

		auto jsonInput = new TextArea();
		jsonInput->setMinimumSize(Dimension(500, 300));
		jsonInput->setText(R"({
	"rootNodes": [
		{
			"title": "Fruits",
			"children": [
				{"title":"Apple"},
				{"title":"Cherry"}
			]
		}
	]
})");
		panel->addChild(jsonInput);

		auto testInput = new TextField();
		testInput->setMinimumSize(Dimension(100, 30));
		panel->addChild(testInput);

		auto tree = new Tree();

		auto button = new Button();
		button->setTitle("To tree");
		panel->addChild(button);
		button->setInternalActionHandler([tree, jsonInput]()
		{
			auto text = jsonInput->getText();
			platformLog(("setting model from JSON: " + text).c_str());
			tree->setModelFromJson(text);
		});


		std::string json = "{"
				"\"rootNodes\": ["
				"{"
				"\"id\": 1,"
				"\"title\": \"Node 1\","
				"\"children\": ["
				"{"
				"\"id\": 2,"
				"\"title\": \"Node 1.1\""
				"},"
				"{"
				"\"id\": 3,"
				"\"title\": \"Node 1.2\""
				"}"
				"]"
				"}"
				"]"
				"}";
		tree->setModelFromJson(json);

		panel->addChild(tree);

		scroller->setContent(panel);

		window->addChild(scroller);

		Server::instance()->screen->addChild(window);
		window->setVisible(true);
	}

	void Test::createTestComponents()
	{
		createTestWindow();
		createTestWindow2();
		createTestWindow3();
	}
}
