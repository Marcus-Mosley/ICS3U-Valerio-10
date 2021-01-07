// Copyright (c) 2020 Marcus A. Mosley All rights reserved.
//
// Created by Marcus A. Mosley
// Created on January 2020
// This program includes a score

#include <gb/gb.h>
#include <rand.h>
#include <stdio.h>
#include "logo_data.c"
#include "logo_map.c"
#include "titlescreen_data.c"
#include "titlescreen_map.c"
#include "battle_data.c"
#include "battle_map.c"
#include "cave_data.c"
#include "cave_map.c"
#include "controls_data.c"
#include "controls_map.c"
#include "gameover_map.c"
#include "journey_data.c"
#include "journey_map.c"
#include "letter_data.c"
#include "letter_map.c"
#include "pause_data.c"
#include "pause_map.c"
#include "return_data.c"
#include "return_map.c"
#include "ValerioCharacter.c"
#include "ValerioSprites.c"
UINT8 i;

ValerioCharacter valerio, enemy_1, enemy_2, enemy_3, enemy_4, enemy_5;
UBYTE spritesize = 8, stage, attack, block;
unsigned int seed = 0;
int dmg, score, topright, topleft, botright, botleft, botrightalt, botleftalt;

void performantdelay(UINT8 numloops) {
    UINT8 ii;
    for (ii = 0; ii < numloops; ii++) {
        wait_vbl_done();
    }
}

int random(int max, int min) {
    int randint;
    randint = rand() % max;
    if (randint < 0) {
        randint = -1 * randint;
    }
    randint += min;
    return randint;
}

void fadeout() {
    for (i=0; i < 4; i++) {
        switch (i) {
            case 0:
                BGP_REG = 0xE4;
                break;
            case 1:
                BGP_REG = 0xF9;
                break;
            case 2:
                BGP_REG = 0xFE;
                break;
            case 3:
                BGP_REG = 0xFF;
                break;
        }
        performantdelay(5);
    }
}

void fadein() {
    for (i=0; i < 3; i++) {
        switch (i) {
            case 0:
                BGP_REG = 0xFE;
                break;
            case 1:
                BGP_REG = 0xF9;
                break;
            case 2:
                BGP_REG = 0xE4;
                break;
        }
        performantdelay(5);
    }
}

void sign() {
    fadeout();
    HIDE_SPRITES;
    set_bkg_data(0, 103, letter_data);
    set_bkg_tiles(0, 0, 20, 18, letter_map);
    fadein();

    waitpad(J_A);

    DISPLAY_OFF;
    set_bkg_data(0, 99, controls_data);
    set_bkg_tiles(0, 0, 20, 18, controls_map);
    DISPLAY_ON;

    waitpad(J_B);

    fadeout();
    SHOW_SPRITES;
    set_bkg_data(0, 97, cave_data);
    set_bkg_tiles(0, 0, 20, 18, cave_map);
    fadein();
}

void gameover() {
    HIDE_SPRITES;
    fadeout();

    set_bkg_data(0, 36, pause_data);
    set_bkg_tiles(0, 0, 20, 18, gameover_map);

    fadein();

    waitpad(J_START);
    DISPLAY_OFF;
    HIDE_BKG;
    printf("Score = %d \n", score);

    performantdelay(60);
    reset();
}

void movegamecharacter(ValerioCharacter* character, UINT8 x, UINT8 y) {
    move_sprite(character->spritids[0], x, y);
    move_sprite(character->spritids[1], x + spritesize, y);
    move_sprite(character->spritids[2], x, y + spritesize);
    move_sprite(character->spritids[3], x + spritesize, y + spritesize);
}

UBYTE checkcollisions(ValerioCharacter* one, ValerioCharacter* two) {
    return ((one->x >= two->x && one->x <= two->x + two->width) &&
        (one->y >= two->y && one->y <= two->y + two->height) ||
        (two->x >= one->x && two->x <= one->x + one->width) &&
        (two->y >= one->y && two->y <= one->y + one->height));
}

void enemydeath(ValerioCharacter* character) {
    dmg = 0;
    set_sprite_tile(character->spritids[0], 54);
    set_sprite_tile(character->spritids[1], 54);
    set_sprite_tile(character->spritids[2], 54);
    set_sprite_tile(character->spritids[3], 54);
    character->x = 0;
    character->y = 0;
    movegamecharacter(character, 0, 0);
    score += (character->type * 50);
    character->dead = 1;
}

UBYTE canplayermove(UINT8 newplayerx, UINT8 newplayery,
                    ValerioCharacter* character) {
    UINT16 indexTLx, indexTLy, tileindexTL;
    UBYTE result;

    indexTLx = (newplayerx) / 8;
    indexTLy = (newplayery) / 8;
    tileindexTL = 20 * indexTLy + indexTLx;

    if (stage == 0) {
        result = cave_map[tileindexTL] == 0x00;
        if (tileindexTL == 87) {
            // Open Sign Menu
            sign();
        } else if (tileindexTL == 29 || tileindexTL == 30) {
            // Change to Battle Scene
            DISPLAY_OFF;
            HIDE_SPRITES;
            set_bkg_data(0, 33, journey_data);
            set_bkg_tiles(0, 0, 20, 18, journey_map);
            DISPLAY_ON;
            while (1) {
                if (joypad() & J_A) {
                    stage = 1;
                    fadeout();
                    newplayerx = 80;
                    newplayery = 120;
                    movegamecharacter(&valerio, newplayerx, newplayery);
                    set_bkg_data(0, 51, battle_data);
                    set_bkg_tiles(0, 0, 20, 18, battle_map);
                    SHOW_SPRITES;
                    fadein();
                    break;
                } else if (joypad() & J_B) {
                    DISPLAY_OFF;
                    SHOW_SPRITES;
                    set_bkg_data(0, 97, cave_data);
                    set_bkg_tiles(0, 0, 20, 18, cave_map);
                    DISPLAY_ON;
                    break;
                }
            }
        }
    } else {
        result = battle_map[tileindexTL] == 0x00;
        if (tileindexTL == 329 || tileindexTL == 330) {
            DISPLAY_OFF;
            HIDE_SPRITES;
            set_bkg_data(0, 32, return_data);
            set_bkg_tiles(0, 0, 20, 18, return_map);
            DISPLAY_ON;

            waitpad(J_B);

            DISPLAY_OFF;
            SHOW_SPRITES;
            set_bkg_data(0, 51, battle_data);
            set_bkg_tiles(0, 0, 20, 18, battle_map);
            DISPLAY_ON;
        } else if (result == 1) {
            if (newplayerx <= 0 || newplayerx >= 160) {
                result = 0;
            } else if (newplayery <= 0) {
                result = 0;
            }
        }

        if (character->type == 0) {
            if (attack == 1) {
                if (checkcollisions(&valerio, &enemy_1)) {
                    enemydeath(&enemy_1);}
                if (checkcollisions(&valerio, &enemy_2)) {
                    enemydeath(&enemy_2);}
                if (checkcollisions(&valerio, &enemy_3)) {
                    enemydeath(&enemy_3);}
                if (checkcollisions(&valerio, &enemy_4)) {
                    enemydeath(&enemy_4);}
                if (checkcollisions(&valerio, &enemy_5)) {
                    enemydeath(&enemy_5);}
            } else if (block == 1) {
                if (checkcollisions(&valerio, &enemy_1)) {result = 0;}
                if (checkcollisions(&valerio, &enemy_2)) {result = 0;}
                if (checkcollisions(&valerio, &enemy_3)) {result = 0;}
                if (checkcollisions(&valerio, &enemy_4)) {result = 0;}
                if (checkcollisions(&valerio, &enemy_5)) {result = 0;}
            } else {
                if (checkcollisions(&valerio, &enemy_1)) {
                } else if (checkcollisions(&valerio, &enemy_2)) {
                } else if (checkcollisions(&valerio, &enemy_3)) {
                } else if (checkcollisions(&valerio, &enemy_4)) {
                } else if (checkcollisions(&valerio, &enemy_5)) {
                } else {dmg = 0;}
            }
        } else {
            if (attack == 1) {
                if (checkcollisions(character, &valerio)) {
                    enemydeath(character);}
            } else if (block == 1) {
                if (checkcollisions(character, &valerio)) {result = 0;}
            } else {
                if (checkcollisions(character, &valerio)) {dmg += 1;}
            }
        }
    }
    if (dmg >= 2) {gameover();}
    return result;
}

void setupvalerio() {
    valerio.x = 80;
    valerio.y = 80;
    valerio.width = 16;
    valerio.height = 16;
    valerio.left = 1;
    valerio.right = 1;
    valerio.down = 1;
    valerio.up = 1;

    // load sprites for valerio
    set_sprite_tile(0, 0);
    valerio.spritids[0] = 0;
    set_sprite_tile(1, 1);
    valerio.spritids[1] = 1;
    set_sprite_tile(2, 2);
    valerio.spritids[2] = 2;
    set_sprite_tile(3, 3);
    valerio.spritids[3] = 3;

    movegamecharacter(&valerio, valerio.x, valerio.y);
}

int valeriofront() {
    // change sprites for valerio
    if (valerio.down > 0) {
        set_sprite_tile(0, 1);
        set_sprite_prop(0, S_FLIPX);
        set_sprite_tile(1, 0);
        set_sprite_prop(1, S_FLIPX);
        set_sprite_tile(2, 3);
        set_sprite_prop(2, S_FLIPX);
        set_sprite_tile(3, 2);
        set_sprite_prop(3, S_FLIPX);
        if (valerio.down == 2) {
            valerio.down = 0;
        } else {
            valerio.down += 1;
        }
    } else {
        set_sprite_tile(0, 0);
        set_sprite_prop(0, get_sprite_prop(0) & ~S_FLIPX);
        set_sprite_tile(1, 1);
        set_sprite_prop(1, get_sprite_prop(1) & ~S_FLIPX);
        set_sprite_tile(2, 2);
        set_sprite_prop(2, get_sprite_prop(2) & ~S_FLIPX);
        set_sprite_tile(3, 3);
        set_sprite_prop(3, get_sprite_prop(3) & ~S_FLIPX);
        if (valerio.down == -1) {
            valerio.down = 1;
        } else {
            valerio.down -= 1;
        }
    }
}

int valerioback() {
    // change sprites for valerio
    if (valerio.up > 0) {
        set_sprite_tile(0, 5);
        set_sprite_prop(0, S_FLIPX);
        set_sprite_tile(1, 4);
        set_sprite_prop(1, S_FLIPX);
        set_sprite_tile(2, 7);
        set_sprite_prop(2, S_FLIPX);
        set_sprite_tile(3, 6);
        set_sprite_prop(3, S_FLIPX);
        if (valerio.up == 2) {
            valerio.up = 0;
        } else {
            valerio.up += 1;
        }
    } else {
        set_sprite_tile(0, 4);
        set_sprite_prop(0, get_sprite_prop(0) & ~S_FLIPX);
        set_sprite_tile(1, 5);
        set_sprite_prop(1, get_sprite_prop(1) & ~S_FLIPX);
        set_sprite_tile(2, 6);
        set_sprite_prop(2, get_sprite_prop(2) & ~S_FLIPX);
        set_sprite_tile(3, 7);
        set_sprite_prop(3, get_sprite_prop(3) & ~S_FLIPX);
        if (valerio.up == -1) {
            valerio.up = 1;
        } else {
            valerio.up -= 1;
        }
    }
}

int valerioleft() {
    // change sprites for valerio
    if (valerio.left > 0) {
        set_sprite_tile(0, 9);
        set_sprite_prop(0, S_FLIPX);
        set_sprite_tile(1, 8);
        set_sprite_prop(1, S_FLIPX);
        set_sprite_tile(2, 11);
        set_sprite_prop(2, S_FLIPX);
        set_sprite_tile(3, 10);
        set_sprite_prop(3, S_FLIPX);
        if (valerio.left == 2) {
            valerio.left = 0;
        } else {
            valerio.left += 1;
        }
    } else {
        set_sprite_tile(0, 9);
        set_sprite_prop(0, S_FLIPX);
        set_sprite_tile(1, 8);
        set_sprite_prop(1, S_FLIPX);
        set_sprite_tile(2, 13);
        set_sprite_prop(2, S_FLIPX);
        set_sprite_tile(3, 12);
        set_sprite_prop(3, S_FLIPX);
        if (valerio.left == -1) {
            valerio.left = 1;
        } else {
            valerio.left -= 1;
        }
    }
}

int valerioright() {
    // change sprites for valerio
    if (valerio.right > 0) {
        set_sprite_tile(0, 8);
        set_sprite_prop(0, get_sprite_prop(0) & ~S_FLIPX);
        set_sprite_tile(1, 9);
        set_sprite_prop(1, get_sprite_prop(1) & ~S_FLIPX);
        set_sprite_tile(2, 10);
        set_sprite_prop(2, get_sprite_prop(2) & ~S_FLIPX);
        set_sprite_tile(3, 11);
        set_sprite_prop(3, get_sprite_prop(3) & ~S_FLIPX);
        if (valerio.right == 2) {
            valerio.right = 0;
        } else {
            valerio.right += 1;
        }
    } else {
        set_sprite_tile(0, 8);
        set_sprite_prop(0, get_sprite_prop(0) & ~S_FLIPX);
        set_sprite_tile(1, 9);
        set_sprite_prop(1, get_sprite_prop(1) & ~S_FLIPX);
        set_sprite_tile(2, 12);
        set_sprite_prop(2, get_sprite_prop(2) & ~S_FLIPX);
        set_sprite_tile(3, 13);
        set_sprite_prop(3, get_sprite_prop(3) & ~S_FLIPX);
        if (valerio.right == -1) {
            valerio.right = 1;
        } else {
            valerio.right -= 1;
        }
    }
}

void setupenemy(ValerioCharacter* enemy, int number) {
    enemy->x = random(144, 8);
    enemy->y = random(88, 8);
    enemy->width = 16;
    enemy->height = 16;
    enemy->type = random(3, 1);
    enemy->left = 1;
    enemy->right = 1;
    enemy->down = 1;
    enemy->up = 1;
    enemy->dead = 0;

    if (enemy->type == 1) {
        topright = 14;
        topleft = 15;
        botright = 16;
        botleft = 17;
    } else if (enemy->type == 2) {
        topright = 28;
        topleft = 29;
        botright = 30;
        botleft = 31;
    } else if (enemy->type == 3) {
        topright = 40;
        topleft = 41;
        botright = 42;
        botleft = 43;
    }

    // load sprites for enemy
    if (number == 1) {
        set_sprite_tile(4, topright);
        enemy->spritids[0] = 4;
        set_sprite_tile(5, topleft);
        enemy->spritids[1] = 5;
        set_sprite_tile(6, botright);
        enemy->spritids[2] = 6;
        set_sprite_tile(7, botleft);
        enemy->spritids[3] = 7;
    } else if (number == 2) {
        set_sprite_tile(8, topright);
        enemy->spritids[0] = 8;
        set_sprite_tile(9, topleft);
        enemy->spritids[1] = 9;
        set_sprite_tile(10, botright);
        enemy->spritids[2] = 10;
        set_sprite_tile(11, botleft);
        enemy->spritids[3] = 11;
    } else if (number == 3) {
        set_sprite_tile(12, topright);
        enemy->spritids[0] = 12;
        set_sprite_tile(13, topleft);
        enemy->spritids[1] = 13;
        set_sprite_tile(14, botright);
        enemy->spritids[2] = 14;
        set_sprite_tile(15, botleft);
        enemy->spritids[3] = 15;
    } else if (number == 4) {
        set_sprite_tile(16, topright);
        enemy->spritids[0] = 16;
        set_sprite_tile(17, topleft);
        enemy->spritids[1] = 17;
        set_sprite_tile(18, botright);
        enemy->spritids[2] = 18;
        set_sprite_tile(19, botleft);
        enemy->spritids[3] = 19;
    } else if (number == 5) {
        set_sprite_tile(20, topright);
        enemy->spritids[0] = 20;
        set_sprite_tile(21, topleft);
        enemy->spritids[1] = 21;
        set_sprite_tile(22, botright);
        enemy->spritids[2] = 22;
        set_sprite_tile(23, botleft);
        enemy->spritids[3] = 23;
    }
    movegamecharacter(enemy, enemy->x, enemy->y);
}

int enemyfront(ValerioCharacter* enemy) {
    // change sprites for enemy

    if (enemy->type == 1) {
        topright = 14;
        topleft = 15;
        botright = 16;
        botleft = 17;
    } else if (enemy->type == 2) {
        topright = 28;
        topleft = 29;
        botright = 30;
        botleft = 31;
    } else if (enemy->type == 3) {
        topright = 40;
        topleft = 41;
        botright = 42;
        botleft = 43;
    }

    if (enemy->down > 0) {
        set_sprite_tile(enemy->spritids[0], topleft);
        set_sprite_prop(enemy->spritids[0], S_FLIPX);
        set_sprite_tile(enemy->spritids[1], topright);
        set_sprite_prop(enemy->spritids[1], S_FLIPX);
        set_sprite_tile(enemy->spritids[2], botleft);
        set_sprite_prop(enemy->spritids[2], S_FLIPX);
        set_sprite_tile(enemy->spritids[3], botright);
        set_sprite_prop(enemy->spritids[3], S_FLIPX);
        if (enemy->down == 2) {
            enemy->down = 0;
        } else {
            enemy->down += 1;
        }
    } else {
        set_sprite_tile(enemy->spritids[0], topright);
        set_sprite_prop(enemy->spritids[0], get_sprite_prop(4) & ~S_FLIPX);
        set_sprite_tile(enemy->spritids[1], topleft);
        set_sprite_prop(enemy->spritids[1], get_sprite_prop(5) & ~S_FLIPX);
        set_sprite_tile(enemy->spritids[2], botright);
        set_sprite_prop(enemy->spritids[2], get_sprite_prop(6) & ~S_FLIPX);
        set_sprite_tile(enemy->spritids[3], botleft);
        set_sprite_prop(enemy->spritids[3], get_sprite_prop(7) & ~S_FLIPX);
        if (enemy->down == -1) {
            enemy->down = 1;
        } else {
            enemy->down -= 1;
        }
    }
}

int enemyback(ValerioCharacter* enemy) {
    // change sprites for enemy

    if (enemy->type == 1) {
        topright = 18;
        topleft = 19;
        botright = 20;
        botleft = 21;
    } else if (enemy->type == 2) {
        topright = 32;
        topleft = 33;
        botright = 34;
        botleft = 35;
    } else if (enemy->type == 3) {
        topright = 44;
        topleft = 45;
        botright = 46;
        botleft = 47;
    }

    if (enemy->up > 0) {
        set_sprite_tile(enemy->spritids[0], topleft);
        set_sprite_prop(enemy->spritids[0], S_FLIPX);
        set_sprite_tile(enemy->spritids[1], topright);
        set_sprite_prop(enemy->spritids[1], S_FLIPX);
        set_sprite_tile(enemy->spritids[2], botleft);
        set_sprite_prop(enemy->spritids[2], S_FLIPX);
        set_sprite_tile(enemy->spritids[3], botright);
        set_sprite_prop(enemy->spritids[3], S_FLIPX);
        if (enemy->up == 2) {
            enemy->up = 0;
        } else {
            enemy->up += 1;
        }
    } else {
        set_sprite_tile(enemy->spritids[0], topright);
        set_sprite_prop(enemy->spritids[0], get_sprite_prop(4) & ~S_FLIPX);
        set_sprite_tile(enemy->spritids[1], topleft);
        set_sprite_prop(enemy->spritids[1], get_sprite_prop(5) & ~S_FLIPX);
        set_sprite_tile(enemy->spritids[2], botright);
        set_sprite_prop(enemy->spritids[2], get_sprite_prop(6) & ~S_FLIPX);
        set_sprite_tile(enemy->spritids[3], botleft);
        set_sprite_prop(enemy->spritids[3], get_sprite_prop(7) & ~S_FLIPX);
        if (enemy->up == -1) {
            enemy->up = 1;
        } else {
            enemy->up -= 1;
        }
    }
}

int enemyleft(ValerioCharacter* enemy) {
    // change sprites for enemy

    if (enemy->type == 1) {
        topright = 22;
        topleft = 23;
        botright = 24;
        botleft = 25;
        botrightalt = 26;
        botleftalt = 27;
    } else if (enemy->type == 2) {
        topright = 36;
        topleft = 37;
        botright = 38;
        botleft = 39;
        botrightalt = 38;
        botleftalt = 39;
    } else if (enemy->type == 3) {
        topright = 48;
        topleft = 49;
        botright = 50;
        botleft = 51;
        botrightalt = 52;
        botleftalt = 53;
    }

    if (enemy->left > 0) {
        set_sprite_tile(enemy->spritids[0], topleft);
        set_sprite_prop(enemy->spritids[0], S_FLIPX);
        set_sprite_tile(enemy->spritids[1], topright);
        set_sprite_prop(enemy->spritids[1], S_FLIPX);
        set_sprite_tile(enemy->spritids[2], botleft);
        set_sprite_prop(enemy->spritids[2], S_FLIPX);
        set_sprite_tile(enemy->spritids[3], botright);
        set_sprite_prop(enemy->spritids[3], S_FLIPX);
        if (enemy->left == 2) {
            enemy->left = 0;
        } else {
            enemy->left += 1;
        }
    } else {
        set_sprite_tile(enemy->spritids[0], topleft);
        set_sprite_prop(enemy->spritids[0], S_FLIPX);
        set_sprite_tile(enemy->spritids[1], topright);
        set_sprite_prop(enemy->spritids[1], S_FLIPX);
        set_sprite_tile(enemy->spritids[2], botleftalt);
        set_sprite_prop(enemy->spritids[2], S_FLIPX);
        set_sprite_tile(enemy->spritids[3], botrightalt);
        set_sprite_prop(enemy->spritids[3], S_FLIPX);
        if (enemy->left == -1) {
            enemy->left = 1;
        } else {
            enemy->left -= 1;
        }
    }
}

int enemyright(ValerioCharacter* enemy) {
    // change sprites for enemy

    if (enemy->type == 1) {
        topright = 22;
        topleft = 23;
        botright = 24;
        botleft = 25;
        botrightalt = 26;
        botleftalt = 27;
    } else if (enemy->type == 2) {
        topright = 36;
        topleft = 37;
        botright = 38;
        botleft = 39;
        botrightalt = 38;
        botleftalt = 39;
    } else if (enemy->type == 3) {
        topright = 48;
        topleft = 49;
        botright = 50;
        botleft = 51;
        botrightalt = 52;
        botleftalt = 53;
    }

    if (enemy->right > 0) {
        set_sprite_tile(enemy->spritids[0], topright);
        set_sprite_prop(enemy->spritids[0], get_sprite_prop(4) & ~S_FLIPX);
        set_sprite_tile(enemy->spritids[1], topleft);
        set_sprite_prop(enemy->spritids[1], get_sprite_prop(5) & ~S_FLIPX);
        set_sprite_tile(enemy->spritids[2], botright);
        set_sprite_prop(enemy->spritids[2], get_sprite_prop(6) & ~S_FLIPX);
        set_sprite_tile(enemy->spritids[3], botleft);
        set_sprite_prop(enemy->spritids[3], get_sprite_prop(7) & ~S_FLIPX);
        if (enemy->right == 2) {
            enemy->right = 0;
        } else {
            enemy->right += 1;
        }
    } else {
        set_sprite_tile(enemy->spritids[0], topright);
        set_sprite_prop(enemy->spritids[0], get_sprite_prop(4) & ~S_FLIPX);
        set_sprite_tile(enemy->spritids[1], topleft);
        set_sprite_prop(enemy->spritids[1], get_sprite_prop(5) & ~S_FLIPX);
        set_sprite_tile(enemy->spritids[2], botrightalt);
        set_sprite_prop(enemy->spritids[2], get_sprite_prop(6) & ~S_FLIPX);
        set_sprite_tile(enemy->spritids[3], botleftalt);
        set_sprite_prop(enemy->spritids[3], get_sprite_prop(7) & ~S_FLIPX);
        if (enemy->right == -1) {
            enemy->right = 1;
        } else {
            enemy->right -= 1;
        }
    }
}

void enemymove(ValerioCharacter* enemy) {
    int direction;
    if (stage > 0) {
        direction = random(4, 1);
        if (direction == 1) {
            if (canplayermove(enemy->x - 4, enemy->y, enemy)) {
                enemy->x -= 4;
                enemyleft(enemy);
                movegamecharacter(enemy, enemy->x, enemy->y);
            }
        } else if (direction == 2) {
            if (canplayermove(enemy->x + 4, enemy->y, enemy)) {
                enemy->x += 4;
                enemyright(enemy);
                movegamecharacter(enemy, enemy->x, enemy->y);
            }
        } else if (direction == 3) {
            if (canplayermove(enemy->x, enemy->y - 4, enemy)) {
                enemy->y -= 4;
                enemyback(enemy);
                movegamecharacter(enemy, enemy->x, enemy->y);
            }
        } else if (direction == 4) {
            if (canplayermove(enemy->x, enemy->y + 4, enemy)) {
                enemy->y += 4;
                enemyfront(enemy);
                movegamecharacter(enemy, enemy->x, enemy->y);
            }
        }
    }
}

void start() {
    set_bkg_data(0, 110, logo_data);
    set_bkg_tiles(0, 0, 20, 18, logo_map);

    SHOW_BKG;
    DISPLAY_ON;

    performantdelay(30);

    fadeout();

    set_bkg_data(0, 45, titlescreen_data);
    set_bkg_tiles(0, 0, 20, 18, titlescreen_map);

    fadein();

    while (!joypad()) {seed++; if (seed >= 255) {seed=1;}}
    waitpad(J_START);
    initrand(seed);
}

void pause() {
    DISPLAY_OFF;
    HIDE_SPRITES;
    set_bkg_data(0, 36, pause_data);
    set_bkg_tiles(0, 0, 20, 18, pause_map);
    DISPLAY_ON;

    waitpad(J_START);

    DISPLAY_OFF;
    SHOW_SPRITES;
    set_bkg_data(0, 97, cave_data);
    set_bkg_tiles(0, 0, 20, 18, cave_map);
    DISPLAY_ON;
}

void main() {
    start();
    score = 0;
    dmg = 0;

    set_bkg_data(0, 97, cave_data);
    set_bkg_tiles(0, 0, 20, 18, cave_map);

    set_sprite_data(0, 55, ValerioSprites);
    setupvalerio();

    SHOW_SPRITES;
    DISPLAY_ON;

    while (1) {
        attack = 0;
        block = 0;

        if (stage == 1) {
            valerio.x = 80;
            valerio.y = 120;
            setupenemy(&enemy_1, 1);
            setupenemy(&enemy_2, 2);
            setupenemy(&enemy_3, 3);
            setupenemy(&enemy_4, 4);
            setupenemy(&enemy_5, 5);
            stage = 2;
        }

        if (joypad() & J_A) {attack = 1;
        } else if (joypad() & J_B) {
            block = 1;}

        if (joypad() & J_SELECT) {pause();
        } else if (joypad() & J_START) {printf("Score = %d \n", score);;}

        if (joypad() & J_LEFT) {
            if (canplayermove(valerio.x - 2, valerio.y, &valerio)) {
                valerio.x -= 2;
                valerioleft();
                movegamecharacter(&valerio, valerio.x, valerio.y);
           }
        } else if (joypad() & J_RIGHT) {
            if (canplayermove(valerio.x + 2, valerio.y, &valerio)) {
                valerio.x += 2;
                valerioright();
                movegamecharacter(&valerio, valerio.x, valerio.y);
            }
        } else if (joypad() & J_UP) {
            if (canplayermove(valerio.x, valerio.y - 2, &valerio)) {
                valerio.y -= 2;
                valerioback();
                movegamecharacter(&valerio, valerio.x, valerio.y);
            }
        } else if (joypad() & J_DOWN) {
            if (canplayermove(valerio.x, valerio.y + 2, &valerio)) {
                valerio.y += 2;
                valeriofront();
                movegamecharacter(&valerio, valerio.x, valerio.y);
            }
        }

        if (enemy_1.dead == 0) {enemymove(&enemy_1);
        } else if (enemy_1.dead == 10) {setupenemy(&enemy_1, 1);
        } else {enemy_1.dead += 1;}
        if (enemy_2.dead == 0) {enemymove(&enemy_2);
        } else if (enemy_2.dead == 10) {setupenemy(&enemy_2, 2);
        } else {enemy_2.dead += 1;}
        if (enemy_3.dead == 0) {enemymove(&enemy_3);
        } else if (enemy_3.dead == 10) {setupenemy(&enemy_3, 3);
        } else {enemy_3.dead += 1;}
        if (enemy_4.dead == 0) {enemymove(&enemy_4);
        } else if (enemy_4.dead == 10) {setupenemy(&enemy_4, 4);
        } else {enemy_4.dead += 1;}
        if (enemy_5.dead == 0) {enemymove(&enemy_5);
        } else if (enemy_5.dead == 10) {setupenemy(&enemy_5, 5);
        } else {enemy_5.dead += 1;}

        performantdelay(5);
    }
}
