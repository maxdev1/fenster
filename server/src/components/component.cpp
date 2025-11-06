// fenster — lightweight window server and UI toolkit
// Copyright (c) 2025 Max Schlüssel
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "components/component.hpp"
#include "component_registry.hpp"
#include "components/window.hpp"
#include "events/key_event.hpp"
#include "events/locatable.hpp"
#include "events/mouse_event.hpp"
#include "layout/flex_layout.hpp"
#include "layout/flow_layout.hpp"
#include "layout/grid_layout.hpp"
#include "layout/stack_layout.hpp"
#include "libfenster/json/json.hpp"
#include "server.hpp"

#include <cairo/cairo.h>
#include <libfenster/properties.hpp>
#include <libfenster/interface.hpp>
#include <algorithm>
#include <cstdlib>

#include "checkable_component.hpp"

namespace fensterserver
{
	Component::Component() :
		BoundingComponent(this),
		FocusableComponent(this),
		visible(true),
		requirements(COMPONENT_REQUIREMENT_ALL),
		childRequirements(COMPONENT_REQUIREMENT_ALL),
		parent(nullptr),
		_layout(nullptr)
	{
		id = ComponentRegistry::add(this);
	}

	Component::~Component()
	{
		delete _layout;
	}

	void Component::setBoundsInternal(const fenster::Rectangle& newBounds)
	{
		fenster::platformAcquireMutex(lock);

		fenster::Rectangle oldBounds = bounds;
		markDirty();

		bounds = newBounds;
		if(bounds.width < minimumSize.width)
			bounds.width = minimumSize.width;
		if(bounds.height < minimumSize.height)
			bounds.height = minimumSize.height;
		markDirty();

		if(oldBounds.width != bounds.width || oldBounds.height != bounds.height)
		{
			if(hasGraphics())
			{
				graphics.resize(bounds.width, bounds.height);
			}
			markFor(COMPONENT_REQUIREMENT_ALL);
			handleBoundChanged(oldBounds);
		}

		fenster::platformReleaseMutex(lock);
	}

	void Component::recheckGraphics()
	{
		if(hasGraphics())
			graphics.resize(bounds.width, bounds.height);
	}

	fenster::Rectangle Component::getBounds() const
	{
		fenster::platformAcquireMutex(lock);
		auto bounds = this->bounds;
		fenster::platformReleaseMutex(lock);
		return bounds;
	}

	std::vector<ComponentChildReference>& Component::acquireChildren()
	{
		fenster::platformAcquireMutex(childrenLock);
		return children;
	}

	void Component::releaseChildren() const
	{
		fenster::platformReleaseMutex(childrenLock);
	}


	void Component::update()
	{
		markFor(COMPONENT_REQUIREMENT_LAYOUT);
	}

	void Component::layout()
	{
		if(_layout)
		{
			_layout->layout();
			markFor(COMPONENT_REQUIREMENT_PAINT);
		}
	}

	void Component::paint()
	{
	}

	bool Component::canHandleEvents() const
	{
		if(!visible)
			return false;
		if(parent)
			return parent->canHandleEvents();
		return true;
	}

	void Component::setVisible(bool visible)
	{
		fenster::platformAcquireMutex(lock);
		this->visible = visible;
		markDirty();
		fenster::platformReleaseMutex(lock);

		if(visible)
		{
			markFor(COMPONENT_REQUIREMENT_ALL);
		}
		markParentFor(COMPONENT_REQUIREMENT_LAYOUT);

		this->callForListeners(fenster::ComponentEventType::Visible, [visible](EventListenerInfo& info)
		{
			fenster::ComponentVisibleEvent e;
			e.header.type = fenster::ComponentEventType::Visible;
			e.header.component_id = info.component_id;
			e.visible = visible;
			fenster::platformSendMessage(info.target_thread, &e, sizeof(fenster::ComponentVisibleEvent),SYS_TX_NONE);
		});
	}

	void Component::markDirty(fenster::Rectangle rect)
	{
		if(parent)
		{
			rect.x += bounds.x;
			rect.y += bounds.y;
			parent->markDirty(rect);
		}
	}

	void Component::blit(Graphics* out, const fenster::Rectangle& parentClip, const fenster::Point& screenPosition)
	{
		if(!this->visible)
			return;

		fenster::platformAcquireMutex(lock);

		fenster::Rectangle clip = getBounds();
		clip.x = screenPosition.x;
		clip.y = screenPosition.y;
		clip = clip.clip(parentClip);

		if(hasGraphics())
		{
			graphics.blitTo(out, clip, screenPosition);
		}
		fenster::platformReleaseMutex(lock);

		this->blitChildren(out, clip, screenPosition);
	}

	void Component::blitChildren(Graphics* out, const fenster::Rectangle& clip, const fenster::Point& screenPosition)
	{
		fenster::platformAcquireMutex(childrenLock);
		for(auto& c: children)
		{
			if(c.component->visible)
			{
				fenster::Point childPositionOnParent = screenPosition + c.component->bounds.getStart();
				c.component->blit(out, clip, childPositionOnParent);
			}
		}
		fenster::platformReleaseMutex(childrenLock);
	}

	void Component::addChild(Component* comp, ComponentChildReferenceType type)
	{
		if(comp->parent)
			comp->parent->removeChild(comp);

		comp->parent = this;

		ComponentChildReference reference{};
		reference.component = comp;
		reference.type = type;

		fenster::platformAcquireMutex(childrenLock);
		children.push_back(reference);
		std::stable_sort(children.begin(), children.end(),
		                 [](const ComponentChildReference& c1, const ComponentChildReference& c2)
		                 {
			                 return c1.component->zIndex < c2.component->zIndex;
		                 });
		fenster::platformReleaseMutex(childrenLock);

		comp->markFor(COMPONENT_REQUIREMENT_ALL);
		markFor(COMPONENT_REQUIREMENT_ALL);
	}

	void Component::removeChild(Component* comp)
	{
		comp->parent = 0;

		fenster::platformAcquireMutex(childrenLock);
		for(auto itr = children.begin(); itr != children.end();)
		{
			if((*itr).component == comp)
			{
				itr = children.erase(itr);
			}
			else
			{
				++itr;
			}
		}
		fenster::platformReleaseMutex(childrenLock);

		markFor(COMPONENT_REQUIREMENT_LAYOUT);
	}

	Component* Component::getComponentAt(fenster::Point p)
	{
		Component* target = this;

		fenster::platformAcquireMutex(childrenLock);
		for(auto it = children.rbegin(); it != children.rend(); ++it)
		{
			auto child = (*it).component;
			if(child->visible && child->bounds.contains(p))
			{
				fenster::platformReleaseMutex(childrenLock);
				target = child->getComponentAt(fenster::Point(p.x - child->bounds.x, p.y - child->bounds.y));
				break;
			}
		}
		fenster::platformReleaseMutex(childrenLock);

		return target;
	}

	Window* Component::getWindow()
	{
		if(isWindow())
			return dynamic_cast<Window*>(this);

		if(parent)
			return parent->getWindow();

		return nullptr;
	}

	void Component::bringChildToFront(Component* comp)
	{
		fenster::platformAcquireMutex(childrenLock);
		for(uint32_t index = 0; index < children.size(); index++)
		{
			if(children[index].component == comp)
			{
				auto ref = children[index];
				children.erase(children.begin() + index);
				children.push_back(ref);

				markDirty(comp->bounds);
				break;
			}
		}
		fenster::platformReleaseMutex(childrenLock);
	}

	void Component::bringToFront()
	{
		if(parent)
			parent->bringChildToFront(this);
	}

	fenster::Point Component::getLocationOnScreen()
	{
		fenster::Point location(bounds.x, bounds.y);

		if(parent)
			location += parent->getLocationOnScreen();

		return location;
	}

	Component* Component::handleMouseEvent(MouseEvent& event)
	{
		Component* handledByChild = nullptr;

		fenster::platformAcquireMutex(childrenLock);
		for(auto it = children.rbegin(); it != children.rend(); ++it)
		{
			auto child = (*it).component;
			if(!child->visible)
				continue;

			if(child->bounds.contains(event.position))
			{
				event.position.x -= child->bounds.x;
				event.position.y -= child->bounds.y;

				handledByChild = child->handleMouseEvent(event);
				if(handledByChild)
				{
					break;
				}

				event.position.x += child->bounds.x;
				event.position.y += child->bounds.y;
			}
		}
		fenster::platformReleaseMutex(childrenLock);

		if(handledByChild)
			return handledByChild;

		auto handledByListener = this->callForListeners(fenster::ComponentEventType::Mouse,
		                                                [event](EventListenerInfo& info)
		                                                {
			                                                fenster::ComponentMouseEvent postedEvent;
			                                                postedEvent.header.type =
					                                                fenster::ComponentEventType::Mouse;
			                                                postedEvent.header.component_id = info.component_id;
			                                                postedEvent.position = event.position;
			                                                postedEvent.type = event.type;
			                                                postedEvent.buttons = event.buttons;
			                                                postedEvent.clickCount = event.clickCount;
			                                                postedEvent.scroll = event.scroll;
			                                                fenster::platformSendMessage(
					                                                info.target_thread, &postedEvent,
					                                                sizeof(fenster::ComponentMouseEvent),SYS_TX_NONE);
		                                                });

		if(handledByListener)
			return this;

		return nullptr;
	}

	Component* Component::handleKeyEvent(KeyEvent& event)
	{
		Component* handledByChild = nullptr;

		fenster::platformAcquireMutex(childrenLock);
		for(auto it = children.rbegin(); it != children.rend(); ++it)
		{
			auto child = (*it).component;
			if(!child->visible)
				continue;

			handledByChild = child->handleKeyEvent(event);
			if(handledByChild)
			{
				break;
			}
		}
		fenster::platformReleaseMutex(childrenLock);

		if(!handledByChild)
		{
			if(sendKeyEventToListener(event))
				return this;
		}

		return handledByChild;
	}

	bool Component::sendKeyEventToListener(KeyEvent& event)
	{
		return this->callForListeners(fenster::ComponentEventType::Key,
		                              [event](EventListenerInfo& info)
		                              {
			                              fenster::ComponentKeyEvent posted_key_event;
			                              posted_key_event.header.type =
					                              fenster::ComponentEventType::Key;
			                              posted_key_event.header.component_id = info.component_id;
			                              posted_key_event.key_info = event.info;
			                              fenster::platformSendMessage(
					                              info.target_thread, &posted_key_event,
					                              sizeof(fenster::ComponentKeyEvent),SYS_TX_NONE);
		                              });
	}

	void Component::setPreferredSize(const fenster::Dimension& size)
	{
		if(preferredSize != size)
		{
			preferredSize = size;
			markParentFor(COMPONENT_REQUIREMENT_UPDATE | COMPONENT_REQUIREMENT_LAYOUT);
		}
	}

	fenster::Dimension Component::getEffectivePreferredSize()
	{
		auto preferred = getPreferredSize();
		auto min = getMinimumSize();
		preferred.width = std::max(preferred.width, min.width);
		preferred.height = std::max(preferred.height, min.height);
		return preferred;
	}

	void Component::setMinimumSize(const fenster::Dimension& size)
	{
		minimumSize = size;
		markParentFor(COMPONENT_REQUIREMENT_UPDATE | COMPONENT_REQUIREMENT_LAYOUT);
	}

	void Component::setMaximumSize(const fenster::Dimension& size)
	{
		maximumSize = size;
		markParentFor(COMPONENT_REQUIREMENT_UPDATE | COMPONENT_REQUIREMENT_LAYOUT);
	}

	void Component::setLayout(Layout* newMgr)
	{
		newMgr->setComponent(this);
		this->_layout = newMgr;
		markFor(COMPONENT_REQUIREMENT_LAYOUT);
	}

	void Component::markParentFor(ComponentRequirement req)
	{
		if(parent)
			parent->markFor(req);
	}

	void Component::markFor(ComponentRequirement req)
	{
		requirements |= req;

		if(parent)
			parent->markChildsFor(req);

		Server::instance()->requestUpdateLater();
	}

	void Component::markChildsFor(ComponentRequirement req)
	{
		childRequirements |= req;

		if(parent)
			parent->markChildsFor(req);
	}

	/**
	 * Resolves a single requirement in the component tree. Layouting is done top-down,
	 * while updating and painting is done bottom-up.
	 */
	void Component::resolveRequirement(ComponentRequirement req, int lvl)
	{
		if((childRequirements & req) && (req & COMPONENT_REQUIREMENT_UPDATE || req & COMPONENT_REQUIREMENT_PAINT))
		{
			fenster::platformAcquireMutex(childrenLock);
			for(auto& child: children)
			{
				if(child.component->visible)
				{
					fenster::platformAcquireMutex(child.component->lock);
					child.component->resolveRequirement(req, lvl + 1);
					fenster::platformReleaseMutex(child.component->lock);
				}
			}
			childRequirements &= ~req;
			fenster::platformReleaseMutex(childrenLock);
		}

		fenster::platformAcquireMutex(lock);
		if(requirements & req)
		{
			if(req & COMPONENT_REQUIREMENT_UPDATE)
			{
				update();
			}
			if(req & COMPONENT_REQUIREMENT_LAYOUT)
			{
				layout();
			}
			if(req & COMPONENT_REQUIREMENT_PAINT)
			{
				paint();
				markDirty();
			}

			requirements &= ~req;
		}
		fenster::platformReleaseMutex(lock);

		if((childRequirements & req) && (req & COMPONENT_REQUIREMENT_LAYOUT))
		{
			fenster::platformAcquireMutex(childrenLock);
			for(auto& child: children)
			{
				if(child.component->visible)
				{
					fenster::platformAcquireMutex(child.component->lock);
					child.component->resolveRequirement(req, lvl + 1);
					fenster::platformReleaseMutex(child.component->lock);
				}
			}
			childRequirements &= ~req;
			fenster::platformReleaseMutex(childrenLock);
		}
	}

	void Component::addListener(fenster::ComponentEventType eventType, SYS_TID_T target_thread, fenster::ComponentId id)
	{
		fenster::platformAcquireMutex(lock);

		auto entry = new ComponentListenerEntry();
		entry->info.target_thread = target_thread;
		entry->info.component_id = id;
		entry->type = eventType;
		listeners.push_back(entry);

		fenster::platformReleaseMutex(lock);
	}

	bool Component::callForListeners(fenster::ComponentEventType eventType,
	                                 const std::function<void(EventListenerInfo&)>& func)
	{
		fenster::platformAcquireMutex(lock);

		bool handled = false;
		for(auto& entry: listeners)
		{
			if(entry->type == eventType)
			{
				func(entry->info);
				handled = true;
			}
		}

		fenster::platformReleaseMutex(lock);
		return handled;
	}

	void Component::clearSurface()
	{
		auto cr = graphics.acquireContext();
		if(!cr)
			return;

		cairo_save(cr);
		cairo_set_source_rgba(cr, 0, 0, 0, 0);
		cairo_set_operator(cr, CAIRO_OPERATOR_SOURCE);
		cairo_paint(cr);
		cairo_restore(cr);

		graphics.releaseContext();
	}

	bool Component::isChildOf(Component* component)
	{
		Component* next = parent;
		while(next)
		{
			if(next == component)
			{
				return true;
			}
			next = next->getParent();
		}

		return false;
	}

	bool Component::getNumericProperty(fenster::ComponentProperty property, uint32_t* out)
	{
		if(property == fenster::ComponentProperty::Visible)
		{
			*out = this->isVisible() ? 1 : 0;
			return true;
		}
		else if(property == fenster::ComponentProperty::Checked)
		{
			auto checkableComponent = dynamic_cast<CheckableComponent*>(getLayout());
			if(checkableComponent)
			{
				*out = checkableComponent->isChecked();
				return true;
			}
		}

		if(FocusableComponent::getNumericProperty(property, out))
		{
			return true;
		}
		return false;
	}

	bool Component::setNumericProperty(fenster::ComponentProperty property, uint32_t value)
	{
		if(property == fenster::ComponentProperty::Layout)
		{
			if((fenster::LayoutType) value == fenster::LayoutType::Flow)
			{
				setLayout(new FlowLayout());
				return true;
			}
			if((fenster::LayoutType) value == fenster::LayoutType::Grid)
			{
				setLayout(new GridLayout());
				return true;
			}
			if((fenster::LayoutType) value == fenster::LayoutType::Flex)
			{
				setLayout(new FlexLayout());
				return true;
			}
			if((fenster::LayoutType) value == fenster::LayoutType::Stack)
			{
				setLayout(new StackLayout());
				return true;
			}
		}
		else if(property == fenster::ComponentProperty::Visible)
		{
			setVisible(value == 1);
			return true;
		}
		else if(property == fenster::ComponentProperty::Checked)
		{
			auto checkableComponent = dynamic_cast<CheckableComponent*>(this);
			if(checkableComponent)
			{
				checkableComponent->setChecked(value == 1, false);
				return true;
			}
		}
		else if(property == fenster::ComponentProperty::LayoutOrientation)
		{
			auto layoutWithOrientation = dynamic_cast<OrientationSupport*>(getLayout());
			if(layoutWithOrientation)
			{
				layoutWithOrientation->setOrientation((fenster::Orientation) value);
			}
		}
		else if(property == fenster::ComponentProperty::LayoutSpace)
		{
			auto layoutWithSingleSpacing = dynamic_cast<SingleSpacingSupport*>(getLayout());
			if(layoutWithSingleSpacing)
			{
				layoutWithSingleSpacing->setSpace(value);
				return true;
			}
		}
		else if(property == fenster::ComponentProperty::LayoutSpaceHorizontal)
		{
			auto layoutWithSpacing = dynamic_cast<SpacingSupport*>(getLayout());
			if(layoutWithSpacing)
			{
				layoutWithSpacing->setHorizontalSpace(value);
				return true;
			}
		}
		else if(property == fenster::ComponentProperty::LayoutSpaceVertical)
		{
			auto layoutWithSpacing = dynamic_cast<SpacingSupport*>(getLayout());
			if(layoutWithSpacing)
			{
				layoutWithSpacing->setVerticalSpace(value);
				return true;
			}
		}

		if(FocusableComponent::setNumericProperty(property, value))
		{
			return true;
		}
		return false;
	}

	bool Component::setStringProperty(fenster::ComponentProperty property, std::string text)
	{
		if(property == fenster::ComponentProperty::Title)
		{
			if(auto titled = dynamic_cast<TitledComponent*>(this))
			{
				titled->setTitle(text);
				return true;
			}
		}
		else if(property == fenster::ComponentProperty::LayoutPadding)
		{
			fenster::JsonNode result = fenster::Json::parse(text);

			if(result.isObject())
			{
				auto obj = result.asObject();
				fenster::Insets insets(
						(int) obj["top"].asNumber(),
						(int) obj["left"].asNumber(),
						(int) obj["bottom"].asNumber(),
						(int) obj["right"].asNumber()
						);

				auto layoutWithPaddingSupport = dynamic_cast<PaddingSupport*>(getLayout());
				if(layoutWithPaddingSupport)
				{
					layoutWithPaddingSupport->setPadding(insets);
					return true;
				}
			}
		}
		else if(property == fenster::ComponentProperty::LayoutFlexComponentInfo)
		{
			fenster::JsonNode result = fenster::Json::parse(text);

			if(result.isObject())
			{
				auto obj = result.asObject();

				auto childId = (fenster::ComponentId) obj["component"].asNumber();
				auto child = ComponentRegistry::get(childId);
				if(!child)
				{
					fenster::platformLog("Attempted to set component info for non-existant component %i", child);
					return false;
				}

				float grow = obj["grow"].asNumber();
				float shrink = obj["shrink"].asNumber();
				int basis = obj["basis"].asNumber();

				auto flexLayout = dynamic_cast<FlexLayout*>(getLayout());
				if(flexLayout)
				{
					flexLayout->setComponentInfo(child, grow, shrink, basis);
					return true;
				}
			}
		}

		return false;
	}

	bool Component::getStringProperty(fenster::ComponentProperty property, std::string& out)
	{
		if(property == fenster::ComponentProperty::Title)
		{
			if(auto titled = dynamic_cast<TitledComponent*>(this))
			{
				out = titled->getTitle();
				return true;
			}
		}

		return false;
	}

	bool Component::getChildReference(Component* child, ComponentChildReference& out)
	{
		fenster::platformAcquireMutex(childrenLock);
		for(auto& ref: children)
		{
			if(ref.component == child)
			{
				out = ref;
				fenster::platformReleaseMutex(childrenLock);
				return true;
			}
		}
		fenster::platformReleaseMutex(childrenLock);
		return false;
	}
}
