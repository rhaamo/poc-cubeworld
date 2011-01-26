#include <noise/noise.h>
#include "noiseutils/noiseutils.h"

class LandMapErosion
{
public:
	utils::NoiseMap		m_HeightMap;
	module::Perlin		m_NoiseSource;

	model::Plane planeModel;

	virtual void CreateBaseNoise (int m_Width, int m_Height)
	{
		utils::NoiseMapBuilderPlane HeightMapBuilder;

		m_NoiseSource.SetOctaveCount(12);

		HeightMapBuilder.SetSourceModule (m_NoiseSource);
		HeightMapBuilder.SetDestNoiseMap (m_HeightMap);
		HeightMapBuilder.SetDestSize (m_Width, m_Height);
		HeightMapBuilder.SetBounds (0.0, 1.0, 0.0, 1.0);
		HeightMapBuilder.Build ();

		planeModel = HeightMapBuilder.planeModel;
	}

};
