//
// Created by vzubieta on 5/14/26.
//

#ifndef TP_TALLER_FIUBA_1C2026_ARGENTUM_CLIENT_BUSINESS_H
#define TP_TALLER_FIUBA_1C2026_ARGENTUM_CLIENT_BUSINESS_H
#include "../common/protocolo/comando_jugador.h"


class ClientBusiness {

private:
    Queue<ComandoJugador>& incoming_data;


    // hago uso de command_handler con una función
    using command_handler = std::function<void(std::istringstream&)>;
    std::map<std::string, command_handler> commands;
    void setup_commands();
    std::string get_remaining_input(std::istringstream& iss) const;
    uint8_t translate_str(const std::string& direccion);


public:
    explicit ClientBusiness( Queue<ComandoJugador>& incoming_data);
    void run();
};

#endif //TP_TALLER_FIUBA_1C2026_ARGENTUM_CLIENT_BUSINESS_H
