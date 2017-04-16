#pragma once


struct InlineFrameData
{
	Rect region;

	Rect contentsArea;

	Vec2 contentsOffset;
};

class InlineFrame
{
private:

	Rect m_region{ 0,0 };

	Size m_contentsSize{ 0,0 };

	int32 m_barThickness = 18;

	double m_scrollSpeed = 10.0;

	Vec2 m_offset{ 0, 0 };

	bool m_horizontalBar = false;

	bool m_verticalBar = false;

	bool m_horizontalBarGrabbed = false;

	bool m_verticalBarGrabbed = false;

	bool m_leftPushed = false;

	bool m_rightPushed = false;

	bool m_upPushed = false;

	bool m_downPushed = false;

	Rect getHorizontalArea() const
	{
		return Rect(m_region.x, m_region.y + m_region.h - m_barThickness,
			m_region.w - m_verticalBar * m_barThickness, m_barThickness);
	}

	Rect getHorizontalBarArea() const
	{
		return getHorizontalArea().stretched(-m_barThickness, 0);
	}

	RectF getHorizontalBar() const
	{
		if (isEmpty() || !m_horizontalBar)
		{
			return RectF(0);
		}

		const double scale = static_cast<double>(getContentsArea().w) / m_contentsSize.x;
		RectF bar = getHorizontalBarArea();
		bar.w *= scale;
		bar.x += (getHorizontalBarArea().w - bar.w) * (m_offset.x / getMaxOffsetX());

		return bar.stretched(0, -1.5);
	}

	RectF getLeftButton() const
	{
		if (isEmpty() || !m_horizontalBar)
		{
			return RectF(0);
		}

		return RectF(getHorizontalArea().pos, m_barThickness);
	}

	RectF getRightButton() const
	{
		if (isEmpty() || !m_horizontalBar)
		{
			return RectF(0);
		}

		return RectF(getHorizontalArea().tr.movedBy(-m_barThickness, 0), m_barThickness);
	}

	Rect getVerticalArea() const
	{
		return Rect(m_region.x + m_region.w - m_barThickness, m_region.y,
			m_barThickness, m_region.h - m_horizontalBar * m_barThickness);
	}

	Rect getVerticalBarArea() const
	{
		return getVerticalArea().stretched(0, -m_barThickness);
	}

	RectF getVerticalBar() const
	{
		if (isEmpty() || !m_verticalBar)
		{
			return RectF(0);
		}

		const double scale = static_cast<double>(getContentsArea().h) / m_contentsSize.y;
		RectF bar = getVerticalBarArea();
		bar.h *= scale;
		bar.y += (getVerticalBarArea().h - bar.h) * (m_offset.y / getMaxOffsetY());

		return bar.stretched(-1.5, 0);
	}

	RectF getUpButton() const
	{
		if (isEmpty() || !m_verticalBar)
		{
			return RectF(0);
		}

		return RectF(getVerticalArea().pos, m_barThickness);
	}

	RectF getDownButton() const
	{
		if (isEmpty() || !m_verticalBar)
		{
			return RectF(0);
		}

		return RectF(getVerticalArea().br.movedBy(-m_barThickness, -m_barThickness), m_barThickness);
	}

	Rect getBR() const
	{
		return Rect(m_region.x + m_region.w - m_barThickness, m_region.y + m_region.h - m_barThickness,
			m_barThickness, m_barThickness);
	}

	double getMaxOffsetX() const
	{
		return Max(0, m_contentsSize.x - getContentsArea().w);
	}

	double getMaxOffsetY() const
	{
		return Max(0, m_contentsSize.y - getContentsArea().h);
	}

	Rect getContentsArea() const
	{
		return m_region.stretched(0, m_verticalBar * -m_barThickness, m_horizontalBar * -m_barThickness, 0);
	}

public:

	InlineFrame() = default;

	explicit InlineFrame(const Rect& region)
		: m_region(region) {}

	explicit operator bool() const
	{
		return isEmpty();
	}

	bool isEmpty() const
	{
		return m_contentsSize.x == 0 || m_contentsSize.y == 0;
	}

	void update(const Size& contentsSize)
	{
		m_horizontalBar = (m_region.w < contentsSize.x);
		m_verticalBar = (m_region.h < contentsSize.y);
		m_contentsSize = contentsSize;

		m_horizontalBarGrabbed &= m_horizontalBar;
		m_verticalBarGrabbed &= m_verticalBar;

		if (Input::MouseL.released)
		{
			m_horizontalBarGrabbed = m_verticalBarGrabbed = false;
			m_leftPushed = m_rightPushed = m_upPushed = m_downPushed = false;
		}

		if (getHorizontalBar().leftClicked)
		{
			m_horizontalBarGrabbed = true;
		}
		else if (m_horizontalBarGrabbed)
		{
			const double scale = static_cast<double>(getContentsArea().w) / m_contentsSize.x;
			const double barWidth = getHorizontalBarArea().w * scale;
			const double barSpace = getHorizontalBarArea().w - barWidth;
			m_offset.x += getMaxOffsetX() * (Mouse::Delta().x / barSpace);
		}

		if (getVerticalBar().leftClicked)
		{
			m_verticalBarGrabbed = true;
		}
		else if (m_verticalBarGrabbed)
		{
			const double scale = static_cast<double>(getContentsArea().h) / m_contentsSize.y;
			const double barHeight = getVerticalBarArea().h * scale;
			const double barSpace = getVerticalBarArea().h - barHeight;
			m_offset.y += getMaxOffsetY() * (Mouse::Delta().y / barSpace);
		}

		const bool longTime = Input::MouseL.pressedDuration >= 350;

		if (getLeftButton().leftClicked || (m_leftPushed && longTime && getLeftButton().leftPressed))
		{
			m_leftPushed = true;
			m_offset.x -= m_scrollSpeed;
		}
		else if (getRightButton().leftClicked || (m_rightPushed && longTime && getRightButton().leftPressed))
		{
			m_rightPushed = true;
			m_offset.x += m_scrollSpeed;
		}
		else if (getUpButton().leftClicked || (m_upPushed && longTime && getUpButton().leftPressed))
		{
			m_upPushed = true;
			m_offset.y -= m_scrollSpeed;
		}
		else if (getDownButton().leftClicked || (m_downPushed && longTime && getDownButton().leftPressed))
		{
			m_downPushed = true;
			m_offset.y += m_scrollSpeed;
		}

		if (m_region.mouseOver)
		{
			m_offset.x += Mouse::WheelH() * m_scrollSpeed * (static_cast<double>(m_contentsSize.x) / getContentsArea().w);
			m_offset.y += Mouse::Wheel() * m_scrollSpeed * (static_cast<double>(m_contentsSize.y) / getContentsArea().h);
		}

		m_offset.x = Clamp<double>(m_offset.x, 0, getMaxOffsetX());
		m_offset.y = Clamp<double>(m_offset.y, 0, getMaxOffsetY());
	}

	InlineFrameData getFrameData() const
	{
		return{ m_region, getContentsArea(), m_offset };
	}

	void draw(const Color& barBackground = Color(246),
		const Color& bar = Color(210),
		const Color& barGrabbed = Color(170)) const
	{
		if (m_horizontalBar)
		{
			getHorizontalArea().draw(barBackground);
			Triangle(getHorizontalArea().tl.movedBy(m_barThickness / 2, m_barThickness / 2), m_barThickness * 0.4, -90_deg).draw(Color(160));
			Triangle(getHorizontalArea().tr.movedBy(-m_barThickness / 2, m_barThickness / 2), m_barThickness * 0.4, 90_deg).draw(Color(160));
			getHorizontalBar().draw(m_horizontalBarGrabbed ? barGrabbed : bar);
		}

		if (m_verticalBar)
		{
			getVerticalArea().draw(barBackground);
			Triangle(getVerticalArea().tl.movedBy(m_barThickness / 2, m_barThickness / 2), m_barThickness * 0.4, 0_deg).draw(Color(160));
			Triangle(getVerticalArea().bl.movedBy(m_barThickness / 2, -m_barThickness / 2), m_barThickness * 0.4, 180_deg).draw(Color(160));
			getVerticalBar().draw(m_verticalBarGrabbed ? barGrabbed : bar);
		}

		if (m_horizontalBar && m_verticalBar)
		{
			getBR().draw(barBackground);
		}
	}
};
