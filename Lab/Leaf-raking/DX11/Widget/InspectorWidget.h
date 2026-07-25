// UserInterface/Widget/InspectorWidget.h
#pragma once
#include <vector>
#include <memory>
#include "IWidget.h"

class ActorObject;

class InspectorWidget : public IWidget {
public:
    InspectorWidget();
    InspectorWidget(const InspectorWidget&) = delete;
    virtual ~InspectorWidget() = default;

    virtual void Frame() override;
public:
    void SetTarget(ActorObject*);
    void SetActorList(const std::vector<std::unique_ptr<ActorObject>>& actors);

private:
    ActorObject* m_selectedActor;
    std::vector<ActorObject*> m_actorList;
}; // InspectorWidget