#ifndef GAME_ADDRESSES_H
#define GAME_ADDRESSES_H

#ifdef __APPLE__
#define MAC(va) ((unsigned)((va) - 0x100000000ULL))
#define ADDR(win, mac) MAC(mac)
#else
#define ADDR(win, mac) (win)
#endif
#define ADDR_DIFFERS(win, mac) ADDR(win, mac)

#ifdef __APPLE__
#define FRAME_OFFSET(win, mac) (mac)
#else
#define FRAME_OFFSET(win, mac) (win)
#endif

#define MAC_INLINED            no_mac_address__the_function_is_copied_into_its_callers
#define MAC_NO_CALL            no_mac_address__this_call_is_not_made_on_mac
#define MAC_IN_CODE            no_mac_address__the_value_is_built_into_the_instructions
#define MAC_STORED_DIFFERENTLY no_mac_address__mac_keeps_this_data_another_way
#define MAC_FILE_FORMAT        no_mac_address__part_of_the_windows_file_format
#define MAC_NOT_FOUND          no_mac_address__searched_for_and_not_found
#define MAC_NOT_LOOKED_UP      no_mac_address__not_searched_for_yet
#define WIN_NO_EQUIVALENT      no_windows_address__only_the_mac_build_has_this
#define WIN_FILE_FORMAT        no_windows_address__part_of_the_mac_file_format
#define WIN_NOT_LOOKED_UP      no_windows_address__not_searched_for_yet
#define WIN_NOT_FOUND          no_windows_address__searched_for_and_not_found
#define WIN_INLINED            no_windows_address__the_function_is_copied_into_its_callers
#define WIN_NO_CALL            no_windows_address__this_call_is_not_made_on_windows
#define WIN_IN_CODE            no_windows_address__the_value_is_built_into_the_instructions
#define WIN_STORED_DIFFERENTLY no_windows_address__windows_keeps_this_data_another_way

// ---- Built-in map rendering and frame controls (5.43, both builds) ------------------------------------
#define RVA_FPS_HIGH_DECOR_DRAW                ADDR(        0x1ab380,          MAC_INLINED)
#define RVA_FPS_HIGH_DECOR_CALL                ADDR(        0x19d621,          MAC_NO_CALL)
#define RVA_FPS_HIGH_DECOR_SPRITE_CALL_A       ADDR(        0x1ab7c1,          0x10049e75c)
#define RVA_FPS_HIGH_DECOR_SPRITE_CALL_B       ADDR(        0x1ab9f1,          0x10049e7f4)
#define RVA_FPS_LOW_DECOR_SPRITE_CALL          ADDR(        0x1ab2c2,          0x10049e00c)
#define RVA_FPS_GRAPHICS_SECTION_END_CALL      ADDR(        0x197dff,          0x10032c6a0)
#define RVA_FPS_PREFERENCES_WAIT_CALL          ADDR(        0x19833d,          0x10032cce8)
#define RVA_FPS_FRAME_CHOICES_CALL             ADDR(        0x197d20,          0x10032cb80)
#define RVA_FPS_GPU_WAIT                       ADDR_DIFFERS(0x58243ac,          0x106ceedec)
#define RVA_FPS_GET_TICK_COUNT_IAT             ADDR_DIFFERS(0x33a268,          0x1005ae500)
#define RVA_FPS_SLEEP_IAT                      ADDR_DIFFERS(0x33a278,          0x1005ae8fc)
#define RVA_FPS_CLOCK_CALL_A                   ADDR(        0x23f23f,          0x100487988)
#define RVA_FPS_CLOCK_CALL_B                   ADDR(        0x2406ec,          0x10048c554)
#define RVA_FPS_SLEEP_CALL_A                   ADDR(        0x23f27a,          0x100487a08)
#define RVA_FPS_SLEEP_CALL_B                   ADDR(        0x240727,          0x10048c5d0)
#define RVA_FPS_CLOCK_RETURN_A                 ADDR(        0x23f245,          0x10048798c)
#define RVA_FPS_CLOCK_RETURN_B                 ADDR(        0x2406f2,          0x10048c558)
#define RVA_FPS_SLEEP_RETURN_A                 ADDR(        0x23f280,          0x100487a0c)
#define RVA_FPS_SLEEP_RETURN_B                 ADDR(        0x24072d,          0x10048c5d4)
#define RVA_FPS_GL_BEGIN_IAT                   ADDR_DIFFERS(0x33a3e0,          0x1005ae518)
#define RVA_FPS_GL_END_IAT                     ADDR_DIFFERS(0x33a410,          0x1005ae5cc)
#define RVA_FPS_GL_BIND_TEXTURE_IAT            ADDR_DIFFERS(0x33a4b0,          0x1005ae524)
#define RVA_FPS_GL_COLOR3F_IAT                 ADDR_DIFFERS(0x33a388,          0x1005ae554)
#define RVA_FPS_GL_ENABLE_IAT                  ADDR_DIFFERS(0x33a4a8,          0x1005ae5b4)
#define RVA_FPS_GL_DISABLE_IAT                 ADDR_DIFFERS(0x33a458,          0x1005ae590)
#define RVA_FPS_GL_BLEND_FUNC_IAT              ADDR_DIFFERS(0x33a498,          0x1005ae530)
#define RVA_FPS_GL_ALPHA_FUNC_IAT              ADDR_DIFFERS(0x33a488,          0x1005ae50c)
#define RVA_FPS_GL_DEPTH_MASK_IAT              ADDR_DIFFERS(0x33a4a0,          0x1005ae584)
#define RVA_FPS_GL_TEX_ENVI_IAT                ADDR_DIFFERS(0x33a468,          0x1005ae710)
#define RVA_FPS_GL_TEX_COORD2F_IAT             ADDR_DIFFERS(0x33a400,          0x1005ae6f8)
#define RVA_FPS_GL_VERTEX3F_IAT                ADDR_DIFFERS(0x33a420,          0x1005ae740)
#define RVA_FPS_GL_ORTHO_IAT                   ADDR_DIFFERS(0x33a328,          0x1005ae698)
#define RVA_FPS_GL_GET_ERROR_IAT               ADDR_DIFFERS(0x33a330,          0x1005ae620)
#define RVA_FPS_GL_FINISH_IAT                  ADDR_DIFFERS(0x33a338,          0x1005ae5d8)
#define RVA_FPS_GL_LOAD_IDENTITY_IAT           ADDR_DIFFERS(0x33a340,          0x1005ae674)
#define RVA_FPS_GL_MATRIX_MODE_IAT             ADDR_DIFFERS(0x33a348,          0x1005ae68c)
#define RVA_FPS_GL_FOGFV_IAT                   ADDR_DIFFERS(0x33a350,          0x1005ae5fc)
#define RVA_FPS_GL_FOGF_IAT                    ADDR_DIFFERS(0x33a358,          0x1005ae5f0)
#define RVA_FPS_GL_CLEAR_IAT                   ADDR_DIFFERS(0x33a360,          0x1005ae53c)
#define RVA_FPS_GL_PIXEL_STOREI_IAT            ADDR_DIFFERS(0x33a368,          0x1005ae6a4)
#define RVA_FPS_GL_TEX_IMAGE2_D_IAT            ADDR_DIFFERS(0x33a370,          0x1005ae71c)
#define RVA_FPS_GL_TEX_PARAMETERI_IAT          ADDR_DIFFERS(0x33a378,          0x1005ae728)
#define RVA_FPS_GL_VIEWPORT_IAT                ADDR_DIFFERS(0x33a398,          0x1005ae758)
#define RVA_FPS_GL_PUSH_MATRIX_IAT             ADDR_DIFFERS(0x33a3b0,          0x1005ae6bc)
#define RVA_FPS_GL_SCALEF_IAT                  ADDR_DIFFERS(0x33a3b8,          0x1005ae6d4)
#define RVA_FPS_GL_ROTATEF_IAT                 ADDR_DIFFERS(0x33a3c0,          0x1005ae6c8)
#define RVA_FPS_GL_TRANSLATEF_IAT              ADDR_DIFFERS(0x33a3c8,          0x1005ae734)
#define RVA_FPS_GL_TEX_COORD_POINTER_IAT       ADDR_DIFFERS(0x33a3d8,          0x1005ae704)
#define RVA_FPS_GL_DELETE_TEXTURES_IAT         ADDR_DIFFERS(0x33a3e8,          0x1005ae578)
#define RVA_FPS_GL_SCISSOR_IAT                 ADDR_DIFFERS(0x33a408,          0x1005ae6e0)
#define RVA_FPS_GL_FLUSH_IAT                   ADDR_DIFFERS(0x33a418,          0x1005ae5e4)
#define RVA_FPS_GL_POP_MATRIX_IAT              ADDR_DIFFERS(0x33a430,          0x1005ae6b0)
#define RVA_FPS_GL_LIGHT_MODELI_IAT            ADDR_DIFFERS(0x33a438,          0x1005ae650)
#define RVA_FPS_GL_SHADE_MODEL_IAT             ADDR_DIFFERS(0x33a440,          0x1005ae6ec)
#define RVA_FPS_GL_MATERIALFV_IAT              ADDR_DIFFERS(0x33a448,          0x1005ae680)
#define RVA_FPS_GL_LIGHTFV_IAT                 ADDR_DIFFERS(0x33a450,          0x1005ae65c)
#define RVA_FPS_GL_COLOR_MATERIAL_IAT          ADDR_DIFFERS(0x33a470,          0x1005ae56c)
#define RVA_FPS_GL_FOGI_IAT                    ADDR_DIFFERS(0x33a478,          0x1005ae608)
#define RVA_FPS_GL_CLEAR_COLOR_IAT             ADDR_DIFFERS(0x33a480,          0x1005ae548)
#define RVA_FPS_GL_HINT_IAT                    ADDR_DIFFERS(0x33a490,          0x1005ae644)

// ---- Image layout -----------------------------------------------------------------------------------
#define RVA_TEXT_SECTION                       ADDR(        0x001000,          MAC_FILE_FORMAT)
#define RVA_RDATA_SECTION                      ADDR(        0x33a000,          MAC_FILE_FORMAT)
#define RVA_GLU_BUILD_2D_MIPMAPS               ADDR_DIFFERS(0x33a000,          0x1005ae764)
#define RVA_DATA_SECTION                       ADDR(        0x42c000,          MAC_FILE_FORMAT)
#define RVA_PDATA_SECTION                      ADDR(        0x136f2000,        MAC_FILE_FORMAT)
#define RVA_DATA_CONST_SEGMENT                 ADDR(        WIN_FILE_FORMAT,   0x100774000)
#define RVA_DATA_SEGMENT                       ADDR(        WIN_FILE_FORMAT,   0x10084c000)
#define RVA_LINKEDIT_SEGMENT                   ADDR(        WIN_FILE_FORMAT,   0x11399c000)
#define RVA_TEXT_CODE                          ADDR(        WIN_FILE_FORMAT,   0x100002d60)
#define RVA_STUBS                              ADDR(        WIN_FILE_FORMAT,   0x1005ad8d0)
#define RVA_HEADER_PADDING                     ADDR(        WIN_FILE_FORMAT,   0x1000013f0)

// ---- Main menu --------------------------------------------------------------------------------------
#define RVA_MAIN_MENU                          ADDR(        0x191370,          0x10032d974)
#define RVA_MENU_PREFERENCES                   ADDR(        0x197670,          0x10032b748)
#define RVA_NETWORK_SCREEN                     ADDR(        0x1f1480,          MAC_INLINED)
#define RVA_MENU_ID5                           ADDR(        0x1b6dc0,          0x1005146bc)
#define RVA_CREDITS                            ADDR(        0x0b65a0,          MAC_INLINED)
#define RVA_TEST_GAME                          ADDR(        WIN_NO_CALL,       MAC_INLINED)
#define RVA_READ_TIP                           ADDR(        0x23ec10,          MAC_INLINED)
#define RVA_READ_TIP_2                         ADDR(        0x23e490,          MAC_INLINED)
#define RVA_COUNT_SAVES                        ADDR(        0x18ac90,          0x1004eafdc)
#define RVA_MENU_LOOP                          ADDR_DIFFERS(0x191482,          0x10032da88)
#define RVA_MODS_SCREEN_WAIT                   ADDR(        0x1ca10f,          0x10032ed78)
#define RVA_NETWORK_SCREEN_WAIT                ADDR(        0x1f1749,          0x10032e43c)
#define RVA_MENU_FADE_ON                       ADDR(        0x1a425ec,         0x100872bec)
#define RVA_FRAME_DELTA                        ADDR(        0xd63ec3c,         0x10c922c48)
#define RVA_MENU_LABEL_SIZE                    ADDR(        0x406d44,          MAC_IN_CODE)
#define RVA_MENU_X_OFFSET                      ADDR(        0x406d94,          MAC_IN_CODE)
#define RVA_MENU_Y_STEP                        ADDR(        0x406f0c,          MAC_IN_CODE)
#define RVA_MENU_BUTTON_W                      ADDR(        0x406dc4,          MAC_IN_CODE)
#define RVA_MENU_BG_INSET                      ADDR(        0x406cd8,          MAC_IN_CODE)
#define RVA_FLOAT_ONE                          ADDR(        0x4069c4,          MAC_IN_CODE)
#define RVA_DEFAULT_COLOUR                     ADDR(        0x406e8c,          MAC_IN_CODE)
#define RVA_FADE_START                         ADDR(        0x406e78,          MAC_IN_CODE)
#define RVA_FADE_RATE                          ADDR(        0x406988,          MAC_IN_CODE)
#define RVA_FADE_CAP                           ADDR(        0x406ca8,          MAC_IN_CODE)
#define RVA_QUIT_FADE_DELAY                    ADDR(        0x406cb0,          MAC_IN_CODE)
#define RVA_BG_FADE_START                      ADDR(        0x406e58,          MAC_IN_CODE)
#define RVA_BG_FADE_RATE                       ADDR(        0x406918,          MAC_IN_CODE)
#define RVA_BG_FADE_CAP                        ADDR(        0x406ab0,          MAC_IN_CODE)
#define RVA_MENU_FLOAT4                        ADDR(        WIN_NO_EQUIVALENT, 0x1007578f0)
#define RVA_MAIN_MENU_WAIT_PATTERN             ADDR(        WIN_NOT_FOUND,     MAC_FILE_FORMAT)
#define RVA_NETWORK_WAIT_PATTERN               ADDR(        WIN_NOT_FOUND,     MAC_FILE_FORMAT)
#define RVA_MENU_BG_FADE_STEP                  ADDR(        0x1915a3,          0x10032dbf0)
#define RVA_MENU_FADE_TIMER_STEP               ADDR(        0x1915da,          0x10032dc18)
#define RVA_MENU_FADE_RELOAD                   ADDR(        WIN_NO_EQUIVALENT, 0x10032dd54)
#define RVA_MENU_BANNER_FADE_SET               ADDR(        0x192100,          0x10032e0d4)
#define RVA_MENU_LOOP_JUMP                     ADDR(        0x19210f,          0x10032e0d8)
#define RVA_MENU_PREFS_CALL                    ADDR(        0x19215e,          0x10032e1b0)
#define RVA_MENU_TEST_GAME                     ADDR_DIFFERS(0x1921b3,          0x10032e774)
#define RVA_MENU_READ_TIP                      ADDR_DIFFERS(0x1921ff,          0x10032ef04)
#define RVA_CREDITS_LOGO_CALL                  ADDR(        0x0b6705,          0x10032e618)
#define RVA_QUIT_MESSAGE                       ADDR(        0x5822fb2,         MAC_NOT_FOUND)

// ---- Screens and widgets ------------------------------------------------------------------------
#define RVA_WAIT_FOR_CLICK                     ADDR(        0x23ec30,          0x100485000)
#define RVA_DRAW_BUTTON                        ADDR(        0x0363a0,          0x10033f5bc)
#define RVA_DRAW_BUTTON_PLAIN                  ADDR(        0x036120,          0x10033fa1c)
#define RVA_DRAW_FRAME_IMAGE                   ADDR_DIFFERS(0x036cc0,          0x10033ef8c)
#define RVA_DRAW_IMAGE                         ADDR(        0x0370c0,          0x10033ec34)
#define RVA_SCREEN_W                           ADDR(        0x435d1c,          0x104904c78)
#define RVA_SCREEN_H                           ADDR(        0x431824,          0x104904c7c)
#define RVA_DATA_DIR                           ADDR(        0x5828b30,         0x10c922038)

#define RVA_SCREEN_BEGIN                       ADDR(        0x03ae60,          0x10033bd68)
#define RVA_SCREEN_TITLE                       ADDR(        0x033610,          0x1003418f4)
#define RVA_SECTION_BEGIN                      ADDR(        0x035560,          0x100340564)
#define RVA_SECTION_END                        ADDR(        0x035400,          0x1003408a8)
#define RVA_SECTION_HEADER                     ADDR(        0x089600,          0x10048fe4c)
#define RVA_RADIO_OPTION                       ADDR(        0x0389a0,          0x10033def4)
#define RVA_TEXT_BUTTON                        ADDR(        0x039ad0,          0x10033d0c8)
#define RVA_BIND_HOTKEY                        ADDR(        0x03aae0,          0x10033c76c)
#define RVA_BIND_HOTKEY_CTRL                   ADDR(        0x03a940,          0x10033c890)
#define RVA_BIND_HOTKEY_ALT                    ADDR(        0x03a8b0,          MAC_INLINED)
#define RVA_CHECKBOX                           ADDR(        0x038d50,          0x10033d8a0)
#define CHECKBOX_ID_GREYED                     (-3)
#define CHECKBOX_ID_OFF                        (-2)
#define RVA_CHECKBOX_GREYED_ID_CHECK           ADDR(        0x038e87,          MAC_NOT_FOUND)
#define RVA_CHECKBOX_GREYED_ALPHA              ADDR(        0x406900,          MAC_NOT_FOUND)
#define CHECKBOX_GREYED_ALPHA                  0.5f
#define RVA_HIT_TEST_CHECKBOX_OFF              ADDR(        0x02bb0b,          0x1004875e0)
#define RVA_TOOLTIP                            ADDR(        0x039180,          0x10033d66c)
#define RVA_OPTION_TOOLTIP                     ADDR(        0x039350,          0x10033d550)
#define RVA_TEXT                               ADDR(        0x038f60,          0x10033d804)
#define SCREEN_TITLE_TEXT_SIZE                 0x64
#define TEXT_BUTTON_TEXT_SIZE                  0x1388
#define TEXT_LINE_TEXT_SIZE                    0x1388
#define TOOLTIP_TEXT_SIZE                      0x1388
#define CHECKBOX_TEXT_SIZE                     0x3e8
#define RADIO_OPTION_TEXT_SIZE                 0x3e7
#define PROGRESS_TEXT_TEXT_SIZE                0x1388
#define RVA_LAYOUT_NEW                         ADDR(        0x03a3f0,          0x10033c998)
#define RVA_LAYOUT_PASS                        ADDR(        0x0304e0,          0x10034198c)
#define RVA_GUI_ALLOC                          ADDR(        0x03ac50,          0x10033c5f4)
#define RVA_TRANSLATE_FORMAT                   ADDR(        0x03d200,          0x10033b4a8)
#define RVA_SCREEN_WIDTH                       ADDR(        0x03a6d0,          MAC_INLINED)
#define RVA_BUTTON_ROW                         ADDR(        0x038490,          MAC_INLINED)
#define RVA_SCREEN_LAYOUT                      ADDR(        0x0304c0,          MAC_INLINED)
#define RVA_CLICK_SOUND                        ADDR(        0x0b0bd0,          MAC_INLINED)
#define RVA_SCREEN_MUSIC                       ADDR(        0x034c40,          MAC_INLINED)
#define RVA_ESCAPE_KEY                         ADDR_DIFFERS(0x12ab0222,        0x113997cf6)
#define RVA_DISABLED_BUTTON                    ADDR(        0x0396b0,          0x10033d3dc)
#define RVA_LINE_ADVANCE                       ADDR(        0x03a500,          MAC_INLINED)
#define RVA_DRAW_SPRITE                        ADDR_DIFFERS(0x035da0,          0x10033fd14)
#define RVA_LOAD_SPRITE                        ADDR_DIFFERS(0x0643c0,          0x10034e7e0)
#define RVA_SPRITE_BUTTON                      ADDR(        0x038380,          0x10033df90)
#define RVA_ICON_BUTTON                        ADDR(        0x035a50,          0x10033ff88)
#define RVA_SPRITE_BUTTON_TINTED               ADDR(        0x0382e0,          MAC_INLINED)
#define RVA_DRAW_SPRITE_40                     ADDR(        0x035b70,          MAC_INLINED)
#define RVA_DRAW_SPRITE_30                     ADDR(        0x035b90,          MAC_INLINED)
#define SPRITE_KIND_MONSTER                    0x21
#define SPRITE_KIND_TERRAIN                    0x20
#define SPRITE_KIND_ITEM                       2
#define SPRITE_KIND_RES                        0xa
#define RES_TRS_SPRITES                        668
#define SPRITE_KIND_COUNT                      0x26
#define SPRITES_PER_KIND                       0x7fbc
#define RVA_SPRITE_DRAW_SCALED                 ADDR(        0x0651a0,          0x10034f3a8)
#define RVA_LOAD_SPRITE_FILE                   ADDR(        0x0648b0,          0x10034e15c)
#define RVA_SPRITE_FILES                       ADDR(        0x5ea8420,         0x1062a5800)
#define RVA_SPRITE_REPLACED                    ADDR(        0x57f2f80,         0x1062a5930)
#define RVA_REPLACED_SPRITE                    ADDR(        0x064c20,          MAC_INLINED)
#define RVA_LOAD_SPRITE_FSA_EXIT               ADDR(        0x064727,          0x10034f2d0)
#define RVA_LOAD_SPRITE_COUNT_BRANCH           ADDR(        0x0646b9,          0x10034ecc4)
#define RVA_LOAD_SPRITE_FSA_PATH               ADDR(        0x0646bf,          0x10034f260)
#define RVA_LOAD_SPRITE_TRS_READ               ADDR(        0x06473b,          0x10034ecc8)
#define RVA_LOAD_SPRITE_SLOT_RETURN            ADDR_DIFFERS(0x06464e,          0x10034f180)
#define RVA_RESET_MODS_SPRITE_WIPE             ADDR(        0x1ca8d8,          0x100529178)
#define RVA_CLEAR_REPLACED_SPRITES             ADDR(        0x063620,          MAC_NOT_FOUND)
#define RVA_SPRITE_WIDTHS                      ADDR(        0x5c0eed0,         0x1062a6424)
#define RVA_SPRITE_HEIGHTS                     ADDR(        0x5ea8550,         0x106504ff4)
#define RVA_TOKEN_ITEM_ICON_CALL               ADDR(        0x0fe977,          0x10057ea00)
#define RVA_BATTLE_TOKEN_DRAW                  ADDR(        0x0fd1c0,          0x10057d590)
#define SPRITE_KIND_ICON                       0x1b
#define SPRITE_KIND_ITEM                       2
#define LAYOUT_TYPE_SPRITE_BUTTON              7
#define LAYOUT_TYPE_SPRITE                     0x14
#define LAYOUT_TYPE_ICON_BUTTON                0x2b
#define LAYOUT_TYPE_ITEM_GAP                   0x2e
#define ITEM_CARD_GAP_WIDTH                    2.0f
#define ITEM_TYPE_ICON_BASE                    0x34
#define RVA_MONSTER_PICKER                     ADDR(        0x087f10,          MAC_INLINED)
#define RVA_RECRUIT_ROW                        ADDR(        0x108970,          MAC_INLINED)
#define RVA_SETUP_WIDTH_SCALE                  ADDR(        0x1a5ab2c,         0x104904ca4)
#define RVA_SETUP_WIDTH_UNIT                   ADDR(        0x406df8,          MAC_IN_CODE)
#define RVA_ESCAPE_ON                          RVA_ESCAPE_KEY
#define RVA_TEXT_ROW                           ADDR_DIFFERS(0x03a100,          0x10033cab4)
#define RVA_GUI_LAST_SLOT                      ADDR(        0x42dc24,          0x100872bf0)
#define RVA_LAYOUT_LAST                        ADDR(        0x436724,          0x100857618)
#define RVA_COLUMN_BASE                        ADDR(        0x42dcb4,          0x100858038)
#define RVA_TEXT_HEIGHT_BASE                   ADDR(        0x432218,          0x100862640)
#define RVA_SET_TEXT_SCALE                     ADDR(        0x03a660,          MAC_INLINED)
#define RVA_LINE_WIDTH                         ADDR(        0x57c630c,         0x10611b920)
#define RVA_WIDEST_LINE                        ADDR(        0x57ee7c8,         0x10611b918)
#define RVA_ROW_OPEN                           ADDR(        0x42dc8c,          0x10611b91c)
#define RVA_SCREEN_WIDTH_A                     ADDR(        0x436728,          0x100862684)
#define RVA_SCREEN_WIDTH_B                     ADDR(        0x437b18,          0x100862688)
#define RVA_GUI_NEXT_SLOT                      ADDR(        0x57ee7d0,         0x105b68178)
#define RVA_HOVERED_ID                         ADDR(        0x432204,          0x1008626ec)
#define RVA_TEXT_FIELD_FOCUS                   ADDR(        0x42e704,          0x1008626a0)
#define RVA_TEXT_FIELD_CURSOR                  ADDR(        0x57c591c,         0x10611b990)
#define RVA_LAYOUT_TOP_Y                       ADDR(        0x434974,          0x10085761c)
#define RVA_SCREEN_RESET_A                     ADDR(        0x435d20,          0x100872b64)
#define RVA_SCREEN_RESET_B                     ADDR(        0x430e54,          0x10086269c)
#define RVA_SCREEN_RESET_C                     ADDR(        0x42dcfc,          0x1008626a4)
#define RVA_SCREEN_RESET_BYTE                  ADDR(        WIN_NOT_FOUND,     0x106cee568)
#define RVA_LAYOUT_MARK_OPEN                   ADDR(        0x0297e0,          MAC_INLINED)
#define RVA_LAYOUT_MARK_CLOSE                  ADDR(        0x0297a0,          MAC_INLINED)
#define LAYOUT_TYPE_MARK                       0x67

#define RVA_ROW_STRIPE                         ADDR(        0x038590,          0x10033e530)
#define RVA_ROW_BEGIN                          ADDR(        0x03a5e0,          MAC_INLINED)
#define RVA_ROW_AREA                           ADDR(        0x035830,          MAC_INLINED)
#define RVA_COLUMN_X                           ADDR(        0x03a380,          MAC_INLINED)
#define RVA_LIST_GAP                           ADDR(        0x038530,          MAC_INLINED)
#define RVA_LIST_STYLE                         ADDR(        0x038490,          MAC_INLINED)

#define RVA_GUI_TABLE                          ADDR(        0x12ab2b20,        0x10b979b10)
#define GUI_STRIDE                             0x78
#define GUI_ELEMENT_Y                          0x08
#define GUI_ELEMENT_WIDTH                      0x0c
#define GUI_ELEMENT_TOOLTIP                    0x4c
#define GUI_ELEMENT_LABEL                      0x60
#define GUI_ELEMENT_HEIGHT                     0x10
#define GUI_ELEMENT_VALUE                      0x50
#define GUI_ELEMENT_KIND                       0x68
#define GUI_KIND_CHECKBOX                      3
#define RVA_LAYOUT_ELEMENTS                    ADDR(        0x1319abc0,        0x105ba4288)
#define LAYOUT_STRIDE                          0x70
#define LAYOUT_ELEMENT_COLUMN_X                0x0c
#define LAYOUT_ELEMENT_WIDTH                   0x10
#define LAYOUT_ELEMENT_TYPE                    0x60
#define LAYOUT_ELEMENT_LINE_END                0x6c
#define LAYOUT_ELEMENT_LABEL                   0x58
#define LAYOUT_ELEMENT_COLOUR                  0x14
#define LAYOUT_TYPE_LIST_STYLE                 8
#define LAYOUT_TYPE_BUTTON_ROW                 9
#define LAYOUT_TYPE_LIST_GAP                   10
#define LAYOUT_TYPE_COLUMN_X                   12
#define LAYOUT_TYPE_ROW_AREA                   15
#define LAYOUT_TYPE_OVERLAP                    14
#define LAYOUT_ELEMENT_OVERLAP                 0x24
#define SQUARE_PANEL_LINE_HEIGHT               15.0f
#define LAYOUT_ELEMENT_PAD                     0.1f
#define LAYOUT_TYPE_ROW_BEGIN_LIMIT            100
#define RVA_LAYOUT_TABLE                       RVA_LAYOUT_ELEMENTS
#define RVA_TOOLTIP_TABLE                      ADDR(        0x13190ac0,        0x10c057810)
#define RVA_TOOLTIP_COUNT                      ADDR(        0x433fa8,          0x100857620)
#define TOOLTIP_SLOTS                          5000
#define RVA_TOOLTIP_SHOWN                      ADDR(        0x42dd1c,          0x1008626f0)
#define RVA_TOOLTIP_DRAWN                      ADDR(        0x42dc54,          0x1008626f8)
#define RVA_TOOLTIP_SHOWN_AT                   ADDR(        0x583605c,         0x10611be6c)
#define RVA_TOOLTIP_FADE_ON                    ADDR(        0x42f0d8,          0x10085762c)
#define RVA_TOOLTIP_LINE_HEIGHT                ADDR(        0x42dbfc,          0x100862644)
#define TOOLTIP_BOX_BOTTOM                     16
#define TOOLTIP_BOX_DROP                       2
#define RVA_DRAW_TOOLTIP                       ADDR(        0x029ce0,          MAC_INLINED)
#define RVA_TOOLTIP_FADE_START                 ADDR(        0x406aa8,          MAC_IN_CODE)
#define RVA_TOOLTIP_FADE_RATE                  ADDR(        0x406948,          MAC_IN_CODE)
#define RVA_TOOLTIP_LINES                      ADDR(        0x123e9ba0,        0x1061a69f8)
#define TOOLTIP_LINE_SIZE                      0x180
#define RVA_TOOLTIP_LINE_WIDTHS                ADDR(        0x123e8c00,        0x1062045f8)
#define RVA_TEXT_WRAP                          ADDR(        0x053c50,          0x10034d7b8)
#define RVA_BUTTON_CORE                        ADDR(        0x039790,          0x10033d240)
#define RVA_TOOLTIP_STORE                      ADDR(        0x0393d2,          0x10033d59c)
#define RVA_DRAW_SPRITE_LOAD_CALL              ADDR(        0x035dd1,          0x10033fd54)
#define RVA_NAME_BOX_SPRITE_CALL               ADDR_DIFFERS(0x041d4e,          0x100349f9c)
#define RVA_FILE_BROWSER_FOLDER_ROW            ADDR(        0x040c54,          0x10034af8c)
#define RVA_FILE_BROWSER_HOVERED_READ          ADDR_DIFFERS(0x0411fa,          0x10034b568)

#define RVA_OPEN_GUI_FONT                      ADDR(        0x0535e0,          0x10034c19c)
#define RVA_FONT_HANDLES                       ADDR(        0x59daa70,         0x1061a67e8)
#define RVA_PUT_TEXT_IN_CACHE                  ADDR(        0x052680,          0x10034c938)
#define RVA_RENDER_TEXT_PIXELS                 ADDR(        WIN_NOT_FOUND,     MAC_NOT_FOUND)
#define RVA_TTF_RENDER_UTF8_BLENDED            ADDR_DIFFERS(0x33a690,          0x1000cf340)
#define RVA_DRAW_TEXT                          ADDR(        0x02f380,          MAC_INLINED)
#define RVA_RENDER_TEXT                        ADDR(        0x0544e0,          0x10034d6a8)
#define RVA_TEXT_MEASURE                       ADDR(        0x0521d0,          0x10034d3b0)
#define RVA_TEXT_COLOUR                        ADDR(        0x430480,          0x100857660)
#define RVA_TEXT_CELL_COLOUR                   ADDR(        WIN_INLINED,       MAC_INLINED)
#define RITUAL_GREYED_ALPHA                    0.5f

// ---- Dialogs and messages -----------------------------------------------------------------------
#define RVA_CONFIRM                            ADDR(        0x042b30,          0x1003483ec)
#define RVA_NAME_BOX                           ADDR(        0x041bb0,          0x100349cb8)
#define RVA_NAME_BOX_END                       ADDR(        0x041f42,          0x10034a1b8)
#define NAME_BOX_LENGTH_MAX                    42
#define NAME_BOX_PROMPT_SIZE                   0x1f4
#define RVA_MESSAGE_BOX                        ADDR(        0x041f50,          0x10034940c)
#define RVA_ENGLISH_TEXT_PASS                  ADDR(        0x05c8a0,          0x1003346f0)
#define RVA_FILE_BROWSER                       ADDR(        0x03f430,          0x10034a1b8)
#define RVA_FILE_BROWSER_LAST                  ADDR(        0x43922c,          0x100862700)
#define RVA_FILE_BROWSER_NAMES                 ADDR(        0x593afd0,         0x106124539)
#define RVA_FILE_BROWSER_FOLDERS               ADDR(        0x5992fd0,         0x106164539)
#define FILE_BROWSER_NAME_SIZE                 0x80
#define FILE_BROWSER_ROWS_MAX                  0x800
#define RVA_HOVERED_ELEMENT                    ADDR(        0x029cd0,          MAC_INLINED)
#define RVA_FIND_HOVERED                       ADDR(        0x02cac0,          0x10034790c)
#define RVA_ADD_MESSAGE                        ADDR(        0x1e13f0,          0x1004c2020)
#define RVA_MESSAGE_TABLE                      ADDR(        0xe4a54c0,         0x1085ba830)
#define MESSAGE_SIZE                           0x8a0
#define MESSAGE_TO                             0x0
#define ADD_MESSAGE_TEXT_SIZE                  0x800
#define MESSAGE_TITLE                          0x6
#define MESSAGE_TEXT                           0x6a
#define RVA_EVENT_MESSAGE_CALL                 ADDR(        0x1c6bf7,          0x10052be8c)

// ---- Game flow ----------------------------------------------------------------------------------
#define RVA_PLAY_GAME                          ADDR(        0x0917d0,          0x100389628)
#define RVA_NEW_WORLD                          ADDR(        0x302cd0,          0x10058708c)
#define RVA_MAP_SETTINGS_SCREEN                ADDR(        0x2a9f90,          0x1004b255c)
#define RVA_PROGRESS_TEXT                      ADDR(        0x089740,          0x1004901c8)
#define RVA_PROGRESS_LAST_DRAW                 ADDR(        0x58033b8,         0x106cf0180)
#define RVA_FRAME_DELAY_MS                     ADDR(        0x1a5324c,         0x104904cac)
#define RVA_RUN_ONE_MOD_PROGRESS_CALL          ADDR_DIFFERS(0x1bb2b1,          0x10052a1d4)
#define RVA_STR_CUSTOM_EVENTS                  ADDR(        0x1c611f8,         0x100706b56)
#define RVA_MAP_SCREEN                         ADDR(        0x0944c0,          0x10037afe8)
#define RVA_LOAD_GAME                          ADDR(        0x185820,          0x1004eb1ec)
#define RVA_RESET_GAME_STATE                   ADDR(        0x31c620,          0x100585a00)
#define RVA_RESET_SQUARES                      ADDR(        0x302a40,          MAC_NOT_LOOKED_UP)
#define RVA_LOAD_MAP                           ADDR(        0x1b5390,          0x100513d1c)
#define RVA_INIT_PARTICIPANTS                  ADDR(        0x2e9d10,          MAC_INLINED)
#define RVA_NETWORK_PARTICIPANTS               ADDR(        0x1ed1e0,          0x100502c58)
#define RVA_COMMAND_LINE_SWITCH                ADDR(        0x194bd0,          0x1003287d0)
#define RVA_SWITCH_DEBUG_LEVEL_ADD             ADDR(        0x18fb87,          0x100329288)
#define RVA_CREATE_LOBBY_GAME                  ADDR(        WIN_NOT_FOUND,     MAC_NOT_FOUND)
#define RVA_GAME_MENU                          ADDR(        0x123e70,          MAC_INLINED)
#define RVA_EXIT_GAME                          ADDR(        0x198d30,          0x10032b2ac)
#define RVA_GAME_START_PREP                    ADDR(        0x1b07a0,          0x100491d44)
#define RVA_GAME_START_PREP_DONE               ADDR_DIFFERS(0x582b264,         0x107dcb5cc)
#define RVA_GAME_START_ONCE_TEST               ADDR_DIFFERS(0x091b7b,          0x100491d74)
#define RVA_PLAYER_TURN_START                  ADDR(        0x167600,          0x100392658)
#define RVA_AI_MOVE                            ADDR_DIFFERS(0x06f9d0,          0x1004e0590)
#define RVA_AI_COMMANDER_RITUALS               ADDR(        0x06ae20,          0x1004dccfc)
#define RVA_AI_PLAN_TRACE_CALL                 ADDR(        0x06be3f,          0x1004de1cc)
#define RVA_AI_RITUAL_CANDIDATE_CALL           ADDR(        0x06bb37,          0x1004ddfe8)
#define RVA_AI_MOVE_ALL_COMMANDERS             ADDR(        0x070400,          0x1004d4750)
#define RVA_AI_COMMANDER_ROUTINE               ADDR(        0x07a840,          0x1004d18ec)
#define RVA_AI_RECRUIT                         ADDR(        0x0772d0,          MAC_INLINED)
#define RVA_AI_RECRUIT_CALL                    ADDR(        0x06fb2b,          MAC_INLINED)
#define RVA_AI_RECRUIT_COPY_START              ADDR(        WIN_NO_EQUIVALENT, 0x1004e0cb4)
#define RVA_AI_RECRUIT_COPY_END                ADDR(        WIN_NO_EQUIVALENT, 0x1004e38cc)
#define RVA_AI_CONQUER_STEP                    ADDR(        0x075310,          MAC_INLINED)
#define RVA_AI_EXPLORE_STEP                    ADDR(        0x074260,          MAC_INLINED)
#define RVA_AI_RALLY_COPY_START                ADDR(        WIN_NO_EQUIVALENT, 0x1004d6228)
#define RVA_AI_RALLY_COPY_END                  ADDR(        WIN_NO_EQUIVALENT, 0x1004dbb98)
#define RVA_AI_RALLY_COPY_NONE                 ADDR(        WIN_NO_EQUIVALENT, 0x1004d6400)
#define RVA_AI_FRIENDLY_COPY_START             ADDR(        WIN_NO_EQUIVALENT, 0x1004d640c)
#define RVA_AI_FRIENDLY_COPY_END               ADDR(        WIN_NO_EQUIVALENT, 0x1004d8564)
#define RVA_AI_CONQUER_COPY_START              ADDR(        WIN_NO_EQUIVALENT, 0x1004d85ac)
#define RVA_AI_CONQUER_COPY_END                ADDR(        WIN_NO_EQUIVALENT, 0x1004d9bac)
#define RVA_AI_EXPLORE_COPY_START              ADDR(        WIN_NO_EQUIVALENT, 0x1004d9bf8)
#define RVA_AI_EXPLORE_COPY_END                ADDR(        WIN_NO_EQUIVALENT, 0x1004da548)
#define AI_MOVE_MAC_COMMANDER_AT               0xc0
#define AI_MOVE_MAC_COMSTR_AT                  0x70
#define RVA_AI_EXPLORE_STEP_CALL               ADDR(        0x070c72,          MAC_INLINED)
#define RVA_AI_FAR_EXPLORE_STEP                ADDR(        0x073d50,          0x1004d3990)
#define RVA_AI_RALLY_STEP                      ADDR(        0x076640,          MAC_INLINED)
#define RVA_AI_RALLY_STEP_CALL                 ADDR(        0x070b92,          MAC_NOT_LOOKED_UP)
#define RVA_AI_FRIENDLY_MOVE_STEP              ADDR(        0x076230,          MAC_INLINED)
#define RVA_AI_FRIENDLY_MOVE_STEP_CALL         ADDR(        0x070bdb,          MAC_NOT_LOOKED_UP)
#define RVA_AI_CONQUER_STEP_CALL               ADDR(        0x070c24,          MAC_INLINED)
#define RVA_AI_FAR_EXPLORE_STEP_CALL           ADDR(        0x070cc0,          MAC_NOT_LOOKED_UP)
#define RVA_AI_THREAT_NEAR                     ADDR(        0x076f30,          MAC_NOT_LOOKED_UP)
#define RVA_COMMANDER_STRENGTH                 ADDR_DIFFERS(0x128c50,          0x10047f82c)
#define RVA_AI_COMMANDER_CAN_MOVE              ADDR(        WIN_INLINED,       MAC_NOT_LOOKED_UP)
#define RVA_MOVE_ORDER_ROUTE                   ADDR(        0x09e0a0,          0x100373b1c)
#define RVA_PATH_FOG                           ADDR_DIFFERS(0x58355f8,         0x1121124dc)
#define RVA_PATH_NO_TIE_RANDOM                 ADDR_DIFFERS(0x58355fc,         0x1121124e8)
#define RVA_PATH_SPREAD                        ADDR(        0xe6c05c0,         0x1121124f0)
#define RVA_PATH_X                             ADDR(        0xe75c9c0,         0x1121ae908)
#define RVA_PATH_Y                             ADDR(        0xe75cdc0,         0x1121aed08)
#define RVA_PATH_LENGTH                        ADDR(        0x5835608,         0x1121124ec)
#define RVA_PATH_SQUARE_CACHE_RESET            ADDR(        0x1dc070,          MAC_INLINED)
#define RVA_AI_COUNT_ALLY_TARGETS_NEAR         ADDR(        0x0798b0,          MAC_INLINED)
#define RVA_COMMANDER_AI_TARGET_X              ADDR(        0x11c0dd40,        0x10c6368c4)
#define RVA_COMMANDER_AI_TARGET_Y              ADDR(        0x11d37a40,        0x10c63b6d0)
#define RVA_COMMANDER_AI_MOVE_KIND             ADDR(        0x11d32c20,        0x10859d368)
#define RVA_WAYTOGO                            ADDR_DIFFERS(0x1dac60,          0x10059bd90)
#define RVA_WAYTOGO2                           ADDR_DIFFERS(0x1df250,          0x10059d77c)
#define RVA_WAYTOGO2_EXTRA_ARG                 ADDR_DIFFERS(0x1df250,          0x10059d77c)
#define RVA_WAYTOGO_FAR_SQUARE_CHECK           ADDR(        0x1dbde0,          0x10059b9bc)
#define RVA_FAR_CHECK_SQUARE_CALL              ADDR(        0x1dc002,          0x10059bd48)
#define RVA_FAR_CHECK_SQUARE_CALL_2            ADDR(        0x1dc01d,          MAC_NO_CALL)
#define RVA_FAR_CHECK_SQUARE_CALL_3            ADDR(        0x1dc031,          MAC_NO_CALL)
#define RVA_MAY_ENTER_SQUARE                   ADDR(        0x1def30,          0x10059a1b4)
#define RVA_MAY_ENTER_SQUARE_CORE              ADDR(        0x1dccd0,          0x100599d54)
#define RVA_RANDOM_PORTAL_EXIT                 ADDR(        0x254fe0,          0x100571484)
#define RVA_ARMY_MOVE_FLAGS                    ADDR(        0x328e10,          0x100569fd8)
#define RVA_ARMY_UNIT_FLAG_42_CHECK            ADDR_DIFFERS(0x325450,          0x10056b9bc)
#define RVA_SQUARE_IS_SEA                       ADDR(        0x2adb30,          MAC_INLINED)
#define RVA_AI_STATIONARY_CALL                 ADDR(        WIN_NO_CALL,       MAC_NOT_LOOKED_UP)
#define RVA_AI_EXPLORE_GATE_CMP                ADDR(        0x070c57,          MAC_NOT_LOOKED_UP)
#define RVA_AI_FAR_EXPLORE_GATE_CMP            ADDR(        0x070ca5,          MAC_NOT_LOOKED_UP)
#define RVA_AI_MOVE_ROUTE_CALL                 ADDR(        0x070e60,          MAC_NOT_LOOKED_UP)
#define RVA_AI_DANGEROUS_STEP_CALL             ADDR(        0x070f7d,          MAC_NOT_LOOKED_UP)
#define RVA_AI_MOVE_STEP_CALL                  ADDR(        0x07104b,          0x1004db3b4)
#define RVA_AI_GOLDRALLY_STOP_CALL             ADDR(        0x071507,          MAC_NOT_LOOKED_UP)
#define RVA_AI_RALLY_ALLY_COUNT_CALL           ADDR(        0x076e0d,          MAC_NOT_LOOKED_UP)
#define RVA_AI_FRIENDLY_PLAN_BONUS_CALL        ADDR(        0x076474,          MAC_NOT_LOOKED_UP)
#define RVA_AI_FRIENDLY_THREAT_CALL            ADDR(        0x0764a6,          MAC_NOT_LOOKED_UP)
#define RVA_AI_CONQUER_STRENGTH_CALL           ADDR(        WIN_NO_CALL,       MAC_NOT_LOOKED_UP)
#define RVA_AI_CONQUER_PLAN_BONUS_CALL         ADDR(        0x0759d6,          MAC_NOT_LOOKED_UP)
#define RVA_AI_CONQUER_KILLER_ROUTE_CALL       ADDR(        0x075e4f,          MAC_NOT_LOOKED_UP)
#define RVA_AI_CONQUER_ALLY_COUNT_CALL         ADDR(        0x075f28,          MAC_NOT_LOOKED_UP)
#define RVA_AI_CONQUER_THREAT_CUTOFF           ADDR(        0x075f4b,          MAC_NOT_LOOKED_UP)
#define RVA_AI_CONQUER_THREAT_CALL             ADDR(        0x075f60,          MAC_NOT_LOOKED_UP)
#define RVA_AI_THREAT_STRENGTH_CALL            ADDR(        0x077032,          MAC_NOT_LOOKED_UP)
#define RVA_AI_CONQUER_ROUTE_CALL              ADDR(        0x0760d2,          MAC_NOT_LOOKED_UP)
#define RVA_ROUTE_BOUND_OTHER                  ADDR(        0x5be324c,         0x1121ae8fc)
#define RVA_ROUTE_BOUND_AI                     ADDR(        0x5be3e08,         0x1121ae900)
#define RVA_ROUTE_BOUND_HUMAN                  ADDR(        0x5be3e0c,         0x1121ae904)
#define RVA_ROUTE_STRENGTH_CALL                ADDR(        0x1dc002,          0x10059bd48)
#define RVA_ROUTE_EXEMPT_X                     ADDR(        0x1aa7ffc,         0x105a0aea4)
#define RVA_ROUTE_EXEMPT_Y                     ADDR(        0x1aa7c44,         0x105a0aea8)
#define RVA_PATH_NO_SEA                        ADDR(        0x583560c,         0x1121ae8f0)
#define RVA_PATH_NO_SKY                        ADDR(        0x5835610,         0x1121ae8f4)
#define RVA_PATH_SPREAD_SQUARE                 ADDR(        0x1db900,          MAC_INLINED)
#define RVA_SQUARE_MOVE_COST                   ADDR(        0x1dc0c0,          0x10059ac88)
#define RVA_SQUARE_IS_SKY                      ADDR(        0x2ab5d0,          0x1004ad238)
#define RVA_PLANE_SAME_SPOT                    ADDR(        0x253ee0,          0x100570644)
#define RVA_MOVE_STEP_BEFORE                   ADDR_DIFFERS(0x1deeb0,          0x10059d8e4)
#define RVA_MOVE_STEP_AFTER                    ADDR(        0x1df050,          0x10059d9dc)
#define RVA_OVERRUN_CHECK                      ADDR(        0x1dca30,          0x10059b0c8)
#define RVA_SPEND_ALL_AP_AT                    ADDR(        0x104d60,          0x1004816c0)
#define RVA_SHIPMOVE_SPEND_AP                  ADDR(        0x104fd0,          0x100481120)
#define RVA_STEP_TARGET_X                      ADDR(        0x1aa906c,         0x105a0aeb8)
#define RVA_STEP_TARGET_Y                      ADDR(        0x1aa7d94,         0x105a0aebc)
#define RVA_STEP_TARGET_WATCHED                ADDR_DIFFERS(0x5835604,         0x1121af108)
#define RVA_GROUP_MOVE                         ADDR(        0x1de4b0,          0x10059b598)
#define RVA_HUMAN_MULTI_MOVE                   ADDR(        0x09dae0,          0x100376bd8)
#define RVA_CONTINUE_MOVE_ORDERS               ADDR(        0x09d9e0,          0x100377100)
#define RVA_MAP_SCREEN_MULTI_MOVE_CALL         ADDR(        0x098786,          0x100387e30)
#define RVA_GATEWAY_BUTTON_CALL                ADDR(        0x09977d,          0x10037dc64)
#define RVA_COMMANDER_PANEL                    ADDR(        0x098f40,          MAC_INLINED)
#define RVA_COMMANDER_PANEL_CALL               ADDR(        0x094cb3,          MAC_INLINED)
#define RVA_COMMANDER_PANEL_FRAME_CALL         ADDR_DIFFERS(0x09ac02,          0x10037f194)
#define RVA_CMD_PANEL_CACHED_FOR               ADDR(        0x43d0ac,          MAC_NOT_LOOKED_UP)
#define RVA_CMD_PANEL_CHOP                     ADDR(        0x5804ba0,         0x1069b1680)
#define RVA_CMD_PANEL_BURN                     ADDR(        0x5804b94,         0x1069b1684)
#define RVA_UI_TEXT_SCALE                      ADDR(        0x1a59344,         0x1049c8350)
#define RVA_GATEWAY_PORTAL_READ                ADDR(        0x098aa1,          0x100388114)
#define RVA_GATEWAY_EXIT_CALL                  ADDR(        0x098ac0,          0x100388134)
#define RVA_GATEWAY_REVEAL_CALL                ADDR_DIFFERS(0x098af5,          0x100388194)
#define RVA_GATEWAY_GROUP_MOVE_CALL            ADDR(        0x098b67,          0x1003882e0)
#define RVA_MOVE_REFUSED_TEXT_CALL             ADDR_DIFFERS(0x098c47,          0x1003883c8)
#define RVA_STEP_PORTAL_TO_READ                ADDR(        0x1df6bb,          0x10059a568)
#define RVA_STEP_PORTAL_FROM_READ              ADDR(        0x1df6f9,          0x10059a54c)
#define RVA_STEP_REFUSED_RETURN                ADDR_DIFFERS(0x1dfc92,          0x10059a674)
#define RVA_SPREAD_PORTAL_READ                 ADDR(        0x1dbbef,          0x10059caf4)
#define RVA_WAYTOGO_BACK_PORTAL_READ           ADDR(        0x1db410,          0x10059d260)
#define RVA_MONSTER_PATH                       ADDR(        0x1de780,          0x10059dcac)
#define RVA_MONSTER_PATH_SPREAD_SQUARE         ADDR(        0x1da840,          MAC_INLINED)
#define RVA_MONSTER_PATH_PORTAL_READ           ADDR(        0x1daaf7,          0x10059e5e8)
#define RVA_INDEP_PORTAL_READ                  ADDR(        0x16d1c2,          0x10054703c)
#define RVA_AI_EXPLORE_PORTAL_READ             ADDR(        0x0749aa,          0x1004dbce0)
#define RVA_WANDER_MOVE_STEP_CALL              ADDR(        0x17ce81,          0x10038d8e4)
#define RVA_WANDER_MOVE_STEP_CALL_2            ADDR(        0x17d0b4,          0x10038dca0)
#define RVA_TUNNEL_DIG                         ADDR(        0x127a90,          0x1004849f0)
#define RVA_METEOR_STRIKE                      ADDR(        0x12d730,          MAC_INLINED)
#define RVA_NET_RECEIVE_SQUARE                 ADDR(        0x1eb3a0,          0x100505fa8)
#define RVA_NET_EVENTS_RECV_SQUARE_CALL        ADDR(        0x1e837b,          0x10050d41c)
#define RVA_NET_CLIENT_RECV_SQUARE_CALL        ADDR(        0x1ea665,          0x100509d3c)
#define RVA_GROUP_MOVE_STEP_CALL               ADDR(        0x1de68c,          0x10059b88c)
#define RVA_INDEP_EXIT_CALL                    ADDR(        0x16d1f4,          0x10054706c)
#define RVA_SPREAD_EXIT_CHECK_CALL             ADDR(        0x1dbcf6,          0x10059ccfc)
#define RVA_SPREAD_EXIT_CHECK_CALL_2           ADDR(        WIN_NO_EQUIVALENT, 0x10059cde0)
#define RVA_WAYTOGO_BACK_TRACE_CALL            ADDR(        0x1db4cd,          0x10059d408)
#define RVA_WAYTOGO_BACK_TRACE_CALL_2          ADDR(        WIN_NO_EQUIVALENT, 0x10059d518)
#define RVA_WAYTOGO_CALL                       ADDR(        0x1df2cc,          0x10059d7d8)
#define RVA_WAYTOGO_CALL_2                     ADDR(        0x1df394,          0x10059d88c)
#define MOVE_STEP_REFUSED                      -9
#define RVA_ARMY_ABILITY_TOTAL                 ADDR(        0x12ba40,          0x100468fc8)
#define RVA_AI_PROGRESS_DRAW                   ADDR(        0x07dce0,          MAC_NOT_LOOKED_UP)
#define RVA_AI_COUNT_CITADELS                  ADDR(        0x077210,          0x1004d31ac)
#define RVA_AI_CITADEL_COUNT                   ADDR(        0x59eac7c,         MAC_IN_CODE)
#define RVA_AI_ATTACK_DIV_OTHER                ADDR(        0x1aa7bc4,         0x105a0aeac)
#define RVA_AI_ATTACK_DIV_AI                   ADDR(        0x1aa65c4,         0x105a0aeb0)
#define RVA_AI_ATTACK_DIV_HUMAN                ADDR(        0x1aa62ac,         0x105a0aeb4)
#define AI_ATTACK_MARGIN                       15
#define RVA_AI_ATTACK_MARGIN                   ADDR(        0x1aa74f4,         MAC_IN_CODE)
#define RVA_AI_SET_ATTACK_DIVS_COPY            ADDR(        0x06fc20,          MAC_NOT_LOOKED_UP)
#define RVA_AI_STRONGEST_ENEMY                 ADDR(        WIN_NOT_FOUND,     MAC_NOT_LOOKED_UP)
#define RVA_AI_STRONGEST_ENEMY_SCAN            ADDR(        WIN_NOT_FOUND,     MAC_NOT_LOOKED_UP)
#define RVA_AI_WANTS_HANDS                     ADDR(        0x5802f58,         0x10857c568)
#define RVA_AI_WANTS_HERBS                     ADDR(        0x5802f64,         0x10857c55c)
#define RVA_AI_WANTS_FUNGUS                    ADDR(        0x580335c,         0x10857c558)
#define RVA_AI_WANTS_SACRIFICES                ADDR(        0x5802f5c,         0x10857c560)
#define RVA_AI_WANTS_WEED                      ADDR(        0x5803358,         0x10857c56c)
#define RVA_AI_WANTS_GEMS                      ADDR(        0x5802f54,         0x10857c574)
#define RVA_AI_IRON_FLAG                       ADDR_DIFFERS(0x5802f68,         0x10857c564)
#define RVA_AI_WANTS_LIFEFORCE                 ADDR(        0x5802f6c,         0x10857c570)
#define RVA_AI_SAVE_RESOURCE                   ADDR(        0x6128620,         0x10857c58c)
#define RVA_AI_UPDATE_STRENGTH_MEMORY          ADDR(        0x06d870,          0x1004e4148)
#define RVA_AI_STRENGTH_MEMORY_CALL            ADDR(        0x06fb32,          0x1004e38d0)
#define RVA_AI_STRENGTH_MEMORY_CALL_2          ADDR(        0x06fb47,          0x1004e3bb0)
#define RVA_AI_STRENGTH_MEMORY_CALL_3          ADDR(        WIN_NO_EQUIVALENT, 0x1004e3bc4)
#define RVA_AI_STRENGTH_MEMORY_CALL_4          ADDR(        WIN_NO_EQUIVALENT, 0x1004e3bd0)
#define RVA_AI_STRENGTH_MEMORY_SQUARE_CALL     ADDR(        0x06d917,          0x1004e4210)
#define RVA_AI_STRENGTH_MEMORY_PROGRESS_CALL   ADDR(        0x06d9d2,          0x1004e4228)
#define RVA_AI_REMEMBERED_STRENGTH             ADDR(        0x06d800,          MAC_NOT_LOOKED_UP)
#define RVA_AI_DANGEROUS_STEP                  ADDR(        0x079630,          0x1004d2d30)
#define RVA_AI_GATHER_TROOPS                   ADDR(        0x06edd0,          MAC_INLINED)
#define RVA_AI_GATHER_TROOPS_CALL              ADDR(        0x070def,          MAC_NOT_LOOKED_UP)
#define RVA_AI_GATHER_COPY_START               ADDR(        WIN_NO_EQUIVALENT, 0x1004da718)
#define RVA_AI_GATHER_COPY_END                 ADDR(        WIN_NO_EQUIVALENT, 0x1004db174)
#define RVA_AI_GATHER_LEAVE_UNLINK             ADDR(        0x06f37a,          MAC_NOT_LOOKED_UP)
#define RVA_AI_GATHER_DROP_UNLINK              ADDR(        0x06f722,          MAC_NOT_LOOKED_UP)
#define RVA_AI_MOVE_COMSTR_CALL                ADDR(        0x070df9,          MAC_NOT_LOOKED_UP)
#define RVA_COMMANDER_NO_GATHER                ADDR(        0x0b0dc0,          MAC_INLINED)
#define RVA_COMMANDER_FLAGGED_STRENGTH         ADDR(        0x128c50,          0x10047f70c)
#define RVA_UNIT_LEADER_OF                     ADDR(        0x32a030,          MAC_INLINED)
#define RVA_UNIT_SQUARE                        ADDR(        0x3295d0,          MAC_INLINED)
#define UNIT_AI_SAILOR                         0x200000000000
#define UNIT_FLAGS_AI_SKIP                     0x4000000022000000
#define UNIT_INSANE                            0x2000
#define ABILITY_WANDER                         0x4b9
#define ABILITY_FARSIGHT                       0x92
#define ABILITY_FARSIGHTSEA                    0x50b
#define ABILITY_WANDERATTACK                   0x4c9
#define ABILITY_REVERTOWNER                    0x1ef
#define RVA_REVERTOWNER_BOUND                  ADDR(        0x1075c4,          0x100472f98)
#define RVA_PLAYER_TINT_CLAMP                  ADDR(        0x2e82f9,          0x10056d508)
#define UNIT_RESISTANCE_NOLEADER               39
#define UNIT_AI_GUARD                          0x8000000000
#define RVA_AI_SPLIT_ARMY                      ADDR(        0x07b5a0,          MAC_INLINED)
#define RVA_AI_SPLIT_ARMY_CALL                 ADDR(        0x07b3fc,          MAC_INLINED)
#define RVA_AI_SPLIT_MIN_UNITS                 ADDR(        WIN_IN_CODE,       MAC_NOT_LOOKED_UP)
#define RVA_AI_SPLIT_MIN_UNITS_TRACE           ADDR(        0x07b6ca,          0x1004d2788)
#define RVA_AI_SPLIT_MIN_UNITS_TEST            ADDR(        0x07b6da,          0x1004d27a0)
#define RVA_AI_SPLIT_COPY_START                ADDR(        WIN_NO_EQUIVALENT, 0x1004d2608)
#define RVA_AI_SPLIT_COPY_END                  ADDR(        WIN_NO_EQUIVALENT, 0x1004d2678)
#define AI_SPLIT_MIN_UNITS_MAC                 240
#define RVA_AI_MAKE_GUARDS_AT                  ADDR(        0x06ebd0,          0x1004e01a0)
#define RVA_AI_MAKE_DEFENDERS                  ADDR(        0x06fe90,          MAC_INLINED)
#define RVA_AI_RANDOM_ENEMY                    ADDR(        0x07b480,          0x1004d17cc)
#define RVA_AI_VIEW_MOVE_START                 ADDR(        0x06de10,          MAC_NOT_LOOKED_UP)
#define RVA_AI_VIEW_MOVE_END                   ADDR(        0x06da50,          MAC_NOT_LOOKED_UP)
#define RVA_AI_PLAN_TEXT                       ADDR(        0x06ad70,          MAC_NOT_LOOKED_UP)
#define RVA_AI_COUNT_TROOPS                    ADDR(        0x0794b0,          0x1004d3024)
#define RVA_AI_UNIT_STAYS                      ADDR(        0x07d970,          MAC_INLINED)
#define RVA_AI_REFORMS_HERE                    ADDR(        0x07ce80,          0x1004d10f4)
#define RVA_AI_UNIT_JOIN_KIND                  ADDR(        0x07c750,          0x1004d1260)
#define RVA_AI_PICK_UP_LONERS                  ADDR(        0x07bcf0,          0x1004d1404)
#define RVA_AI_PICK_UP_LONER_CALL              ADDR(        0x07c0fd,          0x1004d17b0)
#define RVA_AI_UNIT_MAY_GUARD                  ADDR(        0x07a2e0,          0x1004d2bf4)
#define RVA_AI_BUY_COMMANDER                   ADDR(        0x079fa0,          MAC_INLINED)
#define RVA_AI_BUY_TROOPS                      ADDR(        0x079ac0,          MAC_INLINED)
#define RVA_AI_BUY_UPGRADE_UNLINK              ADDR(        0x079d15,          MAC_NOT_LOOKED_UP)
#define RVA_RESET_INCOME_CACHES                 ADDR(        0x328db0,          0x100567dfc)
#define RVA_AI_MOVE_ALL_CALL                   ADDR(        0x06fb39,          MAC_NOT_LOOKED_UP)
#define RVA_AI_MAKE_DEFENDERS_CALL             ADDR(        0x06fb40,          MAC_NOT_LOOKED_UP)
#define RVA_AI_TRADE_CHOICE_CALL               ADDR(        0x06fb4e,          MAC_NOT_LOOKED_UP)
#define RVA_INT16_FILL                         ADDR(        0x058fd0,          MAC_NOT_LOOKED_UP)
#define RVA_AI_STRONGEST_ENEMY_CALL            ADDR(        WIN_NOT_FOUND,     MAC_NOT_LOOKED_UP)
#define RVA_AI_VIEW_PICK_UNIT                  ADDR(        0x1b2bb0,          MAC_NOT_LOOKED_UP)
#define RVA_AI_RITUALS_TAINT_TEST              ADDR(        0x06af22,          MAC_NOT_LOOKED_UP)
#define RVA_AI_APPRSPAM_EXECUTE_CALL           ADDR(        0x06b256,          0x1004dd2d0)
#define RVA_AI_PLAN_GATE_ROLL                  ADDR(        0x06b337,          MAC_NOT_LOOKED_UP)
#define RVA_AI_PLAN_SCRAP_CANNOT_CAST          ADDR(        0x06be6a,          MAC_NOT_LOOKED_UP)
#define RVA_AI_PLAN_ACCEPT_STORE               ADDR(        0x06be90,          MAC_NOT_LOOKED_UP)
#define RVA_AI_PLAN_SCRAP_POINTLESS            ADDR(        0x06be6a,          MAC_NOT_LOOKED_UP)
#define RVA_AI_PLAN_SCRAP_AI_BAD               ADDR(        0x06be6a,          MAC_NOT_LOOKED_UP)
#define RVA_AI_PLAN_ANYWHERE_CALL              ADDR(        0x06bee0,          MAC_NOT_LOOKED_UP)
#define RVA_AI_PLAN_SCRAP_NOWHERE              ADDR(        0x06be6a,          MAC_NOT_LOOKED_UP)
#define RVA_AI_PLAN_TARGET_CALL                ADDR(        0x06c04e,          MAC_NOT_LOOKED_UP)
#define RVA_AI_PLAN_PAY_CALL                   ADDR(        0x06c1e4,          MAC_NOT_LOOKED_UP)
#define RVA_AI_SAVE_RESOURCE_STORE             ADDR(        0x06c28c,          MAC_NOT_LOOKED_UP)
#define RVA_AI_PLAN_EXECUTE_CALL               ADDR(        0x06c2df,          0x1004de76c)
#define RVA_AI_PLAN_DONE_STORE                 ADDR(        0x06c300,          MAC_NOT_LOOKED_UP)
#define RVA_AI_ALWAYS_EXECUTE_CALL             ADDR(        0x06c451,          0x1004de598)
#define RVA_AI_RITUALS_PICK_UP_CALL            ADDR(        0x06c485,          MAC_NOT_LOOKED_UP)
#define RVA_AI_NEAREST_TREQ_ON_PLANE           ADDR(        0x06cea0,          MAC_NOT_LOOKED_UP)
#define RVA_AI_NEAREST_TREQ                    ADDR(        0x06ccf0,          0x1004dfe70)
#define RVA_AI_RITUAL_MASTER                   ADDR(        0x06ca10,          MAC_NOT_LOOKED_UP)
#define RVA_AI_BEST_SPAWNBOOSTER_AT            ADDR(        WIN_NOT_FOUND,     MAC_NOT_LOOKED_UP)
#define RVA_AI_RECRUIT_TURN_GATE_LATE          ADDR(        0x077355,          MAC_NOT_LOOKED_UP)
#define RVA_AI_RECRUIT_SAVE_CAP                ADDR(        0x07739b,          MAC_NOT_LOOKED_UP)
#define RVA_AI_RECRUIT_TURN_GATE_SHIPS         ADDR(        0x0773f3,          MAC_NOT_LOOKED_UP)
#define RVA_AI_RECRUIT_FIRST_OFFER_TEST        ADDR(        0x0775b3,          MAC_NOT_LOOKED_UP)
#define RVA_AI_RECRUIT_IMPORTANT_TRY_GATE      ADDR(        0x077934,          MAC_NOT_LOOKED_UP)
#define RVA_AI_RECRUIT_MELEE_TRY_GATE          ADDR(        0x077a98,          MAC_NOT_LOOKED_UP)
#define RVA_AI_RECRUIT_TROOP_GOLD_GATE         ADDR(        0x077b21,          MAC_NOT_LOOKED_UP)
#define RVA_AI_RECRUIT_COM_GOLD_GATE           ADDR(        0x077b8b,          MAC_NOT_LOOKED_UP)
#define RVA_AI_RECRUIT_COM_ESCORT_GATE         ADDR(        0x077bbd,          MAC_NOT_LOOKED_UP)
#define RVA_AI_RECRUIT_SITE_DRAWS              ADDR(        0x077e7a,          MAC_NOT_LOOKED_UP)
#define RVA_AI_RECRUIT_ESCORT_STRENGTH         ADDR(        0x077f38,          MAC_NOT_LOOKED_UP)
#define RVA_AI_RECRUIT_TRIES                   ADDR(        0x07814e,          MAC_NOT_LOOKED_UP)
#define RVA_AI_PORT_FREE_BOAT_PLACES           ADDR(        0x07d510,          MAC_INLINED)
#define RVA_AI_COMMANDER_BOAT_PLACES           ADDR(        0x072bc0,          MAC_NOT_LOOKED_UP)
#define RVA_AI_SHIP_DROP_CREW                  ADDR(        0x071c70,          MAC_NOT_LOOKED_UP)
#define RVA_AI_SHIP_TAKE_BOAT                  ADDR(        0x072110,          MAC_NOT_LOOKED_UP)
#define RVA_AI_SHIP_TAKE_CREW                  ADDR(        0x072660,          MAC_NOT_LOOKED_UP)
#define RVA_AI_GROUP_BOAT_TOTAL                ADDR(        0x073060,          MAC_NOT_LOOKED_UP)
#define RVA_AI_PORTS_BOAT_TOTALS               ADDR(        0x07cfa0,          MAC_NOT_LOOKED_UP)
#define RVA_AI_SHIPS_AT                        ADDR(        0x07d170,          MAC_NOT_LOOKED_UP)
#define RVA_AI_SPEC_MESSAGES_OFF               ADDR(        0x07a9d8,          MAC_NOT_LOOKED_UP)
#define RVA_AI_GATEWAY_CALL                    ADDR(        0x07ab75,          MAC_NOT_LOOKED_UP)
#define RVA_AI_ITEM_USE_RATING                 ADDR(        0x183790,          0x1004e8420)
#define MONSTER_ABILITY_AIHOLD                 0x34
#define MONSTER_ABILITY_AIPOWCOM1              0x66
#define MONSTER_ABILITY_AIMAXSHOP              0x90
#define MONSTER_ABILITY_SHIP                   0x1cf
#define RVA_AI_COUNT_PLANNERS                  ADDR(        0x0799e0,          MAC_NOT_LOOKED_UP)
#define RVA_AI_FREE_SPAWNBOOSTERS              ADDR(        0x078e30,          MAC_INLINED)
#define RVA_AI_BEST_BOOSTER_AT                 ADDR(        0x079140,          MAC_INLINED)
#define RVA_AI_MAKE_SPAWNBOOSTER               ADDR(        0x0781f0,          MAC_INLINED)
#define RVA_AI_TRADE_CHOICE                    ADDR(        0x0700f0,          MAC_NOT_LOOKED_UP)
#define RVA_TRADE_INCOME_APPLY                 ADDR(        0x1490b0,          MAC_INLINED)
#define RVA_AI_PLAN_SQUARE_BONUS               ADDR(        0x0770a0,          MAC_NOT_LOOKED_UP)
#define RVA_AI_ITEM_VALUE                      ADDR(        0x073810,          0x1004d41f8)
#define RVA_AI_REDISTRIBUTE_ITEMS              ADDR(        0x0734b0,          MAC_INLINED)
#define RVA_AI_SHIPS                           ADDR(        0x071630,          MAC_INLINED)
#define RVA_AI_RITUAL_AI_BAD                   ADDR(        0x06d070,          0x1004df834)
#define RVA_AI_RITUAL_TARGET                   ADDR(        0x06c4f0,          0x1004df98c)
#define RVA_AI_RITUAL_BAD_TIME                 ADDR(        0x06d290,          0x1004df230)
#define RVA_AI_EXPLORE_TERRAIN_SKIP            ADDR(        0x0751a0,          MAC_NOT_LOOKED_UP)
#define RVA_AI_TARGET_IMPOSSIBLE               ADDR(        0x07cc70,          MAC_NOT_LOOKED_UP)
#define RVA_PLAYER_ARMY_STRENGTH               ADDR(        0x0f8b30,          MAC_NOT_LOOKED_UP)
#define RVA_UNIT_STRENGTH                      ADDR(        0x0f8c80,          MAC_INLINED)
#define RVA_UNIT_STRENGTH_CORE                 ADDR(        0x0bbe50,          0x1003f6168)
#define RVA_UNIT_ARMOR                         ADDR(        0x3210a0,          0x10056aa70)
#define RVA_SQUARE_ICEPROT_TEST                ADDR(        0x1b1580,          0x1004a8e30)
#define RVA_PATH_STRENGTH_WEIGHT               ADDR(        0x0bce10,          MAC_INLINED)
#define RVA_INT_MIN                            ADDR(        0x0576b0,          MAC_NOT_FOUND)
#define RVA_INT_MAX                            ADDR(        0x05c890,          MAC_NOT_FOUND)
#define RVA_STRONGEST_PLAYER                   ADDR(        WIN_NOT_FOUND,     MAC_NOT_LOOKED_UP)
#define RVA_STRONGEST_PLAYERS                  ADDR(        0x0f4420,          MAC_NOT_LOOKED_UP)
#define RVA_SQUARE_TOP_UNIT                    ADDR_DIFFERS(0x1b1930,          0x100498ff8)
#define RVA_SQUARE_STRENGTH                    ADDR(        0x0f8fe0,          0x1003f6f14)
#define RVA_UNIT_STRENGTH_FOR                  ADDR(        0x0bbd60,          MAC_INLINED)
#define RVA_UNIT_COUNTS_AS_MONSTER             ADDR(        0x1d9010,          0x100520f94)
#define RVA_SQUARE_SIDE_STRENGTH               ADDR(        0x0f8cb0,          0x1003e7038)
#define RVA_COMMANDER_MAY_ACT                  ADDR(        0x1df3f0,          0x100599b90)
#define RVA_COMMANDER_RITPOW                   ADDR(        0x2c3e00,          MAC_NOT_FOUND)
#define RVA_COMMANDER_LED_COUNT                ADDR(        0x128680,          0x10046c6b0)
#define RVA_UNIT_HAS_RANGED                    ADDR(        0x322270,          0x10056c998)
#define RVA_MONSTER_HAS_RANGED                 ADDR(        0x326d50,          MAC_NOT_FOUND)
#define RVA_UNIT_RETURNS                       ADDR(        0x326440,          0x10056925c)
#define RVA_SHOP_OFFER_AVAILABLE               ADDR(        0x128460,          0x10047d4e0)
#define RVA_FIND_UPGRADE_UNIT                  ADDR(        0x12b840,          0x100479390)
#define RVA_PLAYER_GOLD_INCOME                 ADDR(        0x334a30,          0x1004cef60)
#define RVA_PLAYER_IRON_INCOME                 ADDR(        0x32d230,          0x1004cf37c)
#define RVA_PLAYER_HERBS_INCOME                ADDR(        0x32d680,          0x1004cff44)
#define RVA_PLAYER_FUNGUS_INCOME               ADDR(        0x32cd60,          0x1004d0228)
#define RVA_PLAYER_SACRIFICES_INCOME           ADDR(        0x10a410,          0x100476d7c)
#define RVA_PLAYER_HANDS_INCOME                ADDR(        0x32dc50,          0x1004cfa84)
#define RVA_PLAYER_WEED_INCOME                 ADDR(        0x32d500,          0x1004d0520)
#define RVA_PLAYER_GEM_INCOME                  ADDR(        0x32d350,          0x1004cf7c4)
#define RVA_PLAYER_TRADE_INCOME                ADDR(        0x32e280,          0x1004cf5a0)
#define RVA_PLAYER_RELICS_INCOME               ADDR(        0x32d110,          0x1004cfcac)
#define RVA_PLAYER_LIFEFORCE_INCOME            ADDR(        0x32e3a0,          0x1004d0cf4)
#define RVA_PLAYER_GATHERS_RELICS              ADDR(        0x0b0570,          0x1003721c8)
#define RVA_SQUARE_INCOME                      ADDR(        0x32f100,          0x1004cd508)
#define RVA_SQUARE_GOLD_INCOME                 ADDR(        0x335350,          MAC_NOT_FOUND)
#define RVA_TERRAIN_BASE_INCOME                ADDR(        0x330a60,          0x1004ce01c)
#define RVA_SQUARE_LIFEFORCE_AT                ADDR(        0x334830,          MAC_INLINED)
#define RVA_LIFEFORCE_DRAIN_SQUARE             ADDR(        0x32dd70,          0x1004d0754)
#define RVA_PLAYER_INCOME_CACHE                 ADDR(        0x11c1bb2c,         0x10ccebe20)
#define RVA_FREE_TROOPS_STRENGTH               ADDR(        0x128ae0,          0x100475010)
#define RVA_FREE_TROOPS_COUNT                  ADDR(        0x5822f54,         0x10857c554)
#define RVA_SQUARE_FORT_LEVEL                  ADDR(        0x128ef0,          MAC_INLINED)
#define RVA_SQUARE_HAS_FORT                    ADDR(        0x129500,          MAC_INLINED)
#define RVA_PLAYER_ABILITY                     ADDR(        0x0b0790,          0x100361ffc)
#define RVA_SHOP_PROCESS_OFFER                 ADDR(        0x0ae8f0,          0x1003627d4)
#define RVA_AI_LEVEL_SHIFT_ALL                 ADDR(        0x2e9ae0,          MAC_NOT_LOOKED_UP)
#define RVA_SHARE_TEAM_SIGHT                   ADDR(        0x1af340,          0x10049f5ac)
#define RVA_ILLUSION_TERRAIN                   ADDR(        0x1b2d70,          0x10049173c)
#define RVA_JOIN_COMMANDER                     ADDR(        0x129400,          0x100484204)
#define RVA_RESOURCE_STOCK                     ADDR(        WIN_INLINED,       MAC_NOT_LOOKED_UP)
#define RVA_ORDER_BURN_FOREST                  ADDR_DIFFERS(0x2e1420,          0x100359578)
#define RVA_ORDER_CHOP_FOREST                  ADDR_DIFFERS(0x2e15a0,          0x100359aa4)
#define RVA_ORDER_SLAVE_HUNT                   ADDR(        0x2e3120,          0x100359d28)
#define RVA_ORDER_EAT_VILLAGE                  ADDR(        0x2e1790,          0x10035b8fc)
#define RVA_ORDER_PRIEST                       ADDR(        0x2e1c50,          0x1003520d4)
#define RVA_INQUISITION_SPAWN                  ADDR(        0x2d8fc0,          0x100353c94)
#define RVA_CONSCRIPTION                       ADDR(        0x2da3a0,          MAC_INLINED)
#define RVA_PRIEST_COSTS                       ADDR(        0x2e2cd0,          0x1003519b0)
#define RVA_PRIEST_COSTS_CALL_AI               ADDR(        0x07b270,          0x1004d21a4)
#define RVA_PRIEST_COSTS_CALL_MAP              ADDR_DIFFERS(0x09e596,          0x100386cdc)
#define RVA_PRIEST_COSTS_CALL_NO_CALLER        ADDR(        0x09e596,          MAC_INLINED)
#define RVA_PRIEST_ORDER_NO_CALLER             ADDR(        0x09e260,          MAC_INLINED)
#define RVA_ORDER_PRIEST_CALL_SCREEN           ADDR(        0x2d514c,          0x10035f3b8)
#define RVA_ORDER_PRIEST_CALL_CRUSADE          ADDR_DIFFERS(0x2d513c,          0x10035f3b8)
#define RVA_ORDER_PRIEST_CALL_MAP              ADDR(        WIN_NO_CALL,       0x100386cec)
#define RVA_ORDER_PRIEST_CALL_NO_CALLER        ADDR(        0x09e5a9,          MAC_INLINED)
#define RVA_ORDER_PRIEST_CALLS_AI              ADDR(        0x07b292,          0x1004d21c8)
#define RVA_CEREMONY_SWITCH_BOUND              ADDR_DIFFERS(0x2e201e,          0x100352208)
#define CEREMONY_LAST                          10
#define CEREMONY_CONVERT                       1
#define CEREMONY_BREAK_SEAL                    2
#define CEREMONY_BOLSTER_DEVOTION              3
#define CEREMONY_START_CRUSADE                 4
#define CEREMONY_CALL_INQUISITION              5
#define CEREMONY_MASTERY_ARCH_BISHOP           7
#define CEREMONY_MASTERY_CARDINAL              8
#define CEREMONY_MASTERY_PONTIFF               9
#define CEREMONY_INCREASE_FAVOR                10
#define RVA_CEREMONY_COSTS                     ADDR(        WIN_STORED_DIFFERENTLY,MAC_STORED_DIFFERENTLY)
#define RVA_CEREMONY_COST_BOLSTER_STEP         ADDR(        0x11f97a4c,        0x106cee4ec)
#define RVA_CEREMONY_COST_SEAL_STEP            ADDR(        0x11f48f78,        0x106cee4e8)
#define RVA_CEREMONY_COST_FAVOR_STEP           ADDR(        0x11f493c0,        0x106cee4e4)
#define RVA_CEREMONY_COST_PONTIFF              ADDR(        0x11f97a48,        MAC_STORED_DIFFERENTLY)
#define RVA_CEREMONY_COST_CARDINAL             ADDR(        0x11f48ee0,        MAC_STORED_DIFFERENTLY)
#define RVA_CEREMONY_COST_ARCH_BISHOP          ADDR(        0x11f48ea4,        MAC_STORED_DIFFERENTLY)
#define RVA_CEREMONY_COST_ARCH_BISHOP_SET      ADDR(        WIN_STORED_DIFFERENTLY,0x106cee4d8)
#define RVA_CEREMONY_COST_CARDINAL_SET         ADDR(        WIN_STORED_DIFFERENTLY,0x106cee4dc)
#define RVA_CEREMONY_COST_PONTIFF_SET          ADDR(        WIN_STORED_DIFFERENTLY,0x106cee4e0)
#define RVA_MASTERY_COST_FILL_1                ADDR(        WIN_NO_EQUIVALENT, 0x100351af4)
#define RVA_MASTERY_COST_FILL_2                ADDR(        WIN_NO_EQUIVALENT, 0x100351afc)
#define RVA_MASTERY_COST_FILL_3                ADDR(        WIN_NO_EQUIVALENT, 0x100351b04)
#define RVA_MASTERY_COST_FILL_4                ADDR(        WIN_NO_EQUIVALENT, 0x10035e4fc)
#define RVA_MASTERY_COST_FILL_5                ADDR(        WIN_NO_EQUIVALENT, 0x10035e504)
#define RVA_MASTERY_COST_FILL_6                ADDR(        WIN_NO_EQUIVALENT, 0x10035e50c)
#define RVA_MASTERY_COST_READ_1                ADDR(        WIN_NO_EQUIVALENT, 0x1003525d4)
#define RVA_MASTERY_COST_READ_2                ADDR(        WIN_NO_EQUIVALENT, 0x100352680)
#define RVA_MASTERY_COST_READ_3                ADDR(        WIN_NO_EQUIVALENT, 0x10035274c)
#define RVA_MASTERY_COST_READ_4                ADDR(        WIN_NO_EQUIVALENT, 0x10035ed3c)
#define RVA_MASTERY_COST_READ_5                ADDR(        WIN_NO_EQUIVALENT, 0x10035ee08)
#define RVA_MASTERY_COST_READ_6                ADDR(        WIN_NO_EQUIVALENT, 0x10035ee3c)
#define RVA_MASTERY_COST_READ_7                ADDR(        WIN_NO_EQUIVALENT, 0x10035eed8)
#define RVA_MASTERY_COST_READ_8                ADDR(        WIN_NO_EQUIVALENT, 0x10035ef80)
#define RVA_MASTERY_COST_READ_9                ADDR(        WIN_NO_EQUIVALENT, 0x100352620)
#define RVA_MASTERY_COST_READ_10               ADDR(        WIN_NO_EQUIVALENT, 0x1003526e8)
#define RVA_MASTERY_COST_READ_11               ADDR(        WIN_NO_EQUIVALENT, 0x1003527b4)
#define MASTERY_COST_FILLS                     6
#define MASTERY_COST_READS                     11
#define CEREMONY_COST_ARCH_BISHOP_GOLD         100
#define CEREMONY_COST_CARDINAL_GOLD            250
#define CEREMONY_COST_PONTIFF_GOLD             600
#define RVA_CEREMONY_COST_INQUISITION          ADDR(        0x11f493bc,        0x106cee4d4)
#define RVA_CEREMONY_COST_CRUSADE              ADDR(        0x11f4981c,        0x106cee4d0)
#define RVA_CEREMONY_COST_BOLSTER_BASE         ADDR(        0x11d37a2c,        0x10857c550)
#define RVA_CEREMONY_COST_SEAL_BASE            ADDR(        0xfb31b00,         0x10857c54c)
#define RVA_CEREMONY_COST_CONVERT              ADDR(        0x11f493b8,        0x106cee4cc)
#define CEREMONY_COSTS_COUNT                   11
#define CEREMONY_COST_BOLSTER_STEP_AT          0x00
#define CEREMONY_COST_SEAL_STEP_AT             0x04
#define CEREMONY_COST_FAVOR_STEP_AT            0x08
#define CEREMONY_COST_PONTIFF_AT               0x0c
#define CEREMONY_COST_CARDINAL_AT              0x10
#define CEREMONY_COST_ARCH_BISHOP_AT           0x14
#define CEREMONY_COST_INQUISITION_AT           0x18
#define CEREMONY_COST_CRUSADE_AT               0x1c
#define CEREMONY_COST_BOLSTER_BASE_AT          0x20
#define CEREMONY_COST_SEAL_BASE_AT             0x24
#define CEREMONY_COST_CONVERT_AT               0x28
#define RVA_STR_CEREMONY_CONVERT               ADDR(        0x5698588,         0x10060931c)
#define RVA_STR_CEREMONY_BREAK_SEAL            ADDR(        0x5698628,         0x1006093db)
#define RVA_STR_CEREMONY_BOLSTER               ADDR(        0x5698698,         0x100609442)
#define RVA_STR_CEREMONY_CRUSADE               ADDR(        0x5698700,         0x100609460)
#define RVA_STR_CEREMONY_INQUISITION           ADDR(        0x5698770,         0x1006094ad)
#define RVA_STR_CEREMONY_MASTERY               ADDR(        0x5698790,         0x1006094f4)
#define RVA_STR_CEREMONY_FAVOR                 ADDR(        0x5698638,         0x100609403)
#define RVA_CEREMONY_SCREEN                    ADDR(        0x2d3470,          MAC_INLINED)
#define RVA_RITUALS_OR_CEREMONIES              ADDR_DIFFERS(0x2e0aa0,          0x10035e1d0)
#define RVA_CEREMONY_SCREEN_COSTS              ADDR_DIFFERS(0x2d38da,          0x10035e530)
#define RVA_CEREMONY_SCREEN_COSTS_END          ADDR_DIFFERS(0x2d38df,          0x10035e530)
#define CEREMONY_SCREEN_COSTS_SIZE             0x5
#define RVA_FAIL_IF_FLAG                       ADDR(        0x198eb0,          MAC_NOT_FOUND)
#define RVA_SEALS_BROKEN                       ADDR(        0xf07b028,         0x11307d7fc)
#define RVA_SEAL_EVENTS_DONE                   ADDR(        0xf07b02c,         0x11307d800)
#define RVA_SEAL_EVENT_STEP                    ADDR(        0x164aa4,          0x1003aac14)
#define RVA_SEAL_EVENT_SWITCH                  ADDR_DIFFERS(0x164ac3,          0x1003aac2c)
#define RVA_SEAL_EVENT_THIRD                   ADDR(        0x164acc,          0x1003aac50)
#define RVA_SEAL_EVENT_NOT_THIRD               ADDR(        0x164b7d,          MAC_NOT_FOUND)
#define RVA_SEAL_EVENT_NONE                    ADDR_DIFFERS(0x165268,          0x1003ac304)
#define RVA_SEAL_EVENT_END                     ADDR(        0x16526f,          0x1003ac304)
#define RVA_APOCALYPSE_MONTHS                  ADDR(        0xf07b034,         0x11307d808)
#define RVA_PRIEST_FREESPAWN                   ADDR(        0x2db5f0,          MAC_INLINED)
#define RVA_BOLSTER_BLESSING                   ADDR(        0x2d5590,          0x100352938)
#define RVA_NET_SEND_ORDER                     ADDR(        0x1f1de0,          0x10050ede4)
#define RVA_NET_SEND_BLESSINGS                 ADDR(        0x1e6dd0,          0x10050eff8)
#define RVA_SEAL_BREAK_CEREMONY_CALL           ADDR(        0x2e21b2,          0x1003523a4)
#define RVA_SEAL_BREAK_ITEM_CALL               ADDR(        0x184ab2,          0x1004e81e0)
#define RVA_USE_SPECIAL_SITE                   ADDR(        0x0b1600,          0x100388804)
#define RVA_DESTROY_GATEWAY                    ADDR(        0x254e20,          0x100574e64)
#define RVA_ALTER_TERRAIN                      ADDR(        0x1f7400,          0x10050dc18)
#define RVA_SITE_VOID_INSANITY                 ADDR(        0x2dee40,          0x10035aa94)
#define RVA_REVEAL_DISC                        ADDR(        0x2dd390,          0x100354d94)
#define RVA_RUIN_SQUARE                        ADDR(        0x2e05b0,          0x100356744)
#define RVA_ARMY_BURN_VALUE                    ADDR(        0x2e0190,          MAC_INLINED)
#define RVA_ORDER_CHOP_SELECTED_CALL           ADDR_DIFFERS(0x2e15e7,          0x100359af4)
#define RVA_MAP_INSPECTED_X                    ADDR(        0xfb31b04,         0x10cb0e194)
#define RVA_MAP_INSPECTED_Y                    ADDR(        0x11d37a34,        0x10cb0e198)
#define RVA_UNIT_REPICK_MEMORIZED              ADDR(        0x2e6f10,          MAC_NOT_FOUND)
#define RVA_COMMANDER_AI_MOVES                 ADDR(        0x11c08f20,        0x1085a2174)
#define RVA_COMMANDER_AI_FROM_X                ADDR(        0xfb2cca0,         0x1085a6f80)
#define RVA_COMMANDER_AI_FROM_Y                ADDR(        0xfb36940,         0x1085abd8c)
#define RVA_COMMANDER_AI_LAST_TARGET_X         ADDR(        0xfb89960,         0x1085b0b98)
#define RVA_COMMANDER_AI_LAST_TARGET_Y         ADDR(        0xfb31b20,         0x1085b59a4)
#define RVA_PLAYER_TRADE_TYPE                  ADDR(        0x11c12b94,        0x10cce2e88)
#define RVA_PLAYER_TRADE_MODE                  ADDR(        0x11c12b96,        0x10cce2e8a)
#define RVA_COMMANDER_AT                       ADDR(        0x128d70,          MAC_NOT_FOUND)
#define RVA_SQUARE_HAS_SEEN_UNIT               ADDR(        0x128a10,          MAC_INLINED)
#define RVA_COMMANDER_USE_ITEMS                ADDR(        0x185060,          MAC_INLINED)
#define RVA_UNIT_AI_SKIP                       ADDR(        0x1d97b0,          MAC_NOT_LOOKED_UP)
#define RVA_RITUAL_POINTLESS                   ADDR(        0x2cb840,          0x10055c5b0)
#define RVA_RITUAL_CASTABLE_ANYWHERE           ADDR(        0x2cb620,          0x10055baf4)
#define RVA_COMMANDER_CASTER_BONUS             ADDR(        0x128430,          0x10047ea5c)
#define RVA_SEA_AREA_SIZE                      ADDR(        0x2adbc0,          0x1004ace44)
#define RVA_AI_PROGRESS_MODE                   ADDR(        0x43c8c4,          MAC_NOT_LOOKED_UP)
#define RVA_ROUTE_REFUSE_PLAYER                ADDR(        0x43c394,          MAC_NOT_LOOKED_UP)
#define RVA_ROUTE_REFUSE_CLASS                 ADDR(        0x43c2dc,          MAC_NOT_LOOKED_UP)
#define RVA_ROUTE_MAX_SEAS                     ADDR(        0x58355f4,         0x1121124e0)
#define RVA_ROUTE_MAX_SKIES                    ADDR(        0x58355f0,         0x1121124e4)
#define RVA_MOVE_COMMANDER_STEP                ADDR(        0x1df540,          0x10059a324)
#define RVA_CLEAR_SQUARE_MARKS                 ADDR(        0x1b3370,          MAC_NOT_LOOKED_UP)
#define RVA_UNIT_RESISTANCE_1D1280             ADDR(        0x3299c0,          0x10056cb10)
#define RVA_UNIT_FIRE_RESISTANCE               RVA_UNIT_RESISTANCE_1D1280
#define RVA_UNIT_ACID_RESISTANCE               ADDR(        0x3205a0,          MAC_INLINED)
#define RVA_UNIT_COLD_RESISTANCE               ADDR(        0x329cd0,          0x10056cb74)
#define RVA_UNIT_SHOCK_RESISTANCE              ADDR(        0x329600,          0x10056cbc0)
#define RVA_UNIT_POISON_RESISTANCE             ADDR(        0x326130,          0x10056cc0c)
#define RVA_UNIT_STATE_FLAGS                   ADDR(        0x32a090,          0x10056725c)
#define RVA_UNIT_AFFLICTION_LIST               ADDR(        0x1d3a60,          0x10051fca8)
#define UNIT_FEEBLEMINDED                      (1ULL << 25)
#define RVA_UNIT_AFFLICTION_LINE               ADDR(        0x1d0dc0,          0x10051c884)
#define RVA_COMMANDER_AP_LEFT                  ADDR(        0x3289a0,          0x10056a66c)
#define RVA_RESOURCE_INCOME                    ADDR(        0x3293c0,          0x100567cac)
#define RVA_RITUAL_FX_FIRST                    ADDR(        0x2c56c0,          MAC_NOT_FOUND)
#define RVA_RITUAL_FX_FIRST_OR                 ADDR(        0x2cb830,          MAC_NOT_FOUND)
#define RVA_COUNT_UNITS_NAMED                  ADDR(        0x1295c0,          0x10047d228)
#define RVA_COUNT_MONSTER_AT                   ADDR(        0x12c340,          0x10047eb20)
#define RVA_SQUARE_ABILITY_TOTAL               ADDR(        0x1b33d0,          0x1004a95d8)
#define RVA_RELEASE_FOLLOWERS                  ADDR(        0x128c00,          MAC_INLINED)
#define RVA_CHECK_FOLLOWER_LINKS               ADDR(        0x110c50,          0x100467848)
#define RVA_RITUAL_LINK_CHECK_CALL             ADDR(        0x2c98dd,          0x100562d68)
#define RVA_RITUAL_UNFOLLOW_TARGET             ADDR(        0x2c8a66,          0x100561d28)
#define RVA_TELEPORT_ARMY                      ADDR(        0x2dc0c0,          0x100358a34)
#define RVA_TELEPORT_SQUARE                    ADDR(        0x2dc5a0,          MAC_INLINED)
#define RVA_TELEPORT_UNIT                      ADDR(        0x2dc790,          MAC_INLINED)
#define RVA_TELEPORT_COMMANDER                 ADDR(        0x2dc7c0,          0x100358ef0)
#define RVA_NET_SEND_MOVED_UNITS               ADDR(        0x1f0b20,          0x10050c650)
#define RVA_SQUARE_TERRAIN_RECORD              ADDR(        0x3349a0,          MAC_NOT_LOOKED_UP)
#define RVA_TERRAIN_BURNABLE_AT                ADDR(        0x106b60,          MAC_INLINED)
#define RVA_DIG_TUNNEL_SQUARE                  ADDR_DIFFERS(0x1af550,          0x1004a9cac)
#define RVA_PLAYER_BLESSING_VALUE              ADDR(        0x113840,          MAC_INLINED)
#define RVA_STEP_BURN_FOREST                   ADDR(        0x106af0,          MAC_INLINED)
#define RVA_AI_SPLIT_LEADER_UNLINK             ADDR(        0x07bbcd,          MAC_NOT_LOOKED_UP)
#define RVA_TRANSFER_UNIT_TO_COMMANDER         ADDR(        0x111140,          0x10046b5a8)
#define RVA_UNIT_MAY_JOIN_COMMANDER            ADDR(        0x111200,          MAC_NOT_LOOKED_UP)
#define RVA_UNIT_MAY_LEAVE_LEADER              ADDR(        0x1112d0,          MAC_INLINED)
#define RVA_UNIT_SPEED_CLASS                   ADDR(        0x325610,          0x10056a5a4)
#define RVA_ARMY_COMMANDS_MENU                 ADDR(        0x111650,          0x10046ae60)
#define RVA_ARMY_COMMANDS_MENU_CALL            ADDR(        0x121551,          0x10038599c)
#define RVA_TRANSFER_UNITS_IN_RECT             ADDR(        0x110df0,          0x10046bba8)
#define RVA_ARMY_GRID_UNIT_AT                  ADDR(        0x0e5d60,          MAC_INLINED)
#define RVA_ARMY_UNIT_GRID_CELL                ADDR(        0x0e5ac0,          MAC_INLINED)
#define RVA_ARMY_CLICK_TRANSFER_CALL           ADDR(        0x121b07,          MAC_NOT_LOOKED_UP)
#define RVA_ARMY_CLICK_FOLLOWERS_CALL          ADDR(        0x121b3a,          MAC_NOT_LOOKED_UP)
#define RVA_ARMY_CLICK_NEW_ID                  ADDR(        0x121b69,          MAC_NOT_LOOKED_UP)
#define RVA_ARMY_CLICK_RELEASE                 ADDR(        0x121b94,          MAC_NOT_LOOKED_UP)
#define RVA_ARMY_RECT_RELEASE                  ADDR_DIFFERS(0x11108c,          0x10046bfe8)
#define RVA_ARMY_TAKE_ALL_CALL                 ADDR(        0x1206af,          MAC_NOT_LOOKED_UP)
#define RVA_ARMY_DROP_STATIONARY               ADDR(        0x121d2f,          MAC_NOT_LOOKED_UP)
#define RVA_ARMY_SELF_LINK_CLEAR               ADDR(        0x121d63,          MAC_NOT_LOOKED_UP)
#define RVA_ARMY_EXIT_FLUSH_ALL                ADDR(        0x121d74,          0x100386d78)
#define RVA_LED_COUNT_CACHE_READY              ADDR(        0x5822edc,         MAC_NOT_LOOKED_UP)
#define RVA_COMMANDER_PRICE                    ADDR(        0x0b0e10,          MAC_NOT_LOOKED_UP)
#define RVA_COUNT_FREE_COMMANDERS              ADDR(        0x129360,          MAC_NOT_FOUND)
#define RVA_UNIT_HAS_SPELL_ATTACK              ADDR(        0x1d9730,          0x1005288f0)
#define RVA_UNIT_HAS_SPELL_WEAPON              ADDR(        0x3294e0,          0x10056c43c)
#define RVA_UNIT_HAS_WATER_MOVE                ADDR(        0x329940,          0x10056ce24)
#define RVA_UNIT_FLIGHT_KIND                   ADDR(        0x329fd0,          0x10056bc28)
#define RVA_UNIT_PLANE                         ADDR(        0x329990,          MAC_INLINED)
#define RVA_UNIT_BATTLE_SIZE                   ADDR(        0x321a10,          0x10056bb74)
#define RVA_UNIT_BATTLE_SIZE_2                 ADDR(        0x325270,          0x10056bac0)
#define RVA_MONSTER_BATTLE_SIZE                ADDR(        0x31ea10,          MAC_INLINED)
#define RVA_MONSTER_SPRITE_SIZE                ADDR(        0x0f95d0,          0x10057d2dc)
#define RVA_UNIT_BOAT_SIZE                     ADDR(        0x326db0,          0x10056a268)
#define RVA_UNIT_AFFLICTION_COUNT              ADDR(        0x0f69a0,          MAC_INLINED)
#define RVA_UNIT_REMOVE_ITEM                   ADDR(        0x184fc0,          0x1004e6230)
#define RVA_ITEM_ABILITY_NTH                   ADDR(        0x17e260,          0x1004e49f0)
#define RVA_ITEM_ABILITY                       ADDR(        0x185050,          MAC_NOT_FOUND)
#define RVA_TERRAIN_FLAG_21_TEST               ADDR(        0x31cba0,          MAC_NOT_LOOKED_UP)
#define RVA_SQUARE_TEXT                        ADDR(        0x1b3270,          0x1004a9268)
#define RVA_GAME_GLOBALS_INIT                  ADDR(        0x116370,          MAC_INLINED)
#define RVA_GATEWAY_CHECK                      ADDR(        0x255240,          MAC_NOT_LOOKED_UP)
#define RVA_PLAYER_TURN_END                    ADDR(        0x1e17c0,          0x1004c54d0)
#define RVA_MOVE_WANDERING_MONSTERS            ADDR(        0x17abe0,          MAC_INLINED)
#define RVA_MOVE_PLAYER_LONERS                 ADDR(        0x167140,          MAC_INLINED)
#define RVA_MOVE_PLAYER_STUPIDS                ADDR(        0x167260,          MAC_INLINED)
#define RVA_DO_ALL_FIGHTS                      ADDR(        0x0f4a10,          0x1003dbf84)
#define RVA_BATTLE_SAVE_OTHERS                 ADDR(        0x18e1a0,          MAC_INLINED)
#define RVA_BATTLE_LOAD_OTHERS                 ADDR(        0x18df60,          0x1004f4bc8)
#define RVA_BATTLE_TEMP_FOLDER                 ADDR(        0x1987e0,          MAC_INLINED)
#define RVA_PLAY_GAME_WANDERERS_CALL           ADDR(        0x091fd7,          MAC_INLINED)
#define RVA_PLAY_GAME_STUPIDS_CALL             ADDR(        0x09253b,          MAC_INLINED)
#define RVA_PLAY_GAME_REPEAT_RITUALS_CALL      ADDR(        0x092546,          0x10038ee74)
#define RVA_PLAY_GAME_IMMORTALS_CALL           ADDR(        0x0925a3,          0x10038eec8)
#define RVA_MONTH_IMMORTALS_CALL               ADDR(        0x16665a,          0x1003b713c)
#define RVA_RETURN_IMMORTALS                   ADDR(        0x123700,          0x100469d8c)
#define RVA_MOVE_INFERNO_PLAYER                ADDR(        0x1799c0,          MAC_INLINED)
#define RVA_MOVE_HADES_PLAYER                  ADDR(        0x178460,          MAC_INLINED)
#define RVA_MOVE_CELESTIAL_PLAYER              ADDR(        0x17d280,          MAC_INLINED)
#define RVA_MOVE_ALL_INDEPENDENTS              ADDR(        0x17a8a0,          0x100547c50)
#define RVA_MOVE_INDEPENDENT_ARMY              ADDR(        0x1726d0,          0x1005475dc)
#define RVA_INDEP_MOVED                        ADDR(        0x11f49840,        0x10c5e86c4)
#define RVA_INDEP_LOVED_TERRAIN                ADDR(        0x16e0f0,          0x100544fb4)
#define RVA_INDEP_LIKED_TERRAIN                ADDR(        0x16dcb0,          0x1005452d8)
#define RVA_INDEP_NEAR_PREFERRED               ADDR(        0x16dc10,          0x100545758)
#define RVA_INDEP_HATED_TERRAIN                ADDR(        0x16da80,          0x1005457ec)
#define RVA_INDEP_MOVE_TRY                     ADDR(        0x16cdb0,          0x100546a78)
#define RVA_INDEP_MOVE_STEP                    ADDR(        0x16d490,          0x1005462ac)
#define RVA_SET_HOLDIT                         ADDR(        0x172f00,          MAC_INLINED)
#define RVA_GOLD_MINE_ATTACK                   ADDR(        0x26c950,          0x1004069a8)
#define RVA_NEAREST_UNIT_ABILITY_IS            ADDR(        0x1ae2b0,          0x1004a9a48)
#define RVA_UNIT_SWIMS                         ADDR(        0x104850,          MAC_NOT_FOUND)
#define RVA_UNIT_NOLAND                        ADDR(        0x104830,          MAC_NOT_FOUND)
#define RVA_UNIT_MINDLESS                      ADDR(        0x1d9870,          0x10051fc48)
#define RVA_SQUARE_SOUTH                       ADDR(        0x2ad410,          MAC_INLINED)
#define RVA_SQUARE_NORTH                       ADDR(        0x2ad3c0,          MAC_INLINED)
#define RVA_COUNT_WATER_NEAR                   ADDR(        0x32cc40,          0x1004c8f58)
#define RVA_COUNT_LAND_NEAR                    ADDR(        0x32cb20,          0x1004c9090)
#define RVA_TERRAIN_RUIN_FORM                  ADDR(        0x32e1a0,          0x1004d0fe4)
#define SQUARE_FLAG_HOLDIT                     0x10
#define SQUARE_FLAG_ILLUSION                   0x4
#define SQUARE_FLAG_NO_SIGHT                   0x20000
#define UNIT_FLAG2_HOLDIT                      41
#define RVA_SQUARE_ARMY_STATIONARY             ADDR(        0x112170,          MAC_NOT_FOUND)
#define RVA_WANDERER_PICK_TARGET               ADDR(        0x169b90,          0x100548204)
#define RVA_WANDERER_AT                         ADDR(        0x177620,          0x100547ad8)
#define RVA_WANDERER_PICK_UP                   ADDR(        0x147610,          MAC_INLINED)
#define RVA_LONER_STEP                         ADDR(        0x172f80,                0x100545954)
#define RVA_COUNT_UNITS_FLAGGED_AT             ADDR(        0x117b20,          MAC_NOT_FOUND)
#define RVA_SQUARE_ALL_STRENGTH                ADDR(        0x0bbd40,          MAC_NOT_FOUND)
#define RVA_REMOVE_COMMANDER_ARMY              ADDR(        WIN_INLINED,       MAC_NOT_LOOKED_UP)
#define RVA_IMMORTAL_REFORM_PLACE              ADDR(        0x326de0,          0x1005682b0)
#define RVA_NEW_COMMANDER_ID                   ADDR(        0x119000,          MAC_INLINED)
#define RVA_INFERNO_ACTIVE                      ADDR(        0x5822f94,          0x10c5e86b0)
#define RVA_CELESTIAL_ACTIVE                    ADDR(        0x5822f98,          0x10c5e86b4)
#define RVA_PRIMAL_PORTALS_OPEN                 ADDR(        0x5822f90,          0x10c5e86b8)
#define RVA_GAME_STATISTICS                    ADDR(        0x31d1a0,          0x10053bb38)
#define RVA_AFTER_GAME_TURN                    ADDR(        0x161780,          0x1003a069c)
#define RVA_MONTH_SQUARE_RESET                 ADDR(        0x140580,          MAC_INLINED)
#define RVA_MONTH_SQUARE_RESET_CALL            ADDR(        0x16667d,          MAC_NO_CALL)
#define RVA_MAGIC_TAINTED_MONTHS               ADDR(        0x5822f80,         0x10ce02edc)
#define RVA_DROWN_CHECK                        ADDR(        0x114170,          MAC_INLINED)
#define RVA_DROWN_CHECK_CALL                   ADDR_DIFFERS(0x16664b,          0x1003b5bd8)
#define RVA_DROWN_CHECK_AFTER                  ADDR_DIFFERS(0x166650,          0x1003b6968)
#define RVA_DROWN_CHECK_COUNT                  ADDR(        0x5822eec,         0x106cd54cc)
#define RVA_DROWN_CHECK_TEST_VALUE             ADDR(        0x58356c4,         0x10b903c54)
#define RVA_DROWN_CHECK_TEST_FLAG              ADDR_DIFFERS(0x58356f0,         0x106cee4f4)
#define RVA_DROWN_COMMANDER_SEARCH             ADDR(        0x114815,          0x1003b6154)
#define RVA_DROWN_COMMANDER_SEARCH_2           ADDR(        WIN_NO_EQUIVALENT, 0x1003b6420)
#define RVA_DROWN_UNIT_SKIP                    ADDR(        0x1149b1,          0x1003b6140)
#define RVA_DROWN_UNIT_SKIP_2                  ADDR(        WIN_NO_EQUIVALENT, 0x1003b640c)
#define RVA_DROWN_KILL_CALL                    ADDR(        0x11499a,          0x1003b6398)
#define RVA_DROWN_KILL_CALL_2                  ADDR(        WIN_NO_EQUIVALENT, 0x1003b6668)
#define ABILITY_AIRBREATHER                    0x1d0
#define RVA_LAND_DROWN_KILL_CALL               ADDR(        0x114aed,          0x1003b5ff8)
#define RVA_UNIT_MAY_BE_ON_LAND                ADDR_DIFFERS(0x31faa0,          0x100548048)
#define UNIT_STATE_FLYING_OVER_WATER           0x2000000000
#define RVA_CRASH_LANDING_CHECK                ADDR(        0x113b30,          MAC_INLINED)
#define RVA_TURN_UNIT_UPKEEP                   ADDR(        0x143a70,          0x100399a14)
#define RVA_UPKEEP_SOUL_CONTRACTS              ADDR(        0x1462f1,          0x10039ea9c)
#define RVA_SQUARE_UPKEEP                      ADDR(        0x1dd4e0,          0x100599008)
#define RVA_BATTLE_SETUP                       ADDR(        0x0f6b60,          0x1003cf448)
#define RVA_BATTLE_SETUP_CALL                  ADDR(        0x0f5675,          0x1003dd504)
#define RVA_BATTLE_BG_IMAGE_CALL               ADDR(        0x0fb8cb,          0x1005812a4)
#define RVA_BATTLE_SEED_TRACE_CALL             ADDR(        0x0f6d45,          0x1003cf638)
#define RVA_BATTLE_SIZE_TRACE_CALL             ADDR(        0x0f7843,          0x1003cfdd4)
#define RVA_BATTLE_TREES_TRACE_CALL            ADDR(        0x0ee548,          0x1003d1208)
#define RVA_BATTLE_FORT_TRACE_CALL             ADDR(        0x0f825f,          0x1003d19c8)
#define RVA_BATTLE_EDITOR_SAVE_OPEN_CALL       ADDR(        0x085f18,          0x10051a8a4)
#define RVA_BATTLE_EDITOR_SAVE_CLOSE_CALL      ADDR(        0x087ed7,          0x10051b2d8)
#define RVA_BATTLE_EDITOR_BROWSE_CALL          ADDR(        0x085c1e,          0x100519f4c)
#define RVA_BATTLE_EDITOR_LOAD_CALL            ADDR_DIFFERS(0x085c85,          0x10051ada4)
#define RVA_BATTLE_EDITOR_ACTIVE               ADDR_DIFFERS(0x58033b4,         0x10c922c60)
#define RVA_LOAD_BATMAP                        ADDR_DIFFERS(0x087110,          0x1005180dc)
#define RVA_SAVE_BATMAP                        ADDR(        0x087bb0,          MAC_INLINED)
#define RVA_BATTLE_CREATE_TREES                ADDR(        0x0ee450,          MAC_INLINED)
#define RVA_BATTLE_PLACE_TERRAIN_UNIT          ADDR_DIFFERS(0x0f05e0,          0x1003c7c28)
#define RVA_BATTLE_BACKGROUND                  ADDR(        0x0fb6b0,          0x100580a3c)
#define RVA_SQUARE_DEFAULT_GROUND              ADDR(        0x0b4e40,          0x100550a50)
#define RVA_BATTLE_DEPLOY                      ADDR(        0x0f82f0,          0x1003dbc88)
#define RVA_BATTLE_DEPLOY_CALL                 ADDR(        0x0f5681,          MAC_INLINED)
#define RVA_BATTLE_SETUP_SIDE                  ADDR(        0x0e5da0,          0x1003d80f0)
#define RVA_BATTLE_SETUP_AMBUSH                ADDR(        0x0e8d70,          0x1003d6fe8)
#define RVA_BATTLE_BEST_DEPLOY_SQUARE          ADDR(        0x0ea660,          0x1003d6814)
#define RVA_BATTLE_COLUMN_FREE_ROW             ADDR(        0x0e9f70,          MAC_INLINED)
#define RVA_BATTLE_INTRO_SCREEN                ADDR(        0x0e17e0,          MAC_INLINED)
#define RVA_BATTLE_SHOWN_TO_SLOT               ADDR(        0x32ac50,          0x1005120b4)
#define RVA_FIGHT_SHOWN_STORE                  ADDR_DIFFERS(0x0f5553,          0x1003dc908)
#define RVA_FIGHT_SHOWN_STORE_NEXT             ADDR_DIFFERS(0x0f5558,          0x1003dc90c)
#define DO_ALL_FIGHTS_ROLES                    0x40
#define DO_ALL_FIGHTS_SHOWN                    0x58
#define RVA_FIGHT_ADVANCE_ATT_CALL             ADDR(        0x0f2c01,          0x1003e117c)
#define RVA_FIGHT_PLAN_CALL                    ADDR(        0x0f2daf,          MAC_INLINED)
#define RVA_FIGHT_DOATTACK_CALL                ADDR(        0x0f32c7,          0x1003e39e4)
#define RVA_FIGHT_WINNER_CALL                  ADDR(        0x0f34d1,          MAC_INLINED)
#define RVA_BATTLE_ADVANCE_UNIT                ADDR(        0x0c6c90,          0x1003e89b4)
#define RVA_BATTLE_FIGHT_MOVE                  ADDR(        0x0cb4d0,          0x1003f43b4)
#define RVA_BATTLE_KILLER_MOVE                 ADDR(        0x0cb110,          MAC_INLINED)
#define RVA_BATTLE_ADVANCE_RANK                ADDR(        0x0caf60,          0x1003f3d64)
#define RVA_BATTLE_FLY_MOVE                    ADDR(        0x0ca720,          0x1003f37c0)
#define RVA_BATTLE_PICK_FIXED_TARGET           ADDR(        0x0daf60,          0x1003c815c)
#define RVA_ADVANCE_FIXED_TARGET_CALL          ADDR(        0x0c8310,          MAC_NOT_LOOKED_UP)
#define RVA_BATTLE_PATHFIND                    ADDR(        0x0b7c50,          0x1003f55c8)
#define RVA_BATTLE_NEAREST_ENEMY               ADDR(        0x0cdc80,          0x1003f2344)
#define RVA_BATTLE_NEAREST_ENEMY_RING          ADDR(        0x0cde80,          0x1003f24fc)
#define RVA_BATTLE_UNIT_DISTANCE               ADDR(        0x0db6e0,          0x1003c1520)
#define RVA_BATTLE_CAN_STAND                   ADDR(        0x0ebf80,          0x1003ccd70)
#define RVA_UNIT_IN_CONTACT                    ADDR(        0x0c9290,          0x1003f4000)
#define RVA_BATTLE_LINE_OF_SIGHT               ADDR(        0x0db480,          0x1003c1bf0)
#define RVA_BATTLE_ENEMY_EXTENT                ADDR(        0x0dbb70,          MAC_INLINED)
#define RVA_BATTLE_COUNT_RANK                  ADDR(        0x0dbd40,          0x1003e735c)
#define RVA_BATTLE_WINNER                      ADDR(        0x0cc350,          MAC_INLINED)
#define RVA_BATTLE_RANDOM_TARGET               ADDR(        0x0b98d0,          0x1003cbaa0)
#define RVA_BATTLE_AOE_SCORE                   ADDR(        0x0b9fe0,          0x1003cb4bc)
#define RVA_BATTLE_TARGET_USEFUL               ADDR(        0x0b88c0,          0x1003cc410)
#define BATTLE_TARGET_NOT_AFFECTED             -3
#define RVA_WEAPON_RANGE_FOR                   ADDR(        0x31eae0,          0x10056c120)
#define RVA_UNIT_ATTACK_ROW                    ADDR_DIFFERS(0x0d7570,          0x1003f184c)
#define RVA_UNIT_BATTLE_RANK                   ADDR(        0x323600,          0x10056a49c)
#define RVA_UNIT_BATTLE_SLOWNESS               ADDR(        0x3251c0,          MAC_INLINED)
#define RVA_UNIT_BATTLE_FAST                   ADDR(        0x3221f0,          MAC_INLINED)
#define RVA_UNIT_INCAPACITATED                 ADDR(        0x0f0d40,          MAC_INLINED)
#define RVA_UNIT_MORALE                        ADDR(        0x3223e0,          0x10056aba0)
#define RVA_BATTLE_LOCAL_LEADERSHIP            ADDR(        0x0e4a30,          MAC_INLINED)
#define RVA_BATTLE_FEAR_AT                     ADDR(        0x0e4480,          MAC_INLINED)
#define RVA_BATTLE_FLEE                        ADDR(        0x0e4430,          MAC_INLINED)
#define RVA_BATTLE_REMOVE_FROM_FIELD           ADDR(        0x0e58e0,          0x1003be8a4)
#define RVA_BATTLE_UNIT_AT                     ADDR(        0x0edc30,          0x1003c0e3c)
#define RVA_BATTLE_UNIT_AT_2                   ADDR(        0x0eda80,          0x1003c1168)
#define RVA_BATTLE_UNIT_POSITION               ADDR(        0x0e5bc0,          MAC_INLINED)
#define RVA_BATTLE_PLACE_UNIT                  ADDR(        0x0e56e0,          0x1003cd6d0)
#define RVA_BATTLE_AFTERMATH_RETURN            ADDR(        0x0e2000,          MAC_INLINED)
#define RVA_BATTLE_AFTER_FIGHT_UPKEEP          ADDR(        0x0cf010,          MAC_INLINED)
#define RVA_FIGHT_THE_FIGHT_CALL               ADDR(        0x0f5705,          0x1003ddcc8)
#define RVA_REPLAY_FIGHT_CALL                  ADDR(        0x0f42d6,          0x1004c3b1c)
#define RVA_BATTLE_ROUND_CRC_CALL              ADDR(        0x0f2465,          0x1003e0a64)
#define RVA_BATTLE_ROUND_TRACE_CALL            ADDR(        0x0f247d,          0x1003e0a78)
#define RVA_UNIT_DEATH_TRACE_CALL              ADDR(        0x0bf07e,          0x1003ba4c4)
#define RVA_DAMAGE_FLAG_BERSERK_DEATH          ADDR_DIFFERS(0x5804c38,         0x106cc64c0)
#define RVA_DAMAGE_FLAG_UPKEEP                 ADDR_DIFFERS(0x5804c30,         0x106cc64c4)
#define RVA_DAMAGE_FLAG_WEAPON                 ADDR_DIFFERS(0x5804c48,         0x1069f64a8)
#define RVA_DAMAGE_UNIT_PLAIN                  ADDR(        0x0bd230,          MAC_INLINED)
#define RVA_BERSERK_END                        ADDR(        0x0b71b0,          0x1003cab58)
#define RVA_BATTLE_MESSAGE                     ADDR(        0x100500,          0x1005818d0)
#define RVA_BATTLE_CRC                         ADDR(        0x0b62b0,          0x1004c1304)
#define RVA_BATTLE_FIELD_CHECK                 ADDR(        0x0f1640,          0x1003b8900)
#define RVA_BATTLE_CLOUD_STEP                  ADDR(        0x0c0e00,          0x1003e750c)
#define RVA_BATTLE_ENSNARE_CHECK               ADDR(        0x0d3d70,          0x1003eafd0)
#define RVA_BATTLE_ENSNARE_RELEASE             ADDR(        0x0d3780,          0x1003ecf84)
#define RVA_BATTLE_WALL_ENCHANTS               ADDR(        0x0c3cf0,          0x1003d44b0)
#define RVA_BATTLE_WALL_EFFECTS                ADDR(        0x0c1e40,          0x1003efe2c)
#define RVA_BATTLE_DELAY_FOR_SPEED             ADDR(        0x0ff690,          MAC_INLINED)
#define RVA_COUNT_SIDE_UNITS                   ADDR(        0x116da0,          MAC_INLINED)
#define RVA_BATTLE_ROUND                       ADDR(        0x11f41958,        0x106cee4c8)
#define RVA_BATTLE_DELAY                       ADDR(        0xfb2cc84,         0x10cb0e188)
#define RVA_BATTLE_WIDTH                       ADDR(        0x43e084,          0x1048937f0)
#define RVA_BATTLE_HEIGHT                      ADDR(        WIN_IN_CODE,       MAC_NOT_LOOKED_UP)
#define RVA_BATTLE_WINNER_PLAYER               ADDR(        0x43e1f4,          0x100872b5c)
#define RVA_BATTLE_STARVE_ROUND                ADDR(        0x43dd84,          0x100872b08)
#define RVA_BATTLE_CHARGE_ROUND                ADDR(        0x43dd6c,          0x100872b0c)
#define RVA_BATTLE_IS_AMBUSH                   ADDR_DIFFERS(0x5804c08,         0x106cd5de4)
#define RVA_BATTLE_AT_SEA                      ADDR_DIFFERS(0x5804c14,         0x107ee6490)
#define RVA_BATTLE_PLANE_3_OR_4                ADDR(        0x5804c24,         0x107ee6494)
#define RVA_BATTLE_IS_SIEGE                    ADDR_DIFFERS(0x5804c54,         0x10c921d10)
#define RVA_BATTLE_OVER                        ADDR_DIFFERS(0x5804c1c,         0x10c96df3c)
#define RVA_BATTLE_NOT_A_SIEGE                 ADDR_DIFFERS(0x5804c4c,         0x1069b16b4)
#define RVA_BATTLE_ATT_RANK_COUNTS             ADDR_DIFFERS(0x64ab8c0,         0x1069f64ac)
#define RVA_BATTLE_DEF_RANK_COUNTS             ADDR_DIFFERS(0x64ab8d0,         0x1069f64b4)
#define RVA_BATTLE_ENEMY_EXTENTS               ADDR(        0x123358e0,        MAC_NOT_LOOKED_UP)
#define RVA_BATTLE_GRID                        ADDR(        0x11fb18c0,        0x10ca5a188)
#define BATTLE_ROWS                            20
#define RVA_BATTLE_SPEED_SETTING               ADDR(        0x1a3b3ec,         0x1048937ec)
#define UNIT_BATTLE_SIDE                       0x110
#define UNIT_ATTACKS_MADE                      0x111
#define UNIT_ATTACK_PLAN                       0x112
#define UNIT_ZAP_ITEM                          0x122
#define UNIT_FIXED_TARGET                      0x150
#define RVA_GAME_OVER                          ADDR(        0x11f720,          MAC_INLINED)
#define RVA_GAME_OVER_SCREEN                   ADDR(        0x10cbd0,          0x100473c1c)
#define RVA_GAME_OVER_EFFECT                   ADDR_DIFFERS(0x2cfde0,          0x10051c670)
#define RVA_START_TUNE                         ADDR(        0x122580,          0x100474d64)
#define RVA_SAVE_GAME                          ADDR_DIFFERS(0x18e460,          0x1004f1f1c)
#define RVA_AUTOSAVE                           ADDR(        0x093eb0,          MAC_INLINED)
#define RVA_SAVE_DIALOG                        ADDR(        0x18afb0,          0x1004f4a10)
#define RVA_LOAD_GAME_SCREEN                   ADDR(        0x18ab60,          MAC_INLINED)
#define RVA_STR_LOAD_GAME_TITLE                ADDR(        0x1c58b60,         0x1006fe5f9)
#define RVA_LOAD_GAME_BROWSER_SKIPS            ADDR_DIFFERS(0x1a3b0f4,         0x100837fc0)
#define RVA_LOAD_GAME_BROWSER_NAME             ADDR_DIFFERS(0x5822fac,         0x1005cc573)
#define RVA_STR_LOADED_START_MM                ADDR(        0x1c5c058,         0x100605ebb)
#define RVA_LOAD_START_TEST                    ADDR(        0x58356c8,         0x11307d814)
#define RVA_LOAD_START_MARK                    ADDR_DIFFERS(0x58356fc,         0x106cee4f0)
#define RVA_PLAYER_ELIMINATED                  ADDR(        0x1109a0,          MAC_INLINED)
#define RVA_REMOVE_DEAD_PLAYERS                ADDR(        0x11eb30,          0x100471b38)
#define RVA_STR_ELIMINATED                     ADDR(        0x1b29790,         0x10063186c)
#define RVA_ELIMINATED_WARN_CALL               ADDR(        0x110b22,          0x1004729cc)
#define RVA_STR_PERCENT_S_ELIMINATED           ADDR(        0x440734,          0x1005b6e1d)
#define RVA_WARN_WAIT_CALL                     ADDR(        0x043166,          0x1003483a4)
#define RVA_WARN_TIMED_WAIT_CALL               ADDR(        0x043186,          0x100348348)
#define RVA_WARN_CLOSE_MS                      ADDR(        0x57ef24c,         MAC_NOT_FOUND)
#define RVA_MODS_SCREEN                        ADDR(        0x1c9b40,          MAC_INLINED)
#define RVA_KEYBOARD_SHORTCUTS                 ADDR(        0x12c5f0,          0x100512444)
#define RVA_ROLL_CLASS                         ADDR(        0x0af7b0,          MAC_INLINED)
#define RVA_AUTO_NAME                          ADDR(        0x124a90,          0x100471334)
#define RVA_AUTO_NAME_CALL_COSTLIST            ADDR(        0x0af6b6,          0x1003622f8)
#define RVA_AUTO_NAME_CALL_PLAYER              ADDR(        0x0b0295,          0x1003611ec)
#define RVA_AUTO_NAME_CALL_HELPER              ADDR(        0x10e935,          0x100471a14)
#define RVA_AUTO_NAME_CALL_NEW_COMMANDER       ADDR(        0x128399,          0x10046de24)
#define RVA_AUTO_NAME_CALL_NET_SETUP           ADDR(        0x1ee0e2,          0x1005042e4)
#define RVA_AUTO_NAME_CALL_SETUP               ADDR(        0x304b54,          0x100589944)
#define RVA_AUTO_NAME_CALL_SERVER              ADDR(        WIN_NO_CALL,       0x1004fa398)
#define RVA_MONSTER_NAME_TYPE                  ADDR(        0x11f030,          0x100470cd0)
#define RVA_AUTO_NAME_LAST                     ADDR(        0x660efd0,         0x106cd54dc)
#define RVA_AUTO_NAME_COUNTED                  ADDR(        0x5822ed4,         0x106cd54d8)
#define AUTO_NAME_TYPES                        62
#define AUTO_NAME_LIST_SIZE                    0x5dc
#define RVA_PLACE_PLAYER                       ADDR(        0x0a8440,          0x10036ddb0)
#define RVA_ROLL_SUB_SOCIETY                   ADDR(        0x2d0150,          0x100512820)
#define RVA_MONTHLY_UPDATE                     ADDR(        0x198880,          0x100333800)
#define RVA_SQUARE_INFO                        ADDR(        0x3313f0,          0x1004c9c08)
#define RVA_SQUARE_INFO_ROW_START              ADDR_DIFFERS(0x33422c,          0x1004cd304)
#define RVA_SQUARE_PANEL                       ADDR(        0x0a3470,          MAC_INLINED)
#define RVA_MAP_SQUARE_INFO_PANEL              ADDR(        0x09fa00,          0x1003741b8)
#define RVA_LAYOUT_OVERLAP                     ADDR(        0x03a2e0,          MAC_INLINED)
#define RVA_LOBBY                              ADDR(        0x1e2fa0,          0x10059ee70)
#define RVA_DISCONNECT                         ADDR(        0x1f25c0,          0x10050713c)
#define RVA_SDL_MAIN                           ADDR(        0x18f8b0,          0x10032efe0)
#define RVA_WIN_MAIN                           ADDR(        0x001180,          MAC_NOT_FOUND)
#define RVA_WIN_MAIN_SET_MAIN_READY_CALL       ADDR(        0x0012b5,          MAC_NO_CALL)
#define RVA_WIN_MAIN_SDL_MAIN_CALL             ADDR(        0x0012bf,          MAC_NO_CALL)
#define RVA_SET_MAIN_READY_THUNK               ADDR(        0x02434d,          MAC_NO_CALL)
#define RVA_SET_MAIN_READY_IMPORT              ADDR(        0x33a4e0,          MAC_NO_CALL)
#define RVA_STEAM_DLOPEN                       ADDR(        WIN_INLINED,       0x10000fcb4)
#define RVA_STEAM_DLSYM                        ADDR(        WIN_INLINED,       0x10000fcc0)
#define RVA_GAME_MENU_JUMPS                    ADDR(        WIN_NO_EQUIVALENT, 0x100758768)
#define RVA_RESET_MODS                         ADDR(        0x1ca5d0,          0x100528d40)
#define RVA_BEFORE_GAME_START                  ADDR(        0x093fd0,          0x100389358)
#define RVA_TOP_BAR                            ADDR(        0x09bf80,          0x100377348)
#define RVA_INCOME_DETAIL_SCREEN               ADDR(        0x11afe0,          MAC_INLINED)
#define RVA_INCOME_DETAIL_CALL                 ADDR(        0x0976d9,          MAC_NOT_LOOKED_UP)
#define RVA_DROUGHT_MONTHS                     ADDR(       0x5822f68,          0x10ce02ee0)
#define RVA_TRADE_BONUS_PERCENT                ADDR(        0x32ee80,          MAC_INLINED)
#define RVA_DATE                               ADDR(        0x118890,          MAC_INLINED)
#define RVA_UNIT_SHOWN_NAME                    ADDR(        0x0f8520,          0x1003bb774)
#define RVA_COMMANDER_THE_NAME                 ADDR(        0x0f8bd0,          MAC_INLINED)
#define RVA_COMMANDER_THE_NAME_INDEX           ADDR(       0x5804c40,          0x1069b1eb0)
#define RVA_COMMANDER_THE_NAME_RING            ADDR(       0x64ebae0,          0x1069b1eb4)
#define RVA_RECRUIT_SCREEN                     ADDR(        0x124e50,          0x100479d0c)
#define RVA_RECRUIT_LOCATION_LAST              ADDR(        0x441914,          0x100872b60)
#define RVA_RECRUIT_LOCATION_X                 ADDR(        0x11f48f80,        0x106cd55e0)
#define RVA_RECRUIT_LOCATION_Y                 ADDR(        0x11f49400,        0x106cd59e0)
#define FRAME_MESSAGE_BOX_TEXT                 FRAME_OFFSET(0x000020,          0x10)
#define FRAME_RECRUIT_SCREEN_PLAYER            FRAME_OFFSET(0x000098,          MAC_NOT_LOOKED_UP)
#define FRAME_RECRUIT_SCREEN_X                 FRAME_OFFSET(0x000094,          MAC_NOT_LOOKED_UP)
#define FRAME_RECRUIT_SCREEN_Y                 FRAME_OFFSET(0x000090,          MAC_NOT_LOOKED_UP)
#define FRAME_RECRUIT_SCREEN_CLICK_LATCH       FRAME_OFFSET(0x0000d0,          MAC_NOT_LOOKED_UP)
#define RVA_PLANE_SCREEN                       ADDR(        0x1afb10,          0x1004a6880)
#define RVA_UNIT_STATS_SCREEN                  ADDR(        0x1d4290,          0x100521160)
#define RVA_ABILITY_LIST                       ADDR(        0x1d0dc0,          0x10051c884)
#define RVA_ABILITY_LOOKUP                     ADDR(        0x0b6cc0,          0x1004ea560)
#define RVA_ADDPLANE                           ADDR(        0x2505b0,          0x10056deac)
#define RVA_PLAYER_COLOUR_RPC                  ADDR(        0x301d30,          0x100586ca0)
#define RVA_DIFFICULTY_POPUP                   ADDR(        0x3028a0,          0x10058652c)
#define RVA_SAVE_LOBBY_GAME                    ADDR(        0x12e3e0,          0x1004f8278)
#define RVA_SERVER_SCREEN                      ADDR(        0x1f3220,          0x1004f8d64)
#define RVA_SETUP_SERVER                       ADDR(        0x1f0f10,          0x1005022f4)
#define RVA_NET_CLIENT_GAME                    ADDR(        0x1e8b60,          0x100507278)
#define RVA_NET_MODVAR                         ADDR(        0x1ea8c0,          MAC_INLINED)
#define RVA_NET_BEGIN_PACKET                   ADDR(        0x069c10,          0x1004f512c)
#define RVA_NET_PUT_BYTE                       ADDR(        0x069bb0,          0x1004f5198)
#define RVA_NET_PUT_STRING                     ADDR(        0x0693e0,          0x1004f55fc)
#define RVA_NET_SEND_PACKET                    ADDR(        0x0691e0,          0x1004f5854)
#define RVA_NET_GET_BYTE                       ADDR(        0x0690d0,          0x1004f58bc)
#define RVA_NET_CHECK_EVENTS                   ADDR(        0x1e8290,          0x10050d2a0)
#define RVA_NET_EVENTS_UNKNOWN_TRACE           ADDR(        0x1e83de,          0x10050d3cc)
#define RVA_NET_CLIENT_UNKNOWN_TRACE           ADDR(        0x1ea74b,          0x10050b49c)
#define RVA_NET_SERVER_LOBBYMSG_TRACE          ADDR(        0x1f681a,          0x1004fadd0)
#define RVA_PARTICIPANTS_ROW_STRIPE_CALL       ADDR(        0x1ed7f9,          0x1005032a8)
#define RVA_PARTICIPANTS_WAIT_CALL             ADDR(        0x1edf4f,          0x100503a50)
#define RVA_PARTICIPANTS_LOBBY_TRACE           ADDR(        0x1ee7da,          0x100503b84)
#define RVA_LOBBY_MESSAGE                      ADDR(        0x5827590,         0x108810294)
#define NET_PACKET_LOBBY_MESSAGE               0x43
#define NET_PACKET_LOBBY_MESSAGE_ALL           0x44
#define NET_LOBBY_MESSAGE_MAX                  0x200
#define RVA_NEW_WORLD_NET_AFTER                ADDR(        0x1ad1b0,          MAC_INLINED)
#define RVA_OPENDIR                            ADDR(        0x05c230,          0x1005aea4c)

#define RVA_NEW_WORLD_RESET_CALL               ADDR(        0x302d97,          0x10058711c)
#define RVA_NEW_WORLD_UNIQUE_INIT              ADDR(        0x302d9c,          0x100587128)
#define RVA_PARTICIPANTS_RANDOM_COLOURS        ADDR(        0x30466e,          0x1005879e8)
#define RVA_PARTICIPANTS_COLOUR_BUTTON         ADDR(        0x304944,          0x100589124)
#define RVA_NEW_WORLD_NET_INIT_CALL            ADDR(        0x302e4b,          MAC_NO_CALL)
#define RVA_NEW_WORLD_INIT_CALL                ADDR(        0x30315d,          MAC_NO_CALL)
#define RVA_TWO_PLAYER_TEAMS                   ADDR_DIFFERS(0x5826bb4,       0x10ce0cb4c)
#define RVA_INIT_PARTICIPANTS_COPY             ADDR(        WIN_NO_EQUIVALENT, 0x1005874ec)
#define RVA_INIT_PARTICIPANTS_JOIN             ADDR(        WIN_NO_EQUIVALENT, 0x1005878ac)
#define RVA_INIT_PARTICIPANTS_CONTROLLERS      ADDR(        WIN_IN_CODE,       0x1007580c0)
#define RVA_PARTICIPANTS_ROW_START             ADDR_DIFFERS(0x303bc4,          0x100588850)
#define RVA_PARTICIPANTS_ROW_START_NET         ADDR_DIFFERS(0x303bc4,          0x1005882f4)
#define RVA_PARTICIPANTS_DIFFICULTY_CALL       ADDR(        0x304780,          0x100589024)
#define RVA_NEW_WORLD_UNIQUE_CHECKBOX          ADDR_DIFFERS(0x303bae,          0x100588830)
#define RVA_NEW_WORLD_CREATING_WORLD_JOIN      ADDR(        0x304978,          0x100589258)
#define RVA_NEW_WORLD_CLASS_CHECK              ADDR(        0x3049ac,          0x1005893b8)
#define RVA_NEW_WORLD_UNIQUE_READ              ADDR(        0x304990,          0x100589278)
#define RVA_NEW_WORLD_NAME_CHECK               ADDR(        0x304a48,          0x100589870)
#define RVA_NEW_WORLD_NAME_BUFFER              ADDR(        0x304a72,          0x10058986c)
#define RVA_NEW_WORLD_NAME_BOX_CALL            ADDR(        0x304aa2,          0x100589894)
#define RVA_NEW_WORLD_PLACE_PLAYER_CALL        ADDR(        0x304d36,          0x100589ee8)
#define RVA_NEW_WORLD_CREATING_WORLD_2         ADDR(        WIN_NO_EQUIVALENT, 0x10058a5c0)
#define FRAME_NEW_WORLD_UNIQUE_RANDOM          FRAME_OFFSET(0x000064,          0x228)

#define RVA_GAME_START_CALL                    ADDR(        0x091b8f,          0x10038a074)
#define RVA_MONTH_START_CALL                   ADDR(        0x091d6c,          0x10038a3a4)
#define RVA_TURN_START_AI_CALL                 ADDR(        0x091f1f,          0x10038a9a0)
#define RVA_TURN_START_AI_SINGLE_CALL          ADDR(        WIN_NO_CALL,       MAC_NO_CALL)
#define RVA_TURN_START_INDEPENDENT_CALL        ADDR(        0x0921b6,          0x10038a774)
#define RVA_TURN_START_HUMAN_CALL              ADDR(        0x0922ce,          0x10038a5ec)
#define RVA_TURN_START_HUMAN_SINGLE_CALL       ADDR(        WIN_NO_CALL,       MAC_NO_CALL)
#define RVA_AI_TURN_CALL                       ADDR(        0x091f51,          0x10038a9dc)
#define RVA_HUMAN_TURN_CALL                    ADDR(        0x0924bf,          0x10038ae34)
#define RVA_TURN_HANDOFF_BOX                   ADDR(        WIN_INLINED,       MAC_INLINED)
#define RVA_TURN_HANDOFF_BOX_CALL              ADDR_DIFFERS(0x0923f3,          0x10038ad80)
#define RVA_STR_TURN_HANDOFF                   ADDR(        0x1b29ba0,         0x100631bc1)
#define RVA_LONE_HUMAN                         ADDR(        0xf07b020,         0x11307d7f4)
#define RVA_PLAYER_TURN_END_CALL               ADDR(        0x091fd2,          0x10038bd90)
#define RVA_GAME_OVER_CALL                     ADDR(        WIN_NO_CALL,       MAC_NO_CALL)
#define RVA_WORLD_SIZE_LOG                     ADDR(        0x092a59,          0x1003900b4)
#define RVA_GAME_OVER_SCREEN_CALL              ADDR(        0x093b2c,          0x10038fe8c)
#define RVA_GAME_OVER_WAIT_CALL                ADDR(        0x10d033,          0x1004743cc)
#define RVA_LOAD_TURN_END_CALL                 ADDR(        0x18678e,          0x1004ed444)
#define RVA_EXIT_DISCONNECT_CALL               ADDR(        0x198dee,          0x10032b37c)
#define RVA_FILE_WRITE_SPARSE_INTS             ADDR(        0x057e60,          MAC_INLINED)
#define RVA_RESET_EVENT_VARS                   ADDR(        0x31caa0,          0x100586034)
#define RVA_MAP_GAME_MENU_CALL                 ADDR(        0x09724b,          MAC_NO_CALL)
#define RVA_MAP_SQUARE_INFO_CALL               ADDR(        0x097713,          0x1003840f0)
#define RVA_MAP_SQUARE_PANEL_CALL              ADDR(        0x0a01a0,          MAC_NOT_FOUND)
#define RVA_GAME_MENU_REDRAW                   ADDR(        0x123f12,          0x10038023c)
#define RVA_GAME_MENU_MAP_DESCRIPTION          ADDR(        0x124086,          0x100380340)
#define RVA_GAME_MENU_SETTINGS_SECTION_END     ADDR(        0x124178,          0x1003803cc)
#define RVA_GAME_MENU_Q_KEY                    ADDR(        0x124408,          0x100380538)
#define RVA_GAME_MENU_SAVE_CALL                ADDR(        0x1248a4,          0x10038081c)
#define RVA_GAME_MENU_PREFS_CALL               ADDR(        0x1248c4,          0x100380834)
#define RVA_GAME_MENU_SAVE_QUIT_CALL           ADDR(        0x1248ee,          0x10038082c)
#define RVA_GAME_MENU_RESIGN_SET               ADDR(        0x1249e4,          0x100380cf4)
#define RVA_SAVE_DIALOG_SAVE_CALL              ADDR(        0x18b20c,          0x1004f4b3c)
#define RVA_SAVE_DIALOG_SAVE_FOPEN_CALL        ADDR(        0x18b1da,          MAC_NO_CALL)
#define RVA_LOBBY_SAVE_CALL                    ADDR(        WIN_NO_CALL,       0x1003b7d7c)
#define RVA_SAVE_LOBBY_GAME_CALL               ADDR(        0x12e486,          0x1004f8308)
#define RVA_SERVER_SAVE_CALL                   ADDR(        WIN_NO_CALL,       0x1004fa664)
#define RVA_SETUP_SERVER_LOAD_CALL             ADDR(        0x1f135f,          0x100502968)
#define RVA_NET_CLIENT_LOAD_CALL               ADDR(        0x1ea54a,          0x10050b9bc)
#define RVA_NET_CLIENT_WAIT_CALL               ADDR(        0x1e9957,          0x10050a6b4)
#define RVA_SETUP_SERVER_LOAD_SCREEN_CALL      ADDR(        0x1f1380,          0x10050292c)
#define RVA_SETUP_SERVER_PLAY_GAME_CALL        ADDR(        0x1f13be,          0x1005026ac)
#define RVA_SETUP_SERVER_GAME_LOOP             ADDR(        0x1f13c3,          0x1005026c0)
#define RVA_MAIN_MENU_LOAD_CALL                ADDR(        0x18ac76,          0x10032e16c)
#define RVA_MAIN_MENU_LOADED_SET               ADDR(        0x192130,          0x10032e180)
#define RVA_CMDLINE_LOAD_CALL                  ADDR(        0x190e71,          0x1003336ec)
#define RVA_CMDLINE_PLAY_GAME_CALL             ADDR(        0x190eb4,          0x10033377c)
#define RVA_BENCH_LOAD_CALL                    ADDR(        0x190c65,          0x10033321c)
#define RVA_BENCH_PLAY_GAME_CALL               ADDR(        0x190cb2,          0x100333348)
#define RVA_AUTORUN_SAVE_CALL                  ADDR(        0x091c2e,          0x10038a124)
#define RVA_AUTOSAVE_SAVE_CALL                 ADDR(        0x092495,          0x10038adfc)
#define RVA_AUTOSAVE_FUNCTION_SAVE_CALL        ADDR(        0x093f6b,          MAC_INLINED)
#define RVA_AUTORUN_SAVE_FOPEN_CALL            ADDR(        0x091c05,          MAC_NO_CALL)
#define RVA_AUTOSAVE_SAVE_FOPEN_CALL           ADDR(        0x09246d,          MAC_NO_CALL)
#define RVA_AUTOSAVE_FUNCTION_SAVE_FOPEN_CALL  ADDR(        0x093f41,          MAC_NO_CALL)
#define RVA_NET_CLIENT_MAP_SCREEN_CALL         ADDR(        0x1ea5b7,          0x10050bc04)
#define RVA_NET_CLIENT_SQUARE_INFO_CALL        ADDR(        0x1e9100,          MAC_NOT_FOUND)
#define RVA_NET_CLIENT_DATE_CALL               ADDR(        0x1e92ee,          MAC_NO_CALL)
#define RVA_TOP_BAR_DATE_CALL                  ADDR(        0x09c27d,          MAC_NO_CALL)

#define RVA_BATTLE_MR_ROLL                     ADDR(        0x0b7810,          MAC_NOT_FOUND)
#define RVA_UNIT_DRAIN_HP_CAP                  ADDR(        0x0baf10,          MAC_NOT_FOUND)
#define RVA_UNIT_CURE_AFFLICTION               ADDR(        0x0bb370,          0x1003f7644)
#define RVA_UNIT_HP_SHAPE_CHANGE               ADDR(        0x0bfcb0,          0x1003bbfe4)
#define RVA_BATTLE_START_SETUP                 ADDR(        0x0bff10,          MAC_NOT_FOUND)
#define RVA_BATTLE_TUNNEL_MOVE                 ADDR(        0x0c9c70,          0x1003f3064)
#define RVA_UNIT_CLEAR_BENEFITS                ADDR(        0x0d0160,          0x1003bea6c)
#define RVA_BATTLE_TURN_EFFECTS                ADDR(        0x0d08b0,          0x1003ed6e0)
#define RVA_WEAPON_AREA_CELLS                  ADDR(        0x0d9a60,          0x1003c6358)
#define RVA_AREA_LAST                          ADDR(        0x1b1cd90,         0x100872b20)
#define RVA_AREA_CELLS                         ADDR(        0x120658e0,        0x1069f64bc)
#define RVA_DOATTACK_SPELL_AREA_CALL           ADDR(        0x0d47c1,          0x1003ec608)
#define RVA_DOATTACK_AREA_CALL                 ADDR(        0x0d4bf1,          0x1003ecbd8)
#define RVA_WEAPON_ATTACK_AREA_CALL            ADDR(        0x0d9122,          0x1003bd4e4)
#define RVA_AREA_ADD                           ADDR(        0x0dadd0,          MAC_INLINED)
#define RVA_AREA_LISTED                        ADDR(        0x0dad40,          MAC_INLINED)
#define RVA_AREA_BEAM_DIRECTION                ADDR(        0x0da870,          0x1003c8944)
#define RVA_AREA_UNIT_NEAR                     ADDR(        0x0db230,          0x1003c98fc)
#define RVA_BATTLE_IN_RANGE                    ADDR(        0x0db400,          MAC_INLINED)
#define RVA_BATTLE_SUMMON_WEAPON               ADDR(        0x0dbee0,          MAC_INLINED)
#define RVA_BATTLE_WALL_COVER                  ADDR(        0x0e02f0,          0x1003c9d8c)
#define RVA_BATTLE_TRANSFORM                   ADDR(        0x0e0480,          0x1003cac58)
#define RVA_REMOVE_UNIMPORTANT_LOSERS          ADDR(        0x0e0d10,          MAC_INLINED)
#define RVA_BATTLE_BANISH                      ADDR(        0x0e2df0,          MAC_INLINED)
#define RVA_BATTLE_FEAR_CHECK                  ADDR(        0x0e40f0,          MAC_INLINED)
#define RVA_BATTLE_DEPLOY_UNIT                 ADDR(        0x0e5260,          0x1003bba28)
#define RVA_BATTLE_CELL_TEST                   ADDR(        0x0ecfb0,          MAC_INLINED)
#define RVA_BATTLE_SQUARE_ABILITY              ADDR(        0x0edd80,          MAC_INLINED)
#define RVA_BATTLE_GATE_SWAP                   ADDR(        0x0ee250,          0x1003cf140)
#define RVA_FOUND_ITEM                         ADDR(        0x0f0990,          0x1003ce484)
#define RVA_KILL_UNIT_999                      ADDR(        0x0f14e0,          MAC_NOT_FOUND)
#define RVA_UNIT_ADD_XP                        ADDR(        0x0f1bf0,          MAC_INLINED)
#define RVA_REMOVE_TEMP_MEN                    ADDR(        0x1177b0,          0x100469788)
#define RVA_UNIT_REGEN_PERCENT                 ADDR(        0x321ae0,          0x10056cc58)
#define RVA_UNIT_DAMAGE_BONUS                  ADDR(        0x3239b0,          0x10056b128)
#define RVA_UNIT_MR                            ADDR(        0x324820,          0x10056a3bc)
#define RVA_UNIT_IS_HUGE                       ADDR(        0x3209f0,          0x100569bec)
#define RVA_UNIT_LIFELESS                      ADDR(        0x322b80,          MAC_INLINED)
#define RVA_UNIT_PRIMABLE                      ADDR(        0x324170,          0x10056ccf8)
#define RVA_UNIT_IS_UNDEAD                     ADDR(        0x320890,          MAC_INLINED)
#define RVA_UNIT_IS_DEMON                      ADDR(        0x320f20,          MAC_INLINED)
#define RVA_HUMANOID_CORPSE_TEST               ADDR(        0x1d8f00,          MAC_NOT_FOUND)
#define RVA_UNIT_ABILITY_SUM                   ADDR(        0x128760,          MAC_INLINED)
#define RVA_BATTLE_IS_REPLAY                   ADDR_DIFFERS(0x5804be8,         0x10c96df38)

#define RVA_UNIT_REMOVE_TEMP_ABILITY           ADDR(        0x117e40,          MAC_INLINED)
#define RVA_BATTLE_SPREAD_COLLAPSE             ADDR(        0x0bf7f0,          0x1003bc4f0)
#define RVA_BATTLE_SPOILS_GOLD                 ADDR(        0x5804be0,         0x1069b16c0)
#define RVA_BATTLE_SPOILS_IRON                 ADDR(        0x5804c0c,         0x1069b16c4)
#define RVA_IN_BATTLE                          ADDR_DIFFERS(0x5804c44,         0x10c921d08)
#define RVA_BATTLE_PULSE                       ADDR(        0x5804c7c,         0x10c922c38)
#define RVA_ETHEREAL_ALPHA_BASE                ADDR(        0x406ad8,          MAC_IN_CODE)
#define RVA_ETHEREAL_ALPHA_PULSE               ADDR(        0x406b00,          MAC_IN_CODE)

#define RVA_WORLD_FIRE_UPDATE                  ADDR(        0x1433a0,          MAC_NOT_FOUND)
#define RVA_CLEAN_SQUARE                       ADDR(        0x140790,          0x10039ff68)
#define RVA_COMMANDER_GATHER_HERBS             ADDR(        0x08b300,          MAC_NOT_FOUND)
#define RVA_MOD_MAP_CHOOSER                    ADDR(        0x0883c0,          MAC_INLINED)
#define RVA_SQUARE_IS_COAST                    ADDR(        0x1b0d60,          0x1004a8fc0)

#define RVA_SANDSTORM_CLOUD_UPDATE             ADDR(        0x12ed80,          MAC_INLINED)
#define RVA_ADJUST_FOR_SNOW                    ADDR(        0x0b4040,          0x100552314)
#define RVA_SQUARE_EFFECTIVE_TEMP              ADDR(        0x330550,          0x1004c7324)
#define RVA_GROVE_REVERSION                    ADDR(        0x2d9fc0,          MAC_INLINED)
#define RVA_TROLL_FOREST_SPAWN                 ADDR(        0x2d27f0,          MAC_INLINED)
#define RVA_VOID_SHUFFLE                       ADDR(        0x250e60,          MAC_INLINED)
#define RVA_SWAP_SQUARES                       ADDR(        0x240ec0,          0x10057c578)
#define RVA_SQUARE_TEAM_ABILITY_SUM            ADDR(        0x1995f0,          0x1004a9860)
#define RVA_SQUARE_UNITS_ABILITY               ADDR(        0x1273a0,          0x1004693f4)
#define RVA_SENSEDEAD_TEXT                     ADDR(        0x2c2030,          0x100565250)
#define RVA_ARMY_SENSEDEAD                     ADDR(        0x5804b78,         MAC_NOT_FOUND)

#define RVA_TEAM_START_LOCATIONS               ADDR(        0x0a50e0,                MAC_INLINED)
#define RVA_TRIM_TEAM_LOCATIONS                ADDR(        0x0a5d40,                MAC_INLINED)
#define RVA_SWAP_TEAM_LOCATIONS                ADDR(        0x0a62a0,                MAC_INLINED)
#define RVA_CREATE_CITADELS_STEP               ADDR(        0x0a6b40,                MAC_INLINED)
#define RVA_FIX_PORTALS                        ADDR(        0x250970,          MAC_INLINED)
#define RVA_VOID_FLUCTUATIONS                  ADDR(        0x251970,                MAC_INLINED)
#define RVA_PLANESHIFT_COMMANDER               ADDR(        0x254920,          MAC_NOT_LOOKED_UP)
#define RVA_MIST_SPREAD                        ADDR(        0x12fb90,                MAC_INLINED)
#define RVA_TEMPERATURE_CHANGE                 ADDR_DIFFERS(0x0b3aa0,                0x100551978)
#define RVA_SPECIAL_SITE_POPULATE                      ADDR(        0x140dc0,                MAC_INLINED)
#define RVA_UNIT_SWALLOW                       ADDR(        0x0e2d50,                0x1003ca974)
#define RVA_UNIT_UNSWALLOW                     ADDR(        0x0e2aa0,                0x1003bc784)
#define RVA_LIFEFORCE_DRAIN_UNIT               ADDR(        0x0d59e0,                0x1003f1b48)
#define RVA_UNIT_CATCH_FIRE                    ADDR(        0x0d2b70,                0x1003f152c)
#define RVA_UNIT_CRUSH                         ADDR(        0x0d3290,                0x1003f2c84)
#define RVA_REMOVE_BAD_FOLLOWERS               ADDR(        0x110ba0,          MAC_INLINED)
#define RVA_OVERPOP_CULL                       ADDR(        0x12e4c0,                MAC_INLINED)
#define RVA_MONKEY_CLOAK                       ADDR(        0x12caf0,                MAC_INLINED)
#define RVA_RECYCLE_UNITS                      ADDR(        0x12d150,                MAC_INLINED)
#define RVA_RANDOM_TELEPORT                    ADDR_DIFFERS(0x17e2f0,          0x1004e6350)
#define RVA_POP_HORRORS                        ADDR(        0x142410,          0x1003b8498)
#define RVA_KILL_TERRAIN_UNITS                 ADDR(        0x0e1600,                MAC_INLINED)
#define RVA_KILL_SUNKEN_SHIPS                  ADDR(        0x0e0e50,                MAC_INLINED)
#define RVA_CREATE_BOATS                       ADDR(        0x0ef5b0,                MAC_INLINED)
#define RVA_BATTLE_TRAPS                       ADDR(        0x0c32b0,                MAC_INLINED)
#define RVA_BATTLE_ENCHANT_TRAPS               ADDR(        0x0c4bd0,                MAC_INLINED)
#define RVA_BATTLE_SUMMON_STRING               ADDR(        0x0d6c60,                MAC_INLINED)
#define RVA_PAY_SPELL_LIFEFORCE                ADDR(        0x0d5080,                MAC_INLINED)
#define RVA_WORLD_NEWS                         ADDR(        0x106680,                0x10047e9f8)
#define RVA_SEASON_NEWS                        ADDR(        0x103ee0,                MAC_INLINED)
#define RVA_CHECK_GAME_WON                     ADDR(        0x1107e0,                MAC_INLINED)
#define RVA_SPY_CAUGHT                         ADDR(        0x1138a0,                0x100467a0c)
#define RVA_TOGGLE_AUTO_RECRUIT                ADDR(        0x1082f0,          MAC_INLINED)
#define RVA_CHANGE_LOYALTY                     ADDR(        0x1734b0,          0x10053e3c0)

#define RVA_ALTER_TERRAIN_RECT                 ADDR(        0x32d7a0,                0x1004c6fe4)
#define RVA_RESPAWN_VOID                       ADDR(        0x2a03f0,                MAC_INLINED)
#define RVA_BATTLE_SIEGE_UNIT                  ADDR(        0x0c94a0,                0x1003e803c)
#define RVA_GATHER_SHIPS                       ADDR(        0x0c4e60,                MAC_INLINED)
#define RVA_ADVANCE_SHIPS                      ADDR(        0x0c5c30,                MAC_INLINED)
#define RVA_MOVE_ENCHANTS                      ADDR(        0x2dc410,          MAC_INLINED)
#define RVA_MOVE_GEMS                          ADDR(        0x2dc5e0,          MAC_INLINED)
#define RVA_TAINTED_RITUAL_EFFECT              ADDR(        0x2d6c00,                0x100351574)
#define RVA_LOCATION_SWAP                      ADDR_DIFFERS(0x2d8cc0,          0x100358f3c)
#define RVA_APE_POSSESSION                     ADDR_DIFFERS(0x2b0ad0,                0x10056462c)
#define RVA_RITUAL_ASK_HEMISPHERE              ADDR(        0x19a1b0,                0x1004a65a0)
#define RVA_GET_TITHES                         ADDR(        0x2d9c80,                MAC_INLINED)

#define RVA_UNIT_RANDOM_AFFLICTION             ADDR(        0x0bb900,          0x1003f73e4)
#define RVA_REVEAL_TERRAIN_ON_PLANE            ADDR(        0x2d7650,          MAC_INLINED)
#define RVA_NEAR_TERRAIN_SQUARE                ADDR_DIFFERS(0x1adef0,          0x1004a8ae0)

#define RVA_PLANE_IS_REACHABLE                 ADDR(        0x1da490,          MAC_INLINED)
#define RVA_CLAIM_SQUARES                      ADDR(        0x1438d0,          MAC_NOT_LOOKED_UP)
#define RVA_REMOVE_MIRROR_IMAGES               ADDR(        0x0e2a20,          MAC_NOT_LOOKED_UP)
#define RVA_CREATE_ICE_SHEETS                  ADDR(        0x0ef410,          MAC_NOT_LOOKED_UP)
#define RVA_EXHAUSTION_DAMAGE                  ADDR(        0x0d4f10,          MAC_NOT_LOOKED_UP)
#define RVA_ENCHANTS_DESTROYED_AT              ADDR(        0x2be650,          0x100565fc4)
#define RVA_ENCHANTS_CHECK_ALL                 ADDR(        0x2be8b0,          MAC_NOT_LOOKED_UP)
#define RVA_STORM_SPAWN                        ADDR(        0x2d1590,                MAC_INLINED)
#define RVA_WILD_SPAWN                         ADDR(        0x2d1a40,          MAC_INLINED)
#define RVA_REVELS_SPAWN                       ADDR(        0x2d2440,                MAC_INLINED)
#define RVA_FOREST_SHEPHERD_SPAWN              ADDR(        0x2d2380,          MAC_INLINED)
#define RVA_HORROR_LORD_PICK                   ADDR(        0x29ffa0,          MAC_NOT_LOOKED_UP)
#define RVA_DESERT_CLOUDS_REMOVE               ADDR(        0x12ff30,          MAC_NOT_LOOKED_UP)

// ---- User folder and startup folders ------------------------------------------------------------
#define RVA_SETUP_DIRS                         ADDR(        0x194020,          MAC_INLINED)
#define RVA_EXPAND_PATH                        ADDR(        0x05bb50,          0x1003360d4)
#define RVA_MAKE_DIR                           ADDR(        0x059400,          0x10033653c)
#define RVA_GETENV                             ADDR_DIFFERS(0x00ede8,          0x1005ae4dc)
#define RVA_USER_DIR                           ADDR(        0x58261f0,         0x113827870)
#define RVA_USER_DIR_EXPAND_CALL               ADDR_DIFFERS(0x1943ff,          0x10032f3c8)
#define RVA_EXPAND_PATH_VAR_GETENV_CALL        ADDR(        0x05bc37,          0x100336194)
#define RVA_EXPAND_PATH_HOME_GETENV_CALL       ADDR(        0x05bc7a,          0x1003361dc)
#define RVA_COE5_SAVE_GETENV_CALL              ADDR(        0x19440b,          0x10032f3d4)
#define RVA_COE5_DATA_GETENV_CALL              ADDR(        0x19411f,          0x10032f264)
#define RVA_MAPS_DIR                           ADDR(        0x5829ed0,         0x10b848b00)
#define RVA_BATMAPS_DIR                        ADDR(        0x5829500,         0x10b903289)
#define RVA_SAVES_DIR                          ADDR(        0x582a8a0,         0x1122079cd)
#define RVA_GAMELOGS_DIR                       ADDR(        0x5825750,         0x112207008)
#define RVA_OLDIPADR_FILE                      ADDR(        0x5827790,         0x1088104d5)
#define RVA_TEMP_DIR                           ADDR(        0x582b270,         0x10880f8d0)
#define RVA_STR_USER_DIR_DEFAULT               ADDR(        0x1c5ade8,         MAC_IN_CODE)
#define RVA_STR_COE5_SAVE                      ADDR(        0x1c5afe8,         0x100605207)
#define RVA_STR_COE5_DATA                      ADDR(        0x1c5afb8,         0x1006051b4)

// ---- Game state ---------------------------------------------------------------------------------
#define RVA_TURN                               ADDR(        0x443894,          0x1049c81c0)
#define RVA_ACTIVE_PLAYER                      ADDR(        0x443a9c,          0x105a0ae90)
#define RVA_LOADED_GAME                        ADDR_DIFFERS(0x5822f58,         0x10ce02ef4)
#define RVA_LOADED_MAP_NAME                    ADDR(        0x582bc40,         0x1136f574d)
#define RVA_RESIGN                             ADDR_DIFFERS(0x58294f4,         0x10ce02f00)
#define RVA_AUTORUN                            ADDR_DIFFERS(0x582815c,         0x10ce0cb68)
#define RVA_SKIP_SETUP_A                       ADDR_DIFFERS(0x5822fb8,         0x1136f6148)
#define RVA_TEXT_ONLY                          RVA_SKIP_SETUP_A
#define RVA_SKIP_SETUP_B                       ADDR_DIFFERS(0x582399c,         0x10c39946c)
#define RVA_MAP_DESCRIPTION                    ADDR_DIFFERS(0xf06c420,         0x11306ebf4)
#define RVA_SIMULTANEOUS                       ADDR(        WIN_IN_CODE,       MAC_IN_CODE)
#define RVA_AUTOSAVE_ON                        ADDR_DIFFERS(0x5828b28,         0x1069b16a4)
#define RVA_AI_MOVE_DELAY                      ADDR(        0x1a52bdc,         0x101cce574)
#define RVA_WORLD_W                            ADDR(        0xd63e948,         0x1121121ec)
#define RVA_WORLD_H                            ADDR(        0xd63e94c,         0x1121121f0)
#define RVA_SQUARE_INFO_X                      ADDR(        0x1abaff4,         0x100873504)
#define RVA_SQUARE_INFO_Y                      ADDR(        0x1aba39c,         0x100873508)
#define RVA_POINTER_SQUARE                     ADDR(        WIN_INLINED,       MAC_INLINED)
#define RVA_PLANES                             ADDR(        0xd63e920,         0x1121121c4)
#define PLANE_SIZE                             0x48
#define PLANE_NAME_SIZE                        0x20
#define PLANE_TABLE_MAX                        12
#define PLANE_X                                0x20
#define PLANE_Y                                0x24
#define PLANE_W                                0x28
#define PLANE_H                                0x2c
#define PLANE_TINT_ON                          0x30
#define PLANE_TINT                             0x34
#define RVA_MAP_DRAW_PLANE_TINT                ADDR(        0x19da74,          0x10049ec28)
#define RVA_BLOOD_SUN_UPDATE                   ADDR(        0x1b1020,          MAC_INLINED)
#define RVA_MAP_SCREEN_BLOOD_SUN_CALL          ADDR(        0x0946b0,          MAC_NO_CALL)
#define RVA_RITUAL_BLOOD_SUN_CALL              ADDR(        0x2c98f5,          MAC_NO_CALL)
#define RVA_MAP_SCREEN_BLOOD_SUN_DONE          ADDR(        0x0946b5,          0x10037b4fc)
#define RVA_RITUAL_BLOOD_SUN_DONE              ADDR(        0x2c98fa,          0x100562e54)
#define RVA_BLOOD_SUN_COLOUR                   ADDR(        0x406fd0,          0x100757cf0)
#define RVA_SKY_UNDERLAY_CHECK                 ADDR_DIFFERS(0x23fc2b,          0x10048922c)
#define PLANE_FLAGS                            0x44
#define PLANE_FLAG_DISCOVERED                  1
#define RVA_LAST_PLANE                         ADDR(        0x1adb234,         0x1049c81b8)
#define RVA_PLANE_AT                           ADDR(        0x2551d0,          0x10056de34)
#define PLANE_ELYSIUM2                         10
#define RVA_CREATE_EXTRA_PLANES                ADDR(        0x2a3370,          0x1004b46f0)
#define RVA_LOAD_MAP_PLANE                     ADDR(        0x1b5840,          0x100513bf8)
#define RVA_LOAD_AZTLAN_PLANE                  ADDR(        0x246e70,          MAC_INLINED)
#define RVA_LOAD_PRIMAL_PLANE                  ADDR(        0x24b370,          0x100578500)
#define RVA_CREATE_CELESTIAL_PLANE             ADDR(        0x24ada0,          0x1005788a0)
#define RVA_CREATE_SKY_PLANE                   ADDR(        0x24ee90,          0x100579a84)
#define RVA_CREATE_AGARTHA_PLANE               ADDR(        0x24cc40,          0x1005724d4)
#define RVA_CREATE_INFERNO_PLANE               ADDR(        0x24f950,          0x10056e004)
#define RVA_CREATE_HADES_PLANE                 ADDR(        0x244730,          MAC_INLINED)
#define RVA_CREATE_ELEMENTAL_PLANE             ADDR(        0x248610,          0x1005752c0)
#define RVA_MAP_PLANE_INDEX                    ADDR(        0x1a6b714,         0x100873500)
#define RVA_MAP_FILE_PLANE                     ADDR(        0x582c6bc,         0x108810eec)
#define RVA_MAP_FILE_HAS_PLANE                 ADDR(        0x582c680,         0x10ce0cb58)
#define RVA_MAP_PLANE_WORD_CHECK               ADDR(        0x1b5b63,          0x100513288)
#define RVA_LOAD_MAP_PLANE_LINE_CALL           ADDR(        0x1b5904,          0x100513cb4)
#define RVA_MAP_LINE_MAPSIZE_ADDPLANE_CALL     ADDR(        0x1b5cec,          0x100513364)
#define RVA_MAP_VERSION                        ADDR(        0x582c68c,         0x108810ee8)
#define RVA_MAP_LINE_ROW_BOUNDS                ADDR(        0x1b5d64,          0x1005133ac)
#define RVA_MAP_LINE_SQUARENAME_BOUNDS         ADDR(        0x1b6579,          0x100513910)
#define RVA_MAP_LINE_SQUARESPEC_OR             ADDR(        0x1b66c3,          0x100513a0c)
#define RVA_MAP_LINE_PORTAL_BOUNDS             ADDR(        0x1b6742,          0x100513a58)
#define RVA_ADDPLANE_CAP_CHECK                 ADDR(        WIN_INLINED,       0x10056def8)
#define RVA_ELYSIUM2_INDEX_CHECK               ADDR(        0x2a37da,          0x1004b4d08)
#define RVA_VOID_INDEX_CHECK                   ADDR(        0x2a36c0,          0x1004b4c74)
#define RVA_CENTER_MAP_ON                      ADDR(        0x0b0610,          0x10036d604)
#define RVA_VIEW_PLANE                         ADDR(        0x23dfc0,          0x10048f718)
#define RVA_PLANE_CAMERA_X                     ADDR(        0x9fab220,         0x106cee504)
#define RVA_PLANE_CAMERA_Y                     ADDR(        0x9fab250,         0x106cee530)
#define RVA_TOP_BAR_PLANE_SLOT_CHECK           ADDR(        0x09d196,          0x100379654)
#define RVA_LOAD_PLANE_RECORD                  ADDR(        0x1894b0,          MAC_INLINED)
#define RVA_LOAD_GAME_PLANES_CALL              ADDR(        0x186b25,          MAC_INLINED)
#define RVA_LOAD_GAME_DEPTH_FROM_ELYSIUM2      RVA_LOAD_GAME_MAP_DEPTH
#define RVA_SAVE_PLANE_RECORD                  ADDR(        0x1896d0,          MAC_INLINED)
#define RVA_SAVE_GAME_PLANES_CALL              ADDR(        0x18f35a,          MAC_INLINED)
#define RVA_MAP_DRAW_PLANE                     ADDR_DIFFERS(0x19c230,          0x10049ad74)
#define RVA_PLANE_SPAWN_CHANCE                 ADDR_DIFFERS(0x252e30,          0x100571bcc)
#define RVA_TAINTED_MAGIC_PASS                 ADDR(        0x1427b0,          MAC_INLINED)
#define RVA_WORLD_STEP_MMR                     RVA_MARK_RIVER
#define RVA_WORLD_STEP_RL2                     ADDR(        0x242450,          MAC_INLINED)
#define RVA_VOID_SPECIAL_TERRAIN               RVA_WORLD_STEP_RL2
#define RVA_PICK_PLANE_SLOT_CHECK              ADDR(        0x1afc82,          MAC_NOT_FOUND)
#define RVA_PLANE_SAME_SPOT_CAP_CHECK          ADDR(        0x253f66,          0x1005706b4)
#define RVA_PLANESHIFT_UNIT                     ADDR(        0x253aa0,          MAC_INLINED)
#define RVA_NEW_PORTAL_PAIR                    ADDR(        0x253cb0,          0x10057813c)
#define RVA_PLANE_ORDER_STEP                   ADDR(        0x2524f0,          MAC_INLINED)
#define RVA_MAP_VIEW_PLANE_LAST_STOP           ADDR(        0x23d2fa,          0x10048dd2c)
#define RVA_COUNT_TERRAIN_ON_PLANE             ADDR_DIFFERS(0x32bdd0,          0x1004ceb94)
#define RVA_SET_GROUND_PASS                    RVA_SET_GROUND_RECT
#define RVA_UNIT_SHAPE_UPDATE                  RVA_UNIT_FIT_TERRAIN
#define RVA_UNIT_PLANE_SHAPE_ID                ADDR_DIFFERS(0x1157cb,          0x1004677a4)
#define RVA_UNIT_PLANE_SHAPE_CALL              ADDR_DIFFERS(0x115807,          0x1004677bc)
#define MONSTER_ABILITY_LANDSHAPE             0x9e
#define MONSTER_ABILITY_ELYSIUMSHAPE           0x4d8
#define RITUAL_FX_SETPLANE                     938
#define RITUAL_FX_PLANEREQ                     939
#define RITUAL_FX_PLANELOC                     960
#define RITUAL_FX_RANDOMLOC                    969
#define RITUAL_FX_RANDTERRLOC                  971
#define RITUAL_FX_RANDNEWLOC                   972
#define RITUAL_FX_PLANESWAP                    984
#define RITUAL_FX_MULTIPLANE                   1048
#define RITUAL_FX_AIONLYPLANE                  1104
#define RVA_RITUAL_SETPLANE_RANDOM             ADDR_DIFFERS(0x2c5f4f,          0x1005603b0)
#define RVA_RITUAL_PLANESWAP_DST_CHECK         ADDR_DIFFERS(0x2c8737,          0x10056211c)
#define RVA_PLACE_HOME_PORTAL                  ADDR(        0x0a3ef0,          MAC_INLINED)
#define RVA_START_CHECK                        ADDR(        0x2e9e30,          MAC_INLINED)
#define RVA_AI_ONLY_PLANE_CMP                  ADDR(        0x06b8a7,          MAC_NOT_LOOKED_UP)
#define RVA_CREATE_RANDOM_MAP_ADDPLANE         ADDR_DIFFERS(0x2a0e07,          0x1004b4dec)
#define RVA_ADDPLANE_MAP_DEPTH                 ADDR(        0x2506e1,          0x10056dfb8)
#define RVA_LOAD_GAME_MAP_DEPTH                ADDR(        0x186b3a,          0x1004edfbc)
#define RVA_BUG_CHECK_ON_ELYSIUM2              ADDR(        0x12a72d,          0x100464144)
#define RVA_VIEW_PLANE_CLAMP                   ADDR(        0x23dfe6,          0x10048f744)
#define RVA_VIEW_PLANE_CAMERA_RESET            ADDR_DIFFERS(0x1ad9b0c,         0x106cee500)
#define RVA_TOP_BAR_PLANE_LOOP_END             ADDR(        0x09d2ed,          0x10037964c)
#define RVA_TOP_BAR_PLANE_SLOT_WIDTH           ADDR_DIFFERS(0x09d17f,          0x100379594)
#define RVA_PLANE_SEEN                         ADDR(        0x253de0,          MAC_INLINED)
#define RVA_PLAYER_PLANE_SEEN                  ADDR(        0x9fab280,         0x10c921ed8)
#define RVA_MAP_BG_BUILT                       ADDR(        0x63fbbc0,         0x10c6919f0)
#define RVA_MAP_BG_PIXELS                      ADDR(        0x63fbbf0,         0x10c691a78)
#define RVA_MAP_BG_DIRTY                       ADDR(        0x6498050,         0x10c691ad0)
#define RVA_MAP_BG_SIZE                        ADDR(        0x6498080,         0x10c691a1c)
#define RVA_MAP_BG_BUILDER                     ADDR(        0x0b2c00,          0x100550f50)
#define RVA_MAP_BG_BUILDER_IMAGE_CALL          ADDR(        0x0b2def,          0x100551338)
#define RVA_MAP_DRAW_PLANE_CLAMP               ADDR(        0x19c291,          MAC_NOT_FOUND)
#define RVA_MAP_BG_DIRTY_CHECK                 ADDR(        0x17e673,          0x1004e6c88)
#define RVA_MAP_BG_DIRTY_CHECK_EDITOR          ADDR_DIFFERS(0x1b4165,          0x100516188)
#define RVA_MAP_BG_MARK                        ADDR(        0x0b5120,          0x1005514d8)
#define RVA_MAP_BG_MARK_CHECK                  ADDR_DIFFERS(0x0b5136,          0x1005514f0)
#define RVA_PLANE_TABLE_REF                    ADDR_DIFFERS(0x1b05d4,          0x10056de4c)
#define RVA_MAP_BG_BUILT_REF                   ADDR_DIFFERS(0x091847,          0x1003896fc)
#define RVA_MAP_BG_DIRTY_REF                   ADDR_DIFFERS(0x0b517a,          0x100551518)
#define RVA_ICON_BUTTON_AT                     ADDR_DIFFERS(0x037c90,          0x10033eb28)
#define RVA_ICON_BUTTON_TINTED_AT              ADDR(        0x0372b0,          0x10033ed80)
#define PLANE_SPRITE_SHOWN                     0x59
#define PLANE_SPRITE_VIEWED                    0x63
#define PLANE_BUTTON_ID_FIRST                  1000
#define MISC_TRS_SPRITES                       181
#define RVA_TOP_BAR_PLANE_SHOWN_TEXT_CALL      ADDR(        0x09d28f,          0x100379640)
#define RVA_TOP_BAR_PLANE_VIEWED_TEXT_CALL     ADDR(        0x09d222,          MAC_NO_CALL)
#define RVA_TOP_BAR_PLANE_UNKNOWN_TEXT_CALL    ADDR(        0x09d2e1,          MAC_NO_CALL)
#define RVA_EDITOR_PLANE_SHOWN_TEXT_CALL       ADDR(        0x1b3902,          0x100514fe0)
#define RVA_EDITOR_PLANE_VIEWED_TEXT_CALL      ADDR(        0x1b38ac,          MAC_NO_CALL)
#define RVA_EDITOR_PLANE_UNKNOWN_TEXT_CALL     ADDR(        0x1b3952,          MAC_NO_CALL)
#define RVA_PLANE_SCREEN_CALL_RITUAL_A         ADDR(        0x2c70d4,          0x1005608b0)
#define RVA_PLANE_SCREEN_CALL_RITUAL_B         ADDR(        0x2c7206,          0x10056094c)
#define RVA_PLANE_SCREEN_CALL_OTHER            ADDR(        0x2ded7b,          0x100354b10)
#define RVA_PLANE_SCREEN_ROW_END_CALL          ADDR(        0x1b00aa,          0x1004a7128)
#define RVA_GAME_OVER_PLANE_ROW_END_CALL       ADDR(        0x1af07c,          0x1004748f8)
#define RVA_PLANE_INFO_SCREEN                  ADDR(        0x254470,          0x10057c7c8)
#define RVA_PLANE_INFO_ICON_CALL               ADDR(        0x2545f6,          0x10057ca20)
#define RVA_SAVE_GAME_MAGIC_WRITE              ADDR(        0x18e491,          0x1004f1f74)
#define RVA_SERVER_SAVE_MAGIC_WRITE            ADDR(        0x1f3e39,          MAC_NO_CALL)
#define RVA_LOAD_GAME_MAGIC_READ               ADDR(        0x1858bf,          0x1004eb3dc)
#define RVA_COUNT_SAVES_MAGIC_READ             ADDR(        0x18ae51,          0x1004eb0f0)
#define RVA_RITUAL_SETPLANE_RANDOM_CALL        ADDR(        0x2c5f53,          0x1005603b4)
#define RVA_REPLACE_SPRITE                     ADDR(        0x063770,          0x10034fc24)
#define RVA_LONG_PRESS                         ADDR(        0x23e6d0,          MAC_INLINED)
#define SQUARE_AI_STRENGTH                     0xac
#define SQUARE_TRUE_TERRAIN_SEEN               0x83
#define SQUARE_SEEN                            0x43
#define SQUARE_SEEN_MARK                       0x63
#define RVA_DATE_TEXT                          ADDR(        0x5822ef0,         0x106cee464)
#define RVA_SEASON_NAMES                       ADDR(        0x344400,          0x100834710)
#define RVA_UNIT_NAME_RING                     ADDR(        0x64a70e0,         0x1069b16dc)
#define RVA_STACK_COOKIE                       ADDR_DIFFERS(0x42c000,          0x100774220)
#define RVA_SEASON                             ADDR(        0x4438e4,          0x1049c81c4)
#define RVA_SEASON_TURNS_LEFT                  ADDR(        0x443864,          0x1049c81c6)
#define RVA_UNIT_NAME_RING_INDEX               ADDR(        0x5804bf4,         0x1069b16d8)
#define RVA_MESSAGE_LAST                       ADDR(        0x1aaa15c,         0x1049c81bc)
#define RVA_SQUARES                            ADDR(        0xa20acc0,         0x10d011c98)
#define RVA_TEST_GAME_FLAG                     ADDR_DIFFERS(0x4438bc,          0x1059bcc88)
#define RVA_TIP_SHOWN                          ADDR_DIFFERS(0x5835760,         0x106cee5ac)
#define RVA_TIP_TEXT                           ADDR(        0x58357a0,         0x106cee5ad)

#define RVA_NET_CLIENT                         ADDR_DIFFERS(0x5828b24,         0x1136f572c)
#define RVA_NET_SERVER                         ADDR_DIFFERS(0x58294f8,         0x10880c9ec)
#define RVA_NET_LOBBY                          ADDR_DIFFERS(0x5823988,         0x1136f5728)
#define RVA_NET_SCREEN_SERVER_SET              ADDR_DIFFERS(0x1f1758,          0x10032e208)
#define RVA_NET_SCREEN_CLIENT_SET              ADDR_DIFFERS(0x1f1780,          0x10032e460)
#define RVA_LOBBY_START                        ADDR(        0x1e6130,          0x1005a1a7c)
#define RVA_NET_SCREEN_LOBBY_CALL              ADDR(        0x1f17a1,          0x10032e484)
#define RVA_NET_SCREEN_LOBBY_SHAPE             ADDR(        0x1f1796,          0x10032e470)
#define RVA_SDL_MAIN_LOBBY_CALL                ADDR(        0x190d84,          0x100333290)
#define RVA_SWITCH_LOBBY_CLIENT                ADDR(        0x196ba4,          0x100329da0)

// ---- Plane generation: Celestial, Sky, Agartha, Void -------------------------------------------
#define RVA_GEN_SCRATCH_MARKS                  ADDR(        0x9fab3e0,         0x107e98282)
#define RVA_GEN_MARK_WALK                      ADDR(        0x2a84c0,          0x1004ba8f0)
#define RVA_GEN_MARKS_TO_TERRAIN               ADDR(        0x2a9050,          0x1004b9d74)
#define RVA_GEN_MARKS_TO_TERRAIN_CHANCE        ADDR_DIFFERS(0x2a9d80,          0x1004ae4b4)
#define RVA_GEN_MARK_NEAR_TERRAIN              ADDR(        0x2a8280,          0x1004ba038)
#define RVA_GEN_MARK_VOID_NEAR                 ADDR_DIFFERS(0x2a9c40,          0x1004b9ecc)
#define RVA_GEN_MARK_RECT                      ADDR(        0x2a95d0,          0x1004b9b08)
#define RVA_GEN_SET_FIXED_TEMPERATURE          ADDR(        0x2a8030,          0x1004ab608)
#define RVA_GEN_FIND_CAVE_SPOT                 ADDR_DIFFERS(0x2a9360,          0x1004ba410)
#define RVA_GEN_DIAGONAL_FIX                   ADDR_DIFFERS(0x254040,          0x100572078)
#define RVA_GEN_FLOOD_PLANE                    ADDR(        0x199240,          0x1004aa164)
#define RVA_GEN_SQUARE_IS_WET                  ADDR(        0x2acd20,          MAC_INLINED)
#define RVA_GEN_WET_NEIGHBOURS                 ADDR(        0x2ad4c0,          0x1004acc8c)
#define RVA_GEN_UPDATE_MOIST_WALLS             ADDR(        0x199040,          0x1004aa590)
#define RVA_GEN_SET_GROUND                     RVA_SET_GROUND_RECT
#define RVA_GEN_SPREAD_TERRAIN                 ADDR(        0x2412b0,          0x100573ad0)
#define RVA_CHANGE_SQUARE_TERRAIN              ADDR(        0x3345f0,          0x1004c621c)
#define RVA_GEN_KATABATIC_FIRST                ADDR(        0x1b15a24,         0x100872c38)
#define RVA_CREATE_VOID_PLANE_CODE             ADDR(        0x2a365b,          0x1004b4c5c)
#define RVA_GEN_SET_TEMPERATURES               RVA_WORLD_PART_BANDS

// ---- Plane generation: Inferno, Hades, Elemental, Aztlan, Primal, specials ----------------------------
#define RVA_PLANE_SPECIAL_TERRAIN              ADDR(        0x2e86d0,          0x100596dac)
#define RVA_INFERNO_SPECIAL_TERRAIN            ADDR(        0x246f20,          0x10056f0a4)
#define RVA_HADES_SPECIAL_TERRAIN              ADDR(        0x243580,          MAC_INLINED)
#define RVA_ELEMENTAL_SPECIAL_TERRAIN          ADDR(        0x244e20,          0x10057667c)
#define RVA_SKY_SPECIAL_TERRAIN                ADDR(        0x24b420,          0x10057a680)
#define RVA_CELESTIAL_SPECIAL_TERRAIN          ADDR(        0x24dab0,          0x100578c54)
#define RVA_AGARTHA_SPECIAL_TERRAIN            ADDR(        0x249ab0,          0x100573d28)
#define RVA_GEN_TERRAIN_BLOB                   ADDR(        0x241530,          0x1005731f4)
#define RVA_GEN_FIND_SPOT_BY_NEIGHBOURS        ADDR(        0x199c10,          0x1004a83b0)
#define RVA_GROUND_FROM_NEIGHBOURS             ADDR(        0x0b34d0,          0x100550154)
#define RVA_INIT_SQUARE_CORPSES                ADDR(        0x32c010,          0x1004ce1f0)
#define RVA_INIT_SQUARE_GEMS                   ADDR(        0x32ce80,          0x1004c66f4)
#define RVA_NAME_ALL_SQUARES                   ADDR(        0x2e8c00,          MAC_INLINED)
#define RVA_CITY_NAMES_SWITCH                  ADDR(        0x1a3d23c,         0x10ce0cb6c)
#define RVA_SQUARE_NAME_TEXT                   ADDR(        0x2e8e70,          0x1005983b0)
#define RVA_MAP_MARKS                          RVA_GEN_SCRATCH_MARKS
#define RVA_MAP_MARKS_CLEAR_CALL               ADDR_DIFFERS(0x2a2203,          0x1004b4ed4)
#define RVA_MARKS_SAVE                         ADDR(        0x2a0c90,          MAC_NOT_FOUND)
#define RVA_MARKS_RESTORE                      ADDR(        0x2a0c20,          MAC_NOT_FOUND)
#define RVA_MARK_RECT                          RVA_GEN_MARK_RECT
#define RVA_MARK_TO_TERRAIN                    RVA_GEN_MARKS_TO_TERRAIN_CHANCE
#define RVA_MARK_TERRAIN_CHANGE                RVA_GEN_MARKS_TO_TERRAIN
#define RVA_MARK_BLOB                          ADDR(        0x2a7d80,          0x1004b98a0)
#define RVA_MARK_CAVE                          RVA_GEN_MARK_WALK
#define RVA_MARK_RIVER                         ADDR(        0x2a87e0,          0x1004ad8c4)
#define RVA_RIVER_DISTANCE                     ADDR(        0xd5a2520,         0x107e4a078)
#define RVA_RIVER_COST                         ADDR(        0xd5f0720,         0x107dfbe78)
#define RVA_MARK_RANDOM_LOC                    ADDR(        0x2a8150,          0x1004ba1a8)
#define RVA_MARK_NEIGHBOURS                    RVA_GEN_MARK_NEAR_TERRAIN
#define RVA_CAVE_WALL_SPOT                     RVA_GEN_FIND_CAVE_SPOT
#define RVA_SET_COLD_FLAGS                     ADDR(        0x2a83d0,          MAC_INLINED)
#define RVA_HEAT_BLOB                          ADDR(        0x2a6630,          MAC_INLINED)
#define RVA_SET_PORTAL_PAIR                    ADDR(        0x241080,          0x1005703fc)
#define RVA_LINK_SQUARES_NEW_PORTAL            RVA_NEW_PORTAL_PAIR
#define RVA_RANDOM_LAND_SQUARE                 RVA_RANDOM_RITUAL_SQUARE
#define RVA_SQUARE_IS_OPEN_LAND                ADDR(        0x2a3970,          MAC_INLINED)
#define RVA_SQUARE_WATERLIKE                   ADDR(        0x2ad910,          0x1004acb7c)
#define RVA_SPREAD_TERRAIN                     RVA_GEN_SPREAD_TERRAIN
#define RVA_INFERNO_CENTRE_X                   ADDR(        0x1ae05c4,         0x10487ff30)
#define RVA_INFERNO_CENTRE_Y                   ADDR(        0x1ae0c34,         0x10487ff34)
#define RVA_TERRAIN_MATCH_ELEMENTAL_QUARTER    ADDR_DIFFERS(0x2c49e6,          0x1005550f8)
#define RVA_TERRAIN_MATCH_INFERNO_REGION       ADDR_DIFFERS(0x2c4f53,          0x1005559a4)
#define RVA_SET_TEMPERATURE_RECT               RVA_WORLD_PART_BANDS
#define SQUARE_GROUND                          0x22
#define SQUARE_TEMPERATURE                     0x26
#define SQUARE_TEMPERATURE_OFFSET              0x28
#define SQUARE_FLAG_COLD                       8
#define SQUARE_COLD_TEMPERATURE_MAX            25
#define SQUARE_GROUND_LAST                     28
#define SQUARE_NAME                            0x0
#define SQUARE_NAME_SIZE                       0x20

// ---- New game settings --------------------------------------------------------------------------
#define RVA_SOCIETY                            ADDR(        0xf07b00a,         0x11307d7de)
#define RVA_COMMON_CAUSE                       ADDR(        0xf07b018,         0x11307d7ec)
#define RVA_SCORE_GRAPHS                       ADDR(        0xf07b01c,         0x11307d7f0)
#define RVA_BATTLE_REPORTS                     ADDR(        0xfb2cc2c,         0x10ce0cb56)
#define RVA_START_FLAGS                        ADDR(        0xf07b008,         0x11307d7dc)
#define RVA_WILDER                             ADDR(        0xf07b030,         0x11307d804)
#define RVA_MAP_W                              ADDR(        0x582c60c,         0x11307d80c)
#define RVA_MAP_H                              ADDR(        0x582c608,         0x11307d810)
#define RVA_MAP_DEPTH                          ADDR(        0x582c604,         0x1121121c0)
#define RVA_MAP_EDITOR_DEPTH_NOP               ADDR(        WIN_NO_EQUIVALENT, 0x1005161ec)
#define RVA_NORTH_PART                       ADDR(        0x1a51e94,         0x105a0aecc)
#define RVA_SOUTH_PART                         ADDR(        0x1a462c4,         0x105a0aed0)
#define RVA_UNIQUE_RANDOM_DEFAULT              ADDR_DIFFERS(0x5824d7c,         0x10ce0cb50)
#define RVA_TEST_GAME_A                        RVA_MAP_W
#define RVA_TEST_GAME_B                        RVA_MAP_H
#define RVA_CLI_MAP_W                          ADDR(        0x1a406f4,         0x100872c14)
#define RVA_CLI_MAP_H                          ADDR(        0x1a50dfc,         0x100872c18)
#define RVA_CLI_SOCIETY                        ADDR(        0x1a3b3ac,         0x100872c20)
#define RVA_START_DEFAULT                      ADDR(        0x1a3b2fc,         0x100872c1c)
#define RVA_WILDER_DEFAULT                     ADDR_DIFFERS(0x1a3b334,         0x107e9827c)
#define RVA_SUBSOC                             ADDR_DIFFERS(0x58261ec,         0x107e98280)
#define RVA_SETUP_NETWORK                      ADDR(        0xf07ae80,         0x11307d654)
#define RVA_CLI_GRAPHS                         ADDR_DIFFERS(0x5822fbc,         0x107e98278)
#define RVA_CLI_COMMON_CAUSE                   ADDR_DIFFERS(0x5823990,         0x106cd54d4)
#define RVA_CLI_NEW_GAME                       ADDR_DIFFERS(0x582437c,         0x108810494)
#define RVA_CLI_MAP_EDITOR                     ADDR_DIFFERS(0x5826118,         0x105aa6804)
#define RVA_SDL_MAIN_STARTUP_MODS_RUN_CALL     ADDR(        0x190ba9,          0x1003328e4)
#define RVA_CLI_BATTLE_REPORTS                 ADDR_DIFFERS(0x5823998,         0x10ce0cb54)
#define RVA_MAP_SIZE_WIDTHS                    ADDR_DIFFERS(0x406fa0,          0x10075c4d0)
#define RVA_MAP_SIZE_HEIGHTS                   ADDR_DIFFERS(0x406f90,          0x10075c4e4)
#define RVA_SOCIETY_TABLE                      ADDR(        0x5697b80,         0x100872c90)
#define SOCIETY_TABLE_MAX                      12
#define RVA_MAP_SETTINGS_SUB_SOCIETY_CALL      ADDR(        WIN_NO_CALL,       0x1004b2cf0)
#define RVA_MAP_SETTINGS_COMMON_CAUSE_CHECKBOX ADDR(        0x2aa8f9,          0x1004b28c8)
#define RVA_MAP_SETTINGS_START_FLAGS_CLEAR     ADDR(        0x2aae9a,          0x1004b364c)
#define RVA_MAP_SETTINGS_WIDTHS_REF            ADDR(        0x2aa020,          0x1004b3a70)
#define RVA_MAP_SETTINGS_HEIGHTS_REF           ADDR(        0x2aa02d,          0x1004b3b1c)
#define RVA_AUTO_NAME_TABLE                    ADDR(        0x344420,          0x10077ecd0)
#define RVA_CLI_CLUSTER_START_STORE            ADDR_DIFFERS(0x19537c,          0x10032945c)
#define RVA_CLI_NO_CLUSTER_START_STORE         ADDR(        0x1953ad,          0x100329468)
#define RVA_CLI_WEST_EAST_STORE                ADDR_DIFFERS(0x19540b,          0x10032945c)
#define RVA_CLI_NORTH_PART_STORE               ADDR(        0x1966cb,          0x100329b4c)
#define RVA_CLI_SOUTH_PART_STORE               ADDR(        0x19670d,          0x100329b74)
#define RVA_MAP_SETTINGS_START_READ            ADDR(        0x2aa0a4,          0x1004b2608)
#define RVA_MAP_SETTINGS_NORTH_DEFAULT         ADDR(        0x2aa319,          0x1004b2840)
#define RVA_MAP_SETTINGS_SOUTH_DEFAULT         ADDR(        0x2aa32f,          0x1004b2858)
#define RVA_MAP_SETTINGS_NORTH_STORE           ADDR(        0x2aad23,          0x1004b303c)
#define RVA_MAP_SETTINGS_SOUTH_STORE           ADDR(        0x2aada6,          0x1004b30d8)
#define RVA_MAP_SETTINGS_WILDER_STORE          ADDR(        0x2aaed4,          0x1004b36bc)
#define RVA_NUMBER_BOX                         ADDR(        0x042770,          0x1003489ac)
#define RVA_SETTINGS_FROM_CLI                  ADDR(        0x110d80,          MAC_INLINED)
#define RVA_LOAD_GAME_WILDER                   ADDR(        0x18a66c,          0x1004ebc5c)
#define RVA_SAVE_GAME_WILDER                   ADDR(        0x18a9d0,          0x1004f2500)
#define RVA_SETTINGS_BLOCK_LOAD                ADDR(        0x18a3e0,          MAC_NOT_FOUND)
#define RVA_SETTINGS_BLOCK_SAVE                ADDR(        0x18a820,          MAC_NOT_FOUND)
#define RVA_WORLD_NORTH_PART                   ADDR(        0x5835f9c,         0x108810ea8)
#define RVA_WORLD_SOUTH_PART                   ADDR(        0x5835f98,         0x108810eac)
#define RVA_SOCIETY_PRESETS                    ADDR(        0x2d0200,          MAC_INLINED)
#define RVA_WORLD_PART_BANDS                   ADDR(        0x2a96b0,          0x1004ab808)
#define RVA_WORLD_PART_CHECK                   ADDR(        0x2a6334,          0x1004b8548)
#define RVA_WORLD_PART_TERRAIN                 ADDR(        0x2a6330,          MAC_NOT_FOUND)

#define RVA_NEW_GAME_WORLD                     ADDR(        0x304fe0,          0x1005914ec)
#define RVA_NEW_WORLD_NEW_GAME_CALL            ADDR(        0x304c5a,          0x100589e40)
#define RVA_NEW_WORLD_EXTRA_PLANES_CALL        ADDR(        0x304c68,          0x100589e24)
#define RVA_NEW_WORLD_SPECIAL_TERRAIN_CALL     ADDR(        0x304c6f,          0x100589e2c)
#define RVA_NEW_GAME_RESET_CACHE_CALL          ADDR(        0x3050df,          0x100591630)
#define RVA_NEW_GAME_PRESETS_CALL              ADDR(        0x3050eb,          MAC_NO_CALL)
#define RVA_NEW_GAME_CREATE_MAP_CALL           ADDR(        0x30510b,          0x1005919b4)
#define RVA_NEW_GAME_EXTRA_PLANES_CALL         ADDR(        0x305119,          0x1005919c4)
#define RVA_NEW_GAME_CROSSINGS_CALL            ADDR(        0x30512e,          MAC_NO_CALL)
#define RVA_NEW_GAME_DECORATE_CALL             ADDR(        0x31ad05,          0x1005924d4)
#define RVA_NEW_GAME_SET_GROUND_CALL           ADDR_DIFFERS(0x31c4cb,          0x100596868)
#define RVA_NEW_GAME_SPECIAL_TERRAIN_CALL      ADDR(        0x31c5f0,          0x100596d48)
#define RVA_CREATE_RANDOM_MAP                  ADDR(        0x2a0d50,          0x1004b4d64)
#define RVA_CREATE_MAP_SETTEMP_CALL            ADDR(        0x2a2762,          0x1004b7348)
#define RVA_CREATE_MAP_ENLARGE_CALL_1          ADDR_DIFFERS(0x2a32b7,          0x1004b97a4)
#define RVA_CREATE_MAP_ENLARGE_CALL_2          ADDR_DIFFERS(0x2a32da,          0x1004b97b4)
#define RVA_MAPGEN_HEIGHT_MAP                  ADDR(        0x054670,          MAC_INLINED)
#define RVA_MAPGEN_REMOVE_SMALL_ISLANDS        ADDR(        0x2a55c0,          MAC_INLINED)
#define RVA_MAPGEN_SEA_PART                    ADDR(        0x2a3a10,          MAC_INLINED)
#define RVA_MAPGEN_RANDOM_SQUARE               ADDR(        0x2a3810,          0x1004b444c)
#define RVA_MAPGEN_SPEC_BLOB                   ADDR(        0x2a5c40,          0x1004ac400)
#define RVA_MAPGEN_ADJUST_TEMPERATURE          ADDR(        0x2a64c0,          MAC_INLINED)
#define RVA_MAPGEN_SIDE_WATER                  ADDR(        0x2ad460,          MAC_INLINED)
#define RVA_MAPGEN_NEAREST_TERRAIN             ADDR(        0x2a49b0,          0x1004ae704)
#define RVA_MAPGEN_DRAW_RIVER                  ADDR(        0x2a5320,          0x1004ae1c4)
#define RVA_MAPGEN_SQUARE_SCRATCH              RVA_GEN_SCRATCH_MARKS
#define RVA_RANDOM_OPEN_DIE_SUM                RVA_DIE_OPEN_ENDED
#define RVA_SET_GROUND_RECT                    ADDR(        0x0b4610,          0x1005502b8)
#define RVA_SET_GROUND_PLANE_FOUND             ADDR_DIFFERS(0x0b4768,          0x100550448)
#define RVA_SET_GROUND_TERRAIN_READ            ADDR_DIFFERS(0x0b46e7,          0x1005503d8)
#define RVA_SET_GROUND_PLANE                   ADDR(        0x0b3490,          MAC_INLINED)
#define RVA_ENLARGE_PLANE                      ADDR_DIFFERS(0x241ac0,          0x10056eb64)
#define RVA_MAPGEN_RIVER_CROSSINGS             ADDR(        0x2a4b90,          MAC_INLINED)
#define RVA_DECORATE_SQUARE                    ADDR(        0x2ab610,          0x1004af87c)
#define RVA_DECORATE_PLANE0_TEST               ADDR_DIFFERS(0x2aca8c,          0x1004b2408)
#define RVA_DECORATE_WORLD_W_READ              ADDR_DIFFERS(0x2ac647,          0x1004b1b8c)
#define RVA_PUT_RARE_SQUARE                    ADDR(        0x301b60,          MAC_INLINED)
#define RVA_MAPGEN_PORTAL_PAIR                 RVA_NEW_PORTAL_PAIR
#define RVA_MAPGEN_ANCIENT_FOREST              ADDR(        0x2e98f0,          MAC_INLINED)
#define RVA_MAPGEN_CLEAR_AREA                  ADDR(        0x2a57c0,          MAC_INLINED)
#define RVA_MAPGEN_SLOT_IN_PLAY                ADDR(        0x31e730,          MAC_INLINED)
#define RVA_MAPGEN_COUNT_TERRAIN               ADDR(        0x2a4ad0,          MAC_INLINED)
#define RVA_GEN_RUIN_TEMPLE_SHARE              ADDR(        0x11f48ed8,        0x108810ea2)
#define RVA_GEN_DEAD_FOREST_CHANCE             ADDR(        0x11f49380,        0x108810e9e)
#define RVA_GEN_ANCIENT_FOREST_RATE            ADDR(        0x11f9efa0,        0x10ce0cb72)
#define RVA_GEN_SWAMP_CHANCE                   ADDR(        0x11f49818,        0x108810ef2)
#define RVA_GEN_BATTLEFIELD_SHARE              ADDR(        0x11f49816,        0x108810e9c)
#define RVA_GEN_BATTLEFIELD_CHANCE             ADDR(        0x11f97a44,        0x10c922a00)
#define RVA_GEN_CASTLE_CHANCE                  ADDR(        0x11f48edc,        0x108810e9a)
#define RVA_GEN_TOWER_CHANCE                   ADDR(        0x11f48f16,        0x10c922a02)
#define RVA_GEN_FORD_CHANCE                    ADDR(        0x11f49814,        0x108810ea4)
#define RVA_GEN_UNUSED                         ADDR(        0x11f49820,        MAC_NOT_FOUND)
#define RVA_GEN_SKY_FEATURE_CHANCE             ADDR(        0x11f48f34,        0x10ce0cb76)
#define RVA_GEN_LIBRARY_CHANCE                 ADDR(        0x11f48f54,        0x10ce0cb74)
#define RVA_GEN_RARE_CHANCE                    ADDR(        0x11f48eda,        0x10ce0cb70)
#define RVA_GEN_HILLS_CHANCE                   ADDR_DIFFERS(0x11f991d0,        0x108810ef4)
#define RVA_GEN_TOWN_SHARE                     ADDR(        0x11f48f14,        0x10c9229fc)
#define RVA_GEN_ANCIENT_RUIN_CHANCE            ADDR(        0x11f48ea8,        0x10c9229fe)
#define RVA_GEN_VILLAGE_CHANCE                 ADDR(        0x11f493c4,        0x10ce0cb6e)
#define RVA_GEN_MINE_CHANCE                    ADDR(        0x11f493b4,        0x108810ea0)
#define RVA_GEN_REALM_WORD                     ADDR(        0x11d20bd8,        0x10cdf0ecc)

#define RVA_NEW_WORLD_BATTLE_REPORTS_INIT      ADDR(        0x302db3,          0x10058714c)
#define RVA_SAVE_GAME_BATTLE_REPORTS           ADDR(        0x18e864,          0x1004f233c)
#define RVA_SAVE_GAME_COMMON_CAUSE             ADDR(        0x18a884,          0x1004f2410)
#define RVA_SAVE_GAME_SCORE_GRAPHS             ADDR(        0x18a8c0,          0x1004f2438)
#define RVA_LOAD_GAME_BATTLE_REPORTS           ADDR(        0x185e1a,          0x1004eba20)
#define RVA_LOAD_GAME_COMMON_CAUSE             ADDR(        0x18a4c4,          0x1004ebb1c)
#define RVA_LOAD_GAME_SCORE_GRAPHS             ADDR(        0x18a508,          0x1004ebb58)
#define RVA_SAVE_PLAYERS                       ADDR(        0x187920,          MAC_INLINED)
#define RVA_SAVE_PLAYERS_AI_LEVEL              ADDR(        0x187b27,          0x1004f2ca4)
#define RVA_LOAD_PLAYERS_AI_LEVEL              ADDR(        0x1875e7,          0x1004eca60)
#define RVA_TURN_START_AI_LEVEL                ADDR(        0x167743,          0x100392940)
#define RVA_AI_LEVEL_BONUS                     ADDR(        0x344290,          0x10075c518)
#define RVA_ADD_RESOURCE                       ADDR(        0x326d30,          MAC_NOT_FOUND)
#define AI_LEVEL_TABLE_MAX                     11
#define RVA_COMMON_CAUSE_CHECK                 ADDR(        0x11eb30,          0x100471b38)
#define RVA_WINNER_CHECK                       ADDR(        0x11c6a0,          0x10046caec)
#define RVA_GAME_WINNER                        ADDR(        0xfb2cc20,         0x10ce02f08)
#define RVA_PLAYER_HAS_COMMANDER               ADDR(        0x10cac0,          MAC_INLINED)
#define RVA_PLAYER_HAS_CITADEL                 ADDR(        0x10c7d0,          0x10046c8b4)
#define RVA_PLANES_ALIKE                       ADDR(        0x253950,          MAC_INLINED)
#define RVA_PLAYER_CLASS_ABILITY               ADDR(        0x0b0c30,          MAC_INLINED)
#define RVA_CLASS_ABILITY_NTH                  ADDR(        0x0afef0,          0x10036155c)
#define RVA_COMMANDER_OWNER                    ADDR(        0x1289c0,          MAC_INLINED)
#define RVA_BATTLE_REPORTS_BATTLE_READ         ADDR(        0x0f55c5,          0x1003dd160)
#define RVA_TOP_BAR_SCORE_GRAPHS               ADDR(        0x09d665,          0x10037aa74)
#define RVA_MISC_MENU_SCORE_GRAPHS             ADDR(        0x0a106c,          0x1003730d0)
#define RVA_PLAYER_OVERVIEW_SCORE_GRAPHS       ADDR(        0x122d71,          0x100482440)
#define RVA_NET_SCORE_GRAPHS_READ              ADDR(        0x1e9db1,          0x10050a7bc)
#define RVA_MAP_KEY_SCORE_GRAPHS               ADDR(        0x098069,          MAC_NOT_FOUND)
#define RVA_TYPED_WORD_SCORE_GRAPHS            ADDR(        WIN_NOT_FOUND,     MAC_NOT_FOUND)

// ---- Players ------------------------------------------------------------------------------------
#define PLAYER_SLOTS                           32
#define RVA_SLOT_CONTROLLER                    ADDR(        0xfb31ac0,         0x1121af140)
#define RVA_SLOT_CLASS                         ADDR(        0xfb2cc40,         0x112206fc8)
#define RVA_PLAYER_HOME_X                      ADDR(        0x11d32be0,        0x10cce2dd4)
#define RVA_PLAYER_HOME_Y                      ADDR(        0xfb8e780,         0x10cce2e14)
#define RVA_SLOT_AI                            ADDR(        0x11c12b92,        0x10cce2e86)
#define RVA_SLOT_TEAM                          ADDR(        0x11c12b90,        0x10cce2e84)
#define RVA_SLOT_NAME                          ADDR(        0x11c12b60,        0x10cce2e54)
#define PLAYER_BLOCK_SIZE                      0x9004
#define RVA_CAST_COUNTS                        ADDR(        0x11c12b98,        0x10cce2e8c)
#define RVA_AI_LEVEL_NAMES                     ADDR(        0x3442c0,          0x100848390)
#define RVA_SLOT_COLOUR                        ADDR(        0x11c1bb18,        0x10ccebe0c)
#define RVA_SLOT_LOCK_CLASS                    ADDR(        0xf07ae86,         0x11307d65a)
#define RVA_SLOT_LOCK_CONTROLLER               ADDR(        0xf07af06,         0x11307d6da)
#define RVA_SLOT_LOCK_AI                       ADDR(        0xf07af46,         0x11307d71a)
#define RVA_SLOT_LOCK_TEAM                     ADDR(        0xf07aec6,         0x11307d69a)
#define RVA_PLAYER_INTS                        ADDR(        0x11c14490,        0x10cce4784)
#define RVA_PLAYER_SHORT                       ADDR(        0x11c14488,        0x10cce477c)
#define RVA_PLAYER_LEADER                      ADDR(        0x11c1448c,        0x10cce4780)
#define RVA_PLAYER_BLESSINGS                   ADDR(        0x11c144c8,        0x10cce47bc)
#define PLAYER_BLESSINGS_MAX                   24
#define RVA_PLAYER_BOLSTERS                    ADDR(        0x11c1bb0c,        0x10ccebe00)
#define RVA_PLAYER_DIVINE_FAVOR                ADDR(        0x11c1bb10,        0x10ccebe04)
#define RVA_PLAYER_INT_A                       RVA_PLAYER_BOLSTERS
#define RVA_PLAYER_INT_B                       RVA_PLAYER_DIVINE_FAVOR
#define RVA_PLAYER_INT_C                       ADDR(        0x11c1bb14,        0x10ccebe08)
#define RVA_PLAYER_COLOURS                     ADDR(        0x1a299b0,         0x1049c81c8)
#define RVA_RESET_SLOT_COLOURS                 ADDR(        0x31c848,          0x100585e84)
#define RVA_CHOOSE_COLOUR_SCREEN               ADDR(        0x088b90,          0x100490614)
#define RVA_LOAD_PLAYER_2H                     ADDR(        0x185460,          0x1004f4d60)
#define RVA_SERVER_LOAD_2H_CALL                ADDR(        0x1f7316,          0x1004feed4)
#define RVA_SAVE_PLAYER_2H                     ADDR(        0x185120,          MAC_INLINED)
#define RVA_MAP_LINE                           ADDR(        0x1b5950,          0x100512b4c)
#define RVA_MAP_FIXEDPLAYER_LOCKS              ADDR(        0x1b6431,          0x10051381c)
#define RVA_RESET_MAP_LOCKS                    ADDR(        0x1ad4f0,          MAC_INLINED)
#define RVA_PLAY_GAME_CONTROLLER_DONE          ADDR(        0x091f8d,          0x10038bd18)
#define RVA_ELIMINATE_PLAYER                   ADDR(        0x106bd0,          MAC_INLINED)
#define RVA_ELIMINATE_CONTROLLER               ADDR(        0x106c0a,          0x100472a14)
#define RVA_ELIMINATE_CONTROLLER_HUMAN         ADDR(        0x106c33,          0x100472a58)
#define RVA_GAME_OVER_WINNER_HUMAN             ADDR(        0x10cca6,          0x100473f9c)
#define RVA_CONQUER_SQUARE                     ADDR(        0x1ae620,          0x1004aa750)
#define RVA_CONQUER_CONTROLLER_STORE           ADDR(        0x1ae920,          0x1004aa938)
#define RVA_SQUARE_IS_CITADEL_FOR              ADDR(        0x0b1210,          0x10036255c)
#define RVA_CAN_OWN_SQUARE                     ADDR(        0x330c30,          0x1004c5d38)
#define RVA_SQUARE_SET_OWNER                   ADDR(        0x334950,          MAC_INLINED)
#define RVA_PLAYER_GATHERS_FUNGUS              ADDR(        0x0b0d10,          0x100371ac4)
#define RVA_PLAYER_GATHERS_HERBS               ADDR(        0x0b2330,          0x100371c38)
#define RVA_PLAYER_GATHERS_WEED                ADDR(        0x0b1160,          0x100371dac)
#define RVA_PLAYER_GATHERS_SACRIFICES          ADDR(        0x0b0410,          MAC_INLINED)
#define RVA_PLAYER_GATHERS_HANDS               ADDR(        0x0b0b20,          0x100371fa0)
#define RVA_PLAYER_GATHERS_GEMS                ADDR(        0x0b1560,          MAC_INLINED)
#define RVA_PLAYER_GATHERS_ANY_GEMS            ADDR(        0x0b04c0,          0x1003718ac)
#define RVA_COMMANDER_GATHERS_SACRIFICES       ADDR(        0x08cd70,          0x100371340)
#define RVA_COMMANDER_GATHERS_GEMS             ADDR(        0x08f000,          0x1003715b0)
#define RVA_PLAYER_GATHERS_LIFEFORCE           ADDR(        0x0b0360,          0x1003722f8)
#define RVA_SERVER_SET_CONTROLLER              ADDR(        0x1f3ada,          0x1004fa768)
#define RVA_SERVER_SET_CONTROLLER_HUMAN        ADDR(        0x1f3af6,          0x1004fa79c)
#define RVA_REPLACE_CONTROLLER                 ADDR(        0x1f02d0,          0x100501ff0)
#define RVA_LOAD_PLAYERS_TURN_CONTROLLER       ADDR_DIFFERS(0x187215,          0x1004ec390)
#define RVA_SAVE_GAME_CONTROLLERS              ADDR_DIFFERS(0x1879d1,          0x1004f2b1c)
#define RVA_LOAD_PLAYERS_TEAM                  ADDR(        0x1875ad,          0x1004eca38)
#define RVA_SAVE_PLAYERS_TEAM                  ADDR(        0x187b02,          0x1004f2c7c)
#define RVA_PARTICIPANTS_TEAM_WRAP             ADDR(        0x304828,          0x1005890dc)
#define RVA_COUNT_SLOTS_WITH_CONTROLLER        ADDR(        0x302c10,          MAC_INLINED)
#define RVA_PARTICIPANTS_ADD_SLOT_BOUND        ADDR(        0x304271,          MAC_NOT_LOOKED_UP)
#define RVA_TEAMLESS_OWN_TEAM                  ADDR(        0x32a750,          0x1005114dc)
#define RVA_PLAYERCOLOR_STORE                  ADDR(        0x1bba08,          0x10052f45c)
#define RVA_FILE_READ_FIXED                    ADDR(        0x058580,          0x100336e78)
#define RVA_FILE_WRITE_FIXED                   ADDR_DIFFERS(0x058650,          0x1004f3084)
#define RVA_NET_READ_FIXED8                    ADDR(        0x068c90,          MAC_INLINED)
#define RVA_PLAYER_COLOUR_RPC_TEAMMATE         ADDR(        0x3020ce,          0x100586fdc)

#define RVA_START_LIKES_FILTER                 ADDR(        0x0a59e0,          0x100370d90)
#define RVA_START_PLACEMENT_SCORE              ADDR(        0x0a6660,          0x100370ed8)
#define RVA_FIND_START_SQUARE                  ADDR(        0x0a7420,          0x100370384)
#define RVA_PLAYER_INCOME_BY_TYPE              ADDR(        0x31e6b0,          0x100567bf4)
#define RVA_IRON_BONUS_PERCENT                 ADDR(        0x32e5e0,          MAC_INLINED)
#define RVA_GOLD_BONUS_PERCENT                 ADDR(        0x32ec20,          MAC_INLINED)
#define RVA_PLAYER_RITUAL_FLAGS                ADDR(        0x2c13e0,          MAC_INLINED)
#define RVA_ADD_PLAYER_BLESSING                ADDR(        0x2d3320,          0x1003542c0)
#define RVA_MAP_FIXED_START                    ADDR(        0xf07af86,         MAC_NOT_FOUND)

// ---- Mods ---------------------------------------------------------------------------------------
#define RVA_MOD_NAMES                          ADDR(        0xeed0260,         0x11307d818)
#define RVA_MOD_FOLDERS                        ADDR(        0xeed0360,         0x11307d918)
#define MOD_RECORD_SIZE                        0xce0
#define MOD_TABLE_MAX                          500
#define MOD_RECORD_ENABLED                     0xcd0
#define RVA_MOD_LAST_INDEX                     ADDR(        0x1a6b904,         0x105a0aed4)
#define RVA_EVENT_VARS                         ADDR(        0xf0627e0,         0x10ce02f0c)
#define RVA_LOAD_EVENT_VARS                    ADDR(        0x188860,          MAC_INLINED)
#define RVA_SAVE_EVENT_VARS                    ADDR(        0x1889f0,          MAC_INLINED)
#define EVENT_VAR_COUNT                        10000
#define RVA_EVENT_STATE                        ADDR(        0x582edf0,         0x10c127a18)
#define RVA_EVENT_FILE                         ADDR(        0x582ee20,         0x10c398aa8)
#define RVA_RUN_ENABLED_MODS                   ADDR(        0x1cb2c0,          0x10053a030)
#define RVA_RUN_ONE_MOD                        ADDR(        0x1badd0,          0x100529d64)
#define RVA_MOD_NAME_CHECK                     ADDR(        WIN_INLINED,       MAC_INLINED)
#define RVA_BAD_MOD_NAME_ERROR                 ADDR(        0x1bafbc,          0x100529f60)
#define RVA_FIND_ALL_MODS_RUN_CALL             ADDR(        0x1c9ad7,          0x10033325c)
#define RVA_EVENT_RUNNER                       ADDR(        0x1c5210,          0x10052a4dc)
#define RVA_RUN_ONE_MOD_EVENT_CALL             ADDR_DIFFERS(0x1bb188,          0x10052a358)
#define RVA_EVENT_FILE_EVENT_CALL              ADDR(        0x1cb1b6,          0x100539de8)
#define RVA_RANDOM_SQUARE_OF_TERRAIN           ADDR(        0x1b1740,          0x1004a8604)
#define RVA_RANDOM_RITUAL_SQUARE               ADDR(        0x2acf30,          0x1004b3c48)
#define RVA_EVENT_RANDLOC_CALL                 ADDR(        0x1c6d15,          0x10052c31c)
#define RVA_MOD_COMMAND                        ADDR(        0x1bb4b0,          0x10052ec70)
#define RVA_MOD_ERROR                          ADDR(        0x1c9400,          0x100529b84)
#define RVA_MOD_ERROR_MODE                     ADDR_DIFFERS(0x1a9236c,         0x10c063cf0)
#define RVA_FIND_MOD                           ADDR(        0x1baa80,          0x100539e6c)
#define RVA_WRITE_CONFIG                       ADDR(        0x193800,          0x10032a310)
#define RVA_PREFS_WRITE_CONFIG                 ADDR(        0x1987c9,          0x10032d96c)
#define RVA_MODS_SCREEN_WRITE_CONFIG           ADDR(        0x1ca304,          0x10032ef30)
#define RVA_MOD_ERROR_WRITE_CONFIG             ADDR(        0x1c9550,          0x100529d14)
#define RVA_READ_CONFIG                        ADDR(        0x192bc0,          0x10032a928)
#define RVA_SDL_MAIN_READ_CONFIG_SETTINGS      ADDR(        0x18fb25,          0x10032f8d8)
#define RVA_SDL_MAIN_READ_CONFIG_MODS          ADDR(        0x190b1b,          0x1003328c8)
#define RVA_READ_CONFIG_ENABLE_MOD             ADDR(        0x19319e,          0x10032ae00)
#define RVA_SDL_MAIN_FIND_ALL_MODS_CALL        ADDR(        0x190b11,          MAC_INLINED)
#define RVA_FIND_ALL_MODS                      ADDR(        0x1c9640,          MAC_INLINED)
#define RVA_ENABLED_MODS_SCREEN                ADDR(        0x1cb480,          0x10053b5dc)
#define RVA_ENABLE_SAVE_MODS                   ADDR(        0x1883c0,          MAC_NOT_FOUND)
#define RVA_OLD_WORKSHOP                       ADDR(        0x3362e0,          MAC_INLINED)
#define RVA_OLD_WORKSHOP_READ                  ADDR(        0x33664e,          0x100331fd0)
#define RVA_WRITE_STRING                       ADDR(        0x057c90,          0x100337044)
#define RVA_READ_STRING                        ADDR(        0x057b10,          MAC_INLINED)
#define RVA_FONT_FILE                          ADDR(        0x03bef0,          MAC_INLINED)
#define RVA_FONT_FILE_CHECK                    ADDR_DIFFERS(0x1bbc98,          0x100538a30)
#define RVA_FONT_FILE_TTF_CHECK                ADDR(        0x1bbcd4,          0x100538ab8)
#define RVA_HAS_EXTENSION                      ADDR(        0x059cf0,          MAC_INLINED)
#define RVA_ADD_TRANSLATION                    ADDR(        0x03d000,          0x10033b950)
#define RVA_ADD_FX_NAME                        ADDR(        0x1c80a0,          0x100538d0c)
#define RVA_PARSER_ABILITY_WORD                RVA_ADD_FX_NAME
#define RVA_MOD_IMAGE_CHECK                    ADDR(        0x1c8cf0,          0x100538734)
#define RVA_IMAGE_SIZE_READ                    ADDR(        0x027320,          0x100339c8c)
#define RVA_FILE_CASE_CHECK                    ADDR(        0x05a750,          0x100335994)
#define RVA_DIR_CASE_CHECK                     ADDR(        0x1c8dd0,          0x100529984)
#define RVA_DIR_NAME_CASE_CHECK                ADDR(        0x05a980,          0x100335750)
#define RVA_DIR_EXISTS                         ADDR(        0x05c110,          MAC_INLINED)
#define RVA_FILE_EXISTS                        ADDR(        0x05e470,          MAC_INLINED)
#define RVA_DIR_NEXT_ENTRY                     ADDR_DIFFERS(0x00f4b8,          0x1005aec38)
#define RVA_MODS_DIR                           ADDR(        0x5828160,         0x10c39946d)
#define RVA_WORKSHOP_ITEMS                     ADDR(        0xa0d95e0,         0x1136f61b0)
#define WORKSHOP_ITEM_SIZE                     0x9c6
#define RVA_MODS_CLOSEDIR_CALL                 ADDR(        WIN_NO_CALL,       0x100332678)
#define RVA_MODS_WORKSHOP_FIND_MOD_CALL        ADDR(        0x1c9823,          0x1003326cc)
#define RVA_LOBBY_MODS_OPENDIR_CALL            ADDR(        0x1c98a3,          0x100332740)
#define RVA_LOBBY_MODS_CLOSEDIR_CALL           ADDR(        WIN_NO_CALL,       0x1003327c8)
#define RVA_CLOSEDIR                           ADDR(        0x05c170,          0x1005ae2a8)
#define RVA_PARSER_WORD                        ADDR(        0x9e9df60,         0x10c127a1c)
#define RVA_PARSER_ARGS                        ADDR(        0x582f800,         0x10c127aa0)
#define RVA_SQUARE_EVENT_A                     ADDR(        0x9e76e60,         0x10c063d48)
#define SQUARE_EVENT_A_SIZE                    160000
#define RVA_SQUARE_EVENT_B                     ADDR(        0x9e9e7a0,         0x10c08ae48)
#define RVA_EVENT_PLAYER                       ADDR(        0x1a93b8c,         0x101cb6e14)
#define RVA_EVENT_TARGET_X                     ADDR(        0x1a95284,         0x101cb6e20)
#define RVA_EVENT_TARGET_Y                     ADDR(        0x1a923d4,         0x101cb6e24)
#define RVA_EVENT_SQUARE_X                     ADDR(        0x1a928d4,         0x101cb6e18)
#define RVA_EVENT_SQUARE_Y                     ADDR(        0x1a92924,         0x101cb6e1c)
#define RVA_EVENT_PLAYER_ARG                   ADDR(        0x1c7ff0,          0x1005398a0)
#define RVA_EVENT_INT_ARG                      ADDR(        0x1b6c30,          0x100512a0c)
#define RVA_EVENT_TEXT_ARG                     ADDR(        0x1b6b00,          0x100512a90)
#define RVA_FIND_UNIT_AT                       ADDR(        0x105340,          0x1004809a8)
#define RVA_FIND_ITEM_HOLDER_AT                ADDR(        0x17de70,          0x1004e997c)
#define RVA_FIND_UNIT_AT_END                   ADDR(        0x1054db,          0x100480e9c)
#define RVA_FIND_ITEM_HOLDER_AT_END            ADDR(        0x17e033,          0x1004ea188)
#define RVA_ARMY_AT_FOR_PLAYER                 ADDR(        0x103e50,          MAC_INLINED)
#define RVA_SAME_TEAM                          ADDR(        0x32b130,          MAC_INLINED)
#define RVA_EVENT_HASUNIT_CALL                 ADDR(        0x1c5eda,          0x10052db50)
#define RVA_EVENT_HASUNITHERE_CALL             ADDR(        0x1c5fd0,          0x10052dcdc)
#define RVA_EVENT_HASITEM_CALL                 ADDR(        0x1c63ae,          0x10052e29c)
#define RVA_EVENT_HASITEMHERE_CALL             ADDR(        0x1c6470,          0x10052e3dc)
#define RVA_EVENT_HASAFFUNITHERE_CALL          ADDR(        0x1c60d8,          0x10052de54)
#define RVA_EVENT_MINUNITSHERE_CALL            ADDR(        0x1c620c,          0x10052e184)
#define RVA_EVENT_MINUNITS_CALL                ADDR(        0x1c62ac,          0x10052e0fc)
#define RVA_COUNT_UNITS_AT                     ADDR(        0x105e40,          0x100479198)
#define RVA_STR_COUNT_UNITS_AT_TRACE           ADDR(        0x1b2b9b0,         0x100632d40)
#define RVA_FIND_AFFLICTED_UNIT_AT             ADDR(        0x1054f0,          0x10048046c)
#define RVA_COUNT_UNITS                        ADDR(        0x104650,          0x100481c54)
#define RVA_SQUARE_OWNER_AT                    ADDR(        0x334ff0,          MAC_INLINED)
#define RVA_ARMY_OWNER_AT                      ADDR(        0x129520,          MAC_INLINED)
#define RVA_SQUARE_ARMY_AT                     ADDR(        0x129050,          MAC_INLINED)
#define RVA_SQUARE_ARMY                        ADDR(        0xfb3b760,         0x1120c3fc0)
#define RVA_MOD_LINE                           ADDR(        0x1a95724,         0x101cce558)
#define RVA_EVENT_FILE_RUNNER                  ADDR(        0x1cafb0,          0x100539b04)
#define RVA_WORKSHOP_LAST                      ADDR(        0x1b18704,         0x105a0aee0)
#define RVA_EVENT_KILL_SQUARE                  ADDR(        0x1187e0,          0x100481bb0)
#define RVA_PROMOTE_UNITS_AT                   ADDR(        0x10edd0,          0x100471574)
#define RVA_PROMOTE_UNIT                       ADDR(        0x10efc0,          0x10046d534)
#define RVA_DAMAGE_UNIT                        ADDR(        0x0bd330,          0x1003b8dac)
#define RVA_REMOVE_UNIT                        ADDR(        0x0f8400,          0x1003b8a6c)
#define RVA_GIVE_ITEM_BY_NAME                  ADDR(        0x1835e0,          0x1004e5710)
#define RVA_TAKE_ITEM_BY_NAME                  ADDR(        0x17ff70,          0x1004e582c)
#define RVA_GIVE_RANDOM_ITEM                   ADDR(        0x183620,          0x1004e5dd0)
#define RVA_RANDOM_ITEM_PICK                   ADDR(        0x17fa90,          0x1004e59d8)
#define RVA_EVENT_FILE_COPY                    ADDR(        WIN_INLINED,       0x100529ebc)
#define RVA_EVENT_FILE_CLEAR                   ADDR(        0x1cb274,          0x100539e14)
#define RVA_EVENT_STATE_START                  ADDR(        0x1c52af,          0x10052a674)
#define RVA_EVENT_STATE_SQUARE                 ADDR(        0x1c5360,          0x10052a6f4)
#define RVA_EVENT_STATE_END                    ADDR(        0x1c53a7,          0x10052a718)
#define RVA_EVENT_STATE_FAIL                   ADDR(        0x1c7e1e,          0x10052bd98)
#define RVA_EVENT_STATE_CLEAR                  ADDR(        0x1bb1c0,          0x10052a38c)
#define RVA_RUN_MODS_SELF_CALL                 ADDR(        0x1cb3fb,          0x10053a358)
#define RVA_PLAY_GAME_START_RUN_MODS_CALL      ADDR(        0x091a8c,          0x100389b6c)
#define RVA_PLAY_GAME_END_RUN_MODS_CALL        ADDR(        0x093b60,          0x10039143c)
#define RVA_GAME_END_FN_RUN_MODS_CALL          ADDR(        0x093e81,          MAC_NOT_FOUND)
#define RVA_TURN_START_RUN_MODS_CALL           ADDR(        0x167692,          0x10039271c)
#define RVA_LOAD_GAME_RUN_MODS_CALL            ADDR(        0x185a56,          0x1004eb7a8)
#define RVA_STARTUP_RUN_MODS_CALL              ADDR(        0x190ba9,          0x1003328e4)
#define RVA_MODS_SCREEN_RESET_CALL             ADDR(        0x1ca2f7,          0x10032ef24)
#define RVA_MODS_SCREEN_RUN_MODS_CALL          ADDR(        0x1ca2ff,          0x10032ef2c)
#define RVA_MODS_OPENDIR_CALL                  ADDR(        0x1c96c2,          0x1003325e8)
#define RVA_MOD_NAME_COMPARE                   ADDR(        0x05c1c0,          MAC_INLINED)
#define RVA_MOD_TABLE_SORT                     ADDR(        0x1c99d0,          0x1003327cc)
#define RVA_LINE_NUMBER_ARG                    ADDR(        0x1b6c30,          0x100512a0c)
#define RVA_PARSER_MODVERSION                  ADDR(        0x1bb74b,          0x10052ee00)
#define RVA_PARSER_MODPRIO                     ADDR(        0x1bb7a7,          0x10052f1d4)
#define RVA_PARSER_CLEAR_CURRENT               ADDR(        0x1c51c0,          MAC_INLINED)
#define RVA_ADD_MOD_FILE                       ADDR_DIFFERS(0x1c8770,          0x100538814)
#define RVA_MOD_FILE_LIST                      ADDR(        0xe9ee260,         0x11320fda0)
#define MOD_FILE_ENTRY_SIZE                    0x200
#define RVA_MOD_FILE_LAST_INDEX                ADDR(        0x1a94fdc,         0x105a0aed8)
#define RVA_MOD_FILE_BYTES                     ADDR(        0x582edf4,         0x11320fd98)
#define RVA_MOD_CHECKSUMS                      ADDR(        0xeed0f34,         0x11307e4ec)
#define RVA_MOD_CRC                            ADDR(        0x1ca4f0,          MAC_INLINED)
#define RVA_LOAD_MOD_LIST                      ADDR(        0x1883c0,          MAC_NOT_FOUND)
#define RVA_SAVE_MOD_LIST                      ADDR(        0x1886d0,          MAC_NOT_FOUND)
#define RVA_SAVE_MOD_LIST_NAME_WRITE           ADDR(        0x188746,          MAC_NOT_FOUND)
#define RVA_SAVE_MOD_LIST_CRC_WRITE            ADDR(        0x188788,          MAC_NOT_FOUND)
#define RVA_MOD_CRC_FIRST_TURN                 ADDR(        0x1a3b2ac,         0x100872c68)
#define RVA_MOD_CRC_WARNED                     ADDR_DIFFERS(0x5822fb4,         0x1085ba7b0)
#define RVA_LOAD_GAME_MOD_CRC_CALL             ADDR(        0x18862e,          MAC_NO_CALL)
#define RVA_SAVE_GAME_MOD_CRC_CALL             ADDR(        0x18e56c,          MAC_NO_CALL)
#define RVA_SAVE_MOD_LIST_MOD_CRC_CALL         ADDR(        0x188753,          MAC_NOT_FOUND)
#define RVA_LOAD_MOD_LIST_MOD_CRC_CALL         ADDR(        0x18862e,          MAC_NOT_FOUND)
#define RVA_LOAD_GAME_FOPEN_CALL               ADDR(        0x18588f,          0x1004eb3bc)
#define RVA_LOAD_GAME_MOD_NAME_READ            ADDR(        WIN_INLINED,       MAC_INLINED)
#define RVA_LOAD_GAME_MOD_NAME_READ_NEXT       ADDR(        WIN_INLINED,       MAC_INLINED)
#define RVA_LOAD_GAME_MOD_CRC_READ             ADDR_DIFFERS(0x188556,          0x1004eb624)
#define LOAD_GAME_MOD_NAME_AFTER_CRC           4
#define LOAD_MOD_LIST_NAME_AFTER_CRC           0x10
#define RVA_WARN_FORMAT                        ADDR(        0x042f40,          0x100348090)
#define RVA_LOAD_GAME_MOD_DIFFERS_CALL         ADDR(        0x188645,          0x1004eb750)
#define RVA_LOAD_GAME_MOD_WARNED_SET           ADDR_DIFFERS(0x18864a,          0x1004eb758)
#define RVA_LOAD_GAME_RESET_MODS_CALL          ADDR(        0x185a27,          0x1004eb770)
#define RVA_LOAD_GAME_SKIP_RESET_TEST          ADDR(        0x185a25,          0x1004eb76c)
#define SAVE_VERSION_MOD_CHECKSUMS             0xc3
#define RVA_LOAD_GAME_MOD_NOT_FOUND            ADDR(        0x18869c,          0x1004ee50c)
#define RVA_LOAD_GAME_MOD_FLAGS_CLEAR          ADDR(        0x188410,          0x1004eb51c)
#define RVA_LOAD_GAME_MOD_ENABLE               ADDR(        0x1885ef,          0x1004eb680)
#define RVA_SAVE_GAME_MOD_NAME_WRITE           ADDR(        0x18e55e,          0x1004f20d8)
#define RVA_SAVE_GAME_MOD_CRC_WRITE            ADDR(        0x18e59c,          0x1004f20a0)
#define RVA_SAVE_GAME_MOD_LIST_END             ADDR(        WIN_INLINED,       0x1004f2168)

#define RVA_MOD_DESCRIPTION_WINDOW             ADDR(        0x1ba020,          0x10053a620)
#define RVA_WORKSHOP_UPLOAD_MOD                ADDR(        0x1ba550,          MAC_INLINED)
#define RVA_STEAM_SAVE_ITEM                    ADDR(        0x335cb0,          MAC_INLINED)
#define RVA_MOD_SAMPLE_CHECK                   ADDR(        0x1c8900,          0x100538b08)
#define RVA_STRING_TABLE_SET                   ADDR(        0x1c9060,          0x100529748)
#define RVA_CARET_TO_NEWLINE                   ADDR(        0x1c9600,          MAC_INLINED)
#define RVA_TERRAIN_PRESENT_RESCAN             ADDR(        0x1c7f10,          MAC_INLINED)
#define RVA_SQUAREEVENT_START                  ADDR(        0x1c8050,          MAC_NOT_FOUND)
#define RVA_SEND_ONE_MOD                       ADDR(        0x1e4bb0,          MAC_INLINED)
#define RVA_INIT_TERRAIN_RECORD                ADDR(        0x32bf60,          MAC_INLINED)
#define RVA_RITUAL_DESCRIPTION                 ADDR(        0x0b7070,          0x1004eadf8)
#define RVA_RECOUNT_ITEMS                      ADDR(        0x1807d0,          0x1004e4310)
#define RVA_SOUND_MIXER                        ADDR(        0x060180,          0x1005aba6c)
#define RVA_DRAW_TERRAIN_SMOKE                 ADDR(        0x19e970,          MAC_NOT_FOUND)
#define RVA_PARSER_RITUAL_SCHOOL               ADDR(        0x582ee10,         MAC_NOT_FOUND)
#define RVA_PARSER_RITUAL_COSTS                ADDR(        0x582ede8,         MAC_NOT_FOUND)
#define RVA_ITEM_SPRITE_NEXT                   ADDR(        0x1a8a6dc,         MAC_NOT_FOUND)
#define RVA_MONSTER_SPRITE_NEXT                ADDR(        0x1a94f44,         MAC_NOT_FOUND)
#define RVA_SAMPLE_KINDS                       ADDR(        0x1b30fc6,         0x105a0b8b6)
#define RVA_STEAM_CONNECTED                    ADDR(        0x583602c,         MAC_NOT_FOUND)
#define RVA_RITUAL_DESCR_FALLBACK              ADDR(        0x43d7d4,          MAC_NOT_FOUND)
#define RVA_TERRAIN_SMOKE_POINTS               ADDR(        0xd2428a0,         MAC_NOT_FOUND)

#define RVA_SQUARE_NUMBERS_WINDOW              ADDR(        0x331030,          MAC_INLINED)
#define RVA_WEAPON_INFO_WINDOW                 ADDR(        0x1cc5f0,          MAC_INLINED)
#define RVA_WEAPON_CHAIN_TEXT                  ADDR(        0x1d1160,          0x100527420)
#define RVA_MAP_UNIT_BOX_DRAW                  ADDR(        0x1a1be0,          0x100499bf4)
#define RVA_MAP_DRAW_UNITS                     ADDR(        0x19e440,          MAC_INLINED)
#define RVA_MAP_DRAW_SQUARE_ARMY               ADDR(        0x1a10f0,          MAC_INLINED)
#define RVA_SPRITE_FIGURE_SIZE                 ADDR(        0x0659d0,          0x10034ddcc)
#define RVA_SAMPLE_TABLE                       ADDR(        0x1b30600,         0x105a0aef0)
#define SAMPLE_SIZE                            0x9cc
#define RVA_SAMPLE_HIGHEST                     ADDR(        0x43bccc,          0x105a0aee8)
#define RVA_SAMPLE_LAST_TUNES                  ADDR(        0x1b2bcd0,         0x100872b80)
#define RVA_PLAY_SAMPLE                        ADDR(        0x128000,          0x1004762d0)
#define RVA_SOUND_START                        ADDR(        0x05f6e0,          0x1005ab5c4)
#define RVA_SET_SAMPLE_FILE                    ADDR(        0x05f060,          0x1005ab844)
#define RVA_SAMPLE_FETCH                       ADDR(        0x05f8f0,          0x1005aaf1c)
#define RVA_SDL_AUDIO_CALLBACK                 ADDR(        0x05ede0,          MAC_INLINED)

// ---- Game data tables ---------------------------------------------------------------------------
#define RVA_MONSTER_TABLE                      ADDR(        0x1c65280,         0x104b515f8)
#define RVA_MONSTER_COUNT                      ADDR(        0x1aa611c,         0x10487ff28)
#define RVA_MONSTER_CURRENT                    ADDR(        0x1a928fc,         0x101cb6e30)
#define RVA_MONSTER_SPRITES                    ADDR(        0x1c652b0,         0x104b51628)
#define MONSTER_SIZE                           0x9d8
#define MONSTER_NAME_SIZE                      0x30
#define MONSTER_TABLE_MAX                      6000
#define MONSTER_TABLE_BYTES                    0xe6b680
#define RVA_MONSTER_BACKUP_SIZE                ADDR(        0x1909f3,          0x100332484)
#define RVA_MONSTER_BACKUP_COPY_SIZE           ADDR(        0x1909fd,          0x1003324a0)
#define RVA_MONSTER_RESTORE_SIZE               ADDR(        0x1ca672,          0x100528dac)
#define RVA_NEWMONSTER_CAP                     ADDR(        0x1bbe87,          0x10052f6fc)
#define RVA_FIND_MONSTER_TABLE_REF             ADDR(        0x1d9e4b,          0x100491378)
#define RVA_ARMY_LIST_COUNTS                   ADDR(        0x11f430e0,        0x106cdfa04)
#define RVA_ARMY_LIST_UNITS                    ADDR(        0x11f991e0,        0x106ce57c4)
#define RVA_ARMY_LIST_LEAST_A                  ADDR(        0x11f97a60,        0x106ceb584)
#define RVA_ARMY_LIST_LEAST_B                  ADDR(        0x11f41960,        0x106ceccf4)
#define RVA_MONSTER_SPRITE_LABELS              ADDR(        0x582f830,         0x10b8494c4)
#define RVA_MONSTER_SPRITE_LABELS_FILL_BOUND   ADDR(        0x1d9b53,          0x100490d98)
#define RVA_BATTLE_SUMMONS                     ADDR(        0x12335900,        0x107ee6496)
#define ARMY_LIST_COUNT_SIZE                   0x4
#define ARMY_LIST_LEAST_SIZE                   0x1
#define BATTLE_SUMMONS_SIZE                    0x2
#define MONSTER_NAME_SIZE                      0x30
#define RVA_MONSTER_FLAGS                      ADDR(        0x1c65450,         0x104b517c8)
#define MONSTER_FLAG_NOT_ARMY                  0x400000000
#define MONSTER_MOVE_FLAGS                     0x1c8
#define ITEM_GROUP_MASK                        0x188
#define ITEM_MOVE_FLAGS                        0x58
#define ITEM_ABILITIES                         0x60
#define ITEM_ABILITY_PAIRS                     20
#define ITEM_WEAPON                            0x38
#define ITEM_SPRITE                            0x30
#define ITEM_AFFLICTIONS                       0x48
#define ITEM_BLESSINGS                         0x50
#define ITEM_COMBAT_WORDS                      0x100
#define ITEM_COMBAT_WORDS_SIZE                 0x88
#define ITEM_ABILITY_USEABLE                   0x16
#define ITEM_ABILITY_CONSUMABLE                0xaf
#define ITEM_ABILITY_EXTRA_LIFE                0x197
#define ITEM_ABILITY_EYE                       0x1be
#define ITEM_ABILITY_HEART                     0x1bf
#define ITEM_ABILITY_REVEAL                    0x1a
#define ITEM_ABILITY_TELEPORT                  0x19f
#define ITEM_ABILITY_PLANE_TRAVEL              0x1e3
#define ITEM_ABILITY_BEANSTALK                 0x1e5
#define ITEM_ABILITY_USE_ONLY_IN               0x1e6
#define ITEM_ABILITY_PIT                       0x1e7
#define ITEM_ABILITY_BREAK_SEAL                0x4d0
#define ITEM_ABILITY_CURE_AFFLICTION           0x1f3
#define ITEM_ABILITY_NEW_SPELL1                0x17
#define ITEM_ABILITY_NEW_SPELL2                0x18
#define ITEM_ABILITY_NEW_SPELL3                0x19
#define ITEM_FLAG_CURSED                       0x400000000000
#define RVA_PARSER_MOVE_WORD                   ADDR(        0x1c82e0,          0x100539480)
#define RVA_PARSER_FLAG_WORD                   ADDR(        0x1c8670,          0x100538c20)
#define RVA_PARSER_TERRAIN_FLAG_WORD           ADDR(        0x1c8540,          0x100539588)
#define RVA_PARSER_WEAPON_FLAG_WORD            ADDR(        0x1c8610,          0x100539420)
#define RVA_PARSER_RITUAL_FLAG_WORD            ADDR(        0x1c85b0,          0x100539528)
#define RVA_PARSER_TERRAIN_ABILITY_WORD        ADDR(        0x1c83e0,          0x100539604)
#define RVA_SOULLESS_FORM                      ADDR(        0x1d9c60,          0x100491090)
#define RVA_UNIT_TRAIT_LIST                    ADDR(        0x1d1300,          0x10051cc74)
#define MONSTER_ABILITY_HATES                  0x9b
#define MONSTER_ABILITY_HATED                  0x9c
#define HATES_KILL_CHANCE                      15
#define RVA_RUN_HATREDS                        ADDR(        0x142c90,          MAC_INLINED)
#define RVA_UNIT_WITH_ABILITY_AT               ADDR(        0x112d10,          0x100468a7c)
#define MONSTER_ABILITY_LIMITED_GEM_BONUS      0xbf
#define MONSTER_ABILITY_RANGE_BONUS            0xc7
#define MONSTER_ABILITY_WALL_COVER             0x198
#define MONSTER_ABILITY_WALL_RANGE_BONUS       0x1a6
#define MONSTER_ABILITY_TRIPLE_ENTITY          0x1c6
#define MONSTER_ABILITY_MIRROR_CLOAKING        0x43b
#define MONSTER_ABILITY_MAGIC_TATTOO           0x4cb
#define MONSTER_ABILITY_REINCARNATION          0x4f4
#define MONSTER_ABILITY_BRINGER_OF_PLENTY      0x4fa
#define MONSTER_FLAG_FIRE_100                  0
#define MONSTER_FLAG_COLD_100                  1
#define MONSTER_FLAG_POISON_100                2
#define MONSTER_FLAG_SHOCK_100                 15
#define MONSTER_FLAG_CAUSES_FEAR               29
#define RVA_HATED_KIND_NAMES                   ADDR(        0x3faad0,          0x100838cd0)
#define RVA_FIND_MONSTER                       ADDR(        0x1d9e70,          0x1004912c0)
#define MONSTER_ABILITIES                      0x1d8
#define MONSTER_ABILITY_PAIRS                  256
#define MONSTER_ABILITY_POWER_BASE             0x1f4
#define MONSTER_ABILITY_STARTITEM              0x4a
#define PARSER_STARTITEM_MAX                   4
#define RVA_PARSER_STARTITEM_COUNT             ADDR(        0x582ee14,         MAC_NOT_LOOKED_UP)
#define MONSTER_ARMOR                          0x34
#define MONSTER_HP                             0x36
#define MONSTER_STR                            0x38
#define MONSTER_MORALE                         0x3a
#define MONSTER_RANK                           0x3c
#define MONSTER_MR                             0x1c0
#define MONSTER_ATTACKS                        0x40
#define MONSTER_ATTACK_SIZE                    0x18
#define MONSTER_ATTACK_MAX                     16
#define MONSTER_ATTACK_WEAPON                  0x8
#define MONSTER_ATTACK_VALUE                   0x10
#define RVA_MONSTER_ABILITY                    ADDR(        0x129300,          MAC_INLINED)
#define RVA_NAME_COMPARE                       ADDR_DIFFERS(0x0579e0,          0x1005aed70)
#define RVA_UNIT_SPAWN_OFFSET_READ             ADDR(        0x144ee8,          0x10039c680)
#define RVA_UNIT_GROW_OFFSET_READ              ADDR(        0x1452b1,          0x10039d0c0)
#define RVA_WEAPON_TABLE                       ADDR(        0x5700790,         0x104904cb0)
#define WEAPON_SIZE                            0xa0
#define WEAPON_TRGRANK                         0x40
#define WEAPON_RANGE                           0x44
#define WEAPON_INIT                            0x48
#define WEAPON_DMGTYPE                         0x4c
#define WEAPON_DMG                             0x50
#define WEAPON_AOE                             0x58
#define WEAPON_FLAGS                           0x60
#define WEAPON_NEXT                            0x90
#define WEAPON_NEXT_DMG                        0x98
#define WEAPON_SOUND_PAIRS                     0x7c
#define WEAPON_SOUND_PAIRS_MAX                 2
#define RVA_WEAPON_SET_SOUND_PAIR              ADDR(        0x0b7ac0,          MAC_NOT_FOUND)
#define RVA_MONTH_END_FIGHTS_CALL              ADDR(        0x166655,          0x1003b7138)
#define RVA_WEAPON_FREE_SLOT                   ADDR(        0x0b8570,          0x1003f808c)
#define RVA_WEAPON_CURRENT                     ADDR(        0x1a93f44,         0x101cb6e34)
#define WEAPON_RECORDS_GAME                    5000
#define RVA_WEAPON_FREE_SLOT_CAP               ADDR(        0x0b85b4,          0x1003f80d0)
#define RVA_WEAPON_OUT_OF_WEAPONS_CALL         ADDR(        0x0b85c5,          0x1003f80f0)
#define RVA_FIND_WEAPON_TABLE_REF              ADDR(        0x0b880b,          0x1003f7e10)
#define RVA_SELECTWEAPON_BOUND                 ADDR(        0x1bea15,          0x100532428)
#define RVA_NEXTWEP_BOUND                      ADDR(        0x1bec61,          0x100532574)
#define RVA_NEXTALWAYSWEP_BOUND                ADDR(        0x1bed21,          0x100532658)
#define RVA_ITEMWEP_BOUND                      ADDR(        0x1c0c61,          0x1005341f4)
#define RVA_WEAPON_NAME_LINE_BOUND             ADDR(        0x1cf98b,          0x100526e0c)
#define RVA_WEAPON_CLEAR_SIZE                  ADDR(        0x164a87,          0x1003aabf0)
#define RVA_WEAPON_BACKUP_SIZE                 ADDR(        0x190a65,          0x1003324fc)
#define RVA_WEAPON_BACKUP_COPY_SIZE            ADDR(        0x190a6f,          0x100332518)
#define RVA_ITEM_BACKUP_SIZE                   ADDR(        0x190aeb,          0x100332584)
#define RVA_ITEM_BACKUP_COPY_SIZE              ADDR(        0x190af5,          0x1003325a0)
#define RVA_ITEM_RESTORE_SIZE                  ADDR(        0x1ca726,          0x100528e6c)
#define RVA_WEAPON_RESTORE_SIZE                ADDR(        0x1ca6cc,          0x100528e0c)
#define RVA_WEAPON_HIT                         ADDR(        0x0dcaa0,          0x1003c1f7c)
#define RVA_WEAPON_HIT_MULTIPLY                ADDR_DIFFERS(0x0dd333,          0x1003c2cd0)
#define RVA_WEAPON_HIT_DAMAGE_CHECK            ADDR_DIFFERS(0x0dd338,          0x1003c2cd4)
#define RVA_WEAPON_HIT_DAMAGE_GO_ON            ADDR(        0x0dd341,          0x1003c2cd8)
#define RVA_WEAPON_HIT_NO_DAMAGE               ADDR(        0x0dd4f9,          MAC_IN_CODE)
#define RVA_WEAPON_HIT_MULTIPLY_END            ADDR(        0x0dd379,          0x1003c2d04)
#define RVA_WEAPON_HIT_PROLOGUE_END            ADDR(        0x0dcabc,          0x1003c1ff0)
#define RVA_WEAPON_HIT_DAMAGE_DIE_CALL         ADDR(        0x0dcd4f,          0x1003c2574)
#define RVA_BATTLE_LOG                         ADDR(        0x1005c0,          0x100581730)
#define RVA_WEAPON_TEXT                        ADDR(        0x1cc910,          0x1005275f4)
#define RVA_WEAPON_TEXT_CALL                   ADDR(        0x1d11f2,          0x1005274a4)
#define RVA_WEAPON_TEXT_NEXT_CALL              ADDR(        0x1d12d3,          0x100527500)
#define RVA_DO_WEAPON_ATTACK                   ADDR(        0x0d7730,          0x1003bcb70)
#define RVA_DO_WEAPON_ATTACK_HIT_CALL          ADDR(        0x0d939b,          0x1003bda40)
#define RVA_DO_WEAPON_ATTACK_NEXT_CALL         ADDR(        0x0d9451,          0x1003bdd7c)
#define RVA_DO_WEAPON_ATTACK_NEXT_EMPTY_CALL   ADDR(        0x0d956b,          0x1003be090)
#define RVA_PLACE_WEAPON_LOOK                  ADDR(        0x0d99e0,          MAC_INLINED)
#define RVA_PLACE_WEAPON_LOOK_CALL             ADDR_DIFFERS(0x0d91a9,          0x1003bd70c)
#define RVA_DRAW_VISUAL_FX                     ADDR(        0x1029e0,          0x1004bfeb0)
#define RVA_DRAW_VISUAL_FX_BOUND               ADDR_DIFFERS(0x102a4a,          0x1004bfef4)
#define RVA_FATAL_IF                           ADDR(        0x05c6b0,          0x100334040)
#define RVA_VISUAL_FX_TABLE                    ADDR(        0x1b1dfc0,         0x100834780)
#define VISUAL_FX_SIZE                         0x30
#define VISUAL_FX_ROWS                         300
#define RVA_PARTICLES                          ADDR(        0x124477a0,        0x107efbc5c)
#define PARTICLE_SIZE                          0x30
#define PARTICLES_MAX                          140000
#define RVA_PARTICLES_LAST                     ADDR(        0x43a654,          0x100862780)
#define RVA_PARTICLE_FREE_HINT                 ADDR(        0x57f1084,         0x1067c5674)
#define PARTICLE_FLAGS                         0x00
#define PARTICLE_SIZE_PX                       0x18
#define RVA_PARTICLE_ADD                       ADDR(        0x051d00,          0x100350dd4)
#define RVA_PARTICLE_ADD_AT                    ADDR(        0x051e30,          MAC_INLINED)
#define RVA_PARTICLES_UPDATE                   ADDR(        0x051720,          MAC_INLINED)
#define RVA_PARTICLES_DRAW                     ADDR(        0x050be0,          MAC_INLINED)
#define RVA_PARTICLE_TEXTURES_LOAD             ADDR(        0x052030,          0x10035112c)
#define RVA_VISUAL_FX_LIGHTNING                ADDR(        0x1026f0,          0x1004bfc94)
#define RVA_VISUAL_FX_BOLT                     ADDR(        0x102600,          MAC_INLINED)
#define RVA_VISUAL_FX_BOLT_FLAGS               ADDR(        0x5822ec4,         0x107efbc58)
#define RVA_VISUAL_FX_ORB_X                    ADDR(        0x43eaac,          0x100872c28)
#define RVA_VISUAL_FX_ORB_Y                    ADDR(        0x43ea14,          0x100872c2c)
#define RVA_VISUAL_FX_CHAIN_X                  ADDR(        0x43ea3c,          0x100872c30)
#define RVA_VISUAL_FX_CHAIN_Y                  ADDR(        0x43ea84,          0x100872c34)
#define RVA_BATTLE_CELL_PX                     ADDR(        0x43e284,          0x1049c81b4)
#define RVA_BATTLE_SCREEN_CARRY_US             ADDR(        0x5804e84,         0x10c96df44)
#define RVA_ADD_FLY_WEAPON                     ADDR(        0x101640,          0x100584ff8)
#define RVA_FLY_WEAPONS                        ADDR(        0x65612b0,         0x10c9b6088)
#define FLY_WEAPON_SIZE                        0x38
#define FLY_WEAPONS_MAX                        12000
#define RVA_FLY_WEAPONS_LAST                   ADDR(        0x43e7f4,          0x1049c81b0)
#define RVA_DOATTACK_FLY_CALL_A                ADDR_DIFFERS(0x0d480f          ,0x1003ec5ec)
#define RVA_DOATTACK_FLY_CALL_B                ADDR_DIFFERS(0x0d4834          ,0x1003ec668)
#define RVA_DOATTACK_FLY_CALL_C                ADDR_DIFFERS(0x0d48a0          ,0x1003ec6c0)
#define RVA_DOATTACK_FLY_CALL_D                ADDR_DIFFERS(0x0d4c43          ,0x1003ecbbc)
#define RVA_DOATTACK_FLY_CALL_E                ADDR_DIFFERS(0x0d4d0e          ,0x1003ecc38)
#define RVA_DOATTACK_FLY_CALL_F                ADDR_DIFFERS(0x0d4eb5          ,0x1003eccb0)
#define RVA_FLY_STEP_DRAW_CALL_A               ADDR(0x101b14          ,0x100585770)
#define RVA_FLY_STEP_DRAW_CALL_B               ADDR(0x101ba6          ,0x1005857c4)
#define RVA_FLY_STEP_DRAW_CALL_C               ADDR(0x101be4          ,0x1005857f0)
#define FLY_WEAPON_ATTACKER                    0x00
#define FLY_WEAPON_WEAPON                      0x04
#define FLY_WEAPON_FROM_X                      0x14
#define FLY_WEAPON_FROM_Y                      0x18
#define FLY_WEAPON_TO_X                        0x1c
#define FLY_WEAPON_TO_Y                        0x20
#define FLY_WEAPON_MODE                        0x24
#define FLY_WEAPON_PROGRESS                    0x2c
#define RVA_FLY_WEAPONS_STEP                   ADDR(        0x101960,          0x100585538)
#define RVA_FLY_WEAPON_DRAW_FX                 ADDR(        0x101250,          0x100585898)
#define RVA_FLY_WEAPON_DRAW_SPRITES            ADDR(        0x100ca0,          MAC_INLINED)
#define RVA_FLY_SPRITE_DRAW_CALL               ADDR_DIFFERS(0x100f40,          0x10048a568)
#define RVA_SPRITE_DRAW_ROTATED                ADDR(        0x066130,          0x10034f794)
#define SPRITE_KIND_BLAST                      3
#define WEAPON_FLYMODE                         0x68
#define WEAPON_FLYLOOK                         0x6c
#define WEAPON_FLYSOUND                        0x70
#define WEAPON_LOOK                            0x74
#define WEAPON_SOUND                           0x78
#define WEAPON_EXTRAS                          0x7c
#define RVA_WEAPON_EXTRA                       ADDR(        0x0b7a60,          MAC_INLINED)
#define RVA_WEAPON_HIT_AFFLICT                 ADDR(        0x0de26f,          0x1003c4670)
#define RVA_WEAPON_HIT_BENEFIT                 ADDR(        0x0de2ce,          0x1003c46bc)
#define RVA_AFFLICTION_IMMUNE                  ADDR(        0x0dfe70,          0x1003ca034)
#define RVA_ADD_BENEFIT                        ADDR(        0x0bbbb0,          MAC_INLINED)
#define RVA_BATTLE_TARGET_USEFUL_CALL          ADDR(        0x0d6459,          0x1003c0564)
#define RVA_SPELL_USEFUL_TARGET_CALL           ADDR_DIFFERS(0x2e5578,          0x1004bfa90)
#define RVA_ITEM_TABLE                         ADDR(        0x1be6730,         0x1048937f8)
#define ITEM_SIZE                              0x1d0
#define ITEM_NAME_SIZE                         0x30
#define ITEM_TABLE_MAX                         1000
#define RVA_ITEM_COUNT                         ADDR(        0x1a3b04c,         0x101cce550)
#define RVA_NEWITEM_CAP                        ADDR(        0x1c0651,          0x100533e04)
#define RVA_GROUND_ITEMS_LOAD_CHECK            ADDR(        0x18601f,          0x1004ebe8c)
#define RVA_GROUND_ITEMS_LOAD_CHECK_2          ADDR(        0x18a1b1,          MAC_NOT_FOUND)
#define RVA_FIND_ITEM_TABLE_REF                ADDR(        0x184d22,          0x1004e57d8)
#define RVA_ITEM_CURRENT                       ADDR(        0x1a923b4,         0x101cb6e2c)
#define RVA_ITEM_FLAGS                         ADDR(        0x1be6770,         0x104893838)
#define RVA_ITEM_STATE_FLAGS                   ADDR(        0x1be6778,         0x104893840)
#define RVA_CLASS_TABLE                        ADDR(        0x18a0710,         0x1049c8358)
#define RVA_CLASS_END_SCAN_STOP                ADDR(        0x0b02ea,          0x100529254)
#define RVA_CLASS_END_SCAN_STOP_2              ADDR(        0x190808,          0x100331088)
#define RVA_CLASS_END_SCAN_STOP_3              ADDR(        0x0b02ea,          0x1003913f8)
#define CLASS_SIZE                             0x1f74
#define CLASS_NAME_SIZE                        0x40
#define CLASS_TABLE_MAX                        200
#define RVA_CLASS_COUNT                        ADDR(        0x43d0d4,          0x1049c8354)
#define RVA_CLASS_LEADERS                      ADDR(        0x18a0750,         0x1049c8398)
#define RVA_CLASS_SIDE_A                       ADDR(        0x45cf10,          0x1008735d8)
#define RVA_CLASS_SIDE_B                       ADDR(        0x445810,          0x101cb6e50)
#define RVA_CLASS_CURRENT                      ADDR(        0x1a95904,         0x101cb6e44)
#define RVA_CLASS_LAST_RECRUIT                 ADDR(        0x1a93d34,         0x101cb6e48)
#define RVA_RECLIMITER_COUNT                   ADDR(        0x582f7e4,         0x10c127a8c)
#define CLASS_HEROES_SIZE                      0x1e0
#define RVA_WESTEAST_TEAM_ANGLES               ADDR(        0x62240e0,         0x1069b07a0)
#define RVA_CLASS_TABLE_COPY                   ADDR(        0x6224400,         0x1068274f4)
#define RVA_NEWCLASS_CAP                       ADDR(        0x1bd347,          0x100530aa8)
#define RVA_CLASS_BACKUP_SIZE                  ADDR(        0x190a19,          0x1003324ac)
#define RVA_CLASS_RECRUITS_BACKUP_SIZE         ADDR(        0x190a3f,          0x1003324d4)
#define RVA_CLASS_RESTORE_SIZE                 ADDR(        0x1ca690,          0x100528dcc)
#define RVA_CLASS_RECRUITS_RESTORE_SIZE        ADDR(        0x1ca6ae,          0x100528dec)
#define RVA_CLASS_COPY_SIZE_1                  ADDR(        0x093b33,          0x1003913d0)
#define RVA_CLASS_COPY_SIZE_2                  ADDR(        0x093e54,          MAC_NOT_FOUND)
#define RVA_CLASS_COPY_SIZE_3                  ADDR(        0x0b02a4,          MAC_NOT_FOUND)
#define RVA_CLASS_COPY_SIZE_4                  ADDR(        0x0b0317,          0x100529294)
#define RVA_CLASS_COPY_SIZE_5                  ADDR(        0x190859,          0x1003310c8)
#define RVA_FIND_CLASS                         ADDR(        0x0898d0,          0x100391c2c)
#define RVA_FIND_CLASS_TABLE_REF               ADDR(        0x0898e7,          0x100391c5c)
#define RVA_CLASS_PICKER                       ADDR(        0x3021e0,          0x100586054)
#define RVA_CLASS_PICKER_INFO_ID               ADDR(        0x3024fd,          0x100586358)
#define RVA_CLASS_PICKER_DECODE                ADDR(        0x3027fb,          0x1005864cc)
#define RVA_CLASS_PICKER_RETURN                ADDR(        0x30284d,          0x1005864f8)
#define RVA_CLASS_PICKER_INFO_CALL             ADDR(        0x302843,          0x10058610c)
#define RVA_CLASS_PICKER_LOOP                  ADDR(        0x302280,          0x100586114)
#define CLASS_HOME_TERRAIN                     0x70
#define CLASS_START_UNITS                      0x84
#define CLASS_START_UNITS_MAX                  16
#define CLASS_START_UNIT_SIZE                  0x40
#define CLASS_ABILITIES                        0x484
#define CLASS_ABILITY_PAIRS                    16
#define CLASS_ABILITY_STDTROOPS                0xde
#define CLASS_ABILITY_AI_CLASS                 0xcb
#define CLASS_ABILITY_STARTPLANE               0xeb
#define CLASS_ABILITY_ASSASSIN_CHANCE          0xe1
#define CLASS_ABILITY_RANDOM_SKIP              0xcc
#define CLASS_ABILITY_AI_LEVEL_MIN             0xd2
#define CLASS_ABILITY_AI_LEVEL_MAX             0xd3
#define CLASS_RECRUITS_SIZE                    0x19f00
#define CLASS_RECRUITS_MAX                     160
#define CLASS_RECRUIT_SIZE                     0x298
#define RECRUIT_MONSTER_NAME                   0x0
#define RECRUIT_CHANCE                         0x30
#define RECRUIT_COUNT                          0x34
#define RECRUIT_GOLD                           0x38
#define RECRUIT_GOLD_DIE                       0x3c
#define RECRUIT_IRON                           0x40
#define RECRUIT_FLAGS                          0x44
#define RECRUIT_EXTRA_COSTS                    0x48
#define RECRUIT_LIMITERS                       0x52
#define RECRUIT_LIMITERS_MAX                   12
#define RECRUIT_LIMITER_SIZE                   0x30
#define RECRUIT_TERRAIN                        0x294
#define CLASS_HERO_SIZE                        0x78
#define CLASS_HERO_NAME                        0x30
#define RVA_CLASS_ADD_RECRUIT                  ADDR(        0x0b0070,          0x1003611f0)
#define RVA_CLASS_ADD_START_UNIT               ADDR(        0x0aff60,          0x1003612ec)
#define RVA_CLASS_ABILITY                      ADDR(        0x0afef0,          0x10036155c)
#define RVA_CLASS_SET_ABILITY                  ADDR(        0x0afd40,          0x100361ad4)
#define RVA_CLASS_CLEAR_ABILITY                ADDR_DIFFERS(0x0afe00,          0x100361864)
#define RVA_CLASS_PRINT_ALL_RECRUITS           ADDR(        0x0b55f0,          0x1004c1d38)
#define RVA_CLASS_PRINT_RECRUITS               ADDR(        WIN_NOT_FOUND,     MAC_NOT_FOUND)
#define RVA_HERO_ARRIVAL                       ADDR(        0x089d60,          MAC_INLINED)
#define RVA_TERRAIN_TABLE                      ADDR(        0x56aead0,         0x1059bcc90)
#define TERRAIN_SIZE                           0xa0
#define TERRAIN_NAME_SIZE                      0x50
#define TERRAIN_TABLE_MAX                      2000
#define TERRAIN_MATCH_CODE_MIN                 -1500
#define TERRAIN_GOLD                           0x30
#define TERRAIN_IRON                           0x32
#define TERRAIN_TRADE                          0x34
#define TERRAIN_HERBS                          0x36
#define TERRAIN_FUNGUS                         0x38
#define TERRAIN_WEED                           0x3a
#define TERRAIN_SACRIFICES                     0x3c
#define TERRAIN_HANDS                          0x40
#define TERRAIN_LIFEFORCE                      0x44
#define TERRAIN_ABILITIES                      0x58
#define TERRAIN_ABILITY_PAIRS                  4
#define TERRAIN_FLAGS                          0x50
#define TERRAIN_FLAG_PORT                      0x400000000000000
#define TERRAIN_FLAG_SIGHT_SQUARE_ONLY         0x2000000
#define TERRAIN_FLAG_SIGHT_FARSIGHT            0x1000000
#define TERRAIN_FLAG_SEEN_NEAR                 0x10000000000000
#define TERRAIN_FLAG_SEEPAST                   0x4000000000000000
#define TERRAIN_APCOST                         0x46
#define RVA_TERRAIN_CURRENT                  ADDR(        0x1a95c84,         0x101cb6e3c)
#define RVA_TERRAIN_GROUP_NAMES                ADDR(        0xd4a65e0,         0x10c81c184)
#define RVA_TERRAIN_GROUP_MEMBERS              ADDR(        0xd4ae2e0,         0x10c823e84)
#define RVA_TERRAIN_GROUP_CURRENT              ADDR(        0x1a9238c,         0x101cb6e40)
#define RVA_BUILTIN_GROUP_NAMES                ADDR(        0x405c80,          0x100847ed8)
#define RVA_RITUAL_TABLE                       ADDR(        0x2ae2ff0,         0x101cce668)
#define RVA_RITUAL_CODE_FIRST                  ADDR(        0x2adcf0,          MAC_FILE_FORMAT)
#define RVA_RITUAL_CODE_LAST                   ADDR(        0x2cdd40,          MAC_FILE_FORMAT)
#define RITUAL_SIZE                            0x3ba8
#define RITUAL_NAME_SIZE                       0x30
#define RITUAL_RITPOW                          0x30
#define RITUAL_LEVEL                           0x34
#define RITUAL_COSTS                           0x38
#define RITUAL_COSTS_MAX                       4
#define RITUAL_COST_SIZE                       8
#define RITUAL_TERRAIN                         0x5c
#define RITUAL_FX_VALUE                        8
#define RITUAL_FX                              0x60
#define RITUAL_FX_MAX                          20
#define RITUAL_FX_SIZE                         0x10
#define RITUAL_FX_SUMMONING                    900
#define RITUAL_FLAGS                           0x1a0
#define RITUAL_STRINGS                         0x1a8
#define RITUAL_STRINGS_MAX                     58
#define RITUAL_STRING_SIZE                     0x100
#define RVA_RITUAL_COUNT                       ADDR(        0x1afec74,         0x101cce58c)
#define RVA_RITPOW_COUNT                       ADDR(        0x1afe3fc,         0x101cce594)
#define RVA_RITUAL_CURRENT                     ADDR(        0x1a923f4,         0x101cb6e38)
#define RVA_FIND_RITUAL                        ADDR(        0x2be0a0,          MAC_INLINED)
#define RVA_ALLOC_RITUAL                       ADDR(        0x2be170,          0x1005670b0)
#define RVA_ALLOC_RITUAL_TABLE_REF             ADDR_DIFFERS(0x2be198,          0x1005670ec)

#define RVA_INIT_RITUALS                       ADDR(        0x2be770,          MAC_INLINED)
#define RVA_INIT_RITUALS_BOUND                 ADDR_DIFFERS(0x190894,          0x100331114)
#define RVA_INIT_RITUALS_BOUND_RESET_MODS      ADDR_DIFFERS(0x2be78f,          0x1005292e0)
#define RVA_INIT_RITUALS_BOUND_3               ADDR_DIFFERS(0x2be78f,          0x100552efc)
#define RVA_INIT_RITUALS_BOUND_RESET_STATE     ADDR_DIFFERS(0x2be78f,          0x100585ae0)
#define RVA_INIT_RITUALS_NO_END_CALL           ADDR(        0x2be7ce,          0x100331124)
#define RVA_RITUAL_ADD_FX                      ADDR(        WIN_INLINED,       MAC_INLINED)
#define RVA_RITUAL_ADD_FX_BOUND                ADDR(        0x1c8181,          0x100538f1c)
#define RVA_RITUAL_ADD_STRING                  ADDR(        0x2be2f0,          0x100566958)
#define RVA_RITUAL_ADD_STRING_BOUND            ADDR(        0x2be2fe,          0x100566968)
#define RVA_ALLOC_RITUAL_BOUND                 ADDR_DIFFERS(0x2be186,          0x1005670d4)
#define RVA_ALLOC_RITUAL_FAIL_BRANCH           ADDR_DIFFERS(0x2be18b,          0x1005670d8)
#define RVA_OUT_OF_RITUALS_CALL                ADDR(        0x1bf26b,          0x100532cf0)
#define RVA_MOD_COMMAND_EXIT                   ADDR(        0x1c5183,          0x10052ecbc)
#define RVA_RANDOM_RITUAL                      ADDR(        0x2bdb50,          MAC_INLINED)
#define RVA_RANDOM_RITUAL_BOUND                ADDR(        0x2bdbaa,          MAC_INLINED)
#define RVA_RANDOM_RITUAL_1A_ENTRY             ADDR(        WIN_NO_EQUIVALENT, 0x100553214)
#define RVA_RANDOM_RITUAL_1A_RESUME            ADDR(        WIN_NO_EQUIVALENT, 0x1005532f4)
#define RVA_RANDOM_RITUAL_1B_ENTRY             ADDR(        WIN_NO_EQUIVALENT, 0x10055338c)
#define RVA_RANDOM_RITUAL_1B_RESUME            ADDR(        WIN_NO_EQUIVALENT, 0x100553454)
#define RVA_RANDOM_RITUAL_2A_ENTRY             ADDR(        WIN_NO_EQUIVALENT, 0x100553cc8)
#define RVA_RANDOM_RITUAL_2A_RESUME            ADDR(        WIN_NO_EQUIVALENT, 0x100553d98)
#define RVA_RANDOM_RITUAL_2B_ENTRY             ADDR(        WIN_NO_EQUIVALENT, 0x100553e50)
#define RVA_RANDOM_RITUAL_2B_RESUME            ADDR(        WIN_NO_EQUIVALENT, 0x100553f18)
#define RVA_RANDOM_RITUAL_3_ENTRY              ADDR(        WIN_NO_EQUIVALENT, 0x100554b34)
#define RVA_RANDOM_RITUAL_3_RESUME             ADDR(        WIN_NO_EQUIVALENT, 0x100554c34)
#define RVA_EXECUTE_RITUAL                     ADDR(        0x2c5720,          0x10055d708)
#define RVA_RITUAL_FX_RANDOM_EQUIP             ADDR_DIFFERS(0x2c94b6,          0x100562b94)
#define RVA_RITUAL_FX_MAKE_ITEM                ADDR_DIFFERS(0x2c923f,          0x100562968)
#define RVA_EXECUTE_RITUAL_BOUND               ADDR(        0x2c57f6,          0x10055d774)
#define RVA_RITUAL_PR_BOUND                    ADDR(        0x2c2533,          0x100558970)
#define RVA_RITUAL_CPR_BOUND                   ADDR(        0x2c3354,          0x100558554)
#define RVA_UNIT_RITUAL_LIST_BOUND             ADDR(        0x1d757b,          MAC_NOT_FOUND)
#define RVA_RITUAL_SCREEN_LOOP_1               ADDR(        0x2ae66a,          0x10035f6d0)
#define RVA_RITUAL_SCREEN_LOOP_2               ADDR(        0x2afb72,          0x10035f9dc)
#define RVA_RITUAL_ROW_ID_GREYED               ADDR_DIFFERS(0x2afaf5,          0x1003601b8)
#define RVA_RITUAL_ROW_ID_GREYED_2             ADDR_DIFFERS(0x2afaf5,          0x10036021c)
#define RVA_RITUAL_ROW_ID_GREYED_3             ADDR_DIFFERS(0x2afaf5,          0x100360274)
#define RVA_RITUAL_ROW_ID_GREYED_4             ADDR_DIFFERS(0x2afaf5,          0x100360418)
#define RVA_RITUAL_ROW_ID_GREYED_5             ADDR_DIFFERS(0x2afaf5,          0x100360320)
#define RVA_RITUAL_ROW_ID_GREYED_6             ADDR_DIFFERS(0x2afaf5,          0x100360354)
#define RVA_RITUAL_ROW_ID_GREYED_7             ADDR_DIFFERS(0x2afaf5,          0x1003603a8)
#define RVA_RITUAL_ROW_ID_CASTABLE             ADDR_DIFFERS(0x2afd67,          0x100360474)
#define RVA_RITUAL_ROW_ID_JOIN                 ADDR(        WIN_NO_EQUIVALENT, 0x100360288)
#define RVA_RITUAL_CLICK_DECODE                ADDR(        0x2b003d,          0x100360e6c)
#define RVA_RITUAL_RIGHT_CLICK_DECODE          ADDR(        0x2b028d,          0x10036075c)
#define RVA_RITUAL_CLICK_CASTABLE_BOUND        ADDR(        0x2b004a,          0x100360e70)
#define RVA_RITUAL_RIGHT_CLICK_BAND            ADDR_DIFFERS(0x2b0293,          0x100360764)
#define RVA_RITUAL_RIGHT_CLICK_GREYED          ADDR_DIFFERS(0x2b029b,          0x100360768)
#define RVA_BACKUP_TABLES                      ADDR(        0x1ca350,          MAC_INLINED)
#define RVA_RITUAL_BACKUP                      ADDR(        0x582f7f0,         0x10c063d20)
#define RVA_RITUAL_BACKUP_SIZE                 ADDR(        0x190a8b,          0x100332524)
#define RVA_RITUAL_BACKUP_COPY_SIZE            ADDR(        0x190a95,          0x100332540)
#define RVA_RITUAL_RESTORE_SIZE                ADDR(        0x1ca6ea,          0x100528e2c)
#define RVA_CHECK_VANILLA_NAMES                ADDR(        0x115e20,          MAC_INLINED)
#define RVA_TURN_CAST_COUNTS                   ADDR(        0x2bfb60,          MAC_INLINED)
#define RVA_TURN_CAST_COUNTS_REF               ADDR_DIFFERS(0x169994,          0x1003997b0)
#define RVA_LOAD_2H_CAST_COUNTS                ADDR_DIFFERS(0x188039,          0x1004f507c)
#define RVA_LOAD_GAME_CAST_COUNTS              ADDR_DIFFERS(0x18766f,          0x1004ecac0)
#define RVA_LOAD_PLAYERS                       ADDR(        0x187030,          MAC_INLINED)
#define RVA_EXECUTE_RITUAL_CAST_COUNTS         ADDR_DIFFERS(0x2c6d1f,          0x10055ee40)
#define RVA_SAVE_GAME_CAST_COUNT_READ          ADDR_DIFFERS(0x187bb6,          0x1004f2d20)
#define RVA_SAVE_2H_CAST_COUNT_READ            ADDR_DIFFERS(0x1882a0,          0x10050aff0)
#define RVA_CAST_STRIDE_AI_MAXCAST             ADDR(        0x06d1f7,          0x1004df8fc)
#define RVA_CAST_STRIDE_EXECUTE_ADD            ADDR_DIFFERS(0x2c6d15,          0x10055ee28)
#define RVA_CAST_STRIDE_EXECUTE_INC            ADDR(        0x2c98ac,          MAC_STORED_DIFFERENTLY)
#define RVA_CAST_STRIDE_CAN_CAST               ADDR(        0x2ca106,          0x10055c088)
#define RVA_CAST_STRIDE_FX_40E                 ADDR(        0x2b626d,          0x100557634)
#define RVA_CAST_STRIDE_TURN                   ADDR(        0x16998d,          0x1003997ac)
#define RVA_CAST_STRIDE_TURN_PLAYER_29         ADDR(        WIN_NO_EQUIVALENT, 0x100392a30)
#define RVA_SAVE_GAME_END_MARKER               ADDR(        0x18f3f0,          0x1004f49d4)
#define RVA_SAVE_GAME_FCLOSE                   ADDR(        0x18f400,          0x1004f49e8)
#define RVA_SERVER_SAVE_END_MARKER             ADDR(        0x1f4355,          MAC_NO_CALL)
#define RVA_SAVE_2H_FCLOSE                     ADDR_DIFFERS(0x185386,          0x10050b054)
#define RVA_LOAD_GAME_END_MARKER               ADDR(        0x186ba8,          0x1004ee12c)
#define RVA_SAVE_2H_END_MARKER                 ADDR_DIFFERS(0x185364,          0x10050b040)
#define RVA_LOAD_2H_END_MARKER                 ADDR(        0x185726,          0x1004f50d8)
#define RVA_LOAD_2H_CAST_COUNTS_CALL           ADDR(        0x188042,          0x1004f5084)
#define RVA_SAVE_2H_CAST_COUNTS_CALL           ADDR(        WIN_NO_CALL,       MAC_INLINED)
#define RVA_SAVE_GAME_CAST_COUNTS_CALL         ADDR(        WIN_NO_CALL,       MAC_INLINED)
#define RVA_LOAD_GAME_CAST_COUNTS_CALL         ADDR(        0x187675,          0x1004ecac8)
#define RVA_SAVE_PLAYER_BLOCK                  ADDR(        0x188090,          MAC_INLINED)
#define RVA_SAVE_PLAYER_BLOCK_CALL             ADDR(        0x185348,          MAC_INLINED)
#define RVA_LOAD_PLAYER_BLOCK                  ADDR(        0x187e00,          MAC_INLINED)
#define RVA_RESET_PLAYERS_CLEAR                ADDR_DIFFERS(0x31c747,          0x100585c70)
#define RVA_KEY_CHECK_PLAYERS_CLEAR            ADDR_DIFFERS(0x14334c,          0x1003b54bc)
#define RVA_COMPACT_PLAYERS_MOVE               ADDR_DIFFERS(0x1f05e3,          0x1004f8cf4)
#define RVA_COMPACT_PLAYERS_MOVE_2             ADDR_DIFFERS(0x1f05e3,          0x1004f8cf4)
#define RVA_ENCHANTS                           ADDR(        0xd49c9a0,         0x10c9180c4)
#define ENCHANT_SIZE                           0x28
#define ENCHANT_KIND                           0x0
#define ENCHANT_CASTER                         0x4
#define ENCHANT_OWNER                          0x8
#define ENCHANT_RITUAL                         0xc
#define ENCHANT_X                              0x10
#define ENCHANT_Y                              0x14
#define ENCHANT_KIND_ILLUSION                  0x1d
#define ENCHANT_KIND_GUILD_TRADERS             31
#define ENCHANT_KIND_GUILD_MERCHANTS           32
#define ENCHANT_KIND_GUILD_THIEVES             33
#define ENCHANT_KIND_GUILD_ASSASSINS           34
#define ENCHANT_KIND_GUILD_SHIPWRIGHTS         35
#define ENCHANT_KIND_GUILD_CARTOGRAPHERS       36
#define ENCHANT_KIND_GUILD_MINERS              37
#define ENCHANT_KIND_GUILD_MASONS              38
#define ENCHANT_KIND_GUILD_SAGES               39
#define ENCHANT_KIND_GUILD_MAGES               40
#define ENCHANT_KIND_GUILD_MERCENARIES         41
#define ENCHANT_KIND_GUILD_ADVENTURERS         42
#define ENCHANT_KIND_GUILD_APOTHECARIES        43
#define ENCHANT_KIND_GUILD_ALCHEMISTS          44
#define ENCHANT_KIND_GUILD_BANKERS             45
#define ENCHANT_KIND_GUILD_SHADOW_STALKERS     46
#define ENCHANT_KIND_GUILD_VOID_PILOTS         47
#define ENCHANT_KIND_GUILD_ENGINEERS           48
#define ENCHANT_KIND_GUILD_ARCANE              49
#define ENCHANT_KIND_GUILD_BEASTMASTERS        50
#define ENCHANT_KIND_GUILD_ALCHEMIST_MAGUS     51
#define RVA_LAST_ENCHANT                       ADDR(        0x1afe6d4,         0x101cce590)
#define ENCHANTS_MAX                           1000
#define RVA_LOAD_ENCHANTS                      ADDR(        0x18ce70,          0x1004efb68)
#define RVA_NEW_ENCHANT                        ADDR(        0x2c1110,          0x10056393c)
#define RVA_NET_SEND_ENCHANT                   ADDR(        WIN_INLINED,       MAC_INLINED)
#define RVA_NET_ENCHANT_UPDATE                 ADDR(        0x1eabb0,          0x100506be4)
#define RVA_COMMANDER_REPEAT_RITUAL            ADDR(        0x11d3c8a4,        0x10ce0cbc4)
#define RVA_COMMANDER_REPEAT_PERCENT           ADDR(        0x11d3c8a8,        0x10ce0cbc8)
#define RVA_COMMANDER_FIELD_10                 ADDR(        0x11d3c8a0,        0x10ce0cbc0)
#define RVA_COMMANDER_AI_PLAN                  ADDR(        0x11d3c92c,        0x10ce0cc4c)
#define RVA_PERFORM_REPEAT_RITUALS             ADDR(        0x2c1970,          0x1005662f8)
#define RVA_SAVE_COMMANDER                     ADDR(        0x18c370,          0x1004f0de0)
#define RVA_LOAD_COMMANDER                     ADDR(        0x18b260,          0x1004ef68c)
#define RVA_NET_PUT_FULL_UNIT                  ADDR(        0x1e71d0,          0x100500494)
#define RVA_NET_PUT_UNIT_RECORD                ADDR(        0x1e7990,          MAC_INLINED)
#define RVA_NET_PUT_UNIT_ITEMS_LOOP            ADDR(        0x1e7a4a,          0x100500ca0)
#define RVA_NET_RECV_UNITS                     ADDR(        0x1eb020,          0x10050686c)
#define RVA_NET_GET_FULL_UNIT                  ADDR(        0x1e7070,          0x1004ff138)
#define RVA_NET_GET_UNIT_RECORD                ADDR(        0x1e7390,          0x1005013dc)
#define RVA_NET_GET_CMDR_RECORD                ADDR(        0x1e7d90,          MAC_INLINED)
#define RVA_NET_GET_UNIT_ITEMS                 ADDR(        0x069480,          MAC_INLINED)
#define RVA_NET_PUT_UNIT_ITEMS_END             ADDR(        0x1e7ae0,          0x100500e3c)
#define NET_PUT_FRAME_UNIT                     FRAME_OFFSET(0,                0x18)
#define RVA_NET_GET_UNIT_ITEMS_CLEAR           ADDR(        0x069494,          0x100501930)
#define RVA_NET_GET_UNIT_ITEMS_TEST            ADDR(        0x06958d,          0x1005019fc)
#define RVA_NET_GET_UNIT_ITEMS_STORE           ADDR(        0x0695c6,          0x100501a24)
#define RVA_NET_GET_UNIT_ITEMS_BAD             ADDR(        0x069592,          0x100501a04)
#define RVA_NET_GET_UNIT_ITEMS_NEXT            ADDR(        0x06949b,          0x100501958)
#define RVA_SEND_ALL_ENABLED_MODS              ADDR(        0x1e4b40,          MAC_INLINED)

#define RVA_SPELL_TABLE                        ADDR(        0x569f590,         0x100839030)
#define SPELL_SIZE                             0x28
#define SPELL_PATH                             0x8
#define SPELL_LEVEL                            0xc
#define SPELL_WEAPON                           0x10
#define SPELL_FLAGS                            0x20
#define RVA_SPELL_TABLE_END                    ADDR(        0x56adff0,         0x100847a90)
#define RVA_SPELL_PATH_NAMES                   ADDR(        0x406060,          0x100838d10)
#define RVA_UNIT_PATH_LEVEL                    ADDR(        0x2e6f40,          0x1004bb46c)
#define RVA_UNIT_PATH_BONUS                    ADDR(        0x2e4830,          0x1004bb004)
#define RVA_UNIT_PATH_BOOSTS                   ADDR(        WIN_INLINED,       MAC_INLINED)
#define RVA_UNIT_PATH_BOOST_ITEMS              ADDR(        0x2e4bb3,          0x1004bb0f0)
#define RVA_UNIT_PATH_BOOST_ITEMS_END          ADDR(        0x2e4c34,          0x1004bb29c)
#define RVA_UNIT_PATH_BOOSTS_UNCALLED          ADDR(        0x1136d0,          MAC_NOT_FOUND)
#define RVA_UNIT_RESISTANCES_ITEMS_END         ADDR(        0x32a429,          0x100567610)
#define RVA_UNIT_STATE_FLAGS_ITEMS_END         ADDR_DIFFERS(0x32a1ff,          0x100567374)
#define RVA_UNIT_BENEFITS_ITEMS_END            ADDR_DIFFERS(0x320cd4,          0x10056793c)
#define RVA_UNIT_FLAGS_ITEMS_END               ADDR(        0x328bdc,          0x100569eac)
#define RVA_UNIT_SPELL_WEAPON_LEVEL            ADDR(        0x2e7050,          0x1004bba60)
#define RVA_UNIT_ABILITY_TOTAL                 ADDR(        0x1174d0,          0x1004662fc)
#define RVA_ITEM_ABILITY_HAS_VALUE             ADDR(        WIN_INLINED,       0x1004e8698)
#define RVA_IS_SPELL_ATTACK                    ADDR(        0x0e0460,          MAC_INLINED)
#define RVA_ADD_MONSTER_ATTACK                 ADDR(        0x0b8710,          0x1003f7e54)
#define RVA_MONSTER_SET_ABILITY                ADDR(        0x1d98d0,          0x100490f64)
#define RVA_ITEM_SET_ABILITY                   ADDR(        0x17e0e0,          MAC_INLINED)
#define RVA_ITEM_ADD_ABILITY                   ADDR(        0x17e1b0,          MAC_INLINED)
#define RVA_PARSER_SPELLWEAPON50S_PATH         ADDR(        0x1bcecb,          0x100530560)
#define RVA_PARSER_SPELLWEAPON_PATH            ADDR(        0x1bcf4b,          0x1005305f8)
#define RVA_PARSER_SPELLWEAPONBONUS_PATH       ADDR(        0x1bcfcb,          0x100530694)
#define RVA_PARSER_SPELLWEAPON50_PATH          ADDR(        0x1bd04b,          0x100530710)
#define RVA_PARSER_SPELLWEAPONSINGLE_PATH      ADDR(        0x1bd0cb,          0x1005307a8)
#define RVA_ITEM_ABILITY_LINES                 ADDR(        0x17ee50,          MAC_INLINED)
#define RVA_ITEM_PATH_BOOST_BOUND              ADDR_DIFFERS(0x17f07b,          0x100525a30)
#define RVA_ITEM_PATH_BOOST_BOUND_2            ADDR_DIFFERS(0x17f07b,          0x100525a7c)
#define RVA_ITEM_PATH_BOOST_BOUND_3            ADDR_DIFFERS(0x17f07b,          0x100525ac8)
#define RVA_ITEM_PATH_BOOST_BOUND_4            ADDR_DIFFERS(0x17f07b,          0x100525b10)
#define RVA_UNIT_DEBUG_SCREEN                  ADDR(        0x1cf650,          MAC_INLINED)
#define RVA_UNIT_DEBUG_PATH_BOUND              ADDR(        0x1cf96d,          0x100526e00)
#define RVA_SPELL_BROWSER_PATH_BOUND           ADDR(        WIN_NOT_FOUND,     MAC_NOT_FOUND)
#define RVA_STATS_SPELL_ATTACK_PATH_BOUND      ADDR(        0x1d6006,          0x1005237dc)
#define RVA_STATS_KNOWN_SPELLS_PATH_LOOP       ADDR(        0x1d7378,          0x100524034)
#define RVA_LIBRARY_PATH_LOOP                  ADDR(        0x2de1a8,          0x10035d0fc)
#define RVA_LEARN_START_SPELLS_PATH_LOOP       ADDR(        0x2e614c,          0x1004bc974)
#define RVA_CHOOSE_MEMORIZED_PATH_LOOP         ADDR(        0x2e6a20,          0x1004bda00)
#define RVA_SPELL_BUG_CHECK_PATH_BOUND         ADDR(        0x12a879,          0x10046432c)
#define RVA_MASTERY_TRANSFORM                  ADDR(        0x2b2b10,          0x10055d388)
#define RVA_RITUAL_MASTERY_PATH_LOOP           ADDR(        0x2c66bd,          0x10055f908)
#define RVA_RITUAL_TRANSFORM_PATH_LOOP         ADDR(        0x2c6762,          0x10055fae8)
#define RVA_PATH_NAMES_READ_ITEM_BOOST         ADDR_DIFFERS(0x17f080,          0x100525a38)
#define RVA_PATH_NAMES_READ_ITEM_BOOST_2       ADDR_DIFFERS(0x17f080,          0x100525a84)
#define RVA_PATH_NAMES_READ_ITEM_BOOST_3       ADDR_DIFFERS(0x17f080,          0x100525ad0)
#define RVA_PATH_NAMES_READ_ITEM_BOOST_4       ADDR_DIFFERS(0x17f080,          0x100525b18)
#define RVA_PATH_NAMES_READ_STATS_LEVEL        ADDR_DIFFERS(0x1d6029,          0x1005237ec)
#define RVA_PATH_NAMES_READ_STATS_KNOWN        ADDR_DIFFERS(0x1d6950,          0x10052417c)
#define RVA_PATH_NAMES_READ_LIBRARY            ADDR_DIFFERS(0x2dded1,          0x10035d200)
#define RVA_PATH_NAMES_READ_LIBRARY_2          ADDR(        0x2ddf04,          MAC_NOT_FOUND)
#define RVA_PATH_NAMES_READ_DEBUG_UNIT         ADDR_DIFFERS(0x1cf973,          0x100526da0)
#define RVA_PATH_NAMES_READ_SPELL_BROWSER      ADDR(        WIN_NOT_FOUND,     MAC_NOT_FOUND)
#define RVA_SPELL_BUG_CHECK_SPELL_LOOP_SKIP    ADDR_DIFFERS(0x12a850,          0x10046432c)
#define RVA_SPELL_BUG_CHECK_SPELL_LOOP_END     ADDR(        0x12a933,          0x1004643a8)
#define RVA_UNIT_COMMANDER_INDEX               ADDR(        0x129580,          MAC_INLINED)
#define RVA_MASTERY_COMMANDER_CALL             ADDR(        WIN_NO_CALL,       MAC_NO_CALL)
#define RVA_MASTERY_SNAPSHOT_CALL              ADDR(        0x2b2ba6,          0x10055d408)
#define RVA_MASTERY_CHOOSE_MEMORIZED_CALL      ADDR(        0x2b308d,          0x10055d574)
#define RVA_LIBRARY_MASTERY_CALL               ADDR(        0x2de3c6,          0x10035d6e4)
#define RVA_RITUAL_MASTERY_COMMANDER_CALL      ADDR(        WIN_NO_CALL,       MAC_NO_CALL)
#define RVA_RITUAL_MASTERY_SNAPSHOT_CALL       ADDR(        0x2c66bd,          0x10055f908)
#define RVA_RITUAL_MASTERY_MEMORIZE_CALL       ADDR(        0x2c66bd,          0x10055f908)
#define RVA_RITUAL_TRANSFORM_SNAPSHOT_CALL     ADDR(        0x2c6755,          0x10055fad4)
#define RVA_RITUAL_TRANSFORM_FIND_MONSTER_CALL ADDR(        0x2c6773,          0x10055faf8)
#define RVA_RITUAL_TRANSFORM_MEMORIZE_CALL     ADDR(        0x2c67f4,          0x100560524)
#define RVA_LEARN_START_SPELLS_CALL            ADDR(        0x0beacd,          0x1003b9e28)
#define RVA_LEARN_START_SPELLS_CALL_2          ADDR(        0x10f2c3,          0x10046d640)
#define RVA_LEARN_START_SPELLS_CALL_3          ADDR(        0x129b69,          0x10046ecb4)
#define RVA_LEARN_START_SPELLS_CALL_4          ADDR(        0x123c6c,          0x10046a150)
#define RVA_LEARN_START_SPELLS_CALL_5          ADDR(        0x186cb5,          0x1004ee1d8)
#define RVA_LEARN_START_SPELLS_CALL_6          ADDR(        0x186fdc,          MAC_NOT_FOUND)
#define RVA_CHOOSE_MEMORIZED_CALL              ADDR(        0x0bead5,          0x1003b9e30)
#define RVA_CHOOSE_MEMORIZED_CALL_2            ADDR(        0x10fcd8,          0x10046d9a4)
#define RVA_CHOOSE_MEMORIZED_CALL_3            ADDR(        0x10fdee,          MAC_NOT_FOUND)
#define RVA_CHOOSE_MEMORIZED_CALL_4            ADDR(        0x129ee3,          0x10046ecbc)
#define RVA_CHOOSE_MEMORIZED_CALL_5            ADDR(        0x123c74,          0x10046a158)
#define RVA_CHOOSE_MEMORIZED_CALL_6            ADDR(        0x1d81ce,          0x100527150)
#define RVA_CHOOSE_MEMORIZED_CALL_7            ADDR(        0x1f6ccf,          0x1004faea8)
#define RVA_CHOOSE_MEMORIZED_CALL_8            ADDR(        0x2e25ce,          0x100352844)
#define RVA_CHOOSE_MEMORIZED_CALL_9            ADDR(        0x2e4739,          0x1004bed6c)
#define RVA_CHOOSE_MEMORIZED_JUMP              ADDR_DIFFERS(0x2e402b,          0x1004bf374)
#define RVA_CHOOSE_MEMORIZED_JUMP_2            ADDR(        0x2e6f29,          MAC_INLINED)
#define RVA_CHOOSE_MEMORIZED_CALL_10           ADDR(        0x0706f6,          0x1004d4e58)
#define RVA_CHOOSE_MEMORIZED_CALL_11           ADDR(        0x186cc1,          0x1004ee1f0)
#define RVA_CHOOSE_MEMORIZED_CALL_12           ADDR(        0x186fe7,          MAC_NOT_FOUND)
#define RVA_CHOOSE_MEMORIZED_CALL_13           ADDR(        0x184000,          MAC_NOT_FOUND)
#define RVA_CHOOSE_MEMORIZED_CALL_14           ADDR(        0x1840c4,          MAC_NOT_FOUND)
#define RVA_CHOOSE_MEMORIZED_CALL_15           ADDR(        0x184151,          MAC_NOT_FOUND)
#define RVA_CHOOSE_MEMORIZED_CALL_16           ADDR(        0x2b3057,          MAC_NOT_FOUND)
#define RVA_LIBRARY_KNOWN_COUNT_CALL           ADDR(        0x2dddee,          MAC_INLINED)
#define RVA_LIBRARY_ROWS_END_CALL              ADDR(        WIN_NO_CALL,       MAC_INLINED)
#define RVA_LIBRARY_ROWS_END_LAYOUT_CALL       ADDR(        0x2de1b1,          MAC_INLINED)
#define RVA_STATS_KNOWN_COUNT_CALL             ADDR(        WIN_NO_CALL,       MAC_INLINED)
#define RVA_STATS_KNOWN_END_CALL               ADDR(        WIN_NO_CALL,       MAC_INLINED)
#define RVA_STATS_KNOWN_END_LAYOUT_CALL        ADDR(        0x1d75c7,          MAC_INLINED)
#define RVA_STATS_KNOWN_BOXES_SKIP             ADDR(        0x1d6884,          MAC_INLINED)
#define FRAME_STATS_SCREEN_UNIT                FRAME_OFFSET(0x000044,          MAC_INLINED)
#define FRAME_STATS_SCREEN_STRIPE              FRAME_OFFSET(0x000048,          MAC_INLINED)
#define FRAME_DOATTACK_SPELL_WEAPON            FRAME_OFFSET(0x00006c,         0xb8)
#define FRAME_DOATTACK_WEAPON                  FRAME_OFFSET(0x000050,         0x108)
#define FRAME_WEAPON_ATTACK_WEAPON             FRAME_OFFSET(0x000090,         0x168)
#define FRAME_WEAPON_HIT_WEAPON                FRAME_OFFSET(0x0000a0,         0xd0)
#define RVA_TITLE_CELL                         ADDR(        0x03a000,          MAC_INLINED)
#define RVA_HEADER_TEXT                        ADDR(        0x039f40,          0x10033cdfc)
#define RVA_TEXT_CELL                          ADDR(        0x039be0,          MAC_INLINED)
#define RVA_SPELL_NAME_BUTTON                  ADDR(        0x039620,          MAC_INLINED)
#define RVA_MEMORIZE_ALLOWANCE                 ADDR(        0x2e43e0,          MAC_INLINED)
#define RVA_MEMORIZED_SLOT                     ADDR(        0x2e4390,          0x1004be76c)
#define RVA_SPELL_RANGE_BONUS                  ADDR(        0x31eae0,          0x10056c120)
#define RVA_DAMAGE_TEXT                        ADDR(        0x1ce860,          0x100527220)
#define RVA_UNIT_OWNER                         ADDR(        0x128ec0,          MAC_INLINED)
#define RVA_GET_UNIT_X                         ADDR(        0x329910,          MAC_INLINED)
#define RVA_GET_UNIT_Y                         ADDR(        0x32a220,          MAC_INLINED)
#define RVA_LIBRARY_LEVEL                      ADDR(        0x1b14a0,          MAC_INLINED)
#define RVA_PLAYER_LIBRARY_TOTAL               ADDR(        0x1b0b30,          0x1004ab4d0)
#define RVA_COUNT_SQUARES_OF_TERRAIN           ADDR(        0x32daf0,          0x1004ced94)
#define RVA_COUNT_GROUP_CITADELS               ADDR(        0x0af190,          MAC_INLINED)
#define RVA_COUNT_GROUP_SQUARES                ADDR(        0x2c16d0,          MAC_INLINED)
#define RVA_MONSTER_GATHERS_RESOURCE           ADDR(        0x090d20,          0x100363a70)
#define RVA_SQUARE_LIBRARY_LEVEL               ADDR(        0x1b16e0,          MAC_INLINED)
#define RVA_SQUARE_ENCHANT_VALUE               ADDR(        0x2be4b0,          0x100565cbc)
#define RVA_UNIT_MAX_HP                        ADDR(        0x3259d0,          0x10056a8e0)
#define RVA_UNIT_FLAGS                         ADDR(        0x328a00,          0x100569d20)
#define RVA_UNIT_AP_LEFT                       ADDR(        0x320290,          MAC_INLINED)
#define UNIT_AP_USED                           0x1c
#define UNIT_FLAG_SLOW                         0x8
#define UNIT_FLAG_FAST                         0x10
#define UNIT_FLAG_FLY_OR_FLOAT                 0x81
#define UNIT_FLAG_WATER_MOVE                   0x40
#define UNIT_FLAG_MT_NOLAND                    0x200
#define UNIT_FLAG_PASSWALL                     0x100000000
#define UNIT_FLAG_SHIPMOVE                     0x10000000000
#define UNIT_FLAG_TUNNEL                       0x40000000000
#define RVA_UNIT_HAS_FLAG_4                    ADDR(        0x1d9820,          MAC_INLINED)
#define RVA_COUNT_MEMORIZED_OF_TYPE            ADDR(        0x2e3be0,          MAC_INLINED)
#define RVA_PARSER_LEVELUP_EFFECT              ADDR(        0x1c3f37,          0x1005374d8)
#define RVA_PARSER_LEVELUPMON_EFFECT           ADDR(        0x1c3f50,          0x1005374f4)
#define RVA_PARSER_RITPOW_STORE                ADDR(        0x1bf526,          0x100532de4)
#define RVA_ADD_KNOWN_SPELL                    ADDR(        0x2e6b70,          0x1004bc38c)
#define RVA_RANDOM_SPELL                       ADDR(        0x2e3cc0,          0x1004bad84)
#define RVA_RANDOM_SPELL_NOT_KNOWN             ADDR(        0x2e3f10,          0x1004beacc)
#define RVA_LEARN_START_SPELLS                 ADDR(        0x2e55a0,          0x1004bc90c)
#define RVA_LEARN_RANDOM_SPELL                 ADDR(        0x2e46d0,          0x1004beb68)
#define RVA_CHOOSE_MEMORIZED                   ADDR(        0x2e61c0,          0x1004bd988)
#define RVA_RANDOM_MEMORIZED_SPELL             ADDR(        0x2e5370,          0x1004bd208)
#define RVA_ADD_AFFLICTION                     ADDR(        0x0f45d0,          0x1003ca1e4)
#define RVA_BATTLE_WOUND                       ADDR(        0x0bb560,          MAC_INLINED)
#define RVA_BATTLE_WOUND_ADD_CALL              ADDR(        0x0bb8cc,          0x1003b97a0)
#define RVA_WEAPON_HIT_AFFLICT_ADD_CALL        ADDR(        0x0de2c4,          0x1003c48b0)
#define RVA_REANIMATE_FORM_CALL_A              ADDR(        0x0df329,          0x1003c49b4)
#define RVA_REANIMATE_FORM_CALL_B              ADDR(        0x0df357,          0x1003c49e4)
#define RVA_MUTATE_LOG_CALL                    ADDR(        0x0df64b,          0x1003c4c24)
#define RVA_UNIT_TRAIT_LIST_UNIT_CALL          ADDR(        0x1d5a09,          0x100523168)
#define RVA_UNIT_TRAIT_LIST_PREVIEW_CALL       ADDR(        0x1d59d0,          0x100523134)
#define RVA_UNIT_TRAIT_LIST_ITEM_CALL          ADDR(        0x17eb25,          0x100525ff8)
#define RVA_ABILITY_DESCRIPTIONS               ADDR(        0x1abc260,         0x10487ff38)
#define RVA_ABILITY_LOOKUP_CALL                ADDR(        0x1d0f0f,          0x10051c95c)
#define RVA_UNIT_CARD                          ADDR(        0x1d4290,          0x100521160)
#define RVA_MEMORIZE_SPELL                     ADDR(        0x2e6d70,          0x1004bc44c)
#define RVA_UNMEMORIZE_SPELL                   ADDR(        0x2e4280,          MAC_INLINED)
#define RVA_UNMEMORIZE_SPELL_BOUND             ADDR_DIFFERS(0x2e42a0,          0x1004bda94)
#define RVA_UNMEMORIZE_SPELL_BOUND_2           ADDR_DIFFERS(0x2e42a0,          0x1004be584)
#define RVA_UNMEMORIZE_SPELL_BOUND_3           ADDR_DIFFERS(0x2e42a0,          0x100526a4c)
#define RVA_COUNT_KNOWN_SPELLS                 ADDR(        0x2e4410,          0x1004bcd64)
#define RVA_COUNT_SPELLS                       ADDR(        0x2e3dc0,          MAC_INLINED)
#define RVA_SPELL_INFO_BOX                     ADDR(        0x2e40a0,          MAC_INLINED)
#define RVA_COUNT_MEMORIZED_SPELLS             ADDR(        0x2e4040,          0x1004bcecc)
#define RVA_LEARN_PROMOTION_SPELLS             ADDR(        0x2e3fc0,          0x1004bf1a8)
#define RVA_UNIT_FIRST_SPELL_PATH              ADDR(        0x2e6e70,          0x1004bed90)
#define RVA_RANDOM_1_TO_N                      ADDR(        0x058b70,          0x100336d08)
#define RVA_COMMANDER_ADD_AP_USED              ADDR(        0x12c210,          0x100481578)
#define RVA_UNIT_ADD_AP_USED                   ADDR(        0x118460,          MAC_INLINED)
#define RVA_FLUSH_COMMANDER_CACHES             ADDR(        0x1296c0,          MAC_INLINED)
#define RVA_COMMANDER_ARMY_AP_LEFT             ADDR(        0x328880,          0x10056a728)
#define COMMANDER_AP_CACHE                     0xa3
#define RVA_COMMANDER_CACHE_INTS               ADDR(        0x66053b0,         0x106cd5de8)
#define RVA_START_SPELLS_MORE_SPELLS_CALL      ADDR(        WIN_NO_CALL,       0x1004bca64)
#define RVA_START_SPELLS_FORTUNE_CHECK         ADDR_DIFFERS(0x2e5f32,          0x1004bcb54)
#define RVA_START_SPELLS_FORTUNE_CHECK_2       ADDR(        WIN_NO_EQUIVALENT, 0x1004bcd2c)
#define RVA_RITUAL_NEWSPELL_LEARN_CALL         ADDR_DIFFERS(0x2c66dc,          0x10055f920)
#define RVA_RITUAL_NEWSPELL_LEARN_CALL_2       ADDR_DIFFERS(0x2c66fb,          0x10055f920)
#define RVA_RITUAL_NEWSPELL_LEARN_CALL_3       ADDR_DIFFERS(0x2c671a,          0x10055f920)
#define RVA_RITUAL_SUMMON_LEARN_CALL           ADDR(        0x2c6227,          0x100560b30)
#define RVA_RITUAL_CAN_CAST_NEWSPELL           ADDR(        0x2ca4f7,          0x10055c234)
#define RVA_RITUAL_CAN_CAST_NEWSPELL_2         ADDR(        0x2ca55e,          0x10055c264)
#define RVA_RITUAL_CAN_CAST_NEWSPELL_3         ADDR(        0x2ca5be,          0x10055c294)
#define RVA_ITEM_USE_SCROLL_LEARN_CALL         ADDR(        WIN_NO_CALL,       0x1004e6908)
#define RVA_ITEM_USE_SCROLL_LEARN_CALL_2       ADDR(        WIN_NO_CALL,       0x1004e6970)
#define RVA_ITEM_USE_SCROLL_LEARN_CALL_3       ADDR(        WIN_NO_CALL,       0x1004e69d8)
#define RVA_LIBRARY_ROW_PATH_LEVEL_CALL        ADDR(        0x2dde09,          0x10035d10c)
#define RVA_LIBRARY_LEARN_CALL                 ADDR_DIFFERS(0x2de5b6,          0x10035db64)
#define RVA_USE_LIBRARY_LEARN_CALL             ADDR(        0x2e36fe,          0x10035ca58)
#define RVA_AI_USE_LIBRARY_CALL                ADDR(        0x07abeb,          0x1004d1f40)
#define RVA_PRIEST_PROMOTION_LEARN_CALL        ADDR(        0x2e25c5,          0x10035272c)
#define RVA_DOATTACK_PATH_BONUS_CALL           ADDR(        0x0d437b,          0x1003eb990)
#define RVA_DOATTACK_SPELL_PICK_CALL           ADDR(        0x0d4454,          0x1003eba5c)
#define RVA_DOATTACK_CAST_COUNT                ADDR_DIFFERS(0x0d44c7,          0x1003ebac4)
#define RVA_STATS_KNOWN_PATH_LEVEL_CALL        ADDR(        0x1d68ff,          0x1005240a8)
#define RVA_FIND_SPELL                         ADDR(        0x2e6c80,          MAC_INLINED)
#define RVA_SPELL_USEFUL                       ADDR(        0x2e5480,          0x1004bf9ac)
#define RVA_SPELL_FLAG_LINES                   ADDR(        0x2e44a0,          0x1004bf378)
#define RVA_SPELL_FLAG_LINES_CALL_A            ADDR(        0x17ec06,          MAC_INLINED)
#define RVA_SPELL_FLAG_LINES_CALL_B            ADDR(        0x17f98f,          MAC_INLINED)
#define RVA_SPELL_FLAG_LINES_CALL_C            ADDR(        0x2e414d,          MAC_INLINED)
#define RVA_ARMY_HAS_SPELL_FLAG                ADDR(        0x2e4770,          0x1004bfaac)
#define RVA_UNIT_HEAL_RATE                     ADDR(        0x324500,          MAC_INLINED)
#define RVA_UPKEEP_HEAL_RATE_CALL              ADDR(        0x144506,          MAC_INLINED)
#define RVA_UPKEEP_HEAL_RESIST_CALL            ADDR(        WIN_NO_CALL,       0x10039b510)
#define RVA_SPELL_BROWSER                      ADDR(        WIN_NOT_FOUND,     MAC_NOT_FOUND)
#define RVA_SPELL_BROWSER_BOUND                ADDR(        WIN_NOT_FOUND,     MAC_NOT_FOUND)
#define RVA_LISTSPELLS_BOUND                   ADDR(        WIN_NOT_FOUND,     MAC_NOT_FOUND)
#define RVA_NET_RECV_MEMORIZED_BOUND           ADDR_DIFFERS(0x1f6c9b,          0x1004fabf0)
#define RVA_STATS_SPELL_ROW_ID                 ADDR_DIFFERS(0x1d6d1f,          0x100524488)
#define RVA_STATS_SPELL_ROW_DECODE             ADDR(        0x1d8036,          0x100524de8)
#define RVA_STATS_SPELL_ROW_RIGHT_BOUND        ADDR_DIFFERS(0x1d803c,          0x100524df4)
#define RVA_STATS_SPELL_ROW_RIGHT_SHIFT        ADDR(        WIN_NO_EQUIVALENT, 0x100524df0)
#define RVA_STATS_SPELL_ROW_LEFT_OFFSET        ADDR_DIFFERS(0x1d8047,          0x100524dfc)
#define RVA_STATS_SPELL_ROW_LEFT_SHIFT         ADDR(        WIN_NO_EQUIVALENT, 0x100524e04)
#define RVA_STATS_SPELL_ROW_LEFT_BOUND         ADDR_DIFFERS(0x1d804d,          0x100524e08)
#define RVA_STATS_SPELL_ROW_LEFT_BOUND_2       ADDR(        WIN_NO_EQUIVALENT, 0x100525580)
#define RVA_STATS_SPELL_ROW_LEFT_SPELL         ADDR_DIFFERS(0x1d8093,          0x100524dfc)
#define RVA_STATS_SPELL_ROW_INFO_SPELL         ADDR_DIFFERS(0x1d8150,          0x100524e1c)
#define RVA_STATS_SPELL_ROW_INFO_SHIFT         ADDR(        WIN_NO_EQUIVALENT, 0x100524e24)
#define RVA_STATS_SPELL_ROW_INFO_BOUND         ADDR(        WIN_NO_EQUIVALENT, 0x100524e28)
#define RVA_FIND_SPELL_TABLE_REF               ADDR_DIFFERS(0x2e6cab,          0x1004bc3b8)
#define RVA_NET_RECV_MEMORIZED_BRANCH          ADDR_DIFFERS(0x1f6ca1,          0x1004fabf4)
#define RVA_SLOW_SPELL_ENCODE                  ADDR_DIFFERS(0x0d45fa,          0x1003ec888)
#define RVA_SLOW_SPELL_DECODE                  ADDR(        0x0d43e3,          0x1003eb9f8)
#define RVA_AI_ZAP_SLOW_SPELL_CHECK            ADDR_DIFFERS(0x0cd04d,          0x1003e4ce4)
#define RVA_SPELL_BUG_CHECK                    ADDR(        0x129fc0,          0x100463ca0)
#define RVA_NO_CHECKS                          ADDR_DIFFERS(0x582398c,         0x106cd54c8)
#define RVA_PREF_END_TURN_WARNING              ADDR(        0x5828158,         0x1069b1698)
#define RVA_UNIT_ALIVE_AT                      ADDR(        0x3260f0,          MAC_NOT_FOUND)
#define RVA_DOATTACK                           ADDR(        0x0d40b0,          0x1003eb41c)
#define RVA_GET_TARGET_SQUARE                  ADDR(        0x0d5ce0,          0x1003bf0e4)
#define RVA_LIBRARY_SCREEN                     ADDR(        0x2dd8e0,          0x10035cb3c)
#define RVA_USE_LIBRARY                        ADDR(        0x2e3570,          0x10035c7b4)
#define RVA_ITEM_USE                           ADDR(        0x183c70,          0x1004e6640)
#define RVA_FIGHT_THE_FIGHT                    ADDR(        0x0f1cb0,          0x1003dfad0)
#define RVA_FIGHT_ZAP_CASTABLE_CALL            ADDR(        0x0f2fec,          0x1003e3bd4)
#define RVA_AI_ZAP_CHOICE                      ADDR(        0x0cc5f0,          MAC_INLINED)
#define RVA_AI_ZAP_CASTABLE_CALL               ADDR(        0x0cc758,          0x1003e3d60)
#define RVA_BATTLE_ITEM_EFFECTS                ADDR(        0x0d69a0,          MAC_INLINED)
#define RVA_ITEM_CASTABLE_SPELL                ADDR(        0x183220,          0x1004e8de0)
#define RVA_ITEM_INFO_SCREEN                   ADDR(        0x17e850,          MAC_INLINED)
#define RVA_ITEM_SPELL_LINE                    ADDR(        0x17f9b0,          MAC_NOT_FOUND)
#define RVA_ITEM_SPELL_STATS                   ADDR(        0x17f930,          MAC_NOT_FOUND)
#define RVA_KNOWN_SPELL_SLOT_BY_NAME           ADDR(        0x2e39a0,          MAC_NOT_FOUND)
#define RVA_MEMORIZE_SPELL_BY_NAME             ADDR(        0x2e3c90,          MAC_NOT_FOUND)
#define RVA_UNIT_KNOWN_SPELLS                  ADDR(        0xfb8e800,         0x1100498a0)
#define RVA_UNIT_MEMORIZED_SPELLS              ADDR(        0xfb8e878,         0x110049918)
#define RVA_UNIT_SLOW_SPELL                    ADDR(        0xfb8e904,         0x1100499a4)
#define RVA_SAVE_UNIT                          ADDR(        0x18bee0,          0x1004f07d0)
#define RVA_LOAD_UNIT                          ADDR(        0x18d200,          0x1004ef09c)
#define RVA_NET_SEND_UNITS                     ADDR(        0x1f26b0,          0x10050e114)
#define RVA_FIND_WEAPON                        ADDR(        0x0b87e0,          0x1003f7de0)

#define RVA_UNIT_ITEMS                         ADDR(        0xfb8e8b4,         0x110049954)
#define UNIT_ITEM_SLOTS                        8
#define RVA_UNIT_TEMP_ABILITY_IDS              ADDR(        0xfb8e8ce,         0x11004996e)
#define RVA_UNIT_TEMP_ABILITY_VALUES           ADDR(        0xfb8e8e0,         0x110049980)
#define UNIT_TEMP_ABILITIES                    4
#define RVA_SET_TEMP_ABILITY                   ADDR_DIFFERS(0x1184a0,          0x10046823c)
#define RVA_GET_TEMP_ABILITY                   ADDR(        0x116d40,          0x100468338)
#define RVA_COMMANDER_KNOWN_RITUALS            ADDR(        0x11d3c8ac,        0x10ce0cbcc)
#define RVA_GROUND_ITEMS                       ADDR(        0xf958020,         0x10cb0e1d4)
#define GROUND_ITEMS_PER_SQUARE                6
#define MAP_COLUMN_SQUARES                     160
#define MAP_COLUMNS_MAX                        1000
#define RVA_ADD_GROUND_ITEMS                   ADDR(        0x26c2a0,          0x1003f830c)
#define RVA_SQUARE_RECORDS                     ADDR(        0xa20aca0,         0x10d011c78)
#define SQUARE_SIZE                            0x13c
#define SQUARE_ARMY_OWNER                      0x3a
#define SQUARE_HOLDER                          0x3c
#define SQUARE_OWNER                           0x42
#define SQUARE_GEMS                            0xa3
#define SQUARE_GEM_KINDS                       4
#define SQUARE_TERRAIN_TYPE                    0x20
#define RVA_SAVE_SQUARE                        ADDR(        0x18bb10,          0x1004f13c8)
#define RVA_LOAD_SQUARE                        ADDR(        0x18c7d0,          0x1004ee684)
#define RVA_SAVE_GAME_SQUARES_CALL             ADDR(        0x18ead8,          0x1004f28cc)
#define RVA_LOAD_GAME_SQUARES_CALL             ADDR(        0x1860c7,          0x1004ebf24)
#define RVA_SERVER_SAVE_SQUARES_CALL           ADDR(        0x1f3f87,          MAC_NO_CALL)
#define RVA_GROUND_ITEMS_CLEAR                 ADDR(        0x185f18,          0x1004ebdac)
#define RVA_SQUARE_WORK_I16                    RVA_GEN_SCRATCH_MARKS
#define RVA_MAP_IMAGE_SQUARES_I32              ADDR(        0x63fbc50,         0x10c691afc)
#define RVA_MAP_IMAGE_SQUARES_I16              ADDR(        0x63ad9c0,         MAC_NOT_FOUND)
#define RVA_MAP_IMAGE_SQUARE_FLAGS             ADDR(        0x6128740,         0x10c660898)
#define RVA_BACKGROUND_SQUARES_A               RVA_MAP_BG_FLOODED
#define RVA_BACKGROUND_SQUARES_B               RVA_MAP_BG_GROUNDS
#define RVA_WORLD_GEN_SQUARES_A                RVA_RIVER_DISTANCE
#define RVA_WORLD_GEN_SQUARES_B                RVA_RIVER_COST
#define RVA_PATH_SQUARE_CACHE                  ADDR(        0x9f7f300,         0x1121dfec4)
#define RVA_SAVE_ENCHANTS                      ADDR(        0x18b850,          0x1004f11ac)
#define RVA_SHOP_LAST                          ADDR(        0x11c14588,        0x10cce487c)
#define RVA_SHOP_OFFERS                        ADDR(        0x11c1458c,        0x10cce4880)
#define SHOP_OFFERS_MAX                        160
#define RVA_SHOP_OFFER_COUNTS                  ADDR(        0x11c1480c,        0x10cce4b00)
#define RVA_SHOP_ADD                           ADDR(        0x0af410,          0x100362174)
#define RVA_SHOP_BUILD                         ADDR(        0x0a8f40,          0x100364850)
#define RVA_SHOP_BUILD_CALL                    ADDR(        0x167ad0,          0x100395018)
#define RVA_SHOP_CLASS_OFFER_CALL              ADDR(        0x0a95d9,          0x1003657c0)
#define RVA_SHOP_LOCATION_OFFER_CALL           ADDR(        0x0ad2f5,          0x10036a720)
#define RVA_SHOP_UNITS_COUNT_CALL              ADDR(        0x0aee05,          0x100363000)
#define RVA_SHOP_UNITS_STEP_3                  ADDR(        0x0aee0f,          0x10036300c)
#define RVA_SHOP_UNITS_STEP_9                  ADDR(        0x0aee1a,          0x10036301c)
#define SHOP_BUILD_MESSAGE_CALLS               48
#define RVA_SHOP_ADD_ITEM_OFFER_CALL           ADDR(        0x0ae884,          MAC_NOT_LOOKED_UP)
#define RVA_SPECIAL_LOCATION_RECRUITS          ADDR(        0x1a29b30,         MAC_NOT_LOOKED_UP)
#define RVA_PLAYER_HAS_TERRAIN_CITADEL         ADDR(        0x1b06a0,          MAC_INLINED)
#define RVA_COUNT_SQUARES_WITH_FLAGS           ADDR(        0x330ec0,          MAC_INLINED)
#define RVA_LOAD_SHOP                          ADDR(        0x189870,          MAC_INLINED)
#define RVA_FIND_ITEM                          ADDR(        0x184d00,          0x1004e57ac)
#define RVA_AUTO_RECRUIT_SQUARES               ADDR(        0x11c14308,        0x10cce45fc)
#define RVA_AUTO_RECRUIT_MONSTERS              ADDR(        0x11c14408,        0x10cce46fc)
#define AUTO_RECRUIT_ORDERS_MAX                64
#define RVA_AUTO_RECRUIT                       ADDR(        0x166720,          MAC_INLINED)
#define RVA_MESSAGE_ATTACH_SIZES               ADDR(        0xe4a5d38,         0x1085bb0a8)
#define RVA_MESSAGE_ATTACHMENTS                ADDR(        0xe4a5d48,         0x1085bb0b8)
#define RVA_MESSAGE_SCREEN                     ADDR(        0x1e1ba0,          0x1004c2498)
#define RVA_TURN_MESSAGES_CALL                 ADDR(        0x0947c7,          0x10037b650)
#define RVA_VIEW_MESSAGES_CALL                 ADDR(        0x0975c8,          0x100380c30)
#define RVA_MESSAGES_WAIT_CALL                 ADDR(        0x1e2379,          0x1004c2cf0)
#define RVA_LOAD_BATTLE_REPLAY                ADDR(        0x0f3cb0,          MAC_INLINED)
#define RVA_LOAD_PART_STATS                    ADDR(        0x0ba450,          0x1003f77bc)
#define RVA_SAVE_PART_STATS                    ADDR(        0x0ba7d0,          0x1003df3c0)
#define RVA_SAVE_BATTLE_REPLAY                 ADDR(        0x0f1780,          MAC_INLINED)
#define RVA_NET_SEND_SQUARE                    ADDR(        0x1f2ea0,          0x10050c9bc)
#define RVA_NET_SQUARE_CLAMP                   ADDR(        0x1f0a40,          0x1004f8184)
#define RVA_SERVER_SQUARE_CLAMP_CALL           ADDR(        0x1f5914,          0x1004fd6b4)
#define RVA_NET_SEND_UNIT                      ADDR(        0x1f22a0,          0x10050e540)
#define RVA_NET_SEND_MEMORIZED                 ADDR(        0x1f0c20,          0x100510d94)
#define RVA_NET_SEND_AUTO_RECRUIT              ADDR(        0x1f2870,          0x100510a80)
#define RVA_NET_GIVE_ITEM                      ADDR(        0x1f0d10,          MAC_INLINED)
#define RVA_FIND_SHOP_OFFER                    ADDR(        0x11e830,          MAC_INLINED)
#define RVA_SHOP_OFFER_MESSAGE                 ADDR(        0x0af270,          0x10036240c)
#define RVA_SHOP_MERGE_MESSAGES                ADDR(        0x1e1940,          MAC_INLINED)
#define RVA_COUNT_NAME_TEXT                    ADDR(        0x1192d0,          0x10046c0e8)
#define RVA_SHOP_COSTS                         ADDR(        0x11c14a8c,        0x10cce4d80)
#define SHOP_COST_COUNT                        19
#define RVA_SHOP_OFFER_FLAGS                   ADDR(        0x11c17a0c,        0x10cce7d00)
#define SHOP_FLAG_ONCE                         0x1
#define SHOP_FLAG_AI_IMPORTANT                 0x2
#define SHOP_FLAG_EXTRA                        0x4
#define SHOP_FLAG_AI_NEVER                     0x10
#define SHOP_FLAG_AI_FIRST_TURNS               0x200
#define SHOP_OFFER_TEXT_SIZE                   0x30
#define SQUARE_FLAG_RECRUITED                  0x1
#define RVA_SHOP_OFFER_TERRAIN                 ADDR(        0x11c17c8c,        0x10cce7f80)
#define RVA_SHOP_OFFER_NAMES                   ADDR(        0x11c17f0c,        0x10cce8200)
#define RVA_SHOP_OFFER_LIMITS                  ADDR(        0x11c19d0c,        0x10ccea000)
#define RVA_ITEM_SLOT_TYPE                     ADDR(        0x1be6764,         0x10489382c)
#define RVA_GIVE_ITEM                          ADDR(        0x180030,          0x1004e4e70)
#define RVA_GIVE_ITEM_AND_ENFORCE              ADDR(        0x184f80,          0x1004e556c)
#define RVA_SHOP_ITEM_CAN_RECEIVE              ADDR(        WIN_INLINED,       MAC_NOT_LOOKED_UP)
#define RVA_SHOP_ITEM_RECEIVER                 ADDR(        0x182d50,          0x1004e9458)
#define RVA_ENFORCE_ITEMS                      ADDR(        0x1833d0,          0x1004e55ac)
#define RVA_CAN_EQUIP_ITEM                     ADDR(        0x184dd0,          0x1004e4548)
#define RVA_UNIT_NTH_ITEM_OF_KIND              ADDR(        0x183520,          0x1004e5f74)
#define RVA_ITEM_EQUIP_EFFECTS                 ADDR(        0x180220,          0x1004e4da8)
#define RVA_UNIT_EYE_COUNT                     ADDR(        0x322f00,          MAC_INLINED)
#define RVA_ITEM_TRANSFER                      ADDR(        0x1cfe10,          0x100520340)
#define RVA_UNIT_CARD_ITEM_ROW                 ADDR(        0x1d57fd,          0x1005225fc)
#define RVA_UNIT_CARD_ITEM_CLICK               ADDR(        0x1d7e8c,          0x1005255b4)
#define RVA_UNIT_CARD_ITEM_CLICK_CALL          ADDR(        0x1d7f19,          0x1005270e0)
#define RVA_UNIT_CARD_ITEM_ROW_START           ADDR_DIFFERS(0x1d5819,          0x1005225fc)
#define RVA_UNIT_CARD_ITEM_LOOP_TOP            ADDR(        0x1d5821,          MAC_IN_CODE)
#define RVA_UNIT_CARD_ITEM_ROW_END             ADDR(        0x1d58d6,          0x100522ff4)
#define RVA_UNIT_CARD_ITEM_INFO_READ           ADDR_DIFFERS(0x1d8174,          0x1005255e8)
#define RVA_UNIT_CARD_ITEM_INFO_SKIP           ADDR(        0x1d8181,          MAC_IN_CODE)
#define RVA_ITEM_TRANSFER_ITEM_READ            ADDR_DIFFERS(0x1cfeda,          0x1005203f4)
#define RVA_ITEM_TRANSFER_ITEM_WRITE           ADDR_DIFFERS(0x1d0366,          0x100520c48)
#define RVA_ITEM_TRANSFER_ALLY_REMOVE          ADDR_DIFFERS(0x1d02a3,          0x100520960)
#define RVA_ITEM_TRANSFER_ALLY_REMOVED         ADDR_DIFFERS(0x1d02a8,          0x100520a1c)
#define RVA_ITEM_SLOT_TOOLTIP                  ADDR(        0x1c63308,         0x100702b92)
#define UNIT_CARD_ITEM_IDS                     100
#define ITEM_CARD_HELD_FLAGS                   4
#define ITEM_CARD_EMPTY_FLAGS                  2
#define RVA_ITEM_TYPE_NAMES                    ADDR(        0x3faa80,          MAC_IN_CODE)
#define RVA_DROP_UNIT_ITEMS_AT                 ADDR(        0x17dd50,          0x1004ea188)
#define RVA_SQUARE_ARMY_HERE                   ADDR(        0xa20acda,         0x10d011cb2)
#define RVA_ANY_UNIT_HOLDS_ITEM                ADDR(        0x17e040,          MAC_INLINED)
#define RVA_UNIT_REMOVE_EXTRA_LIFE             ADDR(        0x183310,          MAC_INLINED)
#define RVA_DAMAGE_UNIT_EXTRA_LIFE             ADDR(        0x0be949,          0x1003b9b14)
#define RVA_COMMANDER_PANEL_USE_ITEMS          ADDR(        0x09a826,          MAC_INLINED)
#define ITEM_ABILITY_NODROP                    0x50f
#define ITEM_ABILITY_SOUL_CONTRACT             0x505
#define ITEM_COMBATSUM_MONSTER                 0x104
#define ITEM_COMBATSPELL_NAME                  0x148
#define ITEM_SOUL_CONTRACT_MASTER              0x190
#define ITEM_LOOT_IN_FIGHT_X                   (-3)
#define FOUND_ITEM_LOOT                        3
#define RVA_LOOT_PAST_7_AT                     ADDR_DIFFERS(0x0f6593,          0x1003deedc)
#define LOOT_FRAME_X                           FRAME_OFFSET(0x50,             0x100)
#define LOOT_FRAME_Y                           FRAME_OFFSET(0x54,             0xf8)
#define LOOT_FRAME_WINNER                      FRAME_OFFSET(0x64,             0xd8)
#define LOOT_FRAME_MESSAGE                     FRAME_OFFSET(0x60,             0)
#define LOOT_FRAME_MATCH_X                     FRAME_OFFSET(0,                0xf0)
#define LOOT_FRAME_MATCH_Y                     FRAME_OFFSET(0,                0xe0)
#define RVA_EXTRA_LIFE_LOOP_END                ADDR_DIFFERS(0x0be976,          0x1003b9cd4)
#define RVA_EXTRA_LIFE_LAST_TEST               ADDR_DIFFERS(0x0be96c,          0x1003b9cec)
#define RVA_EXTRA_LIFE_LOOP_TOP                ADDR(        0x0be958,          MAC_IN_CODE)
#define RVA_EXTRA_LIFE_LIVES_AGAIN             ADDR(        0x0be9aa,          0x1003b9d04)
#define RVA_UPKEEP_SOUL_CONTRACTS_END          ADDR_DIFFERS(0x1463ac,          0x100399b68)
#define RVA_UPKEEP_SOUL_CONTRACTS_TOP          ADDR(        0x146302,          MAC_IN_CODE)
#define RVA_UPKEEP_SOUL_CONTRACTS_AFTER        ADDR(        0x1463ba,          0x100399b68)
#define SOUL_CONTRACT_FRAME_PLAYER             FRAME_OFFSET(0x54,             0)
#define SOUL_CONTRACT_FRAME_X                  FRAME_OFFSET(0,                0x68)
#define SOUL_CONTRACT_FRAME_Y                  FRAME_OFFSET(0,                0x70)
#define RVA_FIND_NAMED_UNIT                    ADDR_DIFFERS(0x1131a0,          0x1004686a0)
#define RVA_AI_USE_ITEMS_LOOP                  ADDR_DIFFERS(0x0709e1,          0x1004d5094)
#define RVA_AI_USE_ITEMS_DONE                  ADDR(        0x070a18,          0x1004d51f4)
#define RVA_AI_REDISTRIBUTE_UNITS              ADDR_DIFFERS(0x0734d5,          0x1004d4b34)
#define RVA_AI_REDISTRIBUTE_NEXT_SQUARE        ADDR(        WIN_NO_EQUIVALENT, 0x1004d4a2c)
#define AI_REDISTRIBUTE_FRAME_PLAYER           FRAME_OFFSET(0,                -0x88)
#define RVA_PANEL_USE_ITEM_READ                ADDR_DIFFERS(0x09a830,          0x10037ed84)
#define RVA_PANEL_USE_ITEMS_NEXT               ADDR_DIFFERS(0x09a9a8,          0x10037ed78)
#define RVA_PANEL_USE_ITEMS_DONE               ADDR(        0x09a9b9,          0x10037ef50)
#define PANEL_FRAME_UNIT                       FRAME_OFFSET(0,                0x138)
#define RVA_BATTLE_ITEM_EFFECTS_NEXT           ADDR_DIFFERS(0x0d6c08,          0x1003e41ec)
#define RVA_BATTLE_ITEM_EFFECTS_READ           ADDR_DIFFERS(0x0d6ab0,          0x1003e41fc)
#define UNIT_CARD_FRAME_MONSTER                FRAME_OFFSET(0x60,             0x224)
#define RVA_CLEAR_UNIT_RECORD                  ADDR(        0x0bae40,          MAC_INLINED)
#define RVA_RESET_ALL_UNITS                    ADDR(        0x0f1ae0,          MAC_INLINED)
#define RVA_UNIT_LONGEST_RANGE                 ADDR(        0x3208e0,          MAC_NOT_FOUND)
#define RVA_UNIT_ATTACK_ROW_COPY               ADDR(        0x321ef0,          MAC_INLINED)
#define RVA_MAP_TOKEN_ITEMS                    ADDR(        0x09af80,          MAC_INLINED)
#define RVA_SAVE_UNIT_ITEMS_LOOP               ADDR(        0x18c0d4,          0x1004f09b8)
#define RVA_LOAD_UNIT_ITEMS_READ               ADDR(        0x18d47d,          0x1004ef3e0)
#define ITEM_TYPE_MISSILE_WEAPON               2
#define RVA_REMOVE_ENCHANT                     ADDR(        0x2bdeb0,          0x100563fcc)
#define RVA_REBUILD_ENCHANT_FLAGS              ADDR(        0x2bfac0,          0x1005657e4)
#define RVA_LOAD_GAME_ENCHANT_FLAGS            ADDR(        0x186bb2,          0x1004ee134)
#define RVA_SQUARE_FLAGS                       ADDR(        0xa20ad48,         0x10d011d20)
#define RVA_FREE                               ADDR(        0x05e8c0,          0x1003340cc)
#define RVA_MESSAGE_ATTACH                     ADDR(        WIN_INLINED,       0x1004c5758)
#define RVA_MESSAGE_DETACH                     ADDR(        0x1e0c10,          MAC_INLINED)
#define RVA_BATTLE_ATTACH_CALL                 ADDR(        WIN_NO_CALL,       0x1003de584)
#define RVA_BATTLE_REPLAY_WRITE                ADDR(        0x0f55e0,          0x1003dd18c)
#define RVA_REPLAY_WRITE_FCLOSE_CALL           ADDR_DIFFERS(0x0f1a75,         0x1003dd48c)
#define RVA_SAVE_BATTLE_REPLAY_CALL            ADDR(        0x0f55f4,         MAC_INLINED)
#define RVA_READ_FILE_ALLOC                    ADDR(        0x059000,         0x100335e18)
#define RVA_READ_FILE_ALLOC_SIZE               ADDR(        0x123e8be8,       0x1138b58b0)
#define RVA_WRITE_FILE_BYTES                   ADDR(        WIN_INLINED,      0x1003362bc)
#define RVA_SAVE_ALL_UNITS_TEMP                ADDR(        0x18d9d0,         MAC_INLINED)
#define RVA_REPLAY_PARK_UNITS_CALL             ADDR_DIFFERS(0x1e259d,         0x1004c3418)
#define RVA_REPLAY_LOAD_CALL                   ADDR(        0x1e25a9,         MAC_INLINED)
#define RVA_REPLAY_FLAG_SET                    ADDR_DIFFERS(0x0f3f82,         0x1004c38e4)
#define RVA_REPLAY_LOAD_UNIT_CALL              ADDR(        0x0f408f,         0x1004c39ec)
#define RVA_REPLAY_RELOAD_CALL                 ADDR(        0x1e25b3,         0x1004c3ca0)
#define RVA_SAVE_FILE_VERSION                  ADDR(        0x5822fa8,         0x10857c9a0)
#define RVA_BATTLE_SEED                        ADDR(        0x11fb18a0,        0x108564670)
#define RVA_BATTLE_PLAYER_A                    ADDR(        0x43e234,          0x101cce59c)
#define RVA_BATTLE_PLAYER_B                    ADDR(        0x43e26c,          0x101cce598)
#define RVA_BATTLE_X                           ADDR(        0x43e014,          0x104904c84)
#define RVA_BATTLE_Y                           ADDR(        0x43dfc4,          0x104904c88)
#define RVA_FILE_WRITE_INT64                   ADDR(        0x058490,          MAC_INLINED)
#define RVA_FILE_READ_INT64                    ADDR(        0x0589a0,          MAC_INLINED)
#define RVA_FILE_WRITE_ENDED_SHORTS            ADDR(        WIN_INLINED,       MAC_INLINED)
#define RVA_FILE_READ_ENDED_SHORTS             ADDR(        WIN_INLINED,       MAC_INLINED)
#define REPLAY_MAGIC                           0x775615
#define REPLAY_VERSION                         0xcb
#define PART_STATS_MAGIC                       0x77491
#define PART_STATS_VERSION_MAX                 1
#define PART_STATS_RECORD_SIZE                 16
#define SAVED_UNIT_SIZE                        270
#define SAVED_UNIT_KNOWN_SPELLS                0x20
#define SAVED_UNIT_MEMORIZED_SPELLS            0x98
#define SAVED_UNIT_ITEMS                       0xd4
#define SAVED_UNIT_MONSTER                     0xe8
#define SAVED_UNIT_TEMP_ABILITY_IDS            0xee
#define SAVED_UNIT_TEMP_ABILITY_VALUES         0xf6
#define SAVED_COMMANDER_AFTER_NAME             0x20
#define SAVED_SQUARE_BEFORE_CORPSES            0x2e
#define SAVED_SQUARE_AFTER_CORPSES             0xcd

#define RVA_MAP_FILES                          ADDR(        0xe77d260,         0x10c127aa8)
#define RVA_MAP_FILE_COUNT                     ADDR(        0x1a95dc4,         0x1008735d4)
#define RVA_TRANSLATION_HASHES                 ADDR(        0x58ffed0,         0x105b8fc50)
#define RVA_TRANSLATION_STRINGS                ADDR(        0x57c6310,         0x105b68b50)
#define RVA_TRANSLATION_COUNT                  ADDR(        0x435d58,          0x100857670)
#define RVA_TRANSLATION_HASH                   ADDR(        0x05c480,          MAC_INLINED)
#define RVA_TRANSLATION_NORMALIZE              ADDR(        0x05c660,          MAC_INLINED)
#define RVA_DUMPSTRINGS_FILE                   ADDR(        0x57eddf0,         0x105b68188)

#define TERRAIN_GEMS                           0x4c
#define MOD_RECORD_PRIO                        0xcd2
#define ITEM_COMBATSUM_MODE                    0x100
#define ITEM_COMBATSPELL_MODE                  0x144
#define COMMANDER_INSANITY                     0x10
#define COMMANDER_REPEAT_RITUAL                0x14
#define ENCHANT_FLAGS                          0x1c
#define ENCHANT_TERRAIN                        0x24
#define SQUARE_FIRE                            0x24
#define SQUARE_DRAIN                           0x40

// ---- Rituals and the map ------------------------------------------------------------------------
#define RVA_RITUAL_SCREEN                      ADDR(        0x2ae180,          0x10035e1d0)
#define RVA_RITUAL_CAN_CAST                    ADDR(        0x2c9970,          0x10055bcfc)
#define RVA_AI_RITUAL_CAN_CAST_CALL_A          ADDR(        0x06b11c,          0x1004dd1e0)
#define RVA_AI_RITUAL_CAN_CAST_CALL_B          ADDR(        0x06bb7b,          0x1004de014)
#define RVA_AI_RITUAL_CAN_CAST_CALL_C          ADDR(        0x06be49,          0x1004de1d8)
#define RVA_AI_RITUAL_CAN_CAST_CALL_D          ADDR(        0x06c1b8,          0x1004de4a8)
#define RVA_REPEAT_RITUAL_CAN_CAST_CALL        ADDR(        0x2c1dd4,          0x1005664d0)
#define RVA_RITUAL_LOCATION_CHECK              ADDR(        0x2ccea0,          0x1005596bc)
#define RVA_RITUAL_ROW                         ADDR_DIFFERS(0x2d6690,          0x100351b44)
#define RITUAL_COST_TYPE_MAX                   50
#define RVA_RESOURCE_NAMES                     ADDR(        0x3f9e90,          0x1008482b8)
#define RVA_INCOME_NAMES                       ADDR(        0x344360,          0x100847b68)
#define INCOME_NAME_COUNT                      19
#define RVA_RITUAL_CHECK_COSTS                 ADDR(        0x2c3320,          0x100558528)
#define RVA_RITUAL_PAY_COSTS                   ADDR(        0x2c2510,          0x100558950)
#define RVA_RESOURCE_AVAILABLE                 ADDR(        0x31fdd0,          0x1005693a4)
#define RVA_CAN_AFFORD_AT                      ADDR(        0x31fee0,          0x100569508)
#define RVA_PAY_RESOURCE_AT                    ADDR(        0x320010,          0x100569614)
#define RVA_COUNT_EXPENDABLE_AT                ADDR(        0x2b5990,          0x100557b7c)
#define RVA_COUNT_ABILITY_AT                   ADDR(        0x2c0a60,          0x100558034)
#define RVA_REMOVE_ABILITY_UNITS_AT            ADDR(        0x2b5610,          0x100558200)
#define RVA_CORPSES_AT                         ADDR(        0x335090,          MAC_INLINED)
#define RVA_UNIT_LEAVE_CORPSE                  ADDR(        0x0bfad0,          0x1003bed74)
#define RVA_DAMAGE_LEAVE_CORPSE_CALL           ADDR(        0x0bf64a,          0x1003bb034)
#define RVA_HP_LOSS_LEAVE_CORPSE_CALL          ADDR(        0x0bd28f,          0x1003a805c)
#define RVA_RAISE_DEAD                         ADDR(        0x10b390,          0x1004754e4)
#define RVA_RAISE_ANIMALS                      ADDR(        0x10a510,          0x10047642c)
#define RVA_RITUAL_RAISE_DEAD_CALL             ADDR(        0x2c8479,          0x100561ca8)
#define RVA_RITUAL_RAISE_ANIMALS_CALL          ADDR(        0x2c848d,          0x1005620dc)
#define RVA_RITUAL_PUT_CORPSES_TEXT_CALL       ADDR(        0x2c942e,          0x100562b7c)
#define RVA_PANEL_SENSEDEAD_TEXT_CALL          ADDR(        0x09a9da,          0x10037ef78)
#define RVA_RITUAL_SENSEDEAD_TEXT_CALL         ADDR(        0x2aea0a,          0x10035f8ec)
#define SENSEDEAD_TEXT_SIZE                    0xc8
#define RVA_INSANITY_ADD                       ADDR(        0x115a70,          MAC_INLINED)
#define SQUARE_CORPSES                         0x2a
#define SQUARE_CORPSE_KINDS                    8
#define MONSTER_ABILITY_EXPENDABLE             0x1ae
#define MONSTER_ABILITY_EXPENDABLE_2           0x4ca
#define MONSTER_ABILITY_GOBLIN                 0x4b3
#define RVA_COMMANDERS                         ADDR(        0x11d3c890,        0x10ce0cbb0)
#define COMMANDER_SIZE                         0xd4
#define COMMANDER_TABLE_MAX                    9990
#define COMMANDER_KNOWN_RITUALS_MAX            64
#define RVA_COMMANDER_NAMES                    ADDR(        0x11d3c860,        0x10ce0cb80)
#define COMMANDER_NAME_SIZE                    0x30
#define RVA_LAST_COMMANDER                    ADDR(        0x11d37a30,        0x10ce0cb7c)
#define RVA_COMMANDER_DONE                     ADDR(        0x11d3c932,        0x10ce0cc52)
#define RVA_COMMANDER_SET_DONE                 ADDR(        0x0a0c40,          MAC_INLINED)
#define RVA_FIND_NEXT_COMMANDER                ADDR(        0x0a0940,          0x100373d74)
#define RVA_SELECT_NEXT_COMMANDER              ADDR(        0x0a08a0,          0x100374068)
#define RVA_NEXT_COMMANDER_MODE                ADDR(        0x1a5428c,         0x100872af4)
#define RVA_SELECT_PREV_COMMANDER              ADDR(        0x0a0640,          MAC_INLINED)
#define RVA_FOLLOW_COMMANDER_PLANE             ADDR(        0x0a8e90,          0x10036d830)
#define RVA_DROP_DEAD_FROM_GROUP               ADDR(        0x1decc0,          0x100598e40)
#define RVA_COUNT_COMMANDERS_WITH_AP           ADDR(        0x09d8a0,          0x10037722c)
#define RVA_SELECTED_COMMANDER                 ADDR(        0xfb2cc28,         0x10cb0e18c)
#define RVA_SELECTED_COMMANDER_GROUP           ADDR(        0xfb2cc24,         0x10cb0e190)
#define COMMANDER_ORDER_GROUP                  0x4
#define COMMANDER_MOVE_ORDER_X                 0x8
#define COMMANDER_MOVE_ORDER_Y                 0xc
#define RVA_COMMANDER_UNIT                     ADDR(        0x12c1e0,          MAC_INLINED)
#define RVA_UNIT_X                             ADDR(        0xfb8e7f4,         0x110049894)
#define UNIT_SIZE                              0x158
#define RVA_UNIT_LEADER                        ADDR(        0xfb8e7f0,         0x110049890)
#define RVA_UNITS                              ADDR(        0xfb8e7e0,         0x110049880)
#define RVA_UNIT_MONSTER                       ADDR(        0xfb8e8c8,         0x110049968)
#define RVA_UNIT_COUNT                         ADDR(        0xfb8e7c0,         0x110049878)
#define RVA_UNIT_HP                            ADDR(        0xfb8e8c6,         0x110049966)
#define RVA_UNIT_PLAYER                        ADDR(        0xfb8e8ca,         0x11004996a)
#define RVA_UNIT_XP                            ADDR(        0xfb8e8c4,         0x110049964)
#define RVA_UNIT_KILLS                         ADDR(        0xfb8e8cc,         0x11004996c)
#define UNIT_COMMANDER                         0x10
#define UNIT_TABLE_MAX                         99000
#define MONSTER_FLAG_NO_COMMANDER              35
#define MONSTER_ABILITY_UNIQUE                 0x39
#define RVA_NEW_UNIT                           ADDR(        0x1297b0,          0x10046e828)
#define RVA_NEW_COMMANDER                      ADDR(        0x128160,          0x10046da30)
#define RVA_ALLOC_UNIT                         ADDR(        0x10fff0,          0x10046d184)
#define RVA_ALLOC_TEMP_UNIT                    ADDR(        0x110410,          MAC_INLINED)
#define RVA_RESERVED_UNIT                      ADDR(        0x1f1ef0,          MAC_INLINED)
#define RVA_RESERVED_COMMANDER                 ADDR(        WIN_INLINED,       MAC_INLINED)
#define RVA_RESERVE_UNITS                      ADDR(        0x1e8630,        0x10050bfc4)
#define RVA_RESERVE_COMMANDERS                 ADDR(        0x1e8420,        0x10050c1b4)
#define RVA_RESERVE_FOR_ALL                    ADDR(        0x1f3140,        0x10050c45c)
#define RVA_2H_UNIT_INCLUDED                   ADDR(        0x1853a0,        MAC_INLINED)
#define RVA_TEMP_FILES_REMOVE                  ADDR(        0x191060,        0x10032b438)
#define RVA_CLIENT_TMP_MYTURN_NAME             ADDR(        0x1ea38b,        0x10050b97c)
#define RVA_CLIENT_NO_SLOT_EXIT                ADDR(        0x1ea76e,        0x10050bec4)
#define SLOT_CONTROLLER_MINE                   5
#define RVA_NET_SEND_RICHES                    ADDR(        0x1f2370,        0x10050e75c)
#define RVA_NET_SEND_KILL_UNIT                 ADDR(        0x1e6fb0,        0x10050e384)
#define RVA_NET_RECV_TIMER                     ADDR(        0x1ea930,        MAC_INLINED)
#define RVA_NET_WAITING_FOR_TURN               ADDR_DIFFERS(0x5835668,         0x10880c9e0)
#define RVA_TURN_TIMER_MS                      ADDR(        0x583566c,       0x10880c9e4)
#define RVA_TURN_TIMER_LEFT                    ADDR(        0x5835670,       0x10880c9e8)
#define RVA_TIMER_TICK_READ                    ADDR(        0x24089b,        0x10048e394)
#define RVA_MAP_SCREEN_TIME_UP_READ            ADDR(        0x9715c,         0x10037fec8)
#define RVA_GAME_MENU_CLIENT_TEST              ADDR_DIFFERS(0x124193, 0x1003803e4)
#define RVA_REBUILD_SQUARE_ARMY                ADDR(        0x18f440,          0x1004effc0)
#define RVA_UNIT_HOLDS_SQUARE                  ADDR(        0x31fa40,          MAC_INLINED)
#define RVA_UNIT_FIT_TERRAIN                   ADDR(        0x114e50,          0x100467450)
#define RVA_CONQUER_CHECK                      ADDR(        0x1af950,          0x1004ab1b0)
#define RVA_SQUARE_STRONGEST_OWNER             ADDR(        0x1042b0,          0x100465b54)
#define RVA_UPDATE_SIGHT                       ADDR(        0x19a590,          0x10049f820)
#define RVA_UPDATE_SIGHT_AROUND                ADDR(        0x1aeba0,          MAC_INLINED)
#define RVA_RESET_SQUARE_CACHE                 ADDR(        0x1b3210,          0x100498e4c)
#define RVA_SIGHT_RAYS                         ADDR(        0x19bc40,          MAC_INLINED)
#define RVA_SIGHT_BOX                          ADDR(        0x1adaa0,          0x1004a5fdc)
#define RVA_SIGHT_SOURCE_NEAR                  ADDR(        0x19b840,          MAC_INLINED)
#define RVA_MARK_SQUARE_IN_VIEW                ADDR(        0x1b0ad0,          MAC_INLINED)
#define RVA_UPDATE_TRUE_TERRAIN_SEEN           ADDR(        WIN_NOT_FOUND,     MAC_NOT_FOUND)
#define RVA_COPY_TEAM_SIGHT                    ADDR(        WIN_NOT_FOUND,     MAC_NOT_FOUND)
#define RVA_REVEAL_MAP_CALL                    ADDR(        0x10d0bd,          0x10047445c)
#define RVA_PLANE_DISCOVERED_PASS              ADDR(        0x253320,          MAC_INLINED)
#define RVA_PLANE_DISCOVERED_NEXT_CALL         ADDR(        0x16666e,          0x1003b72bc)
#define RVA_DEAD_UNITS_PASS                    ADDR_DIFFERS(0x1809c0,          0x1004ea3ac)
#define RVA_REVEAL_MAP_FOR                     ADDR(        0x1b0550,          0x1004a614c)
#define RVA_SIGHT_CACHE_RESET                  ADDR(        WIN_INLINED,       MAC_INLINED)
#define RVA_SQUARE_SEEN_AT                     ADDR(        0x335360,          MAC_INLINED)
#define RVA_SEEN_NEIGHBOURS_COUNT              ADDR(        WIN_NOT_FOUND,     MAC_NOT_FOUND)
#define RVA_SQUARE_SIGHT_LEVEL                 ADDR(        WIN_INLINED,       MAC_INLINED)
#define RVA_UNIT_VISIBLE_TO                    ADDR(        0x117c10,          0x100467d04)
#define RVA_UNIT_STEALTH_LEVEL                 ADDR(        0x31f520,          MAC_INLINED)
#define RVA_UNIT_BLINDNESS                     ADDR(        0x324bb0,          0x10056717c)
#define RVA_FIRST_TEAM_UNIT_AT                 ADDR(        0x104b40,          MAC_INLINED)
#define RVA_PLAYER_UNIT_AT                     ADDR(        0x104c30,          0x100468070)
#define RVA_PLAYER_HAS_UNIT_AT                 ADDR(        0x31f460,          0x100569b00)
#define RVA_SQUARE_TEAM_ABILITY                ADDR(        0x112230,          MAC_INLINED)
#define RVA_SQUARE_TEAM_ABILITY_B              ADDR(        0x112750,          0x100469224)
#define RVA_SQUARE_TERRAIN_FLAGS               ADDR(        0x334f80,          MAC_INLINED)
#define RVA_SQUARE_MARK_BOX                    ADDR(        0x19bb60,          MAC_INLINED)
#define RVA_SQUARE_MARK_DRAW                   RVA_MAP_DRAW_PLANE
#define RVA_MONSTER_VARIANT                    ADDR(        0x108130,          0x100479640)
#define RVA_RECRUIT_BUY                        ADDR(        0x107b80,          0x10047982c)
#define RVA_POPULATE                           ADDR(        0x26be10,          0x1003f855c)
#define RVA_POPULATE_NO_COMMANDER              ADDR(        0x272b00,          MAC_INLINED)
#define RVA_POPULATE_COUNT_FIRST               ADDR(        WIN_INLINED,       MAC_INLINED)
#define RVA_POPULATE_COMMANDER                 ADDR(        0x272850,          0x1003f8a04)
#define RVA_SUMMON_STRING                      ADDR(        0x171a40,          0x10053e598)
#define RVA_SUMMON_STRING_FULL                 ADDR(        0x178260,          MAC_INLINED)
#define RVA_LAST_SPAWNED_UNIT                  ADDR(        0x1aeb984,         0x101cb6e28)
#define RVA_LAST_SPAWNED_COMMANDER             ADDR(        0x1ae8f0c,         0x100872b50)
#define RVA_NEW_WORLD_POPULATE_CALL            ADDR(        0x304dd9,          0x10058c614)
#define RVA_NEW_WORLD_POP_CELESTIAL_CALL       ADDR(        0x304dec,          MAC_NO_CALL)
#define RVA_NEW_WORLD_POP_SKY_CALL             ADDR(        0x304dff,          0x10058e218)
#define RVA_NEW_WORLD_POP_AGARTHA_CALL         ADDR(        0x304e12,          MAC_NO_CALL)
#define RVA_NEW_WORLD_POP_INFERNO_CALL         ADDR(        0x304e25,          0x100590344)
#define RVA_NEW_WORLD_POP_HADES_CALL           ADDR(        0x304e38,          0x100590358)
#define RVA_NEW_WORLD_POP_AZTLAN_CALL          ADDR(        0x304e4b,          0x10059036c)
#define RVA_NEW_WORLD_POP_PRIMAL_CALL          ADDR(        0x304e5e,          0x100590380)
#define RVA_NEW_WORLD_POP_ELEMENTAL_CALL       ADDR(        0x304e71,          0x100590394)
#define RVA_NEW_WORLD_POP_VOID_CALL            ADDR(        0x304e84,          MAC_NO_CALL)
#define RVA_NEW_WORLD_POP_AFTER_VOID_CALL      ADDR_DIFFERS(0x304e84,          0x100590c18)
#define RVA_POPULATE_ELYSIUM                   ADDR(        0x2552a0,          0x10040c080)
#define RVA_POPULATE_ELYSIUM_CASE_CHECK        ADDR(        0x255468,          0x10040c2a8)
#define RVA_POPULATE_ELYSIUM_CASES             ADDR_DIFFERS(0x25a544,          0x100758cae)
#define RVA_POPULATE_CELESTIAL                 ADDR(        0x2799f0,          MAC_INLINED)
#define RVA_POPULATE_SKY                       ADDR(        0x299440,          0x100451888)
#define RVA_POPULATE_AGARTHA                   ADDR(        0x272b20,          MAC_INLINED)
#define RVA_POPULATE_INFERNO                   ADDR(        0x28b920,          0x10043ad34)
#define RVA_POPULATE_HADES                     ADDR(        0x27bc20,          0x100446780)
#define RVA_POPULATE_AZTLAN                    ADDR(        0x274c30,          0x10045d280)
#define RVA_POPULATE_PRIMAL                    ADDR(        0x293340,          0x10045873c)
#define RVA_POPULATE_ELEMENTAL                 ADDR(        0x284a90,          0x10042dd44)
#define RVA_POPULATE_VOID                      ADDR(        0x29e590,          MAC_INLINED)
#define RVA_POPULATE_VOID_POOL                 ADDR(        0x29f610,          0x100450540)
#define RVA_POPULATE_WANDERERS                 ADDR(        0x25a8c0,          MAC_INLINED)
#define RVA_POPULATE_WANDERER_SQUARE           ADDR(        0x2ace60,          MAC_INLINED)
#define RVA_POPULATE_WANDERER_SQUARE_CALL      ADDR(        0x25a392,          0x10042b9c4)
#define RVA_RANDOM_RITUAL_SQUARE_EMPTY_TEST    ADDR(        0x2ad22e,          0x1004b4048)
#define RVA_POPULATE_ELYSIUM_FILTER            ADDR(        0x255413,          0x10040c264)
#define RVA_POPULATE_CELESTIAL_FILTER          ADDR(        0x279ac8,          MAC_INLINED)
#define RVA_POPULATE_SKY_FILTER                ADDR(        0x29953b,          0x1004519bc)
#define RVA_POPULATE_AGARTHA_FILTER            ADDR(        0x272c06,          MAC_INLINED)
#define RVA_POPULATE_INFERNO_FILTER            ADDR(        0x28ba0d,          0x10043af04)
#define RVA_POPULATE_INFERNO_CASES             ADDR_DIFFERS(0x291688,          0x10075950c)
#define RVA_POPULATE_INFERNO_BELPHEGOR         ADDR(        0x28c37e,          0x10043b444)
#define RVA_POPULATE_INFERNO_SIN               ADDR(        0x28d434,          0x10043b4c0)
#define RVA_POPULATE_INFERNO_ABADDON           ADDR(        0x28e753,          0x10043c20c)
#define RVA_POPULATE_INFERNO_BUER              ADDR(        0x28f6c8,          0x10043b600)
#define RVA_POPULATE_INFERNO_GERYON            ADDR(        0x290605,          0x10043c28c)
#define RVA_POPULATE_INFERNO_CAPITOLIUM        ADDR(        0x290a4f,          0x10043c388)
#define RVA_POPULATE_INFERNO_MINE_POOL         ADDR(        0x2917e0,          0x10043aa68)
#define RVA_POPULATE_INFERNO_DEVIL_POOL        ADDR(        0x292190,          MAC_INLINED)
#define RVA_POPULATE_INFERNO_VILLAGE_POOL      ADDR(        0x2924c0,          0x10043a714)
#define RVA_POPULATE_INFERNO_OVEN_POOL         ADDR(        0x2926c0,          MAC_INLINED)
#define RVA_POPULATE_INFERNO_SINNERS_POOL      ADDR(        0x2929f0,          MAC_INLINED)
#define RVA_POPULATE_INFERNO_CITY_POOL         ADDR(        0x292d90,          0x1004397d0)
#define RVA_POPULATE_HADES_CITADEL             ADDR(        0x27da91,          0x1004471fc)
#define RVA_POPULATE_HADES_FILTER              ADDR(        0x27bd2a,          0x1004468c8)
#define RVA_POPULATE_AZTLAN_FILTER             ADDR(        0x274d12,          0x10045d3ac)
#define RVA_POPULATE_PRIMAL_FILTER             ADDR(        0x293436,          0x10045886c)
#define RVA_POPULATE_ELEMENTAL_FILTER          ADDR(        0x284b86,          0x10042dea4)
#define RVA_POPULATE_NEXUS_FILTER              ADDR(        0x285015,          0x1004350e4)
#define RVA_POPULATE_VOID_FILTER               ADDR(        0x29e682,          MAC_INLINED)
#define RVA_POPULATE_SET_HOLDIT                ADDR(        0x26c880,          0x1003f88dc)
#define RVA_POPULATE_ITEM_CHANCE               ADDR(        0x26c1c0,          MAC_INLINED)
#define RVA_POPULATE_FOLLOWERS                 ADDR(        0x26c390,          0x1003f8c44)
#define RVA_POPULATE_FUNGUS                    ADDR(        0x26c5a0,          0x1003f9140)
#define RVA_POPULATE_COMMANDERS                ADDR(        0x272590,          MAC_INLINED)
#define RVA_POPULATE_IMPERIAL_STATUE           ADDR(        0x25a730,          0x10040be4c)
#define RVA_SQUARE_COLD_TEST                   ADDR(        0x1b0c10,          MAC_INLINED)
#define RVA_DIE_OPEN_ENDED                     ADDR(        0x058ee0,          0x1003366f4)
#define RVA_DICE_OPEN_ENDED                    ADDR(        0x058dc0,          0x100336848)
#define RVA_DICE                               ADDR(        0x058b30,          MAC_INLINED)
#define RVA_EXECUTE_RITUAL_STRING_PICK         ADDR(        0x2c5c02,          MAC_NO_CALL)
#define RVA_EXECUTE_RITUAL_SUMMON_CALL         ADDR(        0x2c6381,          0x100560bc8)
#define RVA_EXECUTE_RITUAL_FARSUMMON_CALL      ADDR(        0x2c6432,          0x10055f804)
#define RVA_RITUAL_SUMMON_STRING               ADDR(        0x2b3100,          0x10055cb70)
#define RVA_RITUAL_SUMMON_PERCENT_CURVE        ADDR(        0x2b3257,          0x10055ccd4)
#define RVA_RITUAL_SUMMON_CONTROL_ROLL         ADDR(        WIN_INLINED,       MAC_INLINED)
#define RVA_RITUAL_SUMMON_UNITS                ADDR(        0x2d5db0,          0x100357d34)
#define RVA_RITUAL_SUMMON_CONTROL_CHECK        ADDR(        0x2d63c5,          MAC_NO_CALL)
#define RVA_RITUAL_FAR_SUMMON_UNITS            ADDR(        0x2d7210,          0x100356dd0)
#define RVA_RANDOM_OPEN_DICE                   ADDR(        0x058b90,          0x100336b4c)
#define RVA_RITUAL_SUMMON_PART_TRACE_CALL      ADDR(        0x2b3375,          0x10055ce1c)
#define RVA_RITUAL_SUMMON_DICE_CALL            ADDR(        0x2b38dc,          0x10055cfd8)
#define RVA_RITUAL_STRING_COPY_CALL            ADDR(        0x2c5d9a,          0x10055ebec)
#define RVA_STRING_COPY                        ADDR_DIFFERS(0x05e7d0,          0x1005ae194)
#define RVA_GET_START_RITUALS                  ADDR(        0x2bfe30,          0x1005540f0)
#define RVA_GET_START_RITUALS_CONTROLLER       ADDR(        0x2bff10,          0x100554218)
#define RVA_START_RITUAL_SLOTS                 ADDR(        0x406f40,          0x10071e4d0)
#define RVA_RITUAL_MAY_LEARN                   ADDR(        0x2bc550,          0x1005534f8)
#define RVA_COMMANDER_LEARN_RITUAL             ADDR(        0x2bddb0,          MAC_INLINED)
#define RVA_COMMANDER_KNOWS_RITUAL             ADDR(        0x2be040,          MAC_INLINED)
#define RVA_LEARN_LEVEL_RITUALS                ADDR(        0x2bfc60,          0x1005548dc)
#define RVA_LEARN_NEW_SCHOOL_RITUALS           ADDR(        0x2bc220,          0x100554d94)
#define RVA_RANDOM_MONSTER_WITH_ABILITY        ADDR(        0x1d8db0,          0x10052866c)
#define RVA_RITUAL_NEWRIT_CALL                 ADDR(        0x2c669a,          0x10055f9d0)
#define RVA_RITUAL_GAINRIT_CALL                ADDR(        0x2c84bf,          MAC_NO_CALL)
#define RVA_RITUAL_TRANSFORMTARG               ADDR(        0x2c7fa2,          0x100561b18)
#define RVA_LIBRARY_MASTERY_LEARN_CALL         ADDR(        0x2de3e8,          0x10035d94c)
#define RVA_LIBRARY_MASTERY_COST               ADDR(        0x2d06e0,          MAC_INLINED)
#define RVA_PROMOTE_GET_START_RITUALS_CALL     ADDR(        0x10fc1c,          0x10046d8f8)
#define RVA_NEW_COMMANDER_START_RITUALS_CALL   ADDR(        0x1283d5,          0x10046e298)
#define RITUAL_FLAG_FREE                       0x1
#define RITUAL_FLAG_NOSTART                    0x2
#define RITUAL_FLAG_NODEMON                    0x4
#define RITUAL_FLAG_RARESTART                  0x8
#define RITUAL_FLAG_NOTFORPOOR                 0x10
#define RITUAL_FLAG_START                      0x20
#define RITUAL_FLAG_NEXTTOO                    0x40
#define RITUAL_FLAG_NOUNDEAD                   0x80
#define RITUAL_FLAG_NOFEMALE                   0x100
#define RITUAL_FLAG_AFTERPREV                  0x200
#define RITUAL_FX_NEWRIT                       904
#define RITUAL_FX_TRANSFORMTARG                914
#define RITUAL_FX_GAINRIT                      916
#define RITUAL_FX_LEVELUP                      944
#define RITUAL_FX_LEVELUPMON                   945
#define RITUAL_FX_NOLEARNFX                    963
#define RITUAL_FX_PROMOTION                    974
#define RITUAL_FX_AIRARE                       1102
#define MONSTER_ABILITY_ALLRIT                 0x62
#define MONSTER_ABILITY_MASTERY                0x32b
#define MONSTER_ABILITY_LEVELUPMON_OK          0x32c
#define MONSTER_ABILITY_LIBMASTERY             0x32d
#define MONSTER_ABILITY_MONTAG_MASTERY         0x32e
#define MONSTER_ABILITY_MONTAG                 0x320
#define RVA_EXECUTE_RITUAL_SIGHT_CALL          ADDR_DIFFERS(0x2c98f0,          0x100562d9c)
#define RVA_UPDATE_SIGHT_WHOLE_MAP             ADDR(        0x1b1470,          MAC_INLINED)
#define RVA_UPDATE_SIGHT_HUMAN_CONTROLLER      ADDR(        0x19b1b0,          0x10049facc)
#define RVA_UPDATE_DRAW_SQUARES                ADDR(        0x1b21a0,          0x1004a4558)
#define RVA_CAST_RITUAL_BY_NAME                ADDR(        0x2bde30,          0x10055d650)
#define RVA_RITUAL_SCREEN_EXECUTE_CALL         ADDR(        0x2b0201,          0x100361060)
#define RVA_REPEAT_RITUAL_EXECUTE_CALL         ADDR(        0x2c1f36,          0x100566554)
#define RVA_REPEAT_RITUAL_EXECUTE_CALL_B       ADDR(        WIN_NO_EQUIVALENT, 0x100566624)
#define RVA_REPEAT_RITUAL_EXECUTE_CALL_C       ADDR(        WIN_NO_EQUIVALENT, 0x1005666f4)
#define RVA_REPEAT_RITUAL_EXECUTE_CALL_D       ADDR(        WIN_NO_EQUIVALENT, 0x100566794)
#define RVA_CAST_BY_NAME_EXECUTE_CALL          ADDR(        0x2bde8e,          0x10055d6ec)
#define RVA_ELYSIUM_EXECUTE_CALL_A             ADDR(        0x25593b,          0x10041bbd8)
#define RVA_ELYSIUM_EXECUTE_CALL_B             ADDR(        0x25598f,          0x10041da7c)
#define RVA_ELYSIUM_EXECUTE_CALL_C             ADDR(        0x255be7,          0x10041f758)
#define RVA_STARS_RELIABILITY                  ADDR(        0xf07b00c,         0x11307d7e0)
#define RVA_STARS_POWER                        ADDR(        0xf07b00e,         0x11307d7e2)
#define RVA_STARS_MONTHS                       ADDR(        0xf07b010,         0x11307d7e4)
#define RVA_STARS_FLAGS                        ADDR(        0xf07b014,         0x11307d7e8)
#define RVA_STARS_READING_TEXT                 ADDR(        0x2dc820,          0x10035dc28)
#define RITUAL_FX_DEFCTRL                      901
#define RITUAL_FX_VARCOST                      902
#define RITUAL_FX_ASTROSUM                     908
#define RITUAL_FX_TERRBOOST                    948
#define RITUAL_FX_TERRSCALE50                  949
#define RITUAL_FX_SACRSCALE                    950
#define RITUAL_FX_SUM0CHANCE                   951
#define RITUAL_FX_SUM0SNOW                     956
#define RITUAL_FX_SETPLAYER                    961
#define RITUAL_FX_FAILPLAYER                   966
#define RITUAL_FX_PROMPLAYER                   1079
#define RVA_FAILPLAYER_CLAMP                   ADDR(        0x2c5fec,          0x10055f7a8)
#define RITUAL_FX_FARSUMMON                    968
#define MONSTER_ABILITY_CTRLCHANCE             0x321
#define RVA_UNIT_NO_COMMANDER_FLAG             ADDR(        0x1d96e0,          MAC_INLINED)
#define RVA_RITUAL_SUMMON_STRING               ADDR(        0x2b3100,          0x10055cb70)
#define RVA_RITUAL_SUMMON_CALL                 ADDR(        0x2c6381,          0x100560bc8)
#define RVA_RITUAL_FARSUMMON_CALL              ADDR(        0x2c6432,          0x10055f804)
#define RVA_SUMMON_TO_UNIT                     ADDR(        0x2d5db0,          0x100357d34)
#define RVA_SUMMON_TO_UNIT_LINK                ADDR(        0x2d6432,          0x100358620)
#define RVA_SUMMON_TO_UNIT_FAIL_24             ADDR(        0x2dc770,          MAC_NOT_FOUND)
#define RVA_SUMMON_AT_SQUARE                   ADDR(        0x2d7210,          0x100356dd0)
#define RVA_SUMMONED_UNIT_SETUP                ADDR(        0x2d3160,          0x100356a74)
#define RVA_FIND_MONSTER_RANDOM                ADDR(        0x1d9e30,          MAC_INLINED)
#define RVA_MONSTER_UNIT_FIRST                 ADDR(        0x106600,          MAC_INLINED)
#define RVA_RITUAL_PROMOTE_EXISTING            ADDR(        0x2b23d0,          0x1005632e8)
#define RVA_MAP_X_BOUND                        RVA_MAP_DEPTH
#define RVA_LAST_TERRAIN                       ADDR(        0x1b185b4,         0x1059bcc7c)
#define RVA_TERRAIN_NAME                       ADDR(        0x2be7e0,          0x100554e64)
#define RVA_TERRAIN_NAME_TEXT                  ADDR(        0x5835fb0,         0x10c81c120)
#define TERRAIN_NAME_TEXT_SIZE                 0x64
#define RVA_SQUARE_TERRAIN                     ADDR(        0x335040,          MAC_INLINED)
#define RVA_TERRAIN_MATCH                      ADDR(        0x2c40c0,          0x100554f18)
#define RVA_TERRAIN_NEAR_COUNT                 ADDR(        0x1999e0,          0x1004a9120)
#define RITUAL_FX_NONEARBY1REQ                 1003
#define RVA_RITUAL_STRING_DECODE               ADDR(        0x2b3c80,          0x10055afb4)
#define RVA_COUNT_MONSTERS_AT                  ADDR(        0x11ef00,          0x10046d394)
#define RVA_COUNT_MONSTERS_HOME_AT             ADDR(        0x106390,          MAC_INLINED)
#define RVA_COUNT_MONSTERS_ON_PLANE            ADDR(        0x106490,          0x10047edac)
#define RVA_COUNT_PLAYER_MONSTERS              ADDR(        0x128f30,          0x10047ef2c)
#define RVA_ENCHANT_COUNT                      ADDR(        0x2cf4f0,          0x1005562ac)
#define RVA_FIND_ENCHANT                       ADDR(        0x2c1790,          0x10055b258)
#define RVA_ENCHANT_FX_SUM                     ADDR(        0x2c14b0,          0x100565a68)
#define RVA_ENCHANT_COUNT_KINDS                ADDR(        0x2ccdb0,          MAC_NOT_LOOKED_UP)
#define RVA_TERRAIN_COUNT_IN_BOX               ADDR(        0x334ee0,          MAC_NOT_LOOKED_UP)
#define RVA_ENCHANT_FIND                       ADDR(        0x2c1790,          0x10055b258)
#define RVA_SQUARE_FORT_PARTS                  ADDR(        0x12af40,          0x10047516c)
#define RVA_FORT_WORD_MATCH                    ADDR(        0x1c8380,          0x100539840)
#define RVA_FORT_WORD_CALLS                    ADDR(        0x1c508f,          0x100538668)
#define FORT_WORD_CALLS_SIZE                   0x100
#define RVA_FIND_UNIT_WITH_ABILITY_AT          ADDR(        0x116990,          0x100480e9c)
#define RVA_UNIT_RESISTANCES                   ADDR(        0x32a250,          0x100567474)
#define RVA_UNIT_BENEFITS                      ADDR(        0x320b30,          0x1005677e0)
#define RVA_COMMANDER_ABILITY_TOTAL            ADDR(        0x129090,          0x1004683e0)
#define RVA_COUNT_PLAYER_PORTALS               ADDR(        0x2513a0,          0x1005710ec)
#define RVA_SQUARE_PORTAL_AT                   ADDR(        0x241030,          MAC_INLINED)
#define SQUARE_PORTAL                          0x3e
#define RVA_AI_REVEAL_PORTAL_EXITS             ADDR(        0x2514f0,          0x10057175c)
#define RVA_TURN_START_REVEAL_PORTALS_CALL     ADDR(        0x1676be,          MAC_INLINED)
#define RVA_STEP_REVEAL_PORTALS_CALL           ADDR(        0x1dfc81,          0x10059ac40)
#define RVA_REVEAL_PORTAL_EXITS                ADDR(        0x242010,          0x1005716b8)
#define RVA_WAYTOGO_BACK_SEEN_READ             ADDR(        0x1db4a4,          0x10059c424)
#define RVA_SPREAD_PORTAL_SEEN_READ            ADDR(        0x1dbc60,          MAC_INLINED)
#define RVA_FAR_CHECK_SEEN_READ                ADDR(        0x1dbede,          0x10059bb00)
#define RVA_FAR_CHECK_IN_VIEW_READ             ADDR(        0x1dbf44,          0x10059bbe0)
#define RVA_STEP_PORTAL_SEEN_READ              ADDR(        0x1df725,          0x10059a5a8)
#define RVA_RANDOM_SQUARE_SEEN_READ            ADDR(        0x1234b3,          0x100469b24)
#define RVA_WANDERER_TRUE_SEEN_READ            ADDR(        0x169f3d,          0x1005485bc)
#define RVA_WANDERER_TRUE_SEEN_READ_B          ADDR(        0x16a572,          0x100548a14)
#define RVA_TOP_UNIT_SEEN_READ                 ADDR(        0x1b1a07,          0x10049909c)
#define RVA_TOP_UNIT_SEEN_READ_B               ADDR(        0x1b1a2f,          0x1004990c0)
#define RVA_RITUAL_PORTAL_SEEN_WRITE           ADDR(        0x2c6e6c,          0x100560324)
#define RVA_RITUAL_PORTAL_SEEN_WRITE_B         ADDR(        0x2c6fff,          0x10055f644)
#define RVA_RITUAL_PORTAL_SEEN_WRITE_C         ADDR(        0x2c702c,          0x10055f668)
#define RVA_SET_SEEN_DISC                      ADDR(        0x2dac20,          0x100359f28)
#define RVA_METEOR_SEEN_WRITE                  ADDR(        0x12dcd0,          MAC_INLINED)
#define RVA_UPDATE_VOID                        ADDR(        0x252710,          MAC_INLINED)
#define RVA_UPDATE_VOID_SEEN_CLEAR             ADDR_DIFFERS(0x2527f2,          0x1003b4ea4)
#define RVA_CLOUD_OWNER_MARK_TEST              ADDR(        0x12f808,          0x1003a56b4)
#define RVA_AUTO_RECRUIT_AGING_START           ADDR(        0x167033,          MAC_INLINED)
#define RVA_AUTO_RECRUIT_AGING_END             ADDR(        0x1670f9,          MAC_INLINED)
#define RVA_SITEPOP                            ADDR(        0x1724c0,          0x10053ec68)
#define RVA_SPAWN_CARRY                        ADDR(        0x5822f8c,         0x10c5e86bc)
#define RVA_MONTHLY_SITE_SPAWNS                ADDR(        0x173600,          0x10053fbe8)
#define RVA_MONTHLY_SITE_SPAWNS_CALL           ADDR(        0x16663c,          0x1003b51ec)
#define RVA_SITE_MAY_SPAWN                     ADDR(        0x178280,          0x10053f680)
#define RVA_SITEPOP_MODE                       ADDR(        0x171810,          0x10053ec68)
#define RVA_SITEPOP_PEDDLAR                    ADDR(        0x171400,          MAC_INLINED)
#define RVA_SITEPOP_GUARDS                     ADDR(        0x16f770,          0x10053f86c)
#define RVA_SITEPOP_MAUSOLEUM                  ADDR(        0x16e490,          MAC_INLINED)
#define RVA_SITEPOP_GRAVEYARD                  ADDR(        0x16ee00,          MAC_INLINED)
#define RVA_EVIL_TIMES_MONTHS                  ADDR(        0x5822f78,         0x10ce02ee4)
#define RVA_REMOVE_MONSTERS_AT                 ADDR(        0x0f6a50,          0x1003b8b94)
#define RVA_COUNT_TERRAIN_AROUND               ADDR(        0x32d000,          0x1004c9488)
#define PORTAL_INFERNO_GATE                    30
#define TERRAIN_MAGMA                          53
#define TERRAIN_FROZEN_WASTE                   54
#define TERRAIN_INFERNAL_WASTE                 55
#define TERRAIN_CODE_CITADEL                   (-23)
#define RVA_WANDER_ABILITY_TEST_29              ADDR(        0x17adb0,          0x10038bebc)
#define RVA_WANDER_ABILITY_TEST_24              ADDR(        0x17b0fd,          0x10038bef0)
#define RVA_WANDER_ABILITY_TEST_28              ADDR(        0x17b40a,          0x10038bf14)
#define RVA_WANDER_ABILITY_TEST_27              ADDR(        0x17b71a,          0x10038bf38)
#define RVA_WANDER_SECOND_TEST_29               ADDR(        0x17c70c,          0x10038c6c0)
#define RVA_WANDER_SECOND_TEST_24               ADDR(        0x17c718,          0x10038dd20)
#define RVA_WANDER_SECOND_TEST_28               ADDR_DIFFERS(0x17c724,          0x10038bdfc)
#define RVA_WANDER_SECOND_TEST_27               ADDR_DIFFERS(0x17c730,          0x10038be08)
#define RVA_WANDER_GATE_TEST                    ADDR_DIFFERS(0x17bde7,          0x10038c0a4)
#define RVA_WANDER_GATE_PASS                    ADDR(        0x17be2a,          0x10038c10c)
#define RVA_WANDER_GATE_WAIT                    ADDR(        0x17d1b8,          0x10038be54)
#define RVA_WANDERER_AT_TEST_24                 ADDR(        0x177c30,          0x100547bbc)
#define RVA_WANDERER_AT_TEST_28                 ADDR(        0x177ef7,          0x100547bf0)
#define RVA_WANDERER_AT_TEST_27                 ADDR(        0x1781b0,          0x100547c24)
#define RVA_WANDERER_AT_TEST_24_B               ADDR(        WIN_NO_EQUIVALENT, 0x100547ef0)
#define RVA_WANDERER_AT_TEST_28_B               ADDR(        WIN_NO_EQUIVALENT, 0x100547f24)
#define RVA_WANDERER_AT_TEST_27_B               ADDR(        WIN_NO_EQUIVALENT, 0x100547f58)
#define RVA_AI_VIEW_SKIP_TEST_24                ADDR(        0x06e4bf,          0x1004deba0)
#define RVA_AI_VIEW_SKIP_TEST_28                ADDR(        0x06e78f,          0x1004debec)
#define RVA_AI_VIEW_SKIP_TEST_27                ADDR(        0x06ea5f,          0x1004dec38)
#define RVA_TITHE_EXCEPTION_TEST                ADDR_DIFFERS(0x167705,          0x100392cc4)
#define RVA_TITHE_POOL                          ADDR(        0xf07b024,          0x11307d7f8)
#define RVA_NO_EVENTS                           ADDR_DIFFERS(0x58239a0,          0x1069b16ac)
#define RVA_MONUMENT_EVENT_TURN_TEST            ADDR_DIFFERS(0x162e50,          0x1003a435c)
#define RVA_RULERSHIP_CHECK                     ADDR(        0x10d140,          0x10046cc74)
#define RVA_RULERSHIP_SLOT                      ADDR(        0x44194c,           0x100872b98)
#define RVA_COUNT_UNITS_WITH_ABILITY            ADDR(        0x105fd0,          MAC_INLINED)
#define RVA_SQUARE_SHOWN_NAME                   ADDR(        0x1af6f0,          0x1004a9344)
#define RVA_CARAVAN_EVENT_TURN_TEST             ADDR_DIFFERS(0x165f04,          0x1003b2434)
#define RVA_HADES_SHIFT_MONTHS                 ADDR(        0x5822f84,        0x10ce02ed4)
#define RVA_ELEMENTAL_ANOMALY_MONTHS           ADDR(        0x5822f70,        0x10ce02ef0)
#define RVA_BUMPER_HARVEST_FLAG                ADDR(        0x5822f74,        0x10ce02ed8)
#define RVA_FUNGUS_YEAR_FLAG                   ADDR(        0x5822f7c,        0x10ce02eec)
#define RVA_MIST_COOLDOWN                      ADDR(        0x5822f88,        0x10ce02ee8)
#define RVA_EVENT_GATE_HADES_SHIFT             ADDR_DIFFERS(0x162e1d,          0x1003a4260)
#define RVA_EVENT_GATE_MONUMENT                ADDR(        0x162e71,          0x1003a42f0)
#define RVA_EVENT_GATE_FORETELLING             ADDR(        0x162fcf,          0x1003a4744)
#define RVA_EVENT_GATE_TAINTED_MAGIC           ADDR(        0x16338a,          0x1003a69b0)
#define RVA_EVENT_GATE_ELEMENTAL_ANOMALY       ADDR(        0x16340c,          0x1003a7660)
#define RVA_EVENT_GATE_HEAT_WAVE               ADDR_DIFFERS(0x163485,          0x1003a7794)
#define RVA_EVENT_GATE_BUMPER_HARVEST          ADDR(        0x1635b5,          0x1003a7848)
#define RVA_EVENT_GATE_FUNGUS_YEAR             ADDR(        0x16360b,          0x1003a7908)
#define RVA_EVENT_GATE_METEOR                  ADDR(        0x163662,          0x1003a7c54)
#define RVA_EVENT_GATE_INSANE_FISHERMAN        ADDR(        0x163790,          0x1003a8788)
#define RVA_EVENT_GATE_FISHERMAN_CUBE          ADDR(        0x163866,          0x1003a8930)
#define RVA_EVENT_GATE_DROUGHT                 ADDR(        0x1639df,          0x1003a8cf0)
#define RVA_EVENT_GATE_STRANGE_MIST            ADDR(        0x163a46,          0x1003a8e30)
#define RVA_EVENT_GATE_EVIL_TIMES              ADDR(        0x1641ca,          0x1003a9390)
#define RVA_EVENT_GATE_INFERNO_FIRST           ADDR(        0x164277,          0x1003a9b00)
#define RVA_EVENT_GATE_INFERNO_GATE            ADDR(        0x164296,          0x1003a9b7c)
#define RVA_EVENT_GATE_PRIMAL_FIRST            ADDR(        0x164562,          0x1003a9d20)
#define RVA_EVENT_GATE_PRIMAL_GATE             ADDR(        0x164581,          0x1003a9db4)
#define RVA_EVENT_GATE_GOLD_MINE               ADDR(        0x165c8f,          0x1003b0cc4)
#define RVA_EVENT_GATE_YOUNG_DVALA             ADDR(        0x165d06,          0x1003b0d90)
#define RVA_EVENT_GATE_MERCHANT                ADDR(        0x165f1e,          0x1003b23c0)
#define RVA_EVENT_GATE_MERCHANT_SKIP           ADDR_DIFFERS(0x165f0b,          0x1003b26b4)
#define RVA_EVENT_GATE_MERCHANT_SOCIETY        ADDR(        0x165f2d,          0x1003b2440)
#define RVA_HERO_TYPE_CLASS_HERO               ADDR_DIFFERS(0x089e3b,          0x1003939dc)
#define RVA_HERO_TYPE_UNEXPECTED               ADDR_DIFFERS(0x089ebd,          0x100393a74)
#define RVA_HERO_TYPE_UNEXPECTED_FLAG          ADDR(        WIN_NO_EQUIVALENT, 0x100393a78)
#define RVA_HERO_TYPE_WEAPON_MASTER            ADDR_DIFFERS(0x089ecf,          0x100393b04)
#define RVA_SITEPOP_GATE_ANT_HILLS             ADDR(        0x1769dc,          0x1005415a0)
#define RVA_SPAWN_HORROR_GROUP                  ADDR(        0x2a00e0,          0x10045017c)
#define RVA_SPAWN_HORROR_SINGLE                 ADDR(        0x2a0310,          MAC_INLINED)
#define RVA_HORROR_SINGLE_POPULATE_CALL         ADDR(        0x2a03df,          MAC_INLINED)
#define RVA_POPHORRORS_SINGLE_CALL              ADDR_DIFFERS(0x1425e8,          0x1003b8500)
#define RVA_POPHORRORS_GROUP_CALL               ADDR(        0x142784,          0x1003b88d8)
#define RVA_INDEP_AVOID_MOVER_TEST              ADDR(        0x16d3c9,          0x100547470)
#define RVA_INDEP_AVOID_TARGET_TEST             ADDR(        0x16d3ce,          0x100547478)
#define RVA_INDEP_AVOID_SKIP                    ADDR_DIFFERS(0x16d3e1,          0x100547494)
#define RVA_INDEP_AVOID_ALLOW                   ADDR_DIFFERS(0x16d3e1,          0x100547490)
#define RVA_INDEP_AVOID_REFUSE                  ADDR(        0x16d448,          0x100546df0)
#define RVA_EVENT_SQUARE_TEST_A                 ADDR(        0x162ee2,          0x1003a435c)
#define RVA_EVENT_SQUARE_TEST_B                 ADDR(        0x163040,          0x1003a47b0)
#define RVA_EVENT_SQUARE_TEST_C                 ADDR(        0x163d38,          0x1003a9e84)
#define RVA_EVENT_ARMY_TEST                     ADDR(        0x164056,          0x1003a96e8)
#define RVA_EVENT_SQUARE_TEST_D                 ADDR(        0x164081,          0x1003a972c)
#define RVA_EVENT_SQUARE_TEST_E                 ADDR(        0x165d52,          0x1003b0dd4)
#define RVA_METEOR_OWNER_TEST                   ADDR(        0x12d8d1,          MAC_IN_CODE)
#define RVA_RANDOM_SQUARE_OWNER_TEST            ADDR(        0x123557,          0x100469ba8)
#define RVA_CONQUER_SQUARE_OWNER_TEST           ADDR(        0x1ae6af,          0x1004aa88c)
#define RVA_WANDERER_TARGET_CITY_TEST           ADDR(        0x16c9ee,          0x100549e4c)
#define RVA_IMPERIAL_STATUE_TEST                ADDR(        0x25a85e,          0x10040bfa4)
#define RVA_MONUMENT_OWNER_TEST                 ADDR(        0x10d1ff,          0x10046ce6c)
#define RVA_GOD_VESSEL_OWNER_TEST               ADDR(        0x10d2af,          0x10046cf9c)
#define RVA_GOD_VESSEL_OWNER_TEST_B             ADDR(        0x10d345,          0x10046d0d0)
#define RVA_POPHORRORS_OWNER_TEST               ADDR(        0x1425ca,          0x1003b86b0)
#define RVA_HORRORS_OWNER_TEST_B                ADDR(        0x14296b,          0x1003a6788)
#define RVA_HORRORS_OWNER_TEST_C                ADDR(        0x142b4b,          MAC_NOT_FOUND)
#define RVA_SITEPOP_HADES_OWNER_TEST            ADDR(        0x172495,          MAC_NOT_FOUND)
#define RVA_PLANE_ABILITY_SUM                  ADDR(        0x251680,          0x10057501c)
#define ABILITY_MONUMENT                       0x8a
#define RVA_PORTALS_LINKED_TO                  ADDR(        0x250810,          0x1005707e0)
#define RVA_COMMANDER_FOLLOWERS_NAMED          ADDR(        0x105d10,          0x10047f098)
#define RVA_COMMANDER_RELEASE_FOLLOWERS        ADDR(        0x105c30,          MAC_INLINED)
#define RVA_UNIT_TRACE_NAME                    ADDR(        0x0f8f00,          0x1003bec88)
#define RVA_SQUARE_SACRIFICE_TAKEN             ADDR(        0x2db170,          0x1003561c0)
#define ABILITY_SINNER_CARRIER                 0x4c8
#define TERRAIN_DESTROYED_GATEWAY              200
#define TERRAIN_INFERNO_GATEWAY                201
#define TERRAIN_INFERNO_GATEWAY_EXIT           203
#define TERRAIN_CODE_INFERNAL_TARGET           (-14)
#define OBJECTIVE_INFERNAL_WANDERER            6
#define OBJECTIVE_HADES_WANDERER               7
#define RVA_RITUAL_LISTED                      ADDR(        0x2b4330,          MAC_INLINED)
#define RVA_GET_UNIT_HP                        ADDR(        0x328d80,          MAC_INLINED)
#define RVA_UNIT_HOME_X                        ADDR(        0xfb8e8da,         0x11004997a)
#define RITUAL_FX_LEVELREQ                     905
#define RITUAL_FX_RESRESTRICT                  912
#define RITUAL_FX_RESREQ                       913
#define RITUAL_FX_BENRESTRICT                  924
#define RITUAL_FX_NOMONREQ                     946
#define RITUAL_FX_NOMONHOMEREQ                 947
#define RITUAL_FX_FXREQ                        962
#define RITUAL_FX_NOFXHEREREQ                  973
#define RITUAL_FX_ENCHHEREREQ                  980
#define RITUAL_FX_NOFORTREQ                    986
#define RITUAL_FX_FORTREQ                      987
#define RITUAL_FX_HASPORTALREQ                 993
#define RITUAL_FX_NOPORTALREQ                  998
#define RITUAL_FX_NOMONWORLDREQ                1015
#define RITUAL_FX_NOMONPLAYERREQ               1016
#define RITUAL_FX_NOMONMASTERYREQ              1017
#define RITUAL_FX_NOMONELYSIUMREQ              1018
#define RITUAL_FX_NOPLANEREQ                   1034
#define RITUAL_FX_HOMEREQ                      1060
#define RITUAL_FX_FEWMONREQ                    1061
#define RITUAL_FX_MONWORLDREQ                  1062
#define RITUAL_FX_MONPLAYERREQ                 1063
#define RITUAL_FX_MINMONREQ                    1071
#define RITUAL_FX_EVENTVARREQ                  1072
#define RITUAL_FX_NOEVENTVARREQ                1073
#define RITUAL_FX_HOMECITREQ                   1074
#define RITUAL_FX_SEASONREQ                    1085
#define RVA_FORMAT_AREA_SHAPE                  ADDR(        0x1cf3e0,          0x100520d88)

#define RVA_EVENT_VAR_CHANGED                  ADDR(        0x1e6630,          MAC_INLINED)
#define RVA_KILLTARG_REMOVE_SEND               ADDR(        0x1e6fb0,          MAC_NOT_FOUND)
#define RVA_RITUAL_EFFECT_VALUE                ADDR(        0x2b4f10,          MAC_NOT_FOUND)
#define RVA_RITUAL_COST_PERCENT                ADDR(        0x2b5ea0,          0x1005574e4)
#define RVA_ANIMAL_SUMMONING                   ADDR(        0x2b7860,          0x1005567a4)
#define RVA_COMMANDER_FORGET_RITUAL            ADDR(        0x2bdc80,          0x100552f34)
#define RVA_GUILD_ENCHANT_SPAWN                ADDR(        0x2bea20,          MAC_INLINED)
#define RVA_ORDER_SCRY                         ADDR(        0x2d0cc0,          MAC_NOT_FOUND)
#define RVA_UNIQUE_SUMMON_ALLOWED              ADDR(        0x2d5c70,          0x100357b64)
#define RVA_CRUSADE_RUN                        ADDR(        0x2d7ea0,          0x10035325c)
#define RVA_DRAINED_TERRAIN                    ADDR(        0x2d9a30,          0x100356578)
#define RVA_CHOP_BURN_SCREEN                   ADDR_DIFFERS(0x2dcce0,          0x10035bbb4)
#define RVA_SLAVE_HUNT_SCREEN                  ADDR_DIFFERS(0x2e0e20,          0x10035c2cc)
#define RVA_UNIT_SPELL_FLAGS                   ADDR(        0x2e3e30,          MAC_NOT_FOUND)
#define RVA_TERRAIN_NAME_TYPE                  ADDR(        0x2e9780,          MAC_INLINED)
#define RVA_UNIT_REFORMLOC                     ADDR(        0x31de60,          0x100568e4c)
#define RVA_TERRAIN_SPECIAL                    ADDR(        0x32d620,          MAC_INLINED)
#define RVA_REMOVE_CORPSES                     ADDR(        0x32e840,          0x1004c97f8)
#define RVA_OWNSLOC_SEARCH                     ADDR(        0x199da0,          0x1004a7f30)
#define RVA_CREATE_CITADEL                     ADDR(        0x118990,          0x10046a360)
#define RVA_RANDOM_COMMANDER_AT                ADDR(        0x117ec0,          0x10047fbec)
#define RVA_ITEM_SLOT_KIND                     ADDR(        0x1806a0,          MAC_INLINED)
#define MONSTER_FLAG_ALLITEMSLOTS              35
#define MONSTER_FLAG_WEAPONSLOTS               36
#define MONSTER_FLAG_NOSLOTS                   43
#define MONSTER_FLAG_NOBOOTSLOTS               47
#define MONSTER_FLAG_INANIMATE                 45
#define RVA_UNIT_FLUSH_CACHES                  ADDR(        0x117dd0,          0x10047f14c)
#define RVA_GIVE_START_EQUIPMENT               ADDR(        0x180b90,          MAC_INLINED)
#define RVA_RANDOM_MONSTER_WITH_ABILITY_2      ADDR(        0x0b5f00,          MAC_NOT_FOUND)
#define RVA_SCRYLOC_RADIUS_SCALE               ADDR(        0x4068ac,          MAC_NOT_FOUND)
#define RVA_REVEAL_RADIUS_SCALE                ADDR(        0x406a48,          MAC_NOT_FOUND)

// ---- Input --------------------------------------------------------------------------------------
#define RVA_POLL_SDL_EVENTS                    ADDR(        0x04eaa0,          MAC_INLINED)
#define RVA_GET_MOUSE_X                        ADDR(        WIN_INLINED,       MAC_INLINED)
#define RVA_GET_MOUSE_Y                        ADDR(        WIN_INLINED,       MAC_INLINED)
#define RVA_MOUSE_Y_UNFLIPPED                  ADDR(        0x57efc54,         MAC_IN_CODE)
#define RVA_WINDOW_FULLSCREEN_MODE             ADDR(        0x593325c,         0x113997040)
#define RVA_MOUSE_Y_UNFLIPPED_OFF              RVA_WINDOW_FULLSCREEN_MODE
#define RVA_GET_FULLSCREEN_MODE                ADDR(        WIN_INLINED,       MAC_INLINED)
#define RVA_FULLSCREEN_SETTING                 ADDR(        0x1a47c04,         0x1008575e0)
#define RVA_GET_MOUSE_WHEEL                    ADDR(        WIN_INLINED,       MAC_INLINED)
#define RVA_GET_MOUSE_BUTTONS                  ADDR(        WIN_INLINED,       MAC_INLINED)
#define RVA_WINDOW_W                           ADDR(        0x12ab03dc,        0x113997160)
#define RVA_WINDOW_H                           ADDR(        0x12ab03e4,        0x113997060)
#define RVA_SDL_WINDOW_W                       ADDR(        0x59937e0,         0x11399715c)
#define RVA_SDL_WINDOW_H                       ADDR(        0x593afcc,         0x113997164)
#define RVA_WHEEL                              ADDR(        0x57f0654,         0x113997068)
#define RVA_KEY_STATE                          ADDR(        0x12ab0240,        0x113997168)
#define RVA_KEY_CODES                          ADDR(        0x12ab01a4,        MAC_STORED_DIFFERENTLY)
#define RVA_KEY_CODE_F1                        ADDR_DIFFERS(0x12ab01ac,        0x113997100)
#define RVA_MOTION_X                           ADDR(        0x57efc1c,         0x11399705c)
#define RVA_MOTION_Y                           ADDR(        0x57f06b0,         0x113997064)
#define RVA_PRESS_X                            ADDR(        0x439e7c,          0x105aa402c)
#define RVA_PRESS_Y                            ADDR(        0x43a2d4,          0x105aa4030)
#define RVA_BUTTONS_HELD                       ADDR(        0x57f068c,         0x113997070)
#define RVA_PRESSED                            ADDR(        0x57f0670,         0x11399706c)
#define RVA_WIN_W                              RVA_WINDOW_W
#define RVA_WIN_H                              RVA_WINDOW_H
#define RVA_SDL_SCALE_W                        RVA_SDL_WINDOW_W
#define RVA_SDL_SCALE_H                        RVA_SDL_WINDOW_H
#define RVA_PLAYER1_NAME                       RVA_SLOT_NAME
#define RVA_HOVER_SLOT                         ADDR(        0x42dd34,          0x100862694)
#define RVA_MOUSE_BUTTONS                      ADDR(        0x5835730,         0x10c96df4c)
#define RVA_CLICK_LATCH                        ADDR_DIFFERS(0x57ef1a4,         0x10611be5c)
#define RVA_UI_MOUSE_X                         ADDR(        0x5835734,         0x10c96df50)
#define RVA_UI_MOUSE_Y                         ADDR(        0x583572c,         0x10c96df54)
#define RVA_ELEMENT_AT                         RVA_FIND_HOVERED
#define RVA_DRAW_CALL                          ADDR(        0x24023f,          0x10048acb0)
#define RVA_DRAW_PASS                          ADDR(        0x02cfc0,          0x100345904)
#define RVA_HIT_TEST_CALL                      ADDR(        0x23f200,          MAC_NO_CALL)
#define RVA_HIT_TEST                           ADDR(        0x02a890,          MAC_INLINED)
#define RVA_SET_SDL_WINDOW_SIZE                ADDR(        0x04fd10,          0x1005aca0c)
#define RVA_INPUT_INIT                         ADDR(        0x04df90,          0x1005aceac)
#define RVA_DRAG_OFFSET                        ADDR(        0x12ab2b00,        0x10611be58)
#define RVA_RESIZE_WINDOW_SIZE                 ADDR(        0x050086,          0x1005acd00)
#define RVA_INPUT_INIT_KEY_CLEAR               ADDR(        0x04dfd3,          0x1005acef0)
#define RVA_ESCAPE_STORE                       ADDR_DIFFERS(0x04e5b2,          0x1005ad420)
#define RVA_BIND_HOTKEY_ESCAPE_READ            ADDR(        0x03ab46,          0x10033c82c)
#define RVA_PRESS_SCALE                        ADDR(        0x04f249,          0x10048586c)
#define RVA_PRESS_DIVISOR                      ADDR(        0x04f254,          0x100485880)
#define RVA_WHEEL_ADD                          ADDR_DIFFERS(0x04f1e9,          0x100485540)
#define RVA_WAIT_WHEEL_READ                    ADDR_DIFFERS(0x23f07c,          0x1004862b0)
#define RVA_WAIT_UI_MOUSE                      ADDR(        0x23f028,          0x100486268)
#define RVA_WAIT_FRAME_BUTTONS                 ADDR(        0x23f0c7,          0x1004862dc)
#define RVA_WAIT_SQUARE_RESET                  ADDR(        0x23f0cd,          0x1004862e8)
#define RVA_ELEMENT_AT_CALL_1                  ADDR(        0x02a960,          0x1004864b0)
#define RVA_HOVER_STORE                        ADDR(        0x02a968,          0x1004864b8)
#define RVA_ELEMENT_AT_CALL_0                  ADDR(        0x02a992,          0x1004864cc)
#define RVA_WAIT_GAME_OVER_FX_1                ADDR(        0x23f43e,          0x100487bf4)
#define RVA_WAIT_GAME_OVER_FX_2                ADDR(        0x23f461,          0x100487c18)
#define RVA_WAIT_BANNER_FADE_STEP              ADDR(        0x23f840,          0x100488b34)
#define RVA_WAIT_LOGO_CALL                     ADDR(        0x23f8d1,          0x100488b9c)

#define RVA_TEXT_EVENT_FIRST_BYTE              ADDR(        0x04eb87,          0x100485750)
#define RVA_TEXT_ACCENT_STRINGS                ADDR(        0x439704,          MAC_IN_CODE)
#define RVA_TEXT_ACCENT_CODES                  ADDR(        0x12ab0208,        MAC_STORED_DIFFERENTLY)
#define RVA_KEY_DOWN                           ADDR(        0x04f460,          MAC_INLINED)
#define RVA_KEY_SYM_TO_CODE                    ADDR(        0x04f8a0,          0x1005ac768)
#define RVA_KEY_BUFFER                         ADDR(        0x5993800,         0x113997074)
#define RVA_KEY_BUFFER_LAST                    ADDR(        0x439dc4,          0x105aa4034)
#define RVA_KEY_SUPPRESS_CODE                  ADDR(        0x57f0688,         0x113997158)
#define RVA_POP_KEY                            ADDR(        0x0503b0,          MAC_INLINED)
#define RVA_HIT_TEST_POP_KEY_1                 ADDR_DIFFERS(0x02ac99,          0x1004867b8)
#define RVA_HIT_TEST_POP_KEY_2                 ADDR_DIFFERS(0x02b753,          0x1004873d0)
#define RVA_GUI_KEYPRESS                       ADDR(        0x431828,          0x100857630)
#define RVA_GUI_KEYPRESS_MORE                  ADDR(        0x432208,          0x100857634)
#define RVA_TEXT_INPUT_START                   ADDR(        0x050280,          MAC_INLINED)
#define RVA_TEXT_INPUT_STOP                    ADDR(        0x050210,          MAC_INLINED)
#define RVA_TEXT_FIELD                         ADDR(        0x038750,          0x10033e110)
#define RVA_NAME_BOX_TEXT_FIELD_CALL           ADDR(        0x041dda,          0x10034a00c)
#define RVA_PLAYER_NAME_MAX_LEN                ADDR_DIFFERS(0x304a88,          0x100349ffc)

// ---- Menus and keys -------------------------------------------------------------------------------
#define RVA_BANNER_FADE                        ADDR(        0x1ab8cec,         0x101cce55c)
#define RVA_SHORTCUTS_TABLE                    ADDR(        0x3f9f10,          0x100837fd0)
#define RVA_HOTKEY_TABLE                       ADDR(        0x1319a700,        0x10611b994)
#define HOTKEY_SIZE                            8
#define RVA_HOTKEY_COUNT                       ADDR(        0x42dca4,          0x100857624)
#define RVA_HOTKEY_COUNT_RESET                 ADDR(        0x03af91,          0x10033be9c)
#define RVA_HOTKEY_LOOKUP                      ADDR(        0x02b581,          0x10048696c)
#define RVA_MAP_BIND_SHORTCUTS_LIST            ADDR(        0x096975,          0x10037fa7c)
#define RVA_MAP_BIND_RECRUIT                   ADDR(        0x094d8f,          0x10037f20c)
#define RVA_MAP_BIND_END_TURN                  ADDR(        0x095105,          0x10037f284)
#define RVA_MAP_BIND_MESSAGES                  ADDR(        0x095291,          0x10037f2a8)
#define RVA_MAP_BIND_NEXT_COMMANDER            ADDR(        0x094d00,          0x10037f200)
#define RVA_MAP_BIND_PREVIOUS_COMMANDER        ADDR(        0x096f6f,          0x10037fb74)
#define RVA_MAP_BIND_COMMANDER_DONE            ADDR(        0x09549f,          0x10037f2d8)
#define RVA_MAP_BIND_TRANSFER                  ADDR(        0x094e13,          0x10037f218)
#define RVA_MAP_BIND_SPECIAL_POWER             ADDR(        0x094e9f,          0x10037f224)
#define RVA_MAP_BIND_UNIT_OVERVIEW             ADDR(        0x096a9b,          0x10037fab4)
#define RVA_MAP_BIND_PLAYER_OVERVIEW           ADDR(        0x0969ff,          0x10037fa98)
#define RVA_MAP_BIND_CLASS_DESCRIPTION         ADDR(        0x096c7a,          0x10037fb08)
#define RVA_MAP_BIND_MAP_DESCRIPTION           ADDR(        0x096d1a,          0x10037fb24)
#define RVA_MAP_BIND_SCORE_GRAPHS              ADDR(        0x096dba,          0x10037fb40)
#define RVA_MAP_BIND_ACTIVE_MODS               ADDR(        0x096e5a,          0x10037fb5c)
#define RVA_MAP_BIND_GAME_MENU_F10             ADDR(        0x094f2a,          0x10037f240)
#define RVA_MAP_BIND_GAME_MENU_F11             ADDR(        0x094fca,          0x10037f25c)
#define RVA_MAP_BIND_GAME_MENU_F12             ADDR(        0x09506a,          0x10037f278)
#define RVA_MAP_BIND_F9                        ADDR(        0x096b40,          0x10037fad0)
#define RVA_MAP_BIND_MENU_KEY                  ADDR(        0x096bdd,          0x10037faec)
#define RVA_MAP_BIND_TERRAIN_INFO              ADDR(        0x096fef,          0x10037fb80)
#define RVA_MAP_BIND_COMMANDER_STATS           ADDR(        0x09518a,          0x10037f290)
#define RVA_MAP_BIND_TRADE                     ADDR(        0x095311,          0x10037f2b4)
#define RVA_MAP_BIND_SENTRY                    ADDR(        0x095396,          0x10037f2c0)
#define RVA_MAP_BIND_LOCATION_POWER            ADDR(        0x09541a,          0x10037f2cc)
#define RVA_MAP_BIND_BURN_FOREST_SCREEN        ADDR(        0x0967c1,          0x10037f964)
#define RVA_MAP_BIND_BURN_FOREST_NOW           ADDR(        0x096eef,          0x10037fb68)
#define RVA_MAP_BIND_HUNT_SLAVES               ADDR(        0x09683f,          0x10037f970)
#define RVA_MAP_BIND_PLANE_SHIFT               ADDR(        0x099b32,          0x10037ddf4)
#define RVA_MAP_BIND_CONTINUE                  ADDR(        0x0970ff,          0x10037fb98)
#define RVA_MAP_BIND_CONTINUE_ALL              ADDR(        0x09707a,          0x10037fb8c)
#define RVA_MAP_BIND_TEST_GAME                 ADDR(        0x095210,          0x10037f29c)
#define RVA_MAP_BIND_HOME                      ADDR(        0x0968da,          0x10037fa70)
#define RVA_MAP_BIND_SQUARE_DETAILS            ADDR_DIFFERS(0x096895,          0x10037fa54)
#define RVA_MAP_BIND_MAP_ICONS                 ADDR_DIFFERS(0x096888,          0x10037f9d4)
#define RVA_MAP_BIND_ASTROLOGY                 ADDR_DIFFERS(0x09d57b,          0x10037a4c8)
#define RVA_MAP_BIND_REPEAT_ORDERS             ADDR_DIFFERS(0x097148,          0x10037fbfc)
#define RVA_MAP_BIND_SCRY                      ADDR_DIFFERS(0x09551d,          0x10037f34c)
#define RVA_MAP_BIND_MOVE_FIRST                ADDR(        0x09558f,          0x10037f374)
#define RVA_MAP_BIND_LOOK_FIRST                ADDR(        0x0962b0,          0x10037f514)
#define RVA_ARMY_SCREEN                        ADDR(        0x11f880,          MAC_INLINED)
#define RVA_BATTLE_SCREEN                      ADDR(        0x0ff6f0,          0x100582498)
#define RVA_BATTLE_EDITOR                      ADDR(        0x084bc0,          0x100518ca0)
#define RVA_VIEW_ARMY_SCREEN                   ADDR(        0x121ed0,          0x10047db40)
#define RVA_UNIT_LIST                          ADDR(        0x0f99a0,          MAC_INLINED)
#define RVA_SHEET_SCREEN                       ADDR(        0x03e480,          MAC_INLINED)
#define RVA_SHEET_ADD_SHORTCUT                 ADDR(        WIN_INLINED,       MAC_INLINED)
#define RVA_SHEET_SHORTCUT_KEYS                ADDR(        0x12ab2a70,        0x10617f3b0)
#define RVA_SHEET_SHORTCUT_IDS                 ADDR(        0x12ab2aa0,        0x10617f3e0)
#define RVA_MISC_MENU                          ADDR(        0x0a0d20,          0x100372ec8)
#define RVA_BIND_HOTKEY_NEGATIVE               ADDR(        WIN_NOT_FOUND,     MAC_NOT_FOUND)
#define RVA_MAP_VIEW_UPDATE                    ADDR(        0x23ca40,          MAC_INLINED)
#define RVA_MAP_VIEW_UPDATE_CALL               ADDR(        0x24080f,          MAC_INLINED)
#define RVA_BATTLE_VIEW_UPDATE                 ADDR(        0x23c360,          MAC_INLINED)
#define RVA_WAIT_TAB_READ                      ADDR(        0x23ff3d,          MAC_NOT_FOUND)
#define RVA_SDL_POLL_EVENT                     ADDR_DIFFERS(WIN_INLINED,       0x10000f764)
#define RVA_SDL_POLL_EVENT_SLOT                ADDR_DIFFERS(0x33a580,          0x100852020)
#define RVA_SDL_PUSH_EVENT_SLOT                ADDR(        WIN_NO_EQUIVALENT, 0x100852038)
#define RVA_SWAP_WINDOW                        ADDR(        WIN_INLINED,       MAC_INLINED)
#define RVA_WAIT_SWAP_CALL                     ADDR_DIFFERS(0x240437,          0x10048c2e8)
#define RVA_SDL_GL_SWAP_WINDOW                 ADDR_DIFFERS(WIN_INLINED,       0x100010c40)
#define RVA_SDL_GL_SWAP_WINDOW_SLOT            ADDR_DIFFERS(0x33a658,          0x100852de8)
#define RVA_SDL_WINDOW                         ADDR(        0x57f0658,         0x113997038)
#define RVA_FRAME_COUNT                        ADDR(        0x5835720,         0x106cee4f8)
#define RVA_KEY_CODE_BACKSPACE                 ADDR(        0x12ab0200,        MAC_STORED_DIFFERENTLY)
#define RVA_KEY_CODE_TAB                       ADDR(        0x12ab0204,        MAC_STORED_DIFFERENTLY)
#define RVA_KEY_CODE_ENTER                     ADDR(        0x12ab01ae,        MAC_STORED_DIFFERENTLY)
#define RVA_KEY_CODE_DELETE                    ADDR(        0x12ab03d6,        MAC_STORED_DIFFERENTLY)
#define RVA_KEY_CODE_PAGE_DOWN                 ADDR(        0x12ab01ee,        MAC_STORED_DIFFERENTLY)
#define RVA_KEY_CODE_PAGE_UP                   ADDR(        0x12ab03e8,        MAC_STORED_DIFFERENTLY)
#define RVA_KEY_CODE_END                       ADDR(        0x12ab020a,        MAC_STORED_DIFFERENTLY)
#define RVA_KEY_CODE_HOME                      ADDR(        0x12ab03d8,        MAC_STORED_DIFFERENTLY)
#define RVA_KEY_CODE_UP                        ADDR(        0x12ab03d4,        MAC_STORED_DIFFERENTLY)
#define RVA_KEY_CODE_DOWN                      ADDR(        0x12ab03e0,        MAC_STORED_DIFFERENTLY)
#define RVA_KEY_CODE_LEFT                      ADDR(        0x12ab03d0,        MAC_STORED_DIFFERENTLY)
#define RVA_KEY_CODE_RIGHT                     ADDR(        0x12ab03e2,        MAC_STORED_DIFFERENTLY)
#define RVA_KEY_CODE_LEFT_SHIFT                ADDR(        0x12ab0218,        MAC_STORED_DIFFERENTLY)
#define RVA_KEY_CODE_LEFT_CTRL                 ADDR(        0x12ab0216,        MAC_STORED_DIFFERENTLY)
#define RVA_KEY_CODE_RIGHT_CTRL                ADDR(        0x12ab01dc,        MAC_STORED_DIFFERENTLY)
#define RVA_KEY_CODE_LEFT_ALT                  ADDR(        0x12ab01f8,        MAC_STORED_DIFFERENTLY)

// ---- Map editor and the map view ----------------------------------------------------------------
#define RVA_MAP_EDITOR_ACTIVE                  ADDR_DIFFERS(0x582c6c0,         0x108810eb4)
#define RVA_MAP_EDITOR_MENU_SKIP               ADDR(        0x1b70d5,          0x100514a48)
#define RVA_MAP_EDITOR_TITLE_CALL              ADDR(        0x1b7189,          0x100514ab4)
#define RVA_MAP_EDITOR_REMOVE_FANCY_CALL       ADDR(        0x1b71fc,          0x100514b2c)
#define RVA_MAP_EDITOR_CREATE_FANCY_CALL       ADDR(        0x1b7219,          0x100514b48)
#define RVA_MAP_EDITOR_SETTINGS_CALL           ADDR(        0x1b722a,          0x100514b58)
#define RVA_MAP_EDITOR_COORDS_CALL             ADDR(        0x1b7365,          0x100514d38)
#define RVA_MAP_EDITOR_OVERLAY_DRAW_CALL       ADDR(        0x1b8783,          0x1005156f8)
#define RVA_CREATE_FANCY_TERRAIN               ADDR(        0x1b45b0,          MAC_INLINED)
#define RVA_SAVE_MAP                           ADDR(        0x1b4d10,          MAC_INLINED)
#define MAP_EDITOR_FREE_ID_FIRST               0x2c
#define RVA_MAP_ZOOM                           ADDR(        0x1ab9d6c,         0x10087350c)
#define RVA_MAP_CAMERA_X                       ADDR(        0x5835728,         0x10b848af8)
#define RVA_MAP_CAMERA_Y                       ADDR(        0x5835724,         0x10b848afc)
#define RVA_MAP_ZOOM_SPEED                     ADDR(        0x5835774,         0x106cee55c)
#define RVA_MAP_VIEW_PLANE                     ADDR(        0x5835738,         0x10c921d04)
#define RVA_MAP_VIEW_DRAW_CALL                 ADDR(        0x23fc3e,          0x100489268)
#define RVA_MAP_SKY_UNDERLAY_DRAW_CALL         ADDR(        0x23fc33,          0x10048924c)
#define RVA_DRAW_LINE_UI                       ADDR(        0x030380,          MAC_INLINED)
#define RVA_FILL_RECT_UI                       ADDR(        0x030280,          0x100344e18)
#define GUI_TYPE_LINE                          0x1d
#define RVA_BATTLE_GRID_ON                     ADDR(        0x43e3a4,          0x1048937d8)
#define RVA_BATTLE_FIELD_DRAW                  ADDR(        0x0fbf30,          0x10057f5d4)
#define RVA_BATTLE_GRID_SPRITE_CALL            ADDR(        0x0fc1fd,          0x10057f810)
#define BATTLE_GRID_SPRITE                     0x17
#define MAP_EDITOR_FREE_ID_LAST                999
#define RVA_MAP_EDITOR_PLANE_ON                ADDR(        0x582c690,         0x108810ec0)
#define RVA_MAP_EDITOR_SETTINGS                ADDR(        0x1b3c60,          MAC_INLINED)
#define RVA_MAP_EDITOR_START_WORLD_CALL        ADDR(        0x1b6f2e,          0x100514794)
#define RVA_MAP_EDITOR_RECREATE                ADDR(        0x1b88c4,          0x1005161fc)
#define RVA_MAP_EDITOR_RECREATE_CALL           ADDR(        0x1b8904,          0x100516230)
#define RVA_MAP_EDITOR_EMPTY_PLANE_CALL        ADDR(        0x1b8a6d,          0x1005162b0)
#define RVA_MAP_EDITOR_UNKNOWN_PLANE_CALL      ADDR(        0x1b89fc,          0x1005162d8)
#define RVA_MAP_EDITOR_LOAD_CALL               ADDR(        0x1b8ce6,          0x10051642c)
#define RVA_MAP_EDITOR_SAVE_CALL               ADDR(        0x1b8f80,          MAC_INLINED)
#define RVA_MAP_EDITOR_PLANE_BAR               ADDR(        0x1b3730,          MAC_INLINED)
#define RVA_MAP_EDITOR_SAVE_TERRAIN_WORD       ADDR(        0x582c67c,         0x108810eb0)
#define RVA_GL_STATE                           ADDR(        0x029020,          0x1003508c0)
#define GL_STATE_LINES                         0x16
#define RVA_TEXT_RENDER_FLAGS                  ADDR(        0x57c5914,         0x105b68184)
#define DRAW_TEXT_SIZE_SCALE                   0.75f
#define RVA_GL_COLOR4F                         ADDR_DIFFERS(0x33a460,          0x1005ae560)
#define RVA_GL_LINE_WIDTH                      ADDR_DIFFERS(0x33a3f0,          0x1005ae668)
#define RVA_GL_ENABLE_CLIENT_STATE             ADDR_DIFFERS(0x33a4b8,          0x1005ae5c0)
#define RVA_GL_DISABLE_CLIENT_STATE            ADDR_DIFFERS(0x33a3f8,          0x1005ae59c)
#define RVA_GL_VERTEX_POINTER                  ADDR_DIFFERS(0x33a428,          0x1005ae74c)
#define RVA_GL_DRAW_ARRAYS                     ADDR_DIFFERS(0x33a3d0,          0x1005ae5a8)

// ---- Map terrain drawing ------------------------------------------------------------------------
#define RVA_TRS_DECODE                         ADDR(        0x06a070,          MAC_INLINED)
#define RVA_TRS_DECODE_RLE                     ADDR(        0x062f10,          0x100350104)
#define RVA_TRS_DECODE_RLE_OLD                 ADDR(        0x0630d0,          0x10034ff20)
#define RVA_TRS_DECODE_RAW                     ADDR(        0x063290,          MAC_INLINED)
#define RVA_SPRITE_ALPHA_MODE                  ADDR(        0x026130,          0x100339950)
#define RVA_IMAGE_TEXTURE_LOAD                 ADDR(        0x06a4d0,          0x10033b33c)
#define RVA_IMAGE_READ                         ADDR(        0x0284b0,          0x100337aec)
#define RVA_TGA_DECODE                         ADDR(        0x025d80,          MAC_INLINED)
#define RVA_PNG_DECODE                         ADDR(        0x024d90,          0x100338a90)
#define RVA_IMAGE_HAS_ALPHA                    ADDR(        0x0257d0,          MAC_INLINED)
#define RVA_IMAGE_COLOUR_MODE                  ADDR(        0x026360,          0x100338cd0)
#define RVA_TEXTURE_CREATE                     ADDR(        0x06a650,          0x10033acd0)
#define RVA_SPRITE_COMPRESSION                 ADDR_DIFFERS(0x57f2584,         0x106ceedf4)
#define RVA_SPRITE_CACHE_DROP                  ADDR(        0x064cd0,          MAC_INLINED)
#define SPRITE_KIND_TREE                       4
#define SPRITE_KIND_WALLS                      0x23
#define TREE_TRS_SPRITES                       943
#define TERRAIN_TRS_SPRITES                    356
#define RVA_CREATE_BACKGROUND_IMAGE            ADDR(        0x07e700,          0x10054db2c)
#define RVA_GROUND_TEXTURES_LOAD               ADDR(        0x07eab0,          MAC_INLINED)
#define RVA_GROUND_TEXTURE_NAMES               ADDR(        0x440b48,          0x100847d10)
#define RVA_GROUND_TEXTURES                    ADDR(        0x123e3000,        0x10c6404f8)
#define GROUND_TEXTURE_SIZE                    0x18
#define GROUND_TEXTURE_W                       0x08
#define GROUND_TEXTURE_H                       0x0c
#define GROUND_COUNT                           29
#define RVA_GROUND_WEIGHT_PASS                 ADDR(        0x07efe0,          MAC_INLINED)
#define RVA_GROUND_WEIGHT_ROWS                 ADDR(        0x07f5a0,          0x10054b92c)
#define RVA_GROUND_EDGE_SPREAD                 ADDR(        0x07fa70,          MAC_INLINED)
#define RVA_GROUND_WEIGHT_SUM                  ADDR(        0x0841c0,          MAC_INLINED)
#define RVA_GROUND_EDGE_LINES                  ADDR(        0x083850,          MAC_INLINED)
#define RVA_GROUND_EDGE_MIDPOINT               ADDR(        0x083ec0,          0x10054abf8)
#define RVA_GROUND_EDGE_NOISE                  ADDR(        0x614f840,         0x10c640898)
#define RVA_GROUND_WEIGHTS                     ADDR(        0x6128650,         0x10c6407b0)
#define RVA_GROUND_COMPOSITE                   ADDR(        0x082cb0,          MAC_INLINED)
#define RVA_GROUND_COMPOSITE_ROWS              ADDR(        0x0830f0,          0x10054ad68)
#define RVA_MAP_BG_GROUNDS                     ADDR(        0x12389000,        0x10c77c120)
#define RVA_MAP_BG_FLOODED                     ADDR(        0x12339000,        0x10c7cc120)
#define RVA_MAP_BG_W_SQUARES                   ADDR(        0x580338c,         0x10c77c118)
#define RVA_MAP_BG_H_SQUARES                   ADDR(        0x5803388,         0x10c77c11c)
#define RVA_MAP_BG_IMAGE                       ADDR(        0x5803398,         0x10c922c70)
#define RVA_MAP_BG_TILE_DRAW                   ADDR(        0x0b2f10,          MAC_INLINED)
#define RVA_MAP_BG_TILE_DRAW_CALL              ADDR(        0x19c7a4,          MAC_NO_CALL)
#define SQUARE_FLAG_FLOODED                    0x2000
#define TERRAIN_FLAG_INVISIBLE                 0x1000
#define TERRAIN_FLAG_UG                        0x20000000000000
#define TERRAIN_FLAG_TEMPLE                    0x4000000
#define TERRAIN_FLAG_TOWN1                     0x1000000000000
#define TERRAIN_FLAG_TOWN2                     0x2000000000000
#define TERRAIN_ATAVI_VILLAGE                  348
#define TERRAIN_FLAG_SMOKE                     0x80
#define RVA_TERRAIN_SPR_SKIP                   ADDR(        0xd259fa0,         0x10857bd76)
#define RVA_TERRAIN_SPR_DRAW_CALL              ADDR(        0x19c879,          0x10049b688)
#define RVA_MAP_DECORATIONS                    ADDR(        0x1a1fb0,          0x100494f30)
#define RVA_MAP_DECORATIONS_CALL               ADDR(        0x19c896,          0x10049b69c)
#define RVA_MAP_DECORATION_CASES               ADDR(        0x1a73b0,          MAC_STORED_DIFFERENTLY)
#define RVA_DECOR_SCATTER                      ADDR(        0x1aa630,          0x1004922b8)
#define RVA_DECOR_SINGLE                       ADDR(        0x1a8260,          0x100493f38)
#define RVA_DECOR_CENTERED                     ADDR(        0x1a7520,          0x100494d28)
#define RVA_DECOR_MOUNTAINS                    ADDR(        0x1a8ca0,          0x100493554)
#define RVA_DECOR_QUEUE                        ADDR(        0xf585720,         0x1071faccc)
#define DECOR_QUEUE_ENTRY_SIZE                 0x28
#define DECOR_ENTRY_KIND                       0x00
#define DECOR_ENTRY_SPRITE                     0x02
#define DECOR_ENTRY_X                          0x04
#define DECOR_ENTRY_W                          0x0c
#define DECOR_ENTRY_H                          0x10
#define RVA_DECOR_QUEUE_LAST                   ADDR(        0x1a65504,         0x100872bfc)
#define RVA_DECOR_QUEUE_ADD                    ADDR(        0x1ad040,          MAC_INLINED)
#define RVA_DECOR_DRAW                         ADDR(        0x1ab0e0,          MAC_INLINED)
#define RVA_DECOR_DRAW_CALL                    ADDR(        0x19d628,          MAC_NO_CALL)
#define RVA_DRAW_SPRITE_SHEARED                ADDR(        0x063ec0,          0x10034f608)
#define RVA_MAP_SWAY                           ADDR(        0xf5708c0,         0x106cf539c)
#define RVA_MAP_SWAY_UPDATE                    ADDR(        0x19e1e0,          MAC_INLINED)
#define RVA_MAP_SWAY_UPDATE_CALL               ADDR(        0x23fc20,          MAC_NO_CALL)
#define RVA_MAP_SWAY_CLOCK                     ADDR(        0x582bc34,         0x107dfb4c8)
#define RVA_FRAME_MS                           ADDR(        0x5835f88,         0x106ceedd0)
#define RVA_MAP_SPRINKLES                      ADDR(        0xf570900,         0x107dcb5d8)
#define RVA_MAP_RANDOM_FLOATS                  ADDR(        0xf580900,         0x106cf057c)
#define RVA_BG_THREADS_OFF                     ADDR(        0x5827584,         0x10c6879a4)
#define RVA_MAP_RANDOM_FLOATS_FILL             ADDR(        0x1ad100,          MAC_INLINED)
#define RVA_MAP_RANDOM_FLOATS_DONE             ADDR(        0x582a898,         0x106cf0578)
#define RVA_DECOR_BUCKET_FILL                  ADDR(        0x1abdf0,          MAC_INLINED)
#define RVA_DECOR_BUCKET_SORT                  ADDR(        0x1abb70,          MAC_INLINED)
#define RVA_GRAPHICS_DETAIL                    ADDR(        0x1a50e2c,         0x100872c08)

// ---- Utilities ----------------------------------------------------------------------------------
#define RVA_TRACE                              ADDR(        0x028e80,          0x10033a798)
#define RVA_SNPRINTF                           ADDR_DIFFERS(0x001430,          0x1005aed34)
#define RVA_VSPRINTF                           ADDR_DIFFERS(0x005724,          0x1005aef08)
#define RVA_PRINTF_OPTIONS                     ADDR(        0x001300,          MAC_NOT_FOUND)
#define RVA_SYSTEM                             ADDR(        0x0062b0,          0x1005aeeb4)
#define RVA_GETTIMEOFDAY                       ADDR(        WIN_NO_EQUIVALENT, 0x1005ae500)
#define RVA_TRACE_B                            ADDR(        0x0288f0,          0x10033a9c0)
#define RVA_TRACE_C                            ADDR(        0x028ce0,          0x10033a8ac)
#define RVA_DEBUG_LEVEL                        ADDR(        0x42d01c,          0x1059bcc78)
#define RVA_TRACE_B_CHECK                      ADDR(        0x028924,          0x10033aa00)
#define RVA_TRACE_C_CHECK                      ADDR(        0x028d14,          0x10033a8ec)
#define RVA_TRACE_ERROR                        ADDR(        0x028a90,          0x10033a62c)
#define RVA_TRACE_ERROR_WRITE                  ADDR(        0x028b50,          MAC_NOT_LOOKED_UP)
#define TRACE_PROLOGUE_SIZE                    0x44
#define TRACE_ERROR_PROLOGUE_SIZE              0x40
#define RVA_TRACE_TIMER                        ADDR(        0x028700,          MAC_NOT_LOOKED_UP)
#define RVA_TRACE_TIMER_MARK                   ADDR(        WIN_INLINED,       MAC_NOT_LOOKED_UP)
#define RVA_TRACE_TO_LOG_FILE                  ADDR(        0x57c58dc,         MAC_NOT_FOUND)
#define RVA_TRACE_LOG_START                    ADDR(        0x028cb0,          MAC_NOT_FOUND)
#define RVA_TRACE_LOG_OPEN_CALL                ADDR(        0x028f24,          MAC_NO_CALL)
#define RVA_TRACE_LOG_CLOSE_CALL               ADDR(        0x028f8f,          MAC_NO_CALL)
#define RVA_TRACE_C_LOG_OPEN_CALL              ADDR(        0x028d84,          MAC_NO_CALL)
#define RVA_TRACE_C_LOG_CLOSE_CALL             ADDR(        0x028def,          MAC_NO_CALL)
#define RVA_TRACE_B_LOG_OPEN_CALL              ADDR(        0x028994,          MAC_NO_CALL)
#define RVA_TRACE_B_LOG_CLOSE_CALL             ADDR(        0x0289ff,          MAC_NO_CALL)
#define RVA_TRACE_ERROR_LOG_OPEN_CALL          ADDR(        0x028b7f,          MAC_NO_CALL)
#define RVA_TRACE_ERROR_LOG_CLOSE_JUMP         ADDR(        0x028bee,          MAC_NO_CALL)
#define RVA_TYPED_WORD_DEBUG_LEVEL             ADDR(        WIN_NOT_FOUND,     MAC_NOT_FOUND)
#define RVA_TRACE_TIMESTAMPS                   ADDR_DIFFERS(0x57c58e4,         0x105b68154)
#define RVA_SDL_LOADOBJECT_SLOT                ADDR(        WIN_NO_EQUIVALENT, 0x100852390)
#define RVA_SDL_LOADFUNCTION_SLOT              ADDR(        WIN_NO_EQUIVALENT, 0x100852398)
#define RVA_SDL_LOADOBJECT_INIT                ADDR(        WIN_NO_EQUIVALENT, 0x10001699c)
#define RVA_SDL_LOADFUNCTION_INIT              ADDR(        WIN_NO_EQUIVALENT, 0x1000169c8)
#define RVA_FAIL_IF                            ADDR(        0x05b600,          0x100334130)
#define RVA_ALLOC                              ADDR(        0x05e4b0,          0x100333db0)
#define RVA_TICKS_MS                           ADDR_DIFFERS(0x05e3c0,          0x1003341fc)
#define RVA_FILE_READ_INT                      ADDR(        0x058a60,          0x100336424)
#define RVA_FILE_READ_SHORT                    ADDR(        0x058870,          MAC_INLINED)
#define RVA_FILE_READ_ARRAY                    ADDR(        0x057fb0,          MAC_INLINED)
#define RVA_FILE_WRITE_INT                     ADDR(        0x058530,          MAC_INLINED)
#define RVA_FILE_WRITE_ARRAY                   ADDR(        0x058050,          MAC_INLINED)
#define RVA_FATAL_FORMAT                       ADDR(        0x05e820,          0x100333ff8)
#define RVA_RANDOM                             ADDR(        0x05b7b0,          0x100335618)
#define RVA_RANDOM_OPEN_DIE                    ADDR(        0x058cc0,          0x1003369e4)
#define RVA_INT_SQRT                           ADDR(        0x056f40,          MAC_INLINED)
#define RVA_RANDOM_INDEX                       ADDR(        0x57f1a74,         0x105aa91e0)
#define RVA_RANDOM_SHIFT                       ADDR(        0x57f1a98,         0x105aa9248)
#define RVA_RANDOM_XOR                         ADDR(        0x57f1a70,         0x105aa924c)
#define RVA_RANDOM_SEED_FROM_CLOCK             ADDR(        0x05adb0,          MAC_INLINED)
#define RVA_RANDOM_SET_SEED                    ADDR(        0x05ad60,          MAC_INLINED)
#define RVA_CRT_RAND_BELOW                     ADDR(        0x05e7a0,          MAC_NOT_FOUND)
#define RVA_CRT_SEED_FROM_CLOCK                ADDR(        0x05ac10,          MAC_INLINED)
#define RVA_CRT_RAND                           ADDR_DIFFERS(0x00eff0,          0x1005aec20)
#define RVA_CRT_SRAND                          ADDR_DIFFERS(0x00f01c,          0x1005aed4c)
#define RVA_BATTLE_SEED_DRAW_CALL              ADDR(        0x0f511e,          0x1003dc69c)
#define RVA_RANDOM_TABLE                       ADDR(        0x343790,          0x10075baa4)
#define RVA_RAND_CHURN_0                       ADDR(        0xd63ec44,         0x10b904630)
#define RVA_RAND_CHURN_1                       ADDR(        0x1ab764c,         0x100873510)
#define RVA_RAND_CHURN_2                       ADDR(        0x58356d4,         0x10b903c50)
#define RVA_RAND_CHURN_3                       ADDR(        0x58356cc,         0x10b903c58)
#define RVA_RAND_CHURN_4                       ADDR(        0x1ab671c,         0x100873518)
#define RVA_RAND_CHURN_5                       ADDR(        0x1ab6b2c,         0x100873514)
#define RVA_RAND_CHURN_6                       ADDR(        0x58356d8,         0x10b903c5c)
#define RVA_RAND_CHURN_7                       ADDR(        0xd63ec40,         0x10b904634)
#define RVA_RAND_CHURN_8                       ADDR(        0x58356dc,         0x10b903c64)
#define RVA_RAND_CHURN_9                       ADDR(        0x11f48ea0,        0x10b904640)
#define RVA_RAND_CHURN_10                      ADDR(        0x11f97a40,        0x10b904644)
#define RVA_RAND_CHURN_11                      ADDR(        0x11f48f38,        0x10b904638)
#define RVA_RAND_CHURN_12                      ADDR(        0x11f48f74,        0x10b90463c)
#define RVA_RAND_CHURN_COUNT_A                 ADDR(        0x44192c,          0x10087351c)
#define RVA_RAND_CHURN_COUNT_B                 ADDR(        0x441894,          0x100873520)
#define RVA_RAND_CHURN_MIX                     ADDR(        WIN_INLINED,       0x10051c0d0)
#define RVA_RAND_CHURN_OUTER                   ADDR(        WIN_INLINED,       0x10051bc08)
#define RVA_RAND_CHURN_WRAP                    ADDR(        WIN_INLINED,       0x10051b9c0)
#define CRT_RAND_STATE_OFFSET                  0x28
#define RVA_CRT_THREAD_DATA                    ADDR(        0x014438,          MAC_STORED_DIFFERENTLY)
#define RVA_PROGRESS_TEXT_WAIT_CALL            ADDR(        0x08984a,          0x100490418)
#define RVA_WINDOW_CLOSE_ASKED                 ADDR_DIFFERS(0x57f0680,         0x113997030)
#define RVA_WHEEL_FRAME                        ADDR(        0xd63ec38,         0x10c96df58)
#define RVA_MAP_WHEEL_LAST                     ADDR(        0x1abbdb4,         0x100872ba4)
#define RVA_MEMSET                             ADDR(        0x002ef0,          0x1005ae8a8)
#define RVA_MEMMOVE                            ADDR(        0x002630,          0x1005ae89c)
#define RVA_MEMCPY                             ADDR_DIFFERS(0x002630,          0x1005ae890)
#define RVA_BZERO                              ADDR(        WIN_NO_EQUIVALENT, 0x1005ae284)
#define RVA_FREAD                              ADDR(        0x00e6a0,          0x1005ae458)
#define RVA_FWRITE                             ADDR(        0x005fe0,          0x1005ae4d0)
#define RVA_FCLOSE                             ADDR(        0x005ab8,          0x1005ae3bc)
#define RVA_FOPEN                              ADDR(        0x005a28,          0x1005ae428)
#define RVA_FGETS                              ADDR(        0x00f950,          0x1005ae3f8)
#define RVA_FFLUSH                             ADDR(        0x006220,          0x1005ae3ec)
#define RVA_STRCMP_STUB                        ADDR(        WIN_NO_EQUIVALENT, 0x1005aeda0)
#define RVA_CHKSTK                             ADDR(        0x023ef0,          MAC_STORED_DIFFERENTLY)
#define RVA_CHKSTK_DARWIN_GOT                  ADDR(        WIN_NO_EQUIVALENT, 0x100774210)
#define RVA_FILE_BYTES_WRITTEN                 ADDR(        0x57f1a80,         0x105aa9290)

#define RVA_UNHANDLED_EXCEPTION_FILTER         ADDR(        0x002370,          MAC_NOT_FOUND)
#define RVA_STRNICMP                           ADDR(        0x057950,          MAC_NOT_FOUND)
#define RVA_GL_NPOT_TEXTURES                   ADDR(        0x439334,          MAC_NOT_FOUND)
#define RVA_NO_GL_EXTENSIONS                   ADDR(        0x1a42d7c,         MAC_NOT_FOUND)

// ---- The CRT's stack-cookie check and exception handlers ----------------------------------------
#define RVA_SECURITY_COOKIE                    ADDR(        0x42c000,          MAC_NOT_FOUND)
#define RVA_SECURITY_CHECK_COOKIE              ADDR(        0x001880,          MAC_NOT_FOUND)
#define RVA_CHECK_COOKIE_FAIL_JMP              ADDR(        0x00189c,          MAC_NOT_FOUND)
#define RVA_REPORT_GSFAILURE                   ADDR(        0x0018d8,          MAC_NOT_FOUND)
#define RVA_GS_HANDLER_CHECK                   ADDR(        0x023e60,          MAC_NOT_FOUND)
#define RVA_C_SPECIFIC_HANDLER                 ADDR(        0x002c74,          MAC_NOT_FOUND)
#define RVA_SCRT_SET_UNHANDLED_FILTER          ADDR(        0x002360,          MAC_NOT_FOUND)
#define RVA_STACK_CHK_FAIL_STUB                ADDR(        WIN_NO_EQUIVALENT, 0x1005ae17c)
#define RVA_SDL_QUIT_SIGNAL_HANDLER            ADDR(        WIN_NO_EQUIVALENT, 0x100025740)
#define RVA_SDL_INSTALL_QUIT_SIGNAL            ADDR(        WIN_NO_EQUIVALENT, 0x1000256ec)
#define RVA_SDL_REMOVE_QUIT_SIGNAL             ADDR(        WIN_NO_EQUIVALENT, 0x100025784)
#define RVA_SDL_QUIT_SIGNALLED                 ADDR(        WIN_NO_EQUIVALENT, 0x105aa586c)

// ---- Strings ------------------------------------------------------------------------------------
#define RVA_STR_MAINMENU                       ADDR(        0x1c5bfa0,         0x100605e64)
#define RVA_STR_START_NEW_GAME                 ADDR(        0x1c5bfe8,         0x100605e9b)
#define RVA_STR_LOAD_GAME                      ADDR(        0x1c5bff8,         0x1006fe5f9)
#define RVA_STR_NETWORK                        ADDR(        0x1c5c018,         0x1006ff3b0)
#define RVA_STR_PREFERENCES                    ADDR(        0x1c5c020,         0x100631441)
#define RVA_STR_MODS                           ADDR(        0x1c5c02c,         0x100706e72)
#define RVA_STR_CREDITS                        ADDR(        0x1c5c038,         0x100605eaa)
#define RVA_STR_READ_TIP                       ADDR(        0x1c5c040,         0x100605eb2)
#define RVA_STR_QUIT                           ADDR(        0x1c5c04c,         0x100710118)
#define RVA_STR_PERCENT_S                      ADDR(        0x1a3ba0c,         0x1005b6e1d)
#define RVA_STR_OPEN_MODS_DIR                  ADDR(        0x1c616d0,         0x100706ef0)
#define RVA_STR_GUIFONT                        ADDR(        0x432230,          0x100857674)
#define RVA_STR_GUIFONT_FANCY                  ADDR(        0x435350,          0x100861c7c)
#define RVA_STR_GUIFONT_TEXTY                  ADDR(        0x431840,          0x10085803c)
#define RVA_FONT_FILE_TABLE                    ADDR(        WIN_NOT_FOUND,     0x10077e6b0)
#define RVA_STR_LOAD_GAME_DISABLED             ADDR(        0x1c5c008,         0x1006fe5f9)
#define RVA_STR_GAME_MENU                      ADDR(        0x1b292a0,         0x10063142e)
#define RVA_STR_TIEVT                          ADDR(        0x43b368,          0x100712290)
#define RVA_STR_TTF_EXTENSION                  ADDR(        0x1c5db78,         0x1007070da)
#define RVA_STR_NO_TTF_FILE                    ADDR(        0x1c5db80,         0x1007070df)
#define RVA_STR_FONT_FILE_MISSING              ADDR(        0x1c5db58,         0x1007070bc)
#define RVA_STR_MOD_DIFFERS                    ADDR(        0x1c58cd0,         0x1006fd0c5)

// ---- Site generators' own sites -----------------------------------------------------------------
#define MAC_OWN_LIST           no_mac_address__the_mac_generator_keeps_its_own_list
#define RVA_SWITCH_TABLE_1                     ADDR(        0x053315,          MAC_OWN_LIST)
#define RVA_SWITCH_TABLE_2                     ADDR(        0x05331d,          MAC_OWN_LIST)
#define RVA_SWITCH_TABLE_3                     ADDR(        0x1775f0,          MAC_OWN_LIST)
#define RVA_SWITCH_TABLE_4                     ADDR(        0x26318e,          MAC_OWN_LIST)
#define RVA_SWITCH_TABLE_5                     ADDR(        0x282284,          MAC_OWN_LIST)
#define RVA_SQUARE_UNITS                       ADDR(        0x0a2a50,          0x100372430)
#define RVA_FG_VALUES_SITE_1                   ADDR(        0x0a2af7,          0x1003724a4)
#define RVA_FG_VALUES_SITE_2                   ADDR(        0x0ba816,          0x1003df424)
#define RVA_FG_VALUES_SITE_3                   ADDR(        0x0ba828,          0x1003df434)
#define RVA_FG_VALUES_SITE_4                   ADDR(        0x0ba83c,          0x1003df440)
#define RVA_FG_VALUES_SITE_5                   ADDR(        0x0ba970,          MAC_OWN_LIST)
#define RVA_FG_VALUES_SITE_6                   ADDR(        0x0ba9b2,          MAC_OWN_LIST)
#define RVA_FG_VALUES_SITE_7                   ADDR(        0x12030d,          0x100384734)
#define RVA_FG_VALUES_SITE_8                   ADDR(        0x1203e7,          MAC_OWN_LIST)
#define RVA_FG_VALUES_SITE_9                   ADDR(        0x0ba48d,          0x1003f780c)
#define RVA_FG_VALUES_SITE_10                  ADDR(        0x0ba49d,          0x1003f781c)
#define RVA_FG_VALUES_SITE_11                  ADDR(        0x0ba4b6,          0x1003f782c)
#define RVA_FG_VALUES_SITE_12                  ADDR(        0x0ba4c6,          0x1003f7838)
#define RVA_FG_VALUES_SITE_13                  ADDR(        0x0ba4da,          0x1003f7844)
#define RVA_FG_VALUES_SITE_14                  ADDR(        0x0ba4f0,          0x1003f7854)
#define RVA_FG_VALUES_SITE_15                  ADDR(        0x103f90,          0x1004827f8)
#define RVA_FG_VALUES_SITE_16                  ADDR(        0x103fa5,          0x10048280c)
#define RVA_FG_VALUES_SITE_17                  ADDR(        0x103fc6,          0x100482820)
#define RVA_FG_VALUES_SITE_18                  ADDR(        0x103fdd,          0x100482834)
#define RVA_FG_VALUES_SITE_19                  ADDR(        0x11d7b0,          MAC_OWN_LIST)
#define RVA_ARMY_SUMMARY_INLINED_HOST          ADDR(        0x11c8d0,          0x100482c74)
#define RVA_FG_VALUES_SITE_20                  ADDR(        0x11d7be,          MAC_OWN_LIST)
#define RVA_FG_VALUES_SITE_21                  ADDR(        0x11d7d8,          MAC_OWN_LIST)
#define RVA_FG_VALUES_SITE_22                  ADDR(        0x11d7ef,          MAC_OWN_LIST)
#define RVA_FG_VALUES_SITE_23                  ADDR(        0x0f6cb0,          0x1003cf5a8)
#define RVA_FGM_EXTRA_FUNCTIONS_SITE_1         ADDR(        WIN_NOT_LOOKED_UP, 0x1004827bc)
#define RVA_FGM_EXTRA_FUNCTIONS_SITE_2         ADDR(        WIN_NOT_LOOKED_UP, 0x1003cf448)
#define RVA_FGM_VALUES_SITE_1                  ADDR(        WIN_NOT_LOOKED_UP, 0x1003724a4)
#define RVA_FGM_VALUES_SITE_2                  ADDR(        WIN_NOT_LOOKED_UP, 0x1003725e8)
#define RVA_FGM_VALUES_SITE_3                  ADDR(        WIN_NOT_LOOKED_UP, 0x1003725f8)
#define RVA_FGM_VALUES_SITE_4                  ADDR(        WIN_NOT_LOOKED_UP, 0x100372610)
#define RVA_FGM_VALUES_SITE_5                  ADDR(        WIN_NOT_LOOKED_UP, 0x1003726a4)
#define RVA_FGM_VALUES_SITE_6                  ADDR(        WIN_NOT_LOOKED_UP, 0x100384734)
#define RVA_FGM_VALUES_SITE_7                  ADDR(        WIN_NOT_LOOKED_UP, 0x100384aa8)
#define RVA_FGM_VALUES_SITE_8                  ADDR(        WIN_NOT_LOOKED_UP, 0x1003df424)
#define RVA_FGM_VALUES_SITE_9                  ADDR(        WIN_NOT_LOOKED_UP, 0x1003df434)
#define RVA_FGM_VALUES_SITE_10                 ADDR(        WIN_NOT_LOOKED_UP, 0x1003df440)
#define RVA_FGM_VALUES_SITE_11                 ADDR(        WIN_NOT_LOOKED_UP, 0x1003df4ec)
#define RVA_FGM_VALUES_SITE_12                 ADDR(        WIN_NOT_LOOKED_UP, 0x1003df6dc)
#define RVA_FGM_VALUES_SITE_13                 ADDR(        WIN_NOT_LOOKED_UP, 0x1003df7b4)
#define RVA_FGM_VALUES_SITE_14                 ADDR(        WIN_NOT_LOOKED_UP, 0x1003df824)
#define RVA_FGM_VALUES_SITE_15                 ADDR(        WIN_NOT_LOOKED_UP, 0x1003df92c)
#define RVA_FGM_VALUES_SITE_16                 ADDR(        WIN_NOT_LOOKED_UP, 0x1003df958)
#define RVA_FGM_VALUES_SITE_17                 ADDR(        WIN_NOT_LOOKED_UP, 0x1004c2568)
#define RVA_FGM_VALUES_SITE_18                 ADDR(        WIN_NOT_LOOKED_UP, 0x1004c4158)
#define RVA_FGM_VALUES_SITE_19                 ADDR(        WIN_NOT_LOOKED_UP, 0x1004c4530)
#define RVA_FGM_VALUES_SITE_20                 ADDR(        WIN_NOT_LOOKED_UP, 0x1004c493c)
#define RVA_FGM_VALUES_SITE_21                 ADDR(        WIN_NOT_LOOKED_UP, 0x1003cf5a8)
#define RVA_FGM_VALUES_SITE_22                 ADDR(        WIN_NOT_LOOKED_UP, 0x1003f780c)
#define RVA_FGM_VALUES_SITE_23                 ADDR(        WIN_NOT_LOOKED_UP, 0x1003f781c)
#define RVA_FGM_VALUES_SITE_24                 ADDR(        WIN_NOT_LOOKED_UP, 0x1003f782c)
#define RVA_FGM_VALUES_SITE_25                 ADDR(        WIN_NOT_LOOKED_UP, 0x1003f7838)
#define RVA_FGM_VALUES_SITE_26                 ADDR(        WIN_NOT_LOOKED_UP, 0x1003f7844)
#define RVA_FGM_VALUES_SITE_27                 ADDR(        WIN_NOT_LOOKED_UP, 0x1003f7854)
#define RVA_FGM_VALUES_SITE_28                 ADDR(        WIN_NOT_LOOKED_UP, 0x1004827f8)
#define RVA_FGM_VALUES_SITE_29                 ADDR(        WIN_NOT_LOOKED_UP, 0x10048280c)
#define RVA_FGM_VALUES_SITE_30                 ADDR(        WIN_NOT_LOOKED_UP, 0x100482820)
#define RVA_FGM_VALUES_SITE_31                 ADDR(        WIN_NOT_LOOKED_UP, 0x100482834)
#define RVA_FGM_VALUES_SITE_32                 ADDR(        WIN_NOT_LOOKED_UP, 0x100482b50)
#define RVA_FGM_VALUES_SITE_33                 ADDR(        WIN_NOT_LOOKED_UP, 0x100482b84)
#define RVA_FGM_VALUES_SITE_34                 ADDR(        WIN_NOT_LOOKED_UP, 0x100482ba0)
#define RVA_FGM_NOT_VALUES_SITE_1              ADDR(        WIN_NOT_LOOKED_UP, 0x1004c2a44)
#define RVA_FGM_NOT_VALUES_SITE_2              ADDR(        WIN_NOT_LOOKED_UP, 0x1004c3060)
#define RVA_SPELL_TABLE_PAGE                   ADDR(        WIN_NOT_LOOKED_UP, 0x100839000)
#define RVA_TM_WEAPON_MAC_SIZE_EXCLUDE_1       ADDR(        WIN_NOT_LOOKED_UP, 0x1005845cc)
#define RVA_TM_WEAPON_MAC_SIZE_EXCLUDE_2       ADDR(        WIN_NOT_LOOKED_UP, 0x1005845d8)
#define RVA_TM_WEAPON_MAC_SIZE_EXCLUDE_3       ADDR(        WIN_NOT_LOOKED_UP, 0x10058b234)
#define RVA_TM_ITEM_NEAR_OK_1                  ADDR(        0x17d5f5,          MAC_NOT_LOOKED_UP)
#define RVA_TM_ITEM_EXCLUDE_1                  ADDR(        0x182e2a,          MAC_NOT_LOOKED_UP)
#define RVA_TM_ITEM_EXCLUDE_2                  ADDR(        0x2c92a3,          MAC_NOT_LOOKED_UP)
#define RVA_TM_ITEM_MAC_EXCLUDE_1              ADDR(        WIN_NOT_LOOKED_UP, 0x1004e44cc)
#define RVA_TM_CLASS_HEROES_NEAR_OK_1          ADDR(        0x0b205f,          MAC_NOT_LOOKED_UP)
#define RVA_TM_CLASS_HEROES_NEAR_OK_2          ADDR(        0x0b206d,          MAC_NOT_LOOKED_UP)
#define RVA_TM_CLASS_HEROES_NEAR_OK_3          ADDR(        0x0b207b,          MAC_NOT_LOOKED_UP)
#define RVA_TM_CLASS_HEROES_NEAR_OK_4          ADDR(        0x0b2089,          MAC_NOT_LOOKED_UP)
#define RVA_TM_CLASS_HEROES_NEAR_OK_5          ADDR(        0x0b2097,          MAC_NOT_LOOKED_UP)
#define RVA_TM_CLASS_HEROES_NEAR_OK_6          ADDR(        0x0b20a5,          MAC_NOT_LOOKED_UP)
#define RVA_TM_CLASS_HEROES_NEAR_OK_7          ADDR(        0x0b20b3,          MAC_NOT_LOOKED_UP)
#define RVA_TM_CLASS_HEROES_NEAR_OK_8          ADDR(        0x0b20c1,          MAC_NOT_LOOKED_UP)
#define RVA_TM_CLASS_HEROES_NEAR_OK_9          ADDR(        0x0b2196,          MAC_NOT_LOOKED_UP)
#define RVA_TM_CLASS_HEROES_NEAR_OK_10         ADDR(        0x0b21aa,          MAC_NOT_LOOKED_UP)
#define RVA_TM_CLASS_RECRUITS_NOT_REFS_1       ADDR(        0x277edf,          MAC_NOT_LOOKED_UP)
#define RVA_TM_CLASS_NEAR_OK_1                 ADDR(        0x19098c,          MAC_NOT_LOOKED_UP)
#define RVA_TM_CLASS_NEAR_OK_2                 ADDR(        0x1bba20,          MAC_NOT_LOOKED_UP)
#define RVA_TM_CLASS_NEAR_OK_3                 ADDR(        0x1bba31,          MAC_NOT_LOOKED_UP)
#define RVA_TM_CLASS_NEAR_OK_4                 ADDR(        0x1ca365,          MAC_NOT_LOOKED_UP)
#define RVA_TM_CLASS_NEAR_OK_5                 ADDR(        0x1ca607,          MAC_NOT_LOOKED_UP)
#define RVA_TM_CLASS_NEAR_OK_6                 ADDR(        0x31c76e,          MAC_NOT_LOOKED_UP)
#define RVA_TM_CLASS_MAC_EXCLUDE_1             ADDR(        WIN_NOT_LOOKED_UP, 0x1003704b4)
#define RVA_TM_CLASS_MAC_EXCLUDE_2             ADDR(        WIN_NOT_LOOKED_UP, 0x100370510)
#define RVA_TM_MONSTER_NAMED_1                 ADDR(        0x0a2c72,          MAC_NOT_LOOKED_UP)
#define RVA_TM_MONSTER_NAMED_2                 ADDR(        0x0a2c8d,          MAC_NOT_LOOKED_UP)
#define RVA_TM_MONSTER_NAMED_3                 ADDR(        0x0a2cf1,          MAC_NOT_LOOKED_UP)
#define RVA_TM_MONSTER_NAMED_4                 ADDR(        0x0ba663,          MAC_NOT_LOOKED_UP)
#define RVA_TM_MONSTER_NAMED_5                 ADDR(        0x0bad18,          MAC_NOT_LOOKED_UP)
#define RVA_TM_MONSTER_NAMED_6                 ADDR(        0x1041af,          MAC_NOT_LOOKED_UP)
#define RVA_TM_MONSTER_NAMED_7                 ADDR(        0x104223,          MAC_NOT_LOOKED_UP)
#define RVA_TM_MONSTER_NAMED_8                 ADDR(        0x11d8d7,          MAC_NOT_LOOKED_UP)
#define RVA_TM_MONSTER_NAMED_9                 ADDR(        0x11f05f,          MAC_NOT_LOOKED_UP)
#define RVA_TM_MONSTER_NAMED_10                ADDR(        0x12980f,          MAC_NOT_LOOKED_UP)
#define RVA_TM_MONSTER_NAMED_11                ADDR(        0x12a4bd,          MAC_NOT_LOOKED_UP)
#define RVA_TM_MONSTER_NAMED_12                ADDR(        0x09b011,          MAC_NOT_LOOKED_UP)
#define RVA_TM_MONSTER_NAMED_13                ADDR(        0x0bf043,          MAC_NOT_LOOKED_UP)
#define RVA_TM_MONSTER_NAMED_14                ADDR(        0x104113,          MAC_NOT_LOOKED_UP)
#define RVA_TM_MONSTER_NAMED_15                ADDR(        0x11da1a,          MAC_NOT_LOOKED_UP)
#define RVA_TM_MONSTER_NAMED_16                ADDR(        0x1204d8,          MAC_NOT_LOOKED_UP)
#define RVA_TM_MONSTER_NAMED_17                ADDR(        0x1dfe15,          MAC_NOT_LOOKED_UP)
#define RVA_TM_MONSTER_NAMED_18                ADDR(        0x1dffe0,          MAC_NOT_LOOKED_UP)
#define RVA_TM_MONSTER_NEAR_OK_1               ADDR(        0x1cc22f,          MAC_NOT_LOOKED_UP)
#define RVA_TM_MONSTER_NEAR_OK_2               ADDR(        0x1cc250,          MAC_NOT_LOOKED_UP)
#define RVA_TM_MONSTER_NEAR_OK_3               ADDR(        0x1cc264,          MAC_NOT_LOOKED_UP)
#define RVA_TM_MONSTER_NEAR_OK_4               ADDR(        0x1cc279,          MAC_NOT_LOOKED_UP)
#define RVA_TM_MONSTER_NEAR_OK_5               ADDR(        0x1cc28e,          MAC_NOT_LOOKED_UP)
#define RVA_TM_MONSTER_NEAR_OK_6               ADDR(        0x1cc2a3,          MAC_NOT_LOOKED_UP)
#define RVA_TM_MONSTER_NEAR_OK_7               ADDR(        0x1cc2f0,          MAC_NOT_LOOKED_UP)
#define RVA_TM_MONSTER_NEAR_OK_8               ADDR(        0x1cc304,          MAC_NOT_LOOKED_UP)
#define RVA_TM_MONSTER_NEAR_OK_9               ADDR(        0x1cc319,          MAC_NOT_LOOKED_UP)
#define RVA_TM_MONSTER_NEAR_OK_10              ADDR(        0x1cc32e,          MAC_NOT_LOOKED_UP)
#define RVA_TM_MONSTER_NEAR_OK_11              ADDR(        0x1cc343,          MAC_NOT_LOOKED_UP)
#define RVA_TM_MONSTER_NEAR_OK_12              ADDR(        0x1cc370,          MAC_NOT_LOOKED_UP)
#define RVA_TM_MONSTER_NEAR_OK_13              ADDR(        0x1cc384,          MAC_NOT_LOOKED_UP)
#define RVA_TM_MONSTER_NEAR_OK_14              ADDR(        0x1cc399,          MAC_NOT_LOOKED_UP)
#define RVA_TM_MONSTER_NEAR_OK_15              ADDR(        0x1cc3ae,          MAC_NOT_LOOKED_UP)
#define RVA_TM_MONSTER_NEAR_OK_16              ADDR(        0x1cc3c3,          MAC_NOT_LOOKED_UP)
#define RVA_TM_MONSTER_NEAR_OK_17              ADDR(        0x1cc400,          MAC_NOT_LOOKED_UP)
#define RVA_TM_MONSTER_NEAR_OK_18              ADDR(        0x1cc414,          MAC_NOT_LOOKED_UP)
#define RVA_TM_MONSTER_NEAR_OK_19              ADDR(        0x1cc429,          MAC_NOT_LOOKED_UP)
#define RVA_TM_MONSTER_NEAR_OK_20              ADDR(        0x1cc43e,          MAC_NOT_LOOKED_UP)
#define RVA_TM_MONSTER_NEAR_OK_21              ADDR(        0x1cc453,          MAC_NOT_LOOKED_UP)
#define RVA_TM_MONSTER_NEAR_OK_22              ADDR(        0x1d9cdd,          MAC_NOT_LOOKED_UP)
#define RVA_TM_MONSTER_NEAR_OK_23              ADDR(        0x1d9adf,          MAC_NOT_LOOKED_UP)
#define RVA_TM_MONSTER_MAC_NAMED_1             ADDR(        WIN_NOT_LOOKED_UP, 0x100464098)
#define RVA_TM_MONSTER_MAC_NAMED_2             ADDR(        WIN_NOT_LOOKED_UP, 0x1003f79c4)
#define RVA_TM_MONSTER_MAC_NAMED_3             ADDR(        WIN_NOT_LOOKED_UP, 0x1003f7adc)
#define RVA_TM_MONSTER_MAC_NAMED_4             ADDR(        WIN_NOT_LOOKED_UP, 0x1003f7c08)
#define RVA_TM_MONSTER_MAC_NAMED_5             ADDR(        WIN_NOT_LOOKED_UP, 0x1003f7d50)
#define RVA_TM_MONSTER_MAC_NAMED_6             ADDR(        WIN_NOT_LOOKED_UP, 0x100482a34)
#define RVA_TM_MONSTER_MAC_NAMED_7             ADDR(        WIN_NOT_LOOKED_UP, 0x100372578)
#define RVA_TM_MONSTER_MAC_NAMED_8             ADDR(        WIN_NOT_LOOKED_UP, 0x10037bca4)
#define RVA_TM_ARMY_LIST_COUNTS_NOT_REFS_1     ADDR(        0x12723e,          MAC_NOT_LOOKED_UP)
#define RVA_TM_ARMY_LIST_COUNTS_NOT_REFS_2     ADDR(        0x127249,          MAC_NOT_LOOKED_UP)
#define RVA_TM_ARMY_LIST_LEAST_B_NOT_REFS_1    ADDR(        0x0a0a4b,          MAC_NOT_LOOKED_UP)
#define RVA_TM_ARMY_LIST_LEAST_B_NOT_REFS_2    ADDR(        0x0f1ba4,          MAC_NOT_LOOKED_UP)
#define RVA_TM_ARMY_LIST_LEAST_B_NOT_REFS_3    ADDR(        0x10ff42,          MAC_NOT_LOOKED_UP)
#define RVA_TM_ARMY_LIST_LEAST_B_NOT_REFS_4    ADDR(        0x18622a,          MAC_NOT_LOOKED_UP)
#define RVA_TM_ARMY_LIST_LEAST_B_NOT_REFS_5    ADDR(        0x18dc83,          MAC_NOT_LOOKED_UP)
#define RVA_TM_ARMY_LIST_LEAST_B_NOT_REFS_6    ADDR(        0x18dcd4,          MAC_NOT_LOOKED_UP)
#define RVA_TM_ARMY_LIST_LEAST_B_NOT_REFS_7    ADDR(        0x18dce3,          MAC_NOT_LOOKED_UP)
#define RVA_TM_ARMY_LIST_LEAST_B_NOT_REFS_8    ADDR(        0x1e84c4,          MAC_NOT_LOOKED_UP)
#define RVA_TM_CLASS_COPY_NOT_REFS_1           ADDR(        0x0a7597,          MAC_NOT_LOOKED_UP)
#define RVA_TM_CLASS_COPY_NEAR_OK_1            ADDR(        0x0a5136,          MAC_NOT_LOOKED_UP)
#define RVA_TM_CLASS_COPY_NEAR_OK_2            ADDR(        0x0a55d9,          MAC_NOT_LOOKED_UP)
#define RVA_TM_CLASS_COPY_NEAR_OK_3            ADDR(        0x0a5698,          MAC_NOT_LOOKED_UP)
#define RVA_TM_CLASS_COPY_NEAR_OK_4            ADDR(        0x0a5790,          MAC_NOT_LOOKED_UP)
#define RVA_TM_CLASS_COPY_NEAR_OK_5            ADDR(        0x0a5880,          MAC_NOT_LOOKED_UP)
#define RVA_TM_CLASS_COPY_NEAR_OK_6            ADDR(        0x0a5d9e,          MAC_NOT_LOOKED_UP)
#define RVA_TM_CLASS_COPY_NEAR_OK_7            ADDR(        0x0a61c1,          MAC_NOT_LOOKED_UP)
#define RVA_TM_CLASS_COPY_NEAR_OK_8            ADDR(        0x0a62fc,          MAC_NOT_LOOKED_UP)
#define RVA_TM_CLASS_COPY_NEAR_OK_9            ADDR(        0x0a6580,          MAC_NOT_LOOKED_UP)
#define RVA_TM_CLASS_COPY_MAC_SIZE_EXCLUDE_1   ADDR(        WIN_NOT_LOOKED_UP, 0x1003310c8)
#define RVA_TM_CLASS_COPY_MAC_SIZE_EXCLUDE_2   ADDR(        WIN_NOT_LOOKED_UP, 0x1003324c8)
#define RVA_TM_CLASS_COPY_MAC_SIZE_EXCLUDE_3   ADDR(        WIN_NOT_LOOKED_UP, 0x1003913d0)
#define RVA_TM_CLASS_COPY_MAC_SIZE_EXCLUDE_4   ADDR(        WIN_NOT_LOOKED_UP, 0x100529294)

#endif

// ---- Call sites -----------------------------------------------------------------------------------
#if defined(GAME_ADDRESSES_SITE_TABLE) && !defined(GAME_ADDRESSES_SITE_TABLE_DONE)
#define GAME_ADDRESSES_SITE_TABLE_DONE

#define MAX_SITE_CALLS                         72

typedef struct {
    unsigned int rvas[MAX_SITE_CALLS], targetRva;
    const char *name;
    int captureMenuFade;
} SiteInfo;

#define SITE_CALLS(...) __VA_ARGS__
#ifdef __APPLE__
#define M(va) MAC(va)
#define SITE(name, fade, winCalls, winTarget, macCalls, macTarget) {{SITE_CALLS macCalls}, macTarget, name, fade}
#else
#define SITE(name, fade, winCalls, winTarget, macCalls, macTarget) {{SITE_CALLS winCalls}, winTarget, name, fade}
#endif
static const SiteInfo SITES[SITE_COUNT] = {
    [SITE_MAIN_MENU_WAIT] = SITE("main_menu_wait", 1,
 (0x192092),       0x23ec30,
 (M(0x10032e068)), M(0x100485000)),
    [SITE_MAIN_MENU_PLAY_GAME] = SITE("main_menu_play_game", 0,
 (0x1920fb),       0x0917d0,
 (M(0x10032e0c8)), M(0x100389628)),
    [SITE_PLAY_GAME_NEW_WORLD] = SITE("play_game_new_world", 0,
 (0x0918f0),       0x302cd0,
 (M(0x100389a4c)), M(0x10058708c)),
    [SITE_NEW_WORLD_MAP_SCREEN] = SITE("new_world_map_screen", 0,
 (0x302e15),       0x2a9f90,
 (M(0x100587290)), M(0x1004b255c)),
    [SITE_NEW_WORLD_INIT_PARTICIPANTS] = SITE("new_world_init_participants", 0,
 (0x30315d), 0x2e9d10,
 (0),        0),
    [SITE_NEW_WORLD_CREATING_WORLD] = SITE("new_world_creating_world", 0,
 (0x30497f),       0x089740,
 (M(0x100589264)), M(0x1004901c8)),
    [SITE_MAP_SETTINGS_WAIT] = SITE("map_settings_wait", 0,
 (0x2aabd7),       0x23ec30,
 (M(0x1004b2a8c)), M(0x100485000)),
    [SITE_PARTICIPANTS_WAIT] = SITE("participants_wait", 0,
 (0x304205),       0x23ec30,
 (M(0x1005885c8)), M(0x100485000)),
    [SITE_PARTICIPANTS_DIFFICULTY_WAIT] = SITE("participants_difficulty_wait", 0,
 (0x3029b5),       0x23ec30,
 (M(0x10058689c)), M(0x100485000)),
    // @platform-split participants_button_row: Mac's chain ends in CoE5SE's copy of button_row
    [SITE_PARTICIPANTS_BUTTON_ROW] = SITE("participants_button_row", 0,
 (0x303bc4),                       0x03a3f0,
 (M(0x100588850), M(0x1005882f4)), M(0x10033c998)),
    [SITE_GAME_MENU_SAVING_SECTION_END] = SITE("game_menu_saving_section_end", 0,
 (0x12440d),       0x035400,
 (M(0x10038053c)), M(0x1003408a8)),
    [SITE_GAME_MENU_WAIT] = SITE("game_menu_wait", 0,
 (0x12485c),       0x23ec30,
 (M(0x1003807ac)), M(0x100485000)),
    [SITE_RITUAL_SCREEN_ROW] = SITE("ritual_screen_row", 0,
 (0x2afb56),       0x2d6690,
 (M(0x1003602c8)), M(0x100351b44)),
    [SITE_RITUAL_SCREEN_LOCATION_CHECK] = SITE("ritual_screen_location_check", 0,
 (0x2afab4),       0x2ccea0,
 (M(0x1003600f0)), M(0x1005596bc)),
    [SITE_RITUAL_SCREEN_CAN_CAST] = SITE("ritual_screen_can_cast", 0,
 (0x2afac1),       0x2c9970,
 (M(0x100360100)), M(0x10055bcfc)),
    [SITE_MAP_SCREEN_WAIT] = SITE("map_screen_wait", 0,
 (0x097157, 0x1e9957),             0x23ec30,
 (M(0x10037febc), M(0x10050a6b4)), M(0x100485000)),
    [SITE_MAP_MENU_BUTTON_TEXT] = SITE("map_menu_button_text", 0,
 (0x09d711),       0x038f60,
 (M(0x10037ade8)), M(0x10033d804)),
    [SITE_GAME_START] = SITE("game_start", 0,
 (0x091b8f),       0x1b07a0,
 (M(0x10038a074)), M(0x100491d44)),
    [SITE_MONTH_START] = SITE("month_start", 0,
 (0x091d6c),       0x089740,
 (M(0x10038a3a4)), M(0x1004901c8)),
    [SITE_PLAYER_TURN_START] = SITE("player_turn_start", 0,
 (0x091f1f, 0x0921b6, 0x0922ce),                   0x167600,
 (M(0x10038a5ec), M(0x10038a774), M(0x10038a9a0)), M(0x100392658)),
    [SITE_HUMAN_TURN] = SITE("human_turn", 0,
 (0x0924bf),       0x0944c0,
 (M(0x10038ae34)), M(0x10037afe8)),
    [SITE_AI_TURN] = SITE("ai_turn", 0,
 (0x091f51),       0x06f9d0,
 (M(0x10038a9dc)), M(0x1004e0590)),
    [SITE_PLAYER_TURN_END] = SITE("player_turn_end", 0,
 (0x091fd2),       0x1e17c0,
 (M(0x10038bd90)), M(0x1004c54d0)),
    // @platform-split game_over: Mac's call is the Game Over screen's, not game_over's
    [SITE_GAME_OVER] = SITE("game_over", 0,
 (0x093b2c),       0x10cbd0,
 (M(0x10038fe8c)), M(0x100473c1c)),
    // @platform-split game_save: Windows' calls pass (name, FILE *); CoE5SE's outermost wrapper passes (path, name) on
    [SITE_GAME_SAVE] = SITE("game_save", 0,
 (0x091c2e, 0x092495, 0x093f6b, 0x18b20c),         0x18e460,
 (M(0x10038a124), M(0x10038adfc), M(0x1004f4b3c)), M(0x1004f1f1c)),
    [SITE_GAME_LOAD] = SITE("game_load", 0,
 (0x18ac76, 0x190c65, 0x190e71),                   0x185820,
 (M(0x10032e16c), M(0x10033321c), M(0x1003336ec)), M(0x1004eb1ec)),
    [SITE_MODS_RUN] = SITE("mods_run", 0,
 (0x091a8c, 0x093b60, 0x093e81, 0x167692, 0x185a56, 0x190ba9, 0x1ca2ff),                           0x1cb2c0,
 (M(0x100389b6c), M(0x10039143c), M(0x10039271c), M(0x1004eb7a8), M(0x1003328e4), M(0x10032ef2c)), M(0x10053a030)),
    [SITE_MOD_COMMAND] = SITE("mod_command", 0,
 (0x1bb1ac),       0x1bb4b0,
 (M(0x10052a37c)), M(0x10052ec70)),
    [SITE_SQUARE_INFO_SCREEN] = SITE("square_info_screen", 0,
 (0x097713),       0x3313f0,
 (M(0x1003840f0)), M(0x1004c9c08)),
    // @platform-split square_info_button_row: Mac's chain ends in CoE5SE's copy of button_row
    [SITE_SQUARE_INFO_BUTTON_ROW] = SITE("square_info_button_row", 0,
 (0x33422c),       0x03a3f0,
 (M(0x1004cd304)), M(0x10033c998)),
    [SITE_SQUARE_INFO_WAIT] = SITE("square_info_wait", 0,
 (0x334495),       0x23ec30,
 (M(0x1004cd3e8)), M(0x100485000)),
    [SITE_TEXT_FORMAT] = SITE("text_format", 0,
 (0x0336a9, 0x039b4d, 0x039fbd, 0x039dfe, 0x039d1d, 0x03982d, 0x039a80, 0x038dc6, 0x038a15,
                       0x038bd1, 0x039216, 0x0361b4, 0x036473, 0x038ff6, 0x042fbe, 0x042bc0), 0x005724,
 (0),                                                                    0),
    [SITE_TEXT_BUILD] = SITE("text_build", 0,
 (0x0f85d3, 0x0f864b, 0x0f89b1, 0x0f8adc, 0x0f8afb, 0x11896c, 0x2af8c2, 0x2af926, 0x2af9b4,
                       0x2afa09, 0x2afa93, 0x2afadf, 0x2afcd7, 0x2afd45, 0x2afd53, 0x2c21a7, 0x2c21f6, 0x2c2268,
                       0x2c22d8, 0x2c2348, 0x2c23ba, 0x2c242c), 0x001430,
 (0),                                      0),
    [SITE_LOAD_GAME_BROWSER] = SITE("load_game_browser", 0,
 (0x18abe4, 0x18ac42), 0x03f430,
 (M(0x10032e148)),     M(0x10034a1b8)),
    [SITE_FILE_BROWSER_ROW_DATE] = SITE("file_browser_row_date", 0,
 (0x040cde),       0x039ad0,
 (M(0x10034affc)), M(0x10033d0c8)),
    [SITE_FILE_BROWSER_WAIT] = SITE("file_browser_wait", 0,
 (0x04118a),       0x23ec30,
 (M(0x10034b524)), M(0x100485000)),
    [SITE_FILE_BROWSER_HOVERED] = SITE("file_browser_hovered", 0,
 (0), 0,
 (0), 0),
    [SITE_MAIN_MENU_MODS_SCREEN] = SITE("main_menu_mods_screen", 0,
 (0x1921eb),       0x1c9b40,
 (M(0x10032ef30)), M(0x10032a310)),
    // @platform-split mod_banner_draw: Mac's call goes to a copy with another signature
    [SITE_MOD_BANNER_DRAW] = SITE("mod_banner_draw", 0,
 (0x23f7e7),       0x02f9c0,
 (M(0x100488adc)), M(0x1003452ac)),
    [SITE_MOD_ERROR_DIALOG] = SITE("mod_error_dialog", 0,
 (0x1c9514),       0x041f50,
 (M(0x100529c80)), M(0x10034940c)),
    [SITE_GAME_OVER_WAIT] = SITE("game_over_wait", 0,
 (0x10d033),       0x23ec30,
 (M(0x1004743cc)), M(0x100485000)),
    [SITE_EVENT_LINE] = SITE("event_line", 0,
 (0x1bb188, 0x1cb1b6),                             0x1c5210,
 (M(0x10052a1b8), M(0x10052a358), M(0x100539de8)), M(0x10052a4dc)),
    [SITE_ARMY_WAIT] = SITE("army_wait", 0,
 (0x120550),       0x23ec30,
 (M(0x100384b24)), M(0x100485000)),
    [SITE_BATTLE_WAIT] = SITE("battle_wait", 0,
 (0x0ffec0),       0x23ec30,
 (M(0x10058283c)), M(0x100485000)),
    [SITE_MAP_EDITOR_WAIT] = SITE("map_editor_wait", 0,
 (0x1b878f),       0x23ec30,
 (M(0x100515704)), M(0x100485000)),
    [SITE_BATTLE_EDITOR_WAIT] = SITE("battle_editor_wait", 0,
 (0x0859e8),       0x23ec30,
 (M(0x100519888)), M(0x100485000)),
    [SITE_RECRUIT_WAIT] = SITE("recruit_wait", 0,
 (0x1269f6),       0x23ec30,
 (M(0x10047c5bc)), M(0x100485000)),
    [SITE_VIEW_ARMY_WAIT] = SITE("view_army_wait", 0,
 (0x122322),       0x23ec30,
 (M(0x10047df58)), M(0x100485000)),
    [SITE_UNIT_LIST_WAIT] = SITE("unit_list_wait", 0,
 (0x03f0d6),       0x23ec30,
 (M(0x100584324)), M(0x100485000)),
    [SITE_KEYBOARD_SHORTCUTS_WAIT] = SITE("keyboard_shortcuts_wait", 0,
 (0x12cab0),       0x23ec30,
 (M(0x100512538)), M(0x100485000)),
    // @platform-split unit_list_sheet: no Mac call
    [SITE_UNIT_LIST_SHEET] = SITE("unit_list_sheet", 0,
 (0x0fa51d), 0x03e480,
 (0),        0),
    // @platform-split poll_event: Windows' calls go through the import slot
    [SITE_POLL_EVENT] = SITE("poll_event", 0,
 (0x04eafa, 0x04f295), 0x33a580,
 (M(0x1004853b0)),     M(0x10000f764)),
    [SITE_KEY_TEXT_BAR] = SITE("key_text_bar", 0,
 (0x09c28c, 0x09d3a4, 0x09d408, 0x09d45c, 0x09d4b0, 0x09d520, 0x09d5c4, 0x09d618, 0x09d67d,
                       0x09d6ac, 0x099323, 0x0993d7, 0x099460, 0x0994ea, 0x099ba5, 0x09a1a5, 0x09a5f0, 0x09a801),       0x038f60,
 (M(0x100377614), M(0x100379924), M(0x100379c6c), M(0x100379e98), M(0x10037a0c8), M(0x10037a310),
                       M(0x10037a620), M(0x10037a84c), M(0x10037aa84), M(0x10037ab9c), M(0x10037d49c), M(0x10037d5cc),
                       M(0x10037d748), M(0x10037d844), M(0x10037de00), M(0x10037e6c0), M(0x10037e9c8), M(0x10037ecd0)), M(0x10033d804)),
    [SITE_KEY_HELP_TEXT] = SITE("key_help_text", 0,
 (0x1b71ce, 0x1b71eb, 0x1b7208, 0x1b7247, 0x1b7297),                               0x039180,
 (M(0x100514b00), M(0x100514b1c), M(0x100514b38), M(0x100514b74), M(0x100514bc0)), M(0x10033d66c)),
    [SITE_KEY_TEXT_BUTTON] = SITE("key_text_button", 0,
 (0x0a042c),       0x039ad0,
 (M(0x10037681c)), M(0x10033d0c8)),
    [SITE_RUN_ONE_MOD] = SITE("run_one_mod", 0,
 (0x1cb432, 0x1ca50a),                                                             0x1badd0,
 (M(0x10053a3a0), M(0x10053a3ec), M(0x10053a438), M(0x10053a484), M(0x10053a4d0), M(0x10053a51c),
                       M(0x10053a568), M(0x10053a5b4), M(0x10053a600), M(0x1004eb6c0), M(0x1004f20f4)), M(0x100529d64)),
    [SITE_RUN_ONE_MOD_PATH] = SITE("run_one_mod_path", 0,
 (0x1baee2),       0x001430,
 (M(0x100529e98)), M(0x1005aed34)),
    [SITE_RUN_ONE_MOD_LINE_READ] = SITE("run_one_mod_line_read", 0,
 (0x1bb09f),                       0x00f950,
 (M(0x10052a07c), M(0x10052a21c)), M(0x1005ae3f8)),
    [SITE_ELIMINATED_WARN] = SITE("eliminated_warn", 0,
 (0x110b22),       0x042f40,
 (M(0x1004729cc)), M(0x100348090)),
    [SITE_WARN_WAIT] = SITE("warn_wait", 0,
 (0x043166, 0x043186),             0x23ec30,
 (M(0x1003483a4), M(0x100348348)), M(0x100485000)),
    [SITE_GAME_MENU_SETTINGS_SECTION_END] = SITE("game_menu_settings_section_end", 0,
 (0x124178),       0x035400,
 (M(0x1003803cc)), M(0x1003408a8)),
    // @platform-split tooltip_line_draw: Mac's call is RVA_RENDER_TEXT, with another signature
    [SITE_TOOLTIP_LINE_DRAW] = SITE("tooltip_line_draw", 0,
 (0x02a24b),       0x02f380,
 (M(0x10048bbec)), M(0x10034d6a8)),
    [SITE_TURN_MESSAGES] = SITE("turn_messages", 0,
 (0x0947c7),       0x1e1ba0,
 (M(0x10037b650)), M(0x1004c2498)),
    [SITE_MESSAGES_WAIT] = SITE("messages_wait", 0,
 (0x1e2379),       0x23ec30,
 (M(0x1004c2cf0)), M(0x100485000)),
    // @platform-split ai_steps_mac: no Mac call; hooks at each end of the copied step
    [SITE_AI_EXPLORE] = SITE("ai_explore", 0,
 (0x070c72), 0x074260,
 (0),        0),
    // @platform-split ai_steps_mac: rally, friendly move and conquest have no Mac call
    [SITE_AI_RALLY] = SITE("ai_rally", 0,
 (0x070b92), 0x076640,
 (0),        0),
    [SITE_AI_FRIENDLY_MOVE] = SITE("ai_friendly_move", 0,
 (0x070bdb), 0x076230,
 (0),        0),
    [SITE_AI_CONQUER] = SITE("ai_conquer", 0,
 (0x070c24), 0x075310,
 (0),        0),
    [SITE_AI_FAR_EXPLORE] = SITE("ai_far_explore", 0,
 (0x070cc0),       0x073d50,
 (M(0x1004da5a4)), M(0x1004d3990)),
    [SITE_AI_RITUAL_CANDIDATE] = SITE("ai_ritual_candidate", 0,
 (0x06bb37),       0x2cb840,
 (M(0x1004ddfe8)), M(0x10055c5b0)),
    // @platform-split ai_steps_mac: no Mac call; hooks at each end of the copied step
    [SITE_AI_RECRUIT] = SITE("ai_recruit", 0,
 (0x06fb2b), 0x0772d0,
 (0),        0),
    // @platform-split ai_steps_mac: the troop pick-up and the split have no Mac call
    [SITE_AI_GATHER_TROOPS] = SITE("ai_gather_troops", 0,
 (0x070def), 0x06edd0,
 (0),        0),
    [SITE_AI_PICK_UP_LONER] = SITE("ai_pick_up_loner", 0,
 (0x07c0fd),       0x129400,
 (M(0x1004d17b0)), M(0x100484204)),
    [SITE_AI_SPLIT_ARMY] = SITE("ai_split_army", 0,
 (0x07b3fc), 0x07b5a0,
 (0),        0),
    [SITE_AI_MOVE_COMSTR] = SITE("ai_move_comstr", 0,
 (0x070df9),       0x128c50,
 (M(0x1004db17c)), M(0x10047f82c)),
    [SITE_CEREMONY_PERFORM] = SITE("ceremony_perform", 0,
 (0x2d514c, 0x2d513c, 0x09e5a9, 0x07b292, 0x07b2b4, 0x07b2d6, 0x07b2f5, 0x07b320, 0x07b34b,
                       0x07b367, 0x07b3dc, 0x07b3ec),                                                   0x2e1c50,
 (M(0x10035f3b8), M(0x100386cec), M(0x1004d21c8), M(0x1004d21ec), M(0x1004d2210), M(0x1004d22b0),
                       M(0x1004d2438), M(0x1004d24dc), M(0x1004d24f8), M(0x1004d25e8), M(0x1004d25f8)), M(0x1003520d4)),
    [SITE_CEREMONY_COSTS] = SITE("ceremony_costs", 0,
 (0x2d38da, 0x07b270, 0x09e596),   0x2e2cd0,
 (M(0x100386cdc), M(0x1004d21a4)), M(0x1003519b0)),
    [SITE_SEAL_BREAK] = SITE("seal_break", 0,
 (0x2e21b2, 0x184ab2),             0x1f1de0,
 (M(0x1003523a4), M(0x1004e81e0)), M(0x10050ede4)),
    [SITE_SEAL_EVENT] = SITE("seal_event", 0,
 (0), 0,
 (0), 0),
    [SITE_MAP_EDITOR] = SITE("map_editor", 0,
 (0x192186, 0x1ca294, 0x190e36),                   0x1b6dc0,
 (M(0x10032e49c), M(0x10032e808), M(0x100333680)), M(0x1005146bc)),
    [SITE_MAP_EDITOR_TOOLBOX_LINE] = SITE("map_editor_toolbox_line", 0,
 (0x1b7219),       0x039ad0,
 (M(0x100514b48)), M(0x10033d0c8)),
    [SITE_MAP_EDITOR_DRAW] = SITE("map_editor_draw", 0,
 (0x24023f),       0x02cfc0,
 (M(0x10048acb0)), M(0x100345904)),
    [SITE_MAP_DRAW] = SITE("map_draw", 0,
 (0), 0x02cfc0,
 (0), 0),
    [SITE_TOP_BAR] = SITE("top_bar", 0,
 (0x094b20), 0x09bf80,
 (0),        0),
    [SITE_TOP_BAR_INCOME_DRAW] = SITE("top_bar_income_draw", 0,
 (0x09c4d1, 0x09c5b0, 0x09c8f8, 0x09c9d5, 0x09caa9, 0x09cb7d, 0x09cc51, 0x09cd40, 0x09ce2a), 0x036120,
 (0),                                                                                        0),
    [SITE_MESSAGE_BOX_WAIT] = SITE("message_box_wait", 0,
 (0x0422ec), 0x23ec30,
 (0),        0),
    [SITE_TOP_BAR_INCOME] = SITE("top_bar_income", 0,
 (0),              0,
 (0),              0),
    [SITE_WEAPON_HIT] = SITE("weapon_hit", 0,
 (0),              0,
 (0),              0),
    [SITE_BATTLE_TARGET_CHECK] = SITE("battle_target_check", 0,
 (0),              0,
 (0),              0),
    [SITE_AFTER_GAME_TURN] = SITE("after_game_turn", 0,
 (0x091c72),       0x161780,
 (M(0x10038a174)), M(0x1003a069c)),
    [SITE_DO_ALL_FIGHTS] = SITE("do_all_fights", 0,
 (0x09259e, 0x0925a8, 0x0925ad, 0x166655, 0x16665f),                               0x0f4a10,
 (M(0x10038eec4), M(0x10038eecc), M(0x10038eed0), M(0x1003b7138), M(0x1003b7140)), M(0x1003dbf84)),
    [SITE_MOVE_ALL_INDEPENDENTS] = SITE("move_all_independents", 0,
 (0x0921ef, 0x178581, 0x179c57, 0x17d4c6),         0x17a8a0,
 (M(0x10038ae64), M(0x10038b54c), M(0x10038b738)), M(0x100547c50)),
    [SITE_CONQUER_SQUARE] = SITE("conquer_square", 0,
 (0x163faf, 0x16413f, 0x1afada, 0x2c6a28, 0x2da359),                                               0x1ae620,
 (M(0x1003a97b4), M(0x1003aa004), M(0x1004ab40c), M(0x10055fc4c), M(0x100560504), M(0x10039e0e0)), M(0x1004aa750)),
    [SITE_AI_DANGEROUS_STEP] = SITE("ai_dangerous_step", 0,
 (0x070f7d, 0x17cd88, 0x17d046),                                                                   0x079630,
 (M(0x1004db2c8), M(0x10038cc5c), M(0x10038d03c), M(0x10038d41c), M(0x10038d7f0), M(0x10038dbd8)), M(0x1004d2d30)),
    [SITE_AI_MAKE_GUARDS] = SITE("ai_make_guards", 0,
 (0x0a886b, 0x07005d),             0x06ebd0,
 (M(0x1003701e8), M(0x1004e3b90)), M(0x1004e01a0)),
    [SITE_AI_RITUAL_TARGET] = SITE("ai_ritual_target", 0,
 (0x06b1f9, 0x06c04e, 0x06c3fd),                   0x06c4f0,
 (M(0x1004dd288), M(0x1004de550), M(0x1004de728)), M(0x1004df98c)),
    [SITE_DECORATE_SQUARE] = SITE("decorate_square", 0,
 (0x1b45e4, 0x1b5611, 0x1b60fc, 0x1b61ec, 0x2abe04, 0x31ad05),                                     0x2ab610,
 (M(0x10051662c), M(0x100514044), M(0x100513648), M(0x100513710), M(0x1004b0ab8), M(0x1005924d4)), M(0x1004af87c)),
    [SITE_BATTLE_DRAW] = SITE("battle_draw", 0,
 (0), 0x02cfc0,
 (0), M(0x100345904)),
    [SITE_FIGHT_THE_FIGHT] = SITE("fight_the_fight", 0,
 (0x0f5705),       0x0f1cb0,
 (M(0x1003ddcc8)), M(0x1003dfad0)),
    [SITE_BATTLE_ROUND] = SITE("battle_round", 0,
 (0x0f2465),       0x0b62b0,
 (M(0x1003e0a64)), M(0x1004c1304)),
    [SITE_DAMAGE_UNIT] = SITE("damage_unit", 0,
 (0x0b72f3, 0x0bd165, 0x0c6b9b, 0x0c8f2b, 0x0d0c36, 0x0d0d9e, 0x0d0e6a, 0x0d119e, 0x0d165e,
                       0x0d17aa, 0x0d19b3, 0x0d1b26, 0x0d1d9f, 0x0d23a8, 0x0d28ac, 0x0d36ba, 0x0d5066, 0x0d58d8,
                       0x0d5cb6, 0x0de13d, 0x0de215, 0x0de7e6, 0x0df04c, 0x0e20f1, 0x0e221e, 0x0f153c, 0x0f1566,
                       0x0f25e7, 0x0f347f, 0x113d8f, 0x11499a, 0x114aed, 0x1431ad, 0x143db8, 0x143e60, 0x1440ca,
                       0x144210, 0x1444f9, 0x1c77b7, 0x1de1d8, 0x1de215, 0x2c8ded),                     0x0bd330,
 (M(0x10039a08c), M(0x10039a0f0), M(0x10039a55c), M(0x10039a5d4), M(0x10039b4f8), M(0x1003b35ec),
                       M(0x1003b57b4), M(0x1003b5ff8), M(0x1003b6398), M(0x1003b6668), M(0x1003c5404), M(0x1003c5778),
                       M(0x1003c5848), M(0x1003c61dc), M(0x1003cac28), M(0x1003de6fc), M(0x1003de7cc), M(0x1003e0b28),
                       M(0x1003e0dd8), M(0x1003e0f6c), M(0x1003e3b54), M(0x1003e5a74), M(0x1003e5bb4), M(0x1003e5d24),
                       M(0x1003eaad0), M(0x1003ec484), M(0x1003edb30), M(0x1003edd60), M(0x1003ede1c), M(0x1003ee534),
                       M(0x1003ee824), M(0x1003eed84), M(0x1003ef420), M(0x1003ef588), M(0x1003ef5c8), M(0x1003ef6e8),
                       M(0x1003efc00), M(0x1003f22fc), M(0x1003f304c), M(0x100473150), M(0x100473190), M(0x10052e334),
                       M(0x1005623d8), M(0x100599684), M(0x1005996c0), M(0x10059b4ec), M(0x10059b524)), M(0x1003b8dac)),
    [SITE_DOATTACK] = SITE("doattack", 0,
 (0x0f32c7, 0x0f3022, 0x0d6bf6),                   0x0d40b0,
 (M(0x1003e39e4), M(0x1003e3c10), M(0x1003e41e8)), M(0x1003eb41c)),
    [SITE_BATTLE_SPELL_PICK] = SITE("battle_spell_pick", 0,
 (0x0d4454),       0x2e5370,
 (M(0x1003eba5c)), M(0x1004bd208)),
    [SITE_EXECUTE_RITUAL] = SITE("execute_ritual", 0,
 (0x06b256, 0x06c2df, 0x06c451, 0x25593b, 0x25598f, 0x255be7, 0x2b0201, 0x2bde8e, 0x2c1f36),       0x2c5720,
 (M(0x1004dd2d0), M(0x1004de598), M(0x1004de76c), M(0x10041bbd8), M(0x10041da7c), M(0x10041f758),
                       M(0x100361060), M(0x10055d6ec), M(0x100566554), M(0x100566624), M(0x1005666f4), M(0x100566794)), M(0x10055d708)),
    [SITE_NEW_UNIT] = SITE("new_unit", 0,
 (0x079d6f, 0x0864c6, 0x0874ce, 0x0be7b6, 0x0c2f4a, 0x0d7350, 0x0dc1fe, 0x0dc9bf, 0x0dec87,
                       0x0dee7e, 0x0ee3d2, 0x0f065a, 0x107f12, 0x128376, 0x1d423a, 0x1e89e1, 0x26c05d, 0x2d6243,
                       0x2d7441),       0x1297b0,
 (M(0x10035708c), M(0x1003582bc), M(0x1003b98b0), M(0x1003bdd0c), M(0x1003bdee0), M(0x1003c5d28),
                       M(0x1003c6060), M(0x1003c7ca4), M(0x1003cf404), M(0x1003e461c), M(0x1003f1134), M(0x1003f87f0),
                       M(0x10046de0c), M(0x100479a24), M(0x10047c9bc), M(0x1004e2ce0), M(0x10050c0c8), M(0x10051867c),
                       M(0x10051afa4)), M(0x10046e828)),
    [SITE_REMOVE_UNIT] = SITE("remove_unit", 0,
 (0x08649a, 0x098f08, 0x0bf10f, 0x0cfa92, 0x0e0df4, 0x0e0e08, 0x0e153d, 0x0e29bc, 0x0e2a66,
                       0x0e2d03, 0x0e2e22, 0x0e4462, 0x0ee39d, 0x0f07cd, 0x0f15e2, 0x0f1621, 0x0f1629, 0x0f3984,
                       0x0f57ea, 0x0f57fe, 0x0f6afe, 0x10797b, 0x10f9d0, 0x1139f3, 0x117845, 0x118851, 0x123da7,
                       0x12e868, 0x12e8cf, 0x145b8a, 0x178a1c, 0x178efc, 0x17924b, 0x17950b, 0x1797cb, 0x179e1c,
                       0x17ca97, 0x17cac4, 0x1c77ab, 0x1d4276, 0x1f6679, 0x2b589a, 0x2c8de0, 0x2d195c, 0x2d19eb,
                       0x2d6158, 0x304c23, 0x327502),                   0x0f8400,
 (M(0x100386eb4), M(0x1003583e4), M(0x100388568), M(0x100388638), M(0x10039e524), M(0x1003b6ce0),
                       M(0x1003b6e40), M(0x1003e6d34), M(0x100437e7c), M(0x1004384e4), M(0x1004387f0), M(0x100438dcc),
                       M(0x100467b14), M(0x100469810), M(0x100469de8), M(0x10046ad14), M(0x10046ada0), M(0x10046d814),
                       M(0x1004731f8), M(0x10047ca00), M(0x10047e1fc), M(0x100481c28), M(0x1004febdc), M(0x100519e80),
                       M(0x100519fc0), M(0x10051af7c), M(0x10052e258), M(0x100558300), M(0x10055839c), M(0x100558434),
                       M(0x1005584bc), M(0x1005613a4), M(0x100568734), M(0x100589a1c), M(0x100589a8c), M(0x100589afc),
                       M(0x100589cb8), M(0x10058bca4), M(0x10058bd30), M(0x10058bda0), M(0x10058be10), M(0x10058bea8),
                       M(0x10058bf18), M(0x10058bf88), M(0x10058bff8), M(0x10058c068), M(0x10058c0d8), M(0x10058c148),
                       M(0x10058c1b8), M(0x10058c228), M(0x10058c298), M(0x10058c308), M(0x10058c378), M(0x10058c3e8),
                       M(0x10058c458), M(0x10058c4cc), M(0x10058c544)), M(0x1003b8a6c)),
    [SITE_TURN_UNIT_UPKEEP] = SITE("turn_unit_upkeep", 0,
 (0x169b1c),       0x143a70,
 (M(0x100399874)), M(0x100399a14)),
    [SITE_SQUARE_UPKEEP] = SITE("square_upkeep", 0,
 (0x149026, 0x169a86, 0x1dfbb9),   0x1dd4e0,
 (M(0x100399944), M(0x10059ab10)), M(0x100599008)),
    [SITE_RECRUIT_BUY] = SITE("recruit_buy", 0,
 (0x127143, 0x166c8e),             0x107b80,
 (M(0x10038efcc), M(0x10047d04c)), M(0x10047982c)),
    [SITE_PROMOTE_UNIT] = SITE("promote_unit", 0,
 (0x10ef17, 0x1452ea, 0x1c7c21, 0x1c7c37, 0x2b26e3),               0x10efc0,
 (M(0x10039d0f8), M(0x1004716a8), M(0x1004717b4), M(0x10052ea7c)), M(0x10046d534)),
    [SITE_MASTERY_TRANSFORM] = SITE("mastery_transform", 0,
 (0x2c66bd, 0x2de3c6),             0x2b2b10,
 (M(0x10035d6e4), M(0x10055f908)), M(0x10055d388)),
    [SITE_BATTLE_SETUP] = SITE("battle_setup", 0,
 (0x084d29, 0x0f42ac, 0x0f5675, 0x111cbf, 0x11f9f7, 0x121fb5, 0x2b24df, 0x2c7f55, 0x2d607d,
                       0x2d6382, 0x2d7378, 0x2d7576, 0x2d7a21, 0x2d7bb0), 0x0f6b60,
 (M(0x10038415c), M(0x100356f18), M(0x100357238), M(0x1003574b8), M(0x100357788), M(0x1003580bc),
                       M(0x100358530), M(0x1003dd504), M(0x10046a934), M(0x10047dccc), M(0x1004c3b00), M(0x100518ec0),
                       M(0x100561abc), M(0x1005633e8)),                   M(0x1003cf448)),
    [SITE_BATTLE_SETUP_SIDE] = SITE("battle_setup_side", 0,
 (0x0f83ae, 0x0f83c5),                                             0x0e5da0,
 (M(0x1003dbdc8), M(0x1003dbde4), M(0x1003dd638), M(0x1003dd654)), M(0x1003d80f0)),
    [SITE_BATTLE_SETUP_AMBUSH] = SITE("battle_setup_ambush", 0,
 (0x0f8390, 0x0f83a7),                                             0x0e8d70,
 (M(0x1003dbda4), M(0x1003dbdc0), M(0x1003dd600), M(0x1003dd61c)), M(0x1003d6fe8)),
    [SITE_BATTLE_ADVANCE_UNIT] = SITE("battle_advance_unit", 0,
 (0x0f2c01, 0x0f2c81, 0x0f2cda, 0x0f2d2c),                         0x0c6c90,
 (M(0x1003e117c), M(0x1003e1224), M(0x1003e1264), M(0x1003e12e8)), M(0x1003e89b4)),
    [SITE_BATTLE_TURN_EFFECTS] = SITE("battle_turn_effects", 0,
 (0x0d085b),                       0x0d08b0,
 (M(0x1003e57b8), M(0x1003e5fb0)), M(0x1003ed6e0)),
    [SITE_BATTLE_TRANSFORM] = SITE("battle_transform", 0,
 (0x0df367, 0x0df3f6, 0x0df4f2, 0x0df5ae, 0x0df657),                               0x0e0480,
 (M(0x1003c49f8), M(0x1003c4b04), M(0x1003c4c34), M(0x1003c4fa0), M(0x1003c5044)), M(0x1003cac58)),
    [SITE_FOUND_ITEM] = SITE("found_item", 0,
 (0x0f6545, 0x17de0e, 0x180ae3, 0x198ff8),         0x0f0990,
 (M(0x1003dedcc), M(0x1004aacac), M(0x1004aacfc), M(0x1004aad58), M(0x1004aadb0), M(0x1004aae10),
                       M(0x1004aae68), M(0x1004ea24c), M(0x1004ea278), M(0x1004ea2a4), M(0x1004ea2d0), M(0x1004ea2fc),
                       M(0x1004ea328), M(0x1004ea354), M(0x1004ea380)), M(0x1003ce484)),
    [SITE_REMOVE_DEAD_PLAYERS] = SITE("remove_dead_players", 0,
 (0x0925b2, 0x0f66ff, 0x1617f8, 0x166673, 0x1ae95f, 0x1ae9cd),                                     0x11eb30,
 (M(0x10038eed4), M(0x1003a0778), M(0x1003b72c0), M(0x1003df174), M(0x1004aa9e0), M(0x1004aaa9c)), M(0x100471b38)),
    [SITE_CREATE_CITADEL] = SITE("create_citadel", 0,
 (0x0a461c, 0x0a46c3),             0x118990,
 (M(0x10036fa10), M(0x10036fac0), M(0x10036faf0), M(0x10036fb20), M(0x10036fb60), M(0x10036fb90),
                       M(0x10036fbc0), M(0x10036fbf0)), M(0x10046a360)),
    [SITE_INQUISITION_SPAWN] = SITE("inquisition_spawn", 0,
 (0x1f5f26, 0x2e24cf),             0x2d8fc0,
 (M(0x1003525b4), M(0x1004fe5a8)), M(0x100353c94)),
    [SITE_ANIMAL_SUMMONING] = SITE("animal_summoning", 0,
 (0x2c5ef8),       0x2b7860,
 (M(0x10055f9b4)), M(0x1005567a4)),
    [SITE_ORDER_SLAVE_HUNT] = SITE("order_slave_hunt", 0,
 (0x07ac9e, 0x09e2b2, 0x2c1d61, 0x2e1390),                         0x2e3120,
 (M(0x10035c6f0), M(0x100382680), M(0x1004d20e4), M(0x100566494)), M(0x100359d28)),
    [SITE_ORDER_EAT_VILLAGE] = SITE("order_eat_village", 0,
 (0x07af27, 0x098256),             0x2e1790,
 (M(0x100387b68), M(0x1004d212c)), M(0x10035b8fc)),
    [SITE_UPDATE_VOID] = SITE("update_void", 0,
 (0x166637), 0x252710,
 (0),        0),
    [SITE_VOID_SHUFFLE] = SITE("void_shuffle", 0,
 (0x162b36), 0x250e60,
 (0),        0),
    [SITE_PLANESHIFT_UNIT] = SITE("planeshift_unit", 0,
 (0x113cd1, 0x119203, 0x119232, 0x11ea24, 0x11ea59, 0x127d39, 0x127d6d, 0x162cfc, 0x166e82,
                       0x166eb6, 0x166fdb, 0x253168, 0x254d5b, 0x254dab), 0x253aa0,
 (0),                                                0),
    [SITE_NEW_PORTAL_PAIR] = SITE("new_portal_pair", 0,
 (0x0a436d, 0x0a444e, 0x164a0c, 0x164fc1, 0x164fe0, 0x184670, 0x18491d, 0x246a23, 0x246d44,
                       0x24b605, 0x24b751, 0x24b879, 0x2c6fdc, 0x31b04d, 0x31b19c), 0x253cb0,
 (0),                                                          0),
    [SITE_PLANE_DISCOVERED_PASS] = SITE("plane_discovered_pass", 0,
 (0x166664), 0x253320,
 (0),        0),
    [SITE_MOVE_INFERNO_PLAYER] = SITE("move_inferno_player", 0,
 (0x0921d0), 0x1799c0,
 (0),        0),
    [SITE_MOVE_HADES_PLAYER] = SITE("move_hades_player", 0,
 (0x0921dc), 0x178460,
 (0),        0),
    [SITE_MOVE_CELESTIAL_PLAYER] = SITE("move_celestial_player", 0,
 (0x0921e8), 0x17d280,
 (0),        0),
    [SITE_POPULATE_CELESTIAL] = SITE("populate_celestial", 0,
 (0x304dec), 0x2799f0,
 (0),        0),
    [SITE_POPULATE_SKY] = SITE("populate_sky", 0,
 (0x304dff), 0x299440,
 (M(0x10058e218)), M(0x100451888)),
    [SITE_POPULATE_AGARTHA] = SITE("populate_agartha", 0,
 (0x304e12), 0x272b20,
 (0),        0),
    [SITE_POPULATE_INFERNO] = SITE("populate_inferno", 0,
 (0x304e25), 0x28b920,
 (M(0x100590344)), M(0x10043ad34)),
    [SITE_POPULATE_HADES] = SITE("populate_hades", 0,
 (0x304e38), 0x27bc20,
 (M(0x100590358)), M(0x100446780)),
    [SITE_POPULATE_AZTLAN] = SITE("populate_aztlan", 0,
 (0x304e4b), 0x274c30,
 (M(0x10059036c)), M(0x10045d280)),
    [SITE_POPULATE_PRIMAL] = SITE("populate_primal", 0,
 (0x304e5e), 0x293340,
 (M(0x100590380)), M(0x10045873c)),
    [SITE_POPULATE_ELEMENTAL] = SITE("populate_elemental", 0,
 (0x304e71), 0x284a90,
 (M(0x100590394)), M(0x10042dd44)),
    [SITE_PLANE_RISE_TO_ELYSIUM] = SITE("plane_rise_to_elysium", 0,
 (0x1784cb), 0x252e30,
 (0),        0),
    [SITE_ENLARGE_PLANE] = SITE("enlarge_plane", 0,
 (0x250522, 0x2a32b7, 0x2a32da), 0x241ac0,
 (0),        0),
    [SITE_GEN_FLOOD_PLANE] = SITE("gen_flood_plane", 0,
 (0x1b0517, 0x24da65), 0x199240,
 (M(0x10036f6e8), M(0x1004aa0fc), M(0x10057319c)), M(0x1004aa164)),
    [SITE_ADDPLANE] = SITE("addplane", 0,
 (0x1b5c30, 0x1b5cec, 0x1b89fc, 0x1b8a6d, 0x2434a1, 0x244795, 0x24866c, 0x24adf9, 0x24cca6,
                       0x24eef5, 0x24f9cd, 0x2a36b0, 0x2a37d5), 0x2505b0,
 (M(0x1004b4928), M(0x1004b4c6c), M(0x1004b4d04), M(0x1004b4dec), M(0x100513300), M(0x100513364),
                       M(0x1005162b0), M(0x1005162d8), M(0x10056e05c), M(0x100572508), M(0x1005752f0), M(0x1005788d0),
                       M(0x100579ab8)), M(0x10056deac)),
    [SITE_LOAD_MAP_PLANE] = SITE("load_map_plane", 0,
 (0x246eb8, 0x24b3b8, 0x2a3407, 0x2a3460, 0x2a34b9, 0x2a3512, 0x2a356b, 0x2a35c4, 0x2a35f6,
                       0x2a3628), 0x1b5840,
 (M(0x1004b47e4), M(0x1004b4810), M(0x1004b4864), M(0x1004b48b8), M(0x1004b490c), M(0x1004b4b70),
                       M(0x1004b4bb0), M(0x1004b4bfc), M(0x1004b4c38), M(0x100516340), M(0x100578560)), M(0x100513bf8)),
    [SITE_UNIT_ADD_XP] = SITE("unit_add_xp", 0,
 (0x0cf1ec, 0x143c9e, 0x14485a), 0x0f1bf0,
 (0),                            0),
    [SITE_USE_SPECIAL_SITE] = SITE("use_special_site", 0,
 (0x07abb2, 0x097683), 0x0b1600,
 (M(0x100380c90), M(0x1004d1e04)), M(0x100388804)),
    [SITE_LONER_STEP] = SITE("loner_step", 0,
 (0x0920dd, 0x0920e4, 0x0920eb, 0x1671e3, 0x1671ea, 0x1671f1), 0x172f80,
 (0),                                                          0),
    [SITE_REMOVE_TEMP_MEN] = SITE("remove_temp_men", 0,
 (0x0f3925, 0x0f6bc6, 0x111e16, 0x111eb5, 0x121de0, 0x122512, 0x167017, 0x2b2aa1, 0x2d6600,
                       0x2d7e07), 0x1177b0,
 (0),        0),
    [SITE_MOVE_WANDERING_MONSTERS] = SITE("move_wandering_monsters", 0,
 (0x091fd7), 0x17abe0,
 (0),        0),
    [SITE_MOVE_PLAYER_STUPIDS] = SITE("move_player_stupids", 0,
 (0x09253b), 0x167260,
 (0),        0),
    [SITE_OVERRUN_CHECK] = SITE("overrun_check", 0,
 (0x1df16e, 0x1ebe20, 0x1f4cd9), 0x1dca30,
 (M(0x1004fcef4), M(0x1005059c0), M(0x10059da6c)), M(0x10059b0c8)),
    [SITE_KILL_UNIT_999] = SITE("kill_unit_999", 0,
 (0x1078e1, 0x1dcc44), 0x0f14e0,
 (0),                  0),
    [SITE_DAMAGE_UNIT_PLAIN] = SITE("damage_unit_plain", 0,
 (0x10feb2, 0x12db78), 0x0bd230,
 (0),                  0),
    [SITE_CONTINUE_MOVE_ORDERS] = SITE("continue_move_orders", 0,
 (0x0973e9, 0x097727, 0x097757), 0x09d9e0,
 (M(0x100380a08), M(0x100386cfc), M(0x100386d34)), M(0x100377100)),
    [SITE_HUMAN_MULTI_MOVE] = SITE("human_multi_move", 0,
 (0x098786, 0x09da95), 0x09dae0,
 (M(0x1003771fc), M(0x100387e30)), M(0x100376bd8)),
    [SITE_GROUP_MOVE] = SITE("group_move", 0,
 (0x098b67, 0x09de10), 0x1de4b0,
 (M(0x100376f40), M(0x1003882e0)), M(0x10059b598)),
    [SITE_MOVE_ORDER_ROUTE] = SITE("move_order_route", 0,
 (0x098816, 0x09da82, 0x0a0c1f), 0x09e0a0,
 (M(0x100373b14), M(0x1003741a4), M(0x1003771ec), M(0x1003872d4), M(0x100388240)), M(0x100373b1c)),
    [SITE_UNIT_REMOVE_ITEM] = SITE("unit_remove_item", 0,
 (0x073745, 0x14635a, 0x182b35, 0x1d02a3, 0x1f0d4f, 0x1f6942), 0x184fc0,
 (0),                                                          0),
    [SITE_MEMORIZE_SPELL] = SITE("memorize_spell", 0,
 (0x1d80bf, 0x2e3ca9, 0x2e63c4, 0x2e64c4, 0x2e65c4, 0x2e66c4, 0x2e6852, 0x2e6a7e), 0x2e6d70,
 (0),                                                                              0),
    [SITE_LEARN_RANDOM_SPELL] = SITE("learn_random_spell", 0,
 (0x2c6227, 0x2c66dc, 0x2c66fb, 0x2c671a, 0x2e36fe), 0x2e46d0,
 (M(0x10035ca58), M(0x1004e6908), M(0x1004e6970), M(0x1004e69d8), M(0x10055f920), M(0x100560b30)), M(0x1004beb68)),
    [SITE_GET_START_RITUALS] = SITE("get_start_rituals", 0,
 (0x10fc1c, 0x1283d5), 0x2bfe30,
 (M(0x10046d8f8), M(0x10046e298)), M(0x1005540f0)),
    [SITE_COMMANDER_LEARN_RITUAL] = SITE("commander_learn_ritual", 0,
 (0x2bfd06, 0x2bfd99, 0x2c0565, 0x2c0577, 0x2c0896, 0x2c84bf), 0x2bddb0,
 (0),                                                          0),
    [SITE_BOLSTER_BLESSING] = SITE("bolster_blessing", 0,
 (0x2c8624, 0x2e2239, 0x304d98), 0x2d5590,
 (M(0x10035243c), M(0x100560f44), M(0x100561370), M(0x10058c574), M(0x10058c584), M(0x10058c594),
                       M(0x10058c5a4), M(0x10058c5b4), M(0x10058c5c4), M(0x10058c5d4)), M(0x100352938)),
    [SITE_ADD_PLAYER_BLESSING] = SITE("add_player_blessing", 0,
 (0x2d5901, 0x2d591e), 0x2d3320,
 (M(0x1003529f0), M(0x100352eac)), M(0x1003542c0)),
    [SITE_REMOVE_ENCHANT] = SITE("remove_enchant", 0,
 (0x1f6fbf, 0x2c6b18, 0x2c6cd9), 0x2bdeb0,
 (M(0x1004fb590), M(0x10055f27c), M(0x10055f360)), M(0x100563fcc)),
    [SITE_RITUAL_PAY_COSTS] = SITE("ritual_pay_costs", 0,
 (0x06b220, 0x06c1e4, 0x06c421, 0x2b01dc, 0x2c1e11, 0x2c989d), 0x2c2510,
 (M(0x10036103c), M(0x1004dd2ac), M(0x1004de574), M(0x1004de748), M(0x100562d44), M(0x100566510),
                       M(0x1005665e0), M(0x1005666b0), M(0x100566750)), M(0x100558950)),
    [SITE_RITUAL_FAR_SUMMON_UNITS] = SITE("ritual_far_summon_units", 0,
 (0x2b3bb4, 0x2b3c01), 0x2d7210,
 (M(0x10055cd78), M(0x10055d164)), M(0x100356dd0)),
    [SITE_PRIEST_FREESPAWN] = SITE("priest_freespawn", 0,
 (0x167ac9), 0x2db5f0,
 (0),        0),
    [SITE_EVENT_KILL_SQUARE] = SITE("event_kill_square", 0,
 (0x1c73ac, 0x2895d9), 0x1187e0,
 (0),                  0),
    [SITE_GIVE_ITEM_BY_NAME] = SITE("give_item_by_name", 0,
 (0x2c9491, 0x2c9540), 0x1835e0,
 (M(0x10052dc08), M(0x1005628b4), M(0x100562c38)), M(0x1004e5710)),
    [SITE_TAKE_ITEM_BY_NAME] = SITE("take_item_by_name", 0,
 (0x1c757a), 0x17ff70,
 (M(0x10052dd8c)), M(0x1004e582c)),
    [SITE_GIVE_RANDOM_ITEM] = SITE("give_random_item", 0,
 (0x180e86, 0x18118f, 0x181440, 0x181700, 0x18197b, 0x181a76, 0x181ad6, 0x181b36, 0x181b96,
                       0x181c27, 0x181c3c, 0x181c4f, 0x181ca9, 0x181cbe, 0x181d39, 0x181d98, 0x181dab, 0x181de7,
                       0x181dfc, 0x181e37, 0x181e4c, 0x181ea6, 0x1c7603, 0x1c768f), 0x183620,
 (0),                                                          0),
    [SITE_SQUAREEVENT_START] = SITE("squareevent_start", 0,
 (0x1c5358), 0x1c8050,
 (0),        0),
    [SITE_TERRAIN_PRESENT_RESCAN] = SITE("terrain_present_rescan", 0,
 (0x1bb079, 0x1c71a7, 0x1cb0a4), 0x1c7f10,
 (0),                            0),
    [SITE_SHOP_OFFER_MESSAGE] = SITE("shop_offer_message", 0,
 (0x0a9307, 0x0a94fa, 0x0a99de, 0x0a9be3, 0x0a9de3, 0x0a9fe3, 0x0aa1e3, 0x0aa3e3, 0x0aa5e3,
                       0x0aa7ef, 0x0aaa0c, 0x0aac13, 0x0aae13, 0x0ab01e, 0x0ab35c, 0x0ab579, 0x0ab758, 0x0ab938,
                       0x0abb18, 0x0abcf8, 0x0abed8, 0x0ac0b8, 0x0ac298, 0x0ac478, 0x0ac658, 0x0ac838, 0x0aca18,
                       0x0acbf8, 0x0acdd5, 0x0acfa2, 0x0ad10f, 0x0ad170, 0x0ad24d, 0x0ad5bc, 0x0ad744, 0x0ad84a,
                       0x0ada5d, 0x0adbef, 0x0ade08, 0x0ae02c, 0x0ae2bd, 0x0ae35c, 0x0ae4cf, 0x0ae560, 0x0ae5f0,
                       0x0ae6cb, 0x0ae777, 0x0ae88b, 0x0af124), 0x0af270,
 (M(0x1003635e4), M(0x1003652f8), M(0x1003655b8), M(0x100365df8), M(0x1003660e0), M(0x1003663c8),
                       M(0x1003666ac), M(0x100366994), M(0x100366c7c), M(0x100366f64), M(0x100367258), M(0x10036754c),
                       M(0x100367820), M(0x100367af4), M(0x100367dc0), M(0x1003680f8), M(0x1003683e0), M(0x1003685dc),
                       M(0x1003687dc), M(0x1003689d8), M(0x100368bd4), M(0x100368dd0), M(0x100368fcc), M(0x1003691c8),
                       M(0x1003693c4), M(0x1003695c0), M(0x1003697bc), M(0x1003699b8), M(0x100369bb4), M(0x100369db0),
                       M(0x100369fb0), M(0x10036a1ac), M(0x10036a3ac), M(0x10036a6b0), M(0x10036ab14), M(0x10036ad54),
                       M(0x10036af80), M(0x10036b1e4), M(0x10036b42c), M(0x10036b69c), M(0x10036bac4), M(0x10036be44),
                       M(0x10036c07c), M(0x10036c384), M(0x10036c5b4), M(0x10036c7e4), M(0x10036cc0c), M(0x10036cf14),
                       M(0x10036d40c)), M(0x10036240c)),
    [SITE_WORLD_FIRE_UPDATE] = SITE("world_fire_update", 0,
 (0x1617fd), 0x1433a0,
 (0),        0),
    [SITE_EVENT_VAR_CHANGED] = SITE("event_var_changed", 0,
 (0x2c82a4, 0x2c82ce, 0x2c82f8, 0x2c8322), 0x1e6630,
 (0),                                      0),
    [SITE_CONSCRIPTION] = SITE("conscription", 0,
 (0x165ec9), 0x2da3a0,
 (0),        0),
    [SITE_GAME_STATISTICS] = SITE("game_statistics", 0,
 (0x091a3d, 0x091c6d), 0x31d1a0,
 (M(0x100389b10), M(0x10038a170)), M(0x10053bb38)),
    [SITE_ALTER_TERRAIN] = SITE("alter_terrain", 0,
 (0x0b18eb, 0x127c7b, 0x127f88, 0x1af608, 0x254ec1, 0x2c7c80, 0x2c7d29, 0x2c8b4b, 0x2c8b6c,
                       0x2db205, 0x2db262, 0x2db2a5, 0x2db2eb, 0x2db32d, 0x2db378, 0x2db499, 0x2db509, 0x2db572,
                       0x2db5ca, 0x2e08c5), 0x1f7400,
 (0),                  0),
    [SITE_RUIN_SQUARE] = SITE("ruin_square", 0,
 (0x0b1c0e, 0x123d30, 0x1436df, 0x165a6d, 0x1b975b, 0x253c51, 0x2c85db), 0x2e05b0,
 (M(0x100388f5c), M(0x1003a0b78), M(0x1003b016c), M(0x1003b040c), M(0x1003b06a8), M(0x10046a21c),
                       M(0x10051715c), M(0x1005612a0), M(0x100575014)), M(0x100356744)),
    [SITE_DESTROY_GATEWAY] = SITE("destroy_gateway", 0,
 (0x07ab75, 0x0983bf, 0x241252), 0x254e20,
 (0),                            0),
    [SITE_DIG_TUNNEL_SQUARE] = SITE("dig_tunnel_square", 0,
 (0x127f06, 0x16d9a4, 0x1dfb80, 0x2c7e03, 0x2c7e26, 0x2c7e57), 0x1af550,
 (0),                                                          0),
    [SITE_MONTH_SQUARE_RESET] = SITE("month_square_reset", 0,
 (0x16667d), 0x140580,
 (0),        0),
    [SITE_ROLL_CLASS] = SITE("roll_class", 0,
 (0x3049bc), 0x0af7b0,
 (0),        0),
    [SITE_RESET_INCOME_CACHES] = SITE("reset_income_caches", 0,
 (0x098ca0, 0x1ae9f6, 0x2c9949), 0x328db0,
 (0),                            0),
    [SITE_TEAM_START_LOCATIONS] = SITE("team_start_locations", 0,
 (0x304ccc), 0x0a50e0,
 (0),        0),
    [SITE_TRIM_TEAM_LOCATIONS] = SITE("trim_team_locations", 0,
 (0x0a5781), 0x0a5d40,
 (0),        0),
    [SITE_SWAP_TEAM_LOCATIONS] = SITE("swap_team_locations", 0,
 (0x0a5874), 0x0a62a0,
 (0),        0),
    [SITE_CREATE_CITADELS_STEP] = SITE("create_citadels_step", 0,
 (0x0a3fd2), 0x0a6b40,
 (0),        0),
    [SITE_FIX_PORTALS] = SITE("fix_portals", 0,
 (0x16662d), 0x250970,
 (0),        0),
    [SITE_VOID_FLUCTUATIONS] = SITE("void_fluctuations", 0,
 (0x162b31), 0x251970,
 (0),        0),
    [SITE_PLANESHIFT_COMMANDER] = SITE("planeshift_commander", 0,
 (0x0981c4, 0x0981d7), 0x254920,
 (0),                  0),
    [SITE_MIST_SPREAD] = SITE("mist_spread", 0,
 (0x16317b), 0x12fb90,
 (0),        0),
    [SITE_TEMPERATURE_CHANGE] = SITE("temperature_change", 0,
 (0x091a42, 0x162b61), 0x0b3aa0,
 (M(0x100389b14), M(0x1003a3e98)), M(0x100551978)),
    [SITE_SPECIAL_SITE_POPULATE] = SITE("special_site_populate", 0,
 (0x1633cd), 0x140dc0,
 (0),        0),
    [SITE_UNIT_SWALLOW] = SITE("unit_swallow", 0,
 (0x0de93d), 0x0e2d50,
 (M(0x1003c59dc)), M(0x1003ca974)),
    [SITE_UNIT_UNSWALLOW] = SITE("unit_unswallow", 0,
 (0x0bf0d0), 0x0e2aa0,
 (M(0x1003ba7e4), M(0x1003dea88)), M(0x1003bc784)),
    [SITE_LIFEFORCE_DRAIN_UNIT] = SITE("lifeforce_drain_unit", 0,
 (0x0d5967, 0x0d5990), 0x0d59e0,
 (M(0x1003ec168), M(0x1003ec320), M(0x1003ec358)), M(0x1003f1b48)),
    [SITE_UNIT_CATCH_FIRE] = SITE("unit_catch_fire", 0,
 (0x0d2b09, 0x0d2b3d), 0x0d2b70,
 (M(0x1003eefa8), M(0x1003ef028)), M(0x1003f152c)),
    [SITE_UNIT_CRUSH] = SITE("unit_crush", 0,
 (0x0d31c9, 0x0d321d), 0x0d3290,
 (M(0x1003eaf2c), M(0x1003eafa0)), M(0x1003f2c84)),
    [SITE_REMOVE_BAD_FOLLOWERS] = SITE("remove_bad_followers", 0,
 (0x107b1a, 0x114141), 0x110ba0,
 (0),                  0),
    [SITE_OVERPOP_CULL] = SITE("overpop_cull", 0,
 (0x166650), 0x12e4c0,
 (0),        0),
    [SITE_MONKEY_CLOAK] = SITE("monkey_cloak", 0,
 (0x167012), 0x12caf0,
 (0),        0),
    [SITE_RECYCLE_UNITS] = SITE("recycle_units", 0,
 (0x166678), 0x12d150,
 (0),        0),
    [SITE_RANDOM_TELEPORT] = SITE("random_teleport", 0,
 (0x18426a, 0x1842b8, 0x1843a5), 0x17e2f0,
 (M(0x1004e79d0), M(0x1004e79fc), M(0x1004e7ac8)), M(0x1004e6350)),
    [SITE_POP_HORRORS] = SITE("pop_horrors", 0,
 (0x162b82, 0x163331), 0x142410,
 (M(0x1003a3eb8), M(0x1003a65bc)), M(0x1003b8498)),
    [SITE_KILL_TERRAIN_UNITS] = SITE("kill_terrain_units", 0,
 (0x0f5830), 0x0e1600,
 (0),        0),
    [SITE_KILL_SUNKEN_SHIPS] = SITE("kill_sunken_ships", 0,
 (0x0f3920), 0x0e0e50,
 (0),        0),
    [SITE_CREATE_BOATS] = SITE("create_boats", 0,
 (0x0f7e44), 0x0ef5b0,
 (0),        0),
    [SITE_BATTLE_TRAPS] = SITE("battle_traps", 0,
 (0x0f292e), 0x0c32b0,
 (0),        0),
    [SITE_BATTLE_ENCHANT_TRAPS] = SITE("battle_enchant_traps", 0,
 (0x0f7e28), 0x0c4bd0,
 (0),        0),
    [SITE_BATTLE_SUMMON_STRING] = SITE("battle_summon_string", 0,
 (0x0d6b22), 0x0d6c60,
 (0),        0),
    [SITE_PAY_SPELL_LIFEFORCE] = SITE("pay_spell_lifeforce", 0,
 (0x0d46b9), 0x0d5080,
 (0),        0),
    [SITE_WORLD_NEWS] = SITE("world_news", 0,
 (0x162a22, 0x162a85, 0x162ae4, 0x162e02, 0x162e39, 0x16335c, 0x1633af, 0x1633eb, 0x16342f,
                       0x16349f, 0x1635d0, 0x16361d, 0x1639c4, 0x163a0f, 0x163c78, 0x163c9e, 0x163e78, 0x1641e6,
                       0x1652e2, 0x272510), 0x106680,
 (0),                  0),
    [SITE_SEASON_NEWS] = SITE("season_news", 0,
 (0x162b1d), 0x103ee0,
 (0),        0),
    [SITE_CHECK_GAME_WON] = SITE("check_game_won", 0,
 (0x107b43), 0x1107e0,
 (0),        0),
    [SITE_SPY_CAUGHT] = SITE("spy_caught", 0,
 (0x149dc7, 0x1df1e4), 0x1138a0,
 (M(0x1003929f0), M(0x10059dc38)), M(0x100467a0c)),
    [SITE_TOGGLE_AUTO_RECRUIT] = SITE("toggle_auto_recruit", 0,
 (0x126bc3, 0x1271ae), 0x1082f0,
 (0),                  0),
    [SITE_CHANGE_LOYALTY] = SITE("change_loyalty", 0,
 (0x162f39, 0x17242f, 0x17855a), 0x1734b0,
 (0),                            0),
    [SITE_ALTER_TERRAIN_RECT] = SITE("alter_terrain_rect", 0,
 (0x163162, 0x16331a), 0x32d7a0,
 (M(0x1003a49b0), M(0x1003a648c)), M(0x1004c6fe4)),
    [SITE_RESPAWN_VOID] = SITE("respawn_void", 0,
 (0x166632), 0x2a03f0,
 (0),        0),
    [SITE_BATTLE_SIEGE_UNIT] = SITE("battle_siege_unit", 0,
 (0x0c7353, 0x0f28e8), 0x0c94a0,
 (M(0x1003e0c5c), M(0x1003e9fdc)), M(0x1003e803c)),
    [SITE_GATHER_SHIPS] = SITE("gather_ships", 0,
 (0x0f2b9d), 0x0c4e60,
 (0),        0),
    [SITE_ADVANCE_SHIPS] = SITE("advance_ships", 0,
 (0x0f2b86), 0x0c5c30,
 (0),        0),
    [SITE_MOVE_ENCHANTS] = SITE("move_enchants", 0,
 (0x166545), 0x2dc410,
 (0),        0),
    [SITE_MOVE_GEMS] = SITE("move_gems", 0,
 (0x16655c), 0x2dc5e0,
 (0),        0),
    [SITE_TAINTED_RITUAL_EFFECT] = SITE("tainted_ritual_effect", 0,
 (0x2c98e8, 0x2e2c05), 0x2d6c00,
 (M(0x1003528f0), M(0x100562d74)), M(0x100351574)),
    [SITE_LOCATION_SWAP] = SITE("location_swap", 0,
 (0x1f6700, 0x2c8800), 0x2d8cc0,
 (M(0x1004fead8), M(0x1005627cc)), M(0x100358f3c)),
    [SITE_APE_POSSESSION] = SITE("ape_possession", 0,
 (0x2c84a4), 0x2b0ad0,
 (M(0x100561fc8)), M(0x10056462c)),
    [SITE_RITUAL_ASK_HEMISPHERE] = SITE("ritual_ask_hemisphere", 0,
 (0x2c727b), 0x19a1b0,
 (M(0x10055f2f8)), M(0x1004a65a0)),
    [SITE_GET_TITHES] = SITE("get_tithes", 0,
 (0x165eff), 0x2d9c80,
 (0),        0),
    [SITE_ELIMINATE_PLAYER] = SITE("eliminate_player", 0,
 (0x11ed50, 0x11edc9), 0x106bd0,
 (0),                  0),
    [SITE_FIND_START_SQUARE] = SITE("find_start_square", 0,
 (0x0a3fcb, 0x304cde), 0x0a7420,
 (M(0x10036ec54), M(0x100589ea4)), M(0x100370384)),
    [SITE_TROLL_FOREST_SPAWN] = SITE("troll_forest_spawn", 0,
 (0x2da13f), 0x2d27f0,
 (0),        0),
    [SITE_AI_MAKE_DEFENDERS] = SITE("ai_make_defenders", 0,
 (0x06fb40), 0x06fe90,
 (0),        0),
    [SITE_AI_REDISTRIBUTE_ITEMS] = SITE("ai_redistribute_items", 0,
 (0x070656), 0x0734b0,
 (0),        0),
    [SITE_AI_SHIPS] = SITE("ai_ships", 0,
 (0x070a6c), 0x071630,
 (0),        0),
    [SITE_AI_SHIP_DROP_CREW] = SITE("ai_ship_drop_crew", 0,
 (0x071c17), 0x071c70,
 (0),        0),
    [SITE_AI_SHIP_TAKE_BOAT] = SITE("ai_ship_take_boat", 0,
 (0x071ba9), 0x072110,
 (0),        0),
    [SITE_AI_SHIP_TAKE_CREW] = SITE("ai_ship_take_crew", 0,
 (0x071bc5), 0x072660,
 (0),        0),
    [SITE_CHECK_FOLLOWER_LINKS] = SITE("check_follower_links", 0,
 (0x110c0f, 0x2c98dd, 0x2dc2d6), 0x110c50,
 (M(0x100358cfc), M(0x1003b5b74), M(0x10047329c), M(0x100562d68)), M(0x100467848)),
    [SITE_STEP_BURN_FOREST] = SITE("step_burn_forest", 0,
 (0x163542, 0x16505b, 0x1dfc54, 0x2dc3a2, 0x2e155d), 0x106af0,
 (0),                                                0),
    [SITE_ORDER_BURN_FOREST] = SITE("order_burn_forest", 0,
 (0x07ac2a, 0x0982cf, 0x17c6ae, 0x2dd31a), 0x2e1420,
 (0),                                      0),
    [SITE_ORDER_CHOP_FOREST] = SITE("order_chop_forest", 0,
 (0x07ac6a, 0x0982bd, 0x2dd310), 0x2e15a0,
 (0),                            0),
    [SITE_LIFEFORCE_DRAIN_SQUARE] = SITE("lifeforce_drain_square", 0,
 (0x145540, 0x32e4d1), 0x32dd70,
 (M(0x10039d698), M(0x1004d0eec)), M(0x1004d0754)),
    [SITE_SPEND_ALL_AP_AT] = SITE("spend_all_ap_at", 0,
 (0x1df21e, 0x2d6573, 0x2d7d7b, 0x2d8f23, 0x2d8f46, 0x2dc355), 0x104d60,
 (M(0x100357908), M(0x100358794), M(0x100358d98), M(0x1003594f8), M(0x100359518), M(0x10059dca8)), M(0x1004816c0)),
    [SITE_SWAP_SQUARES] = SITE("swap_squares", 0,
 (0x250fa0, 0x2512ce), 0x240ec0,
 (M(0x1003a1fa8), M(0x1003a2128)), M(0x10057c578)),
    [SITE_TELEPORT_UNIT] = SITE("teleport_unit", 0,
 (0x2c8710), 0x2dc790,
 (0),        0),
    [SITE_UNIT_CLEAR_BENEFITS] = SITE("unit_clear_benefits", 0,
 (0x0bf542, 0x0cf1c2, 0x1463bd), 0x0d0160,
 (M(0x100399b6c), M(0x1003bb1dc), M(0x1003e6278)), M(0x1003bea6c)),
    [SITE_GAME_MENU] = SITE("game_menu", 0,
 (0x09724b), 0x123e70,
 (0),        0),
    [SITE_ARMY_COMMANDS_MENU] = SITE("army_commands_menu", 0,
 (0x121551), 0x111650,
 (M(0x10038599c)), M(0x10046ae60)),
    [SITE_CEREMONY_SCREEN] = SITE("ceremony_screen", 0,
 (0x2e0d84), 0x2d3470,
 (0),        0),
    [SITE_INCOME_DETAIL_SCREEN] = SITE("income_detail_screen", 0,
 (0x0976d9), 0x11afe0,
 (0),        0),
    [SITE_BATTLE_CREATE_TREES] = SITE("battle_create_trees", 0,
 (0x0f7f9e), 0x0ee450,
 (0),        0),
    [SITE_BATTLE_DEPLOY] = SITE("battle_deploy", 0,
 (0x084d36, 0x0f42c5, 0x0f5681, 0x111ccf, 0x11fa08, 0x121fc2, 0x2b24ed, 0x2c7f69, 0x2d608d,
                       0x2d638f, 0x2d7386, 0x2d7584, 0x2d7a2e, 0x2d7bbd), 0x0f82f0,
 (0),                                                0),
    [SITE_BATTLE_FIGHT_MOVE] = SITE("battle_fight_move", 0,
 (0x0c810c, 0x0c82a4, 0x0c87d1, 0x0c891f, 0x0c8e1a, 0x0c8f19, 0x0c9861, 0x0c9f35, 0x0ca6ea,
                       0x0cacd0, 0x0caed4), 0x0cb4d0,
 (M(0x1003e84b4), M(0x1003e9298), M(0x1003e9888), M(0x1003ea264), M(0x1003ea510), M(0x1003ea9d0),
                       M(0x1003eab84), M(0x1003eabf4), M(0x1003f3518), M(0x1003f3aec), M(0x1003f3c1c)), M(0x1003f43b4)),
    [SITE_BATTLE_FLY_MOVE] = SITE("battle_fly_move", 0,
 (0x0c76a5), 0x0ca720,
 (M(0x1003e9f60)), M(0x1003f37c0)),
    [SITE_BATTLE_FLEE] = SITE("battle_flee", 0,
 (0x0c71c2, 0x0c723d), 0x0e4430,
 (0),                  0),
    [SITE_BATTLE_FEAR_CHECK] = SITE("battle_fear_check", 0,
 (0x0f2924), 0x0e40f0,
 (0),        0),
    [SITE_BATTLE_CLOUD_STEP] = SITE("battle_cloud_step", 0,
 (0x0f2668, 0x0f2c08, 0x0f2c88, 0x0f2ce1, 0x0f2d33), 0x0c0e00,
 (M(0x1003e0c04), M(0x1003e1184), M(0x1003e122c), M(0x1003e126c), M(0x1003e12f0)), M(0x1003e750c)),
    [SITE_BATTLE_ENSNARE_CHECK] = SITE("battle_ensnare_check", 0,
 (0x0f2fe5, 0x0f32a6, 0x0f3319, 0x0f37bb), 0x0d3d70,
 (M(0x1003e39c0), M(0x1003e3b60), M(0x1003e3bcc), M(0x1003e5f5c)), M(0x1003eafd0)),
    [SITE_BATTLE_ENSNARE_RELEASE] = SITE("battle_ensnare_release", 0,
 (0x0f3036, 0x0f32db, 0x0f3320, 0x0f37c2), 0x0d3780,
 (M(0x1003e39f4), M(0x1003e3b68), M(0x1003e3c20), M(0x1003e5f64)), M(0x1003ecf84)),
    [SITE_BATTLE_WALL_ENCHANTS] = SITE("battle_wall_enchants", 0,
 (0x0f25f4, 0x0f35c7, 0x0f38d4, 0x0f7e23), 0x0c3cf0,
 (M(0x1003d0748), M(0x1003e0b34), M(0x1003e5f20), M(0x1003e68c8)), M(0x1003d44b0)),
    [SITE_BATTLE_WALL_EFFECTS] = SITE("battle_wall_effects", 0,
 (0x0f34cc, 0x0f35d1), 0x0c1e40,
 (M(0x1003e5824), M(0x1003e5f28)), M(0x1003efe2c)),
    [SITE_BATTLE_START_SETUP] = SITE("battle_start_setup", 0,
 (0x0f1ddf), 0x0bff10,
 (0),        0),
    [SITE_BATTLE_TUNNEL_MOVE] = SITE("battle_tunnel_move", 0,
 (0x0c76dd), 0x0c9c70,
 (M(0x1003e9fa0)), M(0x1003f3064)),
    [SITE_BATTLE_BANISH] = SITE("battle_banish", 0,
 (0x0de803), 0x0e2df0,
 (0),        0),
    [SITE_BATTLE_DEPLOY_UNIT] = SITE("battle_deploy_unit", 0,
 (0x0be7fc, 0x0bf70f, 0x0c2fb6, 0x0cb8e0, 0x0d741b, 0x0dc22b, 0x0dc9f3, 0x0de52e, 0x0de863,
                       0x0decbc, 0x0deeb3, 0x0df37a, 0x0df409, 0x0df505, 0x0df5c1, 0x0df66a, 0x0e2cee, 0x0e3ed4,
                       0x0e5879, 0x0ee3e4), 0x0e5260,
 (M(0x1003b98e8), M(0x1003ba598), M(0x1003bc81c), M(0x1003bdca8), M(0x1003bde7c), M(0x1003c4a08),
                       M(0x1003c4b14), M(0x1003c4c44), M(0x1003c4fb0), M(0x1003c5054), M(0x1003c544c), M(0x1003c55c0),
                       M(0x1003c5a44), M(0x1003c5cc8), M(0x1003c5ffc), M(0x1003cdac8), M(0x1003cf414), M(0x1003e2a74),
                       M(0x1003e45e4), M(0x1003f11a4), M(0x1003f4b30)), M(0x1003bba28)),
    [SITE_BATTLE_AFTER_FIGHT_UPKEEP] = SITE("battle_after_fight_upkeep", 0,
 (0x0f3839), 0x0cf010,
 (0),        0),
    [SITE_BATTLE_AFTERMATH_RETURN] = SITE("battle_aftermath_return", 0,
 (0x0f62bc), 0x0e2000,
 (0),        0),
    [SITE_BATTLE_MR_ROLL] = SITE("battle_mr_roll", 0,
 (0x0ddf42, 0x0df089), 0x0b7810,
 (0),                  0),
    [SITE_BATTLE_SUMMON_WEAPON] = SITE("battle_summon_weapon", 0,
 (0x0d94e8), 0x0dbee0,
 (0),        0),
    [SITE_BATTLE_GATE_SWAP] = SITE("battle_gate_swap", 0,
 (0x0f7c8e, 0x0f7cbe, 0x0f7ce5, 0x0f7d00, 0x0f7d27, 0x0f7d4e, 0x0f7d75, 0x0f7e1e), 0x0ee250,
 (M(0x1003d03f8), M(0x1003d041c), M(0x1003d0474), M(0x1003d048c), M(0x1003d04b0), M(0x1003d04d4),
                       M(0x1003d04f8), M(0x1003d0740)), M(0x1003cf140)),
    [SITE_BATTLE_SPREAD_COLLAPSE] = SITE("battle_spread_collapse", 0,
 (0x0bed00, 0x0bed66), 0x0bf7f0,
 (M(0x1003bb678), M(0x1003bb714)), M(0x1003bc4f0)),
    [SITE_UNIT_CURE_AFFLICTION] = SITE("unit_cure_affliction", 0,
 (0x144714, 0x144785, 0x184c1f, 0x1c7ad0, 0x2c8e33, 0x327ec2), 0x0bb370,
 (M(0x10039aea8), M(0x10039af4c), M(0x1004e8388), M(0x10052e8cc), M(0x100561db4), M(0x100568b48)), M(0x1003f7644)),
    [SITE_UNIT_HP_SHAPE_CHANGE] = SITE("unit_hp_shape_change", 0,
 (0x0be814, 0x0d290a, 0x144700), 0x0bfcb0,
 (M(0x10039ae90), M(0x1003b98fc), M(0x1003efdb0)), M(0x1003bbfe4)),
    [SITE_BERSERK_END] = SITE("berserk_end", 0,
 (0x0be8fa, 0x0d106f, 0x0d110e, 0x0df23d, 0x0e05cf), 0x0b71b0,
 (0),                                                0),
    [SITE_UNIT_RANDOM_AFFLICTION] = SITE("unit_random_affliction", 0,
 (0x129eca, 0x1c7a7c), 0x0bb900,
 (M(0x100470c58), M(0x10052e854)), M(0x1003f73e4)),
    [SITE_REVEAL_TERRAIN_ON_PLANE] = SITE("reveal_terrain_on_plane", 0,
 (0x2c7b7a, 0x2c7b8d, 0x2c7ba0), 0x2d7650,
 (0),                            0),
    [SITE_NEAR_TERRAIN_SQUARE] = SITE("near_terrain_square", 0,
 (0x16b7df, 0x16ba24, 0x16bb60, 0x16be91, 0x16befa, 0x16bfae, 0x16c160), 0x1adef0,
 (M(0x1005491dc), M(0x100549340), M(0x100549398), M(0x100549408), M(0x10054a130), M(0x10054a564), M(0x10054a7b4)), M(0x1004a8ae0)),
    [SITE_PLAY_SAMPLE] = SITE("play_sample", 0,
 (0x089459, 0x0977d2, 0x0be806, 0x0c2262, 0x0c2492, 0x0c2668, 0x0c2883, 0x0c2a06, 0x0c2c48,
                       0x0c3112, 0x0c3488, 0x0c3638, 0x0c37e8, 0x0c3990, 0x0c3ba0, 0x0c98fa, 0x0c9c04, 0x0c9f7a,
                       0x0ca018, 0x0cbf0e, 0x0d48da, 0x0d4cc8, 0x0d4d7e, 0x0d6b5d, 0x0d7b54, 0x0d91dc, 0x0d96a2,
                       0x0dca1e, 0x0deedb, 0x10aaae, 0x10bd0b, 0x1270f0, 0x127151, 0x17e83e, 0x183f6d, 0x1de423,
                       0x1de6cd, 0x1df19f, 0x1ea585, 0x2b13ab, 0x2b250f, 0x2c6398, 0x2c63a2, 0x2c650d, 0x2c6517,
                       0x2c65f9, 0x2c6603, 0x2c6816, 0x2c7f93, 0x2c8823, 0x2d60c3, 0x2d6504, 0x2d6529, 0x2d659f,
                       0x2d73b3, 0x2d7a66, 0x2d7d12, 0x2d7d33, 0x2d7da9, 0x2de40f, 0x2de5d0, 0x2e2116, 0x2e21bc,
                       0x2e2229, 0x2e236d, 0x2e24ad, 0x2e25d8, 0x2e2bec), 0x128000,
 (0),                                                0),
    [SITE_GROVE_REVERSION] = SITE("grove_reversion", 0,
 (0x165584), 0x2d9fc0,
 (0),        0),
    [SITE_PUT_RARE_SQUARE] = SITE("put_rare_square", 0,
 (0x2abdd6, 0x31adb1), 0x301b60,
 (0),                  0),
    [SITE_RANDOM_ITEM_PICK] = SITE("random_item_pick", 0,
 (0x0ae7e1, 0x145fbd, 0x182afd, 0x183724, 0x183777, 0x26c268, 0x26c301, 0x2c9270), 0x17fa90,
 (0),                                                                              0),
    [SITE_CREATE_RANDOM_MAP] = SITE("create_random_map", 0,
 (0x30510b), 0x2a0d50,
 (M(0x1005919b4)), M(0x1004b4d64)),
    [SITE_WORLD_PART_TERRAIN] = SITE("world_part_terrain", 0,
 (0x2a2c91), 0x2a6330,
 (0),        0),
    [SITE_MAPGEN_REMOVE_SMALL_ISLANDS] = SITE("mapgen_remove_small_islands", 0,
 (0x2a2658), 0x2a55c0,
 (0),        0),
    [SITE_MAPGEN_RIVER_CROSSINGS] = SITE("mapgen_river_crossings", 0,
 (0x30512e), 0x2a4b90,
 (0),        0),
    [SITE_MAPGEN_ANCIENT_FOREST] = SITE("mapgen_ancient_forest", 0,
 (0x31b2ff), 0x2e98f0,
 (0),        0),
    [SITE_MAPGEN_CLEAR_AREA] = SITE("mapgen_clear_area", 0,
 (0x31b76a), 0x2a57c0,
 (0),        0),
    [SITE_MAPGEN_ADJUST_TEMPERATURE] = SITE("mapgen_adjust_temperature", 0,
 (0x2a2c8c), 0x2a64c0,
 (0),        0),
    [SITE_MARK_RIVER] = SITE("mark_river", 0,
 (0x24fdeb, 0x24fe0f, 0x2501f6, 0x250302, 0x2a538a), 0x2a87e0,
 (M(0x1004ae244), M(0x10056e334), M(0x10056e354), M(0x10056e9c8), M(0x10056ea24)), M(0x1004ad8c4)),
    [SITE_GEN_DIAGONAL_FIX] = SITE("gen_diagonal_fix", 0,
 (0x0a4377, 0x24da16), 0x254040,
 (0),        0),
    [SITE_GEN_UPDATE_MOIST_WALLS] = SITE("gen_update_moist_walls", 0,
 (0x1b0532, 0x24da80, 0x24da8a), 0x199040,
 (M(0x10036f70c), M(0x1004aa124), M(0x1005731c0), M(0x1005731e4)), M(0x1004aa590)),
    [SITE_GEN_TERRAIN_BLOB] = SITE("gen_terrain_blob", 0,
 (0x0a4356, 0x24b724, 0x24b99c, 0x24c9ab, 0x24ca97, 0x24cb4a, 0x24cbfa, 0x253c94, 0x2abcaa), 0x241530,
 (M(0x10036f6a4), M(0x1004b0578), M(0x10057b308), M(0x10057b428), M(0x10057b5c0), M(0x10057c2c4),
                       M(0x10057c3dc), M(0x10057c49c), M(0x10057c548)), M(0x1005731f4)),
    [SITE_CREATE_CELESTIAL_PLANE] = SITE("create_celestial_plane", 0,
 (0x1b8925, 0x2a340e), 0x24ada0,
 (M(0x1004b47a8), M(0x100516294)), M(0x1005788a0)),
    [SITE_CREATE_SKY_PLANE] = SITE("create_sky_plane", 0,
 (0x1b8939, 0x2a3467), 0x24ee90,
 (M(0x1004b4820), M(0x1005162e4)), M(0x100579a84)),
    [SITE_CREATE_AGARTHA_PLANE] = SITE("create_agartha_plane", 0,
 (0x1b894d, 0x2a34c0), 0x24cc40,
 (M(0x1004b4874), M(0x1005162f8)), M(0x1005724d4)),
    [SITE_CREATE_INFERNO_PLANE] = SITE("create_inferno_plane", 0,
 (0x1b8961, 0x2a3519), 0x24f950,
 (M(0x1004b48c0), M(0x100516304)), M(0x10056e004)),
    [SITE_CREATE_HADES_PLANE] = SITE("create_hades_plane", 0,
 (0x2a3572), 0x244730,
 (0),        0),
    [SITE_CREATE_ELEMENTAL_PLANE] = SITE("create_elemental_plane", 0,
 (0x1b8990, 0x2a362f), 0x248610,
 (M(0x1004b4c40), M(0x100516240)), M(0x1005752c0)),
    [SITE_LOAD_AZTLAN_PLANE] = SITE("load_aztlan_plane", 0,
 (0x1b8975, 0x2a35cb), 0x246e70,
 (0),                  0),
    [SITE_LOAD_PRIMAL_PLANE] = SITE("load_primal_plane", 0,
 (0x1b8984, 0x2a35fd), 0x24b370,
 (M(0x1004b4c0c), M(0x1005162f0)), M(0x100578500)),
    [SITE_AUTO_RECRUIT] = SITE("auto_recruit", 0,
 (0x0925b7), 0x166720,
 (0),        0),
    [SITE_USE_LIBRARY] = SITE("use_library", 0,
 (0x07abeb, 0x2de5b6), 0x2e3570,
 (M(0x10035db64), M(0x1004d1f40)), M(0x10035c7b4)),
    [SITE_ADD_GROUND_ITEMS] = SITE("add_ground_items", 0,
 (0x24da0c, 0x255ca1, 0x257cec, 0x257de2, 0x258803, 0x25a2ac, 0x260885, 0x260e89, 0x261224,
                       0x2612af, 0x2617d8, 0x261fae, 0x262f13, 0x26312a, 0x2633ec, 0x263567, 0x263de4, 0x263f55,
                       0x264b99, 0x264d0d, 0x265d53, 0x26889a, 0x2688c2, 0x268966, 0x268a12, 0x268cab, 0x268e9d,
                       0x2693c9, 0x26b733, 0x26b99c, 0x26ba63, 0x273673, 0x27f920, 0x285296, 0x28583d, 0x288810,
                       0x288985, 0x288ac2, 0x288e0a, 0x2890f7, 0x2891cb, 0x2895f6, 0x289740, 0x28d41a, 0x29092e,
                       0x29c2c5, 0x29c9f8, 0x29d0b9, 0x29d154), 0x26c2a0,
 (0),                                      0),
    [SITE_VOID_INSANITY] = SITE("void_insanity", 0,
 (0x0b1a1f, 0x2c7a07, 0x2c7af9), 0x2dee40,
 (M(0x100389050), M(0x100560cf8), M(0x100561f40)), M(0x10035aa94)),
    [SITE_WANDERER_PICK_TARGET] = SITE("wanderer_pick_target", 0,
 (0x17c6f4, 0x17c79e), 0x169b90,
 (M(0x10038c6b0), M(0x10038ddf8)), M(0x100548204)),
    [SITE_WANDERER_PICK_UP] = SITE("wanderer_pick_up", 0,
 (0x1454e2, 0x17c64b), 0x147610,
 (0),                  0),
    [SITE_SET_HOLDIT] = SITE("set_holdit", 0,
 (0x1644ad, 0x272555), 0x172f00,
 (0),                  0),
    [SITE_LIBRARY_SCREEN] = SITE("library_screen", 0,
 (0x098238, 0x098244), 0x2dd8e0,
 (0),                  0),
    [SITE_CHOP_BURN_SCREEN] = SITE("chop_burn_screen", 0,
 (0x09811b), 0x2dcce0,
 (M(0x100387968)), M(0x10035bbb4)),
    [SITE_SLAVE_HUNT_SCREEN] = SITE("slave_hunt_screen", 0,
 (0x098207), 0x2e0e20,
 (M(0x100387a84)), M(0x10035c2cc)),
    [SITE_UNIT_STATS_SCREEN] = SITE("unit_stats_screen", 0,
 (0x0972c2, 0x098d19, 0x0fa5ae, 0x10030e, 0x111e9b, 0x1120f9, 0x11e129, 0x1217d9, 0x122048,
                       0x1224f4, 0x18311a, 0x1d0186, 0x1d426f, 0x1d96c2, 0x2b02e0, 0x2d5422, 0x2e00dd), 0x1d4290,
 (M(0x100356188), M(0x10035e320), M(0x10035f58c), M(0x100380b20), M(0x100386118), M(0x100386ab4),
                       M(0x100387040), M(0x10046ac88), M(0x10047c9f8), M(0x10047dc0c), M(0x10047e184), M(0x100482d10),
                       M(0x1004e9914), M(0x1005207e4), M(0x10058442c), M(0x100584504), M(0x100584734)), M(0x100521160)),
    [SITE_ITEM_INFO_SCREEN] = SITE("item_info_screen", 0,
 (0x1d817c), 0x17e850,
 (0),        0),
    [SITE_WEAPON_INFO_WINDOW] = SITE("weapon_info_window", 0,
 (0x1d8149), 0x1cc5f0,
 (0),        0),
    [SITE_PLANE_INFO_SCREEN] = SITE("plane_info_screen", 0,
 (0x097791), 0x254470,
 (M(0x100386d70)), M(0x10057c7c8)),
    [SITE_BATTLE_INTRO_SCREEN] = SITE("battle_intro_screen", 0,
 (0x0f56ed), 0x0e17e0,
 (0),        0),
    [SITE_COMMANDER_SET_DONE] = SITE("commander_set_done", 0,
 (0x097309, 0x097335, 0x097693), 0x0a0c40,
 (0),                            0),
    [SITE_SELECT_NEXT_COMMANDER] = SITE("select_next_commander", 0,
 (0x094708, 0x094912, 0x09730e, 0x097657, 0x097698), 0x0a08a0,
 (M(0x10037b560), M(0x10037b994), M(0x1003800c4), M(0x100380c98), M(0x100382718)), M(0x100374068)),
    [SITE_SELECT_PREV_COMMANDER] = SITE("select_prev_commander", 0,
 (0x09733a), 0x0a0640,
 (0),        0),
    [SITE_RUN_HATREDS] = SITE("run_hatreds", 0,
 (0x166628), 0x142c90,
 (0),        0),
    [SITE_SANDSTORM_CLOUD_UPDATE] = SITE("sandstorm_cloud_update", 0,
 (0x163176), 0x12ed80,
 (0),        0),
    [SITE_TAINTED_MAGIC_PASS] = SITE("tainted_magic_pass", 0,
 (0x16333e), 0x1427b0,
 (0),        0),
    [SITE_HERO_ARRIVAL] = SITE("hero_arrival", 0,
 (0x167ac2), 0x089d60,
 (0),        0),
    [SITE_CLEAN_SQUARE] = SITE("clean_square", 0,
 (0x1405d0, 0x2b2abf), 0x140790,
 (M(0x1003b78dc), M(0x100563898)), M(0x10039ff68)),
    [SITE_ADJUST_FOR_SNOW] = SITE("adjust_for_snow", 0,
 (0x091a59, 0x162b78, 0x1b9289, 0x1b92f1, 0x1b96f6, 0x1b9aa5, 0x1ddcb7, 0x3347fc), 0x0b4040,
 (M(0x100389b28), M(0x1003a3eb0), M(0x1004c66f0), M(0x100515ff8), M(0x100516148), M(0x100517264),
                       M(0x100517f88), M(0x100599310)), M(0x100552314)),
    [SITE_START_TUNE] = SITE("start_tune", 0,
 (0x091b45, 0x092345, 0x093b9b, 0x10cd97, 0x10d07f, 0x10d0d6, 0x19143f, 0x198514, 0x1e8cdd), 0x122580,
 (0),                                                                                        0),
    [SITE_SQUARE_SET_OWNER] = SITE("square_set_owner", 0,
 (0x1ae9c8, 0x1ebe91, 0x334785), 0x334950,
 (0),                            0),
    [SITE_TEAMLESS_OWN_TEAM] = SITE("teamless_own_team", 0,
 (0x1f3c59), 0x32a750,
 (M(0x1004fb2d8), M(0x100589e64)), M(0x1005114dc)),
    [SITE_ADD_BENEFIT] = SITE("add_benefit", 0,
 (0x0de302, 0x14403e, 0x144940), 0x0bbbb0,
 (0),                            0),
    [SITE_SOCIETY_PRESETS] = SITE("society_presets", 0,
 (0x1b5449, 0x3050eb), 0x2d0200,
 (0),                  0),
    [SITE_MOVE_STEP_AFTER] = SITE("move_step_after", 0,
 (0x07109b, 0x098c9b, 0x09deb3, 0x16da56, 0x17ce92, 0x17d0c5, 0x2dc3b1), 0x1df050,
 (M(0x100358e90), M(0x100376ffc), M(0x1003883f8), M(0x10038d8f8), M(0x10038dcb4), M(0x1004db3d8),
                       M(0x100546a70)), M(0x10059d9dc)),
    [SITE_NUMBER_BOX] = SITE("number_box", 0,
 (0x12490f, 0x1b8838, 0x1b888b, 0x1b93ae, 0x1b9472, 0x1b94e8, 0x1b9512, 0x1b9567, 0x1b957d,
                       0x1f0f96, 0x1f1a51, 0x2aad02, 0x2aad30), 0x042770,
 (M(0x100380858), M(0x1004b3020), M(0x1004b3050), M(0x10050248c), M(0x100507474), M(0x100515c8c),
                       M(0x100515e90), M(0x100515ea8), M(0x1005161b8), M(0x100516664), M(0x10051668c), M(0x100516ff4),
                       M(0x10051710c)), M(0x1003489ac)),
    [SITE_ENCHANTS_DESTROYED_AT] = SITE("enchants_destroyed_at", 0,
 (0x1ae97d, 0x1df1d2, 0x1ebed0, 0x1f4dcb), 0x2be650,
 (M(0x1004aa960), M(0x1004fd074), M(0x100505b34), M(0x10059dc24)), M(0x100565fc4)),
    [SITE_STORM_SPAWN] = SITE("storm_spawn", 0,
 (0x2da136), 0x2d1590,
 (0),        0),
    [SITE_WILD_SPAWN] = SITE("wild_spawn", 0,
 (0x2da12d), 0x2d1a40,
 (0),        0),
    [SITE_REVELS_SPAWN] = SITE("revels_spawn", 0,
 (0x2da124), 0x2d2440,
 (0),        0),
    [SITE_HORROR_LORD_PICK] = SITE("horror_lord_pick", 0,
 (0x263b95, 0x2645b5, 0x26fbca, 0x285077, 0x29eeed, 0x29f591, 0x29fd95), 0x29ffa0,
 (0),                                                                    0),
    [SITE_CLIENT_TURN] = SITE("client_turn", 0,
 (0x1ea5b7),       0x0944c0,
 (M(0x10050bc04)), M(0x10037afe8)),
    [SITE_SETUP_SERVER] = SITE("setup_server", 0,
 (0x190d9f, 0x1f1765),             0x1f0f10,
 (M(0x100333530), M(0x10032e214)), M(0x1005022f4)),
    [SITE_SQUARE_PANEL_ICON] = SITE("square_panel_icon", 0,
 (0x0a3592, 0x0a35c5, 0x0a35e7, 0x0a3653, 0x0a36cb, 0x0a375b, 0x0a3802, 0x0a389b, 0x0a3922, 0x0a39ab, 0x0a3a2b, 0x0a3afb, 0x0a3b66, 0x0a3c03, 0x0a3c3e, 0x0a3c71, 0x0a3c94, 0x0a3cbc, 0x0a3cda, 0x0a3dab, 0x0a3ea5), 0x038380,
 (M(0x1003745a4), M(0x1003745e0), M(0x100374720), M(0x100374750), M(0x1003747ac), M(0x1003747e8), M(0x100374940), M(0x10037497c), M(0x100374b90), M(0x100374bec), M(0x100374e00), M(0x100374e5c), M(0x100375110), M(0x1003752b4), M(0x1003752f0), M(0x1003753ac), M(0x100375440), M(0x1003756c4), M(0x10037575c), M(0x1003757f0), M(0x100375884), M(0x100375910), M(0x100375998), M(0x100375a20), M(0x100375aa8), M(0x100375c70), M(0x100375cc4), M(0x100375ecc), M(0x100375f2c), M(0x100375f60), M(0x100375f84), M(0x10037600c), M(0x100376054), M(0x1003760b8), M(0x1003760d4), M(0x100376114), M(0x1003762c8), M(0x100376300)), M(0x10033df90)),
    [SITE_SQUARE_PANEL_LAYOUT] = SITE("square_panel_layout", 0,
 (0x0a0580),       0x0304e0,
 (M(0x100376900)), M(0x10034198c)),
    [SITE_SQUARE_PANEL_WRAP] = SITE("square_panel_wrap", 0,
 (0), 0,
 (0), 0),
};
#undef M
#undef SITE
#endif

// ---- Code CoE5SE copied ---------------------------------------------------------------------------
