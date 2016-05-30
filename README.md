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

###Analisís de requisitos

Estableciendo etiquetas para cada subred, se tiene:

P-net = El Paraiso.
S-net = San Antonio de los Altos.
G-net = Guarenas.
M-net = Maiquetía.

Inicialmente se poseen dos routers con direcciones IP asignadas mediante el ISP CANTV, cada uno con su respectiva subred. 