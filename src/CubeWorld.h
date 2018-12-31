/*
-----------------------------------------------------------------------------
Filename:    CubeWorld.h
-----------------------------------------------------------------------------
*/
#ifndef __CubeWorld_h_
#define __CubeWorld_h_

#include "BaseApp.h"
#include "compat.h"
#include "il_map.h"
#include "il_landscape.h"

typedef unsigned char block_t;
typedef unsigned char blocklight_t;

struct blockinfo_t {
    block_t ID;
    TCHAR Name[64];
    TCHAR Texture[64];
    bool Transparent;
};
struct layer_t {
    block_t BlockID;
    int MinLevel;
    int MaxLevel;
    int SeedOffset;
};


blockinfo_t g_BlockInfo[] =
        {
                {0,   "Air",   "",                                 true},
                {1,   "Grass", "minecraft/terrain-grass.png",      false},
                {2,   "Soil",  "minecraft/terrain-graveldark.png", false},
                {3,   "Rock",  "minecraft/terrain-stone.png",      false},
                {4,   "Lava",  "minecraft/pumpkinface-blank.png",  false},
                {5,   "Water", "minecraft/terrain-water.png",      true},
                {255, "null",  ""} // Must be last
        };
layer_t g_Layers[] =
        {
                {5,   0,   2,   5},     // Water
                {1,   0,   2,   1},     // Grass
                {2,   0,   10,  2},    // Soil
                {3,   20,  200, 3},  // Rock
                {4,   100, 300, 4}, // Lava
                {255, 0,   0,   0}    // Must be last
        };

class CubeWorld : public BaseApp {
public:
    CubeWorld(void);

    virtual ~CubeWorld(void);

    infland::CLandscape m_Landscape;
    blocklight_t *m_BlockLight;

    block_t &GetBlockLight(const int x, const int y, const int z) {
        return m_BlockLight[x + y * WORLD_SIZE + z * WORLD_SIZE2];
    }

private:
    Ogre::ManualObject *createCubeMesh(Ogre::String name, Ogre::String matName);

    static const int WORLD_SIZE = 256;    // We'll change these later for various test worlds
    static const int WORLD_SIZE2 = WORLD_SIZE * WORLD_SIZE;
    static const int WORLD_SIZE3 = WORLD_SIZE * WORLD_SIZE * WORLD_SIZE;
    static const int CHUNK_SIZE = 256;
    static const int BLOCK_NULL = 255;

    int m_ChunkID;                // Used for uniquely naming our chunks

    block_t *m_Blocks;            // Holds the block worlds in a [WORLD_SIZE][WORLD_SIZE][WORLD_SIZE] array

    // Read/write access method for our block world (doesn't check input)
    block_t &GetBlock(const int x, const int y, const int z) {
        return m_Blocks[x + y * WORLD_SIZE + z * WORLD_SIZE * WORLD_SIZE];
    }

    // Used for filling our block world
    void initWorldBlocksRandom(const int Divisor);

    void initWorldBlocksSphere(void);

    void initWorldBlocksLand();

    void initWorldBlocksLandRnd();

    void initWorldBlocksLayers();

    void initWorldBlocksCaves();

    void initWorldBlocksLight();

    // Displays the world using our original "naive" method
    void displaySimpleWorld(void);

    void createChunk(Ogre::ManualObject *MeshChunk, const int StartX, const int StartY, const int StartZ);

    void createChunkWFaces(const int StartX, const int StartY, const int StartZ);

    void createChunkWater(const int StartX, const int StartY, const int StartZ);

    void createWorldChunks(void);

    void createSolidTexture(const TCHAR *pName);

    void createTexture(const TCHAR *pName, const TCHAR *pImageFilename);

    void createWaterTexture(const TCHAR *pName);

    void createSkyTexture(const TCHAR *pName);

    void updateSkyTextureLight(void);


    blockinfo_t GetBlockMaterial(block_t block) {
        return g_BlockInfo[block];
    }

    Ogre::MaterialPtr m_SkyMaterial;
    Ogre::ColourValue m_LightColor;
    Ogre::ColourValue m_AmbientColor;
    Ogre::ColourValue m_FogColor;
    float m_LightAngle;

protected:
    virtual void createScene(void);
};

#endif // #ifndef __CubeWorld_h_
