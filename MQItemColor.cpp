/**
* MQItemColor.cpp
*
* This plugin will set the background color of items according to various item attributes.
* Colors are done in priority order, for example a no trade tradeskill item gets colored in tradeskill colors.
*
* Colors for specific attributes can be set in the ini. A reload is required to take effect.
* Colors are based on ARGB hex format. Hex "0x" Alpha "00-FF" Red "00-FF" Green "00-FF" Blue "00-FF"
* Example: 0xFFC0C0C0
*
* The plugin will try to load an UI XML for a item background texture to give them more visibility.
* A /reload or /loadskin default may be required for the texture background change to show.
*
* To Add a New Color
* Define a new ItemColorAttribute enumeration in MQItemColor.h (in desired priority order, keep this order in the below steps)
* Add Name definition to switch in ItemColor constructor in MQItemColor.h
* Define a new ItemColor in the AvailableItemColors array
* Add new If statement for when your new ItemColor should be used to SetItemBG(), keeping in mind the priority order of the enums
*
*/

#include <mq/Plugin.h>

#include <MQItemColor/MQItemColor.h>
#include "imgui/ImGuiUtils.h"
#include "imgui/ImGuiTextEditor.h"

PreSetup("MQItemColor");
PLUGIN_VERSION(1.3);

// Globals
// Benchmark
uint32_t bmMQItemColor = 0;

// General Settings Section
std::string GeneralSection = "General";

// Flags for checks needed if on FV Server
bool FVServer = false;
bool FVNormalNoTrade = false;

// Default Item Color, used for coloring items back to a default color and default background texture
ItemColor ItemColorDefault(ItemColorAttribute::Default, true, 0xFFC0C0C0, 0xFFFFFFFF);

// ItemColor definitions, stores info for each item attribute we care to color
ItemColor AvailableItemColors[] =
{
    { ItemColor(ItemColorAttribute::Quest_Item, true,  0xFFF01DFF, 0xFFF9AFFF) },
    { ItemColor(ItemColorAttribute::TradeSkills_Item, true, 0xFFF0F000, 0xFFF09253) },
    { ItemColor(ItemColorAttribute::Collectible_Item, true, 0xFFFF8C20, 0xFFFFCA4D) },
    { ItemColor(ItemColorAttribute::Heirloom_Item, false, 0xFFC0C0C0, 0xFFFFFFFF) },
    { ItemColor(ItemColorAttribute::NoTrade_Item, true, 0xFFFF2020, 0xFFFF8080) },
    { ItemColor(ItemColorAttribute::Attuneable_Item, true, 0xFF6BBAFF, 0xFFFFADF4) }
};


/**
* @fn GetItemColor
*
* Returns the ItemColor for the specified ItemColorAttribute, or Default ItemColor if invalid attribute given
*/
static ItemColor& GetItemColor(ItemColorAttribute itemColorAttr)
{
    if (itemColorAttr < ItemColorAttribute::Quest_Item || itemColorAttr >= ItemColorAttribute::Last)
    {
        return ItemColorDefault;
    }

    return AvailableItemColors[static_cast<size_t>(itemColorAttr)];
}


/**
* @fn HelpLabel
*
* Creates a (?) which can be hovered over to display some help text
*
* @param text const char* - Text to dispaly in the help popup
*/
static void HelpLabel(const char* text)
{
    ImGui::SameLine();
    ImGui::TextDisabled("(?)");
    if (ImGui::IsItemHovered())
    {
        ImGui::BeginTooltip();
        ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
        ImGui::TextUnformatted(text);
        ImGui::PopTextWrapPos();
        ImGui::EndTooltip();
    }
}


/**
* @fn WriteGeneralSettingsToINI
*
* Writes any general settings to the INI
*/
static void WriteGeneralSettingsToINI()
{
    // Write out FVNormalNoTrade flag
    WritePrivateProfileBool(GeneralSection, "FVNormalNoTrade", FVNormalNoTrade, INIFileName);
}


/**
* @fn WriteColorSettingsToINI
*
* Writes any ItemColor settings to the INI
*/
static void WriteColorSettingsToINI()
{
    for (ItemColor& itemColor : AvailableItemColors)
    {
        itemColor.WriteColorINI(INIFileName);
    }
}


/**
* @fn ItemColorSettings_General
*
* Sets up the general settings area. Contains any general options for the plugin
*/
static void ItemColorSettings_General()
{
    // Section Title
    ImGui::PushFont(imgui::LargeTextFont);
    ImGui::TextColored(MQColor(255, 255, 0).ToImColor(), "General");
    ImGui::Separator();
    ImGui::PopFont();

    // FV Normal No Trade Checkbox Section
    if (ImGui::Checkbox("Color Items Normally Marked \"No Trade\"", &FVNormalNoTrade))
    {
        WriteGeneralSettingsToINI();
    }
    ImGui::SameLine();
    ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 0.5f), "- Firiona Vie Server Only");
    ImGui::NewLine();
}


/**
* @fn ItemColorSettings_Colors
*
* Sets up the Colors settings area. Contains toggles and color choosers for each ItemColor
*/
static void ItemColorSettings_Colors()
{
    // Section Title
    ImGui::PushFont(imgui::LargeTextFont);
    ImGui::TextColored(MQColor(255, 255, 0).ToImColor(), "Item Colors");
    ImGui::Separator();
    ImGui::PopFont();

    for (ItemColor& itemColor : AvailableItemColors)
    {
        // Enable Checkbox Section
        if (ImGui::Checkbox((itemColor.Name).c_str(), &itemColor.On))
        {
            itemColor.WriteColorINI(INIFileName);
        }
        std::string itemColorHelp = "Color items marked \"" + itemColor.Name + "\"";
        HelpLabel(itemColorHelp.c_str());

        // Normal Color Chooser Section
        ImGui::PushID(itemColor.NormalProfile.c_str());

        ImColor normalColor = itemColor.NormalColor.ToImColor();

        if (ImGui::ColorEdit3("Normal", &normalColor.Value.x))
        {
            itemColor.NormalColor.Blue = static_cast<uint8_t>(normalColor.Value.z * 255);
            itemColor.NormalColor.Green = static_cast<uint8_t>(normalColor.Value.y * 255);
            itemColor.NormalColor.Red = static_cast<uint8_t>(normalColor.Value.x * 255);
            itemColor.NormalColor.Alpha = 255U;
            itemColor.WriteColorINI(INIFileName);
        }

        if (itemColor.NormalColor != itemColor.NormalColorDefault)
        {
            ImGui::SameLine();
            if (ImGui::Button("Reset"))
            {
                itemColor.SetNormalColorToDefault();
                itemColor.WriteColorINI(INIFileName);
            }
        }

        ImGui::PopID();


        // Rollover Color Chooser Section
        ImGui::PushID(itemColor.RolloverProfile.c_str());

        ImColor rolloverColor = itemColor.RolloverColor.ToImColor();

        if (ImGui::ColorEdit3("Rollover", &rolloverColor.Value.x))
        {
            itemColor.RolloverColor.Blue = static_cast<uint8_t>(rolloverColor.Value.z * 255);
            itemColor.RolloverColor.Green = static_cast<uint8_t>(rolloverColor.Value.y * 255);
            itemColor.RolloverColor.Red = static_cast<uint8_t>(rolloverColor.Value.x * 255);
            itemColor.RolloverColor.Alpha = 255U;
            itemColor.WriteColorINI(INIFileName);
        }

        if (itemColor.RolloverColor != itemColor.RolloverColorDefault)
        {
            ImGui::SameLine();
            if (ImGui::Button("Reset"))
            {
                itemColor.SetRolloverColorToDefault();
                itemColor.WriteColorINI(INIFileName);
            }
        }

        ImGui::PopID();

        ImGui::NewLine();
    }
}


/**
* @fn ItemColorSettingsPanel
*
* Sets up the settings ui for the MQ settings window
*/
void ItemColorSettingsPanel()
{
    ItemColorSettings_General();
    ItemColorSettings_Colors();
}


/**
* @fn SetBGTexture
*
* This function will change the given CInvSlotWnd pointer's CTextureAnimation
* to/from the default or a more visible background for inventory slots.
*
* @param pInvSlotWnd CInvSlotWnd* - Pointer to the CInvSlotWnd we want to change the texture of
* @param setDefault bool - True to set the original texture, false (default) to set more visible background texture
*/
void SetBGTexture(CInvSlotWnd* pInvSlotWnd, bool setDefault)
{
	// Currently an issue with creating a hot button for an item with custom texture applied, avoiding for now
	/*
    if ((pInvSlotWnd != nullptr) && (pInvSlotWnd->pBackground != nullptr))
    {
        CTextureAnimation* newTex = nullptr;

        // Return texture to normal
        if (setDefault)
        {
            newTex = pSidlMgr->FindAnimation("A_RecessedBox");
        }
        // Set texture to more visible background
        else
        {
            newTex = pSidlMgr->FindAnimation("A_ItemColorRecessedBox");
        }

        if (newTex != nullptr)
        {
            pInvSlotWnd->pBackground = newTex;
        }
    }
	*/
}


/**
* @fn SetBGColors
*
* This function will change the given CInvSlotWnd pointer's background colors depending on the ItemColorAttribute given.
*
* @param pInvSlotWnd CInvSlotWnd* - Pointer to the CInvSlotWnd we want to change the texture of
* @param itemColorAttr ItemColorAttribute - Item attribute type used to grab what color we need
*/
void SetBGColors(CInvSlotWnd* pInvSlotWnd, ItemColorAttribute itemColorAttr)
{
    if (pInvSlotWnd != nullptr)
    {
        ItemColor itemColor = GetItemColor(itemColorAttr);
        pInvSlotWnd->BGTintNormal = (itemColor.NormalColor).ToARGB();
        pInvSlotWnd->BGTintRollover = (itemColor.RolloverColor).ToARGB();
    }
}


/**
* @fn SetItemBG
*
* This function will change the given CInvSlotWnd pointer's CTextureAnimation
* to/from the default or a more visible background for inventory slots.
* Will also change the tint of the background normal and rollover colors
*
* @param pInvSlotWnd CInvSlotWnd* - Pointer to the CInvSlotWnd we want to change the background color of
* @param pItem ItemPtr - Smart Pointer to the Item we are dealing with
* @param setDefault bool - True to set the original colors, false (default) to set based on item attributes
*/
void SetItemBG(CInvSlotWnd* pInvSlotWnd, ItemPtr pItem, bool setDefault)
{
    // pInvSlotWnd must be valid for background to be changed
    // If pItem (and thus pItemDef) is invalid, we return the slot to default
    // If pItem is valid, and we have a valid pItemDef, we set the background appropriately
    // If pItem is valid, but pItemDef is not, we return the slot to default

    // If we have a valid item pointer, try to grab its ItemDefinition
    ItemDefinition* pItemDef = nullptr;
    if (pItem != nullptr)
    {
        pItemDef = pItem->GetItemDefinition();
    }

    // Valid SlotWnd and ItemDef means we have an item in the slot to color
    if ((pInvSlotWnd != nullptr) && (pItemDef != nullptr))
    {
        // Based on Item Definition Flags in priority order, color background
        // Default (Return to "Normal")
        if (setDefault)
        {
            SetBGColors(pInvSlotWnd, ItemColorAttribute::Default);
            SetBGTexture(pInvSlotWnd, true);
        }
        // Quest
        else if (pItemDef->QuestItem && GetItemColor(ItemColorAttribute::Quest_Item).isOn())
        {
            SetBGColors(pInvSlotWnd, ItemColorAttribute::Quest_Item);
            SetBGTexture(pInvSlotWnd, false);
        }
        // TradeSkill
        else if (pItemDef->TradeSkills && GetItemColor(ItemColorAttribute::TradeSkills_Item).isOn())
        {
            SetBGColors(pInvSlotWnd, ItemColorAttribute::TradeSkills_Item);
            SetBGTexture(pInvSlotWnd, false);
        }
        // Collectible
        else if (pItemDef->Collectible && GetItemColor(ItemColorAttribute::Collectible_Item).isOn())
        {
            SetBGColors(pInvSlotWnd, ItemColorAttribute::Collectible_Item);
            SetBGTexture(pInvSlotWnd, false);
        }
        // Heirloom
        else if (pItemDef->Heirloom && GetItemColor(ItemColorAttribute::Heirloom_Item).isOn())
        {
            SetBGColors(pInvSlotWnd, ItemColorAttribute::Heirloom_Item);
            SetBGTexture(pInvSlotWnd, false);
        }
        // No Trade
        // On FV server, color Normal No Trade only if FVNormalNoTrade setting is enabled
        else if (((!pItemDef->IsDroppable || pItem->NoDropFlag) && GetItemColor(ItemColorAttribute::NoTrade_Item).isOn()) &&
            ((FVServer && FVNormalNoTrade) || (!FVServer)))
        {
            SetBGColors(pInvSlotWnd, ItemColorAttribute::NoTrade_Item);
            SetBGTexture(pInvSlotWnd, false);
        }
        // FV No Trade
        // On FV server, color those that are FV No Trade using Normal No Trade settings
        // Avoids coloring items that are normally No Trade but not on FV
        else if (FVServer && (pItemDef->bIsFVNoDrop) && GetItemColor(ItemColorAttribute::NoTrade_Item).isOn())
        {
            SetBGColors(pInvSlotWnd, ItemColorAttribute::NoTrade_Item);
            SetBGTexture(pInvSlotWnd, false);
        }
        // Attuneable
        else if (pItemDef->Attuneable && GetItemColor(ItemColorAttribute::Attuneable_Item).isOn())
        {
            SetBGColors(pInvSlotWnd, ItemColorAttribute::Attuneable_Item);
            SetBGTexture(pInvSlotWnd, false);
        }
        // Undefined (Return to "Normal")
        else
        {
            SetBGColors(pInvSlotWnd, ItemColorAttribute::Default);
            SetBGTexture(pInvSlotWnd, true);
        }
    }
    // Only pInvSlotWnd valid, empty slot, return to normal
    else if (pInvSlotWnd != nullptr)
    {
        SetBGColors(pInvSlotWnd, ItemColorAttribute::Default);
        SetBGTexture(pInvSlotWnd, true);
    }
}


/**
* @fn SearchInventory
*
* Searches through inventory slots to color only those slots we care about
* Only colors those slots that are part of the player main inventory or bags,
* avoids coloring worn items or any item buttons that may be created.
*
* @param setDefault bool - True to set the original colors, false (default) to set based on item attributes
*/
void SearchInventory(bool setDefault)
{
    if (!pInvSlotMgr)
    {
        return;
    }

    // Loop through each inventory slot
    for (int index = 0; index < pInvSlotMgr->TotalSlots; index++)
    {
        // Grab slot at index
        CInvSlot* pInvSlot = pInvSlotMgr->SlotArray[index];
        if (!pInvSlot || !pInvSlot->bEnabled)
        {
            continue;
        }

        // Grab global index and pointer to item if one exists at slot
        ItemGlobalIndex globalIndex = pInvSlot->pInvSlotWnd ? pInvSlot->pInvSlotWnd->ItemLocation : ItemGlobalIndex();
        ItemContainerInstance location = globalIndex.GetLocation();

        // Check if Index is Valid and the locations we want (Inventory, Bank, or Shared Bank)
        if (globalIndex.IsValidLocation() &&
            ((location == eItemContainerPossessions) || (location == eItemContainerBank) || (location == eItemContainerSharedBank)))
        {
            // Skip if Index is an Equipped Location
            if (globalIndex.IsEquippedLocation())
            {
                continue;
            }

            // Grab the pointer for its CInvSlotWnd
            CInvSlotWnd* pInvSlotWnd = pInvSlot->pInvSlotWnd;

            // Skip if no valid CInvSlotWnd or Hot Button
            if (!pInvSlotWnd || pInvSlotWnd->bHotButton)
            {
                continue;
            }

            // The remaining CInvSlotWnd at this point should be those in
            // Inventory, Bank, or Shared Bank that either contain an item or not.
            ItemPtr pItem = pLocalPC->GetItemByGlobalIndex(globalIndex);

            // Contains Item
            if (pItem)
            {
                // Set background color and texture for InvSlotWnd based on ItemDefinition
                SetItemBG(pInvSlotWnd, pItem, setDefault);
            }
            // Does NOT Contain Item
            else
            {
                // No Item but Valid InvSlotWnd, color default (empty slot)
                SetItemBG(pInvSlotWnd, nullptr, true);
            }
        }
    }
}


/**
* @fn LoadSettingsFromINI
*
* Load settings from the INI for each of our colors and any general settings
*/
void LoadSettingsFromINI()
{
    // Grab FVNormalNoTrade flag from INI
    FVNormalNoTrade = GetPrivateProfileBool(GeneralSection, "FVNormalNoTrade", false, INIFileName);
    // Write out FVNormalNoTrade flag just in case it wasn't there
    WritePrivateProfileBool(GeneralSection, "FVNormalNoTrade", FVNormalNoTrade, INIFileName);

    for (ItemColor& itemColor : AvailableItemColors)
    {
        itemColor.LoadFromIni(INIFileName);
    }
}


/**
* @fn SaveSettingsToINI
*
* Save settings to the INI for each of our colors and any general settings
*/
void SaveSettingsToINI()
{
    // Write General settings to INI
    WriteGeneralSettingsToINI();

    // Write ItemColor settings to INI
    WriteColorSettingsToINI();
}


/**
* @fn InitializePlugin
*
* This is called once on plugin initialization and can be considered the startup
* routine for the plugin.
*/
PLUGIN_API void InitializePlugin()
{
    // Load settings from INI
    LoadSettingsFromINI();

    // Add XML for background texture
    AddXMLFile("MQUI_ItemColorAnimation.xml");

    // Add Benchmark
    bmMQItemColor = AddMQ2Benchmark("MQItemColor");

    // Add Settings UI
    AddSettingsPanel("plugins/ItemColor", ItemColorSettingsPanel);
}


/**
* @fn ShutdownPlugin
*
* This is called once when the plugin has been asked to shutdown. The plugin has
* not actually shut down until this completes.
*/
PLUGIN_API void ShutdownPlugin()
{
    // Set inventory back to default backgrounds
    SearchInventory(true);

    // Save settings to INI
    SaveSettingsToINI();

    // Remove XML for background texture
    RemoveXMLFile("MQUI_ItemColorAnimation.xml");

    // Remove Benchmark
    RemoveMQ2Benchmark(bmMQItemColor);

    // Remove Settings UI
    RemoveSettingsPanel("plugins/ItemColor");
}


/**
 * @fn SetGameState
 *
 * This is called when the GameState changes.  It is also called once after the
 * plugin is initialized.
 *
 * For a list of known GameState values, see the constants that begin with
 * GAMESTATE_.  The most commonly used of these is GAMESTATE_INGAME.
 *
 * When zoning, this is called once after @ref OnBeginZone @ref OnRemoveSpawn
 * and @ref OnRemoveGroundItem are all done and then called once again after
 * @ref OnEndZone and @ref OnAddSpawn are done but prior to @ref OnAddGroundItem
 * and @ref OnZoned
 *
 * @param GameState int - The value of GameState at the time of the call
 */
PLUGIN_API void SetGameState(int GameState)
{
    if (GameState == GAMESTATE_INGAME)
    {
        // Check if we are on FV, set flag to true if we are
        // This flag will be used for any special logic we need if on FV
        if (strcmp(GetServerShortName(), "firiona") == 0)
        {
            FVServer = true;
        }
        else
        {
            FVServer = false;
        }
    }
}


/**
* @fn OnPulse
*
* This is called each time MQ2 goes through its heartbeat (pulse) function.
*
* Every time our pulse counter is hit, we take a look at
* what is in our inventory and color it accordingly.
*/
PLUGIN_API void OnPulse()
{
    // Benchmark on pulse
    MQScopedBenchmark bm(bmMQItemColor);

    static std::chrono::steady_clock::time_point PulseTimer = std::chrono::steady_clock::now();

    // Run only after timer is up
    if ((std::chrono::steady_clock::now() > PulseTimer) && (gGameState == GAMESTATE_INGAME))
    {
        // Search through inventory and color slots
        SearchInventory(false);

        // Wait 100ms before running again
        PulseTimer = std::chrono::steady_clock::now() + std::chrono::milliseconds(100);
    }
}
