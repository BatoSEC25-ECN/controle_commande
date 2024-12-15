***< Remplissez ou supprimer les parties encadrées par *< ... >* >***


# Plan d’exécution du Projet

*< Version 1.0 >*

*< 05/12/24 >*

### Auteurs

*< Equipe  et logo>*

*< Listez les auteurs >*

## 1. Équipe

*< compétences de chaque des membres de l’équipe et mettre en avant les points différenciant de chacun. >*
LEPOURTOIS Benjamin, 
PROCACCIA Paul
DUJARDIN Alexandre : python, C
DE BEAUREPAIRE Jean-Baptiste
LOBET Kadwalon, GAUDIN Siméon
DE MARICOURT Guillaume, KRESS Eloi
DENIAU Maxime : Bases en git et maîtrise de l'agilité.    


*< Identifiez un membre pour chaque période qui aura la charge de s’assurer de la bonne avancée du projet (ce n’est pas le chef de projet, mais plus un « coach » qui s’assurera que l’équipe fonctionne bien). Ce coach doit être différent à chaque période. >*
Période 1 : Kadwalon
Période 2 : Maxime
Période 3 : Alexandre

## 2. État du projet

### 2.1. Sous-projet Mettre au point un système fiable de calcul de position et attitude du bateau 

*< Listez les responsables de ce sous-projet >*
DUJARDIN Alexandre
DENIAU Maxime

*< Copiez cette section pour chaque sous-projet dont vous avez la responsabilité. >*

### 2.1.1. Description du produit

*< Décrivez le produit fini attendu. Montrer clairement quels sont les besoins et comment le produit fini va répondre à ces besoins. >*

Objectif : Choix et maîtrise du composant GPS.

L’objectif est de développer un système permettant d’obtenir la position GPS précise et l’attitude du voilier (inclinaison, orientation) via une fusion des données des capteurs GPS-RTK, IMU, et boussole.


### 2.1.2. Fonctionnalités

*< Décrivez les différentes fonctionnalités (nom, description, etc.) de votre produit. >*
*< Priorisez ces fonctionnalités par ordre d’importance pour le produit. >*


1/ Acquisition des données capteurs, réception de la position
Description : Collecter les données brutes des capteurs GPS-RTK, IMU, et boussole.

2/ Fusion des données
Description : Combiner les données des capteurs pour obtenir une position et une attitude précises.

2/ Transmission des données au système de navigation, communiquer avec le reste du système
Description : Intégrer les données fusionnées dans le réseau du voilier autonome.


### 2.1.3. Exigences opérationnelles

*< Définissez les exigences opérationnelles de votre produit en quantifiant les objectifs à atteindre. Les clients utiliseront ces objectifs pour valider la conformité de votre produit (et votre projet). Organiser cette partie en identifiant de manière unique chaque exigence unitaire. >*

Le produit doit : 
Avoir une précision à la dizaine de centimètres maximum. 
Assurer sa fiabilité et sa bonne intégration dans le système. 
Avoir un temps de réponse ≤ 500 ms pour chaque mise à jour des données.


### 2.1.4. Plan de validation

*< Définissez votre plan de validation du produit pour démontrer que le produit répond bien aux attentes du client. Vous devez définir les procédures de test et les démonstrations d’usage prévus pour valider les exigences définies avant. >*

Test unitaire des capteurs : Vérification individuelle des mesures GPS, IMU et boussole.
   Comparaison des résultats avec un outils GPS de référence (téléphone par exemple) 
   Réaliser ces tests sous plusieurs conditions (pluie, proche de l'eau, hors de l'eau, etc...)
   Réaliser un tour avec le bateau en mode manuel pour enregistrer la position et valider le bon fonctionnement.
Concertation fusion et envoie des données 

### 2.2. Sous-projet : **Concevoir et tester le contrôle du bateau (suivi trajectoire, optimisation puissance)**

- PROCACCIA Paul
- KRESS Éloi
- LEPOURTOIS Benjamin

### 2.2.1. Description du produit

* Une implémentation logicielle permettant le suivi précis d'une trajectoire définie sur un plan d'eau lentique.
* Un mécanisme de contrôle optimisé pour les actionneurs du voilier (voile et safran) en vue de minimiser la consommation d'énergie tout en assurant la stabilité.
* Algorithmes d'optimisation visant à réduire la puissance utilisée, en exploitant les conditions de vent et en ajustant dynamiquement les réglages des voiles.

### 2.2.2. Fonctionnalités

#### Fonctionnalités principales  
1. **Suivi de trajectoire**  
   - **Description** : Le système ajuste automatiquement le cap du voilier pour suivre une trajectoire préalablement définie.  
   - **Priorité** : Haute

2. **Mode manuel et automatique**  
   - **Description** : Possibilité de basculer entre le contrôle manuel et le contrôle automatique à tout moment.  
   - **Priorité** : Haute

3. **Optimisation de l’énergie**  
   - **Description** : Ajustement dynamique des voiles pour minimiser la consommation énergétique en fonction des conditions de vent.  
   - **Priorité** : Haute

4. **Gestion des imprévus**  
   - **Description** : Réaction aux conditions imprévues (pannes, changements brusques de vent) pour garantir la sécurité du voilier.  
   - **Priorité** : Moyenne

#### Fonctionnalités secondaires  
1. **Analyse en temps réel**  
   - **Description** : Affichage des données de navigation (vent, vitesse, cap) sur une interface utilisateur pour évaluation en temps réel (comme un drone FPV)
   - **Priorité** : Basse

2. **Enregistrement des données**  
   - **Description** : Stockage des trajectoires parcourues et des paramètres de navigation pour analyse post-course (boite noire du voilier)
   - **Priorité** : Basse

### 2.2.3. Exigences opérationnelles

*Exemple avec des données plus ou moins aberrantes*

1. **EXIG001** : Le système doit maintenir une précision de suivi de trajectoire de ±2 mètres par rapport à la trajectoire définie. 
2. **EXIG002** : Le système doit permettre une bascule entre le mode manuel et automatique en moins de 2 secondes.  
3. **EXIG003** : La consommation énergétique moyenne ne doit pas dépasser 5 Wh pour une navigation de 30 minutes.  
4. **EXIG004** : Le système doit être capable de s’adapter à des vents de 2 à 15 nœuds sans déviation majeure de la trajectoire (>10 mètres).  
5. **EXIG005** : Le temps de réponse du système pour corriger un changement de cap doit être inférieur à 1 seconde.  


### 2.2.4. Plan de validation

#### Procédures de test  
1. **Test de précision de trajectoire**  
   - **Méthode** : Configurer une trajectoire prédéfinie avec bouées fixes, puis mesurer les écarts moyens au passage de chaque point clé.  
   - **Critère de validation** : Respect de l’exigence **EXIG001**.  

2. **Test de bascule manuel/automatique**  
   - **Méthode** : Simuler une navigation et mesurer le délai entre l’activation de la commande de bascule et le changement effectif de mode.  
   - **Critère de validation** : Respect de l’exigence **EXIG002**.

3. **Test de consommation énergétique**  
   - **Méthode** : Faire naviguer le voilier sur un parcours typique de 30 minutes et mesurer la consommation énergétique.  
   - **Critère de validation** : Respect de l’exigence **EXIG003**.  

4. **Test d’adaptation au vent**  
   - **Méthode** : Naviguer dans des conditions de vent variables (ex. simulateur ou en extérieur) et mesurer la stabilité de la trajectoire.  
   - **Critère de validation** : Respect de l’exigence **EXIG004**.  

5. **Test de temps de réponse**  
   - **Méthode** : Introduire des changements brusques de trajectoire (par exemple, simulation de vent changeant) et mesurer le temps de réaction du système.  
   - **Critère de validation** : Respect de l’exigence **EXIG005**.  

6. **Démonstrations d’usage**  
   - **Description** : Réalisation d’un parcours complet avec contournement de bouées en conditions réelles pour démontrer les capacités du système.  
   - **Critère de validation** : Validation générale du produit par le client et respect des exigences cumulées.  


### 2.3. Sous-projet girouette / annémometre
Guillaume DUMESNIL DE MARICOURT
Kadwalon LOBET

### 2.3.1. Description du produit

Le produit fini attendu est une girouette à ultrason. Ce capteur, essentiel pour le système de navigation du bateau, permet d'ajuster les voiles et le cap en fonction du vent. Sans cette information le bateau ne peut-être en mesure de se déplacer car le vent et l'angle d'incidence dans les voiles détermnine la vitesse de celui-ci.

La girouette doit: 
- Mesurer l'angle et la vitesse du vent en un point donné sur le bateau.
- Transmettre les composantes du vent (force/direction) du vent à un microcontrolleur externe.
- Prendre en charge un protocole de communication des informations utilisant la même API que celle utilisée par la girouette industrielle, avec possiblement une normalisation des données transmises (vecteur d'intensité). 

De plus, il existe des contraintes extérieures quant au capteur. Celui-ci devra apriori être fixé en haut du mat du bateau. (stabilizateur ? )
- La girouette doit être pilotée via un protocole sans fil (bluetooth, zigbee...).
- Autonome en énergie (pour une durée donnée).

### 2.3.2. Fonctionnalités

1. **Mesure de la force du vent**
   - **Description** : Capacité à mesurer la vitesse du vent en temps réel.
   - **Priorité** : Principale - Moyenne

2. **Mesure de la direction du vent**
   - **Description** : Capacité à déterminer la direction du vent par rapport au bateau avec précision.
   - **Priorité** : Principale - Haute
  
3. **Mesure de la direction du vent Nord Sud Est Ouest**
   - **Description** : Capacité à déterminer la direction du vent par rapport au points cardinaux.
   - **Priorité** : Secondaire - Moyenne
  
4. **Communication des Mesures sans fils**
   - **Description** : Capacité a transférer la données au microcontrolleur.
   - **Priorité** :  Principales - Moyenne

### 2.3.3. Exigences opérationnelles

1. **EXIG001** : La girouette doit déterminer la direction du vent avec une précision de ± 1 degré.
2. **EXIG002** : La girouette doit mesurer la force du vent avec une précision de ± 1 m/s.
3. **EXIG003** : La girouette doit communiquer ses mesures via un protocole sans fil (Bluetooth, Zigbee, etc.).
4. **EXIG004** : La girouette doit être autonome en énergie pour une durée minimale de 6 mois.
5. **EXIG005** : La girouette doit être résistante aux conditions maritimes (eau salée, humidité, etc.).

### 2.3.4. Plan de validation

1. **Test de précision de la direction du vent**
   - **Méthode** : Comparer les mesures de la girouette avec une girouette de référence dans des conditions contrôlées.
   - **Critère de validation** : Respect de l'exigence **EXIG001**.

2. **Test de précision de la force du vent**
   - **Méthode** : Comparer les mesures de la girouette avec un anémomètre de référence dans des conditions contrôlées.
   - **Critère de validation** : Respect de l'exigence **EXIG002**.

3. **Test de communication sans fil**
   - **Méthode** : Vérifier la transmission des données de la girouette à un microcontrôleur externe via les protocoles sans fil spécifiés.
   - **Critère de validation** : Respect de l'exigence **EXIG003**.

4. **Test d'autonomie énergétique**
   - **Méthode** : Faire fonctionner la girouette en continu dans des conditions réelles et mesurer la durée de fonctionnement.
   - **Critère de validation** : Respect de l'exigence **EXIG004**.

5. **Test de résistance aux conditions maritimes**
   - **Méthode** : Soumettre la girouette à des conditions simulant l'environnement maritime (eau salée, humidité, etc.) et vérifier son bon fonctionnement.
   - **Critère de validation** : Respect de l'exigence **EXIG005**.


### 2.4. Sous-projet Planification
Siméon Gaudin
Jean-Baptiste de Beaurepaire

### 2.4.1. Description du produit

*< Décrivez le produit fini attendu. Montrer clairement quels sont les besoins et comment le produit fini va répondre à ces besoins. >*
L'objectif est de concevoir un système d'automatisation du bateau, proposant un cap à prendre, et étant capable de voir que le cap donné n'est plus valable (changement de cap, casse d'un matériel)

### 2.4.2. Fonctionnalités

Calcul des trajectoires pour atteindre l'objectif, priorité intérmédiaire.
Tenir à jour l'état des caps à prendre (si le vent change ou autre...), priorité basse.
Ordonnancer les taches du microcontroleur, priorité max.
Donner le cap à suivre à controle du bateau, priorité intérmédiaire.

### 2.4.3. Exigences opérationnelles

Calcul de la cible en moins de 10ms et à une précision centimétrique et 100% de fiablilité.
Se rendre compte qu'un cap n'est pas tenable en moins de 5 secondes.
Ordonnancement fiable, bonne gestion des ressources partagées, 100% de fiabilité

### 2.4.4. Plan de validation

*< Définissez votre plan de validation du produit pour démontrer que le produit répond bien aux attentes du client. Vous devez définir les procédures de test et les démonstrations d’usage prévus pour valider les exigences définies avant. >*
1. **Test de calcul de trajectoires**  
   - **Méthode** : A partir de 2 points GPS, donner la liste des caps à suivre, mesurer l'écart entre la position que l'on retrouve à partir des différents angles et la position GPS du point d'arrivée. Les trajectoires devront être comprise entre 10 et 100m. La stratégie d'accuisition devra être vue comme fiable jusqu'a ce qu'on atteigne l'objectif. 
   - **Critère de validation** : Respect de l’exigence.  

2. **Test d'ordonancement**  
   - **Méthode** : Formalisation sur un réseau de Petri temporisé.  
   - **Critère de validation** : Respect de l’exigence.
!
3. **Test du suivi des trajectoires**  
   - **Méthode** : A l'aide d'une base de données résultats de capteur, simuler un changement de vent.
   - **Critère de validation** : Respect de l’exigence.


## 3. Planning du projet

### 3.1. Jalons et délivrables

*< Rappelez les dates importantes du projet (périodes, revues, etc.) et les délivrables attendus : ce document, la documentation finale, code, etc. >*

### 3.2. Planning de la première période

*< Pour chaque sous-projet, donnez les objectifs de réalisation sur la première période. Lister les fonctionnalités qui seront opérationnelles à la fin de la période (nom, description) ainsi que leurs tests d’acceptation (autrement dit qu’est-ce qui permettra de dire que la fonctionnalité est réalisée). Attention prenez bien en compte le temps que vous pourrez y consacrer. >*

*< Pour chaque fonctionnalité, évaluez leur difficulté/durée et décomposer les en tâches à mener pour les réaliser. >*
Période 1 : 
- Mettre au point un système fiable de calcul de position et attitude du bateau : 
	premières mesures des outils GPS :
		difficulté : 3/10
		durée : moyennement longue
		tâches : 
			Lecture et appropriation de la doc technique
			réalisation de la plateforme d'essai 
			mise en fonctionnement de l'outil 
   
- Sous-projet Planification
      tâches : 
         Analyser des différents états dans lequel peut se trouver le voilier
         Prendre connaissance de l’état de l’art et de ce qui s’est fait les années précédente
         Concevoir un premier draft de machine à état.



### 3.3. Backlog

*< Backlog: donner la liste des fonctionnalités (nom, description) qui sont planifiées sur les périodes suivantes (pas besoin de préciser sur quelle période). Donner une priorité à ces fonctionnalités (au cas où il faudrait faire un choix). >*

## 4. Management du risque

*< Lister les risques (retard, échec, organisation, manque de connaissance, etc.) possible pour les différents sous-projets. Pour chacun proposer les actions à entreprendre pour prévenir le risque et y répondre s'il survient. >*


 

 