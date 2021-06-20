#include "Utils.h"
#include "Settings/settings.h"
#include <algorithm>
#include <cctype>
#include <fcntl.h>
#include "Xorstr.h"


bool Utils::approxEqual(float v1, float v2)
{
	return abs(v1 - v2) < pow(10, -7);
}


float Utils::GetDistance2D(float fLocation2X, float fLocation1X, float fLocation2Y, float fLocation1Y)
{
	return sqrt(((fLocation2X - fLocation1X) * (fLocation2X - fLocation1X)) + ((fLocation2Y - fLocation1Y) * (fLocation2Y - fLocation1Y)));
}

float Utils::GetDistance(C3Vector Pos)
{
	return GetDistance2D(Pos.x, LuaScript::ActivePlayer->GetUnitPosition().x, Pos.y, LuaScript::ActivePlayer->GetUnitPosition().y);
}

bool Utils::ValidCoord(WObject* entity)
{
	if ((entity->GetUnitPosition().x == 0 && entity->GetUnitPosition().y == 0 && entity->GetUnitPosition().z == 0) || GetDistance(entity->GetUnitPosition()) >= 100000) return FALSE;
	return TRUE;
}

long Utils::GetEpochTime()
{
	using namespace std::chrono;
	milliseconds ms = duration_cast<milliseconds>(
		system_clock::now().time_since_epoch()
		);

	return ms.count();
}

ImColor Utils::GetFactionColor(WObject* player)
{
	int RaceID = player->sUnitField->RaceID;
	int TypeID = (int)player->GetType();
	ImColor FactionColor;

	if (TypeID == (int)TypeId::CGPlayer || TypeID == (int)TypeId::CGActivePlayer)
	{
		if (RaceID == WoWRace::Undead || RaceID == WoWRace::Troll || RaceID == WoWRace::TrollFemale || RaceID == WoWRace::Tauren || RaceID == WoWRace::Orc)
			FactionColor = ImColor(1.00f, 0.0f, 0.0f, 1.0f);
		else if (RaceID == WoWRace::Human || RaceID == WoWRace::Dwarf || RaceID == WoWRace::Gnome || RaceID == WoWRace::NightElf)
			FactionColor = ImColor(0.00f, 0.0f, 1.0f, 1.0f);
	}
	else { FactionColor; }

	return FactionColor;
}


ImColor Utils::GetClassColor(WObject* entity)
{
	int RaceID = entity->sUnitField->ClassID;
	ImColor Class;

	if (RaceID == WoWClass::None) { Class = ImColor(0.0f, 0.0f, 0.0f, 1.0f); }
	else if (RaceID == WoWClass::Warrior) { Class = ImColor(0.78f, 0.61f, 0.43f, 1.0f); }
	else if (RaceID == WoWClass::Paladin) { Class = ImColor(0.96f, 0.55f, 0.73f, 1.0f); }
	else if (RaceID == WoWClass::Hunter) { Class = ImColor(0.67f, 0.83f, 0.45f, 1.0f); }
	else if (RaceID == WoWClass::Rogue) { Class = ImColor(1.0f, 0.96f, 0.41f, 1.0f); }
	else if (RaceID == WoWClass::Priest) { Class = ImColor(1.0f, 1.0f, 1.0f, 1.0f); }
	else if (RaceID == WoWClass::DeathKnight) { Class = ImColor(0.77f, 0.12f, 0.23f, 1.0f); }
	else if (RaceID == WoWClass::Shaman) { Class = ImColor(0.00f, 0.44f, 0.87f, 1.0f); }
	else if (RaceID == WoWClass::Mage) { Class = ImColor(0.25f, 0.78f, 0.92f, 1.0f); }
	else if (RaceID == WoWClass::Warlock) { Class = ImColor(0.53f, 0.53f, 0.93f, 1.0f); }
	else if (RaceID == WoWClass::Druid) { Class = ImColor(1.0f, 0.49f, 0.04f, 1.0f); }
	else { Class = ImColor(1.0f, 0.49f, 1.04f, 1.0f); }

	return Class;
}
ImColor Utils::GetRainbowColor(float speed)
{
	speed = 0.002f * speed;
	long now = Utils::GetEpochTime();
	float hue = (now % (int)(1.0f / speed)) * speed;

	return ImColor::HSV(hue, 1.0f, 1.0f);
}

Color Utils::GetHealthColor(int hp)
{
	return Color(
		min(510 * (100 - hp) / 100, 255),
		min(510 * hp / 100, 255),
		25
	);
}

Color Utils::GetHealthColor(WObject* player)
{
	return Color(
		min(510 * (player->sUnitField->MaxHealth - player->sUnitField->Health) / player->sUnitField->MaxHealth, 255),
		min(510 * player->sUnitField->Health / player->sUnitField->MaxHealth, 255),
		25
	);
}

std::string Utils::GetHealth(WObject* entity)
{
	std::string HealthStr;
	int TypeID = (int)entity->GetType();
	if (TypeID == (int)TypeId::CGPlayer || TypeID == (int)TypeId::CGUnit || TypeID == (int)TypeId::CGActivePlayer)
	{
		HealthStr = std::to_string(entity->sUnitField->Health) + "/" + std::to_string(entity->sUnitField->MaxHealth);
	}
	return HealthStr;
}

std::string Utils::GetEnergyOrMana(WObject* entity)
{	
	std::string EnergyOrManaStr;
	int TypeID = (int)entity->GetType();
	if (TypeID == (int)TypeId::CGPlayer || TypeID == (int)TypeId::CGUnit || TypeID == (int)TypeId::CGActivePlayer)
	{
		EnergyOrManaStr = std::to_string(entity->sUnitField->Energy) + "/" + std::to_string(entity->sUnitField->MaxEnergy);
	}
	return EnergyOrManaStr;
}


bool Utils::IsEntityAlive(WObject* entity)
{
	int TypeID = (int)entity->GetType();
	if (TypeID == (int)TypeId::CGPlayer || TypeID == (int)TypeId::CGUnit || TypeID == (int)TypeId::CGActivePlayer)
	{
		if (entity->sUnitField->Health > 0)
			return false;
	}
	return true;
}


int Utils::IsFriendlyOrEnemy(WObject* entity)
{
	int RaceID = entity->sUnitField->RaceID;
	int TypeID = (int)entity->GetType();

	if (TypeID == (int)TypeId::CGPlayer || TypeID == (int)TypeId::CGActivePlayer)
	{
		if (RaceID == WoWRace::Undead || RaceID == WoWRace::Troll || RaceID == WoWRace::TrollFemale || RaceID == WoWRace::Tauren || RaceID == WoWRace::Orc)
			return (int)TeamID::Horde;
		else if (RaceID == WoWRace::Human || RaceID == WoWRace::Dwarf || RaceID == WoWRace::Gnome || RaceID == WoWRace::NightElf)
			return (int)TeamID::Alliance;
	}
	return RaceID;
}


std::string Utils::IsHordeOrAlliance(WObject* player)
{
	int RaceID = player->sUnitField->RaceID;
	int TypeID = (int)player->GetType();
	std::string Result;


	if (TypeID == (int)TypeId::CGPlayer || TypeID == (int)TypeId::CGActivePlayer)
	{
		if (RaceID == WoWRace::Undead || RaceID == WoWRace::Troll || RaceID == WoWRace::TrollFemale || RaceID == WoWRace::Tauren || RaceID == WoWRace::Orc)
			Result = "Horde";
		else if (RaceID == WoWRace::Human || RaceID == WoWRace::Dwarf || RaceID == WoWRace::Gnome || RaceID == WoWRace::NightElf)
			Result = "Alliance";
	}
	else { Result = ""; }
	
	return Result;
}



std::string Utils::GetRace(WObject* entity)
{
	int RaceID = entity->sUnitField->RaceID;
	int TypeID = (int)entity->GetType();
	std::string Race;
	if (TypeID == (int)TypeId::CGPlayer || TypeID == (int)TypeId::CGActivePlayer)
	{		
		if (RaceID == WoWRace::Undead) { Race = "Undead"; }
		else if (RaceID == WoWRace::Troll) { Race = "Troll"; }
		else if (RaceID == WoWRace::TrollFemale) { Race = "Troll"; }
		else if (RaceID == WoWRace::Tauren) { Race = "Tauren"; }
		else if (RaceID == WoWRace::Skeleton) { Race = "Skeleton"; }
		else if (RaceID == WoWRace::Orc) { Race = "Orc"; }
		else if (RaceID == WoWRace::NightElf) { Race = "Nightelf"; }
		else if (RaceID == WoWRace::Naga) { Race = "Naga"; }
		else if (RaceID == WoWRace::Human) { Race = "Human"; }
		else if (RaceID == WoWRace::Goblin) { Race = "Goblin"; }
		else if (RaceID == WoWRace::Gnome) { Race = "Gnome"; }
		else if (RaceID == WoWRace::FelOrc) { Race = "FelOrc"; }
		else if (RaceID == WoWRace::Dwarf) { Race = "Dwarf"; }
		else if (RaceID == WoWRace::Draenei) { Race = "Draenei"; }
		else if (RaceID == WoWRace::Broken) { Race = "Broken"; }
		else if (RaceID == WoWRace::BloodElf) { Race = "BloodElf"; }
	}
		return Race;
}

std::string Utils::GetClass(WObject* entity)
{
	int RaceID = entity->sUnitField->ClassID;
	int TypeID = (int)entity->GetType();
	std::string Class;

	if (TypeID == (int)TypeId::CGPlayer || TypeID == (int)TypeId::CGActivePlayer)
	{
		if (RaceID == WoWClass::None) { Class = "None"; }
		else if (RaceID == WoWClass::Warrior) { Class = "Warrior"; }
		else if (RaceID == WoWClass::Paladin) { Class = "Paladin"; }
		else if (RaceID == WoWClass::Hunter) { Class = "Hunter"; }
		else if (RaceID == WoWClass::Rogue) { Class = "Rogue"; }
		else if (RaceID == WoWClass::Priest) { Class = "Priest"; }
		else if (RaceID == WoWClass::DeathKnight) { Class = "Deathknight"; }
		else if (RaceID == WoWClass::Shaman) { Class = "Shaman"; }
		else if (RaceID == WoWClass::Mage) { Class = "Mage"; }
		else if (RaceID == WoWClass::Warlock) { Class = "Warlock"; }
		else if (RaceID == WoWClass::Druid) { Class = "Druid"; }
	}
	return Class;
}

std::string Utils::GetObjType(WObject* entity)
{
	int TypeID = (int)entity->GetType();
	std::string Object;
	if (TypeID == (int)TypeId::CGActivePlayer) { Object = "CGActivePlayer"; }
	else if (TypeID == (int)TypeId::CGPlayer) { Object = "CGPlayer"; }
	else if (TypeID == (int)TypeId::CGUnit) { Object = "CGUnit"; }
	else if (TypeID == (int)TypeId::CGGameObject) { Object = "CGGameObj"; }
	else if (TypeID == (int)TypeId::CGCorpse) { Object = "CGCorpse"; }
	else if (TypeID == (int)TypeId::CGDynamicObject) { Object = "CGDynamicObj"; }
	else if (TypeID == (int)TypeId::CGObject) { Object = "CGObj"; }
	return Object;
}

int Utils::filterException(int code, PEXCEPTION_POINTERS ex) {
	std::cout << "[!] Filtering " << std::hex << code << std::endl;
	return EXCEPTION_EXECUTE_HANDLER;
}
