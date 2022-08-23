/* ScummVM - Graphic Adventure Engine
 *
 * ScummVM is the legal property of its developers, whose names
 * are too numerous to list here. Please refer to the COPYRIGHT
 * file distributed with this source distribution.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "engines/advancedDetector.h"

#include "base/plugins.h"
#include "toon/toon.h"

static const PlainGameDescriptor toonGames[] = {
	{ "toon", "Toonstruck" },
	{ 0, 0 }
};

static const DebugChannelDef debugFlagList[] = {
	{Toon::kDebugAnim, "Anim", "Animation debug level"},
	{Toon::kDebugCharacter, "Character", "Character debug level"},
	{Toon::kDebugAudio, "Audio", "Audio debug level"},
	{Toon::kDebugHotspot, "Hotspot", "Hotspot debug level"},
	{Toon::kDebugFont, "Font", "Font debug level"},
	{Toon::kDebugPath, "Path", "Path debug level"},
	{Toon::kDebugMovie, "Movie", "Movie debug level"},
	{Toon::kDebugPicture, "Picture", "Picture debug level"},
	{Toon::kDebugResource, "Resource", "Resource debug level"},
	{Toon::kDebugState, "State", "State debug level"},
	{Toon::kDebugTools, "Tools", "Tools debug level"},
	{Toon::kDebugText, "Text", "Text debug level"},
	DEBUG_CHANNEL_END
};

namespace Toon {

static const ADGameDescription gameDescriptions[] = {
	{
		"toon", "",
		AD_ENTRY3s("local.pak", "3290209ef9bc92692108dd2f45df0736", 3237611,
		           "arcaddbl.svl", "c418478cd2833c7c983799f948af41ac", 7844688,
		           "study.svl", "281efa3f33f6712c0f641a605f4d40fd", 2511090),
		Common::EN_ANY, Common::kPlatformDOS, ADGF_NO_FLAGS, GUIO1(GUIO_NOMIDI)
	},
	{
		"toon", "",
		AD_ENTRY3s("local.pak", "517132c3575b38806d1e7b6f59848072", 3224044,
		           "arcaddbl.svl", "ff74008827b62fbef1f46f104c438e44", 9699256,
		           "study.svl", "df056b94ea83f1ed92a539cf636053ab", 2542668),
		Common::FR_FRA, Common::kPlatformDOS, ADGF_NO_FLAGS, GUIO1(GUIO_NOMIDI)
	},
	{
		"toon", "",
		AD_ENTRY3s("local.pak", "bf5da4c03f78ffbd643f12122319366e", 3250841,
		           "arcaddbl.svl", "7a0d74f4d66d1c722b946abbeb0834ef", 9122249,
		           "study.svl", "72fe96a9e10967d3138e918295babc42", 2910283),
		Common::DE_DEU, Common::kPlatformDOS, ADGF_NO_FLAGS, GUIO1(GUIO_NOMIDI)
	},
	{
		"toon", "",
		AD_ENTRY3s("local.pak", "e8645168a247e2abdbfc2f9fa9d1c0fa", 3232222,
		           "arcaddbl.svl", "7893ac4cc78d51356baa058bbee7aa28", 8275016,
		           "study.svl", "b6b1ee2d9d94d53d305856039ab7bde7", 2634620),
		Common::ES_ESP, Common::kPlatformDOS, ADGF_NO_FLAGS, GUIO1(GUIO_NOMIDI)
	},
	{
		"toon", "",
		AD_ENTRY3s("local.pak", "48ec60709bebbdeff791d55ee18ec910", 3417846,
		           "arcaddbl.svl", "1d1b96e317e03ffd3874a8ebe59556f3", 6246232,
		           "study.svl", "d4aff126ee27be3c3d25e2996369d7cb", 2324368),
		Common::RU_RUS, Common::kPlatformDOS, ADGF_NO_FLAGS, GUIO1(GUIO_NOMIDI)
	},
	{
		"toon", "Demo",
		AD_ENTRY4s("local.pak", "bf5da4c03f78ffbd643f12122319366e", 3250841,
		           "wacexdbl.emc", "cfbc2156a31b294b038204888407ebc8", 6974,
		           "generic.svl", "5eb99850ada22f0b8cf6392262d4dd07", 9404599,
		           "onetime.pak", "b344aee5fd43b7438c322fb29f860205", 468667)
		Common::DE_DEU, Common::kPlatformDOS, ADGF_DEMO, GUIO1(GUIO_NOMIDI)
	},
	{
		"toon", "Demo",
		AD_ENTRY2s("local.pak", "8ef3368078b9ea70b305c04db826feea", 2680573,
		           "generic.svl", "5c42724bb93b360dca7044d6b7ef26e5", 7739319),
		Common::EN_ANY, Common::kPlatformDOS, ADGF_DEMO, GUIO1(GUIO_NOMIDI)
	},
	{
		// English 2-CD "Sold out" release
		"toon", "",
		AD_ENTRY2s("local.pak", "3290209ef9bc92692108dd2f45df0736", 3237611,
		           "generic.svl", "331eead1d20af7ee809a9e2f35b8362f", 6945180),
		Common::EN_ANY, Common::kPlatformDOS, ADGF_NO_FLAGS, GUIO1(GUIO_NOMIDI)
	},

	AD_TABLE_END_MARKER
};

static const ADFileBasedFallback fileBasedFallback[] = {
	{ &gameDescriptions[0], { "local.pak", "arcaddbl.svl", "study.svl", 0 } }, // default to english version
	{ 0, { 0 } }
};

} // End of namespace Toon

static const char * const directoryGlobs[] = {
	"misc",
	"act1",
	"arcaddbl",
	"act2",
	"study",
	0
};

class ToonMetaEngineDetection : public AdvancedMetaEngineDetection {
public:
	ToonMetaEngineDetection() : AdvancedMetaEngineDetection(Toon::gameDescriptions, sizeof(ADGameDescription), toonGames) {
		_maxScanDepth = 3;
		_directoryGlobs = directoryGlobs;
	}

	ADDetectedGame fallbackDetect(const FileMap &allFiles, const Common::FSList &fslist, ADDetectedGameExtraInfo **extra) const override {
		return detectGameFilebased(allFiles, Toon::fileBasedFallback);
	}

	const char *getName() const override {
		return "toon";
	}

	const char *getEngineName() const override {
		return "Toonstruck";
	}

	const char *getOriginalCopyright() const override {
		return "Toonstruck (C) 1996 Virgin Interactive";
	}

	const DebugChannelDef *getDebugChannels() const override {
		return debugFlagList;
	}
};

REGISTER_PLUGIN_STATIC(TOON_DETECTION, PLUGIN_TYPE_ENGINE_DETECTION, ToonMetaEngineDetection);
