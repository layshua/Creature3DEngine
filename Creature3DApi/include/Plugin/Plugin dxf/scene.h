/* dxfReader for OpenSceneGraph  Copyright (C) 2005 by GraphArchitecture ( grapharchitecture.com )
 * Programmed by Paul de Repentigny <pdr@grapharchitecture.com>
 * 
 * OpenSceneGraph is (C) 2004 Robert Osfield
 * 
 * This library is provided as-is, without support of any kind.
 *
 * Read DXF docs or OSG docs for any related questions.
 * 
 * You may contact the author if you have suggestions/corrections/enhancements.
 */


/** Simulate the scene with double precision before passing it back to CRCore.
    this permits us to scale down offsets from 0,0,0 with a few matrixtransforms, 
    in case the objects are too far from that center.
    */

#ifndef DXF_SCENE
#define DXF_SCENE 1

#include <CRCore/crMatrixd.h>
#include <CRCore/crGroup.h>
#include <CRCore/crMatrixTransform.h>
#include <CRCore/crGeometry.h>
#include <CRCore/crObject.h>
#include <CRCore/crVector3.h>
#include <CRText/crText.h>
#include <CRUtil/crSmoothingVisitor.h>

class dxfLayerTable;

class bounds {
public:
    bounds() : _min(DBL_MAX, DBL_MAX, DBL_MAX), _max(-DBL_MAX, -DBL_MAX, -DBL_MAX) {}
    inline void expandBy(const CRCore::crVector3d & v) {
        if(v.x()<_min.x()) _min.x() = v.x();
        if(v.x()>_max.x()) _max.x() = v.x();

        if(v.y()<_min.y()) _min.y() = v.y();
        if(v.y()>_max.y()) _max.y() = v.y();

        if(v.z()<_min.z()) _min.z() = v.z();
        if(v.z()>_max.z()) _max.z() = v.z();
    }
    inline void makeMinValid() {
        // we count on _min to offset the whole scene
        // so, we make sure its at 0,0,0 if 
        // bounds are not set (anyway, the scene should be empty,
        // if we need to set any value of _min to 0).
        if (_min.x() == DBL_MAX) _min.x() = 0;
        if (_min.y() == DBL_MAX) _min.y() = 0;
        if (_min.z() == DBL_MAX) _min.z() = 0;
    }
    CRCore::crVector3d _min;
    CRCore::crVector3d _max;
};


static inline 
CRCore::crGeometry* createPtGeometry( CRCore::crPrimitive::Mode pointType, CRCore::Vec3Array* vertices, const CRCore::crVector4 & color)
{
    CRCore::crGeometry* geom = new CRCore::crGeometry;
    geom->setVertexArray(vertices);
    geom->addPrimitive(new CRCore::DrawArrays(pointType, 0, vertices->size())); 
    CRCore::Vec4Array* colors = new CRCore::Vec4Array;
    colors->push_back(color);
    geom->setColorArray(colors);
    geom->setColorBinding(CRCore::crGeometry::BIND_OVERALL);
    CRCore::Vec3Array *norms = new CRCore::Vec3Array;
    norms->push_back(CRCore::crVector3(0,0,1));
    geom->setNormalArray(norms);
    geom->setNormalBinding(CRCore::crGeometry::BIND_OVERALL);
    return geom;
}

static inline 
CRCore::crGeometry* createLnGeometry( CRCore::crPrimitive::Mode lineType, CRCore::Vec3Array* vertices, const CRCore::crVector4 & color)
{
    CRCore::crGeometry* geom = new CRCore::crGeometry;
    geom->setVertexArray(vertices);
    geom->addPrimitive(new CRCore::DrawArrays(lineType, 0, vertices->size())); 
    CRCore::Vec4Array* colors = new CRCore::Vec4Array;
    colors->push_back(color);
    geom->setColorArray(colors);
    geom->setColorBinding(CRCore::crGeometry::BIND_OVERALL);
    CRCore::Vec3Array *norms = new CRCore::Vec3Array;
    norms->push_back(CRCore::crVector3(0,0,1));
    geom->setNormalArray(norms);
    geom->setNormalBinding(CRCore::crGeometry::BIND_OVERALL);
    return geom;
}

static inline 
CRCore::crGeometry* createTricrGeometry( CRCore::Vec3Array* vertices, CRCore::Vec3Array* normals, const CRCore::crVector4 & color)
{
    CRCore::crGeometry* geom = new CRCore::crGeometry;
    geom->setVertexArray(vertices);
    geom->addPrimitive(new CRCore::DrawArrays(CRCore::crPrimitive::PT_TRIANGLES, 0, vertices->size())); 
    CRCore::Vec4Array* colors = new CRCore::Vec4Array;
    colors->push_back(color);
    geom->setColorArray(colors);
    geom->setColorBinding(CRCore::crGeometry::BIND_OVERALL);
    geom->setNormalArray(normals);
    geom->setNormalBinding(CRCore::crGeometry::BIND_PER_PRIMITIVE);
    return geom;
}

static inline 
CRCore::crGeometry* createQuadcrGeometry( CRCore::Vec3Array* vertices, CRCore::Vec3Array* normals, const CRCore::crVector4 & color)
{
    CRCore::crGeometry* geom = new CRCore::crGeometry;
    geom->setVertexArray(vertices);
    geom->addPrimitive(new CRCore::DrawArrays(CRCore::crPrimitive::PT_QUADS, 0, vertices->size())); 
    CRCore::Vec4Array* colors = new CRCore::Vec4Array;
    colors->push_back(color);
    geom->setColorArray(colors);
    geom->setColorBinding(CRCore::crGeometry::BIND_OVERALL);
    geom->setNormalArray(normals);
    geom->setNormalBinding(CRCore::crGeometry::BIND_PER_PRIMITIVE);
    return geom;
}

static inline 
CRCore::crObject* createModel(const std::string & name, CRCore::crDrawable* drawable)
{
    CRCore::crObject* geode = new CRCore::crObject;
    geode->addDrawable(drawable);
    geode->setName(name);
    return geode;
}


static inline CRCore::crVector3d preMultd(const CRCore::crMatrixd& m, const CRCore::crVector3d& v)
{
    double d = 1.0f/(m(3,0)*v.x()+m(3,1)*v.y()+m(3,2)*v.z()+m(3,3)) ;
    return CRCore::crVector3d( (m(0,0)*v.x() + m(1,0)*v.y() + m(2,0)*v.z() + m(3,0))*d,
        (m(0,1)*v.x() + m(1,1)*v.y() + m(2,1)*v.z() + m(3,1))*d,
        (m(0,2)*v.x() + m(1,2)*v.y() + m(2,2)*v.z() + m(3,2))*d) ;
}

static inline CRCore::crVector3d postMultd(const CRCore::crMatrixd& m, const CRCore::crVector3d& v)
{
    double d = 1.0f/(m(3,0)*v.x()+m(3,1)*v.y()+m(3,2)*v.z()+m(3,3)) ;
    return CRCore::crVector3d( (m(0,0)*v.x() + m(0,1)*v.y() + m(0,2)*v.z() + m(0,3))*d,
        (m(1,0)*v.x() + m(1,1)*v.y() + m(1,2)*v.z() + m(1,3))*d,
        (m(2,0)*v.x() + m(2,1)*v.y() + m(2,2)*v.z() + m(2,3))*d) ;
}

typedef std::vector<CRCore::crVector3d> VList;
typedef std::map<unsigned short, VList> MapVList;
typedef std::vector<VList> VListList;
typedef std::map<unsigned short, VListList> MapVListList;


class sceneLayer : public CRCore::Referenced {
public:
    sceneLayer(std::string name) : _name(name) {}
    virtual ~sceneLayer() {}
    void layer2cre(CRCore::crGroup* root, bounds &b)
    {
        crePoints(root, b);
        creLines(root, b);
        creTriangles(root, b);
        creQuads(root, b);
        CRText(root, b);
    }
    MapVListList    _linestrips;
    MapVList        _points;
    MapVList        _lines;
    MapVList        _triangles;
    MapVList        _trinorms;
    MapVList        _quads;
    MapVList        _quadnorms;
    
    struct textInfo
    {
        textInfo(short int color, CRCore::crVector3 point, CRText::crText *text) :
            _color(color), _point(point), _text(text) {};
        short int _color;
        CRCore::crVector3d _point;
        CRCore::ref_ptr<CRText::crText> _text;
    };

    typedef std::vector<textInfo> TextList;    
    TextList _textList;
    
protected:
    std::string        _name;

    CRCore::crVector4        getColor(unsigned short color);

    void crePoints(CRCore::crGroup* root, bounds &b)
    {
       
        for (MapVList::iterator mitr = _points.begin();
            mitr != _points.end(); ++mitr) {
            CRCore::Vec3Array *coords = new CRCore::Vec3Array;
            for (VList::iterator itr = mitr->second.begin();
                itr != mitr->second.end(); ++itr) {
                CRCore::crVector3 v(itr->x() - b._min.x(), itr->y() - b._min.y(), itr->z() - b._min.z());
                coords->push_back(v);
            }
            root->addChild(createModel(_name, createPtGeometry(CRCore::crPrimitive::PT_POINTS, coords, getColor(mitr->first))));
        }
    }

    void creLines(CRCore::crGroup* root, bounds &b)
    {
        for(MapVListList::iterator mlitr = _linestrips.begin();
            mlitr != _linestrips.end();
            ++mlitr)
        {
            for(VListList::iterator itr = mlitr->second.begin();
                itr != mlitr->second.end(); 
                ++itr)
            {
                if (itr->size()) {
                    CRCore::Vec3Array *coords = new CRCore::Vec3Array;
                    for (VList::iterator vitr = itr->begin();
                        vitr != itr->end(); ++vitr) {
                        CRCore::crVector3 v(vitr->x() - b._min.x(), vitr->y() - b._min.y(), vitr->z() - b._min.z());
                        coords->push_back(v);
                    }
                    root->addChild(createModel(_name, createLnGeometry(CRCore::crPrimitive::LINE_STRIP, coords, getColor(mlitr->first))));
                }
            }
        }
        for (MapVList::iterator mitr = _lines.begin();
            mitr != _lines.end(); ++mitr) {
            CRCore::Vec3Array *coords = new CRCore::Vec3Array;
            for (VList::iterator itr = mitr->second.begin();
                itr != mitr->second.end(); ++itr) {
                CRCore::crVector3 v(itr->x() - b._min.x(), itr->y() - b._min.y(), itr->z() - b._min.z());
                coords->push_back(v);
            }
            root->addChild(createModel(_name, createLnGeometry(CRCore::crPrimitive::LINES, coords, getColor(mitr->first))));
        }
    }

    void creTriangles(CRCore::crGroup* root, bounds &b)
    {
        if (_triangles.size()) {
            for (MapVList::iterator mitr = _triangles.begin();
                mitr != _triangles.end(); ++mitr) {
                CRCore::Vec3Array *coords = new CRCore::Vec3Array;
                VList::iterator itr;
                for (itr = mitr->second.begin();
                    itr != mitr->second.end(); ++itr)
                {
                    CRCore::crVector3 v(itr->x() - b._min.x(), itr->y() - b._min.y(), itr->z() - b._min.z());
                    coords->push_back(v);
                }
                CRCore::Vec3Array *norms = new CRCore::Vec3Array;
                VList normlist = _trinorms[mitr->first];
                for (itr = normlist.begin();
                    itr != normlist.end(); ++itr)
                {
                    norms->push_back(CRCore::crVector3(itr->x(), itr->y(), itr->z()));
                }
                root->addChild(createModel(_name, createTricrGeometry(coords, norms, getColor(mitr->first))));
            }
        }
    }
    void creQuads(CRCore::crGroup* root, bounds &b)
    {
        if (_quads.size()) {
            for (MapVList::iterator mitr = _quads.begin();
                mitr != _quads.end(); ++mitr) {
                CRCore::Vec3Array *coords = new CRCore::Vec3Array;
                VList::iterator itr;
                for (itr = mitr->second.begin();
                    itr != mitr->second.end(); ++itr) {
                    CRCore::crVector3 v(itr->x() - b._min.x(), itr->y() - b._min.y(), itr->z() - b._min.z());
                    coords->push_back(v);
                }
                CRCore::Vec3Array *norms = new CRCore::Vec3Array;
                VList normlist = _quadnorms[mitr->first];
                for (itr = normlist.begin();
                    itr != normlist.end(); ++itr) {
                    norms->push_back(CRCore::crVector3(itr->x(), itr->y(), itr->z()));
                }
                root->addChild(createModel(_name, createQuadcrGeometry(coords, norms, getColor(mitr->first))));
            }
        }
    }
    void CRText(CRCore::crGroup* root, bounds &b)
    {
        if (_textList.size()) {
            for (TextList::iterator titr = _textList.begin();
                    titr != _textList.end(); ++titr) {
                titr->_text->setColor(getColor(titr->_color));
                CRCore::crVector3d v1=titr->_point;
                CRCore::crVector3 v2(v1.x() - b._min.x(), v1.y() - b._min.y(), v1.z() - b._min.z());
                titr->_text->setPosition(v2);
                root->addChild(createModel(_name, titr->_text.get()));
            }
        }
    }
};


class scene : public CRCore::Referenced {
public:
    scene(dxfLayerTable* lt = NULL);
    virtual ~scene() {}
    void setLayerTable(dxfLayerTable* lt);
    void pushMatrix(const CRCore::crMatrixd& m, bool protect = false)
    {
        _mStack.push_back(_m);
        if (protect) // equivalent to setMatrix
            _m = m;
        else
            _m = _m * m;
    }
    void popMatrix()
    {
        _mStack.pop_back();
        if (_mStack.size())
            _m = _mStack.back();
        else
            _m.makeIdentity();
    }
    void ocs(const CRCore::crMatrixd& r)
    {
        _r = r;
    }
    void blockOffset(const CRCore::crVector3d& t)
    {
        _t = t;
    }
    void ocs_clear()
    {
        _r.makeIdentity();
    }
    CRCore::crMatrixd& backMatrix() { if (_mStack.size()) return _mStack.back(); else return _m; }

    CRCore::crVector3d addVertex(CRCore::crVector3d v);
    CRCore::crVector3d addNormal(CRCore::crVector3d v);
    sceneLayer* findOrCreateSceneLayer(const std::string & l)
    {
        sceneLayer* ly = _layers[l].get();
        if (!ly) {
            ly = new sceneLayer(l);
            _layers[l] = ly;
        }
        return ly;
    }
    unsigned short correctedColorIndex(const std::string & l, unsigned short color);

    void addPoint(const std::string & l, unsigned short color, CRCore::crVector3d & s);
    void addLine(const std::string & l, unsigned short color, CRCore::crVector3d & s, CRCore::crVector3d & e);
    void addLineStrip(const std::string & l, unsigned short color, std::vector<CRCore::crVector3d> & vertices);
    void addLineLoop(const std::string & l, unsigned short color, std::vector<CRCore::crVector3d> & vertices);
    void addTriangles(const std::string & l, unsigned short color, std::vector<CRCore::crVector3d> & vertices, bool inverted=false);
    void addQuads(const std::string & l, unsigned short color, std::vector<CRCore::crVector3d> & vertices, bool inverted=false);
    void addText(const std::string & l, unsigned short color, CRCore::crVector3d & point, CRText::crText *text);

    CRCore::crGroup* scene2cre()
    {
        CRCore::crGroup* root = NULL;
        CRCore::crGroup* child = NULL;
        _b.makeMinValid();
        CRCore::crVector3 v = CRCore::crVector3(_b._min.x(), _b._min.y(), _b._min.z());
        double x = _b._min.x() - (double)v.x();
        double y = _b._min.y() - (double)v.y();
        double z = _b._min.z() - (double)v.z();
        CRCore::crMatrixd m = CRCore::crMatrixd::translate(v);
        root = new CRCore::crMatrixTransform(m);
        if (x || y || z) {
            m = CRCore::crMatrixd::translate(x,y,z);
            child = new CRCore::crMatrixTransform(m);
            root->addChild(child);
        } else {
            child = root;
        }
//            root = mt;
        for (std::map<std::string, CRCore::ref_ptr<sceneLayer> >::iterator litr = _layers.begin();
            litr != _layers.end(); ++litr) {
            sceneLayer* ly = (*litr).second.get();
            if (!ly) continue;
            CRCore::crGroup* lg = new CRCore::crGroup;
            lg->setName((*litr).first);
            child->addChild(lg);
            ly->layer2cre(lg, _b);
        }
        return root;
    }
protected:
    CRCore::crMatrixd                _m;
    CRCore::crMatrixd                _r;
    CRCore::crVector3d                  _t;
    bounds                      _b;
    std::map<std::string, CRCore::ref_ptr<sceneLayer> >        _layers;
    std::vector<CRCore::crMatrixd>   _mStack;
    dxfLayerTable*              _layerTable;
};

#endif
