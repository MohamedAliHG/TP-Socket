cmake_minimum_required(VERSION 3.29)
project(socket)

set(CMAKE_CXX_STANDARD 20)

add_executable(socket main.cpp
        "Client HTTP en Mode connecte/client_http.cpp"
        "Transfert de messages en mode connecté/client.cpp"
        "Transfert de messages en mode connecté/serveur.cpp"
        "Transfert de messages en mode non connecté/serveur.cpp"
        "Transfert de messages en mode non connecté/client.cpp"
        "Serveur en mode concurrent/serveur.cpp"
        "Serveur en mode concurrent/client.cpp"
        "Serveur en mode concurrent/serveur_multi_services.cpp"
        "Serveur en mode concurrent/client_fichier.cpp"
        "Serveur en mode concurrent/client_processus.cpp"
)
