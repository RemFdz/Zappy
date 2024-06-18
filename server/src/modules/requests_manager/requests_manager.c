/*
** EPITECH PROJECT, 2024
** Zappy-Server
** File description:
** Requests_manager module
*/

#include <stdlib.h>
#include <string.h>
#include "utilities.h"
#include "logs/failures/logs_failures.h"
#include "logs/successes/logs_successes.h"
#include "requests_manager.h"

/**
* @brief check if the client has a team name
* @param client the client to check
* @param manager the clients manager
* @return bool true if the client has a team name, false otherwise
**/
static bool add_to_team(client_t *client, client_manager_t *manager)
{
    client->team_name = client->current_request_to_handle;
    client->current_request_to_handle = NULL;
    if (strcmp(client->team_name, "GRAPHIC") == 0) {
        return clients_manager_add(manager, client, GUI);
    }
    if (clients_manager_add_to_team(
        manager,
        client,
        client->team_name) == false) {
        return false;
    }
    return clients_manager_add(manager, client, AI);
}

/**
* @brief freeing memory of the currently handled request
* @param args parsed args of the request
* @param client the client that sent the request
**/
static void free_request_memory(char **args, client_t *client)
{
    if (client) {
        if (client->current_request_to_handle)
            free(client->current_request_to_handle);
        client->current_request_to_handle = NULL;
    }
    free_double_tab(args);
}

static void handle_ai_request(
    char **args,
    ai_client_node_t *client,
    client_manager_t *manager,
    map_t *map
)
{
    ai_handler_data_t handler_data = {client, args, manager, map};

    for (size_t i = 0; i < AI_HANDLERS_COUNT; i++) {
        if (AI_HANDLERS[i].command_name == NULL)
            continue;
        if (strcmp(AI_HANDLERS[i].command_name, args[0]) == 0) {
            AI_HANDLERS[i].handler(&handler_data);
            return;
        }
    }
    log_failure_request_no_handler(client->client);
}

static void handle_gui_request(
    char **args,
    gui_client_node_t *client,
    client_manager_t *manager,
    map_t *map
)
{
    gui_handler_data_t handler_data = {client, args, manager, map};

    for (size_t i = 0; i < GUI_HANDLERS_COUNT; i++) {
        if (GUI_HANDLERS[i].command_name == NULL)
            continue;
        if (strcmp(GUI_HANDLERS[i].command_name, args[0]) == 0) {
            GUI_HANDLERS[i].handler(&handler_data);
            return;
        }
    }
    log_failure_request_no_handler(client->client);
}

static bool client_have_team(client_t *client, client_manager_t *manager)
{
    static uint64_t id_count = 0;

    if (client->team_name == NULL) {
        if (add_to_team(client, manager) == false) {
            log_failure_add_to_team(client, client->team_name);
            client->team_name = NULL;
            return false;
        }
        client->id = id_count;
        id_count++;
        log_success_add_to_team(client);
        return false;
    }
    return true;
}

//TODO: handle errors correctly
/**
* @brief parse the arguments of the request
 *
* @param client the client that sent the request
* @param manager the clients manager
 * @param args a pointer to a char array that will contain the parsed args
**/
static bool parse_args(client_t *client,
    client_manager_t *manager, char ***args)
{
    if (!client)
        return false;
    remove_newline(client->current_request_to_handle);
    if (!client_have_team(client, manager))
        return false;
    *args = str_array_split(client->current_request_to_handle, " ");
    if (*args == NULL || (*args)[0] == NULL)
        return false;
    return true;
}

static client_t *get_client(client_node_t *current)
{
    client_t *client = current->client;
    client_request_node_t *request = NULL;

    if (client->current_request_to_handle == NULL) {
        request = CIRCLEQ_LAST(&client->requests_queue_to_handle);
        client->current_request_to_handle = request->request;
        CIRCLEQ_REMOVE(&client->requests_queue_to_handle, request, next);
        return client;
    }
    return NULL;
}

static void handle_none_clients_requests(client_manager_t *manager)
{
    client_node_t *current = NULL;
    client_request_node_t *request = NULL;
    client_t *client = NULL;

    for (current = SLIST_FIRST(&manager->clients_list); current;
        current = SLIST_NEXT(current, next)) {
        client = current->client;
        if (NONE == client->type && !client->current_request_to_handle) {
            request = CIRCLEQ_LAST(&client->requests_queue_to_handle);
            client->current_request_to_handle = request->request;
            CIRCLEQ_REMOVE(&client->requests_queue_to_handle, request, next);
            free_request_memory(NULL, client);
        }
    }
}

void requests_manager_handle_requests(client_manager_t *manager, map_t *map)
{
    ai_client_node_t *ai_current = NULL;
    gui_client_node_t *gui_current = NULL;
    client_t *client = NULL;
    char **args = NULL;

    handle_none_clients_requests(manager);
    for (ai_current = SLIST_FIRST(&manager->ai_clients_list); ai_current;
        ai_current = SLIST_NEXT(ai_current, next)) {
        client = get_client((client_node_t *)ai_current);
        if (parse_args(client, manager, &args))
            handle_ai_request(args, ai_current, manager, map);
        free_request_memory(args, client);
    }
    SLIST_FOREACH(gui_current, &manager->gui_clients_list, next) {
        client = get_client((client_node_t *)gui_current);
        if (parse_args(client, manager, &args))
            handle_gui_request(args, gui_current, manager, map);
        free_request_memory(args, client);
    }
}
