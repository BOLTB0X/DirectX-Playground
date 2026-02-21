#pragma once
// Imgui
#include "imgui.h"
// Rendering
#include "Shader/ShaderBuffers.h"


namespace ShaderBufferImGuiDrawer {

    // 수치 조절 시 팁을 보여주는 헬퍼 함수
    inline void HelpMarker(const char* desc)
    {
        ImGui::TextDisabled("(?)");
        if (ImGui::IsItemHovered()) {
            ImGui::BeginTooltip();
            ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
            ImGui::TextUnformatted(desc);
            ImGui::PopTextWrapPos();
            ImGui::EndTooltip();
        }
    } // HelpMarker


    inline bool DrawWater(WaterBuffer& data)
    {
        bool changed = false;

        // 색상 및 기본 외형
        if (ImGui::CollapsingHeader("Appearance", ImGuiTreeNodeFlags_DefaultOpen))
        {
            if (ImGui::ColorEdit3("Base Color", &data.waterBaseColor.x))
                changed = true;

            if (ImGui::SliderFloat("Water Alpha", &data.waterAlpha, 0.0f, 1.0f, "%.2f"))
                changed = true;
            ImGui::SameLine(); HelpMarker("It is the transparency of the water itself.");

            if (ImGui::SliderFloat("Final Alpha", &data.finalAlpha, 0.0f, 1.0f, "%.2f"))
                changed = true;
            ImGui::SameLine(); HelpMarker("The overall transparency that is multiplied into the final blended result");
        }

        // 파도 설정
        if (ImGui::CollapsingHeader("Waves & Physics", ImGuiTreeNodeFlags_DefaultOpen))
        {
            if (ImGui::DragFloat("Translation", &data.waterTranslation, 0.01f))
                changed = true;
            if (ImGui::SliderFloat("Wave Length", &data.waveLength, 0.0f, 5.0f, "%.2f"))
                changed = true;
            if (ImGui::SliderFloat("Refract Scale", &data.reflectRefractScale, 0.0f, 0.2f, "%.3f"))
                changed = true;
        }

        // 바람 설정
        if (ImGui::CollapsingHeader("Environment (Wind)"))
        {
            if (ImGui::SliderFloat2("Wind Direction", &data.windDirection.x, -1.0f, 1.0f))
                changed = true;
            if (ImGui::DragFloat("Wind Force", &data.windForce, 0.05f, 0.0f, 10.0f))
                changed = true;
        }

        // 스펙큘러 및 빛 반사 상세 제어
        if (ImGui::CollapsingHeader("Lighting & Specular"))
        {
            ImGui::BulletText("Specular Settings");
            if (ImGui::DragFloat("Shininess", &data.specularShininess, 1.0f, 1.0f, 500.0f)) changed = true;
            if (ImGui::SliderFloat("Highlights Size", &data.highlightsSize, 0.0f, 1.0f)) changed = true;

            ImGui::Separator();
            ImGui::BulletText("Sun Column");
            if (ImGui::SliderFloat("Column Width", &data.sunColumnWidth, 0.0f, 100.0f)) changed = true;
            if (ImGui::SliderFloat("Column Intensity", &data.sunColumnInensity, 0.0f, 5.0f)) changed = true;

            ImGui::Separator();
            ImGui::BulletText("Effects");
            if (ImGui::SliderFloat("Sparkle Intensity", &data.sparkleIntensity, 0.0f, 1.0f)) changed = true;
        }

        ImGui::Spacing();
        ImGui::Separator();

        // 초기화 기능
        ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(0.0f, 0.6f, 0.6f));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::HSV(0.0f, 0.7f, 0.7f));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor::HSV(0.0f, 0.8f, 0.8f));

        if (ImGui::Button("Reset to Default", ImVec2(ImGui::GetContentRegionAvail().x, 0)))
        {
            data = WaterBuffer();
            changed = true;
        }

        ImGui::PopStyleColor(3);

        return changed;
    } // DrawWater

    inline bool DrawCloud(CloudBuffer& data)
    {
        bool changed = false;

        // 기본 설정 및 레이마칭 최적화
        if (ImGui::CollapsingHeader("Volume Optimization", ImGuiTreeNodeFlags_DefaultOpen))
        {
            if (ImGui::SliderFloat("Max Steps", &data.maxSteps, 10.0f, 300.0f, "%.0f")) changed = true;
            ImGui::SameLine(); HelpMarker("Number of samples along the ray. Higher is better quality but slower.");

            if (ImGui::SliderFloat("March Size", &data.marchSize, 0.01f, 0.5f, "%.3f")) changed = true;
            ImGui::SameLine(); HelpMarker("Step size for each sample. Smaller is more detailed.");

            if (ImGui::DragFloat("Max Depth", &data.maxDepth, 1.0f, 10.0f, 1000.0f)) changed = true;
        }

        // 구름의 형태 및 영역
        if (ImGui::CollapsingHeader("Shape & Transform", ImGuiTreeNodeFlags_DefaultOpen))
        {
            if (ImGui::DragFloat("Radius", &data.radius, 0.1f, 0.0f, 50.0f)) changed = true;
            if (ImGui::DragFloat("Height", &data.height, 0.1f, -10.0f, 50.0f)) changed = true;
            if (ImGui::DragFloat("Thickness", &data.thickness, 0.1f, 0.1f, 20.0f)) changed = true;

            ImGui::Separator();
            if (ImGui::SliderFloat("Density Scale", &data.densityScale, 0.0f, 5.0f)) changed = true;
            if (ImGui::SliderFloat("Falloff Scale", &data.falloffScale, 0.0f, 1.0f)) changed = true;
        }

        // 조명 및 색상
        if (ImGui::CollapsingHeader("Lighting & Colors"))
        {
            if (ImGui::ColorEdit3("Base Color", &data.baseColor.x)) changed = true;
            if (ImGui::ColorEdit3("Ambient Color", &data.ambient.x)) changed = true;
            if (ImGui::ColorEdit3("Shadow Color", &data.shadowColor.x)) changed = true;

            ImGui::Separator();
            ImGui::Text("Scattering (Mie)");
            if (ImGui::SliderFloat("Mie Intensity", &data.mieIntensity, 0.0f, 10.0f)) changed = true;
            if (ImGui::SliderFloat("Mie Power", &data.miePower, 1.0f, 50.0f)) changed = true;

            ImGui::Separator();
            ImGui::Text("Shadow & Shading");
            if (ImGui::SliderFloat("Diffuse Power", &data.diffusePower, 0.0f, 5.0f)) changed = true;
            if (ImGui::SliderFloat("Light Multiply", &data.lightMultiply, 0.0f, 10.0f)) changed = true;
            if (ImGui::SliderFloat("Shadow Distance", &data.shadowDist, 0.0f, 2.0f)) changed = true;
        }

        // 노이즈 및 FBM
        if (ImGui::CollapsingHeader("Noise Detail (FBM)"))
        {
            if (ImGui::DragFloat("Noise Resolution", &data.noiseRes, 1.0f, 1.0f, 512.0f)) changed = true;
            if (ImGui::SliderInt("FBM Octaves", &data.fbmOctaves, 1, 12)) changed = true;
            if (ImGui::SliderFloat("FBM Scale", &data.fbmScale, 0.0f, 2.0f)) changed = true;
            if (ImGui::SliderFloat("FBM Factor", &data.fbmFactor, 0.0f, 5.0f)) changed = true;
            if (ImGui::SliderFloat("FBM Increment", &data.fbmIncrement, 0.0f, 1.0f)) changed = true;
            if (ImGui::SliderFloat("FBM Persistence", &data.fbmPersistance, 0.0f, 1.0f)) changed = true;
        }

        // Wind & Speed
        if (ImGui::CollapsingHeader("Atmosphere Animation"))
        {
            if (ImGui::SliderFloat3("Wind Direction", &data.windDir.x, -1.0f, 1.0f)) changed = true;
            if (ImGui::DragFloat("Cloud Speed", &data.cloudSpeed, 0.01f, 0.0f, 10.0f)) changed = true;
        }

        ImGui::Spacing();
        ImGui::Separator();

        // 초기화 버튼
        ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(0.0f, 0.6f, 0.6f));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::HSV(0.0f, 0.7f, 0.7f));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor::HSV(0.0f, 0.8f, 0.8f));

        if (ImGui::Button("Reset Cloud to Default", ImVec2(ImGui::GetContentRegionAvail().x, 0)))
        {
            data = CloudBuffer();
            changed = true;
        }
        ImGui::PopStyleColor(3);

        return changed;
    } // DrawCloud


    inline bool DrawSky(SkyBuffer& data)
    {
        bool changed = false;

        // 하늘 색상
        if (ImGui::CollapsingHeader("Atmosphere Colors", ImGuiTreeNodeFlags_DefaultOpen))
        {
            if (ImGui::ColorEdit3("Top Color", &data.topColor.x)) changed = true;
            if (ImGui::ColorEdit3("Horizon Color", &data.horizonColor.x)) changed = true;
            if (ImGui::ColorEdit3("Lower Color", &data.lowerColor.x)) changed = true;
            if (ImGui::ColorEdit3("Atmosphere Color", &data.atmosphereColor.x)) changed = true;

            ImGui::Separator();
            if (ImGui::SliderFloat("Sky Exponent", &data.skyExponent, 0.0f, 2.0f)) changed = true;
            ImGui::SameLine(); HelpMarker("Controls the gradient falloff between top and horizon.");
        }

        // 태양 및 글로우
        if (ImGui::CollapsingHeader("Sun & Bloom Settings", ImGuiTreeNodeFlags_DefaultOpen))
        {
            if (ImGui::DragFloat("Sun Size", &data.sunSize, 0.0001f, 0.0f, 0.1f, "%.4f")) changed = true;
            if (ImGui::SliderFloat("Sun Intensity", &data.sunIntensity, 0.0f, 5.0f)) changed = true;
            if (ImGui::SliderFloat("Sun Bloom", &data.sunBloom, 0.0f, 200.0f)) changed = true;

            ImGui::Separator();
            if (ImGui::SliderFloat("Bloom Multiply", &data.bloomMult, 0.0f, 10.0f)) changed = true;
            if (ImGui::SliderFloat("Glow Multiply", &data.glowMult, 0.0f, 10.0f)) changed = true;
            if (ImGui::SliderFloat("Wide Glow Scale", &data.wideGlowScale, 0.0f, 50.0f)) changed = true;
            if (ImGui::SliderFloat("Sun Dist Scale", &data.sunDistScale, 0.0f, 10.0f)) changed = true;
        }

        // 하늘 빛줄기
        if (ImGui::CollapsingHeader("God Rays / Sky Animation"))
        {
            if (ImGui::SliderFloat("Ray Frequency", &data.rayFreq, 0.0f, 20.0f)) changed = true;
            if (ImGui::SliderFloat("Ray Time Scale", &data.rayTimeScale, 0.0f, 2.0f)) changed = true;
        }

        ImGui::Spacing();
        ImGui::Separator();

        ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(0.0f, 0.6f, 0.6f));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::HSV(0.0f, 0.7f, 0.7f));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor::HSV(0.0f, 0.8f, 0.8f));
        if (ImGui::Button("Reset Sky to Default", ImVec2(ImGui::GetContentRegionAvail().x, 0)))
        {
            data = SkyBuffer();
            changed = true;
        }

        ImGui::PopStyleColor(3);

        return changed;
    } // DrawSky


    inline bool DrawLensFlare(LensFlareBuffer& data)
    {
        bool changed = false;

        // 기본 글로벌 설정
        if (ImGui::CollapsingHeader("Global Lens Settings", ImGuiTreeNodeFlags_DefaultOpen))
        {
            if (ImGui::SliderFloat("Master Alpha", &data.alpha, 0.0f, 1.0f)) changed = true;
            if (ImGui::SliderInt("Ghost Count", &data.count, 0, 16)) changed = true;
            if (ImGui::SliderFloat("Ghost Spacing", &data.spacing, 0.0f, 1.0f)) changed = true;
            if (ImGui::SliderFloat("Threshold", &data.threshold, 0.0f, 1.0f)) changed = true;
            ImGui::SameLine(); HelpMarker("Luminance threshold to trigger flare.");
        }

        // 태양 코어 및 광성
        if (ImGui::CollapsingHeader("Sun Core & Stars", ImGuiTreeNodeFlags_DefaultOpen))
        {
            if (ImGui::SliderFloat("Glow Size", &data.glowSize, 0.0f, 5.0f)) changed = true;
            if (ImGui::SliderFloat("Star Scale", &data.starScale, 0.0f, 5.0f)) changed = true;
            if (ImGui::SliderFloat("Core Tightness", &data.sunCoreTightness, 1.0f, 128.0f)) changed = true;
            if (ImGui::DragFloat3("Chromatic Distortion", &data.distortion.x, 0.001f)) changed = true;
        }

        // 고스트 개별 설정 (F2 ~ F6)
        auto DrawGhostGroup = [&](const char* label, DirectX::XMFLOAT3& offset, float& power, DirectX::XMFLOAT3& color) {
            if (ImGui::TreeNode(label)) {
                if (ImGui::DragFloat3("RGB Offset", &offset.x, 0.01f)) changed = true;
                if (ImGui::SliderFloat("Power/Sharpness", &power, 0.1f, 100.0f)) changed = true;
                if (ImGui::ColorEdit3("Color Multiplier", &color.x)) changed = true;
                ImGui::TreePop();
            }
            };

        if (ImGui::CollapsingHeader("Ghost Elements (F-Series)"))
        {
            DrawGhostGroup("Ghost F2 (Main Halo)", data.f2Offset, data.f2Sharpness, data.f2ColorMult);
            DrawGhostGroup("Ghost F4 (Inner Rings)", data.f4Offset, data.f4Power, data.f4ColorMult);
            DrawGhostGroup("Ghost F5 (Small Orbs)", data.f5Offset, data.f5Power, data.f5ColorMult);
            DrawGhostGroup("Ghost F6 (Outer Diffraction)", data.f6Offset, data.f6Power, data.f6ColorMult);

            ImGui::Separator();
            if (ImGui::SliderFloat("Ghost Pull", &data.ghostPull, -1.0f, 1.0f)) changed = true;
            if (ImGui::SliderFloat("Ghost Intensity", &data.ghostIntensity, 0.0f, 10.0f)) changed = true;
        }

        ImGui::Spacing();
        ImGui::Separator();

        ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(0.0f, 0.6f, 0.6f));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::HSV(0.0f, 0.7f, 0.7f));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor::HSV(0.0f, 0.8f, 0.8f));
        if (ImGui::Button("Reset LensFlare to Default", ImVec2(ImGui::GetContentRegionAvail().x, 0)))
        {
            data = LensFlareBuffer();
            changed = true;
        }
        ImGui::PopStyleColor(3);

        return changed;
    } // DrawLensFlare

} // ShaderBufferImGuiDrawer