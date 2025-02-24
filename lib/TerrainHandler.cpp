/*
 * Terrain.cpp, part of VCMI engine
 *
 * Authors: listed in file AUTHORS in main folder
 *
 * License: GNU General Public License v2.0 or later
 * Full text of license available in license.txt file, in main folder
 *
 */

#include "StdInc.h"
#include "TerrainHandler.h"
#include "CGeneralTextHandler.h"
#include "GameSettings.h"
#include "JsonNode.h"
#include "modding/IdentifierStorage.h"

VCMI_LIB_NAMESPACE_BEGIN

TerrainType * TerrainTypeHandler::loadFromJson( const std::string & scope, const JsonNode & json, const std::string & identifier, size_t index)
{
	assert(identifier.find(':') == std::string::npos);

	auto * info = new TerrainType;

	info->id = TerrainId(index);
	info->identifier = identifier;
	info->modScope = scope;
	info->moveCost = static_cast<int>(json["moveCost"].Integer());
	info->musicFilename = AudioPath::fromJson(json["music"]);
	info->tilesFilename = AnimationPath::fromJson(json["tiles"]);
	info->horseSound = AudioPath::fromJson(json["horseSound"]);
	info->horseSoundPenalty = AudioPath::fromJson(json["horseSoundPenalty"]);
	info->transitionRequired = json["transitionRequired"].Bool();
	info->terrainViewPatterns = json["terrainViewPatterns"].String();

	VLC->generaltexth->registerString(scope, info->getNameTextID(), json["text"].String());

	const JsonVector & unblockedVec = json["minimapUnblocked"].Vector();
	info->minimapUnblocked =
	{
		static_cast<ui8>(unblockedVec[0].Float()),
		static_cast<ui8>(unblockedVec[1].Float()),
		static_cast<ui8>(unblockedVec[2].Float())
	};

	const JsonVector &blockedVec = json["minimapBlocked"].Vector();
	info->minimapBlocked =
	{
		static_cast<ui8>(blockedVec[0].Float()),
		static_cast<ui8>(blockedVec[1].Float()),
		static_cast<ui8>(blockedVec[2].Float())
	};

	info->passabilityType = 0;

	for(const auto& node : json["type"].Vector())
	{
		//Set bits
		const auto & s = node.String();
		if (s == "LAND") info->passabilityType |= TerrainType::PassabilityType::LAND;
		if (s == "WATER") info->passabilityType |= TerrainType::PassabilityType::WATER;
		if (s == "ROCK") info->passabilityType |= TerrainType::PassabilityType::ROCK;
		if (s == "SURFACE") info->passabilityType |= TerrainType::PassabilityType::SURFACE;
		if (s == "SUB") info->passabilityType |= TerrainType::PassabilityType::SUBTERRANEAN;
	}

	info->river = River::NO_RIVER;
	if(!json["river"].isNull())
	{
		VLC->identifiers()->requestIdentifier("river", json["river"], [info](int32_t identifier)
		{
			info->river = RiverId(identifier);
		});
	}

	for(const auto & t : json["paletteAnimation"].Vector())
	{
		TerrainPaletteAnimation element{
			static_cast<int>(t["start"].Integer()),
			static_cast<int>(t["length"].Integer())
		};
		info->paletteAnimation.push_back(element);
	}

	info->shortIdentifier = json["shortIdentifier"].String();
	assert(info->shortIdentifier.length() == 2);

	for(const auto & t : json["battleFields"].Vector())
	{
		VLC->identifiers()->requestIdentifier("battlefield", t, [info](int32_t identifier)
		{
			info->battleFields.emplace_back(identifier);
		});
	}

	for(const auto & t : json["prohibitTransitions"].Vector())
	{
		VLC->identifiers()->requestIdentifier("terrain", t, [info](int32_t identifier)
		{
			info->prohibitTransitions.emplace_back(identifier);
		});
	}

	info->rockTerrain = ETerrainId::ROCK;

	if(!json["rockTerrain"].isNull())
	{
		VLC->identifiers()->requestIdentifier("terrain", json["rockTerrain"], [info](int32_t identifier)
		{
			info->rockTerrain = TerrainId(identifier);
		});
	}

	return info;
}

const std::vector<std::string> & TerrainTypeHandler::getTypeNames() const
{
	static const std::vector<std::string> typeNames = { "terrain" };
	return typeNames;
}

std::vector<JsonNode> TerrainTypeHandler::loadLegacyData()
{
	size_t dataSize = VLC->settings()->getInteger(EGameSettings::TEXTS_TERRAIN);

	objects.resize(dataSize);

	CLegacyConfigParser terrainParser(TextPath::builtin("DATA/TERRNAME.TXT"));

	std::vector<JsonNode> result;
	do
	{
		JsonNode terrain;
		terrain["text"].String() = terrainParser.readString();
		result.push_back(terrain);
	}
	while (terrainParser.endLine());

	return result;
}

std::vector<bool> TerrainTypeHandler::getDefaultAllowed() const
{
	return {};
}

bool TerrainType::isLand() const
{
	return !isWater();
}

bool TerrainType::isWater() const
{
	return passabilityType & PassabilityType::WATER;
}

bool TerrainType::isPassable() const
{
	return !(passabilityType & PassabilityType::ROCK);
}

bool TerrainType::isSurface() const
{
	return passabilityType & PassabilityType::SURFACE;
}

bool TerrainType::isUnderground() const
{
	return passabilityType & PassabilityType::SUBTERRANEAN;
}

bool TerrainType::isSurfaceCartographerCompatible() const
{
	return isSurface();
}

bool TerrainType::isUndergroundCartographerCompatible() const
{
	return isLand() && isPassable() && !isSurface();
}

bool TerrainType::isTransitionRequired() const
{
	return transitionRequired;
}

std::string TerrainType::getJsonKey() const
{
	return modScope + ":" + identifier;
}

std::string TerrainType::getNameTextID() const
{
	return TextIdentifier( "terrain", modScope, identifier, "name" ).get();
}

std::string TerrainType::getNameTranslated() const
{
	return VLC->generaltexth->translate(getNameTextID());
}

VCMI_LIB_NAMESPACE_END
