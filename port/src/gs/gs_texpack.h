/* Texture packs (gs_texpack.c): replacement textures found by PCSX2's file names. */
#ifndef PORT_GS_TEXPACK_H
#define PORT_GS_TEXPACK_H
#include <stdint.h>

#define TEXPACK_MAX_LEVELS 14

typedef struct TexPackImage {
    uint8_t *file;                            /* the whole file */
    uint32_t w, h;
    int format;                               /* 0: 8-bit R, G, B, A; 1, 2, 3: DXT1, DXT3, DXT5 */
    int levels;
    const uint8_t *data[TEXPACK_MAX_LEVELS];  /* each level's data, inside `file` */
    uint32_t bytes[TEXPACK_MAX_LEVELS];
} TexPackImage;

void TexPack_Init(void);  /* looks through the folder once */
int TexPack_Count(void);
/* The replacement for the texture the GS registers describe (as it lies in GS memory now), or -1. *maxAlpha: the largest
   alpha byte the original texture has. The game uses bit 7 of the alpha it writes into the picture as a mask for
   later draws (a texture whose alpha stays at 0x7F never sets it); a replacement's alpha is redrawn and compressed
   and strays over 0x80 in places, so the renderer keeps it at or below the original's largest value. */
int TexPack_Lookup(uint32_t tbp, uint32_t tbw, uint32_t psm, uint32_t tw, uint32_t th, uint32_t tcc, uint32_t cbp, uint32_t cpsm,
                   uint32_t *maxAlpha);
const char *TexPack_Path(int id);
/* Replacement `id`'s pixels: 1 and *img (kept by the pack: valid until the next call), 0 while the file is still
   being read (in the background: ask again later), -1 if it cannot be read. */
int TexPack_Get(int id, TexPackImage *img);
/* Anime colours (gs_draw.c, the fighters' anime pass): "<name>.anime.png" beside the pack's names, twice the texture's
   height, its top half the colour of each texel where the light falls, its bottom half where it does not; alpha 0
   where the game's own shading is kept. It may have a third block below: the texture as it was (its colours), so the
   same picture under another palette (a fighter in a fight, a flash) is known for it without having seen it before. The same name may be in several folders: forms of a fighter that share their
   textures (Goku and his Super Saiyan forms). A folder with "anime_form.txt" is a form: its colours go on the shared
   textures only while one of the textures it lists (their hashes, one a line: what only that form has, its hair, its
   face) is being drawn; a folder without it is for any fighter that has those textures.
   TexPack_AnimeFind: those for the texture of the last TexPack_Lookup, at most `max` (one a folder); none: those of the
   same texture under another palette (the game repaints a fighter's palettes for a moment when hit or charging),
   *other = 1. TexPack_AnimeFolder: one's folder; TexPack_AnimeFolderIsForm, TexPack_AnimeIsMarker: as above.
   TexPack_LoadAnime reads it now (1 = read; let go with TexPack_Free). *cut: from "anime_cut.txt" in its folder (one
   number, 0..100), how far that character's edge between light and shadow moves towards the shadow side. */
int TexPack_AnimeFind(int *ids, int max, int *other);
uint64_t TexPack_LastTex(void); /* the texture hash the last TexPack_Lookup worked out (its name's first part), 0 = none */
int TexPack_AnimeFolder(int id);
int TexPack_AnimeFolderIsForm(int folder);
int TexPack_AnimeIsMarker(int id);
/* One of the textures listed in its folder's "anime_hair.txt" (hashes, one a line): hair, drawn with a dark line along
   the edge of each lock (gs.frag mode 4). */
int TexPack_AnimeIsHair(int id);
int TexPack_LoadAnime(int id, TexPackImage *img, int *cut);
void TexPack_Free(TexPackImage *img);

#endif
