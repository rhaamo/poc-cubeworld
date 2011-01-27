/*
-----------------------------------------------------------------------------
Filename:    CubeWorld.cpp
-----------------------------------------------------------------------------
*/

// Linux Compat for wind0ws
typedef char TCHAR;

#include "erosion.h"
#include "CubeWorld.h"

//-------------------------------------------------------------------------------------
CubeWorld::CubeWorld(void)
{
	m_Blocks = new block_t[WORLD_SIZE * WORLD_SIZE * WORLD_SIZE * 16000];
	memset(m_Blocks, 0, sizeof(block_t) * WORLD_SIZE * WORLD_SIZE * WORLD_SIZE);
	//initWorldBlocksSphere();
	initWorldBlocksLand();
	m_ChunkID = 1;

	m_BlockLight = new blocklight_t[WORLD_SIZE3 + 16000];
	memset(m_BlockLight, 255, sizeof(blocklight_t) * WORLD_SIZE3);
	//initWorldBlocksLight(); // WTF FIXME Crashes on linux ??! (not on OSX)
}
//-------------------------------------------------------------------------------------
CubeWorld::~CubeWorld(void)
{
	delete [] m_Blocks;
	delete [] m_BlockLight;
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



void CubeWorld::initWorldBlocksSphere(void)
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

void CubeWorld::initWorldBlocksRandom(const int Divisor)
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

void CubeWorld::createChunk (const int StartX, const int StartY, const int StartZ)
{
	block_t LastBlock = 0;

	Ogre::ManualObject* MeshChunk = new Ogre::ManualObject("MeshMatChunk" + Ogre::StringConverter::toString(m_ChunkID));
	MeshChunk->begin("Combined4");

	int iVertex = 0;
	block_t Block;
	block_t Block1;

	/* Only create visible faces of chunk */
	block_t DefaultBlock = 1;
	int SX = 0;
	int SY = 0;
	int SZ = 0;
	int MaxSize = WORLD_SIZE;

	float BlockLight, BlockLight1, BlockLight2;

	float V1, V2;

	for (int z = StartZ; z < CHUNK_SIZE + StartZ; ++z)
	    {
		    for (int y = StartY; y < CHUNK_SIZE + StartY; ++y)
			{
				for (int x = StartX; x < CHUNK_SIZE + StartX; ++x)
				    {
					    Block = GetBlock(x, y, z);

					    if (Block == 0) continue;
					    if (Block == 5) continue;

					    // Compute the block's texture coordinates
					    V1 = 0.25f * (float)(Block - 1);
					    V2 = V1 + 0.25f;

					    //x-1
					    Block1 = DefaultBlock;
					    if (x > SX)
						    Block1 = GetBlock(x-1, y, z);

					    BlockLight = GetBlockLight(x, y, z) / 255.0f; // x
					    BlockLight1 = BlockLight * 0.9f;              // z
					    BlockLight2 = BlockLight * 0.8f;              // y
					    //MeshChunk->colour(BlockLight, BlockLight1, BlockLight2); // Crash

					    if (Block1 == 0)
						{
							MeshChunk->position(x, y,   z+1); MeshChunk->normal(-1,0,0);	MeshChunk->textureCoord(0, V2);
							MeshChunk->position(x, y+1, z+1); MeshChunk->normal(-1,0,0);	MeshChunk->textureCoord(1, V2);
							MeshChunk->position(x, y+1, z);	  MeshChunk->normal(-1,0,0);	MeshChunk->textureCoord(1, V1);
							MeshChunk->position(x, y,   z);	  MeshChunk->normal(-1,0,0);	MeshChunk->textureCoord(0, V1);

							MeshChunk->triangle(iVertex, iVertex+1, iVertex+2);
							MeshChunk->triangle(iVertex+2, iVertex+3, iVertex);

							iVertex += 4;
						}

					    //x+1
					    Block1 = DefaultBlock;
					    if (x < SX + MaxSize - 1)
						    Block1 = GetBlock(x+1,y,z);

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
					    Block1 = DefaultBlock;
					    if (y > SY)
						    Block1 = GetBlock(x,y-1,z);

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
					    Block1 = DefaultBlock;
					    if (y < SY + MaxSize - 1)
						    Block1 = GetBlock(x,y+1,z);

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
					    Block1 = DefaultBlock;
					    if (z > SZ)
						    Block1 = GetBlock(x,y,z-1);

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
					    Block1 = DefaultBlock;
					    if (z < SZ + MaxSize - 1)
						    Block1 = GetBlock(x,y,z+1);

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
	MeshChunk->colour(BlockLight, BlockLight1, BlockLight2);
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

	float BlockLight, BlockLight1, BlockLight2;

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
							MeshChunk->position(x, y,   z+1); MeshChunk->normal(-1,0,0);	MeshChunk->textureCoord(0, V2);
							MeshChunk->position(x, y+1, z+1); MeshChunk->normal(-1,0,0);	MeshChunk->textureCoord(1, V2);
							MeshChunk->position(x, y+1, z);	  MeshChunk->normal(-1,0,0);	MeshChunk->textureCoord(1, V1);
							MeshChunk->position(x, y,   z);	  MeshChunk->normal(-1,0,0);	MeshChunk->textureCoord(0, V1);

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
							MeshChunk->position(x+1, y,   z);	MeshChunk->normal(1,0,0); MeshChunk->textureCoord(0, V2);
							MeshChunk->position(x+1, y+1, z);	MeshChunk->normal(1,0,0); MeshChunk->textureCoord(1, V2);
							MeshChunk->position(x+1, y+1, z+1);	MeshChunk->normal(1,0,0); MeshChunk->textureCoord(1, V1);
							MeshChunk->position(x+1, y,   z+1);	MeshChunk->normal(1,0,0); MeshChunk->textureCoord(0, V1);

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
							MeshChunk->position(x,   y, z);		MeshChunk->normal(0,-1,0); MeshChunk->textureCoord(0, V2);
							MeshChunk->position(x+1, y, z);		MeshChunk->normal(0,-1,0); MeshChunk->textureCoord(1, V2);
							MeshChunk->position(x+1, y, z+1);	MeshChunk->normal(0,-1,0); MeshChunk->textureCoord(1, V1);
							MeshChunk->position(x,   y, z+1);	MeshChunk->normal(0,-1,0); MeshChunk->textureCoord(0, V1);

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
							MeshChunk->position(x,   y+1, z+1);		MeshChunk->normal(0,1,0); MeshChunk->textureCoord(0, V2);
							MeshChunk->position(x+1, y+1, z+1);		MeshChunk->normal(0,1,0); MeshChunk->textureCoord(1, V2);
							MeshChunk->position(x+1, y+1, z);		MeshChunk->normal(0,1,0); MeshChunk->textureCoord(1, V1);
							MeshChunk->position(x,   y+1, z);		MeshChunk->normal(0,1,0); MeshChunk->textureCoord(0, V1);

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
							MeshChunk->position(x,   y+1, z);		MeshChunk->normal(0,0,-1); MeshChunk->textureCoord(0, V2);
							MeshChunk->position(x+1, y+1, z);		MeshChunk->normal(0,0,-1); MeshChunk->textureCoord(1, V2);
							MeshChunk->position(x+1, y,   z);		MeshChunk->normal(0,0,-1); MeshChunk->textureCoord(1, V1);
							MeshChunk->position(x,   y,   z);		MeshChunk->normal(0,0,-1); MeshChunk->textureCoord(0, V1);

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
}



void CubeWorld::createSolidTexture(const TCHAR* pName)
{
	Ogre::MaterialPtr mat = Ogre::MaterialManager::getSingleton().create("BoxColor", "General", true);
	Ogre::Technique* tech = mat->getTechnique(0);
	Ogre::Pass* pass = tech->getPass(0);
	Ogre::TextureUnitState* tex = pass->createTextureUnitState();
	tex->setColourOperationEx(Ogre::LBX_MODULATE, Ogre::LBS_MANUAL, Ogre::LBS_CURRENT, Ogre::ColourValue(0, 0.5, 0));
}

void CubeWorld::createTexture (const TCHAR* pName, const TCHAR* pImageFilename)
{
	std::cerr << "*** Creating texture " << pImageFilename << std::endl;

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

	std::cerr << "*** Texture created " << pImageFilename << std::endl;
}

void CubeWorld::createWaterTexture(const TCHAR* pName)
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

	std::cerr << "*** Texture created " << pName << std::endl;
}


void CubeWorld::createScene(void)
{
	// Create textures
	createTexture("Combined4", "minecraft/terrain.png");

	createWorldChunks();

	mSceneMgr->setSkyDome(true, "Examples/CloudySky", 2, 8, 100);
	mSceneMgr->setFog(Ogre::FOG_LINEAR, Ogre::ColourValue(0.8, 0.8, 1), 0.5, 0.0, 200);

	mCamera->setFarClipDistance(256); // 256
	mCamera->setNearClipDistance(0.01);

}

void CubeWorld::initWorldBlocksLayers()
{
	// DEACTIVATED BY MISSING FULL TERRAIN GENERATION CODE ... :(
	/*
	int HeightMap[WORLD_SIZE][WORLD_SIZE];
	LandMapErosion LayerMaps[10]; // Hard coded because i'm lazy
	                              // Also, WTF is this var ? The number of Layers ?
	                              // 10 layers seems expensive!
	int NumLayerMaps = 0;
	int BaseSeed;

	LandMapErosion m_Landscape;
	m_Landscape.CreateBaseNoise(WORLD_SIZE, WORLD_SIZE);
	BaseSeed = m_Landscape.m_HeightMap.GetModule.GetSeed();

	LandMapErosion Map = m_Landscape.GetAltitudeMap();

	// Initialize our temporary height map array
	for (int z = 0; z < WORLD_SIZE; ++z)
	    {
		    for (int x = 0; x < WORLD_SIZE; ++x)
			{
				float Height = Map.GetValue(x, z) * WORLD_SIZE/4 + WORLD_SIZE/2;
				HeightMap[x][z] = Height;
			}
	    }

	// Create height maps for each layer
	for (int i = 0; g_Layers[i].BlockID != BLOCK_NULL && i < 10; ++i)
	    {
		    LayerMaps[i].SetSeed(BaseSeed + g_Layers[i].SeedOffset);
		    LayerMaps[i].SetSize(WORLD_SIZE, WORLD_SIZE);
		    LayerMaps[i].CreateAltitude();
		    ++NumLayerMaps;
	    }

	int Layer = 0;

	// Fill in the blocks from all layers
	for (int layer = 0; layer < NumLayerMaps; ++layer)
	    {
		    LandMapErosion Map = LayerMaps[layer].GetAltitudeMap();

		    for (int z = 0; z < WORLD_SIZE; ++z)
			{
				for (int x = 0; x < WORLD_SIZE; ++x)
				    {
					    if (HeightMap[x][z] <= 0)
						    continue;
					    int Height = (Map.GetValue(x, z) + 1)/2.0f*(g_Layers[layer].MaxLevel - g_Layers[layer].MinLevel) + g_Layers[layer].MinLevel;

					    // Don't fill the map below 0 height
					    if (HeightMap[x][z] - Height < 0)
						    Height = HeightMap[x][z];
					    HeightMap[x][z] -= Height;

					    int MaxHeight = HeightMap[x][z] + Height;

					    for (int y = HeightMap[x][z]; y < MaxHeight; ++y)
						{
							GetBlock(x, y, z) = g_Layers[layer].BlockID;
						}
				    }
			}
	    }
	*/
}


void CubeWorld::initWorldBlocksLand(void)
{
	LandMapErosion m_Erosion;

	m_Erosion.CreateBaseNoise(WORLD_SIZE, WORLD_SIZE);

	for (int z = 0; z < WORLD_SIZE; ++z)
	{
		for (int x = 0; x < WORLD_SIZE; ++x)
		{
			float Height = m_Erosion.m_HeightMap.GetValue(x, z) * WORLD_SIZE/4 + WORLD_SIZE/2;

			for (int y = 0; y < Height; ++y)
			{
				// Temporary fix, missing chunk layers
				GetBlock(x, y, z) = 1 ; //(rand() % 4) + 1; // 1-4
			}

		}
	}
}



void CubeWorld::createWorldChunks (void)
{
	//std::vector<int> VertexArray;
	//Ogre::MaterialPtr mat = Ogre::MaterialManager::getSingleton().create("BoxColor", "General", true );
	// Ogre::Technique* tech = mat->getTechnique(0);
	// Ogre::Pass* pass = tech->getPass(0);
	// Ogre::TextureUnitState* tex = pass->createTextureUnitState();
	// tex->setColourOperationEx(Ogre::LBX_MODULATE, Ogre::LBS_MANUAL, Ogre::LBS_CURRENT, Ogre::ColourValue(0, 0.5, 0));

	//Ogre::ManualObject* MeshChunk = new Ogre::ManualObject("MeshManChunk");
	//MeshChunk->begin("BoxColor");

	createTexture("BoxColor", "terrain-grass.png");
	createWaterTexture("WaterTest");

	for (int z = 0; z < WORLD_SIZE; z += CHUNK_SIZE)
	{
		for (int y = 0; y < WORLD_SIZE; y += CHUNK_SIZE)
		{
			for (int x = 0; x < WORLD_SIZE; x += CHUNK_SIZE)
			{
				createChunk(x, y, z);
				createChunkWater(x, y, z);
			}
		}
	}

}

//-------------------------------------------------------------------------------------
void CubeWorld::displaySimpleWorld(void)
{
	// Create a basic green color texture
	Ogre::MaterialPtr mat = Ogre::MaterialManager::getSingleton().create("BoxColor", "General", true);
	Ogre::Technique* tech = mat->getTechnique(0);
	Ogre::Pass* pass = tech->getPass(0);
	Ogre::TextureUnitState* tex = pass->createTextureUnitState();
	tex->setColourOperationEx(Ogre::LBX_MODULATE, Ogre::LBS_MANUAL, Ogre::LBS_CURRENT, Ogre::ColourValue(0, 0.5, 0));

	// Create the one box and the supporting class objects
	Ogre::ManualObject* testBox = createCubeMesh("TestBox1", "BoxColor");
	Ogre::SceneNode* headNode = mSceneMgr->getRootSceneNode()->createChildSceneNode();
	Ogre::MeshPtr Mesh = testBox->convertToMesh("TestBox2");
	Ogre::StaticGeometry* pGeom = new Ogre::StaticGeometry(mSceneMgr, "Boxes");
	Ogre::Entity* pEnt = mSceneMgr->createEntity("TestBox2");

	pGeom->setRegionDimensions(Ogre::Vector3(300, 300, 300));

	// Create out solid block world
	for (int z = 0; z < 10; ++z)
	    {
		    for (int y = 0; y < 256; ++ y)
			{
				for (int x = 0; x < 256; ++x)
				    {
					    pGeom->addEntity(pEnt, Ogre::Vector3(x,y,z));
				    }
			}
	    }

	pGeom->build();

	mSceneMgr->setAmbientLight(Ogre::ColourValue(0.02, 0.02, 0.02));
	Ogre::Light* l = mSceneMgr->createLight("MainLight");
	l->setPosition(20, 80, 50);
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
#warning "Window support not tested"
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
