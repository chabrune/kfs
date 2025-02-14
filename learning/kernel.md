## Un noyau est un programme

### 1. Définition d'un noyau
Un noyau est un logiciel qui agit comme une **couche d'abstraction** entre le matériel et les applications. Il est chargé de :
- Gérer les ressources matérielles (CPU, mémoire, disques, périphériques).
- Assurer la sécurité et l'isolation des processus.
- Fournir des services aux programmes utilisateurs (appels système).

### 2. Pourquoi est-ce un programme spécial ?
Bien qu'un noyau soit un programme, il est différent des programmes classiques pour plusieurs raisons :
- **Il s'exécute en mode privilégié** : Le noyau fonctionne en **mode noyau** (ou **mode superviseur**), ce qui lui donne un accès direct au matériel. Les programmes utilisateurs, eux, s'exécutent en **mode utilisateur** et doivent passer par le noyau pour interagir avec le matériel.
- **Il est toujours en cours d'exécution** : Le noyau est chargé en mémoire au démarrage du système et reste actif tant que l'ordinateur est allumé.
- **Il gère les autres programmes** : Le noyau est responsable du chargement, de l'exécution et de l'arrêt des programmes utilisateurs.

---

## Fonctionnalités principales d'un noyau

### 1. Gestion de la mémoire
Le noyau gère la mémoire physique et virtuelle. Il alloue et libère la mémoire pour les processus, gère la pagination, et protège la mémoire des accès non autorisés.

#### Exemple :
- Lorsqu'un programme demande de la mémoire (par exemple, avec `malloc` en C), c'est le noyau qui alloue cette mémoire.
- Le noyau utilise des tables de pages pour mapper les adresses virtuelles (utilisées par les programmes) aux adresses physiques (mémoire réelle).

---

### 2. Gestion des processus
Le noyau crée, planifie et termine les processus. Il assure également la communication entre les processus (par exemple, via des tubes ou des signaux).

#### Exemple :
- Lorsque vous exécutez un programme, le noyau crée un nouveau processus, lui alloue des ressources, et l'ajoute à la file d'attente du planificateur.
- Le noyau utilise un **planificateur** (scheduler) pour décider quel processus doit s'exécuter à un moment donné.

---

### 3. Gestion des périphériques
Le noyau communique avec les périphériques matériels (clavier, souris, disque dur, etc.) via des **pilotes** (drivers). Il fournit une interface standardisée pour que les programmes utilisateurs puissent interagir avec ces périphériques.

#### Exemple :
- Lorsque vous tapez sur un clavier, le noyau reçoit les interruptions matérielles et transmet les données aux programmes concernés (par exemple, un éditeur de texte).

---

### 4. Appels système (syscalls)
Les programmes utilisateurs ne peuvent pas accéder directement au matériel. Ils doivent passer par des **appels système** pour demander des services au noyau.

#### Exemple :
- En C, la fonction `printf` utilise l'appel système `write` pour afficher du texte à l'écran.
- L'appel système `fork` permet de créer un nouveau processus.

---

## Structure d'un noyau

Un noyau est généralement composé de plusieurs modules ou couches :

### 1. Le cœur du noyau (kernel core)
- Gère les fonctionnalités de base : mémoire, processus, interruptions.
- Contient le **planificateur** et le **gestionnaire de mémoire**.

### 2. Les pilotes de périphériques (drivers)
- Permettent au noyau de communiquer avec le matériel.
- Exemples : pilotes de disque dur, de réseau, de carte graphique.

### 3. Les systèmes de fichiers
- Gèrent le stockage et l'accès aux fichiers sur les disques.
- Exemples : ext4, NTFS, FAT32.

### 4. Les modules supplémentaires
- Certains noyaux (comme Linux) supportent des modules chargés dynamiquement pour ajouter des fonctionnalités sans recompiler le noyau.

---

## Comment un noyau est-il chargé et exécuté ?

### 1. Au démarrage de l'ordinateur
1. Le **BIOS** ou **UEFI** initialise le matériel et charge le **bootloader** (par exemple, GRUB).
2. Le **bootloader** charge le noyau en mémoire.
3. Le noyau prend le contrôle et commence son initialisation.

### 2. Initialisation du noyau
- Le noyau configure le matériel (mémoire, processeur, périphériques).
- Il initialise les structures de données internes (tables de pages, files d'attente de processus).
- Il démarre le premier processus (généralement `init` ou `systemd` sous Linux).

### 3. Exécution du noyau
- Le noyau reste en mémoire et gère les demandes des programmes utilisateurs via les appels système.
- Il gère les interruptions matérielles et les erreurs (par exemple, les **Page Faults**).

---

## Exemple de code minimal pour un noyau

Voici un exemple très simple de ce à quoi pourrait ressembler un noyau minimal en assembleur et en C :

### Fichier `boot.asm` (assembleur)
```asm
section .multiboot
    dd 0x1BADB002  ; Magic number pour Multiboot
    dd 0x00        ; Flags
    dd - (0x1BADB002 + 0x00)  ; Checksum

section .text
global _start
_start:
    call main  ; Appeler la fonction principale en C
    hlt        ; Arrêter le processeur
```

### Fichier `kernel.c` (C)
```c
void main() {
    // Afficher un message à l'écran (exemple simplifié)
    char *message = "Hello, Kernel World!";
    volatile char *video_memory = (volatile char *)0xB8000;  // Mémoire vidéo en mode texte
    for (int i = 0; message[i] != '\0'; i++) {
        video_memory[i * 2] = message[i];  // Caractère
        video_memory[i * 2 + 1] = 0x07;    // Attribut (couleur)
    }
}
```

### Fichier `linker.ld` (linker script)
```ld
ENTRY(_start)

SECTIONS {
    . = 0x100000;  /* Charger le noyau à 1 Mo */

    .text : { *(.text) }
    .rodata : { *(.rodata) }
    .data : { *(.data) }
    .bss : { *(.bss) }
}
```