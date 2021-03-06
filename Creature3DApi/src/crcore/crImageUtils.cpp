/* -*-c++-*- OpenSceneGraph - Copyright (C) 1998-2006 Robert Osfield 
 *
 * This library is open source and may be redistributed and/or modified under  
 * the terms of the OpenSceneGraph Public License (OSGPL) version 0.0 or 
 * (at your option) any later version.  The full license is in LICENSE file
 * included with this distribution, and on the openscenegraph.org website.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the 
 * OpenSceneGraph Public License for more details.
*/
//Modified by Wucaihua
#include <float.h>
#include <string.h>

#include <CRCore/crMath.h>
#include <CRCore/crNotify.h>
#include <CRCore/crImageUtils.h>
#include <CRCore/crTexture.h>

namespace CRCore
{

struct FindRangeOperator
{
    FindRangeOperator():
        m_rmin(FLT_MAX),
        m_rmax(-FLT_MAX),
        m_gmin(FLT_MAX),
        m_gmax(-FLT_MAX),
        m_bmin(FLT_MAX),
        m_bmax(-FLT_MAX),
        m_amin(FLT_MAX),
        m_amax(-FLT_MAX) {}
        
    float m_rmin, m_rmax, m_gmin, m_gmax, m_bmin, m_bmax, m_amin, m_amax;

    inline void luminance(float l) { rgba(l,l,l,l); } 
    inline void alpha(float a) { rgba(1.0f,1.0f,1.0f,a); } 
    inline void luminance_alpha(float l,float a) { rgba(l,l,l,a); } 
    inline void rgb(float r,float g,float b) { rgba(r,g,b,1.0f);  }
    inline void rgba(float r,float g,float b,float a)
    {
        m_rmin = CRCore::minimum(r,m_rmin); 
        m_rmax = CRCore::maximum(r,m_rmax); 
        m_gmin = CRCore::minimum(g,m_gmin); 
        m_gmax = CRCore::maximum(g,m_gmax); 
        m_bmin = CRCore::minimum(b,m_bmin); 
        m_bmax = CRCore::maximum(b,m_bmax); 
        m_amin = CRCore::minimum(a,m_amin); 
        m_amax = CRCore::maximum(a,m_amax);
    }



};

struct OffsetAndScaleOperator
{
    OffsetAndScaleOperator(const CRCore::crVector4& offset, const CRCore::crVector4& scale):
        m_offset(offset), 
        m_scale(scale) {}

    CRCore::crVector4 m_offset;
    CRCore::crVector4 m_scale;

    inline void luminance(float& l) const { l= m_offset.r() + l*m_scale.r(); } 
    inline void alpha(float& a) const { a = m_offset.a() + a*m_scale.a(); } 
    inline void luminance_alpha(float& l,float& a) const
    {
        l= m_offset.r() + l*m_scale.r(); 
        a = m_offset.a() + a*m_scale.a();
    } 
    inline void rgb(float& r,float& g,float& b) const
    {
        r = m_offset.r() + r*m_scale.r(); 
        g = m_offset.g() + g*m_scale.g(); 
        b = m_offset.b() + b*m_scale.b();
    }
    inline void rgba(float& r,float& g,float& b,float& a) const
    {
        r = m_offset.r() + r*m_scale.r(); 
        g = m_offset.g() + g*m_scale.g(); 
        b = m_offset.b() + b*m_scale.b();
        a = m_offset.a() + a*m_scale.a();
    }
};

bool computeMinMax(const CRCore::crImage* image, CRCore::crVector4& minValue, CRCore::crVector4& maxValue)
{
    if (!image) return false;

    CRCore::FindRangeOperator rangeOp;    
    readImage(image, rangeOp);
    minValue.r() = rangeOp.m_rmin;
    minValue.g() = rangeOp.m_gmin;
    minValue.b() = rangeOp.m_bmin;
    minValue.a() = rangeOp.m_amin;

    maxValue.r() = rangeOp.m_rmax;
    maxValue.g() = rangeOp.m_gmax;
    maxValue.b() = rangeOp.m_bmax;
    maxValue.a() = rangeOp.m_amax;
    
    return minValue.r()<=maxValue.r() && 
           minValue.g()<=maxValue.g() &&
           minValue.b()<=maxValue.b() &&
           minValue.a()<=maxValue.a();
}

bool offsetAndScaleImage(CRCore::crImage* image, const CRCore::crVector4& offset, const CRCore::crVector4& scale)
{
    if (!image) return false;

    modifyImage(image,OffsetAndScaleOperator(offset, scale));
    
    return true;
}

template<typename SRC, typename DEST>
void _copyRowAndScale(const SRC* src, DEST* dest, int num, float scale)
{
    if (scale==1.0)
    {
        for(int i=0; i<num; ++i)
        {
            *dest = DEST(*src);
            ++dest; ++src;
        }
    }
    else
    {
        for(int i=0; i<num; ++i)
        {
            *dest = DEST(float(*src)*scale);
            ++dest; ++src;
        }
    }
}

template<typename DEST>
void _copyRowAndScale(const unsigned char* src, GLenum srcDataType, DEST* dest, int num, float scale)
{
    switch(srcDataType)
    {
        case(GL_BYTE):              _copyRowAndScale((char*)src, dest, num, scale); break;
        case(GL_UNSIGNED_BYTE):     _copyRowAndScale((unsigned char*)src, dest, num, scale); break;
        case(GL_SHORT):             _copyRowAndScale((short*)src, dest, num, scale); break;
        case(GL_UNSIGNED_SHORT):    _copyRowAndScale((unsigned short*)src, dest, num, scale); break;
        case(GL_INT):               _copyRowAndScale((int*)src, dest, num, scale); break;
        case(GL_UNSIGNED_INT):      _copyRowAndScale((unsigned int*)src, dest, num, scale); break;
        case(GL_FLOAT):             _copyRowAndScale((float*)src, dest, num, scale); break;
    }
}

void _copyRowAndScale(const unsigned char* src, GLenum srcDataType, unsigned char* dest, GLenum dstDataType, int num, float scale)
{
    switch(dstDataType)
    {
        case(GL_BYTE):              _copyRowAndScale(src, srcDataType, (char*)dest, num, scale); break;
        case(GL_UNSIGNED_BYTE):     _copyRowAndScale(src, srcDataType, (unsigned char*)dest, num, scale); break;
        case(GL_SHORT):             _copyRowAndScale(src, srcDataType, (short*)dest, num, scale); break;
        case(GL_UNSIGNED_SHORT):    _copyRowAndScale(src, srcDataType, (unsigned short*)dest, num, scale); break;
        case(GL_INT):               _copyRowAndScale(src, srcDataType, (int*)dest, num, scale); break;
        case(GL_UNSIGNED_INT):      _copyRowAndScale(src, srcDataType, (unsigned int*)dest, num, scale); break;
        case(GL_FLOAT):             _copyRowAndScale(src, srcDataType, (float*)dest, num, scale); break;
    }
}

struct RecordRowOperator
{
    RecordRowOperator(unsigned int num):_colours(num),_pos(0) {}

    mutable std::vector<CRCore::crVector4>  _colours;
    mutable unsigned int            _pos;
    
    inline void luminance(float l) const { rgba(l,l,l,1.0f); } 
    inline void alpha(float a) const { rgba(1.0f,1.0f,1.0f,a); } 
    inline void luminance_alpha(float l,float a) const { rgba(l,l,l,a);  } 
    inline void rgb(float r,float g,float b) const { rgba(r,g,b,1.0f); }
    inline void rgba(float r,float g,float b,float a) const { _colours[_pos++].set(r,g,b,a); }
};

struct WriteRowOperator
{
    WriteRowOperator():_pos(0) {}
    WriteRowOperator(unsigned int num):_colours(num),_pos(0) {}

    std::vector<CRCore::crVector4>  _colours;
    mutable unsigned int    _pos;
    
    inline void luminance(float& l) const { l = _colours[_pos++].r(); } 
    inline void alpha(float& a) const { a = _colours[_pos++].a(); } 
    inline void luminance_alpha(float& l,float& a) const { l = _colours[_pos].r(); a = _colours[_pos++].a(); } 
    inline void rgb(float& r,float& g,float& b) const { r = _colours[_pos].r(); g = _colours[_pos].g(); b = _colours[_pos].b(); }
    inline void rgba(float& r,float& g,float& b,float& a) const {  r = _colours[_pos].r(); g = _colours[_pos].g(); b = _colours[_pos].b(); a = _colours[_pos++].a(); }
};

bool copyImage(const CRCore::crImage* srcImage, int src_s, int src_t, int src_r, int width, int height, int depth,
               CRCore::crImage* destImage, int dest_s, int dest_t, int dest_r, bool doRescale)
{
    if ((src_s+width) > (dest_s + destImage->s()))
    {
        notify(NOTICE)<<"copyImage("<<srcImage<<", "<<src_s<<", "<< src_t<<", "<<src_r<<", "<<width<<", "<<height<<", "<<depth<<std::endl;
        notify(NOTICE)<<"          "<<destImage<<", "<<dest_s<<", "<< dest_t<<", "<<dest_r<<", "<<doRescale<<")"<<std::endl;
        notify(NOTICE)<<"   input width too large."<<std::endl;
        return false;
    }

    if ((src_t+height) > (dest_t + destImage->t()))
    {
        notify(NOTICE)<<"copyImage("<<srcImage<<", "<<src_s<<", "<< src_t<<", "<<src_r<<", "<<width<<", "<<height<<", "<<depth<<std::endl;
        notify(NOTICE)<<"          "<<destImage<<", "<<dest_s<<", "<< dest_t<<", "<<dest_r<<", "<<doRescale<<")"<<std::endl;
        notify(NOTICE)<<"   input height too large."<<std::endl;
        return false;
    }

    if ((src_r+depth) > (dest_r + destImage->r()))
    {
        notify(NOTICE)<<"copyImage("<<srcImage<<", "<<src_s<<", "<< src_t<<", "<<src_r<<", "<<width<<", "<<height<<", "<<depth<<std::endl;
        notify(NOTICE)<<"          "<<destImage<<", "<<dest_s<<", "<< dest_t<<", "<<dest_r<<", "<<doRescale<<")"<<std::endl;
        notify(NOTICE)<<"   input depth too large."<<std::endl;
        return false;
    }

    float scale = 1.0f;
    if (doRescale && srcImage->getDataType() != destImage->getDataType())
    {
        switch(srcImage->getDataType())
        {
            case(GL_BYTE):              scale = 1.0f/128.0f ; break;
            case(GL_UNSIGNED_BYTE):     scale = 1.0f/255.0f; break;
            case(GL_SHORT):             scale = 1.0f/32768.0f; break;
            case(GL_UNSIGNED_SHORT):    scale = 1.0f/65535.0f; break;
            case(GL_INT):               scale = 1.0f/2147483648.0f; break;
            case(GL_UNSIGNED_INT):      scale = 1.0f/4294967295.0f; break;
            case(GL_FLOAT):             scale = 1.0f; break;
        }
        switch(destImage->getDataType())
        {
            case(GL_BYTE):              scale *= 128.0f ; break;
            case(GL_UNSIGNED_BYTE):     scale *= 255.0f; break;
            case(GL_SHORT):             scale *= 32768.0f; break;
            case(GL_UNSIGNED_SHORT):    scale *= 65535.0f; break;
            case(GL_INT):               scale *= 2147483648.0f; break;
            case(GL_UNSIGNED_INT):      scale *= 4294967295.0f; break;
            case(GL_FLOAT):             scale *= 1.0f; break;
        }
    }

    if (srcImage->getPixelFormat() == destImage->getPixelFormat())
    {
        //notify(NOTICE)<<"copyImage("<<srcImage<<", "<<src_s<<", "<< src_t<<", "<<src_r<<", "<<width<<", "<<height<<", "<<depth<<std::endl;
        //notify(NOTICE)<<"          "<<destImage<<", "<<dest_s<<", "<< dest_t<<", "<<dest_r<<", "<<doRescale<<")"<<std::endl;

        if (srcImage->getDataType() == destImage->getDataType() && !doRescale)
        {
            //notify(NOTICE)<<"   Compatible pixelFormat and dataType."<<std::endl;
            for(int slice = 0; slice<depth; ++slice)
            {
                for(int row = 0; row<height; ++row)
                {
                    const unsigned char* srcData = srcImage->data(src_s, src_t+row, src_r+slice);
                    unsigned char* destData = destImage->data(dest_s, dest_t+row, dest_r+slice);
                    memcpy(destData, srcData, (width*destImage->getPixelSizeInBits())/8);
                }
            }
            return true;
        }
        else
        {
            //notify(NOTICE)<<"   Compatible pixelFormat and incompatible dataType."<<std::endl;
            for(int slice = 0; slice<depth; ++slice)
            {
                for(int row = 0; row<height; ++row)
                {
                    const unsigned char* srcData = srcImage->data(src_s, src_t+row, src_r+slice);
                    unsigned char* destData = destImage->data(dest_s, dest_t+row, dest_r+slice);
                    unsigned int numComponents = CRCore::crImage::computeNumComponents(destImage->getPixelFormat());
                    
                    _copyRowAndScale(srcData, srcImage->getDataType(), destData, destImage->getDataType(), (width*numComponents), scale);
                }
            }
            
            return true;
        }
    }
    else
    {
        //notify(NOTICE)<<"copyImage("<<srcImage<<", "<<src_s<<", "<< src_t<<", "<<src_r<<", "<<width<<", "<<height<<", "<<depth<<std::endl;
        //notify(NOTICE)<<"          "<<destImage<<", "<<dest_s<<", "<< dest_t<<", "<<dest_r<<", "<<doRescale<<")"<<std::endl;
                
        RecordRowOperator readOp(width);
        WriteRowOperator writeOp;

        for(int slice = 0; slice<depth; ++slice)
        {
            for(int row = 0; row<height; ++row)
            {

                // reset the indices to beginning
                readOp._pos = 0;
                writeOp._pos = 0;
            
                // read the pixels into readOp's _colour array
                readRow(width, srcImage->getPixelFormat(), srcImage->getDataType(), srcImage->data(src_s,src_t+row,src_r+slice), readOp);
                                
                // pass readOp's _colour array contents over to writeOp (note this is just a pointer swap).
                writeOp._colours.swap(readOp._colours);
                
                modifyRow(width, destImage->getPixelFormat(), destImage->getDataType(), destImage->data(dest_s, dest_t+row,dest_r+slice), writeOp);

                // return readOp's _colour array contents back to its rightful owner.
                writeOp._colours.swap(readOp._colours);
            }
        }
        
        return false;
    }

}


struct SetToColourOperator
{
    SetToColourOperator(const CRCore::crVector4& colour):
        _colour(colour) {}

    inline void luminance(float& l) const { l = (_colour.r()+_colour.g()+_colour.b())*0.333333; } 
    inline void alpha(float& a) const { a = _colour.a(); } 
    inline void luminance_alpha(float& l,float& a) const { l = (_colour.r()+_colour.g()+_colour.b())*0.333333; a = _colour.a(); }
    inline void rgb(float& r,float& g,float& b) const { r = _colour.r(); g = _colour.g(); b = _colour.b(); }
    inline void rgba(float& r,float& g,float& b,float& a) const { r = _colour.r(); g = _colour.g(); b = _colour.b(); a = _colour.a(); }

    CRCore::crVector4 _colour;
};

bool clearImageToColor(CRCore::crImage* image, const CRCore::crVector4& colour)
{
    if (!image) return false;

    modifyImage(image, SetToColourOperator(colour));
    
    return true;
}

/** Search through the list of Images and find the maximum number of components used amoung the images.*/
unsigned int maximimNumOfComponents(const ImageList& imageList)
{
    unsigned int max_components = 0;
    for(CRCore::ImageList::const_iterator itr=imageList.begin();
        itr!=imageList.end();
        ++itr)
    {
        const CRCore::crImage* image = itr->get();
        GLenum pixelFormat = image->getPixelFormat();
        if (pixelFormat==GL_ALPHA ||
            pixelFormat==GL_INTENSITY ||
            pixelFormat==GL_LUMINANCE ||
            pixelFormat==GL_LUMINANCE_ALPHA ||
            pixelFormat==GL_RGB ||
            pixelFormat==GL_RGBA ||
            pixelFormat==GL_BGR ||
            pixelFormat==GL_BGRA)
        {
            max_components = maximum(crImage::computeNumComponents(pixelFormat), max_components);
        }
    }
    return max_components;
}

CRCore::crImage* createImage3D(const ImageList& imageList,
                          GLenum desiredPixelFormat,
                          int s_maximumImageSize,
                          int t_maximumImageSize,
                          int r_maximumImageSize,
                          bool resizeToPowerOfTwo)
{
    //notify(INFO)<<"createImage3D(..)"<<std::endl;
    int max_s = 0;
    int max_t = 0;
    int total_r = 0;
    for(CRCore::ImageList::const_iterator itr=imageList.begin();
        itr!=imageList.end();
        ++itr)
    {
        const CRCore::crImage* image = itr->get();
        GLenum pixelFormat = image->getPixelFormat();
        if (pixelFormat==GL_ALPHA ||
            pixelFormat==GL_INTENSITY ||
            pixelFormat==GL_LUMINANCE ||
            pixelFormat==GL_LUMINANCE_ALPHA ||
            pixelFormat==GL_RGB ||
            pixelFormat==GL_RGBA ||
            pixelFormat==GL_BGR ||
            pixelFormat==GL_BGRA)
        {
            max_s = maximum(image->s(), max_s);
            max_t = maximum(image->t(), max_t);
            total_r += image->r();
        }
        //else
        //{
        //    notify(INFO)<<"crImage "<<image->getFileName()<<" has unsuitable pixel format 0x"<< std::hex<< pixelFormat << std::dec << std::endl;
        //}
    }

    //bool remapRGBtoLuminance;
    //bool remapRGBtoRGBA;

    if (desiredPixelFormat==0)
    {
        unsigned int max_components = maximimNumOfComponents(imageList);
        switch(max_components)
        {
        case(1):
            //notify(INFO)<<"desiredPixelFormat = GL_LUMINANCE" << std::endl;
            desiredPixelFormat = GL_LUMINANCE;
            break;
        case(2):
           // notify(INFO)<<"desiredPixelFormat = GL_LUMINANCE_ALPHA" << std::endl;
            desiredPixelFormat = GL_LUMINANCE_ALPHA;
            break;
        case(3):
          //  notify(INFO)<<"desiredPixelFormat = GL_RGB" << std::endl;
            desiredPixelFormat = GL_RGB;
            break;
        case(4):
          //  notify(INFO)<<"desiredPixelFormat = GL_RGBA" << std::endl;
            desiredPixelFormat = GL_RGBA;
            break;
        }
    }
    if (desiredPixelFormat==0) return 0;

    // compute nearest powers of two for each axis.

    int size_s = 1;
    int size_t = 1;
    int size_r = 1;

    if (resizeToPowerOfTwo)
    {
        while(size_s<max_s && size_s<s_maximumImageSize) size_s*=2;
        while(size_t<max_t && size_t<t_maximumImageSize) size_t*=2;
        while(size_r<total_r && size_r<r_maximumImageSize) size_r*=2;
    }
    else
    {
        size_s = max_s;
        size_t = max_t;
        size_r = total_r;
    }

    // now allocate the 3d texture;
    CRCore::ref_ptr<CRCore::crImage> image_3d = new CRCore::crImage;
    image_3d->allocateImage(size_s,size_t,size_r,
                            desiredPixelFormat,GL_UNSIGNED_BYTE);

    unsigned int r_offset = (total_r<size_r) ? (size_r-total_r)/2 : 0;

    int curr_dest_r = r_offset;

    // copy across the values from the source images into the image_3d.
    for(CRCore::ImageList::const_iterator itr=imageList.begin();
        itr!=imageList.end();
        ++itr)
    {
        const CRCore::crImage* image = itr->get();
        GLenum pixelFormat = image->getPixelFormat();
        if (pixelFormat==GL_ALPHA ||
            pixelFormat==GL_LUMINANCE ||
            pixelFormat==GL_INTENSITY ||
            pixelFormat==GL_LUMINANCE_ALPHA ||
            pixelFormat==GL_RGB ||
            pixelFormat==GL_RGBA ||
            pixelFormat==GL_BGR ||
            pixelFormat==GL_BGRA)
        {
            
            int num_s = minimum(image->s(), image_3d->s());
            int num_t = minimum(image->t(), image_3d->t());
            int num_r = minimum(image->r(), (image_3d->r() - curr_dest_r));

            unsigned int s_offset_dest = (image->s()<size_s) ? (size_s - image->s())/2 : 0;
            unsigned int t_offset_dest = (image->t()<size_t) ? (size_t - image->t())/2 : 0;

            copyImage(image, 0, 0, 0, num_s, num_t, num_r,
                      image_3d.get(), s_offset_dest, t_offset_dest, curr_dest_r, false);

            curr_dest_r += num_r;
        }
    }
    
    return image_3d.release();
}

struct ModulateAlphaByLuminanceOperator
{
    ModulateAlphaByLuminanceOperator() {}

    inline void luminance(float&) const {}
    inline void alpha(float&) const {}
    inline void luminance_alpha(float& l,float& a) const { a*= l; }
    inline void rgb(float&,float&,float&) const {}
    inline void rgba(float& r,float& g,float& b,float& a) const { float l = (r+g+b)*0.3333333; a *= l;}
};
 
CRCore::crImage* createImage3DWithAlpha(const ImageList& imageList,
            int s_maximumImageSize,
            int t_maximumImageSize,
            int r_maximumImageSize,
            bool resizeToPowerOfTwo)
{
    GLenum desiredPixelFormat = 0;
    bool modulateAlphaByLuminance = false;

    unsigned int maxNumComponents = maximimNumOfComponents(imageList);
    if (maxNumComponents==3)
    {
        desiredPixelFormat = GL_RGBA;
        modulateAlphaByLuminance = true;
    }
    
    CRCore::ref_ptr<CRCore::crImage> image = createImage3D(imageList,
                                        desiredPixelFormat,
                                        s_maximumImageSize,
                                        t_maximumImageSize,
                                        r_maximumImageSize,
                                        resizeToPowerOfTwo);
    if (image.valid())
    {
        if (modulateAlphaByLuminance)
        {
            modifyImage(image.get(), ModulateAlphaByLuminanceOperator());
        }
        return image.release();
    }
    else
    {
        return 0;
    }
}

    
}

