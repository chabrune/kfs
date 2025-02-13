La **ROM** (Read-Only Memory) et le **BIOS** (Basic Input/Output System) sont des éléments fondamentaux du démarrage d'un ordinateur. Ils jouent un rôle crucial dans l'initialisation du matériel et le chargement du système d'exploitation. Voici une explication détaillée de ce qu'est la ROM, comment elle fonctionne, et comment elle interagit avec le BIOS.

---

## Qu'est-ce que la ROM ?

La **ROM** (Read-Only Memory) est un type de mémoire non volatile, ce qui signifie qu'elle conserve ses données même lorsque l'ordinateur est éteint. Contrairement à la RAM (Random Access Memory), la ROM ne peut pas être modifiée facilement (d'où son nom "mémoire morte").

### Caractéristiques de la ROM :
- **Non volatile** : Les données sont conservées même sans alimentation électrique.
- **Lecture seule** : Les données sont écrites une fois (généralement lors de la fabrication) et ne peuvent pas être modifiées par l'utilisateur.
- **Utilisation** : Stockage de firmware (comme le BIOS), de programmes de démarrage, ou de données critiques.

### Types de ROM :
1. **ROM classique** : Les données sont écrites une fois pour toutes lors de la fabrication.
2. **PROM** (Programmable ROM) : Peut être programmée une fois par l'utilisateur.
3. **EPROM** (Erasable Programmable ROM) : Peut être effacée (par exposition aux UV) et reprogrammée.
4. **EEPROM** (Electrically Erasable Programmable ROM) : Peut être effacée et reprogrammée électriquement.
5. **Flash Memory** : Une forme moderne d'EEPROM, utilisée dans les clés USB, les cartes SD, et le firmware des ordinateurs.

---

## Qu'est-ce que le BIOS ?

Le **BIOS** (Basic Input/Output System) est un firmware stocké dans une puce ROM (ou EEPROM/Flash) sur la carte mère. Il est le premier logiciel à s'exécuter lorsque l'ordinateur est allumé.

### Rôles du BIOS :
1. **Initialisation du matériel** : Le BIOS vérifie et configure les composants matériels (CPU, RAM, disques, etc.).
2. **POST** (Power-On Self-Test) : Le BIOS effectue un autotest pour s'assurer que le matériel fonctionne correctement.
3. **Chargement du système d'exploitation** : Le BIOS recherche un système d'exploitation sur les périphériques de stockage (disque dur, clé USB, etc.) et le charge en mémoire.
4. **Fourniture de services bas niveau** : Le BIOS fournit des services pour interagir avec le matériel (par exemple, lire/écrire sur le disque dur).

---

## Comment la ROM et le BIOS fonctionnent ensemble ?

### 1. Stockage du BIOS dans la ROM
Le BIOS est stocké dans une puce ROM (ou EEPROM/Flash) sur la carte mère. Cette puce est accessible dès que l'ordinateur est alimenté, même avant que la RAM ne soit initialisée.

### 2. Démarrage de l'ordinateur
Lorsque vous allumez l'ordinateur, voici ce qui se passe :
1. **Alimentation** : Le courant est fourni à la carte mère.
2. **Exécution du BIOS** : Le processeur commence à exécuter le code du BIOS à partir de la ROM.
3. **POST** : Le BIOS effectue un autotest pour vérifier le matériel.
4. **Initialisation du matériel** : Le BIOS configure les périphériques (clavier, écran, disques, etc.).
5. **Recherche du système d'exploitation** : Le BIOS cherche un secteur de démarrage (boot sector) sur les périphériques de stockage.
6. **Chargement du système d'exploitation** : Le BIOS charge le secteur de démarrage en mémoire et transfère le contrôle au système d'exploitation.

### 3. Interaction avec la ROM
Le BIOS utilise la ROM pour :
- Stocker son code et ses données.
- Conserver les paramètres de configuration (comme l'ordre de démarrage ou les paramètres matériels) dans une zone appelée **CMOS** (Complementary Metal-Oxide-Semiconductor), qui est alimentée par une pile (bouton) sur la carte mère.

---

## Exemple de fonctionnement détaillé

### 1. Démarrage (Power-On)
- Lorsque l'ordinateur est allumé, le processeur commence à exécuter du code à partir de l'adresse mémoire `0xFFFF0` (adresse standard du BIOS en mode réel).
- Ce code est situé dans la ROM et fait partie du BIOS.

### 2. POST (Power-On Self-Test)
- Le BIOS vérifie les composants matériels (CPU, RAM, disques, etc.).
- Si une erreur est détectée, le BIOS émet un signal sonore (beep code) ou affiche un message d'erreur.

### 3. Initialisation du matériel
- Le BIOS configure les périphériques matériels (clavier, écran, disques, etc.).
- Il initialise également les interruptions matérielles (IRQ) et les canaux DMA (Direct Memory Access).

### 4. Recherche du système d'exploitation
- Le BIOS cherche un secteur de démarrage valide (boot sector) sur les périphériques de stockage (disque dur, clé USB, etc.).
- Un secteur de démarrage valide contient une signature `0xAA55` à la fin des 512 premiers octets.

### 5. Chargement du système d'exploitation
- Le BIOS charge le secteur de démarrage en mémoire à l'adresse `0x7C00`.
- Le processeur commence à exécuter le code du secteur de démarrage, qui charge ensuite le système d'exploitation.

---

## Évolution : UEFI vs BIOS

Le BIOS traditionnel est progressivement remplacé par l'**UEFI** (Unified Extensible Firmware Interface), qui offre des fonctionnalités modernes :
- **Meilleure prise en charge du matériel** : UEFI supporte des disques durs de plus de 2 To et le mode 64 bits.
- **Interface graphique** : UEFI propose souvent une interface utilisateur plus conviviale.
- **Sécurité** : UEFI inclut des fonctionnalités comme Secure Boot pour empêcher le chargement de logiciels malveillants.

---

## Exemple de code BIOS (simplifié)

Le code du BIOS est généralement écrit en assembleur ou en C, mais il est propriétaire et spécifique à chaque fabricant. Voici un exemple simplifié de ce à quoi pourrait ressembler une partie du code BIOS :

### Exemple en assembleur (simplifié)
```asm
ORG 0xFFFF0  ; Adresse de départ du BIOS

start:
    jmp initialize  ; Sauter à la routine d'initialisation

initialize:
    ; Initialiser le matériel
    call init_cpu
    call init_ram
    call init_disks

    ; Effectuer le POST
    call post

    ; Chercher un secteur de démarrage
    call find_boot_device

    ; Charger le secteur de démarrage en mémoire
    mov ax, 0x07C0
    mov es, ax
    mov bx, 0x0000
    mov ah, 0x02  ; Fonction de lecture de disque
    mov al, 0x01  ; Nombre de secteurs à lire
    mov ch, 0x00  ; Cylindre
    mov cl, 0x01  ; Secteur
    mov dh, 0x00  ; Tête
    int 0x13      ; Interruption du BIOS pour lire le disque

    ; Transférer le contrôle au secteur de démarrage
    jmp 0x07C0:0x0000

init_cpu:
    ; Initialiser le CPU
    ret

init_ram:
    ; Initialiser la RAM
    ret

init_disks:
    ; Initialiser les disques
    ret

post:
    ; Effectuer le POST
    ret

find_boot_device:
    ; Chercher un périphérique de démarrage
    ret
```

---

## Conclusion

La **ROM** et le **BIOS** sont des éléments essentiels au démarrage d'un ordinateur. La ROM stocke le code du BIOS, qui est responsable de l'initialisation du matériel et du chargement du système d'exploitation. Bien que le BIOS traditionnel soit progressivement remplacé par l'UEFI, son rôle reste fondamental dans le fonctionnement d'un ordinateur.

---