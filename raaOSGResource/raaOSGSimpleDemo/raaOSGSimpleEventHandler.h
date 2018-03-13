#pragma once

#include <osg/PolygonMode>
#include <osgGA/GUIEventHandler>

class raaRotationCallback;

class raaOSGSimpleEventHandler: public osgGA::GUIEventHandler
{
public:
	raaOSGSimpleEventHandler(raaRotationCallback *pRotator);
	virtual ~raaOSGSimpleEventHandler(void);

	virtual bool handle(const osgGA::GUIEventAdapter &ea, osgGA::GUIActionAdapter &aa, osg::Object *, osg::NodeVisitor *);

protected:
	osg::PolygonMode::Mode m_mMode;

	osg::PolygonMode::Mode progressMode();

	raaRotationCallback *m_pRotator;
};

