#pragma once

// ClientProfile.h - which of the game's systems this build offers, whatever theme draws the UI.
//
// This build is the MU 0.97k client, which had none of the later clients' systems below. A theme
// only changes how the client looks (architecture-principles.md §22, §30), so these are fixed at
// compile time rather than read from theme.ini: a player can't turn them back on by editing a
// file, and every theme shows the same game. Set one to true to bring that system back.
namespace Core::ClientProfile
{
// The master skill tree: the A key and the character window's master level button.
inline constexpr bool HasMasterSkillTree = false;

// The server's chat command list: the J key and the command window's Special button.
inline constexpr bool HasChatCommandList = false;

// The friend list: the F key, the HUD's friend button and the command window's Add Friend button.
inline constexpr bool HasFriends = false;

// The window menu: the U key and the HUD's menu button.
inline constexpr bool HasWindowMenu = false;

// The in-game shop: the X key and the HUD's shop button.
inline constexpr bool HasInGameShop = false;

// The Gens ranking: the B key.
inline constexpr bool HasGens = false;

// The fourth potion hotkey, R, as the later clients had. Without it R toggles the repair mode
// instead of L, as the 0.97k client's three potion boxes left it free.
inline constexpr bool HasFourthItemHotKey = false;
} // namespace Core::ClientProfile
