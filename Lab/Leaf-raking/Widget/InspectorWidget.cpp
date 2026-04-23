// UserInterface/Widget/InspectorWidget.cpp
#include "InspectorWidget.h"
#include "imgui.h"
// Framework
#include "Actor/ActorObject.h"
#include "Position/Position.h"

/* default */
/////////////////////////////////////////////////////////////////////

InspectorWidget::InspectorWidget()
	: IWidget("Inspector"),
    m_selectedActor(nullptr)
{
} // InspectorWidget


void InspectorWidget::Frame()
{
    if (m_selectedActor == nullptr)
    {
        ImGui::TextDisabled("m_selectedActor => nullptr");
        return;
    }

    ImGui::TextColored(ImVec4(0.7f, 0.7f, 1.0f, 1.0f), "World Hierarchy");
    if (ImGui::BeginChild("HierarchyRegion", ImVec2(0, 150), true))
    {
        for (auto* actor : m_actorList)
        {
            bool isSelected = (m_selectedActor == actor);
            if (ImGui::Selectable(actor->GetName().c_str(), isSelected))
                m_selectedActor = actor;
        }
    }
    ImGui::EndChild();

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "Inspector");
    if (m_selectedActor == nullptr)
    {
        ImGui::TextDisabled("선택한 actor to inspect 프로퍼티.");
        return;
    }

    ImGui::BulletText("Selected: %s", m_selectedActor->GetName().c_str());
    if (ImGui::BeginTable("InspectorTable", 2, ImGuiTableFlags_BordersInnerV)) {
        ImGui::TableSetupColumn("Property", ImGuiTableColumnFlags_WidthFixed, 80.0f);
        ImGui::TableSetupColumn("Value", ImGuiTableColumnFlags_WidthStretch);

        auto transform = m_selectedActor->GetPosition();

        // Position Row
        ImGui::TableNextRow();
        ImGui::TableNextColumn(); ImGui::Text("Position");
        ImGui::TableNextColumn();
        ImGui::PushItemWidth(-1);
        float pos[3] = { transform->GetPosition().x, transform->GetPosition().y, transform->GetPosition().z};
        if (ImGui::DragFloat3("##P", pos, 0.1f)) transform->SetPosition(pos[0], pos[1], pos[2]);
        ImGui::PopItemWidth();

        // Rotation Row
        ImGui::TableNextRow();
        ImGui::TableNextColumn(); ImGui::Text("Rotation");
        ImGui::TableNextColumn();
        ImGui::PushItemWidth(-1);
        float rot[3] = { transform->GetRotation().x, transform->GetRotation().y, transform->GetRotation().z };
        if (ImGui::DragFloat3("##R", rot, 1.0f)) transform->SetRotation(rot[0], rot[1], rot[2]);
        ImGui::PopItemWidth();

        ImGui::EndTable();
    }
} // Frame


void InspectorWidget::SetTarget(ActorObject* actor)
{
    m_selectedActor = actor;
} // SetTarget


void InspectorWidget::SetActorList(const std::vector<std::unique_ptr<ActorObject>>& actors)
{
    m_actorList.clear();
    for (auto& actor : actors)
        m_actorList.push_back(actor.get());
} // SetActorList