/* Content of this file written by Dave Humphrey
 * And possible modifications by Marc 'rhaamo' Lagrange
 */

#include "il_noiseutils.h"
#include "noiseutils/noiseutils.h"

using namespace noise::utils;
using namespace noise;

namespace infland
{


	void CopyGradient (GradientColor& Dest, const GradientColor& Source)
	{
		const GradientPoint* pPoint = Source.GetGradientPointArray();

		Dest.Clear();
		if (pPoint == NULL) return;

		for (int i = 0; i < Source.GetGradientPointCount(); ++i)
		{
			Dest.AddGradientPoint(pPoint[i].pos, pPoint[i].color);
		}
	}

	void CopyGradient (RendererImage& Dest, const GradientColor& Source)
	{
		const GradientPoint* pPoint = Source.GetGradientPointArray();

		Dest.ClearGradient();
		if (pPoint == NULL) return;

		for (int i = 0; i < Source.GetGradientPointCount(); ++i)
		{
			Dest.AddGradientPoint(pPoint[i].pos, pPoint[i].color);
		}
	}


};
