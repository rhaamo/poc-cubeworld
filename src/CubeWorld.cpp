/*
-----------------------------------------------------------------------------
Filename:    CubeWorld.cpp
-----------------------------------------------------------------------------
*/
#include "CubeWorld.h"

//-------------------------------------------------------------------------------------
CubeWorld::CubeWorld(void)
{
	m_Blocks = new block_t[WORLD_SIZE * WORLD_SIZE * WORLD_SIZE + 16000];
        memset(m_Blocks, 0, sizeof(block_t) * WORLD_SIZE * WORLD_SIZE * WORLD_SIZE);
	initWorldBlocksLayers();
	initWorldBlocksCaves();
	m_ChunkID = 1;

	m_BlockLight = new blocklight_t[WORLD_SIZE3 + 16000];
	memset(m_BlockLight, 255, sizeof(blocklight_t) * WORLD_SIZE3);
	initWorldBlocksLight();

	// Noon light levels to start with
	m_LightAngle = 90; //10;
	m_LightColor = Ogre::ColourValue(1, 1, 1); // 1, 0.6, 0.04
	m_FogColor = m_LightColor * 0.8f;
	m_AmbientColor = m_LightColor / 3.0f;

}
//-------------------------------------------------------------------------------------
CubeWorld::~CubeWorld(void)
{
	delete [] m_Blocks;
	delete [] m_BlockLight;
}

//-------------------------------------------------------------------------------------
void CubeWorld::createScene(void)
{
	createTexture("TerrainImage", "minecraft/terrain.png");
	createWaterTexture("WaterTest");
	createWorldChunks();

	createSkyTexture("SkyDome1");
	mSceneMgr->setSkyDome(true, "SkyDome1", 2, 8, 100);

	// Temporary deactivated
	// mSceneMgr->setFog(Ogre::FOG_LINEAR, Ogre::ColourValue(0.8, 0.8, 1), 0.05, 0.0, 200);
	// mCamera->setFarClipDistance(256);
	// mCamera->setNearClipDistance(0.01);

	mSceneMgr->setAmbientLight(Ogre::ColourValue(0.02, 0.02, 0.02));

}

Ogre::ManualObject* CubeWorld::createCubeMesh (Ogre::String name, Ogre::String matName)
{
   Ogre::ManualObject* cube = new Ogre::ManualObject(name);
   cube->begin(matName);

   cube->position(0.5f,-0.5f,1.0f);cube->normal(0.408248f,-0.816497f,0.408248f);cube->textureCoord(1,0);
   cube->position(-0.5f,-0.5f,0.0f);cube->normal(-0.408248f,-0.816497f,-0.408248f);cube->textureCoord(0,1);
   cube->position(0.5f,-0.5f,0.0f);cube->normal(0.666667f,-0.333333f,-0.666667f);cube->textureCoord(1,1);
   cube->position(-0.5f,-0.5f,1.0f);cube->normal(-0.666667f,-0.333333f,0.666667f);cube->textureCoord(0,0);
   cube->position(0.5f,0.5f,1.0f);cube->normal(0.666667f,0.333333f,0.666667f);cube->textureCoord(1,0);
   cube->position(-0.5,-0.5,1.0);cube->normal(-0.666667f,-0.333333f,0.666667f);cube->textureCoord(0,1);
   cube->position(0.5,-0.5,1.0);cube->normal(0.408248,-0.816497,0.408248f);cube->textureCoord(1,1);
   cube->position(-0.5,0.5,1.0);cube->normal(-0.408248,0.816497,0.408248);cube->textureCoord(0,0);
   cube->position(-0.5,0.5,0.0);cube->normal(-0.666667,0.333333,-0.666667);cube->textureCoord(0,1);
   cube->position(-0.5,-0.5,0.0);cube->normal(-0.408248,-0.816497,-0.408248);cube->textureCoord(1,1);
   cube->position(-0.5,-0.5,1.0);cube->normal(-0.666667,-0.333333,0.666667);cube->textureCoord(1,0);
   cube->position(0.5,-0.5,0.0);cube->normal(0.666667,-0.333333,-0.666667);cube->textureCoord(0,1);
   cube->position(0.5,0.5,0.0);cube->normal(0.408248,0.816497,-0.408248);cube->textureCoord(1,1);
   cube->position(0.5,-0.5,1.0);cube->normal(0.408248,-0.816497,0.408248);cube->textureCoord(0,0);
   cube->position(0.5,-0.5,0.0);cube->normal(0.666667,-0.333333,-0.666667);cube->textureCoord(1,0);
   cube->position(-0.5,-0.5,0.0);cube->normal(-0.408248,-0.816497,-0.408248);cube->textureCoord(0,0);
   cube->position(-0.5,0.5,1.0);cube->normal(-0.408248,0.816497,0.408248);cube->textureCoord(1,0);
   cube->position(0.5,0.5,0.0);cube->normal(0.408248,0.816497,-0.408248);cube->textureCoord(0,1);
   cube->position(-0.5,0.5,0.0);cube->normal(-0.666667,0.333333,-0.666667);cube->textureCoord(1,1);
   cube->position(0.5,0.5,1.0);cube->normal(0.666667,0.333333,0.666667);cube->textureCoord(0,0);

   cube->triangle(0,1,2);      cube->triangle(3,1,0);
   cube->triangle(4,5,6);      cube->triangle(4,7,5);
   cube->triangle(8,9,10);      cube->triangle(10,7,8);
   cube->triangle(4,11,12);   cube->triangle(4,13,11);
   cube->triangle(14,8,12);   cube->triangle(14,15,8);
   cube->triangle(16,17,18);   cube->triangle(16,19,17);
   cube->end();

   return cube;
}

void CubeWorld::initWorldBlocksSphere (void)
{
	for (int z = 0; z < WORLD_SIZE; ++z)
	{
		for (int y = 0; y < WORLD_SIZE; ++y)
		{
			for (int x = 0; x < WORLD_SIZE; ++x)
			{
				if (sqrt((float) (x-WORLD_SIZE/2)*(x-WORLD_SIZE/2) + (y-WORLD_SIZE/2)*(y-WORLD_SIZE/2) + (z-WORLD_SIZE/2)*(z-WORLD_SIZE/2)) < WORLD_SIZE/2) GetBlock(x,y,z) = 1;
			}
		}
	}
}

void CubeWorld::initWorldBlocksRandom (const int Divisor)
{
	srand(12345); // To keep it consistent between runs

	for (int z = 0; z < WORLD_SIZE; ++z)
	{
		for (int y = 0; y < WORLD_SIZE; ++y)
		{
			for (int x = 0; x < WORLD_SIZE; ++x)
			{
				GetBlock(x,y,z) = rand() % Divisor;
			}
		}
	}

}

void CubeWorld::initWorldBlocksLand()
{
	m_Landscape.SetSize(WORLD_SIZE, WORLD_SIZE);
	m_Landscape.CreateAltitude();

	infland::CMap& Map = m_Landscape.GetAltitudeMap();

	for (int z = 0; z < WORLD_SIZE; ++z)
	{
		for (int x = 0; x < WORLD_SIZE; ++x)
		{
			float Height = Map.GetValue(x, z) * WORLD_SIZE/4 + WORLD_SIZE/2;

			for (int y = 0; y < Height; ++y)
			{
				GetBlock(x, y, z) = 1;
			}

		}
	}

}

void CubeWorld::initWorldBlocksLandRnd()
{
	m_Landscape.SetSize(WORLD_SIZE, WORLD_SIZE);
	m_Landscape.CreateAltitude();
	infland::CMap& Map = m_Landscape.GetAltitudeMap();

	for (int z = 0; z < WORLD_SIZE; ++z)
	{
		for (int x = 0; x < WORLD_SIZE; ++x)
		{
			float Height = Map.GetValue(x, z) * WORLD_SIZE/4 + WORLD_SIZE/2;

			for (int y = 0; y < Height; ++y)
			{
				GetBlock(x, y, z) = (rand() % 4) + 1;  //1-4
			}
		}
	}
}

void CubeWorld::initWorldBlocksCaves()
{
	noise::module::RidgedMulti NoiseSource;
	float nx, ny, nz;
	int x, y, z;

	// Change these to increase/decrease the cave size scale
	float delta = 0.01f;
        float ValueLimit = 0.80f;

	// Initialize the noise module
	NoiseSource.SetSeed(0);
	NoiseSource.SetOctaveCount(4);

	for (y = 0, ny = 0; y < WORLD_SIZE; ++y, ny += delta)
	{
		for (z = 0, nz = 0; z < WORLD_SIZE; ++z, nz += delta)
		{
			for (x = 0, nx = 0; x < WORLD_SIZE; ++x, nx += delta)
			{
				float Value = NoiseSource.GetValue(nx, ny, nz);
				if (Value > ValueLimit ) GetBlock(x,y,z) = 0;
			}
		}
	}
}

void CubeWorld::initWorldBlocksLight()
{
	int x, y, z;
	blocklight_t Light;
	blocklight_t DeltaLight = 16;

	for (z = 0; z < WORLD_SIZE; ++z)
	{
		for (x = 0; x < WORLD_SIZE; ++x)
		{
			Light = 255;

			for (y = WORLD_SIZE - 1; y >= 0; --y)
			{
				GetBlockLight(x, y, z) = Light;

				if (GetBlock(x, y, z) != 0)
				{
					if (Light >= DeltaLight)
						Light -= DeltaLight;
					else
						Light = 0;
				}

			}
		}
	}

}

void CubeWorld::displaySimpleWorld (void)
{
	Ogre::MaterialPtr mat = Ogre::MaterialManager::getSingleton().create("BoxColor", "General", true );
	Ogre::Technique* tech = mat->getTechnique(0);
	Ogre::Pass* pass = tech->getPass(0);
	Ogre::TextureUnitState* tex = pass->createTextureUnitState();
	tex->setColourOperationEx(Ogre::LBX_MODULATE, Ogre::LBS_MANUAL, Ogre::LBS_CURRENT, Ogre::ColourValue(0, 0.5, 0));

	Ogre::ManualObject* testBox  = createCubeMesh("TestBox1", "BoxColor");
        Ogre::SceneNode* headNode = mSceneMgr->getRootSceneNode()->createChildSceneNode();
	Ogre::MeshPtr Mesh = testBox->convertToMesh("TestBox2");
	Ogre::StaticGeometry* pGeom = new Ogre::StaticGeometry (mSceneMgr, "Boxes");

	Ogre::Entity* pEnt = mSceneMgr->createEntity("TestBox2");

	pGeom->setRegionDimensions(Ogre::Vector3(300, 300, 300));

	for (int z = 0; z < WORLD_SIZE; ++z)
	{
		for (int y = 0; y < WORLD_SIZE; ++y)
		{
			for (int x = 0; x < WORLD_SIZE; ++x)
			{
				if (GetBlock(x,y,z)) pGeom->addEntity(pEnt, Ogre::Vector3(x,y,z));
			}
		}
	}

	pGeom->build ();
}

void CubeWorld::initWorldBlocksLayers()
{
	int HeightMap[WORLD_SIZE][WORLD_SIZE];
	infland::CLandscape LayerMaps[10];  //Hard coded because I'm lazy!
	int	NumLayerMaps = 0;
	int BaseSeed;

	m_Landscape.SetSize(WORLD_SIZE, WORLD_SIZE);
	m_Landscape.CreateAltitude();
	BaseSeed = m_Landscape.GetSeed();

	infland::CMap& Map = m_Landscape.GetAltitudeMap();

              //Initialize our temporary height map array
	for (int z = 0; z < WORLD_SIZE; ++z)
	{
		for (int x = 0; x < WORLD_SIZE; ++x)
		{
			float Height = Map.GetValue(x, z) * WORLD_SIZE/4 + WORLD_SIZE/2;
			HeightMap[x][z] = Height;
		}
	}

              //Create height maps for each layer
	for (int i = 0; g_Layers[i].BlockID != BLOCK_NULL && i < 10; ++i)
	{
		LayerMaps[i].SetSeed(BaseSeed + g_Layers[i].SeedOffset);
		LayerMaps[i].SetSize(WORLD_SIZE, WORLD_SIZE);
		LayerMaps[i].CreateAltitude();
		++NumLayerMaps;
	}

	int Layer = 0;

               //Fill in the blocks from all layers
	for (int layer = 0; layer < NumLayerMaps; ++layer)
	{
		infland::CMap& Map = LayerMaps[layer].GetAltitudeMap();

		for (int z = 0; z < WORLD_SIZE; ++z)
		{
			for (int x = 0; x < WORLD_SIZE; ++x)
			{
				if (HeightMap[x][z] <= 0) continue;
				int Height = (Map.GetValue(x, z) + 1)/2.0f*(g_Layers[layer].MaxLevel - g_Layers[layer].MinLevel) + g_Layers[layer].MinLevel;

                                      //Don't fill the map below 0 height
				if (HeightMap[x][z] - Height < 0) Height = HeightMap[x][z];
				HeightMap[x][z] -= Height;

				int MaxHeight = HeightMap[x][z] + Height;

				for (int y = HeightMap[x][z]; y <= MaxHeight; ++y)
				{
					GetBlock(x, y, z) = g_Layers[layer].BlockID;
				}

			}
		}
	}

}

void CubeWorld::createChunkWFaces (const int StartX, const int StartY, const int StartZ)
{
	Ogre::ManualObject* MeshChunk = new Ogre::ManualObject("MeshManChunk" + Ogre::StringConverter::toString(m_ChunkID));
	MeshChunk->begin("TerrainImage");

	int iVertex = 0;
	block_t Block;
	block_t Block1;
	float V1, V2;


	for (int z = StartZ; z < CHUNK_SIZE + StartZ; ++z)
	{
		for (int y = StartY; y < CHUNK_SIZE + StartY; ++y)
		{
			for (int x = StartX; x < CHUNK_SIZE + StartX; ++x)
			{
				Block = GetBlock(x,y,z);
				if (Block == 0) continue;

					//x-1
				Block1 = 0;
				if (x > StartX) Block1 = GetBlock(x-1,y,z);

				//Compute the block's texture coordinates
				V1 = 0.25f * (float)(Block - 1);
				V2 = V1 + 0.25f;

                                if (Block1 == 0)
				{
					MeshChunk->position(x, y,   z+1);	MeshChunk->normal(-1,0,0);	MeshChunk->textureCoord(0, V2);
					MeshChunk->position(x, y+1, z+1);	MeshChunk->normal(-1,0,0);	MeshChunk->textureCoord(1, V2);
					MeshChunk->position(x, y+1, z);		MeshChunk->normal(-1,0,0);	MeshChunk->textureCoord(1, V1);
					MeshChunk->position(x, y,   z);		MeshChunk->normal(-1,0,0);	MeshChunk->textureCoord(0, V1);

					MeshChunk->triangle(iVertex, iVertex+1, iVertex+2);
					MeshChunk->triangle(iVertex+2, iVertex+3, iVertex);

					iVertex += 4;
				}

					//x+1
				Block1 = 0;
				if (x < StartX + CHUNK_SIZE - 1) Block1 = GetBlock(x+1,y,z);

				if (Block1 == 0)
				{
					MeshChunk->position(x+1, y,   z);	MeshChunk->normal(1,0,0); MeshChunk->textureCoord(0, V2);
					MeshChunk->position(x+1, y+1, z);	MeshChunk->normal(1,0,0); MeshChunk->textureCoord(1, V2);
					MeshChunk->position(x+1, y+1, z+1);	MeshChunk->normal(1,0,0); MeshChunk->textureCoord(1, V1);
					MeshChunk->position(x+1, y,   z+1);	MeshChunk->normal(1,0,0); MeshChunk->textureCoord(0, V1);

					MeshChunk->triangle(iVertex, iVertex+1, iVertex+2);
					MeshChunk->triangle(iVertex+2, iVertex+3, iVertex);

					iVertex += 4;
				}

					//y-1
				Block1 = 0;
				if (y > StartY) Block1 = GetBlock(x,y-1,z);

				if (Block1 == 0)
				{
					MeshChunk->position(x,   y, z);		MeshChunk->normal(0,-1,0); MeshChunk->textureCoord(0, V2);
					MeshChunk->position(x+1, y, z);		MeshChunk->normal(0,-1,0); MeshChunk->textureCoord(1, V2);
					MeshChunk->position(x+1, y, z+1);	MeshChunk->normal(0,-1,0); MeshChunk->textureCoord(1, V1);
					MeshChunk->position(x,   y, z+1);	MeshChunk->normal(0,-1,0); MeshChunk->textureCoord(0, V1);

					MeshChunk->triangle(iVertex, iVertex+1, iVertex+2);
					MeshChunk->triangle(iVertex+2, iVertex+3, iVertex);

					iVertex += 4;
				}


					//y+1
				Block1 = 0;
				if (y < StartY + CHUNK_SIZE - 1) Block1 = GetBlock(x,y+1,z);

				if (Block1 == 0)
				{
					MeshChunk->position(x,   y+1, z+1);		MeshChunk->normal(0,1,0); MeshChunk->textureCoord(0, V2);
					MeshChunk->position(x+1, y+1, z+1);		MeshChunk->normal(0,1,0); MeshChunk->textureCoord(1, V2);
					MeshChunk->position(x+1, y+1, z);		MeshChunk->normal(0,1,0); MeshChunk->textureCoord(1, V1);
					MeshChunk->position(x,   y+1, z);		MeshChunk->normal(0,1,0); MeshChunk->textureCoord(0, V1);

					MeshChunk->triangle(iVertex, iVertex+1, iVertex+2);
					MeshChunk->triangle(iVertex+2, iVertex+3, iVertex);

					iVertex += 4;
				}

					//z-1
				Block1 = 0;
				if (z > StartZ) Block1 = GetBlock(x,y,z-1);

				if (Block1 == 0)
				{
					MeshChunk->position(x,   y+1, z);		MeshChunk->normal(0,0,-1); MeshChunk->textureCoord(0, V2);
					MeshChunk->position(x+1, y+1, z);		MeshChunk->normal(0,0,-1); MeshChunk->textureCoord(1, V2);
					MeshChunk->position(x+1, y,   z);		MeshChunk->normal(0,0,-1); MeshChunk->textureCoord(1, V1);
					MeshChunk->position(x,   y,   z);		MeshChunk->normal(0,0,-1); MeshChunk->textureCoord(0, V1);

					MeshChunk->triangle(iVertex, iVertex+1, iVertex+2);
					MeshChunk->triangle(iVertex+2, iVertex+3, iVertex);

					iVertex += 4;
				}


					//z+1
				Block1 = 0;
				if (z < StartZ + CHUNK_SIZE - 1) Block1 = GetBlock(x,y,z+1);

				if (Block1 == 0)
				{
					MeshChunk->position(x,   y,   z+1);		MeshChunk->normal(0,0,1); MeshChunk->textureCoord(0, V2);
					MeshChunk->position(x+1, y,   z+1);		MeshChunk->normal(0,0,1); MeshChunk->textureCoord(1, V2);
					MeshChunk->position(x+1, y+1, z+1);		MeshChunk->normal(0,0,1); MeshChunk->textureCoord(1, V1);
					MeshChunk->position(x,   y+1, z+1);		MeshChunk->normal(0,0,1); MeshChunk->textureCoord(0, V1);

					MeshChunk->triangle(iVertex, iVertex+1, iVertex+2);
					MeshChunk->triangle(iVertex+2, iVertex+3, iVertex);

					iVertex += 4;
				}
			}
		}
	}

	MeshChunk->end();
	mSceneMgr->getRootSceneNode()->createChildSceneNode()->attachObject(MeshChunk);

	++m_ChunkID;
}

void CubeWorld::createChunk (Ogre::ManualObject* MeshChunk, const int StartX, const int StartY, const int StartZ)
{
	block_t LastBlock = 0;

	//Ogre::ManualObject* MeshChunk = new Ogre::ManualObject("MeshMatChunk" + Ogre::StringConverter::toString(m_ChunkID));
	//MeshChunk->begin("TerrainImage");

	int iVertex = 0;
	block_t Block;
	block_t Block1;

	/* Only create visible faces of each chunk */
	block_t DefaultBlock = 1;
	int SX = 0;
	int SY = 0;
	int SZ = 0;
	int MaxSize = WORLD_SIZE;
	float V1, V2;
	float BlockLight;

	Ogre::ColourValue BlockColory1;    //Top face
	Ogre::ColourValue BlockColory2;    //Bottom face
	Ogre::ColourValue BlockColorx1;    //Sunset face
	Ogre::ColourValue BlockColorx2;    //Sunrise face
	Ogre::ColourValue BlockColorz;     //Front/back faces

	for (int z = StartZ; z < CHUNK_SIZE + StartZ; ++z)
	    {
		    for (int y = StartY; y < CHUNK_SIZE + StartY; ++y)
			{
				for (int x = StartX; x < CHUNK_SIZE + StartX; ++x)
				    {
					    Block = GetBlock(x,y,z);
					    if (Block == 0) continue; // Ignore air blocks
					    if (Block == 5) continue; // Ignore water blocks

					    //Compute the block's texture coordinates
					    V1 = 0.25f * (float)(Block - 1);
					    V2 = V1 + 0.25f;

					    BlockLight  = GetBlockLight(x, y, z) / 255.0f;  //For the two x faces
					    float BlockLight1 = BlockLight * 0.9f;		//For the two z faces
					    float BlockLight2 = BlockLight * 0.8f;		//For the two y faces
					    // std::cerr << "*** BlockLight: " << BlockLight << std::endl;
					    // std::cerr << "*** BlockLight1: " << BlockLight1 << std::endl;
					    // std::cerr << "*** BlockLight2: " << BlockLight2 << std::endl;

					    float BaseLight = GetBlockLight(x, y, z) / 255.0f;
					    float Factor;

					    if (m_LightAngle >= 0 && m_LightAngle <= 180)
						    Factor = sin(m_LightAngle * 3.1415926f / 180.0f);
					    else
						    Factor = 0;

					    if (Factor < 0.1f)
						    Factor = 0.1f;
					    BlockColory1 = m_LightColor * (Factor * BaseLight) + m_AmbientColor;
					    BlockColory1.saturate();
					    BlockColory2 = m_LightColor * (Factor / 2.0f * BaseLight) + m_AmbientColor;
					    BlockColory2.saturate();
					    BlockColorz  = m_LightColor * (Factor * 0.70f * BaseLight) + m_AmbientColor;
					    BlockColorz.saturate();
					    BlockColorz *= 0.80f;

					    if (m_LightAngle >= 315 || m_LightAngle <= 45)
						    Factor = fabs(cos(m_LightAngle * 3.1415926f / 180.0f));
					    else
						    Factor = fabs(sin(m_LightAngle * 3.1415926f / 180.0f));
					    if (Factor < 0.1f)
						    Factor = 0.1f;
					    BlockColorx1 = m_LightColor * (Factor * 0.80f * BaseLight) + m_AmbientColor;
					    BlockColorx1.saturate();
					    BlockColorx1 *= 0.95f;

					    if (m_LightAngle >= 135 && m_LightAngle <= 225)
						    Factor = fabs(cos(m_LightAngle * 3.1415926f / 180.0f));
					    else
						    Factor = fabs(sin(m_LightAngle * 3.1415926f / 180.0f));

					    if (Factor < 0.1f)
						    Factor = 0.1f;
					    BlockColorx2 = m_LightColor * (Factor * 0.80f * BaseLight) + m_AmbientColor;
					    BlockColorx2.saturate();
					    BlockColorx2 *= 0.95f;


					    Block1 = DefaultBlock;
					    if (x > SX)
						    Block1 = GetBlock(x-1,y,z);
					    //x-1
					    if (g_BlockInfo[Block1].Transparent)
						{
							MeshChunk->position(x, y,   z+1); MeshChunk->normal(-1,0,0);	MeshChunk->textureCoord(0, V2); MeshChunk->colour(BlockColorx1);
							MeshChunk->position(x, y+1, z+1); MeshChunk->normal(-1,0,0);	MeshChunk->textureCoord(1, V2); MeshChunk->colour(BlockColorx1);
							MeshChunk->position(x, y+1, z);	  MeshChunk->normal(-1,0,0);	MeshChunk->textureCoord(1, V1); MeshChunk->colour(BlockColorx1);
							MeshChunk->position(x, y,   z);	  MeshChunk->normal(-1,0,0);	MeshChunk->textureCoord(0, V1); MeshChunk->colour(BlockColorx1);

							MeshChunk->triangle(iVertex, iVertex+1, iVertex+2);
							MeshChunk->triangle(iVertex+2, iVertex+3, iVertex);

							iVertex += 4;
						}

					    //x+1
					    Block1 = DefaultBlock;
					    if (x < SX + MaxSize - 1)
						    Block1 = GetBlock(x+1,y,z);

					    if (g_BlockInfo[Block1].Transparent)
						{
							MeshChunk->position(x+1, y,   z);	MeshChunk->normal(1,0,0); MeshChunk->textureCoord(0, V2); MeshChunk->colour(BlockColorx2);
							MeshChunk->position(x+1, y+1, z);	MeshChunk->normal(1,0,0); MeshChunk->textureCoord(1, V2); MeshChunk->colour(BlockColorx2);
							MeshChunk->position(x+1, y+1, z+1);	MeshChunk->normal(1,0,0); MeshChunk->textureCoord(1, V1); MeshChunk->colour(BlockColorx2);
							MeshChunk->position(x+1, y,   z+1);	MeshChunk->normal(1,0,0); MeshChunk->textureCoord(0, V1); MeshChunk->colour(BlockColorx2);

							MeshChunk->triangle(iVertex, iVertex+1, iVertex+2);
							MeshChunk->triangle(iVertex+2, iVertex+3, iVertex);

							iVertex += 4;
						}

					    //y-1
					    Block1 = DefaultBlock;
					    if (y > SY)
						    Block1 = GetBlock(x,y-1,z);

					    if (g_BlockInfo[Block1].Transparent)
						{
							MeshChunk->position(x,   y, z);		MeshChunk->normal(0,-1,0); MeshChunk->textureCoord(0, V2); MeshChunk->colour(BlockColory1);
							MeshChunk->position(x+1, y, z);		MeshChunk->normal(0,-1,0); MeshChunk->textureCoord(1, V2); MeshChunk->colour(BlockColory1);
							MeshChunk->position(x+1, y, z+1);	MeshChunk->normal(0,-1,0); MeshChunk->textureCoord(1, V1); MeshChunk->colour(BlockColory1);
							MeshChunk->position(x,   y, z+1);	MeshChunk->normal(0,-1,0); MeshChunk->textureCoord(0, V1); MeshChunk->colour(BlockColory1);

							MeshChunk->triangle(iVertex, iVertex+1, iVertex+2);
							MeshChunk->triangle(iVertex+2, iVertex+3, iVertex);

							iVertex += 4;
						}


					    //y+1
					    Block1 = DefaultBlock;
					    if (y < SY + MaxSize - 1)
						    Block1 = GetBlock(x,y+1,z);

					    if (g_BlockInfo[Block1].Transparent)
						{
							MeshChunk->position(x,   y+1, z+1);		MeshChunk->normal(0,1,0); MeshChunk->textureCoord(0, V2); MeshChunk->colour(BlockColory2);
							MeshChunk->position(x+1, y+1, z+1);		MeshChunk->normal(0,1,0); MeshChunk->textureCoord(1, V2); MeshChunk->colour(BlockColory2);
							MeshChunk->position(x+1, y+1, z);		MeshChunk->normal(0,1,0); MeshChunk->textureCoord(1, V1); MeshChunk->colour(BlockColory2);
							MeshChunk->position(x,   y+1, z);		MeshChunk->normal(0,1,0); MeshChunk->textureCoord(0, V1); MeshChunk->colour(BlockColory2);

							MeshChunk->triangle(iVertex, iVertex+1, iVertex+2);
							MeshChunk->triangle(iVertex+2, iVertex+3, iVertex);

							iVertex += 4;
						}

					    //z-1
					    Block1 = DefaultBlock;
					    if (z > SZ)
						    Block1 = GetBlock(x,y,z-1);

					    if (g_BlockInfo[Block1].Transparent)
						{
							MeshChunk->position(x,   y+1, z);		MeshChunk->normal(0,0,-1); MeshChunk->textureCoord(0, V2); MeshChunk->colour(BlockColorz);
							MeshChunk->position(x+1, y+1, z);		MeshChunk->normal(0,0,-1); MeshChunk->textureCoord(1, V2); MeshChunk->colour(BlockColorz);
							MeshChunk->position(x+1, y,   z);		MeshChunk->normal(0,0,-1); MeshChunk->textureCoord(1, V1); MeshChunk->colour(BlockColorz);
							MeshChunk->position(x,   y,   z);		MeshChunk->normal(0,0,-1); MeshChunk->textureCoord(0, V1); MeshChunk->colour(BlockColorz);

							MeshChunk->triangle(iVertex, iVertex+1, iVertex+2);
							MeshChunk->triangle(iVertex+2, iVertex+3, iVertex);

							iVertex += 4;
						}


					    //z+1
					    Block1 = DefaultBlock;
					    if (z < SZ + MaxSize - 1)
						    Block1 = GetBlock(x,y,z+1);

					    if (g_BlockInfo[Block1].Transparent)
						{
							MeshChunk->position(x,   y,   z+1);		MeshChunk->normal(0,0,1); MeshChunk->textureCoord(0, V2); MeshChunk->colour(BlockColorz);
							MeshChunk->position(x+1, y,   z+1);		MeshChunk->normal(0,0,1); MeshChunk->textureCoord(1, V2); MeshChunk->colour(BlockColorz);
							MeshChunk->position(x+1, y+1, z+1);		MeshChunk->normal(0,0,1); MeshChunk->textureCoord(1, V1); MeshChunk->colour(BlockColorz);
							MeshChunk->position(x,   y+1, z+1);		MeshChunk->normal(0,0,1); MeshChunk->textureCoord(0, V1); MeshChunk->colour(BlockColorz);

							MeshChunk->triangle(iVertex, iVertex+1, iVertex+2);
							MeshChunk->triangle(iVertex+2, iVertex+3, iVertex);

							iVertex += 4;
						}



				    }
			}
	    }

	MeshChunk->end();
	mSceneMgr->getRootSceneNode()->createChildSceneNode()->attachObject(MeshChunk);
	++m_ChunkID;

}

void CubeWorld::createChunkWater (const int StartX, const int StartY, const int StartZ)
{
	block_t LastBlock = 0;

	int iVertex = 0;
	block_t Block;
	block_t Block1;

	/* Only create visible faces of chunk */
	block_t DefaultBlock = 1;
	int SX = 0;
	int SY = 0;
	int SZ = 0;
	int MaxSize = WORLD_SIZE;

	float BlockLight;
	float BlockLight1;
	float BlockLight2;

	float V1, V2;

	Ogre::ManualObject* MeshChunk = new Ogre::ManualObject("MeshWaterChunk" + Ogre::StringConverter::toString(m_ChunkID));
	MeshChunk->setDynamic(true);
	MeshChunk->begin("WaterTest");

	for (int z = StartZ; z < CHUNK_SIZE + StartZ; ++z)
	{
		for (int y = StartY; y < CHUNK_SIZE + StartY; ++y)
		{
			for (int x = StartX; x < CHUNK_SIZE + StartX; ++x)
			{
				Block = GetBlock(x,y,z);
				if (Block != 5) continue;   //Only create water meshes


				BlockLight  = GetBlockLight(x, y, z) / 255.0f;
				BlockLight1 = BlockLight * 0.9f;
				BlockLight2 = BlockLight * 0.8f;


				V1 = 1.0f/5.0f * (float)(Block - 1);
				V2 = V1 + 1.0f/5.0f;

				//x-1
				Block1 = DefaultBlock;
				if (x > SX) Block1 = GetBlock(x-1,y,z);

				if (Block1 != 5)
				{
                                        //create face of block
					MeshChunk->position(x, y,   z+1); MeshChunk->normal(-1,0,0);	MeshChunk->textureCoord(0, V2); MeshChunk->colour(BlockLight, BlockLight, BlockLight);
					MeshChunk->position(x, y+1, z+1); MeshChunk->normal(-1,0,0);	MeshChunk->textureCoord(1, V2); MeshChunk->colour(BlockLight, BlockLight, BlockLight);
					MeshChunk->position(x, y+1, z);	  MeshChunk->normal(-1,0,0);	MeshChunk->textureCoord(1, V1); MeshChunk->colour(BlockLight, BlockLight, BlockLight);
					MeshChunk->position(x, y,   z);	  MeshChunk->normal(-1,0,0);	MeshChunk->textureCoord(0, V1); MeshChunk->colour(BlockLight, BlockLight, BlockLight);

					MeshChunk->triangle(iVertex, iVertex+1, iVertex+2);
					MeshChunk->triangle(iVertex+2, iVertex+3, iVertex);

					iVertex += 4;
                                }

				//x+1
				Block1 = DefaultBlock;
				if (x < SX + MaxSize - 1)
					Block1 = GetBlock(x+1,y,z);

				if (Block1 != 5)
				    {
					    MeshChunk->position(x+1, y,   z);	MeshChunk->normal(1,0,0); MeshChunk->textureCoord(0, V2); MeshChunk->colour(BlockLight, BlockLight, BlockLight);
					    MeshChunk->position(x+1, y+1, z);	MeshChunk->normal(1,0,0); MeshChunk->textureCoord(1, V2); MeshChunk->colour(BlockLight, BlockLight, BlockLight);
					    MeshChunk->position(x+1, y+1, z+1);	MeshChunk->normal(1,0,0); MeshChunk->textureCoord(1, V1); MeshChunk->colour(BlockLight, BlockLight, BlockLight);
					    MeshChunk->position(x+1, y,   z+1);	MeshChunk->normal(1,0,0); MeshChunk->textureCoord(0, V1); MeshChunk->colour(BlockLight, BlockLight, BlockLight);

					    MeshChunk->triangle(iVertex, iVertex+1, iVertex+2);
					    MeshChunk->triangle(iVertex+2, iVertex+3, iVertex);

					    iVertex += 4;
				    }

				//y-1
				Block1 = DefaultBlock;
				if (y > SY)
					Block1 = GetBlock(x,y-1,z);

				if (Block1 != 5)
				    {
					    MeshChunk->position(x,   y, z);		MeshChunk->normal(0,-1,0); MeshChunk->textureCoord(0, V2); MeshChunk->colour(BlockLight2, BlockLight2, BlockLight2);
					    MeshChunk->position(x+1, y, z);		MeshChunk->normal(0,-1,0); MeshChunk->textureCoord(1, V2); MeshChunk->colour(BlockLight2, BlockLight2, BlockLight2);
					    MeshChunk->position(x+1, y, z+1);	MeshChunk->normal(0,-1,0); MeshChunk->textureCoord(1, V1); MeshChunk->colour(BlockLight2, BlockLight2, BlockLight2);
					    MeshChunk->position(x,   y, z+1);	MeshChunk->normal(0,-1,0); MeshChunk->textureCoord(0, V1); MeshChunk->colour(BlockLight2, BlockLight2, BlockLight2);

					    MeshChunk->triangle(iVertex, iVertex+1, iVertex+2);
					    MeshChunk->triangle(iVertex+2, iVertex+3, iVertex);

					    iVertex += 4;
				    }


				//y+1
				Block1 = DefaultBlock;
				if (y < SY + MaxSize - 1)
					Block1 = GetBlock(x,y+1,z);

				if (Block1 != 5)
				    {
					    MeshChunk->position(x,   y+1, z+1);		MeshChunk->normal(0,1,0); MeshChunk->textureCoord(0, V2); MeshChunk->colour(BlockLight2, BlockLight2, BlockLight2);
					    MeshChunk->position(x+1, y+1, z+1);		MeshChunk->normal(0,1,0); MeshChunk->textureCoord(1, V2); MeshChunk->colour(BlockLight2, BlockLight2, BlockLight2);
					    MeshChunk->position(x+1, y+1, z);		MeshChunk->normal(0,1,0); MeshChunk->textureCoord(1, V1); MeshChunk->colour(BlockLight2, BlockLight2, BlockLight2);
					    MeshChunk->position(x,   y+1, z);		MeshChunk->normal(0,1,0); MeshChunk->textureCoord(0, V1); MeshChunk->colour(BlockLight2, BlockLight2, BlockLight2);

					    MeshChunk->triangle(iVertex, iVertex+1, iVertex+2);
					    MeshChunk->triangle(iVertex+2, iVertex+3, iVertex);

					    iVertex += 4;
				    }

				//z-1
				Block1 = DefaultBlock;
				if (z > SZ)
					Block1 = GetBlock(x,y,z-1);

				if (Block1 != 5)
				    {
					    MeshChunk->position(x,   y+1, z);		MeshChunk->normal(0,0,-1); MeshChunk->textureCoord(0, V2); MeshChunk->colour(BlockLight1, BlockLight1, BlockLight1);
					    MeshChunk->position(x+1, y+1, z);		MeshChunk->normal(0,0,-1); MeshChunk->textureCoord(1, V2); MeshChunk->colour(BlockLight1, BlockLight1, BlockLight1);
					    MeshChunk->position(x+1, y,   z);		MeshChunk->normal(0,0,-1); MeshChunk->textureCoord(1, V1); MeshChunk->colour(BlockLight1, BlockLight1, BlockLight1);
					    MeshChunk->position(x,   y,   z);		MeshChunk->normal(0,0,-1); MeshChunk->textureCoord(0, V1); MeshChunk->colour(BlockLight1, BlockLight1, BlockLight1);

					    MeshChunk->triangle(iVertex, iVertex+1, iVertex+2);
					    MeshChunk->triangle(iVertex+2, iVertex+3, iVertex);

					    iVertex += 4;
				    }


				//z+1
				Block1 = DefaultBlock;
				if (z < SZ + MaxSize - 1)
					Block1 = GetBlock(x,y,z+1);

				if (Block1 != 5)
				    {
					    MeshChunk->position(x,   y,   z+1);		MeshChunk->normal(0,0,1); MeshChunk->textureCoord(0, V2); MeshChunk->colour(BlockLight1, BlockLight1, BlockLight1);
					    MeshChunk->position(x+1, y,   z+1);		MeshChunk->normal(0,0,1); MeshChunk->textureCoord(1, V2); MeshChunk->colour(BlockLight1, BlockLight1, BlockLight1);
					    MeshChunk->position(x+1, y+1, z+1);		MeshChunk->normal(0,0,1); MeshChunk->textureCoord(1, V1); MeshChunk->colour(BlockLight1, BlockLight1, BlockLight1);
					    MeshChunk->position(x,   y+1, z+1);		MeshChunk->normal(0,0,1); MeshChunk->textureCoord(0, V1); MeshChunk->colour(BlockLight1, BlockLight1, BlockLight1);

					    MeshChunk->triangle(iVertex, iVertex+1, iVertex+2);
					    MeshChunk->triangle(iVertex+2, iVertex+3, iVertex);

					    iVertex += 4;
				    }
			}
		}
	}
	MeshChunk->end();
	mSceneMgr->getRootSceneNode()->createChildSceneNode()->attachObject(MeshChunk);
	++m_ChunkID;

}


void CubeWorld::createWorldChunks (void)
{
	//std::vector<int> VertexArray;
	Ogre::MaterialPtr mat = Ogre::MaterialManager::getSingleton().create("BoxColor", "General", true );
	Ogre::Technique* tech = mat->getTechnique(0);
	Ogre::Pass* pass = tech->getPass(0);
	Ogre::TextureUnitState* tex = pass->createTextureUnitState();
	tex->setColourOperationEx(Ogre::LBX_MODULATE, Ogre::LBS_MANUAL, Ogre::LBS_CURRENT, Ogre::ColourValue(0, 0.5, 0));

	Ogre::ManualObject* MeshChunk = new Ogre::ManualObject("MeshMatChunk");
	MeshChunk->begin("TerrainImage");

	for (int z = 0; z < WORLD_SIZE; z += CHUNK_SIZE)
	{
		for (int y = 0; y < WORLD_SIZE; y += CHUNK_SIZE)
		{
			for (int x = 0; x < WORLD_SIZE; x += CHUNK_SIZE)
			{
				createChunk(MeshChunk, x,y,z); /* WFaces or not */
				//createChunkWater(x, y, z);
			}
		}
	}

}

void CubeWorld::createSolidTexture (const TCHAR* pName)
{
	Ogre::MaterialPtr mat = Ogre::MaterialManager::getSingleton().create("BoxColor", "General", true );
	Ogre::Technique* tech = mat->getTechnique(0);
	Ogre::Pass* pass = tech->getPass(0);
	Ogre::TextureUnitState* tex = pass->createTextureUnitState();
	tex->setColourOperationEx(Ogre::LBX_MODULATE, Ogre::LBS_MANUAL, Ogre::LBS_CURRENT, Ogre::ColourValue(0, 0.5, 0));
}

void CubeWorld::createTexture (const TCHAR* pName, const TCHAR* pImageFilename)
{
	Ogre::MaterialPtr mat = Ogre::MaterialManager::getSingleton().create(pName, "General", true );
	Ogre::Technique* tech = mat->getTechnique(0);
	Ogre::Pass* pass = tech->getPass(0);
	Ogre::TextureUnitState* tex = pass->createTextureUnitState();

	tex->setTextureName(pImageFilename);
	tex->setNumMipmaps(4);
	tex->setTextureAnisotropy(1);
	tex->setTextureFiltering(Ogre::FO_POINT, Ogre::FO_POINT, Ogre::FO_POINT);

	pass->setVertexColourTracking(Ogre::TVC_DIFFUSE);
	tex->setColourOperationEx(Ogre::LBX_MODULATE, Ogre::LBS_DIFFUSE, Ogre::LBS_TEXTURE);

	tech->setLightingEnabled(false);
}

void CubeWorld::createWaterTexture (const TCHAR* pName)
{
	Ogre::MaterialPtr mat = Ogre::MaterialManager::getSingleton().create(pName, "General", true );
	Ogre::Technique* tech = mat->getTechnique(0);
	Ogre::Pass* pass = tech->getPass(0);
	Ogre::TextureUnitState* tex = pass->createTextureUnitState();

	tech->setLightingEnabled(false);

	pass->setSceneBlending(Ogre::SBT_TRANSPARENT_ALPHA);
	pass->setDepthWriteEnabled(false);

	tex->setColourOperationEx(Ogre::LBX_SOURCE1, Ogre::LBS_MANUAL, Ogre::LBS_CURRENT, Ogre::ColourValue(0, 0, 1));
	tex->setAlphaOperation(Ogre::LBX_SOURCE1, Ogre::LBS_MANUAL, Ogre::LBS_CURRENT, 0.5);
}

void CubeWorld::createSkyTexture(const TCHAR* pName)
{
	Ogre::MaterialPtr mat = Ogre::MaterialManager::getSingleton().create(pName, "General", true);
	Ogre::Technique* tech = mat->getTechnique(0);
	Ogre::Pass* pass = tech->getPass(0);
	Ogre::TextureUnitState* tex = pass->createTextureUnitState();

	pass->setLightingEnabled(false);
	pass->setDepthCheckEnabled(false);
	pass->setDepthWriteEnabled(false);
	pass->setFog(true);

	tex->setTextureName("clouds.jpg");
	tex->setScrollAnimation(0.05, 0);

	// This is a new texture state to simulate lightning
	tex = pass->createTextureUnitState();
	tex->setColourOperationEx(Ogre::LBX_MODULATE, Ogre::LBS_MANUAL, Ogre::LBS_CURRENT, Ogre::ColourValue(1, 1, 1));

	m_SkyMaterial = mat;
	updateSkyTextureLight();
}

void CubeWorld::updateSkyTextureLight (void)
{
	if (m_SkyMaterial.isNull())
		return;

	Ogre::Technique* tech = m_SkyMaterial->getTechnique(0);
#if defined(__linux__) || defined(__APPLE__)
	if (tech == NULL) return;
#else
	if (tech == nullptr) return;

#endif
	Ogre::Pass* pass = tech->getPass(0);
#if defined(__linux__) || defined(__APPLE__)
	if (pass == NULL) return;
#else
	if (pass == nulptr) return;
#endif

	Ogre::TextureUnitState* tex = pass->getTextureUnitState(1);
#if defined(__linux__) || defined(__APPLE__)
	if (tex == NULL) return;
#else
	if (tex == nullptr) return;
#endif

	// Update the texture unit's color operation with the world light level
	tex->setColourOperationEx(Ogre::LBX_MODULATE, Ogre::LBS_MANUAL, Ogre::LBS_CURRENT, m_LightColor);
}





#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
#define WIN32_LEAN_AND_MEAN
#include "windows.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
    INT WINAPI WinMain( HINSTANCE hInst, HINSTANCE, LPSTR strCmdLine, INT )
#else
    int main(int argc, char *argv[])
#endif
    {
        // Create application object
        CubeWorld app;

        try {
            app.go();
        } catch( Ogre::Exception& e ) {
#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
            MessageBox( NULL, e.getFullDescription().c_str(), "An exception has occured!", MB_OK | MB_ICONERROR | MB_TASKMODAL);
#else
            std::cerr << "An exception has occured: " <<
                e.getFullDescription().c_str() << std::endl;
#endif
        }

        return 0;
    }

#ifdef __cplusplus
}
#endif
