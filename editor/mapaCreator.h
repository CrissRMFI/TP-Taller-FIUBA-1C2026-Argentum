#ifndef MAPA_CREATOR_H
#define MAPA_CREATOR_H

#include "../common/game/mapa/mapa.h"

class MapaCreator {
    public:
        MapaCreator() {};
        Mapa crearMapaGenerico(uint16_t ancho, uint16_t alto) const {
            Mapa mapa = Mapa(ancho, alto);
            Sacerdote sacerdote1 = Sacerdote(1, Posicion{5, 5, 0});
            Sacerdote sacerdote2 = Sacerdote(2, Posicion{95, 0, 0});
            Sacerdote sacerdote3 = Sacerdote(3, Posicion{0, 95, 0});
            Sacerdote sacerdote4 = Sacerdote(4, Posicion{95, 95, 0});
            Comerciante comerciante = Comerciante(5, Posicion{40, 40, 0});
            Banquero banquero = Banquero(6, Posicion{60, 60, 0});

            sacerdote1.agregarItemCatalogo(18, 10); // Poción de vida
            sacerdote1.agregarItemCatalogo(6, 5); // Baculo misil
            sacerdote2.agregarItemCatalogo(19, 10); // Poción de mana
            sacerdote2.agregarItemCatalogo(9, 5); // Baculo curacion
            sacerdote3.agregarItemCatalogo(20, 5); // Poción de experiencia
            sacerdote3.agregarItemCatalogo(8, 5); // Baculo explosion

            comerciante.agregarItemCatalogo(1, 5, 10); // Espada corta
            comerciante.agregarItemCatalogo(2, 5, 10); // Hacha
            comerciante.agregarItemCatalogo(3, 5, 10); // Martillo
            comerciante.agregarItemCatalogo(4, 5, 10); // Arco
            comerciante.agregarItemCatalogo(5, 5, 10); // Varita
            comerciante.agregarItemCatalogo(10, 5, 10); // Armadura de cuero
            comerciante.agregarItemCatalogo(13, 5, 10); // Capuscha
            comerciante.agregarItemCatalogo(17, 5, 10); // Escudo de hierro
            
            mapa.agregarNpc(sacerdote1);
            mapa.agregarNpc(sacerdote2);
            mapa.agregarNpc(sacerdote3);
            mapa.agregarNpc(sacerdote4);
            mapa.agregarNpc(comerciante);
            mapa.agregarNpc(banquero);

            mapa.agregarPared(Posicion{20, 0, 0});
            mapa.agregarPared(Posicion{20, 1, 0});
            mapa.agregarPared(Posicion{20, 2, 0});
            mapa.agregarPared(Posicion{20, 3, 0});
            mapa.agregarPared(Posicion{20, 4, 0});
            mapa.agregarPared(Posicion{20, 5, 0});
            mapa.agregarPared(Posicion{21, 5, 0});
            mapa.agregarPared(Posicion{22, 5, 0});
            mapa.agregarPared(Posicion{23, 5, 0});
            mapa.agregarPared(Posicion{24, 5, 0});
            mapa.agregarPared(Posicion{25, 5, 0});
            mapa.agregarPared(Posicion{25, 4, 0});
            mapa.agregarPared(Posicion{25, 3, 0});
            mapa.agregarPared(Posicion{25, 2, 0});
            mapa.agregarPared(Posicion{25, 1, 0});
            mapa.agregarPared(Posicion{25, 0, 0});

            mapa.agregarPared(Posicion{45, 45, 0});
            mapa.agregarPared(Posicion{45, 46, 0});
            mapa.agregarPared(Posicion{46, 45, 0});
            mapa.agregarPared(Posicion{46, 46, 0});

            // Zonas seguras (ciudades): no se puede atacar dentro de ellas.
            mapa.agregarCiudad(Ciudad{0, 38, 38, 62, 62});
            mapa.agregarCiudad(Ciudad{0, 0, 0, 10, 10});
            mapa.agregarCiudad(Ciudad{0, 90, 0, 99, 9});
            mapa.agregarCiudad(Ciudad{0, 0, 90, 9, 99});
            mapa.agregarCiudad(Ciudad{0, 90, 90, 99,99});
            return mapa;
        }
};
#endif