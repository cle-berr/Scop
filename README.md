# Scop

Mini-projet d'infographie – Visualisation d'un modèle OBJ avec OpenGL.

## Construire et lancer

Prerequis (Linux): OpenGL, GLU, GLEW, GLFW, GLUT headers/libs.

Build:

```sh
make
```

Run:

```sh
./scop <path/to/model.obj> [path/to/texture.png]
```

Exemples:

```sh
./scop resources/bunny.obj
./scop resources/bunny.obj resources/checker.png
```

## Contrôles clavier / souris

- Souris (clic droit maintenu): orientation caméra (yaw/pitch)
- WASD + Espace / Alt: déplacer la caméra (clic droit maintenu) ou l’objet (sans clic)
	- W/S: avant/arrière
	- A/D: gauche/droite
	- Espace: haut
	- Alt gauche: bas
	- Shift: accélérer (caméra et objet)
- R: reset caméra/objet/états (recalcule aussi la distance caméra et la distance orbitale du soleil selon la taille du modèle)
- P: pause/reprise de la rotation automatique de l’objet
- T: activer/désactiver la texture (si chargée en argument)
- F: activer/désactiver la fourure (si ajouter dans le material)
- L: activer/désactiver la lumière ("sun")
	- Flèches ← → ↑ ↓: faire orbiter la lumière autour de l’objet (Ctrl pour accélérer)
- M: activer/désactiver le matériau (si présent dans le .mtl du modèle)
- 1 / 2 / 3: activer/désactiver la rotation autour des axes X / Y / Z

Notes:
- Clic droit enfoncé = mode déplacement caméra. Sans clic droit = déplacement de l’objet.

## Fonctionnalités implémentées

Obligatoires du sujet:

- Affichage d’un objet 3D en perspective: OUI (gluPerspective 45°)
- Chargement/Parsing d’un fichier .obj (sans librairie externe de parsing): OUI
- Rotation automatique de l’objet: OUI (P pour pause). Axes X/Y/Z togglables via 1/2/3.
- Application d’une texture simple sur l’objet via une touche et retour aux couleurs: OUI (touche T)
- Gestion d’une fenêtre et des événements clavier/souris: OUI (GLFW)
- Makefile: OUI

Bonus réalisés (par rapport à l’extrait fourni):

- Lumière dynamique (sun) orbitant autour de l’objet, visualisée par une sphère.
- Matériaux MTL: chargement et application (Ka/Kd/Ks/Ns/opacity)
- Déplacement libre caméra ET déplacement de l’objet
- Auto-positionnement caméra en fonction de la taille du modèle
- Auto-positionnement du soleil: distance orbitale proportionnelle à la taille du modèle
- Effet "fur" (fourrure) via extensions MTL custom (fur_layers, fur_length, fur_opacity, fur_color) et matériau `newmtl fur`

## Notes techniques

- Caméra auto-positionnée en fonction de la taille de l’objet (rayon de la sphère englobante et FOV de 45°)
- Soleil auto-positionné: distance orbitale proportionnelle à la taille de l’objet
- Normales par face calculées côté CPU (gouraud/flat OpenGL fixed pipeline)
- Texture appliquée via GL_MODULATE (fixed pipeline, pas de shaders)
- Le matériau rétablit l’état OpenGL par défaut quand on le désactive pour éviter des effets persistants

### Utiliser l’effet fur

- Dans votre `.mtl`: utilisez le bloc `newmtl fur` (déjà présent dans `resources/metal.mtl`) ou ajoutez
	- `fur_layers <int>` (ex: 30)
	- `fur_length <float>` (ex: 0.005)
	- `fur_opacity <float 0..1>` (ex: 0.4)
	- `fur_color <float 0..1> <float 0..1> <float 0..1>` (ex: 0.1 0.2 0.3)
- Dans votre `.obj`: référencez le mtl (`mtllib ...`) et assignez `usemtl fur` aux faces souhaitées.
- Dans l’appli: pressez `M` pour activer le matériau (la lumière sera automatiquement activée si nécessaire).