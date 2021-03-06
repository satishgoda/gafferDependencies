
//
// This source file is part of appleseed.
// Visit http://appleseedhq.net/ for additional information and resources.
//
// This software is released under the MIT license.
//
// Copyright (c) 2010-2013 Francois Beaune, Jupiter Jazz Limited
// Copyright (c) 2014-2015 Francois Beaune, The appleseedhq Organization
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//

#ifndef APPLESEED_RENDERER_KERNEL_RENDERING_ISHADINGRESULTFRAMEBUFFERFACTORY_H
#define APPLESEED_RENDERER_KERNEL_RENDERING_ISHADINGRESULTFRAMEBUFFERFACTORY_H

// appleseed.foundation headers.
#include "foundation/core/concepts/iunknown.h"
#include "foundation/math/aabb.h"

// Standard headers.
#include <cstddef>

// Forward declarations.
namespace renderer  { class Frame; }
namespace renderer  { class ShadingResultFrameBuffer; }

namespace renderer
{

class IShadingResultFrameBufferFactory
  : public foundation::IUnknown
{
  public:
    virtual ShadingResultFrameBuffer* create(
        const Frame&                frame,
        const size_t                tile_x,
        const size_t                tile_y,
        const foundation::AABB2u&   tile_bbox) = 0;

    virtual void destroy(
        ShadingResultFrameBuffer*   framebuffer) = 0;
};

}       // namespace renderer

#endif  // !APPLESEED_RENDERER_KERNEL_RENDERING_ISHADINGRESULTFRAMEBUFFERFACTORY_H
