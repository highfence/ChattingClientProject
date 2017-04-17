#pragma once

struct TextData
{
	std::wstring text;
	int textYPostion = 0;
};

class TextScorllBox
{
public :

	void Update();
	void Draw();
	void SetBackgroundColor(const Color);


private :

	int m_X = 0;
	int m_Y = 0;
	int m_W = 0;
	int m_H = 0;

	Color m_BackgroundColor = Color(0, 0, 0);
	std::vector<TextData*> m_TextVector;
};