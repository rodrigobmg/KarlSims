#include "stdafx.h"
#include "SamplePreprocessor.h"
#include "EvolvedVirtualCreatures.h"
#include "SampleUtils.h"
#include "SampleConsole.h"
#include "RendererMemoryMacros.h"
#include "RenderMeshActor.h"

#include "PxPhysics.h"
#include "PxScene.h"
#include "PxRigidDynamic.h"
#include "PxShape.h"
#include "PxPhysicsAPI.h"
#include "RenderBoxActor.h"

#include <SampleBaseInputEventIds.h>
#include <SamplePlatform.h>
#include <SampleUserInput.h>
#include <SampleUserInputIds.h>
#include <SampleUserInputDefines.h>

#include "Creature/Creature.h"
#include "genetic/GeneticAlgorithm.h"
#include "diagram/GenotypeController.h"



using namespace SampleRenderer;
using namespace SampleFramework;

REGISTER_SAMPLE(CEvc, "Evolved Virtual Creatures")

SDbgConfig *g_pDbgConfig = NULL;

///////////////////////////////////////////////////////////////////////////////

CEvc::CEvc(PhysXSampleApplication& app) :
	PhysXSample(app)
,	m_ElapsTime(NULL)
,	m_Age(0)
,	m_Gap(10.f)
,	m_genotypeController(NULL)
,	m_IsApplyCustomGravity(false)
{
	mCreateGroundPlane = true;
	//m_IsApplyCustomGravity = true;
}

CEvc::~CEvc()
{
}

void CEvc::onShutdown()
{
	RemoveAllCreatures();
	evc::CGeneticAlgorithm::Release();
	SAFE_DELETE(m_genotypeController);

	BOOST_FOREACH (auto kv, m_Materials)
		kv.second->release();
	m_Materials.clear();

	CFileLoader::Release();
	PhysXSample::onShutdown();
}


//void CEvc::newMesh(const RAWMesh& data)
//{
//}

//class Console;
//static void gValue(Console* console, const char* text, void* userData)
//{
//	if(!text)
//	{
//		console->out("Usage: value <float>");
//		return;
//	}
//	const float val = (float)::atof(text);
//	printf("value: %f\n", val);
//}
//
//static void gExport(Console* console, const char* text, void* userData)
//{
//	if(!text)
//	{
//		console->out("Usage: export <filename>");
//		return;
//	}
//}
//
//static void gImport(Console* console, const char* text, void* userData)
//{
//	if(!text)
//	{
//		console->out("Usage: import <filename>");
//		return;
//	}
//}



//// A structure for our custom vertex type
//struct CUSTOMVERTEX
//{
//	FLOAT x, y, z, rhw; // The transformed position for the vertex
//	DWORD color;        // The vertex color
//};
//
//LPDIRECT3DVERTEXBUFFER9 g_pVB = NULL; // Buffer to hold vertices
//IDirect3DVertexDeclaration9* g_decl = 0;

void CEvc::onInit()
{
	PhysXSample::onInit();

	CFileLoader::Get()->Init(NULL);

	srand(timeGetTime());
	mApplication.setMouseCursorHiding(true);
	mApplication.setMouseCursorRecentering(true);
	//mCameraController.init(PxVec3(0.0f, 50.0f, 0.0f), PxVec3(1.3f, 0.0f, 0.0f));
	mCameraController.init(PxVec3(0.0f, 3.0f, 10.0f), PxVec3(0.f, 0, 0.0f));
	mCameraController.setMouseSensitivity(0.5f);

	//getPhysics().setParameter();
	//PxSetGroupCollisionFlag(NodeGroup::BODY, NodeGroup::L_ARM, false);
	//PxSetGroupCollisionFlag(NodeGroup::BODY, NodeGroup::R_ARM, false);
	//PxSetGroupCollisionFlag(NodeGroup::HEAD, NodeGroup::L_ARM, false);
	//PxSetGroupCollisionFlag(NodeGroup::HEAD, NodeGroup::R_ARM, false);

	if (!g_pDbgConfig)
	{
		g_pDbgConfig = new SDbgConfig;
		g_pDbgConfig->applyJoint = true;
		g_pDbgConfig->force = 5000.f;
		g_pDbgConfig->value1 = 1;
		g_pDbgConfig->value2 = 1;
		g_pDbgConfig->displaySkinning = true;
		g_pDbgConfig->generationRecursiveCount = 2;
	}

	if (mCreateGroundPlane)
	{
		if (m_Ground)
			m_Planet.push_back(m_Ground);
	}
	else
	{
		PxSceneWriteLock scopedLock(*mScene);
		importRAWFile("planet.raw", 2.0f);
		BOOST_FOREACH (auto actor, mPhysicsActors)
			m_Planet.push_back(actor);
	}

	m_genotypeController = new evc::CGenotypeController(*this);

	//registerInputEvents(m_DiagramController);
	//getPlatform().
	//getPlatform()->getSampleUserInput()->

	
	vector<PxVec3> points;
	points.push_back( PxVec3(0,0,0) );
	points.push_back( PxVec3(0,0,0) );
	points.push_back( PxVec3(4,0,0) );
	points.push_back( PxVec3(4,0,0) );

}


/**
 @brief 
 @date 2014-01-16
*/
void CEvc::customizeRender()
{
	//IDirect3DDevice9 *d3dDevice = (IDirect3DDevice9*)getRenderer()->getDevice();
	//d3dDevice->SetVertexDeclaration(g_decl);
	//d3dDevice->SetStreamSource( 0, g_pVB, 0, sizeof( CUSTOMVERTEX ) );
	//d3dDevice->DrawPrimitive( D3DPT_TRIANGLELIST, 0, 1 );
	if (m_genotypeController)
		m_genotypeController->Render();
}


void	CEvc::onTickPreRender(float dtime)
{
	PhysXSample::onTickPreRender(dtime);

	PxSceneWriteLock scopedLock(*mScene);

	if (m_genotypeController)
		m_genotypeController->Move(dtime);

	BOOST_FOREACH (auto &creature, m_Creatures)
		creature->Move(dtime);
	BOOST_FOREACH (auto &obstacle, m_Obstacles)
		obstacle->Move(dtime);

}

void	CEvc::onTickPostRender(float dtime)
{
	PhysXSample::onTickPostRender(dtime);

}


void CEvc::collectInputEvents(std::vector<const SampleFramework::InputEvent*>& inputEvents)
{
	PhysXSample::collectInputEvents(inputEvents);
	getApplication().getPlatform()->getSampleUserInput()->unregisterInputEvent(CAMERA_SPEED_INCREASE);
	getApplication().getPlatform()->getSampleUserInput()->unregisterInputEvent(CAMERA_SPEED_DECREASE);

	//DIGITAL_INPUT_EVENT_DEF(CAMERA_MOVE_BUTTON, MOUSE_BUTTON_LEFT, XKEY_UNKNOWN, PS3KEY_UNKNOWN, AKEY_UNKNOWN, MOUSE_BUTTON_LEFT, PSP2KEY_UNKNOWN, IKEY_UNKNOWN, MOUSE_BUTTON_LEFT,	WIIUKEY_UNKNOWN);
	//DIGITAL_INPUT_EVENT_DEF(CAMERA_MOVE_BUTTON, MOUSE_BUTTON_RIGHT, XKEY_UNKNOWN, PS3KEY_UNKNOWN, AKEY_UNKNOWN, MOUSE_BUTTON_RIGHT, PSP2KEY_UNKNOWN, IKEY_UNKNOWN, MOUSE_BUTTON_RIGHT,	WIIUKEY_UNKNOWN);

    
	//touch events
	//DIGITAL_INPUT_EVENT_DEF(PICKUP, WKEY_SPACE,			XKEY_1,			PS3KEY_1,		AKEY_UNKNOWN,	OSXKEY_1,		PSP2KEY_UNKNOWN,	IKEY_UNKNOWN,	LINUXKEY_1,			WIIUKEY_UNKNOWN		);
	DIGITAL_INPUT_EVENT_DEF(SPAWN_DEBUG_OBJECT2, WKEY_2,			XKEY_1,			PS3KEY_1,		AKEY_UNKNOWN,	OSXKEY_1,		PSP2KEY_UNKNOWN,	IKEY_UNKNOWN,	LINUXKEY_1,			WIIUKEY_UNKNOWN		);
	DIGITAL_INPUT_EVENT_DEF(SPAWN_DEBUG_OBJECT3, WKEY_3,			XKEY_1,			PS3KEY_1,		AKEY_UNKNOWN,	OSXKEY_1,		PSP2KEY_UNKNOWN,	IKEY_UNKNOWN,	LINUXKEY_1,			WIIUKEY_UNKNOWN		);
	DIGITAL_INPUT_EVENT_DEF(SPAWN_DEBUG_OBJECT4, WKEY_4,			XKEY_1,			PS3KEY_1,		AKEY_UNKNOWN,	OSXKEY_1,		PSP2KEY_UNKNOWN,	IKEY_UNKNOWN,	LINUXKEY_1,			WIIUKEY_UNKNOWN		);
	DIGITAL_INPUT_EVENT_DEF(SPAWN_DEBUG_OBJECT5, WKEY_5,			XKEY_1,			PS3KEY_1,		AKEY_UNKNOWN,	OSXKEY_1,		PSP2KEY_UNKNOWN,	IKEY_UNKNOWN,	LINUXKEY_1,			WIIUKEY_UNKNOWN		);
	DIGITAL_INPUT_EVENT_DEF(SPAWN_DEBUG_OBJECT6, WKEY_6,			XKEY_1,			PS3KEY_1,		AKEY_UNKNOWN,	OSXKEY_1,		PSP2KEY_UNKNOWN,	IKEY_UNKNOWN,	LINUXKEY_1,			WIIUKEY_UNKNOWN		);
	DIGITAL_INPUT_EVENT_DEF(SPAWN_DEBUG_OBJECT7, WKEY_7,			XKEY_1,			PS3KEY_1,		AKEY_UNKNOWN,	OSXKEY_1,		PSP2KEY_UNKNOWN,	IKEY_UNKNOWN,	LINUXKEY_1,			WIIUKEY_UNKNOWN		);
	DIGITAL_INPUT_EVENT_DEF(SPAWN_DEBUG_OBJECT8, WKEY_8,			XKEY_1,			PS3KEY_1,		AKEY_UNKNOWN,	OSXKEY_1,		PSP2KEY_UNKNOWN,	IKEY_UNKNOWN,	LINUXKEY_1,			WIIUKEY_UNKNOWN		);
	DIGITAL_INPUT_EVENT_DEF(SPAWN_DEBUG_OBJECT9, WKEY_9,			XKEY_1,			PS3KEY_1,		AKEY_UNKNOWN,	OSXKEY_1,		PSP2KEY_UNKNOWN,	IKEY_UNKNOWN,	LINUXKEY_1,			WIIUKEY_UNKNOWN		);
	DIGITAL_INPUT_EVENT_DEF(SPAWN_DEBUG_OBJECT0, WKEY_0,			XKEY_1,			PS3KEY_1,		AKEY_UNKNOWN,	OSXKEY_1,		PSP2KEY_UNKNOWN,	IKEY_UNKNOWN,	LINUXKEY_1,			WIIUKEY_UNKNOWN		);
	DIGITAL_INPUT_EVENT_DEF(STEP_ONE_FRAME, WKEY_SPACE,				XKEY_1,			PS3KEY_1,		AKEY_UNKNOWN,	OSXKEY_1,		PSP2KEY_UNKNOWN,	IKEY_UNKNOWN,	LINUXKEY_1,			WIIUKEY_UNKNOWN		);
	//DIGITAL_INPUT_EVENT_DEF(RELEASE_CURSOR, WKEY_BACKSPACE,		XKEY_1,			PS3KEY_1,		AKEY_UNKNOWN,	OSXKEY_1,		PSP2KEY_UNKNOWN,	IKEY_UNKNOWN,	LINUXKEY_1,			WIIUKEY_UNKNOWN		);
	DIGITAL_INPUT_EVENT_DEF(GOTO_NEXT_GENERATION, WKEY_BACKSPACE,		XKEY_1,			PS3KEY_1,		AKEY_UNKNOWN,	OSXKEY_1,		PSP2KEY_UNKNOWN,	IKEY_UNKNOWN,	LINUXKEY_1,			WIIUKEY_UNKNOWN		);
	DIGITAL_INPUT_EVENT_DEF(REMOVE_OBJECT, WKEY_DELETE,		XKEY_1,			PS3KEY_1,		AKEY_UNKNOWN,	OSXKEY_1,		PSP2KEY_UNKNOWN,	IKEY_UNKNOWN,	LINUXKEY_1,			WIIUKEY_UNKNOWN		);
	DIGITAL_INPUT_EVENT_DEF(GOTO_GENOTYPE_CONTROLLER, WKEY_HOME,		XKEY_1,			PS3KEY_1,		AKEY_UNKNOWN,	OSXKEY_1,		PSP2KEY_UNKNOWN,	IKEY_UNKNOWN,	LINUXKEY_1,			WIIUKEY_UNKNOWN		);
	DIGITAL_INPUT_EVENT_DEF(GOTO_ORIENT_EDIT_MODE, WKEY_INSERT,		XKEY_1,			PS3KEY_1,		AKEY_UNKNOWN,	OSXKEY_1,		PSP2KEY_UNKNOWN,	IKEY_UNKNOWN,	LINUXKEY_1,			WIIUKEY_UNKNOWN		);
	DIGITAL_INPUT_EVENT_DEF(SCALE_EDIT_MODE, WKEY_END,		XKEY_1,			PS3KEY_1,		AKEY_UNKNOWN,	OSXKEY_1,		PSP2KEY_UNKNOWN,	IKEY_UNKNOWN,	LINUXKEY_1,			WIIUKEY_UNKNOWN		);
	DIGITAL_INPUT_EVENT_DEF(NEXT_OBJECT, WKEY_TAB,		XKEY_1,			PS3KEY_1,		AKEY_UNKNOWN,	OSXKEY_1,		PSP2KEY_UNKNOWN,	IKEY_UNKNOWN,	LINUXKEY_1,			WIIUKEY_UNKNOWN		);
	

	TOUCH_INPUT_EVENT_DEF(SPAWN_DEBUG_OBJECT,	"Throw Object", ABUTTON_5,	IBUTTON_5);
	TOUCH_INPUT_EVENT_DEF(SPAWN_DEBUG_OBJECT2,	"Throw Object", ABUTTON_5,	IBUTTON_5);
	TOUCH_INPUT_EVENT_DEF(SPAWN_DEBUG_OBJECT3,	"Throw Object", ABUTTON_5,	IBUTTON_5);
	TOUCH_INPUT_EVENT_DEF(SPAWN_DEBUG_OBJECT4,	"Throw Object", ABUTTON_5,	IBUTTON_5);
	TOUCH_INPUT_EVENT_DEF(SPAWN_DEBUG_OBJECT5,	"Throw Object", ABUTTON_5,	IBUTTON_5);
	TOUCH_INPUT_EVENT_DEF(SPAWN_DEBUG_OBJECT6,	"Throw Object", ABUTTON_5,	IBUTTON_5);
	TOUCH_INPUT_EVENT_DEF(SPAWN_DEBUG_OBJECT7,	"Throw Object", ABUTTON_5,	IBUTTON_5);
	TOUCH_INPUT_EVENT_DEF(SPAWN_DEBUG_OBJECT8,	"Throw Object", ABUTTON_5,	IBUTTON_5);
	TOUCH_INPUT_EVENT_DEF(SPAWN_DEBUG_OBJECT9,	"Throw Object", ABUTTON_5,	IBUTTON_5);
	TOUCH_INPUT_EVENT_DEF(SPAWN_DEBUG_OBJECT0,	"Throw Object", ABUTTON_5,	IBUTTON_5);
	TOUCH_INPUT_EVENT_DEF(PICKUP,	"PickUp", ABUTTON_0, ABUTTON_0);
	TOUCH_INPUT_EVENT_DEF(GOTO_NEXT_GENERATION,	"Goto next generation", ABUTTON_0, ABUTTON_0);
	TOUCH_INPUT_EVENT_DEF(REMOVE_OBJECT, "Remove Object", ABUTTON_0, ABUTTON_0);
	TOUCH_INPUT_EVENT_DEF(GOTO_GENOTYPE_CONTROLLER, "Goto GenotypeController", ABUTTON_0, ABUTTON_0);
	TOUCH_INPUT_EVENT_DEF(GOTO_ORIENT_EDIT_MODE, "Goto Orient Edit Mode", ABUTTON_0, ABUTTON_0);
	TOUCH_INPUT_EVENT_DEF(SCALE_EDIT_MODE, "Change Scale EditMode", ABUTTON_0, ABUTTON_0);
	TOUCH_INPUT_EVENT_DEF(NEXT_OBJECT, "Next Object", ABUTTON_0, ABUTTON_0);
	
}


//class PxQueryFilterCallbackOnlyPlanet : public PxQueryFilterCallback
//{
//public:
//	PxRigidActor *m_CheckActor;
//	PxQueryFilterCallbackOnlyPlanet(PxRigidActor *actor) : m_CheckActor(actor) {}
//	virtual PxQueryHitType::Enum preFilter(const PxFilterData& filterData, const PxShape* shape, 
//		const PxRigidActor* actor, PxHitFlags& queryFlags) override
//	{
//		return PxQueryHitType::eNONE;
//	}
//
//	virtual PxQueryHitType::Enum postFilter(const PxFilterData& filterData, const PxQueryHit& hit) override
//	{
//		return PxQueryHitType::eNONE;
//	}
//};


/**
 @brief 
 @date 2013-12-03
*/
void CEvc::spawnNode( const int key )
{
	PxSceneWriteLock scopedLock(*mScene);

	PxVec3 pos = getCamera().getPos() + (getCamera().getViewDir()*10.f);
	const PxVec3 vel = getCamera().getViewDir() * 20.f;

	// find earth position
	{
		PxU32 width, height;
		mApplication.getPlatform()->getWindowSize(width, height);

		PxVec3 rayOrig, rayDir, pickOrig;
		mPicking->computeCameraRay(rayOrig, rayDir, pickOrig, width/2, height/2);

		// raycast rigid bodies in scene
		PxRaycastHit hit; 
		hit.shape = NULL;
		const PxU32 bufferSize = 256;        // [in] size of 'hitBuffer'
		PxRaycastHit hitBuffer[bufferSize];  // [out] User provided buffer for results
		PxRaycastBuffer buf(hitBuffer, bufferSize); // [out] Blocking and touching hits will be stored here
		mScene->raycast(rayOrig, rayDir, PX_MAX_F32, buf);

		for (PxU32 i = 0; i < buf.nbTouches; i++)
		{
			BOOST_FOREACH (auto planet, m_Planet)
			{
				if (buf.touches[ i].actor == planet)
				{
					hit = buf.touches[ i];
					break;
				}
			}
			if (hit.shape)
				break;
		}

		if (!hit.shape)
			return;

		pos = hit.position;
	}


	evc::CCreature *creature = NULL;
	bool IsCreature = true;
	switch (key)
	{
	case SPAWN_DEBUG_OBJECT: 
	case SPAWN_DEBUG_OBJECT2: 
	case SPAWN_DEBUG_OBJECT3: 
	case SPAWN_DEBUG_OBJECT4: 
	case SPAWN_DEBUG_OBJECT5: 
	case SPAWN_DEBUG_OBJECT6: 
		{
			string fileNames[] = {
				string("genotype.txt"), 
				string("genotype_box.txt"), 
				string("genotype_herb.txt"), 
				string("genotype_tree.txt"), 
				string("genotype_flower.txt"), 
				string("genotype_bug.txt")
			};
			const int idx = key - SPAWN_DEBUG_OBJECT;
			if ((sizeof(fileNames)/sizeof(string)) <= idx)
				return;

			if (SPAWN_DEBUG_OBJECT2 == key)
			{
				pos = getCamera().getPos() + (getCamera().getViewDir()*10.f);
			}
			else if (SPAWN_DEBUG_OBJECT6 == key)
			{
				PxVec3 dir = pos;
				dir.normalize();
				//pos += (dir * 5.f);
				pos += PxVec3(0,5,0);
			}

			creature = new evc::CCreature(*this); 
			creature->SetMaxGrowCount(g_pDbgConfig->generationRecursiveCount);
			m_Creatures.push_back( creature );

			if (SPAWN_DEBUG_OBJECT6 == key)
			{
				//pnode->GenerateImmediate(fileNames[ idx], pos, NULL, g_pDbgConfig->generationRecursiveCount, g_pDbgConfig->displaySkinning); 
				creature->GenerateProgressive(fileNames[ idx], pos, NULL, g_pDbgConfig->displaySkinning); 
			}
			else
			{
				creature->GenerateProgressive(fileNames[ idx], pos, ((SPAWN_DEBUG_OBJECT2 == key)? &vel : NULL), g_pDbgConfig->displaySkinning); 
			}

			m_genotypeController->ControllerSceneInit();
			m_genotypeController->SetControlCreature(creature);
		}
		break;

	case SPAWN_DEBUG_OBJECT9: 
		{
			creature = new evc::CCreature(*this); 
			creature->GenerateImmediate("genotype_flower.txt", pos, NULL, g_pDbgConfig->generationRecursiveCount, g_pDbgConfig->displaySkinning); 
			m_Creatures.push_back( creature );
			//PxVec3 initialPos(-50,10,0);
			//for (int i=0; i < 30; ++i)
			//{
			//	pnode = new evc::CCreature(*this); 
			//	pnode->GenerateByGenotype("genotype.txt", initialPos+PxVec3((i%10)*m_Gap, 0, (i/10)*m_Gap));
			//	m_Creatures.push_back( pnode );
			//}
			//IsCreature = false; 
		}
		break;

	//case SPAWN_DEBUG_OBJECT3: pnode->GenerateHuman3(g_pDbgConfig->applyJoint); break;
	//case SPAWN_DEBUG_OBJECT4: pnode->GenerateHuman4(g_pDbgConfig->applyJoint); break;
	//case SPAWN_DEBUG_OBJECT5: pnode->GenerateHuman5(g_pDbgConfig->applyJoint); break;
	//case SPAWN_DEBUG_OBJECT6: pnode->GenerateHuman6(g_pDbgConfig->applyJoint); break;
	//case SPAWN_DEBUG_OBJECT7: pnode->GenerateHuman7(g_pDbgConfig->applyJoint); break;
	//case SPAWN_DEBUG_OBJECT8: pnode->GenerateHuman8(g_pDbgConfig->applyJoint); break;
	//case SPAWN_DEBUG_OBJECT9: pnode->GenerateHuman9(g_pDbgConfig->applyJoint); break;
	//case SPAWN_DEBUG_OBJECT0: pnode->GenerateByGenotype("genotype.txt"); break;
	}

	RET(!creature);
	
}


/**
 @brief 
 @date 2013-12-03
*/
void CEvc::pickup()
{
	PxU32 width;
	PxU32 height;
	mApplication.getPlatform()->getWindowSize(width, height);
	mPicking->moveCursor(width/2,height/2);
	mPicking->lazyPick();
	PxActor *actor = mPicking->letGo();
	//PxRigidDynamic *rigidActor = static_cast<PxRigidDynamic*>(actor->is<PxRigidDynamic>());
	PxRigidDynamic *rigidActor = (PxRigidDynamic*)actor;
	if (rigidActor)
	{
		const PxVec3 pos = getCamera().getPos() + (getCamera().getViewDir()*10.f);
		const PxVec3 vel = getCamera().getViewDir() * 20.f;

		rigidActor->addForce( getCamera().getViewDir()*g_pDbgConfig->force );

		PxU32 nbShapes = rigidActor->getNbShapes();
		if(!nbShapes)
			return;

		PxShape** shapes = (PxShape**)SAMPLE_ALLOC(sizeof(PxShape*)*nbShapes);
		PxU32 nb = rigidActor->getShapes(shapes, nbShapes);
		PX_ASSERT(nb==nbShapes);
		for(PxU32 i=0;i<nbShapes;i++)
		{
			RenderBaseActor *renderActor = getRenderActor(rigidActor, shapes[i]);
			if (renderActor)
			{
				renderActor->setRenderMaterial(mManagedMaterials[ 1]);
			}
		}
		SAMPLE_FREE(shapes);
	}
}


/**
 @brief 
 @date 2013-12-03
*/
void CEvc::onDigitalInputEvent(const SampleFramework::InputEvent &ie, bool val)
{
	if (val)
	{
		switch (ie.m_Id)
		{
		case SPAWN_DEBUG_OBJECT:
		case SPAWN_DEBUG_OBJECT2:
		case SPAWN_DEBUG_OBJECT3:
		case SPAWN_DEBUG_OBJECT4:
		case SPAWN_DEBUG_OBJECT5:
		case SPAWN_DEBUG_OBJECT6:
		case SPAWN_DEBUG_OBJECT7:
		case SPAWN_DEBUG_OBJECT8:
		case SPAWN_DEBUG_OBJECT9:
		case SPAWN_DEBUG_OBJECT0:
			spawnNode(ie.m_Id);
			break;
			
		case PICKUP:
			pickup();
			break;

		case RELEASE_CURSOR:
			{ // nothing~
			}
			break;

		case GOTO_NEXT_GENERATION: gotoNextGenration(); break;

		default:
			PhysXSample::onDigitalInputEvent(ie,val);
			break;
		}
	}

	if (m_genotypeController)
		m_genotypeController->onDigitalInputEvent(ie,val);
}


/**
 @brief pointer input event
 @date 2014-02-12
*/
void CEvc::onPointerInputEvent(const SampleFramework::InputEvent& ie, 
	physx::PxU32 x, physx::PxU32 y, physx::PxReal dx, physx::PxReal dy, bool val)
{
	PhysXSample::onPointerInputEvent(ie, x, y, dx, dy, val);

	if (m_genotypeController)
		m_genotypeController->onPointerInputEvent(ie,x,y,dx,dy,val);
}


/**
 @brief 
 @date 2013-12-03
*/
void CEvc::onSubstepSetup(float dtime, pxtask::BaseTask* cont)
{
	PhysXSample::onSubstepSetup(dtime, cont);

	//PxSceneWriteLock scopedLock(*mScene);
	//BOOST_FOREACH (auto &creature, m_Creatures)
	//	creature->Move(dtime);
	//BOOST_FOREACH (auto &obstacle, m_Obstacles)
	//	obstacle->Move(dtime);

	m_ElapsTime += dtime;
	if (m_ElapsTime > 100) // 1 minutes
	{
		//gotoNextGenration();
		m_ElapsTime = 0;
		++m_Age;
	}
}


PxFilterFlags SampleSubmarineFilterShader(	
	PxFilterObjectAttributes attributes0, PxFilterData filterData0, 
	PxFilterObjectAttributes attributes1, PxFilterData filterData1,
	PxPairFlags& pairFlags, const void* constantBlock, PxU32 constantBlockSize)
{
	// let triggers through
	if(PxFilterObjectIsTrigger(attributes0) || PxFilterObjectIsTrigger(attributes1))
	{
		pairFlags = PxPairFlag::eTRIGGER_DEFAULT;
		return PxFilterFlag::eDEFAULT;
	}
	// generate contacts for all that were not filtered above
	pairFlags = PxPairFlag::eCONTACT_DEFAULT;

	// trigger the contact callback for pairs (A,B) where 
	// the filtermask of A contains the ID of B and vice versa.
	if((filterData0.word0 & filterData1.word1) && (filterData1.word0 & filterData0.word1))
		pairFlags |= PxPairFlag::eNOTIFY_TOUCH_FOUND;

	return PxFilterFlag::eDEFAULT;
}


void CEvc::customizeSceneDesc(PxSceneDesc& sceneDesc)
{
	//sceneDesc.filterShader = SampleSubmarineFilterShader;
	//sceneDesc.simulationEventCallback = this;

	if (m_IsApplyCustomGravity)
		sceneDesc.gravity = PxVec3(0);

	sceneDesc.flags |= PxSceneFlag::eREQUIRE_RW_LOCK;
}


/**
 @brief 
 @date 2013-12-13
*/
void CEvc::gotoNextGenration()
{
	PxSceneWriteLock scopedLock(*mScene);

	evc::CGeneticAlgorithm::Get()->InitGenome();
	BOOST_FOREACH (auto &creature, m_Creatures)
	{
		evc::CGeneticAlgorithm::Get()->AddGenome( creature->GetGenome() );
	}

	evc::CGeneticAlgorithm::Get()->Epoch();

	RemoveAllCreatures();

	const vector<PxRigidActor*> actors = mPhysicsActors;
	BOOST_FOREACH (auto &actor, actors)
		removeActor(actor);

	PxVec3 initialPos(-100,10,0);
	const vector<evc::SGenome> &genomes = evc::CGeneticAlgorithm::Get()->GetGenomes();
	int i = 0;
	BOOST_FOREACH (auto &genome, genomes)
	{
		const float GAP = m_Gap + (m_Age*0.5f);
		evc::CCreature *pCreature = new evc::CCreature(*this);
		pCreature->GenerateByGenome(genome, initialPos + PxVec3((i%10)*GAP,0,(i/10)*GAP));
		m_Creatures.push_back(pCreature);
		++i;
	}
}


/**
 @brief 
 @date 2013-12-13
*/
void CEvc::RemoveAllCreatures()
{
	BOOST_FOREACH(auto &creature, m_Creatures)
		delete creature;
	m_Creatures.clear();

	BOOST_FOREACH(auto &obstacle, m_Obstacles)
		delete obstacle;
	m_Obstacles.clear();
}


/**
 @brief 
 @date 2014-02-10
*/
void CEvc::onSubstep(float dtime)
{
	if (m_IsApplyCustomGravity)
	{
		PxSceneWriteLock scopedLock(*mScene);
		BOOST_FOREACH (auto creature, m_Creatures)
		{
			creature->SetGravity( PxVec3(0,0,0) );
		}
	}
}


/**
 @brief generate material
 @date 2014-02-25
*/
RenderMaterial* CEvc::GetMaterial(const PxVec3 &rgb, bool applyVertexColor) // applyVertexColor=true
{
	int key = (int)(rgb.x*1000000 + rgb.y*10000 + rgb.z*100);
	key = key*10 + applyVertexColor;

	auto it = m_Materials.find(key);
	if (m_Materials.end() != it)
		return it->second;

	const PxReal c = 0.75f;
	const PxReal opacity = 1.0f;
	const bool doubleSided = false;
	const PxU32 id = 0xffffffff;
	RenderMaterial *newMaterial = SAMPLE_NEW2(RenderMaterial)(*getRenderer(), rgb,	opacity, doubleSided, id, NULL, 
		true, false, false, applyVertexColor);

	m_Materials[ key] = newMaterial;
	return newMaterial;
}

