#include "stdafx.h"

#include <Windows.h>

#include <osg/NodeVisitor>
#include <osg/MatrixTransform>

#include "raaRotationCallback.h"


void raaRotationCallback::operator()(osg::Node* pNode,osg::NodeVisitor* pNV)
{
	if(m_bRotate) if(osg::MatrixTransform *pMT=dynamic_cast<osg::MatrixTransform*>(pNode)) pMT->setMatrix(pMT->getMatrix()*m_Matrix);

	if(pNV) pNV->traverse(*pNode);
}

void raaRotationCallback::toggleRotate()
{
	m_bRotate = !m_bRotate;
}

raaRotationCallback::raaRotationCallback(osg::Vec3f vAxis, float fStep, bool bRotate)
{
	m_Matrix.makeRotate(osg::DegreesToRadians(fStep), vAxis);
	m_bRotate = bRotate;
}


raaRotationCallback::~raaRotationCallback()
{
}
