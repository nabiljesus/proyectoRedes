#Marco teórico
##Análisis preliminar

##Justificaciones
1. ¿Qué tipo de sockets decidió emplear?

Para realizar la transmisión de la información se decidió utilizar Protocol de Datagramas de Usuario (User Datagram Protocol en inglés), un protocolo no orientado a conexión. El Protocolo UDP es un protocolo de la capa de transporte para uso con el protocolo IP de la capa de red. El protocolo UDP proveé un servicio de intercambio de datagramas a través de la red en modo Best-Effort, es decir, que no puede asegurar la entrega de los datagramas o paquetes. 

Debido a que se requiere un tiempo de respuesta inmediato para minimizar, en lo posible, el tiempo de espera en la entra o salida de los automoviles en el estacionamiento, es necesario que la velocidad de transmisión del paquete se maximize. UDP se suele utilizar cuando se buscan transmisiones con una cantidad de información baja en los paquetes y altas velocidades de transferencia (aunque se puedan perder algunos paquetes como efecto adverso) por lo que para el caso actual puede satisfacer la solicitud. 

El protocolo TCP, sin embargo, también pudiese satisfacer la 

2. Identifique todos los mensajes del sistema, indicando: el formato del mismo,
su tamaño en bytes, quién genera el mensaje y quién lo recibe y procesa.


3. Realice el diseño completo del protocolo de comunicación que construya y
describa como opera el mismo, debe usar UDP como capa de transporte.
Incluya el punto de vista de los cambios de estado de las entidades que se
comunican. Se sugiere usar diagramas de máquinas de estados finitos y
facilitar la explicación de como es la actividad del protocolo.

4. Describa aspectos del proyecto que funcionan según el enunciado y cuales
no. Cualquier requerimiento no desarrollado o que contenga fallas, deberá
ser señalado claramente.

5. La implementación es en lenguaje C.

6. Opcional: Construya un aplicación confiable, que garantice la entrega, evite
información duplicada y pérdida de información. Todos estos controles
deben estar en capa de aplicación.