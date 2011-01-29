/* Content of this file written by Dave Humphrey
 * And possible modifications by Marc 'rhaamo' Lagrange
 */

#ifndef __IL_DEFS
#define __IL_DEFS

// rhaamo - windows stuff ?
//#include "tchar.h"
#include "compat.h"
#include <string>


/*
 * Private implementation class forward declarations
 */
namespace noise
{
	namespace utils
	{
		class GradientColor;
		class Color;
		class RendererImage;
		class NoiseMap;
	}
}


namespace infland
{

	typedef unsigned char byte;

	typedef std::string ilstring;

	class CColor
	{
		friend class CGradient;
		friend class CLandscape;

	private:
		noise::utils::Color*	m_pColor;

	public:
		CColor();
		CColor(const byte R, const byte G, const byte B, const byte A = 255);
		CColor(const CColor& Source);
		CColor(const size_t Color);
		~CColor();

		CColor& operator=(const CColor &Source);

		CColor Darken (const float Amt) const;

		size_t GetColorInt (void) const;

	};


	class CGradient
	{
	private:
		noise::utils::GradientColor*	m_pGradient;


	public:
		CGradient();
		CGradient(const CGradient& Source);
		~CGradient();

		CGradient& operator=(const CGradient &Source);

		void AddPoint (const double Position, const CColor& Color);

		void Clear (void);

		int GetPointCount (void) const;

		CColor GetColor (const double gradientPos) const;

		void CopyTo (noise::utils::RendererImage& Renderer) const;



	};


	std::string ConvertStringFromWide (const std::wstring Source);




};


#endif
