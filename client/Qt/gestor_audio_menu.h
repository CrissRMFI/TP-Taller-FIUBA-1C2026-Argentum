//
// Created by victoria zubieta on 12/06/2026.
//

#ifndef TALLER_TP_GESTOR_AUDIO_MENU_H
#define TALLER_TP_GESTOR_AUDIO_MENU_H

#include <QHash>
#include <QObject>
#include <QMediaPlayer>
#include <QAudioOutput>
#include <QSoundEffect>
#include <QUrl>

class GestorAudioMenu : public QObject {
    Q_OBJECT
public:
    explicit GestorAudioMenu(QObject* parent = nullptr);

    void configurarMusicaPrincipal(const QUrl& source);
    void registrarEfecto(const QString& nombre, const QUrl& source);
    void setVolumenMusica(float volumen);
    void setVolumenEfectos(float volumen);

    Q_INVOKABLE void reproducirMusicaPrincipal();
    Q_INVOKABLE void detenerMusica();
    Q_INVOKABLE void reproducirEfecto(const QString& nombre);

private:
    QMediaPlayer* player = nullptr;
    QAudioOutput* output = nullptr;
    QUrl musicaPrincipal;
    float volumenEfectos = 0.5f;
    QHash<QString, QUrl> efectos;
    QHash<QString, QSoundEffect*> cacheEfectos;

};

#endif  // TALLER_TP_GESTOR_AUDIO_MENU_H
