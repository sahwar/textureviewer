#ifndef ENUMS_H
#define ENUMS_H

enum Format {
    FormatUnknown              = 0,

    FormatR8G8B8               = 20,
    FormatA8R8G8B8             = 21,
    FormatX8R8G8B8             = 22,
    FormatR5G6B5               = 23,
    FormatX1R5G5B5             = 24,
    FormatA1R5G5B5             = 25,
    FormatA4R4G4B4             = 26,
    FormatR3G3B2               = 27,
    FormatA8                   = 28,
    FormatA8R3G3B2             = 29,
    FormatX4R4G4B4             = 30,
    FormatA2B10G10R10          = 31,
    FormatA8B8G8R8             = 32,
    FormatX8B8G8R8             = 33,
    FormatG16R16               = 34,
    FormatA2R10G10B10          = 35,
    FormatA16B16G16R16         = 36,

    FormatA8P8                 = 40,
    FormatP8                   = 41,

    FormatL8                   = 50,
    FormatA8L8                 = 51,
    FormatA4L4                 = 52,

    FormatV8U8                 = 60,
    FormatL6V5U5               = 61,
    FormatX8L8V8U8             = 62,
    FormatQ8W8V8U8             = 63,
    FormatV16U16               = 64,
    FormatA2W10V10U10          = 67,

    FormatUYVY                 = 0x59565955, // "UYVY"
    FormatR8G8B8G8             = 0x47424752, // "RGBG"
    FormatYUY2                 = 0x32595559, // "YUY2"
    FormatG8R8G8B8             = 0x42475247, // "GRGB"
    FormatDXT1                 = 0x31545844, // "DXT1"
    FormatDXT2                 = 0x32545844, // "DXT2"
    FormatDXT3                 = 0x33545844, // "DXT3"
    FormatDXT4                 = 0x34545844, // "DXT4"
    FormatDXT5                 = 0x35545844, // "DXT5"
    FormatRXGB                 = 0x42475852, // "RXGB"
    FormatATI2                 = 0x32495441, // "ATI2"

    FormatD16Lockable         = 70,
    FormatD32                  = 71,
    FormatD15S1                = 73,
    FormatD24S8                = 75,
    FormatD24X8                = 77,
    FormatD24X4S4              = 79,
    FormatD16                  = 80,

    FormatD32FLockable        = 82,
    FormatD24FS8               = 83,

    FormatD32Lockable         = 84,
    FormatS8Lockable          = 85,

    FormatL16                  = 81,

    FormatVertexData           =100,
    FormatIndex16              =101,
    FormatIndex32              =102,

    FormatQ16W16V16U16         = 110,

    FormatMulti2ARGB8         = 0x3154454d, // "MET1"

    FormatR16F                 = 111,
    FormatG16R16F              = 112,
    FormatA16B16G16R16F        = 113,

    FormatR32F                 = 114,
    FormatG32R32F              = 115,
    FormatA32B32G32R32F        = 116,

    FormatCxV8U8               = 117,

    FormatA1                   = 118,
    FormatA2B10G10R10_XR_BIAS  = 119,
    FormatBinaryBuffer         = 199,

    FormatP4,
    FormatA4P4,

    FormatLast                 = 0x7fffffff
};


#endif // ENUMS_H