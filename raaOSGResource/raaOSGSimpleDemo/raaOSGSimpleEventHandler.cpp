#include "StdAfx.h"

#include <windows.h>

#include <osgViewer/Viewer>
#include <osg/StateSet>

#include "raaOSGPrintVisitor.h"
#include "raaRotationCallback.h"
#include "raaOSGSimpleEventHandler.h"


raaOSGSimpleEventHandler::raaOSGSimpleEventHandler(raaRotationCallback *pRotator)
{
	m_mMode=osg::PolygonMode::FILL;
	m_pRotator = pRotator;
}

raaOSGSimpleEventHandler::~raaOSGSimpleEventHandler(void)
{
}

bool raaOSGSimpleEventHandler::handle( const osgGA::GUIEventAdapter &ea, osgGA::GUIActionAdapter &aa, osg::Object *, osg::NodeVisitor * )
{
	osgViewer::Viewer *pViewer=dynamic_cast<osgViewer::Viewer*>(aa.asView());
	
	if(pViewer && ea.getEventType()==osgGA::GUIEventAdapter::KEYDOWN)
	{	
		switch(ea.getKey())
		{
			case 'i':
			case 'I':
			{
				raaOSGPrintVisitor printer;

				printer.traverse(*(pViewer->getScene()->getSceneData()));
			}
			return true;
			case 'p':
			case 'P':
				pViewer->getSceneData()->getOrCreateStateSet()->setAttributeAndModes(new osg::PolygonMode(osg::PolygonMode::FRONT_AND_BACK, progressMode()), osg::StateAttribute::ON | osg::StateAttribute::OVERRIDE);
				return true;
				return true;
			case 'o':
			case 'O':
				if (m_pRotator) m_pRotator->toggleRotate();
				return true;
		}
	}
	return false;
}

osg::PolygonMode::Mode raaOSGSimpleEventHandler::progressMode()
{
	switch(m_mMode)
	{
		case osg::PolygonMode::POINT:
			return m_mMode=osg::PolygonMode::LINE;
		case osg::PolygonMode::LINE:
			return m_mMode=osg::PolygonMode::FILL;
		default:
			return m_mMode=osg::PolygonMode::POINT;
	}
}
