/* Content of this file written by Dave Humphrey
 * And possible modifications by Marc 'rhaamo' Lagrange
 */

#ifndef __IL_LANDSCAPE_H
#define __IL_LANDSCAPE_H


#include "il_map.h"
#include <vector>
#include <string>
#include "noiseutils/noiseutils.h"


namespace infland
{

	static const int NUM_TEMPERATURE_COEF = 6;

	struct altitude_params_t
	{
		float SeaLevel;
		float MinLevel;
		float MaxLevel;
		int	Octaves;
		float X1, Y1, X2, Y2;
		double Frequency;
		double Lacunarity;
		double Persistence;

		altitude_params_t();
	};


	struct temperature_params_t
	{
		float	Minimum;
		float	Maximum;
		float	Coef[NUM_TEMPERATURE_COEF+1];
		int		Blur;

		temperature_params_t();
	};


	struct rainfall_params_t
	{
		int Blur;

		rainfall_params_t();
	};


	struct wind_params_t
	{
		float	TradeWindLongitude;
		float	TemperatureBlurMagnitude;

		wind_params_t();
	};


	const size_t CLIMMASK_MATCHALL			= 0;
	const size_t CLIMMASK_MIN_TEMPERATURE	= 1;
	const size_t CLIMMASK_MAX_TEMPERATURE	= 2;
	const size_t CLIMMASK_MIN_ALTITUDE		= 4;
	const size_t CLIMMASK_MAX_ALTITUDE		= 8;
	const size_t CLIMMASK_MIN_RAINFALL		= 16;
	const size_t CLIMMASK_MAX_RAINFALL		= 32;
	const size_t CLIMMASK_MIN_LONGITUDE		= 64;
	const size_t CLIMMASK_MAX_LONGITUDE		= 128;


	struct climate_info_t
	{
		int		Type;
		size_t	Mask;
		CColor	Color;
		CColor	TrueColor;
		CGradient Gradient;

		ilstring Name;

		float	MinTemperature;
		float	MaxTemperature;
		float	MinAltitude;
		float	MaxAltitude;
		float	MinRainfall;
		float	MaxRainfall;
		float	MinLongitude;
		float	MaxLongitude;

		int		PixelCount;


		climate_info_t()
		{
			Mask = 0;
			Type = 0;
			PixelCount = 0;
			MinTemperature = 0;
			MaxTemperature = 0;
			MinAltitude = 0;
			MaxAltitude = 0;
			MinRainfall = 0;
			MaxRainfall = 0;
			MinLongitude = 0;
			MaxLongitude = 0;
			Color = 0;
			TrueColor = 0;
		}


		bool IsSet(const size_t Value)
		{
			return (Mask & Value) != 0;
		}


		void SetMask(const size_t Value, const bool Set)
		{
			if (Set)
				Mask |= Value;
			else
				Mask &= ~Value;
		}


		void MakeGradient (void)
		{
			Gradient.Clear();
			Gradient.AddPoint(-1, TrueColor.Darken(1.5));
			Gradient.AddPoint(0, TrueColor);
			Gradient.AddPoint(1, TrueColor.Darken(5));
		}

	};

	typedef std::vector<climate_info_t> CClimateArray;


	struct climate_params_t
	{

		climate_params_t();
	};


	struct erosion_params_t
	{
		float   ErodeDelta;
		size_t	Blur;
		size_t	Blur2;
		float	CombineFactor;

		erosion_params_t();
	};


	class CLandscape
	{
		/* Class constant definitions */
	public:
		static const size_t IL_DEFAULT_MAPWIDTH  = 256;
		static const size_t IL_DEFAULT_MAPHEIGHT = 256;
		static const int    IL_DEFAULT_SEED      = 0;

		static const float IL_DEFAULT_MINTEMPERATURE;
		static const float IL_DEFAULT_MAXTEMPERATURE;

		static altitude_params_t	DEFAULT_ALTITUDE_PARAMS;
		static temperature_params_t DEFAULT_TEMPERATURE_PARAMS;
		static rainfall_params_t	DEFAULT_RAINFALL_PARAMS;
		static wind_params_t		DEFAULT_WIND_PARAMS;
		static climate_params_t		DEFAULT_CLIMATE_PARAMS;
		static erosion_params_t		DEFAULT_EROSION_PARAMS;

		static CGradient DEFAULT_ALTITUDE_GRADIENT;
		static CGradient DEFAULT_TEMPERATURE_GRADIENT;
		static CGradient DEFAULT_RAINFALL_GRADIENT;

		CGradient TERRAIN_GRADIENT;
		CGradient CLIMATE_GRADIENT;


		/* Class member definitions */
	private:
		CMap	m_AltitudeMap;			/* Individual parameter maps */
		CMap	m_TemperatureMap;
		CMap	m_RainfallMap;
		CMap	m_WindMap;
		CMap	m_ClimateMap;
		CMap	m_WaterMap;
		CMap	m_ErosionMap;
		CMap	m_CombinedMap;

		mapwrap_t	m_MapWrapping;

		int		m_Seed;					/* Seed for the pseudo-random number generators */

		size_t	m_Width;				/* Map size in pixels */
		size_t	m_Height;

		altitude_params_t		m_AltitudeParams;
		temperature_params_t	m_TemperatureParams;
		rainfall_params_t		m_RainfallParams;
		wind_params_t			m_WindParams;
		climate_params_t		m_ClimateParams;
		erosion_params_t		m_ErosionParams;

		CClimateArray			m_Climates;


		/* Class private implementation definitions */
	private:

		float ComputeTemperature_Norm (const float  Y, const float  Z);
		float ComputeTemperature      (const size_t X, const size_t Y);

		float ComputeWindDirection (const size_t X, const size_t Y);

		static int CreateTemperatureGradient   (CGradient& Gradient);
		static int CreateRainfallGrayGradient  (CGradient& Gradient);
		static int CreateRainfallColorGradient (CGradient& Gradient);

		int CreateSimpleTerrainGradient (CGradient& Gradient);
		int CreateClimateGradient       (CGradient& Gradient);

		void BlurTemperatureWind(void);

		climate_info_t* FindClimate(const size_t X, const size_t Y);

		void ErodeStart(const size_t X, const size_t Y);
		void ErodePoint(const size_t X, const size_t Y);


		/* Class public interface definitions */
	public:

		CLandscape();
		CLandscape(const CLandscape& Source);
		~CLandscape();

		CLandscape& operator=(const CLandscape &Source);

		void AddClimate(const climate_info_t Climate);
		void CreateDefaultClimates(void);
		void ClearClimates(void);
		void OutputClimateCounts(void);


		void CreateAltitude		(const altitude_params_t    Params = DEFAULT_ALTITUDE_PARAMS);
		void CreateTemperature	(const temperature_params_t Params = DEFAULT_TEMPERATURE_PARAMS);
		void CreateRainfall		(const rainfall_params_t    Params = DEFAULT_RAINFALL_PARAMS);
		void CreateWind         (const wind_params_t		Params = DEFAULT_WIND_PARAMS);
		void CreateClimate      (const climate_params_t		Params = DEFAULT_CLIMATE_PARAMS);
		void CreateErosion		(const erosion_params_t		Params = DEFAULT_EROSION_PARAMS);
		void CreateCombinedMap  (void);
		void CreateWaterMap		(void);


		CMap& GetAltitudeMap    (void) { return m_AltitudeMap; }
		CMap& GetTemperatureMap (void) { return m_TemperatureMap; }
		CMap& GetRainfallMap    (void) { return m_RainfallMap; }
		CMap& GetWindMap        (void) { return m_WindMap; }
		CMap& GetClimateMap     (void) { return m_ClimateMap; }
		CMap& GetWaterMap		(void) { return m_WaterMap; }
		CMap& GetErosionMap		(void) { return m_ErosionMap; }
		CMap& GetCombinedMap	(void) { return m_CombinedMap; }

		int GetSeed (void) const { return m_Seed; }

		void OutputModifiedTerrainBMP (const TCHAR* pFilename);


		void SetSeed(const int Seed);
		void SetSize(const size_t Width, const size_t Height);


		float ComputeLongitude(const size_t PixelY);


	};


};


#endif
