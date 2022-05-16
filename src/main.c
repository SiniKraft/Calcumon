#include <tice.h>
#include <graphx.h>
#include <keypadc.h>
#include <string.h>

#include "gfx/gfx.h"

#include "titlescr.h"
#include "dialog.h"
#include "save.h"
#include "tile_properties.h"

extern unsigned char uthon_map[1122];

#define TILE_WIDTH          16
#define TILE_HEIGHT         16

#define TILEMAP_DRAW_WIDTH  21
#define TILEMAP_DRAW_HEIGHT 16

#define IS_TI               1   /*version TI = 1 CE = 0*/

const uint8_t tilemaps_height[1] = {
    34
};

const uint8_t tilemaps_width[1] = {
    33
};


int main(void)
{
    sk_key_t key;
    bool is_male = true;
    bool instant_exit = false;
    unsigned int x_offset = 0;
    unsigned int y_offset = 0;
    uint8_t tilemap_height = 64;
    uint8_t tilemap_width = 64;
    uint8_t direction = 1; /*0 up 1 down 2 left 3 right*/

    /* Initialize graphics drawing */
    gfx_Begin();
    /* Draw to buffer to avoid tearing */
    gfx_SetDrawBuffer();
    gfx_SetMonospaceFont(8);

    PerformTitleScreen(IS_TI);
    gfx_FillScreen(0);
    gfx_BlitBuffer();
    gfx_SetPalette(global_palette, sizeof_global_palette, 0);
    gfx_SetColor(0);
    gfx_SetTextFGColor(1);
    gfx_SetTextBGColor(0);

    kb_Scan();
    SaveData sav;
    if (!(save_exists())) {
    ShowText("Bonjour cher Aventurier !          Bienvenue dans CALCUMON !");
    ShowText("Tu peux maintenir ANNUL pour sauve-garder et quitter a tout moment.");
    ShowText("Utilise SUPPR pour quitter sans    sauvegarder.");
    ShowText("Utilise 2NDE ou ENTRER comme boutonA et (-) ou ALPHA comme bouton B.");
    ShowText("Note: il est impossible de sauve-  garder pendant certains moments.");
    ShowText("Les sauvegardes de TI ne sont pas  lisibles par CE et inversement.");
    ShowText("Mais, comment t'appelles-tu ???");
    bool finask = false;
    char* inputBuffer;
    kb_Scan();
    if (!(kb_Data[6] == kb_Clear)) {
        while (!(finask)) {
            // gfx_End(); // ASK FOR NAME
            // os_ClrHome();
            // os_GetStringInput("Votre nom : ", inputBuffer, 10/*= size*/);
            // os_ClrHome();
            // gfx_Begin(); // RESUMING
            // gfx_SetPalette(global_palette, sizeof_global_palette, 0);
            // gfx_SetColor(0);
            // gfx_SetTextFGColor(1);
            // gfx_SetTextBGColor(0);
            // gfx_SetDrawBuffer();
            // gfx_SetMonospaceFont(8);
            // kb_Scan();
            // if (kb_Data[6] == kb_Clear) {
            //     break;
            // }
            gfx_ZeroScreen();
            inputBuffer = AskText();
            gfx_ZeroScreen();
            ShowText(inputBuffer);
            if (AskBoolText("C'est bien ton nom ???")) {
                finask = true;
            }
        }
    }
    strcpy(sav.name, inputBuffer);
    sav.nlen = strlen(sav.name);
    bool _tmp_sk = false;
    gfx_FillScreen(0);
    _tmp_sk = AskBoolText("Au fait ... Est-tu un garcon ?");
    sav.is_male = _tmp_sk;
    sav.is_ti = IS_TI;
    sav.x_offset = 0;
    sav.y_offset = 0;
    sav.map_num = 0;
    sav.pballs = 10;
    sav.gballs = 0;
    sav.uballs = 0;
    sav.mball = false;
    sav.location = 0;
    sav.free_control_vertical = false;
    sav.free_control_horizontal = false;
    sav.x = 152;
    sav.y = 112;
    sav.is_down = false;
    sav.is_right = false;
    gfx_ZeroScreen();
    ShowText("Voici 10 Poke Balls.");
    ShowText("Tu dois maitenant choisir ton Pokemon de depart. Lequel veux-tu ???");
    uint8_t choosed;
    choosed = AskNumText("0: Bulbizarre  1: Salameche        2: Carapuce ", 2);
    gfx_ZeroScreen();
    ShowText("Voila, c'est tout ! Bon courage,   et merci d'utiliser CALCUMON :)");
    } else {
        sav = load();
        if (sav.is_ti != IS_TI) {
            ShowText("Données de sauvegardes incompatibles avec ce CALCUMON :/");
            instant_exit = true;
            gfx_End();
            return 0;
        }
    }
    is_male = sav.is_male;
    uint24_t x = sav.x;
    uint8_t y = sav.y;
    bool is_down = sav.is_down;
    bool is_right = sav.is_right;

    gfx_tilemap_t tilemap;
    // static Map map;
    // get_world(&map, sav.map_num);
    // if (map.tilemap_height == 2) {
    //     gfx_End();
    //     os_ClrHome();
    //     os_ThrowError(OS_E_UNDEFINED);
    //     return 1;
    // }
    tilemap_height = tilemaps_height[sav.map_num];
    tilemap_width = tilemaps_width[sav.map_num];

    /* Initialize the tilemap structure */
    if (sav.map_num == 0) {
        tilemap.map = uthon_map;
    } else {
        gfx_End();
        return 1;
    }
    tilemap.tiles       = tileset_tiles;
    tilemap.type_width  = gfx_tile_16_pixel;
    tilemap.type_height = gfx_tile_16_pixel;
    tilemap.tile_height = TILE_HEIGHT;
    tilemap.tile_width  = TILE_WIDTH;
    tilemap.draw_height = TILEMAP_DRAW_HEIGHT;
    tilemap.draw_width  = TILEMAP_DRAW_WIDTH;
    tilemap.height      = tilemap_height;
    tilemap.width       = tilemap_width;
    tilemap.y_loc       = 0;
    tilemap.x_loc       = 0;

    gfx_sprite_t* sprite_tile_18 = gfx_MallocSprite(16, 16);
    gfx_sprite_t* sprite_tile_19 = gfx_MallocSprite(16, 16);
    gfx_sprite_t* sprite_tile_20 = gfx_MallocSprite(16, 16);
    if (sprite_tile_18 == NULL || sprite_tile_19 == NULL || sprite_tile_20 == NULL) {  // Security Measure to prevent the gfx_FlipSpriteY writnig to NULL and cause calc restart!
        gfx_End();
        os_ClrHome();
        os_ThrowError(OS_E_MEMORY);
        return 1;
    }
    if (is_male) {
        gfx_FlipSpriteY(sprite_tile_3, sprite_tile_18);
        gfx_FlipSpriteY(sprite_tile_4, sprite_tile_19);
        gfx_FlipSpriteY(sprite_tile_5, sprite_tile_20);
    } else {
        gfx_FlipSpriteY(sprite_tile_12, sprite_tile_18);
        gfx_FlipSpriteY(sprite_tile_13, sprite_tile_19);
        gfx_FlipSpriteY(sprite_tile_14, sprite_tile_20);
    }

    bool moved = false;
    bool ani = false;
    uint8_t tme = 0;
    uint8_t to_walk = 2;
    x_offset = sav.x_offset;
    y_offset = sav.y_offset;
    bool free_control_vertical = sav.free_control_vertical;
    bool free_control_horizontal = sav.free_control_horizontal;

    do
    {
        kb_Scan();

        if (moved) {
            tme++;
            if (kb_Data[2] == kb_Alpha || kb_Data[4] == kb_Chs) {
                tme++;
            }
        }
        if (tme >= 4) {
            ani = !(ani);
            tme = 0;
        }

        key = kb_Data[7];
        
        gfx_Tilemap(&tilemap, x_offset, y_offset);
        
        if (direction == 0) {
            if (is_male) {
                if (!(moved)) { // MALE UP
                    gfx_TransparentSprite_NoClip(sprite_tile_6, x, y);
                } else if (ani) {
                    gfx_TransparentSprite_NoClip(sprite_tile_8, x, y);
                } else {
                    gfx_TransparentSprite_NoClip(sprite_tile_7, x, y);
                }
            } else {
                if (!(moved)) {  // FEMALE UP
                    gfx_TransparentSprite_NoClip(sprite_tile_15, x, y);
                } else if (ani) {
                    gfx_TransparentSprite_NoClip(sprite_tile_17, x, y);
                } else {
                    gfx_TransparentSprite_NoClip(sprite_tile_16, x, y);
                }
            }
        } else if (direction == 1) {
            if (is_male) {
                if (!(moved)) {  // MALE DOWN
                    gfx_TransparentSprite_NoClip(sprite_tile_0, x, y);
                } else if (ani) {
                    gfx_TransparentSprite_NoClip(sprite_tile_2, x, y);
                } else {
                    gfx_TransparentSprite_NoClip(sprite_tile_1, x, y);
                }
            } else {
                if (!(moved)) {  // FEMALE DOWN
                    gfx_TransparentSprite_NoClip(sprite_tile_9, x, y);
                } else if (ani) {
                    gfx_TransparentSprite_NoClip(sprite_tile_11, x, y);
                } else {
                    gfx_TransparentSprite_NoClip(sprite_tile_10, x, y);
                }
            }
        } else if (direction == 2) {  // MALE - FEMALE LEFT
            if (!(moved)) {
                gfx_TransparentSprite_NoClip(sprite_tile_18, x, y);
            } else if (ani) {
                gfx_TransparentSprite_NoClip(sprite_tile_20, x, y);
            } else {
                gfx_TransparentSprite_NoClip(sprite_tile_19, x, y);
            }
        } else {
            if (is_male) {
                if (!(moved)) {  // MALE RIGHT
                    gfx_TransparentSprite_NoClip(sprite_tile_3, x, y);
                } else if (ani) {
                    gfx_TransparentSprite_NoClip(sprite_tile_5, x, y);
                } else {
                    gfx_TransparentSprite_NoClip(sprite_tile_4, x, y);
                }
            } else {
                if (!(moved)) {  // FEMALE RIGHT
                    gfx_TransparentSprite_NoClip(sprite_tile_12, x, y);
                } else if (ani) {
                    gfx_TransparentSprite_NoClip(sprite_tile_14, x, y);
                } else {
                    gfx_TransparentSprite_NoClip(sprite_tile_13, x, y);
                }
            }
        }
        moved = false;
        if (kb_Data[2] == kb_Alpha || kb_Data[4] == kb_DecPnt) {
            to_walk = 4;
        } else {
            to_walk = 2;
        }

        switch (key)
        {
            case kb_Down:
                direction = 1;
                moved = true;
                if (free_control_vertical) {
                    if (!(can_collide[gfx_GetTile(&tilemap, x_offset + x + 8, y_offset + y + 18)])) {
                        y += to_walk;
                    }
                    if (y >= 112 && !(is_down)) {
                        y = 112;
                        free_control_vertical = false;
                    }
                }
                else if (y_offset < (tilemap_height * TILE_HEIGHT) - (TILEMAP_DRAW_HEIGHT * TILE_HEIGHT))
                {
                    free_control_vertical = false;
                    if (!(can_collide[gfx_GetTile(&tilemap, x_offset + x + 8, y_offset + y + 18)])) {
                        y_offset += to_walk; 
                    }
                } else {
                    free_control_vertical = true;
                    is_down = true;
                    if (!(can_collide[gfx_GetTile(&tilemap, x_offset + x + 8, y_offset + y + 18)])) {
                        y += to_walk;
                    }
                }
                break;

            case kb_Left:
                direction = 2;
                moved = true;
                if (free_control_horizontal) {
                    if (!(can_collide[gfx_GetTile(&tilemap, x_offset + x - 2, y_offset + y + 8)])) {
                        x -= to_walk;
                    }
                    if (x <= 152 && is_right) {
                        x = 152;
                        free_control_horizontal = false;
                        is_right = false;
                    }
                }
                else if (x_offset)  // = (x_offset == 0)
                {   
                    if (!(can_collide[gfx_GetTile(&tilemap, x_offset + x - 2, y_offset + y + 8)])) { // x_offset + 150 
                        x_offset -= to_walk;
                    }
                } else {
                    free_control_horizontal = true;
                    is_right = false;
                    if (!(can_collide[gfx_GetTile(&tilemap, x_offset + x - 2, y_offset + y + 8)])) {
                        x -= to_walk;
                    }
                }
                break;

            case kb_Right:
                direction = 3;
                moved = true;
                if (free_control_horizontal) {
                    if (!(can_collide[gfx_GetTile(&tilemap, x_offset + x + 18, y_offset + y + 8)])) {
                        x += to_walk;
                    }
                    if (x >= 152 && !(is_right)) {
                        x = 152;
                        free_control_horizontal = false;
                    }
                }
                else if (x_offset < (tilemap_width * TILE_WIDTH) - (TILEMAP_DRAW_WIDTH * TILE_WIDTH))
                {
                    if (!(can_collide[gfx_GetTile(&tilemap, x_offset + x + 18, y_offset + y + 8)])) {
                        x_offset += to_walk;
                    }
                } else {
                    free_control_horizontal = true;
                    is_right = true;
                    if (!(can_collide[gfx_GetTile(&tilemap, x_offset + x + 18, y_offset + y + 8)])) {
                        x += to_walk;
                    }
                }
                break;

            case kb_Up:
                direction = 0;
                moved = true;
                if (free_control_vertical) {
                    if (!(can_collide[gfx_GetTile(&tilemap, x_offset + x + 8, y_offset + y + 4)])) {
                        y -= to_walk;
                    }
                    if (y <= 112 && is_down) {
                        y = 112;
                        free_control_vertical = false;
                        is_down = false;
                    }
                }
                else if (y_offset) // = (y_offset == 0)
                {
                    if (!(can_collide[gfx_GetTile(&tilemap, x_offset + x + 8, y_offset + y + 4)])) {
                        y_offset -= to_walk;
                    }
                } else {
                    free_control_vertical = true;
                    is_down = false;
                    if (!(can_collide[gfx_GetTile(&tilemap, x_offset + x + 8, y_offset + y + 4)])) {
                        y -= to_walk;
                    }

                }
                break;

            default:
                break;

            
        }

        if (y_offset > 16777200) {
            y_offset = 0;
        }
        if (x_offset > 16777200) {
            x_offset = 0;
        }  // security
        if (free_control_horizontal || free_control_vertical) {
            if (x > 304) {
                x -= to_walk;
            }
            if (x < 0) {
                x += to_walk;
            }
            if (y > 222) { // 234
                y -= to_walk;
            }
            if (y < 0) {
                y += to_walk;
            }
            if (y > 240 && y <= 255) {
                y = 0;
            }
        }
        gfx_SwapDraw();
        
    } while (kb_Data[1] != kb_Del && kb_Data[6] != kb_Clear && !(instant_exit));
    sav.free_control_vertical = free_control_vertical;
    sav.free_control_horizontal = free_control_horizontal;
    sav.x_offset = x_offset;
    sav.y_offset = y_offset;
    sav.x = x;
    sav.y = y;
    sav.is_down = is_down;
    sav.is_right = is_right;
    gfx_ZeroScreen();
    gfx_BlitBuffer();

    save(&sav);
    free(sprite_tile_18);
    free(sprite_tile_19);
    free(sprite_tile_20);
    gfx_End();

    return 0;
}
