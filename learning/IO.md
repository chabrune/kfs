OUT adresse,accumulateur

Paramètres

Nom 	Description

adresse :
Ce paramètre permet d'indiquer l'adresse du port d'entrée/sortie. Il peut s'agir d'une valeur entre 0 et FFFFh. Voir Référence des ports d'entrée/sortie 80x86 pour plus de détails.

accumulateur :
Ce paramètre permet d'indiquer la valeur envoyer dans le port d'entrée/sortie. Il peut s'agir d'un des 3 registres AL, AX ou EAX. Même avec les microprocesseurs 64 bits, il n'est pas possible d'envoyer une valeur plus grande que 32 bits.


Syntaxe

IN accumulateur,imm
IN accumulateur,DX

Description

Cette instruction permet de demander un octet ou un mot provenant du port d'entrée/sortie et le retourne dans le registre accumulateur (AL, AX, EAX).