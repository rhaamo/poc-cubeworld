/* Content of this file written by Dave Humphrey
 * And possible modifications by Marc 'rhaamo' Lagrange
 */

#include "il_landscape.h"
#include "il_noiseutils.h"
#include "noiseutils/noiseutils.h"

//#include "file/dl_logfile.h"
//#include "time/dl_profile.h"


namespace infland
{

	const float CLandscape::IL_DEFAULT_MINTEMPERATURE = -64;
	const float CLandscape::IL_DEFAULT_MAXTEMPERATURE = 63;


	altitude_params_t		CLandscape::DEFAULT_ALTITUDE_PARAMS;
	temperature_params_t	CLandscape::DEFAULT_TEMPERATURE_PARAMS;
	rainfall_params_t		CLandscape::DEFAULT_RAINFALL_PARAMS;
	wind_params_t			CLandscape::DEFAULT_WIND_PARAMS;
	climate_params_t		CLandscape::DEFAULT_CLIMATE_PARAMS;
	erosion_params_t		CLandscape::DEFAULT_EROSION_PARAMS;


	CGradient CLandscape::DEFAULT_ALTITUDE_GRADIENT;
	CGradient CLandscape::DEFAULT_TEMPERATURE_GRADIENT;
	CGradient CLandscape::DEFAULT_RAINFALL_GRADIENT;


	altitude_params_t::altitude_params_t()
	{
		SeaLevel = 0;
		MinLevel = -1;
		MaxLevel = 1;
		Octaves = noise::module::DEFAULT_PERLIN_OCTAVE_COUNT;
		X1 = 0;
		Y1 = 0;
		X2 = 1;
		Y2 = 1;
		Frequency = noise::module::DEFAULT_PERLIN_FREQUENCY;
		Lacunarity = noise::module::DEFAULT_PERLIN_LACUNARITY;
		Persistence = noise::module::DEFAULT_PERLIN_PERSISTENCE;
	}


	temperature_params_t::temperature_params_t()
	{
		Minimum		= CLandscape::IL_DEFAULT_MINTEMPERATURE;
		Maximum		= CLandscape::IL_DEFAULT_MAXTEMPERATURE;
		Blur		= 20;

		memset(Coef, 0, sizeof(Coef));
		if (NUM_TEMPERATURE_COEF > 0) Coef[0] = 40.0f;
		if (NUM_TEMPERATURE_COEF > 1) Coef[1] = -30.0f;
		if (NUM_TEMPERATURE_COEF > 2) Coef[2] = -140.0f;
		if (NUM_TEMPERATURE_COEF > 3) Coef[3] = -20.0f;
		if (NUM_TEMPERATURE_COEF > 4) Coef[4] = 20.0f;
		if (NUM_TEMPERATURE_COEF > 5) Coef[5] = 80.0f;
	}


	rainfall_params_t::rainfall_params_t()
	{
		Blur = 100;
	}


	wind_params_t::wind_params_t()
	{
		TradeWindLongitude = 30;
		TemperatureBlurMagnitude = 20;
	}


	climate_params_t::climate_params_t()
	{
	}


	erosion_params_t::erosion_params_t()
	{
		ErodeDelta = 0.0005f;
		Blur = 2;
		Blur2 = 0;
		CombineFactor = 0.5f;
	}


	CLandscape::CLandscape()
	{
		m_Seed = IL_DEFAULT_SEED;

		m_MapWrapping = MAPWRAP_NONE;

		m_Width  = IL_DEFAULT_MAPWIDTH;
		m_Height = IL_DEFAULT_MAPHEIGHT;

		static int s_TempGrad = CreateTemperatureGradient(DEFAULT_TEMPERATURE_GRADIENT);
		static int s_RainGrad = CreateRainfallColorGradient(DEFAULT_RAINFALL_GRADIENT);
		static int s_ClimateGrad = CreateClimateGradient(CLIMATE_GRADIENT);

		CreateDefaultClimates();
	}


	CLandscape::CLandscape(const CLandscape& Source)
	{
		m_Seed = Source.m_Seed;
		m_Width = Source.m_Width;
		m_Height = Source.m_Height;
		m_MapWrapping = Source.m_MapWrapping;

		m_AltitudeParams = Source.m_AltitudeParams;
		m_TemperatureParams = Source.m_TemperatureParams;
		m_RainfallParams = Source.m_RainfallParams;
		m_WindParams = Source.m_WindParams;
		m_ClimateParams = Source.m_ClimateParams;
		m_Climates = Source.m_Climates;

		m_AltitudeMap = Source.m_AltitudeMap;
		m_TemperatureMap = Source.m_TemperatureMap;
		m_RainfallMap = Source.m_RainfallMap;
		m_WindMap = Source.m_WindMap;
		m_ClimateMap = Source.m_ClimateMap;
	}


	CLandscape::~CLandscape()
	{
	}


	CLandscape& CLandscape::operator=(const CLandscape &Source)
	{
		m_Seed = Source.m_Seed;
		m_Width = Source.m_Width;
		m_Height = Source.m_Height;
		m_MapWrapping = Source.m_MapWrapping;

		m_AltitudeParams = Source.m_AltitudeParams;
		m_TemperatureParams = Source.m_TemperatureParams;
		m_RainfallParams = Source.m_RainfallParams;
		m_WindParams = Source.m_WindParams;
		m_ClimateParams = Source.m_ClimateParams;
		m_Climates = Source.m_Climates;

		m_AltitudeMap = Source.m_AltitudeMap;
		m_TemperatureMap = Source.m_TemperatureMap;
		m_RainfallMap = Source.m_RainfallMap;
		m_WindMap = Source.m_WindMap;
		m_ClimateMap = Source.m_ClimateMap;

		return *this;
	}


	void CLandscape::BlurTemperatureWind(void)
	{
		float WindDir;
		float* pTempValues;
		size_t x, y;
		int x1;
		int Offset;

		if (m_Height <= 0 || m_Width <= 0 || m_WindParams.TemperatureBlurMagnitude == 0) return;
		pTempValues = new float[m_Height+1];

		for (y = 0; y < m_Height; ++y)
		{
			WindDir = m_WindMap.GetValue(0, y);
			if (WindDir == 0) continue;

			Offset = (int) (WindDir * m_WindParams.TemperatureBlurMagnitude);

			for (x = 0; x < m_Width; ++x)
			{
				pTempValues[x] = m_TemperatureMap.GetValue(x, y);
			}

			for (x = 0; x < m_Width; ++x)
			{
				x1 = x - Offset;
				if (x1 < 0) x1 = 0;
				if (x1 >= (int)m_Width) x1 = m_Width - 1;

				m_TemperatureMap.SetValue(x, y, pTempValues[x1]);
			}
		}

		delete[] pTempValues;
	}



	int CLandscape::CreateTemperatureGradient(CGradient& Gradient)
	{
		Gradient.Clear();

		Gradient.AddPoint( 50, CColor(255,   0,  0, 255));
		Gradient.AddPoint( 40, CColor(255, 128,  0, 255));
		Gradient.AddPoint( 30, CColor(255, 255,  0, 255));
		Gradient.AddPoint( 20, CColor(  0, 217,  0, 255));
		Gradient.AddPoint( 10, CColor(  0, 255,  0, 255));
		Gradient.AddPoint(  0, CColor(255, 255,255, 255));
		Gradient.AddPoint(-10, CColor(192, 192,192, 255));
		Gradient.AddPoint(-20, CColor(  0, 255,255, 255));
		Gradient.AddPoint(-30, CColor(  0, 128,255, 255));
		Gradient.AddPoint(-40, CColor(  0,   0,255, 255));
		Gradient.AddPoint(-50, CColor(  0,   0,128, 255));

		return 1;
	}


	int CLandscape::CreateSimpleTerrainGradient (CGradient& Gradient)
	{
		float SeaLevel = m_AltitudeParams.SeaLevel;

		Gradient.Clear();

		Gradient.AddPoint(           -1.00, CColor(  0,   0, 128, 255));
		Gradient.AddPoint( SeaLevel + 0.00, CColor( 32,  64, 128, 255));
		Gradient.AddPoint( SeaLevel + 0.10, CColor( 64,  96, 192, 255));
		Gradient.AddPoint( SeaLevel + 0.12, CColor(192, 192, 128, 255));
		Gradient.AddPoint( SeaLevel + 0.16, CColor( 50, 100,   0, 255));
		Gradient.AddPoint( SeaLevel + 0.30, CColor(  0,  64,   0, 255));
		Gradient.AddPoint( SeaLevel + 0.60, CColor( 70,  60,  40, 255));
		Gradient.AddPoint(            1.00, CColor(100, 100, 100, 255));
		Gradient.AddPoint(            1.20, CColor(230, 230, 255, 255));

		return 1;
	}


	int CLandscape::CreateRainfallGrayGradient(CGradient& Gradient)
	{
		Gradient.Clear();

		Gradient.AddPoint(  0, CColor(  0,   0,   0, 255));
		Gradient.AddPoint(100, CColor(255, 255, 255, 255));

		return 1;
	}


	int CLandscape::CreateRainfallColorGradient(CGradient& Gradient)
	{
		Gradient.Clear();

		Gradient.AddPoint(  0, CColor(220, 220, 190, 255));
		Gradient.AddPoint(  5, CColor(210, 210, 235, 255));
		Gradient.AddPoint(100, CColor(128, 128, 255, 255));

		return 1;
	}


	int CLandscape::CreateClimateGradient(CGradient& Gradient)
	{
		CClimateArray::iterator i;
		int Index = 0;

		Gradient.Clear();

		for (i = m_Climates.begin(); i != m_Climates.end(); ++i, ++Index)
		{
			Gradient.AddPoint(Index, i->Color);
		}

		return 1;
	}


	void CLandscape::CreateRainfall(const rainfall_params_t Params)
	{
		int DeltaX;
		int StartX;
		int EndX;
		int X, Y;
		float TempWater = 0;
		float WaterEvap;
		float WindDir;
		float Temperature;
		float Altitude;
		float Rainfall;

		m_RainfallParams = Params;
		m_RainfallMap.SetSize(m_Width, m_Height);
		m_RainfallMap.Clear(0);

		for (Y = 0; Y < (int)m_Height; ++Y)
		{
			WindDir = m_WindMap.GetValue(0, Y);

			if (WindDir >= 0)
			{
				StartX = 0;
				EndX   = m_Width;
				DeltaX = 1;
			}
			else
			{
				StartX = m_Width - 1;
				EndX   = -1;
				DeltaX = -1;
			}

			for (X = StartX; X != EndX; X += DeltaX)
			{
				Temperature = m_TemperatureMap.GetValue(X, Y);
				Altitude    = m_AltitudeMap.GetValue(X, Y);
				Rainfall    = m_RainfallMap.GetValue(X, Y);

				if (Altitude <= m_AltitudeParams.SeaLevel)
				{
					if (Temperature <= 0)
						WaterEvap = 5.0f / (5.0f - Temperature);
					else
						WaterEvap = (Temperature + 5.0f) / 10.0f;

					TempWater += WaterEvap;
				}
				else if (TempWater >= 1)
				{

					if (Temperature <= 0)
					{
						float Temp = ((10.0f - Temperature) / 30.0f + 2.7f);
						Rainfall += (unsigned char) Temp;
						TempWater -= Temp;
					}
					else
					{
						float Temp = (30.0f / (Temperature + 10.0f));
						Rainfall += (unsigned char) Temp;
						TempWater -= Temp;
					}
				}

				m_RainfallMap.SetValue(X, Y, Rainfall*100);
			}

			int Loops = 0;

			while (TempWater > 0 && Loops <= 2)
			{
				if (X == EndX)
				{
					X = StartX;
					++Loops;
				}

				Altitude = m_AltitudeMap.GetValue(X, Y);

				if (Altitude > m_AltitudeParams.SeaLevel)
				{
					m_RainfallMap.SetValue(X, Y, m_RainfallMap.GetValue(X,Y) + 50);
					TempWater -= 1;
				}

				X += DeltaX;
			}
		}

		m_RainfallMap.Blur(m_RainfallParams.Blur);
		m_RainfallMap.Normalize(100);
	}


	void CLandscape::CreateAltitude(const altitude_params_t Params)
	{
		noise::module::Perlin				NoiseSource;
		noise::utils::NoiseMapBuilderPlane	MapBuilder;

		m_AltitudeParams = Params;

		NoiseSource.SetOctaveCount(m_AltitudeParams.Octaves);
		NoiseSource.SetFrequency(m_AltitudeParams.Frequency);
		NoiseSource.SetLacunarity(m_AltitudeParams.Lacunarity);
		NoiseSource.SetPersistence(m_AltitudeParams.Persistence);
		NoiseSource.SetSeed(m_Seed);

		MapBuilder.SetSourceModule(NoiseSource);
		MapBuilder.SetDestNoiseMap(m_AltitudeMap.GetMapData());
		MapBuilder.SetDestSize(m_Width, m_Height);
		MapBuilder.SetBounds(m_AltitudeParams.X1, m_AltitudeParams.X2, m_AltitudeParams.Y1, m_AltitudeParams.Y2);
		MapBuilder.Build();

		m_AltitudeMap.SetBorderValue(-1);

		CreateDefaultClimates();
		CreateSimpleTerrainGradient(TERRAIN_GRADIENT);
	}


	/*
	 * Computes the East-West wind direction and normalized speed. Returns a value from
	 * -1.0 (West full speed) to 1.0 (East full speed).
	 */
	float CLandscape::ComputeWindDirection(const size_t X, const size_t Y)
	{
		float WindDir;

		float Y1;

		Y1 = 90.0f - ((float) Y / (float) (m_Height - 1)) * 180.0f;

		if (fabs(Y1) < m_WindParams.TradeWindLongitude)
		{
						//fabs(Y1) * 3.1415926535f / 180.0f * 90.0f / m_WindParams.TradeWindLongitude
			WindDir = -cos(fabs(Y1) * 1.57079632675f / m_WindParams.TradeWindLongitude);
		}
		else
		{				//(fabs(Y1) - m_WindParams.TradeWindLongitude) * 180.0f / m_WindParams.TradeWindLongitude * 3.1415926535f / 180.0f
			WindDir = sin((fabs(Y1) / m_WindParams.TradeWindLongitude - 1.0f) * 3.1415926535f );
		}

		return WindDir;
	}


	void CLandscape::CreateWind(const wind_params_t Params)
	{
		m_WindParams = Params;
		m_WindMap.SetSize(m_Width, m_Height);

		for (size_t Y = 0; Y < m_Height; ++Y)
		{
			for (size_t X = 0; X < m_Width; ++X)
			{
				m_WindMap.SetValue(X, Y, ComputeWindDirection(X,Y));
			}
		}
	}


	float CLandscape::ComputeTemperature_Norm(const float Y, const float Z)
	{
		float T = 0;
		float Item;
		int   Order = 0;
		int	  OrderY = 0;
		int	  j;

		for (int i = 0; i < NUM_TEMPERATURE_COEF; ++i)
		{
			Item = m_TemperatureParams.Coef[i];

			for (j = 0; j < OrderY; ++j)
			{
				Item *= Y;
			}

			for (j = 0; j < Order - OrderY; ++j)
			{
				Item *= Z;
			}

			if (OrderY == 0)
			{
				++Order;
				OrderY = Order;
			}
			else
			{
				--OrderY;
			}

			T += Item;
		}

		if (T > m_TemperatureParams.Maximum) T = m_TemperatureParams.Maximum;
		if (T < m_TemperatureParams.Minimum) T = m_TemperatureParams.Minimum;

		return T;
	}


	float CLandscape::ComputeTemperature(const size_t X, const size_t Y)
	{
		float Z, Z1;
		float Y1;

		Z = m_AltitudeMap.GetValue(X, Y);

		if (Z < m_AltitudeParams.SeaLevel)
			Z = m_AltitudeParams.SeaLevel;
		else if (Z > 1.0f)
			Z = 1.0f;

		if (m_AltitudeParams.SeaLevel < 1.0f)
			Z1 = (Z - m_AltitudeParams.SeaLevel) / (1.0f - m_AltitudeParams.SeaLevel);
		else
			Z1 = 1.0f;

		Z1 *= Z1;

		Y1 = (1.0f - (float) Y * 2.0f / (float) m_Height);
		if (Y1 < 0) Y1 = -Y1;

		return ComputeTemperature_Norm(Y1, Z1);
	}


	void CLandscape::CreateTemperature(const temperature_params_t Params)
	{
		m_TemperatureParams = Params;
		m_TemperatureMap.SetSize(m_Width, m_Height);

		for (size_t Y = 0; Y < m_Height; ++Y)
		{
			for (size_t X = 0; X < m_Width; ++X)
			{
				m_TemperatureMap.SetValue(X, Y, ComputeTemperature(X, Y));
			}
		}

		m_TemperatureMap.Blur(m_TemperatureParams.Blur);
		BlurTemperatureWind();
	}


	float CLandscape::ComputeLongitude(const size_t Y)
	{
		if (m_Height <= 1 || Y >= m_Height) return (0);
		float Longitude;

		Longitude = 90.0f - Y / m_Height * 180.0f;

		return Longitude;
	}


	climate_info_t* CLandscape::FindClimate(const size_t x, const size_t y)
	{
		CClimateArray::iterator i;
		float Temperature;
		float Altitude;
		float Rainfall;
		float Longitude;

		if (m_Width <= 0 || m_Height <= 0) return (NULL);
		if (x >= m_Width || y >= m_Height) return (NULL);

		Altitude = m_AltitudeMap.GetValue(x, y);
		Temperature = m_TemperatureMap.GetValue(x, y);
		Rainfall = m_RainfallMap.GetValue(x,y);
		Longitude = ComputeLongitude(y);

		for (i = m_Climates.begin(); i != m_Climates.end(); ++i)
		{
			if (i->IsSet(CLIMMASK_MATCHALL)) return &*i;

			if (i->IsSet(CLIMMASK_MIN_TEMPERATURE) && Temperature < i->MinTemperature) continue;
			if (i->IsSet(CLIMMASK_MAX_TEMPERATURE) && Temperature > i->MaxTemperature) continue;
			if (i->IsSet(CLIMMASK_MIN_RAINFALL) && Rainfall < i->MinRainfall) continue;
			if (i->IsSet(CLIMMASK_MAX_RAINFALL) && Rainfall > i->MaxRainfall) continue;
			if (i->IsSet(CLIMMASK_MIN_ALTITUDE) && Altitude < i->MinAltitude) continue;
			if (i->IsSet(CLIMMASK_MAX_ALTITUDE) && Altitude > i->MaxAltitude) continue;
			if (i->IsSet(CLIMMASK_MIN_LONGITUDE) && Longitude < i->MinLongitude) continue;
			if (i->IsSet(CLIMMASK_MAX_LONGITUDE) && Longitude > i->MaxLongitude) continue;

			return &*i;
		}

		return (NULL);
	}


	void CLandscape::CreateClimate(const climate_params_t Params)
	{
		size_t x, y;
		climate_info_t* pClimate;

		m_ClimateMap.SetSize(m_Width, m_Height);
		m_ClimateParams = Params;

		for (y = 0; y < m_Height; ++y)
		{
			for (x = 0; x < m_Width; ++x)
			{
				pClimate = FindClimate(x, y);

				if (pClimate)
				{
					m_ClimateMap.SetValue(x, y, (float) pClimate->Type);
					++pClimate->PixelCount;
				}
				else
				{
					m_ClimateMap.SetValue(x, y, 0);
				}
			}
		}

	}


	void CLandscape::SetSeed(const int Seed)
	{
		m_Seed = Seed;
	}


	void CLandscape::SetSize(const size_t Width, const size_t Height)
	{
		m_Width  = Width;
		m_Height = Height;

		m_AltitudeMap.SetSize(Width, Height);
		m_TemperatureMap.SetSize(Width, Height);
		m_RainfallMap.SetSize(Width, Height);
	}


	void CLandscape::AddClimate(const climate_info_t Climate)
	{
		m_Climates.push_back(Climate);
	}


	void CLandscape::ClearClimates(void)
	{
		m_Climates.clear();
	}


	void CLandscape::CreateDefaultClimates(void)
	{
		climate_info_t Climate;

		ClearClimates();

#if defined(__Linux__) || defined(__APPLE__)
		Climate.Name = "Ocean";
#else
		Climate.Name = _T("Ocean");
#endif
		Climate.Mask = CLIMMASK_MAX_ALTITUDE;
		Climate.Type = 0;
		Climate.Color = CColor(0,0,192,255);
		Climate.TrueColor = CColor(32,64,128);
		Climate.MaxAltitude = m_AltitudeParams.SeaLevel;
		AddClimate(Climate);

#if defined(__Linux__) || defined(__APPLE__)
		Climate.Name = "Tundra";
#else
		Climate.Name = _T("Tundra");
#endif
		++Climate.Type;
		Climate.Mask = CLIMMASK_MAX_TEMPERATURE | CLIMMASK_MAX_RAINFALL;
		Climate.Color = CColor(128,128,128,255);
		Climate.TrueColor = CColor(210,210,230,255);
		Climate.MaxTemperature = -5;
		Climate.MaxRainfall = 20;
		AddClimate(Climate);

#if defined(__Linux__) || defined(__APPLE__)
		Climate.Name = "Artic";
#else
		Climate.Name = _T("Artic");
#endif
		++Climate.Type;
		Climate.Mask = CLIMMASK_MAX_TEMPERATURE;
		Climate.Color = CColor(220,220,255,255);
		Climate.TrueColor = CColor(180,180,210,255);
		Climate.MaxTemperature = 0;
		AddClimate(Climate);

#if defined(__Linux__) || defined(__APPLE__)
		Climate.Name = "Desert";
#else
		Climate.Name = _T("Desert");
#endif
		++Climate.Type;
		Climate.Mask = CLIMMASK_MAX_RAINFALL;
		Climate.Color = CColor(220,220,190,255);
		Climate.TrueColor = CColor(192,192,128,255);
		Climate.MaxRainfall = 5;
		AddClimate(Climate);

#if defined(__Linux__) || defined(__APPLE__)
		Climate.Name = "Rain Forest";
#else
		Climate.Name = _T("Rain Forest");
#endif
		++Climate.Type;
		Climate.Mask = CLIMMASK_MIN_RAINFALL | CLIMMASK_MIN_TEMPERATURE;
		Climate.Color = CColor(0,128,0,255);
		Climate.TrueColor = CColor(0,128,0,255);
		Climate.MinRainfall = 60;
		Climate.MinTemperature = 20;
		AddClimate(Climate);

#if defined(__Linux__) || defined(__APPLE__)
		Climate.Name = "Wetland";
#else
		Climate.Name = _T("Wetland");
#endif
		++Climate.Type;
		Climate.Mask = CLIMMASK_MIN_RAINFALL | CLIMMASK_MIN_TEMPERATURE;
		Climate.Color = CColor(0,170,170,255);
		Climate.TrueColor = CColor(0,170,0,255);
		Climate.MinRainfall = 60;
		Climate.MinTemperature = 0;
		AddClimate(Climate);

#if defined(__Linux__) || defined(__APPLE__)
		Climate.Name = "Savanna";
#else
		Climate.Name = _T("Savanna");
#endif
		++Climate.Type;
		Climate.Mask = CLIMMASK_MAX_RAINFALL | CLIMMASK_MIN_TEMPERATURE;
		Climate.Color = CColor(255,128,0,255);
		Climate.TrueColor = CColor(120,120,70,255);
		Climate.MaxRainfall = 20;
		Climate.MinTemperature = 0;
		AddClimate(Climate);

#if defined(__Linux__) || defined(__APPLE__)
		Climate.Name = "Warm Temperate";
#else
		Climate.Name = _T("Warm Temperate");
#endif
		++Climate.Type;
		Climate.Mask = CLIMMASK_MIN_RAINFALL | CLIMMASK_MAX_RAINFALL | CLIMMASK_MIN_TEMPERATURE;
		Climate.Color = CColor(0,255,0,255);
		Climate.TrueColor = CColor(20,170,30,255);
		Climate.MinRainfall = 0;
		Climate.MaxRainfall = 70;
		Climate.MinTemperature = 10;
		AddClimate(Climate);

#if defined(__Linux__) || defined(__APPLE__)
		Climate.Name = "Cold Temperate";
#else
		Climate.Name = _T("Cold Temperate");
#endif
		++Climate.Type;
		Climate.Mask = CLIMMASK_MIN_RAINFALL | CLIMMASK_MAX_RAINFALL | CLIMMASK_MAX_TEMPERATURE;
		Climate.Color = CColor(128,0,255,255);
		Climate.TrueColor = CColor(20,120,10,255);
		Climate.MinRainfall = 0;
		Climate.MaxRainfall = 70;
		Climate.MaxTemperature = 10;
		AddClimate(Climate);

		/* Should be the last one */
#if defined(__Linux__) || defined(__APPLE__)
		Climate.Name = "Undefined";
#else
		Climate.Name = _T("Undefined");
#endif
		++Climate.Type;
		Climate.Mask = CLIMMASK_MATCHALL;
		Climate.Color = CColor(0,0,0,255);
		Climate.TrueColor = CColor(20,120,10,255);
		AddClimate(Climate);

		CreateClimateGradient(CLIMATE_GRADIENT);
	}


	void CLandscape::OutputClimateCounts(void)
	{
		CClimateArray::iterator i;

		for (i = m_Climates.begin(); i != m_Climates.end(); ++i)
		{
			float Percent =  (float) i->PixelCount / (float) m_Width / m_Height * 100.0f;
			// TODO FIXME
			//davelib::Log(_T("Climate '%s' = %d pixels (%.1f%%)"), i->Name.c_str(), i->PixelCount, Percent);
		}

	}


	void CLandscape::CreateWaterMap (void)
	{
		size_t x, y;

		m_WaterMap.SetSize(m_Width, m_Height);

		for (y = 0; y < m_Height; ++y)
		{
			for (x = 0; x < m_Width; ++x)
			{
				if (m_AltitudeMap.GetValue(x, y) <= m_AltitudeParams.SeaLevel)
					m_WaterMap.SetValue(x, y, 1);
				else
					m_WaterMap.SetValue(x, y, 0);
			}
		}

	}


	void CLandscape::OutputModifiedTerrainBMP (const TCHAR* pFilename)
	{
		//std::string				NewFilename = ConvertStringFromWide(pFilename);
		std::string				NewFilename(pFilename);
		utils::Image			OutputImage;
		utils::RendererImage	Renderer;
		utils::WriterBMP		BMPWriter;
		CGradient				Test;
		CGradient**				ppGradients;
		int						NumClimates = m_Climates.size();

		if (m_Width <= 0 || m_Height <= 0) return;
		if (NumClimates <= 0) return;
		OutputImage.SetSize(m_Width, m_Height);

		ppGradients = new CGradient*[NumClimates + 2];
		memset(ppGradients, 0, sizeof(ppGradients));

		for (CClimateArray::iterator i = m_Climates.begin(); i != m_Climates.end(); ++i)
		{
			i->MakeGradient();

			if (i->Type < NumClimates)
			{
				ppGradients[i->Type] = &i->Gradient;
			}
		}

		for (size_t y = 0; y < m_Height; ++y)
		{
			for (size_t x = 0; x < m_Width; ++x)
			{
				float Z = m_AltitudeMap.GetValue(x, y);
				int   Climate = (int) m_ClimateMap.GetValue(x, y);

				if (Climate >= 0 && Climate < NumClimates)
				{

					OutputImage.SetValue(x, y, *ppGradients[Climate]->GetColor(Z).m_pColor);
				}
				else
				{
					OutputImage.SetValue(x, y, *TERRAIN_GRADIENT.GetColor(Z).m_pColor);
				}
			}
		}

		BMPWriter.SetSourceImage(OutputImage);
		BMPWriter.SetDestFilename(NewFilename);
		BMPWriter.WriteDestFile();

		delete [] ppGradients;
	}


	void CLandscape::ErodeStart(const size_t X, const size_t Y)
	{
		bool Result;
		int X1 = X;
		int Y1 = Y;

		while (true)
		{
			ErodePoint(X1, Y1);

			Result = m_AltitudeMap.FindMinimumNeighbour(X1, Y1);
			if (!Result) break;
		}
	}


	void CLandscape::ErodePoint(const size_t X, const size_t Y)
	{
		m_ErosionMap.ModifyValue(X, Y, -m_ErosionParams.ErodeDelta);
	}


	void CLandscape::CreateErosion(const erosion_params_t Params)
	{
		size_t x, y;

		m_ErosionParams = Params;
		m_ErosionMap.SetSize(m_Width, m_Height);

		for (y = 0; y < m_Height; ++y)
		{
			for (x = 0; x < m_Width; ++x)
			{
				ErodeStart(x, y);
			}
		}

		m_ErosionMap.NormalizeMin(-1);
		m_ErosionMap.Blur(m_ErosionParams.Blur);
		m_ErosionMap.Blur(m_ErosionParams.Blur2);
	}


	void CLandscape::CreateCombinedMap(void)
	{
		size_t x, y;
		float Value;

		m_CombinedMap.SetSize(m_Width, m_Height);

		for (y = 0; y < m_Height; ++y)
		{
			for (x = 0; x < m_Width; ++x)
			{
				Value = m_AltitudeMap.GetValue(x, y) + m_ErosionMap.GetValue(x, y) * m_ErosionParams.CombineFactor;
				m_CombinedMap.SetValue(x, y, Value);
			}
		}
	}


};
