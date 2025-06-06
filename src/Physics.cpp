#include "Physics.hpp"
#include <iostream>
#include "RigidbodyComponent.hpp"

#include "Logger.hpp"


struct UserErrorCallback final : physx::PxErrorCallback
{
     void reportError(physx::PxErrorCode::Enum code, const char* message, const char* file, const int line) override
     {
         std::cout << file << " line " << line << ": " << message << "\n" << "\n";
     }
} gErrorCallback;

void physics::PhysicsController::init()
{
    m_foundation = PxCreateFoundation(PX_PHYSICS_VERSION, m_defaultAllocator, gErrorCallback);
    m_pvd = PxCreatePvd(*m_foundation);

    physx::PxTolerancesScale scale;
    m_physics = PxCreatePhysics(PX_PHYSICS_VERSION, *m_foundation, scale, true, m_pvd);

    physx::PxSceneDesc sceneDesc(m_physics->getTolerancesScale());
    sceneDesc.gravity = physx::PxVec3(0.0f, -9.81f, 0.0f);
    sceneDesc.cpuDispatcher = physx::PxDefaultCpuDispatcherCreate(2);
    sceneDesc.filterShader = physx::PxDefaultSimulationFilterShader;
    m_scene = m_physics->createScene(sceneDesc);

    m_defaultMaterial = m_physics->createMaterial(0.5f, 0.5f, 0.6f);

    m_controllerManager = PxCreateControllerManager(*m_scene);
}

physics::PhysicsController& physics::PhysicsController::instance()
{
    static physics::PhysicsController instance;
    return instance;
}

void physics::PhysicsController::simulate(float deltaTime)
{
    if (!m_scene)
        return;

    m_scene->simulate(deltaTime);
    m_scene->fetchResults(true);
}

void physics::PhysicsController::release()
{
    if (m_scene)
        m_scene->release();
    if (m_physics)
        m_physics->release();
    if (m_foundation)
        m_foundation->release();
}

physx::PxControllerManager* physics::PhysicsController::getControllerManager() const
{
    return m_controllerManager;
}

physx::PxMaterial* physics::PhysicsController::getDefaultMaterial() const
{
    return m_defaultMaterial;
}

physx::PxScene* physics::PhysicsController::getScene() const
{
    return m_scene;
}

physx::PxRigidDynamic* physics::PhysicsController::addDynamicActor(std::shared_ptr<GameObject> actor) const
{
    if (!m_physics)
    {
        LOG_ERROR("Physics is not initialized");
        return nullptr;
    }

    if (!m_scene)
    {
        LOG_ERROR("Scene is not initialized");
        return nullptr;
    }

    const physx::PxTransform transform(physx::PxVec3(actor->getPosition().x, actor->getPosition().y, actor->getPosition().z));
    physx::PxRigidDynamic* rigidBody = m_physics->createRigidDynamic(transform);

    if (!rigidBody)
    {
        LOG_ERROR("Failed to create rigid body");
        return nullptr;
    }

    const physx::PxMaterial* material = getDefaultMaterial();

    if (!material)
    {
        LOG_ERROR("Default material missing");
        return nullptr;
    }

    physx::PxShape* shape = m_physics->createShape(physx::PxBoxGeometry(actor->getScale().x * 0.5f, actor->getScale().y * 0.5f, actor->getScale().z * 0.5f), *material);

    if (!shape)
    {
        LOG_ERROR("Failed to create collision shape");
        rigidBody->release();
        return nullptr;
    }

    shape->setFlag(physx::PxShapeFlag::eSCENE_QUERY_SHAPE, true);
    shape->setFlag(physx::PxShapeFlag::eSIMULATION_SHAPE, true);
    physx::PxFilterData filterData;
    filterData.word0 = actor->getLayerMask();
    shape->setSimulationFilterData(filterData);
    shape->setQueryFilterData(filterData);

    if (!rigidBody->attachShape(*shape))
    {
        LOG_ERROR("Failed to attach shape");
        shape->release();
        rigidBody->release();
        return nullptr;
    }

    rigidBody->userData = actor.get();

    physx::PxRigidBodyExt::updateMassAndInertia(*rigidBody, 10.0f);

    m_scene->addActor(*rigidBody);

    return rigidBody;
}

physx::PxRigidStatic * physics::PhysicsController::addStaticActor(std::shared_ptr<GameObject> actor)
{
    if (!m_physics)
    {
        std::cerr << "physics::PhysicsController::addDynamicActor(): physics is not initialized" << std::endl;
        return nullptr;
    }

    if (!m_scene)
    {
        std::cerr << "physics::PhysicsController::addDynamicActor(): scene is not initialized" << std::endl;
        return nullptr;
    }

    const physx::PxMaterial* material = getDefaultMaterial();

    if (!material)
    {
        LOG_ERROR("Default material missing");
        return nullptr;
    }

    const physx::PxTransform transform(physx::PxVec3(actor->getPosition().x, actor->getPosition().y, actor->getPosition().z));
    physx::PxRigidStatic* staticBody = m_physics->createRigidStatic(transform);

    if (!staticBody)
    {
        LOG_ERROR("Failed to create static body");
        return nullptr;
    }

    physx::PxShape* shape = m_physics->createShape(physx::PxBoxGeometry(actor->getScale().x * 0.5f, actor->getScale().y * 0.5f, actor->getScale().z * 0.5f),  *material);

    if (!shape)
    {
        LOG_ERROR("Failed to create collision shape");
        staticBody->release();
        return nullptr;
    }

    shape->setFlag(physx::PxShapeFlag::eSCENE_QUERY_SHAPE, true); // Enable raycasting
    shape->setFlag(physx::PxShapeFlag::eSIMULATION_SHAPE, true);   // Optional if you want physics
    physx::PxFilterData filterData;
    filterData.word0 = actor->getLayerMask();
    shape->setSimulationFilterData(filterData);
    shape->setQueryFilterData(filterData);

    if (!staticBody->attachShape(*shape))
    {
        LOG_ERROR("Failed to attach shape");
        shape->release();
        staticBody->release();
        return nullptr;
    }

    staticBody->userData = actor.get();

    m_scene->addActor(*staticBody);

    return staticBody;
}

void physics::PhysicsController::resizeCollider(const glm::vec3 &newSize, std::shared_ptr<GameObject> collider)
{
    //TODO: Check if actor is in the scene

    auto rigidBody = collider->getComponent<RigidbodyComponent>();

    if (!rigidBody)
        return;

    auto actor = rigidBody->getRigidActor();

    if (!actor)
        return;

    physx::PxShape* shapes[8];
    uint32_t shapeCount = actor->getNbShapes();
    actor->getShapes(shapes, shapeCount);

    for (uint32_t i = 0; i < shapeCount; ++i)
    {
        actor->detachShape(*shapes[i]);
        shapes[i]->release();
    }

    physx::PxBoxGeometry newBox(physx::PxVec3(newSize.x, newSize.y, newSize.z));
    physx::PxMaterial* material = getDefaultMaterial();
    physx::PxShape* newShape = physx::PxRigidActorExt::createExclusiveShape(*actor, newBox, *material);

    newShape->setLocalPose(physx::PxTransform(physx::PxVec3(0, 0, 0)));

    physx::PxFilterData filterData;
    filterData.word0 = collider->getLayerMask();
    newShape->setSimulationFilterData(filterData);
    newShape->setQueryFilterData(filterData);
}

// physx::PxFilterData filterData;
// filterData.word0 = actor->getLayerMask();     // The layer this object belongs to
// filterData.word1 = common::LayerMask::DEFAULT | common::LayerMask::GROUND;  // What this object can collide/interact with
//
// shape->setSimulationFilterData(filterData);
// shape->setQueryFilterData(filterData);

