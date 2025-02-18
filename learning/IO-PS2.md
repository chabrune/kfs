# I/O Instructions

## OUT 

`out port, register`

- Send data from `register` to `port`
- Register must be one of al, ax or eax
- Port is either a constant or dx

port :
Ce paramètre permet d'indiquer l'adresse du port d'entrée/sortie. Il peut s'agir d'une valeur entre 0 et FFFFh.

register :
Ce paramètre permet d'indiquer la valeur envoyer dans le port d'entrée/sortie. Il peut s'agir d'un des 3 registres AL, AX ou EAX.


## IN

`in register, port`

- Receive data from port to register
- Register must be one of al, ax or eax
- Port is either a constant or dx

Cette instruction permet de demander un octet ou un mot provenant du port d'entrée/sortie et le retourne dans le registre accumulateur (AL, AX, EAX).


### **Le Contrôleur PS/2 : Explication Complète**

Le **contrôleur PS/2** est un contrôleur de périphériques utilisé pour gérer les **claviers et souris PS/2**. Il a longtemps été standard sur les PC avant que l’USB ne le remplace progressivement. Cependant, il est toujours présent sur les architectures modernes, notamment en mode compatibilité (Legacy Mode).  

---

## **Fonctionnement du contrôleur PS/2**  
Le contrôleur PS/2 est **connecté à l'IRQ 1** (pour le clavier) et **IRQ 12** (pour la souris). Il communique avec le processeur via les **ports d'entrée/sortie (I/O ports)** suivants :  

| **Port** | **Description** |
|----------|----------------|
| `0x60` | Données du clavier/souris (lecture et écriture) |
| `0x64` | Commandes et statut du contrôleur PS/2 |

Lorsque l'on tape une touche sur un clavier PS/2, le contrôleur **envoie un scan code** via le port `0x60`. Le processeur peut lire ce code pour savoir quelle touche a été pressée ou relâchée.

---

## **Les registres du contrôleur PS/2**
Le contrôleur PS/2 possède **deux registres principaux** accessibles via les ports I/O :  

### **1. Registre de données (`0x60`)**
Ce registre sert à :
- **Lire les données** envoyées par le clavier ou la souris (scan codes, réponses aux commandes).
- **Écrire des commandes** au périphérique sélectionné (clavier ou souris).

### ⚙️ **2. Registre de commande & statut (`0x64`)**
- En **lecture**, ce registre donne le statut du contrôleur PS/2 :
  - Bit `0` = 1 : Des données sont disponibles sur le port `0x60`.
  - Bit `1` = 1 : Le contrôleur est occupé.
  - Bit `2` = 1 : Données en transit pour la souris.
- En **écriture**, il permet d’envoyer des commandes au contrôleur PS/2.

---

## ** Exemples de Commandes pour le PS/2**
On envoie des commandes au contrôleur via le port `0x64`, puis on peut lire/réagir via `0x60`.

| **Commande** | **Description** |
|-------------|----------------|
| `0xAD` | Désactiver le clavier |
| `0xAE` | Activer le clavier |
| `0xD4` | Envoyer la commande suivante à la souris |
| `0xF4` | Activer la souris |
| `0xFE` | Réinitialiser le périphérique |
| `0xFF` | Auto-test du périphérique |

Pour envoyer une commande :
1. Attendre que le **bit 1** du registre de statut (`0x64`) soit **0** (le contrôleur est prêt).
2. Envoyer la commande sur **`0x64`**.
3. Si la commande requiert des données supplémentaires, les envoyer sur **`0x60`**.

---

## ** Lire un scan code du clavier** (Exemple en ASM)
Voici un exemple pour **lire une touche pressée** en **ASM** :

```assembly
wait_key:
    in al, 0x64        ; Lire le registre de statut
    test al, 1         ; Vérifier si des données sont disponibles
    jz wait_key        ; Attendre jusqu'à ce qu'une touche soit pressée
    in al, 0x60        ; Lire le scan code du clavier
    ret
```
💡 Une fois le scan code récupéré, il peut être traduit en caractère ASCII.

---

## **5️⃣ Gérer le clavier en C (Exemple)**
Si tu veux interagir avec le clavier dans ton kernel, voici un **exemple en C** :

```c
#include <stdint.h>

#define PORT_DATA  0x60
#define PORT_STATUS 0x64

// Fonction pour lire un scan code depuis le clavier
uint8_t keyboard_read_scan_code() {
    while (!(inb(PORT_STATUS) & 1)); // Attendre que des données soient disponibles
    return inb(PORT_DATA);           // Lire le scan code
}
```
Ici, `inb()` est une fonction pour lire un port I/O (tu devras la coder avec de l'ASM).

---

## **6️⃣ Initialiser le contrôleur PS/2**
Avant d'utiliser le clavier et la souris, il est souvent nécessaire d'initialiser le contrôleur PS/2 :

```c
void ps2_init() {
    // Désactiver les périphériques
    outb(0x64, 0xAD);  // Désactiver le clavier
    outb(0x64, 0xA7);  // Désactiver la souris

    // Vider le buffer du contrôleur PS/2
    while (inb(0x64) & 1) {
        inb(0x60);
    }

    // Réactiver le clavier
    outb(0x64, 0xAE);
}
```

---

## **Pourquoi doit-on activer A20 ?**
La **ligne A20** (A20 Gate) est une ligne d'adresse utilisée dans les processeurs x86. Son activation est essentielle pour permettre l'accès à la mémoire au-delà de **1 Mo**.

### **Historique du problème**
- Les premiers PC équipés d'un processeur **Intel 8086/8088** ne pouvaient adresser que **1 Mo** de mémoire, car ces CPU avaient un bus d'adresse **de 20 bits**.
- Lorsque **le 80286** est sorti, il pouvait adresser plus de **16 Mo** grâce à un bus d'adresse de **24 bits**.
- Cependant, pour des raisons de compatibilité avec les anciens programmes **MS-DOS**, IBM a ajouté un circuit qui **désactive le 21ème bit** (A20), limitant ainsi l'accès à **1 Mo**.
- Cela posait problème aux nouveaux systèmes d'exploitation voulant utiliser plus de mémoire.
- Pour résoudre cela, il faut **activer la ligne A20** pour permettre aux adresses d'utiliser leur pleine capacité.

---

## **Comment fonctionne l'A20 Gate ?**
### **Principe du Bus d'Adresse**
En mode réel (**Real Mode**), l'adresse physique est calculée ainsi :
```
Adresse physique = (segment * 16) + offset
```
Or, en mode 16 bits, les registres permettent d'adresser **1 Mo de mémoire (20 bits)**.

Exemple :
```
Segment : 0xFFFF
Offset  : 0x0010
```
Cela donne :
```
(0xFFFF * 16) + 0x0010 = 0x100000 → 1 Mo + 1 octet !
```
Sauf que sur les anciens systèmes, **le bit 20 (A20) est bloqué à 0**, donc **l’adresse revient à 0x00000** (effet de "wrap-around" → répétition des adresses).

### **Activer A20**
Pour éviter ce problème, on **active A20**, ce qui permet d’adresser au-delà de 1 Mo sans wrap-around.

---

## **Méthodes pour activer A20**
Il existe plusieurs méthodes pour activer la ligne **A20** :

1. **Via le contrôleur PS/2 (méthode utilisée dans ton code)**  
   - Le contrôleur PS/2 gère également la ligne A20.
   - Il suffit d'envoyer des commandes au **port 0x64** pour l’activer.
   - C'est une méthode fiable mais plus lente que d'autres.

2. **Via le Port 92h**  
   - Certains chipsets modernes offrent un moyen plus rapide d’activer A20 via le **port 0x92**.
   - Il suffit d'écrire `0x02` sur ce port.

3. **Via le BIOS (INT 15h, AX=2401h)**  
   - Le BIOS offre une routine pour activer/désactiver A20.
   - Plus lente mais portable.

4. **En accédant directement au chipset**  
   - Sur certains chipsets, on peut activer A20 en manipulant des registres mémoire.

---

## **Comment fonctionne le contrôleur PS/2 ?**
Le **contrôleur PS/2** est un microcontrôleur intégré qui gère le **clavier et la souris**.

### **Ses deux ports principaux :**
| Port     | Type        | Fonction |
|----------|------------|----------|
| **0x60** | Lecture    | Lire les données du clavier/souris |
| **0x60** | Écriture   | Envoyer une commande au clavier/souris |
| **0x64** | Lecture    | Lire le statut du contrôleur |
| **0x64** | Écriture   | Envoyer une commande au contrôleur |

### **Utilisation des registres d’état (port 0x64)**
Le registre d’état contient plusieurs bits qui indiquent l’état du contrôleur :
- **Bit 0 = 1** : Une donnée est disponible pour lecture (**tampon de sortie plein**)
- **Bit 1 = 1** : Le contrôleur est en train de traiter une commande (**tampon d’entrée occupé**)
- **Bit 2 = 1** : Le clavier est verrouillé
- **Bit 5 = 1** : Une erreur de parité a été détectée

### **Commandes courantes**
| Commande | Fonction |
|----------|----------|
| **0x20** | Lire le registre de configuration |
| **0x60** | Écrire dans le registre de configuration |
| **0xAA** | Tester le contrôleur PS/2 |
| **0xAB** | Tester le premier port PS/2 |
| **0xA7** | Désactiver le second port |
| **0xA8** | Activer le second port |

---

## **Conclusion**
L’activation de **A20** est essentielle pour pouvoir adresser plus de **1 Mo de mémoire**.  
Le **contrôleur PS/2** joue un rôle clé non seulement dans la gestion du clavier/souris, mais aussi dans l’activation d’**A20** sur les anciens systèmes.  