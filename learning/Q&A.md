### 1. Le linker et le fichier boot n'ont rien à voir en fait ?
- **Oui**, le **fichier de liaison** (`linker.ld`) et le **fichier boot** (par exemple, `boot.asm`) sont deux choses distinctes.
- Le **fichier de liaison** est utilisé pour organiser les sections du noyau.
- Le **fichier boot** (ou bootloader) est un programme séparé qui charge le noyau en mémoire.

### 2. Le linker est présent uniquement pour le noyau ?
- **Oui**, le **fichier de liaison** est utilisé uniquement pour organiser les sections du noyau.
- Il n'a aucun rôle dans le fonctionnement du bootloader.

### 3. Le fichier boot est appelé par le BIOS juste pour charger le kernel ?
- **Oui**, le **bootloader** (comme GRUB) est chargé par le **BIOS** ou l'**UEFI**.
- Son rôle est de charger le **noyau** en mémoire et de lui transférer le contrôle.

### 4. Les sections du linker sont pour le kernel en lui-même ?
- **Oui**, les sections définies dans le **fichier de liaison** (`.text`, `.data`, `.bss`, etc.) sont pour le **noyau**.
- Ces sections sont similaires à celles d'un programme classique, mais elles sont organisées pour le noyau.

### 5. La section ENTRY(_start) doit appeler notre kernel, pas notre boot ?
- **Oui**, la directive `ENTRY(_start)` dans le **fichier de liaison** spécifie le point d'entrée du **noyau**.
- Le **bootloader** charge le noyau en mémoire et saute à l'adresse de `_start` pour commencer son exécution.

### 6. Le linker start dans le boot et le boot ensuite call le kernel ?
- **Non**, le **fichier de liaison** est utilisé uniquement pour organiser les sections du **noyau**.
- Le **bootloader** est un programme séparé qui charge le noyau en mémoire et lui transfère le contrôle.

---
