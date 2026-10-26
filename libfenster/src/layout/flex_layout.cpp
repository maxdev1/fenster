// fenster — lightweight window server and UI toolkit
// Copyright (c) 2025 Max Schlüssel
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "libfenster/layout/flex_layout.hpp"
#include "libfenster/components/component.hpp"

namespace fenster
{
	void FlexLayout::setComponentInfo(Component* child, float grow, float shrink, int basis)
	{
		CommandSetFlexComponentInfoRequest request{{ProtocolCommandId::SetFlexComponentInfo},
			Layout::component->getId(), child->getId(), grow, shrink, basis};
		sendUiMutation(&request, sizeof(request));
	}

	FlexLayout* FlexLayout::create(Component* component)
	{
		auto layout = new FlexLayout(component);
		component->setLayout(layout);
		return layout;
	}


}
