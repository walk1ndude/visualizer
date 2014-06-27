#pragma OPENCL EXTENSION cl_khr_3d_image_writes : enable
#pragma OPENCL EXTENSION cl_intel_printf

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
    
    for (int i = - kernGaussSize + 1; i != (int) kernGaussSize; ++ i) {
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
    
    const float2 origin = {pos.x - width / 2.0f, pos.y - height / 2.0f};

    const int posT = pos.y * width + pos.x;

    const float xyPiN = pos.x * pos.y * twoPiN;
    cas[posT] = native_sin(xyPiN) + native_cos(xyPiN);
    
    tanTable[posT] = - atan2pi(origin.y, origin.x) * 180.0f;
    radTable[posT] = sqrt(origin.y * origin.y + origin.x * origin.x);

    if (tanTable[posT] < 0.0f) {
        radTable[posT] = - radTable[posT];
        tanTable[posT] = min(tanTable[posT] + 180.0f, 180.0f);
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
    
    if (pos.z > 140) {
        printf("|%d|", pos.z);
    }

    const int4 size = {get_image_width(src), get_image_depth(src),
                       get_image_width(dst), get_image_height(dst)};

    const float4 center = {size.x / 2.0f, size.y / 2.0f,
                           size.z / 2.0f, size.w / 2.0f};

    const float2 pad = {size.x / (float) size.z, size.y / (float) size.w};

    const int posT = pos.y * size.z + pos.x;

    float4 srcPos = {radTable[posT], pos.z, tanTable[posT], 0.0f};
    
    if (fabs(srcPos.x) <= center.z) {

        const float sinoX = (center.z + srcPos.x) * pad.x;
        srcPos.x = center.x + (srcPos.x < 0 ? 0 : 1) * size.x - sinoX;

        const float dhtValue = calcElem(src, cas, srcPos, 0.0, 1);
        
        write_imagef(dst,
                    (int4) (pos.x + (pos.x < center.z ? 1 : -1) * center.z,
                            pos.y + (pos.y < center.w ? 1 : -1) * center.w,
                            pos.z,
                            0),
                    (float4) (( ((int) sinoX % 2) ? 1 : -1) * dhtValue));
    }
}

__kernel void butterflyDht2d(__read_only image3d_t src, __write_only image3d_t dst) {
    int4 pos = {get_global_id(0), get_global_id(1), get_global_id(2), 0};

    const int4 size = {get_image_width(src), get_image_height(src),
                       get_image_width(dst), get_image_height(dst)};

    const int4 center = {size.x / 2, size.y / 2,
                         size.z / 2, size.w / 2};


    int4 positions = {pos.x, pos.y, (size.x - pos.x) % size.x, (size.y - pos.y) % size.y};

    const float4 readPixels = {
            read_imagef(src, sampler, (int4) (positions.x, positions.y, pos.z, 0)).x,
            read_imagef(src, sampler, (int4) (positions.x, positions.w, pos.z, 0)).x,
            read_imagef(src, sampler, (int4) (positions.z, positions.y, pos.z, 0)).x,
            read_imagef(src, sampler, (int4) (positions.z, positions.w, pos.z, 0)).x
    };

    const float E = 0;//((readPixels.x + readPixels.w) - (readPixels.y + readPixels.z)) / 2.0f;

    positions.x += center.x;
    positions.y += center.y;

    positions.z -= center.x;
    positions.w -= center.y;

    positions.xz -= (center.x - center.z);
    positions.yw -= (center.y - center.w);

    write_imagef(dst, (int4) (positions.x, positions.y, pos.z, 0), (float4) (readPixels.x - E));
    write_imagef(dst, (int4) (positions.x, positions.w, pos.z, 0), (float4) (readPixels.y + E));
    write_imagef(dst, (int4) (positions.z, positions.y, pos.z, 0), (float4) (readPixels.z + E));
    write_imagef(dst, (int4) (positions.z, positions.w, pos.z, 0), (float4) (readPixels.w - E));
}
