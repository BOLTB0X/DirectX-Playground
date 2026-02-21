#include "Pch.h"
#include "ShaderBuffers.h"
#include "ShaderBuffersManager.h"
// Common
#include "ConstantHelper.h"

using namespace PropertyHelper;


ShaderBuffersManager::ShaderBuffersManager()
{
    m_water = std::make_unique<StructContainer<WaterBuffer>>(WaterBuffer());
    m_cloud = std::make_unique<StructContainer<CloudBuffer>>(CloudBuffer((float)ConstantHelper::CloudType::Plane));
    m_sky = std::make_unique<StructContainer<SkyBuffer>>(SkyBuffer());
    m_lensFlare = std::make_unique<StructContainer<LensFlareBuffer>>(LensFlareBuffer());
} // ShaderBuffersManager


ShaderBuffersManager::~ShaderBuffersManager()
{
} // ~ShaderBuffersManager


Property<WaterBuffer> ShaderBuffersManager::GetWaterProperty()
{
    return Property<WaterBuffer>(
        [this]() { return m_water->Get(); },
        [this](const WaterBuffer& v) { m_water->Set(v); }
    );
} // GetWaterProperty


Property<CloudBuffer> ShaderBuffersManager::GetCloudProperty()
{
    return Property<CloudBuffer>(
        [this]() { return m_cloud->Get(); },
        [this](const CloudBuffer& v) { m_cloud->Set(v); }
    );
} // GetCloudProperty


Property<SkyBuffer> ShaderBuffersManager::GetSkyProperty()
{
    return Property<SkyBuffer>(
        [this]() { return m_sky->Get(); },
        [this](const SkyBuffer& v) { m_sky->Set(v); }
    );
} // GetSkyProperty


Property<LensFlareBuffer> ShaderBuffersManager::GetLensFlareProperty()
{
    return Property<LensFlareBuffer>(
        [this]() { return m_lensFlare->Get(); },
        [this](const LensFlareBuffer& v) { m_lensFlare->Set(v); }
    );
} // GetLensFlareProperty