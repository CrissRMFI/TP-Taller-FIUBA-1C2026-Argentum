#ifndef CONFIG_COMPLETA_H
#define CONFIG_COMPLETA_H

#include "config_juego.h"
#include "../objeto/catalogo_items.h"
#include "../objeto/hechizo.h"
#include "../../../common/persistencia/catalogo_criaturas.h"

struct ConfigCompleta {
    ConfigJuego       juego;
    CatalogoItems     items;
    CatalogoHechizos  hechizos;
    CatalogoCriaturas criaturas;
};

#endif
