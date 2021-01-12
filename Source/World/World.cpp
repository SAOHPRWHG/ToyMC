#include "../Utils/Setting.h"
#include "noise.h"
#include "World.h"
#include <math.h>
#include "../Maths/GeneralMaths.h"
#include "../Utils/Inventory.h"
#include <algorithm>
struct NoiseParameters {
    int octaves;
    int amplitude;
    int smoothness;
    int heightOffset;

    double roughness;

    int seed;
};

double getNoise(int n,NoiseParameters para)
{
    n += para.seed;
    n = (n << 13) ^ n;
    auto newN = (n * (n * n * 60493 + 19990303) + 1376312589) & 0x7fffffff;

    return 1.0 - ((double)newN / 1073741824.0);
}

double getNoise(double x, double z,NoiseParameters para)
{
    return getNoise(x + z * 57.0, para);
}

double lerp(double a, double b, double z)
{
    double mu2 = (1 - cos(z * 3.14)) / 2;
    return (a * (1 - mu2) + b * mu2);
}

double noise(double x, double z,NoiseParameters para)
{
    auto floorX = (double)((
        int)x); // This is kinda a cheap way to floor a double integer.
    auto floorZ = (double)((int)z);

    auto s = 0.0, t = 0.0, u = 0.0,
         v = 0.0; // Integer declaration

    s = getNoise(floorX, floorZ,para);
    t = getNoise(floorX + 1, floorZ,para);
    u = getNoise(
        floorX,
        floorZ + 1,para); // Get the surrounding values to calculate the transition.
    v = getNoise(floorX + 1, floorZ + 1,para);

    auto rec1 = lerp(s, t, x - floorX); // Interpolate between the values.
    auto rec2 = lerp(
        u, v,
        x - floorX); // Here we use x-floorX, to get 1st dimension. Don't mind
                     // the x-floorX thingie, it's part of the cosine formula.
    auto rec3 =
        lerp(rec1, rec2,
             z - floorZ); // Here we use y-floorZ, to get the 2nd dimension.
    return rec3;
}

double getheight_map(int x, int z, int chunkX, int chunkZ,NoiseParameters para)
{
    auto newX = (x + (chunkX * CHUNK_SIZE));
    auto newZ = (z + (chunkZ * CHUNK_SIZE));

    if (newX < 0 || newZ < 0) {
        return WATER_LEVEL - 1;
    }

    auto totalValue = 0.0;

    for (auto a = 0; a < para.octaves - 1;
         a++) // This loops through the octaves.
    {
        auto frequency = pow(
            2.0,
            a); // This increases the frequency with every loop of the octave.
        auto amplitude = pow(
            para.roughness,
            a); // This decreases the amplitude with every loop of the octave.
        totalValue +=
            noise(((double)newX) * frequency / para.smoothness,
                  ((double)newZ) * frequency / para.smoothness,para) *
            amplitude;
    }

    auto val = (((totalValue / 2.1) + 1.2) * para.amplitude) +
               para.heightOffset;

    return val > 0 ? val : 1;
}

NoiseParameters getBiome_Parameters(int x)
{
    NoiseParameters para;
    if(x>160)
    {
        para.octaves = 7;
        para.amplitude = 43;
        para.smoothness = 55;
        para.heightOffset = 0;
        para.roughness = 0.50;
        para.seed=50000;
    }
    else if(x>150)
    {
        para.octaves = 9;
        para.amplitude = 85;
        para.smoothness = 235;
        para.heightOffset = -20;
        para.roughness = 0.51;
        para.seed=50000;
    }
    else if(x>130)
    {
        para.octaves = 5;
        para.amplitude = 100;
        para.smoothness = 195; // 195
        para.heightOffset = -32;
        para.roughness = 0.52;
        para.seed=50000;
    }
    else if(x>120)
    {
        para.octaves = 5;
        para.amplitude = 100;
        para.smoothness = 195;
        para.heightOffset = -30;
        para.roughness = 0.52;
        para.seed=50000;
    }
    else if(x>110)
    {
        para.octaves = 5;
        para.amplitude = 100;
        para.smoothness = 195; // 195
        para.heightOffset = -32;
        para.roughness = 0.52;
        para.seed=50000;
    }
    else if(x>100)
    {
        para.octaves = 9;
        para.amplitude = 85;
        para.smoothness = 235;
        para.heightOffset = -20;
        para.roughness = 0.51;
        para.seed=50000;
    }
    else 
    {
        para.octaves = 9;
        para.amplitude = 80;
        para.smoothness = 335;
        para.heightOffset = -7;
        para.roughness = 0.56;
        para.seed=50000;
    }
    return para;
}

int getBiome_TopBlcok(int x)
{
    if(x>160)
    {
        return GRASS;
    }
    else if(x>150)
    {
    	return GRASS;
    }
    else if(x>130)
    {
    	return SNOW;
    }
    else if(x>120)
    {
    	return GRASS;
    }
    else if(x>110)
    {
    	return GRASS;
    }
    else if(x>90)
    {
    	return GRASS;
    }
    else 
    {
    	return SAND;
    }
}


int getBiome_UnderWaterBlcok(int x)
{
    if(x>160)
    {
        return SAND;
    }
    else if(x>150)
    {
    	return DIRT;
    }
    else if(x>130)
    {
    	return DIRT;
    }
    else if(x>120)
    {
    	return SAND;
    }
    else if(x>110)
    {
    	return DIRT;
    }
    else if(x>90)
    {
    	return SAND;
    }
    else 
    {
    	return SAND;
    }
}

double getHeightAt(int x,int z,int p,int q,int biomemap[][CHUNK_SIZE+10])
{
    NoiseParameters para = getBiome_Parameters(biomemap[x][z]);
    return getheight_map(x,z,p,q,para);
}
void getHeightIn(int p,int q,int xMin, int zMin, int xMax,int zMax,int biomemap[][CHUNK_SIZE+10],int heightmap[][CHUNK_SIZE+10])
{

    float bottomLeft = static_cast<float>(getHeightAt(xMin, zMin,p,q,biomemap));
    float bottomRight = static_cast<float>(getHeightAt(xMax, zMin,p,q,biomemap));
    float topLeft = static_cast<float>(getHeightAt(xMin, zMax,p,q,biomemap));
    float topRight = static_cast<float>(getHeightAt(xMax, zMax,p,q,biomemap));

    for (int x = xMin; x < xMax; ++x)
        for (int z = zMin; z < zMax; ++z) {
            if (x == CHUNK_SIZE)
                continue;
            if (z == CHUNK_SIZE)
                continue;

            float h = smoothInterpolation(
                bottomLeft, topLeft, bottomRight, topRight,
                static_cast<float>(xMin), static_cast<float>(xMax),
                static_cast<float>(zMin), static_cast<float>(zMax),
                static_cast<float>(x), static_cast<float>(z));
 
            heightmap[x][z] = static_cast<int>(h/2);
        }
}
int getMaxValue(int m,int biomemap[][CHUNK_SIZE+10])
{
    int x=-0x7f;
    for(int i=0;i<=m;i++)
    {
        for(int j=0;j<=m;j++)
        {
            x = std::max(x,biomemap[i][j]);
        }
    }
    return x;
}
void create_world(int p, int q, world_func func, void *arg) 
{
    int biomemap[CHUNK_SIZE+10][CHUNK_SIZE+10]={0};
    int heightmap[CHUNK_SIZE+10][CHUNK_SIZE+10]={0};
    NoiseParameters biomeParmams;
    biomeParmams.octaves = 5;
    biomeParmams.amplitude = 120;
    biomeParmams.smoothness = 1035;
    biomeParmams.heightOffset = 0;
    biomeParmams.roughness = 0.75;
     for (int x = 0; x < CHUNK_SIZE + 3; x++)//偏移1
        for (int z = 0; z < CHUNK_SIZE + 3; z++)
        {
            double tmp = getheight_map(x,z,p+10,q+10,biomeParmams);
            biomemap[x][z]=static_cast<int>(tmp);
        }
    int HALF_CHUNK = CHUNK_SIZE / 2;
    int CHUNK = CHUNK_SIZE;

    getHeightIn(p,q,0, 0, HALF_CHUNK, HALF_CHUNK,biomemap,heightmap);
    getHeightIn(p,q,HALF_CHUNK, 0, CHUNK, HALF_CHUNK,biomemap,heightmap);
    getHeightIn(p,q,0, HALF_CHUNK, HALF_CHUNK, CHUNK,biomemap,heightmap);
    getHeightIn(p,q,HALF_CHUNK, HALF_CHUNK, CHUNK, CHUNK,biomemap,heightmap);
    int maxHeight = getMaxValue(CHUNK_SIZE + 3,biomemap);
    maxHeight =std::max(maxHeight,WATER_LEVEL);
    int pad = 0;
    for (int dx = -pad; dx < CHUNK_SIZE + pad; dx++) {
        for (int dz = -pad; dz < CHUNK_SIZE + pad; dz++) {
            int flag = 1;
            if (dx < 0 || dz < 0 || dx >= CHUNK_SIZE || dz >= CHUNK_SIZE) {
                flag = -1;
            }
            int x = p * CHUNK_SIZE + dx;
            int z = q * CHUNK_SIZE + dz;
            int he = heightmap[(dx<0?0:dx)][dz<0?0:dz];
            int bi = biomemap[dx<0?0:dx][dz<0?0:dz];
            for(int y = 0;y < maxHeight+1;y++)
            {
                if(y > he)
                {
                    if(y <= WATER_LEVEL)
                    {
                        func(x,y,z,WATER*flag,arg);//
                    }
                }
                else if(y == he)
                {
                    if(y >= WATER_LEVEL)
                    {
                        if(y < WATER_LEVEL + 4)
                        {
                            func(x,y,z,SAND*flag,arg);//
                        }
                        else
                        {
                            int tmp = getBiome_TopBlcok(bi);
                            if(bi>=90&&tmp!=SAND&&tmp!=SNOW)//
                            {
                                if (SHOW_PLANTS) {
                                    // grass
                                    if (simplex2(-x * 0.1, z * 0.1, 4, 0.8, 2) > 0.6) {
                                        func(x, y+1, z, 17 * flag, arg);
                                    }
                                    // flowers
                                    if (simplex2(x * 0.05, -z * 0.05, 4, 0.8, 2) > 0.7) {
                                        int ww = 18 + simplex2(x * 0.1, z * 0.1, 4, 0.8, 2) * 7;
                                        func(x, y+1, z, ww * flag, arg);
                                    }
                                }
                                if(bi<=150)//
                                {
                                    int ok = SHOW_TREES;
                                    if (dx - 4 < 0 || dz - 4 < 0 ||
                                        dx + 4 >= CHUNK_SIZE || dz + 4 >= CHUNK_SIZE)
                                    {
                                        ok = 0;
                                    }
                                    if (ok && simplex2(x, z, 6, 0.5, 2) > 0.84) {
                                        for (int yy = y+1 + 3; yy < y+1 + 8; yy++) {
                                            for (int ox = -3; ox <= 3; ox++) {
                                                for (int oz = -3; oz <= 3; oz++) {
                                                    int d = (ox * ox) + (oz * oz) +
                                                        (yy - (y+1 + 4)) * (yy - (y+1 + 4));
                                                    if (d < 11) {
                                                        func(x + ox, yy, (z + oz), 15, arg);
                                                    }
                                                }
                                            }
                                        }
                                        for (int yy = y+1; yy < y+1 + 7; yy++) {
                                            func(x, yy, z, 5, arg);
                                        }
                                    }
                                }
                            }
                            func(x,y,z,tmp*flag,arg);
                        }
                    }
                    else
                    {
                        int tmp = getBiome_UnderWaterBlcok(bi);
                        func(x,y,z,tmp*flag,arg);
                    }
                }
                else if(y > he - 3)
                {
                    func(x,y,z,DIRT*flag,arg);//
                }
                else
                {
                    func(x,y,z,STONE*flag,arg);//
                }
                
            }
            // clouds
            if (SHOW_CLOUDS) 
            {
                for (int y = 180; y < 190; y++) 
                {
                    if (simplex3(x * 0.01, y * 0.1, z * 0.01, 8, 0.5, 2) > 0.75)
                    {
                        func(x, y, z, 16 * flag, arg);
                    }
                }
            }
        }
    }
}