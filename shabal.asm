; Disassembly of file: shabal.o
; Thu Aug 28 22:55:22 2014
; Mode: 64 bits
; Syntax: MASM/ML64
; Instruction set: SSE2, x64
option dotname

public asm_shabal_init
public asm_shabal
public asm_shabal_close


;_text   SEGMENT ALIGN(8) 'CODE'                         ; section number 1
_text   SEGMENT 'CODE'                         ; section number 1

shabal_inner LABEL NEAR
        push    rbp                                     ; 0000 _ 55
        mov     rbp, rdi                                ; 0001 _ 48: 89. FD
        mov     rdi, rdx                                ; 0004 _ 48: 89. D7
        mov     rax, rsp                                ; 0007 _ 48: 89. E0
        sub     rsp, 8                                  ; 000A _ 48: 83. EC, 08
        and     rsp, 0FFFFFFFFFFFFFFC0H                 ; 000E _ 48: 83. E4, C0
        sub     rsp, 256                                ; 0012 _ 48: 81. EC, 00000100
        mov     qword ptr [rsp+100H], rax               ; 0019 _ 48: 89. 84 24, 00000100
; Filling space: 7H
; Filler type: Multi-byte NOP
;       db 0FH, 1FH, 80H, 00H, 00H, 00H, 00H

ALIGN   8

Lm0     LABEL NEAR
        movdqu  xmm4, xmmword ptr [rsi]                 ; 0028 _ F3: 0F 6F. 26
        movdqu  xmm5, xmmword ptr [rsi+10H]             ; 002C _ F3: 0F 6F. 6E, 10
        movdqu  xmm6, xmmword ptr [rsi+20H]             ; 0031 _ F3: 0F 6F. 76, 20
        movdqu  xmm7, xmmword ptr [rsi+30H]             ; 0036 _ F3: 0F 6F. 7E, 30
        movdqa  xmm0, xmmword ptr [rbp+30H]             ; 003B _ 66: 0F 6F. 45, 30
        movdqa  xmm1, xmmword ptr [rbp+40H]             ; 0040 _ 66: 0F 6F. 4D, 40
        paddd   xmm0, xmm4                              ; 0045 _ 66: 0F FE. C4
        paddd   xmm1, xmm5                              ; 0049 _ 66: 0F FE. CD
        movdqa  xmm2, xmm0                              ; 004D _ 66: 0F 6F. D0
        movdqa  xmm3, xmm1                              ; 0051 _ 66: 0F 6F. D9
        psrld   xmm0, 15                                ; 0055 _ 66: 0F 72. D0, 0F
        psrld   xmm1, 15                                ; 005A _ 66: 0F 72. D1, 0F
        pslld   xmm2, 17                                ; 005F _ 66: 0F 72. F2, 11
        pslld   xmm3, 17                                ; 0064 _ 66: 0F 72. F3, 11
        por     xmm0, xmm2                              ; 0069 _ 66: 0F EB. C2
        por     xmm1, xmm3                              ; 006D _ 66: 0F EB. CB
        movdqa  xmmword ptr [rsp], xmm0                 ; 0071 _ 66: 0F 7F. 04 24
        movdqa  xmmword ptr [rsp+10H], xmm1             ; 0076 _ 66: 0F 7F. 4C 24, 10
        movdqa  xmm0, xmmword ptr [rbp+50H]             ; 007C _ 66: 0F 6F. 45, 50
        movdqa  xmm1, xmmword ptr [rbp+60H]             ; 0081 _ 66: 0F 6F. 4D, 60
        paddd   xmm0, xmm6                              ; 0086 _ 66: 0F FE. C6
        paddd   xmm1, xmm7                              ; 008A _ 66: 0F FE. CF
        movdqa  xmm2, xmm0                              ; 008E _ 66: 0F 6F. D0
        movdqa  xmm3, xmm1                              ; 0092 _ 66: 0F 6F. D9
        psrld   xmm0, 15                                ; 0096 _ 66: 0F 72. D0, 0F
        psrld   xmm1, 15                                ; 009B _ 66: 0F 72. D1, 0F
        pslld   xmm2, 17                                ; 00A0 _ 66: 0F 72. F2, 11
        pslld   xmm3, 17                                ; 00A5 _ 66: 0F 72. F3, 11
        por     xmm0, xmm2                              ; 00AA _ 66: 0F EB. C2
        por     xmm1, xmm3                              ; 00AE _ 66: 0F EB. CB
        movdqa  xmmword ptr [rsp+20H], xmm0             ; 00B2 _ 66: 0F 7F. 44 24, 20
        movdqa  xmmword ptr [rsp+30H], xmm1             ; 00B8 _ 66: 0F 7F. 4C 24, 30
        mov     rax, qword ptr [rbp+0B0H]               ; 00BE _ 48: 8B. 85, 000000B0
        xor     qword ptr [rbp], rax                    ; 00C5 _ 48: 31. 45, 00
        mov     eax, dword ptr [rbp+2CH]                ; 00C9 _ 8B. 45, 2C
        rol     eax, 15                                 ; 00CC _ C1. C0, 0F
        mov     edx, dword ptr [rsp+18H]                ; 00CF _ 8B. 54 24, 18
        lea     eax, [rax+rax*4]                        ; 00D3 _ 8D. 04 80
        movd    ecx, xmm4                               ; 00D6 _ 66: 0F 7E. E1
        xor     eax, dword ptr [rbp]                    ; 00DA _ 33. 45, 00
        not     edx                                     ; 00DD _ F7. D2
        xor     ecx, dword ptr [rsp+34H]                ; 00DF _ 33. 4C 24, 34
        and     edx, dword ptr [rsp+24H]                ; 00E3 _ 23. 54 24, 24
        xor     eax, dword ptr [rbp+90H]                ; 00E7 _ 33. 85, 00000090
        xor     edx, ecx                                ; 00ED _ 31. CA
        mov     ecx, dword ptr [rsp]                    ; 00EF _ 8B. 0C 24
        lea     eax, [rax+rax*2]                        ; 00F2 _ 8D. 04 40
        not     ecx                                     ; 00F5 _ F7. D1
        xor     eax, edx                                ; 00F7 _ 31. D0
        rol     ecx, 1                                  ; 00F9 _ D1. C1
        mov     dword ptr [rbp], eax                    ; 00FB _ 89. 45, 00
        xor     ecx, eax                                ; 00FE _ 31. C1
        rol     eax, 15                                 ; 0100 _ C1. C0, 0F
        mov     dword ptr [rsp+40H], ecx                ; 0103 _ 89. 4C 24, 40
        pshufd  xmm0, xmm4, 01H                         ; 0107 _ 66: 0F 70. C4, 01
        mov     edx, dword ptr [rsp+1CH]                ; 010C _ 8B. 54 24, 1C
        lea     eax, [rax+rax*4]                        ; 0110 _ 8D. 04 80
        movd    ecx, xmm0                               ; 0113 _ 66: 0F 7E. C1
        xor     eax, dword ptr [rbp+4H]                 ; 0117 _ 33. 45, 04
        not     edx                                     ; 011A _ F7. D2
        xor     ecx, dword ptr [rsp+38H]                ; 011C _ 33. 4C 24, 38
        and     edx, dword ptr [rsp+28H]                ; 0120 _ 23. 54 24, 28
        xor     eax, dword ptr [rbp+8CH]                ; 0124 _ 33. 85, 0000008C
        xor     edx, ecx                                ; 012A _ 31. CA
        mov     ecx, dword ptr [rsp+4H]                 ; 012C _ 8B. 4C 24, 04
        lea     eax, [rax+rax*2]                        ; 0130 _ 8D. 04 40
        not     ecx                                     ; 0133 _ F7. D1
        xor     eax, edx                                ; 0135 _ 31. D0
        rol     ecx, 1                                  ; 0137 _ D1. C1
        mov     dword ptr [rbp+4H], eax                 ; 0139 _ 89. 45, 04
        xor     ecx, eax                                ; 013C _ 31. C1
        rol     eax, 15                                 ; 013E _ C1. C0, 0F
        mov     dword ptr [rsp+44H], ecx                ; 0141 _ 89. 4C 24, 44
        pshufd  xmm0, xmm4, 02H                         ; 0145 _ 66: 0F 70. C4, 02
        mov     edx, dword ptr [rsp+20H]                ; 014A _ 8B. 54 24, 20
        lea     eax, [rax+rax*4]                        ; 014E _ 8D. 04 80
        movd    ecx, xmm0                               ; 0151 _ 66: 0F 7E. C1
        xor     eax, dword ptr [rbp+8H]                 ; 0155 _ 33. 45, 08
        not     edx                                     ; 0158 _ F7. D2
        xor     ecx, dword ptr [rsp+3CH]                ; 015A _ 33. 4C 24, 3C
        and     edx, dword ptr [rsp+2CH]                ; 015E _ 23. 54 24, 2C
        xor     eax, dword ptr [rbp+88H]                ; 0162 _ 33. 85, 00000088
        xor     edx, ecx                                ; 0168 _ 31. CA
        mov     ecx, dword ptr [rsp+8H]                 ; 016A _ 8B. 4C 24, 08
        lea     eax, [rax+rax*2]                        ; 016E _ 8D. 04 40
        not     ecx                                     ; 0171 _ F7. D1
        xor     eax, edx                                ; 0173 _ 31. D0
        rol     ecx, 1                                  ; 0175 _ D1. C1
        mov     dword ptr [rbp+8H], eax                 ; 0177 _ 89. 45, 08
        xor     ecx, eax                                ; 017A _ 31. C1
        rol     eax, 15                                 ; 017C _ C1. C0, 0F
        mov     dword ptr [rsp+48H], ecx                ; 017F _ 89. 4C 24, 48
        pshufd  xmm0, xmm4, 03H                         ; 0183 _ 66: 0F 70. C4, 03
        mov     edx, dword ptr [rsp+24H]                ; 0188 _ 8B. 54 24, 24
        lea     eax, [rax+rax*4]                        ; 018C _ 8D. 04 80
        movd    ecx, xmm0                               ; 018F _ 66: 0F 7E. C1
        xor     eax, dword ptr [rbp+0CH]                ; 0193 _ 33. 45, 0C
        not     edx                                     ; 0196 _ F7. D2
        xor     ecx, dword ptr [rsp+40H]                ; 0198 _ 33. 4C 24, 40
        and     edx, dword ptr [rsp+30H]                ; 019C _ 23. 54 24, 30
        xor     eax, dword ptr [rbp+84H]                ; 01A0 _ 33. 85, 00000084
        xor     edx, ecx                                ; 01A6 _ 31. CA
        mov     ecx, dword ptr [rsp+0CH]                ; 01A8 _ 8B. 4C 24, 0C
        lea     eax, [rax+rax*2]                        ; 01AC _ 8D. 04 40
        not     ecx                                     ; 01AF _ F7. D1
        xor     eax, edx                                ; 01B1 _ 31. D0
        rol     ecx, 1                                  ; 01B3 _ D1. C1
        mov     dword ptr [rbp+0CH], eax                ; 01B5 _ 89. 45, 0C
        xor     ecx, eax                                ; 01B8 _ 31. C1
        rol     eax, 15                                 ; 01BA _ C1. C0, 0F
        mov     dword ptr [rsp+4CH], ecx                ; 01BD _ 89. 4C 24, 4C
        mov     edx, dword ptr [rsp+28H]                ; 01C1 _ 8B. 54 24, 28
        lea     eax, [rax+rax*4]                        ; 01C5 _ 8D. 04 80
        movd    ecx, xmm5                               ; 01C8 _ 66: 0F 7E. E9
        xor     eax, dword ptr [rbp+10H]                ; 01CC _ 33. 45, 10
        not     edx                                     ; 01CF _ F7. D2
        xor     ecx, dword ptr [rsp+44H]                ; 01D1 _ 33. 4C 24, 44
        and     edx, dword ptr [rsp+34H]                ; 01D5 _ 23. 54 24, 34
        xor     eax, dword ptr [rbp+80H]                ; 01D9 _ 33. 85, 00000080
        xor     edx, ecx                                ; 01DF _ 31. CA
        mov     ecx, dword ptr [rsp+10H]                ; 01E1 _ 8B. 4C 24, 10
        lea     eax, [rax+rax*2]                        ; 01E5 _ 8D. 04 40
        not     ecx                                     ; 01E8 _ F7. D1
        xor     eax, edx                                ; 01EA _ 31. D0
        rol     ecx, 1                                  ; 01EC _ D1. C1
        mov     dword ptr [rbp+10H], eax                ; 01EE _ 89. 45, 10
        xor     ecx, eax                                ; 01F1 _ 31. C1
        rol     eax, 15                                 ; 01F3 _ C1. C0, 0F
        mov     dword ptr [rsp+50H], ecx                ; 01F6 _ 89. 4C 24, 50
        pshufd  xmm0, xmm5, 01H                         ; 01FA _ 66: 0F 70. C5, 01
        mov     edx, dword ptr [rsp+2CH]                ; 01FF _ 8B. 54 24, 2C
        lea     eax, [rax+rax*4]                        ; 0203 _ 8D. 04 80
        movd    ecx, xmm0                               ; 0206 _ 66: 0F 7E. C1
        xor     eax, dword ptr [rbp+14H]                ; 020A _ 33. 45, 14
        not     edx                                     ; 020D _ F7. D2
        xor     ecx, dword ptr [rsp+48H]                ; 020F _ 33. 4C 24, 48
        and     edx, dword ptr [rsp+38H]                ; 0213 _ 23. 54 24, 38
        xor     eax, dword ptr [rbp+7CH]                ; 0217 _ 33. 45, 7C
        xor     edx, ecx                                ; 021A _ 31. CA
        mov     ecx, dword ptr [rsp+14H]                ; 021C _ 8B. 4C 24, 14
        lea     eax, [rax+rax*2]                        ; 0220 _ 8D. 04 40
        not     ecx                                     ; 0223 _ F7. D1
        xor     eax, edx                                ; 0225 _ 31. D0
        rol     ecx, 1                                  ; 0227 _ D1. C1
        mov     dword ptr [rbp+14H], eax                ; 0229 _ 89. 45, 14
        xor     ecx, eax                                ; 022C _ 31. C1
        rol     eax, 15                                 ; 022E _ C1. C0, 0F
        mov     dword ptr [rsp+54H], ecx                ; 0231 _ 89. 4C 24, 54
        pshufd  xmm0, xmm5, 02H                         ; 0235 _ 66: 0F 70. C5, 02
        mov     edx, dword ptr [rsp+30H]                ; 023A _ 8B. 54 24, 30
        lea     eax, [rax+rax*4]                        ; 023E _ 8D. 04 80
        movd    ecx, xmm0                               ; 0241 _ 66: 0F 7E. C1
        xor     eax, dword ptr [rbp+18H]                ; 0245 _ 33. 45, 18
        not     edx                                     ; 0248 _ F7. D2
        xor     ecx, dword ptr [rsp+4CH]                ; 024A _ 33. 4C 24, 4C
        and     edx, dword ptr [rsp+3CH]                ; 024E _ 23. 54 24, 3C
        xor     eax, dword ptr [rbp+78H]                ; 0252 _ 33. 45, 78
        xor     edx, ecx                                ; 0255 _ 31. CA
        mov     ecx, dword ptr [rsp+18H]                ; 0257 _ 8B. 4C 24, 18
        lea     eax, [rax+rax*2]                        ; 025B _ 8D. 04 40
        not     ecx                                     ; 025E _ F7. D1
        xor     eax, edx                                ; 0260 _ 31. D0
        rol     ecx, 1                                  ; 0262 _ D1. C1
        mov     dword ptr [rbp+18H], eax                ; 0264 _ 89. 45, 18
        xor     ecx, eax                                ; 0267 _ 31. C1
        rol     eax, 15                                 ; 0269 _ C1. C0, 0F
        mov     dword ptr [rsp+58H], ecx                ; 026C _ 89. 4C 24, 58
        pshufd  xmm0, xmm5, 03H                         ; 0270 _ 66: 0F 70. C5, 03
        mov     edx, dword ptr [rsp+34H]                ; 0275 _ 8B. 54 24, 34
        lea     eax, [rax+rax*4]                        ; 0279 _ 8D. 04 80
        movd    ecx, xmm0                               ; 027C _ 66: 0F 7E. C1
        xor     eax, dword ptr [rbp+1CH]                ; 0280 _ 33. 45, 1C
        not     edx                                     ; 0283 _ F7. D2
        xor     ecx, dword ptr [rsp+50H]                ; 0285 _ 33. 4C 24, 50
        and     edx, dword ptr [rsp+40H]                ; 0289 _ 23. 54 24, 40
        xor     eax, dword ptr [rbp+74H]                ; 028D _ 33. 45, 74
        xor     edx, ecx                                ; 0290 _ 31. CA
        mov     ecx, dword ptr [rsp+1CH]                ; 0292 _ 8B. 4C 24, 1C
        lea     eax, [rax+rax*2]                        ; 0296 _ 8D. 04 40
        not     ecx                                     ; 0299 _ F7. D1
        xor     eax, edx                                ; 029B _ 31. D0
        rol     ecx, 1                                  ; 029D _ D1. C1
        mov     dword ptr [rbp+1CH], eax                ; 029F _ 89. 45, 1C
        xor     ecx, eax                                ; 02A2 _ 31. C1
        rol     eax, 15                                 ; 02A4 _ C1. C0, 0F
        mov     dword ptr [rsp+5CH], ecx                ; 02A7 _ 89. 4C 24, 5C
        mov     edx, dword ptr [rsp+38H]                ; 02AB _ 8B. 54 24, 38
        lea     eax, [rax+rax*4]                        ; 02AF _ 8D. 04 80
        movd    ecx, xmm6                               ; 02B2 _ 66: 0F 7E. F1
        xor     eax, dword ptr [rbp+20H]                ; 02B6 _ 33. 45, 20
        not     edx                                     ; 02B9 _ F7. D2
        xor     ecx, dword ptr [rsp+54H]                ; 02BB _ 33. 4C 24, 54
        and     edx, dword ptr [rsp+44H]                ; 02BF _ 23. 54 24, 44
        xor     eax, dword ptr [rbp+70H]                ; 02C3 _ 33. 45, 70
        xor     edx, ecx                                ; 02C6 _ 31. CA
        mov     ecx, dword ptr [rsp+20H]                ; 02C8 _ 8B. 4C 24, 20
        lea     eax, [rax+rax*2]                        ; 02CC _ 8D. 04 40
        not     ecx                                     ; 02CF _ F7. D1
        xor     eax, edx                                ; 02D1 _ 31. D0
        rol     ecx, 1                                  ; 02D3 _ D1. C1
        mov     dword ptr [rbp+20H], eax                ; 02D5 _ 89. 45, 20
        xor     ecx, eax                                ; 02D8 _ 31. C1
        rol     eax, 15                                 ; 02DA _ C1. C0, 0F
        mov     dword ptr [rsp+60H], ecx                ; 02DD _ 89. 4C 24, 60
        pshufd  xmm0, xmm6, 01H                         ; 02E1 _ 66: 0F 70. C6, 01
        mov     edx, dword ptr [rsp+3CH]                ; 02E6 _ 8B. 54 24, 3C
        lea     eax, [rax+rax*4]                        ; 02EA _ 8D. 04 80
        movd    ecx, xmm0                               ; 02ED _ 66: 0F 7E. C1
        xor     eax, dword ptr [rbp+24H]                ; 02F1 _ 33. 45, 24
        not     edx                                     ; 02F4 _ F7. D2
        xor     ecx, dword ptr [rsp+58H]                ; 02F6 _ 33. 4C 24, 58
        and     edx, dword ptr [rsp+48H]                ; 02FA _ 23. 54 24, 48
        xor     eax, dword ptr [rbp+0ACH]               ; 02FE _ 33. 85, 000000AC
        xor     edx, ecx                                ; 0304 _ 31. CA
        mov     ecx, dword ptr [rsp+24H]                ; 0306 _ 8B. 4C 24, 24
        lea     eax, [rax+rax*2]                        ; 030A _ 8D. 04 40
        not     ecx                                     ; 030D _ F7. D1
        xor     eax, edx                                ; 030F _ 31. D0
        rol     ecx, 1                                  ; 0311 _ D1. C1
        mov     dword ptr [rbp+24H], eax                ; 0313 _ 89. 45, 24
        xor     ecx, eax                                ; 0316 _ 31. C1
        rol     eax, 15                                 ; 0318 _ C1. C0, 0F
        mov     dword ptr [rsp+64H], ecx                ; 031B _ 89. 4C 24, 64
        pshufd  xmm0, xmm6, 02H                         ; 031F _ 66: 0F 70. C6, 02
        mov     edx, dword ptr [rsp+40H]                ; 0324 _ 8B. 54 24, 40
        lea     eax, [rax+rax*4]                        ; 0328 _ 8D. 04 80
        movd    ecx, xmm0                               ; 032B _ 66: 0F 7E. C1
        xor     eax, dword ptr [rbp+28H]                ; 032F _ 33. 45, 28
        not     edx                                     ; 0332 _ F7. D2
        xor     ecx, dword ptr [rsp+5CH]                ; 0334 _ 33. 4C 24, 5C
        and     edx, dword ptr [rsp+4CH]                ; 0338 _ 23. 54 24, 4C
        xor     eax, dword ptr [rbp+0A8H]               ; 033C _ 33. 85, 000000A8
        xor     edx, ecx                                ; 0342 _ 31. CA
        mov     ecx, dword ptr [rsp+28H]                ; 0344 _ 8B. 4C 24, 28
        lea     eax, [rax+rax*2]                        ; 0348 _ 8D. 04 40
        not     ecx                                     ; 034B _ F7. D1
        xor     eax, edx                                ; 034D _ 31. D0
        rol     ecx, 1                                  ; 034F _ D1. C1
        mov     dword ptr [rbp+28H], eax                ; 0351 _ 89. 45, 28
        xor     ecx, eax                                ; 0354 _ 31. C1
        rol     eax, 15                                 ; 0356 _ C1. C0, 0F
        mov     dword ptr [rsp+68H], ecx                ; 0359 _ 89. 4C 24, 68
        pshufd  xmm0, xmm6, 03H                         ; 035D _ 66: 0F 70. C6, 03
        mov     edx, dword ptr [rsp+44H]                ; 0362 _ 8B. 54 24, 44
        lea     eax, [rax+rax*4]                        ; 0366 _ 8D. 04 80
        movd    ecx, xmm0                               ; 0369 _ 66: 0F 7E. C1
        xor     eax, dword ptr [rbp+2CH]                ; 036D _ 33. 45, 2C
        not     edx                                     ; 0370 _ F7. D2
        xor     ecx, dword ptr [rsp+60H]                ; 0372 _ 33. 4C 24, 60
        and     edx, dword ptr [rsp+50H]                ; 0376 _ 23. 54 24, 50
        xor     eax, dword ptr [rbp+0A4H]               ; 037A _ 33. 85, 000000A4
        xor     edx, ecx                                ; 0380 _ 31. CA
        mov     ecx, dword ptr [rsp+2CH]                ; 0382 _ 8B. 4C 24, 2C
        lea     eax, [rax+rax*2]                        ; 0386 _ 8D. 04 40
        not     ecx                                     ; 0389 _ F7. D1
        xor     eax, edx                                ; 038B _ 31. D0
        rol     ecx, 1                                  ; 038D _ D1. C1
        mov     dword ptr [rbp+2CH], eax                ; 038F _ 89. 45, 2C
        xor     ecx, eax                                ; 0392 _ 31. C1
        rol     eax, 15                                 ; 0394 _ C1. C0, 0F
        mov     dword ptr [rsp+6CH], ecx                ; 0397 _ 89. 4C 24, 6C
        mov     edx, dword ptr [rsp+48H]                ; 039B _ 8B. 54 24, 48
        lea     eax, [rax+rax*4]                        ; 039F _ 8D. 04 80
        movd    ecx, xmm7                               ; 03A2 _ 66: 0F 7E. F9
        xor     eax, dword ptr [rbp]                    ; 03A6 _ 33. 45, 00
        not     edx                                     ; 03A9 _ F7. D2
        xor     ecx, dword ptr [rsp+64H]                ; 03AB _ 33. 4C 24, 64
        and     edx, dword ptr [rsp+54H]                ; 03AF _ 23. 54 24, 54
        xor     eax, dword ptr [rbp+0A0H]               ; 03B3 _ 33. 85, 000000A0
        xor     edx, ecx                                ; 03B9 _ 31. CA
        mov     ecx, dword ptr [rsp+30H]                ; 03BB _ 8B. 4C 24, 30
        lea     eax, [rax+rax*2]                        ; 03BF _ 8D. 04 40
        not     ecx                                     ; 03C2 _ F7. D1
        xor     eax, edx                                ; 03C4 _ 31. D0
        rol     ecx, 1                                  ; 03C6 _ D1. C1
        mov     dword ptr [rbp], eax                    ; 03C8 _ 89. 45, 00
        xor     ecx, eax                                ; 03CB _ 31. C1
        rol     eax, 15                                 ; 03CD _ C1. C0, 0F
        mov     dword ptr [rsp+70H], ecx                ; 03D0 _ 89. 4C 24, 70
        pshufd  xmm0, xmm7, 01H                         ; 03D4 _ 66: 0F 70. C7, 01
        mov     edx, dword ptr [rsp+4CH]                ; 03D9 _ 8B. 54 24, 4C
        lea     eax, [rax+rax*4]                        ; 03DD _ 8D. 04 80
        movd    ecx, xmm0                               ; 03E0 _ 66: 0F 7E. C1
        xor     eax, dword ptr [rbp+4H]                 ; 03E4 _ 33. 45, 04
        not     edx                                     ; 03E7 _ F7. D2
        xor     ecx, dword ptr [rsp+68H]                ; 03E9 _ 33. 4C 24, 68
        and     edx, dword ptr [rsp+58H]                ; 03ED _ 23. 54 24, 58
        xor     eax, dword ptr [rbp+9CH]                ; 03F1 _ 33. 85, 0000009C
        xor     edx, ecx                                ; 03F7 _ 31. CA
        mov     ecx, dword ptr [rsp+34H]                ; 03F9 _ 8B. 4C 24, 34
        lea     eax, [rax+rax*2]                        ; 03FD _ 8D. 04 40
        not     ecx                                     ; 0400 _ F7. D1
        xor     eax, edx                                ; 0402 _ 31. D0
        rol     ecx, 1                                  ; 0404 _ D1. C1
        mov     dword ptr [rbp+4H], eax                 ; 0406 _ 89. 45, 04
        xor     ecx, eax                                ; 0409 _ 31. C1
        rol     eax, 15                                 ; 040B _ C1. C0, 0F
        mov     dword ptr [rsp+74H], ecx                ; 040E _ 89. 4C 24, 74
        pshufd  xmm0, xmm7, 02H                         ; 0412 _ 66: 0F 70. C7, 02
        mov     edx, dword ptr [rsp+50H]                ; 0417 _ 8B. 54 24, 50
        lea     eax, [rax+rax*4]                        ; 041B _ 8D. 04 80
        movd    ecx, xmm0                               ; 041E _ 66: 0F 7E. C1
        xor     eax, dword ptr [rbp+8H]                 ; 0422 _ 33. 45, 08
        not     edx                                     ; 0425 _ F7. D2
        xor     ecx, dword ptr [rsp+6CH]                ; 0427 _ 33. 4C 24, 6C
        and     edx, dword ptr [rsp+5CH]                ; 042B _ 23. 54 24, 5C
        xor     eax, dword ptr [rbp+98H]                ; 042F _ 33. 85, 00000098
        xor     edx, ecx                                ; 0435 _ 31. CA
        mov     ecx, dword ptr [rsp+38H]                ; 0437 _ 8B. 4C 24, 38
        lea     eax, [rax+rax*2]                        ; 043B _ 8D. 04 40
        not     ecx                                     ; 043E _ F7. D1
        xor     eax, edx                                ; 0440 _ 31. D0
        rol     ecx, 1                                  ; 0442 _ D1. C1
        mov     dword ptr [rbp+8H], eax                 ; 0444 _ 89. 45, 08
        xor     ecx, eax                                ; 0447 _ 31. C1
        rol     eax, 15                                 ; 0449 _ C1. C0, 0F
        mov     dword ptr [rsp+78H], ecx                ; 044C _ 89. 4C 24, 78
        pshufd  xmm0, xmm7, 03H                         ; 0450 _ 66: 0F 70. C7, 03
        mov     edx, dword ptr [rsp+54H]                ; 0455 _ 8B. 54 24, 54
        lea     eax, [rax+rax*4]                        ; 0459 _ 8D. 04 80
        movd    ecx, xmm0                               ; 045C _ 66: 0F 7E. C1
        xor     eax, dword ptr [rbp+0CH]                ; 0460 _ 33. 45, 0C
        not     edx                                     ; 0463 _ F7. D2
        xor     ecx, dword ptr [rsp+70H]                ; 0465 _ 33. 4C 24, 70
        and     edx, dword ptr [rsp+60H]                ; 0469 _ 23. 54 24, 60
        xor     eax, dword ptr [rbp+94H]                ; 046D _ 33. 85, 00000094
        xor     edx, ecx                                ; 0473 _ 31. CA
        mov     ecx, dword ptr [rsp+3CH]                ; 0475 _ 8B. 4C 24, 3C
        lea     eax, [rax+rax*2]                        ; 0479 _ 8D. 04 40
        not     ecx                                     ; 047C _ F7. D1
        xor     eax, edx                                ; 047E _ 31. D0
        rol     ecx, 1                                  ; 0480 _ D1. C1
        mov     dword ptr [rbp+0CH], eax                ; 0482 _ 89. 45, 0C
        xor     ecx, eax                                ; 0485 _ 31. C1
        rol     eax, 15                                 ; 0487 _ C1. C0, 0F
        mov     dword ptr [rsp+7CH], ecx                ; 048A _ 89. 4C 24, 7C
        mov     edx, dword ptr [rsp+58H]                ; 048E _ 8B. 54 24, 58
        lea     eax, [rax+rax*4]                        ; 0492 _ 8D. 04 80
        movd    ecx, xmm4                               ; 0495 _ 66: 0F 7E. E1
        xor     eax, dword ptr [rbp+10H]                ; 0499 _ 33. 45, 10
        not     edx                                     ; 049C _ F7. D2
        xor     ecx, dword ptr [rsp+74H]                ; 049E _ 33. 4C 24, 74
        and     edx, dword ptr [rsp+64H]                ; 04A2 _ 23. 54 24, 64
        xor     eax, dword ptr [rbp+90H]                ; 04A6 _ 33. 85, 00000090
        xor     edx, ecx                                ; 04AC _ 31. CA
        mov     ecx, dword ptr [rsp+40H]                ; 04AE _ 8B. 4C 24, 40
        lea     eax, [rax+rax*2]                        ; 04B2 _ 8D. 04 40
        not     ecx                                     ; 04B5 _ F7. D1
        xor     eax, edx                                ; 04B7 _ 31. D0
        rol     ecx, 1                                  ; 04B9 _ D1. C1
        mov     dword ptr [rbp+10H], eax                ; 04BB _ 89. 45, 10
        xor     ecx, eax                                ; 04BE _ 31. C1
        rol     eax, 15                                 ; 04C0 _ C1. C0, 0F
        mov     dword ptr [rsp+80H], ecx                ; 04C3 _ 89. 8C 24, 00000080
        pshufd  xmm0, xmm4, 01H                         ; 04CA _ 66: 0F 70. C4, 01
        mov     edx, dword ptr [rsp+5CH]                ; 04CF _ 8B. 54 24, 5C
        lea     eax, [rax+rax*4]                        ; 04D3 _ 8D. 04 80
        movd    ecx, xmm0                               ; 04D6 _ 66: 0F 7E. C1
        xor     eax, dword ptr [rbp+14H]                ; 04DA _ 33. 45, 14
        not     edx                                     ; 04DD _ F7. D2
        xor     ecx, dword ptr [rsp+78H]                ; 04DF _ 33. 4C 24, 78
        and     edx, dword ptr [rsp+68H]                ; 04E3 _ 23. 54 24, 68
        xor     eax, dword ptr [rbp+8CH]                ; 04E7 _ 33. 85, 0000008C
        xor     edx, ecx                                ; 04ED _ 31. CA
        mov     ecx, dword ptr [rsp+44H]                ; 04EF _ 8B. 4C 24, 44
        lea     eax, [rax+rax*2]                        ; 04F3 _ 8D. 04 40
        not     ecx                                     ; 04F6 _ F7. D1
        xor     eax, edx                                ; 04F8 _ 31. D0
        rol     ecx, 1                                  ; 04FA _ D1. C1
        mov     dword ptr [rbp+14H], eax                ; 04FC _ 89. 45, 14
        xor     ecx, eax                                ; 04FF _ 31. C1
        rol     eax, 15                                 ; 0501 _ C1. C0, 0F
        mov     dword ptr [rsp+84H], ecx                ; 0504 _ 89. 8C 24, 00000084
        pshufd  xmm0, xmm4, 02H                         ; 050B _ 66: 0F 70. C4, 02
        mov     edx, dword ptr [rsp+60H]                ; 0510 _ 8B. 54 24, 60
        lea     eax, [rax+rax*4]                        ; 0514 _ 8D. 04 80
        movd    ecx, xmm0                               ; 0517 _ 66: 0F 7E. C1
        xor     eax, dword ptr [rbp+18H]                ; 051B _ 33. 45, 18
        not     edx                                     ; 051E _ F7. D2
        xor     ecx, dword ptr [rsp+7CH]                ; 0520 _ 33. 4C 24, 7C
        and     edx, dword ptr [rsp+6CH]                ; 0524 _ 23. 54 24, 6C
        xor     eax, dword ptr [rbp+88H]                ; 0528 _ 33. 85, 00000088
        xor     edx, ecx                                ; 052E _ 31. CA
        mov     ecx, dword ptr [rsp+48H]                ; 0530 _ 8B. 4C 24, 48
        lea     eax, [rax+rax*2]                        ; 0534 _ 8D. 04 40
        not     ecx                                     ; 0537 _ F7. D1
        xor     eax, edx                                ; 0539 _ 31. D0
        rol     ecx, 1                                  ; 053B _ D1. C1
        mov     dword ptr [rbp+18H], eax                ; 053D _ 89. 45, 18
        xor     ecx, eax                                ; 0540 _ 31. C1
        rol     eax, 15                                 ; 0542 _ C1. C0, 0F
        mov     dword ptr [rsp+88H], ecx                ; 0545 _ 89. 8C 24, 00000088
        pshufd  xmm0, xmm4, 03H                         ; 054C _ 66: 0F 70. C4, 03
        mov     edx, dword ptr [rsp+64H]                ; 0551 _ 8B. 54 24, 64
        lea     eax, [rax+rax*4]                        ; 0555 _ 8D. 04 80
        movd    ecx, xmm0                               ; 0558 _ 66: 0F 7E. C1
        xor     eax, dword ptr [rbp+1CH]                ; 055C _ 33. 45, 1C
        not     edx                                     ; 055F _ F7. D2
        xor     ecx, dword ptr [rsp+80H]                ; 0561 _ 33. 8C 24, 00000080
        and     edx, dword ptr [rsp+70H]                ; 0568 _ 23. 54 24, 70
        xor     eax, dword ptr [rbp+84H]                ; 056C _ 33. 85, 00000084
        xor     edx, ecx                                ; 0572 _ 31. CA
        mov     ecx, dword ptr [rsp+4CH]                ; 0574 _ 8B. 4C 24, 4C
        lea     eax, [rax+rax*2]                        ; 0578 _ 8D. 04 40
        not     ecx                                     ; 057B _ F7. D1
        xor     eax, edx                                ; 057D _ 31. D0
        rol     ecx, 1                                  ; 057F _ D1. C1
        mov     dword ptr [rbp+1CH], eax                ; 0581 _ 89. 45, 1C
        xor     ecx, eax                                ; 0584 _ 31. C1
        rol     eax, 15                                 ; 0586 _ C1. C0, 0F
        mov     dword ptr [rsp+8CH], ecx                ; 0589 _ 89. 8C 24, 0000008C
        mov     edx, dword ptr [rsp+68H]                ; 0590 _ 8B. 54 24, 68
        lea     eax, [rax+rax*4]                        ; 0594 _ 8D. 04 80
        movd    ecx, xmm5                               ; 0597 _ 66: 0F 7E. E9
        xor     eax, dword ptr [rbp+20H]                ; 059B _ 33. 45, 20
        not     edx                                     ; 059E _ F7. D2
        xor     ecx, dword ptr [rsp+84H]                ; 05A0 _ 33. 8C 24, 00000084
        and     edx, dword ptr [rsp+74H]                ; 05A7 _ 23. 54 24, 74
        xor     eax, dword ptr [rbp+80H]                ; 05AB _ 33. 85, 00000080
        xor     edx, ecx                                ; 05B1 _ 31. CA
        mov     ecx, dword ptr [rsp+50H]                ; 05B3 _ 8B. 4C 24, 50
        lea     eax, [rax+rax*2]                        ; 05B7 _ 8D. 04 40
        not     ecx                                     ; 05BA _ F7. D1
        xor     eax, edx                                ; 05BC _ 31. D0
        rol     ecx, 1                                  ; 05BE _ D1. C1
        mov     dword ptr [rbp+20H], eax                ; 05C0 _ 89. 45, 20
        xor     ecx, eax                                ; 05C3 _ 31. C1
        rol     eax, 15                                 ; 05C5 _ C1. C0, 0F
        mov     dword ptr [rsp+90H], ecx                ; 05C8 _ 89. 8C 24, 00000090
        pshufd  xmm0, xmm5, 01H                         ; 05CF _ 66: 0F 70. C5, 01
        mov     edx, dword ptr [rsp+6CH]                ; 05D4 _ 8B. 54 24, 6C
        lea     eax, [rax+rax*4]                        ; 05D8 _ 8D. 04 80
        movd    ecx, xmm0                               ; 05DB _ 66: 0F 7E. C1
        xor     eax, dword ptr [rbp+24H]                ; 05DF _ 33. 45, 24
        not     edx                                     ; 05E2 _ F7. D2
        xor     ecx, dword ptr [rsp+88H]                ; 05E4 _ 33. 8C 24, 00000088
        and     edx, dword ptr [rsp+78H]                ; 05EB _ 23. 54 24, 78
        xor     eax, dword ptr [rbp+7CH]                ; 05EF _ 33. 45, 7C
        xor     edx, ecx                                ; 05F2 _ 31. CA
        mov     ecx, dword ptr [rsp+54H]                ; 05F4 _ 8B. 4C 24, 54
        lea     eax, [rax+rax*2]                        ; 05F8 _ 8D. 04 40
        not     ecx                                     ; 05FB _ F7. D1
        xor     eax, edx                                ; 05FD _ 31. D0
        rol     ecx, 1                                  ; 05FF _ D1. C1
        mov     dword ptr [rbp+24H], eax                ; 0601 _ 89. 45, 24
        xor     ecx, eax                                ; 0604 _ 31. C1
        rol     eax, 15                                 ; 0606 _ C1. C0, 0F
        mov     dword ptr [rsp+94H], ecx                ; 0609 _ 89. 8C 24, 00000094
        pshufd  xmm0, xmm5, 02H                         ; 0610 _ 66: 0F 70. C5, 02
        mov     edx, dword ptr [rsp+70H]                ; 0615 _ 8B. 54 24, 70
        lea     eax, [rax+rax*4]                        ; 0619 _ 8D. 04 80
        movd    ecx, xmm0                               ; 061C _ 66: 0F 7E. C1
        xor     eax, dword ptr [rbp+28H]                ; 0620 _ 33. 45, 28
        not     edx                                     ; 0623 _ F7. D2
        xor     ecx, dword ptr [rsp+8CH]                ; 0625 _ 33. 8C 24, 0000008C
        and     edx, dword ptr [rsp+7CH]                ; 062C _ 23. 54 24, 7C
        xor     eax, dword ptr [rbp+78H]                ; 0630 _ 33. 45, 78
        xor     edx, ecx                                ; 0633 _ 31. CA
        mov     ecx, dword ptr [rsp+58H]                ; 0635 _ 8B. 4C 24, 58
        lea     eax, [rax+rax*2]                        ; 0639 _ 8D. 04 40
        not     ecx                                     ; 063C _ F7. D1
        xor     eax, edx                                ; 063E _ 31. D0
        rol     ecx, 1                                  ; 0640 _ D1. C1
        mov     dword ptr [rbp+28H], eax                ; 0642 _ 89. 45, 28
        xor     ecx, eax                                ; 0645 _ 31. C1
        rol     eax, 15                                 ; 0647 _ C1. C0, 0F
        mov     dword ptr [rsp+98H], ecx                ; 064A _ 89. 8C 24, 00000098
        pshufd  xmm0, xmm5, 03H                         ; 0651 _ 66: 0F 70. C5, 03
        mov     edx, dword ptr [rsp+74H]                ; 0656 _ 8B. 54 24, 74
        lea     eax, [rax+rax*4]                        ; 065A _ 8D. 04 80
        movd    ecx, xmm0                               ; 065D _ 66: 0F 7E. C1
        xor     eax, dword ptr [rbp+2CH]                ; 0661 _ 33. 45, 2C
        not     edx                                     ; 0664 _ F7. D2
        xor     ecx, dword ptr [rsp+90H]                ; 0666 _ 33. 8C 24, 00000090
        and     edx, dword ptr [rsp+80H]                ; 066D _ 23. 94 24, 00000080
        xor     eax, dword ptr [rbp+74H]                ; 0674 _ 33. 45, 74
        xor     edx, ecx                                ; 0677 _ 31. CA
        mov     ecx, dword ptr [rsp+5CH]                ; 0679 _ 8B. 4C 24, 5C
        lea     eax, [rax+rax*2]                        ; 067D _ 8D. 04 40
        not     ecx                                     ; 0680 _ F7. D1
        xor     eax, edx                                ; 0682 _ 31. D0
        rol     ecx, 1                                  ; 0684 _ D1. C1
        mov     dword ptr [rbp+2CH], eax                ; 0686 _ 89. 45, 2C
        xor     ecx, eax                                ; 0689 _ 31. C1
        rol     eax, 15                                 ; 068B _ C1. C0, 0F
        mov     dword ptr [rsp+9CH], ecx                ; 068E _ 89. 8C 24, 0000009C
        mov     edx, dword ptr [rsp+78H]                ; 0695 _ 8B. 54 24, 78
        lea     eax, [rax+rax*4]                        ; 0699 _ 8D. 04 80
        movd    ecx, xmm6                               ; 069C _ 66: 0F 7E. F1
        xor     eax, dword ptr [rbp]                    ; 06A0 _ 33. 45, 00
        not     edx                                     ; 06A3 _ F7. D2
        xor     ecx, dword ptr [rsp+94H]                ; 06A5 _ 33. 8C 24, 00000094
        and     edx, dword ptr [rsp+84H]                ; 06AC _ 23. 94 24, 00000084
        xor     eax, dword ptr [rbp+70H]                ; 06B3 _ 33. 45, 70
        xor     edx, ecx                                ; 06B6 _ 31. CA
        mov     ecx, dword ptr [rsp+60H]                ; 06B8 _ 8B. 4C 24, 60
        lea     eax, [rax+rax*2]                        ; 06BC _ 8D. 04 40
        not     ecx                                     ; 06BF _ F7. D1
        xor     eax, edx                                ; 06C1 _ 31. D0
        rol     ecx, 1                                  ; 06C3 _ D1. C1
        mov     dword ptr [rbp], eax                    ; 06C5 _ 89. 45, 00
        xor     ecx, eax                                ; 06C8 _ 31. C1
        rol     eax, 15                                 ; 06CA _ C1. C0, 0F
        mov     dword ptr [rsp+0A0H], ecx               ; 06CD _ 89. 8C 24, 000000A0
        pshufd  xmm0, xmm6, 01H                         ; 06D4 _ 66: 0F 70. C6, 01
        mov     edx, dword ptr [rsp+7CH]                ; 06D9 _ 8B. 54 24, 7C
        lea     eax, [rax+rax*4]                        ; 06DD _ 8D. 04 80
        movd    ecx, xmm0                               ; 06E0 _ 66: 0F 7E. C1
        xor     eax, dword ptr [rbp+4H]                 ; 06E4 _ 33. 45, 04
        not     edx                                     ; 06E7 _ F7. D2
        xor     ecx, dword ptr [rsp+98H]                ; 06E9 _ 33. 8C 24, 00000098
        and     edx, dword ptr [rsp+88H]                ; 06F0 _ 23. 94 24, 00000088
        xor     eax, dword ptr [rbp+0ACH]               ; 06F7 _ 33. 85, 000000AC
        xor     edx, ecx                                ; 06FD _ 31. CA
        mov     ecx, dword ptr [rsp+64H]                ; 06FF _ 8B. 4C 24, 64
        lea     eax, [rax+rax*2]                        ; 0703 _ 8D. 04 40
        not     ecx                                     ; 0706 _ F7. D1
        xor     eax, edx                                ; 0708 _ 31. D0
        rol     ecx, 1                                  ; 070A _ D1. C1
        mov     dword ptr [rbp+4H], eax                 ; 070C _ 89. 45, 04
        xor     ecx, eax                                ; 070F _ 31. C1
        rol     eax, 15                                 ; 0711 _ C1. C0, 0F
        mov     dword ptr [rsp+0A4H], ecx               ; 0714 _ 89. 8C 24, 000000A4
        pshufd  xmm0, xmm6, 02H                         ; 071B _ 66: 0F 70. C6, 02
        mov     edx, dword ptr [rsp+80H]                ; 0720 _ 8B. 94 24, 00000080
        lea     eax, [rax+rax*4]                        ; 0727 _ 8D. 04 80
        movd    ecx, xmm0                               ; 072A _ 66: 0F 7E. C1
        xor     eax, dword ptr [rbp+8H]                 ; 072E _ 33. 45, 08
        not     edx                                     ; 0731 _ F7. D2
        xor     ecx, dword ptr [rsp+9CH]                ; 0733 _ 33. 8C 24, 0000009C
        and     edx, dword ptr [rsp+8CH]                ; 073A _ 23. 94 24, 0000008C
        xor     eax, dword ptr [rbp+0A8H]               ; 0741 _ 33. 85, 000000A8
        xor     edx, ecx                                ; 0747 _ 31. CA
        mov     ecx, dword ptr [rsp+68H]                ; 0749 _ 8B. 4C 24, 68
        lea     eax, [rax+rax*2]                        ; 074D _ 8D. 04 40
        not     ecx                                     ; 0750 _ F7. D1
        xor     eax, edx                                ; 0752 _ 31. D0
        rol     ecx, 1                                  ; 0754 _ D1. C1
        mov     dword ptr [rbp+8H], eax                 ; 0756 _ 89. 45, 08
        xor     ecx, eax                                ; 0759 _ 31. C1
        rol     eax, 15                                 ; 075B _ C1. C0, 0F
        mov     dword ptr [rsp+0A8H], ecx               ; 075E _ 89. 8C 24, 000000A8
        pshufd  xmm0, xmm6, 03H                         ; 0765 _ 66: 0F 70. C6, 03
        mov     edx, dword ptr [rsp+84H]                ; 076A _ 8B. 94 24, 00000084
        lea     eax, [rax+rax*4]                        ; 0771 _ 8D. 04 80
        movd    ecx, xmm0                               ; 0774 _ 66: 0F 7E. C1
        xor     eax, dword ptr [rbp+0CH]                ; 0778 _ 33. 45, 0C
        not     edx                                     ; 077B _ F7. D2
        xor     ecx, dword ptr [rsp+0A0H]               ; 077D _ 33. 8C 24, 000000A0
        and     edx, dword ptr [rsp+90H]                ; 0784 _ 23. 94 24, 00000090
        xor     eax, dword ptr [rbp+0A4H]               ; 078B _ 33. 85, 000000A4
        xor     edx, ecx                                ; 0791 _ 31. CA
        mov     ecx, dword ptr [rsp+6CH]                ; 0793 _ 8B. 4C 24, 6C
        lea     eax, [rax+rax*2]                        ; 0797 _ 8D. 04 40
        not     ecx                                     ; 079A _ F7. D1
        xor     eax, edx                                ; 079C _ 31. D0
        rol     ecx, 1                                  ; 079E _ D1. C1
        mov     dword ptr [rbp+0CH], eax                ; 07A0 _ 89. 45, 0C
        xor     ecx, eax                                ; 07A3 _ 31. C1
        rol     eax, 15                                 ; 07A5 _ C1. C0, 0F
        mov     dword ptr [rsp+0ACH], ecx               ; 07A8 _ 89. 8C 24, 000000AC
        mov     edx, dword ptr [rsp+88H]                ; 07AF _ 8B. 94 24, 00000088
        lea     eax, [rax+rax*4]                        ; 07B6 _ 8D. 04 80
        movd    ecx, xmm7                               ; 07B9 _ 66: 0F 7E. F9
        xor     eax, dword ptr [rbp+10H]                ; 07BD _ 33. 45, 10
        not     edx                                     ; 07C0 _ F7. D2
        xor     ecx, dword ptr [rsp+0A4H]               ; 07C2 _ 33. 8C 24, 000000A4
        and     edx, dword ptr [rsp+94H]                ; 07C9 _ 23. 94 24, 00000094
        xor     eax, dword ptr [rbp+0A0H]               ; 07D0 _ 33. 85, 000000A0
        xor     edx, ecx                                ; 07D6 _ 31. CA
        mov     ecx, dword ptr [rsp+70H]                ; 07D8 _ 8B. 4C 24, 70
        lea     eax, [rax+rax*2]                        ; 07DC _ 8D. 04 40
        not     ecx                                     ; 07DF _ F7. D1
        xor     eax, edx                                ; 07E1 _ 31. D0
        rol     ecx, 1                                  ; 07E3 _ D1. C1
        mov     dword ptr [rbp+10H], eax                ; 07E5 _ 89. 45, 10
        xor     ecx, eax                                ; 07E8 _ 31. C1
        rol     eax, 15                                 ; 07EA _ C1. C0, 0F
        mov     dword ptr [rsp+0B0H], ecx               ; 07ED _ 89. 8C 24, 000000B0
        pshufd  xmm0, xmm7, 01H                         ; 07F4 _ 66: 0F 70. C7, 01
        mov     edx, dword ptr [rsp+8CH]                ; 07F9 _ 8B. 94 24, 0000008C
        lea     eax, [rax+rax*4]                        ; 0800 _ 8D. 04 80
        movd    ecx, xmm0                               ; 0803 _ 66: 0F 7E. C1
        xor     eax, dword ptr [rbp+14H]                ; 0807 _ 33. 45, 14
        not     edx                                     ; 080A _ F7. D2
        xor     ecx, dword ptr [rsp+0A8H]               ; 080C _ 33. 8C 24, 000000A8
        and     edx, dword ptr [rsp+98H]                ; 0813 _ 23. 94 24, 00000098
        xor     eax, dword ptr [rbp+9CH]                ; 081A _ 33. 85, 0000009C
        xor     edx, ecx                                ; 0820 _ 31. CA
        mov     ecx, dword ptr [rsp+74H]                ; 0822 _ 8B. 4C 24, 74
        lea     eax, [rax+rax*2]                        ; 0826 _ 8D. 04 40
        not     ecx                                     ; 0829 _ F7. D1
        xor     eax, edx                                ; 082B _ 31. D0
        rol     ecx, 1                                  ; 082D _ D1. C1
        mov     dword ptr [rbp+14H], eax                ; 082F _ 89. 45, 14
        xor     ecx, eax                                ; 0832 _ 31. C1
        rol     eax, 15                                 ; 0834 _ C1. C0, 0F
        mov     dword ptr [rsp+0B4H], ecx               ; 0837 _ 89. 8C 24, 000000B4
        pshufd  xmm0, xmm7, 02H                         ; 083E _ 66: 0F 70. C7, 02
        mov     edx, dword ptr [rsp+90H]                ; 0843 _ 8B. 94 24, 00000090
        lea     eax, [rax+rax*4]                        ; 084A _ 8D. 04 80
        movd    ecx, xmm0                               ; 084D _ 66: 0F 7E. C1
        xor     eax, dword ptr [rbp+18H]                ; 0851 _ 33. 45, 18
        not     edx                                     ; 0854 _ F7. D2
        xor     ecx, dword ptr [rsp+0ACH]               ; 0856 _ 33. 8C 24, 000000AC
        and     edx, dword ptr [rsp+9CH]                ; 085D _ 23. 94 24, 0000009C
        xor     eax, dword ptr [rbp+98H]                ; 0864 _ 33. 85, 00000098
        xor     edx, ecx                                ; 086A _ 31. CA
        mov     ecx, dword ptr [rsp+78H]                ; 086C _ 8B. 4C 24, 78
        lea     eax, [rax+rax*2]                        ; 0870 _ 8D. 04 40
        not     ecx                                     ; 0873 _ F7. D1
        xor     eax, edx                                ; 0875 _ 31. D0
        rol     ecx, 1                                  ; 0877 _ D1. C1
        mov     dword ptr [rbp+18H], eax                ; 0879 _ 89. 45, 18
        xor     ecx, eax                                ; 087C _ 31. C1
        rol     eax, 15                                 ; 087E _ C1. C0, 0F
        mov     dword ptr [rsp+0B8H], ecx               ; 0881 _ 89. 8C 24, 000000B8
        pshufd  xmm0, xmm7, 03H                         ; 0888 _ 66: 0F 70. C7, 03
        mov     edx, dword ptr [rsp+94H]                ; 088D _ 8B. 94 24, 00000094
        lea     eax, [rax+rax*4]                        ; 0894 _ 8D. 04 80
        movd    ecx, xmm0                               ; 0897 _ 66: 0F 7E. C1
        xor     eax, dword ptr [rbp+1CH]                ; 089B _ 33. 45, 1C
        not     edx                                     ; 089E _ F7. D2
        xor     ecx, dword ptr [rsp+0B0H]               ; 08A0 _ 33. 8C 24, 000000B0
        and     edx, dword ptr [rsp+0A0H]               ; 08A7 _ 23. 94 24, 000000A0
        xor     eax, dword ptr [rbp+94H]                ; 08AE _ 33. 85, 00000094
        xor     edx, ecx                                ; 08B4 _ 31. CA
        mov     ecx, dword ptr [rsp+7CH]                ; 08B6 _ 8B. 4C 24, 7C
        lea     eax, [rax+rax*2]                        ; 08BA _ 8D. 04 40
        not     ecx                                     ; 08BD _ F7. D1
        xor     eax, edx                                ; 08BF _ 31. D0
        rol     ecx, 1                                  ; 08C1 _ D1. C1
        mov     dword ptr [rbp+1CH], eax                ; 08C3 _ 89. 45, 1C
        xor     ecx, eax                                ; 08C6 _ 31. C1
        rol     eax, 15                                 ; 08C8 _ C1. C0, 0F
        mov     dword ptr [rsp+0BCH], ecx               ; 08CB _ 89. 8C 24, 000000BC
        mov     edx, dword ptr [rsp+98H]                ; 08D2 _ 8B. 94 24, 00000098
        lea     eax, [rax+rax*4]                        ; 08D9 _ 8D. 04 80
        movd    ecx, xmm4                               ; 08DC _ 66: 0F 7E. E1
        xor     eax, dword ptr [rbp+20H]                ; 08E0 _ 33. 45, 20
        not     edx                                     ; 08E3 _ F7. D2
        xor     ecx, dword ptr [rsp+0B4H]               ; 08E5 _ 33. 8C 24, 000000B4
        and     edx, dword ptr [rsp+0A4H]               ; 08EC _ 23. 94 24, 000000A4
        xor     eax, dword ptr [rbp+90H]                ; 08F3 _ 33. 85, 00000090
        xor     edx, ecx                                ; 08F9 _ 31. CA
        mov     ecx, dword ptr [rsp+80H]                ; 08FB _ 8B. 8C 24, 00000080
        lea     eax, [rax+rax*2]                        ; 0902 _ 8D. 04 40
        not     ecx                                     ; 0905 _ F7. D1
        xor     eax, edx                                ; 0907 _ 31. D0
        rol     ecx, 1                                  ; 0909 _ D1. C1
        mov     dword ptr [rbp+20H], eax                ; 090B _ 89. 45, 20
        xor     ecx, eax                                ; 090E _ 31. C1
        rol     eax, 15                                 ; 0910 _ C1. C0, 0F
        mov     dword ptr [rsp+0C0H], ecx               ; 0913 _ 89. 8C 24, 000000C0
        pshufd  xmm0, xmm4, 01H                         ; 091A _ 66: 0F 70. C4, 01
        mov     edx, dword ptr [rsp+9CH]                ; 091F _ 8B. 94 24, 0000009C
        lea     eax, [rax+rax*4]                        ; 0926 _ 8D. 04 80
        movd    ecx, xmm0                               ; 0929 _ 66: 0F 7E. C1
        xor     eax, dword ptr [rbp+24H]                ; 092D _ 33. 45, 24
        not     edx                                     ; 0930 _ F7. D2
        xor     ecx, dword ptr [rsp+0B8H]               ; 0932 _ 33. 8C 24, 000000B8
        and     edx, dword ptr [rsp+0A8H]               ; 0939 _ 23. 94 24, 000000A8
        xor     eax, dword ptr [rbp+8CH]                ; 0940 _ 33. 85, 0000008C
        xor     edx, ecx                                ; 0946 _ 31. CA
        mov     ecx, dword ptr [rsp+84H]                ; 0948 _ 8B. 8C 24, 00000084
        lea     eax, [rax+rax*2]                        ; 094F _ 8D. 04 40
        not     ecx                                     ; 0952 _ F7. D1
        xor     eax, edx                                ; 0954 _ 31. D0
        rol     ecx, 1                                  ; 0956 _ D1. C1
        mov     dword ptr [rbp+24H], eax                ; 0958 _ 89. 45, 24
        xor     ecx, eax                                ; 095B _ 31. C1
        rol     eax, 15                                 ; 095D _ C1. C0, 0F
        mov     dword ptr [rsp+0C4H], ecx               ; 0960 _ 89. 8C 24, 000000C4
        pshufd  xmm0, xmm4, 02H                         ; 0967 _ 66: 0F 70. C4, 02
        mov     edx, dword ptr [rsp+0A0H]               ; 096C _ 8B. 94 24, 000000A0
        lea     eax, [rax+rax*4]                        ; 0973 _ 8D. 04 80
        movd    ecx, xmm0                               ; 0976 _ 66: 0F 7E. C1
        xor     eax, dword ptr [rbp+28H]                ; 097A _ 33. 45, 28
        not     edx                                     ; 097D _ F7. D2
        xor     ecx, dword ptr [rsp+0BCH]               ; 097F _ 33. 8C 24, 000000BC
        and     edx, dword ptr [rsp+0ACH]               ; 0986 _ 23. 94 24, 000000AC
        xor     eax, dword ptr [rbp+88H]                ; 098D _ 33. 85, 00000088
        xor     edx, ecx                                ; 0993 _ 31. CA
        mov     ecx, dword ptr [rsp+88H]                ; 0995 _ 8B. 8C 24, 00000088
        lea     eax, [rax+rax*2]                        ; 099C _ 8D. 04 40
        not     ecx                                     ; 099F _ F7. D1
        xor     eax, edx                                ; 09A1 _ 31. D0
        rol     ecx, 1                                  ; 09A3 _ D1. C1
        mov     dword ptr [rbp+28H], eax                ; 09A5 _ 89. 45, 28
        xor     ecx, eax                                ; 09A8 _ 31. C1
        rol     eax, 15                                 ; 09AA _ C1. C0, 0F
        mov     dword ptr [rsp+0C8H], ecx               ; 09AD _ 89. 8C 24, 000000C8
        pshufd  xmm0, xmm4, 03H                         ; 09B4 _ 66: 0F 70. C4, 03
        mov     edx, dword ptr [rsp+0A4H]               ; 09B9 _ 8B. 94 24, 000000A4
        lea     eax, [rax+rax*4]                        ; 09C0 _ 8D. 04 80
        movd    ecx, xmm0                               ; 09C3 _ 66: 0F 7E. C1
        xor     eax, dword ptr [rbp+2CH]                ; 09C7 _ 33. 45, 2C
        not     edx                                     ; 09CA _ F7. D2
        xor     ecx, dword ptr [rsp+0C0H]               ; 09CC _ 33. 8C 24, 000000C0
        and     edx, dword ptr [rsp+0B0H]               ; 09D3 _ 23. 94 24, 000000B0
        xor     eax, dword ptr [rbp+84H]                ; 09DA _ 33. 85, 00000084
        xor     edx, ecx                                ; 09E0 _ 31. CA
        mov     ecx, dword ptr [rsp+8CH]                ; 09E2 _ 8B. 8C 24, 0000008C
        lea     eax, [rax+rax*2]                        ; 09E9 _ 8D. 04 40
        not     ecx                                     ; 09EC _ F7. D1
        xor     eax, edx                                ; 09EE _ 31. D0
        rol     ecx, 1                                  ; 09F0 _ D1. C1
        mov     dword ptr [rbp+2CH], eax                ; 09F2 _ 89. 45, 2C
        xor     ecx, eax                                ; 09F5 _ 31. C1
        rol     eax, 15                                 ; 09F7 _ C1. C0, 0F
        mov     dword ptr [rsp+0CCH], ecx               ; 09FA _ 89. 8C 24, 000000CC
        mov     edx, dword ptr [rsp+0A8H]               ; 0A01 _ 8B. 94 24, 000000A8
        lea     eax, [rax+rax*4]                        ; 0A08 _ 8D. 04 80
        movd    ecx, xmm5                               ; 0A0B _ 66: 0F 7E. E9
        xor     eax, dword ptr [rbp]                    ; 0A0F _ 33. 45, 00
        not     edx                                     ; 0A12 _ F7. D2
        xor     ecx, dword ptr [rsp+0C4H]               ; 0A14 _ 33. 8C 24, 000000C4
        and     edx, dword ptr [rsp+0B4H]               ; 0A1B _ 23. 94 24, 000000B4
        xor     eax, dword ptr [rbp+80H]                ; 0A22 _ 33. 85, 00000080
        xor     edx, ecx                                ; 0A28 _ 31. CA
        mov     ecx, dword ptr [rsp+90H]                ; 0A2A _ 8B. 8C 24, 00000090
        lea     eax, [rax+rax*2]                        ; 0A31 _ 8D. 04 40
        not     ecx                                     ; 0A34 _ F7. D1
        xor     eax, edx                                ; 0A36 _ 31. D0
        rol     ecx, 1                                  ; 0A38 _ D1. C1
        mov     dword ptr [rbp], eax                    ; 0A3A _ 89. 45, 00
        xor     ecx, eax                                ; 0A3D _ 31. C1
        rol     eax, 15                                 ; 0A3F _ C1. C0, 0F
        mov     dword ptr [rsp+0D0H], ecx               ; 0A42 _ 89. 8C 24, 000000D0
        pshufd  xmm0, xmm5, 01H                         ; 0A49 _ 66: 0F 70. C5, 01
        mov     edx, dword ptr [rsp+0ACH]               ; 0A4E _ 8B. 94 24, 000000AC
        lea     eax, [rax+rax*4]                        ; 0A55 _ 8D. 04 80
        movd    ecx, xmm0                               ; 0A58 _ 66: 0F 7E. C1
        xor     eax, dword ptr [rbp+4H]                 ; 0A5C _ 33. 45, 04
        not     edx                                     ; 0A5F _ F7. D2
        xor     ecx, dword ptr [rsp+0C8H]               ; 0A61 _ 33. 8C 24, 000000C8
        and     edx, dword ptr [rsp+0B8H]               ; 0A68 _ 23. 94 24, 000000B8
        xor     eax, dword ptr [rbp+7CH]                ; 0A6F _ 33. 45, 7C
        xor     edx, ecx                                ; 0A72 _ 31. CA
        mov     ecx, dword ptr [rsp+94H]                ; 0A74 _ 8B. 8C 24, 00000094
        lea     eax, [rax+rax*2]                        ; 0A7B _ 8D. 04 40
        not     ecx                                     ; 0A7E _ F7. D1
        xor     eax, edx                                ; 0A80 _ 31. D0
        rol     ecx, 1                                  ; 0A82 _ D1. C1
        mov     dword ptr [rbp+4H], eax                 ; 0A84 _ 89. 45, 04
        xor     ecx, eax                                ; 0A87 _ 31. C1
        rol     eax, 15                                 ; 0A89 _ C1. C0, 0F
        mov     dword ptr [rsp+0D4H], ecx               ; 0A8C _ 89. 8C 24, 000000D4
        pshufd  xmm0, xmm5, 02H                         ; 0A93 _ 66: 0F 70. C5, 02
        mov     edx, dword ptr [rsp+0B0H]               ; 0A98 _ 8B. 94 24, 000000B0
        lea     eax, [rax+rax*4]                        ; 0A9F _ 8D. 04 80
        movd    ecx, xmm0                               ; 0AA2 _ 66: 0F 7E. C1
        xor     eax, dword ptr [rbp+8H]                 ; 0AA6 _ 33. 45, 08
        not     edx                                     ; 0AA9 _ F7. D2
        xor     ecx, dword ptr [rsp+0CCH]               ; 0AAB _ 33. 8C 24, 000000CC
        and     edx, dword ptr [rsp+0BCH]               ; 0AB2 _ 23. 94 24, 000000BC
        xor     eax, dword ptr [rbp+78H]                ; 0AB9 _ 33. 45, 78
        xor     edx, ecx                                ; 0ABC _ 31. CA
        mov     ecx, dword ptr [rsp+98H]                ; 0ABE _ 8B. 8C 24, 00000098
        lea     eax, [rax+rax*2]                        ; 0AC5 _ 8D. 04 40
        not     ecx                                     ; 0AC8 _ F7. D1
        xor     eax, edx                                ; 0ACA _ 31. D0
        rol     ecx, 1                                  ; 0ACC _ D1. C1
        mov     dword ptr [rbp+8H], eax                 ; 0ACE _ 89. 45, 08
        xor     ecx, eax                                ; 0AD1 _ 31. C1
        rol     eax, 15                                 ; 0AD3 _ C1. C0, 0F
        mov     dword ptr [rsp+0D8H], ecx               ; 0AD6 _ 89. 8C 24, 000000D8
        pshufd  xmm0, xmm5, 03H                         ; 0ADD _ 66: 0F 70. C5, 03
        mov     edx, dword ptr [rsp+0B4H]               ; 0AE2 _ 8B. 94 24, 000000B4
        lea     eax, [rax+rax*4]                        ; 0AE9 _ 8D. 04 80
        movd    ecx, xmm0                               ; 0AEC _ 66: 0F 7E. C1
        xor     eax, dword ptr [rbp+0CH]                ; 0AF0 _ 33. 45, 0C
        not     edx                                     ; 0AF3 _ F7. D2
        xor     ecx, dword ptr [rsp+0D0H]               ; 0AF5 _ 33. 8C 24, 000000D0
        and     edx, dword ptr [rsp+0C0H]               ; 0AFC _ 23. 94 24, 000000C0
        xor     eax, dword ptr [rbp+74H]                ; 0B03 _ 33. 45, 74
        xor     edx, ecx                                ; 0B06 _ 31. CA
        mov     ecx, dword ptr [rsp+9CH]                ; 0B08 _ 8B. 8C 24, 0000009C
        lea     eax, [rax+rax*2]                        ; 0B0F _ 8D. 04 40
        not     ecx                                     ; 0B12 _ F7. D1
        xor     eax, edx                                ; 0B14 _ 31. D0
        rol     ecx, 1                                  ; 0B16 _ D1. C1
        mov     dword ptr [rbp+0CH], eax                ; 0B18 _ 89. 45, 0C
        xor     ecx, eax                                ; 0B1B _ 31. C1
        rol     eax, 15                                 ; 0B1D _ C1. C0, 0F
        mov     dword ptr [rsp+0DCH], ecx               ; 0B20 _ 89. 8C 24, 000000DC
        mov     edx, dword ptr [rsp+0B8H]               ; 0B27 _ 8B. 94 24, 000000B8
        lea     eax, [rax+rax*4]                        ; 0B2E _ 8D. 04 80
        movd    ecx, xmm6                               ; 0B31 _ 66: 0F 7E. F1
        xor     eax, dword ptr [rbp+10H]                ; 0B35 _ 33. 45, 10
        not     edx                                     ; 0B38 _ F7. D2
        xor     ecx, dword ptr [rsp+0D4H]               ; 0B3A _ 33. 8C 24, 000000D4
        and     edx, dword ptr [rsp+0C4H]               ; 0B41 _ 23. 94 24, 000000C4
        xor     eax, dword ptr [rbp+70H]                ; 0B48 _ 33. 45, 70
        xor     edx, ecx                                ; 0B4B _ 31. CA
        mov     ecx, dword ptr [rsp+0A0H]               ; 0B4D _ 8B. 8C 24, 000000A0
        lea     eax, [rax+rax*2]                        ; 0B54 _ 8D. 04 40
        not     ecx                                     ; 0B57 _ F7. D1
        xor     eax, edx                                ; 0B59 _ 31. D0
        rol     ecx, 1                                  ; 0B5B _ D1. C1
        mov     dword ptr [rbp+10H], eax                ; 0B5D _ 89. 45, 10
        xor     ecx, eax                                ; 0B60 _ 31. C1
        rol     eax, 15                                 ; 0B62 _ C1. C0, 0F
        mov     dword ptr [rsp+0E0H], ecx               ; 0B65 _ 89. 8C 24, 000000E0
        pshufd  xmm0, xmm6, 01H                         ; 0B6C _ 66: 0F 70. C6, 01
        mov     edx, dword ptr [rsp+0BCH]               ; 0B71 _ 8B. 94 24, 000000BC
        lea     eax, [rax+rax*4]                        ; 0B78 _ 8D. 04 80
        movd    ecx, xmm0                               ; 0B7B _ 66: 0F 7E. C1
        xor     eax, dword ptr [rbp+14H]                ; 0B7F _ 33. 45, 14
        not     edx                                     ; 0B82 _ F7. D2
        xor     ecx, dword ptr [rsp+0D8H]               ; 0B84 _ 33. 8C 24, 000000D8
        and     edx, dword ptr [rsp+0C8H]               ; 0B8B _ 23. 94 24, 000000C8
        xor     eax, dword ptr [rbp+0ACH]               ; 0B92 _ 33. 85, 000000AC
        xor     edx, ecx                                ; 0B98 _ 31. CA
        mov     ecx, dword ptr [rsp+0A4H]               ; 0B9A _ 8B. 8C 24, 000000A4
        lea     eax, [rax+rax*2]                        ; 0BA1 _ 8D. 04 40
        not     ecx                                     ; 0BA4 _ F7. D1
        xor     eax, edx                                ; 0BA6 _ 31. D0
        rol     ecx, 1                                  ; 0BA8 _ D1. C1
        mov     dword ptr [rbp+14H], eax                ; 0BAA _ 89. 45, 14
        xor     ecx, eax                                ; 0BAD _ 31. C1
        rol     eax, 15                                 ; 0BAF _ C1. C0, 0F
        mov     dword ptr [rsp+0E4H], ecx               ; 0BB2 _ 89. 8C 24, 000000E4
        pshufd  xmm0, xmm6, 02H                         ; 0BB9 _ 66: 0F 70. C6, 02
        mov     edx, dword ptr [rsp+0C0H]               ; 0BBE _ 8B. 94 24, 000000C0
        lea     eax, [rax+rax*4]                        ; 0BC5 _ 8D. 04 80
        movd    ecx, xmm0                               ; 0BC8 _ 66: 0F 7E. C1
        xor     eax, dword ptr [rbp+18H]                ; 0BCC _ 33. 45, 18
        not     edx                                     ; 0BCF _ F7. D2
        xor     ecx, dword ptr [rsp+0DCH]               ; 0BD1 _ 33. 8C 24, 000000DC
        and     edx, dword ptr [rsp+0CCH]               ; 0BD8 _ 23. 94 24, 000000CC
        xor     eax, dword ptr [rbp+0A8H]               ; 0BDF _ 33. 85, 000000A8
        xor     edx, ecx                                ; 0BE5 _ 31. CA
        mov     ecx, dword ptr [rsp+0A8H]               ; 0BE7 _ 8B. 8C 24, 000000A8
        lea     eax, [rax+rax*2]                        ; 0BEE _ 8D. 04 40
        not     ecx                                     ; 0BF1 _ F7. D1
        xor     eax, edx                                ; 0BF3 _ 31. D0
        rol     ecx, 1                                  ; 0BF5 _ D1. C1
        mov     dword ptr [rbp+18H], eax                ; 0BF7 _ 89. 45, 18
        xor     ecx, eax                                ; 0BFA _ 31. C1
        rol     eax, 15                                 ; 0BFC _ C1. C0, 0F
        mov     dword ptr [rsp+0E8H], ecx               ; 0BFF _ 89. 8C 24, 000000E8
        pshufd  xmm0, xmm6, 03H                         ; 0C06 _ 66: 0F 70. C6, 03
        mov     edx, dword ptr [rsp+0C4H]               ; 0C0B _ 8B. 94 24, 000000C4
        lea     eax, [rax+rax*4]                        ; 0C12 _ 8D. 04 80
        movd    ecx, xmm0                               ; 0C15 _ 66: 0F 7E. C1
        xor     eax, dword ptr [rbp+1CH]                ; 0C19 _ 33. 45, 1C
        not     edx                                     ; 0C1C _ F7. D2
        xor     ecx, dword ptr [rsp+0E0H]               ; 0C1E _ 33. 8C 24, 000000E0
        and     edx, dword ptr [rsp+0D0H]               ; 0C25 _ 23. 94 24, 000000D0
        xor     eax, dword ptr [rbp+0A4H]               ; 0C2C _ 33. 85, 000000A4
        xor     edx, ecx                                ; 0C32 _ 31. CA
        mov     ecx, dword ptr [rsp+0ACH]               ; 0C34 _ 8B. 8C 24, 000000AC
        lea     eax, [rax+rax*2]                        ; 0C3B _ 8D. 04 40
        not     ecx                                     ; 0C3E _ F7. D1
        xor     eax, edx                                ; 0C40 _ 31. D0
        rol     ecx, 1                                  ; 0C42 _ D1. C1
        mov     dword ptr [rbp+1CH], eax                ; 0C44 _ 89. 45, 1C
        xor     ecx, eax                                ; 0C47 _ 31. C1
        rol     eax, 15                                 ; 0C49 _ C1. C0, 0F
        mov     dword ptr [rsp+0ECH], ecx               ; 0C4C _ 89. 8C 24, 000000EC
        mov     edx, dword ptr [rsp+0C8H]               ; 0C53 _ 8B. 94 24, 000000C8
        lea     eax, [rax+rax*4]                        ; 0C5A _ 8D. 04 80
        movd    ecx, xmm7                               ; 0C5D _ 66: 0F 7E. F9
        xor     eax, dword ptr [rbp+20H]                ; 0C61 _ 33. 45, 20
        not     edx                                     ; 0C64 _ F7. D2
        xor     ecx, dword ptr [rsp+0E4H]               ; 0C66 _ 33. 8C 24, 000000E4
        and     edx, dword ptr [rsp+0D4H]               ; 0C6D _ 23. 94 24, 000000D4
        xor     eax, dword ptr [rbp+0A0H]               ; 0C74 _ 33. 85, 000000A0
        xor     edx, ecx                                ; 0C7A _ 31. CA
        mov     ecx, dword ptr [rsp+0B0H]               ; 0C7C _ 8B. 8C 24, 000000B0
        lea     eax, [rax+rax*2]                        ; 0C83 _ 8D. 04 40
        not     ecx                                     ; 0C86 _ F7. D1
        xor     eax, edx                                ; 0C88 _ 31. D0
        rol     ecx, 1                                  ; 0C8A _ D1. C1
        mov     dword ptr [rbp+20H], eax                ; 0C8C _ 89. 45, 20
        xor     ecx, eax                                ; 0C8F _ 31. C1
        rol     eax, 15                                 ; 0C91 _ C1. C0, 0F
        mov     dword ptr [rsp+0F0H], ecx               ; 0C94 _ 89. 8C 24, 000000F0
        pshufd  xmm0, xmm7, 01H                         ; 0C9B _ 66: 0F 70. C7, 01
        mov     edx, dword ptr [rsp+0CCH]               ; 0CA0 _ 8B. 94 24, 000000CC
        lea     eax, [rax+rax*4]                        ; 0CA7 _ 8D. 04 80
        movd    ecx, xmm0                               ; 0CAA _ 66: 0F 7E. C1
        xor     eax, dword ptr [rbp+24H]                ; 0CAE _ 33. 45, 24
        not     edx                                     ; 0CB1 _ F7. D2
        xor     ecx, dword ptr [rsp+0E8H]               ; 0CB3 _ 33. 8C 24, 000000E8
        and     edx, dword ptr [rsp+0D8H]               ; 0CBA _ 23. 94 24, 000000D8
        xor     eax, dword ptr [rbp+9CH]                ; 0CC1 _ 33. 85, 0000009C
        xor     edx, ecx                                ; 0CC7 _ 31. CA
        mov     ecx, dword ptr [rsp+0B4H]               ; 0CC9 _ 8B. 8C 24, 000000B4
        lea     eax, [rax+rax*2]                        ; 0CD0 _ 8D. 04 40
        not     ecx                                     ; 0CD3 _ F7. D1
        xor     eax, edx                                ; 0CD5 _ 31. D0
        rol     ecx, 1                                  ; 0CD7 _ D1. C1
        mov     dword ptr [rbp+24H], eax                ; 0CD9 _ 89. 45, 24
        xor     ecx, eax                                ; 0CDC _ 31. C1
        rol     eax, 15                                 ; 0CDE _ C1. C0, 0F
        mov     dword ptr [rsp+0F4H], ecx               ; 0CE1 _ 89. 8C 24, 000000F4
        pshufd  xmm0, xmm7, 02H                         ; 0CE8 _ 66: 0F 70. C7, 02
        mov     edx, dword ptr [rsp+0D0H]               ; 0CED _ 8B. 94 24, 000000D0
        lea     eax, [rax+rax*4]                        ; 0CF4 _ 8D. 04 80
        movd    ecx, xmm0                               ; 0CF7 _ 66: 0F 7E. C1
        xor     eax, dword ptr [rbp+28H]                ; 0CFB _ 33. 45, 28
        not     edx                                     ; 0CFE _ F7. D2
        xor     ecx, dword ptr [rsp+0ECH]               ; 0D00 _ 33. 8C 24, 000000EC
        and     edx, dword ptr [rsp+0DCH]               ; 0D07 _ 23. 94 24, 000000DC
        xor     eax, dword ptr [rbp+98H]                ; 0D0E _ 33. 85, 00000098
        xor     edx, ecx                                ; 0D14 _ 31. CA
        mov     ecx, dword ptr [rsp+0B8H]               ; 0D16 _ 8B. 8C 24, 000000B8
        lea     eax, [rax+rax*2]                        ; 0D1D _ 8D. 04 40
        not     ecx                                     ; 0D20 _ F7. D1
        xor     eax, edx                                ; 0D22 _ 31. D0
        rol     ecx, 1                                  ; 0D24 _ D1. C1
        mov     dword ptr [rbp+28H], eax                ; 0D26 _ 89. 45, 28
        xor     ecx, eax                                ; 0D29 _ 31. C1
        rol     eax, 15                                 ; 0D2B _ C1. C0, 0F
        mov     dword ptr [rsp+0F8H], ecx               ; 0D2E _ 89. 8C 24, 000000F8
        pshufd  xmm0, xmm7, 03H                         ; 0D35 _ 66: 0F 70. C7, 03
        mov     edx, dword ptr [rsp+0D4H]               ; 0D3A _ 8B. 94 24, 000000D4
        lea     eax, [rax+rax*4]                        ; 0D41 _ 8D. 04 80
        movd    ecx, xmm0                               ; 0D44 _ 66: 0F 7E. C1
        xor     eax, dword ptr [rbp+2CH]                ; 0D48 _ 33. 45, 2C
        not     edx                                     ; 0D4B _ F7. D2
        xor     ecx, dword ptr [rsp+0F0H]               ; 0D4D _ 33. 8C 24, 000000F0
        and     edx, dword ptr [rsp+0E0H]               ; 0D54 _ 23. 94 24, 000000E0
        xor     eax, dword ptr [rbp+94H]                ; 0D5B _ 33. 85, 00000094
        xor     edx, ecx                                ; 0D61 _ 31. CA
        mov     ecx, dword ptr [rsp+0BCH]               ; 0D63 _ 8B. 8C 24, 000000BC
        lea     eax, [rax+rax*2]                        ; 0D6A _ 8D. 04 40
        not     ecx                                     ; 0D6D _ F7. D1
        xor     eax, edx                                ; 0D6F _ 31. D0
        rol     ecx, 1                                  ; 0D71 _ D1. C1
        mov     dword ptr [rbp+2CH], eax                ; 0D73 _ 89. 45, 2C
        xor     ecx, eax                                ; 0D76 _ 31. C1
        mov     dword ptr [rsp+0FCH], ecx               ; 0D78 _ 89. 8C 24, 000000FC
        movdqa  xmm0, xmmword ptr [rbp+70H]             ; 0D7F _ 66: 0F 6F. 45, 70
        movdqa  xmm1, xmmword ptr [rbp+80H]             ; 0D84 _ 66: 0F 6F. 8D, 00000080
        movdqa  xmm2, xmmword ptr [rbp+90H]             ; 0D8C _ 66: 0F 6F. 95, 00000090
        movdqa  xmm3, xmmword ptr [rbp+0A0H]            ; 0D94 _ 66: 0F 6F. 9D, 000000A0
        psubd   xmm0, xmm4                              ; 0D9C _ 66: 0F FA. C4
        psubd   xmm1, xmm5                              ; 0DA0 _ 66: 0F FA. CD
        psubd   xmm2, xmm6                              ; 0DA4 _ 66: 0F FA. D6
        psubd   xmm3, xmm7                              ; 0DA8 _ 66: 0F FA. DF
        movdqa  xmmword ptr [rbp+30H], xmm0             ; 0DAC _ 66: 0F 7F. 45, 30
        movdqa  xmmword ptr [rbp+40H], xmm1             ; 0DB1 _ 66: 0F 7F. 4D, 40
        movdqa  xmmword ptr [rbp+50H], xmm2             ; 0DB6 _ 66: 0F 7F. 55, 50
        movdqa  xmmword ptr [rbp+60H], xmm3             ; 0DBB _ 66: 0F 7F. 5D, 60
        movdqa  xmm0, xmmword ptr [rbp+70H]             ; 0DC0 _ 66: 0F 6F. 45, 70
        movdqu  xmm1, xmmword ptr [rbp+7CH]             ; 0DC5 _ F3: 0F 6F. 4D, 7C
        movdqu  xmm2, xmmword ptr [rbp+8CH]             ; 0DCA _ F3: 0F 6F. 95, 0000008C
        pslldq  xmm0, 4                                 ; 0DD2 _ 66: 0F 73. F8, 04
        mov     eax, dword ptr [rbp+0ACH]               ; 0DD7 _ 8B. 85, 000000AC
        movdqu  xmm3, xmmword ptr [rbp+9CH]             ; 0DDD _ F3: 0F 6F. 9D, 0000009C
        movdqa  xmm4, xmmword ptr [rbp]                 ; 0DE5 _ 66: 0F 6F. 65, 00
        movd    xmm7, eax                               ; 0DEA _ 66: 0F 6E. F8
        movdqa  xmm5, xmmword ptr [rbp+10H]             ; 0DEE _ 66: 0F 6F. 6D, 10
        movdqa  xmm6, xmmword ptr [rbp+20H]             ; 0DF3 _ 66: 0F 6F. 75, 20
        por     xmm0, xmm7                              ; 0DF8 _ 66: 0F EB. C7
        paddd   xmm5, xmm1                              ; 0DFC _ 66: 0F FE. E9
        paddd   xmm1, xmm3                              ; 0E00 _ 66: 0F FE. CB
        paddd   xmm5, xmm2                              ; 0E04 _ 66: 0F FE. EA
        paddd   xmm6, xmm2                              ; 0E08 _ 66: 0F FE. F2
        paddd   xmm4, xmm1                              ; 0E0C _ 66: 0F FE. E1
        paddd   xmm5, xmm0                              ; 0E10 _ 66: 0F FE. E8
        paddd   xmm6, xmm1                              ; 0E14 _ 66: 0F FE. F1
        paddd   xmm4, xmm0                              ; 0E18 _ 66: 0F FE. E0
        movdqa  xmmword ptr [rbp+10H], xmm5             ; 0E1C _ 66: 0F 7F. 6D, 10
        movdqa  xmmword ptr [rbp+20H], xmm6             ; 0E21 _ 66: 0F 7F. 75, 20
        movdqa  xmmword ptr [rbp], xmm4                 ; 0E26 _ 66: 0F 7F. 65, 00
        movdqa  xmm0, xmmword ptr [rsp+0C0H]            ; 0E2B _ 66: 0F 6F. 84 24, 000000C0
        movdqa  xmm1, xmmword ptr [rsp+0D0H]            ; 0E34 _ 66: 0F 6F. 8C 24, 000000D0
        movdqa  xmm2, xmmword ptr [rsp+0E0H]            ; 0E3D _ 66: 0F 6F. 94 24, 000000E0
        movdqa  xmm3, xmmword ptr [rsp+0F0H]            ; 0E46 _ 66: 0F 6F. 9C 24, 000000F0
        movdqa  xmmword ptr [rbp+70H], xmm0             ; 0E4F _ 66: 0F 7F. 45, 70
        movdqa  xmmword ptr [rbp+80H], xmm1             ; 0E54 _ 66: 0F 7F. 8D, 00000080
        movdqa  xmmword ptr [rbp+90H], xmm2             ; 0E5C _ 66: 0F 7F. 95, 00000090
        movdqa  xmmword ptr [rbp+0A0H], xmm3            ; 0E64 _ 66: 0F 7F. 9D, 000000A0
        inc     qword ptr [rbp+0B0H]                    ; 0E6C _ 48: FF. 85, 000000B0
        add     rsi, 64                                 ; 0E73 _ 48: 83. C6, 40
        dec     rdi                                     ; 0E77 _ 48: FF. CF
        jne     Lm0                                     ; 0E7A _ 0F 85, FFFFF1A8
        mov     rax, qword ptr [rsp+100H]               ; 0E80 _ 48: 8B. 84 24, 00000100
        mov     rsp, rax                                ; 0E88 _ 48: 89. C4
        pop     rbp                                     ; 0E8B _ 5D
        ret                                             ; 0E8C _ C3

asm_shabal_init PROC

  ; Make space for 10 xmm registers, 2 G.P. registers, 
  ; and align stack by 16
  sub     rsp, 184;   10*16 + 2*8 + 8
        
  ; Register rsi, rdi and xmm6 - xmm15 have callee-save status
  ; in Windows, but not in Unix:
  mov     [rsp],     rsi
  mov     [rsp+8],   rdi
  movaps  [rsp+10h], xmm6
  movaps  [rsp+20h], xmm7
  movaps  [rsp+30h], xmm8
  movaps  [rsp+40h], xmm9
  movaps  [rsp+50h], xmm10
  movaps  [rsp+60h], xmm11
  movaps  [rsp+70h], xmm12
  movaps  [rsp+80h], xmm13
  movaps  [rsp+90h], xmm14
  movaps  [rsp+0A0h],xmm15

  ; Windows parameters (rcx,rdx,r8,r9) -> Unix parameters (rdi,rsi,rdx,rcx)
  mov     rdi, rcx
  mov     rsi, rdx
  mov     rdx, r8
  mov     rcx, r9    


        mov     eax, esi                                ; 0E8D _ 89. F0
        call    Li1                                     ; 0E8F _ E8, 00000000

Li1     LABEL NEAR
        pop     rsi                                     ; 0E94 _ 5E
; Note: Immediate operand could be made smaller by sign extension
        ;add     rsi, -116                               ; 0E95 _ 48: 81. C6, FFFFFF8C

  ; Obtain address of 'iv' in a position-independent way.
  add    rsi, iv - Li1 - 176

        mov     dword ptr [rdi+100H], eax               ; 0E9C _ 89. 87, 00000100
        shr     eax, 5                                  ; 0EA2 _ C1. E8, 05
        imul    eax, eax, 176                           ; 0EA5 _ 69. C0, 000000B0
        add     rsi, rax                                ; 0EAB _ 48: 01. C6
        add     rdi, 72                                 ; 0EAE _ 48: 83. C7, 48
        mov     ecx, 22                                 ; 0EB2 _ B9, 00000016
        rep movsq                                       ; 0EB7 _ F3 48: A5
        mov     eax, 1                                  ; 0EBA _ B8, 00000001
        stosq                                           ; 0EBF _ 48: AB
        sub     rdi, 192                                ; 0EC1 _ 48: 81. EF, 000000C0
        xor     eax, eax                                ; 0EC8 _ 31. C0
        stosq                                           ; 0ECA _ 48: AB


  ; Restore saved registers
  mov     rsi,   [rsp]
  mov     rdi,   [rsp+8]
  movaps  xmm6,  [rsp+10h]
  movaps  xmm7,  [rsp+20h] 
  movaps  xmm8,  [rsp+30h] 
  movaps  xmm9,  [rsp+40h] 
  movaps  xmm10, [rsp+50h] 
  movaps  xmm11, [rsp+60h] 
  movaps  xmm12, [rsp+70h] 
  movaps  xmm13, [rsp+80h] 
  movaps  xmm14, [rsp+90h] 
  movaps  xmm15, [rsp+0A0h]        
        
  ; restore stack pointer
  add     rsp, 184

        ret                                             ; 0ECC _ C3
asm_shabal_init ENDP

; Filling space: 3H
; Filler type: Multi-byte NOP
;       db 0FH, 1FH, 00H

ALIGN   8

iv      label byte
        db 24H, 16H, 99H, 30H, 0D1H, 13H, 68H, 80H      ; 0ED0 _ $..0..h.
        db 0CBH, 62H, 0A6H, 0BCH, 3EH, 9EH, 2BH, 0D1H   ; 0ED8 _ .b..>.+.
        db 0E0H, 94H, 5CH, 01H, 9DH, 4CH, 86H, 90H      ; 0EE0 _ ..\..L..
        db 28H, 76H, 0E8H, 8EH, 70H, 66H, 0B1H, 0C9H    ; 0EE8 _ (v..pf..
        db 6BH, 43H, 00H, 63H, 72H, 43H, 0B7H, 00H      ; 0EF0 _ kC.crC..
        db 2CH, 0F2H, 10H, 64H, 0A8H, 0CCH, 7CH, 72H    ; 0EF8 _ ,..d..|r
        db 0CAH, 62H, 57H, 40H, 0DDH, 0A2H, 0AFH, 0ADH  ; 0F00 _ .bW@....
        db 0D4H, 20H, 4AH, 09H, 0EH, 0DBH, 99H, 02H     ; 0F08 _ . J.....
        db 24H, 70H, 69H, 8EH, 29H, 14H, 30H, 80H       ; 0F10 _ $pi.).0.
        db 12H, 3BH, 0D4H, 9FH, 51H, 98H, 42H, 43H      ; 0F18 _ .;..Q.BC
        db 0E7H, 9CH, 0CEH, 0B6H, 0E8H, 01H, 7EH, 0FDH  ; 0F20 _ ......~.
        db 32H, 88H, 0C0H, 05H, 0F9H, 9BH, 0BH, 1BH     ; 0F28 _ 2.......
        db 0BEH, 39H, 31H, 20H, 78H, 1AH, 94H, 0DFH     ; 0F30 _ .91 x...
        db 03H, 0DCH, 28H, 90H, 88H, 9EH, 07H, 0D5H     ; 0F38 _ ..(.....
        db 34H, 0B1H, 16H, 0DCH, 0FH, 2CH, 90H, 1DH     ; 0F40 _ 4....,..
        db 1CH, 0DEH, 0ABH, 7FH, 56H, 0CH, 0D0H, 3BH    ; 0F48 _ ....V..;
        db 0DAH, 30H, 61H, 2CH, 0F4H, 0F5H, 0ACH, 0DDH  ; 0F50 _ .0a,....
        db 82H, 99H, 0C8H, 5FH, 0D5H, 0F1H, 92H, 99H    ; 0F58 _ ..._....
        db 15H, 0C2H, 6EH, 0A2H, 2CH, 0FDH, 0DFH, 82H   ; 0F60 _ ..n.,...
        db 0C1H, 5AH, 1FH, 0CAH, 35H, 0D5H, 05H, 30H    ; 0F68 _ .Z..5..0
        db 86H, 51H, 84H, 4FH, 86H, 81H, 4EH, 62H       ; 0F70 _ .Q.O..Nb
        db 93H, 0AAH, 8EH, 7EH, 3EH, 81H, 0C5H, 0CH     ; 0F78 _ ...~>...
        db 74H, 1DH, 0BAH, 0AFH, 28H, 36H, 0D9H, 43H    ; 0F80 _ t...(6.C
        db 0E5H, 41H, 67H, 0EDH, 0ADH, 0B5H, 0E6H, 24H  ; 0F88 _ .Ag....$
        db 5BH, 19H, 0F8H, 7CH, 4BH, 59H, 48H, 13H      ; 0F90 _ [..|KYH.
        db 0BBH, 67H, 1DH, 0BH, 45H, 09H, 0CEH, 8EH     ; 0F98 _ .g..E...
        db 0FAH, 7DH, 0A5H, 27H, 8CH, 8AH, 0C2H, 0A3H   ; 0FA0 _ .}.'....
        db 3EH, 8CH, 10H, 88H, 0AFH, 0C0H, 48H, 4EH     ; 0FA8 _ >.....HN
        db 6EH, 11H, 81H, 0C5H, 7FH, 2DH, 0EDH, 16H     ; 0FB0 _ n....-..
        db 8FH, 0D2H, 0A2H, 0F5H, 9DH, 55H, 6DH, 0C5H   ; 0FB8 _ .....Um.
        db 92H, 8EH, 0A0H, 0BAH, 0FCH, 0C8H, 0D7H, 64H  ; 0FC0 _ .......d
        db 0C9H, 0AH, 69H, 3EH, 53H, 99H, 0B6H, 38H     ; 0FC8 _ ..i>S..8
        db 05H, 0CDH, 0DCH, 04H, 9AH, 35H, 0C9H, 0D1H   ; 0FD0 _ .....5..
        db 0FAH, 45H, 85H, 6AH, 4EH, 0B3H, 5BH, 51H     ; 0FD8 _ .E.jN.[Q
        db 68H, 0C8H, 0FAH, 81H, 0E5H, 0C1H, 40H, 0EBH  ; 0FE0 _ h.....@.
        db 9CH, 62H, 95H, 6DH, 32H, 0F6H, 0C0H, 45H     ; 0FE8 _ .b.m2..E
        db 0D2H, 5EH, 10H, 02H, 0FFH, 9AH, 0A6H, 0BCH   ; 0FF0 _ .^......
        db 5CH, 0E4H, 02H, 74H, 70H, 0E3H, 3AH, 3BH     ; 0FF8 _ \..tp.:;
        db 0A4H, 4FH, 0A0H, 08H, 1DH, 0D2H, 6CH, 5AH    ; 1000 _ .O....lZ
        db 0C0H, 0B4H, 2EH, 46H, 6CH, 96H, 0C6H, 8CH    ; 1008 _ ...Fl...
        db 0B9H, 69H, 05H, 50H, 56H, 07H, 58H, 5BH      ; 1010 _ .i.PV.X[
        db 0A0H, 0BEH, 23H, 8AH, 0F0H, 95H, 0DH, 0CH    ; 1018 _ ..#.....
        db 5EH, 0CCH, 0A2H, 0A8H, 47H, 02H, 0F9H, 9DH   ; 1020 _ ^...G...
        db 0FH, 5BH, 3FH, 0E3H, 8EH, 4CH, 0A3H, 4CH     ; 1028 _ .[?..L.L
        db 0AEH, 0B7H, 63H, 9DH, 0D2H, 2AH, 0D3H, 30H   ; 1030 _ ..c..*.0
        db 0F8H, 64H, 2DH, 0FDH, 0EDH, 88H, 4FH, 1AH    ; 1038 _ .d-...O.
        db 0EH, 1EH, 8CH, 9FH, 3AH, 0ACH, 40H, 5BH      ; 1040 _ ....:.@[
        db 8AH, 37H, 49H, 0BCH, 0B7H, 0CEH, 0C4H, 78H   ; 1048 _ .7I....x
        db 12H, 04H, 46H, 4DH, 6FH, 60H, 8FH, 36H       ; 1050 _ ..FMo`.6
        db 32H, 71H, 3CH, 20H, 29H, 53H, 77H, 07H       ; 1058 _ 2q< )Sw.
        db 53H, 0D1H, 7DH, 2AH, 0A3H, 4DH, 43H, 9EH     ; 1060 _ S.}*.MC.
        db 4AH, 02H, 86H, 69H, 0C6H, 20H, 06H, 16H      ; 1068 _ J..i. ..
        db 63H, 0DAH, 0DH, 6AH, 9DH, 0AH, 30H, 0CH      ; 1070 _ c..j..0.
        db 82H, 0CH, 8DH, 92H, 6CH, 0A4H, 0AEH, 0A2H    ; 1078 _ ....l...
        db 85H, 66H, 02H, 5BH, 86H, 1BH, 38H, 9DH       ; 1080 _ .f.[..8.
        db 23H, 0F2H, 2DH, 85H, 47H, 0FBH, 6AH, 33H     ; 1088 _ #.-.G.j3
        db 47H, 0B1H, 0E8H, 0FCH, 05H, 78H, 0D7H, 0F7H  ; 1090 _ G....x..
        db 88H, 40H, 0FDH, 24H, 0F4H, 77H, 0A0H, 0DCH   ; 1098 _ .@.$.w..
        db 9EH, 0AFH, 0B9H, 48H, 90H, 6CH, 0DFH, 0ABH   ; 10A0 _ ...H.l..
        db 0D5H, 5EH, 0CDH, 0DH, 06H, 62H, 0CBH, 0EEH   ; 10A8 _ .^...b..
        db 65H, 0A3H, 12H, 6DH, 66H, 7CH, 0F8H, 98H     ; 10B0 _ e..mf|..
        db 8DH, 7EH, 0C8H, 0CDH, 48H, 0A7H, 0B4H, 0B5H  ; 10B8 _ .~..H...
        db 62H, 0C1H, 4AH, 70H, 4BH, 0BDH, 1CH, 9FH     ; 10C0 _ b.JpK...
        db 0FFH, 02H, 19H, 0E6H, 0EAH, 72H, 0BBH, 0B1H  ; 10C8 _ .....r..
        db 0AH, 0E7H, 70H, 29H, 0A6H, 0B6H, 0F5H, 0CEH  ; 10D0 _ ..p)....
        db 0CBH, 55H, 0DFH, 14H, 97H, 3FH, 94H, 20H     ; 10D8 _ .U...?. 
        db 0B9H, 1FH, 0B3H, 78H, 98H, 20H, 75H, 0B6H    ; 10E0 _ ...x. u.
        db 4FH, 5EH, 0AH, 7EH, 76H, 95H, 90H, 0B9H      ; 10E8 _ O^.~v...
        db 11H, 0EAH, 9FH, 0D2H, 0B0H, 0C1H, 01H, 50H   ; 10F0 _ .......P
        db 0EBH, 61H, 30H, 17H, 0FH, 16H, 59H, 62H      ; 10F8 _ .a0...Yb
        db 36H, 4AH, 85H, 0BAH, 13H, 5EH, 33H, 6BH      ; 1100 _ 6J...^3k
        db 5CH, 0CFH, 0DBH, 0B6H, 37H, 80H, 2DH, 1DH    ; 1108 _ \...7.-.
        db 0BBH, 0AFH, 0FBH, 1DH, 7FH, 0D1H, 0A1H, 0DAH ; 1110 _ ........
        db 8FH, 2DH, 43H, 11H, 88H, 69H, 0FEH, 22H      ; 1118 _ .-C..i."
        db 0B8H, 5BH, 8AH, 43H, 0EAH, 0A3H, 7AH, 01H    ; 1120 _ .[.C..z.
        db 3CH, 0F5H, 20H, 0DCH, 59H, 1DH, 2CH, 0A3H    ; 1128 _ <. .Y.,.
        db 99H, 3AH, 0EBH, 0F4H, 9DH, 30H, 6BH, 9FH     ; 1130 _ .:...0k.
        db 0D1H, 0F4H, 5AH, 47H, 9EH, 70H, 0AEH, 04H    ; 1138 _ ..ZG.p..
        db 0B6H, 0FEH, 0E7H, 6FH, 95H, 21H, 0F9H, 88H   ; 1140 _ ...o.!..
        db 0EFH, 47H, 0DDH, 80H, 0DEH, 7FH, 24H, 0ECH   ; 1148 _ .G....$.
        db 20H, 0C9H, 0E7H, 0D1H, 8DH, 3BH, 10H, 0AEH   ; 1150 _  ....;..
        db 6BH, 0ADH, 0C6H, 98H, 42H, 4CH, 0B4H, 66H    ; 1158 _ k...BL.f
        db 0FFH, 0ADH, 0ACH, 26H, 0AH, 77H, 0CEH, 17H   ; 1160 _ ...&.w..
        db 0AH, 61H, 52H, 0ECH, 91H, 9CH, 0A2H, 8EH     ; 1168 _ .aR.....
        db 0B5H, 9EH, 0DAH, 9AH, 0A2H, 0B2H, 0EAH, 68H  ; 1170 _ .......h
        db 0CH, 0FFH, 7CH, 0DH, 0D5H, 10H, 45H, 0F3H    ; 1178 _ ..|...E.
        db 47H, 0C7H, 0A1H, 0CH, 59H, 0CBH, 0F5H, 0AFH  ; 1180 _ G...Y...
        db 0C8H, 3DH, 0BDH, 0EDH, 0BAH, 0FEH, 0E3H, 35H ; 1188 _ .=.....5
        db 95H, 3DH, 04H, 23H, 80H, 0E8H, 0F7H, 0ADH    ; 1190 _ .=.#....
        db 0A7H, 89H, 0A2H, 0F5H, 21H, 12H, 0CEH, 08H   ; 1198 _ ....!...
        db 58H, 31H, 03H, 0D8H, 42H, 43H, 5DH, 65H      ; 11A0 _ X1..BC]e
        db 0BCH, 00H, 0F1H, 0F8H, 81H, 19H, 0B9H, 0AEH  ; 11A8 _ ........
        db 37H, 6AH, 2FH, 2AH, 0E8H, 2EH, 53H, 0AFH     ; 11B0 _ 7j/*..S.
        db 5FH, 0E8H, 37H, 36H, 0B0H, 1DH, 0FFH, 26H    ; 11B8 _ _.76...&
        db 3BH, 0E3H, 04H, 9CH, 5FH, 0A4H, 1EH, 00H     ; 11C0 _ ;..._...
        db 80H, 0ADH, 0F4H, 0BFH, 0DBH, 0FAH, 0EH, 0BEH ; 11C8 _ ........
        db 99H, 16H, 4EH, 3FH, 0FFH, 15H, 0C0H, 4AH     ; 11D0 _ ..N?...J
        db 91H, 49H, 94H, 0CEH, 0A4H, 86H, 7CH, 65H     ; 11D8 _ .I....|e
        db 82H, 70H, 23H, 16H, 32H, 63H, 0E6H, 69H      ; 11E0 _ .p#.2c.i
        db 70H, 61H, 1BH, 08H, 0EFH, 0BCH, 6FH, 61H     ; 11E8 _ pa....oa
        db 9AH, 0ABH, 62H, 0AAH, 8FH, 0D7H, 22H, 0D7H   ; 11F0 _ ..b...".
        db 24H, 06H, 0CAH, 74H, 41H, 83H, 0CAH, 0A6H    ; 11F8 _ $..tA...
        db 30H, 92H, 0FDH, 9FH, 96H, 91H, 0A5H, 29H     ; 1200 _ 0......)
        db 73H, 55H, 3EH, 53H, 63H, 99H, 0DFH, 0D4H     ; 1208 _ sU>Sc...
        db 0EH, 44H, 71H, 61H, 0CFH, 85H, 0CCH, 0EH     ; 1210 _ .Dqa....
        db 9CH, 95H, 2BH, 0CBH, 9FH, 4AH, 65H, 89H      ; 1218 _ ..+..Je.
        db 01H, 0C1H, 0A1H, 4CH, 6CH, 32H, 5CH, 0B5H    ; 1220 _ ...Ll2\.
        db 4CH, 61H, 0D7H, 0F6H, 0CH, 0EDH, 0EH, 38H    ; 1228 _ La.....8
        db 0B0H, 69H, 0ACH, 51H, 25H, 0E9H, 99H, 61H    ; 1230 _ .i.Q%..a
        db 0F6H, 12H, 0ABH, 0CAH, 89H, 0B5H, 3AH, 1BH   ; 1238 _ ......:.
        db 0D4H, 9EH, 74H, 0FDH, 30H, 0E5H, 98H, 0B7H   ; 1240 _ ..t.0...
        db 6FH, 4BH, 90H, 33H, 5EH, 0A8H, 0BDH, 46H     ; 1248 _ oK.3^..F
        db 0B4H, 34H, 69H, 07H, 58H, 40H, 4BH, 45H      ; 1250 _ .4i.X@KE
        db 27H, 45H, 0F7H, 77H, 65H, 0F4H, 4CH, 0FBH    ; 1258 _ 'E.we.L.
        db 0A9H, 1DH, 93H, 62H, 0DBH, 0C8H, 78H, 0E7H   ; 1260 _ ...b..x.
        db 8EH, 99H, 0B3H, 22H, 0B9H, 0CFH, 15H, 0ACH   ; 1268 _ ..."....
        db 0C4H, 0BAH, 0BCH, 58H, 8EH, 0A0H, 47H, 0ECH  ; 1270 _ ...X..G.
        db 0B2H, 33H, 0E9H, 0AEH, 24H, 0C8H, 0CBH, 0DFH ; 1278 _ .3..$...
        db 04H, 48H, 94H, 0A7H, 0B0H, 0BDH, 65H, 0BFH   ; 1280 _ .H....e.
        db 02H, 45H, 9DH, 5AH, 0F7H, 9AH, 97H, 59H      ; 1288 _ .E.Z...Y
        db 4EH, 0A5H, 0CEH, 0C5H, 50H, 81H, 6BH, 4BH    ; 1290 _ N...P.kK
        db 09H, 19H, 0E7H, 16H, 19H, 23H, 63H, 7DH      ; 1298 _ .....#c}
        db 0A0H, 73H, 05H, 93H, 0D1H, 63H, 4CH, 0F3H    ; 12A0 _ .s...cL.
        db 0B4H, 14H, 0F9H, 0CAH, 2CH, 61H, 0D6H, 0FDH  ; 12A8 _ ....,a..
        db 78H, 08H, 55H, 61H, 75H, 2BH, 0EFH, 89H      ; 12B0 _ x.Uau+..
        db 46H, 0CH, 66H, 0A1H, 5BH, 85H, 0F3H, 7EH     ; 12B8 _ F.f.[..~
        db 8CH, 0B5H, 97H, 72H, 93H, 77H, 0C6H, 1BH     ; 12C0 _ ...r.w..
        db 23H, 0C7H, 0B1H, 7FH, 40H, 0C6H, 6FH, 0B6H   ; 12C8 _ #...@.o.
        db 1CH, 0B7H, 48H, 1AH, 17H, 6DH, 97H, 0F0H     ; 12D0 _ ..H..m..
        db 0AH, 0E8H, 8CH, 08H, 0F3H, 0EDH, 54H, 0A4H   ; 12D8 _ ......T.
        db 0F4H, 6BH, 09H, 1CH, 4BH, 22H, 76H, 0ACH     ; 12E0 _ .k..K"v.
        db 1CH, 78H, 15H, 52H, 69H, 26H, 5DH, 0CDH      ; 12E8 _ .x.Ri&].
        db 67H, 14H, 20H, 0A5H, 4AH, 0D9H, 0B8H, 0A9H   ; 12F0 _ g. .J...
        db 97H, 0D9H, 0CEH, 0D4H, 7BH, 9DH, 37H, 68H    ; 12F8 _ ....{.7h
        db 0BAH, 73H, 0FCH, 0A7H, 6BH, 54H, 0A2H, 0F1H  ; 1300 _ .s..kT..
        db 0BFH, 82H, 67H, 60H, 0FH, 0FDH, 0BCH, 0E0H   ; 1308 _ ..g`....
        db 4EH, 37H, 25H, 2FH, 9FH, 14H, 9AH, 06H       ; 1310 _ N7%/....
        db 25H, 0FFH, 2DH, 5EH, 61H, 0F0H, 0ECH, 0FAH   ; 1318 _ %.-^a...
        db 0D8H, 05H, 99H, 0ECH, 0CFH, 50H, 18H, 0F2H   ; 1320 _ .....P..
        db 0C8H, 46H, 0A7H, 0C0H, 98H, 0D4H, 0DAH, 21H  ; 1328 _ .F.....!
        db 0EBH, 6EH, 15H, 35H, 0F2H, 97H, 8CH, 08H     ; 1330 _ .n.5....
        db 40H, 3EH, 30H, 26H, 0B5H, 4FH, 2DH, 8AH      ; 1338 _ @>0&.O-.
        db 0B6H, 44H, 0EEH, 0FEH, 73H, 95H, 1EH, 8AH    ; 1340 _ .D..s...
        db 1AH, 11H, 81H, 7BH, 0F0H, 39H, 0C1H, 0CBH    ; 1348 _ ...{.9..
        db 61H, 38H, 51H, 0A3H, 2EH, 36H, 2CH, 1DH      ; 1350 _ a8Q..6,.
        db 0EH, 58H, 8CH, 91H, 9CH, 1BH, 8EH, 0B5H      ; 1358 _ .X......
        db 0A1H, 73H, 0B5H, 0E4H, 80H, 08H, 1AH, 4CH    ; 1360 _ .s.....L
        db 51H, 7CH, 90H, 1EH, 0FDH, 7EH, 80H, 04H      ; 1368 _ Q|...~..
        db 0E5H, 0CDH, 0D8H, 3AH, 02H, 13H, 0B2H, 16H   ; 1370 _ ...:....
        db 53H, 2CH, 51H, 02H, 18H, 0CBH, 04H, 22H      ; 1378 _ S,Q...."
        db 2DH, 5FH, 40H, 99H, 0A1H, 48H, 0B6H, 0E5H    ; 1380 _ -_@..H..
        db 43H, 1DH, 0ABH, 70H, 0C2H, 25H, 0CH, 0A1H    ; 1388 _ C..p.%..
        db 05H, 0ACH, 0F1H, 16H, 56H, 0EBH, 0BBH, 38H   ; 1390 _ ....V..8
        db 60H, 0DCH, 01H, 9BH, 83H, 6DH, 09H, 0B1H     ; 1398 _ `....m..
        db 52H, 45H, 0F8H, 52H, 99H, 79H, 4BH, 0E5H     ; 13A0 _ RE.R.yK.
        db 0ECH, 0E3H, 8EH, 2DH, 91H, 51H, 64H, 0B9H    ; 13A8 _ ...-.Qd.
        db 86H, 8BH, 07H, 0E0H, 0C9H, 44H, 7CH, 0BBH    ; 13B0 _ .....D|.
        db 0CAH, 0C1H, 0B5H, 0D2H, 8CH, 0EBH, 0D2H, 0B0H; 13B8 _ ........
        db 45H, 5AH, 0CEH, 14H, 0DCH, 50H, 0AFH, 22H    ; 13C0 _ EZ...P."
        db 6BH, 0BCH, 0FDH, 0EFH, 4AH, 0B7H, 21H, 0EBH  ; 13C8 _ k...J.!.
        db 0EEH, 0C6H, 55H, 0B5H, 96H, 05H, 71H, 3EH    ; 13D0 _ ..U...q>
        db 2FH, 65H, 2AH, 0A7H, 5FH, 51H, 01H, 93H      ; 13D8 _ /e*._Q..
        db 0FAH, 0C1H, 28H, 0DAH, 68H, 0D8H, 6FH, 69H   ; 13E0 _ ..(.h.oi
        db 72H, 0BFH, 0B6H, 9CH, 02H, 40H, 0FEH, 0AH    ; 13E8 _ r....@..
        db 15H, 36H, 0E0H, 0A6H, 0D4H, 0C1H, 38H, 51H   ; 13F0 _ .6....8Q
        db 06H, 63H, 21H, 0BEH, 90H, 88H, 8BH, 0B3H     ; 13F8 _ .c!.....
        db 6BH, 0B9H, 0A8H, 3EH, 0E4H, 0ACH, 99H, 32H   ; 1400 _ k..>...2
        db 0D4H, 4DH, 92H, 30H, 0A5H, 34H, 0CBH, 55H    ; 1408 _ .M.0.4.U
        db 31H, 0F0H, 05H, 0B4H, 0BAH, 3EH, 23H, 0C4H   ; 1410 _ 1....>#.
        db 79H, 39H, 73H, 0B3H, 55H, 9DH, 0DDH, 0C0H    ; 1418 _ y9s.U...
        db 0AEH, 28H, 1CH, 0C5H, 0E1H, 0B8H, 27H, 0A3H  ; 1420 _ .(....'.
        db 67H, 61H, 0C5H, 56H, 33H, 44H, 61H, 0EDH     ; 1428 _ ga.V3Da.
        db 60H, 9DH, 0B5H, 88H, 0BAH, 0CEH, 0E2H, 60H   ; 1430 _ `......`
        db 8BH, 4BH, 8BH, 75H, 7FH, 2AH, 0E8H, 83H      ; 1438 _ .K.u.*..
        db 28H, 88H, 96H, 0BCH, 0F7H, 0BH, 0E0H, 0E6H   ; 1440 _ (.......
        db 55H, 9EH, 83H, 0BAH, 60H, 1CH, 49H, 9BH      ; 1448 _ U...`.I.
        db 11H, 3FH, 65H, 66H, 0DBH, 6EH, 44H, 0F9H     ; 1450 _ .?ef.nD.
        db 0FBH, 68H, 85H, 3EH, 5AH, 0F9H, 19H, 0E5H    ; 1458 _ .h.>Z...
        db 54H, 20H, 73H, 0D6H, 0C5H, 0A0H, 0BDH, 0B2H  ; 1460 _ T s.....
        db 0F8H, 3CH, 2FH, 49H, 96H, 3BH, 0D6H, 8FH     ; 1468 _ .</I.;..
        db 48H, 46H, 0EFH, 0B4H, 0E5H, 88H, 54H, 7EH    ; 1470 _ HF....T~
        db 0AH, 17H, 0EDH, 0A7H, 52H, 0FAH, 25H, 5BH    ; 1478 _ ....R.%[
        db 0A1H, 36H, 0C6H, 0BBH, 0E3H, 0B1H, 7FH, 0B8H ; 1480 _ .6......
        db 32H, 0EDH, 8EH, 0EDH, 0D5H, 0E8H, 10H, 6DH   ; 1488 _ 2......m
        db 38H, 83H, 86H, 34H, 07H, 9AH, 0EBH, 56H      ; 1490 _ 8..4...V
        db 0EDH, 0D6H, 0C8H, 27H, 48H, 0EFH, 83H, 02H   ; 1498 _ ...'H...
        db 49H, 0B3H, 1AH, 6BH, 0BH, 87H, 19H, 26H      ; 14A0 _ I..k...&
        db 6CH, 0A5H, 0D0H, 0DDH, 5BH, 93H, 72H, 7FH    ; 14A8 _ l...[.r.
        db 0C6H, 0A3H, 0B1H, 0D2H, 63H, 55H, 0DAH, 0D7H ; 14B0 _ ....cU..
        db 94H, 2EH, 29H, 58H, 0A4H, 20H, 33H, 36H      ; 14B8 _ ..)X. 36
        db 78H, 43H, 90H, 0B8H, 5FH, 13H, 37H, 0BCH     ; 14C0 _ xC.._.7.
        db 0ECH, 0ECH, 0CFH, 2FH, 52H, 0F0H, 4DH, 0C7H  ; 14C8 _ .../R.M.
        db 0DEH, 01H, 0FH, 0B7H, 09H, 0B8H, 3EH, 52H    ; 14D0 _ ......>R
        db 78H, 4AH, 0E1H, 0D8H, 9DH, 65H, 1AH, 18H     ; 14D8 _ xJ...e..
        db 40H, 4EH, 89H, 90H, 0A0H, 5BH, 86H, 48H      ; 14E0 _ @N...[.H
        db 3FH, 1FH, 2CH, 28H, 0E9H, 88H, 24H, 1CH      ; 14E8 _ ?.,(..$.
        db 0E6H, 0B2H, 41H, 01H, 0FBH, 6EH, 0D0H, 0FFH  ; 14F0 _ ..A..n..
        db 68H, 5AH, 0F1H, 0A1H, 87H, 6AH, 0B0H, 0CFH   ; 14F8 _ hZ...j..
        db 0C8H, 76H, 0FBH, 15H, 7DH, 59H, 0A3H, 8BH    ; 1500 _ .v..}Y..
        db 87H, 0A0H, 66H, 1BH, 5BH, 55H, 0C5H, 65H     ; 1508 _ ..f.[U.e
        db 39H, 0E2H, 62H, 39H, 0C7H, 0C7H, 0A9H, 4EH   ; 1510 _ 9.b9...N
        db 1BH, 67H, 0D9H, 93H, 0BEH, 0B7H, 74H, 30H    ; 1518 _ .g....t0
        db 96H, 0EDH, 24H, 0A2H, 8BH, 0A8H, 0A8H, 38H   ; 1520 _ ..$....8
        db 0EAH, 5FH, 51H, 9AH, 0D4H, 0DBH, 5AH, 12H    ; 1528 _ ._Q...Z.
        db 0D1H, 67H, 0EH, 0F3H, 0E4H, 79H, 06H, 0CDH   ; 1530 _ .g...y..
        db 5FH, 3DH, 37H, 0FH, 4DH, 0F7H, 0CEH, 1FH     ; 1538 _ _=7.M...
        db 0FBH, 0F3H, 80H, 5CH, 31H, 06H, 0C7H, 0A6H   ; 1540 _ ...\1...
        db 69H, 6AH, 0CDH, 0EH, 8FH, 5CH, 88H, 73H      ; 1548 _ ij...\.s
        db 69H, 0FEH, 77H, 1BH, 5AH, 0D2H, 87H, 0D4H    ; 1550 _ i.w.Z...
        db 0D1H, 0EEH, 27H, 0C0H, 8FH, 0BDH, 31H, 3FH   ; 1558 _ ..'...1?
        db 14H, 0F7H, 75H, 0E2H, 41H, 74H, 0D0H, 92H    ; 1560 _ ..u.At..
        db 10H, 0FFH, 70H, 5BH, 60H, 0E1H, 7AH, 17H     ; 1568 _ ..p[`.z.
        db 0FBH, 0A5H, 0CFH, 6FH, 22H, 93H, 13H, 67H    ; 1570 _ ...o"..g
        db 0B2H, 09H, 31H, 2DH, 8CH, 08H, 1AH, 60H      ; 1578 _ ..1-...`
        db 0A7H, 86H, 33H, 29H, 0ABH, 84H, 75H, 0F8H    ; 1580 _ ..3)..u.
        db 83H, 2BH, 86H, 00H, 04H, 63H, 0EFH, 87H      ; 1588 _ .+...c..
        db 36H, 0A4H, 9DH, 0F6H, 22H, 80H, 0A8H, 6AH    ; 1590 _ 6..."..j
        db 75H, 0ECH, 0D1H, 09H, 45H, 0B0H, 99H, 80H    ; 1598 _ u...E...
        db 0DEH, 0B2H, 0F8H, 59H, 0A8H, 56H, 34H, 34H   ; 15A0 _ ...Y.V44
        db 0AEH, 6DH, 0F6H, 91H, 0EDH, 30H, 1DH, 0AFH   ; 15A8 _ .m...0..
        db 3AH, 65H, 0D5H, 1BH, 68H, 94H, 0D7H, 63H     ; 15B0 _ :e..h..c
        db 0FAH, 66H, 51H, 0AEH, 0CCH, 0E9H, 0A0H, 3FH  ; 15B8 _ .fQ....?
        db 6EH, 29H, 78H, 35H, 81H, 28H, 0AAH, 0FFH     ; 15C0 _ n)x5.(..
        db 0EDH, 89H, 7EH, 0A0H, 0CAH, 90H, 88H, 0F3H   ; 15C8 _ ..~.....
        db 9EH, 1AH, 11H, 09H, 00H, 0F1H, 08H, 0F9H     ; 15D0 _ ........
        db 4DH, 44H, 76H, 84H, 0F8H, 0E1H, 33H, 0FEH    ; 15D8 _ MDv...3.
        db 89H, 8CH, 22H, 5BH, 10H, 0F7H, 82H, 19H      ; 15E0 _ .."[....
        db 3EH, 0EEH, 51H, 0CH, 0EEH, 68H, 6CH, 0CCH    ; 15E8 _ >.Q..hl.
        db 34H, 0BFH, 29H, 56H, 9FH, 4FH, 0EAH, 0CDH    ; 15F0 _ 4.)V.O..
        db 0CAH, 3EH, 3CH, 20H, 0AH, 0E4H, 52H, 0E3H    ; 15F8 _ .>< ..R.
        db 80H, 0F3H, 0C6H, 3FH, 2DH, 0DAH, 0E1H, 35H   ; 1600 _ ...?-..5
        db 0DBH, 0A9H, 22H, 85H, 0AEH, 66H, 0DBH, 0B1H  ; 1608 _ .."..f..
        db 0A7H, 02H, 0C0H, 07H, 9BH, 0CAH, 95H, 0E4H   ; 1610 _ ........
        db 89H, 1DH, 8BH, 8DH, 0FCH, 0DDH, 48H, 18H     ; 1618 _ ......H.
        db 26H, 7DH, 5EH, 2BH, 7DH, 0F0H, 25H, 1FH      ; 1620 _ &}^+}.%.
        db 78H, 63H, 9EH, 9BH, 0A7H, 0DDH, 0C9H, 8DH    ; 1628 _ xc......
        db 9DH, 1AH, 22H, 44H, 0CBH, 59H, 3AH, 19H      ; 1630 _ .."D.Y:.
        db 55H, 0CAH, 7AH, 0BBH, 0ABH, 25H, 23H, 43H    ; 1638 _ U.z..%#C
        db 4EH, 4BH, 0CEH, 15H, 0DEH, 0B4H, 0D2H, 6EH   ; 1640 _ NK.....n
        db 9FH, 1AH, 26H, 42H, 14H, 0DCH, 0CAH, 8DH     ; 1648 _ ..&B....
        db 8EH, 0BCH, 49H, 0EBH, 0C0H, 0AEH, 66H, 0F5H  ; 1650 _ ..I...f.
        db 31H, 0D4H, 56H, 8AH, 27H, 03H, 6BH, 21H      ; 1658 _ 1.V.'.k!
        db 31H, 0A3H, 0FCH, 0C8H, 4EH, 50H, 5CH, 0E5H   ; 1660 _ 1...NP\.
        db 26H, 0BFH, 3EH, 00H, 83H, 8DH, 6BH, 0BBH     ; 1668 _ &.>...k.
        db 0C1H, 48H, 04H, 7BH, 89H, 27H, 0B8H, 41H     ; 1670 _ .H.{.'.A
        db 01H, 96H, 7CH, 0AH, 0FFH, 9CH, 65H, 8DH      ; 1678 _ ..|...e.
        db 3EH, 67H, 0E2H, 0B6H, 7BH, 0C7H, 54H, 0CAH   ; 1680 _ >g..{.T.
        db 7EH, 0FDH, 60H, 14H, 2DH, 8FH, 0CBH, 3FH     ; 1688 _ ~.`.-..?
        db 0FCH, 91H, 72H, 52H, 5FH, 45H, 16H, 2AH      ; 1690 _ ..rR_E.*
        db 0E5H, 27H, 0E6H, 78H, 9FH, 16H, 4FH, 94H     ; 1698 _ .'.x..O.
        db 16H, 0F0H, 0A6H, 1CH, 25H, 0EAH, 54H, 0A8H   ; 16A0 _ ....%.T.
        db 0BEH, 8AH, 0B9H, 8DH, 41H, 26H, 0C6H, 0F2H   ; 16A8 _ ....A&..
        db 0CBH, 7DH, 11H, 30H, 09H, 43H, 5CH, 0CFH     ; 16B0 _ .}.0.C\.
        db 25H, 1AH, 71H, 93H, 0B8H, 71H, 0F6H, 0F9H    ; 16B8 _ %.q..q..
        db 16H, 21H, 1DH, 0B0H, 89H, 4BH, 3FH, 33H      ; 16C0 _ .!...K?3
        db 65H, 0D1H, 85H, 0B2H, 36H, 9BH, 82H, 86H     ; 16C8 _ e...6...
        db 1AH, 0B1H, 64H, 0F7H, 46H, 21H, 17H, 76H     ; 16D0 _ ..d.F!.v
        db 4DH, 93H, 0F6H, 0CEH, 99H, 83H, 0D2H, 0C6H   ; 16D8 _ M.......
        db 61H, 5FH, 09H, 0FEH, 0B4H, 18H, 60H, 5EH     ; 16E0 _ a_....`^
        db 0F5H, 0ECH, 48H, 50H, 61H, 32H, 35H, 51H     ; 16E8 _ ..HPa25Q
        db 0DCH, 36H, 6EH, 6EH, 0ADH, 0DH, 13H, 63H     ; 16F0 _ .6nn...c
        db 0D6H, 9BH, 0C6H, 0A9H, 0CH, 0EAH, 90H, 1EH   ; 16F8 _ ........
        db 3BH, 07H, 35H, 7CH, 6DH, 5EH, 0D9H, 28H      ; 1700 _ ;.5|m^.(
        db 0DH, 0EH, 34H, 0AAH, 70H, 0EEH, 3DH, 0CBH    ; 1708 _ ..4.p.=.
        db 0F3H, 0FH, 2EH, 35H, 45H, 0C0H, 0C9H, 6AH    ; 1710 _ ...5E..j
        db 9CH, 0EFH, 0B4H, 2DH, 0BDH, 2AH, 0FAH, 77H   ; 1718 _ ...-.*.w
        db 70H, 0DDH, 8FH, 33H, 0FFH, 4EH, 53H, 50H     ; 1720 _ p..3.NSP
        db 0D6H, 0EAH, 4AH, 0FCH, 0FH, 0E9H, 0D5H, 7FH  ; 1728 _ ..J.....
        db 64H, 0A0H, 27H, 0D4H, 00H, 0EAH, 0EFH, 0AEH  ; 1730 _ d.'.....
        db 68H, 44H, 0FEH, 0B2H, 75H, 5DH, 86H, 0E0H    ; 1738 _ hD..u]..
        db 0EFH, 0A3H, 0A3H, 0FBH, 26H, 34H, 6DH, 42H   ; 1740 _ ....&4mB
        db 71H, 0ADH, 0B9H, 0C3H, 71H, 87H, 0BBH, 75H   ; 1748 _ q...q..u
        db 90H, 7DH, 1FH, 64H, 1EH, 0C6H, 0D1H, 0A4H    ; 1750 _ .}.d....
        db 8CH, 07H, 0B1H, 50H, 17H, 0AH, 0D1H, 0DFH    ; 1758 _ ...P....
        db 00H, 03H, 0F0H, 0CBH, 03H, 1EH, 93H, 84H     ; 1760 _ ........
        db 0BBH, 5FH, 78H, 0B3H, 72H, 8CH, 0FEH, 0B2H   ; 1768 _ ._x.r...
        db 0E0H, 0FFH, 0D8H, 0FBH, 0B2H, 69H, 80H, 66H  ; 1770 _ .....i.f
        db 6CH, 0E2H, 89H, 93H, 38H, 55H, 8AH, 0A4H     ; 1778 _ l...8U..
        db 0ADH, 0B4H, 0FEH, 0F4H, 0D4H, 4AH, 79H, 53H  ; 1780 _ .....JyS
        db 68H, 67H, 64H, 0C7H, 26H, 7EH, 0CAH, 95H     ; 1788 _ hgd.&~..
        db 5DH, 17H, 26H, 48H, 0B5H, 62H, 18H, 0FAH     ; 1790 _ ].&H.b..
        db 3CH, 0D6H, 4FH, 3DH, 0B3H, 0D0H, 0D1H, 18H   ; 1798 _ <.O=....
        db 0F6H, 51H, 0FH, 46H, 70H, 0FDH, 0B0H, 0B6H   ; 17A0 _ .Q.Fp...
        db 30H, 0B5H, 95H, 0EBH, 0C3H, 8BH, 0B3H, 36H   ; 17A8 _ 0......6
        db 0DDH, 0C1H, 2EH, 71H, 02H, 82H, 35H, 0F1H    ; 17B0 _ ...q..5.
        db 51H, 7EH, 4BH, 1CH, 0D6H, 6BH, 0B6H, 0F3H    ; 17B8 _ Q~K..k..
        db 15H, 46H, 13H, 1DH, 76H, 83H, 0BH, 0CEH      ; 17C0 _ .F..v...
        db 44H, 59H, 0A3H, 92H, 66H, 64H, 0D2H, 35H     ; 17C8 _ DY..fd.5
        db 0DEH, 0AEH, 0B7H, 1DH, 0CH, 36H, 3CH, 0DDH   ; 17D0 _ .....6<.
        db 0E3H, 0BH, 5DH, 0AFH, 91H, 3EH, 3AH, 5AH     ; 17D8 _ ..]..>:Z
        db 0CDH, 34H, 0CH, 9FH, 0D4H, 0D9H, 20H, 0F5H   ; 17E0 _ .4.... .
        db 0C5H, 8EH, 0A4H, 0DDH, 12H, 0E6H, 3AH, 4DH   ; 17E8 _ ......:M
        db 0A4H, 98H, 56H, 29H, 3DH, 15H, 40H, 92H      ; 17F0 _ ..V)=.@.
        db 78H, 9BH, 38H, 0D6H, 0B0H, 0CDH, 0CAH, 0C5H  ; 17F8 _ x.8.....
        db 90H, 0FDH, 0BBH, 2FH, 4AH, 0A7H, 75H, 9FH    ; 1800 _ .../J.u.
        db 0ACH, 0ABH, 0DFH, 74H, 0CDH, 78H, 98H, 0E7H  ; 1808 _ ...t.x..
        db 21H, 0B4H, 1DH, 74H, 18H, 0D8H, 0B2H, 0F4H   ; 1810 _ !..t....
        db 0D1H, 4AH, 53H, 0A0H, 95H, 48H, 0DBH, 21H    ; 1818 _ .JS..H.!
        db 7DH, 6DH, 70H, 06H, 0E5H, 9AH, 0E4H, 0D4H    ; 1820 _ }mp.....
        db 9FH, 0A1H, 0CEH, 55H, 2CH, 0F7H, 0EEH, 0B2H  ; 1828 _ ...U,...
        db 0BAH, 0DEH, 3BH, 60H, 89H, 98H, 0E6H, 0F3H   ; 1830 _ ..;`....
        db 07H, 16H, 0F8H, 0A2H, 58H, 0FEH, 0ECH, 0A7H  ; 1838 _ ....X...
        db 0A7H, 01H, 84H, 32H, 25H, 61H, 05H, 34H      ; 1840 _ ...2%a.4
        db 5FH, 0BH, 5FH, 27H, 10H, 61H, 09H, 73H       ; 1848 _ _._'.a.s
        db 48H, 75H, 0B1H, 0D1H, 5BH, 0F9H, 85H, 62H    ; 1850 _ Hu..[..b
        db 12H, 7CH, 84H, 0C6H, 4DH, 0CCH, 0B2H, 0B1H   ; 1858 _ .|..M...
        db 46H, 4CH, 9AH, 0E3H, 43H, 0BAH, 6CH, 66H     ; 1860 _ FL..C.lf
        db 0DDH, 0FEH, 67H, 0BFH, 0DH, 92H, 6AH, 6EH    ; 1868 _ ..g...jn
        db 0A7H, 27H, 2DH, 0E1H, 0FCH, 0F4H, 3FH, 04H   ; 1870 _ .'-...?.
        db 67H, 9DH, 1DH, 38H, 88H, 0FDH, 7CH, 0EEH     ; 1878 _ g..8..|.
        db 0F4H, 0D1H, 0FDH, 0FAH, 0F0H, 32H, 0C1H, 3FH ; 1880 _ .....2.?
        db 77H, 37H, 0D2H, 56H, 0FAH, 2BH, 0B0H, 0F1H   ; 1888 _ w7.V.+..
        db 2AH, 80H, 36H, 0FH, 5AH, 0CFH, 56H, 5DH      ; 1890 _ *.6.Z.V]
        db 77H, 7FH, 6FH, 32H, 0C3H, 55H, 0ECH, 15H     ; 1898 _ w.o2.U..
        db 0B8H, 88H, 0BAH, 2DH, 8FH, 0E3H, 28H, 8EH    ; 18A0 _ ...-..(.
        db 2BH, 93H, 15H, 84H, 08H, 99H, 0BCH, 34H      ; 18A8 _ +......4
        db 20H, 0E1H, 7AH, 38H, 0B7H, 5CH, 80H, 38H     ; 18B0 _  .z8.\.8
        db 0D3H, 45H, 0BDH, 0ECH, 83H, 30H, 92H, 09H    ; 18B8 _ .E...0..
        db 3EH, 7CH, 07H, 28H, 0E3H, 47H, 2FH, 0CAH     ; 18C0 _ >|.(.G/.
        db 69H, 28H, 0AAH, 98H, 17H, 70H, 59H, 55H      ; 18C8 _ i(...pYU
        db 5CH, 0DCH, 06H, 97H, 0FAH, 53H, 48H, 0D8H    ; 18D0 _ \....SH.
        db 86H, 0D0H, 0B3H, 64H, 1AH, 5EH, 17H, 2CH     ; 18D8 _ ...d.^.,
        db 0E1H, 58H, 0C6H, 42H, 0ACH, 0A0H, 48H, 19H   ; 18E0 _ .X.B..H.
        db 0AEH, 9CH, 0DDH, 0EEH, 89H, 0CEH, 90H, 0FDH  ; 18E8 _ ........
        db 0C9H, 43H, 0E1H, 0CBH, 79H, 0D7H, 55H, 57H   ; 18F0 _ .C..y.UW
        db 0ECH, 74H, 0C1H, 6DH, 0FDH, 0C5H, 0BDH, 0BBH ; 18F8 _ .t.m....
        db 60H, 87H, 8CH, 0D8H, 0D6H, 0A9H, 52H, 0C0H   ; 1900 _ `.....R.
        db 0EH, 70H, 2DH, 25H, 59H, 59H, 4AH, 08H       ; 1908 _ .p-%YYJ.
        db 63H, 34H, 0DCH, 78H, 2EH, 40H, 0C1H, 69H     ; 1910 _ c4.x.@.i
        db 0CFH, 55H, 50H, 0EEH, 8DH, 9CH, 90H, 5EH     ; 1918 _ .UP....^
        db 0FDH, 8DH, 72H, 20H, 53H, 0BDH, 0C0H, 46H    ; 1920 _ ..r S..F
        db 99H, 0B6H, 82H, 0E7H, 32H, 46H, 30H, 55H     ; 1928 _ ....2F0U
        db 90H, 0EFH, 0B4H, 71H, 2CH, 0E8H, 0A9H, 0EH   ; 1930 _ ...q,...
        db 0F1H, 30H, 0B9H, 0DBH, 8BH, 6BH, 0D0H, 0FAH  ; 1938 _ .0...k..
        db 40H, 0AEH, 0CH, 0BEH, 10H, 44H, 0D1H, 8BH    ; 1940 _ @....D..
        db 0ACH, 0ADH, 0D2H, 76H, 7FH, 0ABH, 0ACH, 28H  ; 1948 _ ...v...(
        db 0B7H, 9CH, 09H, 0C1H, 0F3H, 85H, 0B3H, 07H   ; 1950 _ ........
        db 26H, 2CH, 44H, 0E7H, 40H, 0D6H, 8AH, 0CCH    ; 1958 _ &,D.@...
        db 0C7H, 56H, 6FH, 0EBH, 0A9H, 1AH, 0A8H, 1EH   ; 1960 _ .Vo.....
        db 14H, 0D3H, 0B9H, 73H, 08H, 5DH, 0E8H, 1DH    ; 1968 _ ...s.]..
        db 5AH, 0AH, 91H, 48H, 0DBH, 22H, 3BH, 89H      ; 1970 _ Z..H.";.
        db 44H, 0DFH, 0A0H, 0C5H, 4EH, 32H, 0C4H, 0BBH  ; 1978 _ D...N2..
        db 40H, 0F2H, 0D2H, 72H, 99H, 1DH, 94H, 75H     ; 1980 _ @..r...u
        db 82H, 0DEH, 8BH, 6DH, 2BH, 50H, 0A7H, 0A1H    ; 1988 _ ...m+P..
        db 0D1H, 68H, 0BFH, 0D9H, 50H, 0D7H, 0BAH, 58H  ; 1990 _ .h..P..X
        db 0B2H, 8CH, 02H, 56H, 59H, 0F3H, 34H, 81H     ; 1998 _ ...VY.4.
        db 0D8H, 69H, 0D4H, 0B5H, 0C2H, 8CH, 1AH, 94H   ; 19A0 _ .i......
        db 6EH, 2AH, 8BH, 41H, 80H, 27H, 05H, 04H       ; 19A8 _ n*.A.'..
        db 87H, 0D7H, 07H, 7FH, 8FH, 35H, 94H, 51H      ; 19B0 _ .....5.Q
        db 65H, 0D6H, 60H, 3CH, 9AH, 0D7H, 97H, 0BEH    ; 19B8 _ e.`<....
        db 34H, 34H, 0CH, 95H, 6DH, 0A0H, 0D9H, 0AEH    ; 19C0 _ 44..m...
        db 8DH, 0DCH, 37H, 25H, 69H, 59H, 0DBH, 7CH     ; 19C8 _ ..7%iY.|

reduced_memcpy LABEL NEAR
        push    rcx                                     ; 19D0 _ 51
        shr     rcx, 3                                  ; 19D1 _ 48: C1. E9, 03
        rep movsq                                       ; 19D5 _ F2 48: A5
        pop     rcx                                     ; 19D8 _ 59
        and     ecx, 07H                                ; 19D9 _ 83. E1, 07
        rep movsb                                       ; 19DC _ F2: A4
        ret                                             ; 19DE _ C3

align_structure_enter LABEL NEAR
        push    rax                                     ; 19DF _ 50
        lea     rax, [rbp+74H]                          ; 19E0 _ 48: 8D. 45, 74
        test    eax, 3FH                                ; 19E4 _ A9, 0000003F
        pop     rax                                     ; 19E9 _ 58
        jnz     Lase1                                   ; 19EA _ 75, 05
        lea     rdi, [rbp+48H]                          ; 19EC _ 48: 8D. 7D, 48
        ret                                             ; 19F0 _ C3

Lase1   LABEL NEAR
        push    rdi                                     ; 19F1 _ 57
        push    rsi                                     ; 19F2 _ 56
        push    rcx                                     ; 19F3 _ 51
        lea     rsi, [rbp+48H]                          ; 19F4 _ 48: 8D. 75, 48
        mov     ecx, 23                                 ; 19F8 _ B9, 00000017
        rep movsq                                       ; 19FD _ F3 48: A5
        pop     rcx                                     ; 1A00 _ 59
        pop     rsi                                     ; 1A01 _ 5E
        pop     rdi                                     ; 1A02 _ 5F
        ret                                             ; 1A03 _ C3

align_structure_leave LABEL NEAR
        push    rdi                                     ; 1A04 _ 57
        lea     rdi, [rbp+48H]                          ; 1A05 _ 48: 8D. 7D, 48
        test    rdi, rsi                                ; 1A09 _ 48: 85. F7
        jz      Lasl1                                   ; 1A0C _ 74, 0C
        push    rcx                                     ; 1A0E _ 51
        push    rsi                                     ; 1A0F _ 56
        mov     ecx, 23                                 ; 1A10 _ B9, 00000017
        rep movsq                                       ; 1A15 _ F3 48: A5
        pop     rsi                                     ; 1A18 _ 5E
        pop     rcx                                     ; 1A19 _ 59

Lasl1   LABEL NEAR
        pop     rdi                                     ; 1A1A _ 5F
        ret                                             ; 1A1B _ C3

asm_shabal  PROC
            
  ; Make space for 10 xmm registers, 2 G.P. registers, 
  ; and align stack by 16
  sub     rsp, 184;   10*16 + 2*8 + 8
        
  ; Register rsi, rdi and xmm6 - xmm15 have callee-save status
  ; in Windows, but not in Unix:
  mov     [rsp],     rsi
  mov     [rsp+8],   rdi
  movaps  [rsp+10h], xmm6
  movaps  [rsp+20h], xmm7
  movaps  [rsp+30h], xmm8
  movaps  [rsp+40h], xmm9
  movaps  [rsp+50h], xmm10
  movaps  [rsp+60h], xmm11
  movaps  [rsp+70h], xmm12
  movaps  [rsp+80h], xmm13
  movaps  [rsp+90h], xmm14
  movaps  [rsp+0A0h],xmm15

  ; Windows parameters (rcx,rdx,r8,r9) -> Unix parameters (rdi,rsi,rdx,rcx)
  mov     rdi, rcx
  mov     rsi, rdx
  mov     rdx, r8
  mov     rcx, r9    


        push    rbp                                     ; 1A1C _ 55
        mov     rbp, rdi                                ; 1A1D _ 48: 89. FD
        mov     rax, rsp                                ; 1A20 _ 48: 89. E0
        sub     rsp, 144                                ; 1A23 _ 48: 81. EC, 00000090
        and     rsp, 0FFFFFFFFFFFFFFC0H                 ; 1A2A _ 48: 83. E4, C0
        sub     rsp, 48                                 ; 1A2E _ 48: 83. EC, 30
        mov     qword ptr [rsp+0B8H], rax               ; 1A32 _ 48: 89. 84 24, 000000B8
        mov     ecx, dword ptr [rbp+40H]                ; 1A3A _ 8B. 4D, 40
        mov     eax, 64                                 ; 1A3D _ B8, 00000040
        sub     eax, ecx                                ; 1A42 _ 29. C8
        cmp     rdx, rax                                ; 1A44 _ 48: 39. C2
        jnc     Ls1                                     ; 1A47 _ 73, 14
        lea     rdi, [rbp+rcx]                          ; 1A49 _ 48: 8D. 7C 0D, 00
        mov     ecx, edx                                ; 1A4E _ 89. D1
        call    reduced_memcpy                          ; 1A50 _ E8, FFFFFF7B
        sub     rdi, rbp                                ; 1A55 _ 48: 29. EF
        mov     dword ptr [rbp+40H], edi                ; 1A58 _ 89. 7D, 40
        jmp     Ls9                                     ; 1A5B _ EB, 65

Ls1     LABEL NEAR
        mov     rdi, rsp                                ; 1A5D _ 48: 89. E7
        call    align_structure_enter                   ; 1A60 _ E8, FFFFFF7A
        test    ecx, ecx                                ; 1A65 _ 85. C9
        jz      Ls2                                     ; 1A67 _ 74, 24
        sub     rdx, rax                                ; 1A69 _ 48: 29. C2
        push    rdi                                     ; 1A6C _ 57
        lea     rdi, [rbp+rcx]                          ; 1A6D _ 48: 8D. 7C 0D, 00
        mov     ecx, eax                                ; 1A72 _ 89. C1
        call    reduced_memcpy                          ; 1A74 _ E8, FFFFFF57
        pop     rdi                                     ; 1A79 _ 5F
        push    rdx                                     ; 1A7A _ 52
        push    rsi                                     ; 1A7B _ 56
        push    rdi                                     ; 1A7C _ 57
        mov     edx, 1                                  ; 1A7D _ BA, 00000001
        mov     rsi, rbp                                ; 1A82 _ 48: 89. EE
        call    shabal_inner                            ; 1A85 _ E8, FFFFE576
        pop     rdi                                     ; 1A8A _ 5F
        pop     rsi                                     ; 1A8B _ 5E
        pop     rdx                                     ; 1A8C _ 5A

Ls2     LABEL NEAR
        mov     rax, rdx                                ; 1A8D _ 48: 89. D0
        shr     rax, 6                                  ; 1A90 _ 48: C1. E8, 06
        jz      Ls3                                     ; 1A94 _ 74, 18
        and     edx, 3FH                                ; 1A96 _ 83. E2, 3F
        push    rdx                                     ; 1A99 _ 52
        mov     rdx, rax                                ; 1A9A _ 48: 89. C2
        shl     rax, 6                                  ; 1A9D _ 48: C1. E0, 06
        add     rax, rsi                                ; 1AA1 _ 48: 01. F0
        push    rax                                     ; 1AA4 _ 50
        push    rdi                                     ; 1AA5 _ 57
        call    shabal_inner                            ; 1AA6 _ E8, FFFFE555
        pop     rdi                                     ; 1AAB _ 5F
        pop     rsi                                     ; 1AAC _ 5E
        pop     rdx                                     ; 1AAD _ 5A

Ls3     LABEL NEAR
        push    rdi                                     ; 1AAE _ 57
        mov     rdi, rbp                                ; 1AAF _ 48: 89. EF
        mov     ecx, edx                                ; 1AB2 _ 89. D1
        mov     dword ptr [rbp+40H], edx                ; 1AB4 _ 89. 55, 40
        call    reduced_memcpy                          ; 1AB7 _ E8, FFFFFF14
        pop     rsi                                     ; 1ABC _ 5E
        call    align_structure_leave                   ; 1ABD _ E8, FFFFFF42

Ls9     LABEL NEAR
        mov     rax, qword ptr [rsp+0B8H]               ; 1AC2 _ 48: 8B. 84 24, 000000B8
        mov     rsp, rax                                ; 1ACA _ 48: 89. C4
        pop     rbp                                     ; 1ACD _ 5D


  ; Restore saved registers
  mov     rsi,   [rsp]
  mov     rdi,   [rsp+8]
  movaps  xmm6,  [rsp+10h]
  movaps  xmm7,  [rsp+20h] 
  movaps  xmm8,  [rsp+30h] 
  movaps  xmm9,  [rsp+40h] 
  movaps  xmm10, [rsp+50h] 
  movaps  xmm11, [rsp+60h] 
  movaps  xmm12, [rsp+70h] 
  movaps  xmm13, [rsp+80h] 
  movaps  xmm14, [rsp+90h] 
  movaps  xmm15, [rsp+0A0h]        
        
  ; restore stack pointer
  add     rsp, 184

        ret                                             ; 1ACE _ C3
asm_shabal  ENDP

asm_shabal_close PROC
                 
  ; Make space for 10 xmm registers, 2 G.P. registers, 
  ; and align stack by 16
  sub     rsp, 184;   10*16 + 2*8 + 8
        
  ; Register rsi, rdi and xmm6 - xmm15 have callee-save status
  ; in Windows, but not in Unix:
  mov     [rsp],     rsi
  mov     [rsp+8],   rdi
  movaps  [rsp+10h], xmm6
  movaps  [rsp+20h], xmm7
  movaps  [rsp+30h], xmm8
  movaps  [rsp+40h], xmm9
  movaps  [rsp+50h], xmm10
  movaps  [rsp+60h], xmm11
  movaps  [rsp+70h], xmm12
  movaps  [rsp+80h], xmm13
  movaps  [rsp+90h], xmm14
  movaps  [rsp+0A0h],xmm15

  ; Windows parameters (rcx,rdx,r8,r9) -> Unix parameters (rdi,rsi,rdx,rcx)
  mov     rdi, rcx
  mov     rsi, rdx
  mov     rdx, r8
  mov     rcx, r9    


        push    rbx                                     ; 1ACF _ 53
        push    rbp                                     ; 1AD0 _ 55
        mov     rbp, rdi                                ; 1AD1 _ 48: 89. FD
        push    rcx                                     ; 1AD4 _ 51
        push    rdx                                     ; 1AD5 _ 52
        push    rsi                                     ; 1AD6 _ 56
        pop     rdx                                     ; 1AD7 _ 5A
        pop     rcx                                     ; 1AD8 _ 59
        pop     rsi                                     ; 1AD9 _ 5E
        mov     rax, rsp                                ; 1ADA _ 48: 89. E0
        sub     rsp, 144                                ; 1ADD _ 48: 81. EC, 00000090
        and     rsp, 0FFFFFFFFFFFFFFC0H                 ; 1AE4 _ 48: 83. E4, C0
        sub     rsp, 48                                 ; 1AE8 _ 48: 83. EC, 30
        mov     qword ptr [rsp+0B8H], rax               ; 1AEC _ 48: 89. 84 24, 000000B8
        mov     eax, 128                                ; 1AF4 _ B8, 00000080
        shr     eax, cl                                 ; 1AF9 _ D3. E8
        or      edx, eax                                ; 1AFB _ 09. C2
        neg     eax                                     ; 1AFD _ F7. D8
        and     eax, edx                                ; 1AFF _ 21. D0
        mov     rdi, rbp                                ; 1B01 _ 48: 89. EF
        mov     ecx, dword ptr [rbp+40H]                ; 1B04 _ 8B. 4D, 40
        add     rdi, rcx                                ; 1B07 _ 48: 01. CF
        stosb                                           ; 1B0A _ AA
        neg     ecx                                     ; 1B0B _ F7. D9
        xor     eax, eax                                ; 1B0D _ 31. C0
        add     ecx, 70                                 ; 1B0F _ 83. C1, 46
        shr     ecx, 3                                  ; 1B12 _ C1. E9, 03
        rep stosq                                       ; 1B15 _ F2 48: AB
        mov     rdi, rsp                                ; 1B18 _ 48: 89. E7
        call    align_structure_enter                   ; 1B1B _ E8, FFFFFEBF
        push    rsi                                     ; 1B20 _ 56
        mov     ebx, 4                                  ; 1B21 _ BB, 00000004

Lc1     LABEL NEAR
        mov     edx, 1                                  ; 1B26 _ BA, 00000001
        mov     rsi, rbp                                ; 1B2B _ 48: 89. EE
        push    rdi                                     ; 1B2E _ 57
        call    shabal_inner                            ; 1B2F _ E8, FFFFE4CC
        pop     rdi                                     ; 1B34 _ 5F
        dec     qword ptr [rdi+0B0H]                    ; 1B35 _ 48: FF. 8F, 000000B0
        dec     ebx                                     ; 1B3C _ FF. CB
        jnz     Lc1                                     ; 1B3E _ 75, E6
        mov     eax, dword ptr [rbp+100H]               ; 1B40 _ 8B. 85, 00000100
        mov     ecx, eax                                ; 1B46 _ 89. C1
        shr     eax, 3                                  ; 1B48 _ C1. E8, 03
        shr     ecx, 5                                  ; 1B4B _ C1. E9, 05
        lea     rsi, [rdi+0B0H]                         ; 1B4E _ 48: 8D. B7, 000000B0
        pop     rdi                                     ; 1B55 _ 5F
        sub     rsi, rax                                ; 1B56 _ 48: 29. C6
        rep movsd                                       ; 1B59 _ F3: A5
        mov     rax, qword ptr [rsp+0B8H]               ; 1B5B _ 48: 8B. 84 24, 000000B8
        mov     rsp, rax                                ; 1B63 _ 48: 89. C4
        pop     rbp                                     ; 1B66 _ 5D
        pop     rbx                                     ; 1B67 _ 5B


  ; Restore saved registers
  mov     rsi,   [rsp]
  mov     rdi,   [rsp+8]
  movaps  xmm6,  [rsp+10h]
  movaps  xmm7,  [rsp+20h] 
  movaps  xmm8,  [rsp+30h] 
  movaps  xmm9,  [rsp+40h] 
  movaps  xmm10, [rsp+50h] 
  movaps  xmm11, [rsp+60h] 
  movaps  xmm12, [rsp+70h] 
  movaps  xmm13, [rsp+80h] 
  movaps  xmm14, [rsp+90h] 
  movaps  xmm15, [rsp+0A0h]        
        
  ; restore stack pointer
  add     rsp, 184

        ret                                             ; 1B68 _ C3
asm_shabal_close ENDP

_text   ENDS

END