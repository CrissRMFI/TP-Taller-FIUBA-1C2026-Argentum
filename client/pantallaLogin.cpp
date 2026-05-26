//
// Created by victoria zubieta on 25/05/2026.
//

#include "pantallaLogin.h"

#include <iostream>

PantallaLogin::PantallaLogin(ProtocoloCliente& protocolo)
       : protocolo(protocolo) {}

bool PantallaLogin::preguntarCrear() {
    std::cout << "¿Crear personaje nuevo? (s/n): ";
    char c;
    std::cin >> c;
    std::cin.ignore();
    return (c == 's' || c == 'S');
}

std::string PantallaLogin::pedirNombre() {
    std::string nombre;
    do {
        std::cout << "Nombre (máx 32 caracteres): ";
        std::getline(std::cin, nombre);
    } while (nombre.empty() || nombre.size() > 32);
    return nombre;
}

ClasePersonaje PantallaLogin::pedirClase() {
    std::cout << "Clase: elige un numero \n"
              << "  0 - Mago\n"
              << "  1 - Clerigo\n"
              << "  2 - Paladin\n"
              << "  3 - Guerrero\n"
              << "Elegí: ";
    int opcion;
    std::cin >> opcion;
    std::cin.ignore();
    return static_cast<ClasePersonaje>(opcion % 4);
}

Raza PantallaLogin::pedirRaza() {
    std::cout << "Raza: elige un numero \n"
              << "  0 - Humano\n"
              << "  1 - Elfo\n"
              << "  2 - Enano\n"
              << "  3 - Gnomo\n"
              << "Elegí: ";
    int opcion;
    std::cin >> opcion;
    std::cin.ignore();
    return static_cast<Raza>(opcion % 4);
}

handshakeInicial PantallaLogin::ejecutar() {
    handshakeInicial h;

    h.crearPersonaje = preguntarCrear();
    h.nombre         = pedirNombre();
    if (h.crearPersonaje) {
        h.clasePersonaje = pedirClase();
        h.raza           = pedirRaza();
    }else {

        //valores por default por si no quiere crear personaje
        h.clasePersonaje = ClasePersonaje::GUERRERO;
        h.raza           = Raza::HUMANO;
    }


    protocolo.enviarUsuario(h);
    return h;
}