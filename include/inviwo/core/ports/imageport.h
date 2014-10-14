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

#ifndef IVW_IMAGEPORT_H
#define IVW_IMAGEPORT_H

#include <inviwo/core/common/inviwocoredefine.h>
#include <inviwo/core/ports/datainport.h>
#include <inviwo/core/ports/dataoutport.h>
#include <inviwo/core/datastructures/image/image.h>
#include <inviwo/core/interaction/events/eventhandler.h>
#include <inviwo/core/interaction/events/resizeevent.h>

namespace inviwo {

class IVW_CORE_API ImageInport : public DataInport<Image> {
public:
    ImageInport(std::string identifier, bool outportDeterminesSize = false,
                PropertyOwner::InvalidationLevel invalidationLevel = PropertyOwner::INVALID_OUTPUT);
    virtual ~ImageInport();

    /** 
     * Connects this inport to the outport. Propagates 
     * the inport size to the outport if the processor is 
     * an end processor (Canvas) or any of the 
     * dependent outports of this inport are connected. 
     * 
     * @note Does not check if the outport is an ImageOutport
     * @param Outport * outport ImageOutport to connect
     */
    virtual void connectTo(Outport* outport);

    void changeDataDimensions(ResizeEvent* resizeEvent);
    uvec2 getDimension() const;
    const Image* getData() const;
    uvec3 getColorCode() const;
    void setOutportDeterminesSize(bool outportDeterminesSize);
    bool isOutportDeterminingSize() const;
    static uvec3 colorCode;
    virtual std::string getClassIdentifier() const { return "org.inviwo.ImageInport"; }
    virtual std::string getContentInfo() const;

    void setResizeScale(vec2);
    vec2 getResizeScale();

protected:
    void propagateResizeToPredecessor(ResizeEvent* resizeEvent);

private:
    uvec2 dimensions_;
    vec2 resizeScale_;
    bool outportDeterminesSize_;
};

class IVW_CORE_API ImageOutport : public DataOutport<Image>, public EventHandler {
    friend class ImageInport;

public:
    ImageOutport(std::string identifier,
                 PropertyOwner::InvalidationLevel invalidationLevel = PropertyOwner::INVALID_OUTPUT,
                 bool handleResizeEvents = true);
    ImageOutport(std::string identifier, ImageType type,
                 const DataFormatBase* format = DataVec4UINT8::get(),
                 PropertyOwner::InvalidationLevel invalidationLevel = PropertyOwner::INVALID_OUTPUT,
                 bool handleResizeEvents = true);
    ImageOutport(std::string identifier, ImageInport* src, ImageType type = COLOR_DEPTH,
                 PropertyOwner::InvalidationLevel invalidationLevel = PropertyOwner::INVALID_OUTPUT,
                 bool handleResizeEvents = true);
    virtual ~ImageOutport();

    virtual void invalidate(PropertyOwner::InvalidationLevel invalidationLevel);

    Image* getData();

    virtual void dataChanged();

    /**
     * Resize port and propagate the resizing to the canvas.
     *
     * @param resizeEvent
     */
    void changeDataDimensions(ResizeEvent* resizeEvent);
    uvec2 getDimension() const;

    /**
     * Set the dimension of this port without propagating the size
     * through the network. Will resize the image contained within the port.
     *
     * @param newDimension Dimension to be set
     */
    void setDimension(const uvec2& newDimension);
    uvec3 getColorCode() const;
    virtual std::string getClassIdentifier() const { return "org.inviwo.ImageOutport"; }

    bool addResizeEventListener(EventListener*);
    bool removeResizeEventListener(EventListener*);

    /**
     * Determine if the image data should be
     * resized during a resize event.
     * Also prevents resize events from being propagated further.
     * @param handleResizeEvents True if data should be resized during a resize propagation,
     * otherwise false
     */
    void setHandleResizeEvents(bool handleResizeEvents) {
        handleResizeEvents_ = handleResizeEvents;
    }
    bool isHandlingResizeEvents() const { return handleResizeEvents_; }

    void setInputSource(LayerType, ImageInport*);

protected:
    Image* getResizedImageData(uvec2 dimensions);
    void setLargestImageData(ResizeEvent* resizeEvent);
    /** 
     * \brief Propagates resize event to this processor's inports within the same dependency set (group)
     * 
     * @param ResizeEvent * resizeEvent Event to be propagated
     */
    void propagateResizeEventToPredecessor(ResizeEvent* resizeEvent);
    ResizeEvent* scaleResizeEvent(ImageInport*, ResizeEvent*);
    void updateInputSources();

private:
    uvec2 dimensions_;
    bool mapDataInvalid_;
    bool handleResizeEvents_;  // True if data should be resized during a resize propagation,
                               // otherwise false
    typedef std::map<std::string, Image*> ImagePortMap;
    ImagePortMap imageDataMap_;
    typedef std::map<LayerType, const ImageInport*> ImageInSourceMap;
    ImageInSourceMap inputSources_;
};

} // namespace

#endif // IVW_IMAGEPORT_H
