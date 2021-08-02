// MQItemColor.cpp : Defines the entry point for the DLL application.
//

// PLUGIN_API is only to be used for callbacks.  All existing callbacks at this time
// are shown below. Remove the ones your plugin does not use.  Always use Initialize
// and Shutdown for setup and cleanup.

#include <mq/Plugin.h>
#include <MQItemColor/MQItemColor.h>
#include <fstream>

PreSetup("MQItemColor");
PLUGIN_VERSION(0.1);

unsigned int DefaultNormalColor = 0xFFC0C0C0;
unsigned int DefaultRolloverColor = 0xFFFFFFFF;
itemcolor::ItemColor QuestColor("Quest", true, "0xFFF01DFF", "0xFFFF35FF");
itemcolor::ItemColor TradeSkillsColor("TradeSkills", true, "0xFFF0F000", "0xFFF09253");
itemcolor::ItemColor CollectibleColor("Collectible", true, "0xFFFF8C20", "0xFFFFB120");
itemcolor::ItemColor NoTradeColor("NoTrade", true, "0xFFFF2020", "0xFFFF4120");
itemcolor::ItemColor AttuneableColor("Attuneable", true, "0xFF6BBAFF", "0xFFFFADF4");

void Load_INI();
void SearchInventory(bool setDefault = false);
void SetBGColors(ItemClient* pItem, bool setDefault = false);
void SetBGTexture(CInvSlotWnd* pInvSlotWnd, bool setDefault = false);

/**
 * @fn InitializePlugin
 *
 * This is called once on plugin initialization and can be considered the startup
 * routine for the plugin.
 */
PLUGIN_API void InitializePlugin()
{
	DebugSpewAlways("MQItemColor::Initializing version %f", MQ2Version);

	// Examples:
	// AddCommand("/mycommand", MyCommand);
	// AddXMLFile("MQUI_MyXMLFile.xml");
	// AddMQ2Data("mytlo", MyTLOData);

	sprintf_s(INIFileName, "%s\\MQ2ItemColor.ini", gPathConfig);
	Load_INI();
	AddXMLFile("EQUI_Animations2.xml");
}

/**
 * @fn ShutdownPlugin
 *
 * This is called once when the plugin has been asked to shutdown.  The plugin has
 * not actually shut down until this completes.
 */
PLUGIN_API void ShutdownPlugin()
{
	DebugSpewAlways("MQItemColor::Shutting down");

	SearchInventory(true);

	// Examples:
	// RemoveCommand("/mycommand");
	// RemoveXMLFile("MQUI_MyXMLFile.xml");
	// RemoveMQ2Data("mytlo");
	RemoveXMLFile("EQUI_Animations2.xml");
}

/**
 * @fn OnCleanUI
 *
 * This is called once just before the shutdown of the UI system and each time the
 * game requests that the UI be cleaned.  Most commonly this happens when a
 * /loadskin command is issued, but it also occurs when reaching the character
 * select screen and when first entering the game.
 *
 * One purpose of this function is to allow you to destroy any custom windows that
 * you have created and cleanup any UI items that need to be removed.
 */
PLUGIN_API void OnCleanUI()
{
	// DebugSpewAlways("MQItemColor::OnCleanUI()");
}

/**
 * @fn OnReloadUI
 *
 * This is called once just after the UI system is loaded. Most commonly this
 * happens when a /loadskin command is issued, but it also occurs when first
 * entering the game.
 *
 * One purpose of this function is to allow you to recreate any custom windows
 * that you have setup.
 */
PLUGIN_API void OnReloadUI()
{
	// DebugSpewAlways("MQItemColor::OnReloadUI()");
}


/**
 * @fn OnPulse
 *
 * This is called each time MQ2 goes through its heartbeat (pulse) function.
 *
 * Because this happens very frequently, it is recommended to have a timer or
 * counter at the start of this call to limit the amount of times the code in
 * this section is executed.
 */
PLUGIN_API void OnPulse()
{
	static std::chrono::steady_clock::time_point PulseTimer = std::chrono::steady_clock::now();
	// Run only after timer is up
	if ((std::chrono::steady_clock::now() > PulseTimer) && (gGameState == GAMESTATE_INGAME))
	{
		//clock_t t;
		//t = clock();

		SearchInventory();

		QuestColor.WriteColorINI(INIFileName);
		TradeSkillsColor.WriteColorINI(INIFileName);
		CollectibleColor.WriteColorINI(INIFileName);
		NoTradeColor.WriteColorINI(INIFileName);
		AttuneableColor.WriteColorINI(INIFileName);

		//t = clock() - t;

		//WriteChatf("It took me %d clicks (%f seconds).\n", t, ((float)t) / CLOCKS_PER_SEC);

		// Wait 1 seconds before running again
		PulseTimer = std::chrono::steady_clock::now() + std::chrono::milliseconds(100);
		//DebugSpewAlways("MQ2ItemColor::OnPulse()");
	}
}

/**
 * @fn OnWriteChatColor
 *
 * This is called each time WriteChatColor is called (whether by MQ2Main or by any
 * plugin).  This can be considered the "when outputting text from MQ" callback.
 *
 * This ignores filters on display, so if they are needed either implement them in
 * this section or see @ref OnIncomingChat where filters are already handled.
 *
 * If CEverQuest::dsp_chat is not called, and events are required, they'll need to
 * be implemented here as well.  Otherwise, see @ref OnIncomingChat where that is
 * already handled.
 *
 * For a list of Color values, see the constants for USERCOLOR_.  The default is
 * USERCOLOR_DEFAULT.
 *
 * @param Line const char* - The line that was passed to WriteChatColor
 * @param Color int - The type of chat text this is to be sent as
 * @param Filter int - (default 0)
 */
PLUGIN_API void OnWriteChatColor(const char* Line, int Color, int Filter)
{
	// DebugSpewAlways("MQItemColor::OnWriteChatColor(%s, %d, %d)", Line, Color, Filter);
}

/**
 * @fn OnIncomingChat
 *
 * This is called each time a line of chat is shown.  It occurs after MQ filters
 * and chat events have been handled.  If you need to know when MQ2 has sent chat,
 * consider using @ref OnWriteChatColor instead.
 *
 * For a list of Color values, see the constants for USERCOLOR_. The default is
 * USERCOLOR_DEFAULT.
 *
 * @param Line const char* - The line of text that was shown
 * @param Color int - The type of chat text this was sent as
 *
 * @return bool - Whether to filter this chat from display
 */
PLUGIN_API bool OnIncomingChat(const char* Line, DWORD Color)
{
	// DebugSpewAlways("MQItemColor::OnIncomingChat(%s, %d)", Line, Color);
	return false;
}


/**
 * @fn OnLoadPlugin
 *
 * This is called each time a plugin is loaded (ex: /plugin someplugin), after the
 * plugin has been loaded and any associated -AutoExec.cfg file has been launched.
 * This means it will be executed after the plugin's @ref InitializePlugin callback.
 *
 * This is also called when THIS plugin is loaded, but initialization tasks should
 * still be done in @ref InitializePlugin.
 *
 * @param Name const char* - The name of the plugin that was loaded
 */
PLUGIN_API void OnLoadPlugin(const char* Name)
{
	// DebugSpewAlways("MQItemColor::OnLoadPlugin(%s)", Name);
}

/**
 * @fn OnUnloadPlugin
 *
 * This is called each time a plugin is unloaded (ex: /plugin someplugin unload),
 * just prior to the plugin unloading.  This means it will be executed prior to that
 * plugin's @ref ShutdownPlugin callback.
 *
 * This is also called when THIS plugin is unloaded, but shutdown tasks should still
 * be done in @ref ShutdownPlugin.
 *
 * @param Name const char* - The name of the plugin that is to be unloaded
 */
PLUGIN_API void OnUnloadPlugin(const char* Name)
{
	// DebugSpewAlways("MQItemColor::OnUnloadPlugin(%s)", Name);
}

void Load_INI()
{
	QuestColor.LoadFromIni(INIFileName);
	TradeSkillsColor.LoadFromIni(INIFileName);
	CollectibleColor.LoadFromIni(INIFileName);
	NoTradeColor.LoadFromIni(INIFileName);
	AttuneableColor.LoadFromIni(INIFileName);
}

void SearchInventory(bool setDefault)
{
	// loop through inv slots & worn slots
	for (unsigned short usSlot = InvSlot_FirstBagSlot; usSlot < InvSlot_NumInvSlots; usSlot++)
	{
		if (ItemClient* pItem = GetPcProfile()->GetInventorySlot(usSlot))
		{
			// if there is a non-empty bag in this slot
			if (pItem->IsContainer() && !pItem->IsEmpty())
			{
				for (const ItemPtr& pBagSlot : pItem->GetHeldItems())
				{
					if (pBagSlot)
					{
						// Set color of ItemClient* of bag slot inside bag
						if (ItemClient* bagSlotIC = pBagSlot.get())
						{
							SetBGColors(bagSlotIC, setDefault);
						}
					}
					else
					{
						// Empty Bag Slot, Nothing to Do
					}
				}
			}
			else if (!pItem->IsContainer())
			{
				SetBGColors(pItem, setDefault);
			}
		}
	}
}

void SetBGColors(ItemClient* pItem, bool setDefault)
{
	// Make sure ItemClient pointer is valid
	if (pItem != nullptr)
	{
		// Grab Inv Slot Pointer and Item Definition Pointer
		CInvSlot* pInvSlot = GetInvSlot(pItem->GetItemLocation());
		ItemDefinition* pItemDef = pItem->GetItemDefinition();

		// Make sure both are valid
		if ((pInvSlot != nullptr) && (pItemDef != nullptr))
		{
			// Grab Inv Slot Window Pointer and make sure its valid
			if (CInvSlotWnd* pInvSlotWnd = pInvSlot->pInvSlotWnd)
			{

				// Based on Item Definition Flags in priority order, color background
				if (setDefault)
				{
					SetBGTexture(pInvSlotWnd, setDefault);
					pInvSlotWnd->BGTintNormal = DefaultNormalColor;
					pInvSlotWnd->BGTintRollover = DefaultRolloverColor;
				}
				else if (pItemDef->QuestItem && QuestColor.isOn())
				{
					SetBGTexture(pInvSlotWnd);
					pInvSlotWnd->BGTintNormal = QuestColor.GetNormalColor();
					pInvSlotWnd->BGTintRollover = QuestColor.GetRolloverColor();
				}
				else if (pItemDef->TradeSkills && TradeSkillsColor.isOn())
				{
					SetBGTexture(pInvSlotWnd);
					pInvSlotWnd->BGTintNormal = TradeSkillsColor.GetNormalColor();
					pInvSlotWnd->BGTintRollover = TradeSkillsColor.GetRolloverColor();
				}
				else if (pItemDef->Collectible && CollectibleColor.isOn())
				{
					SetBGTexture(pInvSlotWnd);
					pInvSlotWnd->BGTintNormal = CollectibleColor.GetNormalColor();
					pInvSlotWnd->BGTintRollover = CollectibleColor.GetRolloverColor();
				}
				else if (!pItemDef->IsDroppable && NoTradeColor.isOn())
				{
					SetBGTexture(pInvSlotWnd);
					pInvSlotWnd->BGTintNormal = NoTradeColor.GetNormalColor();
					pInvSlotWnd->BGTintRollover = NoTradeColor.GetRolloverColor();
				}
				else if (pItemDef->Attuneable && AttuneableColor.isOn())
				{
					SetBGTexture(pInvSlotWnd);
					pInvSlotWnd->BGTintNormal = AttuneableColor.GetNormalColor();
					pInvSlotWnd->BGTintRollover = AttuneableColor.GetRolloverColor();
				}
				else
				{
					SetBGTexture(pInvSlotWnd, true);
					pInvSlotWnd->BGTintNormal = DefaultNormalColor;
					pInvSlotWnd->BGTintRollover = DefaultRolloverColor;
				}
			}
		}
		else
		{
			// Inventory Slot Not Visible? Nothing to Do
		}
	}
}

void SetBGTexture(CInvSlotWnd* pInvSlotWnd, bool setDefault)
{
	if (pInvSlotWnd->pBackground != nullptr)
	{
		CTextureAnimation* newTex = nullptr;
		if (setDefault)
		{
			newTex = pSidlMgr->FindAnimation("A_RecessedBox");
		}
		else
		{
			newTex = pSidlMgr->FindAnimation("A_RecessedBoxMQ2");
		}

		if (newTex != nullptr)
		{
			pInvSlotWnd->pBackground = newTex;

		}

		WriteChatf("%s", pInvSlotWnd->pBackground->GetName().c_str());
	}
}