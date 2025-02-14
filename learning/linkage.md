Le **fichier de liaison** (ou **linker script**) est un élément crucial dans la création d'un noyau ou de tout programme bas niveau. Il indique à l'éditeur de liens (**linker**) comment organiser les différentes sections du programme en mémoire. Pour un noyau, cela est particulièrement important car il détermine où le code et les données seront chargés en mémoire, ce qui a un impact direct sur le fonctionnement du système.

---

## 1. À quoi sert un fichier de liaison ?

Le fichier de liaison a plusieurs rôles :
1. **Organiser les sections** : Il spécifie où chaque section (`.text`, `.data`, `.bss`, etc.) doit être placée en mémoire.
2. **Définir le point d'entrée** : Il indique au système où commencer l'exécution du programme.
3. **Gérer les adresses mémoire** : Il définit les adresses de chargement et d'exécution du programme.
4. **Optimiser l'espace mémoire** : Il permet de regrouper les sections de manière efficace.

---

## 2. Structure d'un fichier de liaison

Un fichier de liaison est généralement structuré en plusieurs parties :
- **ENTRY** : Définit le point d'entrée du programme.
- **SECTIONS** : Définit l'organisation des sections en mémoire.
- **Adresses de chargement** : Spécifie où les sections doivent être chargées en mémoire.

---

## 3. Exemple de fichier de liaison pour un noyau

Voici un exemple de fichier de liaison (`linker.ld`) pour un noyau :

```ld
ENTRY(_start)  /* Point d'entrée du programme */

SECTIONS {
    . = 1M;  /* Charger le noyau à l'adresse 1 Mo */

    /* Section .text : Code exécutable */
    .text : {
        *(.text)  /* Inclure toutes les sections .text */
    }

    /* Section .rodata : Données en lecture seule */
    .rodata : {
        *(.rodata)  /* Inclure toutes les sections .rodata */
    }

    /* Section .data : Données initialisées */
    .data : {
        *(.data)  /* Inclure toutes les sections .data */
    }

    /* Section .bss : Données non initialisées */
    .bss : {
        *(COMMON)  /* Inclure les symboles COMMON */
        *(.bss)    /* Inclure toutes les sections .bss */
    }
}
```

---

## 4. Explication détaillée de chaque partie

### 4.1. `ENTRY(_start)`
- **Rôle** : Définit le point d'entrée du programme. C'est la première instruction qui sera exécutée.
- **Exemple** : Si votre noyau commence par une fonction `_start` en assembleur, c'est cette fonction qui sera exécutée en premier.

### 4.2. `. = 1M;`
- **Rôle** : Définit l'adresse de chargement du noyau. Ici, le noyau sera chargé à l'adresse `1 Mo` en mémoire.
- **Pourquoi 1 Mo ?** : Les adresses en dessous de `1 Mo` sont souvent réservées pour le BIOS, les périphériques, ou d'autres utilisations spécifiques.

### 4.3. Sections `.text`, `.rodata`, `.data`, `.bss`
- **`.text`** : Contient le code exécutable du noyau.
- **`.rodata`** : Contient les données en lecture seule (comme les chaînes de caractères constantes).
- **`.data`** : Contient les données initialisées (comme les variables globales initialisées).
- **`.bss`** : Contient les données non initialisées (comme les variables globales non initialisées).

### 4.4. `*(.section)`
- **Rôle** : Inclut toutes les sections correspondantes des fichiers objets.
- **Exemple** : `*(.text)` inclut toutes les sections `.text` de tous les fichiers objets.

---

## 5. Impact du fichier de liaison sur le noyau

Le fichier de liaison a un impact direct sur la manière dont le noyau est chargé et exécuté. Voici quelques points clés :

### 5.1. Adresse de chargement
- Le noyau doit être chargé à une adresse spécifique en mémoire (ici, `1 Mo`). Cette adresse doit être connue et respectée par le bootloader.
- Si l'adresse de chargement est incorrecte, le noyau ne pourra pas s'exécuter correctement.

### 5.2. Organisation des sections
- Les sections `.text`, `.rodata`, `.data`, et `.bss` doivent être organisées de manière à ce que le noyau puisse accéder à ses données et exécuter son code sans erreur.
- Par exemple, si la section `.text` est mal placée, le processeur pourrait essayer d'exécuter des données au lieu du code.

### 5.3. Taille du noyau
- Le fichier de liaison influence la taille du noyau en mémoire. Par exemple, la section `.bss` ne prend pas de place dans le fichier binaire, mais elle est réservée en mémoire.

### 5.4. Point d'entrée
- Le point d'entrée (`_start`) doit être correctement défini. Si ce n'est pas le cas, le processeur ne saura pas où commencer l'exécution du noyau.

---

## 6. Exemple pratique

### 6.1. Code assembleur (`boot.asm`)
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

### 6.2. Code C (`kernel.c`)
```c
void main() {
    char *message = "Hello, Kernel World!";
    volatile char *video_memory = (volatile char *)0xB8000;  // Mémoire vidéo en mode texte
    for (int i = 0; message[i] != '\0'; i++) {
        video_memory[i * 2] = message[i];  // Caractère
        video_memory[i * 2 + 1] = 0x07;    // Attribut (couleur)
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

### 6.4. Compilation et édition de liens
1. Assembler le fichier assembleur :
   ```bash
   nasm -f elf32 boot.asm -o boot.o
   ```
2. Compiler le fichier C :
   ```bash
   gcc -m32 -c kernel.c -o kernel.o
   ```
3. Lier les fichiers objets :
   ```bash
   ld -m elf_i386 -T linker.ld -o kernel.bin boot.o kernel.o
   ```

### 6.5. Résultat
- Le noyau est maintenant un fichier binaire (`kernel.bin`) prêt à être chargé en mémoire à l'adresse `1 Mo`.
- Le point d'entrée est `_start`, et les sections sont correctement organisées.

---

## 7. Conclusion

Le fichier de liaison est un outil puissant pour organiser la mémoire d'un noyau. Il définit où le code et les données sont chargés, ce qui est essentiel pour le bon fonctionnement du système. En comprenant comment il fonctionne, vous pouvez mieux contrôler la structure de votre noyau et éviter des erreurs courantes.

---
