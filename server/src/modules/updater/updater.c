/*
** EPITECH PROJECT, 2024
** Zappy-Server
** File description:
** updater.c
*/

#include <math.h>
#include <stdio.h>
#include "updater.h"

/**
 * @brief Update the map (resources) and the client (actions)
 * depending on the elapsed time and the frequency.
 *
 * @param updater The updater structure,
 * containing all the necessary data for the update.
 * @param elapsed The elapsed time since the server startup.
 */
static void update(updater_t *updater, time_t elapsed)
{
    if (elapsed >= updater->next_generation) {
        updater->map->resources.generate(updater->map);
        updater->next_generation = elapsed + updater->generation_interval;
    }
}

/**
 * @brief Updater structure destructor
 *
 * @param updater The updater to destroy
 */
static void updater_destructor(updater_t *updater)
{
    if (!updater)
        return;
    free(updater);
}

/**
 * @brief Initialize the updater structure.
 *
 * @param network The network structure.
 * @param map The map structure.
 *
 * @return The updater structure, or NULL if an allocation error occurred.
 */
updater_t *create_updater(network_t *network, map_t *map)
{
    updater_t *updater = malloc(sizeof(updater_t));
    double generation_interval;

    if (!updater)
        return NULL;
    generation_interval = 20000 / (double)network->options->freq;
    updater->generation_interval = (time_t)round(generation_interval);
    if (0 == updater->generation_interval)
        updater->generation_interval = 1;
    updater->next_generation = updater->generation_interval;
    updater->network = network;
    updater->map = map;
    updater->update = &update;
    updater->destroy = &updater_destructor;
    return updater;
}
