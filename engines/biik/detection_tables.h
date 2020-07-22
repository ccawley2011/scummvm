/* ScummVM - Graphic Adventure Engine
 *
 * ScummVM is the legal property of its developers, whose names
 * are too numerous to list here. Please refer to the COPYRIGHT
 * file distributed with this source distribution.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 */

namespace Biik {

#define SILENT_OPTIONS GUIO5(GUIO_NOMIDI, GUIO_NOSUBTITLES, GUIO_NOSPEECH, GUIO_NOMUSIC, GUIO_NOSFX)
#define FLOPPY_OPTIONS GUIO4(GUIO_NOMIDI, GUIO_NOSUBTITLES, GUIO_NOSPEECH, GUIO_LINKMUSICTOSFX)
#define CD_OPTIONS GUIO3(GUIO_NOMIDI, GUIO_NOSUBTITLES, GUIO_LINKMUSICTOSFX)

static const ADGameDescription gameDescriptions[] = {
	// Darryl the Dragon - RISC OS demo version 1.00 (24/03/1993)
	// This particular version does not use scripted logic, and as such is unlikely to be supported.
	{
		"darryl",
		"Demo",
		{
			{ "file1", GAME_STARTFILE, "1686823220e6a3a38f092d68913c380b", 504460 },
			{ "Sprites", GAME_SPRITEFILE, "14bba4f913ba14fc6238746f6f81916c", 18752 },
			{ "Templates", GAME_TEMPLATEFILE, "b7f47dbe4f44263abc60fa6dda105a94", 3511 },
			{ NULL, 0, NULL, 0 }
		},
		Common::EN_ANY,
		Common::kPlatformAcorn,
		ADGF_UNSTABLE | ADGF_DEMO,
		FLOPPY_OPTIONS
	},

	// Darryl the Dragon - Windows version 1.03 (08/12/2004)
	{
		"darryl",
		"",
		{
			{ "SETUP.DAS", GAME_STARTFILE, "3d8552419185706c900a5c5c4d8b0ddc", 21464 },
			{ "DARRYL.EXE", GAME_EXEFILE, "4f8c5f5d3df7a143bd41dbb4bfbc8049", 328976 },
			{ NULL, 0, NULL, 0 }
		},
		Common::EN_ANY,
		Common::kPlatformWindows,
		ADGF_UNSTABLE,
		FLOPPY_OPTIONS
	},

	// Dinosaur Discovery - RISC OS demo version 1.00 (11/10/1993)
	{
		"dinosaur",
		"Demo",
		{
			{ "file1", GAME_STARTFILE, "46b6200dd7a385f290947db816a99e36", 88248 },
			{ "Sprites", GAME_SPRITEFILE, "7cf61b2d389a8d59b85a2e101c223815", 9648 },
			{ "Templates", GAME_TEMPLATEFILE, "f1142d255a726984e7b635db4f4f0fb3", 3734 },
			{ NULL, 0, NULL, 0 }
		},
		Common::EN_ANY,
		Common::kPlatformAcorn,
		ADGF_UNSTABLE | ADGF_DEMO,
		SILENT_OPTIONS
	},

	// Dinosaur Discovery - Windows version 1.03 (30/11/2004)
	{
		"dinosaur",
		"",
		{
			{ "FILE1.DAS", GAME_STARTFILE, "dd6a0d568b7d21ea47a3b00bad6664d8", 178224 },
			{ "DINOSAUR.EXE", GAME_EXEFILE, "63352b68ff51b18ef3e78cb1fa5e3509", 310944 },
			{ NULL, 0, NULL, 0 }
		},
		Common::EN_ANY,
		Common::kPlatformWindows,
		ADGF_UNSTABLE,
		FLOPPY_OPTIONS
	},

	// Explore with Flossy the Frog - RISC OS demo version 1.00 (12/08/1993)
	{
		"flossy",
		"Demo",
		{
			{ "file1", GAME_STARTFILE, "6ad387516d28a6c4a7c606b3ca50ef40", 196084 },
			{ "Sprites", GAME_SPRITEFILE, "224b7d77a0f8839fa65e00998e26ac58", 21960 },
			{ "Templates", GAME_TEMPLATEFILE, "50f3e2bcc6ea98318111a34622cbe5da", 3519 },
			{ NULL, 0, NULL, 0 }
		},
		Common::EN_ANY,
		Common::kPlatformAcorn,
		ADGF_UNSTABLE | ADGF_DEMO,
		FLOPPY_OPTIONS
	},

	// Betsi the Tudor Dog - RISC OS demo version 1.00 (24/05/1994)
	{
		"betsi",
		"Demo",
		{
			{ "file1", GAME_STARTFILE, "79d94f51ab5ee293d78e1f8de199195b", 135420},
			{ "Sprites", GAME_SPRITEFILE, "c1370082bd80f2df91971641c15e3a4b", 36452},
			{ "Templates", GAME_TEMPLATEFILE, "3ccbfbeae6f7b3e25b68003c14364b83", 3471},
			{ NULL, 0, NULL, 0 }
		},
		Common::EN_ANY,
		Common::kPlatformAcorn,
		ADGF_UNSTABLE | ADGF_DEMO,
		FLOPPY_OPTIONS
	},

	// Betsi the Tudor Dog - Windows version 4.0.0 (15/01/2007)
	{
		"betsi",
		"",
		{
			{ "FILE1.DAS", GAME_STARTFILE, "3a35bf16dcf3188ef99225ec20f59e23", 588952 },
			{ "Betsi.exe", GAME_EXEFILE, "f3812620fedcf8c083ebddd812fefdd1", 591392 },
			{ NULL, 0, NULL, 0 }
		},
		Common::EN_ANY,
		Common::kPlatformWindows,
		ADGF_UNSTABLE | ADGF_CD,
		CD_OPTIONS
	},

	// Betsi the Tudor Dog - Macintosh version ? (13/10/2004)
	{
		"betsi",
		"",
		{
			{ "FILE1", GAME_STARTFILE, "5818e39cb415a846645b3cc397a5d0e5", 663352 },
			{ "Betsi", GAME_EXEFILE, "0a2d1e6367c4fbacf05e88820ed102dc", 51883 },
			{ NULL, 0, NULL, 0 }
		},
		Common::EN_ANY,
		Common::kPlatformMacintosh,
		ADGF_UNSTABLE | ADGF_CD | ADGF_MACRESFORK,
		CD_OPTIONS
	},

	// A Mouse in Holland - RISC OS demo version 1.00 (23/06/1994)
	{
		"mouse",
		"Demo",
		{
			{ "file1", GAME_STARTFILE, "2538cf4b3d5ff9a6a4b5507b3b884655", 85844 },
			{ "Sprites", GAME_SPRITEFILE, "1e9e368db2d20ae39fec6d5de12f1829", 36180 },
			{ "Templates", GAME_TEMPLATEFILE, "d432688e3c5305f086afa2553eeac79a", 3741 },
			{ NULL, 0, NULL, 0 }
		},
		Common::EN_ANY,
		Common::kPlatformAcorn,
		ADGF_UNSTABLE | ADGF_DEMO,
		FLOPPY_OPTIONS
	},

	// Guardians of the Greenwood - RISC OS demo version 1.00 (15/05/1995)
	{
		"guardians",
		"Demo",
		{
			{ "file1", GAME_STARTFILE, "e94aba8bc1d820bc1e33773655060de6", 501780 },
			{ "Sprites", GAME_SPRITEFILE, "7400da14c16c79fbf16ba7c33c5977d5", 19580 },
			{ "Templates", GAME_TEMPLATEFILE, "c5981d4aea9bee1f5c4aded78a45fc79", 3769 },
			{ NULL, 0, NULL, 0 }
		},
		Common::EN_ANY,
		Common::kPlatformAcorn,
		ADGF_UNSTABLE | ADGF_CD | ADGF_DEMO,
		CD_OPTIONS
	},

	// Patch the Puppy - Windows version 4.0.0 (19/11/2007)
	{
		"puppy",
		"",
		{
			{ "FILE1.DAS", GAME_STARTFILE, "ef4dc086d2b751bcc078c52e0803a5f9", 396 },
			{ "Puppy.exe", GAME_EXEFILE, "6b94ad6b70dc092a2d0dc7968060717f", 720896 },
			{ NULL, 0, NULL, 0 }
		},
		Common::EN_ANY,
		Common::kPlatformWindows,
		ADGF_UNSTABLE | ADGF_CD,
		CD_OPTIONS
	},

	// Patch the Puppy - Macintosh version ? (08/11/2004)
	{
		"puppy",
		"",
		{
			{ "FILE1", GAME_STARTFILE, "2d5b613b52103336bf6f2f5c76c7c53f", 360 },
			{ "Patch", GAME_EXEFILE, "bc8996d3b52466fba5a94b5aff2e259c", 29092 },
			{ NULL, 0, NULL, 0 }
		},
		Common::EN_ANY,
		Common::kPlatformMacintosh,
		ADGF_UNSTABLE | ADGF_CD | ADGF_MACRESFORK,
		CD_OPTIONS
	},

	// Find It, Fix It - RISC OS demo version 1.00 (24/03/1997)
	{
		"finditfixit",
		"Bathroom Demo",
		{
			{ "File1", GAME_STARTFILE, "7387a6acae67fb664a5fe198e30428b8", 1244 },
			{ "Sprites", GAME_SPRITEFILE, "b28b961d0276133963268693b74ddf51", 28540 },
			{ "Templates", GAME_TEMPLATEFILE, "dbfa48c928b988378db3115663cee4bc", 3990 },
			{ "File3", 0, "48a54fc65b06eec6f0c22c5f989d9bd4", 70320 },
			{ NULL, 0, NULL, 0 }
		},
		Common::EN_ANY,
		Common::kPlatformAcorn,
		ADGF_UNSTABLE | ADGF_DEMO,
		SILENT_OPTIONS
	},

	// Find It, Fix It - RISC OS demo version 1.00 (24/03/1997)
	{
		"finditfixit",
		"Kitchen Demo",
		{
			{ "File1", GAME_STARTFILE, "7387a6acae67fb664a5fe198e30428b8", 1244 },
			{ "Sprites", GAME_SPRITEFILE, "b28b961d0276133963268693b74ddf51", 28540 },
			{ "Templates", GAME_TEMPLATEFILE, "dbfa48c928b988378db3115663cee4bc", 3990 },
			{ "File3", 0, "74841eb72ee82e3f2ea45984bb5471a2", 53384 },
			{ NULL, 0, NULL, 0 }
		},
		Common::EN_ANY,
		Common::kPlatformAcorn,
		ADGF_UNSTABLE | ADGF_DEMO,
		SILENT_OPTIONS
	},

	// Find It, Fix It - Windows demo version 1.00 (28/10/1997)
	{
		"finditfixit",
		"Bathroom Demo",
		{
			{ "FILE1.DAS", GAME_STARTFILE, "b708936a3ed6b15cc6080136f7a998a3", 1244 },
			{ "FIFI.EXE", GAME_EXEFILE, "34c551abc76dc6529534dbb942cb727f", 277072 },
			{ "FILE3.DAS", 0, "db0f5f8065de1df7ce53b16f53bfd0cc", 83292 },
			{ NULL, 0, NULL, 0 }
		},
		Common::EN_ANY,
		Common::kPlatformWindows,
		ADGF_UNSTABLE | ADGF_DEMO,
		SILENT_OPTIONS
	},

	// Find It, Fix It - Windows demo version 1.00 (28/10/1997)
	{
		"finditfixit",
		"Kitchen Demo",
		{
			{ "FILE1.DAS", GAME_STARTFILE, "b708936a3ed6b15cc6080136f7a998a3", 1244 },
			{ "FIFI.EXE", GAME_EXEFILE, "34c551abc76dc6529534dbb942cb727f", 277072 },
			{ "FILE3.DAS", 0, "84b0dc75731b1c0516bf65194cecb18b", 88552 },
			{ NULL, 0, NULL, 0 }
		},
		Common::EN_ANY,
		Common::kPlatformWindows,
		ADGF_UNSTABLE | ADGF_DEMO,
		SILENT_OPTIONS
	},

	AD_TABLE_END_MARKER
};

}
