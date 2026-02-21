#pragma once
#include "ShaderBuffers.h"
// Framework
#include "StructContainer.h"
// Common
#include "PropertyHelper.h"
// STL
#include <unordered_map>
#include <string>
#include <memory>
#include <any>


class ShaderBuffersManager {
public:
	ShaderBuffersManager();
	ShaderBuffersManager(const ShaderBuffersManager& other) = delete;
	~ShaderBuffersManager();

    template<typename T>
    T& GetBuffer(const std::string& key)
    {
        if (key == ShaderBufferKeys::Water)
            return reinterpret_cast<StructContainer<T>*>(m_water.get())->Get();
        if (key == ShaderBufferKeys::Cloud)
            return reinterpret_cast<StructContainer<T>*>(m_cloud.get())->Get();
        if (key == ShaderBufferKeys::Sky)
            return reinterpret_cast<StructContainer<T>*>(m_sky.get())->Get();
        if (key == ShaderBufferKeys::LensFlare)
            return reinterpret_cast<StructContainer<T>*>(m_lensFlare.get())->Get();

        static T fallback;
        return fallback;
    } // GetBuffer


    template<typename T>
    PropertyHelper::Property<T> DeliveryBuffer(const std::string& key)
    {
        if (key == ShaderBufferKeys::Water)
        {
            return *reinterpret_cast<PropertyHelper::Property<T>*>(&GetWaterProperty());
        }

        if (key == ShaderBufferKeys::Cloud)
        {
            return *reinterpret_cast<PropertyHelper::Property<T>*>(&GetCloudProperty());
        }

        if (key == ShaderBufferKeys::Sky)
        {
            return *reinterpret_cast<PropertyHelper::Property<T>*>(&GetSkyProperty());
        }

        if (key == ShaderBufferKeys::LensFlare)
        {
            return *reinterpret_cast<PropertyHelper::Property<T>*>(&GetLensFlareProperty());
        }

        return PropertyHelper::Property<T>(nullptr, nullptr);
    } // DeliveryBuffer


private:
    PropertyHelper::Property<WaterBuffer> GetWaterProperty();
    PropertyHelper::Property<CloudBuffer> GetCloudProperty();
    PropertyHelper::Property<SkyBuffer> GetSkyProperty();
    PropertyHelper::Property<LensFlareBuffer> GetLensFlareProperty();

private:
    std::unique_ptr<StructContainer<WaterBuffer>> m_water;
    std::unique_ptr<StructContainer<CloudBuffer>> m_cloud;
    std::unique_ptr<StructContainer<SkyBuffer>> m_sky;
    std::unique_ptr<StructContainer<LensFlareBuffer>> m_lensFlare;
}; // ShaderBuffersManager