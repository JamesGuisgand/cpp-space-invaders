# Déployer Space Invaders derrière Traefik

Le navigateur exécute le moteur C++20 via WebAssembly. Canvas affiche soit les formes et couleurs du projet SFML, soit la version Graphique ++ avec nébuleuses, étoiles animées et vaisseaux lumineux. La pause permet de changer de mode pendant une partie. Le record et le mode sont mémorisés dans le navigateur. Nginx sert les fichiers statiques, et Traefik fournit HTTPS.

## 1. Pointer le DNS

Dans la zone DNS de guisgand-james.fr, crée une entrée A pour spaceinvader vers l’adresse IPv4 du VPS OVH. Ajoute une entrée AAAA uniquement si le VPS sert aussi Traefik sur cette adresse IPv6. Le résolveur existant utilise le challenge HTTP sur le port 80.

## 2. Transférer l’archive

Sur Windows, depuis PowerShell. Utilise ton port SSH habituel si personnalisé.

```powershell
scp "$env:USERPROFILE\Downloads\cpp-space-invaders-web.zip" james@IP_DU_VPS:~/
ssh james@IP_DU_VPS
```

## 3. Dézipper et démarrer sur le VPS

Utilise un dossier distinct de l’ancienne édition lors du premier déploiement.

```bash
sudo apt install -y unzip curl
mkdir -p ~/projects/space-invaders-web
unzip ~/cpp-space-invaders-web.zip -d ~/projects/space-invaders-web
cd ~/projects/space-invaders-web/cpp-space-invaders

docker network inspect web --format '{{range .Containers}}{{println .Name}}{{end}}'
docker compose config --quiet
docker compose up -d --build
docker compose ps
```

La liste du réseau `web` doit inclure Traefik. Compose réutilise ce réseau, l’entrypoint `websecure` et le résolveur `myresolver` du projet Rust. Nginx écoute sur le port interne 80. Le conteneur ne publie aucun port sur l’hôte. Docker compile le moteur, teste son interface WebAssembly avec Node.js, puis ne garde que Nginx et les fichiers du site dans l’image de déploiement.

```bash
curl -fsSI https://spaceinvader.guisgand-james.fr/
curl -fsSI https://spaceinvader.guisgand-james.fr/engine.wasm
```

Le fichier WASM doit répondre en HTTP 200 avec `Content-Type: application/wasm`. Si l’accès public échoue, vérifier le DNS, `docker compose logs --tail=60` et les journaux de Traefik. Le lien du portfolio est https://spaceinvader.guisgand-james.fr/.

## Tester sans Traefik

```bash
docker build -t space-invaders-web .
docker run --rm -p 127.0.0.1:8000:80 space-invaders-web
```

Sur la machine qui exécute Docker, ouvrir http://localhost:8000. Si la commande tourne sur le VPS, lancer `ssh -L 8000:127.0.0.1:8000 james@IP_DU_VPS` dans un autre terminal du PC et ouvrir cette même URL côté PC.
