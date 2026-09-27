//
//  SuperTuxKart - a fun racing game with go-kart
//  Copyright (C) 2025 kimden
//
//  This program is free software; you can redistribute it and/or
//  modify it under the terms of the GNU General Public License
//  as published by the Free Software Foundation; either version 3
//  of the License, or (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program; if not, write to the Free Software
//  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

#include "utils/lobby_gp_manager.hpp"

#include "karts/abstract_kart.hpp"
#include "modes/linear_world.hpp"
#include "modes/world.hpp"
#include "modes/world_with_rank.hpp"
#include "network/game_setup.hpp"
#include "network/network_player_profile.hpp"
#include "network/network_string.hpp"
#include "network/protocols/server_lobby.hpp"
#include "network/server_config.hpp"
#include "utils/gp_scoring.hpp"
#include "utils/string_utils.hpp"
#include "utils/team_manager.hpp"

#include "main_loop.hpp"
#include "utils/communication.hpp"
#include "utils/lobby_settings.hpp"

void LobbyGPManager::setupContextUser()
{
    if (!trySettingGPScoring(ServerConfig::m_gp_scoring))
        gp_data.setGPScoring({});
}   // setupContextUser
//-----------------------------------------------------------------------------

void LobbyGPManager::onStartSelection()
{
    if (!getGameSetupFromCtx()->isGrandPrixStarted())
    {
        gp_data.getGPPlayerScores().clear();
        gp_data.getGPTeamScores().clear();
    }
}   // onStartSelection
//-----------------------------------------------------------------------------

void LobbyGPManager::setScoresToPlayer(std::shared_ptr<NetworkPlayerProfile> player)
{
    std::string username = StringUtils::wideToUtf8(player->getName());
    if (getGameSetupFromCtx()->isGrandPrix())
    {
        auto it = gp_data.getGPPlayerScores().find(username);
        if (it != gp_data.getGPPlayerScores().end())
        {
            player->setScore(it->second.score);
            player->setOverallTime(it->second.time);
        }
    }
}   // setScoresToPlayer
//-----------------------------------------------------------------------------

std::string LobbyGPManager::getGrandPrixStandings(bool showIndividual, bool showTeam)
{
    std::stringstream response;
    response << "Grand Prix standings";

    if (!showIndividual && !showTeam)
    {
        if (gp_data.getGPTeamScores().empty())
            showIndividual = true;
        else
            showTeam = true;
    }

    auto game_setup = getGameSetupFromCtx();
    int passed = (int)game_setup->getAllTracks().size();
    bool ongoing = false;
    if (getLobby()->isWorldPicked() && !getLobby()->isWorldFinished())
    {
        --passed;
        ongoing = true;
    }
    
    uint8_t total = game_setup->getExtraServerInfo();
    if (passed != 0 || ongoing)
    {
        response << " after " << (int)passed << " of " << (int)total << " games";
        if (ongoing)
            response << " (before this game)";
    }
    else
        response << ", " << (int)total << " games";

    response << ":\n";
    
    if (showIndividual)
    {
        std::vector<std::pair<GPScore, std::string>> results;
        for (auto &p: gp_data.getGPPlayerScores())
            results.emplace_back(p.second, p.first);
        std::stable_sort(results.rbegin(), results.rend());
        for (unsigned i = 0; i < results.size(); i++)
        {
            response << (i + 1) << ". ";
            response << "  " << results[i].second;
            response << "  " << results[i].first.score;
            response << "  " << "(" << StringUtils::timeToString(results[i].first.time) << ")";
            response << "\n";
        }
    }

    if (showTeam)
    {
        if (!gp_data.getGPTeamScores().empty())
        {
            std::vector<std::pair<GPScore, int>> results2;
            if (showIndividual)
                response << "\n";
            for (auto &p: gp_data.getGPTeamScores())
                results2.emplace_back(p.second, p.first);
            std::stable_sort(results2.rbegin(), results2.rend());
            for (unsigned i = 0; i < results2.size(); i++)
            {
                response << (i + 1) << ". ";
                response << "  " << TeamUtils::getTeamByIndex(results2[i].second).getNameWithEmoji();
                response << "  " << results2[i].first.score;
                response << "  " << "(" << StringUtils::timeToString(results2[i].first.time) << ")";
                response << "\n";
            }
        }
    }
    return response.str();
}   // getGrandPrixStandings
//-----------------------------------------------------------------------------

void LobbyGPManager::resetGrandPrix()
{
    gp_data.getGPPlayerScores().clear();
    gp_data.getGPTeamScores().clear();
    getGameSetupFromCtx()->stopGrandPrix();

    getLobby()->sendServerInfoToEveryone();
    getLobby()->updatePlayerList();
}   // resetGrandPrix
//-----------------------------------------------------------------------------

void LobbyGPManager::shuffleGPScoresWithPermutation(const std::map<int, int>& permutation)
{
    auto old_scores = gp_data.getGPTeamScores();
    gp_data.getGPTeamScores().clear();
    for (auto& p: old_scores)
    {
        auto it = permutation.find(p.first);
        if (it != permutation.end())
            gp_data.getGPTeamScores()[it->second] = p.second;
        else
            gp_data.getGPTeamScores()[p.first] = p.second;
    }
}   // shuffleGPScoresWithPermutation
//-----------------------------------------------------------------------------

void LobbyGPManager::updateGPScores(std::vector<float>& gp_changes, NetworkString* ns)
{
    // fastest lap
    int fastest_lap =
        static_cast<LinearWorld*>(World::getWorld())->getFastestLapTicks();
    irr::core::stringw fastest_kart_wide =
        static_cast<LinearWorld*>(World::getWorld())
        ->getFastestLapKartName();
    std::string fastest_kart = StringUtils::wideToUtf8(fastest_kart_wide);

    // all gp tracks
    auto game_setup = getGameSetupFromCtx();

    int points_fl = 0;
    // Commented until used to remove the warning
    // int points_pole = 0;
    WorldWithRank *wwr = dynamic_cast<WorldWithRank*>(World::getWorld());
    if (wwr)
    {
        points_fl = wwr->getFastestLapPoints();
        // Commented until used to remove the warning
        // points_pole = wwr->getPolePoints();
    }
    else
    {
        Log::error("LobbyGPManager",
                    "World with scores that is not a WorldWithRank??");
    }

    std::vector<int> last_scores;
    std::vector<int> cur_scores;
    std::vector<float> overall_times;

    for (unsigned i = 0; i < RaceManager::get()->getNumPlayers(); i++)
    {
        int last_score = (World::getWorld()->getKart(i)->isEliminated() ?
                0 : RaceManager::get()->getKartScore(i));
        gp_changes.push_back((float)last_score);
        int cur_score = last_score;
        float overall_time = RaceManager::get()->getOverallTime(i);
        std::string username = StringUtils::wideToUtf8(
            RaceManager::get()->getKartInfo(i).getPlayerName());
        if (username == fastest_kart)
        {
            gp_changes.back() += points_fl;
            cur_score += points_fl;
        }
        int team = getTeamManager()->getTeamForUsername(username);
        if (team > 0)
        {
            auto& item = gp_data.getGPTeamScores()[team];
            item.score += cur_score;
            item.time += overall_time;
        }
        last_score = gp_data.getGPPlayerScores()[username].score;
        cur_score += last_score;
        overall_time = overall_time + gp_data.getGPPlayerScores()[username].time;
        if (auto player =
            RaceManager::get()->getKartInfo(i).getNetworkPlayerProfile().lock())
        {
            player->setScore(cur_score);
            player->setOverallTime(overall_time);
        }
        auto& item = gp_data.getGPPlayerScores()[username];
        item.score = cur_score;
        item.time = overall_time;
        last_scores.push_back(last_score);
        cur_scores.push_back(cur_score);
        overall_times.push_back(overall_time);    
    }

    ns->addUInt32(fastest_lap);
    ns->encodeString(fastest_kart_wide);

    ns->addUInt8((uint8_t)game_setup->getTotalGrandPrixTracks())
                .addUInt8((uint8_t)game_setup->getAllTracks().size());

    for (const std::string& gp_track : game_setup->getAllTracks())
        ns->encodeString(gp_track);

    ns->addUInt8((uint8_t)RaceManager::get()->getNumPlayers());
    for (unsigned i = 0; i < RaceManager::get()->getNumPlayers(); i++)
    {
        ns->addUInt32(last_scores[i])
                    .addUInt32(cur_scores[i])
                    .addFloat(overall_times[i]);
    }
}   // updateGPScores
//-----------------------------------------------------------------------------

bool LobbyGPManager::trySettingGPScoring(const std::string& input)
{
    std::shared_ptr<GPScoring> new_scoring;
    
    try
    {
        new_scoring = GPScoring::createFromIntParamString(input);
    }
    catch (std::logic_error& ex)
    {
        Log::warn("Failed to create GP scoring from string (%s): %s",
                input.c_str(), ex.what());
        return false;
    }

    std::swap(gp_data.getGPScoring(), new_scoring);
    return true;
}   // trySettingGPScoring
//-----------------------------------------------------------------------------

void LobbyGPManager::updateWorldScoring()
{
    WorldWithRank *wwr = dynamic_cast<WorldWithRank*>(World::getWorld());
    if (wwr)
        wwr->setCustomScoringSystem(gp_data.getGPScoring());
}   // updateWorldScoring
//-----------------------------------------------------------------------------

std::string LobbyGPManager::getScoringAsString()
{
    std::string msg = "Current scoring is \"";
    if (gp_data.getGPScoring())
        msg += gp_data.getGPScoring()->toString();
    msg += "\"";
    return msg;
}   // getScoringAsString
//-----------------------------------------------------------------------------


void LobbyGPManager::armIdleQuitTimer()
{
    int minutes = getSettings()->getIdleQuitMinutes();

    if (minutes <= 0)
    {
        m_idle_quit_armed = false;
        return;
    }

    m_idle_quit_start_time = StkTime::getMonoTimeMs();
    m_idle_quit_last_warning = 0;
    m_idle_quit_armed = true;

    Log::info(
        "LobbyGPManager",
        "Idle quit timer armed for %d minute(s).",
        minutes
    );
}

void LobbyGPManager::disarmIdleQuitTimer()
{
    m_idle_quit_armed = false;
    m_idle_quit_start_time = 0;
    m_idle_quit_last_warning = 0;
}


void LobbyGPManager::checkIdleQuitTimer()
{
    if (!m_idle_quit_armed)
        return;

    int minutes = getSettings()->getIdleQuitMinutes();

    if (minutes <= 0)
    {
        disarmIdleQuitTimer();
        return;
    }

    uint64_t now = StkTime::getMonoTimeMs();

    uint64_t timeout =
        static_cast<uint64_t>(minutes) * 60ULL * 1000ULL;

    uint64_t elapsed =
        now - m_idle_quit_start_time;

    if (elapsed >= timeout)
    {
        Comm::sendStringToAllPeers(
            "Server shutting down due to inactivity"
        );

        disarmIdleQuitTimer();
        if (main_loop)
            main_loop->requestAbort();
        return;
    }

    uint64_t remaining_ms = timeout - elapsed;

    int remaining_minutes =
        static_cast<int>(
            (remaining_ms + 59999ULL) / 60000ULL
        );

    if ((remaining_minutes == 15 ||
        (remaining_minutes >= 1 && remaining_minutes <= 5)) &&
        remaining_minutes != m_idle_quit_last_warning)
    {
        Comm::sendStringToAllPeers(
            "Server will shut down in " +
            std::to_string(remaining_minutes) +
            (remaining_minutes == 1
                ? " minute"
                : " minutes") +
            " if no racing activity"
        );

        m_idle_quit_last_warning = remaining_minutes;
    }
}

void LobbyGPManager::armIdleGPTimer()
{
    int minutes = getSettings()->getIdleGPMinutes();

    if (minutes <= 0)
    {
        m_idle_gp_armed = false;
        return;
    }

    m_idle_gp_start_time = StkTime::getMonoTimeMs();
    m_idle_gp_last_warning = 0;
    m_idle_gp_armed = true;

    Log::info(
        "LobbyGPManager",
        "Idle GP timer armed for %d minute(s).",
        minutes
    );
}

void LobbyGPManager::disarmIdleGPTimer()
{
    m_idle_gp_armed = false;
    m_idle_gp_start_time = 0;
    m_idle_gp_last_warning = 0;
}

void LobbyGPManager::checkIdleGPTimer()
{
    if (!m_idle_gp_armed)
        return;

    int minutes = getSettings()->getIdleGPMinutes();

    if (minutes <= 0)
    {
        disarmIdleGPTimer();
        return;
    }

    uint64_t now = StkTime::getMonoTimeMs();

    uint64_t timeout =
        static_cast<uint64_t>(minutes) * 60ULL * 1000ULL;

    uint64_t elapsed =
        now - m_idle_gp_start_time;

    // Timeout reached: reset the unfinished GP.
    if (elapsed >= timeout)
    {
        disarmIdleGPTimer();

        resetGrandPrix();

        // A reset counts as the end of GP activity for idlequit.
        armIdleQuitTimer();

        autoLockGP();

        Comm::sendStringToAllPeers(
            "GP was reset due to inactivity"
        );

        return;
    }

    uint64_t remaining_ms = timeout - elapsed;

    // Round up so e.g. 4:59 remaining is announced as 5 minutes.
    int remaining_minutes =
        static_cast<int>(
            (remaining_ms + 59999ULL) / 60000ULL
        );

    // Warnings at 10, 5, 4, 3, 2 and 1 minute(s).
    if ((remaining_minutes == 10 ||
         (remaining_minutes >= 1 && remaining_minutes <= 5)) &&
        remaining_minutes != m_idle_gp_last_warning)
    {
        Comm::sendStringToAllPeers(
            "GP will be reset in " +
            std::to_string(remaining_minutes) +
            (remaining_minutes == 1
                ? " minute"
                : " minutes") +
            " if the next race is not started"
        );

        m_idle_gp_last_warning = remaining_minutes;
    }
}

void LobbyGPManager::autoLockGP()
{
    if (!getSettings()->isAutoLockGP())
        return;

    getSettings()->setAllowedToStart(false);

    Comm::sendStringToAllPeers(
        getSettings()->getAllowedToStartAsString(true)
    );
}
