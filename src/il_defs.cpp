/* Content of this file written by Dave Humphrey
 * And possible modifications by Marc 'rhaamo' Lagrange
 */

#include "il_defs.h"
#include "noiseutils/noiseutils.h"
#include "il_noiseutils.h"


namespace infland
{

	CColor::CColor()
	{
		m_pColor = new noise::utils::Color;
	}


	CColor::CColor(const byte R, const byte G, const byte B, const byte A)
	{
		m_pColor = new noise::utils::Color;
		m_pColor->red   = R;
		m_pColor->green = G;
		m_pColor->blue  = B;
		m_pColor->alpha = A;
	}


	CColor::CColor(const CColor& Source)
	{
		m_pColor = new noise::utils::Color(*Source.m_pColor);
	}


	CColor::CColor(const size_t Source)
	{
		m_pColor = new noise::utils::Color;

		m_pColor->red   = (Source & 0xff);
		m_pColor->green = (Source >> 8) & 0xff;
		m_pColor->blue  = (Source >> 16) & 0xff;
		m_pColor->alpha = (Source >> 24) & 0xff;
	}


	CColor::~CColor()
	{
		delete m_pColor;
	}


	CColor& CColor::operator=(const CColor &Source)
	{
		*m_pColor = *Source.m_pColor;
		return *this;
	}


	size_t CColor::GetColorInt (void) const
	{
		return m_pColor->red + (m_pColor->green << 8) +(m_pColor->blue << 16) + (m_pColor->alpha << 24);
	}


	CColor CColor::Darken (const float Amt) const
	{
		CColor NewColor(*this);

		NewColor.m_pColor->red   /= Amt;
		NewColor.m_pColor->green /= Amt;
		NewColor.m_pColor->blue  /= Amt;

		return NewColor;
	}


	CGradient::CGradient()
	{
		m_pGradient = new noise::utils::GradientColor;
	}


	CGradient::CGradient(const CGradient& Source)
	{
		m_pGradient = new noise::utils::GradientColor;
		CopyGradient(*m_pGradient, *Source.m_pGradient);
	}


	CGradient::~CGradient()
	{
		delete m_pGradient;
	}


	CGradient& CGradient::operator=(const CGradient &Source)
	{
		*m_pGradient = *Source.m_pGradient;
		return *this;
	}


	void CGradient::Clear (void)
	{
		m_pGradient->Clear();
	}


	void CGradient::AddPoint (const double Position, const CColor& Color)
	{
		m_pGradient->AddGradientPoint(Position, *Color.m_pColor);
	}


	int CGradient::GetPointCount (void) const
	{
		return m_pGradient->GetGradientPointCount();
	}


	CColor CGradient::GetColor (const double Position) const
	{
		const noise::utils::Color& Color = m_pGradient->GetColor(Position);

		return CColor(Color.red, Color.green, Color.blue, Color.alpha);
	}


	void CGradient::CopyTo (noise::utils::RendererImage& Renderer) const
	{
		CopyGradient(Renderer, *m_pGradient);
	}



	std::string ConvertStringFromWide (const std::wstring Source)
	{
		size_t			OrigSize = Source.length();
		size_t			ConvertedChars = 0;
		char*			pTempString;

		if (OrigSize <= 0) return std::string();
		pTempString = new char[OrigSize + 4];

#if defined(__linux__) || defined(__APPLE__)
		wcstombs(pTempString, Source.c_str(), OrigSize+1);
#else
		wcstombs_s(&ConvertedChars, pTempString, OrigSize+1, Source.c_str(), _TRUNCATE);
#endif

		std::string Result(pTempString);
		delete[] pTempString;

		return Result;
	}

};
