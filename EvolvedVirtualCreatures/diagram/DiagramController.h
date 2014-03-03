/**
 @filename DiagramController.h
 
	Diagram Edit and Management 
*/
#pragma once

#include "DiagramGlobal.h"


class CEvc;
class RenderComposition;
class CSimpleCamera;
class RenderBezierActor;
namespace evc
{
	class CCreature;
	class CDiagramNode;
	class CPopupDiagrams;
	class COrientationEditController;
	namespace genotype_parser { struct SExpr; struct SConnection; }


	DECLARE_TYPE_NAME_SCOPE(evc, CDiagramController)
	class CDiagramController : public SampleFramework::InputEventListener,
												public memmonitor::Monitor<CDiagramController, TYPE_NAME(CDiagramController)>
	{
		enum MODE { MODE_NONE, MODE_LINK, MODE_ORIENT };

	public:
		CDiagramController(CEvc &sample);
		virtual ~CDiagramController();

		void ControllerSceneInit();
		void SetControlCreature(CCreature *creature);//const genotype_parser::SExpr *expr);
		void Render();
		void Move(float dtime);
		CDiagramNode *GetRootDiagram();
		vector<CDiagramNode*>& GetDiagrams();
		bool GetDiagramsLinkto(CDiagramNode *to, OUT vector<CDiagramNode*> &out);
		bool GetDiagramsLinkfrom(CDiagramNode *from, OUT vector<CDiagramNode*> &out);
		CDiagramNode* CreateDiagramNode(const genotype_parser::SExpr *expr);
		RenderBezierActor* CreateTransition(CDiagramNode *from, CDiagramNode *to, const u_int order=0);

		// InputEvent from CEvc
		virtual void onPointerInputEvent(const SampleFramework::InputEvent&, physx::PxU32, physx::PxU32, physx::PxReal, physx::PxReal, bool val) override;
		virtual void onAnalogInputEvent(const SampleFramework::InputEvent& , float val) override {}
		virtual void onDigitalInputEvent(const SampleFramework::InputEvent& , bool val) override;


	protected:
		CDiagramNode* CreateGenotypeDiagramTree(const PxVec3 &pos, const genotype_parser::SExpr *expr, 
			map<const genotype_parser::SExpr*, CDiagramNode*> &symbols);

		PxVec3 GetDiagramPosition(CDiagramNode *parent, CDiagramNode *dispNode, OUT u_int &order);
		PxVec3 GetDiagramPositionByIndex(const genotype_parser::SExpr *parent_expr, const PxVec3 &parentPos, const u_int index, OUT u_int &order);
		void MoveTransition(RenderBezierActor *transition, CDiagramNode *from, CDiagramNode *to, const u_int order=0);
		void CalcuateTransitionPositions(CDiagramNode *from, CDiagramNode *to, const u_int order, OUT vector<PxVec3> &out);

		void RemoveDiagramTree(CDiagramNode *node, set<CDiagramNode*> &diagrams);
		void Layout(const PxVec3 &pos=PxVec3(0,0,0));
		PxVec3 LayoutRec(CDiagramNode *node, set<CDiagramNode*> &symbols, const PxVec3 &pos=PxVec3(0,0,0));
		void SelectNode(CDiagramNode *node, const bool isShowPopupDiagrams=true);
		CDiagramNode* PickupDiagram(physx::PxU32 x, physx::PxU32 y, const bool isCheckLinkDiagram, const bool isShowHighLight);
		void TransitionAnimation(const float dtime);

		bool InsertDiagram(CDiagramNode *node, CDiagramNode *insertNode);
		bool RemoveDiagram(CDiagramNode *rmNode);
		void RemoveUnlinkDiagram();

		void UpdateCreature();
		void ChangeControllerMode(const MODE scene);

		// Event Handler
		void MouseLButtonDown(physx::PxU32 x, physx::PxU32 y);
		void MouseLButtonUp(physx::PxU32 x, physx::PxU32 y);
		void MouseRButtonDown(physx::PxU32 x, physx::PxU32 y);
		void MouseRButtonUp(physx::PxU32 x, physx::PxU32 y);
		void MouseMove(physx::PxU32 x, physx::PxU32 y);


	private:
		CEvc &m_sample;
		CSimpleCamera *m_camera;
		COrientationEditController *m_OrientationEditController;

		CCreature *m_creature; // reference
		CDiagramNode *m_rootDiagram;
		vector<CDiagramNode*> m_diagrams; // reference
		CDiagramNode *m_selectNode; // reference
		CPopupDiagrams *m_popupDiagrams;

		bool m_leftButtonDown;
		bool m_rightButtonDown;
		MODE m_controlMode;
		PxVec2 m_dragPos[ 2];

		// layout animation to move transition
		bool m_isLayoutAnimation;
		float m_elapsTime;
	};


	inline CDiagramNode *CDiagramController::GetRootDiagram() { return m_rootDiagram; }
	inline vector<CDiagramNode*>& CDiagramController::GetDiagrams() { return m_diagrams; }
}
