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
#ifndef CRCORE_GLEXTENSIONS_H
#define CRCORE_GLEXTENSIONS_H 1

#include <CRCore/crExport.h>
#include <stdlib.h>
#include <string.h>
#include <string>


namespace CRCore {

/** Return floating-point OpenGL version number.
  * Note: Must only be called within a valid OpenGL context,
  * undefined behavior may occur otherwise.
*/
extern CR_EXPORT float getGLVersionNumber();

/** Return true if "extension" is contained in "extensionString".
*/
extern CR_EXPORT bool isExtensionInExtensionString(const char *extension, const char *extensionString);

/** Return true if OpenGL "extension" is supported.
  * Note: Must only be called within a valid OpenGL context,
  * undefined behavior may occur otherwise.
*/
extern CR_EXPORT bool isGLExtensionSupported(unsigned int contextID, const char *extension);

/** Return the address of the specified OpenGL function.
  * Return NULL if function not supported by OpenGL library.
  * Note, glGLExtensionFuncPtr is declared inline so that the code
  * is compiled locally to the calling code.  This should get by Windows'
  * dumb implementation of having different GL function ptr's for each
  * library when linked to it. 
*/
extern CR_EXPORT void* getGLExtensionFuncPtr(const char *funcName);

/** Set a list of extensions to disable for different OpenGL renderers. This allows
  * OSG applications to work around OpenGL drivers' bugs which are due to problematic extension support.
  * The format of the string is:
  * "GLRendererString : ExtensionName, ExtensionName; GLRenderString2 : ExtensionName;"
  * An example of is : "SUN_XVR1000:GL_EXT_texture_filter_anisotropic"
  * The default setting of GLExtensionDisableString is obtained from the OSG_GL_EXTENSION_DISABLE
  * environmental variable.
*/
extern CR_EXPORT void setGLExtensionDisableString(const std::string& disableString);

/** Get the list of extensions that are disabled for various OpenGL renderers. */
extern CR_EXPORT std::string& getGLExtensionDisableString();

/** Return the address of the specified OpenGL function. If not found then
  * check a second function name, if this fails then return NULL as function is
  * not supported by OpenGL library. This is used for checking something
  * like glActiveTexture (which is in OGL1.3) or glActiveTextureARB.
*/
inline void* getGLExtensionFuncPtr(const char *funcName,const char *fallbackFuncName)
{
    void* ptr = getGLExtensionFuncPtr(funcName);
    if (ptr) return ptr;
    return getGLExtensionFuncPtr(fallbackFuncName);
}


template<typename T>
bool setGLExtensionFuncPtr(T& t, const char* str1)
{
    void* data = CRCore::getGLExtensionFuncPtr(str1);
    if (data)
    {
        memcpy(&t, &data, sizeof(T));
        return false;        
    }
    else
    {
        t = 0;
        return false;        
    }
}

template<typename T>
bool setGLExtensionFuncPtr(T& t, const char* str1, const char* str2)
{
    void* data = CRCore::getGLExtensionFuncPtr(str1,str2);
    if (data)
    {
        memcpy(&t, &data, sizeof(T));
        return false;        
    }
    else
    {
        t = 0;
        return false;        
    }
}


/** Return true if OpenGL "extension" is supported.
  * Note: Must only be called within a valid OpenGL context,
  * undefined behavior may occur otherwise.
*/
extern CR_EXPORT bool isGLUExtensionSupported(unsigned int contextID, const char *extension);


}

#endif
