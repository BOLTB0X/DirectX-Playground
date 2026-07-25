// Framework/Widget/IWidget.h
#pragma once
#include <string>

class IWidget {
public:
    IWidget();
	IWidget(const IWidget&) = delete;
    IWidget(const std::string&);
    virtual ~IWidget() = default;
    virtual void Frame() = 0;

    void SetVisible(bool visible) { i_isVisible = visible; }
    bool IsVisible() const { return i_isVisible; }
    const std::string& GetName() const { return i_name; }

protected:
    std::string i_name;
    bool i_isVisible;
}; // IWidget
