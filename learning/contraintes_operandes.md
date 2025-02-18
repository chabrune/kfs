### **Les contraintes d'opérandes (`constraint modifiers`) en inline assembly GCC**
Les contraintes définissent **comment** le compilateur GCC va **passer les valeurs** entre le C et l'assembleur. Elles lui permettent d'affecter des registres spécifiques ou d'optimiser le code.  

👉 **Elles sont utilisées dans l'inline assembly comme ceci :**
```c
__asm__ ("instruction" : output_operands : input_operands : clobbers);
```
Exemple simple :
```c
int a = 42, b;
__asm__("movl %1, %0" : "=r"(b) : "r"(a));  
// Déplace 'a' dans 'b', en passant par un registre
```

---

## **📌 Les contraintes les plus utilisées**
### **1. Contraintes de registre (`r`, `a`, `b`, etc.)**
| Contrainte | Signification |
|------------|--------------|
| `"r"` | N'importe quel registre général (`EAX`, `EBX`, `ECX`, `EDX`, etc.) |
| `"a"` | Spécifiquement `EAX` |
| `"b"` | Spécifiquement `EBX` |
| `"c"` | Spécifiquement `ECX` |
| `"d"` | Spécifiquement `EDX` |
| `"S"` | Spécifiquement `ESI` |
| `"D"` | Spécifiquement `EDI` |

**Exemple :**  
```c
int value = 5;
__asm__("movl %1, %0" : "=r"(value) : "r"(10));  
// value = 10 (passé dans un registre arbitraire)
```

---

### **2. Contraintes mémoire (`m`, `o`, `V`)**
| Contrainte | Signification |
|------------|--------------|
| `"m"` | N'importe quelle adresse mémoire |
| `"o"` | Adresse mémoire avec un déplacement (ex: `[EBX+4]`) |
| `"V"` | Adresse mémoire sans déplacement (ex: `[EBX]` uniquement) |

**Exemple :**
```c
int var = 10;
__asm__("movl %1, %0" : "=m"(var) : "r"(5));  
// Stocke 5 directement dans var
```

---

### **3. Contraintes immédiates (`i`, `N`, `n`, `I`)**
| Contrainte | Signification |
|------------|--------------|
| `"i"` | Une constante immédiate (ex: `10`, `0x3D4`) |
| `"N"` | Un entier immédiat optimisé (ex: 0-255) |
| `"n"` | Une valeur immédiate simple (ex: un nombre sans calcul) |
| `"I"` | Une constante immédiate spécifique à l'instruction |

**Exemple :**
```c
__asm__("movl %1, %0" : "=r"(val) : "i"(100));  
// '100' est inséré directement dans l'instruction
```

---

### **4. Contraintes spécifiques aux instructions (`g`, `X`, `p`)**
| Contrainte | Signification |
|------------|--------------|
| `"g"` | Soit un registre, soit une mémoire (`r` ou `m`) |
| `"X"` | Un opérande quelconque (y compris des flottants) |
| `"p"` | Une adresse mémoire utilisable comme pointeur |

---

### **5. Contraintes de modification (`=`, `+`)**
| Contrainte | Signification |
|------------|--------------|
| `"="` | Indique une **écriture seule** sur un opérande |
| `"+"` | Indique une **lecture et écriture** sur un opérande |

**Exemple :**
```c
int a = 5;
__asm__("incl %0" : "+r"(a));  
// a est incrémenté et mis à jour
```

---

### **6. Clobbers (Registres modifiés non listés)**
Après les `input_operands`, on peut indiquer les **registres modifiés** (`clobbered registers`) :
```c
__asm__("movl $1, %%eax" ::: "eax");  
// Informe GCC que 'eax' est modifié
```
Si un registre est utilisé **sans être déclaré dans les opérandes**, il **doit être clobberisé**.

---

## **📌 Exemple complet : une fonction `outb` optimisée**
```c
static inline void outb(uint16_t port, uint8_t val) {
    __asm__ volatile (
        "outb %1, %0" 
        :              // Pas de sortie
        : "dN"(port), "a"(val)  // Entrées : DX (port), AL (val)
        : "memory"   // Clobber memory (évite des optimisations indésirables)
    );
}
```
- `"a"(val)` → Charge `val` dans `AL`.
- `"dN"(port)` → Charge `port` dans `DX` **ou** le met en immédiat si possible.
- `"memory"` → GCC sait qu'un accès mémoire peut être affecté.

---

## **📌 Conclusion**
Les **contraintes** permettent au compilateur de choisir **comment passer les valeurs** dans les registres ou la mémoire. Elles sont essentielles pour :
1. **Optimiser le code** en évitant des mouvements de registre inutiles.
2. **S'assurer que les bons registres sont utilisés** pour certaines instructions (`out`, `in`, `mul`, etc.).
3. **Informer GCC des effets de bord**, comme les modifications de mémoire.