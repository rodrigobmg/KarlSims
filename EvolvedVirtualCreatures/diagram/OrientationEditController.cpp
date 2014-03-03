
#include "stdafx.h"
#include "OrientationEditController.h"
#include "../EvolvedVirtualCreatures.h"
#include "DiagramController.h"
#include "DiagramNode.h"


using namespace evc;

COrientationEditController::COrientationEditController(CEvc &sample, CDiagramController &diagramController) :
	m_sample(sample)
,	m_diagramController(diagramController)
,	m_camera(NULL)
,	m_selectDiagram(NULL)
{

}

COrientationEditController::~COrientationEditController()
{
	SAFE_DELETE(m_camera);

}


/**
 @brief Scene Init
 @date 2014-03-02
*/
void COrientationEditController::ControllerSceneInit()
{
	if (!m_camera)
		m_camera = SAMPLE_NEW(DefaultCameraController)();

	m_sample.getApplication().setMouseCursorHiding(false);
	m_sample.getApplication().setMouseCursorRecentering(false);

	m_camera->init(PxVec3(0.0f, 3.0f, 10.0f), PxVec3(0.f, 0, 0.0f));
	m_camera->setMouseSensitivity(0.5f);

	m_sample.getApplication().setCameraController(m_camera);
	m_sample.setCameraController(m_camera);
}


/**
 @brief setting control diagramnode
 @date 2014-03-02
*/
void COrientationEditController::SetControlDiagram(CDiagramNode *node)
{
	m_selectDiagram = node;

	// setting camera
	vector<CDiagramNode*> nodes;
	m_diagramController.GetDiagramsLinkto(node, nodes);
	if (nodes.empty())
		return;

	PxVec3 dir = nodes[ 0]->m_renderNode->getTransform().p - node->m_renderNode->getTransform().p;
	dir.normalize();
	const PxVec3 camPos = node->m_renderNode->getTransform().p - (dir * 5);

	m_sample.getCamera().lookAt(camPos, dir);
	const PxTransform viewTm = m_sample.getCamera().getViewMatrix();
	m_camera->init(viewTm);
}


/**
 @brief 
 @date 2014-03-02
*/
void COrientationEditController::MouseLButtonDown(physx::PxU32 x, physx::PxU32 y)
{

}


void COrientationEditController::MouseLButtonUp(physx::PxU32 x, physx::PxU32 y)
{

}


void COrientationEditController::MouseRButtonDown(physx::PxU32 x, physx::PxU32 y)
{

}


void COrientationEditController::MouseRButtonUp(physx::PxU32 x, physx::PxU32 y)
{

}


void COrientationEditController::MouseMove(physx::PxU32 x, physx::PxU32 y)
{

}

