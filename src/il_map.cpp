/* Content of this file written by Dave Humphrey
 * And possible modifications by Marc 'rhaamo' Lagrange
 */

#include "il_map.h"
#include "il_noiseutils.h"
#include "noiseutils/noiseutils.h"

namespace infland
{

	CGradient CMap::DEFAULT_GRADIENT;

	CMap::CMap()
	{
		m_pMapData = new utils::NoiseMap;
	}


	CMap::CMap(const CMap& Source)
	{
		m_pMapData = new utils::NoiseMap(*Source.m_pMapData);
	}


	CMap::~CMap()
	{
		delete m_pMapData;
	}


	CMap& CMap::operator=(const CMap &Source)
	{
		*m_pMapData = *Source.m_pMapData;
		return *this;
	}


	void CMap::Clear (const float Value)
	{
		m_pMapData->Clear(Value);
	}


	float* CMap::CreateKernelPascal (const int Radius)
	{
		int Level = Radius*2 + 1;
		float* pKernel = new float[Level + 1];
		float KernelSum;
		int i;

		pKernel[0] = 1;
		KernelSum = 1;

		for (i = 1; i < Level; ++i)
		{
			pKernel[i] = (float) floor(pKernel[i-1] * (Level - i) / i);
			KernelSum += pKernel[i];
		}

		for (i = 0; i < Level; ++i)
		{
			pKernel[i] /= KernelSum;
		}

		return (pKernel);
	}


	float* CMap::CreateKernelGaussian (const int Radius)
	{
		int Rows = Radius*2 + 1;
		float* pKernel = new float[Rows + 1];
		float sigma = (float)Radius/3;
		float sigma22 = 2*sigma*sigma;
		float sigmaPi2 = 2*3.1415926535f*sigma;
		float sqrtSigmaPi2 = (float)sqrt(sigmaPi2);
		float radius2 = (float)Radius*Radius;
		float KernelSum = 0;
		int i = 0;

		for (int row = -Radius; row <= Radius; row++) {
			float distance = (float) row*row;

			if (distance > radius2)
				pKernel[i] = 0;
			else
				pKernel[i] = (float) exp(-(distance)/sigma22) / sqrtSigmaPi2;

			KernelSum += pKernel[i];
			i++;
		}

		for (i = 0; i < Rows; ++i)
		{
			pKernel[i] /= KernelSum;
		}

		return (pKernel);
	}


	void CMap::Blur (int Radius, const mapwrap_t MapWrap)
	{
		int Width = m_pMapData->GetWidth();
		int Height = m_pMapData->GetHeight();

		if (Radius > Width)  Radius = Width - 1;
		if (Radius > Height) Radius = Height - 1;
		if (Radius <= 0) return;

		utils::NoiseMap	TempMap(Width, Height);
		float Sum;

		int x, y, i, x1, y1;
		int Rows = Radius*2 + 1;
		bool WrapX = (MapWrap == MAPWRAP_X || MapWrap == MAPWRAP_XY);
		bool WrapY = (MapWrap == MAPWRAP_Y || MapWrap == MAPWRAP_XY);

		float* Kernel = CreateKernelGaussian(Radius);

			/* First blur pass, y-direction */
		for (y = 0; y < Height; ++y)
		{
			for (x = 0; x < m_pMapData->GetWidth(); ++x)
			{
				Sum = 0;

				for (i = 0; i < Rows; ++i)
				{
					y1 =  y - Rows + 1 + i + Radius;

					if (y1 < 0) {
						if (WrapY)
							Sum += Kernel[i] * m_pMapData->GetValue(x, Height - 1 + y1);
						else
							Sum += Kernel[i] * m_pMapData->GetValue(x, 0);
					}
					else if (y1 >= Height) {
						if (WrapY)
							Sum += Kernel[i] * m_pMapData->GetValue(x, y1 - Height);
						else
							Sum += Kernel[i] * m_pMapData->GetValue(x, Height-1);
					}
					else {
						Sum += Kernel[i] * m_pMapData->GetValue(x, y1);
					}
				}

				TempMap.SetValue(x, y, Sum);
			}
		}

			/* Second blur pass, x-direction */
		for (y = 0; y < Height; ++y)
		{

			for (x = 0; x < Width; ++x)
			{
				Sum = 0;

				for (i = 0; i < Rows; ++i)
				{
					x1 =  x - Rows + 1 + i + Radius;

					if (x1 < 0) {
						if (WrapY)
							Sum += Kernel[i] * m_pMapData->GetValue(Width - 1 + x1, y);
						else
							Sum += Kernel[i] * TempMap.GetValue(0, y);
					}
					else if (x1 >= Width) {
						if (WrapX)
							Sum += Kernel[i] * m_pMapData->GetValue(x1 - Width, y);
						else
							Sum += Kernel[i] * TempMap.GetValue(Width-1, y);
					}
					else {
						Sum += Kernel[i] * TempMap.GetValue(x1, y);
					}
				}

				m_pMapData->SetValue(x, y, Sum);
			}
		}

		delete[] Kernel;
	}


	noise::utils::NoiseMap& CMap::GetMapData (void)
	{
		assert(m_pMapData);
		return *m_pMapData;
	}


	float CMap::GetValue (const int X, const int Y)
	{
		return m_pMapData->GetValue(X, Y);
	}


	void CMap::SetSize(const size_t Width, const size_t Height)
	{
		m_pMapData->SetSize(Width, Height);
	}


	void CMap::SetValue(const int X, const int Y, const float Value)
	{
		m_pMapData->SetValue(X, Y, Value);
	}


	void CMap::ModifyValue(const int X, const int Y, const float Delta)
	{
		m_pMapData->SetValue(X, Y, m_pMapData->GetValue(X, Y) + Delta);
	}


	void CMap::Normalize (const float Max)
	{
		if (Max == 0) return;
		float MaxValue = m_pMapData->GetValue(0, 0);
		int X, Y;

		for (Y = 0; Y < m_pMapData->GetHeight(); ++Y)
		{
			for (X = 0; X < m_pMapData->GetWidth(); ++X)
			{
				if (m_pMapData->GetValue(X,Y) > MaxValue) MaxValue = m_pMapData->GetValue(X,Y);
			}
		}

		if (MaxValue == 0) return;

		for (Y = 0; Y < m_pMapData->GetHeight(); ++Y)
		{
			for (X = 0; X < m_pMapData->GetWidth(); ++X)
			{
				m_pMapData->SetValue(X, Y, m_pMapData->GetValue(X,Y)/MaxValue*Max);
			}
		}

	}


	void CMap::NormalizeMin (const float Min)
	{
		if (Min == 0) return;
		float MinValue = m_pMapData->GetValue(0, 0);
		int X, Y;

		for (Y = 0; Y < m_pMapData->GetHeight(); ++Y)
		{
			for (X = 0; X < m_pMapData->GetWidth(); ++X)
			{
				if (m_pMapData->GetValue(X,Y) < MinValue) MinValue = m_pMapData->GetValue(X,Y);
			}
		}

		if (MinValue == 0) return;

		for (Y = 0; Y < m_pMapData->GetHeight(); ++Y)
		{
			for (X = 0; X < m_pMapData->GetWidth(); ++X)
			{
				m_pMapData->SetValue(X, Y, m_pMapData->GetValue(X,Y)/MinValue*Min);
			}
		}

	}


	void CMap::SetBorderValue(const float Value)
	{
		m_pMapData->SetBorderValue(Value);
	}


	bool CMap::OutputBmp(const TCHAR* pFilename, const CGradient& Gradient)
	{
		utils::Image				OutputImage;
		utils::RendererImage		Renderer;
		utils::WriterBMP			BMPWriter;

		if (Gradient.GetPointCount() > 1)
		{
			Gradient.CopyTo(Renderer);
		}

		//std::string	NewFilename = ConvertStringFromWide(pFilename);
		std::string	NewFilename(pFilename);

		Renderer.SetSourceNoiseMap(*m_pMapData);
		Renderer.SetDestImage(OutputImage);
		Renderer.Render();

		BMPWriter.SetSourceImage(OutputImage);
		BMPWriter.SetDestFilename(NewFilename);
		BMPWriter.WriteDestFile();

		return true;
}

	size_t CMap::GetWidth (void) const { return m_pMapData->GetWidth(); }
	size_t CMap::GetHeight (void) const { return m_pMapData->GetHeight(); }


	bool CMap::FindMinimumNeighbour (int& X, int& Y)
	{
		int OrigX = X;
		int OrigY = Y;
		int Width = m_pMapData->GetWidth();
		int Height = m_pMapData->GetHeight();

		if (X < 0 || Y < 0 || X >= Width || Y >= Height) return (false);


		if (m_pMapData->GetValue(OrigX-1, OrigY+0) < m_pMapData->GetValue(X, Y))
		{
			X = OrigX - 1; Y = OrigY;
			if (X < 0) return (true);
		}

		if (m_pMapData->GetValue(OrigX+1, OrigY+0) < m_pMapData->GetValue(X, Y))
		{
			X = OrigX + 1; Y = OrigY;
			if (X >= Width) return (true);
		}

		if (m_pMapData->GetValue(OrigX+0, OrigY-1) < m_pMapData->GetValue(X, Y))
		{
			X = OrigX; Y = OrigY - 1;
			if (Y < 0) return (true);
		}

		if (m_pMapData->GetValue(OrigX+0, OrigY+1) < m_pMapData->GetValue(X, Y))
		{
			X = OrigX; Y = OrigY + 1;
			if (Y >= Height) return (true);
		}

		if (m_pMapData->GetValue(OrigX+1, OrigY-1) < m_pMapData->GetValue(X, Y))
		{
			X = OrigX + 1; Y = OrigY - 1;
			if (X >= Width) return (true);
			if (Y < 0) return (true);
		}

		if (m_pMapData->GetValue(OrigX+1, OrigY+1) < m_pMapData->GetValue(X, Y))
		{
			X = OrigX + 1; Y = OrigY + 1;
			if (X >= Width) return (true);
			if (Y >= Height) return (true);
		}

		if (m_pMapData->GetValue(OrigX-1, OrigY-1) < m_pMapData->GetValue(X, Y))
		{
			X = OrigX - 1; Y = OrigY - 1;
			if (X < 0) return (true);
			if (Y < 0) return (true);
		}

		if (m_pMapData->GetValue(OrigX-1, OrigY+1) < m_pMapData->GetValue(X, Y))
		{
			X = OrigX - 1; Y = OrigY + 1;
			if (X < 0) return (true);
			if (Y >= Height) return (true);
		}

		if (OrigX == X && OrigY == Y) return (false);
		return (true);
	}



};
