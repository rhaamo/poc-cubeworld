/* Content of this file written by Dave Humphrey
 * And possible modifications by Marc 'rhaamo' Lagrange
 */

#ifndef __IL_NOISEUTILS_H
#define __IL_NOISEUTILS_H

	/* Forward declare the noisemap class from the libnoise namespaces */
namespace noise {
	namespace utils {
		class NoiseMap;
		class GradientColor;
		class RendererImage;
	}
};



namespace infland
{


	void CopyGradient (noise::utils::GradientColor& Dest, const noise::utils::GradientColor& Source);
	void CopyGradient (noise::utils::RendererImage& Dest, const noise::utils::GradientColor& Source);


};


#endif
