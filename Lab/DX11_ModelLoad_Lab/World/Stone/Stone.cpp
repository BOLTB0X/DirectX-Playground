// World/Stone/Stone.cpp
#include "Stone.h"
#include "Position/Position.h"
#include "Model/MeshModel.h"
#include "Shader/Shader.h"
#include "EngineHelper.h" 

/* default */
/////////////////////////////////////////////////////////////////////

Stone::Stone()
	: ActorObject()
{
} // Stone

Stone::~Stone()
{
    Shutdown();
} // ~Stone

bool Stone::Init(MeshModel* model, ActorsShader* shader, const std::string& name)
{
    if (ActorObject::Init(model, shader, name) == false)
        return false;
    

    actor_Position->SetPosition(0.0f, 0.0f, 0.0f);

    return true;
} // Init


void Stone::Shutdown()
{
    ActorObject::Shutdown();
} // Shutdown


void Stone::Frame(float frameTime)
{
    ActorObject::Frame(frameTime);
} // Frame


void Stone::Render(ID3D11DeviceContext* context, Camera* camera)
{
    ActorObject::Render(context, camera);
    return;
} // Render

