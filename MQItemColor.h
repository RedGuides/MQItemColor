#pragma once

#include <mq/Plugin.h>

// Enumerations of item attributes for each ItemColor
// Matches index in AvailableItemColors[] except Default which is standalone
enum class ItemColorAttribute
{
    Default = -1,
    Quest_Item = 0,
    TradeSkills_Item = 1,
    Collectible_Item = 2,
    Heirloom_Item = 3,
    NoTrade_Item = 4,
    Attuneable_Item = 5,
    HasAugSlot8_Item = 6,
	PowerSource_Item = 7,
    Last
};

// ItemColor class holds information for each attribute we want to have a special color for
// Holds the Name, Normal Color, and Rollover Color.  Knows how to read/write itself to ini.
class ItemColor
{
public:
    // Attribute Type of ItemColor
    ItemColorAttribute ItemAttribute;

    // Name of ItemColor
    std::string Name;

    // On Flag
    bool On;
    bool OnDefault;

    // Normal Color
    MQColor NormalColor;
    MQColor NormalColorDefault;

    // Rollover Color
    MQColor RolloverColor;
    MQColor RolloverColorDefault;

    // INI Section/Profile Names
    std::string ItemColorSection;
    std::string OnProfile;
    std::string NormalProfile;
    std::string RolloverProfile;

    // itemAttribute - Attribute type this color is for
    // On            - Default Enable state
    // NormalColor   - Color to use for Normal state (used as default too)
    // RolloverColor - Color to use for Rollover state (used as default too)
    ItemColor(ItemColorAttribute itemAttribute, bool On, MQColor NormalColor, MQColor RolloverColor) :
        ItemAttribute(itemAttribute),
        On(On), OnDefault(On),
        NormalColor(NormalColor), NormalColorDefault(NormalColor),
        RolloverColor(RolloverColor), RolloverColorDefault(RolloverColor)
    {
        switch (itemAttribute)
        {
        case ItemColorAttribute::Quest_Item:
            Name = "Quest";
            break;

        case ItemColorAttribute::TradeSkills_Item:
            Name = "TradeSkills";
            break;

        case ItemColorAttribute::Collectible_Item:
            Name = "Collectible";
            break;

        case ItemColorAttribute::Heirloom_Item:
            Name = "Heirloom";
            break;

        case ItemColorAttribute::NoTrade_Item:
            Name = "NoTrade";
            break;

        case ItemColorAttribute::Attuneable_Item:
            Name = "Attuneable";
            break;

		case ItemColorAttribute::HasAugSlot8_Item:
			Name = "HasAugSlot8";
			break;

		case ItemColorAttribute::PowerSource_Item:
			Name = "PowerSource";
			break;

		case ItemColorAttribute::Placeable_Item:
			Name = "Placeable";
			break;

		case ItemColorAttribute::Ornamentation_Item:
			Name = "Ornamentation";
			break;

        default:
            Name = "Unnamed";
            break;
        }

        ItemColorSection = Name;
        OnProfile = Name + "On";
        NormalProfile = Name + "Normal";
        RolloverProfile = Name + "Rollover";
    }

    // Returns On state of Color
    bool isOn() { return On; }

    // Resets for Colors back to the Defaults
    void SetNormalColorToDefault() { NormalColor = NormalColorDefault; }
    void SetRolloverColorToDefault() { RolloverColor = RolloverColorDefault; }

    void WriteColorINI(std::string iniFileName)
    {
        // Grab On flag from INI, write out default if not there
        WritePrivateProfileBool(ItemColorSection, OnProfile, On, iniFileName);

        // Write out Normal Color converted to hex string
        std::string NormalColorStr = fmt::format("0x{:X}", NormalColor.ToARGB());
        WritePrivateProfileString(ItemColorSection, NormalProfile, NormalColorStr, iniFileName);

        // Write out Rollover Color converted to hex string
        std::string RolloverColorStr = fmt::format("0x{:X}", RolloverColor.ToARGB());
        WritePrivateProfileString(ItemColorSection, RolloverProfile, RolloverColorStr, iniFileName);
    }

    void LoadFromIni(std::string iniFileName)
    {
        // Grab On flag from INI
        On = GetPrivateProfileBool(ItemColorSection, OnProfile, OnDefault, iniFileName);
        // Write out On flag just in case it wasn't there
        WritePrivateProfileBool(ItemColorSection, OnProfile, On, iniFileName);

        // Grab Normal Color from INI
        std::string NormalColorStr = GetPrivateProfileString(ItemColorSection, NormalProfile, "", iniFileName);

        // No Normal Color found in INI, Write out Default
        if (NormalColorStr.empty())
        {
            NormalColorStr = fmt::format("0x{:X}", NormalColorDefault.ToARGB());
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
                UNUSED(e);
                WriteChatf("Invalid Normal Color in INI for %s", Name.c_str());
                NormalColor = NormalColorDefault;
            }
        }


        // Grab Rollover Color from INI
        std::string RolloverColorStr = GetPrivateProfileString(ItemColorSection, RolloverProfile, "", iniFileName);

        // No Rollover Color found in INI, Write out Default
        if (RolloverColorStr.empty())
        {
            RolloverColorStr = fmt::format("0x{:X}", RolloverColorDefault.ToARGB());
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
                UNUSED(e);
                WriteChatf("Invalid Rollover Color in INI for %s", Name.c_str());
                RolloverColor = RolloverColorDefault;
            }
        }
    }
};