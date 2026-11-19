# Expliquer le jeu en entretien

## Suivre un tir dans le code

À la lecture du clavier, Game construit un Input. World le transmet à Player. Si Espace est enfoncée et que le délai de 190 ms est écoulé, Player renvoie un projectile dans un std::optional. World le range dans son vecteur, puis fait avancer tous les tirs d’un pas. Pour un tir allié, WaveManager cherche le premier ennemi touché. World attribue 100 points et retire le projectile. Game lit ensuite les positions pour dessiner l’image. On peut reproduire toute cette séquence en test avec un clavier simulé et une graine fixe.

| Type | Responsabilité et limite |
|---|---|
| Game | Possède la fenêtre, la police et la partie. Traduit les événements en transitions entre menu, options, jeu et fin. Dessine et orchestre les sauvegardes. |
| World | Arbitre un pas de simulation. Possède le joueur, la vague, les projectiles, les vies et le score courant. |
| Player | Contraint son rectangle à la zone jouable, normalise la diagonale et décide quand créer un tir. |
| Enemy | Porte le rectangle d’un ennemi et applique le déplacement décidé collectivement. |
| Projectile | Contient une position et un camp Side. Avance verticalement en pixels par seconde. |
| CollisionSystem | Prédicat géométrique pur. Ne décide ni des dégâts ni des points. |
| WaveManager | Construit les formations, choisit le tireur, fait rebondir et descendre toute la vague, retire les ennemis touchés. |
| ScoreManager | Lit le record, valide son contenu et remplace le fichier si le nouveau score est supérieur. |
| Rect, Input | Petites valeurs copiables. Rect exprime la géométrie, Input le déplacement et la demande de tir pendant un pas. |

## Temps réel et indépendance des FPS

steady_clock mesure une durée monotone. Game ajoute le temps écoulé à un accumulateur. Tant que celui-ci contient au moins 1/120 seconde, World exécute un pas de cette durée. À 60 images/s, une image représente en moyenne deux pas. À 30 images/s, elle en représente quatre. Un projectile allié parcourt environ 4,08 pixels par pas (490/120). La simulation conserve la fraction de temps restante pour l’image suivante.

Le rendu vise 60 images/s. Une image très longue ajoute au maximum 100 ms de simulation pour limiter le rattrapage. Cela ralentit volontairement le temps simulé lors d’un blocage prolongé. La perte de focus suspend le jeu. Une alternative est un delta variable, plus court à écrire mais plus sensible au franchissement complet d’une cible entre deux positions.

## Propriété, références et RAII

World possède ses entités par valeur. Les std::vector réservent leur stockage dynamique et le libèrent à leur destruction. reserve(64) anticipe les projectiles courants sans imposer de limite. Les ennemis d’une vague occupent des cases contiguës. Une réallocation invalide les références vers leurs éléments. Le rendu parcourt les vecteurs après la mise à jour, lorsque les suppressions sont terminées.

player(), shots() et enemies() exposent des références constantes pour observer les objets sans copie. Dans la boucle de déplacement, auto& autorise la modification de l’objet existant. Une petite boîte Rect est renvoyée par valeur. std::optional<Projectile> représente zéro ou un tir et stocke la valeur directement.

Le pointeur renvoyé par event->getIf indique que l’événement contient ou non une pression de touche. Il est emprunté et utilisé dans la portée de l’événement. Le pointeur de getenv est également emprunté et son contenu est copié dans un chemin. Aucun de ces pointeurs ne doit être libéré par le jeu.

Game possède directement la fenêtre et la police. La durée de vie de la police couvre tous les textes dessinés. La destruction des objets SFML libère les ressources associées. Les flux de fichiers suivent le même principe RAII. La fermeture explicite du flux de score permet aussi de détecter une erreur d’écriture avant le remplacement du fichier. Un fichier temporaire est écrit à côté du record puis renommé. Ce mécanisme vise une seule instance du jeu.

## Coût des collisions

Les boîtes AABB restent alignées sur les axes. Pour toucher, les projections des deux rectangles doivent se recouvrir sur X et sur Y. Avec deux intervalles horizontaux [0,10) et [10,15), le contact du bord donne faux. Chaque paire demande quatre comparaisons, donc un temps O(1).

Avec A tirs alliés, H tirs hostiles et E ennemis, un pas coûte au pire O(A × E + H + E + A). Le terme dominant vient de la recherche du premier ennemi touché par chaque tir allié. Les tirs hostiles ne vérifient que le joueur. WaveManager remplace l’ennemi touché par le dernier puis appelle pop_back, ce qui rend la suppression O(1) après la recherche. L’ordre visuel des ennemis est sans importance. std::erase_if compacte le vecteur de projectiles en un passage. La mémoire de simulation est O(A + H + E) hors capacité précédemment réservée.

Avec au plus 40 ennemis, le balayage direct est facile à tester. Pour des milliers de cibles, une grille spatiale limiterait les comparaisons aux cellules traversées, au prix de sa mise à jour. Pour un laser très rapide, un test de segment ou de boîte balayée traiterait le franchissement entre deux pas.

## États, alternatives et preuves

enum class sépare les camps et les écrans au niveau des types. Les quatre écrans ont peu de transitions. Des classes d’état polymorphes avec propriété exclusive par std::unique_ptr deviendraient intéressantes si chaque écran possédait beaucoup de ressources et un cycle de vie indépendant. Ici, la composition préserve des types concrets et une propriété lisible. Même raisonnement pour Enemy et Player : partager un rectangle ne suffit pas à justifier une classe de base virtuelle.

Pour expliquer la séparation logique/rendu, ouvrir tests/core.cpp. Une partie avec une graine fixe est pilotée jusqu’à la deuxième vague. Le test attend 1 600 points, soit les seize ennemis initiaux. Un autre scénario laisse le joueur immobile et attend la fin de partie. Ces tests vérifient les règles indépendamment de la fenêtre. Le test graphique prévu en CI ouvre la fenêtre, anime douze images et se termine ; il complète les tests de logique sans vérifier l’apparence pixel par pixel.
