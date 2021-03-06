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
#include <CRCore/crArray.h>

using namespace CRCore;

static char* s_ArrayNames[] =
{
    "Array", // 0
    "ByteArray",     // 1
    "ShortArray",    // 2
    "IntArray",      // 3

    "UByteArray",    // 4
    "UShortArray",   // 5
    "UIntArray",     // 6
    "Vec4ubArray",   // 7

    "FloatArray",    // 8
    "Vec2Array",     // 9
    "Vec3Array",     // 10
    "Vec4Array",      // 11

	"Vec2sArray",    // 12
	"Vec3sArray",    // 13
	"Vec4sArray",    // 14

	"Vec2bArray",    // 15
	"Vec3bArray",    // 16
	"Vec4bArray",    // 17
};

const char* Array::className() const
{
    if (m_arrayType>=ArrayType && m_arrayType<=Vec4bArrayType)
        return s_ArrayNames[m_arrayType];
    else
        return "UnkownArray";
}

