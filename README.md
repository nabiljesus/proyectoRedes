# proyectoRedes 1


##Analisis Preliminar

Distancias -aproximadas- entre las sedes de salud-Caracas:

|             | El Paraiso  | San Antonio | Guarenas    | Maiquetía   | 
|-------------|-------------|-------------|-------------|-------------|
| El Paraiso  |             | 25.8km      | 40.7km      | 28km        |
| San Antonio | 25.8km      |             | 57.7km      | 47.8km      |
| Guarenas    | 40.7km      | 57.7km      |             | 65.4km      |
| Maiquetía   | 28km        | 47.8km      | 65.4km      |             |


De la tabla anterior, se puede apreciar que las dos sedes más distantes son la de Maiquetía y la Guarenas, por lo que estas estarían conectadas a través del ISP para ahorrar en lo posible los costos referentes a la conexión física entre estas, tal y como indica el planteamiento del problema. 

Con el fin de mantener la carga de la red equilibrada, el analisis lógico esperaría poder distribuir las sedes equitativamente entre ambos ISP, sin embargo, dada la distancia física existente entre estas y el costo que implica realizar una conexión física independiente al ISP, se concluyó distribuir las sedes en dos grupos: El primer grupo conformado por las sedes de El Paraiso, San Antonio y Maiquetía, y el segundo conformado únicamente por la sede de Guarenas. 

Una vez establecida la distribución general de la red, fue necesario hacer el subneteo adecuado para diseñar la topología y posterior configuració nde esta, así como el futuro calculo de los costos y las decisiones asociadas a estos. 

##Esquema de direccionamiento

###Requisitos

Tomando en cuenta que el crecimiento estimado se refiere a la cantidad de host en la que puede incrementar la subred partiendo de la cantidad presente, se tienen los siguientes requerimientos generales: 

1. Una subred de 27 hosts para El paraiso (7 Actuales y 20 del crecimiento estimado).
2. Una subred de 8  hosts para San Antonio de los Altos.
3. Una subred de 15 hosts para Guarenas (5 Actuales y 10 del crecimiento estimado).
4. Una subred de 21 hosts para Maiquetía (6 Actuales y 15 del crecimiento estimado).

###Planteamiento de topologías

####Modelo 1

<Acá va una imagen>

En este modelo, solo se conecta el router de Maiquetía al ISP. Sin embargo, los costos requeridos para interconectar Guatire-Maiquetía tanto de cable de fibra óptica como trabajos de perforación y mantenimiento, serían muy elevados, por lo que se descartó este modelo.

####Modelo 2



En este modelo, las redes de Guarenas y Maiquetía estan conectadas mediante el ISP, y Maiquetia establece conexión con El Paraiso y con San Antonio. Este modelo fué descartado debido a los costos requeridos para conectar Maiquetía y San Antonio, teniendo como alternativa inmediata el próximo modelo.

####Modelo 3

<Acá va una imagen>

En este modelo, las redes de Guarenas y Maiquetía estan conectadas mediante el ISP, y Maiquetia establece conexión con El Paraiso y este con San Antonio. El router de el paraiso funciona como enlace entre Maiquetía y San Antonio. Este modelo fué el final a utilizar debido a su buena gestión de recursos y eficiencia en la red. Sin embargo, para que pudiese funcionar, fué requerido una defición de enrutamiento definida más adelante.

###Analisís de requisitos: Totalización.

En cuanto al direccionamiento IP se refiere, se decidió comprar el rango de direcciones IP del ISP CANTV 20.42.10.0/24 debido a que se representa a una clínica de alto alcance y se disponen los medios para ello. A su vez, si se desea a futuro construir otra sede de salud-Caracas, se dispondrían de direcciones IP para asignar a la nueva sede.

Estableciendo etiquetas para cada subred, se tiene:

P-net = El Paraiso.
S-net = San Antonio de los Altos.
G-net = Guarenas.
M-net = Maiquetía.

Inicialmente se poseen dos routers con direcciones IP asignadas mediante el ISP CANTV, cada uno con su respectiva subred. 

| Subred      | Nº Hosts    | Crec. Estim.| Routers     |
|-------------|-------------|-------------|-------------|
| P-net       | 7           | 20          | 1           |
| M-net       | 6           | 15          | 1           |
| G-net       | 5           | 10          | 1           |
| S-net       | 8           | 0           | 1           |

Sin embargo, al requerir interconectar los routers de caracas (sin utilizar costos adicionales en cables al tomar a El Paraiso como nodo central), es necesario crear 2 sub-redes nuevas, MP-net y PS-net, a su vez que son requeridas otras dos para las conexiones de Guarenas al ISP y del ISP a Maiquetia. Actualizando la tabla anterior de esta manera:

| Subred      | Nº Hosts    | Crec. Estim.| Routers     |
|-------------|-------------|-------------|-------------|
| P-net       | 7           | 20          | 1           |
| M-net       | 6           | 15          | 1           |
| G-net       | 5           | 10          | 1           |
| S-net       | 8           | 0           | 1           |
| MP-net      | 2           | 0           | 0           |
| PS-net      | 2           | 0           | 0           |
| GI-net      | 2           | 0           | 0           |
| MI-net      | 2           | 0           | 0           |

+-------------+-------------+-------------+-------------+
|TOTAL        | 34          | 45          | 4           |
+-------------+-------------+-------------+-------------+ 


A partir de la tabla anterior, se puede inferir la cantidad de host necesarios para cada supra-red principal, siendo:

*PMS-net = P-net, S-net, M-net, MP-net y PS-net.
*G-net   = G-net.

| Subred      | Nº Hosts    | Crec. Estim.| Routers     | Total Req.  | Máscara     | IP's Libres |
|-------------|-------------|-------------|-------------|-------------|-------------|-------------|
| PMS-net     | 25          | 35          | 3           | 63          | /25         | 63          |
| G-net       | 5           | 10          | 1           | 16          | /27         | 14          |

A pesar de que para la G-net se estan desperdiciando 14 direcciones, utilizar una máscara más pequeña implicaría aumentar los costos al tener que utilizar otro router con máscara de /30 y los otros instrumentos asociados (Conmutadores, cables, interfaces de red). Como se esta considerando la mejor opción costo-rendimiento, se dejarán libres esa cantidad de direcciones con el fin de evitar costos adicionales. Análogamente para la PMS-net.

###Analisís de requisitos: Información detallada y Resultados.

Sin procesar las subredes en PMS-net, se tiene:

Luego de aplicar la técnica de LVSM para distribución de direcciones IP en estas subredes, se tiene: 

| Subred | Máscara         | Dir Subred   | Broadcast    | Rango       | D. Libres |
|--------|-----------------|--------------|--------------|-------------|-----------|
| PMS-net| 255.255.255.128 | 20.42.10.0   | 20.42.10.127 | .1 - .126   | 63        |
| G-net  | 255.255.255.224 | 20.42.10.128 | 20.42.10.159 | .129 - .159 | 14        |

Y las subredes de PMS-net estan conformadas de esta manera:

| Subred | Máscara         | Dir Subred   | Broadcast    | Rango     | D. Libres |
|--------|-----------------|--------------|--------------|-----------|-----------|
| P-net  | 255.255.255.224 | 20.42.10.0   | 20.42.10.31  | .1 - .30  | 2         |
| M-net  | 255.255.255.224 | 20.42.10.32  | 20.42.10.63  | .33 - .62 | 8         |
| S-net  | 255.255.255.240 | 20.42.10.64  | 20.42.10.79  | .65 - .78 | 5         |
| MP-net | 255.255.255.252 | 20.42.10.80  | 20.42.10.83  | .81 - .82 | 0         |
| PS-net | 255.255.255.252 | 20.42.10.84  | 20.42.10.87  | .85 - .86 | 0         |
| GI-net | 255.255.255.252 | 20.42.10.88  | 20.42.10.91  | .89 - .90 | 0         |
| MI-net | 255.255.255.252 | 20.42.10.92  | 20.42.10.95  | .93 - .94 | 0         |

##Enrutamiento

Debido a la topología escogida, es necesario definir una topología clara para esta... blah blah, estatica por que dinamica no es necesaria y los comandos and so on.

##Asignación de Dir. a los host

DHCP pq crecimiento y bajones de luz como emergencia and stuff


##Costos

de swiches and stuff