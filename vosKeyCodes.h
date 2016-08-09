//ENTER = 0x0A = \n
#ifndef VOSENTERREPLACEMENT
#define VOSENTERREPLACEMENT '\n'
#endif

//ENTER = 0x0D 0 \r
#ifndef VOSENTER
#define VOSENTER '\r'
#endif

#ifndef VOSNOOTPRINTCHAR
#define VOSNOOTPRINTCHAR '·'
#endif

#ifndef CTRL(c)
#define CTRL(c) ((c) & 037)
#endif

#ifndef ALT(c)
#define ALT(c) ((c) & 064)
#endif

#ifndef ESCAPE_KEY
#define ESCAPE_KEY 27
#endif

//SUP = 0x14A = 330
#ifndef VOSKEY_SUP
#define VOSKEY_SUP 330
#endif

//INSERT = 331
#ifndef VOSKEY_INS
#define VOSKEY_INS 331
#endif

//ñ = 0xF1 = 241
#ifndef VOSKEY_enie
#define VOSKEY_enie 241
#endif

//Ñ = 0xF1 = 209
#ifndef VOSKEY_ENIE
#define VOSKEY_ENIE 209
#endif

//á = 225
#ifndef VOSKEY_aTilde
#define VOSKEY_aTilde 225
#endif

//é = 233
#ifndef VOSKEY_eTilde
#define VOSKEY_eTilde 233
#endif

//í = 237
#ifndef VOSKEY_iTilde
#define VOSKEY_iTilde 237
#endif

//ó = 243
#ifndef VOSKEY_oTilde
#define VOSKEY_oTilde 243
#endif

//ú = 250
#ifndef VOSKEY_uTilde
#define VOSKEY_uTilde 250
#endif

int vosIsPrint(int c);
