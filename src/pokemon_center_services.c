#include "global.h"
#include "event_data.h"
#include "list_menu.h"
#include "menu.h"
#include "pokemon.h"
#include "script.h"
#include "task.h"
#include "window.h"
#include "strings.h"
#include "string_util.h"
#include "main.h"

#define tMenuListTaskId data[0]
#define tWindowId       data[1]

// Colori: foreground, background, shadow
static const u8 sColorRed[]     = {TEXT_COLOR_TRANSPARENT, TEXT_COLOR_RED,       TEXT_COLOR_LIGHT_GRAY};
static const u8 sColorBlue[]    = {TEXT_COLOR_TRANSPARENT, TEXT_COLOR_BLUE,      TEXT_COLOR_LIGHT_GRAY};
static const u8 sColorNeutral[] = {TEXT_COLOR_TRANSPARENT, TEXT_COLOR_DARK_GRAY, TEXT_COLOR_LIGHT_GRAY};
static const u8 sColorSlash[]   = {TEXT_COLOR_TRANSPARENT, TEXT_COLOR_DARK_GRAY, TEXT_COLOR_LIGHT_GRAY};

// statUp e statDown per ogni natura (0=nessuna, 1=Atk, 2=Def, 3=Spe, 4=SpA, 5=SpD)
static const u8 sNatureStatUp[]   = {0,1,1,1,1,2,0,2,2,2,3,3,0,3,3,4,4,4,0,4,5,5,5,5,0};
static const u8 sNatureStatDown[] = {0,2,3,4,5,1,0,3,4,5,1,2,0,4,5,1,2,3,0,5,1,2,3,4,0};

static const u8 sText_StatAtk[]  = _("+Atk");
static const u8 sText_StatDef[]  = _("+Def");
static const u8 sText_StatSpd[]  = _("+Speed");
static const u8 sText_StatSpA[]  = _("+SpAtk");
static const u8 sText_StatSpD[]  = _("+SpDef");
static const u8 sText_MinAtk[]   = _("-Atk");
static const u8 sText_MinDef[]   = _("-Def");
static const u8 sText_MinSpd[]   = _("-Speed");
static const u8 sText_MinSpA[]   = _("-SpAtk");
static const u8 sText_MinSpD[]   = _("-SpDef");
static const u8 sText_StatNone[] = _("Neutral");
static const u8 sText_Slash[]    = _("/");

static const u8 *const sStatUpTexts[]   = {sText_StatNone, sText_StatAtk, sText_StatDef, sText_StatSpd, sText_StatSpA, sText_StatSpD};
static const u8 *const sStatDownTexts[] = {sText_StatNone, sText_MinAtk,  sText_MinDef,  sText_MinSpd,  sText_MinSpA,  sText_MinSpD};

static const u8 sText_Hardy[]   = _("Hardy");
static const u8 sText_Lonely[]  = _("Lonely");
static const u8 sText_Brave[]   = _("Brave");
static const u8 sText_Adamant[] = _("Adamant");
static const u8 sText_Naughty[] = _("Naughty");
static const u8 sText_Bold[]    = _("Bold");
static const u8 sText_Relaxed[] = _("Relaxed");
static const u8 sText_Impish[]  = _("Impish");
static const u8 sText_Lax[]     = _("Lax");
static const u8 sText_Timid[]   = _("Timid");
static const u8 sText_Hasty[]   = _("Hasty");
static const u8 sText_Jolly[]   = _("Jolly");
static const u8 sText_Naive[]   = _("Naive");
static const u8 sText_Modest[]  = _("Modest");
static const u8 sText_Mild[]    = _("Mild");
static const u8 sText_Quiet[]   = _("Quiet");
static const u8 sText_Rash[]    = _("Rash");
static const u8 sText_Calm[]    = _("Calm");
static const u8 sText_Gentle[]  = _("Gentle");
static const u8 sText_Sassy[]   = _("Sassy");
static const u8 sText_Careful[] = _("Careful");
static const u8 sText_Exit[]    = _("Exit");

static const struct WindowTemplate sNatureMenuWindowTemplate =
{
    .bg = 0,
    .tilemapLeft = 1,
    .tilemapTop = 1,
    .width = 20,
    .height = 14,
    .paletteNum = 15,
    .baseBlock = 8
};

static const struct ListMenuItem sNatureMenuItems[] =
{
    {sText_Hardy,   0},
    {sText_Lonely,  1},
    {sText_Brave,   2},
    {sText_Adamant, 3},
    {sText_Naughty, 4},
    {sText_Bold,    5},
    {sText_Relaxed, 7},
    {sText_Impish,  8},
    {sText_Lax,     9},
    {sText_Timid,   10},
    {sText_Hasty,   11},
    {sText_Jolly,   13},
    {sText_Naive,   14},
    {sText_Modest,  15},
    {sText_Mild,    16},
    {sText_Quiet,   17},
    {sText_Rash,    19},
    {sText_Calm,    20},
    {sText_Gentle,  21},
    {sText_Sassy,   22},
    {sText_Careful, 23},
    {sText_Exit,    25},
};

static void NatureMenuItemPrintFunc(u8 windowId, u32 natureId, u8 y)
{
    if (natureId >= NUM_NATURES)
        return;

    u8 statUp   = sNatureStatUp[natureId];
    u8 statDown = sNatureStatDown[natureId];

    if (statUp == 0)
    {
        AddTextPrinterParameterized3(windowId, FONT_NORMAL, 80, y, sColorNeutral, 0, sText_StatNone);
    }
    else
    {
        u16 upWidth = GetStringWidth(FONT_NORMAL, sStatUpTexts[statUp], 1);
        u16 slashWidth = GetStringWidth(FONT_NORMAL, sText_Slash, 1);
        AddTextPrinterParameterized3(windowId, FONT_NORMAL, 70, y, sColorRed,     0, sStatUpTexts[statUp]);
        AddTextPrinterParameterized3(windowId, FONT_NORMAL, 70 + upWidth, y, sColorNeutral, 0, sText_Slash);
        AddTextPrinterParameterized3(windowId, FONT_NORMAL, 70 + upWidth + slashWidth, y, sColorBlue, 0, sStatDownTexts[statDown]);
    }
}

static const struct ListMenuTemplate sNatureMenuTemplate =
{
    .items = sNatureMenuItems,
    .moveCursorFunc = ListMenuDefaultCursorMoveFunc,
    .itemPrintFunc = NatureMenuItemPrintFunc,
    .totalItems = 22,
    .maxShowed = 7,
    .windowId = 0,
    .header_X = 0,
    .item_X = 8,
    .cursor_X = 0,
    .upText_Y = 1,
    .cursorPal = 2,
    .fillValue = 1,
    .cursorShadowPal = 3,
    .lettersSpacing = 1,
    .itemVerticalPadding = 0,
    .scrollMultiple = LIST_NO_MULTIPLE_SCROLL,
    .fontId = FONT_NORMAL,
    .cursorKind = CURSOR_BLACK_ARROW
};

static void Task_HandleNatureMenuInput(u8 taskId)
{
    u32 input = ListMenu_ProcessInput(gTasks[taskId].tMenuListTaskId);
    if (JOY_NEW(A_BUTTON))
    {
        gSpecialVar_Result = input;
        DestroyListMenuTask(gTasks[taskId].tMenuListTaskId, NULL, NULL);
        ClearStdWindowAndFrame(gTasks[taskId].tWindowId, TRUE);
        RemoveWindow(gTasks[taskId].tWindowId);
        DestroyTask(taskId);
        ScriptContext_Enable();
    }
    else if (JOY_NEW(B_BUTTON))
    {
        gSpecialVar_Result = 25;
        DestroyListMenuTask(gTasks[taskId].tMenuListTaskId, NULL, NULL);
        ClearStdWindowAndFrame(gTasks[taskId].tWindowId, TRUE);
        RemoveWindow(gTasks[taskId].tWindowId);
        DestroyTask(taskId);
        ScriptContext_Enable();
    }
}

void OpenNatureMenu(void)
{
    struct ListMenuTemplate menuTemplate;
    u8 windowId;
    u8 listMenuTaskId;
    u8 taskId;

    windowId = AddWindow(&sNatureMenuWindowTemplate);
    DrawStdWindowFrame(windowId, FALSE);
    menuTemplate = sNatureMenuTemplate;
    menuTemplate.windowId = windowId;
    listMenuTaskId = ListMenuInit(&menuTemplate, 0, 0);
    CopyWindowToVram(windowId, COPYWIN_FULL);
    taskId = CreateTask(Task_HandleNatureMenuInput, 3);
    gTasks[taskId].tMenuListTaskId = listMenuTaskId;
    gTasks[taskId].tWindowId = windowId;
    ScriptContext_Stop();
}

static bool32 CanChangeGender(u16 species)
{
    static const u16 sGenderChangeSpecies[] = {
        SPECIES_SNORUNT,
        SPECIES_RALTS,
        SPECIES_KIRLIA,
        SPECIES_SALANDIT,
        SPECIES_BURMY,
        SPECIES_COMBEE,
        SPECIES_ESPURR,
        SPECIES_BASCULIN_RED_STRIPED,
        SPECIES_LECHONK,
        SPECIES_WURMPLE,
    };
    for (u32 i = 0; i < ARRAY_COUNT(sGenderChangeSpecies); i++)
    {
        if (species == sGenderChangeSpecies[i])
            return TRUE;
    }
    return FALSE;
}

void SetMonGender(void)
{
    u32 slot = gSpecialVar_0x8004;
    if (slot >= PARTY_SIZE)
        return;

    struct Pokemon *mon = &gPlayerParty[slot];
    u16 species = GetMonData(mon, MON_DATA_SPECIES, NULL);

    if (!CanChangeGender(species))
    {
        gSpecialVar_Result = 1;
        return;
    }
    
    u8 genderRatio = gSpeciesInfo[species].genderRatio;

    if (genderRatio == MON_MALE || genderRatio == MON_FEMALE || genderRatio == MON_GENDERLESS)
    {
        gSpecialVar_Result = 1;
        return;
    }
    
    u8 currentGender = GetMonGender(mon);
    u8 targetGender = (currentGender == MON_FEMALE) ? MON_MALE : MON_FEMALE;

    SetMonGenderKeepData(mon, targetGender);
    gSpecialVar_Result = 0;
}