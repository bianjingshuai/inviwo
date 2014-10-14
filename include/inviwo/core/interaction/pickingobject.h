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
 * Contact: Erik Sund�n
 *
 *********************************************************************************/

#ifndef IVW_PICKINGOBJECT_H
#define IVW_PICKINGOBJECT_H

#include <inviwo/core/common/inviwocoredefine.h>
#include <inviwo/core/common/inviwo.h>
#include <inviwo/core/interaction/pickingcallback.h>

namespace inviwo {

/** \class PickingObject
 */
class IVW_CORE_API PickingObject {

public:
    PickingObject(size_t, DataVec3UINT8::type);

    virtual ~PickingObject();

    const size_t& getPickingId() const;
    const vec3& getPickingColor() const;
    const DataVec3UINT8::type& getPickingColorAsUINT8() const;
    const vec2& getPickingPosition() const;
    const vec2& getPickingMove() const;

    void setReadDepth(bool);
    bool readDepth();
    const double& getPickingDepth() const;

    void picked() const;

    void setPickingMove(vec2);
    void setPickingPosition(vec2);
    void setPickingDepth(double);

    PickingCallback* getCallbackContainer();

private:
    size_t id_;
    DataVec3UINT8::type colorUINT8_;
    vec3 color_;
    vec2 pos_;
    bool readDepth_;
    double depth_;
    vec2 move_;
    PickingCallback* onPickedCallback_;
};

} // namespace

#endif // IVW_PICKINGOBJECT_H