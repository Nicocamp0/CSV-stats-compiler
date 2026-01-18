# AFStat - Compilateur pour le langage AFSTAT

##### Pour voir la desciption du projet, détaillant la structure et les fichier de ce projet, veuillez lire le rapport.
---

## I. Compilation du projet

Pour générer l'exécutable du compilateur (nommé **analyseur**), placez-vous dans le répertoire racine du projet et exécutez :

```bash
make
```

Cette commande :
- compile tous les fichiers situés dans `Source/`,
- inclut les headers de `Include/`,
- génère les fichiers issus de **Flex** (analyseur lexical) et **Bison** (analyseur syntaxique).


## II. Utilisation du compilateur

Une fois l'exécutable **analyseur** généré, vous pouvez traduire un fichier AFSTAT ou explorer les informations internes produites par le compilateur.

### 1.  Générer le code C/C++

Pour traduire un fichier `.afs` en code C (par exemple : `out.c`) :

```bash
./analyseur -o out.c < Exemples/fichier.afs
```

### 2. Afficher la Table des Symboles

Pour afficher les identifiants, les sources, les données associées ou les schémas détectés :

```bash
./analyseur -tos < Exemples/fichier.afs
```

### 3. Informations sur le projet

Pour afficher la version du compilateur et les membres de l’équipe :

```bash
./analyseur -version
```

## III. Nettoyage

Pour supprimer les fichiers compilés et l’exécutable :

```bash
make clean
```

---

## 4. Auteurs

Projet réalisé dans le cadre du module de compilation (M1).

---
