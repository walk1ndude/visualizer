#pragma OPENCL EXTENSION cl_khr_3d_image_writes : enable

__constant sampler_t sampler = CLK_NORMALIZED_COORDS_FALSE | CLK_ADDRESS_CLAMP_TO_EDGE | CLK_FILTER_NEAREST;

float calcElem(image3d_t src, __global float * cas, float4 pos, int offset, float coeff);
int reflect(int maxDir, int curP);

float calcElem(image3d_t src, __global float * cas, float4 pos, int offset, float coeff) {
    int width = get_image_width(src);

    float elem = 0.0f;
    for (int i = 0; i != width; ++ i) {
        elem += (
                 read_imagef(src, sampler, (float4) ((float) i, pos.y, pos.z, 0.0f)).x *
                 cas[(int) (pos.x) * width + offset + i]
                 );
    }

    return elem * coeff;
}

int reflect(int maxDir, int curP) {
    if (curP < 0) {
        return - curP - 1;
    }
    if (curP >= maxDir) {
        return 2 * maxDir - curP - 1;
    }
    return curP;
}

__kernel void gauss1d(__read_only image3d_t src, __write_only image3d_t dst,
                      __constant float * gaussTab,
                      __private uint dirX, __private uint dirY, __private uint dirZ,
                      __private uint kernGaussSize) {
    const int4 pos = {get_global_id(0), get_global_id(1), get_global_id(2), 0};
    float sum = 0.0f;
    
    int4 posR = (int4) (0);
    
    for (int i = - kernGaussSize + 1; i != kernGaussSize; ++ i) {
        posR.x = reflect(get_image_width(src), pos.x + dirX * i);
        posR.y = reflect(get_image_height(src), pos.y + dirY * i);
        posR.z = reflect(get_image_depth(src), pos.z + dirZ * i);
        
        sum += (gaussTab[kernGaussSize + i] * read_imagef(src, sampler, posR).x);
    }
                
    write_imagef(dst, pos, (float4) (sum));
}

__kernel void calcTables(__global float * cas, __global float * tanTable,
                         __global float * radTable,
                         __private size_t width, __private size_t height,
                         __private float twoPiN) {
    const int2 pos = {get_global_id(0), get_global_id(1)};
    
    const float centerX = width / 2.0;
    const float centerY = height / 2.0;
    
    const float xc = pos.x - centerX;
    const float yc = pos.y - centerY;

    const float xyPiN = pos.x * pos.y * twoPiN;

    const int posT = pos.y * width + pos.x;

    cas[posT] = sin(xyPiN) + cos(xyPiN);
    
    tanTable[posT] = - atan2(yc, xc) * 180.0 / M_PI_F;
    radTable[posT] = sqrt(yc * yc + xc * xc);

//    tanTable[posT] = min(tanTable[posT] + 180.0f, 360.0f);
    
//    if (tanTable[posT] > 180.0f && tanTable[posT] < 270.0f) {
//        radTable[posT] = - radTable[posT];
//    }
    
    if (tanTable[posT] < 0.0f) {
        tanTable[posT] = min(tanTable[posT] + 180.0f, 180.0f);
        radTable[posT] = - radTable[posT];
    }

}

__kernel void dht1dTranspose(__read_only image3d_t src, __write_only image3d_t dst,
                             __global float * cas, __private float coeff) {
    const int4 pos = {get_global_id(0), get_global_id(1), get_global_id(2), 0};
    write_imagef(dst, (int4) (pos.y, pos.x, pos.z, 0), (float4) (calcElem(src, cas, (float4) (pos.x, pos.y, pos.z, 0), 0, coeff)));
}

__kernel void fourier2d(__read_only image3d_t src, __write_only image3d_t dst,
                        __global float * cas, __global float * tanTable, __global float * radTable) {
    const int4 pos = {get_global_id(0), get_global_id(1), get_global_id(2), 0};

    const int widthSrc = get_image_width(src);
    const int widthDst = get_image_width(dst);

    const int heightSrc = get_image_depth(src);
    const int heightDst = get_image_height(dst);

    const float centerXDst = widthDst / 2.0f;
    const float centerYDst = heightDst / 2.0f;

    const float centerXSrc = widthSrc / 2.0f;

    const int offset = centerXDst - widthSrc / 2;

    const float ratioPad = widthSrc / (float) widthDst;

    const float xc = pos.x - centerXDst;
    const float yc = pos.y - centerYDst;
    
    const int posT = pos.y * widthDst + pos.x;

    const float sinoY = tanTable[posT];
    const float rad = radTable[posT];
    
    const float sinoX = (centerXDst + rad) * ratioPad;
    const float swappedSinoX = centerXSrc + (rad < 0 ? 0 : 1) * widthSrc - sinoX;
    
    if (fabs(rad) <= centerXDst) {
        
        write_imagef(dst,
            (int4) (pos.x + (pos.x < centerXDst ? 1 : -1) * centerXDst,
                    pos.y + (pos.y < centerYDst ? 1 : -1) * centerYDst,
                    pos.z,
                    0),
            (float4) (((int) sinoX % 2 ? -1 : 1) * calcElem(src, cas,
                                                            (float4) (swappedSinoX, pos.z, sinoY, 0.0f),
                                                            offset, 1)));
           }
}

__kernel void butterflyDht2d(__read_only image3d_t src, __write_only image3d_t dst) {
    int4 pos = {get_global_id(0), get_global_id(1), get_global_id(2), 0};

    const int widthSrc = get_image_width(src);
    const int heightSrc = get_image_height(src);

    const int widthDst = get_image_width(dst);
    const int heightDst = get_image_height(dst);

    const int centerXSrc = widthSrc / 2;
    const int centerYSrc = heightSrc / 2;

    const int centerXDst = widthDst / 2;
    const int centerYDst = heightDst / 2;

    if (pos.x * pos.x + pos.y * pos.y <= centerXDst * centerYDst) {
        int mCol = (widthSrc - pos.x) % widthSrc;
        int mRow = (heightSrc - pos.y) % heightSrc;

        const float A = read_imagef(src, sampler, pos).x;
        const float B = read_imagef(src, sampler, (int4) (pos.x, mRow, pos.z, 0)).x;
        const float C = read_imagef(src, sampler, (int4) (mCol, pos.y, pos.z, 0)).x;
        const float D = read_imagef(src, sampler, (int4) (mCol, mRow, pos.z, 0)).x;

        const float E = ((A + D) - (B + C)) / 2;

        pos.x += centerXSrc;
        pos.y += centerYSrc;

        mCol -= centerXSrc;
        mRow -= centerYSrc;

        pos.x -= (centerXSrc - centerXDst);
        pos.y -= (centerYSrc - centerYDst);

        mCol -= (centerXSrc - centerXDst);
        mRow -= (centerYSrc - centerYDst);

        write_imagef(dst, pos, (float4) (A - E));
        write_imagef(dst, (int4) (pos.x, mRow, pos.z, 0), (float4) (B + E));
        write_imagef(dst, (int4) (mCol, pos.y, pos.z, 0), (float4) (C + E));
        write_imagef(dst, (int4) (mCol, mRow, pos.z, 0), (float4) (D - E));
    }
}
