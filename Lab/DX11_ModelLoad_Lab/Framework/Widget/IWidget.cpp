// Framework/Widget/IWidget.cpp
#include "IWidget.h"

/* defualt */
////////////////////////////////////

IWidget::IWidget()
	: i_name("?"),
	i_isVisible(true) 
{
}


IWidget::IWidget(const std::string& name)
	: i_name(name), i_isVisible(true)
{}
