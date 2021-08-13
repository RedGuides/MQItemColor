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
* TODO: Add colors for other item attributes, anything important to add?
*
* TODO: Coloring can be improved, CInvSlotWnd can have draw overridden to force the background colors we want.
*
*/

#include <mq/Plugin.h>

PreSetup("MQItemColor");
PLUGIN_VERSION(1.1);

class ItemColor
{
protected:
    // Name of ItemColor
    std::string Name;

    // On Flag
    bool On;
    bool OnDefault;

    // Normal Color
    unsigned int NormalColor;
    unsigned int NormalColorDefault;

    // Rollover Color
    unsigned int RolloverColor;
    unsigned int RolloverColorDefault;

    // INI Section/Profile Names
    std::string GeneralSection;
    std::string ItemColorSection;
    std::string OnProfile;
    std::string NormalProfile;
    std::string RolloverProfile;

public:
    ItemColor(std::string Name, bool On, unsigned int NormalColor, unsigned int RolloverColor) :
        Name(Name),
        On(On), OnDefault(On),
        NormalColor(NormalColor), NormalColorDefault(NormalColor),
        RolloverColor(RolloverColor), RolloverColorDefault(RolloverColor)
    {
        GeneralSection = "General";
        ItemColorSection = "ItemColors";
        OnProfile = Name + std::string("On");
        NormalProfile = Name + std::string("Normal");
        RolloverProfile = Name + std::string("Rollover");
    }

    bool isOn() { return On; }

    // Gets for unsigned int version of Normal / Rollover colors
    unsigned int GetNormalColor() { return NormalColor; }
    unsigned int GetRolloverColor() { return RolloverColor; }

    // Sets for unsigned int version of Normal / Rollover colors
    void SetNormalColor(unsigned int newColor) { NormalColor = newColor; }
    void SetRolloverColor(unsigned int newColor) { RolloverColor = newColor; }

    void WriteColorINI(std::string iniFileName)
    {
        // Grab On flag from INI, write out default if not there
        WritePrivateProfileBool(GeneralSection, OnProfile, On, iniFileName);

        // Write out Normal Color converted to hex string
        std::string NormalColorStr = fmt::format("0x{:X}", NormalColor);
        WritePrivateProfileString(ItemColorSection, NormalProfile, NormalColorStr, iniFileName);

        // Write out Rollover Color converted to hex string
        std::string RolloverColorStr = fmt::format("0x{:X}", RolloverColor);
        WritePrivateProfileString(ItemColorSection, RolloverProfile, RolloverColorStr, iniFileName);
    }

    void LoadFromIni(std::string iniFileName)
    {
        // Grab On flag from INI
        On = GetPrivateProfileBool(GeneralSection, OnProfile, OnDefault, iniFileName);
        // Write out On flag just in case it wasn't there
        WritePrivateProfileBool(GeneralSection, OnProfile, On, iniFileName);

        // Grab Normal Color from INI
        std::string NormalColorStr = GetPrivateProfileString(ItemColorSection, NormalProfile, "", iniFileName);

        // No Normal Color found in INI, Write out Default
        if (NormalColorStr.empty())
        {
            NormalColorStr = fmt::format("0x{:X}", NormalColorDefault);
            WritePrivateProfileString(ItemColorSection, NormalProfile, NormalColorStr, iniFileName);
            NormalColor = NormalColorDefault;
        }
        // Normal Color found in INI, attempt to convert to unsigned int
        else
        {
            try
            {
                NormalColor = std::stoul(NormalColorStr, nullptr, 16);
            }
            catch (const std::exception& e)
            {
                UNREFERENCED_PARAMETER(e);
                WriteChatf("Invalid Normal Color in INI for %s", Name.c_str());
                NormalColor = NormalColorDefault;
            }
        }


        // Grab Rollover Color from INI
        std::string RolloverColorStr = GetPrivateProfileString(ItemColorSection, RolloverProfile, "", iniFileName);

        // No Rollover Color found in INI, Write out Default
        if (RolloverColorStr.empty())
        {
            RolloverColorStr = fmt::format("0x{:X}", RolloverColorDefault);
            WritePrivateProfileString(ItemColorSection, RolloverProfile, RolloverColorStr, iniFileName);
            RolloverColor = RolloverColorDefault;
        }
        // Rollover Color found in INI, attempt to convert to unsigned int
        else
        {
            try
            {
                RolloverColor = std::stoul(RolloverColorStr, nullptr, 16);
            }
            catch (const std::exception& e)
            {
                UNREFERENCED_PARAMETER(e);
                WriteChatf("Invalid Rollover Color in INI for %s", Name.c_str());
                RolloverColor = RolloverColorDefault;
            }
        }
    }
};

// Benchmark
uint32_t bmMQItemColor = 0;

// Default Normal and Rollover Background Colors
unsigned int DefaultNormalColor = 0xFFC0C0C0;
unsigned int DefaultRolloverColor = 0xFFFFFFFF;

// ItemColor definitions, stores info for each item attribute we care to color
ItemColor QuestColor("Quest", true, 0xFFF01DFF, 0xFFF9AFFF);
ItemColor TradeSkillsColor("TradeSkills", true, 0xFFF0F000, 0xFFF09253);
ItemColor CollectibleColor("Collectible", true, 0xFFFF8C20, 0xFFFFCA4D);
ItemColor NoTradeColor("NoTrade", true, 0xFFFF2020, 0xFFFF8080);
ItemColor AttuneableColor("Attuneable", true, 0xFF6BBAFF, 0xFFFFADF4);

void LoadColorsFromINI();
void SearchInventory(bool setDefault = false);
void SetBGColors(CInvSlotWnd* pInvSlotWnd, ItemDefinition* pItemDef, bool setDefault = false);
void SetBGTexture(CInvSlotWnd* pInvSlotWnd, bool setDefault = false);

/**
* @fn InitializePlugin
*
* This is called once on plugin initialization and can be considered the startup
* routine for the plugin.
*/
PLUGIN_API void InitializePlugin()
{
    // Load settings from INI
    LoadColorsFromINI();

    // Add XML for background texture
    AddXMLFile("MQUI_ItemColorAnimation.xml");

    // Add Benchmark
    bmMQItemColor = AddMQ2Benchmark("MQItemColor");
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

    // TODO: Write out settings once a method for editing settings while plugin loaded
    // is implemented. For now we only pull from ini while loading
    // TODO: Can probably be improved(add color items into a list and
    // just make this loop through and call function on each ? )
    //QuestColor.WriteColorINI(INIFileName);
    //TradeSkillsColor.WriteColorINI(INIFileName);
    //CollectibleColor.WriteColorINI(INIFileName);
    //NoTradeColor.WriteColorINI(INIFileName);
    //AttuneableColor.WriteColorINI(INIFileName);

    // Remove XML for background texture
    RemoveXMLFile("MQUI_ItemColorAnimation.xml");

    // Remove benchmark
    RemoveMQ2Benchmark(bmMQItemColor);
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
        // Go through inventory and color appropriately
        SearchInventory();

        // Wait 100ms before running again
        // TODO: Is this too quick? too slow?
        PulseTimer = std::chrono::steady_clock::now() + std::chrono::milliseconds(100);
    }
}

/**
* @fn LoadColorsFromINI
*
* Load information from the INI for each of our colors
*
* TODO: Can probably be improved (add color items into a list and
* just make this loop through and call function on each?)
*/
void LoadColorsFromINI()
{
    QuestColor.LoadFromIni(INIFileName);
    TradeSkillsColor.LoadFromIni(INIFileName);
    CollectibleColor.LoadFromIni(INIFileName);
    NoTradeColor.LoadFromIni(INIFileName);
    AttuneableColor.LoadFromIni(INIFileName);
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
        ItemPtr pItem = pLocalPC->GetItemByGlobalIndex(globalIndex);
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

            // Contains Item
            if (pItem)
            {
                // Grab ItemDefinition for item at slot
                ItemDefinition* pItemDef = pItem->GetItemDefinition();
                if (!pItemDef)
                {
                    // Skip, no valid ItemDefinition
                    continue;
                }

                // Set background color and texture for InvSlotWnd based on ItemDefinition
                SetBGColors(pInvSlotWnd, pItemDef, setDefault);
            }
            // Does NOT Contain Item
            else
            {
                // No Item but Valid InvSlotWnd, color default (empty slot)
                SetBGColors(pInvSlotWnd, nullptr, true);
            }
        }
    }
}

/**
* @fn SetBGColors
*
* This function will change the given CInvSlotWnd pointer's CTextureAnimation
* to/from the default or a more visible background for inventory slots.
* Will also change the tint of the background normal and rollover colors
*
* @param pInvSlotWnd CInvSlotWnd* - Pointer to the CInvSlotWnd we want to change the background color of
* @param pItemDef ItemDefinition* - Pointer to the ItemDefinition of the item we are dealing with
* @param setDefault bool - True to set the original colors, false (default) to set based on item attributes
*/
void SetBGColors(CInvSlotWnd* pInvSlotWnd, ItemDefinition* pItemDef, bool setDefault)
{
    // Valid SlotWnd and ItemDef means we have an item in the slot to color
    if ((pInvSlotWnd != nullptr) && (pItemDef != nullptr))
    {
        // Based on Item Definition Flags in priority order, color background
        // Default (Return to "Normal")
        if (setDefault)
        {
            SetBGTexture(pInvSlotWnd, setDefault);
            pInvSlotWnd->BGTintNormal = DefaultNormalColor;
            pInvSlotWnd->BGTintRollover = DefaultRolloverColor;
        }
        // Quest
        else if (pItemDef->QuestItem && QuestColor.isOn())
        {
            SetBGTexture(pInvSlotWnd);
            pInvSlotWnd->BGTintNormal = QuestColor.GetNormalColor();
            pInvSlotWnd->BGTintRollover = QuestColor.GetRolloverColor();
        }
        // TradeSkill
        else if (pItemDef->TradeSkills && TradeSkillsColor.isOn())
        {
            SetBGTexture(pInvSlotWnd);
            pInvSlotWnd->BGTintNormal = TradeSkillsColor.GetNormalColor();
            pInvSlotWnd->BGTintRollover = TradeSkillsColor.GetRolloverColor();
        }
        // Collectible
        else if (pItemDef->Collectible && CollectibleColor.isOn())
        {
            SetBGTexture(pInvSlotWnd);
            pInvSlotWnd->BGTintNormal = CollectibleColor.GetNormalColor();
            pInvSlotWnd->BGTintRollover = CollectibleColor.GetRolloverColor();
        }
        // No Trade
        else if (!pItemDef->IsDroppable && NoTradeColor.isOn())
        {
            SetBGTexture(pInvSlotWnd);
            pInvSlotWnd->BGTintNormal = NoTradeColor.GetNormalColor();
            pInvSlotWnd->BGTintRollover = NoTradeColor.GetRolloverColor();
        }
        // Attuneable
        else if (pItemDef->Attuneable && AttuneableColor.isOn())
        {
            SetBGTexture(pInvSlotWnd);
            pInvSlotWnd->BGTintNormal = AttuneableColor.GetNormalColor();
            pInvSlotWnd->BGTintRollover = AttuneableColor.GetRolloverColor();
        }
        // Undefined (Return to "Normal")
        else
        {
            SetBGTexture(pInvSlotWnd, true);
            pInvSlotWnd->BGTintNormal = DefaultNormalColor;
            pInvSlotWnd->BGTintRollover = DefaultRolloverColor;
        }
    }
    // Only pInvSlotWnd valid, empty slot, return to normal
    else if (pInvSlotWnd != nullptr)
    {
        SetBGTexture(pInvSlotWnd, true);
        pInvSlotWnd->BGTintNormal = DefaultNormalColor;
        pInvSlotWnd->BGTintRollover = DefaultRolloverColor;
    }
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
}