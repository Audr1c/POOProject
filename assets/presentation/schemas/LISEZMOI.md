# Schémas draw.io du diaporama

Un fichier `.drawio` par schéma. On les ouvre et on les modifie dans draw.io,
puis :

```sh
make schemas   # exporte chaque .drawio modifié en figures/schemas/<nom>.pdf
make           # recompile le diaporama
```

Les diapositives incluent les PDF avec `\schema{<nom>}` (voir `macros.tex`).

## Conventions, pour que les retouches restent homogènes

**Échelle 2.** Les schémas sont dessinés deux fois plus grands, puis inclus à
0,5. Les tailles de police en px dans draw.io donnent donc :

| draw.io | diapositive |
|---|---|
| 16 px | `\tiny` (6 pt) : textes secondaires, étiquettes de flèches |
| 21 px | `\scriptsize` (8 pt) : texte courant des boîtes |
| 24 px | `\footnotesize` (9 pt) : titres des petites boîtes |
| 27 px | `\small` (10 pt) : titres des classes |

Largeur utile d'une diapositive : environ 1 100 px. Hauteur : environ 490 px,
et 400 px sur les diapositives avec le fil rouge des étapes. Un schéma plus grand
est réduit automatiquement, mais ses textes rapetissent.

**Polices.** Le texte est en `TeX Gyre Heros` (le clone d'Helvetica du
diaporama) et le code en `Fira Mono` à 85 %. Si elles ne sont pas installées
sur la machine, draw.io prend Helvetica/Arial et Consolas : c'est très proche.
Elles sont toutes deux dans TeX Live et MiKTeX (`texgyreheros-*.otf`,
`FiraMono-*.otf`), il suffit de les installer comme polices système.

**Formules.** Les schémas avec des maths ont l'option *Mathematical Typesetting*
activée (menu Extras). On écrit `\(m\vec g\)` dans un libellé.

**Couleurs** (bord / fond), les mêmes que `theme/beamercolorthemeressorts.sty` :

| rôle | bord | fond |
|---|---|---|
| classe concrète | `#82B366` | `#D5E8D4` |
| interface, classe abstraite (bord pointillé) | `#336CE6` | `#E0F2FE` |
| `World`, propriétaire | `#DC2626` | `#FEE2E2` |
| scène, fabrique | `#D79B00` | `#FFE6CC` |
| souris | `#9673A6` | `#E1D5E7` |
| neutre | `#23445D` | `#FFFFFF` |
| boîte noire (SFML) | `#AAACAD` | `#E8E8E8` |
| cadre d'étage (pointillé) | `#AAACAD` | `#FBFBFC` |

Accent : rouge Phelma `#BD1D1D` (héritage virtuel, pastilles, mises en garde),
marine `#2A2E47` (flèches de flux), gris texte `#5F6368`.
