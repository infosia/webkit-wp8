/*
    Copyright (C) 2012 Samsung Electronics

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
    Boston, MA 02110-1301, USA.
*/

#ifndef AcceleratedCompositingContextEfl_h
#define AcceleratedCompositingContextEfl_h

#if USE(TEXTURE_MAPPER_GL)
#include "EvasGLContext.h"
#include "EvasGLSurface.h"
#include "TextureMapperFPSCounter.h"
#include "Timer.h"
#include <wtf/Noncopyable.h>
#include <wtf/efl/UniquePtrEfl.h>

namespace WebCore {

class GraphicsLayer;
class HostWindow;
class TextureMapper;
class TextureMapperLayer;

class AcceleratedCompositingContext {
    WTF_MAKE_NONCOPYABLE(AcceleratedCompositingContext);
public:
    AcceleratedCompositingContext(Evas_Object* ewkView, Evas_Object* compositingObject);
    ~AcceleratedCompositingContext();

    void setRootGraphicsLayer(GraphicsLayer* rootLayer);
    void resize(const IntSize&);
    void flushAndRenderLayers();

    void extractImageData(Evas_Object*, const IntRect&);

private:
    void paintToGraphicsContext();
    void paintToCurrentGLContext();
    void compositeLayers();

    bool flushPendingLayerChanges();
    void syncLayers(Timer<AcceleratedCompositingContext>*);

    PassRefPtr<cairo_surface_t> getImageData(const IntRect&);
    PassRefPtr<cairo_surface_t> getImageDataGL(const IntRect&);

    Evas_Object* m_view;
    Evas_Object* m_compositingObject;

    std::unique_ptr<TextureMapper> m_textureMapper;
    GraphicsLayer* m_rootLayer;
    Timer<AcceleratedCompositingContext> m_syncTimer;

    EflUniquePtr<Evas_GL> m_evasGL;
    std::unique_ptr<EvasGLContext> m_evasGLContext;
    std::unique_ptr<EvasGLSurface> m_evasGLSurface;

    TextureMapperFPSCounter m_fpsCounter;

    IntSize m_viewSize;
    bool m_isAccelerated;
};

} // namespace WebCore

#endif // USE(TEXTURE_MAPPER_GL)
#endif // AcceleratedCompositingContextEfl_h
