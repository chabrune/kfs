# Kernel From Scratch - Documentation

Ce projet a pour but de créer un système d'exploitation (OS) from scratch, en partant de la base pour bien comprendre chaque étape du processus. Nous allons explorer le fonctionnement du bootloader, la transition entre le BIOS et le noyau, ainsi que la gestion de la mémoire et des périphériques.

---

## Table des matières

1. [Introduction](#introduction)
2. [Processus de démarrage](#processus-de-démarrage)
   - [BIOS](#bios)
   - [Bootloader (GRUB)](#bootloader-grub)
   - [Chargement du noyau](#chargement-du-noyau)
3. [Virtualisation](#virtualisation)
   - [KVM / QEMU](#kvm--qemu)
   - [Débogage avec GDB](#débogage-avec-gdb)
4. [Structure du projet](#structure-du-projet)
   - [Fichier d'amorçage en assembleur](#fichier-damorçage-en-assembleur)
   - [Noyau en C](#noyau-en-c)
   - [Fichier de liaison (linker)](#fichier-de-liaison-linker)
5. [Concepts clés](#concepts-clés)
   - [Segmentation de la mémoire](#segmentation-de-la-mémoire)
   - [Registres du processeur](#registres-du-processeur)
   - [Mode protégé](#mode-protégé)
6. [Outils et commandes](#outils-et-commandes)
7. [Prochaines étapes](#prochaines-étapes)

---

## Introduction

Ce projet consiste à développer un noyau de système d'exploitation from scratch. L'objectif est de comprendre en profondeur comment un OS fonctionne, depuis le démarrage de la machine jusqu'à l'exécution de code utilisateur. Nous utiliserons des outils comme GRUB, QEMU, et GDB pour faciliter le développement et le débogage.

---

## Processus de démarrage

### BIOS

Le **BIOS** (Basic Input/Output System) est le premier logiciel qui s'exécute au démarrage de l'ordinateur. Il est stocké dans une mémoire morte (ROM) et a pour rôle d'initialiser le matériel et de charger le système d'exploitation.

1. **Initialisation** : Le BIOS est copié de la ROM vers la RAM et commence à exécuter du code.
2. **POST** : Le BIOS effectue un autotest au démarrage (POST) pour vérifier le matériel.
3. **Recherche du système d'exploitation** : Le BIOS cherche un OS sur les périphériques de stockage.
4. **Chargement de l'OS** : Le BIOS charge le secteur de démarrage (boot sector) en mémoire à l'adresse `0x7C00` et commence son exécution.

### Bootloader (GRUB)

Le **bootloader** est un programme qui charge le noyau du système d'exploitation en mémoire. GRUB (Grand Unified Bootloader) est un bootloader populaire qui supporte le multiboot, ce qui facilite le chargement de noyaux personnalisés.

- **Multiboot Header** : Un en-tête spécifique doit être présent dans le noyau pour que GRUB puisse le charger.
- **Signature `0xAA55`** : Le BIOS vérifie cette signature dans le premier secteur du disque pour s'assurer qu'il s'agit d'un secteur de démarrage valide.

### Chargement du noyau

Une fois le bootloader exécuté, il charge le noyau en mémoire et transfère le contrôle à celui-ci. Le noyau commence alors son initialisation.

---

## Virtualisation

### KVM / QEMU

Pour tester et déboguer notre noyau, nous utilisons **KVM** (Kernel-based Virtual Machine) avec **QEMU**. Cela permet de créer une machine virtuelle légère et performante.

- **Avantages** :
  - Intégration avec GDB pour le débogage.
  - Support de l'émulation de matériel réel.
  - Possibilité de démarrer en mode CLI avec `kvm -curses`.

### Débogage avec GDB

GDB est un outil essentiel pour déboguer le noyau. Il permet de suivre l'exécution du code, d'inspecter les registres et la mémoire, et de définir des points d'arrêt.

- **Commande utile** :
  ```bash
  qemu-system-x86_64 -kernel kernel.bin -s -S
  gdb kernel.bin
  (gdb) target remote :1234
  ```

---

## Structure du projet

### Fichier d'amorçage en assembleur

Le fichier d'amorçage (`boot.asm`) est écrit en assembleur et contient le code initial exécuté par le BIOS. Il doit inclure un en-tête multiboot et transférer le contrôle au noyau en C.

- **Exemple de structure** :
  ```asm
  section .multiboot
      dd 0x1BADB002              ; Magic number
      dd 0x00                    ; Flags
      dd - (0x1BADB002 + 0x00)   ; Checksum

  section .text
  global _start
  _start:
      call main                  ; Appeler la fonction principale en C
      hlt                        ; Arrêter le processeur
  ```

### Noyau en C

Le noyau (`kernel.c`) contient la logique principale du système. Il initialise les composants essentiels et peut afficher des informations à l'écran.

- **Exemple de code** :
  ```c
  void main() {
      // Initialisation du noyau
      print("Hello, Kernel World!");
  }
  ```

### Fichier de liaison (linker)

Le fichier de liaison (`linker.ld`) spécifie comment les sections du noyau doivent être organisées en mémoire.

- **Exemple** :
  ```ld
  ENTRY(_start)
  SECTIONS {
      . = 1M;                   // Charger le noyau à 1 Mo
      .text : { *(.text) }      // Section de code
      .rodata : { *(.rodata) }  // Données en lecture seule
      .data : { *(.data) }      // Données initialisées
      .bss : { *(.bss) }        // Données non initialisées
  }
  ```

---

## Concepts clés

### Segmentation de la mémoire

La mémoire est divisée en segments de 64 Ko. Chaque adresse est représentée par une paire `segment:offset`, où l'adresse réelle est calculée comme suit :

```
adresse_réelle = segment * 16 + offset

---

## Registres du processeur

Les registres sont des zones de mémoire ultra-rapides intégrées directement dans le processeur. Ils sont utilisés pour stocker des données temporaires, des adresses mémoire, des résultats d'opérations, et bien plus encore. Les registres sont essentiels pour l'exécution des instructions en assembleur et en bas niveau.

### Types de registres

Les registres sont divisés en plusieurs catégories, chacune ayant un rôle spécifique. Voici une description détaillée des principaux types de registres :

---

### 1. Registres généraux (General-Purpose Registers)

Ces registres sont utilisés pour stocker des données temporaires, des adresses mémoire, ou des résultats d'opérations arithmétiques et logiques. Ils sont disponibles en différentes tailles (8 bits, 16 bits, 32 bits, 64 bits).

| Registre 64 bits | Registre 32 bits | Registre 16 bits | Registre 8 bits (haut) | Registre 8 bits (bas) | Description                                                                 |
|------------------|------------------|------------------|------------------------|-----------------------|-----------------------------------------------------------------------------|
| **RAX**          | **EAX**          | **AX**           | **AH**                 | **AL**                | Accumulateur : utilisé pour les opérations arithmétiques et les retours de fonctions. |
| **RBX**          | **EBX**          | **BX**           | **BH**                 | **BL**                | Base : souvent utilisé pour stocker des pointeurs ou des données temporaires. |
| **RCX**          | **ECX**          | **CX**           | **CH**                 | **CL**                | Compteur : utilisé pour les boucles et les opérations de décalage.           |
| **RDX**          | **EDX**          | **DX**           | **DH**                 | **DL**                | Données : utilisé pour les opérations d'entrée/sortie et les multiplications. |
| **RBP**          | **EBP**          | **BP**           | -                      | -                     | Base Pointer : pointeur de base pour la pile (stack frame).                  |
| **RSP**          | **ESP**          | **SP**           | -                      | -                     | Stack Pointer : pointeur de pile (top of the stack).                         |
| **RSI**          | **ESI**          | **SI**           | -                      | -                     | Source Index : utilisé pour les opérations sur les chaînes de caractères.    |
| **RDI**          | **EDI**          | **DI**           | -                      | -                     | Destination Index : utilisé pour les opérations sur les chaînes de caractères. |
| **R8** à **R15** | **R8D** à **R15D**| **R8W** à **R15W**| **R8B** à **R15B**     | -                     | Registres supplémentaires en mode 64 bits.                                   |

#### Exemples d'utilisation :
- **RAX/EAX** : Stocke le résultat d'une addition ou d'une multiplication.
  ```asm
  mov eax, 5   ; Charge la valeur 5 dans EAX
  add eax, 10  ; Ajoute 10 à EAX (EAX = 15)
  ```
- **RCX/ECX** : Utilisé comme compteur dans une boucle.
  ```asm
  mov ecx, 10  ; Initialise ECX à 10
  loop_start:
  dec ecx      ; Décrémente ECX
  jnz loop_start ; Répète tant que ECX n'est pas zéro
  ```

---

### 2. Registres de segment (Segment Registers)

Ces registres sont utilisés pour gérer la segmentation de la mémoire, une technique utilisée pour diviser la mémoire en segments distincts. Chaque segment a une taille maximale de 64 Ko.

| Registre | Description                                                                 |
|----------|-----------------------------------------------------------------------------|
| **CS**   | Code Segment : pointe vers le segment de code en cours d'exécution.         |
| **DS**   | Data Segment : pointe vers le segment de données.                           |
| **SS**   | Stack Segment : pointe vers le segment de pile.                             |
| **ES**   | Extra Segment : segment supplémentaire pour les opérations sur les données.  |
| **FS**   | Segment supplémentaire (souvent utilisé pour des données spécifiques).      |
| **GS**   | Segment supplémentaire (souvent utilisé pour des données spécifiques).      |

#### Exemple d'utilisation :
- Charger une adresse dans un registre de segment :
  ```asm
  mov ax, 0x1000  ; Charge la valeur 0x1000 dans AX
  mov ds, ax      ; Définit DS pour pointer vers le segment 0x1000
  ```

---

### 3. Registres d'état (Status Registers)

Ces registres contiennent des informations sur l'état du processeur, comme les résultats des opérations arithmétiques ou les indicateurs de contrôle.

| Registre   | Description                                                                 |
|------------|-----------------------------------------------------------------------------|
| **RFLAGS** | Contient les indicateurs de statut (flags) en mode 64 bits.                |
| **EFLAGS** | Contient les indicateurs de statut en mode 32 bits.                        |
| **FLAGS**  | Contient les indicateurs de statut en mode 16 bits.                        |

#### Indicateurs importants dans RFLAGS/EFLAGS :
- **ZF (Zero Flag)** : Défini à 1 si le résultat d'une opération est zéro.
- **CF (Carry Flag)** : Défini à 1 si une opération génère une retenue.
- **SF (Sign Flag)** : Défini à 1 si le résultat est négatif.
- **IF (Interrupt Flag)** : Active ou désactive les interruptions matérielles.
- **TF (Trap Flag)** : Utilisé pour le débogage pas à pas.

#### Exemple d'utilisation :
- Vérifier si une opération a produit un résultat nul :
  ```asm
  cmp eax, 0  ; Compare EAX avec 0
  jz zero     ; Saute à l'étiquette "zero" si ZF = 1
  ```

---

### 4. Registres de contrôle (Control Registers)

Ces registres contrôlent le comportement du processeur, comme la gestion de la mémoire virtuelle ou le mode d'exécution.

| Registre | Description                                                                 |
|----------|-----------------------------------------------------------------------------|
| **CR0**  | Contrôle le mode protégé, la pagination, et d'autres fonctionnalités.       |
| **CR2**  | Contient l'adresse qui a causé une erreur de page (Page Fault).             |
| **CR3**  | Pointe vers la table des pages (Page Directory) en mode protégé.           |
| **CR4**  | Active des fonctionnalités avancées comme les extensions SSE.               |

#### Exemple d'utilisation :
- Activer le mode protégé :
  ```asm
  mov eax, cr0
  or eax, 0x1  ; Active le bit PE (Protection Enable)
  mov cr0, eax
  ```

---

### 5. Registres de débogage (Debug Registers)

Ces registres sont utilisés pour le débogage matériel, comme la définition de points d'arrêt.

| Registre | Description                                                                 |
|----------|-----------------------------------------------------------------------------|
| **DR0** à **DR3** | Contiennent les adresses des points d'arrêt.                               |
| **DR6**  | Contient l'état des points d'arrêt.                                         |
| **DR7**  | Contrôle l'activation des points d'arrêt.                                   |

#### Exemple d'utilisation :
- Définir un point d'arrêt à l'adresse `0x1000` :
  ```asm
  mov dr0, 0x1000  ; Définit DR0 pour pointer vers 0x1000
  mov dr7, 0x1     ; Active le point d'arrêt
  ```

---

### 6. Registres de la FPU (Floating-Point Unit)

Ces registres sont utilisés pour les opérations en virgule flottante.

| Registre | Description                                                                 |
|----------|-----------------------------------------------------------------------------|
| **ST(0)** à **ST(7)** | Registres de la pile FPU.                                                 |
| **MXCSR** | Contrôle les opérations SIMD (Single Instruction Multiple Data).           |

#### Exemple d'utilisation :
- Charger une valeur flottante dans ST(0) :
  ```asm
  fld dword [float_value]  ; Charge une valeur flottante dans ST(0)
  ```

---

### 7. Registres SIMD (Single Instruction Multiple Data)

Ces registres sont utilisés pour les opérations vectorielles (par exemple, les calculs parallèles).

| Registre | Description                                                                 |
|----------|-----------------------------------------------------------------------------|
| **XMM0** à **XMM15** | Registres 128 bits pour les opérations SIMD.                              |
| **YMM0** à **YMM15** | Registres 256 bits (extension AVX).                                       |
| **ZMM0** à **ZMM31** | Registres 512 bits (extension AVX-512).                                   |

#### Exemple d'utilisation :
- Additionner deux vecteurs de 4 flottants :
  ```asm
  addps xmm0, xmm1  ; Additionne les 4 flottants de XMM0 et XMM1
  ```

---

### Mode protégé

Le mode protégé est un mode d'exécution du processeur qui permet d'utiliser des fonctionnalités avancées comme la mémoire virtuelle et la protection des tâches.

---

## Outils et commandes

- **Assembleur** : `nasm -f elf32 boot.asm -o boot.o`
- **Compilateur C** : `gcc -m32 -c kernel.c -o kernel.o`
- **Éditeur de liens** : `ld -m elf_i386 -T linker.ld -o kernel.bin boot.o kernel.o`
- **QEMU** : `qemu-system-x86_64 -kernel kernel.bin`

---