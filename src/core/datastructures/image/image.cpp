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

#include <inviwo/core/datastructures/image/image.h>
#include <inviwo/core/datastructures/image/imageram.h>
#include <inviwo/core/datastructures/image/imagedisk.h>

namespace inviwo {

Image::Image(uvec2 dimensions, ImageType type, const DataFormatBase* format, bool allowMissingLayers)
    : DataGroup()
    , allowMissingLayers_(allowMissingLayers)
    , imageType_(type) {
    initialize(dimensions, format);
}

Image::Image(Layer* colorLayer, ImageType type, bool allowMissingLayers)
: DataGroup()
, allowMissingLayers_(allowMissingLayers)
, imageType_(type) {
    if(colorLayer){
        initialize(colorLayer->getDimension(), colorLayer->getDataFormat(), colorLayer);
    }
    else{
        initialize(uvec2(32,32), DataVec4UINT8::get());
    }
}

Image::Image(const Image& rhs)
    : DataGroup(rhs)
    , allowMissingLayers_(rhs.allowMissingLayers_)
    , imageType_(rhs.imageType_)
    , inputSources_(rhs.inputSources_) {
    for (std::vector<Layer*>::const_iterator it = rhs.colorLayers_.begin() ; it != rhs.colorLayers_.end(); ++it)
        addColorLayer((*it)->clone());

    if (rhs.depthLayer_) {
        depthLayer_ = rhs.depthLayer_->clone();
        addLayer(depthLayer_);
    } else
        depthLayer_ = NULL;

    if (rhs.pickingLayer_) {
        pickingLayer_ = rhs.pickingLayer_->clone();
        addLayer(pickingLayer_);
    } else
        pickingLayer_ = NULL;

    for (size_t i = 0; i < rhs.representations_.size(); ++i) {
        representations_.push_back(rhs.representations_[i]->clone());
        ImageRepresentation* imRep = dynamic_cast<ImageRepresentation*>(representations_[i]);

        if (imRep) {
            imRep->setPointerToOwner(this);
            imRep->setAsInvalid();
            //imRep->update(true);
        }
    }
}

Image& Image::operator=(const Image& that) {
    if (this != &that) {
        DataGroup::operator=(that);
        allowMissingLayers_ = that.allowMissingLayers_;
        imageType_ = that.imageType_;
        inputSources_ = that.inputSources_;
        deinitialize();

        for (std::vector<Layer*>::const_iterator it = that.colorLayers_.begin(); it != that.colorLayers_.end(); ++it)
            addColorLayer((*it)->clone());

        if (that.depthLayer_) {
            depthLayer_ = that.depthLayer_->clone();
            addLayer(depthLayer_);
        } else
            depthLayer_ = NULL;

        if (that.pickingLayer_) {
            pickingLayer_ = that.pickingLayer_->clone();
            addLayer(pickingLayer_);
        } else
            pickingLayer_ = NULL;

        for (size_t i = 0; i < that.representations_.size(); ++i) {
            representations_.push_back(that.representations_[i]->clone());
            ImageRepresentation* imRep = dynamic_cast<ImageRepresentation*>(representations_[i]);

            if (imRep) {
                imRep->setPointerToOwner(this);
                imRep->setAsInvalid();
                //imRep->update(true);
            }
        }
    }

    return *this;
}

Image* Image::clone() const {
    return new Image(*this);
}

Image::~Image() {
    //Delete all layers
    deinitialize();
}

std::string Image::getDataInfo() const{
    std::stringstream ss;
    ss << "<table border='0' cellspacing='0' cellpadding='0' style='border-color:white;white-space:pre;'>\n"
       << "<tr><td style='color:#bbb;padding-right:8px;'>Type</td><td><nobr>" << "Image </nobr></td></tr>\n"
       << "<tr><td style='color:#bbb;padding-right:8px;'>Color channels</td><td><nobr>" << colorLayers_.size() << "</nobr></td></tr>\n"
       << "<tr><td style='color:#bbb;padding-right:8px;'>Depth</td><td><nobr>" << (getDepthLayer() ? "Yes" : "No") << "</nobr></td></tr>\n"
       << "<tr><td style='color:#bbb;padding-right:8px;'>Picking</td><td><nobr>" << (getPickingLayer() ? "Yes" : "No") << "</nobr></td></tr>\n"
       << "<tr><td style='color:#bbb;padding-right:8px;'>Format</td><td><nobr>" << getDataFormat()->getString() << "</nobr></td></tr>\n"
       << "<tr><td style='color:#bbb;padding-right:8px;'>Dimension</td><td><nobr>" << "(" << getDimension().x << ", "
       << getDimension().y << ")" << "</nobr></td></tr>\n"
       << "</tr></table>\n";
    return ss.str();
}

void Image::deinitialize() {
    for (std::vector<Layer*>::iterator it = colorLayers_.begin(); it != colorLayers_.end(); ++it)
        delete(*it);
    colorLayers_.clear();
    delete depthLayer_;
    delete pickingLayer_;
}

void Image::initialize(uvec2 dimensions, const DataFormatBase* format, Layer* colorLayer) {
    if(colorLayer)
        addColorLayer(colorLayer);
    else
        addColorLayer(new Layer(dimensions, format));

    depthLayer_ = NULL;
    pickingLayer_ = NULL;

    if (!allowMissingLayers_ || typeContainsDepth(getImageType())) {
        depthLayer_ = new Layer(dimensions, DataFLOAT32::get(), DEPTH_LAYER);
        addLayer(depthLayer_);
    }
    else
        depthLayer_ = NULL;

    if (!allowMissingLayers_ || typeContainsPicking(getImageType())) {
        pickingLayer_ = new Layer(dimensions, format, PICKING_LAYER);
        addLayer(pickingLayer_);
    }
    else
        pickingLayer_ = NULL;
}

uvec2 Image::getDimension() const {
    return getColorLayer()->getDimension();
}

size_t Image::addColorLayer(Layer* layer) {
    colorLayers_.push_back(layer);
    addLayer(layer);
    //Return index to this layer
    return colorLayers_.size()-1;
}

const std::vector<const Layer*>* Image::getAllLayers() const {
    return &allLayersConst_;
}

const std::vector<Layer*>* Image::getAllLayers() {
    return &allLayers_;
}

const Layer* Image::getLayer(LayerType type, size_t idx) const {
    switch (type) {
        case COLOR_LAYER:
            return getColorLayer(idx);

        case DEPTH_LAYER:
            return getDepthLayer();

        case PICKING_LAYER:
            return getPickingLayer();
    }

    return NULL;
}

Layer* Image::getLayer(LayerType type, size_t idx) {
    switch (type) {
        case COLOR_LAYER:
            return getColorLayer(idx);

        case DEPTH_LAYER:
            return getDepthLayer();

        case PICKING_LAYER:
            return getPickingLayer();
    }

    return NULL;
}

const Layer* Image::getColorLayer(size_t idx) const {
    return colorLayers_[idx];
}

Layer* Image::getColorLayer(size_t idx) {
    return colorLayers_[idx];
}

size_t Image::getNumberOfColorLayers() const {
    return colorLayers_.size();
}

const Layer* Image::getDepthLayer() const {
    // Get local depth layer if available. 
    if (typeContainsDepth(getImageType())
        && depthLayer_
        && depthLayer_->getLayerType() == DEPTH_LAYER) {
        return depthLayer_;
    }

    // Look for a depth layer upwards in the network using the uinput sources.
    ImageSourceMap::const_iterator it = inputSources_.find(DEPTH_LAYER);
    if (it != inputSources_.end() && it->second) {
        if(it->second)
            return it->second->getDepthLayer();
    }

    // No depth layer found.
    return NULL;
}

Layer* Image::getDepthLayer() {
    return depthLayer_;
}

const Layer* Image::getPickingLayer() const {
    // Get local picking layer if available.
    if (typeContainsPicking(getImageType())
        && pickingLayer_
        && pickingLayer_->getLayerType() == PICKING_LAYER) {
        return pickingLayer_;
    } 

    // Look for a picking layer upwards in the network using the input sources
    ImageSourceMap::const_iterator it = inputSources_.find(PICKING_LAYER);
    if (it != inputSources_.end() && it->second) {
        if(it->second)
            return it->second->getPickingLayer();
    }
   
    // No picking layer found.
    return NULL;
}

Layer* Image::getPickingLayer() {
    return pickingLayer_;
}

void Image::resize(uvec2 dimensions) {
    setRepresentationsAsInvalid();

    //Resize all layers
    for (std::vector<Layer*>::iterator it = colorLayers_.begin() ; it != colorLayers_.end(); ++it)
        (*it)->resize(dimensions);

    if (depthLayer_)
        depthLayer_->resize(dimensions);

    if (pickingLayer_)
        pickingLayer_->resize(dimensions);
}

void Image::resizeRepresentations(Image* targetImage, uvec2 targetDim) {
    // targetImage->resize(targetDim);
    std::vector<DataGroupRepresentation*>& targetRepresentations = targetImage->representations_;

    if (targetRepresentations.size()) {
        // Avoid resize of ImageRAM and ImageDisk if we have another representation
        bool existsMoreThenDiskAndRAMRepresentation = false;

        for (size_t j = 0; j < targetRepresentations.size(); j++) {
            if (!dynamic_cast<ImageRAM*>(targetRepresentations[j]) &&
                !dynamic_cast<ImageDisk*>(targetRepresentations[j]))
                existsMoreThenDiskAndRAMRepresentation = true;
        }

        ImageRepresentation* sourceImageRepresentation = 0;
        ImageRepresentation* targetImageRepresentation = 0;

        for (size_t i = 0; i < representations_.size(); i++) {
            for (size_t j = 0; j < targetRepresentations.size(); j++) {
                if (typeid(*representations_[i]) == typeid(*targetRepresentations[j])) {
                    if (!existsMoreThenDiskAndRAMRepresentation ||
                        (!dynamic_cast<ImageRAM*>(targetRepresentations[j]) &&
                         !dynamic_cast<ImageDisk*>(targetRepresentations[j]))) {
                        sourceImageRepresentation =
                            static_cast<ImageRepresentation*>(representations_[i]);
                        sourceImageRepresentation->update(false);
                        targetImageRepresentation =
                            static_cast<ImageRepresentation*>(targetRepresentations[j]);
                        targetImageRepresentation->update(false);
                        sourceImageRepresentation->copyAndResizeRepresentation(
                            targetImageRepresentation);
                    }
                }
            }
        }
    } else {
        // If not representation exist, create ImageRAM one
        const ImageRAM* imageRAM = this->getRepresentation<ImageRAM>();
        imageRAM->copyAndResizeRepresentation(targetImage->getEditableRepresentation<ImageRAM>());
    }
}

ImageType Image::getImageType() const {
    return imageType_;
}

const DataFormatBase* Image::getDataFormat() const {
    return getColorLayer()->getDataFormat();
}

void Image::setInputSource(LayerType layer, const Image* src) {
    inputSources_[layer] = src;
}

void Image::addLayer(Layer* layer) {
    allLayers_.push_back(layer);
    allLayersConst_.push_back(static_cast<const Layer*>(layer));
}





} // namespace
