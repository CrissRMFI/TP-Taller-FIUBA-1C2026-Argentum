//
// Created by victoria zubieta on 12/06/2026.
//

#include "gestor_audio_menu.h"

GestorAudioMenu::GestorAudioMenu(QObject* parent)
    : QObject(parent) {
    player = new QMediaPlayer(this);
    output = new QAudioOutput(this);
    player->setAudioOutput(output);
    output->setVolume(0.5f);
    player->setLoops(QMediaPlayer::Infinite);
}

void GestorAudioMenu::configurarMusicaPrincipal(const QUrl& source) {
    musicaPrincipal = source;
}

void GestorAudioMenu::registrarEfecto(const QString& nombre, const QUrl& source) {
    efectos.insert(nombre, source);
}

void GestorAudioMenu::setVolumenMusica(float volumen) {
    output->setVolume(volumen);
}

void GestorAudioMenu::setVolumenEfectos(float volumen) {
    volumenEfectos = volumen;
    for (QSoundEffect* efecto : cacheEfectos) {
        efecto->setVolume(volumenEfectos);
    }
}

void GestorAudioMenu::reproducirMusicaPrincipal() {
    if (!musicaPrincipal.isValid()) {
        return;
    }

    player->setSource(musicaPrincipal);
    player->play();
}

void GestorAudioMenu::detenerMusica() {
    player->stop();
}

void GestorAudioMenu::reproducirEfecto(const QString& nombre) {
    const auto it = efectos.find(nombre);
    if (it == efectos.end() || !it.value().isValid()) {
        return;
    }

    QSoundEffect* efecto = cacheEfectos.value(nombre, nullptr);
    if (!efecto) {
        efecto = new QSoundEffect(this);
        efecto->setSource(it.value());
        efecto->setVolume(volumenEfectos);
        cacheEfectos.insert(nombre, efecto);
    }

    if (efecto->isPlaying()) {
        efecto->stop();
    }

    efecto->play();
}
