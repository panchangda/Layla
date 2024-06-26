// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once
#include "OnlineSessionSettings.h"

/**
 * General session settings for a Layla game
 */
class FLaylaOnlineSessionSettings : public FOnlineSessionSettings
{
public:

	FLaylaOnlineSessionSettings(bool bIsLAN = false, bool bIsPresence = false, int32 MaxNumPlayers = 4);
	virtual ~FLaylaOnlineSessionSettings() {}
};

/**
 * General search setting for a Layla game
 */
class FLaylaOnlineSearchSettings : public FOnlineSessionSearch
{
public:
	FLaylaOnlineSearchSettings(bool bSearchingLAN = false, bool bSearchingPresence = false);

	virtual ~FLaylaOnlineSearchSettings() {}
};

/**
 * Search settings for an empty dedicated server to host a match
 */
class FLaylaOnlineSearchSettingsEmptyDedicated : public FLaylaOnlineSearchSettings
{
public:
	FLaylaOnlineSearchSettingsEmptyDedicated(bool bSearchingLAN = false, bool bSearchingPresence = false);

	virtual ~FLaylaOnlineSearchSettingsEmptyDedicated() {}
};
