#ifndef CONFIG_COMPLETA_H
#define CONFIG_COMPLETA_H

#include "../../../common/persistencia/catalogo_criaturas.h"
#include "../objeto/catalogo_items.h"
#include "../objeto/hechizo.h"
#include "config_juego.h"

struct ConfigCompleta {
    ConfigJuego juego;
    CatalogoItems items;
    CatalogoHechizos hechizos;
    CatalogoCriaturas criaturas;
};

#endif
