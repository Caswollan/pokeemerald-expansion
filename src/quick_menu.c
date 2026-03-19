#include "global.h"
#include "task.h"
#include "field_player_avatar.h"
#include "script_menu.h"
#include "script.h"
#include "sound.h"
#include "pokemon.h"
#include "event_data.h"
#include "field_message_box.h"
#include "map_name_popup.h"
#include "list_menu.h"
#include "text.h"
#include "window.h"
#include "event_object_lock.h"
#include "event_object_movement.h"
#include "constants/script_menu.h"
#include "constants/vars.h"
#include "constants/items.h"
#include "constants/songs.h"
#include "constants/flags.h"
#include "main.h"
#include "menu.h"
#include "text_window.h"
#include "palette.h"
#include "event_scripts.h"
#include "constants/rtc.h"
#include "constants/map_types.h"

extern void HealPlayerParty(void);
extern void RtcInitLocalTimeOffset(s32 hour, s32 minute);

extern void CB2_LoadMap(void);
extern void CB2_OpenFlyMap(void);

extern bool8 gSkipShowMonAnim;
u8 gQuickMenuFlyActive = 0;

#define QUICK_MENU_POKEVIAL  0
#define QUICK_MENU_PC        1
#define QUICK_MENU_REPEL     2
#define QUICK_MENU_TIME      3
#define QUICK_MENU_POKERIDER 4
#define QUICK_MENU_EXIT      5

#define TIME_MENU_DAY      0
#define TIME_MENU_NIGHT    1
#define TIME_MENU_EXIT     2

#define tMenuListTaskId  data[0]
#define tWindowId        data[1]
#define tState           data[2]

//Quick Menu
static EWRAM_DATA struct ListMenuItem sQuickMenuItems[6] = {};
static const u8 sText_QM_PokeVial[]  = _("PokéVial");
static const u8 sText_QM_PC[]        = _("PC");
static const u8 sText_QM_Repel[]     = _("Repel");
static const u8 sText_QM_Time[]      = _("Time");
static const u8 sText_QM_PokeRider[] = _("Poké Rider");
static const u8 sText_QM_Exit[]      = _("Exit");

//Quick Menu PC
static const u8 sText_QM_CantUsePCHere[] = _("Can't use the PC here!");

//Quick Menu Time
static EWRAM_DATA struct ListMenuItem sTimeMenuItems[3] = {};
static const u8 sText_QM_Day[]   = _("Day");
static const u8 sText_QM_Night[] = _("Night");

//Quick Menu Repel
static const u8 sText_QM_RepelGreen[]    = _("{COLOR GREEN}Repel");
static const u8 sText_QM_RepelRed[]      = _("{COLOR RED}Repel");
static const u8 sText_QM_RepelEnabled[]  = _("Infinite Repel {COLOR GREEN}enabled!");
static const u8 sText_QM_RepelDisabled[] = _("Infinite Repel {COLOR RED}disabled!");
static const u8 sText_QM_PartyHealed[]   = _("Your party has been {COLOR GREEN}healed!");

static const u8 sColorNormal[] = {TEXT_COLOR_TRANSPARENT, TEXT_COLOR_DARK_GRAY, TEXT_COLOR_LIGHT_GRAY};
static const u8 sColorGreen[]  = {TEXT_COLOR_TRANSPARENT, TEXT_COLOR_GREEN,     TEXT_COLOR_LIGHT_GRAY};
static const u8 sColorRed[]    = {TEXT_COLOR_TRANSPARENT, TEXT_COLOR_RED,       TEXT_COLOR_LIGHT_GRAY};

//Quick Menu Poké Rider
static const u8 sText_QM_CantPokeRiderHere[] = _("Can't use Poké Rider from here!");

static void QuickMenuItemPrintFunc(u8 windowId, u32 itemId, u8 y)
{
    const u8 *color = sColorNormal;
    const u8 *text;
    switch (itemId)
    {
    case QUICK_MENU_POKEVIAL:
        text = sText_QM_PokeVial;
        break;
    case QUICK_MENU_PC:
        text = sText_QM_PC;
        break;
    case QUICK_MENU_REPEL:
        text  = sText_QM_Repel;
        break;
    case QUICK_MENU_EXIT:
    default:
        text = sText_QM_Exit;
        break;
    }
    AddTextPrinterParameterized3(windowId, FONT_NORMAL, 8, y, color, 0, text);
}

static const struct WindowTemplate sQuickMenuWindowTemplate =
{
    .bg = 0,
    .tilemapLeft = 1,
    .tilemapTop = 1,
    .width = 10,
    .height = 12,
    .paletteNum = 15,
    .baseBlock = 0x8,
};

static const struct WindowTemplate sTimeMenuWindowTemplate =
{
    .bg = 0,
    .tilemapLeft = 1,
    .tilemapTop = 1,
    .width = 8,
    .height = 6,
    .paletteNum = 15,
    .baseBlock = 0x100,
};

static const struct ListMenuTemplate sQuickMenuTemplate =
{
    .items = sQuickMenuItems,
    .moveCursorFunc = ListMenuDefaultCursorMoveFunc,
    .itemPrintFunc = QuickMenuItemPrintFunc,
    .totalItems = 6,
    .maxShowed = 6,
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
    .cursorKind = CURSOR_BLACK_ARROW,
};

static const struct ListMenuTemplate sTimeMenuTemplate =
{
    .items = sTimeMenuItems,
    .moveCursorFunc = ListMenuDefaultCursorMoveFunc,
    .itemPrintFunc = NULL,
    .totalItems = 3,
    .maxShowed = 3,
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
    .cursorKind = CURSOR_BLACK_ARROW,
};

static void Task_QuickMenu(u8 taskId);
extern void ShowPokemonStorageSystemPC(void);
static void Task_TimeMenu(u8 taskId);
static void OpenTimeMenu(void);

void ShowQuickMenu(void)
{
    struct ListMenuTemplate menuTemplate;
    u8 windowId;
    u8 listMenuTaskId;
    u8 taskId;

    sQuickMenuItems[0].name = sText_QM_PokeVial;
    sQuickMenuItems[0].id   = QUICK_MENU_POKEVIAL;
    sQuickMenuItems[1].name = sText_QM_PC;
    sQuickMenuItems[1].id   = QUICK_MENU_PC;
    sQuickMenuItems[2].name = FlagGet(FLAG_QUICK_REPEL_ACTIVE) ? sText_QM_RepelGreen : sText_QM_RepelRed;
    sQuickMenuItems[2].id   = QUICK_MENU_REPEL;
    sQuickMenuItems[3].name = sText_QM_Time;
    sQuickMenuItems[3].id   = QUICK_MENU_TIME;
    sQuickMenuItems[4].name = sText_QM_PokeRider;
    sQuickMenuItems[4].id   = QUICK_MENU_POKERIDER;
    sQuickMenuItems[5].name = sText_QM_Exit;
    sQuickMenuItems[5].id   = QUICK_MENU_EXIT;

    HideMapNamePopUpWindow();
    FreezeObjectEvents();
    PlayerFreeze();
    StopPlayerAvatar();
    LockPlayerFieldControls();

    windowId = AddWindow(&sQuickMenuWindowTemplate);
    LoadUserWindowBorderGfx(0, STD_WINDOW_BASE_TILE_NUM, BG_PLTT_ID(STD_WINDOW_PALETTE_NUM));
    DrawStdWindowFrame(windowId, FALSE);
    menuTemplate = sQuickMenuTemplate;
    menuTemplate.windowId = windowId;
    sQuickMenuItems[QUICK_MENU_REPEL].name = FlagGet(FLAG_QUICK_REPEL_ACTIVE) ? sText_QM_RepelGreen : sText_QM_RepelRed;
    listMenuTaskId = ListMenuInit(&menuTemplate, 0, 0);
    CopyWindowToVram(windowId, COPYWIN_FULL);

    taskId = CreateTask(Task_QuickMenu, 0x50);
    gTasks[taskId].tMenuListTaskId = listMenuTaskId;
    gTasks[taskId].tWindowId = windowId;
    gTasks[taskId].tState = 0;
}

static void Task_QuickMenu(u8 taskId)
{
    struct Task *task = &gTasks[taskId];

    switch (task->tState)
    {
        case 0:
        {
            u32 input = ListMenu_ProcessInput(task->tMenuListTaskId);
            
            if (JOY_NEW(A_BUTTON))
            {
                DestroyListMenuTask(task->tMenuListTaskId, NULL, NULL);
                ClearStdWindowAndFrame(task->tWindowId, TRUE);
                RemoveWindow(task->tWindowId);

                switch (input)
                {
                    case QUICK_MENU_POKEVIAL:
                        {
                            HealPlayerParty();
                            PlayFanfare(MUS_HEAL);
                            ShowFieldMessage(sText_QM_PartyHealed);
                            task->tState = 1;
                            return;
                        }
                    case QUICK_MENU_REPEL:
                        {
                            if (FlagGet(FLAG_QUICK_REPEL_ACTIVE))
                            {
                                FlagClear(FLAG_QUICK_REPEL_ACTIVE);
                                VarSet(VAR_REPEL_STEP_COUNT, 0);
                                ShowFieldMessage(sText_QM_RepelDisabled);
                            }
                            else
                            {
                                FlagSet(FLAG_QUICK_REPEL_ACTIVE);
                                VarSet(VAR_REPEL_STEP_COUNT, 255);
                                VarSet(VAR_LAST_REPEL_LURE_USED, ITEM_MAX_REPEL);
                                ShowFieldMessage(sText_QM_RepelEnabled);
                            }
                            task->tState = 1;
                            return;
                        }
                    case QUICK_MENU_PC:
                        {
                            u8 battleType = gMapHeader.battleType;

                            if (battleType == MAP_BATTLE_SCENE_SIDNEY ||
                                battleType == MAP_BATTLE_SCENE_PHOEBE ||
                                battleType == MAP_BATTLE_SCENE_GLACIA  ||
                                battleType == MAP_BATTLE_SCENE_DRAKE)
                            {
                                ShowFieldMessage(sText_QM_CantUsePCHere);
                                task->tState = 1;
                            }
                            else
                            {
                                DestroyListMenuTask(task->tMenuListTaskId, NULL, NULL);
                                ClearStdWindowAndFrame(task->tWindowId, TRUE);
                                RemoveWindow(task->tWindowId);
                                ScriptContext_SetupScript(EventScript_PCMainMenu);
                                ScriptContext_Enable();
                                DestroyTask(taskId);
                            }
                            return;
                        }
                    case QUICK_MENU_TIME:
                        {
                            OpenTimeMenu();
                            DestroyTask(taskId);
                            return;
                        }
                    case QUICK_MENU_POKERIDER:
                        {
                            u8 mapType = gMapHeader.mapType;
                            if (mapType == MAP_TYPE_TOWN || mapType == MAP_TYPE_ROUTE)
                            {
                                ScriptUnfreezeObjectEvents();
                                UnlockPlayerFieldControls();
                                DestroyTask(taskId);
                                gSkipShowMonAnim = TRUE;
                                gQuickMenuFlyActive = 1; 
                                SetMainCallback2(CB2_OpenFlyMap);
                            }
                            else
                            {
                                ShowFieldMessage(sText_QM_CantPokeRiderHere);
                                task->tState = 1;
                            }
                            return;
                        }
                    case QUICK_MENU_EXIT:
                    default:
                        {
                            ScriptUnfreezeObjectEvents();
                            UnlockPlayerFieldControls();
                            DestroyTask(taskId);
                            return;
                        }
                }
            }
            
            if (JOY_NEW(B_BUTTON))
            {
                DestroyListMenuTask(task->tMenuListTaskId, NULL, NULL);
                ClearStdWindowAndFrame(task->tWindowId, TRUE);
                RemoveWindow(task->tWindowId);
                ScriptUnfreezeObjectEvents();
                UnlockPlayerFieldControls();
                DestroyTask(taskId);
            }
            break;
        }
        case 1:
            if (JOY_NEW(A_BUTTON | B_BUTTON))
            {
                HideFieldMessageBox();
                ScriptUnfreezeObjectEvents();
                UnlockPlayerFieldControls();
                DestroyTask(taskId);
            }
            break;
    }
}

static void Task_TimeMenu(u8 taskId)
{
    struct Task *task = &gTasks[taskId];

    if (task->data[2] == 0)
    {
        task->data[2] = 1;
        return;
    }

    u32 input = ListMenu_ProcessInput(task->tMenuListTaskId);

    if (JOY_NEW(A_BUTTON))
    {
        DestroyListMenuTask(task->tMenuListTaskId, NULL, NULL);
        ClearStdWindowAndFrame(task->tWindowId, TRUE);
        RemoveWindow(task->tWindowId);

        switch (input)
        {
            case TIME_MENU_DAY:
                RtcInitLocalTimeOffset(DAY_HOUR_BEGIN, 0);
                break;
            case TIME_MENU_NIGHT:
                RtcInitLocalTimeOffset(NIGHT_HOUR_BEGIN, 0);
                break;
        }

        if (input != TIME_MENU_EXIT)
            SetMainCallback2(CB2_LoadMap);
        else
        {
            ScriptUnfreezeObjectEvents();
            UnlockPlayerFieldControls();
        }

        DestroyTask(taskId);
    }

    if (JOY_NEW(B_BUTTON))
    {
        DestroyListMenuTask(task->tMenuListTaskId, NULL, NULL);
        ClearStdWindowAndFrame(task->tWindowId, TRUE);
        RemoveWindow(task->tWindowId);
        ScriptUnfreezeObjectEvents();
        UnlockPlayerFieldControls();
        DestroyTask(taskId);
    }
}

static void OpenTimeMenu(void)
{
    struct ListMenuTemplate menuTemplate;
    u8 windowId;
    u8 listMenuTaskId;
    u8 taskId;

    sTimeMenuItems[0].name = sText_QM_Day;
    sTimeMenuItems[0].id   = TIME_MENU_DAY;
    sTimeMenuItems[1].name = sText_QM_Night;
    sTimeMenuItems[1].id   = TIME_MENU_NIGHT;
    sTimeMenuItems[2].name = sText_QM_Exit;
    sTimeMenuItems[2].id   = TIME_MENU_EXIT;

    LoadUserWindowBorderGfx(0, STD_WINDOW_BASE_TILE_NUM, BG_PLTT_ID(STD_WINDOW_PALETTE_NUM));
    windowId = AddWindow(&sTimeMenuWindowTemplate);
    DrawStdWindowFrame(windowId, FALSE);
    menuTemplate = sTimeMenuTemplate;
    menuTemplate.windowId = windowId;
    listMenuTaskId = ListMenuInit(&menuTemplate, 0, 0);
    CopyWindowToVram(windowId, COPYWIN_FULL);

    taskId = CreateTask(Task_TimeMenu, 0x50);
    gTasks[taskId].tMenuListTaskId = listMenuTaskId;
    gTasks[taskId].tWindowId = windowId;
}