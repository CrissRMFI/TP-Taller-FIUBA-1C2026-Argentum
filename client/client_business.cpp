//
// Created by vzubieta on 5/14/26.
//

#include "client_business.h"

#include <algorithm>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <unordered_map>
#include <utility>
#include <vector>
#include "../common/thread/queue.h"


void ClientBusiness::setup_commands() {

    commands["/mover"] = [this](std::istringstream& iss) {
        const auto direccion = get_remaining_input(iss);
        uint8_t dir_num = translate_str(direccion);
        ComandoJugador cmd = {Opcode::MOVER, ComandoMover{dir_num}};
        this->incoming_data.push_back(cmd);
    };
    commands["/atacar"] = [this](std::istringstream& iss){
        const auto id_victima = get_remaining_input(iss);
        uint16_t id_num = static_cast<uint16_t>(id_victima);
        ComandoJugador cmd = {Opcode::ATACAR, ComandoAtacar{id_num}};
    };

    commands["/meditar"] = [this](std::istringstream& iss) {
        this->incoming_data.push({ Opcode::MEDITAR, ComandoMeditar{} });
    };
}
std::string ClientBusiness::get_remaining_input(std::istringstream& iss) const {
    std::string line;
    while (std::getline(iss >> std::ws, line)) {
        return line;
    }
    return "";
}


/* Provisorio el metodo
* terminar de definir bien como tomar y filtrar los distintos comandos  y evitar esto
*/
uint8_t ClientBusiness::translate_str(const std::string& direccion){
    if (direccion == "arriba"){ return 1};
    if (direccion == "debauer"){ return 2; };
    if (direccion == "debut"){ return 3; };
    if (direccion == "fin"){ return 4; };
}

ClientBusiness::ClientBusiness(Queue<ComandoJugador>& incoming_data): incoming_data(incoming_data)
{
}
