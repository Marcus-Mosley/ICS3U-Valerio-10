// Copyright (c) 2020 Marcus A. Mosley All rights reserved.
//
// Created by Marcus A. Mosley
// Created on January 2020
// This program prints the score

#include <gb/gb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <font.c>
#include <score_map.c>

int count;
int length;
int scorestr[];

extern int score;


void prntscore(int score) {
    int tile;
    int digit;

    DISPLAY_OFF;
    set_bkg_data(0, 38, font);
    set_bkg_tiles(0, 0, 20, 18, score_map);
    DISPLAY_ON;

    set_sprite_data(0, 38, font);

    for (count = 0; count <= 23; count++) {
        set_sprite_tile(count, 0);
        move_sprite(count, 152, 136);
    }

    length = 0;
    while (score != 0) {
        digit = score % 10;
        scorestr[length] = digit;
        score = score / 10;
        length++;
    }
    for (count = length; count >= 0; count--) {
        tile = scorestr[count - 1] + 1;
        set_sprite_tile(count, tile);
        set_sprite_prop(count, get_sprite_prop(count) & ~S_FLIPX);
        move_sprite(count, (length - count + 1) * 8, 24);
    }
    set_sprite_tile(0, 1);
}
