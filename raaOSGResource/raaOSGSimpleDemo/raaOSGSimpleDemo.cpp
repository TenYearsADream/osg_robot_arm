// raaOSGSimpleDemo.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include <windows.h>

#include <osgDB/ReadFile>
#include <osgViewer/Viewer>
#include <osgGA/TrackballManipulator>
#include <osgGA/FlightManipulator>
#include <osgGA/DriveManipulator>
#include <osgGA/KeySwitchMatrixManipulator>
#include <osgViewer/ViewerEventHandlers>
#include <osg/MatrixTransform>
#include <osg/Group>
#include <osg/Geode>
#include <osg/StateSet>
#include <osg/Geometry> 
#include <osg/ShapeDrawable>
#include <osg/Material>


const static float csg_AmbCoef = 0.1f;
const static float csg_DiffCoef = 0.8f;
const static float csg_SpecCoef = 1.0f;

osg::Group *g_pRoot = 0;

// applies the fixer to loaded models. Uncomment for this to be applied
#define RAA_FIX_MODELS

// since I'm using defines these now control the exercises - uncomment to use, and make sure this is done in numerical order

#ifdef RAA_FIX_MODELS
class raaFixVisitor : osg::NodeVisitor
{
public:
	virtual void apply(osg::Geode& node)
	{
		for (unsigned int i = 0; i < node.getNumDrawables(); node.getDrawable(i++)->setUseDisplayList(false));
		traverse((osg::Node&)node);
	}

	osg::Node* operator()(osg::Node* pNode)
	{
		if (pNode) traverse(*pNode);
		return pNode;
	}

	raaFixVisitor() : osg::NodeVisitor(osg::NodeVisitor::TRAVERSE_ALL_CHILDREN) {}
	virtual ~raaFixVisitor() {}
};
#endif

class raaOSGPrintVisitor : public osg::NodeVisitor
{
public:
	raaOSGPrintVisitor(void) : osg::NodeVisitor(osg::NodeVisitor::TRAVERSE_ALL_CHILDREN)
	{

	}

	virtual ~raaOSGPrintVisitor(void)
	{

	}

	virtual void apply(osg::Node &node)
	{
		for (unsigned int i = 0; i<getNodePath().size(); i++) std::cout << "|--";

		std::cout << node.className() << " Name: " << node.getName() << " Lib: " << node.libraryName() << " Type: " << node.className() << std::endl;

		traverse(node);
	}
};

template<class T>
class raaOSGFindVisitor : public osg::NodeVisitor
{
public:
	raaOSGFindVisitor(std::string sName, osg::Node* pRoot) : osg::NodeVisitor(osg::NodeVisitor::TRAVERSE_ALL_CHILDREN), m_sName(sName), m_pNode(0)
	{
		traverse(*pRoot);
	}

	virtual ~raaOSGFindVisitor()
	{
		if (m_pNode) m_pNode->unref();
	}

	T *node()
	{
		return m_pNode;
	}

	virtual void apply(osg::Node &node)
	{
		if (dynamic_cast<T*>(&node) && node.getName() == m_sName)
		{
			m_pNode = dynamic_cast<T*>(&node);
			m_pNode->ref();
		}
		else
			traverse(node);
	}

protected:
	std::string m_sName;
	T *m_pNode;
};

//class raaRotationCallback : public osg::NodeCallback
//{
//public:
//
//	raaRotationCallback(osg::Vec3f vAxis, float fStep = 1.0f, bool bRotate = false) : m_bRotate(bRotate)
//	{
//		m_Matrix.makeRotate(osg::DegreesToRadians(fStep), vAxis);
//	}
//
//	virtual ~raaRotationCallback()
//	{
//
//	}
//
//	virtual void operator()(osg::Node* pNode, osg::NodeVisitor* pNV)
//	{
//		if (m_bRotate) if (osg::MatrixTransform *pMT = dynamic_cast<osg::MatrixTransform*>(pNode)) pMT->setMatrix(pMT->getMatrix()*m_Matrix);
//
//		if (pNV) pNV->traverse(*pNode);
//	}
//
//	void toggleRotate()
//	{
//		m_bRotate = !m_bRotate;
//	}
//
//protected:
//	osg::Matrix m_Matrix;
//	bool m_bRotate;
//};

//raaRotationCallback* g_pBodyRotatorCallback = 0;
//raaRotationCallback* g_pUpperArmRotatorCallback = 0;

//class raaOSGSimpleEventHandler : public osgGA::GUIEventHandler
//{
//public:
//	const static unsigned int csm_uiNoRotate = 0;
//	const static unsigned int csm_uiRotateLeft = 1;
//	const static unsigned int csm_uiRotateRight = 2;
//
//	raaOSGSimpleEventHandler()
//	{
//
//	}
//
//	virtual ~raaOSGSimpleEventHandler(void)
//	{
//
//	}
//
//	virtual bool handle(const osgGA::GUIEventAdapter &ea, osgGA::GUIActionAdapter &aa, osg::Object *, osg::NodeVisitor *)
//	{
//		osgViewer::Viewer *pViewer = dynamic_cast<osgViewer::Viewer*>(aa.asView());
//
//		if (pViewer && ea.getEventType() == osgGA::GUIEventAdapter::KEYDOWN)
//		{
//			switch (ea.getKey())
//			{
//			case 'i':
//			case 'I':
//			{
//				raaOSGPrintVisitor printer;
//				printer.traverse(*(pViewer->getScene()->getSceneData()));
//			}
//			return true;
//		}
//		return false;
//
//	}
//
//};

class raaRotator : public osg::NodeCallback
{
public:
	const static unsigned int csm_uiNoRotate = 0;
	const static unsigned int csm_uiRotateLeft = 1;
	const static unsigned int csm_uiRotateRight = 2;
	virtual void operator()(osg::Node* node, osg::NodeVisitor* nv)
	{
		if (m_bRotate)
		{
			/* GET MATRIX */
			// Following lines ensure its a matrix transform without crashing everything
			osg::MatrixTransform *pMatrixTransform = dynamic_cast<osg::MatrixTransform*>(node);
			if (pMatrixTransform)
			{
				osg::Matrixf matrix = pMatrixTransform->getMatrix();
				/* MODIFY MATRIX */
				// this order because non commutative (modify matrix with m_RotationStep so m_RotatioStep is on the left
				if (m_uiRotateDirection == csm_uiRotateLeft) matrix = m_RotationStepLeft * matrix;
				if (m_uiRotateDirection == csm_uiRotateRight) matrix = m_RotationStepRight * matrix;
				/* SET MATRIX */
				pMatrixTransform->setMatrix(matrix);
			}
		}
		/* CONTINUE TRAVERSAL */
		nv->traverse(*node); // NOTE - this is essential for every callback or traversal will stop after callback
	}

	raaRotator(float fRotateStep, osg::Vec3f vAxis, bool bRotate = false, unsigned int uiRotateDirection = csm_uiNoRotate) : m_bRotate(bRotate), m_uiRotateDirection(uiRotateDirection)
	{
		m_RotationStepLeft.makeRotate(osg::DegreesToRadians(-fRotateStep), vAxis);
		m_RotationStepRight.makeRotate(osg::DegreesToRadians(fRotateStep), vAxis);
	}

	void setRotate(unsigned int uiRotateDirection)
	{
		m_uiRotateDirection = uiRotateDirection;
		toggleRotate();
	}

	void toggleRotate()
	{
		m_bRotate = m_uiRotateDirection == csm_uiNoRotate ? false : true;
	}

	virtual ~raaRotator()
	{

	}

protected:
	osg::Matrixf m_RotationStepLeft;
	osg::Matrixf m_RotationStepRight;
	bool m_bRotate;
	unsigned int m_uiRotateDirection;
};

// Here because in current code it needs to know about the rotator class but eventhandler needs to be able to
// use them
raaRotator *g_pBodyRotator = 0;
raaRotator *g_pUpperArmRotator = 0;

// custome event handler EventHandler to extend osgGA::GUIEventHandler 
class raaEventHandler : public osgGA::GUIEventHandler
{
public:

	virtual bool handle(const osgGA::GUIEventAdapter &ea, osgGA::GUIActionAdapter &aa)
	{
		if (ea.getEventType() == osgGA::GUIEventAdapter::KEYDOWN)
		{
			switch (ea.getKey())
			{
			case 'p':
			case 'P':
				{
					raaOSGPrintVisitor printer;
					printer.traverse(*g_pRoot);
				}
				return true;
			case 'b':
				if (g_pBodyRotator) g_pBodyRotator->setRotate(raaRotator::csm_uiRotateLeft);
				return true;
			case 'B':
				if (g_pBodyRotator) g_pBodyRotator->setRotate(raaRotator::csm_uiRotateRight);
				return true;
			case 'u':
				if (g_pUpperArmRotator) g_pUpperArmRotator->setRotate(raaRotator::csm_uiRotateLeft);
				return true;
			case 'U':
				if (g_pUpperArmRotator) g_pUpperArmRotator->setRotate(raaRotator::csm_uiRotateRight);
				return true;
			}
		}
		if (ea.getEventType() == osgGA::GUIEventAdapter::KEYUP)
		{
			switch (ea.getKey())
			{
			case 'p':
			case 'P':
			{
				raaOSGPrintVisitor printer;
				printer.traverse(*g_pRoot);
			}
			return true;
			case 'b':
			case 'B':
				if (g_pBodyRotator) g_pBodyRotator->setRotate(raaRotator::csm_uiNoRotate);
				return true;
			case 'u':
			case 'U':
				if (g_pUpperArmRotator) g_pUpperArmRotator->setRotate(raaRotator::csm_uiNoRotate);
				return true;
			}
		}
		return false;
	}

	raaEventHandler()
	{

	}

	virtual ~raaEventHandler()
	{

	}

};


int main(int argc, char* argv[])
{
	osg::ArgumentParser arguments(&argc, argv);

	g_pRoot = new osg::Group();
	g_pRoot->ref();

	// load model
#ifdef RAA_FIX_MODELS
	raaFixVisitor fixer;
	osg::MatrixTransform *pMT = new osg::MatrixTransform();
	g_pRoot->addChild(pMT);
	pMT->addChild(fixer(osgDB::readNodeFiles(arguments)));
#else
	osg::MatrixTransform *pMT = new osg::MatrixTransform();
	g_pRoot->addChild(pMT);
	pMT->addChild(osgDB::readNodeFiles(arguments));
#endif

	//if (!g_pBodyRotatorCallback)
	//{
	//	raaOSGFindVisitor<osg::MatrixTransform> bodyFinder("Body_Rotator", g_pRoot);
	//	if (bodyFinder.node()) bodyFinder.node()->setUpdateCallback(g_pBodyRotatorCallback = new raaRotationCallback(osg::Vec3f(0.0f, 0.0f, 1.0f)));
	//}

	//if (!g_pUpperArmRotatorCallback)
	//{
	//	raaOSGFindVisitor<osg::MatrixTransform> upperArmFinder("UpperArm_Rotator", g_pRoot);
	//	if (upperArmFinder.node()) upperArmFinder.node()->setUpdateCallback(g_pUpperArmRotatorCallback = new raaRotationCallback(osg::Vec3f(1.0f, 0.0f, 0.0f)));
	//}

	// Look up in the code for the implementation
	raaOSGFindVisitor<osg::MatrixTransform> bodyFinder("Body_Rotator", g_pRoot);
	if (bodyFinder.node()) // If it found anything
	{
		bodyFinder.node()->setUpdateCallback(g_pBodyRotator = new raaRotator(1.0f, osg::Vec3f(0.0f, 0.0f, 1.0f)));
	}

	// Look up in the code for the implementation (CURRENTLY ROTATING AROUND SAME AXIS) (locator because bad naming)
	raaOSGFindVisitor<osg::MatrixTransform> upperArmFinder("UpperArm_Locator", g_pRoot);
	if (upperArmFinder.node()) // If it found anything
	{
		upperArmFinder.node()->setUpdateCallback(g_pUpperArmRotator = new raaRotator(2.0f, osg::Vec3f(1.0f, 0.0f, 0.0f)));
	}
	
	// setup viewer
	osgViewer::Viewer viewer;

	// define graphics context
	osg::GraphicsContext::Traits *pTraits = new osg::GraphicsContext::Traits();
	pTraits->x = 20;
	pTraits->y = 20;
	pTraits->width = 600;
	pTraits->height = 480;
	pTraits->windowDecoration = true;
	pTraits->doubleBuffer = true;
	pTraits->sharedContext = 0;
	osg::GraphicsContext *pGC = osg::GraphicsContext::createGraphicsContext(pTraits);

	osgGA::KeySwitchMatrixManipulator* pKeyswitchManipulator = new osgGA::KeySwitchMatrixManipulator();
	pKeyswitchManipulator->addMatrixManipulator('1', "Trackball", new osgGA::TrackballManipulator());
	pKeyswitchManipulator->addMatrixManipulator('2', "Flight", new osgGA::FlightManipulator());
	pKeyswitchManipulator->addMatrixManipulator('3', "Drive", new osgGA::DriveManipulator());
	viewer.setCameraManipulator(pKeyswitchManipulator);

	osg::Camera *pCamera = viewer.getCamera();

	pCamera->setGraphicsContext(pGC);
	pCamera->setViewport(new osg::Viewport(0, 0, pTraits->width, pTraits->height));

	// add custom handler -> press 'i' for info, 'p' for rendering modes, ('o' for rotation control, exercise 5)
	// see implmentation for how this works

	// custom event handler
	viewer.addEventHandler(new raaEventHandler);

	//viewer.addEventHandler(new raaOSGSimpleEventHandler);

	// add the thread model handler -> press 'm'
	viewer.addEventHandler(new osgViewer::ThreadingHandler);

	// add the window size toggle handler -> press 'f'
	viewer.addEventHandler(new osgViewer::WindowSizeHandler);

	// add the stats handler -> press 's'
	viewer.addEventHandler(new osgViewer::StatsHandler);

	// add the record camera path handler
	viewer.addEventHandler(new osgViewer::RecordCameraPathHandler);

	// add the LOD Scale handler
	viewer.addEventHandler(new osgViewer::LODScaleHandler);

	// add the screen capture handler -> press 'c'. look for image file in working dir($(osg)\bin)
	viewer.addEventHandler(new osgViewer::ScreenCaptureHandler);

	// add the help handler -> press 'h'
	viewer.addEventHandler(new osgViewer::HelpHandler);

	// set the scene to render
	viewer.setSceneData(g_pRoot);

	viewer.realize();

	return viewer.run();
}
