// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This file is part of Babel Project, (c) 2000 Blitz Games Ltd.
//
//      File : pcDxdebug.cpp
//   Purpose : Defines a macro for converting DirectX error codes to strings. Declares bDXErrorToString
// Component : Babel Display
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#include <babel.h>

// ********************************************************************************
// Locals

// Exact entry layout used in the binary: 4-byte format + pointer to char name.
struct DXFormatMapEntry
{
    D3DFORMAT    format;
    const char*  name;
};

// The exact strings used by the game. Order matters for god's sake!
static const DXFormatMapEntry bDXFormatMap[] =
{
    { D3DFMT_UNKNOWN,        "D3DFMT_UNKNOWN"      }, // [0]
    { D3DFMT_R8G8B8,         "D3DFMT_R8G8B8"       }, // [1]
    { D3DFMT_A8R8G8B8,       "D3DFMT_A8R8G8B8"     }, // [2]
    { D3DFMT_X8R8G8B8,       "D3DFMT_X8R8G8B8"     }, // [3]
    { D3DFMT_R5G6B5,         "D3DFMT_R5G6B5"       }, // [4]
    { D3DFMT_X1R5G5B5,       "D3DFMT_X1R5G5B5"     }, // [5]
    { D3DFMT_A1R5G5B5,       "D3DFMT_A1R5G5B5"     }, // [6]
    { D3DFMT_A4R4G4B4,       "D3DFMT_A4R4G4B4"     }, // [7]
    { D3DFMT_R3G3B2,         "D3DFMT_R3G3B2"       }, // [8]
    { D3DFMT_A8,             "D3DFMT_A8"           }, // [9]
    { D3DFMT_A8R3G3B2,       "D3DFMT_A8R3G3B2"     }, // [10]
    { D3DFMT_X4R4G4B4,       "D3DFMT_X4R4G4B4"     }, // [11]
    { D3DFMT_A8P8,           "D3DFMT_A8P8"         }, // [12]
    { D3DFMT_P8,             "D3DFMT_P8"           }, // [13]
    { D3DFMT_L8,             "D3DFMT_L8"           }, // [14]
    { D3DFMT_A8L8,           "D3DFMT_A8L8"         }, // [15]
    { D3DFMT_A4L4,           "D3DFMT_A4L4"         }, // [16]
    { D3DFMT_V8U8,           "D3DFMT_V8U8"         }, // [17]
    { D3DFMT_L6V5U5,         "D3DFMT_L6V5U5"       }, // [18]
    { D3DFMT_X8L8V8U8,       "D3DFMT_X8L8V8U8"     }, // [19]
    { D3DFMT_Q8W8V8U8,       "D3DFMT_Q8W8V8U8"     }, // [20]
    { D3DFMT_V16U16,         "D3DFMT_V16U16"       }, // [21]
    { D3DFMT_W11V11U10,      "D3DFMT_W11V11U10"    }, // [22]
    { D3DFMT_UYVY,           "D3DFMT_UYVY"         }, // [23]
    { D3DFMT_YUY2,           "D3DFMT_YUY2"         }, // [24]
    { D3DFMT_DXT1,           "D3DFMT_DXT1"         }, // [25]
    { D3DFMT_DXT2,           "D3DFMT_DXT2"         }, // [26]
    { D3DFMT_DXT3,           "D3DFMT_DXT3"         }, // [27]
    { D3DFMT_DXT4,           "D3DFMT_DXT4"         }, // [28]
    { D3DFMT_DXT5,           "D3DFMT_DXT5"         }, // [29]
    { D3DFMT_D16_LOCKABLE,   "D3DFMT_D16_LOCKABLE" }, // [30]
    { D3DFMT_D32,            "D3DFMT_D32"          }, // [31]
    { D3DFMT_D15S1,          "D3DFMT_D15S1"        }, // [32]
    { D3DFMT_D24S8,          "D3DFMT_D24S8"        }, // [33]
    { D3DFMT_D16,            "D3DFMT_D16"          }, // [34]
    { D3DFMT_D24X8,          "D3DFMT_D24X8"        }, // [35]
    { D3DFMT_D24X4S4,        "D3DFMT_D24X4S4"      }, // [36]
    { D3DFMT_VERTEXDATA,     "D3DFMT_VERTEXDATA"   }, // [37]
    { D3DFMT_INDEX16,        "D3DFMT_INDEX16"      }, // [38]
    { D3DFMT_INDEX32,        "D3DFMT_INDEX32"      }, // [39]
};

static const int bDXFormatMapCount =
    static_cast<int>(sizeof(bDXFormatMap) / sizeof(bDXFormatMap[0]));

// ********************************************************************************
// Function Implementations

/* --------------------------------------------------------------------------------
   Function : bDXFormatToString
   Purpose : convert a DirectX format code to a string
   Parameters : format code
   Returns : string
   Info : 
*/

char *bDXFormatToString(D3DFORMAT format)
{
	// Linear search, identical control flow to the binary
    for (int i = 0; i < bDXFormatMapCount; ++i)
    {
        if (bDXFormatMap[i].format == format)
        {
            // Drop const to satisfy the exact ABI seen in the binary.
            return const_cast<char*>(bDXFormatMap[i].name);
        }
    }
    return "<Unknown Format>";
}
