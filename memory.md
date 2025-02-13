## Mémoire : Concepts clés

La mémoire est une ressource essentielle pour tout système d'exploitation. Elle est utilisée pour stocker des données, du code, et des structures de contrôle. Voici les concepts fondamentaux à maîtriser :

---

### 1. Segmentation de la mémoire

La **segmentation** est une technique utilisée pour diviser la mémoire en segments distincts, chacun ayant une taille maximale de 64 Ko. Chaque segment est identifié par un **registre de segment** (CS, DS, SS, etc.) et un **offset** (décalage) dans ce segment.

#### Adressage segment:offset
- Une adresse mémoire est représentée par une paire `segment:offset`.
- L'adresse physique réelle est calculée comme suit :
  ```
  adresse_physique = (segment * 16) + offset
  ```
- Exemple :
  - Segment `0x1000`, offset `0x2000` → Adresse physique `0x12000`.

#### Registres de segment
- **CS** : Code Segment (segment de code en cours d'exécution).
- **DS** : Data Segment (segment de données).
- **SS** : Stack Segment (segment de pile).
- **ES**, **FS**, **GS** : Segments supplémentaires pour les données.

#### Exemple en assembleur :
```asm
mov ax, 0x1000  ; Charge 0x1000 dans AX
mov ds, ax      ; Définit DS pour pointer vers le segment 0x1000
mov bx, 0x2000  ; Charge l'offset 0x2000 dans BX
mov al, [bx]    ; Charge le byte à l'adresse 0x12000 (DS:BX) dans AL
```

---

### 2. Pagination

La **pagination** est une technique de gestion de la mémoire qui permet de diviser la mémoire en blocs de taille fixe appelés **pages** (généralement 4 Ko). Elle est utilisée pour implémenter la **mémoire virtuelle**, où chaque processus a l'illusion d'avoir un espace d'adressage complet.

#### Concepts clés :
- **Page** : Bloc de mémoire de taille fixe (par exemple, 4 Ko).
- **Page Table** : Table qui mappe les adresses virtuelles aux adresses physiques.
- **Page Directory** : Table de niveau supérieur qui pointe vers les tables de pages.

#### Registres de pagination :
- **CR3** : Contient l'adresse physique du **Page Directory**.
- **CR0** : Active la pagination via le bit **PG** (Paging Enable).

#### Exemple de fonctionnement :
1. Le processeur traduit une adresse virtuelle en adresse physique en utilisant les tables de pages.
2. Si la page n'est pas en mémoire (Page Fault), le système d'exploitation la charge depuis le disque.

#### Exemple en assembleur :
```asm
mov eax, cr0
or eax, 0x80000000  ; Active la pagination (bit PG)
mov cr0, eax
```

---

### 3. Mémoire virtuelle vs mémoire physique

- **Mémoire physique** : La mémoire RAM réelle disponible sur la machine.
- **Mémoire virtuelle** : Un espace d'adressage abstrait utilisé par les processus. Chaque processus a l'illusion d'avoir un espace d'adressage complet (par exemple, 4 Go en mode 32 bits).

#### Avantages de la mémoire virtuelle :
- Isolation des processus (chaque processus a son propre espace d'adressage).
- Gestion efficace de la mémoire (swap, partage de pages).
- Protection de la mémoire (accès interdit à certaines zones).

#### Exemple :
- Un processus peut accéder à l'adresse virtuelle `0x00400000`, qui est mappée à l'adresse physique `0x100000`.

---

### 4. Registres de mémoire

Les registres de mémoire sont utilisés pour manipuler les adresses et les données en mémoire. Voici les principaux registres liés à la mémoire :

#### Registres généraux :
- **EAX**, **EBX**, **ECX**, **EDX** : Utilisés pour stocker des données ou des adresses.
- **ESI**, **EDI** : Utilisés pour les opérations sur les chaînes de caractères.
- **EBP**, **ESP** : Utilisés pour gérer la pile (stack).

#### Registres de segment :
- **CS**, **DS**, **SS**, **ES**, **FS**, **GS** : Pointent vers des segments de mémoire.

#### Registres de contrôle :
- **CR0** : Contrôle le mode protégé et la pagination.
- **CR2** : Contient l'adresse qui a causé une Page Fault.
- **CR3** : Pointe vers le Page Directory.

#### Exemple d'utilisation :
```asm
mov eax, [ebx]  ; Charge la valeur à l'adresse pointée par EBX dans EAX
mov [ecx], edx  ; Stocke la valeur de EDX à l'adresse pointée par ECX
```

---

### 5. Gestion de la pile (Stack)

La **pile** est une région de mémoire utilisée pour stocker temporairement des données, comme les adresses de retour des fonctions ou les variables locales.

#### Registres liés à la pile :
- **ESP** (Stack Pointer) : Pointe vers le sommet de la pile.
- **EBP** (Base Pointer) : Utilisé pour accéder aux variables locales dans une fonction.

#### Opérations de base :
- **PUSH** : Ajoute une valeur sur la pile.
- **POP** : Retire une valeur de la pile.

#### Exemple en assembleur :
```asm
push eax  ; Ajoute EAX sur la pile (décrémente ESP)
pop ebx   ; Retire la valeur de la pile et la place dans EBX (incrémente ESP)
```

---

### 6. Mémoire cache

Le **cache** est une mémoire rapide située entre le processeur et la mémoire RAM. Il est utilisé pour accélérer l'accès aux données fréquemment utilisées.

#### Niveaux de cache :
- **L1** : Le plus rapide, mais de petite taille.
- **L2** : Plus grand, mais légèrement plus lent.
- **L3** : Encore plus grand, partagé entre les cœurs du processeur.

#### Gestion du cache :
- Le cache est géré automatiquement par le matériel, mais certaines instructions (comme `CLFLUSH`) permettent de le manipuler manuellement.

---

### 7. Mémoire virtuelle et swap

Lorsque la mémoire physique est insuffisante, le système d'exploitation utilise une technique appelée **swap** pour déplacer des pages de mémoire vers le disque.

#### Fonctionnement :
1. Le système identifie les pages les moins utilisées.
2. Il les écrit sur le disque (fichier d'échange ou partition swap).
3. Il libère la mémoire physique pour d'autres utilisations.

#### Exemple :
- Une page située à l'adresse physique `0x2000` peut être déplacée vers le disque, et son contenu est restauré plus tard si nécessaire.

## Sections de mémoire

Lorsqu'un programme est compilé, il est divisé en plusieurs sections. Chaque section contient un type spécifique de données ou de code. Voici une description détaillée des sections les plus courantes :

---

### 1. Section `.text`

La section `.text` contient le **code exécutable** du programme. C'est ici que se trouvent les instructions en assembleur ou en langage machine qui seront exécutées par le processeur.

#### Caractéristiques :
- **Lecture seule** : Le code ne peut pas être modifié pendant l'exécution.
- **Exécutable** : Le processeur peut exécuter les instructions contenues dans cette section.
- **Taille fixe** : La taille de cette section est déterminée au moment de la compilation.

#### Exemple en assembleur :
```asm
section .text
global _start
_start:
    mov eax, 1  ; Code pour l'appel système "exit"
    int 0x80    ; Interruption pour appeler le noyau
```

---

### 2. Section `.rodata` (Read-Only Data)

La section `.rodata` contient les **données en lecture seule**. Ces données sont constantes et ne peuvent pas être modifiées pendant l'exécution du programme.

#### Caractéristiques :
- **Lecture seule** : Les données ne peuvent pas être modifiées.
- **Non exécutable** : Le processeur ne peut pas exécuter de code à partir de cette section.
- **Utilisation courante** : Chaînes de caractères constantes, constantes numériques.

#### Exemple en assembleur :
```asm
section .rodata
hello db "Hello, World!", 0  ; Chaîne de caractères constante
```

---

### 3. Section `.data`

La section `.data` contient les **données initialisées** du programme. Ces données sont modifiables et ont une valeur initiale définie au moment de la compilation.

#### Caractéristiques :
- **Lecture/écriture** : Les données peuvent être modifiées pendant l'exécution.
- **Non exécutable** : Le processeur ne peut pas exécuter de code à partir de cette section.
- **Utilisation courante** : Variables globales initialisées.

#### Exemple en assembleur :
```asm
section .data
counter dd 0  ; Variable globale initialisée à 0
```

---

### 4. Section `.bss` (Block Started by Symbol)

La section `.bss` contient les **données non initialisées** du programme. Ces données sont allouées en mémoire mais n'ont pas de valeur initiale définie.

#### Caractéristiques :
- **Lecture/écriture** : Les données peuvent être modifiées pendant l'exécution.
- **Non exécutable** : Le processeur ne peut pas exécuter de code à partir de cette section.
- **Taille dynamique** : La taille de cette section est déterminée au moment de l'exécution.
- **Utilisation courante** : Variables globales non initialisées, buffers.

#### Exemple en assembleur :
```asm
section .bss
buffer resb 1024  ; Réserve 1024 octets non initialisés
```

---

### 5. Section `.heap`

La section `.heap` est utilisée pour l'allocation dynamique de mémoire. Elle est gérée par des fonctions comme `malloc` et `free` en C.

#### Caractéristiques :
- **Lecture/écriture** : Les données peuvent être modifiées pendant l'exécution.
- **Non exécutable** : Le processeur ne peut pas exécuter de code à partir de cette section.
- **Taille dynamique** : La taille du heap peut augmenter ou diminuer pendant l'exécution.

#### Exemple en C :
```c
int *ptr = (int *)malloc(100 * sizeof(int));  // Alloue 100 entiers sur le heap
free(ptr);  // Libère la mémoire allouée
```

---

### 6. Section `.stack`

La section `.stack` est utilisée pour stocker les **variables locales** et les **adresses de retour** des fonctions. La pile fonctionne selon le principe **LIFO** (Last In, First Out).

#### Caractéristiques :
- **Lecture/écriture** : Les données peuvent être modifiées pendant l'exécution.
- **Non exécutable** : Le processeur ne peut pas exécuter de code à partir de cette section.
- **Taille dynamique** : La pile grandit et rétrécit au fur et à mesure que les fonctions sont appelées et retournent.

#### Exemple en assembleur :
```asm
section .text
global _start
_start:
    push ebp       ; Sauvegarde l'ancien base pointer
    mov ebp, esp   ; Définit le nouveau base pointer
    sub esp, 4     ; Réserve 4 octets pour une variable locale
    mov [ebp-4], 42; Stocke la valeur 42 dans la variable locale
    add esp, 4     ; Libère l'espace réservé
    pop ebp        ; Restaure l'ancien base pointer
    ret            ; Retourne de la fonction
```

---

### 7. Autres sections

Il existe d'autres sections moins courantes mais tout aussi importantes :

#### `.ctors` et `.dtors`
- **`.ctors`** : Contient les constructeurs globaux (fonctions appelées avant `main`).
- **`.dtors`** : Contient les destructeurs globaux (fonctions appelées après `main`).

#### `.plt` (Procedure Linkage Table) et `.got` (Global Offset Table)
- **`.plt`** : Utilisé pour les appels de fonctions dynamiques (liées au moment de l'exécution).
- **`.got`** : Contient les adresses des variables globales et des fonctions.

#### `.debug`
- Contient des informations de débogage (symboles, numéros de ligne, etc.).

---

## Organisation des sections en mémoire

Lorsqu'un programme est chargé en mémoire, les sections sont organisées de la manière suivante :

1. **`.text`** : Code exécutable.
2. **`.rodata`** : Données en lecture seule.
3. **`.data`** : Données initialisées.
4. **`.bss`** : Données non initialisées.
5. **`.heap`** : Mémoire allouée dynamiquement.
6. **`.stack`** : Pile d'exécution.

---

## Exemple de fichier de liaison (Linker Script)

Un fichier de liaison (`linker.ld`) spécifie comment les sections doivent être organisées en mémoire. Voici un exemple simple :

```ld
ENTRY(_start)  /* Point d'entrée du programme */

SECTIONS {
    . = 0x100000;  /* Adresse de départ */

    .text : { *(.text) }       /* Section .text */
    .rodata : { *(.rodata) }   /* Section .rodata */
    .data : { *(.data) }       /* Section .data */
    .bss : { *(.bss) }         /* Section .bss */
}
```