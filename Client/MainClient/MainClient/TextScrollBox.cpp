#include "stdafx.h"
#include "TextScrollBox.h"


TextScorllBox::TextScorllBox(const Rect box)
{
	m_X = box.x;
	m_Y = box.y;
	m_W = box.w;
	m_H = box.h;
}

TextScorllBox::~TextScorllBox()
{
	m_TextVector.clear();
}

void TextScorllBox::Update()
{

}

void TextScorllBox::Draw()
{
	//Rect(m_X, m_Y, m_W, m_H).draw();
	// TODO :: Text Draw.
}

void TextScorllBox::SetBackgroundColor(const Color)
{

}

void TextScorllBox::GetText(std::wstring)
{

}
