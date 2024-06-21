/*
** EPITECH PROJECT, 2024
** Zappy-Server
** File description:
** logs successes
*/

#pragma once
#include "stdio.h"
#include "../logs_colors.h"
#include "../../client/client.h"
#include "netinet/in.h"
#include "arpa/inet.h"
#include "player/player.h"

/** @brief macro to print [SUCCESS] in green
 * directly before the rest of the log**/
#define LOG_SUCCESS(...) printf(GREEN "[SUCCESS] " RESET __VA_ARGS__)

/** @brief log a success message when a client is connected **/
extern void log_success_network_client_connected(client_t *client);

/** @brief log a sucess message when a client join a team **/
extern void log_success_add_to_team(client_t *client);

extern void log_success_init_team(const char *team_name);

//gui
extern void log_success_tna(const client_t *client);
extern void log_success_sst(const client_t *client, uint64_t t);
extern void log_success_sgt(const client_t *client);
extern void log_success_ppo(const client_t *client, const player_t *player);
