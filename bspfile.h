
typedef enum
{
    plane_x = 0,
    plane_y,
    plane_z,
    plane_anyx,
    plane_anyy,
    plane_anyz
}
planetypes;

// upper design bounds

#define MAX_MAP_HULLS            4
// hard limit

#define MAX_MAP_MODELS         400
// variable, but 400 brush entities is very stressful on the engine and network code as it is

#define MAX_MAP_BRUSHES       32768
// arbitrary, but large numbers of brushes generally require more lightmap's than the compiler can handle

#define MAX_ENGINE_ENTITIES   1024
#define MAX_MAP_ENTITIES      2048
// hard limit, in actuallity it is too much, as temporary entities in the game plus static map entities can overflow

#define MAX_MAP_ENTSTRING   (512*1024)
// abitrary, 512Kb of string data should be plenty even with TFC FGD's

#define MAX_MAP_PLANES      32768
#define MAX_INTERNAL_MAP_PLANES (256*1024)
// (from email): I have been building a rather complicated map, and using your latest 
// tools (1.61) it seemed to compile fine.  However, in game, the engine was dropping
// a lot of faces from almost every FUNC_WALL, and also caused a strange texture 
// phenomenon in software mode (see attached screen shot).  When I compiled with v1.41,
// I noticed that it hit the MAX_MAP_PLANES limit of 32k.  After deleting some brushes
// I was able to bring the map under the limit, and all of the previous errors went away.

#define MAX_MAP_NODES        32767
// hard limit (negative short's are used as contents values)
#define MAX_MAP_CLIPNODES    32767
// hard limit (negative short's are used as contents values)

#define MAX_MAP_LEAFS         8192
// hard limit (halflife depends on it to setup pvs bits correctly)

#define MAX_MAP_VERTS        65535
#define MAX_MAP_FACES        65535
#define MAX_MAP_MARKSURFACES 65535
// hard limit (data structures store them as unsigned shorts)

#define MAX_MAP_TEXTURES       512
// hard limit (halflife limitation)

#define MAX_MAP_TEXINFO      32767
// hard limit (face.texinfo is signed short)

#define MAX_MAP_EDGES       256000
#define MAX_MAP_SURFEDGES   512000
// arbtirary

#define DEFAULT_MAX_MAP_MIPTEX      0x400000
// 4Mb of textures is enough especially considering the number of people playing the game
// still with voodoo1 and 2 class cards with limited local memory.

#define DEFAULT_MAX_MAP_LIGHTDATA	0x600000
// arbitrary

#define MAX_MAP_VISIBILITY  0x200000
// arbitrary

// these are for entity key:value pairs
#define MAX_KEY                 32
#define ZHLT3_MAX_VALUE             4096
// quote from yahn: 'probably can raise these values if needed'

// texture size limit

#define MAX_TEXTURE_SIZE     ((256 * 256 * sizeof(short) * 3) / 2)
// this is arbitrary, and needs space for the largest realistic texture plus
// room for its mipmaps.'  This value is primarily used to catch damanged or invalid textures
// in a wad file

//=============================================================================

#define BSPVERSION  30
#define TOOLVERSION 2


//
// BSP File Structures
//


typedef struct
{
    int             fileofs, filelen;
}
lump_t;

#define LUMP_ENTITIES      0
#define LUMP_PLANES        1
#define LUMP_TEXTURES      2
#define LUMP_VERTEXES      3
#define LUMP_VISIBILITY    4
#define LUMP_NODES         5
#define LUMP_TEXINFO       6
#define LUMP_FACES         7
#define LUMP_LIGHTING      8
#define LUMP_CLIPNODES     9
#define LUMP_LEAFS        10
#define LUMP_MARKSURFACES 11
#define LUMP_EDGES        12
#define LUMP_SURFEDGES    13
#define LUMP_MODELS       14
#define HEADER_LUMPS      15

//#define LUMP_MISCPAD      -1
//#define LUMP_ZEROPAD      -2

typedef struct
{
    float           mins[3], maxs[3];
    float           origin[3];
    int             headnode[MAX_MAP_HULLS];
    int             visleafs;                              // not including the solid leaf 0
    int             firstface, numfaces;
}
dmodel_t;

typedef struct
{
    int             version;
    lump_t          lumps[HEADER_LUMPS];
}
dheader_t;

typedef struct
{
    int             nummiptex;
    int             dataofs[4];                            // [nummiptex]
}
dmiptexlump_t;

#define MIPLEVELS   4
typedef struct miptex_s
{
    char            name[16];
    unsigned        width, height;
    unsigned        offsets[MIPLEVELS];                    // four mip maps stored
}
miptex_t;

typedef struct
{
    float           point[3];
}
dvertex_t;

typedef struct
{
    float           normal[3];
    float           dist;
    planetypes      type;                                  // PLANE_X - PLANE_ANYZ ?remove? trivial to regenerate
}
dplane_t;

typedef enum
{
    CONTENTS_EMPTY = -1,
    CONTENTS_SOLID = -2,
    CONTENTS_WATER = -3,
    CONTENTS_SLIME = -4,
    CONTENTS_LAVA = -5,
    CONTENTS_SKY = -6,
    CONTENTS_ORIGIN = -7,                                  // removed at csg time
    CONTENTS_CLIP = -8,                                    // changed to contents_solid

    CONTENTS_CURRENT_0 = -9,
    CONTENTS_CURRENT_90 = -10,
    CONTENTS_CURRENT_180 = -11,
    CONTENTS_CURRENT_270 = -12,
    CONTENTS_CURRENT_UP = -13,
    CONTENTS_CURRENT_DOWN = -14,

    CONTENTS_TRANSLUCENT = -15,
    CONTENTS_HINT = -16,     // Filters down to CONTENTS_EMPTY by bsp, ENGINE SHOULD NEVER SEE THIS

#ifdef ZHLT_NULLTEX
    CONTENTS_NULL = -17,     // AJM  // removed in csg and bsp, VIS or RAD shouldnt have to deal with this, only clip planes!
#endif

#ifdef ZHLT_DETAIL   // AJM
    CONTENTS_DETAIL = -18,  
#endif
}
contents_t;

// !!! if this is changed, it must be changed in asm_i386.h too !!!
typedef struct
{
    int             planenum;
    short           children[2];                           // negative numbers are -(leafs+1), not nodes
    short           mins[3];                               // for sphere culling
    short           maxs[3];
    unsigned short  firstface;
    unsigned short  numfaces;                              // counting both sides
}
dnode_t;

typedef struct
{
    int             planenum;
    short           children[2];                           // negative numbers are contents
}
dclipnode_t;

typedef struct texinfo_s
{
    float           vecs[2][4];                            // [s/t][xyz offset]
    int             miptex;
    int             flags;
}
texinfo_t;

#define TEX_SPECIAL     1                                  // sky or slime or null, no lightmap or 256 subdivision

// note that edge 0 is never used, because negative edge nums are used for
// counterclockwise use of the edge in a face
typedef struct
{
    unsigned short  v[2];                                  // vertex numbers
}
dedge_t;

#define MAXLIGHTMAPS    4
typedef struct
{
    unsigned short	planenum;
    short           side;

    int             firstedge;                             // we must support > 64k edges
    short           numedges;
    short           texinfo;

    // lighting info
    unsigned char   styles[MAXLIGHTMAPS];
    int             lightofs;                              // start of [numstyles*surfsize] samples
}
dface_t;

#define AMBIENT_WATER   0
#define AMBIENT_SKY     1
#define AMBIENT_SLIME   2
#define AMBIENT_LAVA    3

#define NUM_AMBIENTS            4                  // automatic ambient sounds

// leaf 0 is the generic CONTENTS_SOLID leaf, used for all solid areas
// all other leafs need visibility info
typedef struct
{
    int             contents;
    int             visofs;                                // -1 = no visibility info

    short           mins[3];                               // for frustum culling
    short           maxs[3];

    unsigned short  firstmarksurface;
    unsigned short  nummarksurfaces;

    unsigned char   ambient_level[NUM_AMBIENTS];
}
dleaf_t;

//============================================================================

#define ANGLE_UP    -1
#define ANGLE_DOWN  -2
//
// Entity Related Stuff
//

typedef struct epair_s
{
    struct epair_s* next;
    char*           key;
    char*           value;
}
epair_t;

typedef struct
{
    float           origin[3];
    int             firstbrush;
    int             numbrushes;
    epair_t*        epairs;
}
entity_t;