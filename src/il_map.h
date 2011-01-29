/* Content of this file written by Dave Humphrey
 * And possible modifications by Marc 'rhaamo' Lagrange
 */

#ifndef __IL_MAP_H
#define __IL_MAP_H


#include "il_defs.h"



namespace infland
{

	enum mapwrap_t
	{
		MAPWRAP_NONE = 0,
		MAPWRAP_X	 = 1,
		MAPWRAP_Y	 = 2,
		MAPWRAP_XY	 = 3
	};


	class CMap
	{
		friend class CLandscape;

	private:
		noise::utils::NoiseMap*		m_pMapData;

		static CGradient DEFAULT_GRADIENT;


	private:

		float* CreateKernelPascal   (const int Radius);
		float* CreateKernelGaussian (const int Radius);

		noise::utils::NoiseMap&	GetMapData (void);


	public:
		CMap();
		CMap(const CMap& Source);
		~CMap();

		CMap& operator=(const CMap &Source);


		void Clear (const float Value);
		void Blur (int Level, const mapwrap_t MapWrap = MAPWRAP_NONE);
		void Normalize    (const float Max);
		void NormalizeMin (const float Min);


		void SetSize(const size_t Width, const size_t Height);
		size_t GetWidth (void) const;
		size_t GetHeight (void) const;
		void SetBorderValue (const float Value);


		float GetValue(const int X, const int Y);
		void  SetValue(const int X, const int Y, const float Value);
		void  ModifyValue(const int X, const int Y, const float Delta);


		bool OutputBmp(const TCHAR* pFilename, const CGradient& Gradient = DEFAULT_GRADIENT);

		bool FindMinimumNeighbour (int& X, int& Y);

	};

};

#endif
