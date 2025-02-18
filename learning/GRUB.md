Le **Multiboot** et **GRUB** (Grand Unified Bootloader) sont des éléments essentiels pour comprendre comment un noyau est chargé et exécuté sur un système. Voici une explication détaillée de ce qu'est le Multiboot, ce que fait GRUB, et comment ils interagissent pour démarrer un noyau.

---

## 1. Qu'est-ce que le Multiboot ?

Le **Multiboot** est une spécification qui définit une interface standard entre un **bootloader** (comme GRUB) et un **noyau**. Son objectif est de simplifier le processus de démarrage en fournissant une méthode standardisée pour charger un noyau.

### Objectifs du Multiboot :
1. **Compatibilité** : Permettre à un noyau de fonctionner avec n'importe quel bootloader compatible Multiboot.
2. **Simplicité** : Fournir une interface simple et standardisée pour charger un noyau.
3. **Flexibilité** : Permettre au noyau d'accéder à des informations sur le matériel et l'environnement de démarrage.

### Spécification Multiboot :
- Le noyau doit inclure un **en-tête Multiboot** dans son fichier binaire.
- Le bootloader charge le noyau en mémoire et lui transmet des informations via une **structure Multiboot**.

---

## 2. En-tête Multiboot

L'en-tête Multiboot est une structure de données qui doit être incluse dans le noyau pour indiquer au bootloader qu'il est compatible Multiboot.

### Structure de l'en-tête Multiboot :
- **Magic number** : Une valeur spécifique (`0x1BADB002`) qui identifie le noyau comme compatible Multiboot.
- **Flags** : Des indicateurs qui spécifient les fonctionnalités requises ou les informations à fournir.
- **Checksum** : Une valeur qui vérifie l'intégrité de l'en-tête.

### Exemple en assembleur :
```asm
section .multiboot
    dd 0x1BADB002  ; Magic number
    dd 0x00        ; Flags
    dd - (0x1BADB002 + 0x00)  ; Checksum
```

---

## 3. Qu'est-ce que GRUB ?

**GRUB** (Grand Unified Bootloader) est un bootloader open-source largement utilisé pour démarrer des systèmes d'exploitation. Il supporte la spécification Multiboot, ce qui en fait un choix populaire pour charger des noyaux personnalisés.

### Fonctionnalités de GRUB :
1. **Chargement du noyau** : GRUB charge le noyau en mémoire et transfère le contrôle à celui-ci.
2. **Support Multiboot** : GRUB reconnaît les noyaux compatibles Multiboot et leur fournit des informations sur le matériel.
3. **Interface utilisateur** : GRUB propose un menu pour sélectionner le système d'exploitation à démarrer.
4. **Flexibilité** : GRUB supporte plusieurs systèmes de fichiers et formats de noyaux.

---

## 4. Comment GRUB charge un noyau Multiboot ?

Voici les étapes que GRUB suit pour charger un noyau compatible Multiboot :

### Étape 1 : Détection du noyau
- GRUB recherche un noyau sur les périphériques de stockage (disque dur, clé USB, etc.).
- Il vérifie la présence de l'en-tête Multiboot dans le fichier binaire du noyau.

### Étape 2 : Chargement en mémoire
- GRUB charge le noyau en mémoire à l'adresse spécifiée dans l'en-tête Multiboot.
- Il charge également les modules supplémentaires (par exemple, un système de fichiers initial).

### Étape 3 : Transmission des informations
- GRUB transmet des informations sur le matériel et l'environnement de démarrage au noyau via une **structure Multiboot**.
- Cette structure contient des informations comme la mémoire disponible, la carte mémoire, et les modules chargés.

### Étape 4 : Transfert du contrôle
- GRUB transfère le contrôle au point d'entrée du noyau (généralement la fonction `_start`).
- Le noyau commence son exécution.

---

## 5. Structure Multiboot

Lorsque GRUB charge un noyau compatible Multiboot, il lui transmet une structure de données contenant des informations sur le système. Voici quelques champs importants de cette structure :

- **Flags** : Indique quelles informations sont disponibles.
- **Memory** : Informations sur la mémoire disponible (mémoire basse et haute).
- **Boot device** : Périphérique à partir duquel le noyau a été chargé.
- **Command line** : Arguments de ligne de commande passés au noyau.
- **Modules** : Liste des modules chargés (par exemple, un système de fichiers initial).

### Exemple d'accès à la structure Multiboot en C :
```c
#include <multiboot.h>

void main(unsigned long magic, unsigned long addr) {
    if (magic != 0x2BADB002) {
        // Erreur : Le noyau n'a pas été chargé par un bootloader Multiboot
        return;
    }

    multiboot_info_t *mbi = (multiboot_info_t *)addr;
    if (mbi->flags & MULTIBOOT_INFO_MEMORY) {
        // Afficher la mémoire disponible
        print("Memory lower: %u KB\n", mbi->mem_lower);
        print("Memory upper: %u KB\n", mbi->mem_upper);
    }
}
```

---

## 6. Exemple complet

### 6.1. Code assembleur (`boot.asm`)
```asm
section .multiboot
    dd 0x1BADB002  ; Magic number
    dd 0x00        ; Flags
    dd - (0x1BADB002 + 0x00)  ; Checksum

section .text
global _start
_start:
    ; Passer les arguments à la fonction main
    extern main
    push ebx  ; Adresse de la structure Multiboot
    push eax  ; Magic number (0x2BADB002)
    call main
    hlt
```

### 6.2. Code C (`kernel.c`)
```c
#include <multiboot.h>

void main(unsigned long magic, unsigned long addr) {
    if (magic != 0x2BADB002) {
        // Erreur : Le noyau n'a pas été chargé par un bootloader Multiboot
        return;
    }

    multiboot_info_t *mbi = (multiboot_info_t *)addr;
    if (mbi->flags & MULTIBOOT_INFO_MEMORY) {
        // Afficher la mémoire disponible
        print("Memory lower: %u KB\n", mbi->mem_lower);
        print("Memory upper: %u KB\n", mbi->mem_upper);
    }
}
```

### 6.3. Fichier de liaison (`linker.ld`)
```ld
ENTRY(_start)

SECTIONS {
    . = 1M;  /* Charger le noyau à 1 Mo */

    .text : { *(.text) }
    .rodata : { *(.rodata) }
    .data : { *(.data) }
    .bss : { *(.bss) }
}
```

## 7. Conclusion

Le **Multiboot** est une spécification qui simplifie le chargement des noyaux en fournissant une interface standardisée entre le bootloader et le noyau. **GRUB** est un bootloader populaire qui supporte Multiboot et permet de charger des noyaux personnalisés de manière fiable et flexible.

En comprenant comment Multiboot et GRUB fonctionnent, vous pouvez créer des noyaux compatibles avec une grande variété de systèmes et de configurations matérielles.

---