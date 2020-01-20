# UXP1A


Żeby zainstalować flexa i bison na ubnuntu:

sudo apt install flex bison
bison v 3.0.4
flex 2.6.4

Uwaga! Jeżeli nie dziala kompilacja (zwlaszcza po dodaniu nowych plikow), 
pojawiaja sie bledy undefined to często pomaga-> prawym na folder nadrzedny (reload cmake project)

Co do repo: nie commitujemy na mastera. 
Proponuje zeby kazdy stworzyl sobie swojego brancha. Commitujemy zmiany na swoim branchu.
Kiedy chcemy wypchac zmiany na mastera tworzymy pullrequest na githubie. Dopiero kiedy reszta zespolo
zapozna sie ze zmianami (przynajmniej 1 osoba) klikami "rebase and merge pull request"

czy maina mozna dodac w innym pliku niz plik parsera


Proponuje do debugowania uzywac logera zamiast printf, mozna potem w latwy sposob przekeirowac 
logi do pliku.
Api jest wytlumaczone w readme.md na https://github.com/rxi/log.c
