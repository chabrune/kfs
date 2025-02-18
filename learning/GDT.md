Voici une représentation visuelle d'un **Segment Descriptor** utilisé dans la **Global Descriptor Table (GDT)** :  

```
  31                      16 15               0
  ---------------------------------------------
  |      Base 31:24       |      Limit 19:16  |  <- Bits 63-48
  | P | DPL | S |  Type   |      Limit 15:0   |  <- Bits 47-32
  |      Base 23:16       |      Base 15:0    |  <- Bits 31-0
  ---------------------------------------------
```

### Explication des champs :
- **Base (31:24, 23:16, 15:0)** : Adresse de base du segment.
- **Limit (19:16, 15:0)** : Taille du segment (limite).
- **P (Present)** : Indique si le segment est présent en mémoire (1 = présent).
- **DPL (Descriptor Privilege Level)** : Niveau de privilège (0 = kernel, 3 = user).
- **S (Descriptor Type)** : 1 pour un segment de code/données, 0 pour un segment système.
- **Type** : Indique si c'est un segment de code ou de données et ses permissions.

### **Les Access Byte dans la GDT (Global Descriptor Table)**
L'**Access Byte** (ou octet d'accès) est un champ clé dans une **entrée de la GDT** qui définit les permissions et le type du segment. Il est situé à l’octet **5** de chaque entrée de 8 octets.

#### **1. Structure de l’Access Byte (8 bits)**  
Voici comment cet octet est structuré :

```
  7   |  6  |  5  |  4   |  3   |  2   |  1   |  0
---------------------------------------------------
   P  | DPL |  S  |  E   |  DC  |  RW  |  A  
```

| Bit | Nom | Description |
|----|----|------------|
| **7** | **P (Present)** | Indique si le segment est en mémoire (1 = présent, 0 = absent). |
| **6-5** | **DPL (Descriptor Privilege Level)** | Niveau de privilège du segment (0 = kernel, 3 = user). |
| **4** | **S (Descriptor Type)** | 1 = segment de code/données, 0 = segment système. |
| **3** | **E (Executable bit)** | 1 = segment de code, 0 = segment de données. |
| **2** | **DC (Direction/Conforming bit)** | Change selon le type de segment (voir explication plus bas). |
| **1** | **RW (Read/Write bit)** | Active la lecture pour le code ou l’écriture pour les données. |
| **0** | **A (Accessed bit)** | Mis à 1 par le CPU quand le segment est accédé. |

---

#### **2. Explication détaillée des bits**
- **Bit 7 (P - Present bit)**  
  - Indique si le segment est chargé en mémoire.  
  - **1** = Segment valide  
  - **0** = Segment absent (peut être chargé par un OS à la demande).

- **Bits 6-5 (DPL - Privilege Level)**  
  - Définit les niveaux de privilège :  
    - **0** : Mode noyau (Kernel)  
    - **3** : Mode utilisateur (User)  
  - Un segment de niveau 3 ne peut pas accéder à un segment de niveau 0.

- **Bit 4 (S - Descriptor Type bit)**  
  - **1** : Indique que l'entrée est un segment **de code ou de données**.  
  - **0** : Indique que l'entrée est un segment **système** (TSS, LDT, etc.).

- **Bit 3 (E - Executable bit)**  
  - **1** : Segment **de code** (exécutable).  
  - **0** : Segment **de données** (non exécutable).

- **Bit 2 (DC - Direction/Conforming bit)**
  - **Segments de données** (non exécutable) :
    - **0** : Croît vers le haut (ex: heap, variables globales).
    - **1** : Croît vers le bas (ex: stack).
  - **Segments de code** (exécutable) :
    - **0** : Seulement accessible depuis un niveau de privilège supérieur ou égal.
    - **1** : Peut être exécuté depuis un niveau de privilège inférieur.

- **Bit 1 (RW - Read/Write bit)**
  - **Pour un segment de données** :
    - **0** = Lecture seule.
    - **1** = Lecture et écriture autorisées.
  - **Pour un segment de code** :
    - **0** = Exécution seulement.
    - **1** = Exécution + Lecture autorisée.

- **Bit 0 (A - Accessed bit)**
  - Définis par le CPU lorsqu'un segment est utilisé.
  - Peut être ignoré dans la plupart des cas.

---

#### **3. Exemple d'Access Byte**
Si on veut définir un segment de **données en lecture/écriture**, présent en mémoire et accessible en mode utilisateur, on peut avoir un Access Byte comme ceci :

```
P = 1, DPL = 3, S = 1, E = 0, DC = 0, RW = 1, A = 0
```
En binaire :
```
1  1 1  1  0  0  1  0  = 0xF2
```
Cela signifie :
- Présent
- Privilege Level 3 (User mode)
- Segment de données
- Pas exécutable
- Lecture/écriture autorisée
- Non accédé

---

#### **4. Utilisation en programmation**
Dans une table GDT en C, on retrouvera souvent une struct comme ceci :

```c
typedef struct {
    uint16_t limit_low;   // 16 bits de la limite
    uint16_t base_low;    // 16 bits de l'adresse de base
    uint8_t base_middle;  // 8 bits de l'adresse de base
    uint8_t access;       // Octet d'accès
    uint8_t granularity;  // Granularité + bits de la limite
    uint8_t base_high;    // 8 bits de l'adresse de base
} __attribute__((packed)) GDTEntry;
```

On définit un segment de données user mode ainsi :
```c
GDTEntry user_data_segment = {
    .limit_low = 0xFFFF,
    .base_low = 0x0000,
    .base_middle = 0x00,
    .access = 0xF2,   // Access Byte (User Data RW)
    .granularity = 0xCF, // Granularité (4K blocks, 32-bit segment)
    .base_high = 0x00
};
```

---

### **Conclusion**
L’**Access Byte** est un élément clé qui définit le comportement et les permissions d’un segment mémoire dans la GDT. Il est essentiel pour le **mode protégé** en x86 et permet de contrôler quels segments sont accessibles et de quelle manière.
