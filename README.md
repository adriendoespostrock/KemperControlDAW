# Kemper Control 0.3.6 — interface Kemper Player

Cette version reprend le contrôleur MIDI direct et remplace l’interface stock par
un panneau JUCE inspiré du Kemper Player. Une seule piste Ableton ou Logic peut
porter le plugin, ses automations et la sortie MIDI directe.

## Interface

- En-tête sombre avec indicateur MIDI OUT et édition PLAYER.
- Zone RIG SELECT avec affichage BANK / SLOT, réglage de banque et cinq boutons
  de slot. `Bank` et `Slot` sont des paramètres à choix discrets : l’hôte ne
  propose que les valeurs entières disponibles (1–124 et 1–5).
- Chaîne EFFECT SLOTS : A, B, C, D, X, MOD, DLY et REV.
- Quatre potentiomètres à couronne verte : MORPH, EXP, WAH et GAIN, sans champ
  numérique encombrant sous les boutons.
- Bouton MORPH SWITCH placé à droite dans RIG SELECT, sans chevaucher les slots.
- Section EXTRA COMMANDS placée à droite des pédales, avec les quatre boutons
  **I, II, III et IIII**, reliés aux paramètres `extraI`, `extraII`, `extraIII`
  et `extraIIII` et aux CC 75–78.
- Panneau LOOPER séparé, avec les sept commandes NRPN réparties à l’intérieur
  du cadre.
- Sélecteur de sortie MIDI, canal et réémission de l’état. Les ports MIDI sont
  actualisés automatiquement lorsqu’un périphérique apparaît ou disparaît.

Les IDs de paramètres d’automation sont conservés. Les sessions existantes ne
perdent donc pas leurs enveloppes ; elles bénéficient simplement de la nouvelle
présentation.

## Compilation sur le Mac

Fermer Live, Logic et l’ancienne version, puis exécuter depuis le dossier
`KemperControl` :

```bash
bash build-mac.command "/Users/adriendeurveilher/Desktop/JUCE"
```

Le script utilise le dossier `build-direct-midi` et compile arm64 + x86_64.
Les formats sont copiés dans les dossiers utilisateur standards :

- `~/Library/Audio/Plug-Ins/VST3/Kemper Control.vst3`
- `~/Library/Audio/Plug-Ins/Components/Kemper Control.component`

Le titre de l’éditeur doit afficher **KEMPER CONTROL — PLAYER EDITION**.

## Utilisation dans Ableton

1. Charge le VST3 sur une piste MIDI.
2. Dans **Sortie MIDI**, sélectionne le Kemper, MIDI Monitor ou l’IAC.
3. Sélectionne le canal du Kemper.
4. Utilise **Configure** dans Ableton pour exposer les paramètres que tu veux
   automatiser : Bank, Slot, effets, I–IIII, looper et pédales.
5. Dessine les enveloppes dans la même piste que le plugin.

Le plugin conserve une sortie audio silencieuse pour être accepté par Live. Les
messages MIDI sont envoyés directement au port sélectionné ; aucune deuxième
piste de routage n’est nécessaire.

## Validation effectuée

- Syntaxe C++ vérifiée avec JUCE 8.0.9 en configuration VST3 et AU MIDI FX.
- IDs d’automation et mappings MIDI conservés.
- Les quatre boutons I, II, III et IIII sont effectivement créés et attachés.
- Script Bash vérifié.

La compilation/link finale, l’affichage sur macOS et le test avec le Kemper
estent à effectuer sur le Mac, car ils dépendent de Xcode, de JUCE installé et
des ports MIDI disponibles.


## Correctif 0.3.1

Compatibilité renforcée avec les versions de JUCE qui ne proposent pas
`Font::getStringWidthFloat()`. Le calcul de largeur des titres de section utilise
désormais une largeur fixe adaptée à la typographie de l’interface.

## Correctif 0.3.3

Le bouton MORPH SWITCH a été déplacé dans RIG SELECT. Le bouton de rafraîchissement
manuel et le texte d’état sans retour Kemper ont été retirés pour alléger le pied
de l’interface ; la liste MIDI continue de se mettre à jour automatiquement.

## Correctif 0.3.4

`Bank` et `Slot` utilisent désormais des paramètres JUCE de type choix. Les
automations restent compatibles avec les mêmes IDs et ne génèrent plus de
valeurs intermédiaires entre deux banques ou deux slots.

## Correctif 0.3.5

Le bouton MORPH SWITCH renvoie désormais le CC du slot actuellement sélectionné
au lieu d’envoyer le CC de pédale Morph. Chaque changement du bouton simule ainsi
un nouvel appui sur le même rig et alterne entre l’état de base et l’état Morph.

## Correctif 0.3.6

Ajout d’un bouton ABOUT dans l’en-tête, avec le crédit développeur et un accès
direct à la chaîne YouTube de When Waves Collide.
