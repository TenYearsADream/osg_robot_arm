#pragma once

#include <osg/NodeCallback>

class raaRotationCallback : public osg::NodeCallback
{
public:
	raaRotationCallback(osg::Vec3f vAxis, float fStep=1.0f, bool bRotate=false);
	virtual ~raaRotationCallback();
	virtual void operator()(osg::Node* pNode, osg::NodeVisitor* pNV);
	void toggleRotate();

protected:
	osg::Matrix m_Matrix;
	bool m_bRotate;
};

