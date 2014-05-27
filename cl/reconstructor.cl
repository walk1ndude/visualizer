#pragma OPENCL EXTENSION cl_khr_3d_image_writes : enable

__constant sampler_t sampler = CLK_NORMALIZED_COORDS_FALSE | CLK_ADDRESS_CLAMP_TO_EDGE | CLK_FILTER_LINEAR;

float calcElem(image3d_t src, __global float * cas, int4 pos, int offset, float coeff);

float calcElem(image3d_t src, __global float * cas, int4 pos, int offset, float coeff) {
    int width = get_image_width(src);

    float elem = 0.0f;
    for (int i = 0; i != width; ++ i) {
        elem += (
                 read_imagef(src, sampler, (int4) (i, pos.y, pos.z, 0)).x *
                 cas[pos.x * width + offset + i]
                 );
    }

    return elem * coeff;
}

__kernel void calcCas(__global float * dst, __private size_t width, __private float twoPiN) {
    const int2 pos = {get_global_id(0), get_global_id(1)};
    dst[pos.y * width + pos.x] = sin(pos.x * pos.y * twoPiN) + cos(pos.x * pos.y * twoPiN);
}

__kernel void calcTrig(__global float * cosTable, __global float * sinTable) {
    const int pos = get_global_id(0);
    const float angle = M_PI_F / 180.0 * pos;
    
    cosTable[pos] = cos(angle);
    sinTable[pos] = sin(angle);
}

__kernel void dht1dTranspose(__read_only image3d_t src, __write_only image3d_t dst,
                             __global float * cas, __private float coeff) {
    const int4 pos = {get_global_id(0), get_global_id(1), get_global_id(2), 0};
    write_imagef(dst, (int4) (pos.y, pos.x, pos.z, 0), (float4) (calcElem(src, cas, pos, 0, coeff)));
}

__kernel void fourier2d(__read_only image3d_t src, __write_only image3d_t dst,
                        __global float * cas,
                        __constant float * cosTable, __constant float * sinTable) {
    const int4 pos = {get_global_id(0), get_global_id(1), get_global_id(2), 0};

    const int widthSrc = get_image_width(src);
    const int widthDst = get_image_width(dst);
    
    const int heightSrc = get_image_height(src);
    const int heightDst = get_image_height(dst);

    const int centerX = widthDst / 2;
    const int centerY = heightDst / 2;

    const int offset = centerX - widthSrc / 2;
    
    const float ratioPad = widthSrc / (float) widthDst;
    
    int swappedX;

    int circlePosX;
    int circlePosY;
    
    int radius;
    
    const int dx = ((pos.x < widthSrc / 2) ? 1 : -1) * widthSrc / 2;
    int curX = pos.x - dx * ceil(1 / ratioPad);
    
    while (curX != pos.x) {
        curX += dx;
        
        if (curX < 0 || curX > widthDst - 1) {
            continue;
        }
        
        swappedX = (centerX - curX + widthDst) % widthDst;
        radius = swappedX - centerX;
        
        circlePosX = centerX + ratioPad * radius * cosTable[pos.z];
        circlePosY = centerY - ratioPad * radius * sinTable[pos.z];
        
        write_imagef(dst,
                     (int4) (circlePosX + (circlePosX < centerX ? 1 : -1) * centerX,
                             circlePosY + (circlePosY < centerY ? 1 : -1) * centerY,
                             pos.y,
                             0),
                     (float4) ((swappedX % 2 ? -1 : 1) * calcElem(src, cas,
                                                                  (int4) (curX, pos.y, pos.z, 0),
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
