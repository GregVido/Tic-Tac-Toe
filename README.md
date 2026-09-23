<table width="100%">
    <tr>
        <td width="100" align="center">
            <img src="assets/logo.png" alt="logo" width="70%" />
        </td>
        <td width="700" align="center">
            <h1>IA Puissance 4</h1>
        </td>
    </td>
</table>

<div align="center">

[![C++](https://img.shields.io/badge/C%2B%2B-20-00599C?logo=cplusplus&logoColor=white)](https://isocpp.org/)
[![Windows](https://img.shields.io/badge/Windows-10%20%2F%2011-0078D6?logo=windows&logoColor=white)](https://www.microsoft.com/windows)
[![Build](https://github.com/GregVido/Tic-Tac-Toe/actions/workflows/build-release.yml/badge.svg)](https://github.com/GregVido/Tic-Tac-Toe/actions/workflows/build-release.yml)
[![Release](https://img.shields.io/github/v/release/GregVido/Tic-Tac-Toe?label=release)](https://github.com/GregVido/Tic-Tac-Toe/releases/latest)
[![License](https://img.shields.io/badge/License-Apache%202.0-blue.svg)](LICENSE.txt)

</div>

## 🎮 À propos

**Tic-Tac-Toe** est une implémentation native du célèbre Morpion développée en **C++20** avec l'API **Win32**.

Le projet propose une interface graphique moderne ainsi que plusieurs niveaux d'intelligence artificielle utilisant différentes stratégies, allant du simple choix aléatoire jusqu'à un **Minimax avec élagage alpha-bêta**.

Chaque joueur peut être configuré indépendamment comme **Humain** ou **Bot**, permettant plusieurs types de parties :

- 👤 **Humain vs Humain**
- 👤 **Humain vs Bot**
- 🤖 **Bot vs Bot**
- 🤖 Choix indépendant de la difficulté pour chaque bot

## ✨ Fonctionnalités

- Interface graphique native Windows
- Plateau de Tic-Tac-Toe interactif
- Joueur de départ choisi aléatoirement
- Configuration indépendante des deux joueurs
- Trois niveaux d'intelligence artificielle
- Parties Humain vs Humain, Humain vs IA et IA vs IA

## 🤖 Intelligences artificielles

| Difficulté | Algorithme | Description |
|:---:|:---:|---|
| **Facile** | Random | Sélectionne aléatoirement une case disponible. |
| **Moyen** | MCTS | Utilise **Monte Carlo Tree Search** pour simuler différentes parties avant de choisir un coup. |
| **Difficile** | Minimax | Explore les possibilités avec **Minimax + élagage alpha-bêta** afin de jouer de manière optimale. |

### Random

Le niveau facile récupère l'ensemble des cases disponibles puis en choisit une aléatoirement.

Il permet d'obtenir un adversaire rapide, imprévisible et volontairement imparfait.

### Monte Carlo Tree Search

Le niveau moyen utilise une implémentation de **MCTS - Monte Carlo Tree Search**.

L'algorithme fonctionne en quatre étapes :

1. **Sélection** d'une branche prometteuse
2. **Expansion** de l'arbre
3. **Simulation** d'une partie aléatoire
4. **Backpropagation** du résultat

La sélection utilise un compromis entre **exploration** et **exploitation** inspiré de la formule UCT.

### Minimax + Alpha-Beta

Le niveau difficile utilise **Minimax** afin d'analyser récursivement les coups possibles.

L'IA cherche à maximiser son propre score tout en supposant que son adversaire jouera lui aussi de manière optimale.

L'**élagage alpha-bêta** permet d'éviter l'analyse de branches qui ne peuvent plus influencer le résultat final.

Sur un plateau de Tic-Tac-Toe 3×3, cette méthode permet à l'IA de rechercher l'ensemble des positions pertinentes et de jouer de manière optimale.