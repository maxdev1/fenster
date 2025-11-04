// fenster — lightweight window server and UI toolkit
// Copyright (c) 2025 Max Schlüssel
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef FENSTER_SERVER_COMPONENTS_COMPONENT
#define FENSTER_SERVER_COMPONENTS_COMPONENT

#include "components/bounding_component.hpp"
#include "components/event_listener_info.hpp"
#include "events/key_event.hpp"
#include "events/mouse_event.hpp"
#include "focusable_component.hpp"
#include "layout/layout.hpp"
#include "video/graphics.hpp"

#include <libfenster/interface.hpp>
#include <libfenster/metrics/rectangle.hpp>
#include <vector>
#include <bits/std_function.h>


namespace fensterserver
{
    // forward declarations
    class Window;

    /**
     * A component requirement is a flag that is put on the component
     * to mark that one specific action or multiple actions are required
     * for the component.
     *
     * For example, when a component has changed model data in an update
     * cycle, the component may also want to repaint its content, therefore
     * marks itself as paint-required.
     */
#define COMPONENT_REQUIREMENT_NONE  0b0000
#define COMPONENT_REQUIREMENT_PAINT 0b0001
#define COMPONENT_REQUIREMENT_LAYOUT 0b0010
#define COMPONENT_REQUIREMENT_UPDATE 0b0100
#define COMPONENT_REQUIREMENT_ALL (COMPONENT_REQUIREMENT_PAINT | COMPONENT_REQUIREMENT_LAYOUT | COMPONENT_REQUIREMENT_UPDATE)

    typedef uint8_t ComponentRequirement;

    /**
     * Type used to differentiate how a child component relates to the parent
     * component. If it is an internal component that may not be deleted
     * separately, it is referenced with the internal type.
     */
    typedef uint8_t ComponentChildReferenceType;
#define COMPONENT_CHILD_REFERENCE_TYPE_DEFAULT 0
#define COMPONENT_CHILD_REFERENCE_TYPE_INTERNAL 1

    class Component;

    class ComponentChildReference
    {
    public:
        Component* component;
        ComponentChildReferenceType type;
    };

    struct ComponentListenerEntry
    {
        fenster::ComponentEventType type;
        EventListenerInfo info;
    };

    /**
     *
     */
    class Component : public BoundingComponent, public FocusableComponent
    {
    protected:
        SYS_MUTEX_T lock = fenster::platformInitializeMutex(true);
        fenster::Rectangle bounds;
        Component* parent;
        std::vector<ComponentChildReference> children;
        SYS_MUTEX_T childrenLock = fenster::platformInitializeMutex(true);

        fenster::Dimension minimumSize;
        fenster::Dimension preferredSize;
        fenster::Dimension maximumSize;

        ComponentRequirement requirements;
        ComponentRequirement childRequirements;

        std::vector<ComponentListenerEntry*> listeners;

        int zIndex = 1000;

        Layout* _layout;
        Graphics graphics;

        bool visible;

        /**
         * If a component that extends this class does not need to paint anything, it should return false here.
         */
        virtual bool hasGraphics() const
        {
            return true;
        }

    public:
        fenster::ComponentId id;

        Component();
        ~Component() override;

        bool isVisible() const
        {
            return this->visible;
        }

        /**
         * Sets the z-index of this component on its parent. When new children are added,
         * all children are sorted based on this z-index.
         */
        void setZIndex(int zIndex)
        {
            this->zIndex = zIndex;
        }

        /**
         * Returns the components parent
         *
         * @return the components parent
         */
        Component* getParent()
        {
            return parent;
        }

        std::vector<ComponentChildReference>& acquireChildren();

        void releaseChildren() const;

        bool canHandleEvents() const;

        void setVisible(bool visible);

        /**
         * Sets the bounds of the component and recreates its graphics buffer.
         *
         * @param bounds	the new bounds of the component
         */
        void setBoundsInternal(const fenster::Rectangle& bounds) override;

        /**
         * Returns the bounds of the component.
         *
         * @return the bounds
         */
        fenster::Rectangle getBounds() const;

        void setPreferredSize(const fenster::Dimension& size);

        virtual fenster::Dimension getPreferredSize()
        {
            return preferredSize;
        }

        virtual fenster::Dimension getEffectivePreferredSize();

        void setMinimumSize(const fenster::Dimension& size);

        fenster::Dimension getMinimumSize() const
        {
            return minimumSize;
        }

        void setMaximumSize(const fenster::Dimension& size);

        fenster::Dimension getMaximumSize() const
        {
            return maximumSize;
        }

        /**
         * This method is used to blit the component and all of its children
         * to the out buffer
         *
         * @param clip	absolute bounds that may not be exceeded
         * @param screenPosition	absolute screen position to blit to
         */
        virtual void blit(Graphics* out, const fenster::Rectangle& clip, const fenster::Point& screenPosition);

        virtual void blitChildren(Graphics* out, const fenster::Rectangle& clip, const fenster::Point& screenPosition);

        /**
         * Adds the given component as a child to this component
         *
         * @param comp	the component to add
         */
        virtual void addChild(Component* comp,
                              ComponentChildReferenceType type = COMPONENT_CHILD_REFERENCE_TYPE_DEFAULT);

        /**
         * Removes the given component from this component
         *
         * @param comp	the component to remove
         */
        virtual void removeChild(Component* comp);

        /**
         * Returns the component at the given Point
         *
         * @param p		the Point
         */
        virtual Component* getComponentAt(fenster::Point p);

        /**
         * Returns the first in the hierarchy that is a Window
         *
         * @return the window
         */
        virtual Window* getWindow();

        /**
         * @return whether this type of component is a window.
         */
        virtual bool isWindow()
        {
            return false;
        }

        /**
         * Brings this component to the front
         */
        virtual void bringToFront();

        /**
         * Brings the given child component to the front
         *
         * @param comp	the child component
         */
        virtual void bringChildToFront(Component* comp);

        /**
         * Returns the location of the component on screen
         *
         * @return the location
         */
        virtual fenster::Point getLocationOnScreen();

        virtual Component* handleMouseEvent(MouseEvent& event);

        virtual Component* handleKeyEvent(KeyEvent& event);
        bool sendKeyEventToListener(KeyEvent& event);

        virtual void handleBoundChanged(const fenster::Rectangle& oldBounds)
        {
            // May be implemented by subtypes
        }

        virtual void setLayout(Layout* layout);

        virtual Layout* getLayout() const
        {
            return _layout;
        }

        /**
         * Marks the given area as dirty so it is copied to the framebuffer
         *
         * @param rect	the Rectangle to mark dirty
         */
        virtual void markDirty(fenster::Rectangle rect);

        /**
         * Marks the entire component as dirty
         */
        virtual void markDirty()
        {
            markDirty(fenster::Rectangle(0, 0, bounds.width, bounds.height));
        }

        /**
         * Places the flag for the given requirement on the parent component (if non-null).
         */
        void markParentFor(ComponentRequirement req);

        /**
         * Places the flag for the given requirement on this component.
         */
        void markFor(ComponentRequirement req);

        /**
         * Places the flag for the given requirement in the list of child-requirements.
         */
        void markChildsFor(ComponentRequirement req);

        /**
         * Resolves the given requirement
         */
        void resolveRequirement(ComponentRequirement req, int lvl);

        bool hasChildRequirements() const
        {
            return childRequirements != COMPONENT_REQUIREMENT_NONE;
        }

        bool isChildOf(Component* c);

        /**
         * Returns the reference to the given component (if the given component is a child of this component).
         *
         * @return true if the component was found
         */
        bool getChildReference(Component* child, ComponentChildReference& out);

        /**
         * This method is called by the window manager if the update requirement flag is set.
         * The component may here change the contents of it's model.
         */
        virtual void update();

        /**
         * This method is called by the window manager if the layout requirement flag is set.
         * The component may here change the bounds of each child component.
         */
        virtual void layout();

        /**
         * This method is called by the window manager if the paint requirement flag is set.
         * The component may here repaint itself.
         */
        virtual void paint();

        void clearSurface();

        virtual bool getNumericProperty(int property, uint32_t* out);
        virtual bool setNumericProperty(int property, uint32_t value);

        virtual bool getStringProperty(int property, std::string& out);
        virtual bool setStringProperty(int property, std::string value);

        void addListener(fenster::ComponentEventType eventType, SYS_TID_T target_thread, fenster::ComponentId id);
        bool callForListeners(fenster::ComponentEventType eventType,
                              const std::function<void(EventListenerInfo&)>& func);

        void recheckGraphics();
    };
}

#endif
