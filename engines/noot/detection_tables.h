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

namespace Noot {

static const ADGameDescription gameDescriptions[] = {
	{
		"granny",
		"Demo",
		AD_ENTRY1s("Granny", "928963ae8b69f4bf10857eb4d2e0a9e4", 399836),
		Common::EN_ANY,
		Common::kPlatformAcorn,
		ADGF_UNSTABLE|ADGF_DEMO,
		GUIO1(GUIO_NONE)
	},

	/**
	 * Examples supplied with the authoring system.
	 */

	{
		"dweek",
		"",
		AD_ENTRY1s("Dweek", "d43238b1c85a0f0a9ec47c0c27de327c", 744580),
		Common::EN_ANY,
		Common::kPlatformAcorn,
		ADGF_UNSTABLE,
		GUIO1(GUIO_NONE)
	},

	{
		"dweep",
		"",
		AD_ENTRY1s("Dweep", "cfa9da642eff7388fbf6af40d716874e", 694400),
		Common::EN_ANY,
		Common::kPlatformAcorn,
		ADGF_UNSTABLE,
		GUIO1(GUIO_NONE)
	},

	{
		"farm",
		"",
		AD_ENTRY1s("Farm", "2d541c4eb58db6747697c60f249658b4", 810492),
		Common::EN_ANY,
		Common::kPlatformAcorn,
		ADGF_UNSTABLE,
		GUIO1(GUIO_NONE)
	},

	{
		"howitworks",
		"",
		AD_ENTRY1s("HowItWorks", "8db18478df6d12357c7fe0698fd615e7", 407908),
		Common::EN_ANY,
		Common::kPlatformAcorn,
		ADGF_UNSTABLE,
		GUIO1(GUIO_NONE)
	},

	{
		"playroom",
		"",
		AD_ENTRY1s("Playroom", "8ee4bfd18763240c2aa5f7a5eddde85d", 404536),
		Common::EN_ANY,
		Common::kPlatformAcorn,
		ADGF_UNSTABLE,
		GUIO1(GUIO_NONE)
	},

	{
		"power",
		"",
		AD_ENTRY1s("Power", "e69bc96946ed8317695708a204fbeefc", 662528),
		Common::EN_ANY,
		Common::kPlatformAcorn,
		ADGF_UNSTABLE,
		GUIO1(GUIO_NONE)
	},

	/**
	 * Examples supplied with the demo version.
	 */

	{
		"dweek",
		"Demo",
		AD_ENTRY1s("Dweek", "872cece73bf299d27269b96f94739490", 142960),
		Common::EN_ANY,
		Common::kPlatformAcorn,
		ADGF_UNSTABLE|ADGF_DEMO,
		GUIO1(GUIO_NONE)
	},

	{
		"farm",
		"Demo",
		AD_ENTRY1s("Farm", "ad6ff318ee437131c87df0b3cebec73b", 112552),
		Common::EN_ANY,
		Common::kPlatformAcorn,
		ADGF_UNSTABLE|ADGF_DEMO,
		GUIO1(GUIO_NONE)
	},

	{
		"howitworks",
		"Demo",
		AD_ENTRY1s("HowItWorks", "0d7f48c1631c36edc06ece82c1c7b9c0", 95020),
		Common::EN_ANY,
		Common::kPlatformAcorn,
		ADGF_UNSTABLE|ADGF_DEMO,
		GUIO1(GUIO_NONE)
	},

	{
		"playroom",
		"Demo",
		AD_ENTRY1s("Playroom", "2c7aa088d68f0ef7b49b3b4bd9a5127a", 138392),
		Common::EN_ANY,
		Common::kPlatformAcorn,
		ADGF_UNSTABLE|ADGF_DEMO,
		GUIO1(GUIO_NONE)
	},

	AD_TABLE_END_MARKER
};

} // End of namespace Noot
