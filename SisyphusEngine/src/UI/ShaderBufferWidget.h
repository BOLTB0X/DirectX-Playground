#pragma once
#include "IWidget.h"
#include "PropertyHelper.h"
// STL
#include <functional>
#include <string>
// Imgui
#include "imgui.h"


template <typename T>
class ShaderBufferWidget : public IWidget {
public:
    using DrawCallback = std::function<bool(T&)>;

    ShaderBufferWidget(const std::string& name,
        PropertyHelper::Property<T> prop, DrawCallback drawCallback)
        : IWidget(name), m_Property(prop), m_DrawCallback(drawCallback)
    {
    };
    ShaderBufferWidget(const ShaderBufferWidget& other) = delete;
    virtual ~ShaderBufferWidget() = default;
    

    virtual void Frame() override
    {
        if (i_isVisible == false) return;

        if (ImGui::Begin(i_name.c_str(), &i_isVisible))
        {
            T data = m_Property.Get();

            if (m_DrawCallback(data))
            {
                m_Property.Set(data);
            }
        }
        ImGui::End();
    } // Frame

private:
    PropertyHelper::Property<T> m_Property;
    DrawCallback m_DrawCallback;
}; // ShaderBufferWidget