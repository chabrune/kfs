section .multiboot
align 4
dd 0x1BADB002  ; magic number 
dd 0x01 | 0x02
dd - (0x1BADB002 + (0x01 | 0x02)) ; checksum

KbdControllerDataPort               equ 0x60  ; Port de données du contrôleur PS/2
                                                 ; - Lecture : Données provenant du clavier/souris
                                                 ; - Écriture : Données envoyées au clavier/souris

KbdControllerCommandPort            equ 0x64  ; Port de commande du contrôleur PS/2
                                                 ; - Lecture : Lit le registre d'état du contrôleur
                                                 ; - Écriture : Envoie une commande au contrôleur

KbdControllerDisableKeyboard        equ 0xAD  ; Commande pour désactiver le clavier PS/2

KbdControllerEnableKeyboard         equ 0xAE  ; Commande pour réactiver le clavier PS/2

KbdControllerReadCtrlOutputPort     equ 0xD0  ; Demande au contrôleur PS/2 de lire son port de sortie

KbdControllerWriteCtrlOutputPort    equ 0xD1  ; Demande au contrôleur PS/2 d'écrire dans son port de sortie


extern kmain

global _start

section .bss
   align 16
   stack_bottom:
      resb 4096
   stack_top:

section .text
_start:
   call EnableA20
   mov esp, stack_top
   call kmain
   hang:
      cli   ; Désactive les interruptions / Protected mode
      hlt   ; Met le processeur en mode veille
      jmp hang

EnableA20:
   ; Désactiver le clavier
   call A20WaitInput
   mov al, KbdControllerDisableKeyboard
   out KbdControllerCommandPort, al

   ; Lire le port de sortie du contrôleur
   call A20WaitInput
   mov al, KbdControllerReadCtrlOutputPort
   out KbdControllerCommandPort, al

   call A20WaitOuput
   in al, KbdControllerDataPort
   push eax

   ; Écrire dans le port de sortie du contrôleur
   call A20WaitInput
   mov al, KbdControllerWriteCtrlOutputPort
   out KbdControllerCommandPort, al

   call A20WaitInput
   pop eax
   or al, 2  ; Activer la ligne A20 (bit 2)
   out KbdControllerDataPort, al

   ; Réactiver le clavier
   call A20WaitInput
   mov al, KbdControllerEnableKeyboard
   out KbdControllerCommandPort, al

   ret

A20WaitInput:
   ; Attendre que le tampon d'entrée soit vide
   in al, KbdControllerCommandPort
   test al, 2
   jnz A20WaitInput
   ret

A20WaitOuput:
   ; Attendre que le tampon de sortie contienne une donnée
   in al, KbdControllerCommandPort
   test al, 1
   jz A20WaitOuput
   ret
