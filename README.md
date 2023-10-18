# Directed ring network
Ce projet consiste en la mise en place d'un réseau sous forme d'un anneau orienté (graphe dirigé) 
et le calcul (de manière répartie) de la taille de cet anneau.  
Le projet utilise les concepts de programmation socket TCP/UDP, sans utiliser de multi-thread ni de multiplexage.
## Compilation

Pour compiler le projet, suivez ces étapes :

1. Assurez-vous d'avoir CMake installé.
2. Clonez le dépôt GitHub sur votre ordinateur et naviguez vers le répertoire du projet :
```bash
git clone https://github.com/krxdow/HAI721I-Programmation-repartie.git
cd HAI721I-Programmation-reparti
 ```
3. Configurez le projet avec CMake :
```
cmake .
```
4. Compilez le projet :
 ```
cmake --build .
 ```

## Utilisation
Respectivement les binaries se trouve dans les dossiers Mainserver et Pi

### Premièrement

Pour utiliser le programme, exécutez la commande suivante :

```bash
./pconfig -n 3
```
L'option -n permet de spécifier le nombre de processus attendus (dans cet exemple, "3" correspond au nombre de processus attendus).

### Deuxièmement

Ensuite, lancez un processus en exécutant la commande suivante :
```bash
./p
```
Exécutez cette commande autant de fois que nécessaire pour correspondre aux spécifications de pconfig.

### TODO
In Pi/p.c  

- [ ] add help option to pass arguments function  
- [ ] add a system of communication  
