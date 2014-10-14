/*********************************************************************************
 *
 * Inviwo - Interactive Visualization Workshop
 * Version 0.6b
 *
 * Copyright (c) 2012-2014 Inviwo Foundation
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 * list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * 
 * Contact: Sathish Kottravel
 *
 *********************************************************************************/

#include <inviwo/core/interaction/events/mouseevent.h>

namespace inviwo {

MouseEvent::MouseEvent(ivec2 position, int delta, MouseEvent::MouseButton button,
                       MouseEvent::MouseState state, MouseEvent::MouseWheelOrientation orientation, 
                       InteractionEvent::Modifier modifier, uvec2 canvasSize)
    : InteractionEvent(),
      position_(position),
      wheelSteps_(delta),
      state_(state),
      wheelOrientation_(orientation),
      canvasSize_(canvasSize) {
    buttonNames_[MOUSE_BUTTON_LEFT] = "Left mouse button";
    buttonNames_[MOUSE_BUTTON_RIGHT] = "Right mouse button";
    buttonNames_[MOUSE_BUTTON_MIDDLE] = "Middle mouse button";
    buttonNames_[MOUSE_BUTTON_NONE] = "";
    modifierName_ = modifierNames_[modifier];
    modifier_ = modifier;
    buttonName_ = buttonNames_[button];
    button_ = button;
}

MouseEvent::MouseEvent(ivec2 position, MouseEvent::MouseButton button,
                       MouseEvent::MouseState state, InteractionEvent::Modifier modifier, uvec2 canvasSize)
    : InteractionEvent(),
      position_(position),
      wheelSteps_(0),
      state_(state),
      wheelOrientation_(MOUSE_WHEEL_NONE),
      canvasSize_(canvasSize) {
    buttonNames_[MOUSE_BUTTON_LEFT] = "Left mouse button";
    buttonNames_[MOUSE_BUTTON_RIGHT] = "Right mouse button";
    buttonNames_[MOUSE_BUTTON_MIDDLE] = "Middle mouse button";
    buttonNames_[MOUSE_BUTTON_NONE] = "";
    modifierName_ = modifierNames_[modifier];
    modifier_ = modifier;
    buttonName_ = buttonNames_[button];
    button_ = button;
}

MouseEvent::MouseEvent(MouseEvent::MouseButton button, InteractionEvent::Modifier modifier)
    : InteractionEvent(), 
    position_(ivec2(0)),
    wheelSteps_(0),
    state_(MOUSE_STATE_NONE),
    wheelOrientation_(MOUSE_WHEEL_NONE),
    canvasSize_(uvec2(0)) {
    buttonNames_[MOUSE_BUTTON_LEFT] = "Left mouse button";
    buttonNames_[MOUSE_BUTTON_RIGHT] = "Right mouse button";
    buttonNames_[MOUSE_BUTTON_MIDDLE] = "Middle mouse button";
    buttonNames_[MOUSE_BUTTON_NONE] = "";
    modifierName_ = modifierNames_[modifier];
    modifier_ = modifier;
    buttonName_ = buttonNames_[button];
    button_ = button;
}

MouseEvent::~MouseEvent() {}

void MouseEvent::modify(ivec2 newPosition, uvec2 newCanvasSize) {
    position_ = newPosition;
    canvasSize_ = newCanvasSize;
};

void MouseEvent::serialize(IvwSerializer& s) const {
    InteractionEvent::serialize(s);
    s.serialize("button", buttonName_);
}

void MouseEvent::deserialize(IvwDeserializer& d) {
    InteractionEvent::deserialize(d);
    d.deserialize("button", buttonName_);

    for (int i = 0; i < COUNT; ++i) {
        if (buttonNames_[i] == buttonName_) {
            button_ = i;
            break;
        }
    }
}

} // namespace