#pragma once

namespace mq::itemcolor {

	class ItemColor
	{
	protected:
		// Name of ItemColor
		std::string Name;

		// On Flag
		bool On;
		bool OnDefault;

		// Normal Color
		std::string NormalColor;
		std::string NormalColorDefault;

		// Rollover Color
		std::string RolloverColor;
		std::string RolloverColorDefault;

		// INI Section/Profile Names
		std::string GeneralSection;
		std::string ItemColorSection;
		std::string OnProfile;
		std::string NormalProfile;
		std::string RolloverProfile;

	public:
		ItemColor(std::string Name, bool On, std::string NormalColor, std::string RolloverColor) :
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
		unsigned int GetNormalColor() { return std::stoul(NormalColor, nullptr, 16); }
		unsigned int GetRolloverColor() { return std::stoul(RolloverColor, nullptr, 16); }

		// Sets for unsigned int version of Normal / Rollover colors
		unsigned int SetNormalColor(unsigned int newColor) { NormalColor = newColor; }
		unsigned int SetRolloverColor(unsigned int newColor) { RolloverColor = newColor; }

		// Gets for string version of Normal / Rollover colors
		std::string GetNormalColorString() { return NormalColor; }
		std::string GetRolloverColorString() { return RolloverColor; }

		// Sets for string version of Normal / Rollover colors
		void SetNormalColorString(std::string& newColor) { RolloverColor = newColor; }
		void SetRolloverColorString(std::string& newColor) { RolloverColor = newColor; }


		void ReadColorINI(std::string iniFileName)
		{
			// Grab On flag from INI, write out default if not there
			On = GetPrivateProfileBool(GeneralSection, OnProfile, OnDefault, iniFileName);

			// Grab Normal Color from INI, write out default if not there
			NormalColor = GetPrivateProfileString(ItemColorSection, NormalProfile, NormalColorDefault, iniFileName);

			// Grab Rollover Color from INI, write out default if not there
			RolloverColor = GetPrivateProfileString(ItemColorSection, RolloverProfile, RolloverColor, iniFileName);
		}

		void WriteColorINI(std::string iniFileName)
		{
			// Grab On flag from INI, write out default if not there
			WritePrivateProfileBool(GeneralSection, OnProfile, On, iniFileName);

			// Grab Normal Color from INI, write out default if not there
			WritePrivateProfileString(ItemColorSection, NormalProfile, NormalColor, iniFileName);

			// Grab Rollover Color from INI, write out default if not there
			WritePrivateProfileString(ItemColorSection, RolloverProfile, RolloverColor, iniFileName);
		}

		void LoadFromIni(std::string iniFileName)
		{
			// Grab On flag from INI, write out default if not there
			On = GetPrivateProfileBool(GeneralSection, OnProfile, OnDefault, iniFileName);
			WritePrivateProfileBool(GeneralSection, OnProfile, On, iniFileName);

			// Grab Normal Color from INI, write out default if not there
			NormalColor = GetPrivateProfileString(ItemColorSection, NormalProfile, NormalColorDefault, iniFileName);
			WritePrivateProfileString(ItemColorSection, NormalProfile, NormalColor, iniFileName);

			// Grab Rollover Color from INI, write out default if not there
			RolloverColor = GetPrivateProfileString(ItemColorSection, RolloverProfile, RolloverColor, iniFileName);
			WritePrivateProfileString(ItemColorSection, RolloverProfile, RolloverColor, iniFileName);
		}

	};

}