//-----------------------------------------
// myTFT.h - CHEAP TFT and TOUCH SCREEN
//-----------------------------------------
// Cheap Ardino 3.5" 320x480 TFT's
// Uses my modified version of LCDWIKI, which
// I was not using the myLCDWici TouchScreen.
// I had denormalized that into the TeensyExpression source.

#include "myTFT.h"

#define CHEAP_TFT_DATA0     19      // needed by ts
#define CHEAP_TFT_DATA1     13      // needed by ts
#define CHEAP_TFT_DATA2     34
#define CHEAP_TFT_DATA3     35
#define CHEAP_TFT_DATA4     36
#define CHEAP_TFT_DATA5     37
#define CHEAP_TFT_DATA6     38
#define CHEAP_TFT_DATA7     39

#define CHEAP_TFT_RD         14
#define CHEAP_TFT_WR         15
#define CHEAP_TFT_CD_RS      16      // needed by ts - labelled "RS" on board
#define CHEAP_TFT_CS         17      // needed by ts
#define CHEAP_TFT_RESET      18


myLcdDevice mylcd(
    ILI9486,
    CHEAP_TFT_CS,
    CHEAP_TFT_CD_RS,
    CHEAP_TFT_WR,
    CHEAP_TFT_RD,
    CHEAP_TFT_RESET);


void initMyTFT()
{
    setTFTDataPins(
        CHEAP_TFT_DATA0,
        CHEAP_TFT_DATA1,
        CHEAP_TFT_DATA2,
        CHEAP_TFT_DATA3,
        CHEAP_TFT_DATA4,
        CHEAP_TFT_DATA5,
        CHEAP_TFT_DATA6,
        CHEAP_TFT_DATA7);
    mylcd.Init_LCD();
    mylcd.Set_Rotation(1);
    // mylcd.Set_Text_Mode(0);
    // starts with default my "m_use_bc=1"
    mylcd.Fill_Screen(0);
}
