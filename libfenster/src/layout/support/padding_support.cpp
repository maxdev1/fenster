// fenster — lightweight window server and UI toolkit
// Copyright (c) 2025 Max Schlüssel
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "libfenster/layout/support/padding_support.hpp"
#include "libfenster/components/component.hpp"

namespace fenster
{
	void PaddingSupport::setPadding(Insets insets)
	{
		CommandSetLayoutPaddingRequest request{{ProtocolCommandId::SetLayoutPadding}, component->getId(), insets};
		sendUiMutation(&request, sizeof(request));
	}
}
