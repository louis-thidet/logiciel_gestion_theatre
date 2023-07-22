//--------------- Préfixe général ----------------
#define ECHAP_SEQ(sequence)   "\033[" sequence
//------------- Attributs de texte -------------
#define TEXTE_ATTR(x)   ECHAP_SEQ(#x) "m"
// Couleur de TEXTE
#define TEXTE_NOIR          TEXTE_ATTR(30)
#define TEXTE_ROUGE         TEXTE_ATTR(91)
#define TEXTE_ROUGEFONCE    TEXTE_ATTR(31)
#define TEXTE_VERT          TEXTE_ATTR(32)
#define TEXTE_VERTCLAIR     TEXTE_ATTR(92)
#define TEXTE_JAUNE         TEXTE_ATTR(33)
#define TEXTE_JAUNEVIF      TEXTE_ATTR(93)
#define TEXTE_BLEU          TEXTE_ATTR(34)
#define TEXTE_MAGENTA       TEXTE_ATTR(35)
#define TEXTE_CYAN          TEXTE_ATTR(36)
#define TEXTE_CYANCLAIR     TEXTE_ATTR(96)
#define TEXTE_BLANC         TEXTE_ATTR(37)
// Couleur de FOND
#define FOND_NOIR           TEXTE_ATTR(40)
#define FOND_ROUGE          TEXTE_ATTR(41)
#define FOND_VERT           TEXTE_ATTR(42)
#define FOND_JAUNE          TEXTE_ATTR(43)
#define FOND_BLEU           TEXTE_ATTR(44)
#define FOND_MAGENTA        TEXTE_ATTR(45)
#define FOND_CYAN           TEXTE_ATTR(46)
#define FOND_BLANC          TEXTE_ATTR(47)
// Attributs divers
#define CLIGNOTEMENT        TEXTE_ATTR(5)
#define ITALIQUE            TEXTE_ATTR(3)
#define SOULIGNER           TEXTE_ATTR(4)
#define EFFACER_SOULIGNER   TEXTE_ATTR(24)
// Effacement des attributs
#define EFFACER_ATTR        TEXTE_ATTR(0)
